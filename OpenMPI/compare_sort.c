#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "utils.h"

#define ARR_SIZE (1<<26)

int main()
{
    int32_t *arr;

    alloc_fill_array(&arr, ARR_SIZE, 2*ARR_SIZE);

    qsort(&arr[0], ARR_SIZE, sizeof(*arr), &cmpfunc_int32);

    save_results("result_ref", &arr[0], sizeof(*arr), ARR_SIZE);

    free(arr);
    return 0;
}

