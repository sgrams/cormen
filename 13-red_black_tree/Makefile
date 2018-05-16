#
# Makefile for building 1-tree program
# Stanisław J. Grams (20180406 UG MFI)
#

CC=gcc 
CFLAGS=$(shell pkg-config --cflags glib-2.0)
LDFLAGS=$(shell pkg-config --libs glib-2.0)
main:	main.o
main.o:	main.c

clean: 
	rm -f main main.o
run: main
	./main
