#include "utils.h"
#include <stdlib.h>
#include <sys/time.h>

static double timestamp_start, timestamp_end;

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

void save_results(const char* filename, const void *arr, size_t elem_size, size_t count)
{
    FILE *result_file = fopen(filename, "wb");
    if (result_file != NULL)
    {
        fwrite(arr, elem_size, count, result_file);
        fclose(result_file);
    }
}


void start_time(void)
{

  /* Get current time */
  struct timeval ts;
  gettimeofday(&ts, (struct timezone*)NULL);

  /* Store time-stamp in micro-seconds */
  timestamp_start = (double)ts.tv_sec * 1000000.0 + (double)ts.tv_usec;

}

void stop_time(void)
{

  /* Get current time */
  struct timeval ts;
  gettimeofday(&ts, (struct timezone*)NULL);

  /* Store time-stamp in microseconds */
  timestamp_end = (double)ts.tv_sec * 1000000.0 + (double)ts.tv_usec;

}

double elapsed_time_ms(void)
{
  double time_diff;

  /* Compute difference */
  time_diff = timestamp_end - timestamp_start;

  return time_diff <= 0.0 ? 0.0 : time_diff / 1000.0;
}


int cmpfunc_int32 (const void *a, const void *b)
{
    return ( *(int32_t *)a - *(int32_t *)b);
}

