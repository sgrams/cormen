#!/usr/bin/python
"""
  This file is part of 23-kruskal task.

  grapher.py
  
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

  results = []
  filepath = argv[1]
  filename = filepath.split('.')

  with open (filepath, newline='') as inputFile:
    for row in csv.reader (inputFile, delimiter=';'):
      results.append (row)

  V = int (results[0][0])
  E = int (results[0][1])
  G = pgv.AGraph (ranksep='0.5')

  G.graph_attr['label'] = filename[0]
  G.node_attr['shape'] = 'circle'
  G.edge_attr['lblstyle'] = 'above, sloped'

  for i in range (1, V+1):
    G.add_node (i)

  for i in range (1, E+1):
    G.add_edge (results[i][0], results[i][1])
    edge = G.get_edge (results[i][0], results[i][1])
    edge.attr['label'] = results[i][2]

  G.layout (prog='sfdp')
  G.draw (filename[0]+'.svg')

if __name__ == "__main__":
    sys.exit(main(sys.argv))