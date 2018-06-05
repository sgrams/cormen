 ## 
 #  This file is a part of 22-elementary_graph_algs task.
 # 
 #  Makefile
 #  Stanisław J. Grams
 ##

CC=g++
CXXFLAGS=$(shell pkg-config --cflags glib-2.0) -Wall -Werror
LDFLAGS=$(shell pkg-config --libs glib-2.0)

main:	main.o graph.o util.o
main.o:	main.cc
graph.o: graph.cc
util.o: util.cc

clean: 
	rm -f main main.o graph.o
run: main
	./main
rmobj:
	rm -f main.o graph.o