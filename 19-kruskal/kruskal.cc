#include "kruskal.h"
DisjointSet::DisjointSet (gint32 x) {
  this->x    = x;
  this->pa   = new gint32[x+1];
  this->rank = new gint32[x+1];

  for (gint32 i=0; i <= x; i++)
  {
    rank[i] = 0;
    pa[i]   = i;
  }
}

gint32
DisjointSet::FindSet (gint32 u) {
  if (u != pa[u]) {
    pa[u] = FindSet (pa[u]);
  }
  return pa[u];
}

void
DisjointSet::Link (gint32 a, gint32 b) {
  if (rank[a] > rank[b]) {
    pa[b] = a;
  }
  else {
    pa[a] = b;
  }

  if (rank[a] == rank[b]) {
    rank[b]++;
  }
}

void
DisjointSet::Union (gint32 a, gint32 b) {
  DisjointSet::Link (FindSet (a), FindSet(b));
};

void
DisjointSet::Destroy() {
  delete[] this->pa;
  delete[] this->rank;
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
Graph::FindKruskalMST () {
  gint32 A = 0;
  sort (edges.begin(), edges.end());
  DisjointSet ds (V);

  vector<pair<gint32, pair<gint32, gint32>>>::iterator iter;
  for (iter=edges.begin(); iter!=edges.end(); iter++)
  {
    gint32 u = iter->second.first;
    gint32 v = iter->second.second;

    gint32 u_pa = ds.FindSet (u);
    gint32 v_pa = ds.FindSet (v);

    if (u_pa != v_pa) {
      printf ("  [%i – %i]\n", u, v);
      A += iter->first;
      ds.Union (u_pa, v_pa);
    }
  }

  ds.Destroy();
  return A;
};