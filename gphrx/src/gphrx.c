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

DLLEXPORT GphrxGraph new_undirected_gphrx()
{
    return new_gphrx(true);
}

DLLEXPORT GphrxGraph new_directed_gphrx()
{
    return new_gphrx(false);
}

DLLEXPORT GphrxGraph duplicate_gphrx(GphrxGraph *graph)
{
    size_t edge_count = graph->adjacency_matrix.matrix_col_idx_list.size;
        
    CsrAdjMatrix adjacency_matrix = {
        .matrix_col_idx_list = new_dynarr_u64_with_capacity(edge_count),
        .matrix_row_idx_list = new_dynarr_u64_with_capacity(edge_count),
    };

    adjacency_matrix.matrix_col_idx_list.size = edge_count;
    adjacency_matrix.matrix_row_idx_list.size = edge_count;

    memcpy(adjacency_matrix.matrix_col_idx_list.arr,
           graph->adjacency_matrix.matrix_col_idx_list.arr,
           edge_count * sizeof(u64));

    memcpy(adjacency_matrix.matrix_row_idx_list.arr,
           graph->adjacency_matrix.matrix_row_idx_list.arr,
           edge_count * sizeof(u64));

    GphrxGraph duplicate_graph = {
        .is_undirected = graph->is_undirected,
        .highest_vertex_id = graph->highest_vertex_id,
        .adjacency_matrix = adjacency_matrix,
    };

    return duplicate_graph;
}

DLLEXPORT void free_gphrx(GphrxGraph *graph)
{
    free_dynarr_u64(&graph->adjacency_matrix.matrix_col_idx_list);
    free_dynarr_u64(&graph->adjacency_matrix.matrix_row_idx_list);
}

DLLEXPORT void gphrx_shrink(GphrxGraph *graph)
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

DLLEXPORT bool gphrx_does_edge_exist(GphrxGraph *graph, u64 from_vertex_id, u64 to_vertex_id)
{
    size_t vertex_idx = index_of_vertex(&graph->adjacency_matrix.matrix_col_idx_list,
                                        &graph->adjacency_matrix.matrix_row_idx_list,
                                        from_vertex_id, to_vertex_id);

    if (dynarr_u64_get(&graph->adjacency_matrix.matrix_col_idx_list, vertex_idx) == from_vertex_id
        && dynarr_u64_get(&graph->adjacency_matrix.matrix_row_idx_list, vertex_idx) == to_vertex_id)
    {
        return true;
    }

    return false;
}

DLLEXPORT void gphrx_add_vertex(GphrxGraph *graph, u64 vertex_id, u64 *vertex_edges, u64 vertex_edge_count)
{
    if (vertex_id > graph->highest_vertex_id)
        graph->highest_vertex_id = vertex_id;
        
    for (u64 i = 0; i < vertex_edge_count; ++i)
        gphrx_add_edge(graph, vertex_id, vertex_edges[i]);
}

DLLEXPORT void gphrx_remove_vertex(GphrxGraph *graph, u64 vertex_id)
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

DLLEXPORT void gphrx_add_edge(GphrxGraph *graph, u64 from_vertex_id, u64 to_vertex_id)
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

DLLEXPORT GphrxErrorCode gphrx_remove_edge(GphrxGraph *graph, u64 from_vertex_id, u64 to_vertex_id)
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

