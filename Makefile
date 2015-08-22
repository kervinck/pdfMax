CFLAGS=-Wall -O3

all: module

module:
	python setup.py build

test:
	python pdfMaxTest.py

pdfMaxTest : pdfMaxTest.c pdfMax.c

install:
	python setup.py install --user

clean:
	python setup.py clean

# vi: noexpandtab
