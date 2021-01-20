#include "utils.h"
#include <stdlib.h>

void alloc_fill_array(int32_t **arr, int32_t size, int32_t max_val)
{
    srand(0);

    *arr = (int32_t *)malloc(sizeof(**arr) * size);

    int32_t *ptr = *arr;

    for (int32_t i = 0; i < size; ++i)
    {
        ptr[i] = rand()%(max_val);
    }
}


void print_array(FILE *f, int32_t *arr, int32_t size)
{
    for (int32_t i = 0; i < size; ++i)
    {
        fprintf(f, "%d, ", arr[i]);
    }
    fprintf(f, "\n");
}

int cmpfunc_int32 (const void *a, const void *b)
{
    return ( *(int32_t *)a - *(int32_t *)b);
}

void save_results(const char* filename, const void *arr, size_t elem_size, size_t count)
{
    FILE *result_file = fopen(filename, "wb");
    if (result_file != NULL)
    {
        fwrite(arr, elem_size, count, result_file);
        fclose(result_file);
    }
}

