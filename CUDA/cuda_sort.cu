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

#define N_BLOCKS    (1<<15)
#define N_THREADS   (1<<9)
#define ARR_SIZE    ((N_BLOCKS)*(N_THREADS))

__global__ void gpu_sort(int32_t *arr, int32_t stage_idx, int32_t substage_idx);

void sort(int32_t *arr, int32_t size);
static int32_t compare_arr(int32_t *arr1, int32_t *arr2, int32_t size);

int main(int argc, char **argv)
{
    int32_t diff_cnt;
    int32_t *arr;
    int32_t *cuda_arr;
    int32_t *arr_ref;
    cudaEvent_t start, stop, start_mem;
    float time, time_mem;

#ifdef LOG_TO_FILE
    log_file = fopen("logs", "w");
#else
    log_file = stdout;
#endif

    alloc_fill_array(&arr, ARR_SIZE, 2*ARR_SIZE);
    arr_ref = (int32_t *)malloc(ARR_SIZE * sizeof(*arr));
    memcpy(&arr_ref[0], &arr[0], ARR_SIZE * sizeof(*arr));
    LOG_ARRAY(&arr[0], ARR_SIZE);


    /* REF START */

    start_time();

    qsort(&arr_ref[0], ARR_SIZE, sizeof(*arr_ref), &cmpfunc_int32);

    stop_time();

    printf("[Ref qsort] Elapsed time: %lf ms\n", elapsed_time_ms());
    LOG("[Ref qsort] Sorted array");
    LOG_ARRAY(&arr_ref[0], ARR_SIZE);

    /* REF STOP */


    /* GPU START */
    cudaEventCreate(&start_mem, 0);
    cudaEventCreate(&start, 0);
    cudaEventCreate(&stop, 0);

    cudaEventRecord(start_mem, 0);

    cudaMalloc((void **)&cuda_arr, sizeof(*cuda_arr) * ARR_SIZE);
    cudaMemcpy(cuda_arr, arr, ARR_SIZE * sizeof(*arr), cudaMemcpyHostToDevice);

    cudaEventRecord(start, 0);

    sort(&cuda_arr[0], ARR_SIZE);

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&time, start, stop);
    cudaEventElapsedTime(&time_mem, start_mem, stop);

    printf("[CUDA] Elapsed time: %f (%f) ms\n", time, time_mem);
    cudaMemcpy(arr, cuda_arr, ARR_SIZE * sizeof(*arr), cudaMemcpyDeviceToHost);
    LOG("[CUDA] Sorted array:");
    LOG_ARRAY(&arr[0], ARR_SIZE);

    /* GPU STOP */

    
    diff_cnt = compare_arr(&arr[0], &arr_ref[0], ARR_SIZE);

    if (diff_cnt != 0)
    {
        printf("Results differ on %d elements\n", diff_cnt);
    }
    else
    {
        printf("Arrays are the same!\n");
    }

    cudaFree(cuda_arr);
    free(arr);
    free(arr_ref);


#ifdef LOG_TO_FILE
    fclose(log_file);
#endif

    return 0;
}

void sort(int32_t *arr, int32_t size)
{
    for (int32_t i = 2; i <= size; i <<= 1)
    {
        for (int32_t j = i>>1; j > 0; j >>= 1)
        {
            gpu_sort<<<N_BLOCKS, N_THREADS>>>(&arr[0], i, j);
            LOG("stage=%d, substage=%d", i, j);
        }
    }
}

__global__ void gpu_sort(int32_t *arr, int32_t stage_idx, int32_t substage_idx)
{
    int32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
    int32_t idx_s = idx ^ substage_idx;

    if (idx_s > idx)
    {
        if (idx & stage_idx)
        {
            if (arr[idx] < arr[idx_s])
            {
                int32_t temp = arr[idx];
                arr[idx] = arr[idx_s];
                arr[idx_s] = temp;
            }
        }
        else
        {
            if (arr[idx] > arr[idx_s])
            {
                int32_t temp = arr[idx];
                arr[idx] = arr[idx_s];
                arr[idx_s] = temp;
            }
        }
    }
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

