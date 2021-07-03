#include "mm.h"
#include "memlib.h"
#include "csapp.h"
//#include <stddef.h>

int main(int argc, char ** argv){
    printf("P2 should be different with P4. \n");
    size_t size = 4;
	int value;
    void *p1 = mm_malloc(size);
	value = *(int*)(p1-4) & 0x3;
	printf("header = %#x \n", value);
	printf("p1 = %d \n", p1);
	void *p2 = mm_malloc(size);
	value = *(int*)(p2-4) & 0x3;
	printf("header = %#x \n", value);
	printf("p2 = %d \n", p2);
	void *p3 = mm_malloc(size);
	value = *(int*)(p3-4) & 0x3;
	printf("header = %#x \n", value);
	printf("p3 = %d \n", p3);
	mm_free(p2);
	void *p4 = mm_malloc(size);
	value = *(int*)(p4-4) & 0x3;
	printf("header = %#x \n", value);
	printf("p4 = %d \n", p4);
	mm_free(p1);
	mm_free(p3);
	mm_free(p4);

	exit(0);
}