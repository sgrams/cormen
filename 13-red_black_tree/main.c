/* AL2.1.1+2, Stanisław Juliusz Grams (251000 UG MFI), 20180306
 *
 * 1. USAGE
 * 
 *   Install following packages:
 *     - graphviz
 *   Compile with command `gcc -o main main.c
 *   `pkg-config --cflags glib-2.0 --libs glib-2.0`
 *
 *   Syntax: "./main -i keys.txt [-d (display tree) -p (print tree in-order) -r (remove elements)]"
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
#define defaultSyntax         "%s -i keys.txt [-d (display tree) "\
"-p (print tree in-order) -r (remove elements) -o (output dot file)]\n"
#define defaultErrorSyntax    "%s: \033[31mfatal error:\033[0m "
#define defaultDotOutFilepath "tree.in"

typedef struct node {
  gint val;
  enum {red, black} colour;
  struct node *le;
  struct node *ri;
  struct node *pa;
  struct node *root;
} node_t;

void tree_close         (node_t *tree);
void tree_print         (node_t *tree, FILE *output);
void tree_print_dot     (node_t *tree, FILE *output);
void tree_print_reds    (node_t *tree, FILE *output);
void tree_print_conn    (node_t *tree, FILE *output);
void tree_print_node    (node_t *node);
void tree_display       (node_t *tree, gint space);

void tree_rotate_le   (node_t *tree, node_t *x);
void tree_rotate_ri   (node_t *tree, node_t *x);

node_t *tree_init         (gint value);
node_t *tree_insert       (node_t *tree, gint value);
node_t *tree_insert_node  (node_t *tree, node_t *z);
node_t *tree_minimum      (node_t *tree);
node_t *tree_search       (node_t *tree, gint value);
node_t *tree_transplant   (node_t *tree, node_t *u, node_t *v);
node_t *tree_delete       (node_t *tree, node_t *x);
node_t *tree_delete_fixup (node_t *x);

gint tree_level         (node_t *tree);
gint tree_node_number   (node_t *node);

gint main (gint argc, gchar *argv[])
{
  FILE *input_file      = NULL;
  FILE *output_dot_file = NULL;

  gchar *input_filepath = NULL;
  gchar *output_dot_filepath = NULL;
  gchar *buf_str        = NULL;
  gchar ch;

  gint i;
  gint buf_num;
  gint buf_rm;

  node_t *tree = NULL;
  gboolean display_flag = FALSE;
  gboolean print_flag   = FALSE;
  gboolean remove_flag  = FALSE;

  while ((ch = getopt(argc, argv, "i:dpro::")) != -1)
  {
    switch (ch) {
      case 'i':
        // Allocates memory and sets input filepath
        if (!optarg) {
          fprintf(stderr, defaultErrorSyntax
              "input filepath not specified\nprogram terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        input_filepath = g_strdup(optarg);
        input_file = fopen((const gchar *) input_filepath, "r");
        if (!input_file) {
          fprintf(stderr, defaultErrorSyntax
              "input filepath not specified\nprogram terminated\n", argv[0]);
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
      case 'o':
        output_dot_filepath = g_strdup(defaultDotOutFilepath);
        output_dot_file = fopen((const gchar *) output_dot_filepath, "w");
        if(!output_dot_file) {
          fprintf(stderr, defaultErrorSyntax
              "output dot filepath not specified\nprogram terminated\n", argv[0]);
        }
        break;
      case '?':
        return EXIT_FAILURE;
      default:
        break;
    }
  }
  
  if (!input_file || !(display_flag || print_flag || output_dot_file)) {
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

    if (tree == NULL) {
      tree = tree_init(buf_num);
      tree->colour = black;
      tree->root = tree;
    }
    else {
      tree_insert(tree, buf_num);
    }
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
      if (tree_search(tree, buf_rm))
        tree_delete(tree, tree_search(tree, buf_rm));
    }
  }
  
  if (display_flag) {
    printf("\n");
    tree_display(tree->root, 0);
    printf("\n");
  }
  
  if (print_flag) {
    printf("\n");
    tree_print(tree->root, stdout);
    printf("\n");
  }

  if (output_dot_file) {
    tree_print_dot(tree->root, output_dot_file);

  }
  
  /*
   * Freeing memory...
   */
  tree_close(tree);
  g_free(buf_str);
  g_free(input_filepath);
  g_free(output_dot_filepath);

  return 0;
}

void tree_close (node_t *tree) {
  if (!tree)
    return;
  tree_close(tree->ri);
  tree_close(tree->le);
  g_free(tree);
}

