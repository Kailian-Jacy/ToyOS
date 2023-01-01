all:
	${MAKE} -C kernel all
# LD -T points the linking script.
	${LD} -T kernel/vmlinux.lds \
	kernel/*.o ../../init/*.o ../../lib/*.o \
	-o ../../vmlinux 
# -o output.
	$(shell test -d boot || mkdir -p boot)
# use OBJCOPY and NM to build kernel Image and symbol tables.
	${OBJCOPY} -O binary ../../vmlinux ./boot/Image
	nm ../../vmlinux >  ../../System.map

clean:
	${MAKE} -C kernel clean
	$(shell test -d boot && rm -rf boot)
