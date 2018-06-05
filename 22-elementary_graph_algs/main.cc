/*
 *  1. USAGE
 *     Compile with command `g++ -o main main.cc kruskal.cc
 *     $(pkg-config --cflags glib-2.0 --libs glib-2.0)`
 *
 *     Syntax: "./main <example.txt>"
 *     Tested under kernel 4.16.8-1-ARCH and g++ 8.1.0
 * 
 *  This file is a part of 22-elementary_graph_algs task.
 * 
 *  main.cc
 *  Stanisław J. Grams
 */
#include <glib.h>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "graph.h"

#define DEFAULT_SYNTAX       "Syntax: %s -i <input_file> -p [prints graph] -d [prints graph in DFS order]\n"
#define DEFAULT_ERROR_SYNTAX "%s: \033[31mfatal error:\033[0m "

using namespace std;

gint32 main (gint32 argc, gchar **argv) {
  ifstream input_file;
  gchar  getopt_input;

  gint32 vertices = 0;

  bool print_flag = false;
  bool   DFS_flag = false;


  while ((getopt_input = getopt (argc, argv, "i:pd")) != -1)
  {
    switch (getopt_input) {
      case 'i':
        // allocates memory and sets input filepath
        input_file.open (optarg);

        // if optarg specified but unable to open file
        if (!input_file.is_open()) {
          fprintf (stderr, DEFAULT_ERROR_SYNTAX "unable to open input file!\nProgram terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        break;
      case 'p':
        print_flag = true;
        break;
      case 'd':
        DFS_flag = true;
        break;
      default:
        return EXIT_FAILURE;
    }
  }
  
  if (argc == 1 || (!DFS_flag && !print_flag)) {
    fprintf (stderr, DEFAULT_SYNTAX, argv[0]);
    return EXIT_FAILURE;
  }

  if (!input_file.is_open()) {
    fprintf (stderr, DEFAULT_ERROR_SYNTAX "unable to open input file!\nProgram terminated\n", argv[0]);
    return EXIT_FAILURE;
  }

  input_file >> vertices;

  Graph G (vertices);
  G.read_from_adj_matrix_file (input_file);

  if (print_flag) {
    cout << "Graph printed in form of adjacency list:" << endl;
    G.print ();
  }
  
  if (DFS_flag) {
    cout << "Graph printed in DFS order:" << endl;
    G.DFS ();
  }
  
  return EXIT_SUCCESS;
}
