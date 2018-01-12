/* AL5.2, Stanisław Juliusz Grams (251000 UG MFI), 20171211
 * 
 * 1. USAGE
 * 
 *  Compile with command `gcc -o list list.c
 * `pkg-config --cflags glib-2.0 --libs glib-2.0`
 * 
 *  Syntax: list
 * 
 *  Tested under kernel 4.13.12-1-ARCH and gcc 7.2.1
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#define defaultStringBuffer  1024
#define defaultSyntax        "./list -i in.txt -o out.txt [-d (del duplicates)"\
" -v (verbose) -m merge.txt (merge) -r abc (removes)]\n"
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

typedef struct list_t {
  struct no#060606de_t *sentinel;
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
list *list_concat           (list *list_1, list *list_2);
list *list_duplicate_unique (list *L);

gint main (gint argc, gchar *argv[])
{
  FILE *input_file = NULL;
  FILE *merge_file = NULL;
  FILE *output_file = NULL;
  
  gchar *input_filepath     = NULL;
  gchar *merge_filepath     = NULL;
  gchar *output_filepath    = NULL;
  gchar *element_to_remove  = NULL;
  gchar c = '0';
  gchar ch;
  gchar stringBuffer[defaultStringBuffer];
  
  list *list_1 = NULL;
  list *list_2 = NULL;
  list *list_3 = NULL;
  list *list_nodup = NULL;
  
  node *node_tmp = NULL;
  
  bool remove_duplicates_flag = FALSE;
  bool verbose_flag = FALSE;
  bool remove_flag = FALSE;
  
  while ((ch = getopt(argc, argv, "i:m:o:vdr:")) != -1)
  {
    switch (ch) {
      case 'i':
      /*
       * Allocates memory and sets input filename (or path)
       * Checks if not file and stream at once
       */
        if (!isatty(fileno(stdin))) {
          fprintf(stderr, defaultErrorSyntax 
          "input cannot be a file and a stream at once\n"
              "sorting terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        // Checks if input file is specified
        if (optarg == NULL) {
          fprintf(stderr, defaultErrorSyntax 
          "input file not specified\n program terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        input_filepath = g_strdup(optarg);
        input_file = fopen((const gchar *) input_filepath, "r");
        if (input_file == NULL) {
          fprintf(stderr, defaultErrorSyntax 
          "input file not specified\n program terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        break;
      case 'm':
      /*
       * Allocates memory and sets second file input filename (or path)
       */
        if(optarg == NULL) {
          fprintf(stderr, defaultErrorSyntax 
          "input file not specified\n program terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        merge_filepath = g_strdup(optarg);
        merge_file = fopen((const gchar *) merge_filepath, "r");
        if (merge_file == NULL) {
          fprintf(stderr, defaultErrorSyntax 
          "merge file not specified\n program terminated\n", argv[0]);
          if (input_file) {
            fclose(input_file);
            g_free(input_filepath);
          }
          if (output_file) {
            fclose(output_file);
            g_free(output_filepath);
          }
          g_free(merge_filepath);
          return EXIT_FAILURE;
        }
        break;
      case 'o':
        if (optarg == NULL) {
          fprintf(stderr, "%s: \033[31mfatal error:\033[0m "
          "output file not specified\n sorting terminated\n", argv[0]);
          fclose(input_file);
          g_free(input_filepath);
          return EXIT_FAILURE;
        }
        output_filepath = g_strdup(optarg);
        output_file = fopen((const gchar *) output_filepath, "w");
        if (output_file == NULL) {
          fprintf(stderr, defaultErrorSyntax 
          "output file not specified\n program terminated\n", argv[0]);
        if (input_file) {
          g_free(input_filepath);
          fclose(input_file);
        }
        if (merge_file) {
          g_free(merge_filepath);
          fclose(merge_file);
        }
          return EXIT_FAILURE;
        }
        break;
      case 'v':
        verbose_flag = TRUE;
        break;
      case 'd':
        remove_duplicates_flag = TRUE;
        break;
      case 'r':
        if(optarg == NULL) {
          fprintf(stderr, defaultErrorSyntax 
          "name not specified\n program terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        element_to_remove = g_strdup(optarg);
        remove_flag = TRUE;
        break;
      case '?':
        return EXIT_FAILURE;
      default:
        break;
    }
  }
  /*
   * There has to be a pipe/file on stdin,
   * or input_file has to be set, to run the program
   */
  if (!isatty(fileno(stdin))) {
    input_file = stdin;
  }
  else {
    if (input_file == NULL) {
      fprintf(stderr, defaultSyntax);
      return EXIT_FAILURE;
    }
  }
  if (input_file == NULL) {
    fprintf(stderr, defaultErrorSyntax "unable to open file %s\n"
            "program terminated\n", argv[0], input_filepath);
    g_free(input_filepath);
    return EXIT_FAILURE;
  }
  
  if (!isatty(fileno(stdout))) {
    output_file = stdout;
  }
  else {
    if (output_file == NULL) {
      fprintf(stderr, defaultSyntax);
      if (input_file) {
        g_free(input_filepath);
        fclose(input_file);
      }
      if (merge_file) {
        g_free(merge_filepath);
        fclose(merge_file);
      }
      return EXIT_FAILURE;
    }
  }
  if (output_file == NULL) {
    fprintf(stderr, defaultErrorSyntax "unable to open file %s\n"
            "program terminated\n", argv[0], output_filepath);
    if (input_file) {
      g_free(input_filepath);
      fclose(input_file);
    }
    if (merge_file) {
      g_free(merge_filepath);
      fclose(merge_file);
    }
    return EXIT_FAILURE;
  }
  
  /*
   * Initialising main list using
   * list_init(list *L)
   */
  list_1 = g_malloc(sizeof(list));
  list_init(list_1);
  /*
   * Reading input to list_1
   */
  while (c != EOF)
  {
    c = fscanf(input_file, "%[^\n]\n", stringBuffer);
    if (c == EOF)
      break;
    list_insert_beg(list_1, stringBuffer);
  }
  
  /*
   * Reading input to list_2 and initialising
   */
  if (merge_file) {
    list_2 = g_malloc(sizeof(list));
    list_init(list_2);
    c = '0';
    while (c != EOF)
    {
      c = fscanf(merge_file, "%[^\n]\n", stringBuffer);
      if (c == EOF)
        break;
      list_insert_beg(list_2, stringBuffer);
    }
  }
  /*
   * Executing commands
   */
  if (verbose_flag) {
    printf("--- list ---\n");
    list_print(list_1);
  }
  
  if (merge_file)
  {
    if (verbose_flag) {
      printf("--- list to concatenate---\n");
      list_print(list_2);
    }
    list_3 = list_concat(list_1, list_2);
    if (verbose_flag) {
      printf("--- list after concatenating---\n");
      list_print(list_3);
    }
  }
  
  if (remove_duplicates_flag) {
    if (merge_file) {
      list_nodup = list_duplicate_unique(list_3);
      if (verbose_flag) {
        fprintf(stderr, "--- merged list after removing duplicates ---\n");
        list_print(list_nodup);
      }
      list_remove(list_3);
      g_free(list_3);
      list_3 = list_nodup;
    }
    else {
      list_nodup = list_duplicate_unique(list_1);
      if (verbose_flag) {
        fprintf(stderr, "--- list after removing duplicates ---\n");
        list_print(list_nodup);
      }
      list_remove(list_1);
      g_free(list_1);
      list_1 = list_nodup;
    }
  }
  
  if (remove_flag)
  {
    if (merge_file)
    {
      list_remove_element(list_3, element_to_remove);
      if (verbose_flag) {
        fprintf(stderr, "--- merged list after removing element ---\n");
        list_print(list_3);
      }
    }
    else {
      list_remove_element(list_1, element_to_remove);
      if (verbose_flag) {
        fprintf(stderr, "--- list after removing element ---\n");
        list_print(list_1);
      }
    }
  }
  
  /* 
   *  Writing results to file
   */
   if (merge_file) {
     node_tmp = list_3->sentinel->next;
     while (node_tmp != list_get_end(list_3))
     {
       fprintf(output_file, "%s\n", node_tmp->data);
       node_tmp = node_tmp->next;
     }
   }
   else {
     node_tmp = list_1->sentinel->next;
     while (node_tmp != list_get_end(list_1))
     {
       fprintf(output_file, "%s\n", node_tmp->data);
       node_tmp = node_tmp->next;
     }
   }
  /*
   * Removing list using
   * list_remove(list *L)
  */
  if (merge_file) {
    list_remove(list_3);
    g_free(list_3);
  }
  else {
    list_remove(list_1);
    g_free(list_1);
  }
  
  /*
   * Freeing memory
   * closing files
   * and returning SUCCESS to system
   */
  if (element_to_remove)
    g_free(element_to_remove);
  
  fclose(output_file);
  g_free(output_filepath);
  fclose(input_file);
  g_free(input_filepath);
  
  if (merge_file) {
    fclose(merge_file);
    g_free(merge_filepath);
  }
  
  return EXIT_SUCCESS;
}

/*
 * Initializes given list
 */
void list_init (list *L)
{
  L->sentinel = g_malloc(sizeof(sentinel));
  L->sentinel->next = L->sentinel;
  L->sentinel->prev = L->sentinel;
}

/*
 * Prints given list
 */
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

/*
 * Clears given list
 */
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

/*
 * Performs insert into front on given list
 */
void list_insert_beg (list *L, gchar *data)
{
  list_insert_node(list_get_beg(L), data);
}

/*
 * Performs insert into end on given list
 */
void list_insert_end (list *L, gchar *data)
{
  list_insert_node(list_get_end(L), data);
}

/*
 * Performs a node insert (util to list_insert_(beg/end))
 */
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

/*
 * Removes given list
 */
void list_remove (list *L)
{
  if (L->sentinel || L)
  {
    list_clear(L);
    g_free(L->sentinel);
    L->sentinel=NULL;
  }
}

/*
 * Removes given node from list
 */
void list_remove_node (node *n)
{
  node *tmp;
  tmp = n;
  n->prev->next = n->next;
  n->next->prev = n->prev;
  g_free(tmp->data);
  g_free(tmp);
}

/* Removes given element (gchar *)
 * from list
 */
void list_remove_element (list *L, gchar *data)
{
  node *n = list_find(L, data);
  if (n != L->sentinel)
    list_remove_node(n);
}

/*
 * Returns given element (gchar *) in given list
 */
node *list_find (list *L, gchar *data)
{
  node *n = list_get_beg(L);
  while (n != list_get_end(L) && g_strcmp0(n->data, data))
    n = n->next;
  return n;
}

/*
 * Returns first element of given list
 */
node *list_get_beg (list *L)
{
  return L->sentinel->next;
}

/*
 * Returns last element of given list
 */
node *list_get_end (list *L)
{
  return L->sentinel;
}

/*
 * Returns a clone of given list
 */
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

/*
 * Returns merged list from two given
 * O(n*log(n))
 */
list *list_merge (list *list_1, list *list_2)
{
  list *list_out;
  list_out = g_malloc(sizeof(list));
  list_out = list_concat(list_1, list_2);
  return list_out;
}

/*
 * Performs concatenation on two given lists
 * and returns third one.
 * The two given lists become empty and freed.
 */
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
  
  // linking nodes on both ends
  node_1->prev = list_out->sentinel;
  node_4->next = list_out->sentinel;
  node_2->next = node_3;
  node_3->prev = node_2;
  
  // linking list
  list_out->sentinel->next = node_1;
  list_out->sentinel->prev = node_4;
  
  /* 
   * list_1 and list_2
   * become empty and freed.
   */
  g_free(list_1->sentinel);
  g_free(list_2->sentinel);
  g_free(list_1);
  g_free(list_2);
  
  return list_out;
}

/*
 * Returns a duplicated list from given one, without duplicated elements
 * It has to be free'd!
 */ 
list *list_duplicate_unique (list *L)
{
  list *list_2 = list_clone(L);
  
  node *node_1 = NULL;
  node *node_2 = NULL;
  node *node_3 = NULL;
  node *node_dup = NULL;
  
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
      {
        node_dup = node_3;
        node_3 = node_3->next; 
        list_remove_node(node_dup);
      }
      else
        node_3 = node_3->next;
    }
    node_2 = node_2->next;
  }
 
  return list_2;
}
