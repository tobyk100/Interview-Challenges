import sys
import string

SEPARATOR = '\t'  # The character separating nodes in a line of input

def main(source):
  edges = read_graph(source)
  graphs = parse_graph(edges)
  remove_duplicates(graphs)
  print_results(graphs[0])

def read_graph(source): 
  # Iteratre through each line of stdin, stripping trailing whitespace and
  # splitting on separator 
  edges = [line.rstrip(string.whitespace).split(SEPARATOR) for line in source]

  return edges

def parse_graph(edges):
  ingraph = {}
  outgraph = {}
  for [V1, V2] in edges:
    outgraph.setdefault(V1, set()).add(V2)
    ingraph.setdefault(V2, set()).add(V1)
    # If V1 is in graph, dereference the list and append, otherwise insert V1
    # with an empty list as its value and append to that new empty list

  return [outgraph, ingraph] 

def remove_duplicates(graphs):
  ingraph = graphs[0]
  outgraph = graphs[1]
  
  for V1 in ingraph.keys():
    if V1 in ingraph:
      check_remove_node(ingraph, outgraph, V1)

def check_remove_node(ingraph, outgraph, node):
    if len(ingraph[node]) == len(outgraph[node]) == 1:
      inNode = ingraph.pop(node).pop()
      outNode = outgraph.pop(node).pop()
      ingraph[outNode].remove(node)
      outgraph[inNode].remove(node)
      if inNode != outNode:
        ingraph[outNode].add(inNode)
        outgraph[inNode].add(outNode)
      else:
        if len(ingraph[inNode]) == len(outgraph[inNode]) == 0:
          ingraph.pop(inNode)
          outgraph.pop(outNode)

def print_results(graph):
  for k, v in graph.items():
    for node in v:
      print k + SEPARATOR + node


if __name__ == "__main__":
  main(sys.stdin)
