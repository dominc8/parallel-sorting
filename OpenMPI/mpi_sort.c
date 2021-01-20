#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <mpi.h>

#define LOG_TO_FILE

FILE *log_file;

#ifndef NDEBUG

    #define LOG(format, ...) fprintf(log_file, "%d: " format "\n", rank, ##__VA_ARGS__)
    #define LOG_ARRAY(arr_ptr, size) print_array(log_file, (arr_ptr), (size))

#else

    #define LOG(format, ...) do {} while (0)
    #define LOG_ARRAY(arr_ptr, size) do {} while (0)

#endif /*NDEBUG*/


#define ARR_SIZE (1<<26)
#ifndef N_SUBARRAYS
    #define N_SUBARRAYS 4
#endif /*N_SUBARRAYS*/
#define SUBARR_SIZE (ARR_SIZE/N_SUBARRAYS)

static int rank;

void merge_sorted_in_place(int32_t *arr, int32_t arr_half_size, int32_t *tmp_buf);
static void alloc_fill_array(int32_t **arr, int32_t size);
static void print_array(FILE *f, int32_t *arr, int32_t size);

/* Helper function for qsort */
int cmpfunc_int32 (const void *a, const void *b)
{
    return ( *(int32_t *)a - *(int32_t *)b);
}

int main(int argc, char **argv)
{
    const int root = 0;
    int size;
    int recv_rank, send_rank;
    int32_t sub_arr_size;
    int32_t *arr;
    int32_t *sub_arr;
    int32_t *tmp_buf;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    assert(size == N_SUBARRAYS);

#ifdef LOG_TO_FILE
    char log_file_name[15] = {0};
    snprintf(&log_file_name[0], sizeof(log_file_name), "proc%d.log", rank);
    log_file = fopen(&log_file_name[0], "w");
#else
    log_file = stdout;
#endif

    if (root == rank)
    {
        alloc_fill_array(&arr, ARR_SIZE);
        LOG_ARRAY(&arr[0], ARR_SIZE);
    }


    sub_arr_size = ARR_SIZE / (size / (size / (rank + 1)));
    sub_arr = (int32_t*)malloc(sub_arr_size * sizeof(*sub_arr));
    tmp_buf = (int32_t*)malloc(sub_arr_size * sizeof(*sub_arr) >> 1);
    LOG("sub_arr_size= %d", sub_arr_size);


    MPI_Scatter(&arr[0], SUBARR_SIZE, MPI_INT32_T,
                &sub_arr[0], SUBARR_SIZE, MPI_INT32_T, root, MPI_COMM_WORLD);

    qsort(&sub_arr[0], SUBARR_SIZE, sizeof(*sub_arr), &cmpfunc_int32);
    LOG_ARRAY(&sub_arr[0], sub_arr_size);

    int temp_size = size>>1;
    int temp_rank = rank;
    int32_t filled_sub_arr = SUBARR_SIZE;
    MPI_Status status;
    while (temp_rank<<1 < size)
    {
        recv_rank = rank + temp_size;

        if (recv_rank == rank)
            break;

        LOG("Before receive from %d", recv_rank);
        LOG_ARRAY(&sub_arr[0], sub_arr_size);
        LOG("Receive from %d", recv_rank);
        MPI_Recv(&sub_arr[filled_sub_arr], filled_sub_arr, MPI_INT32_T,
                 recv_rank, 0, MPI_COMM_WORLD, &status);
        LOG("After receive from %d", recv_rank);
        LOG_ARRAY(&sub_arr[0], sub_arr_size);

        merge_sorted_in_place(&sub_arr[0], filled_sub_arr, &tmp_buf[0]);
        LOG("After merge sorted:");
        LOG_ARRAY(&sub_arr[0], sub_arr_size);
        temp_size >>= 1;
        temp_rank <<= 1;
        filled_sub_arr <<= 1;

    }
    if (root == rank)
    {
        LOG("Final array:");
        LOG_ARRAY(&sub_arr[0], sub_arr_size);
        /* print_array(stdout, &sub_arr[0], sub_arr_size); */
    }
    else
    {
        send_rank = rank - temp_size;
        LOG("Send to %d", send_rank);
        MPI_Send(&sub_arr[0], filled_sub_arr, MPI_INT32_T,
                 send_rank, 0, MPI_COMM_WORLD);
        LOG_ARRAY(&sub_arr[0], filled_sub_arr);
    }


    if (root == rank)
    {
        free(arr);
    }
    free(sub_arr);
    free(tmp_buf);


#ifdef LOG_TO_FILE
    fclose(log_file);
#endif

    MPI_Finalize();
    return 0;
}

/* 
 * arr: 2 sorted arrays next to eachother, each of size arr_half_size 
 * tmp_buf: temporary buffer of size arr_half_size
 */
void merge_sorted_in_place(int32_t *arr, int32_t arr_half_size, int32_t *tmp_buf)
{
    int32_t i = 0;
    int32_t l = 0;
    int32_t r = 0;
    int32_t *arr_in1;
    int32_t *arr_in2;

    memcpy(&tmp_buf[0], &arr[0], arr_half_size * sizeof(*arr));
    arr_in1 = &tmp_buf[0];
    arr_in2 = &arr[arr_half_size];

    LOG("merge_sorted_in_place: arr_half_size=%d, arr[2*arr_half_size], tmp_buf[arr_half_size] after copy", arr_half_size);
    LOG_ARRAY(&arr[0], 2*arr_half_size);
    LOG_ARRAY(&tmp_buf[0], arr_half_size);

    while (l < arr_half_size && r < arr_half_size)
    {
        if (arr_in1[l] < arr_in2[r])
        {
            arr[i] = arr_in1[l];
            ++l;
        }
        else
        {
            arr[i] = arr_in2[r];
            ++r;
        }
        ++i;
    }

    while (l < arr_half_size)
    {
        arr[i] = arr_in1[l];
        ++l;
        ++i;
    }

    while (r < arr_half_size)
    {
        arr[i] = arr_in2[r];
        ++r;
        ++i;
    }
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


void print_array(FILE *f, int32_t *arr, int32_t size)
{
    for (int32_t i = 0; i < size; ++i)
    {
        fprintf(f, "%d, ", arr[i]);
    }
    fprintf(f, "\n");
}

