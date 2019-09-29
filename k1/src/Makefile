.PHONY: all
all: main

.PHONY: main
main: io kern
	cd main && $(MAKE) clean && $(MAKE) install

.PHONY: kern
kern:
	cd kern && $(MAKE) clean && $(MAKE) install

.PHONY: io
io:
	cd io && $(MAKE) clean && $(MAKE) install

clean:
	cd main && rm -f *.s *.a *.o main.elf
	cd kern && rm -f *.s *.a *.o
	cd io && rm -f *.s *.a *.o
