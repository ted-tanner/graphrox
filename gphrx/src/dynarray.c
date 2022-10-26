#include "dynarray.h"

DynamicArray8 new_dynarr8_with_capacity(size_t start_capacity)
{
    byteval8 *arr = malloc(sizeof(byteval8) * start_capacity);

    assert(arr != 0, "malloc failure");

    DynamicArray8 vec = {
        .capacity = start_capacity,
        .size = 0,
        .arr = arr,
    };
    
    return vec;
}

void dynarr8_shrink(DynamicArray8 *arr)
{
    size_t new_capacity = arr->size;
    byteval8 *new_arr = realloc(arr->arr, new_capacity * sizeof(byteval8));
    
    assert(new_arr != 0, "realloc failue");
    
    arr->arr = new_arr;
    arr->capacity = new_capacity;
}

void dynarr8_expand(DynamicArray8 *arr, size_t desired_capacity)
{
    if (desired_capacity <= arr->capacity)
        return;
    
    byteval8 *new_arr = realloc(arr->arr, desired_capacity * sizeof(byteval8));
    
    assert(new_arr != 0, "realloc failue");
    
    arr->arr = new_arr;
    arr->capacity = desired_capacity;
}

void dynarr8_grow_and_zero(DynamicArray8* arr, size_t desired_size)
{
    if (desired_size <= arr->size)
        return;
    
    dynarr8_expand(arr, desired_size);
    
    size_t delta = desired_size - arr->size;
    memset(arr->arr + arr->size, 0, delta);
    
    arr->size = desired_size;
}

void dynarr8_push_multiple(DynamicArray8 *arr, byteval8 *item_arr, size_t count)
{
    if (arr->size + count >= arr->capacity)
    {
        size_t new_capacity = arr->capacity * 2;
        for(; new_capacity < arr->size + count; new_capacity *= 2);

        byteval8 *new_arr = realloc(arr->arr, new_capacity * sizeof(byteval8));

        assert(new_arr != 0, "realloc failue");

        arr->arr = new_arr;
        arr->capacity = new_capacity;
    }

    memcpy(arr->arr + arr->size, item_arr, count * sizeof(byteval8));
    arr->size += count;
}

void dynarr8_remove_at(DynamicArray8 *arr, size_t idx)
{
    assert(arr->size > idx, "Invalid array index");
    memmove(arr->arr + idx, arr->arr + idx + 1, (arr->size - idx - 1) * sizeof(byteval8));
    --arr->size;
}

void dynarr8_remove_multiple_at(DynamicArray8 *arr, size_t start_idx, size_t count)
{
    assert(arr->size >= start_idx + count, "Invalid array index or count");
    memmove(arr->arr + start_idx,
            arr->arr + start_idx + count,
            (arr->size - start_idx - count) * sizeof(byteval8));
    arr->size -= count;
}

void _dynarr8_push_at(DynamicArray8 *arr, byteval8 item, size_t idx)
{
    assert(arr->size >= idx, "Invalid array index");
    
    if (arr->size == arr->capacity)
    {
        byteval8 *new_arr = realloc(arr->arr, arr->capacity * sizeof(byteval8) * 2);

        assert(new_arr != 0, "realloc failue");

        arr->arr = new_arr;
        arr->capacity *= 2;
    }

    byteval8 *location = arr->arr + idx;
    
    if (idx != arr->size)
        memmove(location + 1, location, (arr->size - idx) * sizeof(byteval8));

    arr->arr[idx] = item;
    ++arr->size;
}

DynamicArray16 new_dynarr16_with_capacity(size_t start_capacity)
{
    byteval16 *arr = malloc(sizeof(byteval16) * start_capacity);

    assert(arr != 0, "malloc failure");

    DynamicArray16 vec = {
        .capacity = start_capacity,
        .size = 0,
        .arr = arr,
    };
    
    return vec;
}

void dynarr16_shrink(DynamicArray16 *arr)
{
    size_t new_capacity = arr->size;
    byteval16 *new_arr = realloc(arr->arr, new_capacity * sizeof(byteval16));
    
    assert(new_arr != 0, "realloc failue");
    
    arr->arr = new_arr;
    arr->capacity = new_capacity;
}

void dynarr16_expand(DynamicArray16 *arr, size_t desired_capacity)
{
    if (desired_capacity <= arr->capacity)
        return;
    
    byteval16 *new_arr = realloc(arr->arr, desired_capacity * sizeof(byteval16));
    
    assert(new_arr != 0, "realloc failue");
    
    arr->arr = new_arr;
    arr->capacity = desired_capacity;
}

void dynarr16_grow_and_zero(DynamicArray16* arr, size_t desired_size)
{
    if (desired_size <= arr->size)
        return;
    
    dynarr16_expand(arr, desired_size);
    
    size_t delta = desired_size - arr->size;
    memset(arr->arr + arr->size, 0, delta);
    
    arr->size = desired_size;
}

void dynarr16_push_multiple(DynamicArray16 *arr, byteval16 *item_arr, size_t count)
{
    if (arr->size + count >= arr->capacity)
    {
        size_t new_capacity = arr->capacity * 2;
        for(; new_capacity < arr->size + count; new_capacity *= 2);

        byteval16 *new_arr = realloc(arr->arr, new_capacity * sizeof(byteval16));

        assert(new_arr != 0, "realloc failue");

        arr->arr = new_arr;
        arr->capacity = new_capacity;
    }

    memcpy(arr->arr + arr->size, item_arr, count * sizeof(byteval16));
    arr->size += count;
}

