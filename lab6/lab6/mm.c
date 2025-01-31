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
 * 
 * I used explicit free list allocator and first fit strategy to allocate and free my memory.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

//책에 있는 basic constants and macros for manipulating the free list 일단 쓰고 시작
/* Basic constants and macros */
#define WSIZE 4               /* Word and header/footer size (bytes) */
#define DSIZE 8               /* Double word size (bytes) */
#define CHUNKSIZE (1<<12)     /* Extend heap by this amount (bytes) */

#define MAX(x, y) ((x) > (y)? (x) :(y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))
/* Read and write a word at address p */
#define GET(p)            (*(unsigned int *)(p))
#define PUT(p, val)       (*(unsigned int *)(p) = (val))
/* Read the size and allocated fields from address p */
#define GET_SIZE(p)       (GET(p) & ~0x7)
#define GET_ALLOC(p)      (GET(p) & 0x1)
/* Get and set prev and next ptr from address p */
#define GET_PREV(p)       (*(unsigned int *)(p))
#define GET_NEXT(p)       (*((unsigned int *)(p) + 1))
#define SET_PREV(p, val)  (*(unsigned int *)(p) = (val))
#define SET_NEXT(p, val)  (*((unsigned int *)(p) + 1) = (val))
/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)          ((char *)(bp) - WSIZE)
#define FTRP(bp)          ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)
/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)     ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)     ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

char *heap_ptr;
char *list_head;

//#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))
//#define HDR_SIZE (ALIGN(sizeof(struct header)))
// struct header { //double linked list를 위한 header
//     struct header *next_p;
//     struct header *before_p;
//     size_t size;
// };
// 룰을 다 안 읽었어서 struct를 쓰면 안되는 것을 인제 알았다.

void *extend_heap(size_t words);
void list_insert(void *bp); //block을 free list의 맨 처음에 넣는 함수
void list_remove(void *bp); //block을 free list에서 remove 하는 함수
void *coalesce(void *bp); //인접한 빈 블락 합치는 함수
void *find_fit(size_t size); //free list에서 size의 block을 찾는 함수
void print_heap(); //iterate through the entire heap 하는 함수(gdb에서 check용)
void place(void *bp, size_t size);

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{   //allocating the initial heap area.
    //return value should be -1 if 초기화하는데 problem 있으면. 아니면 0
    if((heap_ptr = mem_sbrk(WSIZE * 4)) == (void *)(-1)) return -1; //초기화에 문제
    PUT(heap_ptr, 0); //alignment padding
    PUT(heap_ptr + WSIZE, PACK(DSIZE, 1)); //prologue header
    PUT(heap_ptr + WSIZE * 2, PACK(DSIZE, 1)); //prologue footer
    PUT(heap_ptr + WSIZE * 3, PACK(0, 1)); //epilogue header
    heap_ptr = heap_ptr + WSIZE * 2;
    list_head = NULL;
    if(extend_heap(CHUNKSIZE / WSIZE) == NULL) return -1; //extend가 안될 때
    return 0;
}

void *extend_heap(size_t words) { //word 개수만큼 wsize를 곱해서 heap size extend하는 함수
    char *bp;
    size_t size;
    if(words % 2 == 1) size = WSIZE * (words + 1); //alignment rule을 맞추기 위해 짝수인지 보기
    else size = WSIZE * words;
    if((long)(bp = mem_sbrk(size)) == -1) return NULL;
    //free block의 header와 footer 초기화시켜주기
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    SET_PREV(bp, 0);
    SET_NEXT(bp, 0);
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    return coalesce(bp); //prev block이 free 상태이면 coalesce 시켜주기
}

void list_insert(void *bp) { //block을 free list에 insert시키는 함수
    if(bp == NULL) return;
    if(list_head == NULL) { //list가 비어 있을 때
        list_head = bp;
        return;
    }
    SET_NEXT(bp, list_head); //linked list 이어주기
    SET_PREV(list_head, bp);
    list_head = bp;
}

void list_remove(void *bp) { //block을 free list에서 remove시키는 함수
    if(bp == NULL) return;
    if(GET_ALLOC(HDRP(bp))) return;
    void *prev = GET_PREV(bp);
    void *next = GET_NEXT(bp);
    SET_PREV(bp, 0);
    SET_NEXT(bp, 0);
    //block이 하나밖에 없으면 list_head = NULL
    if(prev == NULL && next == NULL) list_head = NULL;
    else if(prev == NULL) { //이전것만 없을 때 list이어주기
        SET_PREV(next, 0);
        list_head = next;
    } else if(next == NULL) { //다음것만 없을 때 list이어주기
        SET_NEXT(prev, 0);
    } else { //둘다 없을 때 list 이어주기
        SET_NEXT(prev, next);
        SET_PREV(next, prev);
    }
}

