import unittest, os
from connectNeighbors import read_edges, Graph 

input_dir = 'input'
output_dir = 'output'

class ConnectNeighborsTestCase(unittest.TestCase):
  def get_expected(self, filename):
    output = ""
    for line in open(os.path.join(output_dir, filename)):
      output += line
    return output

  def get_graph(self, filename):
    edges = read_edges(open(os.path.join(input_dir, filename)))
    g = Graph(edges)

    return g

  def test_zero(self):
    output = self.get_expected('0')
    g = self.get_graph('0')
    g.connect_neighbors()
    self.assertEqual(g.__str__(), output)

  def test_one(self):
    output = self.get_expected('1')
    g = self.get_graph('1')
    g.connect_neighbors()
    self.assertEqual(g.__str__(), output)

  def test_two(self):
    output = self.get_expected('2')
    g = self.get_graph('2')
    g.connect_neighbors()
    self.assertEqual(g.__str__(), output)

  def test_three(self):
    output = self.get_expected('3')
    g = self.get_graph('3')
    g.connect_neighbors()
    self.assertEqual(g.__str__(), output)
  def test_four(self):
    output = self.get_expected('4')
    g = self.get_graph('4')
    g.connect_neighbors()
    self.assertEqual(g.__str__(), output)

  def test_five(self):
    output = self.get_expected('5')
    g = self.get_graph('5')
    g.connect_neighbors()
    self.assertEqual(g.__str__(), output)

if __name__ == '__main__':
  unittest.main()
