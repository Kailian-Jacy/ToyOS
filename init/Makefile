# sort is used to compose "main.c test.c"
C_SRC       = $(sort $(wildcard *.c))
OBJ		    = $(patsubst %.c,%.o,$(C_SRC))
# OBJ replace .c to .o to build obj target.
# final obj is filename.o separated by whitespace.
# % in patsubst is used for wildcard.

file = main.o
all:$(OBJ)
	
%.o:%.c
	${GCC} ${CFLAG} -c $<
# Generate object files named as main.o
# $< is used to point the first source file.
# $^ all source file
# $@ target name.

.PHONY: clean
clean:
	$(shell rm *.o 2>/dev/null)
	$(shell rm -r ._* 2> /dev/null)
