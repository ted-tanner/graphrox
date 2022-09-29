#include "dynarray.h"

void dynarr_shrink(DynArray *arr)
{
    size_t new_capacity = arr->size * arr->element_size;
    byte *new_arr = realloc(arr->arr, new_capacity);

    assert(new_arr != 0, "realloc failue");
    
    arr->arr = new_arr;
    arr->capacity = new_capacity;
}

void dynarr_push_multiple(DynArray *arr, void *item_arr, size_t count)
{
    size_t item_arr_size = count * arr->element_size;

    if (arr->size + count >= arr->capacity)
    {
        size_t new_capacity = arr->capacity * 2;

        while (new_capacity < arr->size + count)
        {
            new_capacity *= 2;
        }

        byte *new_arr = realloc(arr->arr, new_capacity * arr->element_size);

        assert(new_arr != 0, "realloc failue");

        arr->arr = new_arr;
        arr->capacity = new_capacity;
    }

    memcpy(arr->arr + arr->size * arr->element_size, item_arr, item_arr_size);
    arr->size += count;
}

DynArray _create_dynarr(size_t start_capacity, size_t element_size)
{
    byte *arr = malloc(element_size * start_capacity);

    assert(arr != 0, "malloc failure");

    DynArray vec = {
        .capacity = start_capacity,
        .size = 0,
        .element_size = element_size,
        .arr = arr,
    };
    
    return vec;
}

void _dynarr_push(DynArray *arr, void *item)
{
    if (arr->size == arr->capacity)
    {
        byte *new_arr = realloc(arr->arr, arr->capacity * arr->element_size * 2);

        assert(new_arr != 0, "realloc failue");

        arr->arr = new_arr;
        arr->capacity *= 2;
    }

    memcpy(arr->arr + arr->size * arr->element_size, item, arr->element_size);
    ++arr->size;
}
