import sys
 
sys.path.append('../')
from graphrox import gphrx


if __name__ == '__main__':
    print("GraphRox Demo")
    print()

    print("GraphRox is a C library (with a Python wrapper) for generating approximations")
    print("of graphs for the purpose of fast graph calculations.")
    print()

    graph = gphrx.GphrxUndirectedGraph()

    graph.add_edge(7, 8)
    graph.add_vertex(1, [0, 2, 4, 7, 3])
    graph.add_vertex(5, [6, 8, 0, 1, 5, 4, 2])

    print("Adjacency matrix of an example graph with dimension of " + str(graph.adjacency_matrix.dimension()))
    print(graph.adjacency_matrix)
    print()

    print("2x2-block Occurrence Probability Matrix")
    print()

    print(graph.find_occurrence_matrix(2))
    print()

    print("Approximation using 2x2 blocks and a threshold of 0.3")
    print(graph.approximate(2, 0.3).adjacency_matrix)
    print()

    print("3x3-block Occurrence Probability Matrix")
    print()
    
    print(graph.find_occurrence_matrix(3))
    print()

    print("Approximation using 3x3 blocks and a threshold of 0.2")
    print(graph.approximate(3, 0.2).adjacency_matrix)
    print()

    print("--------------------------------------------------------------------------------")
    print()

    print("Loading graph from ca-HepPh.gphrx...")
    print()
    print("This graph represents the co-author network from Arxiv in the HEP-PH (High")
    print("Energy Physics - Phenomenology) category for papers published between January")
    print("1993 and April 2003")
    print()

    graph = gphrx.GphrxGraph.load_from_file("./sample-graphs/ca-HepPh.gphrx")

    print("This graph has " + str(graph.node_count()) + " nodes and " + str(graph.edge_count()) + " edges")
    print()

    print("Approximation using 10000x10000 blocks and a threshold of 0.00004")
    print(graph.approximate(10000, 0.00004).adjacency_matrix)
    print()

    print("--------------------------------------------------------------------------------")
    print()

    print("Loading graph from ca-AstroPh.gphrx...")
    print()
    print("This graph represents the co-author network from Arxiv in the ASTRO-PH (Astro")
    print("Physics) category for papers published between January 1993 and April 2003")
    print()

    graph = gphrx.GphrxGraph.load_from_file("./sample-graphs/ca-AstroPh.gphrx")

    print("This graph has " + str(graph.node_count()) + " nodes and " + str(graph.edge_count()) + " edges")
    print()

    print("Approximation using 10000x10000 blocks and a threshold of 0.00003")
    print(graph.approximate(10000, 0.00003).adjacency_matrix)
    print()

    print()
    print()
    print("Co-author network graphs are provied courtesy of Stanford University and can")
    print("be found at the following web address: http://snap.stanford.edu/data/")
    print()
    print("Citation for co-author network graphs:")
    print("J. Leskovec, J. Kleinberg and C. Faloutsos. Graph Evolution: Densification and")
    print("Shrinking Diameters. ACM Transactions on Knowledge Discovery from Data (ACM")
    print("TKDD), 1(1), 2007.")

    
