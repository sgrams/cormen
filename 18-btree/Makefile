 ## 
 #  This file is a part of 18-btree task.
 # 
 #  Makefile
 #  Stanisław J. Grams
 ##

CC=gcc
CFLAGS=$(shell pkg-config --cflags glib-2.0)
LDFLAGS=$(shell pkg-config --libs glib-2.0)

main:	main.o btree.o
main.o:	main.c
btree.o: btree.c btree.h
clean: 
	rm -f main main.o btree.o
run: main
	./main
rmobj:
	rm -f main.o btree.o