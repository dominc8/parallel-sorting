#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

extern "C"
{
#include "utils.h"
}

FILE *log_file;

#ifndef NDEBUG

    #define LOG(format, ...) fprintf(log_file, format "\n", ##__VA_ARGS__)
    #define LOG_ARRAY(arr_ptr, size) print_array(log_file, (arr_ptr), (size))

#else

    #define LOG(format, ...) do {} while (0)
    #define LOG_ARRAY(arr_ptr, size) do {} while (0)

#endif /*NDEBUG*/

#define N_BLOCKS    512
#define N_THREADS   512

__global__ void gpu_sort(int32_t *arr, int32_t size);
static int32_t compare_arr(int32_t *arr1, int32_t *arr2, int32_t size);

int main(int argc, char **argv)
{
    const int32_t arr_size = (1<<4);
    int32_t diff_cnt;
    int32_t *arr;
    int32_t *arr_ref;

#ifdef LOG_TO_FILE
    log_file = fopen("logs", "w");
#else
    log_file = stdout;
#endif

    alloc_fill_array(&arr, arr_size, 2*arr_size);
    arr_ref = (int32_t *)malloc(arr_size * sizeof(*arr));
    memcpy(&arr_ref[0], &arr[0], arr_size * sizeof(*arr));
    LOG_ARRAY(&arr[0], arr_size);


    /* REF START */

    start_time();

    qsort(&arr_ref[0], arr_size, sizeof(*arr_ref), &cmpfunc_int32);

    stop_time();

    printf("[Ref qsort] Elapsed time: %lf ms\n", elapsed_time_ms());
    LOG("[Ref qsort] Sorted array");
    LOG_ARRAY(&arr_ref[0], arr_size);

    /* REF STOP */


    /* GPU START */
    start_time();

    gpu_sort<<<N_BLOCKS, N_THREADS>>>(&arr[0], arr_size);

    stop_time();

    printf("[CUDA] Elapsed time: %lf ms\n", elapsed_time_ms());
    LOG("[CUDA] Sorted array:");
    LOG_ARRAY(&arr[0], arr_size);

    /* GPU STOP */

    
    diff_cnt = compare_arr(&arr[0], &arr_ref[0], arr_size);

    if (diff_cnt != 0)
    {
        printf("Results differ on %d elements\n", diff_cnt);
    }
    else
    {
        printf("Arrays are the same!\n");
    }

    free(arr);
    free(arr_ref);


#ifdef LOG_TO_FILE
    fclose(log_file);
#endif

    return 0;
}

__global__ void gpu_sort(int32_t *arr, int32_t size)
{
}

static int32_t compare_arr(int32_t *arr1, int32_t *arr2, int32_t size)
{
    int32_t cnt = 0;
    int32_t i;
    for (i = 0; i < size; ++i)
    {
        if (arr1[i] != arr2[i])
            ++cnt;
    }
    return cnt;
}

