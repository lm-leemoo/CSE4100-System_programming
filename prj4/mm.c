/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your information in the following struct.
 ********************************************************/
team_t team = {
    /* Your student ID */
    "20211582",
    /* Your full name*/
    "Nahyun Lim",
    /* Your email address */
    "julialim5@sogang.ac.kr",
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))


#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1 << 12)
#define MAX(x, y) ((x) > (y)? (x) : (y))
#define PACK(size, alloc) ((size) | (alloc))
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))


#define NEXT_P(bp) (*(char **)((bp) + WSIZE))
#define PREV_P(bp) (*(char **)(bp))

/*private global variables*/
static void *heap_listp;
static char *free_listp;
static void *extend_heap(size_t words);
static void *find_fit(size_t asize);
static void add_fb(void *bp);
static void delete_fb(void *bp);
static void place(void* p, size_t size);
static void* coalesce(void* bp);

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
    if ((heap_listp = mem_sbrk(6* WSIZE)) == (void*)-1)
        return -1;
    PUT(heap_listp, 0);
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));  // Prologue header
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));  // Prologue footer
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));      // Epilogue header
    heap_listp += 2 * WSIZE;
    free_listp = heap_listp;
    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;
    
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void* mm_malloc(size_t size) //강의자료
{
    size_t asize;
    size_t extendsize;
    char* bp;
    if (size == 0) return NULL;
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE)+(DSIZE - 1)) / DSIZE);

    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    return bp;
}

static void* extend_heap(size_t words) { //강의자료
    char* bp;
    size_t size;
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
    return coalesce(bp);
}

static void *find_fit(size_t asize)
{
    void *bp;
    for(bp = free_listp; GET_ALLOC(HDRP(bp)) == 0; bp = NEXT_P(bp)){ //change
        if(GET_SIZE(HDRP(bp)) >= asize){
            return bp;
        }
    }
    return NULL;
}

static void delete_fb(void *bp)
{
    if(PREV_P(bp) == NULL) { //첫번째
        free_listp=NEXT_P(bp);
        PREV_P(free_listp) = NULL;
        return;
    }
    if (NEXT_P(bp) == NULL) {//마지막
        NEXT_P(PREV_P(bp)) = NEXT_P(bp);
        return;
    }
    PREV_P(NEXT_P(bp)) = PREV_P(bp);
    NEXT_P(PREV_P(bp)) = NEXT_P(bp);
}

static void add_fb(void *bp)
{
    void *temp = free_listp;
    free_listp = bp;
    PREV_P(bp) = NULL;
    NEXT_P(bp) = temp;
    if (temp != NULL){
        PREV_P(temp) = bp;
    }    
}

static void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));
    delete_fb(bp);

    if((csize - asize) >= (2*DSIZE)){ //일부사용
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        void* next_bp = NEXT_BLKP(bp);
        PUT(HDRP(next_bp), PACK(csize-asize, 0));
        PUT(FTRP(next_bp), PACK(csize-asize, 0));
        add_fb(next_bp);
    }
    else{ //전부사용
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}

static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp))) || PREV_BLKP(bp) == bp;
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if(prev_alloc && next_alloc){//case1
        add_fb(bp);
        return bp;
    }
    else if(prev_alloc && !next_alloc){//case2
        delete_fb(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }
    else if(!prev_alloc && next_alloc){//case3
        delete_fb(PREV_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        bp = PREV_BLKP(bp);
        PUT(HDRP(bp), PACK(size, 0));
    }
    else{ //case4
        delete_fb(PREV_BLKP(bp));
        delete_fb(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
        PUT(HDRP(bp), PACK(size, 0));
    }
    add_fb(bp);
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr) //강의자료
{
    size_t size = GET_SIZE(HDRP(ptr));
    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    size_t oldsize, newsize;
    void *oldptr = ptr;
    void *newptr;

    if (size <= 0) {
        mm_free(ptr);
        return NULL;
    }
    if (!oldptr) {
        return mm_malloc(size);
    }

    oldsize = GET_SIZE(HDRP(ptr));
    newsize = size + (2 * WSIZE);
    if (newsize <= oldsize) return ptr;
    if (!GET_ALLOC(HDRP(NEXT_BLKP(oldptr)))){
        size_t n_blk_size = GET_SIZE(HDRP(NEXT_BLKP(oldptr)));
        size_t co_fsize = oldsize + n_blk_size;
        delete_fb(NEXT_BLKP(oldptr));
        PUT(HDRP(oldptr), PACK(co_fsize, 1));
        PUT(FTRP(oldptr), PACK(co_fsize, 1));
        return oldptr;
    }
    else{
        newptr=mm_malloc(newsize);
        if(newptr == NULL) return NULL;

        place(newptr, newsize);
        memcpy(newptr, oldptr, oldsize);
        mm_free(oldptr);
        return newptr;
    }
}