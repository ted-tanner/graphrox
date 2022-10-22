#include "gphrx.h"

static GphrxGraph new_gphrx(bool is_undirected)
{
    GphrxCsrAdjacencyMatrix adjacency_matrix = {
        .dimension = 0,
        .col_indices = new_dynarr_u64(),
        .row_indices = new_dynarr_u64(),
    };
    
    GphrxGraph graph = {
        .is_undirected = is_undirected,
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
    size_t edge_count = graph->adjacency_matrix.col_indices.size;
        
    GphrxCsrAdjacencyMatrix adjacency_matrix = {
        .dimension = graph->adjacency_matrix.dimension,
        .col_indices = new_dynarr_u64_with_capacity(edge_count),
        .row_indices = new_dynarr_u64_with_capacity(edge_count),
    };

    adjacency_matrix.col_indices.size = edge_count;
    adjacency_matrix.row_indices.size = edge_count;

    memcpy(adjacency_matrix.col_indices.arr,
           graph->adjacency_matrix.col_indices.arr,
           edge_count * sizeof(u64));

    memcpy(adjacency_matrix.row_indices.arr,
           graph->adjacency_matrix.row_indices.arr,
           edge_count * sizeof(u64));

    GphrxGraph duplicate_graph = {
        .is_undirected = graph->is_undirected,
        .adjacency_matrix = adjacency_matrix,
    };

    return duplicate_graph;
}

DLLEXPORT void free_gphrx(GphrxGraph *graph)
{
    free_gphrx_csr_adj_matrix(&graph->adjacency_matrix);
}

// TODO: Add to Python
DLLEXPORT void free_gphrx_csr_matrix(GphrxCsrMatrix *matrix)
{
    free_dynarr_dbl(&matrix->entries);
    free_dynarr_u64(&matrix->col_indices);
    free_dynarr_u64(&matrix->row_indices);
}

// TODO: Add to Python
DLLEXPORT void free_gphrx_csr_adj_matrix(GphrxCsrAdjacencyMatrix *matrix)
{
    free_dynarr_u64(&matrix->col_indices);
    free_dynarr_u64(&matrix->row_indices);
}

// TODO: Add to Python (make sure to free memory)
DLLEXPORT char *gphrx_csr_matrix_to_string(GphrxCsrMatrix *matrix, int decimal_digits)
{
    double highest = 0.0;
    for (size_t i = 0; i < matrix->entries.size; ++i)
    {
        double curr = dynarr_dbl_get(&matrix->entries, i);
        if (curr > highest)
            highest = curr;
    }

    int digit_count = 1;
    for (double curr = highest; curr >= 10.0; curr /= 10, ++digit_count);

    int entry_size = digit_count + 1 + decimal_digits;

    const size_t extra_chars_per_row_at_front = 2; // '[ '
    const size_t extra_chars_per_row_at_back = 3; // ']\r\n'
    // Minus one to account for trailing comma being removed from final entry in row
    const size_t extra_chars_per_row_total = extra_chars_per_row_at_front + extra_chars_per_row_at_back - 1;
    size_t chars_per_entry = entry_size + 2; // number, comma, space

    char *buffer = malloc(extra_chars_per_row_total * matrix->dimension +
                          chars_per_entry * matrix->dimension * matrix->dimension);

    size_t pos = 0;
    for (u64 row = 0; row < matrix->dimension; ++row)
    {
        buffer[pos++] = '[';
        buffer[pos++] = ' ';
        
        for (u64 col = 0; col < matrix->dimension - 1; ++col)
        {
            sprintf(buffer + pos, "%*.*f", entry_size, decimal_digits, 0.0);
            pos += entry_size;
            buffer[pos++] = ',';
            buffer[pos++] = ' ';
        }

        // Last entry in row, without trailing comma
        sprintf(buffer + pos, "%*.*f", entry_size, decimal_digits, 0.0);
        pos += entry_size;
        buffer[pos++] = ' ';
        
        buffer[pos++] = ']';
        buffer[pos++] = '\r';
        buffer[pos++] = '\n';
    }

    // Chop off the last two chars (\r\n) in the string. There will be an extra two bytes allocated to the
    // buffer, but that is okay.
    buffer[pos - 2] = 0;

    size_t chars_per_row = extra_chars_per_row_total + matrix->dimension * chars_per_entry;

    for (size_t i = 0; i < matrix->col_indices.size; ++i)
    {
        u64 col = dynarr_u64_get(&matrix->col_indices, i);
        u64 row = dynarr_u64_get(&matrix->row_indices, i);
        double entry = dynarr_dbl_get(&matrix->entries, i);

        pos = row * chars_per_row + extra_chars_per_row_at_front + chars_per_entry * col;
        
        sprintf(buffer + pos, "%*.*f", entry_size, decimal_digits, entry);
        if (matrix->dimension == 1 || (col != 0 && col % (matrix->dimension - 1) == 0))
            buffer[pos + entry_size] = ' ';
        else 
            buffer[pos + entry_size] = ',';
    }

    return buffer;
}

// TODO: Add to Python (make sure to free memory)
DLLEXPORT char *gphrx_csr_adj_matrix_to_string(GphrxCsrAdjacencyMatrix *matrix)
{
    // Each row of the matrix is represented like this: [ 0, 0, 1, 0, 1, 1, 0 ]
    const size_t extra_chars_per_row_at_front = 2; // '[ '
    const size_t extra_chars_per_row_at_back = 3; // ']\r\n'
    // Minus one to account for trailing comma being removed from final entry in row
    const size_t extra_chars_per_row_total = extra_chars_per_row_at_front + extra_chars_per_row_at_back - 1;
    const size_t chars_per_entry = 3; // number, comma, space

    char *buffer = malloc(extra_chars_per_row_total * matrix->dimension +
                          chars_per_entry * matrix->dimension * matrix->dimension);

    size_t pos = 0;
    for (u64 row = 0; row < matrix->dimension; ++row)
    {
        buffer[pos++] = '[';
        buffer[pos++] = ' ';
        
        for (u64 col = 0; col < matrix->dimension - 1; ++col)
        {
            buffer[pos++] = '0';
            buffer[pos++] = ',';
            buffer[pos++] = ' ';
        }

        // Last entry in row, without trailing comma
        buffer[pos++] = '0';
        buffer[pos++] = ' ';
        
        buffer[pos++] = ']';

        buffer[pos++] = '\r';
        buffer[pos++] = '\n';
    }

    // Chop off the last two chars (\r\n) in the string. There will be an extra two bytes allocated to the
    // buffer, but that is okay.
    buffer[pos - 2] = 0;

    size_t chars_per_row = extra_chars_per_row_total + matrix->dimension * chars_per_entry;

    for (size_t i = 0; i < matrix->col_indices.size; ++i)
    {
        u64 col = dynarr_u64_get(&matrix->col_indices, i);
        u64 row = dynarr_u64_get(&matrix->row_indices, i);

        pos = row * chars_per_row + extra_chars_per_row_at_front + chars_per_entry * col;

        buffer[pos] = '1';
    }

    return buffer;
}

DLLEXPORT void gphrx_shrink(GphrxGraph *graph)
{
    dynarr_u64_shrink(&graph->adjacency_matrix.col_indices);
    dynarr_u64_shrink(&graph->adjacency_matrix.row_indices);
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
        if (dynarr_u64_get(col_arr, curr_idx) != from_vertex_id || curr_idx == 0)
            break;

        if (dynarr_u64_get(row_arr, curr_idx) < to_vertex_id &&
            curr_idx + 1 < col_arr->size &&
            dynarr_u64_get(col_arr, curr_idx + 1) == from_vertex_id &&
            dynarr_u64_get(row_arr, curr_idx + 1) > to_vertex_id)
        {
            return curr_idx + 1;
        }
        else if (dynarr_u64_get(row_arr, curr_idx) == to_vertex_id)
            return curr_idx;
    }

    curr_idx = curr_idx_temp;

    bool was_prev_greater = false;
    if (row_arr->size != 0)
    {
        for (; curr_idx <= row_arr->size; ++curr_idx)
        {
            if (dynarr_u64_get(col_arr, curr_idx) != from_vertex_id)
                return was_prev_greater ? curr_idx - 1 : curr_idx;
            
            if (dynarr_u64_get(row_arr, curr_idx) > to_vertex_id)
            {
                if (curr_idx - 1 >= 0 &&
                    dynarr_u64_get(col_arr, curr_idx - 1) == from_vertex_id &&
                    dynarr_u64_get(row_arr, curr_idx - 1) < to_vertex_id)
                {
                    return curr_idx - 1;
                }
                else
                    was_prev_greater = true;
            }
            else if (dynarr_u64_get(row_arr, curr_idx) == to_vertex_id)
                return curr_idx;
        }
    }

    return curr_idx > row_arr->size ? row_arr->size : curr_idx;
}