void tree_print (node_t *tree, FILE *output) {
  if (tree) {
    tree_print (tree->le, output);
    if (tree->colour == red)
      fprintf (output, "\033[1m\033[31m%i\033[0m\n", tree->val);
    else
      fprintf (output, "%i\n", tree->val);
    tree_print (tree->ri, output);
  }
}

void tree_print_dot (node_t *tree, FILE *output) {
  gchar* cmd[] = { "dot", "-Tsvg", "-otree.svg", "tree.in", NULL };
  fprintf(output,
      "digraph G {\n"
      "  graph [ratio=.36];\n"
      "  node [style=filled, color=black, shape=circle, width=.6\n"
      "    fontname=Helvetica, fontweight=bold, fontcolor=white,\n"
      "    fontsize=18, fixedsize=true];\n"
      );

  tree_print_conn(tree, output);
  fprintf(output, "}\n");
  fclose(output);
  if (!g_spawn_sync(NULL, cmd, NULL, G_SPAWN_SEARCH_PATH, 0, NULL, NULL, NULL, NULL, NULL)) {
    fprintf(stderr, "Unable to open dot program.\n");
  }
  else {
    gchar* cmd2[] = { "display", "tree.svg", NULL };
    if (!g_spawn_async(NULL, cmd2, NULL, G_SPAWN_SEARCH_PATH, 0, NULL, NULL, NULL)) {
      fprintf(stderr, "Unable to start display\n");
    }
  }
}

void tree_print_reds (node_t *tree, FILE *output) {
  static gint counter = 0;
  if (tree) {
    tree_print_reds (tree->le, output);
      if (tree->colour == red) {
        if (!counter++)
          fprintf(output, "%i", tree->val);
        else
          fprintf(output, ", %i", tree->val);
      }
    tree_print_reds (tree->ri, output);
  }
}

void tree_print_conn (node_t *tree, FILE *output) {
  static gint null_counter = 0;
  if (tree) {
    if (tree->le) {
        fprintf(output, "  %i -> %i [weight=%i];\n", tree_node_number(tree), tree_node_number(tree->le), tree_level(tree));
      if (tree->le->colour == red)
        fprintf(output, "%i [fillcolor=\"red\", color=\"red\"];\n", tree_node_number(tree->le));
      fprintf(output, "  %i [label=\"%i\"]\n", tree_node_number(tree), tree->val);

    }
    else {
      fprintf(output, "  %i -> null%i [weight=%i];\n  null%i [shape=box, label=\"nil\", fontsize=12, width=0.5, height=0.3];\n",
          tree_node_number(tree), null_counter, tree_level(tree), null_counter);
      fprintf(output, "  %i [label=\"%i\"]\n", tree_node_number(tree), tree->val);
      null_counter++;
    }
    tree_print_conn (tree->le, output);
    if (tree->ri)
    {
      fprintf(output, "  %i -> %i [weight=%i];\n", tree_node_number(tree), tree_node_number(tree->ri), tree_level(tree));
      if (tree->ri->colour == red)
        fprintf(output, "%i [fillcolor=\"red\", color=\"red\"];\n", tree_node_number(tree->ri));
      fprintf(output, "  %i [label=\"%i\"]\n", tree_node_number(tree), tree->val);
    }
    else {
      fprintf(output, "  %i -> null%i [weight=%i];\n  null%i [shape=box, label=\"nil\", fontsize=12, width=0.5, height=0.3];\n",
          tree_node_number(tree), null_counter, tree_level(tree), null_counter);
      fprintf(output, "  %i [label=\"%i\"]\n", tree_node_number(tree), tree->val);
      null_counter++;
    }
    tree_print_conn (tree->ri, output);
  }
}
void tree_print_node (node_t *node) {
  if (node)
    printf("%i\n", node->val);
}

gint tree_node_number (node_t *node) {
  if (node->pa == NULL)
    return 0;

  if (node == node->pa->le)
    return tree_node_number (node->pa) * 2 + 1;
  else
    return tree_node_number (node->pa) * 2 + 2;
}
void tree_display (node_t *tree, gint space) {
  if (tree) {
    tree_display(tree->ri, space+4);
    if (space>0)
      for (gint i=0; i<space; i++)
        printf(" ");
    if (tree->colour == red)
      printf("\033[1m\033[31m");
    printf("%i\n", tree->val);
    printf("\033[0m");
    tree_display(tree->le, space+4);
  }
}

