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
CPPFLAGS = -fno-exceptions -fno-unwind-tables -fno-rtti -std=c++17 -g -S -fPIC -Wall -mcpu=arm920t -msoft-float -I. -I../../include -lstdc++

.PHONY: all
all: Kernel.o Syscall.o UserSyscall.o ContextSwitch.o TimingTasks.o

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

TimingTasks.s: TimingTasks.cpp
	$(CC) -S $(CPPFLAGS) TimingTasks.cpp

TimingTasks.o: TimingTasks.s
	$(AS) $(ASFLAGS) -o TimingTasks.o TimingTasks.s

.PHONY: install
install: all
	mv *.s *.o ../../build
	cp *.S ../../build

.PHONY: install-O0
install-O0: CPPFLAGS+=-O0
install-O0: all
	mv *.s *.o ../../build
	cp *.S ../../build

.PHONY: install-O1
install-O1: CPPFLAGS+=-O1
install-O1: all
	mv *.s *.o ../../build
	cp *.S ../../build

.PHONY: install-O2
install-O2: CPPFLAGS+=-O2
install-O2: all
	mv *.s *.o ../../build
	cp *.S ../../build

.PHONY: install-O3
install-O3: CPPFLAGS+=-O3
install-O3: all
	mv *.s *.o ../../build
	cp *.S ../../build

.PHONY: clean
clean:
	-rm -f *.s *.o
