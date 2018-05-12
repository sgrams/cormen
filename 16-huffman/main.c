/*
 *  1. USAGE
 *     Compile with command `gcc -o main main.c huff.c
 *     $(pkg-config --cflags glib-2.0 --libs glib-2.0)`
 *
 *     Syntax: "./main -i input.txt"
 *     Tested under kernel 4.15.7-1-ARCH and gcc 7.3.0
 * 
 *  This file is a part of 16-huffman task.
 * 
 *  main.c
 *  Stanisław J. Grams
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <glib.h>
#include "huff.h"

#define DEFAULT_SYNTAX                    "Syntax: %s [-d (decode) || -e (encode)] -i <input_file> -o <output_file>\n"
#define DEFAULT_ERROR_SYNTAX              "%s: \033[31mfatal error:\033[0m "

#define DEFAULT_ENCODE_OUTPUT_EXTENSION   ".heff"
#define DEFAULT_ENCODE_OUTPUT_FILE        "encoded"
#define DEFAULT_DECODE_OUTPUT_FILE        "decoded"

gint32 main (gint32 argc, gchar *argv[]) {
  FILE *input_file  = NULL;
  FILE *output_file = NULL;

  gchar *input_filepath  = NULL;
  gchar *output_filepath = NULL;

  huff_encode_t *encode_huff = NULL;
  huff_decode_t *decode_huff = NULL;

  guint8 *buff = NULL;
  gchar   getopt_input;

  gboolean encode_flag = FALSE;
  gboolean decode_flag = FALSE;

  gint32 i, huff_length = 0;

  while ((getopt_input = getopt(argc, argv, "edi:o:")) != -1)
  {
    switch (getopt_input) {
      case 'e':
        encode_flag = TRUE;
        break;
      case 'd':
        decode_flag = TRUE;
        break;
      case 'i':
        // allocates memory and sets input filepath
        // case 1: optarg empty
        if (!optarg) {
          fprintf (stderr, DEFAULT_ERROR_SYNTAX
              "input filepath not specified\nprogram terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        input_filepath = g_strdup (optarg);
        input_file = fopen ((const gchar *) input_filepath, "rb");
        // case 2: optarg specified but unable to open file
        if (!input_file) {
          fprintf(stderr, DEFAULT_ERROR_SYNTAX
              "input filepath does not exist!\nprogram terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        break;
      case 'o':
        if (!optarg) {
          fprintf (stderr, DEFAULT_ERROR_SYNTAX
              "output filepath not specified\nprogram terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        output_filepath = g_strdup (optarg);
        output_file = fopen ((const gchar *) output_filepath, "w+");
        // case 2: optarg specified but unable to open file
        if (!output_file) {
          fprintf(stderr, DEFAULT_ERROR_SYNTAX
              "output filepath does not exist!\nprogram terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        break;
      case '?':
        return EXIT_FAILURE;
      default:
        break;
    }
  }
  if (!encode_flag  && !decode_flag) {
    fprintf (stderr, DEFAULT_SYNTAX, argv[0]);
    return EXIT_FAILURE;
  }

  if (!input_file) {
    fprintf (stderr, DEFAULT_SYNTAX, argv[0]);
    if (input_filepath)
      g_free (input_filepath);
    return EXIT_FAILURE;
  }

  if (!output_file && encode_flag) {
    output_filepath = g_strdup_printf ("%s.huff", input_filepath);
    output_file = fopen ((const gchar *) output_filepath, "w+");
    if (!output_file) {
      fprintf(stderr, DEFAULT_ERROR_SYNTAX
      "unable to create output file!\nprogram terminated\n", argv[0]);
      return EXIT_FAILURE;
    }
  }

  if (encode_flag && decode_flag) {
    fprintf(stderr, DEFAULT_ERROR_SYNTAX
        "cannot decode and encode at once!\nprogram terminated\n", argv[0]);
    return EXIT_FAILURE;
  }

  // reads the size of file
  fseek (input_file, 0, SEEK_END);
  huff_length = ftell (input_file);
  rewind (input_file);

  if (huff_length <= 2) {
    fprintf (stderr, DEFAULT_ERROR_SYNTAX
    "file contains less than three bytes\nprogram terminated\n", argv[0]);
    
    fclose (input_file);
    g_free (input_filepath);
    return EXIT_FAILURE;
  }

  buff = g_malloc0 ((huff_length) * sizeof(guint8));
  fread (buff, sizeof(guint8), huff_length, input_file);

  if (decode_flag)
  {
    /*
     * THIS PART CONTROLS FILE DECODING
     */
    rewind (input_file);
    decode_huff = huff_decode_init((gsize)huff_length, 0);
    // let's decode header and the dictionary
    if (huff_read_head (input_file, decode_huff) == -1) {
      fprintf(stderr, DEFAULT_ERROR_SYNTAX
      "input file is not a HUFF file\nprogram terminated\n", argv[0]);
      g_free (buff);
      huff_decode_close (decode_huff);
      return EXIT_FAILURE;
    }
    huff_read_code (output_file, decode_huff, input_file);

    huff_decode_close (decode_huff);
  }
  else {
    /*
     * THIS PART CONTROLS FILE ENCODING
     */

    // initialize huff structure with huff_length and unique size of 0
    encode_huff = huff_encode_init ((gsize) huff_length, 0);
    for (i=0; i<encode_huff->size; i++)
    {
      huff_append (encode_huff, *(buff+i));
    }

    // create Huffman's tree and code
    encode_huff = huff_create_tree (encode_huff);
    encode_huff = huff_create_dict (encode_huff);
    
    // DEBUG!!!
    //huff_print_dict (encode_huff);

    // write huff structure into output_file
    huff_save_head   (output_file, encode_huff);
    huff_save_code   (output_file, encode_huff, buff);
    huff_save_crc32  (output_file);
      
    // free memory
    huff_encode_close (encode_huff);
  }
  
  g_free (buff);

  fclose (input_file);
  fclose (output_file);

  g_free (input_filepath);
  g_free (output_filepath);

  return EXIT_SUCCESS;
}