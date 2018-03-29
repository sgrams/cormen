#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

gint cmp_dynamic (gint *nominals, gint num, gint sum);
gint cmp_greedy  (gint *nominals, gint num, gint sum);

gint main (void) {
  gint i;
  gint *nominals = NULL;
  gint num, sum;

  /* Fetching data */
  scanf("%i", &num);
  nominals = g_malloc0(num*sizeof(gint));

  for (i=0; i<num; i++)
    scanf("%i", &nominals[i]);
  
  scanf("%i", &sum);

  g_free(nominals);
  return 0;
}

gint cmp_dynamic (gint *n, gint s, gint r, gint *k) {

}
