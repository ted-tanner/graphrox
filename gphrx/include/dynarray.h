#ifndef __DYN_ARRAY_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "intrinsics.h"

// 8-byte value array
typedef union {
    u64 u64_val;
    double double_val;
} byteval8;

typedef struct {
    size_t capacity;
    size_t size;
    byteval8 *arr;
} DynamicArray8;

// 16-byte value array
typedef struct {
    u64 u64_val;
    double double_val;
} Tuple_u64_double;

typedef union {
    Tuple_u64_double tuple_val;
} byteval16;

typedef struct {
    size_t capacity;
    size_t size;
    byteval16 *arr;
} DynamicArray16;

/** 8-byte value array */
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

void dynarr8_push_multiple(DynamicArray8 *arr, byteval8 *item_arr, size_t count);
void dynarr8_remove_at(DynamicArray8 *arr, size_t idx);
void dynarr8_remove_multiple_at(DynamicArray8 *arr, size_t start_idx, size_t count);

void _dynarr8_push_at(DynamicArray8 *arr, byteval8 item, size_t idx);

/** 16-byte value array */
#define new_dynarr16() new_dynarr16_with_capacity(1)
#define free_dynarr16(arr_ptr) free((arr_ptr)->arr)

#define dynarr16_push(arr_ptr, item) _dynarr16_push_at((arr_ptr), item, (arr_ptr)->size)
#define dynarr16_push_at(arr_ptr, item, idx) _dynarr16_push_at((arr_ptr), item, (idx))

#define dynarr16_get(arr_ptr, pos) ((arr_ptr)->arr[pos])
#define dynarr16_get_ptr(arr_ptr, pos) ((arr_ptr)->arr + (pos))
#define dynarr16_pop(arr_ptr) ((arr_ptr)->arr[--((arr_ptr)->size)])

DynamicArray16 new_dynarr16_with_capacity(size_t start_capacity);

void dynarr16_shrink(DynamicArray16 *arr);
void dynarr16_expand(DynamicArray16 *arr, size_t desired_capacity);
void dynarr16_grow_and_zero(DynamicArray16 *arr, size_t desired_size);

void dynarr16_push_multiple(DynamicArray16 *arr, byteval16 *item_arr, size_t count);
void dynarr16_remove_at(DynamicArray16 *arr, size_t idx);
void dynarr16_remove_multiple_at(DynamicArray16 *arr, size_t start_idx, size_t count);

void _dynarr16_push_at(DynamicArray16 *arr, byteval16 item, size_t idx);

#ifdef TEST_MODE

#include "test.h"

ModuleTestSet dynarray_h_register_tests();

#endif


#define __DYN_ARRAY_H
#endif
