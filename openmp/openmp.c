#include<omp.h>
#include<stdio.h>
#include<stdlib.h>
#include"my_timers.h"

#define MAX_SIZE 10000000
#define MAX_THREADS 32

int number_of_threads;

void combine(long* array, int startI, int halfI, int endI){
    // Define length of left and right part of an array
    int lenL = halfI - startI + 1;
    int lenR = endI - halfI;

    // Create sub-arrays and allocate memory
    long L[lenL], R[lenR];
    
    int i, j;

     for (i = 0; i < lenL; i++)
        L[i] = array[startI + i];   
     for (j = 0; j < lenR; j++)
        R[j] = array[halfI + j + 1];


    // Create indexes for sub-arrays (i,j) and main array (k)
    int k;
    i = 0;
    j = 0;
    k = startI;

    // Place elements in the correct positions
    while (i < lenL && j < lenR) {
        if (L[i] <= R[j]) {
            array[k] = L[i];
            i++;
        } else {
            array[k] = R[j];
            j++;
        }
        k++;
    }

    // Place remaining elements
    while (i < lenL) {
        array[k] = L[i];
        i++;
        k++;
    }   

    while (j < lenR) {
        array[k] = R[j];
        j++;
        k++;
    }

}

void divide(long* array, int startI, int endI){
    int halfI = 0;
    if(startI < endI){
        halfI = (startI + endI)/2;
        #pragma omp parallel sections
        {
            #pragma omp section
            divide(array, startI, halfI);
            #pragma omp section
            divide(array, halfI + 1, endI);               
        }
        combine(array, startI, halfI, endI);
    }
}


void merge_sort(long* array, int size){
      divide(array, 0, size - 1);
}

main(int argc, char *argv[]){
    if(argc == 2){
        FILE *in, *out;
        in = fopen("input.txt", "r");
        out = fopen("output.txt", "w");
        if(in == NULL || out == NULL){
            printf("Error reading from file\n");
        }else{
            char *a = argv[1];
            number_of_threads = atoi(a);
            if(number_of_threads > MAX_THREADS){
                number_of_threads = MAX_THREADS;
                printf("Too large number of threads, set to MAX (%d)\n", MAX_THREADS);
            }

            int i;
            int check_flag = 1;
            long *array;
            int size = 0;
            int val = 0;
            array = (long*)malloc(sizeof *array * (MAX_SIZE + 1));

            /* Read from file */
            char c = ' ';
            while(c != EOF){
                fscanf(in, "%d", &val);
                array[size] = val;
                size++;
                c = getc(in); 
            }
            
            omp_set_num_threads(number_of_threads);
            start_time();
            
            merge_sort(array, size);         

            stop_time();

            
            /* Save to file */
            for(i = 0; i < size - 1; i++){
                fprintf(out, "%d ", array[i]);
            }

            /* Check if output array is sorted */
            for(i = 0; i < size - 1; i++){
                if(array[i] > array[i+1]){
                    check_flag = 0;
                    printf("Problem at index %d\n", i);
                    break;
                }
            }

            if(check_flag){
                printf("Output OK\n");
            }else{
                printf("Oops, error in output (array is not sorted)\n");
            }
            print_time("Sorting algorithm time:");

            free(array);
            fclose(in);
            fclose(out);
        }
    }else{
        printf("Invalid number of arguments (provide number of threads).\n");
    }
}