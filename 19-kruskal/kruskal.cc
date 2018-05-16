/*
 *  This file is a part of 19-kruskal task.
 * 
 *  kruskal.cc
 *  Stanisław J. Grams
 */

#include "kruskal.h"

Node::Node () {
 // no constructor
}

Node::Node (gint32 val) {
  this->val  = val;
  this->rank = 0;
  this->pa = this;
}

gint32
Node::getValue () {
  return val;
}

void
Node::setValue (gint32 val) {
  this->val = val;
}

gint32
Node::getRank () {
  return rank;
}

void
Node::setRank (gint32 rank) {
  this->rank = rank;
}

void
Node::incRank () {
  this->rank++;
}

Node *
Node::getParent () {
  return pa;
}

void
Node::setParent (Node *pa) {
  this->pa = pa;
}

DisjointSet::DisjointSet (gint32 V) {
  for (gint32 i=1; i<V+1; i++)
  {
    nodeSet.push_back(MakeSet(i));
  }
};

DisjointSet::~DisjointSet() {
  for (gint32 i=0; i < (gint32) nodeSet.size(); i++) {
    delete nodeSet.at(i);
  }
  nodeSet.clear();
}
Node *
DisjointSet::MakeSet (gint32 x) {
  return new Node (x);
}

Node *
DisjointSet::FindSet (Node *x) {
  if (x != x->getParent ()) {
    x->setParent (FindSet (x->getParent()));
  }
  return x->getParent ();
}

void
DisjointSet::Link (Node *a, Node *b) {
  
  if (a->getRank () > b->getRank ()) {
    b->setParent (a);
  }
  else {
    a->setParent (b);
  }

  if (a->getRank () == b->getRank ()) {
    b->incRank ();
  }
}

void
DisjointSet::Union (Node *a, Node *b) {
  DisjointSet::Link (FindSet (a), FindSet (b));
}

vector<Node*>
DisjointSet::getNodeSet () {
  return nodeSet;
}

Node *
DisjointSet::getNode (gint32 i) {
  return nodeSet.at (i-1);
}

Graph::Graph (gint32 V, gint32 E) {
  this->V = V;
  this->E = E;
}

void
Graph::AddEdge (gint32 u, gint32 v, gint32 w) {
  edges.push_back ({w, {u, v}});
}

void
Graph::Set (gint32 V, gint32 E) {
  this->V = V;
  this->E = E;
}

gint32
Graph::getVerticesNumber () {
  return this->V;
}

gint32
Graph::getEdgesNumber () {
  return this->E;
}

vector <pair<gint32, pair<gint32, gint32>>>
Graph::getEdges () {
  return edges;
}

gint32
Graph::FindKruskalMST () {
  gint32 A = 0;
  sort (edges.begin (), edges.end ());
  DisjointSet ds (this->getVerticesNumber ());

  vector<pair<gint32, pair<gint32, gint32>>>::iterator iter;
  for (iter=edges.begin (); iter!=edges.end (); iter++)
  {
    gint32 u = iter->second.first;
    gint32 v = iter->second.second;
    
    Node *u_pa = ds.FindSet (ds.getNode (u));
    Node *v_pa = ds.FindSet (ds.getNode (v));
    
    if (u_pa != v_pa) {
      printf ("  [%i – %i]\n", u, v);
      A += iter->first;
      ds.Union (u_pa, v_pa);
    }
  }

  return A;
}
