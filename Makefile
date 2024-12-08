# Makefile principal
all:
	$(MAKE) -C source/detection
	$(MAKE) -C source/interface
	$(MAKE) -C source/neural_network
	$(MAKE) -C source/pre_processe
	$(MAKE) -C source/solver

.PHONY: clean format

clean:
	$(MAKE) -C source/detection clean
	$(MAKE) -C source/interface clean
	$(MAKE) -C source/neural_network clean
	$(MAKE) -C source/pre_processe clean
	$(MAKE) -C source/solver clean

format:
	find source -type f -name "*.c" -o -name "*.h" | xargs clang-format -i -style=file
