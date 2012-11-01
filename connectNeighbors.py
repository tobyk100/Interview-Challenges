import sys
import string

SEPARATOR = '\t'  # The character separating nodes in a line of input

def main(source):
  edges = read_graph(source)
  graph = parse_graph(edges)
  print(graph)
  remove_duplicates(graph)
  print_results(graph)

def read_graph(source): 
  # Iteratre through each line of stdin, stripping trailing whitespace and
  # splitting on separator 
  edges = [line.rstrip(string.whitespace).split(SEPARATOR) for line in source]

  return edges

def parse_graph(edges):
  graph = {}
  for [V1, V2] in edges:
    graph.setdefault(V1, set()).add(V2)
    # If V1 is in graph, dereference the list and append, otherwise insert V1
    # with an empty list as its value and append to that new empty list

  return graph

def remove_duplicates(graph):
  # Pseudo code for edge inmap, outmap solution
  #   for each V1 in inmap.keys()
  #     if inmap[V1].size() == 1 && outmap[V1].size() == 1
  #       inNode = inmap[V1].pop()
  #       outNode = outmap[V1].pop()
  #       inmap[outNode].add(inNode)
  #       outmap[inNode].add(outNode)
  #       remove V1 from inmap and outmap

  

def print_results(graph):
  print "Hello"

if __name__ == "__main__":
  main(sys.stdin)
