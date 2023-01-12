C_SRC=$(sort $(wildcard *.c))
OBJ=$(patsubst %.c,%.o,$(C_SRC))
AR=ar

all:$(OBJ)
	$(AR) rcs mystdio.a mystdio.o

%.o:%.c
	${GCC} ${CFLAG} -c $<

clean:
	$(shell rm *.o 2>/dev/null)	
	$(shell rm -r ._* 2> /dev/null)