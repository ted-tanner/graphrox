#include "dynarray.h"

void dynarr_shrink(DynamicArray *arr)
{
    size_t new_capacity = arr->size * arr->element_size;
    byte *new_arr = realloc(arr->arr, new_capacity);

    assert(new_arr != 0, "realloc failue");
    
    arr->arr = new_arr;
    arr->capacity = new_capacity;
}

void dynarr_expand(DynamicArray *arr, size_t desired_capacity)
{
    if (desired_capacity <= arr->capacity)
        return;
    
    size_t new_capacity = arr->capacity;
    for (; new_capacity < desired_capacity; new_capacity *= 2);

    byte *new_arr = realloc(arr->arr, new_capacity * arr->element_size);
    
    assert(new_arr != 0, "realloc failue");
    
    arr->arr = new_arr;
    arr->capacity = new_capacity;
}

void dynarr_expand_and_zero(DynamicArray* arr, size_t desired_size)
{
    if (desired_size <= arr->size)
        return;
    
    dynarr_expand(arr, desired_size);
    
    size_t delta = desired_size - arr->size;
    memset(arr->arr + (arr->size * arr->element_size), 0, delta * arr->element_size);
    
    arr->size = desired_size;
}

void dynarr_push_multiple(DynamicArray *arr, void *item_arr, size_t count)
{
    size_t item_arr_size = count * arr->element_size;

    if (arr->size + count >= arr->capacity)
    {
        size_t new_capacity = arr->capacity * 2;
        for(; new_capacity < arr->size + count; new_capacity *= 2);

        byte *new_arr = realloc(arr->arr, new_capacity * arr->element_size);

        assert(new_arr != 0, "realloc failue");

        arr->arr = new_arr;
        arr->capacity = new_capacity;
    }

    memcpy(arr->arr + arr->size * arr->element_size, item_arr, item_arr_size);
    arr->size += count;
}

void dynarr_remove_at(DynamicArray *arr, size_t idx)
{
    size_t real_idx = idx * arr->element_size;
    
    memmove(arr->arr + real_idx,
            arr->arr + real_idx + arr->element_size,
            (arr->size - idx - 1) * arr->element_size);
    
    --arr->size;
}

DynamicArray _create_dynarr(size_t start_capacity, size_t element_size)
{
    byte *arr = malloc(element_size * start_capacity);

    assert(arr != 0, "malloc failure");

    DynamicArray vec = {
        .capacity = start_capacity,
        .size = 0,
        .element_size = element_size,
        .arr = arr,
    };
    
    return vec;
}

void _dynarr_push_at(DynamicArray *arr, void *item_ptr, size_t idx)
{
    if (arr->size == arr->capacity)
    {
        byte *new_arr = realloc(arr->arr, arr->capacity * arr->element_size * 2);

        assert(new_arr != 0, "realloc failue");

        arr->arr = new_arr;
        arr->capacity *= 2;
    }

    size_t real_idx = idx * arr->element_size;
    byte *location = arr->arr + real_idx;
    
    if (idx != arr->size)
        memmove(location + arr->element_size, location, (arr->size - idx) * arr->element_size);

    memcpy(arr->arr + real_idx, item_ptr, arr->element_size);
    ++arr->size;
}
