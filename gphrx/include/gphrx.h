#ifndef __GPHRX_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "assert.h"
#include "dynarray.h"
#include "intrinsics.h"

// TODO: In separate file, create a GphrxWeightedGraph struct and wgphrx_* functions. For
//       approximation, the entries in the approximated GphrxWeightedGraph will be averaged
//       together to form the entry in the approximated matrix.

// TODO: Functions
//         - Get from CSR matrix (give col and row, return entry)
//         - Scale an approximation given a threshold and scale factor

// TODO: Split matrices into their own files gphrx_matrix.h gphrx_matrix.c

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
    u64 adjacency_matrix_dimension;
    u64 csr_adjacency_matrix_size;
    u8 is_undirected;
    u8 is_weighted;
} GphrxByteArrayHeader;

// TODO: These matricies should really be hash tables so we don't have to perform a log(n)
//       search to find anything. Hash the column index and use a list of row indices (along
//       with entries for a weighted graph) as the value. We can perform a binary search on
//       the row indices because that search will be much smaller.
/**
 * Compress Space Row formatted adjacency matrix stored with dynamic arrays.
 */
typedef struct {
    u64 dimension;
    DynamicArray8 col_indices;
    DynamicArray8 row_indices;
} GphrxCsrAdjacencyMatrix;

/**
 * Compress Space Row formatted matrix stored with dynamic arrays.
 */
typedef struct {
    u64 dimension;
    DynamicArray8 entries;
    DynamicArray8 col_indices;
    DynamicArray8 row_indices; 
} GphrxCsrMatrix;

/**
 * Metadata and representation of a graph.
 */
typedef struct {
    bool is_undirected;
    GphrxCsrAdjacencyMatrix adjacency_matrix;
} GphrxGraph;

/**
 * Metadata and representation of a compressed graph.
 */
typedef struct {
    bool is_undirected;
    u64 dimension;
    GphrxCsrAdjacencyMatrix adjacency_matrix;
} GphrxCompressedGraph;

/**
 * Creates an empty undirected GraphRox graph.
 */
DLLEXPORT GphrxGraph new_undirected_gphrx();

/**
 * Creates an empty directed GraphRox graph.
 */
DLLEXPORT GphrxGraph new_directed_gphrx();

/**
 * Creates a copy of the given GraphRox graph.
 */
DLLEXPORT GphrxGraph duplicate_gphrx(GphrxGraph *restrict graph);

/**
 * Frees the memory used by the given graph.
 */
DLLEXPORT void free_gphrx(GphrxGraph *restrict graph);

/**
 * Frees the memory used by the given GphrxCsrMatrix.
 */
DLLEXPORT void free_gphrx_csr_matrix(GphrxCsrMatrix *restrict matrix);

/**
 * Frees the memory used by the given GphrxCsrAdjacencyMatrix.
 */
DLLEXPORT void free_gphrx_csr_adj_matrix(GphrxCsrAdjacencyMatrix *restrict matrix);

/**
 * Converts the given GphrxCsrMatrix to a string representation.
 */
DLLEXPORT char *gphrx_csr_matrix_to_string(GphrxCsrMatrix *restrict matrix, int decimal_digits);

/**
 * Converts the given GphrxCsrAdjacencyMatrix to a string representation.
 */
DLLEXPORT char *gphrx_csr_adj_matrix_to_string(GphrxCsrAdjacencyMatrix *restrict matrix);

/**
 * Frees up excess memory used by the lists that describe the graph. This can substantially reduce memory
 * usage for graphs that are static (meaning edges and vertices are no longer being added), but can make
 * subsequent modifications to the graph slower.
 */
DLLEXPORT void gphrx_shrink(GphrxGraph *restrict graph);

/**
 * Returns `true` if an edge with the given to and from vertex IDs exists and `false` otherwise.
 */
DLLEXPORT bool gphrx_does_edge_exist(GphrxGraph *restrict graph, u64 from_vertex_id, u64 to_vertex_id);

/**
 * Adds a vertex to the given graph.
 */
DLLEXPORT void gphrx_add_vertex(GphrxGraph *restrict graph, u64 vertex_id, u64 *vertex_edges, u64 vertex_edge_count);

