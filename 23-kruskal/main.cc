/*
 *  1. USAGE
 *     Compile with command `g++ -o main main.cc kruskal.cc
 *     $(pkg-config --cflags glib-2.0 --libs glib-2.0)`
 *
 *     Syntax: "./main <example.txt>"
 *     Tested under kernel 4.16.8-1-ARCH and g++ 8.1.0
 * 
 *  This file is a part of 23-kruskal task.
 * 
 *  main.cc
 *  Stanisław J. Grams
 */

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <glib.h>

#include "kruskal.h"

#define  DEFAULT_ERROR_SYNTAX "%s: Error occured!\nProgram terminated.\n"

gint32 main (gint argc, gchar *argv[]) {
  gint32 V = 0;
  gint32 E = 0;

  gchar *input_name = NULL;
  FILE  *input_file = NULL;
  
  // create new empty Graph
  Graph G = Graph (V, E);

  if (argc != 2) {
    // input data
    printf ("Please enter V (number of vertices): ");
    scanf ("%i", &V);

    printf ("Please enter E (number of edges): ");
    scanf ("%i", &E);

    if (!V || !E) {
      fprintf (stderr, DEFAULT_ERROR_SYNTAX, argv[0]);
      return EXIT_FAILURE;
    }
    printf ("\n");
    
    // create graph and proceed with adding edges
    // ie. 2;3;4
    //     1;2;1
    G.Set (V, E);

    printf ("Please enter %i edges with their ranks.\n", E);
    for (gint32 i=0; i < E; i++)
    {
      gint32 u = 0, v = 0, w = 0;
      scanf ("%i;%i;%i", &u, &v, &w);
      G.AddEdge (u, v, w);
    }
  } else {
    input_name = g_strdup(argv[1]);
    input_file = fopen ((const gchar *) input_name, "rb");
    if (!input_file) {
      g_free  (input_name);
      fprintf (stderr, DEFAULT_ERROR_SYNTAX, argv[0]);
      return EXIT_FAILURE;
    }

    // create graph and proceed with adding edges from file
    fscanf (input_file, "%i;%i\n", &V, &E);
    G.Set  (V, E);

    for (gint32 i=1; i <= E; i++)
    {
      gint32 u=0, v=0, w=0;
      fscanf (input_file, "%i;%i;%i\n", &u, &v, &w);
      G.AddEdge (u, v, w);
    }
  }

  // output the data
  G.FindKruskalMST ();
  printf ("%i;%i\n", (gint32) G.getVerticesNumber(), (gint32) G.getKruskals().size()-1);
  
  // iterate over all kruskal edges
  vector<pair<gint32, pair<gint32, gint32>>>::iterator iter;
  for (iter = G.getKruskals ().begin ()+1; iter != G.getKruskals ().end (); iter++)
  {
    gint32 w = iter->first;
    gint32 u = iter->second.first;
    gint32 v = iter->second.second;

    printf ("%i;%i;%i\n", u, v, w);
  }

  // free memory
  g_free (input_name);

  if (input_file)
    fclose (input_file);
  return EXIT_SUCCESS;
}