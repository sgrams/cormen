/*
 *  This file is a part of 32-strings task.
 * 
 *  strings.h
 *  Stanisław J. Grams
 */
#ifndef STRINGS_H_
#define STRINGS_H_

#include <string.h>
#include <stdio.h>
#include <glib.h>

#define DEFAULT_BUFF_SIZE 1024

gint
str_matcher_naive
(gchar *str, gchar *pat);

gint
str_matcher_rabin_karp
(gchar *str, gchar *pat, gint d, gint q);

gint
str_matcher_knuth_morris_pratt
(gchar *str, gchar *pat);

gint *
str_utils_comp_prefix_func
(gchar *str);

void
str_utils_remove_newline
(gchar *str);

gchar *
str_utils_read_str
(FILE *in);

#endif