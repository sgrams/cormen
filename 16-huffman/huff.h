/*
 *  This file is a part of 16-huffman task.
 * 
 *  huff.h
 *  Stanisław J. Grams
 */
#ifndef HUFF_H_
#define HUFF_H_

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEFAULT_MAGIC_NUMBER            0x46465548
#define DEFAULT_ENCRYPTED_MAGIC_NUMBER   0x46464548 //this must be different

typedef struct huff_tree_entry {
  guint8 *uniq_byte;
  guint8 *code;
  gsize   quantity;
} huff_tree_entry_t;

typedef struct huff_tree {
  struct huff_tree_entry *entry;
  struct huff_tree *le;
  struct huff_tree *ri;
  struct huff_tree *pa;
} huff_tree_t;

typedef struct huff_list {
  struct huff_list *next;
  struct huff_list *prev;
  struct huff_tree *tree;
} huff_list_t;

typedef struct huff_encode {
  huff_list_t *list;
  gsize uniq_size;
  gsize size;
} huff_encode_t;

typedef struct huff_decode {
  GHashTable *htab;
  gsize uniq_size;
  gsize size;
} huff_decode_t;

typedef struct huff_decode_fifo {
  guint8 *buf;
  gint32 head;
  gint32 tail;
  gint32 size;
} huff_decode_fifo_t;

// initializes new huff_encode structure
// default call: file = huff_encode_init(buff, 0, 0);
huff_encode_t *
huff_encode_init          (gsize size, gsize uniq_size);

// initializes new huff_decode structure
// default call: file = huff_encode_init(buff, 0, 0);
huff_decode_t *
huff_decode_init          (gsize size, gsize uniq_size);

// inserts new byte into huff_encode structure
huff_encode_t *
huff_append               (huff_encode_t *file, guint8 byte);

// creates huffmann tree (as in the huffman algorithm)
huff_encode_t *
huff_create_tree          (huff_encode_t *file);

// puts in every node of the huffman tree coresponding code
huff_encode_t *
huff_create_dict          (huff_encode_t *file);

// checks for uniq byte in the list (before creating huffman tree)
huff_tree_t *
huff_list_find_byte       (huff_encode_t *file, guint8 byte);

// checks for uniq byte in the tree (after creating huffman tree)
huff_tree_t *
huff_tree_find_byte       (huff_tree_t *node, guint8 byte);

// extracts minimal element from huff list
huff_list_t *
huff_list_extract_min     (huff_encode_t *file);

// appends new element to the huff list
huff_list_t *
huff_list_append          (huff_encode_t *file, huff_list_t *list);

// traverses tree for huff_create_dict
void
huff_tree_traverse        (huff_tree_t *node);

// traverses and prints huffman tree and returns the total length of code
gint32
huff_tree_traverse_print  (huff_tree_t *node);

// saves header in given FILE *output
gint32
huff_save_head            (FILE *output, huff_encode_t *dict);

// reads header from give FILE *input into given huff_encode_t *dict
gint32
huff_read_head            (FILE *input, huff_decode_t *dict);

// saves dictionary in given FILE *output
gint32
huff_save_dict            (FILE *output, huff_tree_t *node);

// reads dictionary out of a given FILE *
gint32
huff_read_dict            (FILE *input, GHashTable *htab, guint32 size);

// saves code in given FILE *output
gint32
huff_save_code            (FILE *output, huff_encode_t *dict, guint8 *stream);

// converts from huffman into byte to FILE *output
gint32
huff_read_code            (FILE *output, huff_decode_t *dict, FILE *input);

// calculates crc32 of header, dict, message and padding
guint32
huff_calc_crc32           (guint8 *stream, gint32 n);

// saves crc32 sum to given FILE *output
guint32
huff_save_crc32           (FILE *output);

// reflects bits of 32-bit word
guint32
huff_calc_crc32_reverse   (guint32 crc);

// prints dictionary and info about length
void
huff_print_dict           (huff_encode_t *huff);

// closes huff_encode file and frees memory
void
huff_encode_close         (huff_encode_t *file);

// closes huff_decode file and frees memory
void
huff_decode_close         (huff_decode_t *file);

// deallocates nodes in huff tree
void
huff_close_tree           (huff_tree_t *node);

// converts binary to guint8 * (string)
guint8 *
huff_calc_binary          (guint16 x, gint8 bit);
#endif