/* AL2.2.1, Stanisław Juliusz Grams, 20180320
 *
 * 1. USAGE
 *
 *   Compile with command `gcc -o 1-lcs 1-lcs.c
 *   `pkg-config --cflags glib-2.0 --libs glib-2.0`
 *
 *   Syntax: "./1-lcs"
 *
 *   Tested under kernel 4.15.7-1-ARCH and gcc 7.3.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>

#define defaultStrBuffer 1001

gint    lcs_length          (gint **c, gchar *x, gchar *y);
GList  *lcs_print_all       (gint **c, gchar *x, gchar *y, gint i, gint j);
GList  *g_list_remove_dupes (GList *lis);

gint main (void) {
  gint i, j, str1_len, str2_len;
  gint **c = NULL;

  gchar *str1 = NULL, *str2 = NULL;
  gchar str_buf[defaultStrBuffer];

  GList *res = NULL;

  /* Getting data from user */
  for (i=0; i<2; i++)
  {
    memset(str_buf, '\0', defaultStrBuffer*sizeof(gchar));
    scanf("%s", str_buf);
    if (!i) str1 = g_strdup(str_buf);
    else   str2 = g_strdup(str_buf);
  }

  str1_len = strlen(str1);
  str2_len = strlen(str2);
  
  /* Allocating memory for b and c tables */
  c = g_malloc0((str1_len+1)*sizeof(gint *));
  for (i=0; i<=str1_len; i++)
    c[i] = g_malloc0((str2_len+1)*sizeof(gint));

  lcs_length(c, str1, str2);
  res = lcs_print_all (c, str1, str2, str1_len, str2_len);


  res = g_list_remove_dupes(res);
  GList *res_iter = res;

  while (res_iter != NULL ) {
    printf("%s\n", res_iter->data);
    res_iter = res_iter->next;
  }

  /* Freeing memory */
  g_free(str1);
  g_free(str2);

  g_list_free_full(res, g_free);

  for (i=0; i<=str1_len; i++)
    g_free(c[i]);
  g_free(c);

  return 0;
}

gint lcs_length (gint **c, gchar *x, gchar *y) {
  gint i, j;
  gint m = strlen(x);
  gint n = strlen(y);

  for (i=0; i<=m; i++)
    c[i][0] = 0;

  for (i=0; i<=n; i++)
    c[0][i] = 0;

  for (i=1; i<=m; i++)
  {
    for (j=1; j<=n; j++)
    {
      if (x[i-1] == y[j-1])
        c[i][j] = c[i-1][j-1] + 1;
      else
        (c[i][j-1] >= c[i-1][j]) ? (c[i][j] = c[i][j-1]) : (c[i][j] = c[i-1][j]);
    }
  }

  return c[m][n];
}

GList *lcs_print_all (gint **c, gchar *x, gchar *y, gint i, gint j) {
  GList *res = NULL;
  GList *tmp = NULL;
  GList *res_iter = NULL;
  GList *tmp_iter = NULL;

  if (!i || !j) {
    res = g_list_append(res, "");
    return res;
  }

  if (x[i-1] == y[j-1]) {
    // recurse in the matrix
    tmp = lcs_print_all(c, x, y, i-1, j-1);

    // append current character to every string in the list
    tmp_iter = tmp;
    while (tmp_iter)
    {
      res = g_list_append(res, g_strdup_printf("%s%c", tmp_iter->data, x[i-1]));
      tmp_iter = tmp_iter->next;
    }

    return res;
  }
  if (c[i][j-1] >= c[i-1][j]) {
    res = lcs_print_all(c, x, y, i, j-1);
  }
    
  if (c[i-1][j] >= c[i][j-1]) {
    tmp = lcs_print_all(c, x, y, i-1, j);
  }
  // merge two lists and remove duplicates
  res = g_list_concat(tmp, res);
  res = g_list_remove_dupes (res);

  return res;
}

GList *g_list_remove_dupes (GList *lis) {
  GList *a, *b, *dup;

  for (a=lis; a; a = a->next)
  {
    for (b = a->next; b; b = b->next)
    {
      dup = b;
      if (!g_strcmp0(a->data, dup->data))
        lis = g_list_delete_link(lis, dup);
    }
  }
  return lis;
}
