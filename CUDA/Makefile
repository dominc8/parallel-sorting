C_FLAGS := -Wall -Wextra -pedantic -I../utils -std=c99
CC := nvcc

debug:
	$(CC) $(C_FLAGS) -DSAVE_RESULT -DLOG_TO_FILE cuda_sort.cu ../utils/utils.c -o cuda_sort

release:
	$(CC) $(C_FLAGS) -DNDEBUG -Ofast cuda_sort.cu ../utils/utils.c -o cuda_sort

release_save:
	$(CC) $(C_FLAGS) -DSAVE_RESULT -DNDEBUG -Ofast cuda_sort.cu ../utils/utils.c -o cuda_sort

