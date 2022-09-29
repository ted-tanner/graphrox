#include "gphrx.h"

GphrxGraph new_gphrx()
{
    CsrAdjMatrix adjacency_matrix = {
        .matrix_col_idx_list = new_dynarr(u8),
        .matrix_row_idx_list = new_dynarr(u8),
    };
    
    GphrxGraph graph = {
        .node_count = 0,
        .adjacency_matrix = adjacency_matrix,
    };

    return graph;
}
