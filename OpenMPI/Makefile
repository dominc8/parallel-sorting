debug:
	mpicc -Wall -Wextra -pedantic mpi_sort.c -o mpi_sort

release:
	mpicc -Wall -Wextra -pedantic -DNDEBUG -Ofast mpi_sort.c -o mpi_sort

run:
	mpirun --use-hwthread-cpus mpi_sort

