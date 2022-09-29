#ifndef __GPHRX_H

#include <stdio.h>

#include "assert.h"
#include "debug.h"
#include "dynarray.h"
#include "intrinsics.h"

typedef u8 GPHRX_ERROR;
#define GPHRX_NO_ERROR 0
#define GPHRX_FILE_IO_FAILURE 1

/**
 * Compress Space Row formatted adjacency matrix stored with dynamic arrays
 */
typedef struct {
    DynArray matrix_col_idx_list;
    DynArray matrix_row_idx_list; 
} CsrAdjMatrix;

/**
 * Metadata and representation of a graph
 */
typedef struct {
    u64 node_count;
    CsrAdjMatrix adjacency_matrix;
} GphrxGraph;

/**
 * Creates an empty GraphRox graph.
 */
GphrxGraph new_gphrx();

/**
 * Generates an approximation of a graph at a given depth in the bit-order tree representation of the graph.
 * This is where the magic of GraphRox happens.
 */
GphrxGraph approximate_gphrx(GphrxGraph *graph, u64 depth);

/**
 * Loads a graph from a GraphRox file
 */
GphrxGraph gphrx_load_from_file(char *file_path);

/**
 * Saves a GraphRox graph to a file
 */
void gphrx_save(GphrxGraph *graph, char *file_path);

#define __GPHRX_H
#endif
