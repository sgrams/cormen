/*
 *  1. USAGE
 *     Compile with command `gcc -o main main.c huff.c
 *     $(pkg-config --cflags glib-2.0 --libs glib-2.0)`
 *
 *     Syntax: "./main -i input.txt"
 *     Tested under kernel 4.15.7-1-ARCH and gcc 7.3.0
 * 
 *  This is part of 16-huffman task.
 * 
 *  main.c
 *  Stanisław J. Grams
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <glib.h>

#include "huff.h"

#define DEFAULT_SYNTAX          "Syntax: ./main -i input.txt\n"
#define DEFAULT_ERROR_SYNTAX    "%s: \033[31mfatal error:\033[0m "

gint main (gint argc, gchar *argv[]) {
  FILE *input_file  = NULL;
  FILE *output_file = NULL;

  gchar *input_filepath  = NULL;
  gchar *output_filepath = NULL;
  gchar  getopt_input;

  guchar buff;

  huff_t *huff = NULL;

  while ((getopt_input = getopt(argc, argv, "i:")) != -1)
  {
    switch (getopt_input) {
      case 'i':
        // allocates memory and sets input filepath
        // case 1: optarg empty
        if (!optarg) {
          fprintf(stderr, DEFAULT_ERROR_SYNTAX
              "input filepath not specified\nprogram terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        input_filepath = g_strdup(optarg);
        input_file = fopen((const gchar *) input_filepath, "rb");
        // case 2: optarg specified but unable to open file
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

  for (gint n=1; fread(&buff, sizeof(guchar), 1, input_file) && !feof(input_file);n++)
  {
    // let's insert another byte into tree
    huff_append(huff, buff);
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

  return EXIT_SUCCESS;
}