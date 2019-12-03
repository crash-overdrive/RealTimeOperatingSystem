#
# Makefile for entire kernel
#
XDIR	= /u/cs452/public/xdev
XBINDIR	= $(XDIR)/bin
XLIBDIR1= $(XDIR)/arm-none-eabi/lib
XLIBDIR2= $(XDIR)/lib/gcc/arm-none-eabi/9.2.0/
CPP 	= $(XBINDIR)/arm-none-eabi-g++
AS 		= $(XBINDIR)/arm-none-eabi-as
LD 		= $(XBINDIR)/arm-none-eabi-ld

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

OPTIMIZATION:= $(if $(O),$(addprefix -O, $(O)),-O3)
CPPFLAGS		+= $(OPTIMIZATION)

SRCDIRS		:= $(shell find src -type d)
OBJDIRS		:= $(patsubst src%,obj%,$(SRCDIRS))

ECH01 := $(echo $(SRCDIRS))
ECHO2 := $(echo $(OBJDIRS))

CPPFILES	:= $(shell find src -type f -name '*.cpp')
SFILES		:= $(shell find src -type f -name '*.s')
OBJFILES	:= $(patsubst src%,obj%,$(patsubst %.cpp,%.o,$(CPPFILES)) $(patsubst %.s,%.o,$(SFILES)))
OBJFILESA	:= $(patsubst src%,obj%,$(patsubst %.cpp,%.o,$(CPPFILES)))
OBJFILESS	:= $(patsubst src%,obj%,$(patsubst %.s,%.o,$(SFILES)))
ASMFILES	:= $(patsubst src%,obj%,$(patsubst %.cpp,%.a,$(CPPFILES)))

OBJDIR		= obj
BUILDDIR	= build
ELF			:= tc2.elf
ELFPATH		:= $(BUILDDIR)/$(ELF)

.PHONY: all dirs clean

all : dirs $(ELFPATH)
	@ cp $(ELFPATH) /u/cs452/tftp/ARM/$(USER)
	@ chmod 777 /u/cs452/tftp/ARM/$(USER)/tc1.elf

$(ELFPATH): $(OBJFILESS) $(OBJFILESA) $(ASMFILES)
	$(LD) $(LDFLAGS) -o $@ $(OBJFILES) -lgcc -lc

.SECONDEXPANSION:
$(OBJFILESA): $$(patsubst %.o,%.a,$$@)
	$(AS) $(ASFLAGS) -o $@ $(patsubst %.o,%.a,$@)

$(OBJFILESS): $$(patsubst obj%,src%,$(patsubst %.o,%.s,$$@))
	 $(AS) $(ASFLAGS) -o $@ $(patsubst obj%,src%,$(patsubst %.o,%.s,$@))

$(ASMFILES): $$(patsubst obj%,src%,$$(patsubst %.a,%.cpp,$$@))
	$(CPP) $(CPPFLAGS) $(patsubst obj%,src%,$(patsubst %.a,%.cpp,$@)) -o $@

dirs:
	@ mkdir -p $(BUILDDIR) $(OBJDIRS)

clean:
	@ rm -r $(BUILDDIR)/$(ELF) $(OBJDIR)
