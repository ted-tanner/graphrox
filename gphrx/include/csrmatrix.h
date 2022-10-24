#ifndef __CSR_MATRIX_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "dynarray.h"
#include "intrinsics.h"

typedef struct {
    size_t size;
    size_t active_items;
    DynamicArray_TableEntry *table;
} HashTable;

/**
 * Compress Space Row formatted matrix stored with a hash table of dynamic arrays.
 */
typedef struct {
    u64 dimension;
    bool is_adjacency_matrix;
    HashTable col_table;
} GphrxCsrMatrix;

GphrxCsrMatrix new_gphrx_csr_matrix(bool is_adjacency_matrix, size_t table_size_exp_2);
void free_gphrx_csr_matrix(GphrxCsrMatrix *matrix);

void gphrx_csr_adj_matrix_add(GphrxCsrMatrix *matrix, u64 col, u64 row);
void gphrx_csr_matrix_add(GphrxCsrMatrix *matrix, u64 col, u64 row, double value);
void gphrx_csr_matrix_remove(GphrxCsrMatrix *matrix, u64 key);

#define __CSR_MATRIX_H
#endif
