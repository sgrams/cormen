#include "rbt.h"

rbt_node_t *rbt_insert_node (rbt_node_t *tree, rbt_node_t *z, GCompareFunc compare_func) {
  rbt_node_t *y = NULL;
  rbt_node_t *x = tree;

  while (x)
  {
    y = x;
    if (compare_func(z, x))
      x = x->le;
    else
      x = x->ri;
  }
  z->pa = y;
  if (!y)
    tree = z;
  else if (compare_func(z, y))
    y->le = z;
  else
    y->ri = z;

  return tree;
}

rbt_node_t *rbt_insert (rbt_node_t *tree, gpointer *value) {
  rbt_node_t *y = NULL;
  rbt_node_t *x = tree_init(value);

  x = tree;
  tree_insert_node (tree, x);

  while ((x->pa) && (x->pa->colour == red))
  {
    if (x->pa->pa->le && x->pa->val == x->pa->pa->le->val) {
      if (x->pa->pa)
        y = x->pa->pa->ri;
      if (y && y->colour == red) {
        // case 1
        x->pa->colour = black;
        y->colour = black;
        x->pa->pa->colour = red;

        if (x->pa->pa)
          x = x->pa->pa;
      }
      else {
        if (x->pa && x == x->pa->ri) {
          // case 2
          x = x->pa;
          tree_rotate_le(tree, x);
        }
        // case 3
        x->pa->colour = black;
        x->pa->pa->colour = red;
        tree_rotate_ri(tree, x->pa->pa);
      }
    }
    else {
      if (x->pa->pa->le)
       y = x->pa->pa->le;
      if (y && y->colour == red) {
        // case 1
        x->pa->colour = black;
        y->colour = black;
        x->pa->pa->colour = red;

        if (x->pa->pa)
          x = x->pa->pa;
      }
      else {
        if (x->pa->le && x == x->pa->le) {
          // case 2
          x = x->pa;
          tree_rotate_ri(tree, x);
        }
        // case 3
        x->pa->colour = black;
        x->pa->pa->colour = red;
        tree_rotate_le(tree, x->pa->pa);
      }
    }
  }

  tree->colour = black;
  return tree;
}