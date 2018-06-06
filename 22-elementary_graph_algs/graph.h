/*
 *  This file is a part of 22-elementary_graph_algs task.
 * 
 *  graph.h
 *  Stanisław J. Grams
 */
#ifndef  _GRAPH_H
#define  _GRAPH_H

#include <glib.h>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <stack>
#include <list>

using namespace std;

enum color_t {
  white,
  gray,
  black
};

typedef struct vertex {
  gint32 index;
  color_t color;
  vertex *PI;
  vertex (gint32 _index) : index (_index),
                           color (color_t::white),
                           PI (nullptr)
                           {}
} vertex_t;

typedef struct adj_node {
  gint32 value;
  struct adj_node *next;
  adj_node (gint32 _value) : value (_value),
                             next (nullptr)
                             {}
} adj_node_t;

typedef vector<vector<gint32> > adj_matrix_t;

typedef struct adj_list {
  adj_node_t *head;
} adj_list_t;

class Graph {
  protected:
    vector <adj_list_t> adjacencies;
    vector <vertex_t> vertices;
    unordered_map <int, vector<int> > spanning_edges;

  public:
    Graph (gint32 V);
   ~Graph ();
    
    void add_edge (gint32 index, gint32 value);
    void read_from_adj_matrix_file (ifstream& input_file);
    void write_DFS_to_adj_matrix_file (ofstream& output_file);

    void DFS ();
    void print ();
    
  private:
    void DFS_visit (gint32 vertex);
};

#endif