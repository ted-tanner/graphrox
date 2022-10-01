#ifndef __DYN_ARRAY_H

#include <stddef.h>
#include <string.h>

#include "assert.h"
#include "intrinsics.h"

typedef struct _dynarr {
    size_t capacity;
    size_t size;
    size_t element_size;
    byte *arr;
} DynamicArray;

#define new_dynarr(type) _create_dynarr(1, sizeof(type))
#define new_dynarr_with_size(start_capacity, type) _create_dynarr(start_capacity, sizeof(type))
#define free_dynarr(arr_ptr) free((arr_ptr)->arr)

#define dynarr_push(arr_ptr, item) _dynarr_push_at((arr_ptr), &(item), (arr_ptr)->size)
#define dynarr_push_at(arr_ptr, item, idx) _dynarr_push_at((arr_ptr), &(item), (idx))

#define dynarr_get(arr_ptr, pos, type) (*((type*) ((arr_ptr)->arr + (pos) * (arr_ptr)->element_size)))
#define dynarr_get_ptr(arr_ptr, pos, type) ((type*) ((arr_ptr)->arr + (pos) * (arr_ptr)->element_size))
#define dynarr_pop(arr_ptr, type) (*((type*) ((arr_ptr)->arr + (arr_ptr)->size-- + (arr_ptr)->element_size)))

void dynarr_shrink(DynamicArray *arr);
void dynarr_expand(DynamicArray *arr, size_t desired_capacity);
void dynarr_expand_and_zero(DynamicArray *arr, size_t desired_size);

void dynarr_push_multiple(DynamicArray *arr, void *item_arr, size_t count);

DynamicArray _create_dynarr(size_t start_capacity, size_t element_size);
void _dynarr_push_at(DynamicArray *arr, void *item_ptr, size_t idx);

#define __DYN_ARRAY_H
#endif
