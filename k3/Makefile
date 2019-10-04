#
# Makefile for entire kernel
#
XDIR=/u/cs452/public/xdev
XBINDIR=$(XDIR)/bin
XLIBDIR1=$(XDIR)/arm-none-eabi/lib
XLIBDIR2=$(XDIR)/lib/gcc/arm-none-eabi/9.2.0/
CPP = $(XBINDIR)/arm-none-eabi-g++
AS = $(XBINDIR)/arm-none-eabi-as
LD = $(XBINDIR)/arm-none-eabi-ld

# -static: force static linking
# -e: set entry point
# -nmagic: no page alignment
# -T: use linker script
LDFLAGS = -static -e main -nmagic -T ../src/linker.ld -L $(XLIBDIR1) -L lib -L $(XLIBDIR2) -lstdc++

# -g: include debug information for gdb
# -S: only compile and emit assembly
# -fPIC: emit position-independent code
# -Wall: report all warnings
# -mcpu=arm920t: generate code for the 920t architecture
# -msoft-float: no FP co-processor
CPPFLAGS = -fno-exceptions -fno-unwind-tables -fno-rtti -std=c++17 -g -S -fPIC -Wall -mcpu=arm920t -msoft-float -I. -I./include -lstdc++

OPTIMIZATION := $(if $(O),$(addprefix -O, $(O)),-O3)
CPPFLAGS += $(OPTIMIZATION)

CPPFILES := $(shell find src -name '*.cpp')

SFILES := $(patsubst %.s, %.o, $(shell find src -name '*.s'))

ASMFILES := $(patsubst %.cpp,%.s,$(CPPFILES))

OBJECTS := $(patsubst %.s, %.o, $(ASMFILES))

ELF := k3.elf

all : $(ELF)
	cp build/k3.elf /u/cs452/tftp/ARM/spratap

$(ELF): $(OBJECTS) $(SFILES)
	cd build && $(LD) $(LDFLAGS) -o $@ *.o -lgcc -lc

$(OBJECTS): %.o : %.s
	$(AS) $(ASFLAGS) -o $(addprefix build/, $(notdir $@)) $(addprefix build/, $(notdir $<)) 

$(SFILES): %.o : %.s
	$(AS) $(ASFLAGS) -o   $(addprefix build/, $(notdir $@)) $<

$(ASMFILES): %.s : %.cpp
	$(CPP) $(CPPFLAGS) $< -o $(addprefix build/, $(notdir $@))

clean:
	cd build && rm -f *.s *.o *.elf
	