void dynarr16_remove_at(DynamicArray16 *arr, size_t idx)
{
    assert(arr->size > idx, "Invalid array index");
    memmove(arr->arr + idx, arr->arr + idx + 1, (arr->size - idx - 1) * sizeof(byteval16));
    --arr->size;
}

void dynarr16_remove_multiple_at(DynamicArray16 *arr, size_t start_idx, size_t count)
{
    assert(arr->size >= start_idx + count, "Invalid array index or count");
    memmove(arr->arr + start_idx,
            arr->arr + start_idx + count,
            (arr->size - start_idx - count) * sizeof(byteval16));
    arr->size -= count;
}

void _dynarr16_push_at(DynamicArray16 *arr, byteval16 item, size_t idx)
{
    assert(arr->size >= idx, "Invalid array index");
    
    if (arr->size == arr->capacity)
    {
        byteval16 *new_arr = realloc(arr->arr, arr->capacity * sizeof(byteval16) * 2);

        assert(new_arr != 0, "realloc failue");

        arr->arr = new_arr;
        arr->capacity *= 2;
    }

    byteval16 *location = arr->arr + idx;
    
    if (idx != arr->size)
        memmove(location + 1, location, (arr->size - idx) * sizeof(byteval16));

    arr->arr[idx] = item;
    ++arr->size;
}

DynamicArray_TableEntry new_dynarr_table_entry_with_capacity(size_t start_capacity)
{
    double *arr = malloc(sizeof(TableEntry) * start_capacity);

    assert(arr != 0, "malloc failure");

    DynamicArray_TableEntry vec = {
        .capacity = start_capacity,
        .size = 0,
        .arr = arr,
    };
    
    return vec;
}

void dynarr_table_entry_shrink(DynamicArray_TableEntry *arr)
{
    size_t new_capacity = arr->size;
    TableEntry *new_arr = realloc(arr->arr, new_capacity * sizeof(TableEntry));

    assert(new_arr != 0, "realloc failue");
    
    arr->arr = new_arr;
    arr->capacity = new_capacity;
}

void dynarr_table_entry_expand(DynamicArray_TableEntry *arr, size_t desired_capacity)
{
    if (desired_capacity <= arr->capacity)
        return;
    
    TableEntry *new_arr = realloc(arr->arr, desired_capacity * sizeof(TableEntry));
    
    assert(new_arr != 0, "realloc failue");
    
    arr->arr = new_arr;
    arr->capacity = desired_capacity;
}

void dynarr_table_entry_grow_and_zero(DynamicArray_TableEntry* arr, size_t desired_size)
{
    if (desired_size <= arr->size)
        return;
    
    dynarr_table_entry_expand(arr, desired_size);
    
    size_t delta = desired_size - arr->size;
    memset(arr->arr + arr->size, 0, delta);
    
    arr->size = desired_size;
}

void dynarr_table_entry_push_multiple(DynamicArray_TableEntry *arr, TableEntry *item_arr, size_t count)
{
    if (arr->size + count >= arr->capacity)
    {
        size_t new_capacity = arr->capacity * 2;
        for(; new_capacity < arr->size + count; new_capacity *= 2);

        TableEntry *new_arr = realloc(arr->arr, new_capacity * sizeof(TableEntry));

        assert(new_arr != 0, "realloc failue");

        arr->arr = new_arr;
        arr->capacity = new_capacity;
    }

    memcpy(arr->arr + arr->size, item_arr, count * sizeof(TableEntry));
    arr->size += count;
}

void dynarr_table_entry_remove_at(DynamicArray_TableEntry *arr, size_t idx)
{
    assert(arr->size > idx, "Invalid array index");
    memmove(arr->arr + idx, arr->arr + idx + 1, (arr->size - idx - 1) * sizeof(TableEntry));
    --arr->size;
}

void dynarr_table_entry_remove_multiple_at(DynamicArray_TableEntry *arr, size_t start_idx, size_t count)
{
    assert(arr->size >= start_idx + count, "Invalid array index or count");
    memmove(arr->arr + start_idx,
            arr->arr + start_idx + count,
            (arr->size - start_idx - count) * sizeof(TableEntry));
    arr->size -= count;
}

void _dynarr_table_entry_push_at(DynamicArray_TableEntry *arr, TableEntry item, size_t idx)
{
    assert(arr->size >= idx, "Invalid array index");
    
    if (arr->size == arr->capacity)
    {
        TableEntry *new_arr = realloc(arr->arr, arr->capacity * sizeof(TableEntry) * 2);

        assert(new_arr != 0, "realloc failue");

        arr->arr = new_arr;
        arr->capacity *= 2;
    }

    TableEntry *location = arr->arr + idx;
    
    if (idx != arr->size)
        memmove(location + 1, location, (arr->size - idx) * sizeof(TableEntry));

    arr->arr[idx] = item;
    ++arr->size;
}

#ifdef TEST_MODE

