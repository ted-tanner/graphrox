#include "gphrx.h"

static GphrxGraph new_gphrx(bool is_directed)
{
    CsrAdjMatrix adjacency_matrix = {
        .matrix_col_idx_list = new_dynarr(u8),
        .matrix_row_idx_list = new_dynarr(u8),
    };
    
    GphrxGraph graph = {
        .is_directed = is_directed,
        .node_count = 0,
        .adjacency_matrix = adjacency_matrix,
    };

    return graph;
}

GphrxGraph new_undirected_gphrx()
{
    return new_gphrx(false);
}

GphrxGraph new_directed_gphrx()
{
    return new_gphrx(true);
}

void gphrx_add_vertex(GphrxGraph *graph, u64 *adjacent_vertices)
{
    // TODO: Insert row into adjacency_matrix for new vertex
    // TODO: Loop over adjacent_vertices. For new row in matrix, place 1 per adjacent vertex. If
    //       graph is undirected, also add 1 to adjacent matricies' rows. If a provided adjacent
    //       vertex doesn't exist, create it by extending the matrix out

    ++graph->node_count;
}

void gphrx_remove_vertex(GphrxGraph *graph, u64 vertex_id);

void gphrx_add_edge(GphrxGraph *graph, u64 from_vertex_id, u64 to_vertex_id);

void gphrx_delete_edge(GphrxGraph *graph, u64 from_vertex_id, u64 to_vertex_id);

GphrxGraph approximate_gphrx(GphrxGraph *graph, u64 depth, float threshold);
