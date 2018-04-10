/*
 *  This is part of 16-huffman task.
 * 
 *  huff.h
 *  Stanisław J. Grams
 */
#ifndef HUFF_H_
#define HUFF_H_
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct huff_tree_entry {
  guchar *uniq_byte;
  guchar *code;
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
  gsize uniq_size;
  gsize size;
} huff_t;

// initializes new huff structure
// default call: file = huff_init(NULL, 0, 0);
huff_t *
huff_init                 (gpointer *data, gsize size, gsize uniq_size);
// inserts new byte into huff structure
huff_t *
huff_append               (huff_t *file, guchar ch);
// creates huffmann tree (as in the huffman algorithm)
huff_t *
huff_create_tree          (huff_t *file);
// puts in every node of the huffman tree coresponding code
huff_t *
huff_create_code          (huff_t *file);

// checks for uniq byte in the tree
huff_tree_t *
huff_tree_check           (huff_t *file, guchar ch);


// extracts minimal element from huff list
huff_list_t *
huff_list_extract_min     (huff_t *file);
// appends new element to the huff list
huff_list_t *
huff_list_append          (huff_t *file, huff_list_t *list);

// traverses tree for huff_create_code
void
huff_tree_traverse        (huff_tree_t *node);
// traverses and prints huffman tree and returns the total length of code
gint
huff_tree_traverse_print  (huff_tree_t *node);
// prints dictionary and info about length
void
huff_print_dict (huff_t *huff);
// closes huff file and frees memory
void
huff_close                (huff_t *file);

#endif