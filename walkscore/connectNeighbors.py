import sys
import string

SEPARATOR = '\t'  # The character separating nodes in a line of input

# The Graph class represents a directed graph
class Graph: 
  # Create a new instance of Graph.
  # Parameters
  #  - edges: a set of lines where each line represents an edge. Each line
  #           contains two nodes separated by a tab where there is an edge
  #           from the first node to the second.
  def __init__(self, edges):
    # If V1 is in graph, dereference the list and append, otherwise insert V1
    # with an empty list as its value and append to that new empty list
    self.ingraph = {}
    self.outgraph = {}
    for [V1, V2] in edges:
      self.ingraph.setdefault(V1, set()).add(V2)
      self.outgraph.setdefault(V2, set()).add(V1)

  # Connects neighbors in the instance object. Removes any node such that
  # the node has exactly one incoming edge and one outgoing edge. 
  def connect_neighbors(self):
    for V1 in self.ingraph.keys():
      if (V1 in self.ingraph) & (V1 in self.outgraph):  # Since we are concurrently
                                                        # modifying the graphs, make
                                                        # sure V1 is still there

        self._check_remove_node(V1)

  def _check_remove_node(self, node):
      if len(self.ingraph[node]) == len(self.outgraph[node]) == 1:
        inNode = self.ingraph.pop(node).pop()   # Pull out node's value set and
        outNode = self.outgraph.pop(node).pop() # it's one member
        self.ingraph[outNode].remove(node)  # Remove node from graphs
        self.outgraph[inNode].remove(node)
        if inNode != outNode:
          self.ingraph[outNode].add(inNode)  # create edge between two remaining
          self.outgraph[inNode].add(outNode) # nodes
        elif len(self.ingraph[inNode]) == len(self.outgraph[inNode]) == 0:
          # Node we removed cause another node to have no edges (disconect)
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
  graph.connect_neighbors()
  sys.stdout.write(graph.__str__())  # Using sys to avoid auto newline of print

def read_edges(source): 
  # Iteratre through each line of stdin, stripping trailing whitespace and
  # splitting on separator 
  edges = [line.rstrip(string.whitespace).split(SEPARATOR) for line in source]

  return edges

if __name__ == "__main__":
  main(sys.stdin)
