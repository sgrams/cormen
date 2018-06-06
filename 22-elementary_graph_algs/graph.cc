/*
 *  This file is a part of 22-elementary_graph_algs task.
 * 
 *  graph.cc
 *  Stanisław J. Grams
 */
#include "graph.h"

Graph::Graph (gint32 V) {
  adjacencies.resize (V);
  for (gint32 i=0; i < V; i++)
  {
    vertices.push_back (vertex_t (i));
    adjacencies[i].head = nullptr;
  }
}

Graph::~Graph () {
  for (const auto& e: adjacencies)
  {
    for (adj_node_t *iter = e.head; iter;)
    {
      adj_node_t *temp = iter;
      iter = iter->next;
      delete temp;
    }
  }

  adjacencies.clear ();
  vertices.clear ();
}

void
Graph::add_edge (gint32 index, gint32 value) {
  adj_node_t *node = new adj_node_t (value);
  
  if (adjacencies[index].head == nullptr) {
    adjacencies[index].head = node;
  }
  else {
    adj_node_t *temp;
    for (temp = adjacencies[index].head; temp->next; temp=temp->next);
    temp->next = node;
  }
}

void
Graph::read_from_adj_matrix_file (ifstream& input_file) {
  gint32 V = (gint32) vertices.size();
  gint32 temp;

  for (gint32 i = 0; i < V; i++)
  {
    for (gint32 j = 0; j < V; j++)
    {
      temp = 0;
      input_file >> temp;
      if (temp)
        add_edge (i, j);
    }
  }
}

void
Graph::write_DFS_to_adj_matrix_file (ofstream& output_file) {
  gint32 V = vertices.size ();
  output_file << V << "\n";
  bool set = false;
  
  for (gint32 i=0; i < V; i++)
  {
    for (gint32 j=0; j < V; j++)
    {
      set = false;
      if (spanning_edges.find(i) != spanning_edges.end()) {
        for (auto iter = spanning_edges.find(i)->second.begin(); iter != spanning_edges.find(i)->second.end(); iter++)
        {
          if (*iter == j)
            set = true;
        }
      }
      if (set) {
        output_file << "1 ";
      }
      else {
        output_file << "0 ";
      }
    }
    output_file << "\n";
  }

}

void
Graph::DFS () {
  cout << "Graph vertices printed in DFS order:" << endl;
  for (const auto& u: vertices)
  {
    if (u.color == white) {
      DFS_visit (u.index);
      cout << endl;
    }
  }

  cout << "Spanning edges created by passing the graph with DFS algorithm:" << endl;
  for (const auto& u: vertices)
  {
    if (u.PI) {
      cout << u.PI->index << " -> " << u.index << endl;
      spanning_edges[u.PI->index].push_back (u.index);
    }
  }
}

void
Graph::DFS_visit (gint32 u) {
  vertices[u].color = gray;
  cout << vertices[u].index;

  for (adj_node_t *iter = adjacencies[u].head; iter; iter=iter->next)
  {
    if (vertices[iter->value].color == white) {
      cout <<  ", ";
      vertices[iter->value].PI = &vertices[u];
      DFS_visit (iter->value);
    }
  }
  vertices[u].color = black;
}

void
Graph::print () {
  cout << "Graph printed in form of adjacency list:" << endl;
  for (const auto& v: vertices)
  {
    cout << v.index << ": ";
    const auto &a = adjacencies.at (v.index);
    for (adj_node_t *iter = a.head; iter; iter=iter->next)
    {
      cout << iter->value << " ";
    }
    cout << endl;
  }
}