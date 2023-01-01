#include <defs.h>
#include <string.h>

// __aligned__? Used for?
unsigned long early_pgtbl[512] __attribute__((__aligned__(0x1000)));
// 1GB for each item. So 512 GB in total.
// 512 items indexed with 9 bits. 2 ^ 9 = 512.
// 1GB content for each page indexed with 30bits. 2^30  = 1GB
unsigned long swapper_pg_dir[512] __attribute__((__aglined__(0x1000)));

void setup_vm(void);
void setup_vm_final(void);
void create_mapping(uint64 *pgtbl, uint64 va, uint64 pa, uint64 sz, int perm);

extern void _set_satp_with_swapper(uint64 addr);

// No uint64 _stext. Why??(大小端序)
extern char _stext[];
extern char _srodata[];
extern char _sdata[];
// ...more

// Early page tabel is a temporary page table for booting stage.
// Or root pgtable?

void setup_vm(void)
{
/*
	1. 由于是进行 1GB 的映射 这里不需要使用多级页表
	2. 将 va 的 64bit 作为如下划分: | high bit | 9 bit | 30 bit |
		high bit 可以忽略
		中间 9 bit 作为 early_pgtbl 的 index
		低 30 bit 作为 页内偏移 这里注意到 30 = 9 + 9 + 12， 即我们只使用根页表， 根
		页表的每个 entry 都对应 1GB 的区域。
	3. Page Table Entry 的权限 V | R | W | X 位设置为 1 
*/

	// Mapping PA=VA
	unsigned long long VA = 0x0000000080000000;
	unsigned long long PA = 0x0000000080000000;
	// [DEBUG] Warning: << is not < 
	/*
		VA divided into:
		- 0-11 offset;
		- 12-20 VPN[0];
		- 21-29 VPN[1];
		- 30-38 VPN[2];
		In one-level translation: 
			EARLY_PAGE[VA.VPN[2]] = PTE, where PTE >> 10 = PPN, PPN << 12 + VA.offset = PA.
			So, index = VA.VPN[2] = VA >> 30;
				PPN = PA >> 12 << 10.
	*/
	int index = (VA >> 30 & ((1<<9)-1) ); // 1{9} = (1 << 9) - 1!
	// [DEBUG] Warning: << is lower than + !!!!!!!!!!! bracket << !!!!
	early_pgtbl[index] = ((PA >> 12) << 10) + 15;

	// Mapping VA=PA+PA2VA_offset
	VA = 0xffffffe000000000;
	PA = 0x0000000080000000;
	index = (VA >> 30 & ((1<<9)-1));
	early_pgtbl[index] = (( PA>>12 ) << 10) + 15;
}

/*
	swapper_pg_dir: kernel pagetable根目录，在 setup_vm_final 进行映射
*/
unsigned long swapper_pg_dir[512] __attribute__((__aglined__(0x1000)));

void setup_vm_final(void){
	memset(swapper_pg_dir, 0x0, PGSIZE);

	// No OpenSBI mapping required.

	/*
		Flag are in sequence X W R V. 
		- X|-|R|V is 1011 -> 11
		  -|-|R|V is 0011 -> 3
		  -|W|R|V is 0111 -> 7
	*/
	// Mapping kernel text X|-|R|V 
	create_mapping(swapper_pg_dir, _stext , _stext - PA2VA_OFFSET , (_srodata - _stext) >> 12 , 11);

	// Mapping kernel rodata -|-|R|V
	create_mapping(swapper_pg_dir, _srodata, _srodata - PA2VA_OFFSET, (_sdata - _srodata) >> 12,3);

	// Mapping other memory -|W|R|V
	create_mapping(swapper_pg_dir, VM_START , PHY_START , PHY_SIZE >> 12 ,7);


	// Set satp with swapper_pg_dir
	/*
		System.map.swapper_pg_dir is virtual address, so 
			swapper_pg_dir.pa = System.map.swapper_pg_dir - PA2VA_OFFSET
		I <guess> satp would read the physical address.
		Satp: mode = 1 << 63 + ppn (swapper_pg_dir.pa >> 12)

		inline asm: Input %1, output none, but :: reserves. during this t0 modified.
	*/
	_set_satp_with_swapper(((uint64)swapper_pg_dir - PA2VA_OFFSET) & (((uint64)1<<44) - 1));

	asm volatile("sfence.vma zero, zero");
	return;
}