DLLEXPORT bool gphrx_does_edge_exist(GphrxGraph *graph, u64 from_vertex_id, u64 to_vertex_id)
{
    if (from_vertex_id > graph->adjacency_matrix.dimension || to_vertex_id > graph->adjacency_matrix.dimension)
        return false;
    
    size_t vertex_idx = index_of_vertex(&graph->adjacency_matrix.col_indices,
                                        &graph->adjacency_matrix.row_indices,
                                        from_vertex_id, to_vertex_id);

    if (dynarr_u64_get(&graph->adjacency_matrix.col_indices, vertex_idx) == from_vertex_id
        && dynarr_u64_get(&graph->adjacency_matrix.row_indices, vertex_idx) == to_vertex_id)
    {
        return true;
    }

    return false;
}

DLLEXPORT void gphrx_add_vertex(GphrxGraph *graph, u64 vertex_id, u64 *vertex_edges, u64 vertex_edge_count)
{
    if (vertex_id + 1 > graph->adjacency_matrix.dimension)
        graph->adjacency_matrix.dimension = vertex_id + 1;


    // TODO: Considering the vertex_id is always the same, we shouldn't have to search the list for the col
    //       every time.
    for (u64 i = 0; i < vertex_edge_count; ++i)
        gphrx_add_edge(graph, vertex_id, vertex_edges[i]);
}

DLLEXPORT void gphrx_remove_vertex(GphrxGraph *graph, u64 vertex_id)
{
    size_t vertex_idx = index_of_edge(&graph->adjacency_matrix.col_indices, vertex_id);
    size_t first_edge_idx = vertex_idx;
    size_t last_edge_idx = vertex_idx;

    while (first_edge_idx >= 1 &&
           dynarr_u64_get(&graph->adjacency_matrix.col_indices, first_edge_idx - 1) == vertex_id)
        --first_edge_idx;

    while (last_edge_idx < graph->adjacency_matrix.col_indices.size - 1 &&
           dynarr_u64_get(&graph->adjacency_matrix.col_indices, last_edge_idx + 1) == vertex_id)
        ++last_edge_idx;

    if (first_edge_idx != graph->adjacency_matrix.col_indices.size)
    {
        dynarr_u64_remove_multiple_at(&graph->adjacency_matrix.col_indices,
                                      first_edge_idx,
                                      last_edge_idx + 1 - first_edge_idx);

        dynarr_u64_remove_multiple_at(&graph->adjacency_matrix.row_indices,
                                      first_edge_idx,
                                      last_edge_idx + 1 - first_edge_idx);
    }

    for (size_t i = 0; i < graph->adjacency_matrix.row_indices.size; ++i)
    {
        if (dynarr_u64_get(&graph->adjacency_matrix.row_indices, i) == vertex_id)
        {
            dynarr_u64_remove_at(&graph->adjacency_matrix.col_indices, i);
            dynarr_u64_remove_at(&graph->adjacency_matrix.row_indices, i);
            --i; // The rest of the array has been shifted over, so must revisit same i to avoid skipping 
        }
    }

    if (vertex_id + 1 == graph->adjacency_matrix.dimension)
        --graph->adjacency_matrix.dimension;
}

DLLEXPORT void gphrx_add_edge(GphrxGraph *graph, u64 from_vertex_id, u64 to_vertex_id)
{
    size_t vertex_idx = from_vertex_id > graph->adjacency_matrix.dimension - 1
        ? graph->adjacency_matrix.col_indices.size
        : index_of_vertex(&graph->adjacency_matrix.col_indices,
                          &graph->adjacency_matrix.row_indices,
                          from_vertex_id, to_vertex_id);

    // TODO: This will only be effective once nodes are sorted by row as well
    // TODO: Test - Add this case to test_gphrx_add_edge 
    if (vertex_idx < graph->adjacency_matrix.col_indices.size &&
        dynarr_u64_get(&graph->adjacency_matrix.col_indices, vertex_idx) == from_vertex_id &&
        dynarr_u64_get(&graph->adjacency_matrix.row_indices, vertex_idx) == to_vertex_id)
    {
        // The edge already exists
        return;
    }

    dynarr_u64_push_at(&graph->adjacency_matrix.col_indices, from_vertex_id, vertex_idx);
    dynarr_u64_push_at(&graph->adjacency_matrix.row_indices, to_vertex_id, vertex_idx);
    
    if (graph->is_undirected && from_vertex_id != to_vertex_id)
    {
        size_t vertex_idx = index_of_vertex(&graph->adjacency_matrix.col_indices,
                                            &graph->adjacency_matrix.row_indices,
                                            to_vertex_id, from_vertex_id);

        dynarr_u64_push_at(&graph->adjacency_matrix.col_indices, to_vertex_id, vertex_idx);
        dynarr_u64_push_at(&graph->adjacency_matrix.row_indices, from_vertex_id, vertex_idx);
    }

    if (from_vertex_id + 1 > graph->adjacency_matrix.dimension)
        graph->adjacency_matrix.dimension = from_vertex_id + 1;

    if (to_vertex_id + 1 > graph->adjacency_matrix.dimension)
        graph->adjacency_matrix.dimension = to_vertex_id + 1;
}

