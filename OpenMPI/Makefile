debug:
	mpicc mpi_sort.c -o mpi_sort

release:
	mpicc -DNDEBUG -Ofast mpi_sort.c -o mpi_sort

run:
	mpirun --use-hwthread-cpus mpi_sort

