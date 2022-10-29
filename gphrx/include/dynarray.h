#ifndef __DYN_ARRAY_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "intrinsics.h"

typedef union {
    u64 u64_val;
    double dbl_val;
} Byte8Val;

typedef struct {
    size_t capacity;
    size_t size;
    Byte8Val *arr;
} DynamicArray8;

/** Byte8Val */
#define new_dynarr8() new_dynarr8_with_capacity(1)
#define free_dynarr8(arr_ptr) free((arr_ptr)->arr)

#define dynarr8_push(arr_ptr, item) _dynarr8_push_at((arr_ptr), item, (arr_ptr)->size)
#define dynarr8_push_at(arr_ptr, item, idx) _dynarr8_push_at((arr_ptr), item, (idx))

#define dynarr8_get(arr_ptr, pos) ((arr_ptr)->arr[pos])
#define dynarr8_get_ptr(arr_ptr, pos) ((arr_ptr)->arr + (pos))
#define dynarr8_pop(arr_ptr) ((arr_ptr)->arr[--((arr_ptr)->size)])

DynamicArray8 new_dynarr8_with_capacity(size_t start_capacity);

void dynarr8_shrink(DynamicArray8 *arr);
void dynarr8_expand(DynamicArray8 *arr, size_t desired_capacity);
void dynarr8_grow_and_zero(DynamicArray8 *arr, size_t desired_size);

void dynarr8_push_multiple(DynamicArray8 *arr, Byte8Val *item_arr, size_t count);
void dynarr8_remove_at(DynamicArray8 *arr, size_t idx);
void dynarr8_remove_multiple_at(DynamicArray8 *arr, size_t start_idx, size_t count);

void _dynarr8_push_at(DynamicArray8 *arr, Byte8Val item, size_t idx);

#ifdef TEST_MODE

#include "test.h"

ModuleTestSet dynarray_h_register_tests();

#endif


#define __DYN_ARRAY_H
#endif
