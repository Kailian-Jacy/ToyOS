ASM_SRC		= $(filter-out fork.S, $(sort $(wildcard *.S)))
C_SRC       = $(sort $(wildcard *.c))
OBJ		    = $(patsubst %.S,%.o,$(ASM_SRC)) $(patsubst %.c,%.o,$(C_SRC))

CFLAG = -march=$(ISA) -mabi=$(ABI) -mcmodel=medany -fno-builtin -ffunction-sections -fdata-sections -nostartfiles -nostdlib -nostdinc -static -lgcc -Wl,--nmagic,--build-id=none -g  
LLIBDIR = -L$(HOMEDIR)/lib
LLIB = -l:mystdio.a

all: fork.o

fork.o: fork.S fork.bin fork
	${GCC} ${CFLAG} -c fork.S
# -c generates object file

%.o:%.c
	${GCC} ${CFLAG} ${INCLUDE} -c $<

%.o:%.S
	${GCC} ${CFLAG} -c $<

fork.bin: $(OBJ)
	${GCC} ${CFLAG} -fpie -T link.lds -o fork.elf ${OBJ} ${LLIBDIR} ${LLIB} 
	${OBJCOPY} fork.elf -O binary fork.bin

clean:
	$(shell rm *.o fork.o fork fork.elf fork.bin 2>/dev/null)
	$(shell rm -r ._* 2> /dev/null)

fork: $(OBJ) 
	${GCC} ${CFLAG} -o fork ${OBJ} ${LLIBDIR} ${LLIB}