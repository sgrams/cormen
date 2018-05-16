/*
 *  This file is a part of 19-kruskal task.
 * 
 *  kruskal.h
 *  Stanisław J. Grams
 */

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

class Node {
  private:
    int  val;
    int  rank;
    Node *pa;
  
  public:
    Node ();
    Node (gint32 val);

    gint32 getValue ();
    void   setValue (gint32 val);

    gint32 getRank ();
    void   setRank (gint32 rank);
    void   incRank ();

    Node  *getParent ();
    void   setParent (Node *pa);
};

class DisjointSet {
  private:
    vector<Node*> nodeSet;
  public:
    DisjointSet(gint32 V);
    // methods
    Node   *FindSet (Node *x);
    Node   *MakeSet (gint32 x);
    void    Union   (Node *a, Node *b);
    void    Link    (Node *a, Node *b);
    Node   *getNode (gint32 i);
    vector<Node*> getNodeSet ();
   ~DisjointSet();
};

class Graph {
  private:
    gint32 V;
    gint32 E;
    vector < pair<gint32, pair<gint32, gint32>> > edges;

  public:
    // constructor
    Graph (gint32 V, gint32 E);

    // methods
    void   AddEdge (gint32 u, gint32 v, gint32 w);
    
    gint32 getVerticesNumber();
    gint32 getEdgesNumber();

    vector <pair<gint32, pair<gint32, gint32>>>
    getEdges();

    gint32 FindKruskalMST ();
    void   Set (gint32 V, gint32 E);
};

#endif