static TEST_RESULT test_new_dynarr8() {
    DynamicArray8 arr = new_dynarr8();

    assert(arr.capacity == 1, "Incorrect array capacity");
    assert(arr.size == 0, "Incorrect array size");
    assert(arr.arr != 0, "Null array pointer");

    // This size will exceed page size on practically all systems, so segmentation
    // faults for stepping out of bounds are possible
    const size_t arr2_capacity = 32769;
    DynamicArray8 arr2 = new_dynarr8_with_capacity(arr2_capacity);

    assert(arr2.capacity == arr2_capacity, "Incorrect array capacity");
    assert(arr2.size == 0, "Incorrect array size");
    assert(arr2.arr != 0, "Null array pointer");

    // Check no segmentation fault
    for (byteval8 i = 0; i < arr2_capacity; ++i)
        dynarr8_push(&arr2, i);

    assert(arr2.capacity == arr2_capacity, "Incorrect array capacity");
    assert(arr2.size == arr2_capacity, "Incorrect array size");

    free_dynarr8(&arr);
    free_dynarr8(&arr2);

    return TEST_PASS;
}

static TEST_RESULT test_free_dynarr8() {
    const size_t arr_capacity = 32769;
    DynamicArray8 arr = new_dynarr8_with_capacity(arr_capacity);

    // Just make sure this causes no errors
    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_push() {
    DynamicArray8 arr = new_dynarr8();

    dynarr8_push(&arr, 42);
    dynarr8_push(&arr, 67);
    dynarr8_push(&arr, 1);

    assert(arr.arr[0] == 42, "Incorrect value in array");
    assert(arr.arr[1] == 67, "Incorrect value in array");
    assert(arr.arr[2] == 1, "Incorrect value in array");

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_push_at() {
    DynamicArray8 arr = new_dynarr8();

    dynarr8_push(&arr, 41);
    dynarr8_push(&arr, 66);
    dynarr8_push(&arr, 2);

    dynarr8_push_at(&arr, 3, 1);

    assert(arr.arr[0] == 41, "Incorrect value in array");
    assert(arr.arr[1] == 3, "Incorrect value in array");
    assert(arr.arr[2] == 66, "Incorrect value in array");
    assert(arr.arr[3] == 2, "Incorrect value in array");

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_get() {
    DynamicArray8 arr = new_dynarr8();

    dynarr8_push(&arr, 43);
    dynarr8_push(&arr, 68);
    dynarr8_push(&arr, 4);

    assert(dynarr8_get(&arr, 0) == 43, "Incorrect value in array");
    assert(dynarr8_get(&arr, 1) == 68, "Incorrect value in array");
    assert(dynarr8_get(&arr, 2) == 4, "Incorrect value in array");

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_get_ptr() {
    DynamicArray8 arr = new_dynarr8();

    dynarr8_push(&arr, 44);
    dynarr8_push(&arr, 69);
    dynarr8_push(&arr, 5);

    assert(*(dynarr8_get_ptr(&arr, 0)) == 44, "Incorrect pointer to value in array");
    assert(*(dynarr8_get_ptr(&arr, 1)) == 69, "Incorrect pointer to value in array");
    assert(*(dynarr8_get_ptr(&arr, 2)) == 5, "Incorrect pointer to value in array");

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_pop() {
    DynamicArray8 arr = new_dynarr8();

    dynarr8_push(&arr, 100);
    dynarr8_push(&arr, 101);
    dynarr8_push(&arr, 102);

    assert(arr.size == 3, "Incorrect array size");

    assert(dynarr8_pop(&arr) == 102, "Incorrect value in array");
    assert(arr.size == 2, "Incorrect array size");

    assert(dynarr8_pop(&arr) == 101, "Incorrect value in array");
    assert(arr.size == 1, "Incorrect array size");
    
    assert(dynarr8_pop(&arr) == 100, "Incorrect value in array");
    assert(arr.size == 0, "Incorrect array size");

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_shrink() {
    DynamicArray8 arr = new_dynarr8_with_capacity(25);

    assert(arr.size == 0, "Incorrect array size");
    assert(arr.capacity == 25, "Incorrect array capacity");

    dynarr8_push(&arr, 100);
    dynarr8_push(&arr, 101);
    dynarr8_push(&arr, 102);

    assert(arr.size == 3, "Incorrect array size");
    assert(arr.capacity == 25, "Incorrect array capacity");

    dynarr8_shrink(&arr);

    assert(arr.size == 3, "Incorrect array size");
    assert(arr.capacity == 3, "Incorrect array capacity");

    dynarr8_pop(&arr);

    assert(arr.size == 2, "Incorrect array size");
    assert(arr.capacity == 3, "Incorrect array capacity");

    dynarr8_shrink(&arr);

    assert(arr.size == 2, "Incorrect array size");
    assert(arr.capacity == 2, "Incorrect array capacity");

    free_dynarr8(&arr);
    
    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_expand() {
    const size_t arr_capacity = 1000;
    DynamicArray8 arr = new_dynarr8_with_capacity(arr_capacity);

    assert(arr.capacity == arr_capacity, "Incorrect array capacity");
    assert(arr.size == 0, "Incorrect array size");

    const size_t arr_new_capacity = 32769;
    dynarr8_expand(&arr, arr_new_capacity);

    assert(arr.capacity == arr_new_capacity, "Incorrect array capacity");
    assert(arr.size == 0, "Incorrect array size");

    // Check no segmentation fault
    for (byteval8 i = 0; i < arr_new_capacity; ++i)
        dynarr8_push(&arr, i);

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_grow_and_zero() {
    const size_t arr_capacity = 2000;
    DynamicArray8 arr = new_dynarr8_with_capacity(arr_capacity);
    memset(arr.arr, 1, arr_capacity * sizeof(byteval8));

    dynarr8_shrink(&arr);

    const size_t arr_new_capacity = 32769;
    dynarr8_grow_and_zero(&arr, arr_new_capacity);
    
    assert(arr.capacity == arr_new_capacity, "Incorrect array capacity");
    assert(arr.size == arr_new_capacity, "Incorrect array size");

    assert(arr.arr[arr_capacity - 5] == 0, "Array was not correctly zeroed");

    // Check no segmentation fault
    for (byteval8 i = 0; i < arr_new_capacity; ++i)
        dynarr8_push(&arr, i);

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_push_multiple() {
    DynamicArray8 arr = new_dynarr8();

    dynarr8_push(&arr, 41);
    dynarr8_push(&arr, 66);

    assert(arr.size == 2, "Incorrect array size");

    byteval8 second_array[] = {98, 95, 93, 90};
    dynarr8_push_multiple(&arr, second_array, 4);

    assert(arr.size == 6, "Incorrect array size");

    assert(dynarr8_get(&arr, 0) == 41, "Incorrect value in array");
    assert(dynarr8_get(&arr, 1) == 66, "Incorrect value in array");
    assert(dynarr8_get(&arr, 2) == second_array[0], "Incorrect value in array");
    assert(dynarr8_get(&arr, 3) == second_array[1], "Incorrect value in array");
    assert(dynarr8_get(&arr, 4) == second_array[2], "Incorrect value in array");
    assert(dynarr8_get(&arr, 5) == second_array[3], "Incorrect value in array");

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_remove_at() {
    DynamicArray8 arr = new_dynarr8();

    dynarr8_push(&arr, 41);
    dynarr8_push(&arr, 66);
    dynarr8_push(&arr, 2);
    dynarr8_push(&arr, 3);
    dynarr8_push(&arr, 4);

    assert(arr.size == 5, "Incorrect array size");

    dynarr8_remove_at(&arr, 1); // Remove 66
    dynarr8_remove_at(&arr, 2); // Remove 4 (its now at index 2 because 66 was removed)

    assert(arr.size == 3, "Incorrect array size");

    assert(arr.arr[0] == 41, "Incorrect value in array");
    assert(arr.arr[1] == 2, "Incorrect value in array");
    assert(arr.arr[2] == 4, "Incorrect value in array");

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_remove_multiple_at() {
    DynamicArray8 arr = new_dynarr8();

    dynarr8_push(&arr, 0);
    dynarr8_push(&arr, 1);
    dynarr8_push(&arr, 2);
    dynarr8_push(&arr, 3);
    dynarr8_push(&arr, 4);
    dynarr8_push(&arr, 5);
    dynarr8_push(&arr, 6);
    dynarr8_push(&arr, 7);
    dynarr8_push(&arr, 8);
    dynarr8_push(&arr, 9);

    assert(arr.size == 10, "Incorrect array size");

    dynarr8_remove_multiple_at(&arr, 3, 4);

    assert(arr.size == 6, "Incorrect array size");

    assert(arr.arr[0] == 0, "Incorrect value in array");
    assert(arr.arr[1] == 1, "Incorrect value in array");
    assert(arr.arr[2] == 2, "Incorrect value in array");
    assert(arr.arr[3] == 7, "Incorrect value in array");
    assert(arr.arr[4] == 8, "Incorrect value in array");
    assert(arr.arr[5] == 9, "Incorrect value in array");

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_new_dynarr16() {
    DynamicArray16 arr = new_dynarr16();

    assert(arr.capacity == 1, "Incorrect array capacity");
    assert(arr.size == 0, "Incorrect array size");
    assert(arr.arr != 0, "Null array pointer");

    // This size will exceed page size on practically all systems, so segmentation
    // faults for stepping out of bounds are possible
    const size_t arr2_capacity = 32769;
    DynamicArray16 arr2 = new_dynarr16_with_capacity(arr2_capacity);

    assert(arr2.capacity == arr2_capacity, "Incorrect array capacity");
    assert(arr2.size == 0, "Incorrect array size");
    assert(arr2.arr != 0, "Null array pointer");

    // Check no segmentation fault
    for (size_t i = 0.0; i < arr2_capacity; ++i)
        dynarr16_push(&arr2, i + 0.25);

    assert(arr2.capacity == arr2_capacity, "Incorrect array capacity");
    assert(arr2.size == arr2_capacity, "Incorrect array size");

    free_dynarr16(&arr);
    free_dynarr16(&arr2);

    return TEST_PASS;
}

static TEST_RESULT test_free_dynarr16() {
    const size_t arr_capacity = 32769;
    DynamicArray16 arr = new_dynarr16_with_capacity(arr_capacity);

    // Just make sure this causes no errors
    free_dynarr16(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr16_push() {
    DynamicArray16 arr = new_dynarr16();

    dynarr16_push(&arr, 42.1);
    dynarr16_push(&arr, 67.1);
    dynarr16_push(&arr, 1.1);

    assert(arr.arr[0] == 42.1, "Incorrect value in array");
    assert(arr.arr[1] == 67.1, "Incorrect value in array");
    assert(arr.arr[2] == 1.1, "Incorrect value in array");

    free_dynarr16(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr16_push_at() {
    DynamicArray16 arr = new_dynarr16();

    dynarr16_push(&arr, 41.1);
    dynarr16_push(&arr, 66.1);
    dynarr16_push(&arr, 2.1);

    dynarr16_push_at(&arr, 3.1, 1);

    assert(arr.arr[0] == 41.1, "Incorrect value in array");
    assert(arr.arr[1] == 3.1, "Incorrect value in array");
    assert(arr.arr[2] == 66.1, "Incorrect value in array");
    assert(arr.arr[3] == 2.1, "Incorrect value in array");

    free_dynarr16(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr16_get() {
    DynamicArray16 arr = new_dynarr16();

    dynarr16_push(&arr, 43.1);
    dynarr16_push(&arr, 68.1);
    dynarr16_push(&arr, 4.1);

    assert(dynarr16_get(&arr, 0) == 43.1, "Incorrect value in array");
    assert(dynarr16_get(&arr, 1) == 68.1, "Incorrect value in array");
    assert(dynarr16_get(&arr, 2) == 4.1, "Incorrect value in array");

    free_dynarr16(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr16_get_ptr() {
    DynamicArray16 arr = new_dynarr16();

    dynarr16_push(&arr, 44.1);
    dynarr16_push(&arr, 69.1);
    dynarr16_push(&arr, 5.2);

    assert(*(dynarr16_get_ptr(&arr, 0)) == 44.1, "Incorrect pointer to value in array");
    assert(*(dynarr16_get_ptr(&arr, 1)) == 69.1, "Incorrect pointer to value in array");
    assert(*(dynarr16_get_ptr(&arr, 2)) == 5.2, "Incorrect pointer to value in array");

    free_dynarr16(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr16_pop() {
    DynamicArray16 arr = new_dynarr16();

    dynarr16_push(&arr, 100.1);
    dynarr16_push(&arr, 101.1);
    dynarr16_push(&arr, 102.1);

    assert(arr.size == 3, "Incorrect array size");

    assert(dynarr16_pop(&arr) == 102.1, "Incorrect value in array");
    assert(arr.size == 2, "Incorrect array size");

    assert(dynarr16_pop(&arr) == 101.1, "Incorrect value in array");
    assert(arr.size == 1, "Incorrect array size");
    
    assert(dynarr16_pop(&arr) == 100.1, "Incorrect value in array");
    assert(arr.size == 0, "Incorrect array size");

    free_dynarr16(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr16_shrink() {
    DynamicArray16 arr = new_dynarr16_with_capacity(25);

    assert(arr.size == 0, "Incorrect array size");
    assert(arr.capacity == 25, "Incorrect array capacity");

    dynarr16_push(&arr, 100.1);
    dynarr16_push(&arr, 101.1);
    dynarr16_push(&arr, 102.1);

    assert(arr.size == 3, "Incorrect array size");
    assert(arr.capacity == 25, "Incorrect array capacity");

    dynarr16_shrink(&arr);

    assert(arr.size == 3, "Incorrect array size");
    assert(arr.capacity == 3, "Incorrect array capacity");

    dynarr16_pop(&arr);

    assert(arr.size == 2, "Incorrect array size");
    assert(arr.capacity == 3, "Incorrect array capacity");

    dynarr16_shrink(&arr);

    assert(arr.size == 2, "Incorrect array size");
    assert(arr.capacity == 2, "Incorrect array capacity");

    free_dynarr16(&arr);
    
    return TEST_PASS;
}

static TEST_RESULT test_dynarr16_expand() {
    const size_t arr_capacity = 1000;
    DynamicArray16 arr = new_dynarr16_with_capacity(arr_capacity);

    assert(arr.capacity == arr_capacity, "Incorrect array capacity");
    assert(arr.size == 0, "Incorrect array size");

    const size_t arr_new_capacity = 32769;
    dynarr16_expand(&arr, arr_new_capacity);

    assert(arr.capacity == arr_new_capacity, "Incorrect array capacity");
    assert(arr.size == 0, "Incorrect array size");

    // Check no segmentation fault
    for (size_t i = 0; i < arr_new_capacity; ++i)
        dynarr16_push(&arr, i + 0.25);

    free_dynarr16(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr16_grow_and_zero() {
    const size_t arr_capacity = 2000;
    DynamicArray16 arr = new_dynarr16_with_capacity(arr_capacity);
    memset(arr.arr, 1, arr_capacity * sizeof(byteval16));

    dynarr16_shrink(&arr);

    const size_t arr_new_capacity = 32769;
    dynarr16_grow_and_zero(&arr, arr_new_capacity);
    
    assert(arr.capacity == arr_new_capacity, "Incorrect array capacity");
    assert(arr.size == arr_new_capacity, "Incorrect array size");

    assert(arr.arr[arr_capacity - 5] == 0, "Array was not correctly zeroed");

    // Check no segmentation fault
    for (size_t i = 0; i < arr_new_capacity; ++i)
        dynarr16_push(&arr, i + 0.25);

    free_dynarr16(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr16_push_multiple() {
    DynamicArray16 arr = new_dynarr16();

    dynarr16_push(&arr, 41.1);
    dynarr16_push(&arr, 66.1);

    assert(arr.size == 2, "Incorrect array size");

    double second_array[] = {98.1, 95.1, 93.1, 90.1};
    dynarr16_push_multiple(&arr, second_array, 4);

    assert(arr.size == 6, "Incorrect array size");

    assert(dynarr16_get(&arr, 0) == 41.1, "Incorrect value in array");
    assert(dynarr16_get(&arr, 1) == 66.1, "Incorrect value in array");
    assert(dynarr16_get(&arr, 2) == second_array[0], "Incorrect value in array");
    assert(dynarr16_get(&arr, 3) == second_array[1], "Incorrect value in array");
    assert(dynarr16_get(&arr, 4) == second_array[2], "Incorrect value in array");
    assert(dynarr16_get(&arr, 5) == second_array[3], "Incorrect value in array");

    free_dynarr16(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr16_remove_at() {
    DynamicArray16 arr = new_dynarr16();

    dynarr16_push(&arr, 41.1);
    dynarr16_push(&arr, 66.1);
    dynarr16_push(&arr, 2.1);
    dynarr16_push(&arr, 3.1);
    dynarr16_push(&arr, 4.1);

    assert(arr.size == 5, "Incorrect array size");

    dynarr16_remove_at(&arr, 1); // Remove 66
    dynarr16_remove_at(&arr, 2); // Remove 4 (its now at index 2 because 66 was removed)

    assert(arr.size == 3, "Incorrect array size");

    assert(arr.arr[0] == 41.1, "Incorrect value in array");
    assert(arr.arr[1] == 2.1, "Incorrect value in array");
    assert(arr.arr[2] == 4.1, "Incorrect value in array");

    free_dynarr16(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr16_remove_multiple_at() {
    DynamicArray16 arr = new_dynarr16();

    dynarr16_push(&arr, 0.1);
    dynarr16_push(&arr, 1.1);
    dynarr16_push(&arr, 2.1);
    dynarr16_push(&arr, 3.1);
    dynarr16_push(&arr, 4.1);
    dynarr16_push(&arr, 5.1);
    dynarr16_push(&arr, 6.1);
    dynarr16_push(&arr, 7.1);
    dynarr16_push(&arr, 8.1);
    dynarr16_push(&arr, 9.1);

    assert(arr.size == 10, "Incorrect array size");

    dynarr16_remove_multiple_at(&arr, 3, 4);

    assert(arr.size == 6, "Incorrect array size");

    assert(arr.arr[0] == 0.1, "Incorrect value in array");
    assert(arr.arr[1] == 1.1, "Incorrect value in array");
    assert(arr.arr[2] == 2.1, "Incorrect value in array");
    assert(arr.arr[3] == 7.1, "Incorrect value in array");
    assert(arr.arr[4] == 8.1, "Incorrect value in array");
    assert(arr.arr[5] == 9.1, "Incorrect value in array");

    free_dynarr16(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_new_dynarr_table_entry() {
    DynamicArray_TableEntry arr = new_dynarr_table_entry();

    assert(arr.capacity == 1, "Incorrect array capacity");
    assert(arr.size == 0, "Incorrect array size");
    assert(arr.arr != 0, "Null array pointer");

    // This size will exceed page size on practically all systems, so segmentation
    // faults for stepping out of bounds are possible
    const size_t arr2_capacity = 32769;
    DynamicArray_TableEntry arr2 = new_dynarr_table_entry_with_capacity(arr2_capacity);

    assert(arr2.capacity == arr2_capacity, "Incorrect array capacity");
    assert(arr2.size == 0, "Incorrect array size");
    assert(arr2.arr != 0, "Null array pointer");

    // Check no segmentation fault
    for (size_t i = 0.0; i < arr2_capacity; ++i)
        dynarr_table_entry_push(&arr2, i + 0.25);

    assert(arr2.capacity == arr2_capacity, "Incorrect array capacity");
    assert(arr2.size == arr2_capacity, "Incorrect array size");

    free_dynarr_table_entry(&arr);
    free_dynarr_table_entry(&arr2);

    return TEST_PASS;
}

static TEST_RESULT test_free_dynarr_table_entry() {
    const size_t arr_capacity = 32769;
    DynamicArray_TableEntry arr = new_dynarr_table_entry_with_capacity(arr_capacity);

    // Just make sure this causes no errors
    free_dynarr_table_entry(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_table_entry_push() {
    DynamicArray_TableEntry arr = new_dynarr_table_entry();

    dynarr_table_entry_push(&arr, 42.1);
    dynarr_table_entry_push(&arr, 67.1);
    dynarr_table_entry_push(&arr, 1.1);

    assert(arr.arr[0] == 42.1, "Incorrect value in array");
    assert(arr.arr[1] == 67.1, "Incorrect value in array");
    assert(arr.arr[2] == 1.1, "Incorrect value in array");

    free_dynarr_table_entry(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_table_entry_push_at() {
    DynamicArray_TableEntry arr = new_dynarr_table_entry();

    dynarr_table_entry_push(&arr, 41.1);
    dynarr_table_entry_push(&arr, 66.1);
    dynarr_table_entry_push(&arr, 2.1);

    dynarr_table_entry_push_at(&arr, 3.1, 1);

    assert(arr.arr[0] == 41.1, "Incorrect value in array");
    assert(arr.arr[1] == 3.1, "Incorrect value in array");
    assert(arr.arr[2] == 66.1, "Incorrect value in array");
    assert(arr.arr[3] == 2.1, "Incorrect value in array");

    free_dynarr_table_entry(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_table_entry_get() {
    DynamicArray_TableEntry arr = new_dynarr_table_entry();

    dynarr_table_entry_push(&arr, 43.1);
    dynarr_table_entry_push(&arr, 68.1);
    dynarr_table_entry_push(&arr, 4.1);

    assert(dynarr_table_entry_get(&arr, 0) == 43.1, "Incorrect value in array");
    assert(dynarr_table_entry_get(&arr, 1) == 68.1, "Incorrect value in array");
    assert(dynarr_table_entry_get(&arr, 2) == 4.1, "Incorrect value in array");

    free_dynarr_table_entry(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_table_entry_get_ptr() {
    DynamicArray_TableEntry arr = new_dynarr_table_entry();

    dynarr_table_entry_push(&arr, 44.1);
    dynarr_table_entry_push(&arr, 69.1);
    dynarr_table_entry_push(&arr, 5.2);

    assert(*(dynarr_table_entry_get_ptr(&arr, 0)) == 44.1, "Incorrect pointer to value in array");
    assert(*(dynarr_table_entry_get_ptr(&arr, 1)) == 69.1, "Incorrect pointer to value in array");
    assert(*(dynarr_table_entry_get_ptr(&arr, 2)) == 5.2, "Incorrect pointer to value in array");

    free_dynarr_table_entry(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_table_entry_pop() {
    DynamicArray_TableEntry arr = new_dynarr_table_entry();

    dynarr_table_entry_push(&arr, 100.1);
    dynarr_table_entry_push(&arr, 101.1);
    dynarr_table_entry_push(&arr, 102.1);

    assert(arr.size == 3, "Incorrect array size");

    assert(dynarr_table_entry_pop(&arr) == 102.1, "Incorrect value in array");
    assert(arr.size == 2, "Incorrect array size");

    assert(dynarr_table_entry_pop(&arr) == 101.1, "Incorrect value in array");
    assert(arr.size == 1, "Incorrect array size");
    
    assert(dynarr_table_entry_pop(&arr) == 100.1, "Incorrect value in array");
    assert(arr.size == 0, "Incorrect array size");

    free_dynarr_table_entry(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_table_entry_shrink() {
    DynamicArray_TableEntry arr = new_dynarr_table_entry_with_capacity(25);

    assert(arr.size == 0, "Incorrect array size");
    assert(arr.capacity == 25, "Incorrect array capacity");

    dynarr_table_entry_push(&arr, 100.1);
    dynarr_table_entry_push(&arr, 101.1);
    dynarr_table_entry_push(&arr, 102.1);

    assert(arr.size == 3, "Incorrect array size");
    assert(arr.capacity == 25, "Incorrect array capacity");

    dynarr_table_entry_shrink(&arr);

    assert(arr.size == 3, "Incorrect array size");
    assert(arr.capacity == 3, "Incorrect array capacity");

    dynarr_table_entry_pop(&arr);

    assert(arr.size == 2, "Incorrect array size");
    assert(arr.capacity == 3, "Incorrect array capacity");

    dynarr_table_entry_shrink(&arr);

    assert(arr.size == 2, "Incorrect array size");
    assert(arr.capacity == 2, "Incorrect array capacity");

    free_dynarr_table_entry(&arr);
    
    return TEST_PASS;
}

static TEST_RESULT test_dynarr_table_entry_expand() {
    const size_t arr_capacity = 1000;
    DynamicArray_TableEntry arr = new_dynarr_table_entry_with_capacity(arr_capacity);

    assert(arr.capacity == arr_capacity, "Incorrect array capacity");
    assert(arr.size == 0, "Incorrect array size");

    const size_t arr_new_capacity = 32769;
    dynarr_table_entry_expand(&arr, arr_new_capacity);

    assert(arr.capacity == arr_new_capacity, "Incorrect array capacity");
    assert(arr.size == 0, "Incorrect array size");

    // Check no segmentation fault
    for (size_t i = 0; i < arr_new_capacity; ++i)
        dynarr_table_entry_push(&arr, i + 0.25);

    free_dynarr_table_entry(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_table_entry_grow_and_zero() {
    const size_t arr_capacity = 2000;
    DynamicArray_TableEntry arr = new_dynarr_table_entry_with_capacity(arr_capacity);
    memset(arr.arr, 1, arr_capacity * sizeof(Tuple_u64_double));

    dynarr_table_entry_shrink(&arr);

    const size_t arr_new_capacity = 32769;
    dynarr_table_entry_grow_and_zero(&arr, arr_new_capacity);
    
    assert(arr.capacity == arr_new_capacity, "Incorrect array capacity");
    assert(arr.size == arr_new_capacity, "Incorrect array size");

    assert(arr.arr[arr_capacity - 5] == 0, "Array was not correctly zeroed");

    // Check no segmentation fault
    for (size_t i = 0; i < arr_new_capacity; ++i)
        dynarr_table_entry_push(&arr, i + 0.25);

    free_dynarr_table_entry(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_table_entry_push_multiple() {
    DynamicArray_TableEntry arr = new_dynarr_table_entry();

    dynarr_table_entry_push(&arr, 41.1);
    dynarr_table_entry_push(&arr, 66.1);

    assert(arr.size == 2, "Incorrect array size");

    double second_array[] = {98.1, 95.1, 93.1, 90.1};
    dynarr_table_entry_push_multiple(&arr, second_array, 4);

    assert(arr.size == 6, "Incorrect array size");

    assert(dynarr_table_entry_get(&arr, 0) == 41.1, "Incorrect value in array");
    assert(dynarr_table_entry_get(&arr, 1) == 66.1, "Incorrect value in array");
    assert(dynarr_table_entry_get(&arr, 2) == second_array[0], "Incorrect value in array");
    assert(dynarr_table_entry_get(&arr, 3) == second_array[1], "Incorrect value in array");
    assert(dynarr_table_entry_get(&arr, 4) == second_array[2], "Incorrect value in array");
    assert(dynarr_table_entry_get(&arr, 5) == second_array[3], "Incorrect value in array");

    free_dynarr_table_entry(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_table_entry_remove_at() {
    DynamicArray_TableEntry arr = new_dynarr_table_entry();

    dynarr_table_entry_push(&arr, 41.1);
    dynarr_table_entry_push(&arr, 66.1);
    dynarr_table_entry_push(&arr, 2.1);
    dynarr_table_entry_push(&arr, 3.1);
    dynarr_table_entry_push(&arr, 4.1);

    assert(arr.size == 5, "Incorrect array size");

    dynarr_table_entry_remove_at(&arr, 1); // Remove 66
    dynarr_table_entry_remove_at(&arr, 2); // Remove 4 (its now at index 2 because 66 was removed)

    assert(arr.size == 3, "Incorrect array size");

    assert(arr.arr[0] == 41.1, "Incorrect value in array");
    assert(arr.arr[1] == 2.1, "Incorrect value in array");
    assert(arr.arr[2] == 4.1, "Incorrect value in array");

    free_dynarr_table_entry(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_table_entry_remove_multiple_at() {
    DynamicArray_TableEntry arr = new_dynarr_table_entry();

    dynarr_table_entry_push(&arr, 0.1);
    dynarr_table_entry_push(&arr, 1.1);
    dynarr_table_entry_push(&arr, 2.1);
    dynarr_table_entry_push(&arr, 3.1);
    dynarr_table_entry_push(&arr, 4.1);
    dynarr_table_entry_push(&arr, 5.1);
    dynarr_table_entry_push(&arr, 6.1);
    dynarr_table_entry_push(&arr, 7.1);
    dynarr_table_entry_push(&arr, 8.1);
    dynarr_table_entry_push(&arr, 9.1);

    assert(arr.size == 10, "Incorrect array size");

    dynarr_table_entry_remove_multiple_at(&arr, 3, 4);

    assert(arr.size == 6, "Incorrect array size");

    assert(arr.arr[0] == 0.1, "Incorrect value in array");
    assert(arr.arr[1] == 1.1, "Incorrect value in array");
    assert(arr.arr[2] == 2.1, "Incorrect value in array");
    assert(arr.arr[3] == 7.1, "Incorrect value in array");
    assert(arr.arr[4] == 8.1, "Incorrect value in array");
    assert(arr.arr[5] == 9.1, "Incorrect value in array");

    free_dynarr_table_entry(&arr);

    return TEST_PASS;
}

ModuleTestSet dynarray_h_register_tests()
{
    ModuleTestSet set = {
        .module_name = __FILE__,
        .tests = {0},
        .count = 0,
    };

    register_test(&set, test_new_dynarr8);
    register_test(&set, test_free_dynarr8);
    register_test(&set, test_dynarr8_push);
    register_test(&set, test_dynarr8_push_at);
    register_test(&set, test_dynarr8_get);
    register_test(&set, test_dynarr8_get_ptr);
    register_test(&set, test_dynarr8_pop);
    register_test(&set, test_dynarr8_shrink);
    register_test(&set, test_dynarr8_expand);
    register_test(&set, test_dynarr8_grow_and_zero);
    register_test(&set, test_dynarr8_push_multiple);
    register_test(&set, test_dynarr8_remove_at);

    register_test(&set, test_new_dynarr16);
    register_test(&set, test_free_dynarr16);
    register_test(&set, test_dynarr16_push);
    register_test(&set, test_dynarr16_push_at);
    register_test(&set, test_dynarr16_get);
    register_test(&set, test_dynarr16_get_ptr);
    register_test(&set, test_dynarr16_pop);
    register_test(&set, test_dynarr16_shrink);
    register_test(&set, test_dynarr16_expand);
    register_test(&set, test_dynarr16_grow_and_zero);
    register_test(&set, test_dynarr16_push_multiple);
    register_test(&set, test_dynarr16_remove_at);

    register_test(&set, test_new_dynarr_table_entry);
    register_test(&set, test_free_dynarr_table_entry);
    register_test(&set, test_dynarr_table_entry_push);
    register_test(&set, test_dynarr_table_entry_push_at);
    register_test(&set, test_dynarr_table_entry_get);
    register_test(&set, test_dynarr_table_entry_get_ptr);
    register_test(&set, test_dynarr_table_entry_pop);
    register_test(&set, test_dynarr_table_entry_shrink);
    register_test(&set, test_dynarr_table_entry_expand);
    register_test(&set, test_dynarr_table_entry_grow_and_zero);
    register_test(&set, test_dynarr_table_entry_push_multiple);
    register_test(&set, test_dynarr_table_entry_remove_at);

    return set;
}

#endif