DLLEXPORT GphrxGraph approximate_gphrx(GphrxGraph *graph, u16 level, float threshold)
{
    if (level == 0)
        return duplicate_gphrx(graph);
    
    if (threshold > 1.0f)
        threshold = 1.0f;
    else if (threshold <= 0.0f)
        threshold = 0.000001f;

    size_t partition_size = fast_div_pow_2(graph->highest_vertex_id, level);
    
    if (partition_size >= graph->highest_vertex_id)
        partition_size = graph->highest_vertex_id / 2;

    // NOTE: This makes the approximation by using a binary tree representation of the column vectors
    //       in the adjacency_matrix. This could also be done using the row vectors, but the column
    //       index list is sorted in the GphrxGraph whereas the row index list is not.

    size_t estimated_approx_adj_matrix_size =
        fast_div_pow_2(graph->adjacency_matrix.matrix_row_idx_list.size, level);
    
    CsrAdjMatrix approx_adj_matrix = {
        .matrix_col_idx_list = new_dynarr_u64_with_capacity(estimated_approx_adj_matrix_size),
        .matrix_row_idx_list = new_dynarr_u64_with_capacity(estimated_approx_adj_matrix_size),
    };

    GphrxGraph approx_graph = {
        .is_undirected = graph->is_undirected,
        .highest_vertex_id = 0,
        .adjacency_matrix = approx_adj_matrix,
    };

    u64 current_partition_max = partition_size - 1;


    // Loop over the col list in the matrix_col_idx_list. Each sequence of the same vertex_id represents a column
    // in the adjacency_matrix (not all columns are represented in the list if not all the vertices have edges).
    // Go through each column (each sequence of the same number) and use the associated vertex_id from the
    // matrix_row_idx_list to see how many nodes are in each partition. Perhaps create "buckets," an
    // array that keeps track of the count of each vertex_id associated with the current sequence value.
    // Each index of the array will become a 1 or a 0 in the resulting column vector of the adjacency matrix,
    // depending on whether the count in the array meets the threshold provided by the user. For each array value
    // that meets the threshold, create an edge in the approx_graph with col = sequence value and row = array idx.

    
    
    // TODO: Figure out what the highest_vertex_id would be for the approximated graph. Will it always be the
    //       same as partition_size?
    
    return duplicate_gphrx(graph);
}

DLLEXPORT byte *gphrx_to_byte_array(GphrxGraph *graph)
{
    GphrxByteArrayHeader header = {
        .magic_number = GPHRX_HEADER_MAGIC_NUMBER,
        .version = GPHRX_BYTE_ARRAY_VERSION,
        .highest_vertex_id = graph->highest_vertex_id,
        .adjacency_matrix_dimension = (u64) graph->adjacency_matrix.matrix_col_idx_list.size,
        .is_undirected = graph->is_undirected,
        .is_weighted = false,
    };
    
    size_t buffer_size = 2 * graph->adjacency_matrix.matrix_col_idx_list.size * sizeof(u64)
        + sizeof(header);

    byte *buffer = malloc(buffer_size);
    size_t pos = 0;

    assert(buffer != 0, "malloc failure");

    if (is_system_big_endian())
    {
        memcpy(buffer, &header, sizeof(header));
        pos += sizeof(header);

        memcpy(buffer + pos,
               graph->adjacency_matrix.matrix_col_idx_list.arr,
               sizeof(u64) * graph->adjacency_matrix.matrix_col_idx_list.size);

        pos += sizeof(u64) * graph->adjacency_matrix.matrix_col_idx_list.size;

        memcpy(buffer + pos,
               graph->adjacency_matrix.matrix_row_idx_list.arr,
               sizeof(u64) * graph->adjacency_matrix.matrix_row_idx_list.size);
    }
    else
    {
        u32 temp_u32 = u32_reverse_bits(GPHRX_HEADER_MAGIC_NUMBER);
        memcpy(buffer + pos, &temp_u32, sizeof(u32));
        pos += sizeof(u32);

        temp_u32 = u32_reverse_bits(GPHRX_BYTE_ARRAY_VERSION);
        memcpy(buffer + pos, &temp_u32, sizeof(u32));
        pos += sizeof(u32);

        u64 temp_u64 = u64_reverse_bits(graph->highest_vertex_id);
        memcpy(buffer + pos, &temp_u64, sizeof(u64));
        pos += sizeof(u64);

        temp_u64 = u64_reverse_bits((u64) graph->adjacency_matrix.matrix_col_idx_list.size);
        memcpy(buffer + pos, &temp_u64, sizeof(u64));
        pos += sizeof(u64);

        memcpy(buffer + pos, &graph->is_undirected, sizeof(u8));
        pos += sizeof(u8);

        u8 temp_u8 = (u8) false;
        memcpy(buffer + pos, &temp_u8, sizeof(u8));
        pos += sizeof(u8);
        
        for (size_t i = 0; i < graph->adjacency_matrix.matrix_col_idx_list.size; ++i, pos += sizeof(u64))
        {
            temp_u64 = u64_reverse_bits(graph->adjacency_matrix.matrix_col_idx_list.arr[i]);
            memcpy(buffer + pos, &temp_u64, sizeof(u64));
        }

        for (size_t i = 0; i < graph->adjacency_matrix.matrix_row_idx_list.size; ++i, pos += sizeof(u64))
        {
            temp_u64 = u64_reverse_bits(graph->adjacency_matrix.matrix_row_idx_list.arr[i]);
            memcpy(buffer + pos, &temp_u64, sizeof(u64));
        }
    }

    return buffer;
}

