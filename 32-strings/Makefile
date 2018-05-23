 ## 
 #  This file is a part of 32-strings task.
 # 
 #  Makefile
 #  Stanisław J. Grams
 ##

CC=gcc
CFLAGS=$(shell pkg-config --cflags glib-2.0) -Wall -Werror
LDFLAGS=$(shell pkg-config --libs glib-2.0) -lm

main:	main.o strings.o
main.o:	main.c
strings.o: strings.c

clean: 
	rm -f main main.o strings.o
run: main
	./main
rmobj:
	rm -f main.o strings.o