#ifndef HUFF_H_
#define HUFF_H_
#include <glib.h>
#include <stdlib.h>

typedef struct huff_tree_entry {
  guchar *uniq_byte;
  gsize   quantity;
} huff_tree_entry_t;

typedef struct huff_tree {
  struct huff_tree_entry *entry;
  struct huff_tree *le;
  struct huff_tree *ri;
  struct huff_tree *pa;
} huff_tree_t;

typedef struct huff_list {
  struct huff_list *next;
  struct huff_list *prev;
  struct huff_tree *tree;
} huff_list_t;

typedef struct huff {
  huff_list_t *list;
  gpointer *data;
  gsize size;
} huff_t;

huff_t
*huff_init              (gpointer *data, gsize size);
void
huff_close              (huff_t *file);

huff_t
*huff_tree_append       (huff_t *file, guchar ch);
huff_tree_t
*huff_tree_check        (huff_t *file, guchar ch);

huff_list_t
*huff_list_extract_min  (huff_t *file);
huff_list_t
*huff_list_append       (huff_t *file, huff_list_t *list);

huff_t
*huff_create_code       (huff_t *file);

gint
huff_tree_compare_func  (gpointer a, gpointer b, gpointer user_data);
#endif