import sys
import string

SEPARATOR = '\t'  # The character separating nodes in a line of input

class Graph: 
  def __init__(self, edges):
    # If V1 is in graph, dereference the list and append, otherwise insert V1
    # with an empty list as its value and append to that new empty list
    self.ingraph = {}
    self.outgraph = {}
    for [V1, V2] in edges:
      self.ingraph.setdefault(V1, set()).add(V2)
      self.outgraph.setdefault(V2, set()).add(V1)

  def remove_duplicates(self):
    for V1 in self.ingraph.keys():
      if (V1 in self.ingraph) & (V1 in self.outgraph):
        self._check_remove_node(V1)

  def _check_remove_node(self, node):
      if len(self.ingraph[node]) == len(self.outgraph[node]) == 1:
        inNode = self.ingraph.pop(node).pop()
        outNode = self.outgraph.pop(node).pop()
        self.ingraph[outNode].remove(node)
        self.outgraph[inNode].remove(node)
        if inNode != outNode:
          self.ingraph[outNode].add(inNode)
          self.outgraph[inNode].add(outNode)
        else:
          if len(self.ingraph[inNode]) == len(self.outgraph[inNode]) == 0:
            self.ingraph.pop(inNode)
            self.outgraph.pop(outNode)

  def __str__(self):
    retval = ""
    for k, v in self.ingraph.items():
      for node in v:
        retval += k + SEPARATOR + node + '\n'
    return retval


def main(source):
  edges = read_edges(source)
  graph = Graph(edges)
  graph.remove_duplicates()
  sys.stdout.write(graph.__str__())  # Using sys to avoid auto newline of print

def read_edges(source): 
  # Iteratre through each line of stdin, stripping trailing whitespace and
  # splitting on separator 
  edges = [line.rstrip(string.whitespace).split(SEPARATOR) for line in source]

  return edges

if __name__ == "__main__":
  main(sys.stdin)
