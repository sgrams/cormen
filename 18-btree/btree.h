/*
 *  This file is a part of 18-btree task.
 * 
 *  btree.h
 *  Stanisław J. Grams
 */

#ifndef _BTREE_H
#define _BTREE_H

#include <glib.h>
#include <stdlib.h>

#define  M 3
#define  t 3

typedef struct btree {
  struct btree *pa;
  struct btree *ch;
  struct btree *root;

  gint32  *key;
  gboolean is_leaf;
  guint32  n;
  guint32  order;
} btree_t;

btree_t *
btree_create          (btree_t *tree);

void
btree_destroy         (btree_t *tree);

btree_t *
btree_allocate_node   (void);

void
btree_deallocate_node (btree_t *node);

void
btree_split_child     (btree_t *x, gint32 i);

btree_t *
btree_search          (btree_t *x, gint32 k, gint32 *i);

#endif