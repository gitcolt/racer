/*
 * Example usage (example.c):
 * 
 * #define DYN_ARR_IMPL
 * #include "dyn_arr.h"
 *
 * #include <stdio.h>
 *
 * float *arr = ARR(float, 10);
 * ARR_APPEND(arr, 12.34);
 * ARR_APPEND(arr, 56.78);
 * for (int i = 0; i < ARR_LEN(arr); ++i) {
 *      printf("arr[%d]: %.2f\n", i, arr[i]);
 * }
 * arr_free(arr);
 */

#ifndef DYN_ARR_H
#define DYN_ARR_H

#include <stddef.h>

typedef struct {
    size_t len;
    size_t cap;
} ArrHeader;

#define ARR_HEADER(a) ((ArrHeader *)(a) - 1)
#define ARR_LEN(a) (ARR_HEADER(a)->len)
#define ARR_CAPACITY(a) (ARR_HEADER(a)->cap)

void *arr_init(size_t item_size, size_t capacity);

#define ARR(T, initial_capacity) \
    (T *)arr_init(sizeof(T), initial_capacity)

void *arr_ensure_capacity(void *arr, size_t item_count, size_t item_size);

#define ARR_APPEND(arr, v) ( \
    (arr) = arr_ensure_capacity(arr, ARR_LEN(arr) + 1, sizeof(v)), \
    (arr)[ARR_HEADER(arr)->len] = (v), \
    &(arr)[ARR_HEADER(arr)->len++] )

void *arr_clear(void *arr);

void arr_free(void *arr);

#endif // end DYN_ARR_H

#ifdef DYN_ARR_IMPL

#include <malloc.h>

void *arr_init(size_t item_size, size_t capacity) {
    void *ptr = NULL;
    size_t size = item_size * capacity + sizeof(ArrHeader);
    ArrHeader *h = malloc(size);
    if (h) {
        h->cap = capacity;
        h->len = 0;
        ptr = h + 1;
    }
    return ptr;
}

void *arr_ensure_capacity(void *a, size_t item_count, size_t item_size) {
    ArrHeader *h = ARR_HEADER(a);
    if (h->cap < item_count) {
        h = realloc(h, sizeof(ArrHeader) + item_size * item_count);
        h->cap = item_count;
    }
    if (h)
        ++h;
    return h;
}

void *arr_clear(void *arr) {
    ArrHeader *h = ARR_HEADER(arr);
    h = realloc(h, sizeof(ArrHeader));
    h->len = 0;
    h->cap = 0;

    return h + 1;
}

void arr_free(void *arr) {
    free(ARR_HEADER(arr));
}

#endif
