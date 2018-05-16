#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include "btree.h"

#define DEFAULT_M_NUMBER    3

gint main (gint argc, gchar *argv[]) {
  btree_t *bt = btree_create (bt);

  btree_destroy (bt);


  return EXIT_SUCCESS;
}