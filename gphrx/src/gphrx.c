#include "gphrx.h"

static GphrxGraph new_gphrx(bool is_undirected)
{
    CsrAdjMatrix adjacency_matrix = {
        .matrix_col_idx_list = new_dynarr(u64),
        .matrix_row_idx_list = new_dynarr(u64),
    };
    
    GphrxGraph graph = {
        .is_undirected = is_undirected,
        .adjacency_matrix = adjacency_matrix,
    };

    return graph;
}

GphrxGraph new_undirected_gphrx()
{
    return new_gphrx(true);
}

GphrxGraph new_directed_gphrx()
{
    return new_gphrx(false);
}

void free_gphrx(GphrxGraph *graph)
{
    free_dynarr(&graph->adjacency_matrix.matrix_col_idx_list);
    free_dynarr(&graph->adjacency_matrix.matrix_row_idx_list);
}

static size_t index_of_vertex(DynamicArray *arr, u64 vertex)
{
    size_t middle = arr->size / 2;
    size_t delta = middle /= 2;

    size_t curr_idx = middle;
    u64 curr_val = dynarr_get(arr, middle, u64);

    while (curr_val != vertex && delta != 0)
    {
        if (curr_val > vertex)
            curr_idx = middle - delta;
        else if (curr_val < vertex)
            curr_idx = middle + delta;

        curr_val = dynarr_get(arr, curr_idx, u64);
        delta /= 2;
    }

    return curr_idx;
}

void gphrx_remove_vertex(GphrxGraph *graph, u64 vertex_id);

void gphrx_add_edge(GphrxGraph *graph, u64 from_vertex_id, u64 to_vertex_id)
{
    size_t from_vertex_idx = index_of_vertex(&graph->adjacency_matrix.matrix_col_idx_list, from_vertex_id);

    dynarr_push_at(&graph->adjacency_matrix.matrix_col_idx_list, from_vertex_id, from_vertex_idx);
    dynarr_push_at(&graph->adjacency_matrix.matrix_col_idx_list, to_vertex_id, from_vertex_idx);

    if (graph->is_undirected)
    {
        dynarr_push_at(&graph->adjacency_matrix.matrix_col_idx_list, to_vertex_id, from_vertex_idx);
        dynarr_push_at(&graph->adjacency_matrix.matrix_col_idx_list, from_vertex_id, from_vertex_idx);
    }
}

void gphrx_delete_edge(GphrxGraph *graph, u64 from_vertex_id, u64 to_vertex_id);

GphrxGraph approximate_gphrx(GphrxGraph *graph, u64 depth, float threshold);
