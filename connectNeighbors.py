import sys
import string

SEPARATOR = '\t'  # The character separating nodes in a line of input

def read_graph(source): 
  # Iteratre through each line of stdin, stripping trailing whitespace and
  # splitting on separator 
  edges = [line.rstrip(string.whitespace).split(SEPARATOR) for line in source]
  parse_graph(edges)

def parse_graph(edges):
  # initialize empty graph as a mapping from node to edge list
  # for each edge (V1, V2) in edges
  #   if V1 is in graph
  #     insert V2 into graph.get(V1)
  #   else
  #     create new kv listing mapping from V1 to a new list containing V2
  graph = {}
  for [V1, V2] in edges:
    if V1 in graph:
      graph[V1].append(V2)
    else:
      graph[V1] = [V2]

  print graph

def remove_duplicates(graph):
  print "Hello"

def print_results(graph):
  print "Hello"

if __name__ == "__main__":
  read_graph(sys.stdin)
