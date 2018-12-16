/*
 *  1. USAGE
 *     Compile with command `g++ -o main main.cc
 *     $(pkg-config --cflags glib-2.0 --libs glib-2.0)`
 *
 *     Syntax: "./main"
 *     Tested under kernel 4.16.8-1-ARCH and g++ 8.1.0
 * 
 *  This file is a part of 18-btree task.
 * 
 *  main.cc
 *  Stanisław J. Grams
 */

#include <iostream>
#include <cstdlib>
#include <glib.h>
#include "btree.h"

#define DEFAULT_M_NUMBER    3

gint main (gint argc, gchar *argv[]) {
  btree_t *bt = btree_create (bt);

  btree_destroy (bt);
  return EXIT_SUCCESS;
}