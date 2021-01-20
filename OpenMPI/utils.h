#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdio.h>

void alloc_fill_array(int32_t **arr, int32_t size, int32_t max_val);
void print_array(FILE *f, int32_t *arr, int32_t size);
int cmpfunc_int32 (const void *a, const void *b);
void save_results(const char* filename, const void *arr, size_t elem_size, size_t count);

#endif /* UTILS_H */

