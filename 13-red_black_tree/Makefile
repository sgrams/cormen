#
# Makefile for building 1-tree program
# Stanisław J. Grams (20180406 UG MFI)
#

CC=gcc 
CFLAGS=$(shell pkg-config --cflags glib-2.0)
LDFLAGS=$(shell pkg-config --libs glib-2.0)
1-tree:	1-tree.o
1-tree.o:	1-tree.c

clean: 
	rm -f 1-tree 1-tree.o
run: 1-tree
	./1-tree