void create_mapping(uint64 *pgtbl, uint64 va, uint64 pa, uint64 sz, int perm){
	/*
		pgtbl 根页表的基地址
		va pa 为需要映射的物理地址 虚拟地址
		sz 为映射的大小, 4KB each.
		perm为读写权限

		创建多级页表的时候可以使用kalloc来获取一页作为页表目录
		可使用 V bit来判断页表项是否存在
	*/
	for (int i = 0 ; i < sz; i++){
		// First level pte.
		uint64* tbl = pgtbl;
		// You can't ADD to pointers. They behaves differently from integers.
		int idx = (va >> 30) & (((uint64)1<<9) - (uint64)1);
		uint64* pte = &(tbl[idx]);
		if (*pte & (uint64)0x1) {
		} else {
			uint64 pg = (uint64)kalloc() - (uint64)PA2VA_OFFSET;
			// To show that this is not a leaf page, pte.r should not be 1. only v should be 1.
			uint64 newpte = ((((uint64)pg >> 12) & (((uint64)1<<44)-(uint64)1)) << 10) + (uint64)1;
			*pte = newpte;
		}
		// Second level.
		// How could 44 bits pte + 12 bit offset extended to a 64 address? Implenmented by OS. See specification p71.
		// tbl = (uint64 *)((((*pte >> 10) & (((uint64)1<<44)-1)) << 12) | (uint64)0xff00000000000000);
		tbl = (uint64 *)(((*pte >> 10) & (((uint64)1<<44)-(uint64)1)) << 12);
		idx = (va >> 21) & (((uint64)1<<9) - (uint64)1);
		pte = &(tbl[idx]);
		if (*pte & (uint64)0x1) {
		} else {
			uint64 pg = (uint64)kalloc() - (uint64)PA2VA_OFFSET;
			// To show that this is not a leaf page, pte.r should not be 1. only v should be 1.
			uint64 newpte = ((((uint64)pg >> 12) & (((uint64)1<<44)-(uint64)1)) << 10) + (uint64)1;
			*pte = newpte;
		}
		// Third level.
		tbl = (uint64 *)(((*pte >> 10) & (((uint64)1<<44)-(uint64)1)) << 12);
		idx = (va >> 12) & (((uint64)1<<9) - (uint64)1);
		pte = &(tbl[idx]);
		// Skip the item if it has been valid. 
		if (*pte & (uint64)0x1) {
		} else {
			*pte = (((pa >> 12) & (((uint64)1<<44)- (uint64)1)) << 10) + (uint64)perm;
		}

		// Move next.
		va += 0x1000;
		pa += 0x1000;
	}
	/*
		Just map from page to page. During linking, all the symbols are rouded to 4KB with ALIGN(1000).
		Writing to address is not through satp, so I <guress> I need to use pgtbl as va here.
		I <guess> satp(ppn) << 12 >> 12 == pgtbl - PA2VA_Offset
		0. pgtbl as table, VA{30:38} = VPN[2] used as index => get PTE at pgtbl + ((VA >> 30) & ((1<<9) - 1))
		1. Check PTE validity: if PTE{0}, then refer to next layer,
			else get new page PTE = kalloc() >> 12 << 10 + 1; 
			For this is not a leaf page, no W R X allowed to be set.
		2. Get PPN = (PTE >> 10) & (1<<44)-1). Use PPN << 12 as another level of table to refer to. pas to step 0.
		3. In last leaf page, set perm and no more diving in.
		For now, simply use loop to assign in range.
	*/

}