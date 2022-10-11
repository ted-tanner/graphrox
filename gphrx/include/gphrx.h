#ifndef __GPHRX_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "assert.h"
#include "dynarray.h"
#include "intrinsics.h"


/** Error codes */
typedef u8 GphrxErrorCode;

#define GPHRX_NO_ERROR 0
#define GPHRX_ERROR_NOT_FOUND 1
#define GPHRX_ERROR_INVALID_FORMAT 2


/**
 * Header for byte array representation of a GphrxGraph.
 */
#define GPHRX_HEADER_MAGIC_NUMBER 0x7AE71FFD
#define GPHRX_BYTE_ARRAY_VERSION 1

typedef struct {
    u32 magic_number;
    u32 version;
    u64 highest_vertex_id;
    u64 adjacency_matrix_dimension;
    u8 is_undirected;
    u8 is_weighted;
} GphrxByteArrayHeader;


/**
 * Compress Space Row formatted adjacency matrix stored with dynamic arrays.
 */
typedef struct {
    DynamicArrayU64 matrix_col_idx_list;
    DynamicArrayU64 matrix_row_idx_list; 
} CsrAdjMatrix;

/**
 * Metadata and representation of a graph.
 */
typedef struct {
    bool is_undirected;
    u64 highest_vertex_id;
    CsrAdjMatrix adjacency_matrix;
} GphrxGraph;


/**
 * Creates an empty undirected GraphRox graph.
 */
DLLEXPORT GphrxGraph new_undirected_gphrx();

/**
 * Creates an empty directed GraphRox graph.
 */
DLLEXPORT GphrxGraph new_directed_gphrx();

/**
 * Frees the memory used by a given graph.
 */
DLLEXPORT void free_gphrx(GphrxGraph *graph);

/**
 * Frees up excess memory used by the lists that describe the graph. This can substantially reduce memory
 * usage for graphs that are static (meaning edges and vertices are no longer being added), but can make
 * subsequent modifications to the graph slower.
 */
DLLEXPORT void gphrx_shrink(GphrxGraph *graph);

/**
 * Adds a vertex to the given graph.
 */
DLLEXPORT void gphrx_add_vertex(GphrxGraph *graph, u64 vertex_id, u64 *vertex_edges, u64 vertex_edge_count);

/**
 * Removes a vertex from the given graph.
 */
DLLEXPORT void gphrx_remove_vertex(GphrxGraph *graph, u64 vertex_id);

/**
 * Adds a link between two vertices. The "from" and "to" qualifiers on parameter names are only significant
 * when the graph is directed.
 */
DLLEXPORT void gphrx_add_edge(GphrxGraph *graph, u64 from_vertex_id, u64 to_vertex_id);

/**
 * Removes a link between two vertices. The "from" and "to" qualifiers on parameter names are only
 * significant when the graph is directed.
 */
DLLEXPORT GphrxErrorCode gphrx_remove_edge(GphrxGraph *graph, u64 from_vertex_id, u64 to_vertex_id);

// TODO: Instead of depth as param, do height from bottom (perhaps call it compression_level or compression_power?)
/**
 * Generates an approximation of a graph at a given depth in the bit-order tree representation of the graph
 * and a threshold of what percentage of edges must exist in a segment of the adjacency matrix for the
 * segment to be represented in the approximation. This is where the magic of GraphRox happens.
 */
DLLEXPORT GphrxGraph approximate_gphrx(GphrxGraph *graph, u64 depth, float threshold);

/**
 * Converts the given GphrxGraph to a big-endian byte array representation.
 */
DLLEXPORT byte *gphrx_to_byte_array(GphrxGraph *graph);

/**
 * Converts the given byte array from big-endian byte array representation of a GphrxGraph to a GphrxGraph.
 */
DLLEXPORT GphrxGraph gphrx_from_byte_array(byte *arr, GphrxErrorCode *error);

/**
 * Calls the C standard library `free()` on the provided pointer. This function is only intended for use by
 * foreign function interfaces for other languages importing GraphRox as a dynamic link library so they can
 * free memory allocated for byte arrays created by the `gphrx_from_byte_array()` function.
 */
DLLEXPORT void free_gphrx_byte_array(void *arr);


#ifdef TEST_MODE

#include "test.h"

ModuleTestSet gphrx_h_register_tests();

#endif


#define __GPHRX_H
#endif
