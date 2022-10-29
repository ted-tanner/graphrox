#include "dynarray.h"

DynamicArray8 new_dynarr8_with_capacity(size_t start_capacity)
{
    Byte8Val *arr = malloc(sizeof(Byte8Val) * start_capacity);

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
    Byte8Val *new_arr = realloc(arr->arr, new_capacity * sizeof(Byte8Val));
    
    assert(new_arr != 0, "realloc failue");
    
    arr->arr = new_arr;
    arr->capacity = new_capacity;
}

void dynarr8_expand(DynamicArray8 *arr, size_t desired_capacity)
{
    if (desired_capacity <= arr->capacity)
        return;
    
    Byte8Val *new_arr = realloc(arr->arr, desired_capacity * sizeof(Byte8Val));
    
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

void dynarr8_push_multiple(DynamicArray8 *arr, Byte8Val *item_arr, size_t count)
{
    if (arr->size + count >= arr->capacity)
    {
        size_t new_capacity = arr->capacity * 2;
        for(; new_capacity < arr->size + count; new_capacity *= 2);

        Byte8Val *new_arr = realloc(arr->arr, new_capacity * sizeof(Byte8Val));

        assert(new_arr != 0, "realloc failue");

        arr->arr = new_arr;
        arr->capacity = new_capacity;
    }

    memcpy(arr->arr + arr->size, item_arr, count * sizeof(Byte8Val));
    arr->size += count;
}

void dynarr8_remove_at(DynamicArray8 *arr, size_t idx)
{
    assert(arr->size > idx, "Invalid array index");
    memmove(arr->arr + idx, arr->arr + idx + 1, (arr->size - idx - 1) * sizeof(Byte8Val));
    --arr->size;
}

void dynarr8_remove_multiple_at(DynamicArray8 *arr, size_t start_idx, size_t count)
{
    assert(arr->size >= start_idx + count, "Invalid array index or count");
    memmove(arr->arr + start_idx,
            arr->arr + start_idx + count,
            (arr->size - start_idx - count) * sizeof(Byte8Val));
    arr->size -= count;
}

void _dynarr8_push_at(DynamicArray8 *arr, Byte8Val item, size_t idx)
{
    assert(arr->size >= idx, "Invalid array index");
    
    if (arr->size == arr->capacity)
    {
        Byte8Val *new_arr = realloc(arr->arr, arr->capacity * sizeof(Byte8Val) * 2);

        assert(new_arr != 0, "realloc failue");

        arr->arr = new_arr;
        arr->capacity *= 2;
    }

    Byte8Val *location = arr->arr + idx;
    
    if (idx != arr->size)
        memmove(location + 1, location, (arr->size - idx) * sizeof(Byte8Val));

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
    for (u64 i = 0; i < arr2_capacity; ++i)
    {
        Byte8Val val = { .u64_val = i };
        dynarr8_push(&arr2, val);
    }

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

    Byte8Val val1 = { .u64_val = 42 };
    Byte8Val val2 = { .u64_val = 67 };
    Byte8Val val3 = { .dbl_val = 1.2 };

    dynarr8_push(&arr, val1);
    dynarr8_push(&arr, val2);
    dynarr8_push(&arr, val3);

    assert(arr.arr[0].u64_val == val1.u64_val, "Incorrect value in array");
    assert(arr.arr[1].u64_val == val2.u64_val, "Incorrect value in array");
    assert(arr.arr[2].dbl_val == val3.dbl_val, "Incorrect value in array");

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_push_at() {
    DynamicArray8 arr = new_dynarr8();

    Byte8Val val1 = { .u64_val = 41 };
    Byte8Val val2 = { .u64_val = 66 };
    Byte8Val val3 = { .dbl_val = 1.7 };
    Byte8Val val4 = { .u64_val = 167 };

    dynarr8_push(&arr, val1);
    dynarr8_push(&arr, val2);
    dynarr8_push(&arr, val3);

    dynarr8_push_at(&arr, val4, 1);

    assert(arr.arr[0].u64_val == val1.u64_val, "Incorrect value in array");
    assert(arr.arr[1].u64_val == val4.u64_val, "Incorrect value in array");
    assert(arr.arr[2].u64_val == val2.u64_val, "Incorrect value in array");
    assert(arr.arr[3].dbl_val == val3.dbl_val, "Incorrect value in array");

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_get() {
    DynamicArray8 arr = new_dynarr8();

    Byte8Val val1 = { .u64_val = 43 };
    Byte8Val val2 = { .u64_val = 68 };
    Byte8Val val3 = { .u64_val = 4 };

    dynarr8_push(&arr, val1);
    dynarr8_push(&arr, val2);
    dynarr8_push(&arr, val3);

    assert(dynarr8_get(&arr, 0).u64_val == val1.u64_val, "Incorrect value in array");
    assert(dynarr8_get(&arr, 1).u64_val == val2.u64_val, "Incorrect value in array");
    assert(dynarr8_get(&arr, 2).u64_val == val3.u64_val, "Incorrect value in array");

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_get_ptr() {
    DynamicArray8 arr = new_dynarr8();

    Byte8Val val1 = { .u64_val = 44 };
    Byte8Val val2 = { .u64_val = 69 };
    Byte8Val val3 = { .u64_val = 5 };

    dynarr8_push(&arr, val1);
    dynarr8_push(&arr, val2);
    dynarr8_push(&arr, val3);

    assert((*(dynarr8_get_ptr(&arr, 0))).u64_val == val1.u64_val, "Incorrect pointer to value in array");
    assert((*(dynarr8_get_ptr(&arr, 1))).u64_val == val2.u64_val, "Incorrect pointer to value in array");
    assert((*(dynarr8_get_ptr(&arr, 2))).u64_val == val3.u64_val, "Incorrect pointer to value in array");

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_pop() {
    DynamicArray8 arr = new_dynarr8();
    
    Byte8Val val1 = { .u64_val = 100 };
    Byte8Val val2 = { .u64_val = 101 };
    Byte8Val val3 = { .u64_val = 102 };

    dynarr8_push(&arr, val1);
    dynarr8_push(&arr, val2);
    dynarr8_push(&arr, val3);

    assert(arr.size == 3, "Incorrect array size");

    assert(dynarr8_pop(&arr).u64_val == val3.u64_val, "Incorrect value in array");
    assert(arr.size == 2, "Incorrect array size");

    assert(dynarr8_pop(&arr).u64_val == val2.u64_val, "Incorrect value in array");
    assert(arr.size == 1, "Incorrect array size");
    
    assert(dynarr8_pop(&arr).u64_val == val1.u64_val, "Incorrect value in array");
    assert(arr.size == 0, "Incorrect array size");

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_shrink() {
    DynamicArray8 arr = new_dynarr8_with_capacity(25);

    assert(arr.size == 0, "Incorrect array size");
    assert(arr.capacity == 25, "Incorrect array capacity");

    Byte8Val val1 = { .u64_val = 100 };
    Byte8Val val2 = { .u64_val = 101 };
    Byte8Val val3 = { .u64_val = 102 };

    dynarr8_push(&arr, val1);
    dynarr8_push(&arr, val2);
    dynarr8_push(&arr, val3);

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
    for (u64 i = 0; i < arr_new_capacity; ++i)
    {
        Byte8Val val = { .u64_val = i };
        dynarr8_push(&arr, val);
    }

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_grow_and_zero() {
    const size_t arr_capacity = 2000;
    DynamicArray8 arr = new_dynarr8_with_capacity(arr_capacity);
    memset(arr.arr, 1, arr_capacity * sizeof(Byte8Val));

    dynarr8_shrink(&arr);

    const size_t arr_new_capacity = 32769;
    dynarr8_grow_and_zero(&arr, arr_new_capacity);
    
    assert(arr.capacity == arr_new_capacity, "Incorrect array capacity");
    assert(arr.size == arr_new_capacity, "Incorrect array size");

    assert(arr.arr[arr_capacity - 5].u64_val == 0, "Array was not correctly zeroed");

    // Check no segmentation fault
    for (u64 i = 0; i < arr_new_capacity; ++i)
    {
        Byte8Val val = { .u64_val = i };
        dynarr8_push(&arr, val);
    }

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_push_multiple() {
    DynamicArray8 arr = new_dynarr8();

    Byte8Val val1 = { .u64_val = 41 };
    Byte8Val val2 = { .u64_val = 66 };

    dynarr8_push(&arr, val1);
    dynarr8_push(&arr, val2);

    assert(arr.size == 2, "Incorrect array size");

    Byte8Val val3 = { .u64_val = 98 };
    Byte8Val val4 = { .u64_val = 95 };
    Byte8Val val5 = { .u64_val = 93 };
    Byte8Val val6 = { .u64_val = 90 };

    Byte8Val second_array[] = {val3, val4, val5, val6};
    dynarr8_push_multiple(&arr, second_array, 4);

    assert(arr.size == 6, "Incorrect array size");

    assert(dynarr8_get(&arr, 0).u64_val == val1.u64_val, "Incorrect value in array");
    assert(dynarr8_get(&arr, 1).u64_val == val2.u64_val, "Incorrect value in array");
    assert(dynarr8_get(&arr, 2).u64_val == second_array[0].u64_val, "Incorrect value in array");
    assert(dynarr8_get(&arr, 3).u64_val == second_array[1].u64_val, "Incorrect value in array");
    assert(dynarr8_get(&arr, 4).u64_val == second_array[2].u64_val, "Incorrect value in array");
    assert(dynarr8_get(&arr, 5).u64_val == second_array[3].u64_val, "Incorrect value in array");

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_remove_at() {
    DynamicArray8 arr = new_dynarr8();

    Byte8Val val1 = { .u64_val = 41 };
    Byte8Val val2 = { .u64_val = 66 };
    Byte8Val val3 = { .u64_val = 2 };
    Byte8Val val4 = { .u64_val = 3 };
    Byte8Val val5 = { .u64_val = 4 };

    dynarr8_push(&arr, val1);
    dynarr8_push(&arr, val2);
    dynarr8_push(&arr, val3);
    dynarr8_push(&arr, val4);
    dynarr8_push(&arr, val5);

    assert(arr.size == 5, "Incorrect array size");

    dynarr8_remove_at(&arr, 1); // Remove val2
    dynarr8_remove_at(&arr, 2); // Remove val5 (its now at index 2 because val2 was removed)

    assert(arr.size == 3, "Incorrect array size");

    assert(arr.arr[0].u64_val == val1.u64_val, "Incorrect value in array");
    assert(arr.arr[1].u64_val == val3.u64_val, "Incorrect value in array");
    assert(arr.arr[2].u64_val == val5.u64_val, "Incorrect value in array");

    free_dynarr8(&arr);

    return TEST_PASS;
}

static TEST_RESULT test_dynarr8_remove_multiple_at() {
    DynamicArray8 arr = new_dynarr8();

    Byte8Val val1 = { .u64_val = 0 };
    Byte8Val val2 = { .u64_val = 1 };
    Byte8Val val3 = { .u64_val = 2 };
    Byte8Val val4 = { .u64_val = 3 };
    Byte8Val val5 = { .u64_val = 4 };
    Byte8Val val6 = { .u64_val = 5 };
    Byte8Val val7 = { .u64_val = 6 };
    Byte8Val val8 = { .u64_val = 7 };
    Byte8Val val9 = { .u64_val = 8 };
    Byte8Val val10 = { .u64_val = 9 };
    
    dynarr8_push(&arr, val1);
    dynarr8_push(&arr, val2);
    dynarr8_push(&arr, val3);
    dynarr8_push(&arr, val4);
    dynarr8_push(&arr, val5);
    dynarr8_push(&arr, val6);
    dynarr8_push(&arr, val7);
    dynarr8_push(&arr, val8);
    dynarr8_push(&arr, val9);
    dynarr8_push(&arr, val10);

    assert(arr.size == 10, "Incorrect array size");

    dynarr8_remove_multiple_at(&arr, 3, 4);

    assert(arr.size == 6, "Incorrect array size");

    assert(arr.arr[0].u64_val == 0, "Incorrect value in array");
    assert(arr.arr[1].u64_val == 1, "Incorrect value in array");
    assert(arr.arr[2].u64_val == 2, "Incorrect value in array");
    assert(arr.arr[3].u64_val == 7, "Incorrect value in array");
    assert(arr.arr[4].u64_val == 8, "Incorrect value in array");
    assert(arr.arr[5].u64_val == 9, "Incorrect value in array");

    free_dynarr8(&arr);

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

    return set;
}

#endif
