/* AL5.2, Stanisław Juliusz Grams (251000 UG MFI), 20171211
 * 1. USAGE
 *    Compile with command `gcc -o list list.c `pkg-config --cflags glib-2.0 --libs glib-2.0`
 *    Syntax: list
 * 
 *    Tested under kernel 4.13.12-1-ARCH and gcc 7.2.1
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

typedef struct node_t {
  struct node_t *next;
  struct node_t *prev;
  gchar  *data;
} node;

typedef struct sentinel_t {
  struct node_t *next;
  struct node_t *prev;
} sentinel;

typedef struct {
  struct node_t *sentinel;
} list;

void list_init            (list *L);
void list_print           (list *L);
void list_clear           (list *L);
void list_insert_beg      (list *L, gchar *data);
void list_insert_end      (list *L, gchar *data);
void list_insert_node     (node *n, gchar *data);
void list_remove          (list *L);
void list_remove_node     (node *x);
void list_remove_element  (list *L, gchar *data);

node *list_get_beg        (list *L);
node *list_get_end        (list *L);
node *list_find           (list *L, gchar *data);

list *list_duplicate      (list *L);

int main (void)
{
  gchar abc[] = "abe";
  list list_1;
  list_init(&list_1);

  // do some magic stuff here
  list_insert_beg(&list_1, "ghi");
  list_insert_beg(&list_1, "def");
  list_insert_beg(&list_1, "ijk");
  list_print(&list_1);
  printf("---\n");
  list_remove_element(&list_1, "abc");
  list_print(&list_1);
  printf("---\n");
  list_insert_beg(&list_1, "dupa");
  list_print(&list_1);
  printf("---\n");
  list_remove_element(&list_1, "ghi");
  list_print(&list_1);


  list_remove(&list_1);
  return 0;
}

void list_init (list *L)
{
  L->sentinel = g_malloc(sizeof(sentinel));
  L->sentinel->next = L->sentinel;
  L->sentinel->prev = L->sentinel;
}

void list_print (list *L)
{
  node *i = list_get_beg(L);
  while (i != list_get_end(L))
  {
    printf("%s\n", i->data);
    i = i->next;
  }
}

void list_clear (list *L)
{
  node *i = list_get_beg(L);
  node *e;

  while (i != list_get_end(L))
  {
    e = i->next;
    g_free(i->data);
    g_free(i);
    i = e;
  }
}

void list_insert_beg (list *L, gchar *data)
{
  list_insert_node(list_get_beg(L), data);
}
void list_insert_end (list *L, gchar *data)
{
  list_insert_node(list_get_end(L), data);
}

void list_insert_node (node *n, gchar *data)
{
  node *new = g_malloc(sizeof(node));
  // duplicating string that has to be freed with g_free
  new->data = g_strdup((const gchar *)data);
  // linking nodes
  new->prev = n->prev;
  new->next = n;
  // linking list
  new->prev->next = new;
  new->next->prev = new;

}

void list_remove (list *L)
{
  list_clear(L);
  g_free(L->sentinel);
  L->sentinel=NULL;
}

void list_remove_node (node *n)
{
  node *tmp;
  tmp = n;
  n->prev->next = n->next;
  n->next->prev = n->prev;
  g_free(tmp->data);
  g_free(tmp);
}

void list_remove_element (list *L, gchar *data)
{
  node *n = list_find(L, data);
  if (n != L->sentinel)
    list_remove_node (n);
}

node *list_get_beg (list *L)
{
  return L->sentinel->next;
}

node *list_get_end (list *L)
{
  return L->sentinel;
}

node *list_find (list *L, gchar *data)
{
  node *n = list_get_beg(L);
  while (n != list_get_end(L) && g_strcmp0(n->data, data))
    n = n->next;
  return n;
}
