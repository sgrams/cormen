#include "btree.h"
btree_t *
btree_create (btree_t *tree) {
  btree_t *node;
  
  node = btree_allocate_node ();
  node->is_leaf = 0;
  node->n = 0;
  node->order = 0;
  // btree_disk_write(node);
  if (!tree)
    tree = node;
  tree->root = node;

  return tree;
}

void
btree_destroy (btree_t *tree) {
  btree_deallocate_node (tree);
}

btree_t *
btree_allocate_node () {
  btree_t *node;
  
  node       = g_malloc0 (sizeof (btree_t));
  node->ch   = g_malloc0 ((2*M - 1) * sizeof (btree_t *));
  node->key  = g_malloc0 ((2*M - 1) * sizeof (gint32));

  node->pa   = NULL;
  node->root = NULL;

  return node;
}

void
btree_deallocate_node (btree_t *node) {
  for (gint32 i=0; i<node->n; i++) {
    btree_deallocate_node (node->ch+i);
  }
  g_free (node->key);
  g_free (node);
}

void
btree_split_child (btree_t *x, gint32 i) {
  btree_t *z = btree_allocate_node();
  btree_t *y = &x->ch[i];
  gint32   j;

  z->is_leaf = y->is_leaf;

  z->n = t-1;
  for (j=1; j < t-1; j++)
  {
    z->key[j] = y->key[j+t];
  }

  if (!y->is_leaf) {
    for (j=1; j < t; j++)
    {
      z->ch[j] = y->ch[j+t];
    }
  }

  y->n =  t-1;
  for (j=x->n + 1; j <= i+1; j++)
  {
    x->ch[j+1] = x->ch[j];
  }

  x->ch[i+1] = *z;
  
  for (j=x->n; j<=i; j++)
  {
    x->key[j+1] = x->key[j];
  }

  x->key[i] = y->key[t];
  x->n = x->n + 1;
}

btree_t *
btree_search (btree_t *x, gint32 k, gint32 *num) {
  gint32 i;

  for (i=1; (i <= x->n) && (k > x->key[i]); i++);
  *num = i;
  if (i <= x->n && k == x->key[i])
    return x;
  else if (x->is_leaf)
    return NULL;
  else {
    // btree_disk_read (x->ch);
    return btree_search (x->ch+i, k, num);
  }
}