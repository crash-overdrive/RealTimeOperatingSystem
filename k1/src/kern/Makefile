#
# Makefile for Kernel.cpp
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
CPPFLAGS = -fno-exceptions -fno-unwind-tables -fno-rtti -std=c++17 -g -S -fPIC -Wall -mcpu=arm920t -msoft-float -I. -I../../include -lstdc++ -lc

# c: create archive, if necessary
# r: insert with replacement
# s: add index to archive
ARFLAGS = crs

.PHONY: all
all: Kernel.o Syscall.o ContextSwitch.o

Kernel.s: Kernel.cpp
	$(CC) -S $(CPPFLAGS) Kernel.cpp

Kernel.o: Kernel.s
	$(AS) $(ASFLAGS) -o Kernel.o Kernel.s

Syscall.s: Syscall.cpp
	$(CC) -S $(CPPFLAGS) Syscall.cpp

Syscall.o: Syscall.s
	$(AS) $(ASFLAGS) -o Syscall.o Syscall.s

ContextSwitch.o:
	$(AS) $(ASFLAGS) -o ContextSwitch.o ContextSwitch.S

.PHONY: install
install: all

.PHONY: clean
clean:
	-rm -f *.s *.a *.o
