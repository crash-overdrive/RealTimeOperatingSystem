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
all: install
# echo '\e[31m'"WARNING: kern.elf NOT INSTALLED REMOTELY!"'\e[0m'

.PHONY: O0
O0: kern-O0.elf
	echo '\e[31m'"WARNING: kern-00.elf NOT INSTALLED REMOTELY!"'\e[0m'

.PHONY: O1
O1: kern-O1.elf
	echo '\e[31m'"WARNING: kern-01.elf NOT INSTALLED REMOTELY!"'\e[0m'

.PHONY: O2
O2: kern-O2.elf
	echo '\e[31m'"WARNING: kern-02.elf NOT INSTALLED REMOTELY!"'\e[0m'

.PHONY: O3
O3: kern-O3.elf
	echo '\e[31m'"WARNING: kern-03.elf NOT INSTALLED REMOTELY!"'\e[0m'

.PHONY: main
main: io kern
	cd src/main && $(MAKE) install
	
.PHONY: main-O0
main-O0: io-O0 kern-O0
	cd src/main && $(MAKE) install-O0

.PHONY: main-O1
main-O1: io-O1 kern-O1
	cd src/main && $(MAKE) install-O1

.PHONY: main-O2
main-O2: io-O2 kern-O2
	cd src/main && $(MAKE) install-O2

.PHONY: main-O3
main-O3: io-O3 kern-O3
	cd src/main && $(MAKE) install-O3


.PHONY: kern
kern:
	cd src/kern && $(MAKE) install

.PHONY: kern-O0
kern-O0:
	cd src/kern && $(MAKE) install-O0

.PHONY: kern-O1
kern-O1:
	cd src/kern && $(MAKE) install-O1

.PHONY: kern-O2
kern-O2:
	cd src/kern && $(MAKE) install-O2

.PHONY: kern-O3
kern-O3:
	cd src/kern && $(MAKE) install-O3


.PHONY: io
io:
	cd src/io && $(MAKE) install

.PHONY: io-O0
io-O0:
	cd src/io && $(MAKE) install-O0

.PHONY: io-O1
io-O1:
	cd src/io && $(MAKE) install-O1

.PHONY: io-O2
io-O2:
	cd src/io && $(MAKE) install-O2

.PHONY: io-O3
io-O3:
	cd src/io && $(MAKE) install-O3

kern.elf: main-O3
	cd build && $(LD) $(LDFLAGS) -o $@ main.o bwio.o ContextSwitch.o Kernel.o Syscall.o UART.o UserSyscall.o -lgcc -lc

kern-O0.elf: main-O0
	cd build && $(LD) $(LDFLAGS) -o $@ main.o bwio.o ContextSwitch.o Kernel.o Syscall.o UART.o UserSyscall.o -lgcc -lc

kern-O1.elf: main-O1
	cd build && $(LD) $(LDFLAGS) -o $@ main.o bwio.o ContextSwitch.o Kernel.o Syscall.o UART.o UserSyscall.o -lgcc -lc

kern-O2.elf: main-O2
	cd build && $(LD) $(LDFLAGS) -o $@ main.o bwio.o ContextSwitch.o Kernel.o Syscall.o UART.o UserSyscall.o -lgcc -lc

kern-O3.elf: main-O3
	cd build && $(LD) $(LDFLAGS) -o $@ main.o bwio.o ContextSwitch.o Kernel.o Syscall.o UART.o UserSyscall.o -lgcc -lc

install: kern.elf
	cd build && cp kern.elf /u/cs452/tftp/ARM/spratap/k2.elf
	chmod 777 /u/cs452/tftp/ARM/spratap/k2.elf

install-O0: kern-O0.elf
	cd build && cp kern-O0.elf /u/cs452/tftp/ARM/spratap/k2.elf
	chmod 777 /u/cs452/tftp/ARM/spratap/k2.elf

install-O1: kern-O1.elf
	cd build && cp kern-O1.elf /u/cs452/tftp/ARM/spratap/k2.elf
	chmod 777 /u/cs452/tftp/ARM/spratap/k2.elf

install-O2: kern-O2.elf
	cd build && cp kern-O2.elf /u/cs452/tftp/ARM/spratap/k2.elf
	chmod 777 /u/cs452/tftp/ARM/spratap/k2.elf

install-O3: kern-O3.elf
	cd build && cp kern-O3.elf /u/cs452/tftp/ARM/spratap/k2.elf
	chmod 777 /u/cs452/tftp/ARM/spratap/k2.elf

clean:
	cd build && rm -f *.s *.o *.S *.elf
	
