#include "dynarray.h"

DynamicArrayU64 new_dynarr_u64_with_capacity(size_t start_capacity)
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
    
    u64 *new_arr = realloc(arr->arr, desired_capacity * sizeof(u64));
    
    assert(new_arr != 0, "realloc failue");
    
    arr->arr = new_arr;
    arr->capacity = desired_capacity;
}

void dynarr_u64_grow_and_zero(DynamicArrayU64* arr, size_t desired_size)
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
        memmove((void*) (location + 1), (void*) location, (arr->size - idx) * sizeof(u64));

    arr->arr[idx] = item;
    ++arr->size;
}

#ifdef TEST_MODE

static TEST_RESULT test_new_dynarr_u64() {
    DynamicArrayU64 arr = new_dynarr_u64();

    assert(arr.capacity == 1, "Incorrect array capacity");
    assert(arr.size == 0, "Incorrect array size");
    assert(arr.arr != 0, "Null array pointer");

    // This size will exceed page size on practically all systems, so segmentation
    // faults for stepping out of bounds are possible
    const size_t arr2_capacity = 32769;
    DynamicArrayU64 arr2 = new_dynarr_u64_with_capacity(arr2_capacity);

    assert(arr2.capacity == arr2_capacity, "Incorrect array capacity");
    assert(arr2.size == 0, "Incorrect array size");
    assert(arr2.arr != 0, "Null array pointer");

    // Check no segmentation fault
    for (u64 i = 0; i < arr2_capacity; ++i)
        dynarr_u64_push(&arr2, i);

    assert(arr2.capacity == arr2_capacity, "Incorrect array capacity");
    assert(arr2.size == arr2_capacity, "Incorrect array size");

    free_dynarr_u64(&arr);
    free_dynarr_u64(&arr2);

    return TEST_PASS;
}