DLLEXPORT GphrxErrorCode gphrx_remove_edge(GphrxGraph *graph, u64 from_vertex_id, u64 to_vertex_id)
{
    size_t vertex_idx = index_of_vertex(&graph->adjacency_matrix.col_indices,
                                        &graph->adjacency_matrix.row_indices,
                                        from_vertex_id, to_vertex_id);

    if (dynarr_u64_get(&graph->adjacency_matrix.col_indices, vertex_idx) != from_vertex_id ||
        dynarr_u64_get(&graph->adjacency_matrix.row_indices, vertex_idx) != to_vertex_id)
        return GPHRX_ERROR_NOT_FOUND;
    
    dynarr_u64_remove_at(&graph->adjacency_matrix.col_indices, vertex_idx);
    dynarr_u64_remove_at(&graph->adjacency_matrix.row_indices, vertex_idx);

    if (graph->is_undirected)
    {
        size_t vertex_idx = index_of_vertex(&graph->adjacency_matrix.col_indices,
                                            &graph->adjacency_matrix.row_indices,
                                            to_vertex_id, from_vertex_id);
        
        dynarr_u64_remove_at(&graph->adjacency_matrix.col_indices, vertex_idx);
        dynarr_u64_remove_at(&graph->adjacency_matrix.row_indices, vertex_idx);
    }

    return GPHRX_NO_ERROR;
}

// TODO: Add to Python
DLLEXPORT GphrxCsrMatrix gphrx_find_occurrence_matrix(GphrxGraph *graph, u64 block_dimension)
{
    if (block_dimension < 1)
        block_dimension = 1;

    u64 vertex_count = graph->adjacency_matrix.dimension;

    if (block_dimension > vertex_count)
        block_dimension = vertex_count;

    bool are_edge_blocks_padded = !(vertex_count % block_dimension == 0);
    
    u64 blocks_per_row = (vertex_count / block_dimension) + (are_edge_blocks_padded ? 1 : 0);
    u64 block_count = blocks_per_row * blocks_per_row;

    u64 *restrict occurrences = calloc(block_count, sizeof(u64));

    for (size_t i = 0; i < graph->adjacency_matrix.col_indices.size; ++i)
    {
        u64 col = dynarr_u64_get(&graph->adjacency_matrix.col_indices, i);
        u64 row = dynarr_u64_get(&graph->adjacency_matrix.row_indices, i);

        u64 col_pos = col / block_dimension;
        u64 row_pos = row / block_dimension;

        size_t occurrences_pos = row_pos * blocks_per_row + col_pos;
        ++occurrences[occurrences_pos];
    }

    GphrxCsrMatrix occurrence_matrix = {
        .dimension = blocks_per_row,
        .entries = new_dynarr_dbl_with_capacity(block_count),
        .col_indices = new_dynarr_u64_with_capacity(block_count),
        .row_indices = new_dynarr_u64_with_capacity(block_count),
    };
    
    double block_size = block_dimension * block_dimension;
    for (size_t col = 0; col < blocks_per_row; ++col)
    {
        for (size_t row = 0; row < blocks_per_row; ++row)
        {
            size_t row_start = row * blocks_per_row;
            double entry = occurrences[row_start + col] / (double) block_size;

            if (entry != 0.0)
            {
                dynarr_dbl_push(&occurrence_matrix.entries, entry);
                dynarr_u64_push(&occurrence_matrix.col_indices, col);
                dynarr_u64_push(&occurrence_matrix.row_indices, row);
            }
        }
    }

    free(occurrences);

    return occurrence_matrix;
}

// TODO: Add to Python
DLLEXPORT GphrxGraph approximate_gphrx(GphrxGraph *graph, u64 block_dimension, double threshold)
{
    if (block_dimension <= 1 || graph->adjacency_matrix.col_indices.size <= 1)
        return duplicate_gphrx(graph);

    u64 vertex_count = graph->adjacency_matrix.dimension;

    if (block_dimension > vertex_count)
        block_dimension = vertex_count;

    bool are_edge_blocks_padded = !(vertex_count % block_dimension == 0);
    
    u64 blocks_per_row = (vertex_count / block_dimension) + (are_edge_blocks_padded ? 1 : 0);
    u64 block_count = blocks_per_row * blocks_per_row;
    
    if (threshold > 1.0f)
        threshold = 1.0f;
    else if (threshold <= 0.0f)
        threshold = 0.00000001f;

    GphrxCsrMatrix occurrence_matrix = gphrx_find_occurrence_matrix(graph, block_dimension);

    GphrxCsrAdjacencyMatrix approx_adj_matrix = {
        .dimension = occurrence_matrix.dimension,
        .col_indices = new_dynarr_u64_with_capacity(block_count),
        .row_indices = new_dynarr_u64_with_capacity(block_count),
    };

    GphrxGraph approx_graph = {
        .is_undirected = graph->is_undirected,
        .adjacency_matrix = approx_adj_matrix,
    };

    for (size_t i = 0; i < occurrence_matrix.entries.size; ++i)
    {
        if (dynarr_dbl_get(&occurrence_matrix.entries, i) >= threshold)
        {
            dynarr_u64_push(&approx_graph.adjacency_matrix.col_indices,
                            dynarr_dbl_get(&occurrence_matrix.col_indices, i));
            dynarr_u64_push(&approx_graph.adjacency_matrix.row_indices,
                            dynarr_dbl_get(&occurrence_matrix.row_indices, i));
        }
    }

    free_gphrx_csr_matrix(&occurrence_matrix);
    
    return approx_graph;
}

