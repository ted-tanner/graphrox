#include "gphrx.h"

static GphrxGraph new_gphrx(bool is_undirected)
{
    CsrAdjMatrix adjacency_matrix = {
        .matrix_col_idx_list = new_dynarr_u64(),
        .matrix_row_idx_list = new_dynarr_u64(),
    };
    
    GphrxGraph graph = {
        .is_undirected = is_undirected,
        .highest_vertex_id = 0,
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
    free_dynarr_u64(&graph->adjacency_matrix.matrix_col_idx_list);
    free_dynarr_u64(&graph->adjacency_matrix.matrix_row_idx_list);
}

void gphrx_shrink(GphrxGraph *graph)
{
    dynarr_u64_shrink(&graph->adjacency_matrix.matrix_col_idx_list);
    dynarr_u64_shrink(&graph->adjacency_matrix.matrix_row_idx_list);
}

static size_t index_of_edge(DynamicArrayU64 *arr, u64 vertex_id)
{
    size_t low = 0;
    size_t high = arr->size;

    size_t middle = 0;

    while (low <= high)
    {
        middle = (low + high) / 2;
        u64 curr = dynarr_u64_get(arr, middle);

        if (vertex_id == curr)
            return middle;
        else if (middle != 0 && dynarr_u64_get(arr, middle - 1) < vertex_id
                 &&  dynarr_u64_get(arr, middle) > vertex_id)
            return middle;
        else if (vertex_id > curr)
            low = middle + 1;
        else
        {
            if (middle == 0)
                return 0;
            
            high = middle - 1;
        }
    }

    return middle;
}

static size_t index_of_vertex(DynamicArrayU64 *col_arr, DynamicArrayU64 *row_arr,
                              u64 from_vertex_id, u64 to_vertex_id)
{
    size_t curr_idx = index_of_edge(col_arr, from_vertex_id);

    u64 curr_idx_temp = curr_idx;
    for (; curr_idx >= 0; --curr_idx)
    {
        if (dynarr_u64_get(col_arr, curr_idx) != from_vertex_id)
            break;
        if (dynarr_u64_get(row_arr, curr_idx) == to_vertex_id)
            return curr_idx;
    }

    curr_idx = curr_idx_temp;
    for (; curr_idx < row_arr->size; ++curr_idx)
    {
        if (dynarr_u64_get(col_arr, curr_idx) != from_vertex_id)
            break;
        if (dynarr_u64_get(row_arr, curr_idx) == to_vertex_id)
            return curr_idx;
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
    size_t vertex_idx = index_of_edge(&graph->adjacency_matrix.matrix_col_idx_list, vertex_id);
    size_t first_edge_idx = vertex_idx;
    size_t last_edge_idx = vertex_idx;

    while (first_edge_idx >= 1 &&
           dynarr_u64_get(&graph->adjacency_matrix.matrix_col_idx_list, first_edge_idx - 1) == vertex_id)
        --first_edge_idx;

    while (last_edge_idx < graph->adjacency_matrix.matrix_col_idx_list.size - 1 &&
           dynarr_u64_get(&graph->adjacency_matrix.matrix_col_idx_list, last_edge_idx + 1) == vertex_id)
        ++last_edge_idx;

    if (first_edge_idx != graph->adjacency_matrix.matrix_col_idx_list.size)
    {
        dynarr_u64_remove_multiple_at(&graph->adjacency_matrix.matrix_col_idx_list,
                                      first_edge_idx,
                                      last_edge_idx + 1 - first_edge_idx);

        dynarr_u64_remove_multiple_at(&graph->adjacency_matrix.matrix_row_idx_list,
                                      first_edge_idx,
                                      last_edge_idx + 1 - first_edge_idx);
    }

    for (size_t i = 0; i < graph->adjacency_matrix.matrix_row_idx_list.size; ++i)
    {
        if (dynarr_u64_get(&graph->adjacency_matrix.matrix_row_idx_list, i) == vertex_id)
        {
            dynarr_u64_remove_at(&graph->adjacency_matrix.matrix_col_idx_list, i);
            dynarr_u64_remove_at(&graph->adjacency_matrix.matrix_row_idx_list, i);
            --i; // The rest of the array has been shifted over, so must revisit same i to avoid skipping 
        }
    }

    if (vertex_id == graph->highest_vertex_id)
        --graph->highest_vertex_id;
}

void gphrx_add_edge(GphrxGraph *graph, u64 from_vertex_id, u64 to_vertex_id)
{
    size_t vertex_idx = from_vertex_id >= graph->highest_vertex_id
        ? graph->adjacency_matrix.matrix_col_idx_list.size
        : index_of_vertex(&graph->adjacency_matrix.matrix_col_idx_list,
                          &graph->adjacency_matrix.matrix_row_idx_list,
                          from_vertex_id, to_vertex_id);

    dynarr_u64_push_at(&graph->adjacency_matrix.matrix_col_idx_list, from_vertex_id, vertex_idx);
    dynarr_u64_push_at(&graph->adjacency_matrix.matrix_row_idx_list, to_vertex_id, vertex_idx);

    if (graph->is_undirected)
    {
        size_t vertex_idx = index_of_vertex(&graph->adjacency_matrix.matrix_col_idx_list,
                                            &graph->adjacency_matrix.matrix_row_idx_list,
                                            to_vertex_id, from_vertex_id);
        
        dynarr_u64_push_at(&graph->adjacency_matrix.matrix_col_idx_list, to_vertex_id, vertex_idx);
        dynarr_u64_push_at(&graph->adjacency_matrix.matrix_row_idx_list, from_vertex_id, vertex_idx);
    }

    if (from_vertex_id > graph->highest_vertex_id)
        graph->highest_vertex_id = from_vertex_id;

    if (to_vertex_id > graph->highest_vertex_id)
        graph->highest_vertex_id = to_vertex_id;
}

GphrxErrorCode gphrx_remove_edge(GphrxGraph *graph, u64 from_vertex_id, u64 to_vertex_id)
{
    size_t vertex_idx = index_of_vertex(&graph->adjacency_matrix.matrix_col_idx_list,
                                        &graph->adjacency_matrix.matrix_row_idx_list,
                                        from_vertex_id, to_vertex_id);

    if (dynarr_u64_get(&graph->adjacency_matrix.matrix_col_idx_list, vertex_idx) != from_vertex_id ||
        dynarr_u64_get(&graph->adjacency_matrix.matrix_row_idx_list, vertex_idx) != to_vertex_id)
        return GPHRX_ERROR_NOT_FOUND;
    
    dynarr_u64_remove_at(&graph->adjacency_matrix.matrix_col_idx_list, vertex_idx);
    dynarr_u64_remove_at(&graph->adjacency_matrix.matrix_row_idx_list, vertex_idx);

    if (graph->is_undirected)
    {
        size_t vertex_idx = index_of_vertex(&graph->adjacency_matrix.matrix_col_idx_list,
                                            &graph->adjacency_matrix.matrix_row_idx_list,
                                            to_vertex_id, from_vertex_id);
        
        dynarr_u64_remove_at(&graph->adjacency_matrix.matrix_col_idx_list, vertex_idx);
        dynarr_u64_remove_at(&graph->adjacency_matrix.matrix_row_idx_list, vertex_idx);
    }

    return GPHRX_NO_ERROR;
}

GphrxGraph approximate_gphrx(GphrxGraph *graph, u64 depth, float threshold);


#ifdef TEST_MODE

static TEST_RESULT test_new_gphrx()
{
    GphrxGraph undirected_graph = new_undirected_gphrx();
    GphrxGraph directed_graph = new_directed_gphrx();

    assert(undirected_graph.is_undirected, "Incorrect graph metadata");
    assert(!directed_graph.is_undirected, "Incorrect graph metadata");

    assert(undirected_graph.highest_vertex_id == 0, "Incorrect graph metadata");
    assert(directed_graph.highest_vertex_id == 0, "Incorrect graph metadata");

    assert(undirected_graph.adjacency_matrix.matrix_col_idx_list.size == 0, "Incorrect initial adjacency matrix");
    assert(undirected_graph.adjacency_matrix.matrix_row_idx_list.size == 0, "Incorrect initial adjacency matrix");
    assert(directed_graph.adjacency_matrix.matrix_col_idx_list.size == 0, "Incorrect initial adjacency matrix");
    assert(directed_graph.adjacency_matrix.matrix_row_idx_list.size == 0, "Incorrect initial adjacency matrix");

    free_gphrx(&undirected_graph);
    free_gphrx(&directed_graph);
    
    return TEST_PASS;
}

static TEST_RESULT test_free_gphrx()
{
    GphrxGraph undirected_graph = new_undirected_gphrx();
    GphrxGraph directed_graph = new_directed_gphrx();

    // Verify no segmentation fault
    free_gphrx(&undirected_graph);
    free_gphrx(&directed_graph);

    return TEST_PASS;
}

static TEST_RESULT test_gphrx_shrink()
{
    GphrxGraph undirected_graph = new_undirected_gphrx();
    GphrxGraph directed_graph = new_directed_gphrx();

    dynarr_u64_expand(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 500);
    dynarr_u64_expand(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 300);
    assert(undirected_graph.adjacency_matrix.matrix_col_idx_list.capacity == 500, "Incorrect matrix cols size");
    assert(undirected_graph.adjacency_matrix.matrix_row_idx_list.capacity == 300, "Incorrect matrix rows size");

    dynarr_u64_expand(&directed_graph.adjacency_matrix.matrix_col_idx_list, 500);
    dynarr_u64_expand(&directed_graph.adjacency_matrix.matrix_row_idx_list, 300);
    assert(directed_graph.adjacency_matrix.matrix_col_idx_list.capacity == 500, "Incorrect matrix cols size");
    assert(directed_graph.adjacency_matrix.matrix_row_idx_list.capacity == 300, "Incorrect matrix rows size");

    gphrx_shrink(&undirected_graph);
    assert(undirected_graph.adjacency_matrix.matrix_col_idx_list.capacity == 0, "Incorrect matrix cols size");
    assert(undirected_graph.adjacency_matrix.matrix_row_idx_list.capacity == 0, "Incorrect matrix rows size");

    gphrx_shrink(&directed_graph);
    assert(directed_graph.adjacency_matrix.matrix_col_idx_list.capacity == 0, "Incorrect matrix cols size");
    assert(directed_graph.adjacency_matrix.matrix_row_idx_list.capacity == 0, "Incorrect matrix rows size");

    free_gphrx(&undirected_graph);
    free_gphrx(&directed_graph);
    
    return TEST_PASS;
}

static TEST_RESULT test_gphrx_add_vertex()
{
    u64 to_edges[] = {3, 2, 100, 20, 9};
    
    GphrxGraph undirected_graph = new_undirected_gphrx();
    
    gphrx_add_edge(&undirected_graph, 1, 1000);
    gphrx_add_vertex(&undirected_graph, 7, to_edges, 5);    
    gphrx_add_edge(&undirected_graph, 500, 1001);
    gphrx_add_edge(&undirected_graph, 500, 7);    
    
    assert(undirected_graph.adjacency_matrix.matrix_col_idx_list.size == 16, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.matrix_row_idx_list.size == 16, "Incorrect adjacency matrix");
    
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 0) == 1,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 1) == 2,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 2) == 3,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 3) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 4) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 5) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 6) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 7) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 8) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 9) == 9,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 10) == 20,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 11) == 100,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 12) == 500,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 13) == 500,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 14) == 1000,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 15) == 1001,
           "Incorrect adjacency matrix");

    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 0) == 1000,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 1) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 2) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 3) == 3,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 4) == 2,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 5) == 100,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 6) == 20,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 7) == 9,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 8) == 500,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 9) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 10) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 11) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 12) == 1001,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 13) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 14) == 1,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 15) == 500,
           "Incorrect adjacency matrix");
    
    GphrxGraph directed_graph = new_directed_gphrx();
        
    gphrx_add_edge(&directed_graph, 1, 1000);
    gphrx_add_vertex(&directed_graph, 7, to_edges, 5);
    gphrx_add_edge(&directed_graph, 500, 1001);
    gphrx_add_edge(&directed_graph, 500, 7);
    
    assert(directed_graph.adjacency_matrix.matrix_col_idx_list.size == 8, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.matrix_row_idx_list.size == 8, "Incorrect adjacency matrix");
    
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_col_idx_list, 0) == 1,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_col_idx_list, 1) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_col_idx_list, 2) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_col_idx_list, 3) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_col_idx_list, 4) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_col_idx_list, 5) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_col_idx_list, 6) == 500,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_col_idx_list, 7) == 500,
           "Incorrect adjacency matrix");
  
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_row_idx_list, 0) == 1000,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_row_idx_list, 1) == 3,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_row_idx_list, 2) == 2,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_row_idx_list, 3) == 100,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_row_idx_list, 4) == 20,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_row_idx_list, 5) == 9,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_row_idx_list, 6) == 1001,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_row_idx_list, 7) == 7,
           "Incorrect adjacency matrix");

    free_gphrx(&undirected_graph);
    free_gphrx(&directed_graph);
    
    return TEST_PASS;
}

