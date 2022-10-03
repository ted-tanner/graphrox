#ifndef __DYN_ARRAY_H

#include <stddef.h>
#include <string.h>

#include "assert.h"
#include "intrinsics.h"

typedef struct {
    size_t capacity;
    size_t size;
    u64 *arr;
} DynamicArrayU64;

#define new_dynarr_u64(type) _create_dynarr_u64(1)
#define new_dynarr_u64_with_size(start_capacity, type) _create_dynarr_u64(start_capacity, sizeof(type))
#define free_dynarr_u64(arr_ptr) free((arr_ptr)->arr)

#define dynarr_u64_push(arr_ptr, item) _dynarr_u64_push_at((arr_ptr), item, (arr_ptr)->size)
#define dynarr_u64_push_at(arr_ptr, item, idx) _dynarr_u64_push_at((arr_ptr), item, (idx))

#define dynarr_u64_get(arr_ptr, pos) ((arr_ptr)->arr[pos])
#define dynarr_u64_get_ptr(arr_ptr, pos) ((arr_ptr)->arr + (pos))
#define dynarr_u64_pop(arr_ptr) ((arr_ptr)->arr[--((arr_ptr)->size)])

void dynarr_u64_shrink(DynamicArrayU64 *arr);
void dynarr_u64_expand(DynamicArrayU64 *arr, size_t desired_capacity);
void dynarr_u64_expand_and_zero(DynamicArrayU64 *arr, size_t desired_size);

void dynarr_u64_push_multiple(DynamicArrayU64 *arr, u64 *item_arr, size_t count);
void dynarr_u64_remove_at(DynamicArrayU64 *arr, size_t idx);

DynamicArrayU64 _create_dynarr_u64(size_t start_capacity);
void _dynarr_u64_push_at(DynamicArrayU64 *arr, u64 item, size_t idx);

#define __DYN_ARRAY_H
#endif
