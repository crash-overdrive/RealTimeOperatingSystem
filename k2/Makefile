#
# Makefile for entire kernel
#
XDIR=/u/cs452/public/xdev
XBINDIR=$(XDIR)/bin
XLIBDIR1=$(XDIR)/arm-none-eabi/lib
XLIBDIR2=$(XDIR)/lib/gcc/arm-none-eabi/9.2.0/
LD = $(XBINDIR)/arm-none-eabi-ld

# -static: force static linking
# -e: set entry point
# -nmagic: no page alignment
# -T: use linker script
LDFLAGS = -static -e main -nmagic -T ../src/linker.ld -L $(XLIBDIR1) -L lib -L $(XLIBDIR2) -lstdc++

.PHONY: all
all: kern.elf

.PHONY: main
main: io kern
	cd src/main && $(MAKE) clean && $(MAKE) install

.PHONY: kern
kern:
	cd src/kern && $(MAKE) clean && $(MAKE) install

.PHONY: io
io:
	cd src/io && $(MAKE) clean && $(MAKE) install

kern.elf: io kern main
	cd build && $(LD) $(LDFLAGS) -o $@ main.o bwio.o ContextSwitch.o Kernel.o Syscall.o UART.o UserSyscall.o -lgcc -lc

install: kern.elf
	cd build && cp kern.elf /u/cs452/tftp/ARM/spratap/k2.elf
	chmod 777 /u/cs452/tftp/ARM/spratap/k2.elf

clean:
	cd build && rm -f *.s *.o *.elf
	
