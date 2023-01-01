#include "vm.h"

// No uint64 _stext. Why??(大小端序)
extern char _stext[];
extern char _srodata[];
extern char _sdata[];
// ...more
// __aligned__? Used for?
unsigned long early_pgtbl[512] __attribute__((__aligned__(0x1000)));

// 1GB for each item. So 512 GB in total.
// 512 items indexed with 9 bits. 2 ^ 9 = 512.
// 1GB content for each page indexed with 30bits. 2^30  = 1GB
unsigned long swapper_pg_dir[512] __attribute__((__aglined__(0x1000)));

void flush(){
	asm volatile("sfence.vma zero, zero");
	// asm volatile("fence.i");
}

void do_mmap(struct task_struct *task, uint64_t addr, uint64_t length, uint64_t flags,
			 uint64_t vm_content_offset_in_file, uint64_t vm_content_size_in_file){
	flags |= VM_VALID;
	if (task->mm.vma_cnt == 0) {
		// task->mm.mmap[0].vm_mm = &task->mm;
		task->mm.mmap[0].vm_start = addr;
		task->mm.mmap[0].vm_end = addr+length;
		task->mm.mmap[0].vm_next = NULL;
		task->mm.mmap[0].vm_prev = NULL;
		task->mm.mmap[0].file_offset = vm_content_offset_in_file;
		task->mm.mmap[0].file_size = vm_content_size_in_file;
		task->mm.mmap[0].flags = flags;
		task->mm.vma_cnt++;
		return;
	} else {
		// TODO: Check max.
		// Set new
		// task->mm.mmap[task->mm.vma_cnt].vm_mm = &task->mm;
		task->mm.mmap[task->mm.vma_cnt].vm_start = addr;
		task->mm.mmap[task->mm.vma_cnt].vm_end = addr + length;
		task->mm.mmap[task->mm.vma_cnt].vm_next = NULL;
		task->mm.mmap[task->mm.vma_cnt].vm_prev = &task->mm.mmap[task->mm.vma_cnt];
		task->mm.mmap[task->mm.vma_cnt].file_offset = vm_content_offset_in_file;
		task->mm.mmap[task->mm.vma_cnt].file_size = vm_content_size_in_file;
		task->mm.mmap[task->mm.vma_cnt].flags = flags;
		// Set prev.
		task->mm.mmap[task->mm.vma_cnt-1].vm_next = &task->mm.mmap[task->mm.vma_cnt];
		task->mm.vma_cnt++;
	}
	return;
}

struct vm_area_struct * find_vma(struct task_struct *task, uint64_t addr){
	if (task->mm.vma_cnt == 0){
		return 0;
	}
	struct vm_area_struct * vma = task->mm.mmap;
	while (vma != 0){
		if (addr < vma->vm_end && addr >= vma->vm_start){
			return vma;
		}
		vma = vma->vm_next;
	}
	return 0;
}

unsigned long get_swapper_pg_dir() {
	return swapper_pg_dir;
}

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
	int index = (VA >> 30 & ((1 << 9) - 1)); // 1{9} = (1 << 9) - 1!
	// [DEBUG] Warning: << is lower than + !!!!!!!!!!! bracket << !!!!
	early_pgtbl[index] = ((PA >> 12) << 10) + 15;

	// Mapping VA=PA+PA2VA_offset
	VA = 0xffffffe000000000;
	PA = 0x0000000080000000;
	index = (VA >> 30 & ((1 << 9) - 1));
	early_pgtbl[index] = ((PA >> 12) << 10) + 15;
}

/*
	swapper_pg_dir: kernel pagetable根目录，在 setup_vm_final 进行映射
*/
unsigned long swapper_pg_dir[512] __attribute__((__aglined__(0x1000)));

void setup_vm_final(void)
{
	memset(swapper_pg_dir, 0x0, PGSIZE);

	// No OpenSBI mapping required.

	/*
		Flag are in sequence X W R V.
		- X|-|R|V is 1011 -> 11
		  -|-|R|V is 0011 -> 3
		  -|W|R|V is 0111 -> 7
	*/
	// Mapping kernel text X|-|R|V
	create_mapping(swapper_pg_dir, _stext, _stext - PA2VA_OFFSET, (_srodata - _stext) >> 12, 11, 0, 0, 0);

	// Mapping kernel rodata -|-|R|V
	create_mapping(swapper_pg_dir, _srodata, _srodata - PA2VA_OFFSET, (_sdata - _srodata) >> 12, 3, 0, 0, 0);

	// Mapping other memory -|W|R|V
	create_mapping(swapper_pg_dir, VM_START, PHY_START, PHY_SIZE >> 12, 7, 0, 0, 0);

	// Set satp with swapper_pg_dir
	/*
		System.map.swapper_pg_dir is virtual address, so
			swapper_pg_dir.pa = System.map.swapper_pg_dir - PA2VA_OFFSET
		I <guess> satp would read the physical address.
		Satp: mode = 1 << 63 + ppn (swapper_pg_dir.pa >> 12)

		inline asm: Input %1, output none, but :: reserves. during this t0 modified.
	*/
	_set_satp_with_swapper(((uint64)swapper_pg_dir - PA2VA_OFFSET) & (((uint64)1 << 44) - 1));

	flush();
	return;
}