/**
 * Removes a vertex from the given graph.
 */
DLLEXPORT void gphrx_remove_vertex(GphrxGraph *restrict graph, u64 vertex_id);

/**
 * Adds a link between two vertices. The "from" and "to" qualifiers on parameter names are only significant
 * when the graph is directed.
 */
DLLEXPORT void gphrx_add_edge(GphrxGraph *restrict graph, u64 from_vertex_id, u64 to_vertex_id);

/**
 * Removes a link between two vertices. The "from" and "to" qualifiers on parameter names are only
 * significant when the graph is directed.
 */
DLLEXPORT GphrxErrorCode gphrx_remove_edge(GphrxGraph *restrict graph, u64 from_vertex_id, u64 to_vertex_id);

/**
 * Returns an avg pool matrix for a given graph given a block dimension. The block_dimension
 * parameter determines the size of the blocks the graph's adjacency matrix will be split into to generate
 * the matrix. See the documentation for the approximate_gphrx function for more information.
 */
DLLEXPORT GphrxCsrMatrix gphrx_find_avg_pool_matrix(GphrxGraph *restrict graph, u64 block_dimension);

/**
 * Generates an approximation of a graph. This is where the magic of GraphRox happens.
 *
 * @param block_dimension determines the size of the blocks in the adjacency matrix that the threshold
 * will be applied to in the graph approximation. With a block size of 3, for example, the adjacency
 * matrix will be partitioned into 3x3 blocks. If the percentage of non-zero entries in the block
 * meets the threshold, the block will be represented by a one in the approximated adjacency matrix.
 * Blocks on the edge of the adjacency matrix will be zero-padded to form a block of full dimension for
 * the approximation.
 *
 * @param threshold is the percentage of entries in a block that must be non-zero for the block to be
 * represented by a one in the approximated adjacency matrix.
 */
DLLEXPORT GphrxGraph approximate_gphrx(GphrxGraph *restrict graph, u64 block_dimension, double threshold);

/**
 * Compresses a matrix by average pooling 8x8 blocks in a graph's adjacency matrix, applying a threshold
 * (blocks that fall below the threshold will be dropped in the compression, meaning those blocks in the
 * resulting compressed graph's adjacency matrix will be filled with zeros and therefore not represented
 * in edge lists), then representing entries in each 8x8 block with an unsigned 64-bit integer, with a
 * single bit representing a single edge in the block.
 */
DLLEXPORT GphrxCompressedGraph gphrx_compress_lossy(GphrxGraph *restrict graph, double threshold);

/**
 * Converts a GrphxGraph
 */
DLLEXPORT GphrxGraph gphrx_decompress(GphrxCompressedGraph *restrict graph);

/**
 * Converts the given GphrxGraph to a big-endian byte array representation.
 */
DLLEXPORT byte *gphrx_to_byte_array(GphrxGraph *restrict graph);

/**
 * Converts the given GphrxCompressedGraph to a big-endian byte array representation.
 */
DLLEXPORT byte *gphrx_compressed_to_byte_array(GphrxCompressedGraph *restrict graph);

/**
 * Converts the given byte array from big-endian byte array representation of a GphrxGraph to a GphrxGraph.
 */
DLLEXPORT GphrxGraph gphrx_from_byte_array(byte *restrict arr, GphrxErrorCode *restrict error);

/**
 * Converts the given byte array from big-endian byte array representation of a GphrxCompressedGraph to a
 * GphrxCompressedGraph.
 */
DLLEXPORT GphrxCompressedGraph gphrx_compressed_from_byte_array(byte *restrict arr, GphrxErrorCode *restrict error);

/**
 * Calls the C standard library `free()` on the provided pointer. This function is only intended for use by
 * foreign function interfaces for other languages importing GraphRox as a dynamic link library so they can
 * free memory allocated for byte arrays created by the `gphrx_from_byte_array()` or
 * `gphrx_compressed_from_byte_array()` functions.
 */
DLLEXPORT void free_gphrx_byte_array(void *restrict arr);


#ifdef TEST_MODE

#include "test.h"

ModuleTestSet gphrx_h_register_tests();

#endif


#define __GPHRX_H
#endif
