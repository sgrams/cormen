/*
 *  This is part of 16-huffman task.
 * 
 *  huff.c
 *  Stanisław J. Grams
 */
#include "huff.h"

huff_t *
huff_init  (gpointer *data, gsize size, gsize uniq_size) {
  huff_t *file = g_malloc0(sizeof(huff_t));
  file->data = data;
  file->size = size;
  file->uniq_size = uniq_size;
  return file;
}

huff_t *
huff_append  (huff_t *file, guchar ch) {
  // check if byte exists in the directory (list)
  huff_tree_t *tree_iter = huff_tree_check(file, ch);
  if (tree_iter) {
    tree_iter->entry->quantity++;
    return file;
  }
  // increment unique_size
  file->uniq_size++;
  // declare new tree entry with given byte (guchar) inside huff file
  huff_tree_entry_t *tree_entry = g_malloc0(sizeof(huff_tree_entry_t));
  tree_entry->uniq_byte = g_malloc0(sizeof(guchar));
  *tree_entry->uniq_byte = ch; 
  tree_entry->code = NULL;
  tree_entry->quantity  = 1;

  // declare new tree with given tree entry
  huff_tree_t *tree = g_malloc0(1*sizeof(huff_tree_t));
  tree->entry = tree_entry;
  tree->le = NULL;
  tree->ri = NULL;
  tree->pa = NULL;

  // declare new list node with given tree
  huff_list_t *new_node = g_malloc0(1*sizeof(huff_list_t));
  new_node->tree = tree;
  huff_list_t *iter = file->list;

  // iterate to the end of list
  while (iter && iter->next)
  {
    iter = iter->next;
  }

  if (!iter) {
    iter = new_node;
    iter->prev = NULL;
    iter->next = NULL;
    if (!file->list)
    file->list = new_node;
  } else {
    // link nodes inside list
    iter->next = new_node;
    new_node->prev = iter;
    new_node->next = NULL;
  }
  
  // return huff file
  return file;
}
huff_tree_t *
huff_tree_check (huff_t *file, guchar ch) {
  huff_list_t *list = file->list;
  huff_tree_t *node = NULL;
  for (;list;list=list->next)
  {
    if (*(list->tree->entry->uniq_byte) == ch)
      node = list->tree;
  }
  return node;
}

huff_t *
huff_create_tree (huff_t *file) {
  huff_list_t *node = NULL;
  huff_list_t *min  = NULL;
  huff_tree_t *tree = NULL;

  huff_tree_entry_t *entry = NULL;

  gint i;
  gint n = file->uniq_size;
  
  /*
   * iterating through whole list and building huff_tree
   * this procedure should leave just one element in the
   * list inside huff_t *file
   */
  for (i=1; i<n && n>2; i++)
  {
    node  = g_malloc0 (sizeof(huff_list_t));
    tree  = g_malloc0 (sizeof(huff_tree_t));
    entry = g_malloc0 (sizeof(huff_tree_entry_t));

    // find two minimums in the list and join them together under new tree
    min = huff_list_extract_min(file);
    if (min) {
      tree->le = min->tree;
      min->tree->pa = tree->le;
      tree->le->pa = tree;
    }
    min = huff_list_extract_min(file);
    if (min) {
      tree->ri = min->tree;
      min->tree->pa = tree->ri;
      tree->ri->pa = tree;
    } else {
      break;
    }

    tree->pa = NULL;

    // quantity of new "void" node is a sum of its' children
    tree->entry = entry;
    tree->entry->uniq_byte = NULL;
    tree->entry->code = NULL;
    tree->entry->quantity  = (tree->le->entry->quantity) + (tree->ri->entry->quantity);

    // append new element of list to main list
    node->tree = tree;
    file->list = huff_list_append (file, node);
  }

  // returning huff file with header created in the first element of list
  return file;
}

