#include <stdio.h>
#include <string.h> /* For memcpy & memcmp */
#include <stdlib.h>
#include <pthread.h>

#include "array.h"
#include "errors.h"

const ssize_t DEFAULT_CAPACITY = 16;
const ssize_t MAX_CAPACITY = 0x7fffffff >> 2;

static pthread_mutex_t destroyerMutex = PTHREAD_MUTEX_INITIALIZER;

Array *newArray(ssize_t capacity) {
    if (capacity < 0)
        capacity = DEFAULT_CAPACITY;

    Array *arr = (Array *)malloc(sizeof(*arr) + (sizeof(void *) * capacity));
    if (arr == NULL) {
        return arr;
    }

    arr->_freed = 0;
    arr->_capacity = capacity;
    return arr;
}

Array *append(Array *src, void *item) {
    ssize_t capacity, newCapacity;
    capacity = newCapacity = 0;

    if (src != NULL)
        capacity = src->_capacity;

    if (capacity >= (MAX_CAPACITY - 2)) {
        raiseError("overflow detected (capacity + 1) %d is >= %d", capacity, MAX_CAPACITY);
    }

    newCapacity = capacity + 1;
    Array *fresh = newArray(newCapacity);

    // Next stop is to copy the memory
    if (src != NULL && src->_data != NULL) {
        memcpy(fresh->_data, (const void *)src->_data, capacity);
    }

    fresh->_data[capacity] = item;

    return fresh;
}

Array *destroyArray(Array *arr) {
    if (arr == NULL || arr->_freed)
        return arr;

    pthread_mutex_lock(&destroyerMutex);

    arr->_freed = 1;
    free(arr);
    arr = NULL;

    pthread_mutex_unlock(&destroyerMutex);

    return arr;
}
