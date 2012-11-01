import sys
import string

SEPARATOR = '\t'  # The character separating nodes in a line of input

def read_graph(source): 
  # Iteratre through each line of stdin, stripping trailing whitespace and
  # splitting on separator 
  edges = [line.rstrip(string.whitespace).split(SEPARATOR) for line in source]
  print edges

def parse_graph(edges):
  print "Hello"

def remove_duplicates(graph):
  print "Hello"

def print_results(graph):
  print "Hello"

if __name__ == "__main__":
  read_graph(sys.stdin)
