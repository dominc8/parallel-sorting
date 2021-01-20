# parallel-sorting
Application that performs parallel sorting in 3 variants: OpenMP, OpenMPI and CUDA.

## OpenMPI
Pass N_PROC variable to *make* set number of processors/threads to use (has to be power of 2).

To compare mpi output with reference run command below (uses only physical cores)
```
make compare
```
To use also logical cores run
```
make compare_hw
```

