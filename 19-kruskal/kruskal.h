#ifndef KRUSKAL_H_
#define KRUSKAL_H_

#include <iostream>
#include <utility>
#include <cstring>
#include <vector>
#include <algorithm>
#include <iterator>
#include <glib.h>

using namespace std;

class DisjointSet {
  private:
    gint32 *pa;
    gint32 *rank;
    gint32  x;

  public:
    // constructor
    DisjointSet (gint32 x);

    // methods
    gint32 FindSet (gint32 u);
    void   Union   (gint32 a, gint32 b);
    void   Link    (gint32 a, gint32 b);
    
    // destructor
    ~DisjointSet ();
};

class Graph {
  private:
    gint32 V;
    gint32 E;
    std::vector < pair<gint32, pair<gint32, gint32>> > edges;

  public:
    // constructor
    Graph (gint32 V, gint32 E);

    // methods
    gint32 FindKruskalMST ();
    void   AddEdge (gint32 u, gint32 v, gint32 w);
    void   Set (gint32 V, gint32 E);
};

#endif