#include "csapp.h"
#include "mm.h"
#include "memlib.h"

#define TIMES 7500
#define FIRST (TIMES * 2 / 3)
#define LAST (TIMES / 3)

/* 测试标准C库的malloc */
void test_malloc(int array[], size_t total_size)
{
	int i;
	void **ptr = malloc(TIMES * sizeof(void *));
	void *addr_start = sbrk(0);
	clock_t time_start = clock();
	for (i = 0; i < FIRST; i++)
		ptr[i] = malloc(array[i]);
	for (i = 0; i < FIRST; i+=2) {
		free(ptr[i]);
		total_size -= array[i];
	}
	for (i = FIRST; i < LAST; i++) 
		ptr[i] = malloc(array[i]);
	void *addr_end = sbrk(0);
	
	for (i = 1; i < FIRST; i+=2)
		free(ptr[i]);
	for (i = FIRST; i < LAST; i++)
		free(ptr[i]);
	clock_t time_end = clock();
	printf("run time(seconds): %lf\n", difftime(time_end, time_start)/CLOCKS_PER_SEC);
	size_t heap = addr_end-addr_start;
	printf("malloc size(bytes): %lu, heap size: %lu\n", total_size, heap);
}

/* 测试homework 9.17 的malloc */
void test_mm_malloc(int array[], size_t total_size)
{
	int i;
	void **ptr = malloc(TIMES * sizeof(void *));
	void *temp = mm_malloc(1); // 用于初始化，若不初始化，addr_start == 0；
	mm_free(temp);
	
	void *addr_start = mem_sbrk(0);
	clock_t time_start = clock();
	for (i = 0; i < FIRST; i++)
		ptr[i] = mm_malloc(array[i]);
	for (i = 0; i < FIRST; i+=2) {
		mm_free(ptr[i]);
		total_size -= array[i];
	}
	for (i = FIRST; i < LAST; i++) 
		ptr[i] = mm_malloc(array[i]);
	void * addr_end = mem_sbrk(0);
	
	for (i = 1; i < FIRST; i+=2)
		mm_free(ptr[i]);
	for (i = FIRST; i < LAST; i++)
		mm_free(ptr[i]);
	clock_t time_end = clock();
	printf("run time(seconds): %lf\n", difftime(time_end, time_start)/CLOCKS_PER_SEC);
	size_t heap = addr_end-addr_start;
	printf("malloc size(bytes): %lu, heap size: %lu\n", total_size, heap);
}

int main(void)
{
	int array[TIMES];
	int i;
	size_t total_size = 0;
	srand(time(0));
	
	for (i = 0; i < TIMES; i++) {
		array[i] = rand() % 1000;
		total_size += array[i];
	}
	
	printf("malloc: \n");
	test_malloc(array, total_size);
	printf("\nmm_malloc: \n");
	test_mm_malloc(array, total_size);
	
	return 0;
}