static TEST_RESULT test_gphrx_remove_vertex()
{
    u64 to_edges[] = {3, 2, 100, 20, 9};
    
    GphrxGraph undirected_graph = new_undirected_gphrx();
    
    gphrx_add_edge(&undirected_graph, 1, 1000);
    gphrx_add_vertex(&undirected_graph, 7, to_edges, 5);    
    gphrx_add_edge(&undirected_graph, 500, 1001);
    gphrx_add_edge(&undirected_graph, 500, 7);    
    
    assert(undirected_graph.adjacency_matrix.matrix_col_idx_list.size == 16, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.matrix_row_idx_list.size == 16, "Incorrect adjacency matrix");

    gphrx_remove_vertex(&undirected_graph, 7);

    assert(undirected_graph.adjacency_matrix.matrix_col_idx_list.size == 4, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.matrix_row_idx_list.size == 4, "Incorrect adjacency matrix");
    
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 0) == 1,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 1) == 500,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 2) == 1000,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 3) == 1001,
           "Incorrect adjacency matrix");

    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 0) == 1000,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 1) == 1001,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 2) == 1,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 3) == 500,
           "Incorrect adjacency matrix");
    
    gphrx_remove_vertex(&undirected_graph, 1000);

    assert(undirected_graph.adjacency_matrix.matrix_col_idx_list.size == 2, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.matrix_row_idx_list.size == 2, "Incorrect adjacency matrix");

    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 0) == 500,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 1) == 1001,
           "Incorrect adjacency matrix");

    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 0) == 1001,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 1) == 500,
           "Incorrect adjacency matrix");
    
    GphrxGraph directed_graph = new_directed_gphrx();
        
    gphrx_add_edge(&directed_graph, 1, 1000);
    gphrx_add_vertex(&directed_graph, 7, to_edges, 5);
    gphrx_add_edge(&directed_graph, 500, 1001);
    gphrx_add_edge(&directed_graph, 500, 7);
    
    assert(directed_graph.adjacency_matrix.matrix_col_idx_list.size == 8, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.matrix_row_idx_list.size == 8, "Incorrect adjacency matrix");

    gphrx_remove_vertex(&directed_graph, 7);

    assert(directed_graph.adjacency_matrix.matrix_col_idx_list.size == 2, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.matrix_row_idx_list.size == 2, "Incorrect adjacency matrix");
    
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_col_idx_list, 0) == 1,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_col_idx_list, 1) == 500,
           "Incorrect adjacency matrix");

    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_row_idx_list, 0) == 1000,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_row_idx_list, 1) == 1001,
           "Incorrect adjacency matrix");

    gphrx_remove_vertex(&directed_graph, 1000);

    assert(directed_graph.adjacency_matrix.matrix_col_idx_list.size == 1, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.matrix_row_idx_list.size == 1, "Incorrect adjacency matrix");

    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_col_idx_list, 0) == 500,
           "Incorrect adjacency matrix");

    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_row_idx_list, 0) == 1001,
           "Incorrect adjacency matrix");

    gphrx_remove_vertex(&directed_graph, 500);

    assert(directed_graph.adjacency_matrix.matrix_col_idx_list.size == 0, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.matrix_row_idx_list.size == 0, "Incorrect adjacency matrix");

    free_gphrx(&undirected_graph);
    free_gphrx(&directed_graph);
    
    return TEST_PASS;
}

