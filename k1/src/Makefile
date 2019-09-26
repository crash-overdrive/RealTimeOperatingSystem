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
