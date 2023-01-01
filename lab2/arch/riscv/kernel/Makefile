ASM_SRC		= $(sort $(wildcard *.S))
C_SRC       = $(sort $(wildcard *.c))
# OBJ = "head.o sbi.o"
OBJ		    = $(patsubst %.S,%.o,$(ASM_SRC)) $(patsubst %.c,%.o,$(C_SRC))

all:$(OBJ)

# all .S asm code would be compiled from this rule.
%.o:%.S
# Similarly, .S asm code are build into obj files for linking. All the same.
	${GCC}  ${CFLAG} -c $<

# use all .c files to generate .o file.
%.o:%.c
	${GCC}  ${CFLAG} -c $<

clean:
	$(shell rm *.o 2>/dev/null)
