/*
 *  This file is a part of 32-strings task.
 * 
 *  strings.c
 *  Stanisław J. Grams
 */

#include "strings.h"

gint
str_matcher_naive (gchar *str, gchar *pat) {
  gint n, m, s, j;
  
  if (!str || !pat) {
    return 1;
  }
  
  n = strlen (str);
  m = strlen (pat);
  
  for (s=0; s <= (n - m); s++)
  {
    for (j=0; j < m; j++)
    {
      if (str[s + j] != pat[j]) {
        break;
      }
    }

    if (j == m) {
      fprintf (stdout, "Pattern occurs with shift %i.\n", s);
    }
  }

  return 0;
}

gint
str_matcher_rabin_karp (gchar *str, gchar *pat, gint d, gint q) {
  gint n, m, s, i, j;
  gint p = 0, t = 0, h = 1;

  if (!str || !pat) {
    return 1;
  }

  n = strlen (str);
  m = strlen (pat);

  // calculate value of h = d^(m-1) mod q 
  for (i=0; i < m-1; i++)
  {
    h = (d * h) % q;
  }

  // preprocessing
  for (i=0; i < m; i++)
  {
    p = (d*p + pat[i]) % q;
    t = (d*t + str[i]) % q;
  }

  // matching
  for (s=0; s <= n-m; s++)
  {
    if (p == t) {
      for (j=0; j < m; j++)
      {
        if (str[s+j] != pat[j]) {
          break;
        }
      }
      if (j == m) {
        fprintf (stdout, "Pattern occurs with shift %i.\n", s);
      }
    }
    if (s < n-m) {
      t = (d*(t - str[s] * h) + str[s+m]) % q;

      if (t < 0) {
        t = t+q;
      }
    }
  }

  return 0;
}

gint
str_matcher_knuth_morris_pratt (gchar *str, gchar *pat) {
  gint  n, m, q, i;
  gint *PI;

  if (!str || !pat) {
    return 1;
  }
  
  n  = strlen (str);
  m  = strlen (pat);
  PI = str_utils_comp_prefix_func (pat);
  q  = -1;

  for (i=0; i < n; i++)
  {
    while (q > -1 && (pat[q+1] != str[i]))
    {
      q = PI[q];
    }
    if (pat[q+1] == str[i]) {
      q++;
    }
    // if all pattern matches
    if (q == m-1) {
      fprintf (stdout, "Pattern occurs with shift %i.\n", i-q);
      // look for the next match
      q = PI[q];
    }
  }

  g_free (PI);
  return 0;
}

gint *
str_utils_comp_prefix_func (gchar *str) {
  gint  m, *PI;

  if (!str) {
    return NULL;
  }

  m  = strlen (str);
  PI = g_malloc0 (m * sizeof (gint));

  PI[0] = -1;

  for (gint q=1, k=-1; q < m; q++)
  {
    while (k > -1 && (str[k+1] != str[q]))
    {
      k = PI[k];
    }
    if (str[k+1] == str[q]) {
      k++;
    }
    PI[q] = k;
  }

  return PI;
}

void
str_utils_remove_newline (gchar *str) {
  gchar *pr = str;
  gchar *pw = str;
  
  // while *pw exists continue with
  // removing newline characters (0xA)
  // and shifting whole string
  while (*pw)
  {
    *pw  =  *pr++;
     pw += (*pw != 0x0A);
  }

  *pw = 0x00;
}

gchar *
str_utils_read_str (FILE *in) {
  gchar *str     = NULL;
  gchar *str_old = NULL;
  gchar  buf [DEFAULT_BUFF_SIZE];

  fflush (in);

  while (fgets (buf, DEFAULT_BUFF_SIZE, in))
  {
    str_old = str;
    if (str)
      str = g_strdup_printf ("%s%s", str_old, buf);
    else
      str = g_strdup_printf ("%s", buf);
    g_free (str_old);
  }

  return str;
}