static TEST_RESULT test_free_dynarr_u64() {
    const size_t arr_capacity = 32769;
    DynamicArrayU64 arr = new_dynarr_u64_with_capacity(arr_capacity);

    // Just make sure this causes no errors
    free_dynarr_u64(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_u64_push() {
    DynamicArrayU64 arr = new_dynarr_u64();

    dynarr_u64_push(&arr, 42);
    dynarr_u64_push(&arr, 67);
    dynarr_u64_push(&arr, 1);

    assert(arr.arr[0] == 42, "Incorrect value in array");
    assert(arr.arr[1] == 67, "Incorrect value in array");
    assert(arr.arr[2] == 1, "Incorrect value in array");

    free_dynarr_u64(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_u64_push_at() {
    DynamicArrayU64 arr = new_dynarr_u64();

    dynarr_u64_push(&arr, 41);
    dynarr_u64_push(&arr, 66);
    dynarr_u64_push(&arr, 2);

    dynarr_u64_push_at(&arr, 3, 1);

    assert(arr.arr[0] == 41, "Incorrect value in array");
    assert(arr.arr[1] == 3, "Incorrect value in array");
    assert(arr.arr[2] == 66, "Incorrect value in array");
    assert(arr.arr[3] == 2, "Incorrect value in array");

    free_dynarr_u64(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_u64_get() {
    DynamicArrayU64 arr = new_dynarr_u64();

    dynarr_u64_push(&arr, 43);
    dynarr_u64_push(&arr, 68);
    dynarr_u64_push(&arr, 4);

    assert(dynarr_u64_get(&arr, 0) == 43, "Incorrect value in array");
    assert(dynarr_u64_get(&arr, 1) == 68, "Incorrect value in array");
    assert(dynarr_u64_get(&arr, 2) == 4, "Incorrect value in array");

    free_dynarr_u64(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_u64_get_ptr() {
    DynamicArrayU64 arr = new_dynarr_u64();

    dynarr_u64_push(&arr, 44);
    dynarr_u64_push(&arr, 69);
    dynarr_u64_push(&arr, 5);

    assert(*(dynarr_u64_get_ptr(&arr, 0)) == 44, "Incorrect pointer to value in array");
    assert(*(dynarr_u64_get_ptr(&arr, 1)) == 69, "Incorrect pointer to value in array");
    assert(*(dynarr_u64_get_ptr(&arr, 2)) == 5, "Incorrect pointer to value in array");

    free_dynarr_u64(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_u64_pop() {
    DynamicArrayU64 arr = new_dynarr_u64();

    dynarr_u64_push(&arr, 100);
    dynarr_u64_push(&arr, 101);
    dynarr_u64_push(&arr, 102);

    assert(arr.size == 3, "Incorrect array size");

    assert(dynarr_u64_pop(&arr) == 102, "Incorrect value in array");
    assert(arr.size == 2, "Incorrect array size");

    assert(dynarr_u64_pop(&arr) == 101, "Incorrect value in array");
    assert(arr.size == 1, "Incorrect array size");
    
    assert(dynarr_u64_pop(&arr) == 100, "Incorrect value in array");
    assert(arr.size == 0, "Incorrect array size");

    free_dynarr_u64(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_u64_shrink() {
    DynamicArrayU64 arr = new_dynarr_u64_with_capacity(25);

    assert(arr.size == 0, "Incorrect array size");
    assert(arr.capacity == 25, "Incorrect array capacity");

    dynarr_u64_push(&arr, 100);
    dynarr_u64_push(&arr, 101);
    dynarr_u64_push(&arr, 102);

    assert(arr.size == 3, "Incorrect array size");
    assert(arr.capacity == 25, "Incorrect array capacity");

    dynarr_u64_shrink(&arr);

    assert(arr.size == 3, "Incorrect array size");
    assert(arr.capacity == 3, "Incorrect array capacity");

    dynarr_u64_pop(&arr);

    assert(arr.size == 2, "Incorrect array size");
    assert(arr.capacity == 3, "Incorrect array capacity");

    dynarr_u64_shrink(&arr);

    assert(arr.size == 2, "Incorrect array size");
    assert(arr.capacity == 2, "Incorrect array capacity");

    free_dynarr_u64(&arr);
    
    return TEST_PASS;
}

static TEST_RESULT test_dynarr_u64_expand() {
    const size_t arr_capacity = 1000;
    DynamicArrayU64 arr = new_dynarr_u64_with_capacity(arr_capacity);

    assert(arr.capacity == arr_capacity, "Incorrect array capacity");
    assert(arr.size == 0, "Incorrect array size");

    const size_t arr_new_capacity = 32769;
    dynarr_u64_expand(&arr, arr_new_capacity);

    assert(arr.capacity == arr_new_capacity, "Incorrect array capacity");
    assert(arr.size == 0, "Incorrect array size");

    // Check no segmentation fault
    for (u64 i = 0; i < arr_new_capacity; ++i)
        dynarr_u64_push(&arr, i);

    free_dynarr_u64(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_u64_grow_and_zero() {
    const size_t arr_capacity = 2000;
    DynamicArrayU64 arr = new_dynarr_u64_with_capacity(arr_capacity);
    memset(arr.arr, 1, arr_capacity * sizeof(u64));

    dynarr_u64_shrink(&arr);

    const size_t arr_new_capacity = 32769;
    dynarr_u64_grow_and_zero(&arr, arr_new_capacity);
    
    assert(arr.capacity == arr_new_capacity, "Incorrect array capacity");
    assert(arr.size == arr_new_capacity, "Incorrect array size");

    assert(arr.arr[arr_capacity - 5] == 0, "Array was not correctly zeroed");

    // Check no segmentation fault
    for (u64 i = 0; i < arr_new_capacity; ++i)
        dynarr_u64_push(&arr, i);

    free_dynarr_u64(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_u64_push_multiple() {
    DynamicArrayU64 arr = new_dynarr_u64();

    dynarr_u64_push(&arr, 41);
    dynarr_u64_push(&arr, 66);

    assert(arr.size == 2, "Incorrect array size");

    u64 second_array[] = {98, 95, 93, 90};
    dynarr_u64_push_multiple(&arr, second_array, 4);

    assert(arr.size == 6, "Incorrect array size");

    assert(dynarr_u64_get(&arr, 0) == 41, "Incorrect value in array");
    assert(dynarr_u64_get(&arr, 1) == 66, "Incorrect value in array");
    assert(dynarr_u64_get(&arr, 2) == second_array[0], "Incorrect value in array");
    assert(dynarr_u64_get(&arr, 3) == second_array[1], "Incorrect value in array");
    assert(dynarr_u64_get(&arr, 4) == second_array[2], "Incorrect value in array");
    assert(dynarr_u64_get(&arr, 5) == second_array[3], "Incorrect value in array");

    free_dynarr_u64(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr_u64_remove_at() {
    DynamicArrayU64 arr = new_dynarr_u64();

    dynarr_u64_push(&arr, 41);
    dynarr_u64_push(&arr, 66);
    dynarr_u64_push(&arr, 2);
    dynarr_u64_push(&arr, 3);
    dynarr_u64_push(&arr, 4);

    assert(arr.size == 5, "Incorrect array size");

    dynarr_u64_remove_at(&arr, 1); // Remove 66
    dynarr_u64_remove_at(&arr, 2); // Remove 4 (its now at index 2 because 66 was removed)

    assert(arr.size == 3, "Incorrect array size");

    assert(arr.arr[0] == 41, "Incorrect value in array");
    assert(arr.arr[1] == 2, "Incorrect value in array");
    assert(arr.arr[2] == 4, "Incorrect value in array");

    free_dynarr_u64(&arr);

    return TEST_PASS;
}

ModuleTestSet dynarray_h_register_tests()
{
    ModuleTestSet set = {
        .module_name = __FILE__,
        .tests = {0},
        .count = 0,
    };

    register_test(&set, test_new_dynarr_u64);
    register_test(&set, test_free_dynarr_u64);
    register_test(&set, test_dynarr_u64_push);
    register_test(&set, test_dynarr_u64_push_at);
    register_test(&set, test_dynarr_u64_get);
    register_test(&set, test_dynarr_u64_get_ptr);
    register_test(&set, test_dynarr_u64_pop);
    register_test(&set, test_dynarr_u64_shrink);
    register_test(&set, test_dynarr_u64_expand);
    register_test(&set, test_dynarr_u64_grow_and_zero);
    register_test(&set, test_dynarr_u64_push_multiple);
    register_test(&set, test_dynarr_u64_remove_at);

    return set;
}

#endif
