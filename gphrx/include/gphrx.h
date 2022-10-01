#ifndef __GPHRX_H

#include <stdbool.h>
#include <stdio.h>

#include "assert.h"
#include "debug.h"
#include "dynarray.h"
#include "intrinsics.h"

/** Error codes */
#define GPHRX_NO_ERROR 0
#define GPHRX_ERROR_NOT_FOUND 1

// TODO: Create separate struct and functions for weighted graphs. The CSR adjacency matrix will
//       be fundamentally different for weighted graphs functions for adding vertices and edges
//       will accept a weight as a parameter

/**
 * Compress Space Row formatted adjacency matrix stored with dynamic arrays
 */
typedef struct {
    DynamicArray matrix_col_idx_list;
    DynamicArray matrix_row_idx_list; 
} CsrAdjMatrix;

/**
 * Metadata and representation of a graph
 */
typedef struct {
    bool is_undirected;
    CsrAdjMatrix adjacency_matrix;
} GphrxGraph;

/**
 * Creates an empty undirected GraphRox graph.
 */
GphrxGraph new_undirected_gphrx();

/**
 * Creates an empty directed GraphRox graph.
 */
GphrxGraph new_directed_gphrx();

/**
 * Frees the memory used by the graph.
 */
void free_gphrx(GphrxGraph *graph);

/**
 * Removes a vertex from the given graph.
 */
void gphrx_remove_vertex(GphrxGraph *graph, u64 vertex_id);

/**
 * Adds a link between two vertices. The "from" and "to" qualifiers on parameter names are only significant
 * when the graph is directed.
 */
void gphrx_add_edge(GphrxGraph *graph, u64 from_vertex_id, u64 to_vertex_id);

/**
 * Removes a link between two vertices. The "from" and "to" qualifiers on parameter names are only
 * significant when the graph is directed.
 */
void gphrx_delete_edge(GphrxGraph *graph, u64 from_vertex_id, u64 to_vertex_id);

/**
 * Generates an approximation of a graph at a given depth in the bit-order tree representation of the graph
 * and a threshold of what percentage of edges must exist in a segment of the adjacency matrix for the
 * segment to be represented in the approximation. This is where the magic of GraphRox happens.
 */
GphrxGraph approximate_gphrx(GphrxGraph *graph, u64 depth, float threshold);

#define __GPHRX_H
#endif
