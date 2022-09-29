#ifndef __GPHRX_H

#include <stdio.h>

#include "assert.h"
#include "debug.h"
#include "dynarr.h"
#include "intrinsics.h"

typedef u8 GPHRX_ERROR
#define GPHRX_NO_ERROR 0
#define GPHRX_FILE_IO_FAILURE 1

// TODO: Create DynArray of node structs containing ID, pointer to node in adjacency matrix, and degree
// TODO: Nodes are numberd with an ID. The nodes will be ordered in the matrix so they won't be in ID
//       order, but can be referenced by ID in the hash table
// TODO: Assign a padding to each node for when new nodes are added

typedef struct {
    u64 id;
    u64 degree;
    byte *matrix_pos;
    u16 padding;
} GphrxNode;

typedef struct {
    DynArray adjacency_matrix;
    DynArray node_list;
    u64 dimension;
} GraphRox;

GraphRox new_gphrx();

GraphRox gphrx_load_from_file(char *file_path);
void gphrx_save(GraphRox* graph, char *file_path);

#define __GPHRX_H
#endif