void *coalesce(void *bp) { //인접한 empty blocks들을 합치기
    void *prev = PREV_BLKP(bp); //이전 block_ptr
    void *next = NEXT_BLKP(bp); //다음 block_ptr
    size_t prevalloc = GET_ALLOC(FTRP(prev));
    size_t nextalloc = GET_ALLOC(HDRP(next));
    size_t size = GET_SIZE(HDRP(bp));
    if(prevalloc && nextalloc) {} //양쪽 다 존재하면 그냥 넘어가기
    else if(!prevalloc && nextalloc) { //prev부분이 비어있으면 없애고 list 이어주기
        list_remove(prev);
        size = size + GET_SIZE(HDRP(prev));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(prev), PACK(size, 0));
        bp = prev;
    } else if(prevalloc && !nextalloc) { //next 부분이 비어있으면 없애고 list 이어주기
        list_remove(next);
        size = size + GET_SIZE(HDRP(next));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    } else { //양쪽 다 비어있으면 다 없애고 list 이어주기
        list_remove(prev);
        list_remove(next);
        size = size + GET_SIZE(HDRP(prev)) + GET_SIZE(FTRP(next));
        PUT(HDRP(prev), PACK(size, 0));
        PUT(FTRP(next), PACK(size, 0));
        bp = prev;
    }
    list_insert(bp);
    return bp;
}

void print_heap() { //gdb에서 run 시킨다음 allocated block들 확인용
    void *bp = mem_heap_lo(); //first block
    //navigate through the heap and print out
//    char *block_state = (p->size & 1) ? "allocated" : "free";
    while (bp < mem_heap_hi()) {
        printf("%s block at %p, size %d\n", (GET_SIZE(bp) & 1) ? "allocated" : "free", bp, (int)(GET_SIZE(bp) & ~1));
        bp = (void *)((char *)bp + (GET_SIZE(bp) & ~1));
    }
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{   //return a pointer to an allocated block payload of at least size bytes.
    size_t adjustsize; // adjust된 block size
    size_t extendsize; // no fit일 때 heap을 extend 시켜야 할 양
    char *bp;
    if(size == 0) return NULL;
    if(size <= DSIZE) adjustsize = 2 * DSIZE; //head를 포함하는 DSIZE보다 작으면 adjust해주기
    else adjustsize = ((size + DSIZE + (DSIZE - 1)) / DSIZE) * DSIZE;
    //block이 있으면
    if((bp = find_fit(adjustsize)) != NULL) {
        place(bp, adjustsize);
        return bp;
    }
    //block이 없으면(no fit)
    extendsize = MAX(adjustsize, CHUNKSIZE);
    if((bp = extend_heap(extendsize / WSIZE)) == NULL) return NULL;
    place(bp, adjustsize);
    return bp;
}

void *find_fit(size_t size) { //free list에 given size의 free block을 찾는 함수
    for (void *bp = list_head; bp != 0; bp = GET_NEXT(bp)) {
        if(GET_SIZE(HDRP(bp)) >= size) return bp;
    }
    return NULL;
}

void place(void *bp1, size_t adjustsize) { //remove the block pointer from free list
    //remaining part가 smallest block 이상이면 split 할 생각.
    size_t size = GET_SIZE(HDRP(bp1));
    list_remove(bp1);
    if(size - adjustsize >= 16) { //16 = smallest block size
        PUT(HDRP(bp1), PACK(adjustsize, 1));
        PUT(FTRP(bp1), PACK(adjustsize, 1));
        void *bp2 = NEXT_BLKP(bp1);
        PUT(HDRP(bp2), PACK(size - adjustsize, 0));
        PUT(FTRP(bp2), PACK(size - adjustsize, 0));
        SET_PREV(bp2, 0);
        SET_NEXT(bp2, 0);
        coalesce(bp2);
    } else { //split 안하는 경우
        PUT(HDRP(bp1), PACK(size, 1));
        PUT(FTRP(bp1), PACK(size, 1));
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    if(ptr == NULL) return;
    size_t size = GET_SIZE(HDRP(ptr));
    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    SET_PREV(ptr, 0);
    SET_NEXT(ptr, 0);
    coalesce(ptr); //양쪽 비우고 인접한 빈 block들 합치기
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size) 
{   
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;
    size = GET_SIZE(HDRP(oldptr));
    copySize = GET_SIZE(HDRP(newptr));
    if (size < copySize)
        copySize = size;
    memcpy(newptr, oldptr, copySize - WSIZE);
    mm_free(oldptr);
    return newptr;
}














