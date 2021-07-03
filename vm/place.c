#include <stddef.h>

#define DSIZE 8

void place(void*bp, size_t asize){
    size_t cur_size = GET_SIZE(HDRP(bp));
    if (cur_size - asize >= 2*DSIZE){
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(cur_size - asize, 1));
        PUT(FTRP(bp), PACK(cur_size - asize, 1));
    }
    else{
        PUT(HDRP(bp), PACK(cur_size, 1));
        PUT(FTRP(bp), PACK(cur_size, 1));
    }
}