DLLEXPORT byte *gphrx_to_byte_array(GphrxGraph *graph)
{
    GphrxByteArrayHeader header = {
        .magic_number = GPHRX_HEADER_MAGIC_NUMBER,
        .version = GPHRX_BYTE_ARRAY_VERSION,
        .adjacency_matrix_dimension = graph->adjacency_matrix.dimension,
        .csr_adjacency_matrix_size = (u64) graph->adjacency_matrix.col_indices.size,
        .is_undirected = graph->is_undirected,
        .is_weighted = false,
    };
    
    size_t buffer_size = 2 * graph->adjacency_matrix.col_indices.size * sizeof(u64)
        + sizeof(header);

    byte *buffer = malloc(buffer_size);
    size_t pos = 0;

    assert(buffer != 0, "malloc failure");

    if (is_system_big_endian())
    {
        memcpy(buffer, &header, sizeof(header));
        pos += sizeof(header);

        memcpy(buffer + pos,
               graph->adjacency_matrix.col_indices.arr,
               sizeof(u64) * graph->adjacency_matrix.col_indices.size);

        pos += sizeof(u64) * graph->adjacency_matrix.col_indices.size;

        memcpy(buffer + pos,
               graph->adjacency_matrix.row_indices.arr,
               sizeof(u64) * graph->adjacency_matrix.row_indices.size);
    }
    else
    {
        u32 temp_u32 = u32_reverse_bits(GPHRX_HEADER_MAGIC_NUMBER);
        memcpy(buffer + pos, &temp_u32, sizeof(u32));
        pos += sizeof(u32);

        temp_u32 = u32_reverse_bits(GPHRX_BYTE_ARRAY_VERSION);
        memcpy(buffer + pos, &temp_u32, sizeof(u32));
        pos += sizeof(u32);

        u64 temp_u64 = u64_reverse_bits((u64) graph->adjacency_matrix.dimension);
        memcpy(buffer + pos, &temp_u64, sizeof(u64));
        pos += sizeof(u64);
        
        temp_u64 = u64_reverse_bits((u64) graph->adjacency_matrix.col_indices.size);
        memcpy(buffer + pos, &temp_u64, sizeof(u64));
        pos += sizeof(u64);

        memcpy(buffer + pos, &graph->is_undirected, sizeof(u8));
        pos += sizeof(u8);

        u8 temp_u8 = (u8) false;
        memcpy(buffer + pos, &temp_u8, sizeof(u8));
        pos += sizeof(u8);
        
        for (size_t i = 0; i < graph->adjacency_matrix.col_indices.size; ++i, pos += sizeof(u64))
        {
            temp_u64 = u64_reverse_bits(graph->adjacency_matrix.col_indices.arr[i]);
            memcpy(buffer + pos, &temp_u64, sizeof(u64));
        }

        for (size_t i = 0; i < graph->adjacency_matrix.row_indices.size; ++i, pos += sizeof(u64))
        {
            temp_u64 = u64_reverse_bits(graph->adjacency_matrix.row_indices.arr[i]);
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

    memcpy(&header.adjacency_matrix_dimension, arr + pos, sizeof(u64));
    pos += sizeof(u64);

    memcpy(&header.csr_adjacency_matrix_size, arr + pos, sizeof(u64));
    pos += sizeof(u64);

    memcpy(&header.is_undirected, arr + pos, sizeof(u8));
    pos += sizeof(u8);

    memcpy(&header.is_weighted, arr + pos, sizeof(u8));
    pos += sizeof(u8);

    if (!is_system_big_endian())
    {
        header.version = u32_reverse_bits(header.version);
        header.adjacency_matrix_dimension = u64_reverse_bits(header.adjacency_matrix_dimension);
        header.csr_adjacency_matrix_size = u64_reverse_bits(header.csr_adjacency_matrix_size);
    }

    GphrxCsrAdjacencyMatrix adjacency_matrix = {
        .col_indices = new_dynarr_u64_with_capacity(header.csr_adjacency_matrix_size),
        .row_indices = new_dynarr_u64_with_capacity(header.csr_adjacency_matrix_size),
    };
    
    graph.is_undirected = header.is_undirected;
    graph.adjacency_matrix = adjacency_matrix;

    graph.adjacency_matrix.dimension = header.adjacency_matrix_dimension;
    graph.adjacency_matrix.col_indices.size = header.csr_adjacency_matrix_size;
    graph.adjacency_matrix.row_indices.size = header.csr_adjacency_matrix_size;

    memcpy(graph.adjacency_matrix.col_indices.arr,
           arr + pos,
           header.csr_adjacency_matrix_size * sizeof(u64));
    pos += header.csr_adjacency_matrix_size * sizeof(u64);
    
    memcpy(graph.adjacency_matrix.row_indices.arr,
           arr + pos,
           header.csr_adjacency_matrix_size * sizeof(u64));

    if (!is_system_big_endian())
    {
        for (size_t i = 0; i < header.csr_adjacency_matrix_size; ++i)
        {
            graph.adjacency_matrix.col_indices.arr[i] =
                u64_reverse_bits(graph.adjacency_matrix.col_indices.arr[i]);
        }

        for (size_t i = 0; i < header.csr_adjacency_matrix_size; ++i)
        {
            graph.adjacency_matrix.row_indices.arr[i] =
                u64_reverse_bits(graph.adjacency_matrix.row_indices.arr[i]);
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

    assert(undirected_graph.adjacency_matrix.col_indices.size == 0, "Incorrect initial adjacency matrix");
    assert(undirected_graph.adjacency_matrix.row_indices.size == 0, "Incorrect initial adjacency matrix");
    assert(undirected_graph.adjacency_matrix.dimension == 0, "Incorrect initial adjacency matrix");
    assert(directed_graph.adjacency_matrix.col_indices.size == 0, "Incorrect initial adjacency matrix");
    assert(directed_graph.adjacency_matrix.row_indices.size == 0, "Incorrect initial adjacency matrix");
    assert(directed_graph.adjacency_matrix.dimension == 0, "Incorrect initial adjacency matrix");

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
    assert(undirected_graph.adjacency_matrix.dimension == dup_undirected_graph.adjacency_matrix.dimension,
           "Graph was incorrectly duplicated");
    assert(undirected_graph.adjacency_matrix.col_indices.size ==
           dup_undirected_graph.adjacency_matrix.col_indices.size, "Graph was incorrectly duplicated");
    assert(undirected_graph.adjacency_matrix.row_indices.size ==
           dup_undirected_graph.adjacency_matrix.row_indices.size, "Graph was incorrectly duplicated");

    assert(directed_graph.is_undirected == dup_directed_graph.is_undirected, "Graph was incorrectly duplicated");
    assert(directed_graph.adjacency_matrix.dimension == dup_directed_graph.adjacency_matrix.dimension,
           "Graph was incorrectly duplicated");
    assert(directed_graph.adjacency_matrix.col_indices.size ==
           dup_directed_graph.adjacency_matrix.col_indices.size, "Graph was incorrectly duplicated");
    assert(directed_graph.adjacency_matrix.row_indices.size ==
           dup_directed_graph.adjacency_matrix.row_indices.size, "Graph was incorrectly duplicated");

    for (size_t i = 0; i < undirected_graph.adjacency_matrix.col_indices.size; ++i)
    {
        assert(undirected_graph.adjacency_matrix.col_indices.arr[i]
               == dup_undirected_graph.adjacency_matrix.col_indices.arr[i],
               "Graph was incorrectly duplicated");
        
        assert(undirected_graph.adjacency_matrix.row_indices.arr[i]
               == dup_undirected_graph.adjacency_matrix.row_indices.arr[i],
               "Graph was incorrectly duplicated");
    }

    for (size_t i = 0; i < directed_graph.adjacency_matrix.col_indices.size; ++i)
    {
        assert(directed_graph.adjacency_matrix.col_indices.arr[i]
               == dup_directed_graph.adjacency_matrix.col_indices.arr[i],
               "Graph was incorrectly duplicated");
        
        assert(directed_graph.adjacency_matrix.row_indices.arr[i]
               == dup_directed_graph.adjacency_matrix.row_indices.arr[i],
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

    gphrx_add_edge(&undirected_graph, 1, 2);
    gphrx_add_edge(&undirected_graph, 3, 2);
    gphrx_add_edge(&undirected_graph, 3, 1);
    gphrx_add_edge(&undirected_graph, 4, 3);
    gphrx_add_edge(&undirected_graph, 4, 2);
    gphrx_add_edge(&undirected_graph, 100, 10000);
    
    // Verify no segmentation fault
    free_gphrx(&undirected_graph);

    return TEST_PASS;
}

static TEST_RESULT test_free_gphrx_csr_matrix()
{
    GphrxCsrMatrix matrix = {
        .dimension = 5,
        .entries = new_dynarr_dbl_with_capacity(100),
        .col_indices = new_dynarr_u64_with_capacity(150),
        .row_indices = new_dynarr_u64_with_capacity(125),
    };

    // Verify no segmentation fault
    free_gphrx_csr_matrix(&matrix);
    
    return TEST_PASS;
}

static TEST_RESULT test_free_gphrx_csr_adj_matrix()
{
    GphrxCsrAdjacencyMatrix matrix = {
        .dimension = 5,
        .col_indices = new_dynarr_u64_with_capacity(100),
        .row_indices = new_dynarr_u64_with_capacity(10),
    };

    // Verify no segmentation fault
    free_gphrx_csr_adj_matrix(&matrix);

    return TEST_PASS;
}

static TEST_RESULT test_gphrx_csr_matrix_to_string()
{
    GphrxGraph undirected_graph = new_undirected_gphrx();

    gphrx_add_edge(&undirected_graph, 0, 1);
    gphrx_add_edge(&undirected_graph, 1, 1);
    gphrx_add_edge(&undirected_graph, 2, 1);
    gphrx_add_edge(&undirected_graph, 2, 0);
    gphrx_add_edge(&undirected_graph, 3, 2);
    gphrx_add_edge(&undirected_graph, 3, 1);
    gphrx_add_edge(&undirected_graph, 3, 4);
    gphrx_add_edge(&undirected_graph, 0, 6);
    gphrx_add_edge(&undirected_graph, 6, 6);
    
    GphrxCsrMatrix occurrence_matrix = gphrx_find_occurrence_matrix(&undirected_graph, 3);
    char *occurrence_matrix_str = gphrx_csr_matrix_to_string(&occurrence_matrix, 3);

    char *expected_str = "[ 0.778, 0.222, 0.111 ]\r\n[ 0.222, 0.222, 0.000 ]\r\n[ 0.111, 0.000, 0.111 ]";
    assert(strcmp(occurrence_matrix_str, expected_str) == 0, "CSR matrix string does not match expected");

    free_gphrx_byte_array(occurrence_matrix_str);

    occurrence_matrix.entries.arr[2] = 500023.977;
    char *matrix_str = gphrx_csr_matrix_to_string(&occurrence_matrix, 2);

    char *matrix_expected_str = "[      0.78,      0.22,      0.11 ]\r\n[      0.22,      0.22,      0.00 ]\r\n[ 500023.98,      0.00,      0.11 ]";
    assert(strcmp(matrix_str, matrix_expected_str) == 0, "CSR matrix string does not match expected");

    free_gphrx_byte_array(matrix_str);

    free_gphrx_csr_matrix(&occurrence_matrix);
    free_gphrx(&undirected_graph);

    return TEST_PASS;
}

static TEST_RESULT test_gphrx_csr_adj_matrix_to_string()
{
    GphrxGraph undirected_graph = new_undirected_gphrx();

    gphrx_add_edge(&undirected_graph, 0, 1);
    gphrx_add_edge(&undirected_graph, 1, 1);
    gphrx_add_edge(&undirected_graph, 2, 1);
    gphrx_add_edge(&undirected_graph, 2, 0);
    gphrx_add_edge(&undirected_graph, 3, 2);
    gphrx_add_edge(&undirected_graph, 3, 1);

    char *undir_adj_matrix_str = gphrx_csr_adj_matrix_to_string(&undirected_graph.adjacency_matrix);

    char *expected_str = "[ 0, 1, 1, 0 ]\r\n[ 1, 1, 1, 1 ]\r\n[ 1, 1, 0, 1 ]\r\n[ 0, 1, 1, 0 ]";
    assert(strcmp(undir_adj_matrix_str, expected_str) == 0, "Adjacency matrix string does not match expected");

    free_gphrx_byte_array(undir_adj_matrix_str);
    free_gphrx(&undirected_graph);

    return TEST_PASS;
}

static TEST_RESULT test_gphrx_shrink()
{
    GphrxGraph undirected_graph = new_undirected_gphrx();
    GphrxGraph directed_graph = new_directed_gphrx();

    dynarr_u64_expand(&undirected_graph.adjacency_matrix.col_indices, 500);
    dynarr_u64_expand(&undirected_graph.adjacency_matrix.row_indices, 300);
    assert(undirected_graph.adjacency_matrix.col_indices.capacity == 500, "Incorrect matrix cols size");
    assert(undirected_graph.adjacency_matrix.row_indices.capacity == 300, "Incorrect matrix rows size");

    dynarr_u64_expand(&directed_graph.adjacency_matrix.col_indices, 500);
    dynarr_u64_expand(&directed_graph.adjacency_matrix.row_indices, 300);
    assert(directed_graph.adjacency_matrix.col_indices.capacity == 500, "Incorrect matrix cols size");
    assert(directed_graph.adjacency_matrix.row_indices.capacity == 300, "Incorrect matrix rows size");

    gphrx_shrink(&undirected_graph);
    assert(undirected_graph.adjacency_matrix.col_indices.capacity == 0, "Incorrect matrix cols size");
    assert(undirected_graph.adjacency_matrix.row_indices.capacity == 0, "Incorrect matrix rows size");

    gphrx_shrink(&directed_graph);
    assert(directed_graph.adjacency_matrix.col_indices.capacity == 0, "Incorrect matrix cols size");
    assert(directed_graph.adjacency_matrix.row_indices.capacity == 0, "Incorrect matrix rows size");

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

    assert(undirected_graph.adjacency_matrix.dimension == 1002, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.col_indices.size == 16, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.row_indices.size == 16, "Incorrect adjacency matrix");
    
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 0) == 1,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 1) == 2,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 2) == 3,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 3) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 4) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 5) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 6) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 7) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 8) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 9) == 9,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 10) == 20,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 11) == 100,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 12) == 500,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 13) == 500,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 14) == 1000,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 15) == 1001,
           "Incorrect adjacency matrix");

    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 0) == 1000,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 1) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 2) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 3) == 2,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 4) == 3,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 5) == 9,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 6) == 20,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 7) == 100,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 8) == 500,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 9) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 10) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 11) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 12) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 13) == 1001,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 14) == 1,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 15) == 500,
           "Incorrect adjacency matrix");
    
    GphrxGraph directed_graph = new_directed_gphrx();
        
    gphrx_add_edge(&directed_graph, 1, 1000);
    gphrx_add_vertex(&directed_graph, 7, to_edges, 5);
    gphrx_add_edge(&directed_graph, 500, 1001);
    gphrx_add_edge(&directed_graph, 500, 7);

    assert(directed_graph.adjacency_matrix.dimension == 1002, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.col_indices.size == 8, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.row_indices.size == 8, "Incorrect adjacency matrix");
    
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.col_indices, 0) == 1,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.col_indices, 1) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.col_indices, 2) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.col_indices, 3) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.col_indices, 4) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.col_indices, 5) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.col_indices, 6) == 500,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.col_indices, 7) == 500,
           "Incorrect adjacency matrix");
  
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.row_indices, 0) == 1000,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.row_indices, 1) == 2,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.row_indices, 2) == 3,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.row_indices, 3) == 9,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.row_indices, 4) == 20,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.row_indices, 5) == 100,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.row_indices, 6) == 7,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.row_indices, 7) == 1001,
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

    assert(undirected_graph.adjacency_matrix.dimension == 1002, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.col_indices.size == 16, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.row_indices.size == 16, "Incorrect adjacency matrix");

    gphrx_remove_vertex(&undirected_graph, 7);

    assert(undirected_graph.adjacency_matrix.dimension == 1002, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.col_indices.size == 4, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.row_indices.size == 4, "Incorrect adjacency matrix");
    
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 0) == 1,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 1) == 500,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 2) == 1000,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 3) == 1001,
           "Incorrect adjacency matrix");

    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 0) == 1000,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 1) == 1001,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 2) == 1,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 3) == 500,
           "Incorrect adjacency matrix");
    
    gphrx_remove_vertex(&undirected_graph, 1000);
    
    assert(undirected_graph.adjacency_matrix.dimension == 1002, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.col_indices.size == 2, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.row_indices.size == 2, "Incorrect adjacency matrix");

    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 0) == 500,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 1) == 1001,
           "Incorrect adjacency matrix");

    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 0) == 1001,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 1) == 500,
           "Incorrect adjacency matrix");

    gphrx_remove_vertex(&undirected_graph, 1001);

    assert(undirected_graph.adjacency_matrix.dimension == 1001, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.col_indices.size == 0, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.row_indices.size == 0, "Incorrect adjacency matrix");
    
    GphrxGraph directed_graph = new_directed_gphrx();
        
    gphrx_add_edge(&directed_graph, 1, 1000);
    gphrx_add_vertex(&directed_graph, 7, to_edges, 5);
    gphrx_add_edge(&directed_graph, 500, 1001);
    gphrx_add_edge(&directed_graph, 500, 7);

    assert(directed_graph.adjacency_matrix.dimension == 1002, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.col_indices.size == 8, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.row_indices.size == 8, "Incorrect adjacency matrix");

    gphrx_remove_vertex(&directed_graph, 7);

    assert(directed_graph.adjacency_matrix.dimension == 1002, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.col_indices.size == 2, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.row_indices.size == 2, "Incorrect adjacency matrix");
    
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.col_indices, 0) == 1,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.col_indices, 1) == 500,
           "Incorrect adjacency matrix");

    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.row_indices, 0) == 1000,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.row_indices, 1) == 1001,
           "Incorrect adjacency matrix");

    gphrx_remove_vertex(&directed_graph, 1000);

    assert(directed_graph.adjacency_matrix.dimension == 1002, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.col_indices.size == 1, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.row_indices.size == 1, "Incorrect adjacency matrix");

    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.col_indices, 0) == 500,
           "Incorrect adjacency matrix");

    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.row_indices, 0) == 1001,
           "Incorrect adjacency matrix");

    gphrx_remove_vertex(&directed_graph, 500);
    
    assert(directed_graph.adjacency_matrix.dimension == 1002, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.col_indices.size == 0, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.row_indices.size == 0, "Incorrect adjacency matrix");

    gphrx_add_edge(&directed_graph, 1001, 500);
    gphrx_remove_vertex(&directed_graph, 1001);

    assert(directed_graph.adjacency_matrix.dimension == 1001, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.col_indices.size == 0, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.row_indices.size == 0, "Incorrect adjacency matrix");

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

    assert(undirected_graph.adjacency_matrix.dimension == 101, "Incorrect adjacency matrix dimension");
    assert(directed_graph.adjacency_matrix.dimension == 1002, "Incorrect adjacency matrix dimension");

    assert(undirected_graph.adjacency_matrix.col_indices.size == 2, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.row_indices.size == 2, "Incorrect adjacency matrix");

    assert(directed_graph.adjacency_matrix.col_indices.size == 1, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.row_indices.size == 1, "Incorrect adjacency matrix");

    // Try adding the same edges again and make sure the edge doesn't get added a second time
    gphrx_add_edge(&undirected_graph, 100, 5);
    gphrx_add_edge(&directed_graph, 9, 1001);

    // TODO: This fails because the rows in each column are not sorted
    assert(undirected_graph.adjacency_matrix.col_indices.size == 2, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.row_indices.size == 2, "Incorrect adjacency matrix");

    assert(directed_graph.adjacency_matrix.col_indices.size == 1, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.row_indices.size == 1, "Incorrect adjacency matrix");
    
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 0) == 5,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.col_indices, 1) == 100,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 0) == 100,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&undirected_graph.adjacency_matrix.row_indices, 1) == 5,
           "Incorrect adjacency matrix");
    
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.col_indices, 0) == 9,
           "Incorrect adjacency matrix");
    assert(dynarr_u64_get(&directed_graph.adjacency_matrix.row_indices, 0) == 1001,
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

    assert(undirected_graph.adjacency_matrix.col_indices.size == 2, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.row_indices.size == 2, "Incorrect adjacency matrix");

    error = gphrx_remove_edge(&undirected_graph, 100, 5);
    assert(error == GPHRX_NO_ERROR, "Edge not found");

    assert(undirected_graph.adjacency_matrix.col_indices.size == 0, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.row_indices.size == 0, "Incorrect adjacency matrix");
    
    gphrx_add_edge(&undirected_graph, 100, 5);

    assert(undirected_graph.adjacency_matrix.col_indices.size == 2, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.row_indices.size == 2, "Incorrect adjacency matrix");

    error = gphrx_remove_edge(&undirected_graph, 5, 100);
    assert(error == GPHRX_NO_ERROR, "Edge not found");

    assert(undirected_graph.adjacency_matrix.col_indices.size == 0, "Incorrect adjacency matrix");
    assert(undirected_graph.adjacency_matrix.row_indices.size == 0, "Incorrect adjacency matrix");

    GphrxGraph directed_graph = new_directed_gphrx();
    gphrx_add_edge(&directed_graph, 9, 1001);
        
    assert(directed_graph.adjacency_matrix.col_indices.size == 1, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.row_indices.size == 1, "Incorrect adjacency matrix");

    error = gphrx_remove_edge(&directed_graph, 9, 1001);
    assert(error == GPHRX_NO_ERROR, "Edge not found");

    assert(directed_graph.adjacency_matrix.col_indices.size == 0, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.row_indices.size == 0, "Incorrect adjacency matrix");

    gphrx_add_edge(&directed_graph, 9, 995);
    gphrx_add_edge(&directed_graph, 9, 996);
    gphrx_add_edge(&directed_graph, 9, 997);
    gphrx_add_edge(&directed_graph, 9, 998);
    gphrx_add_edge(&directed_graph, 9, 999);
    
    assert(directed_graph.adjacency_matrix.col_indices.size == 5, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.row_indices.size == 5, "Incorrect adjacency matrix");

    error = gphrx_remove_edge(&directed_graph, 9, 998);
    assert(error == GPHRX_NO_ERROR, "Edge not found");

    assert(directed_graph.adjacency_matrix.col_indices.size == 4, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.row_indices.size == 4, "Incorrect adjacency matrix");

    bool found_val_before = false;
    bool found_val = false;
    bool found_val_after = false;
    for (size_t i = 0; i < directed_graph.adjacency_matrix.row_indices.size; ++i)
    {
        if (dynarr_u64_get(&directed_graph.adjacency_matrix.row_indices, i) == 998)
            found_val = true;
        if (dynarr_u64_get(&directed_graph.adjacency_matrix.row_indices, i) == 997)
            found_val_before = true;
        if (dynarr_u64_get(&directed_graph.adjacency_matrix.row_indices, i) == 999)
            found_val_after = true;
    }

    assert(found_val_before && found_val_after && !found_val, "Incorrect vertex was removed");

    error = gphrx_remove_edge(&directed_graph, 9, 996);
    assert(error == GPHRX_NO_ERROR, "Edge not found");

    error = gphrx_remove_edge(&directed_graph, 9, 938);
    assert(error == GPHRX_ERROR_NOT_FOUND, "Edge found that does not exist");

    assert(directed_graph.adjacency_matrix.col_indices.size == 3, "Incorrect adjacency matrix");
    assert(directed_graph.adjacency_matrix.row_indices.size == 3, "Incorrect adjacency matrix");

    found_val_before = false;
    found_val = false;
    found_val_after = false;
    for (size_t i = 0; i < directed_graph.adjacency_matrix.row_indices.size; ++i)
    {
        if (dynarr_u64_get(&directed_graph.adjacency_matrix.row_indices, i) == 996)
            found_val = true;
        if (dynarr_u64_get(&directed_graph.adjacency_matrix.row_indices, i) == 995)
            found_val_before = true;
        if (dynarr_u64_get(&directed_graph.adjacency_matrix.row_indices, i) == 997)
            found_val_after = true;
    }

    assert(found_val_before && found_val_after && !found_val, "Incorrect vertex was removed");

    free_gphrx(&undirected_graph);
    free_gphrx(&directed_graph);
    
    return TEST_PASS;
}


