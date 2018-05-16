 ## 
 #  This file is a part of 23-kruskal task.
 # 
 #  Makefile
 #  Stanisław J. Grams
 ##

CC=g++
CXXFLAGS=$(shell pkg-config --cflags glib-2.0) -Wall -Werror
LDFLAGS=$(shell pkg-config --libs glib-2.0)

main:	main.o kruskal.o
main.o:	main.cc
kruskal.o: kruskal.cc

clean: 
	rm -f main main.o kruskal.o
run: main
	./main
rmobj:
	rm -f main.o kruskal.o