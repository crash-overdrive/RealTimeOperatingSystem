#
# Makefile for busy-wait IO library
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

.PHONY: all
all:  bwio.a UART.o

bwio.s: bwio.c 
	$(CC) -S $(CPPFLAGS) bwio.c

bwio.o: bwio.s
	$(AS) $(ASFLAGS) -o bwio.o bwio.s

bwio.a: bwio.o
	$(AR) $(ARFLAGS) $@ bwio.o

UART.s: UART.cpp
	$(CC) -S $(CPPFLAGS) UART.cpp

UART.o: UART.s
	$(AS) $(ASFLAGS) -o UART.o UART.s

.PHONY: install
install: all
	cp bwio.a ../../lib/libbwio.a

.PHONY: clean
clean:
	-rm -f *.s *.a *.o main.elf ../../lib/libbwio.a
