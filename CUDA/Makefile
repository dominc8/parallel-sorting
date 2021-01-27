C_FLAGS := -I../utils
CC := nvcc

debug:
	$(CC) $(C_FLAGS) cuda_sort.cu ../utils/utils.c -o cuda_sort

debug_file:
	$(CC) $(C_FLAGS) -DLOG_TO_FILE cuda_sort.cu ../utils/utils.c -o cuda_sort

release:
	$(CC) $(C_FLAGS) -DNDEBUG -O3 cuda_sort.cu ../utils/utils.c -o cuda_sort

