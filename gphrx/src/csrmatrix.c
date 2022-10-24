#include "csrmatrix.h"

GphrxCsrMatrix new_gphrx_csr_matrix(bool is_adjacency_matrix, size_t table_size_exp_2)
{
    size_t size = fast_pow_2(table_size_exp_2);
    DynamicArray_TableEntry *arr = malloc(sizeof(DynamicArray_TableEntry) * size);

    for (size_t i = 0; i < size; ++i)
        arr[i] = new_dynarr_table_entry();

    assert(arr != 0, "malloc failure");

    HashTable table = {
        .size_pow_2 = size,
        .active_items = 0,
        .arr = arr,
    };

    GphrxCsrMatrix matrix = {
        .dimension = 0,
        .is_adjacency_matrix = is_adjacency_matrix,
        .col_table = table,
    }
    
    return table;
}

void free_gphrx_csr_matrix(GphrxCsrMatrix *matrix)
{
    for (size_t i = 0; i < matrix->col_table.size; ++i)
        free_dynarr_table_entry(&matrix->col_table.arr[i]);
    
    free(matrix->col_table.arr);
}

// This hash function was adapted from the following repository:
// https://github.com/skeeto/hash-prospector
static size_t hash(u64 key)
{
    // We don't need the full key. Our hash tables aren't large enough for that to matter
    u32 key_32bit = (u32) key;

    key_32bit ^= key_32bit >> 16;
    key_32bit *= 0x21f0aaad;
    key_32bit ^= key_32bit >> 15;
    key_32bit *= 0xd35a2d97;
    key_32bit ^= key_32bit >> 15;
    
    return (size_t) key_32bit;
}

void gphrx_csr_adj_matrix_add(GphrxCsrMatrix *matrix, u64 col, u64 row)
{
    assert(matrix->is_adjacency_matrix, "Matrix is not an adjacency matrix");
    
    size_t pos = fast_mod_pow_2(hash(key), table->size);

    // TODO: This should be a binary search
    TableEntry entry;
    bool found_entry = false;
    for (size_t i = 0; i < matrix->arr[pos].size; ++i)
    {
        if (dynarr_table_entry_get(&matrix->arr[pos], i).key == col)
        {
            found_entry = true;
            entry = dynarr_table_entry_get(&matrix->arr[pos], i);
        }
    }

    // TODO: Insert in order
    if (!found_entry)
    {
        DynamicArray_TableEntry new_entry {
        };
        
        dynarr_table_entry_push(&matrix->arr[pos], );
    }
}

