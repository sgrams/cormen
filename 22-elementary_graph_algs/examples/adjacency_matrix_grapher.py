#!/usr/bin/python
"""
  This file is part of 22-elementary_graph_algs task.

  adjacency_matrix_grapher.py
  
  Stanisław J.Grams
"""

import pygraphviz as pgv
import csv
import sys
import os

__author__ = """Stanisław Grams (sjg@fmdx.pl)"""

def main (argv):
  if len (sys.argv) < 2:
    print ("Usage: %s <example.txt>" % argv[0])
    return 1

  if not os.path.exists (argv[1]):
    sys.stderr.write ("Fatal error! File not found!\nProgram terminated!")
    return 1

  filepath = argv[1]
  filename = filepath.split('.')

  G = pgv.AGraph (directed=True, ranksep='0.5')
  G.graph_attr['label'] = filename[0]
  G.node_attr['shape'] = 'circle'
  G.edge_attr['lblstyle'] = 'above, sloped'

  with open (filepath, newline='\n') as inputFile:
    V = int(inputFile.read(1))
    for i in range (0, V):
      G.add_node (i)
    
    i=-1
    for line in inputFile:
      j=0
      for word in line.split():
        if int(word) == 1:
          G.add_edge (i, j)
          edge = G.get_edge (i, j)
        j = j+1
      i = i+1
  
  G.layout (prog='circo')
  G.draw (filename[0]+'.svg')


if __name__ == "__main__":
    sys.exit(main(sys.argv))