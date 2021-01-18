#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <mpi.h>

#ifndef NDEBUG
#define LOG(format, ...) printf("%d: " format "\n", rank, ##__VA_ARGS__)
#else
#define LOG(format, ...) do {} while (0)
#endif


#define ARR_SIZE 16
#define N_SUBARRAYS 4
#define LOG2_N_SUBARRAYS 2
#define SUBARR_SIZE (ARR_SIZE/N_SUBARRAYS)


void alloc_fill_array(int32_t **arr, int32_t size);
void print_array(int32_t *arr, int32_t size);
void merge_sorted(int32_t *arr_out, int32_t *arr_in1, int32_t *arr_in2, int32_t arr_in_size);

int cmpfunc_int32 (const void *a, const void *b)
{
    return ( *(int32_t *)a - *(int32_t *)b);
}

int main(int argc, char **argv)
{
    const int root = 0;
    int rank, size, size_mod, rank_mod;
    int recv_rank, send_rank;
    int32_t sub_arr_size;
    int32_t *arr;
    int32_t *sub_arr;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    assert(size == 4);

    if (root == rank)
    {
        alloc_fill_array(&arr, ARR_SIZE);
    }

    sub_arr_size = ARR_SIZE / (size / (size / (rank + 1)));
    sub_arr = (int32_t*)malloc(sub_arr_size * sizeof(*sub_arr));
    LOG("sub_arr_size= %d", sub_arr_size);


    MPI_Scatter(&arr[0], SUBARR_SIZE, MPI_INT32_T,
                &sub_arr[0], SUBARR_SIZE, MPI_INT32_T, root, MPI_COMM_WORLD);

    qsort(&sub_arr[0], SUBARR_SIZE, sizeof(*sub_arr), &cmpfunc_int32);

    int temp_size = size>>1;
    int temp_rank = rank;
    int32_t filled_sub_arr = SUBARR_SIZE;
    MPI_Status status;
    while (temp_rank<<1 < size)
    {
        // recv_rank = rank + temp_size;
        // odbierz od kogos z recv_rank filled_sub_arr elementów do &sub_arr[filled_sub_arr]
        recv_rank = rank + temp_size;

        if (recv_rank == rank)
            break;

        LOG("Receive from %d", recv_rank);
        MPI_Recv(&sub_arr[filled_sub_arr], filled_sub_arr, MPI_INT32_T,
                 recv_rank, 0, MPI_COMM_WORLD, &status);

        temp_size >>= 1;
        temp_rank <<= 1;
        filled_sub_arr <<= 1;
    }
    if (root == rank)
    {
    }
    else
    {
        // send_rank = rank - temp_size
        // wyslij do kogos z send_rank filled_sub_arr elementów z &sub_arr[0]
        send_rank = rank - temp_size;
        LOG("Send to %d", send_rank);
        MPI_Send(&sub_arr[0], filled_sub_arr, MPI_INT32_T,
                 send_rank, 0, MPI_COMM_WORLD);
    }

    print_array(&sub_arr[0], sub_arr_size);

    if (root == rank)
    {
        free(arr);
    }
    free(sub_arr);

    MPI_Finalize();
    return 0;
}


void alloc_fill_array(int32_t **arr, int32_t size)
{
    *arr = (int32_t *)malloc(sizeof(**arr) * size);

    int32_t *ptr = *arr;

    for (int32_t i = 0; i < size; ++i)
    {
        ptr[i] = rand()%(2*ARR_SIZE);
    }
}

void print_array(int32_t *arr, int32_t size)
{
    for (int32_t i = 0; i < size; ++i)
    {
        printf("%d, ", arr[i]);
    }
    printf("\n");
}

