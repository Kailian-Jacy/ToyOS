.PHONY: all
all:
# Top level makefile, triggering make in sub directories.
	${MAKE} -C forkloader all
	${MAKE} -C getpid all
# echo -e enable \n to be interpreted as switching line.
# Before-Command-Marks: @ is used not to print the command, - means ignore the output error.
clean:
	${MAKE} -C getpid clean
	${MAKE} -C forkloader clean
	