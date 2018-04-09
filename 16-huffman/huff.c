#include "huff.h"
#include <stdio.h>

huff_t
*huff_init  (gpointer *data, gsize size) {
  huff_t *file = g_malloc0(sizeof(huff_t));
  file->data = data;
  file->size = size;
  return file;
}

huff_t
*huff_tree_append  (huff_t *file, guchar ch) {
  // check if byte exists in the directory (list)
  huff_tree_t *tree_iter = huff_tree_check(file, ch);
  if (tree_iter) {
    tree_iter->entry->quantity++;
    return file;
  }

  // declare new tree entry with given byte (guchar) inside huff file
  huff_tree_entry_t *tree_entry = g_malloc0(1*sizeof(huff_tree_entry_t));
  tree_entry->uniq_byte = g_strdup_printf("%c", ch);
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
huff_tree_t *huff_tree_check (huff_t *file, guchar ch) {
  huff_list_t *list = file->list;
  huff_tree_t *node = NULL;
  for (;list;list=list->next)
  {
    if (*(list->tree->entry->uniq_byte) == ch)
      node = list->tree;
  }
  return node;
}

huff_t
*huff_create_code (huff_t *file) {
  huff_list_t *res_list;
  huff_list_t *itr_list;
  huff_list_t *node = NULL;
  huff_list_t *min  = NULL;
  huff_tree_t *tree = NULL;

  huff_tree_entry_t *entry = NULL;

  gint i;
  gint n = file->size;
  
  // iterating through whole list and building huff_tree
  // this procedure should leave just one element on the
  // list inside huff_t *file
  for (i=0; i<n && n>2; i++)
  {
    node  = g_malloc0 (1*sizeof(huff_list_t));
    tree  = g_malloc0 (1*sizeof(huff_tree_t));
    entry = g_malloc0 (1*sizeof(huff_tree_entry_t));

    // find two minimums in the list and join them together under new tree
    min = huff_list_extract_min(file);
    if (min) {
      tree->le = min->tree;
      min->tree->pa = tree->le;
    }
    min = huff_list_extract_min(file);
    if (min) {
      tree->ri = min->tree;
      min->tree->pa = tree->ri;
    } else {
      break;
    }

    tree->pa = NULL;

    // quantity of new "void" node is a sum of its' children
    tree->entry = entry;
    tree->entry->uniq_byte = NULL;
    tree->entry->quantity  = (tree->le->entry->quantity) + (tree->ri->entry->quantity);

    // append new element of list to main list
    node->tree = tree;
    file->list = huff_list_append (file, node);

    for (huff_list_t *iter = file->list; iter; iter=iter->next)
    {
     printf("Iteracja %i: Suma w korzeniu = %i\n", i, iter->tree->entry->quantity);
    }
    printf("\n");
  }

  // returning huff file with header created in the first element of list
  return file;
}

huff_list_t
*huff_list_extract_min (huff_t *file) {
  huff_list_t *list = file->list;
  huff_list_t *iter = list;
  huff_list_t *min  = iter;
  huff_list_t *tmp  = NULL;

  // iterate through whole list to find minimal element
  while (iter)
  {
    if (min->tree->entry->quantity > iter->tree->entry->quantity) {
      min = iter;
    }
    iter = iter->next;
  }

  // un-link minimal element
  // in case it was first element list must point to next element
  tmp = min;
  
  if (!list || !min)
    return NULL;
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

  // return minimal element without links to other elements
  // REMEMBER about deallocating!
  return min;
}

void huff_close (huff_t *file) {
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

huff_list_t
*huff_list_append (huff_t *file, huff_list_t *node) {
  // appends new element to existing list
  huff_list_t *main = file->list;
  file->list = node;

  node->next = main;
  node->prev = NULL;
  if (main)
    main->prev = node;
  
  return node;
}