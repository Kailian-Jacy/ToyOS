# qemu=qemu-system-riscv64 -nographic -M virt -m 256M -bios share/opensbi/lp64/generic/firmware/fw_payload.bin
qemu=qemu-system-riscv64 -nographic -M virt -m 256M -bios share/opensbi/lp64/generic/firmware/fw_jump.bin -kernel vmlinux

# export all variables to be used in sub makefile.
export
HOMEDIR=$(shell pwd)
CROSS_=riscv64-unknown-elf-

SCHEDULE=PRIORITY
# SCHEDULE=SJF

# gnu toolchains
GCC=${CROSS_}gcc
LD=${CROSS_}ld
OBJCOPY=${CROSS_}objcopy

ISA=rv64imafd
# Select ABI version of RISCV. ABI is pointed with supported length of types.
ABI=lp64

# include header file libs.
INCLUDE = -I $(HOMEDIR)/include -I $(HOMEDIR)/arch/riscv/include 
CF = -march=$(ISA)  -mabi=$(ABI) -mcmodel=medany -fno-builtin -ffunction-sections -fdata-sections -nostartfiles -nostdlib -nostdinc -lgcc -Wl,--nmagic -Wl,--gc-sections -g 
STATIC = $(CF) -static
CFLAG = ${STATIC} ${INCLUDE} -D${SCHEDULE}
# -mxxx options are used to point target cpu.
# -march points out the arch/ -mavx512 points out the extension.
# -mcpu use target cpu to point the target arch.
# -mabi is used only for RISCV. RISCV has many optional modules or extensions, which hereby described with $(ABI).
# ???: Generate code for the medium-any code model. 
# -fno-builtin: Gcc promotes performance with replacing functions with inbuilt-functions or asm code. This option is used to prevent replacement causing debugging failure.

# these target are not making outcoming files but only for execution. .PHONY explicitly show that these are not files.
.PHONY:all run debug clean
all:
# Top level makefile, triggering make in sub directories.
	${MAKE} -C lib all
	${MAKE} -C user all
	${MAKE} -C init all
	${MAKE} -C arch/riscv all
# echo -e enable \n to be interpreted as switching line.
# Before-Command-Marks: @ is used not to print the command, - means ignore the output error.
	@echo -e '\n'Build Finished OK

run: all
	@echo Launch the qemu ......
	@$(qemu)

debug: all
	@echo Launch the qemu for debug ......
	@${qemu} -S -s

clean:
	${MAKE} -C lib clean
	${MAKE} -C init clean
	${MAKE} -C arch/riscv clean
	${MAKE} -C user clean
	$(shell test -f vmlinux && rm vmlinux)
	$(shell test -f System.map && rm System.map)
	$(shell rm -r ._* 2> /dev/null)
	$(shell rm -r include/._* 2> /dev/null)
	@echo -e '\n'Clean Finished
