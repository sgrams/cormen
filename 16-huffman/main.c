/* AL2.3.1, Stanisław Juliusz Grams (251000 UG MFI), 20180406
 *
 * 1. USAGE
 *
 *   Compile with command `gcc -o 1-huff 1-huff.c
 *   `pkg-config --cflags glib-2.0 --libs glib-2.0`
 *
 *   Syntax: "./1-huff"
 *
 *   Tested under kernel 4.15.7-1-ARCH and gcc 7.3.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <glib.h>

#include "huff.h"

#define defaultStringBuffer   1024
#define defaultSyntax         "ok\n"
#define defaultErrorSyntax    "zesrauosie\n"

gint main (gint argc, gchar *argv[]) {
  FILE *input_file = NULL;
  FILE *output_file = NULL;

  gchar *input_filepath = NULL;
  gchar *output_filepath = NULL;
  gchar  ch;

  huff_t *huff = NULL;

  while ((ch = getopt(argc, argv, "i:")) != -1)
  {
    switch (ch) {
      case 'i':
        // Allocates memory and sets input filepath
        if (!optarg) {
          fprintf(stderr, defaultErrorSyntax
              "input filepath not specified\n program terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        input_filepath = g_strdup(optarg);
        input_file = fopen((const gchar *) input_filepath, "r");
        if (!input_file) {
          fprintf(stderr, defaultErrorSyntax
              "input filepath not specified\n program terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        break;
      case '?':
        return EXIT_FAILURE;
      default:
        break;
    }
  }

  /* Reading file and calculating quantity of every char */
  if (!input_file) {
    printf (defaultSyntax, argv[0]);
    if (input_filepath)
      g_free (input_filepath);
    return EXIT_SUCCESS;
  }

  printf("input_filepath=%s\n", input_filepath);
  // initialize huff structure with NULL pointer to data and unique size of 0
  huff = huff_init (NULL, 0);

  for (gint n=0; (ch = fgetc (input_file)) && !feof (input_file);)
  {
    ++n;
    huff_tree_append(huff, ch);
    huff->size = n;
  }

  for (huff_list_t *iter = huff->list; iter; iter=iter->next)
  {
    printf("%c", *(iter->tree->entry->uniq_byte));
  }
  printf("\n");

  huff_create_code(huff);
  
  /*for (huff_list_t *iter = huff->list; iter; iter=iter->next)
  {
    printf("xx%i\n", (iter->tree->entry->quantity));
  }*/

  printf("znakow ogolnie: %i\n", huff->size);
  
  huff_close(huff);
  if (input_file)
    fclose(input_file);
  g_free(input_filepath);
  return 0;
}