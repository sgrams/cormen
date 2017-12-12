/* AL5.2, Stanisław Juliusz Grams (251000 UG MFI), 20171211
 * 
 * 1. USAGE
 * 
 *    Compile with command `gcc -o list list.c `pkg-config --cflags glib-2.0 --libs glib-2.0`
 *    Syntax: list
 * 
 *    Tested under kernel 4.13.12-1-ARCH and gcc 7.2.1
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <unistd.h>
#include <time.h>

#define defaultStringBuffer      1024
#define defaultSyntax         "./list -i in.txt\n"
#define defaultErrorSyntax    "%s: \033[31mfatal error:\033[0m "

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

void list_init              (list *L);
void list_print             (list *L);
void list_clear             (list *L);

void list_insert_beg        (list *L, gchar *data);
void list_insert_end        (list *L, gchar *data);
void list_insert_node       (node *n, gchar *data);

void list_remove            (list *L);
void list_remove_node       (node *n);
void list_remove_element    (list *L, gchar *data);

node *list_find             (list *L, gchar *data);
node *list_get_beg          (list *L);
node *list_get_end          (list *L);

list *list_clone            (list *L);
list *list_merge            (list *list_1, list *list_2);
list *list_mergesort        (list *L);
list *list_mergesort_merge  (list *list_1, list *list_2);
list *list_concat           (list *list_1, list *list_2);
list *list_duplicate_unique (list *L);

gint main (int argc, char *argv[])
{
  FILE *inputFile   = NULL;
  
  gchar *inputFileName  = NULL;
  gchar c = 'a';
  gchar ch;
  
  gchar stringBuffer[defaultStringBuffer];
  
  list *list_1 = NULL;
  list *list_2 = NULL;
  list *list_3 = NULL;
  
  while ((ch = getopt(argc, argv, "i:")) != -1)
  {
    switch (ch) {
      case 'i':
      // Allocates memory and sets input filename (or path)
      // Checks if not file and stream at once
        if (!isatty(fileno(stdin))) {
          fprintf(stderr, defaultErrorSyntax "input cannot be a file and a stream at once\n"
              "sorting terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        // Checks if input file is specified
        if (optarg == NULL) {
          fprintf(stderr, defaultErrorSyntax "input file not specified\n sorting terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        inputFileName = g_strdup(optarg);
        inputFile = fopen((const gchar *) inputFileName, "r");
        if (inputFile == NULL) {
          fprintf(stderr, defaultErrorSyntax "input file not specified\n sorting terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        break;
      case '?':
        return EXIT_FAILURE;
      default:
        break;
    }
  }
  // There has to be a pipe/file on stdin, or inputFile has to be set, to run the program
  if (!isatty(fileno(stdin))) {
    inputFile = stdin;
  }
  else {
    if (inputFile == NULL) {
      fprintf(stderr, defaultSyntax);
      return EXIT_FAILURE;
    }
  }
  if (inputFile == NULL) {
    fprintf(stderr, defaultErrorSyntax "unable to open file %s\n"
            "sorting terminated\n", argv[0], inputFileName);
    g_free(inputFileName);
    return EXIT_FAILURE;
  }
  
  
  /*
      Initialising list using
      list_init(list *L)
  */
  list_1 = g_malloc(sizeof(list));
  list_init(list_1);
  
  /*
      Reading input to a list
  */
  while (c != EOF)
  {
    c = fscanf(inputFile, "%[^\n]\n", stringBuffer);
    if (c == EOF)
      break;
    //nameBuffer = stringBuffer;
    //strsep(&nameBuffer, " ");
    list_insert_beg(list_1, stringBuffer);
  }
  
  /*
     Printing list
  */
  printf("--- list_1 ---\n");
  list_print(list_1);
  printf("--- list_2 ---\n");
  list_2 = g_malloc(sizeof(list));
  list_init(list_2);
  list_insert_end(list_2, "d");
  list_insert_end(list_2, "e");
  list_insert_end(list_2, "f");  
  list_print(list_2);
  //
  printf("---concat\n");
  list_3 = list_concat(list_1, NULL);
  list_print(list_3);
  
  /*
     Removing list using
     list_remove(list *L)
  */
  list_remove(list_3);
  g_free(list_3);
  /*
   * Freeing memory
   * closing input file
   * and returning SUCCESS to system
   */
  fclose(inputFile);
  g_free(inputFileName);
  
  return EXIT_SUCCESS;
}