static TEST_RESULT test_gphrx_add_edge()
{
    GphrxGraph undirected_graph = new_undirected_gphrx();
    GphrxGraph directed_graph = new_directed_gphrx();

    gphrx_add_edge(&undirected_graph, 100, 5);
    gphrx_add_edge(&directed_graph, 9, 1001);

    assert(undirected_graph.highest_vertex_id == 100, "Incorrect highest vertex ID");
    assert(directed_graph.highest_vertex_id == 1001, "Incorrect highest vertex ID");

    assert(undirected_graph.adjacency_matrix.matrix_col_idx_list.size == 2, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.matrix_row_idx_list.size == 2, "Incorrect adjacency matrix");
    
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 0) == 5,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_col_idx_list, 1) == 100,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 0) == 100,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.matrix_row_idx_list, 1) == 5,
           "Incorrect adjacency matrix");

    assert(directed_graph.adjacency_matrix.matrix_col_idx_list.size == 1, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.matrix_row_idx_list.size == 1, "Incorrect adjacency matrix");
    
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_col_idx_list, 0) == 9,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_row_idx_list, 0) == 1001,
           "Incorrect adjacency matrix");

    free_gphrx(&undirected_graph);
    free_gphrx(&directed_graph);
    
    return TEST_PASS;
}

