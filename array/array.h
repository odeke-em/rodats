#ifndef _ARRAY_H
#define _ARRAY_H

typedef struct {
    unsigned int _freed:1;
    ssize_t _capacity;
    // Get this flex array in for locality
    void *_data[];
} Array;

Array *append(Array *src, void *item);
Array *destroyArray(Array *arr);

Array *newArray(ssize_t capacity);

#endif // _ARRAY_H
