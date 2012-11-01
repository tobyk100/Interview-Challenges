import sys
import string

SEPARATOR = '\t'  # The character separating nodes in a line of input

class Graph: 
  def __init__():
    __init__(self, {}, {})

  def __init__(self, ingraph, outgraph):
    self.ingraph = ingraph
    self.outgraph = outgraph

  def __str__(self):
    retval = ""
    for k, v in self.ingraph.items():
      for node in v:
        retval += k + SEPARATOR + node + '\n'
    return retval


def main(source):
  edges = read_graph(source)
  graph = parse_graph(edges)
  remove_duplicates(graph)
  sys.stdout.write(graph.__str__())  # Using sys to avoid auto newline of print

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

  return Graph(ingraph, outgraph)

def remove_duplicates(graph):
  for V1 in graph.ingraph.keys():
    if (V1 in graph.ingraph) & (V1 in graph.outgraph):
      check_remove_node(graph, V1)

def check_remove_node(graph, node):
    if len(graph.ingraph[node]) == len(graph.outgraph[node]) == 1:
      inNode = graph.ingraph.pop(node).pop()
      outNode = graph.outgraph.pop(node).pop()
      graph.ingraph[outNode].remove(node)
      graph.outgraph[inNode].remove(node)
      if inNode != outNode:
        graph.ingraph[outNode].add(inNode)
        graph.outgraph[inNode].add(outNode)
      else:
        if len(graph.ingraph[inNode]) == len(graph.outgraph[inNode]) == 0:
          graph.ingraph.pop(inNode)
          graph.outgraph.pop(outNode)

if __name__ == "__main__":
  main(sys.stdin)