DLLEXPORT GphrxGraph gphrx_from_byte_array(byte *arr, GphrxErrorCode *error)
{
    *error = GPHRX_NO_ERROR;
    GphrxGraph graph = {
        .is_undirected = 0,
        .highest_vertex_id = 0,
        .adjacency_matrix = 0,
    };
    
    size_t pos = 0;
    GphrxByteArrayHeader header;

    memcpy(&header.magic_number, arr, sizeof(u32));
    pos += sizeof(u32);

    if (!is_system_big_endian())
        header.magic_number = u32_reverse_bits(header.magic_number);

    if (header.magic_number != GPHRX_HEADER_MAGIC_NUMBER)
    {
        *error = GPHRX_ERROR_INVALID_FORMAT;
        return graph;
    }
    
    memcpy(&header.version, arr + pos, sizeof(u32));
    pos += sizeof(u32);

    memcpy(&header.highest_vertex_id, arr + pos, sizeof(u64));
    pos += sizeof(u64);

    memcpy(&header.adjacency_matrix_dimension, arr + pos, sizeof(u64));
    pos += sizeof(u64);

    memcpy(&header.is_undirected, arr + pos, sizeof(u8));
    pos += sizeof(u8);

    memcpy(&header.is_weighted, arr + pos, sizeof(u8));
    pos += sizeof(u8);

    if (!is_system_big_endian())
    {
        header.version = u32_reverse_bits(header.version);
        header.highest_vertex_id = u64_reverse_bits(header.highest_vertex_id);
        header.adjacency_matrix_dimension = u64_reverse_bits(header.adjacency_matrix_dimension);
    }

    CsrAdjMatrix adjacency_matrix = {
        .matrix_col_idx_list = new_dynarr_u64_with_capacity(header.adjacency_matrix_dimension),
        .matrix_row_idx_list = new_dynarr_u64_with_capacity(header.adjacency_matrix_dimension),
    };
    
    graph.is_undirected = header.is_undirected;
    graph.highest_vertex_id = header.highest_vertex_id;
    graph.adjacency_matrix = adjacency_matrix;

    graph.adjacency_matrix.matrix_col_idx_list.size = header.adjacency_matrix_dimension;
    graph.adjacency_matrix.matrix_row_idx_list.size = header.adjacency_matrix_dimension;

    memcpy(graph.adjacency_matrix.matrix_col_idx_list.arr,
               arr + pos,
               header.adjacency_matrix_dimension * sizeof(u64));
        
    pos += header.adjacency_matrix_dimension * sizeof(u64);
    
    memcpy(graph.adjacency_matrix.matrix_row_idx_list.arr,
           arr + pos,
           header.adjacency_matrix_dimension * sizeof(u64));

    if (!is_system_big_endian())
    {
        for (size_t i = 0; i < header.adjacency_matrix_dimension; ++i)
        {
            graph.adjacency_matrix.matrix_col_idx_list.arr[i] =
                u64_reverse_bits(graph.adjacency_matrix.matrix_col_idx_list.arr[i]);
        }

        for (size_t i = 0; i < header.adjacency_matrix_dimension; ++i)
        {
            graph.adjacency_matrix.matrix_row_idx_list.arr[i] =
                u64_reverse_bits(graph.adjacency_matrix.matrix_row_idx_list.arr[i]);
        }
    }
    
    return graph;
}

DLLEXPORT void free_gphrx_byte_array(void *arr)
{
    free(arr);
}

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