static TEST_RESULT test_gphrx_find_occurrence_matrix()
{
    u64 to_edges_1[] = {0, 2, 4, 7, 3};
    u64 to_edges_5[] = {6, 8, 0, 1, 5, 4, 2};
    
    GphrxGraph undirected_graph = new_directed_gphrx();
    
    gphrx_add_edge(&undirected_graph, 7, 8);
    gphrx_add_vertex(&undirected_graph, 1, to_edges_1, 5);
    gphrx_add_vertex(&undirected_graph, 5, to_edges_5, 7);
    
    GphrxCsrMatrix occurrence_matrix = gphrx_find_occurrence_matrix(&undirected_graph, 5);

    assert(occurrence_matrix.entries.size == 4, "Incorrect occurrence matrix");
    assert(occurrence_matrix.col_indices.size == 4, "Incorrect occurrence matrix");
    assert(occurrence_matrix.row_indices.size == 4, "Incorrect occurrence matrix");
    
    assert(dynarr_u64_get(&occurrence_matrix.col_indices, 0) == 0, "Incorrect occurrence matrix");
    assert(dynarr_u64_get(&occurrence_matrix.col_indices, 1) == 0, "Incorrect occurrence matrix");
    assert(dynarr_u64_get(&occurrence_matrix.col_indices, 2) == 1, "Incorrect occurrence matrix");
    assert(dynarr_u64_get(&occurrence_matrix.col_indices, 3) == 1, "Incorrect occurrence matrix");

    assert(dynarr_u64_get(&occurrence_matrix.row_indices, 0) == 0, "Incorrect occurrence matrix");
    assert(dynarr_u64_get(&occurrence_matrix.row_indices, 1) == 1, "Incorrect occurrence matrix");
    assert(dynarr_u64_get(&occurrence_matrix.row_indices, 2) == 0, "Incorrect occurrence matrix");
    assert(dynarr_u64_get(&occurrence_matrix.row_indices, 3) == 1, "Incorrect occurrence matrix");

    assert((int) (dynarr_dbl_get(&occurrence_matrix.entries, 0) * 100) == 16, "Incorrect occurrence matrix");
    assert((int) (dynarr_dbl_get(&occurrence_matrix.entries, 1) * 100) == 4, "Incorrect occurrence matrix");
    assert((int) (dynarr_dbl_get(&occurrence_matrix.entries, 2) * 100) == 16, "Incorrect occurrence matrix");
    assert((int) (dynarr_dbl_get(&occurrence_matrix.entries, 3) * 100) == 16, "Incorrect occurrence matrix");
    
    free_gphrx_csr_matrix(&occurrence_matrix);
    free_gphrx(&undirected_graph);
    
    GphrxGraph directed_graph = new_directed_gphrx();

    gphrx_add_edge(&directed_graph, 0, 1);
    gphrx_add_edge(&directed_graph, 1, 1);
    gphrx_add_edge(&directed_graph, 2, 1);
    gphrx_add_edge(&directed_graph, 2, 0);
    gphrx_add_edge(&directed_graph, 3, 2);
    gphrx_add_edge(&directed_graph, 3, 1);
    gphrx_add_edge(&directed_graph, 3, 4);
    gphrx_add_edge(&directed_graph, 0, 6);
    gphrx_add_edge(&directed_graph, 6, 6);
    
    occurrence_matrix = gphrx_find_occurrence_matrix(&directed_graph, 3);
        
    assert(occurrence_matrix.entries.size == 5, "Incorrect occurrence matrix");
    assert(occurrence_matrix.col_indices.size == 5, "Incorrect occurrence matrix");
    assert(occurrence_matrix.row_indices.size == 5, "Incorrect occurrence matrix");
    
    assert(dynarr_u64_get(&occurrence_matrix.col_indices, 0) == 0, "Incorrect occurrence matrix");
    assert(dynarr_u64_get(&occurrence_matrix.col_indices, 1) == 0, "Incorrect occurrence matrix");
    assert(dynarr_u64_get(&occurrence_matrix.col_indices, 2) == 1, "Incorrect occurrence matrix");
    assert(dynarr_u64_get(&occurrence_matrix.col_indices, 3) == 1, "Incorrect occurrence matrix");
    assert(dynarr_u64_get(&occurrence_matrix.col_indices, 4) == 2, "Incorrect occurrence matrix");

    assert(dynarr_u64_get(&occurrence_matrix.row_indices, 0) == 0, "Incorrect occurrence matrix");
    assert(dynarr_u64_get(&occurrence_matrix.row_indices, 1) == 2, "Incorrect occurrence matrix");
    assert(dynarr_u64_get(&occurrence_matrix.row_indices, 2) == 0, "Incorrect occurrence matrix");
    assert(dynarr_u64_get(&occurrence_matrix.row_indices, 3) == 1, "Incorrect occurrence matrix");
    assert(dynarr_u64_get(&occurrence_matrix.row_indices, 4) == 2, "Incorrect occurrence matrix");

    assert((int) (dynarr_dbl_get(&occurrence_matrix.entries, 0) * 100) == 44, "Incorrect occurrence matrix");
    assert((int) (dynarr_dbl_get(&occurrence_matrix.entries, 1) * 100) == 11, "Incorrect occurrence matrix");
    assert((int) (dynarr_dbl_get(&occurrence_matrix.entries, 2) * 100) == 22, "Incorrect occurrence matrix");
    assert((int) (dynarr_dbl_get(&occurrence_matrix.entries, 3) * 100) == 11, "Incorrect occurrence matrix");
    assert((int) (dynarr_dbl_get(&occurrence_matrix.entries, 4) * 100) == 11, "Incorrect occurrence matrix");

    free_gphrx_csr_matrix(&occurrence_matrix);
    free_gphrx(&directed_graph);

    return TEST_PASS;
}

