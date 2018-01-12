/* AL7.5, Stanisław Juliusz Grams (251000 UG MFI), 20180107
 *
 * 1. USAGE
 *
 *   Compile with command `gcc -o 5-tree 5-tree.c
 *   `pkg-config --cflags glib-2.0 --libs glib-2.0`
 *
 *   Syntax: "./5-tree -i keys.txt [-d (display tree) -p (print tree in-order) -r (remove elements)]"
 *
 *   Tested under kernel 4.14.11-1-ARCH and gcc 7.2.1
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>

#define defaultStringBuffer   1024
#define defaultLineSize       256
#define defaultSyntax         "%s -i keys.txt [-d (display tree) -p (print tree in-order) -r (remove elements)]\n"
#define defaultErrorSyntax    "%s: \033[31mfatal error:\033[0m "
typedef struct node {
  gint val;
  gint flag;
  struct node *le;
  struct node *ri;
  struct node *pa;
} node_t;

void tree_print       (node_t *tree);
void tree_print_node  (node_t *node);
void tree_display     (node_t *tree, gint space);

node_t *tree_delete      (node_t *tree, gint key);
node_t *tree_delete_node (node_t *tree, node_t *node);
node_t *tree_init         (node_t *tree, gint value);
node_t *tree_insert       (node_t *tree, gint value);
node_t *tree_search       (node_t *tree, gint value);
node_t *tree_node_parent  (node_t *node);
node_t *tree_transplant   (node_t *tree, node_t *a, node_t *b);
node_t *tree_minimum (node_t *root);

gint main (gint argc, gchar *argv[])
{
  FILE *input_file      = NULL;

  gchar *input_filepath = NULL;
  gchar *buf_str        = NULL;
  gchar c = '0', ch;
  gchar stringBuffer[defaultStringBuffer];

  gint i;
  gint buf_num;
  gint buf_rm;

  node_t *tree = NULL;
  gboolean display_flag = FALSE;
  gboolean print_flag   = FALSE;
  gboolean remove_flag  = FALSE;

  while ((ch = getopt(argc, argv, "i:dpr")) != -1)
  {
    switch (ch) {
      case 'i':
        // Allocates memory and sets input filepath
        if (!optarg) {
          fprintf(stderr, defaultErrorSyntax
              "input filepath not specified\n program terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        input_filepath = g_strdup(optarg);
        input_file = fopen((const gchar *) input_filepath, "r");
        if (!input_file) {
          fprintf(stderr, defaultErrorSyntax
              "input filepath not specified\n program terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        break;
      case 'd':
        display_flag = TRUE;
        break;
      case 'p':
        print_flag = TRUE;
        break;
      case 'r':
        remove_flag = TRUE;
        break;
      case '?':
        return EXIT_FAILURE;
      default:
        break;
    }
  }
  if (!input_file) {
    printf(defaultSyntax, argv[0]);
    if (input_filepath)
      g_free(input_filepath);
    return EXIT_SUCCESS;
  }
  buf_str = g_malloc(defaultStringBuffer * sizeof(gchar));

  while (!feof(input_file))
  {
    fgets(buf_str, defaultLineSize, input_file);
    if (feof(input_file))
      break;
    buf_num = strtol(buf_str, NULL, 0);
    if (tree == NULL)
      tree = tree_init(tree, buf_num);
    else
      tree_insert(tree, buf_num);
  }
  /*
   * Main body of program
   */
  if (remove_flag) {
    printf("How many numbers do you wish to remove?\n");
    scanf("%i", &buf_num);
    for (i=0; i<buf_num; i++)
    {
      scanf("%i", &buf_rm);
      tree = tree_delete(tree, buf_rm);
    }
    
  }
  if (display_flag) {
    printf("\n");
    tree_display(tree, 0);
    printf("\n");
  }
  if (print_flag) {
    printf("\n");
    tree_print(tree);
    printf("\n");
  }

  /*
   * Freeing memory...
   */
  g_free(buf_str);
  g_free(input_filepath);

  fclose(input_file);
  return 0;
}

node_t *tree_init  (node_t *tree, gint value) {
  node_t *node = g_malloc(sizeof(node_t));

  node->pa = NULL;
  node->le = NULL;
  node->ri = NULL;
  node->val = value;
  node->flag = 0;

  return node;
}

node_t *tree_insert (node_t *tree, gint value) {
  if (!tree)
    return tree_init(tree, value);

  if (value < tree->val)
  {
    tree->le = tree_insert(tree->le, value);
    tree->le->pa = tree;
  }

  else if (value > tree->val)
  {
    tree->ri = tree_insert(tree->ri, value);
    tree->ri->pa = tree;
  }
  else if (value == tree->val)
  {
    if (!tree->flag)
    {
      tree->le = tree_insert(tree->le, value);
      tree->le->pa = tree;
      tree->flag = 1;
    }
    else
    {
      tree->ri = tree_insert(tree->ri, value);
      tree->ri->pa = tree;
    }
  }

  return tree;
}

void tree_print (node_t *tree) {
  if (tree) {
    tree_print (tree->le);
    printf ("%i\n", tree->val);
    tree_print (tree->ri);
  }
}

void tree_print_node (node_t *node) {
  if (node)
    printf("%i\n", node->val);
}

void tree_display (node_t *tree, gint space) {
  if (tree) {
    tree_display(tree->ri, space+4);
    if (space>0)
      for (gint i=0; i<space; i++)
        printf(" ");
    printf("%i\n", tree->val);
    tree_display(tree->le, space+4);
  }
}

node_t *tree_search (node_t *tree, gint value) {
  node_t *root = tree;
  while (root && root->val != value)
  {
    if (value < root->val)
      root = root->le;
    else
      root = root->ri;
  }
  return root;
}

node_t *tree_delete_node (node_t *tree, node_t *z) {
  node_t *y = NULL;

  if (!z->le) {
    tree_transplant(tree, z, z->ri);
  }
  else if (!z->ri) {
    tree_transplant(tree, z, z->le);
  }
  else {
    y = tree_minimum(z->ri);
    if (y->pa != z) {
      tree_transplant(tree, y, y->ri);
      y->ri = z->ri;
      y->ri->pa = y;
    }
    tree_transplant(tree, z, y);
    y->le = z->le;
    y->le->pa = y;
  }
  return y;
}

node_t *tree_delete (node_t *tree, gint key) {
  node_t *res = NULL;
  node_t *tmp = tree_search(tree, key);
  if (tmp) {
    res = tree_delete_node(tree, tmp);
    g_free(tmp);
    return res;
  }
  return tree;
}

node_t *tree_node_parent (node_t *node) {
  return node->pa;
}


node_t *tree_minimum (node_t *tree) {
  node_t *x = tree;
  while (x->le)
    x = x->le;
  return x;
}

node_t *tree_transplant (node_t *tree, node_t *u, node_t *v) {
  if (!u->pa)
    tree = v;
  else if (u == u->pa->le)
    u->pa->le = v;
  else
    u->pa->ri = v;
  
  if (v)
    v->pa = u->pa;

  return tree;
}
