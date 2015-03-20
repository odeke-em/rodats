#include <stdio.h>

#include "array.h"
#include "errors.h"

int main() {
    Array *arr = append(NULL, arr);

    Array *arr2 = append(arr, main);

    printf("arr: %p arr2: %p\n", arr, arr2);
    arr  = destroyArray(arr);
    arr2 = destroyArray(arr2);

    assert(arr == NULL);
    assert(arr2 == NULL);
    printf("after free:: arr: %p arr2: %p\n", arr, arr2);

    destroyArray(NULL);

    return 0;
}
