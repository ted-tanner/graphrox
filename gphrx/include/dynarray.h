#ifndef __DYN_ARRAY_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "assert.h"
#include "intrinsics.h"

typedef struct _dynarr {
    size_t capacity;
    size_t size;
    size_t element_size;
    byte* arr;
} DynArray;

#define new_dynarr(start_capacity, type) _create_dynarr(start_capacity, sizeof(type))
#define free_dynarr(arr_ptr) free((arr_ptr)->arr)
#define dynarr_push(arr_ptr, item) _dynarr_push((arr_ptr), &(item))
#define dynarr_get(arr_ptr, pos, type) (*((type*) ((arr_ptr)->arr + (pos) * (arr_ptr)->element_size)))
#define dynarr_get_ptr(arr_ptr, pos, type) ((type*) ((arr_ptr)->arr + (pos) * (arr_ptr)->element_size))
#define dynarr_pop(arr_ptr, type) (*((type*) ((arr_ptr)->arr + (arr_ptr)->size-- + (arr_ptr)->element_size)))

void dynarr_shrink(DynArray* arr);
void dynarr_push_multiple(DynArray* arr, void* item_arr, size_t count);

DynArray _create_dynarr(size_t start_capacity, size_t element_size);
void _dynarr_push(DynArray* arr, void* item_ptr);

#define __DYN_ARRAY_H
#endif
