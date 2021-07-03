#include "mm.h"
#include "memlib.h"
#include "csapp.h"
//#include <stddef.h>

int main(int argc, char ** argv){
    printf("P2 should be different with P4. \n");
    size_t size = 64;
    void *p1 = mm_malloc(size);
	printf("p1 = %#x\n", p1);
	void *p2 = mm_malloc(size);
	printf("p2 = %#x\n", p2);
	void *p3 = mm_malloc(size);
	printf("p3 = %#x\n", p3);
	mm_free(p2);
	void *p4 = mm_malloc(size);
	printf("p4 = %#x\n", p4);
	mm_free(p1);
	mm_free(p3);
	mm_free(p4);

	exit(0);
}