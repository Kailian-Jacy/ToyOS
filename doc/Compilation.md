# Compilation
----------------------------------------------------------------
refer to: https://renenyffenegger.ch/notes/development/languages/C-C-plus-plus/GCC/create-libraries/index

## User
`user` directory is composed of two sub directories `getpid` `forkloader`. These two parts are separated ELF executables used to test `fork()` and `exec()`.

For the current time, there is no external device like harddisk to load executable from, so these two are directly linked into kernel when building.

## Linking with static libraries

For `getpid` and `forkloader` requires to the same functions in `stdio` like `printf`, I need to provide outside library for linking.

TA provided code with kernel linking this way:
1. Generate obj files in arch/kernel
2. Just linking all these up to form a executable.
```
// arch/kernel/Makefile
...
%.o:%.S
	${GCC}  ${CFLAG} -c $<

%.o:%.c
	${GCC}  ${CFLAG} -c $<
...


// arch/riscv/Makefile
...
${LD} -T kernel/vmlinux.lds \
	kernel/*.o ../../init/*.o ../../lib/*.o ../../user/getpid/*.o ../../user/forkloader/*.o \
	-o ../../vmlinux 
...
```

This avoided building static library but just link them all.

I tried linking with static libraries in building `user`:
1. Build **static library** as `lib/mystio.a`.
2. Link that up into executables using shared functions.

```
// lib/Makefile
all:$(OBJ)
	$(AR) rcs mystdio.a mystdio.o

// user/getpid/Makefile
LLIBDIR = -L$(HOMEDIR)/lib
LLIB = -l:mystdio.a
...
fork.bin:{OBJ}
        ${GCC} ${CFLAG} -fpie -T link.lds -o fork.elf ${OBJ} ${LLIBDIR} ${LLIB}
...
```

Both of which works well. In both case, redundant symbols are ignored. But compared to the first type, static library brings more convenience:

1.  Static library hides internal symbols, you can choose to expose those symbols of functions you wish to use. 
2.  Hiding complex directory details. User don't care about directory, which obj to link into, they just needs a static library, and a doc showing available functions. That's intuitive and simple to use.

## Static library
Building static library:
```
$(CC) -c $(SRC) -o $(OBJ)
ar rcs $(Dir)/$(static).a $(OBJ)
```
Linking:
```
$(CC) main.o -L$(Dir) -l$(static) -o main
```

Points:
1. `-L` and `-l` follows with no space.
2. When linking, use static library name with out postfix. Use `lq84` but not `lq84.a`.
3. Put main before libraries. Target should be put before its dependencies.

`ar` is just archive tools, just like tar. Difference:
1. `tar` maintains directory structure but `ar` just assembles files together. 
2. `ar` is natively understood by `ld`. Now we are using which to build static library. 

## Shared library

Building:
```
$(CC) -c -fPIC ${SRC} -o ${OBJ}
$(CC) -shared $(OBJ) -o $(DIR)/$(shared).so
```
Linking:
```
$(CC) main.o -L$(DIR) -l$(shared) -o main
```

Points:
1. Build obj files with `-fPIC` (Position independent code) meant for shared library.
2. Use $(CC) -shared to assemble. 
3. Linking without postfix.

Calling:
```
LD_LIBRARY_PATH=$(pwd)/somepath main
```
Share library would be searched by default `/usr/lib`. You can use environment param `LD_LIBRARY_PATH` to point which.

