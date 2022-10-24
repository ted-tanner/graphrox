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
} DynamicArray_u64;

typedef struct {
    u64 u64_val;
    double double_val;
} Tuple_u64_double;

typedef struct {
    size_t capacity;
    size_t size;
    Tuple_u64_double *arr;
} DynamicArray_u64_double;

typedef union {
    DynamicArray_u64 u64_arr;
    DynamicArray_u64_double tuple_arr;
} TableEntryData;

typedef struct {
    u64 key;
    TableEntryData val;
} TableEntry;

typedef struct {
    size_t capacity;
    size_t size;
    TableEntry *arr;
} DynamicArray_TableEntry;

/** u64 */
#define new_dynarr_u64() new_dynarr_u64_with_capacity(1)
#define free_dynarr_u64(arr_ptr) free((arr_ptr)->arr)

#define dynarr_u64_push(arr_ptr, item) _dynarr_u64_push_at((arr_ptr), item, (arr_ptr)->size)
#define dynarr_u64_push_at(arr_ptr, item, idx) _dynarr_u64_push_at((arr_ptr), item, (idx))

#define dynarr_u64_get(arr_ptr, pos) ((arr_ptr)->arr[pos])
#define dynarr_u64_get_ptr(arr_ptr, pos) ((arr_ptr)->arr + (pos))
#define dynarr_u64_pop(arr_ptr) ((arr_ptr)->arr[--((arr_ptr)->size)])

DynamicArray_u64 new_dynarr_u64_with_capacity(size_t start_capacity);

void dynarr_u64_shrink(DynamicArray_u64 *arr);
void dynarr_u64_expand(DynamicArray_u64 *arr, size_t desired_capacity);
void dynarr_u64_grow_and_zero(DynamicArray_u64 *arr, size_t desired_size);

void dynarr_u64_push_multiple(DynamicArray_u64 *arr, u64 *item_arr, size_t count);
void dynarr_u64_remove_at(DynamicArray_u64 *arr, size_t idx);
void dynarr_u64_remove_multiple_at(DynamicArray_u64 *arr, size_t start_idx, size_t count);

void _dynarr_u64_push_at(DynamicArray_u64 *arr, u64 item, size_t idx);

/** u64-double tuple */
#define new_dynarr_u64_dbl() new_dynarr_u64_dbl_with_capacity(1)
#define free_dynarr_u64_dbl(arr_ptr) free((arr_ptr)->arr)

#define dynarr_u64_dbl_push(arr_ptr, item) _dynarr_u64_dbl_push_at((arr_ptr), item, (arr_ptr)->size)
#define dynarr_u64_dbl_push_at(arr_ptr, item, idx) _dynarr_u64_dbl_push_at((arr_ptr), item, (idx))

#define dynarr_u64_dbl_get(arr_ptr, pos) ((arr_ptr)->arr[pos])
#define dynarr_u64_dbl_get_ptr(arr_ptr, pos) ((arr_ptr)->arr + (pos))
#define dynarr_u64_dbl_pop(arr_ptr) ((arr_ptr)->arr[--((arr_ptr)->size)])

DynamicArray_u64_double new_dynarr_u64_dbl_with_capacity(size_t start_capacity);

void dynarr_u64_dbl_shrink(DynamicArray_u64_double *arr);
void dynarr_u64_dbl_expand(DynamicArray_u64_double *arr, size_t desired_capacity);
void dynarr_u64_dbl_grow_and_zero(DynamicArray_u64_double *arr, size_t desired_size);

void dynarr_u64_dbl_push_multiple(DynamicArray_u64_double *arr, Tuple_u64_double *item_arr, size_t count);
void dynarr_u64_dbl_remove_at(DynamicArray_u64_double *arr, size_t idx);
void dynarr_u64_dbl_remove_multiple_at(DynamicArray_u64_double *arr, size_t start_idx, size_t count);

void _dynarr_u64_dbl_push_at(DynamicArray_u64_double *arr, Tuple_u64_double item, size_t idx);

/** u64-double tuple */
#define new_dynarr_table_entry() new_dynarr_u64_dbl_with_capacity(1)
#define free_dynarr_table_entry(arr_ptr) free((arr_ptr)->arr)

#define dynarr_table_entry_push(arr_ptr, item) _dynarr_table_entry_push_at((arr_ptr), item, (arr_ptr)->size)
#define dynarr_table_entry_push_at(arr_ptr, item, idx) _dynarr_table_entry_push_at((arr_ptr), item, (idx))

#define dynarr_table_entry_get(arr_ptr, pos) ((arr_ptr)->arr[pos])
#define dynarr_table_entry_get_ptr(arr_ptr, pos) ((arr_ptr)->arr + (pos))
#define dynarr_table_entry_pop(arr_ptr) ((arr_ptr)->arr[--((arr_ptr)->size)])

DynamicArray_TableEntry new_dynarr_table_entry_with_capacity(size_t start_capacity);

void dynarr_table_entry_shrink(DynamicArray_TableEntry *arr);
void dynarr_table_entry_expand(DynamicArray_TableEntry *arr, size_t desired_capacity);
void dynarr_table_entry_grow_and_zero(DynamicArray_TableEntry *arr, size_t desired_size);

void dynarr_table_entry_push_multiple(DynamicArray_TableEntry *arr, TableEntry_u64_arr *item_arr, size_t count);
void dynarr_table_entry_remove_at(DynamicArray_TableEntry *arr, size_t idx);
void dynarr_table_entry_remove_multiple_at(DynamicArray_TableEntry *arr, size_t start_idx, size_t count);

void _dynarr_table_entry_push_at(DynamicArray_TableEntry *arr, TableEntry_u64_arr item, size_t idx);


#ifdef TEST_MODE

#include "test.h"

ModuleTestSet dynarray_h_register_tests();

#endif


#define __DYN_ARRAY_H
#endif
