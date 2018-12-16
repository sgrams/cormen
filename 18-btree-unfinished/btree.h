/*
 *  This file is a part of 18-btree task.
 * 
 *  btree.h
 *  Stanisław J. Grams
 */

#ifndef _BTREE_H
#define _BTREE_H

#include <glib.h>
#include <cstdlib>
#include <iostream>

class BTree {
  private:
    gint    *data;
    gint     n;
    gboolean isLeaf;
    BTree  **ch;
  public:
    
}

#endif