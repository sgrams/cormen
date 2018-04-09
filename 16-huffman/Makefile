#
# Makefile for building 16-huffman program
# Stanisław J. Grams
#

CC=gcc 
CFLAGS=$(shell pkg-config --cflags glib-2.0)
LDFLAGS=$(shell pkg-config --libs glib-2.0)
main:	main.o huff.o
main.o:	main.c
huff.o: huff.c huff.h
clean: 
	rm -f main main.o huff.o
run: main
	./main
