#ifndef _RBT_H
#define _RBT_H
#include <glib.h>

typedef struct rbt_node {
  gpointer *value;
  enum {red, black} colour;
  struct rbt_node *le;
  struct rbt_node *ri;
  struct rbt_node *pa;
} rbt_node_t;

rbt_node_t *rbt_init         (GCompareFunc *func, key_compare_func);
rbt_node_t *rbt_insert       (rbt_node_t *tree, gint value);
rbt_node_t *rbt_insert_node  (rbt_node_t *tree, rbt_node_t *z);
rbt_node_t *rbt_minimum      (rbt_node_t *tree);
rbt_node_t *rbt_search       (rbt_node_t *tree, gint value);
rbt_node_t *rbt_transplant   (rbt_node_t *tree, rbt_node_t *u, rbt_node_t *v);
rbt_node_t *rbt_delete       (rbt_node_t *tree, rbt_node_t *x);
rbt_node_t *rbt_delete_fixup (rbt_node_t *x);

void rbt_close      (rbt_node_t *tree);
void rbt_print      (rbt_node_t *tree, FILE *output);
void rbt_print_dot  (rbt_node_t *tree, FILE *output);
void rbt_print_conn (rbt_node_t *tree, FILE *output);
void rbt_print_node (rbt_node_t *node);
void rbt_display    (rbt_node_t *tree, gint space);
void rbt_rotate_le  (rbt_node_t *tree, rbt_node_t *x);
void rbt_rotate_ri  (rbt_node_t *tree, rbt_node_t *x);

#endif