#include "dynarray.h"

void dynarr_u64_shrink(DynamicArrayU64 *arr)
{
    size_t new_capacity = arr->size;
    u64 *new_arr = realloc(arr->arr, new_capacity * sizeof(64));

    assert(new_arr != 0, "realloc failue");
    
    arr->arr = new_arr;
    arr->capacity = new_capacity;
}

void dynarr_u64_expand(DynamicArrayU64 *arr, size_t desired_capacity)
{
    if (desired_capacity <= arr->capacity)
        return;
    
    size_t new_capacity = arr->capacity;
    for (; new_capacity < desired_capacity; new_capacity *= 2);

    u64 *new_arr = realloc(arr->arr, new_capacity * sizeof(u64));
    
    assert(new_arr != 0, "realloc failue");
    
    arr->arr = new_arr;
    arr->capacity = new_capacity;
}

void dynarr_u64_expand_and_zero(DynamicArrayU64* arr, size_t desired_size)
{
    if (desired_size <= arr->size)
        return;
    
    dynarr_u64_expand(arr, desired_size);
    
    size_t delta = desired_size - arr->size;
    memset((void*) (arr->arr + arr->size), 0, delta);
    
    arr->size = desired_size;
}

void dynarr_u64_push_multiple(DynamicArrayU64 *arr, u64 *item_arr, size_t count)
{
    if (arr->size + count >= arr->capacity)
    {
        size_t new_capacity = arr->capacity * 2;
        for(; new_capacity < arr->size + count; new_capacity *= 2);

        u64 *new_arr = realloc(arr->arr, new_capacity * sizeof(u64));

        assert(new_arr != 0, "realloc failue");

        arr->arr = new_arr;
        arr->capacity = new_capacity;
    }

    memcpy((void*) (arr->arr + arr->size), item_arr, count * sizeof(u64));
    arr->size += count;
}

void dynarr_u64_remove_at(DynamicArrayU64 *arr, size_t idx)
{
    memmove((void*) (arr->arr + idx),
            (void*) (arr->arr + idx + 1),
            (arr->size - idx - 1) * sizeof(u64));
    
    --arr->size;
}

DynamicArrayU64 _create_dynarr_u64(size_t start_capacity)
{
    u64 *arr = malloc(sizeof(u64) * start_capacity);

    assert(arr != 0, "malloc failure");

    DynamicArrayU64 vec = {
        .capacity = start_capacity,
        .size = 0,
        .arr = arr,
    };
    
    return vec;
}

void _dynarr_u64_push_at(DynamicArrayU64 *arr, u64 item, size_t idx)
{
    if (arr->size == arr->capacity)
    {
        u64 *new_arr = realloc(arr->arr, arr->capacity * sizeof(u64) * 2);

        assert(new_arr != 0, "realloc failue");

        arr->arr = new_arr;
        arr->capacity *= 2;
    }

    u64 *location = arr->arr + idx;
    
    if (idx != arr->size)
        memmove((void*) location + 1, (void*) location, (arr->size - idx) * sizeof(u64));

    arr->arr[idx] = item;
    ++arr->size;
}
