import sys
import string

SEPARATOR = '\t'  # The character separating nodes in a line of input

def read_graph(source): 
  # Iteratre through each line of stdin, stripping trailing whitespace and
  # splitting on separator 
  edges = [line.rstrip(string.whitespace).split(SEPARATOR) for line in source]
  parse_graph(edges)

def parse_graph(edges):
  graph = {}
  for [V1, V2] in edges:
    graph.setdefault(V1, []).append(V2)
    # If V1 is in graph, dereference the list and append, otherwise insert V1
    # with an empty list as its value and append to that new empty list

  print graph

def remove_duplicates(graph):
  print "Hello"

def print_results(graph):
  print "Hello"

if __name__ == "__main__":
  read_graph(sys.stdin)
