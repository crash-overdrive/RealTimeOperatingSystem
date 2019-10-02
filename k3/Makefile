#
# Makefile for entire kernel
#
XDIR=/u/cs452/public/xdev
XBINDIR=$(XDIR)/bin
XLIBDIR1=$(XDIR)/arm-none-eabi/lib
XLIBDIR2=$(XDIR)/lib/gcc/arm-none-eabi/9.2.0/
CC = $(XBINDIR)/arm-none-eabi-gcc
AS = $(XBINDIR)/arm-none-eabi-as
LD = $(XBINDIR)/arm-none-eabi-ld

# -static: force static linking
# -e: set entry point
# -nmagic: no page alignment
# -T: use linker script
LDFLAGS = -static -e main -nmagic -T src/linker.ld -L $(XLIBDIR1) -L lib -L $(XLIBDIR2) -lstdc++

# -g: include debug information for gdb
# -S: only compile and emit assembly
# -fPIC: emit position-independent code
# -Wall: report all warnings
# -mcpu=arm920t: generate code for the 920t architecture
# -msoft-float: no FP co-processor
CPPFLAGS = -fno-exceptions -fno-unwind-tables -fno-rtti -std=c++17 -g -S -fPIC -Wall -mcpu=arm920t -msoft-float -I. -I./include -lstdc++

CPPFILES := $(shell find src -name '*.cpp')
ASMFILES := $(shell find build -name '*.s')

ASM := $(patsubst src/%.cpp, build/%.s , $(CPPFILES))
OBJECTS := $(patsubst build/%.s, build/%.o, $(ASMFILES))

build/%.s: src/%.cpp
	$(CC) $(CPPFLAGS) $< -o build/$(notdir $@)
	

$(ELF): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ main.o bwio.o ContextSwitch.o Kernel.o Syscall.o UART.o UserSyscall.o TimingTasks.o -lgcc -lc

$(OBJECTS): $(ASM)
	$(AS) $(ASFLAGS) -o Kernel.o Kernel.s



clean:
	cd build && rm -f *.s *.o *.S *.elf
	
