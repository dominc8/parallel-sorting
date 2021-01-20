C_FLAGS := -Wall -Wextra -pedantic

debug:
	mpicc $(C_FLAGS) -DSAVE_RESULT -DLOG_TO_FILE mpi_sort.c utils.c -o mpi_sort

release:
	mpicc $(C_FLAGS) -DNDEBUG -Ofast mpi_sort.c utils.c -o mpi_sort

release_save:
	mpicc $(C_FLAGS) -DSAVE_RESULT -DNDEBUG -Ofast mpi_sort.c utils.c -o mpi_sort

run:
	mpirun --use-hwthread-cpus mpi_sort

compare: compare_bin release_save
	mpirun --use-hwthread-cpus mpi_sort
	./compare_sort
	diff --brief --report-identical-files result result_ref

compare_bin:
	gcc $(C_FLAGS) -DNDEBUG -Ofast compare_sort.c utils.c -o compare_sort