void check_created(uint64 *pgtbl, uint64 va, int *check){
	create_mapping(pgtbl, va, 0, 1, 0, 1, 0, check);
}

void create_mapping(uint64 *pgtbl, uint64 va, uint64 pa, uint64 sz, int perm, int in_virtual, int rewrite, int *check)
{
	/*
		pgtbl 根页表的基地址
		va pa 为需要映射的物理地址 虚拟地址
		sz 为映射的大小, 4KB each.
		perm 为读写权限
		in_virtual 表示此时高地址的va是否已在页表中。在pgtbl为learly_pgtbl时为0，使用swapping_pg_dir为1。
		rewrite 表示是否需要覆盖原有权限。如果rewrite为0，则将会跳过valid的page。

		创建多级页表的时候可以使用kalloc来获取一页作为页表目录
		可使用 V bit来判断页表项是否存在
	*/
	perm |= VM_VALID;
	for (int i = 0; i < sz; i++)
	{
		// First level pte.
		uint64 *tbl = pgtbl;
		// You can't ADD to pointers. They behaves differently from integers.
		int idx = (va >> 30) & (((uint64)1 << 9) - (uint64)1);
		uint64 *pte = &(tbl[idx]);
		if (*pte & (uint64)0x1)
		{
		}
		else
		{
			if (check != 0){
				*check = -1;	
				return;
			}
			uint64 pg = (uint64)kalloc() - (uint64)PA2VA_OFFSET;
			// To show that this is not a leaf page, pte.r should not be 1. only v should be 1.
			uint64 newpte = ((((uint64)pg >> 12) & (((uint64)1 << 44) - (uint64)1)) << 10) + (uint64)1;
			*pte = newpte;
		}
		// Second level.
		// How could 44 bits pte + 12 bit offset extended to a 64 address? Implenmented by OS. See specification p71.
		// tbl = (uint64 *)((((*pte >> 10) & (((uint64)1<<44)-1)) << 12) | (uint64)0xff00000000000000);
		tbl = (uint64 *)((((*pte >> 10) & (((uint64)1 << 44) - (uint64)1)) << 12) + PA2VA_OFFSET * in_virtual);
		idx = (va >> 21) & (((uint64)1 << 9) - (uint64)1);
		pte = &(tbl[idx]);
		if (*pte & (uint64)0x1)
		{
		}
		else
		{
			if (check != 0){
				*check = -1;	
				return;
			}
			uint64 pg = (uint64)kalloc() - (uint64)PA2VA_OFFSET;
			// To show that this is not a leaf page, pte.r should not be 1. only v should be 1.
			uint64 newpte = ((((uint64)pg >> 12) & (((uint64)1 << 44) - (uint64)1)) << 10) + (uint64)1;
			*pte = newpte;
		}
		// Third level.
		tbl = (uint64 *)((((*pte >> 10) & (((uint64)1 << 44) - (uint64)1)) << 12) + PA2VA_OFFSET * in_virtual);
		idx = (va >> 12) & (((uint64)1 << 9) - (uint64)1);
		pte = &(tbl[idx]);
		// Skip the item if it has been valid.
		if (*pte & (uint64)0x1)
		{
			if (rewrite){
				// Rewrite the permission.
				*pte = (((pa >> 12) & (((uint64)1 << 44) - (uint64)1)) << 10) + (uint64)perm;
			}
			if (check != 0){
				*check = 1;	
				return;
			}
		}
		else
		{
			if (check != 0){
				*check = -1;	
				return;
			}
			*pte = (((pa >> 12) & (((uint64)1 << 44) - (uint64)1)) << 10) + (uint64)perm;
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

uint64 copy_pgdir(struct task_struct * father, struct task_struct * new){
	uint64_t new_pgdir = alloc_page();
	// Copy entirely.
	copy_page(new_pgdir, swapper_pg_dir);
	// Clear user part. Kernel part reserved.
	uint64_t stack = alloc_page();
	create_mapping(new_pgdir, USER_END - PGSIZE, stack - PA2VA_OFFSET, 1, VM_WRITE | VM_READ | VM_USER, 1, 0, 0);
	copy_page(stack, USER_END - PGSIZE);

	for (struct vm_area_struct * vma = father->mm.mmap; vma != NULL; vma = vma->vm_next){
		for (uint64_t pg_start = PGROUNDDOWN(vma->vm_start); pg_start < vma->vm_end; pg_start += PGSIZE){
			int check = 0;
			check_created(father->pgd + PA2VA_OFFSET, pg_start, &check);
			if (check != 0){
				uint64_t new_temp_pg = alloc_page();
				copy_page(new_temp_pg, pg_start);
				uint64_t flag = (vma->flags << 1) | VM_USER;
				create_mapping(new_pgdir, pg_start, new_temp_pg - PA2VA_OFFSET, 1, flag, 1, 0, 0);
			}
		}
	}

	new->pgd = new_pgdir - PA2VA_OFFSET;
}

void copy_page(pagetable_t dst, pagetable_t src){
	for (int i = 0; i < PGSIZE; i++){
		((char *)dst)[i] = ((char *)src)[i];
	}
}