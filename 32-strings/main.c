/*
 *  1. USAGE
 *     Compile with command `g++ -o main main.cc strings.cc
 *     $(pkg-config --cflags glib-2.0 --libs glib-2.0)`
 *
 *     Syntax: "./main"
 *     Tested under kernel 4.16.8-1-ARCH and g++ 8.1.0
 * 
 *  This file is a part of 32-strings task.
 * 
 *  main.c
 *  Stanisław J. Grams
 */

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <time.h>
#include <math.h>

#include "strings.h"

#define  DEFAULT_DICT_SIZE 128
#define  DEFAULT_PRIM_NUMB 21377
#define  MLD 1000000000.0

gint32 main (gint argc, gchar *argv[]) {
  gchar *str = NULL;
  gchar *pat = NULL;

  struct timespec t0, t1;
  double time_dif;

  // Inputting data
  printf ("Please enter text: ");
  str = str_utils_read_str (stdin);
  str_utils_remove_newline (str);

  printf ("Please enter pattern: ");
  pat = str_utils_read_str (stdin);
  str_utils_remove_newline (pat);
  
  // Matching and time-testing
  printf ("\nNaive Algorithm:\n");
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&t0);
  str_matcher_naive (str, pat);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&t1);
  time_dif = (t1.tv_sec+t1.tv_nsec/MLD)-(t0.tv_sec+t0.tv_nsec/MLD);
  printf ("~ %.5lfs.\n", time_dif);

  printf ("\nRabin-Karp Algorithm:\n");
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&t0);
  str_matcher_rabin_karp (str, pat, DEFAULT_DICT_SIZE, DEFAULT_PRIM_NUMB);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&t1);
  time_dif = (t1.tv_sec+t1.tv_nsec/MLD)-(t0.tv_sec+t0.tv_nsec/MLD);
  printf ("~ %.5lfs.\n", time_dif);

  printf ("\nKnuth-Morris-Pratt Algorithm:\n");
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&t0);
  str_matcher_knuth_morris_pratt (str, pat);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&t1);
  time_dif = (t1.tv_sec+t1.tv_nsec/MLD)-(t0.tv_sec+t0.tv_nsec/MLD);
  printf ("~ %.5lfs.\n", time_dif);

  // Freeing memory
  g_free (str);
  g_free (pat);

  return EXIT_SUCCESS;
}