static TEST_RESULT test_duplicate_gphrx()
{
    GphrxGraph undirected_graph = new_undirected_gphrx();
    GphrxGraph directed_graph = new_directed_gphrx();

    gphrx_add_edge(&undirected_graph, 1, 1000);
    gphrx_add_edge(&undirected_graph, 500, 1001);
    gphrx_add_edge(&undirected_graph, 500, 7);
    
    gphrx_add_edge(&directed_graph, 1, 1000);
    gphrx_add_edge(&directed_graph, 500, 1001);
    gphrx_add_edge(&directed_graph, 500, 7);

    GphrxGraph dup_undirected_graph = duplicate_gphrx(&undirected_graph);
    GphrxGraph dup_directed_graph = duplicate_gphrx(&directed_graph);

    assert(undirected_graph.is_undirected == dup_undirected_graph.is_undirected, "Graph was incorrectly duplicated");
    assert(undirected_graph.highest_vertex_id == dup_undirected_graph.highest_vertex_id,
           "Graph was incorrectly duplicated");
    assert(undirected_graph.adjacency_matrix.matrix_col_idx_list.size ==
           dup_undirected_graph.adjacency_matrix.matrix_col_idx_list.size, "Graph was incorrectly duplicated");
    assert(undirected_graph.adjacency_matrix.matrix_row_idx_list.size ==
           dup_undirected_graph.adjacency_matrix.matrix_row_idx_list.size, "Graph was incorrectly duplicated");

    assert(directed_graph.is_undirected == dup_directed_graph.is_undirected, "Graph was incorrectly duplicated");
    assert(directed_graph.highest_vertex_id == dup_directed_graph.highest_vertex_id,
           "Graph was incorrectly duplicated");
    assert(directed_graph.adjacency_matrix.matrix_col_idx_list.size ==
           dup_directed_graph.adjacency_matrix.matrix_col_idx_list.size, "Graph was incorrectly duplicated");
    assert(directed_graph.adjacency_matrix.matrix_row_idx_list.size ==
           dup_directed_graph.adjacency_matrix.matrix_row_idx_list.size, "Graph was incorrectly duplicated");

    for (size_t i = 0; i < undirected_graph.adjacency_matrix.matrix_col_idx_list.size; ++i)
    {
        assert(undirected_graph.adjacency_matrix.matrix_col_idx_list.arr[i]
               == dup_undirected_graph.adjacency_matrix.matrix_col_idx_list.arr[i],
               "Graph was incorrectly duplicated");
        
        assert(undirected_graph.adjacency_matrix.matrix_row_idx_list.arr[i]
               == dup_undirected_graph.adjacency_matrix.matrix_row_idx_list.arr[i],
               "Graph was incorrectly duplicated");
    }

    for (size_t i = 0; i < directed_graph.adjacency_matrix.matrix_col_idx_list.size; ++i)
    {
        assert(directed_graph.adjacency_matrix.matrix_col_idx_list.arr[i]
               == dup_directed_graph.adjacency_matrix.matrix_col_idx_list.arr[i],
               "Graph was incorrectly duplicated");
        
        assert(directed_graph.adjacency_matrix.matrix_row_idx_list.arr[i]
               == dup_directed_graph.adjacency_matrix.matrix_row_idx_list.arr[i],
               "Graph was incorrectly duplicated");
    }
    
    free_gphrx(&undirected_graph);
    free_gphrx(&directed_graph);

    free_gphrx(&dup_undirected_graph);
    free_gphrx(&dup_directed_graph);
    
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

static TEST_RESULT test_gphrx_does_edge_exist()
{
    GphrxGraph undirected_graph = new_undirected_gphrx();
    GphrxGraph directed_graph = new_directed_gphrx();

    assert(!gphrx_does_edge_exist(&undirected_graph, 1, 1000), "Edge should not exist");
    assert(!gphrx_does_edge_exist(&undirected_graph, 1000, 1), "Edge should not exist");
    assert(!gphrx_does_edge_exist(&directed_graph, 1, 1000), "Edge should not exist");
    
    gphrx_add_edge(&undirected_graph, 1, 1000);
    gphrx_add_edge(&directed_graph, 1, 1000);
    
    assert(gphrx_does_edge_exist(&undirected_graph, 1, 1000), "Edge should exist");
    assert(gphrx_does_edge_exist(&undirected_graph, 1000, 1), "Edge should exist");
    assert(gphrx_does_edge_exist(&directed_graph, 1, 1000), "Edge should exist");
    
    assert(!gphrx_does_edge_exist(&directed_graph, 1000, 1), "Edge should not exist");
    gphrx_add_edge(&directed_graph, 1000, 1);
    assert(gphrx_does_edge_exist(&directed_graph, 1000, 1), "Edge should exist");

    assert(!gphrx_does_edge_exist(&undirected_graph, 100, 100), "Edge should not exist");
    assert(!gphrx_does_edge_exist(&directed_graph, 100, 100), "Edge should not exist");
    
    gphrx_add_edge(&undirected_graph, 100, 100);
    gphrx_add_edge(&directed_graph, 100, 100);
    
    assert(gphrx_does_edge_exist(&undirected_graph, 100, 100), "Edge should exist");
    assert(gphrx_does_edge_exist(&directed_graph, 100, 100), "Edge should exist");
    
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

static TEST_RESULT test_gphrx_to_from_byte_array()
{
    u64 to_edges[] = {3, 2, 100, 20, 9};
    
    GphrxGraph undirected_graph = new_undirected_gphrx();
    
    gphrx_add_edge(&undirected_graph, 2, 1002);
    gphrx_add_vertex(&undirected_graph, 8, to_edges, 5);
    gphrx_add_edge(&undirected_graph, 501, 1003);
    gphrx_add_edge(&undirected_graph, 501, 8);
    
    byte *arr = gphrx_to_byte_array(&undirected_graph);

    GphrxErrorCode error;
    GphrxGraph undir_graph_from_arr = gphrx_from_byte_array(arr, &error);

    assert(error == GPHRX_NO_ERROR, "Error unpacking graph from byte array");

    assert(undir_graph_from_arr.is_undirected, "Incorrectly loaded graph");
    assert(undir_graph_from_arr.highest_vertex_id == undirected_graph.highest_vertex_id,
           "Incorrectly loaded graph");

    assert(undir_graph_from_arr.adjacency_matrix.matrix_col_idx_list.size ==
           undirected_graph.adjacency_matrix.matrix_col_idx_list.size,
           "Incorrectly loaded graph adjacency matrix");

    assert(undir_graph_from_arr.adjacency_matrix.matrix_row_idx_list.size ==
           undirected_graph.adjacency_matrix.matrix_row_idx_list.size,
           "Incorrectly loaded graph adjacency matrix");

    for (size_t i = 0; i < undirected_graph.adjacency_matrix.matrix_col_idx_list.size; ++i)
    {
        assert(undirected_graph.adjacency_matrix.matrix_col_idx_list.arr[i] ==
               undir_graph_from_arr.adjacency_matrix.matrix_col_idx_list.arr[i],
               "Incorrectly loaded graph adjacency matrix");

        assert(undirected_graph.adjacency_matrix.matrix_row_idx_list.arr[i] ==
               undir_graph_from_arr.adjacency_matrix.matrix_row_idx_list.arr[i],
               "Incorrectly loaded graph adjacency matrix");
    }
    
    GphrxGraph directed_graph = new_directed_gphrx();
    
    gphrx_add_edge(&directed_graph, 1, 1000);
    gphrx_add_vertex(&directed_graph, 7, to_edges, 5);
    gphrx_add_edge(&directed_graph, 500, 1001);
    gphrx_add_edge(&directed_graph, 500, 7);

    arr = gphrx_to_byte_array(&directed_graph);

    GphrxGraph dir_graph_from_arr = gphrx_from_byte_array(arr, &error);

    assert(error == GPHRX_NO_ERROR, "Error unpacking graph from byte array");

    assert(!dir_graph_from_arr.is_undirected, "Incorrectly loaded graph");
    assert(dir_graph_from_arr.highest_vertex_id == directed_graph.highest_vertex_id,
           "Incorrectly loaded graph");

    assert(dir_graph_from_arr.adjacency_matrix.matrix_col_idx_list.size ==
           directed_graph.adjacency_matrix.matrix_col_idx_list.size,
           "Incorrectly loaded graph adjacency matrix");

    assert(dir_graph_from_arr.adjacency_matrix.matrix_row_idx_list.size ==
           directed_graph.adjacency_matrix.matrix_row_idx_list.size,
           "Incorrectly loaded graph adjacency matrix");

    for (size_t i = 0; i < directed_graph.adjacency_matrix.matrix_col_idx_list.size; ++i)
    {
        assert(directed_graph.adjacency_matrix.matrix_col_idx_list.arr[i] ==
               dir_graph_from_arr.adjacency_matrix.matrix_col_idx_list.arr[i],
               "Incorrectly loaded graph adjacency matrix");

        assert(directed_graph.adjacency_matrix.matrix_row_idx_list.arr[i] ==
               dir_graph_from_arr.adjacency_matrix.matrix_row_idx_list.arr[i],
               "Incorrectly loaded graph adjacency matrix");
    }
    
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
    register_test(&set, test_duplicate_gphrx);
    register_test(&set, test_free_gphrx);
    register_test(&set, test_gphrx_shrink);
    register_test(&set, test_gphrx_does_edge_exist);
    register_test(&set, test_gphrx_add_vertex);
    register_test(&set, test_gphrx_remove_vertex);
    register_test(&set, test_gphrx_add_edge);
    register_test(&set, test_gphrx_remove_edge);
    register_test(&set, test_gphrx_to_from_byte_array);

    return set;
}

#endif 
