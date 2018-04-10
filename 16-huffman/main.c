/* AL2.3.1, Stanisław Juliusz Grams (251000 UG MFI), 20180406
 *
 * 1. USAGE
 *
 *   Compile with command `gcc -o enco enco.c
 *   `pkg-config --cflags glib-2.0 --libs glib-2.0`
 *
 *   Syntax: "./enco -i input.txt"
 *
 *   Tested under kernel 4.15.7-1-ARCH and gcc 7.3.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <glib.h>

#include "huff.h"

#define DEFAULT_SYNTAX          "Syntax: ./enco -i input.txt\n"
#define DEFAULT_ERROR_SYNTAX    "%s: \033[31mfatal error:\033[0m "

gint main (gint argc, gchar *argv[]) {
  FILE *input_file = NULL;
  FILE *output_file = NULL;

  gchar *input_filepath = NULL;
  gchar *output_filepath = NULL;
  gchar  ch;

  guchar buf;

  huff_t *huff = NULL;

  while ((ch = getopt(argc, argv, "i:")) != -1)
  {
    switch (ch) {
      case 'i':
        // allocates memory and sets input filepath
        if (!optarg) {
          fprintf(stderr, DEFAULT_ERROR_SYNTAX
              "input filepath not specified\nprogram terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        input_filepath = g_strdup(optarg);
        input_file = fopen((const gchar *) input_filepath, "rb");
        if (!input_file) {
          fprintf(stderr, DEFAULT_ERROR_SYNTAX
              "input filepath does not exist!\nprogram terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        break;
      case '?':
        return EXIT_FAILURE;
      default:
        break;
    }
  }

  // reading file and calculating quantity of every char
  if (!input_file) {
    printf (DEFAULT_SYNTAX, argv[0]);
    if (input_filepath)
      g_free (input_filepath);
    return EXIT_SUCCESS;
  }

  // initialize huff structure with NULL pointer to data, size 0 and unique size of 0
  huff = huff_init (NULL, 0, 0);

  for (gint n=0; fread(&buf, sizeof(guchar), 1, input_file) && !feof(input_file);)
  {
    ++n;
    huff_append(huff, buf);
    huff->size = n;
  }

  // create Huffman's tree and code
  huff = huff_create_tree (huff);
  huff = huff_create_code (huff);
  
  // call huff_print_dict to print information about dictionary
  huff_print_dict (huff);
  
  // free memory
  huff_close(huff);
  fclose(input_file);
  g_free(input_filepath);

  return 0;
}