#
# Makefile for main.cpp
#
XDIR=/u/cs452/public/xdev
XBINDIR=$(XDIR)/bin
XLIBDIR1=$(XDIR)/arm-none-eabi/lib
XLIBDIR2=$(XDIR)/lib/gcc/arm-none-eabi/9.2.0/
CC = $(XBINDIR)/arm-none-eabi-gcc
AR = $(XBINDIR)/arm-none-eabi-ar
AS = $(XBINDIR)/arm-none-eabi-as
LD = $(XBINDIR)/arm-none-eabi-ld

# -g: include debug information for gdb
# -S: only compile and emit assembly
# -fPIC: emit position-independent code
# -Wall: report all warnings
# -mcpu=arm920t: generate code for the 920t architecture
# -msoft-float: no FP co-processor
CPPFLAGS = -fno-exceptions -fno-unwind-tables -fno-rtti -std=c++17 -g -S -fPIC -Wall -mcpu=arm920t -msoft-float -I. -I../../include -lstdc++ -O3
# CPPFLAGS = -fno-exceptions -fno-unwind-tables -fno-rtti -std=c++17 -g -S -fPIC -Wall -mcpu=arm920t -msoft-float -I. -I../../include -lstdc++

# c: create archive, if necessary
# r: insert with replacement
# s: add index to archive
ARFLAGS = crs

# -static: force static linking
# -e: set entry point
# -nmagic: no page alignment
# -T: use linker script
LDFLAGS = -static -e main -nmagic -T ../linker.ld -L $(XLIBDIR1) -L ../../lib -L $(XLIBDIR2) -lstdc++ -lc -lgcc

.PHONY: all
all: main.elf

main.s: main.cpp
	$(CC) -S $(CPPFLAGS) main.cpp

main.o: main.s
	$(AS) $(ASFLAGS) -o main.o main.s

# TODO: Fix this all up to be linked from somewhere sensible
main.elf: main.o
	$(LD) $(LDFLAGS) -o $@ main.o ../kern/ContextSwitch.o ../kern/Kernel.o ../kern/Syscall.o ../kern/UserSyscall.o ../io/UART.o -lbwio -lgcc -lc

.PHONY: install
install: all
	cp main.elf ../../build
	cp main.elf /u/cs452/tftp/ARM/sgaweda/k2.elf
	chmod 777 /u/cs452/tftp/ARM/sgaweda/k2.elf

.PHONY: clean
clean:
	-rm -f *.s *.a *.o main.elf
