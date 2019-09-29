#
# Makefile for Kernel.cpp Syscall.cpp UserSyscall.cpp ContextSwitch.S
#
XDIR=/u/cs452/public/xdev
XBINDIR=$(XDIR)/bin
XLIBDIR1=$(XDIR)/arm-none-eabi/lib
XLIBDIR2=$(XDIR)/lib/gcc/arm-none-eabi/9.2.0/
CC = $(XBINDIR)/arm-none-eabi-gcc
AS = $(XBINDIR)/arm-none-eabi-as

# -g: include debug information for gdb
# -S: only compile and emit assembly
# -fPIC: emit position-independent code
# -Wall: report all warnings
# -mcpu=arm920t: generate code for the 920t architecture
# -msoft-float: no FP co-processor
CPPFLAGS = -fno-exceptions -fno-unwind-tables -fno-rtti -std=c++17 -g -S -fPIC -Wall -mcpu=arm920t -msoft-float -I. -I../../include -lstdc++ -lc -O3

.PHONY: all
all: Kernel.o Syscall.o UserSyscall.o ContextSwitch.o

Kernel.s: Kernel.cpp
	$(CC) -S $(CPPFLAGS) Kernel.cpp

Kernel.o: Kernel.s
	$(AS) $(ASFLAGS) -o Kernel.o Kernel.s

Syscall.s: Syscall.cpp
	$(CC) -S $(CPPFLAGS) Syscall.cpp

Syscall.o: Syscall.s
	$(AS) $(ASFLAGS) -o Syscall.o Syscall.s

UserSyscall.s: UserSyscall.cpp
	$(CC) -S $(CPPFLAGS) UserSyscall.cpp

UserSyscall.o: UserSyscall.s
	$(AS) $(ASFLAGS) -o UserSyscall.o UserSyscall.s

ContextSwitch.o:
	$(AS) $(ASFLAGS) -o ContextSwitch.o ContextSwitch.S

.PHONY: install
install: all
	mv *.s *.o ../../build

.PHONY: clean
clean:
	-rm -f *.s *.o