static TEST_RESULT test_approximate_gphrx()
{
    u64 to_edges_1[] = {0, 2, 4, 7, 3};
    u64 to_edges_5[] = {6, 8, 0, 1, 5, 4, 2};
    
    GphrxGraph undirected_graph = new_undirected_gphrx();

    gphrx_add_edge(&undirected_graph, 7, 8);
    gphrx_add_vertex(&undirected_graph, 1, to_edges_1, 5);
    gphrx_add_vertex(&undirected_graph, 5, to_edges_5, 7);

    printf("Dimension: %llu\n\n", undirected_graph.adjacency_matrix.dimension);

    printf("%s\n\n", gphrx_csr_adj_matrix_to_string(&undirected_graph.adjacency_matrix));

    // This approximation doesn't seem quite right. The edges in the bottom corner are beign
    // incorrectly set to 0.50 in the occurrence matrix when they should only be 0.25
    GphrxGraph approx_graph1 = approximate_gphrx(&undirected_graph, 2, 0.3);

    // The occurence matrix here is also making the nodes at the bottom corner of the adjacency
    // matrix to be more than they are. They should be 2/16 = 1/8 = 0.125, but they are instead
    // listed as 0.19
    GphrxGraph approx_graph2 = approximate_gphrx(&undirected_graph, 4, 0.1);

    printf("%s\n\n", gphrx_csr_adj_matrix_to_string(&approx_graph1.adjacency_matrix));
    
    GphrxCsrMatrix occurrence_matrix = gphrx_find_occurrence_matrix(&undirected_graph, 2);
    char *occurrence_matrix_str = gphrx_csr_matrix_to_string(&occurrence_matrix, 2);

    printf("%s\n\n", occurrence_matrix_str);

    occurrence_matrix = gphrx_find_occurrence_matrix(&undirected_graph, 4);
    occurrence_matrix_str = gphrx_csr_matrix_to_string(&occurrence_matrix, 2);

    printf("%s\n\n", occurrence_matrix_str);

    printf("%s\n\n", gphrx_csr_adj_matrix_to_string(&approx_graph2.adjacency_matrix));
           
    free_gphrx(&undirected_graph);

    // TODO: Test again with directed graphs
    
    return TEST_FAIL;
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

    assert(undir_graph_from_arr.adjacency_matrix.dimension == undirected_graph.adjacency_matrix.dimension,
           "Incorrectly loaded graph");

    assert(undir_graph_from_arr.adjacency_matrix.col_indices.size ==
           undirected_graph.adjacency_matrix.col_indices.size,
           "Incorrectly loaded graph adjacency matrix");

    assert(undir_graph_from_arr.adjacency_matrix.row_indices.size ==
           undirected_graph.adjacency_matrix.row_indices.size,
           "Incorrectly loaded graph adjacency matrix");

    for (size_t i = 0; i < undirected_graph.adjacency_matrix.col_indices.size; ++i)
    {
        assert(undirected_graph.adjacency_matrix.col_indices.arr[i] ==
               undir_graph_from_arr.adjacency_matrix.col_indices.arr[i],
               "Incorrectly loaded graph adjacency matrix");

        assert(undirected_graph.adjacency_matrix.row_indices.arr[i] ==
               undir_graph_from_arr.adjacency_matrix.row_indices.arr[i],
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

    assert(dir_graph_from_arr.adjacency_matrix.dimension == directed_graph.adjacency_matrix.dimension,
           "Incorrectly loaded graph");

    assert(dir_graph_from_arr.adjacency_matrix.col_indices.size ==
           directed_graph.adjacency_matrix.col_indices.size,
           "Incorrectly loaded graph adjacency matrix");

    assert(dir_graph_from_arr.adjacency_matrix.row_indices.size ==
           directed_graph.adjacency_matrix.row_indices.size,
           "Incorrectly loaded graph adjacency matrix");

    for (size_t i = 0; i < directed_graph.adjacency_matrix.col_indices.size; ++i)
    {
        assert(directed_graph.adjacency_matrix.col_indices.arr[i] ==
               dir_graph_from_arr.adjacency_matrix.col_indices.arr[i],
               "Incorrectly loaded graph adjacency matrix");

        assert(directed_graph.adjacency_matrix.row_indices.arr[i] ==
               dir_graph_from_arr.adjacency_matrix.row_indices.arr[i],
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
    register_test(&set, test_free_gphrx_csr_matrix);
    register_test(&set, test_free_gphrx_csr_adj_matrix);
    register_test(&set, test_gphrx_csr_matrix_to_string);
    register_test(&set, test_gphrx_csr_adj_matrix_to_string);
    register_test(&set, test_gphrx_shrink);
    register_test(&set, test_gphrx_does_edge_exist);
    register_test(&set, test_gphrx_add_vertex);
    register_test(&set, test_gphrx_remove_vertex);
    register_test(&set, test_gphrx_add_edge);
    register_test(&set, test_gphrx_remove_edge);
    register_test(&set, test_gphrx_find_occurrence_matrix);
    register_test(&set, test_approximate_gphrx);
    register_test(&set, test_gphrx_to_from_byte_array);

    return set;
}

#endif
 
