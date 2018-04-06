#
# Makefile for building 1-lcs program
# Stanisław J. Grams
#

CC=gcc 
CFLAGS=$(shell pkg-config --cflags glib-2.0)
LDFLAGS=$(shell pkg-config --libs glib-2.0)
1-lcs:	1-lcs.o
1-lcs.o:	1-lcs.c

clean: 
	rm -f 1-lcs 1-lcs.o
run: 1-lcs
	./1-lcs