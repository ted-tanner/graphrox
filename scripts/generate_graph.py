import sys
 
sys.path.append('../')
from graphrox import gphrx


# Reads a graph from a text file. Any lines beginning with a pound (#) will be ignored. Each line is
# expected to have two numbers separated by whitespace. The first is the FromNodeId and the second is
# the ToNodeId
def graph_from_text_file(file_name, out_file_name):
    graph = gphrx.GphrxUndirectedGraph()

    with open(file_name, 'r') as file:
        for line in file:
            if line.startswith('#'):
                continue

            ids = line.split()

            if len(ids) != 2:
                continue

            try:
                id0 = int(ids[0])
                id1 = int(ids[1])
            except ValueError:
                continue

            graph.add_edge(id0, id1)

    graph.save_to_file(out_file_name)