static TEST_RESULT test_gphrx_remove_edge()
{
    GphrxErrorCode error = 0;
    
    GphrxGraph undirected_graph = new_undirected_gphrx();
    gphrx_add_edge(&undirected_graph, 100, 5);

    assert(undirected_graph.adjacency_matrix.matrix_col_idx_list.size == 2, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.matrix_row_idx_list.size == 2, "Incorrect adjacency matrix");

    error = gphrx_remove_edge(&undirected_graph, 100, 5);
    assert(error == GPHRX_NO_ERROR, "Edge not found");

    assert(undirected_graph.adjacency_matrix.matrix_col_idx_list.size == 0, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.matrix_row_idx_list.size == 0, "Incorrect adjacency matrix");
    
    gphrx_add_edge(&undirected_graph, 100, 5);

    assert(undirected_graph.adjacency_matrix.matrix_col_idx_list.size == 2, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.matrix_row_idx_list.size == 2, "Incorrect adjacency matrix");

    error = gphrx_remove_edge(&undirected_graph, 5, 100);
    assert(error == GPHRX_NO_ERROR, "Edge not found");

    assert(undirected_graph.adjacency_matrix.matrix_col_idx_list.size == 0, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.matrix_row_idx_list.size == 0, "Incorrect adjacency matrix");

    GphrxGraph directed_graph = new_directed_gphrx();
    gphrx_add_edge(&directed_graph, 9, 1001);
        
    assert(directed_graph.adjacency_matrix.matrix_col_idx_list.size == 1, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.matrix_row_idx_list.size == 1, "Incorrect adjacency matrix");

    error = gphrx_remove_edge(&directed_graph, 9, 1001);
    assert(error == GPHRX_NO_ERROR, "Edge not found");

    assert(directed_graph.adjacency_matrix.matrix_col_idx_list.size == 0, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.matrix_row_idx_list.size == 0, "Incorrect adjacency matrix");

    gphrx_add_edge(&directed_graph, 9, 995);
    gphrx_add_edge(&directed_graph, 9, 996);
    gphrx_add_edge(&directed_graph, 9, 997);
    gphrx_add_edge(&directed_graph, 9, 998);
    gphrx_add_edge(&directed_graph, 9, 999);
    
    assert(directed_graph.adjacency_matrix.matrix_col_idx_list.size == 5, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.matrix_row_idx_list.size == 5, "Incorrect adjacency matrix");

    error = gphrx_remove_edge(&directed_graph, 9, 998);
    assert(error == GPHRX_NO_ERROR, "Edge not found");

    assert(directed_graph.adjacency_matrix.matrix_col_idx_list.size == 4, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.matrix_row_idx_list.size == 4, "Incorrect adjacency matrix");

    bool found_val_before = false;
    bool found_val = false;
    bool found_val_after = false;
    for (size_t i = 0; i < directed_graph.adjacency_matrix.matrix_row_idx_list.size; ++i)
    {
        if (dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_row_idx_list, i) == 998)
            found_val = true;
        if (dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_row_idx_list, i) == 997)
            found_val_before = true;
        if (dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_row_idx_list, i) == 999)
            found_val_after = true;
    }

    assert(found_val_before && found_val_after && !found_val, "Incorrect vertex was removed");

    error = gphrx_remove_edge(&directed_graph, 9, 996);
    assert(error == GPHRX_NO_ERROR, "Edge not found");

    assert(directed_graph.adjacency_matrix.matrix_col_idx_list.size == 3, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.matrix_row_idx_list.size == 3, "Incorrect adjacency matrix");

    found_val_before = false;
    found_val = false;
    found_val_after = false;
    for (size_t i = 0; i < directed_graph.adjacency_matrix.matrix_row_idx_list.size; ++i)
    {
        if (dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_row_idx_list, i) == 996)
            found_val = true;
        if (dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_row_idx_list, i) == 995)
            found_val_before = true;
        if (dynarr_u64_get(&directed_graph.adjacency_matrix.matrix_row_idx_list, i) == 997)
            found_val_after = true;
    }

    assert(found_val_before && found_val_after && !found_val, "Incorrect vertex was removed");

    free_gphrx(&undirected_graph);
    free_gphrx(&directed_graph);
    
    return TEST_PASS;
}

ModuleTestSet gphrx_h_register_tests()
{
    ModuleTestSet set = {
        .module_name = __FILE__,
        .tests = {0},
        .count = 0,
    };

    register_test(&set, test_new_gphrx);
    register_test(&set, test_free_gphrx);
    register_test(&set, test_gphrx_shrink);
    register_test(&set, test_gphrx_add_vertex);
    register_test(&set, test_gphrx_remove_vertex);
    register_test(&set, test_gphrx_add_edge);
    register_test(&set, test_gphrx_remove_edge);

    return set;
}

#endif 