huff_list_t *
huff_list_extract_min (huff_t *file) {
  huff_list_t *list = file->list;
  huff_list_t *iter = list;
  huff_list_t *min  = iter;

  // iterate through whole list to find minimal element
  while (iter)
  {
    if (min->tree->entry->quantity > iter->tree->entry->quantity) {
      min = iter;
    }
    iter = iter->next;
  }

  /*
   * un-link minimal element
   * in case it was first element list must point to next element
   */
  
  if (!list || !min) {
    return NULL;
  }
  if (file->list == min) {
    file->list = min->next;
  }
  if (min->next) {
    min->next->prev = min->prev;
  }
  if (min->prev) {
    min->prev->next = min->next;
  }

  // finally set next and prev links to NULL inside min element
  min->prev = NULL;
  min->next = NULL;

  /*
   * return minimal element without links to other elements
   * REMEMBER about deallocating!
   */
  return min;
}

void
huff_close (huff_t *file) {
  /*
   * THIS FUNCTION NEEDS EDIT
   * THE ELEMENTS IN THE TREE STILL NEED
   * DEALLOCATING!!!
   * `valgrind --leak-check=full -v`
   * will throw errors!
   */
  huff_list_t *iter = file->list;
  while (iter && iter->next)
  {
    g_free(iter->tree->entry);
    g_free(iter->tree);
    iter = iter->next;
  }
  g_free(file->list);
  g_free(file);
}

huff_list_t *
huff_list_append (huff_t *file, huff_list_t *node) {
  // appends new element to existing list
  huff_list_t *huff = file->list;
  file->list = node;

  node->next = huff;
  node->prev = NULL;
  if (huff) {
    huff->prev = node;
  }
  // return new node (and list at the same time)
  return node;
}

huff_t *
huff_create_code (huff_t *file) {
  huff_tree_t *tree = file->list->tree;
  huff_tree_traverse (tree);  
  return file;
}
void
huff_tree_traverse (huff_tree_t *node) {  
  if (!node)
    return;
  if (node->pa && node->pa->entry && node->entry)
  {
    if (node == node->pa->le) {
      if (node->pa->entry->code)
        node->entry->code = (guchar *) g_strdup_printf("%s0", node->pa->entry->code);
      else
        node->entry->code = (guchar *) g_strdup("0");
    }
    if (node == node->pa->ri) {
      if (node->pa->entry->code)
        node->entry->code = (guchar *) g_strdup_printf("%s1", node->pa->entry->code);
      else
        node->entry->code = (guchar *) g_strdup("1");
    }
  }
  huff_tree_traverse (node->le);
  huff_tree_traverse (node->ri);
}

gint
huff_tree_traverse_print (huff_tree_t *node) {
  static gint huff_length = 0;
  if (!node)
    return huff_length;

  if (node->entry && node->entry->uniq_byte) {
    if (*node->entry->uniq_byte >= 0x21 && *node->entry->uniq_byte <= 0x7E)
      printf("|      %4c      |    %8i    |        %16s        |\n",
      *node->entry->uniq_byte, (gint) node->entry->quantity, node->entry->code);
    else if (*node->entry->uniq_byte == 0x20)
      printf("|      space     |    %8i    |        %16s        |\n",
      (gint) node->entry->quantity, node->entry->code);
    else
      printf("|      0x%.2x      |    %8i    |        %16s        |\n",
      *node->entry->uniq_byte, (gint) node->entry->quantity, node->entry->code);
    huff_length += (node->entry->quantity) * (gint)strlen((gchar *)node->entry->code);
  }

  huff_tree_traverse_print (node->le);
  huff_tree_traverse_print (node->ri);

  return huff_length;
}

void
huff_print_dict (huff_t *huff) {
  gint huff_length;
  // prints header of info dict
  printf("====================================================================\n");
  printf("|      byte      |      freq      |              code              |\n");
  printf("====================================================================\n");
  // gets huff_length, traverses the tree and prints data
  huff_length = huff_tree_traverse_print (huff->list->tree);
  printf("====================================================================\n");
  // prints size in bits...
  printf("  before coding:  %i bits\n", (gint)huff->size*8);
  printf("   after coding:  %i bits\n\n", huff_length);
  printf("compress. ratio:  %.2lf\n", (gdouble)(huff->size*8.0)/(gdouble)(huff_length));
  printf("  space savings:  %.2lf\%\n", ((gdouble) 1.0 - ((gdouble)(huff_length)/(gdouble)(huff->size*8.0)))*100.0);
}