void tree_rotate_le (node_t *tree, node_t *x) {
  node_t *p = x->pa;
  node_t *y = x->ri;
  x->ri = y->le;

  if (x->ri)
    x->ri->pa = x;

  y->le = x;
  x->pa = y;

  y->pa = p;

  if (y->pa) {
    if (y->pa->le == x) y->pa->le = y;
    else y->pa->ri = y;
  }
  else {
    tree->root = y;
  }
}
void tree_rotate_ri (node_t *tree, node_t *x) {
  node_t *p = x->pa;
  node_t *y = x->le;
  x->le = y->ri;

  if (x->le)
    x->le->pa = x;

  y->ri = x;
  x->pa = y;

  y->pa = p;

  if (y->pa) {
    if (y->pa->le == x) y->pa->le = y;
    else y->pa->ri = y;
  }
  else {
    tree->root = y;
  }
}
node_t *tree_init (gint value) {
  node_t *node = g_malloc(sizeof(node_t));

  node->pa = NULL;
  node->le = NULL;
  node->ri = NULL;
  node->val = value;
  node->colour = red;

  return node;
}

node_t *tree_insert_node (node_t *tree, node_t *z) {
  node_t *y = NULL;
  node_t *x = tree;

  while (x)
  {
    y = x;
    if (z->val < x->val)
      x = x->le;
    else
      x = x->ri;
  }
  z->pa = y;
  if (!y)
    tree = z;
  else if (z->val < y->val)
    y->le = z;
  else
    y->ri = z;

  return tree->root;
}

node_t *tree_insert (node_t *tree, gint value) {
  node_t *y = NULL;
  node_t *x = tree_init(value);

  x->root = tree;
  tree_insert_node (tree->root, x);

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

  tree->root->colour = black;
  return tree;
}
node_t *tree_transplant (node_t *tree, node_t *u, node_t *v) {
  node_t *p = u->pa;

  if (!p)
    tree->root = v;
  else if (u == p->le)
    p->le = v;
  else
    p->ri = v;

  if (v)
    v->pa = p;

  return tree;
}

node_t *tree_delete  (node_t *tree, node_t *z) {
  node_t *y = z;
  node_t *x = NULL;
  gint y_org_col = 0;

  if (y->colour == red)
    y_org_col = 1;

  if (!z->le) {
    x = z->ri;
    tree_transplant(tree, z, z->ri);
  }
  else if (!z->ri) {
    x = z->le;
    tree_transplant(tree, z, z->le);
  }
  else {
    y_org_col = y->colour;
    x = y->ri;
    if (y->pa == z) {
      x->pa = y;
    }
    else {
      tree_transplant(tree, y, y->ri);
      y->ri = z->ri;
      y->ri->pa = y;
    }
    tree_transplant(tree, z, y);
    y->le = z->le;
    y->le->pa = y;
    y->colour = z->colour;
  }

  if (!y_org_col)
    tree_delete_fixup(z);

  return tree;
}

node_t *tree_delete_fixup (node_t *x) {
  node_t *t = x->root;
  node_t *w = NULL;
  while (x != t && (x->colour == black))
  {
    if (x->pa && x == x->pa->le) {
      w = x->pa->ri;
      if (w && w->colour == red) {
        // case 1
        w->colour = black;
        x->pa->colour = red;
        tree_rotate_le(t, x->pa);
        w = x->pa->ri;
      }
      if (w && w->le->colour == black && w->ri->colour == black) {
        // case 2
        w->colour = red;
        x = x->pa;
      }
      else {
        if (w && w->ri->colour == black) {
          // case 3
          w->le->colour = black;
          w->colour = red;
          tree_rotate_ri(t, w);
          w = x->pa->ri;
        }
        // case 4
        w->colour = x->pa->colour;
        x->pa->colour = black;
        w->ri->colour = black;
        tree_rotate_le(t, x->pa);
        x = t;
      }
    }
    else {
      w = x->pa->le;
      if (w && w->colour == red) {
        // case 1
        w->colour = black;
        x->pa->colour = red;
        tree_rotate_ri(t, x->pa);
        w = x->pa->le;
      }
      if (w && w->ri->colour == black && w->le->colour == black) {
        // case 2
        w->colour = red;
        x = x->pa;
      }
      else {
        if (w && w->le->colour == black) {
          // case 3
          w->ri->colour = black;
          w->colour = red;
          tree_rotate_le(t, w);
          w = x->pa->le;
        }
        // case 4
        w->colour = x->pa->colour;
        x->pa->colour = black;
        w->le->colour = black;
        tree_rotate_ri(t, x->pa);
        x = t;
      }
    }
  }
  x->colour = black;
  return t;
}

node_t *tree_minimum (node_t *tree) {
  node_t *x = tree;
  while (x && x->le)
    x = x->le;
  return x;
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

gint tree_level (node_t *tree) {
  gint level=0;
  node_t *tmp = tree;

  for (level=0; tmp->pa!=NULL; level++)
    tmp = tmp->pa;

  return level;
}
