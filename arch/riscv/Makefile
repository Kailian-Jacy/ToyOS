SBIDIR=-L${HOMEDIR}/lib64/lp64 -l:libsbi.a -L${HOMEDIR}/lib64/lp64/opensbi/generic/lib -l:libplatsbi.a
all:
	${MAKE} -C kernel all
# LD -T points the linking script.
	${LD} -T kernel/vmlinux.lds \
	kernel/*.o ../../init/*.o ../../lib/*.o ../../user/getpid/*.o ../../user/forkloader/*.o \
	${SBIDIR} \
	-o ../../vmlinux 
# -o output.
	$(shell test -d boot || mkdir -p boot)
# use OBJCOPY and NM to build kernel Image and symbol tables.
	${OBJCOPY} -O binary ../../vmlinux ./boot/Image
	nm ../../vmlinux >  ../../System.map

clean:
	${MAKE} -C kernel clean
	$(shell test -d boot && rm -rf boot)
	$(shell rm ._* 2> /dev/null)
	$(shell rm -dfr './-p' 2> /dev/null)
	$(shell rm include/._* 2> /dev/null)