void list_init (list *L)
{
  L->sentinel = g_malloc(sizeof(sentinel));
  L->sentinel->next = L->sentinel;
  L->sentinel->prev = L->sentinel;
}

void list_print (list *L)
{
  node *node_1;
  if (!L)
    return;
  
  node_1 = list_get_beg(L);
  while (node_1 != list_get_end(L))
  {
    printf("%s\n", node_1->data);
    node_1 = node_1->next;
  }
}

void list_clear (list *L)
{
  node *node_1 = list_get_beg(L);
  node *node_2;

  while (node_1 != list_get_end(L))
  {
    node_2 = node_1->next;
    g_free(node_1->data);
    g_free(node_1);
    node_1 = node_2;
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
  // node has to be freed with g_free later on...
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
  if (L->sentinel || L)
  {
    list_clear(L);
    g_free(L->sentinel);
    L->sentinel=NULL;
  }
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

node *list_find (list *L, gchar *data)
{
  node *n = list_get_beg(L);
  while (n != list_get_end(L) && g_strcmp0(n->data, data))
    n = n->next;
  return n;
}

node *list_get_beg (list *L)
{
  return L->sentinel->next;
}

node *list_get_end (list *L)
{
  return L->sentinel;
}

list *list_clone (list *list_1)
{
  list *list_2 = NULL;
  node *in_node_1 = NULL;
  node *in_node_2 = NULL;
  
  if (list_get_beg(list_1) == list_get_end(list_1))
    return NULL;
  
  in_node_1 = list_get_beg(list_1);
  in_node_2 = list_get_end(list_1);
  
  list_2 = g_malloc(sizeof(list));
  list_init(list_2);
  
  while (in_node_1 != in_node_2)
  {
    list_insert_end(list_2, in_node_1->data);
    in_node_1=in_node_1->next;
  }

  return list_2;
}

list *list_merge (list *list_1, list *list_2)
{
  list *list_out;
  
  
}

list *list_concat (list *list_1, list *list_2)
{
  node *node_1, *node_2, *node_3, *node_4;
  list *list_out;
  
  if (!list_1 || !list_1->sentinel)
    return list_2;
  if (!list_2 || !list_2->sentinel)
    return list_1;
  
  node_1 = list_1->sentinel->next;
  node_2 = list_1->sentinel->prev;
  node_3 = list_2->sentinel->next;
  node_4 = list_2->sentinel->prev;
  list_out = g_malloc(sizeof(list));
  list_init(list_out);
  
  node_1->prev = list_out->sentinel;
  node_4->next = list_out->sentinel;
  node_2->next = node_3;
  node_3->prev = node_2;
  
  list_out->sentinel->next = node_1;
  list_out->sentinel->prev = node_4;
  
  /* 
   * Thanks to that, list_1 and list_2
   * become empty and freed.
   */
  g_free(list_1->sentinel);
  g_free(list_2->sentinel);
  g_free(list_1);
  g_free(list_2);
  
  return list_out;
}

list *list_duplicate_unique (list *L)
{
  list *list_2 = list_clone(L);
  
  node *node_1 = NULL;
  node *node_2 = NULL;
  node *node_3 = NULL;
  
  gchar *val   = NULL;
  
  node_2 = list_2->sentinel->next;
  
  /*
     In case we have empty list or just one element
  */
  if (list_2->sentinel->next == list_2->sentinel)
    return list_2;
  
  while (node_2 != list_2->sentinel)
  {
    node_1 = node_2;
    node_3 = node_1->next;
    
    while (node_3 != list_2->sentinel)
    {
      val = node_2->data;
      if (!g_strcmp0(node_3->data, val))
        node_1->next = node_3->next;
      else
        node_1 = node_3;
      node_3 = node_3->next;
    }
    node_2 = node_2->next;
  }
  return list_2;
}
