# Makefile for adapter tests

.PHONY: all test clean

all: adapter_test

adapter_test:
	./build_test.sh

test: adapter_test
	./adapter_test

clean:
	rm -f adapter_test
