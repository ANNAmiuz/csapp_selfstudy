#include <stddef.h>

void * heap_listp;
static void * find_fit(size_t asize){
	//first-fit method
	void * bp;
	for (bp=heap_listp; GET_SIZE(HDRP(bp))>0; bp=NEXT_BLKP(bp)){
		if (!GET_ALLOC(HDRP(bp) && GET_SIZE(HDRP(bp))>asize))
			return bp;
	}
	return NULL;
}

