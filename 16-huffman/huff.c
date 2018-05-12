/*
 *  This file is a part of 16-huffman task.
 * 
 *  huff.c
 *  Stanisław J. Grams
 */
#include "huff.h"

huff_encode_t *
huff_encode_init  (gsize size, gsize uniq_size) {
  huff_encode_t *dict = g_malloc0 (sizeof (huff_encode_t));
  dict->size = size;
  dict->uniq_size = uniq_size;
  return dict;
}

huff_decode_t *
huff_decode_init (gsize size, gsize uniq_size) {
  huff_decode_t *dict = g_malloc (sizeof (huff_decode_t));
  dict->size = size;
  dict->uniq_size = uniq_size;

  dict->htab      = NULL;
  return dict;
}

huff_encode_t *
huff_append  (huff_encode_t *file, guint8 byte) {
  // check if byte exists in the directory (list)
  huff_tree_t *tree_iter = huff_list_find_byte (file, byte);
  if (tree_iter) {
    tree_iter->entry->quantity++;
    return file;
  }
  // increment unique_size
  file->uniq_size++;

  // declare new tree entry with given byte (guint8) inside huff file
  huff_tree_entry_t *tree_entry = g_malloc0 (sizeof (huff_tree_entry_t));
  tree_entry->uniq_byte   = g_malloc0 (sizeof (guint8));
  *tree_entry->uniq_byte  = byte; 
  tree_entry->quantity    = 1;
  tree_entry->code        = NULL;

  // declare new tree with given tree entry
  huff_tree_t *tree = g_malloc0 (sizeof (huff_tree_t));
  tree->entry = tree_entry;
  tree->le    = NULL;
  tree->ri    = NULL;
  tree->pa    = NULL;

  // declare new list node with given tree
  huff_list_t *new_node = g_malloc0 (sizeof (huff_list_t));
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
huff_list_find_byte (huff_encode_t *file, guint8 byte) {
  huff_list_t *list = file->list;
  huff_tree_t *node = NULL;
  if (list)
    for (;list;)
    {
      if (*(list->tree->entry->uniq_byte) == byte)
        node = list->tree;
      if (list->next)
        list = list->next;
      else
        list = NULL;
    }
  return node;
}

huff_tree_t *
huff_tree_find_byte (huff_tree_t *node, guint8 byte) {
  huff_tree_t *ret = NULL;
  if (!node)
    return NULL;

  if (node->entry) {
    if (node->entry->uniq_byte && (node->entry->uniq_byte[0] == byte)) {
      return node;
    }
  }
  ret = huff_tree_find_byte (node->le, byte);
  if (ret)
    return ret;
  ret = huff_tree_find_byte (node->ri, byte);

  return ret;
}

huff_encode_t *
huff_create_tree (huff_encode_t *file) {
  huff_list_t *element = NULL;
  huff_list_t *min     = NULL;
  huff_tree_t *tree    = NULL;

  huff_tree_entry_t *entry = NULL;

  gint32 i;
  gint32 n = file->uniq_size;
  
  /*
   * iterating through whole list and building huff_tree
   * this procedure should leave just one element in the
   * list inside huff_encode_t *file
   */
  for (i=1; i<n && n>2; i++)
  {
    element = g_malloc0 (sizeof (huff_list_t));
    tree    = g_malloc0 (sizeof (huff_tree_t));
    entry   = g_malloc0 (sizeof (huff_tree_entry_t));

    // find two minimums in the list and join them together under new tree
    min = huff_list_extract_min (file);
    if (min) {
      tree->le = min->tree;
      min->tree->pa = tree->le;
      tree->le->pa = tree;
    }
    g_free (min);
    min = huff_list_extract_min (file);
    if (min) {
      tree->ri = min->tree;
      min->tree->pa = tree->ri;
      tree->ri->pa = tree;
    } else {
      break;
    }
    g_free (min);

    tree->pa = NULL;

    // quantity of new "void" node is a sum of its' children
    tree->entry = entry;
    tree->entry->uniq_byte = NULL;
    tree->entry->code = NULL;
    tree->entry->quantity  = (tree->le->entry->quantity) + (tree->ri->entry->quantity);

    // append new element of list to main list
    element->tree = tree;
    file->list = huff_list_append (file, element);
  }

  // returning huff file with header created in the first element of list
  return file;
}

huff_list_t *
huff_list_extract_min (huff_encode_t *file) {
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
huff_encode_close (huff_encode_t *file) {
  huff_list_t *element = NULL;
  if (file->list) {
    element = file->list;
  }
  // iterates list and closes trees for every element
  while (element)
  {
    // traverses tree and closes its nodes
    huff_close_tree (element->tree);
    element = element->next;
  }
  // closes final element of a list
  g_free (file->list);
  // closes huff_encode_t file
  g_free (file);
}

void
huff_decode_close (huff_decode_t *file) {
  if (file->htab)
    g_hash_table_destroy (file->htab);
  
  g_free (file);
}

void
huff_close_tree (huff_tree_t *node) {
  if (node->le)
    huff_close_tree (node->le);
  if (node->ri)
    huff_close_tree (node->ri);

  if (node->entry)
  {
    if (node->entry->code)
      g_free (node->entry->code);
    if (node->entry->uniq_byte)
      g_free (node->entry->uniq_byte);
    g_free (node->entry);
  }

  g_free (node);
}

huff_list_t *
huff_list_append (huff_encode_t *file, huff_list_t *node) {
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

huff_encode_t *
huff_create_dict (huff_encode_t *file) {
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
        node->entry->code = (guint8 *)g_strdup_printf ("%s0", node->pa->entry->code);
      else
        node->entry->code = (guint8 *)g_strdup ("0");
    }
    if (node == node->pa->ri) {
      if (node->pa->entry->code)
        node->entry->code = (guint8 *)g_strdup_printf ("%s1", node->pa->entry->code);
      else
        node->entry->code = (guint8 *)g_strdup ("1");
    }
  }
  huff_tree_traverse (node->le);
  huff_tree_traverse (node->ri);
}

gint32
huff_tree_traverse_print (huff_tree_t *node) {
  static gint32 huff_length = 0;
  if (!node)
    return huff_length;

  if (node->entry && node->entry->uniq_byte) {
    if (*node->entry->uniq_byte >= 0x21 && *node->entry->uniq_byte <= 0x7E)
      printf ("|      %4c      |    %8i    |        %16s        |\n",
      *node->entry->uniq_byte, (gint32) node->entry->quantity, node->entry->code);
    else if (*node->entry->uniq_byte == 0x20)
      printf ("|      space     |    %8i    |        %16s        |\n",
      (gint32) node->entry->quantity, node->entry->code);
    else
      printf ("|      0x%.2x      |    %8i    |        %16s        |\n",
      *node->entry->uniq_byte, (gint32) node->entry->quantity, node->entry->code);
    huff_length += (node->entry->quantity) * (gint32)strlen ((gchar *)node->entry->code);
  }

  huff_tree_traverse_print (node->le);
  huff_tree_traverse_print (node->ri);

  return huff_length;
}

gint32
huff_read_head (FILE *input, huff_decode_t *dict) {
  guint32 hello;

  // read magic number from input
  fread (&hello, sizeof (guint32), 1, input);
  if (hello != DEFAULT_MAGIC_NUMBER) {
    return -1;
  }
  
  // read size and uniq_size to new dict
  fread (&dict->size, sizeof (guint32), 1, input);
  fread (&dict->uniq_size, sizeof (guint32), 1, input);
  
  // if unable to read size and uniq_size return -2
  if (!(dict->size && dict->uniq_size))
    return -2;

  dict->htab = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
  huff_read_dict (input, dict->htab, dict->uniq_size);
  return 0;
}

gint32
huff_save_head (FILE *output, huff_encode_t *dict) {
  guint32 hello   = (guint32)DEFAULT_MAGIC_NUMBER;

  if (!dict || !output)
    return -1;

  // write hello bytes to file
  fwrite (&hello, sizeof (guint32), 1, output);
  
  // write size and uniq_size to file
  fwrite (&dict->size, sizeof (guint32), 1, output);
  fwrite (&dict->uniq_size, sizeof (guint32), 1, output);

  // write whole dictionary
  huff_save_dict (output, dict->list->tree);

  return 0;
}

gint32
huff_read_dict (FILE *input, GHashTable *htab, guint32 size) {
  guint8 uniq_byte;
  guint8 bit;
  guint8 str[1024];

  for (gint32 i = 0; i < size; i++) {
    // read uniq_byte into variable
    fread (&uniq_byte, sizeof (guint8), 1, input);
    for (gint32 j = 0; TRUE; j++)
    {
      fread (&bit, sizeof (guint8), 1, input);

      if (bit == 0xff) {
        str[j] = '\0';
        break;
      }
      str[j] = bit;
    }
    // this function inserts into htab a pair of key and value, respectively
    // a key == huffman code and value == coded byte.

    g_hash_table_insert (htab, g_strdup_printf("%s", str), GUINT_TO_POINTER(uniq_byte));
  }
  return 0;
}

gint32
huff_save_dict (FILE *output, huff_tree_t *node) {
  guint8 padd = 0xFF;
  if (!node)
    return -1;
  
  if (node->entry->uniq_byte && node->entry->code) {
    // writes uniq_byte (guint8) to output file
    fwrite (node->entry->uniq_byte, sizeof (guint8), 1, output);
    // writes code into file (string ended with '\0' character)
    fwrite (node->entry->code, sizeof (guint8), strlen ((const gchar *)node->entry->code), output);
    // writes separator of 1 byte
    fwrite (&padd, sizeof (guint8), 1, output);
  }

  huff_save_dict (output, node->le);
  huff_save_dict (output, node->ri);
  return 0;
}

gint32
huff_save_code (FILE *output, huff_encode_t *dict, guint8 *stream) {
  gint8   c    = 0x00;
  guint8  buff = 0x00;
  guint8 *code = NULL;
  
  // if the dict does not exist return -1
  if (!dict)
    return -1;
    
  for (gint32 i = 0; i < dict->size; i++)
  {
    code = huff_tree_find_byte (dict->list->tree, stream[i])->entry->code;
    for (gint32 j = 0; j < strlen ((const gchar *)code); j++, c++)
    {
      if (c == 8) {
        fwrite (&buff, sizeof (guint8), 1, output);
        buff = 0x00;
        c    = 0x00;
      }
      buff = (buff << 1);
      if (code[j] == '1')
          buff++;
    }
  }

  // writes padding
  if (c < 8) {
    buff = (buff << (8-c));
    fwrite (&buff, sizeof (guint8), 1, output);
  }
  else if (c == 8) {
    fwrite (&buff, sizeof (guint8), 1, output);
  }
  return 0;
}

gint32
huff_read_code (FILE *output, huff_decode_t *dict, FILE *input) {
  guint64  pos_cur  = 0;
  guint64  pos_end  = 0;
  guint64  pos_dif  = 0;
  
  guint8  *buf_read = NULL;
  gchar   *str_buff = NULL;
  guint8   byte;

  guint8  str_buf[17];
  GString *in_str   = NULL;

  gsize c = 0;
  
  pos_cur = ftell (input);
  fseek (input, -4, SEEK_END);
  pos_end = ftell (input);
  pos_dif = pos_end - pos_cur;
  rewind (input);
  fseek (input, pos_cur, SEEK_CUR);

  buf_read = g_malloc0 (pos_dif * sizeof (guint8));
  fread (buf_read, sizeof (guint8), pos_dif, input);

  in_str = g_string_new (NULL);

  // main loop
  for (gsize i = 0; i < pos_dif; i++)
  {
    in_str = g_string_append (in_str, (const gchar*) huff_calc_binary(buf_read[i], 8));
  }

  for (gsize j = 0; c < dict->size;)
  {
    j++;
    if (g_hash_table_contains (dict->htab, (const gchar *)str_buf)) {
      byte = GPOINTER_TO_UINT (g_hash_table_lookup (dict->htab, (const gchar *)str_buf));
      fwrite (&byte, sizeof (guint8), 1, output);
      in_str = g_string_erase (in_str, 0, j);
      j = 1;
      c++;
    }
  }
  g_string_free (in_str, TRUE);
  return 0;
}

void
huff_print_dict (huff_encode_t *huff) {
  gint32 huff_length;
  // prints header of info dict
  printf ("====================================================================\n");
  printf ("|      byte      |      freq      |              code              |\n");
  printf ("====================================================================\n");
  // gets huff_length, traverses the tree and prints data
  huff_length = huff_tree_traverse_print (huff->list->tree);
  printf ("====================================================================\n");
  // prints size in bits...
  printf ("  before coding:  %i bits\n", (gint32)huff->size*8);
  printf ("   after coding:  %i bits\n\n", huff_length);
  printf ("compress. ratio:  %.2lf\n", (gdouble)(huff->size*8.0)/(gdouble)(huff_length));
  printf ("  space savings:  %.2lf%%\n", ((gdouble) 1.0 - ((gdouble)(huff_length)/(gdouble)(huff->size*8.0)))*100.0);
}

guint32
huff_calc_crc32 (guint8 *stream, gint32 n) {
  guint32 crc  = 0xFFFFFFFF;
  guint32 byte;

  if (!stream) {
    return -1;
  }

  for (guint32 i=0; i < n; i++)
  {
    byte = *(stream+i);
    byte = huff_calc_crc32_reverse (byte);
    for (guint32 j = 0; j < 8; j++)
    {
      if ((gint32)(crc^byte) < 0) {
        crc = (crc << 1) ^ 0x992c1a4c;
      }
      else {
        crc = crc << 1;
      }
      byte = byte << 1;
    }
  }

  return huff_calc_crc32_reverse (~crc);
}

guint32
huff_calc_crc32_reverse (guint32 crc) {
  crc = ((crc & 0x55555555) << 1) | ((crc >> 1) & 0x55555555);
  crc = ((crc & 0x33333333) << 2) | ((crc >> 2) & 0x33333333);
  crc = ((crc & 0x0f0f0f0f) << 4) | ((crc >> 4) & 0x0f0f0f0f);
  crc =  (crc << 24) | ((crc & 0xff00) << 8) |
  ((crc >> 8) & 0xff00) | (crc >> 24);

  return crc;
}

guint32
huff_save_crc32 (FILE *output) {
  guint32 len;
  guint32 crc;
  guint8 *buff = NULL;

  if (!output) {
    return -1;
  }
  
  // rewinds file to the end
  fseek (output, 0, SEEK_END);
  len  = ftell(output);
  rewind (output);

  // reserves memory for pos elements
  buff = g_malloc0 (len * sizeof (guint8));

  // if unable to read return -1
  if (fread (buff, sizeof (guint8), len, output) == 0)
  {
    return -1;
  }

  // calculates crc32 from given FILE
  crc = huff_calc_crc32 (buff, len);

  // if unable to write return -1
  if (fwrite (&crc, sizeof (guint32), 1, output) == 0) {
    return -1;
  }

  // frees memory and jumps to the end of file
  fseek  (output, 0, SEEK_END);
  g_free (buff);
  return crc;
}

guint8 *
huff_calc_binary (guint16 x, gint8 bit) {
  guint8 buf[17];
  
  buf[bit] = '\0';

  for(gint32 i = bit-1; i >= 0; i--) {
    buf[bit-1-i] = ('0' + ((x >> i) & 1));
  }

  return (guint8 *)strdup((const gchar *)buf);
}