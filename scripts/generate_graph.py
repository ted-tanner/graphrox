import numpy as np
import sys
 
sys.path.append('../')
from graphrox import gphrx


# Reads a graph from a text file. Any lines beginning with a pound (#) will be ignored. Each line is
# expected to have two numbers separated by whitespace. The first is the FromNodeId and the second is
# the ToNodeId
def graph_from_text_file(file_name, out_file_name):
    id_pairs = []
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

            id_pairs.append((id0, id1))

    id_pairs.sort()
    id_max = id_pairs[-1][0]

    id_to_new_id_map = np.full(id_max + 1, -1, dtype=int)

    new_id = 0
    for id_pair in id_pairs:
        if id_to_new_id_map[id_pair[0]] == -1:
            id_to_new_id_map[id_pair[0]] = new_id
            new_id += 1

    graph = gphrx.GphrxUndirectedGraph()

    for id_pair in id_pairs:
        from_id = id_pair[0]
        to_id = id_pair[1]
        new_from_id = id_to_new_id_map[from_id]
        new_to_id = id_to_new_id_map[to_id]

        graph.add_edge(new_from_id, new_to_id)
        
    graph.save_to_file(out_file_name)


def verify_graph(text_file_name, gphrx_file_name):
    id_pairs = []
    with open(text_file_name, 'r') as file:
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

            id_pairs.append((id0, id1))

    id_pairs.sort()
    id_max = id_pairs[-1][0]

    id_to_new_id_map = np.full(id_max + 1, -1, dtype=int)

    new_id = 0
    for id_pair in id_pairs:
        if id_to_new_id_map[id_pair[0]] == -1:
            id_to_new_id_map[id_pair[0]] = new_id
            new_id += 1

    graph = gphrx.GphrxGraph.load_from_file(gphrx_file_name)

    for id_pair in id_pairs:
        if not graph.does_edge_exist(id_to_new_id_map[id0], id_to_new_id_map[id1]):
            print("Missing edge " + str(id0) + "-" + str(id1))
