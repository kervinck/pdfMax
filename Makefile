CFLAGS:=-Wall -O3 -std=c99
LOADLIBES:=-lm

all: module pdfMaxTest

module:
	python setup.py build

test:
	@echo "Raw:"
	./pdfMaxTest
	@echo "Module:"
	python pdfMaxTest.py

pdfMaxTest : pdfMaxTest.c pdfMax.c

install:
	python setup.py install --user

clean:
	python setup.py clean

# vi: noexpandtab
