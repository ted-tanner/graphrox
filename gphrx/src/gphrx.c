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

void gphrx_shrink(GphrxGraph *graph)
{
    dynarr_shrink(&graph->adjacency_matrix.matrix_col_idx_list);
    dynarr_shrink(&graph->adjacency_matrix.matrix_row_idx_list);
}

static size_t index_of_vertex(DynamicArray *arr, u64 vertex_id)
{
    size_t middle = arr->size / 2;
    size_t delta = middle /= 2;

    size_t curr_idx = middle;
    u64 curr_val = dynarr_get(arr, middle, u64);

    while (curr_val != vertex_id && delta != 0)
    {
        if (curr_val > vertex_id)
            curr_idx = middle - delta;
        else if (curr_val < vertex_id)
            curr_idx = middle + delta;

        curr_val = dynarr_get(arr, curr_idx, u64);
        delta /= 2;
    }

    return curr_idx;
}

void gphrx_add_vertex(GphrxGraph *graph, u64 vertex_id, u64 *vertex_edges, u64 vertex_edge_count)
{
    for (u64 i = 0; i < vertex_edge_count; ++i)
        gphrx_add_edge(graph, vertex_id, vertex_edges[i]);
}

void gphrx_remove_vertex(GphrxGraph *graph, u64 vertex_id)
{
    size_t col_vertex_idx = index_of_vertex(&graph->adjacency_matrix.matrix_col_idx_list, vertex_id);

    if (dynarr_get(&graph->adjacency_matrix.matrix_col_idx_list, col_vertex_idx, u64) == vertex_id)
        dynarr_remove_at(&graph->adjacency_matrix.matrix_col_idx_list, col_vertex_idx);

    for (size_t i = 0; i < graph->adjacency_matrix.matrix_row_idx_list.size; ++i)
    {
        if (dynarr_get(&graph->adjacency_matrix.matrix_row_idx_list, i, u64) == vertex_id)
        {
            dynarr_remove_at(&graph->adjacency_matrix.matrix_row_idx_list, i);
        }
    }
}

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

void gphrx_delete_edge(GphrxGraph *graph, u64 from_vertex_id, u64 to_vertex_id)
{
    size_t vertex_idx = index_of_vertex(&graph->adjacency_matrix.matrix_col_idx_list, from_vertex_id);
    dynarr_remove_at(&graph->adjacency_matrix.matrix_col_idx_list, vertex_idx);
    dynarr_remove_at(&graph->adjacency_matrix.matrix_row_idx_list, vertex_idx);
}

GphrxGraph approximate_gphrx(GphrxGraph *graph, u64 depth, float threshold);
