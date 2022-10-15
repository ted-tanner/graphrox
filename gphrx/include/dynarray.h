#ifndef __DYN_ARRAY_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "intrinsics.h"

typedef struct {
    size_t capacity;
    size_t size;
    u64 *arr;
} DynamicArrayU64;

typedef struct {
    size_t capacity;
    size_t size;
    double *arr;
} DynamicArrayDouble;

/** u64 */
#define new_dynarr_u64() new_dynarr_u64_with_capacity(1)
#define free_dynarr_u64(arr_ptr) free((arr_ptr)->arr)

#define dynarr_u64_push(arr_ptr, item) _dynarr_u64_push_at((arr_ptr), item, (arr_ptr)->size)
#define dynarr_u64_push_at(arr_ptr, item, idx) _dynarr_u64_push_at((arr_ptr), item, (idx))

#define dynarr_u64_get(arr_ptr, pos) ((arr_ptr)->arr[pos])
#define dynarr_u64_get_ptr(arr_ptr, pos) ((arr_ptr)->arr + (pos))
#define dynarr_u64_pop(arr_ptr) ((arr_ptr)->arr[--((arr_ptr)->size)])

DynamicArrayU64 new_dynarr_u64_with_capacity(size_t start_capacity);

void dynarr_u64_shrink(DynamicArrayU64 *arr);
void dynarr_u64_expand(DynamicArrayU64 *arr, size_t desired_capacity);
void dynarr_u64_grow_and_zero(DynamicArrayU64 *arr, size_t desired_size);

void dynarr_u64_push_multiple(DynamicArrayU64 *arr, u64 *item_arr, size_t count);
void dynarr_u64_remove_at(DynamicArrayU64 *arr, size_t idx);
void dynarr_u64_remove_multiple_at(DynamicArrayU64 *arr, size_t start_idx, size_t count);

void _dynarr_u64_push_at(DynamicArrayU64 *arr, u64 item, size_t idx);

/** double */
#define new_dynarr_dbl() new_dynarr_dbl_with_capacity(1)
#define free_dynarr_dbl(arr_ptr) free((arr_ptr)->arr)

#define dynarr_dbl_push(arr_ptr, item) _dynarr_dbl_push_at((arr_ptr), item, (arr_ptr)->size)
#define dynarr_dbl_push_at(arr_ptr, item, idx) _dynarr_dbl_push_at((arr_ptr), item, (idx))

#define dynarr_dbl_get(arr_ptr, pos) ((arr_ptr)->arr[pos])
#define dynarr_dbl_get_ptr(arr_ptr, pos) ((arr_ptr)->arr + (pos))
#define dynarr_dbl_pop(arr_ptr) ((arr_ptr)->arr[--((arr_ptr)->size)])

DynamicArrayDouble new_dynarr_dbl_with_capacity(size_t start_capacity);

void dynarr_dbl_shrink(DynamicArrayDouble *arr);
void dynarr_dbl_expand(DynamicArrayDouble *arr, size_t desired_capacity);
void dynarr_dbl_grow_and_zero(DynamicArrayDouble *arr, size_t desired_size);

void dynarr_dbl_push_multiple(DynamicArrayDouble *arr, double *item_arr, size_t count);
void dynarr_dbl_remove_at(DynamicArrayDouble *arr, size_t idx);
void dynarr_dbl_remove_multiple_at(DynamicArrayDouble *arr, size_t start_idx, size_t count);

void _dynarr_dbl_push_at(DynamicArrayDouble *arr, double item, size_t idx);


#ifdef TEST_MODE

#include "test.h"

ModuleTestSet dynarray_h_register_tests();

#endif


#define __DYN_ARRAY_H
#endif
