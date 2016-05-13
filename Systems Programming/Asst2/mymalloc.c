/**************************************************************
* This is the source code for the custom malloc library "mymalloc".
* Authors: Jordan Lyon and Biggie Emmanuel
***************************************************************/

#include <unistd.h>
#include<string.h>
#include<stdio.h>
#include "mymalloc.h"

/*
* This number represents the smallest # of bytes for a block of memory
* that can be broken off from a larger block of free memory.
*
* This is done because creating really small chunks of memory
* in between allocated blocks can lead to more fragmentation.
*/
#define MIN_BLOCK_SIZE 100
/* max # of bytes that is to be given out */
#define MAX_MEMORY 10000

/*
* This struct acts as a descriptor for a block
* of memory in the heap by holding the properties of the block.
* All mementries form a linked list to allow traversal of
* all the memory blocks.
*
* numBytes: Number of bytes in the block
* isFree: set to any nonzero number if the block is free and 0 otherwise.
* ptr: A ptr to where the block memory begins.
* next: a pointer to the next mementry in the heap.
*/
typedef struct mementry_
{
    size_t numBytes;
    int isFree;
    void *ptr;
    struct mementry_ *next;
} mementry;

/* Pointer to the first memory block in the heap */
static mementry *head;

/* forward declarations */
mementry* findEntry(void *ptr);
void editMementry(mementry *m, size_t numBytes,
    int isFree, void *ptr, mementry *next);
int cleanup(mementry *m, int place);
void printHeap();
void* mymalloc(size_t numBytes, const char *filename, const int lineNumber);
void* myrealloc(void *ptr, size_t numBytes,
    const char *filename, const int lineNumber);
void* mycalloc(size_t numItems, size_t size,
    const char *filename, const int lineNumber);
void myfree(void *ptr, const char *filename, const int lineNumber);

void* mymalloc(size_t numBytes, const char *filename, const int lineNumber)
{
    /* no mementries; the heap is empty */
    if(!head){
        head = (mementry*) sbrk(sizeof(mementry) + numBytes);
        if(head == (void*)-1){
            printf("Ran out of memory on line: %d in file: %s\n",
                lineNumber, filename);
            head = NULL;
            return NULL;
        }

        editMementry(head, numBytes, 0, head + 1, NULL);
        return head->ptr;
    }

    mementry *prev = NULL;
    mementry *curr = head;
    while(curr)
    {
        // diff in size between blocks
        size_t diff = curr->numBytes - numBytes;

        if(curr->isFree)
        {
            /* perfect size block; will be given to user */
            if(diff == 0){
                curr->isFree = 0;
            }
            /* very large block; must be segmented into 2 */
            else if(diff > MIN_BLOCK_SIZE){
                /* create mementry for the new free segment */
                mementry *freeBlock = curr;
                // advance ptr past mementry and new user memory
                freeBlock += 1;
                char *temp = ((char *) freeBlock) + numBytes; //move by bytes
                freeBlock = (mementry*) temp;
                // create the mementry for new free block
                editMementry(freeBlock,
                    (curr->numBytes) - (numBytes) - (sizeof(mementry)),
                    1, freeBlock + 1, curr->next);

                /* update the old block's mementry */
                editMementry(curr, numBytes, 0, curr->ptr, freeBlock);
            }
            /* slightly larger block; give entire block to user*/
            else if(diff > 0){
                curr->isFree = 0;
            }

            return curr->ptr;
        }

        prev = curr;
        curr = curr->next;
    }

    /* allocate memory on the heap */
    curr = (mementry*) sbrk(sizeof(mementry) + numBytes);
    if(curr == (void*)-1){
        printf("Ran out of memory on line: %d in file: %s\n",
            lineNumber, filename);
        return NULL;
    }
    editMementry(curr, numBytes, 0, (void*) (curr + 1), NULL);
    prev->next = curr; // point last block to new block

    return curr->ptr;
}

void* myrealloc(void *ptr, size_t numBytes,
    const char *filename, const int lineNumber)
{
    void* block = malloc(numBytes);
    if(!block){
        return NULL;
    }

    /* TODO:
     * The amount copied by memcpy() should be the smaller value
     * between the number bytes in the users previous memory block
     * and the number of bytes they would like to realloc() for.
     *
     * This should be done to handle the case where a user wants
     * to realloc() for a smaller block of memory and thus copying over
     * all data from the previous block would result in undefined
     * behavior (most likely a segmentation fault).
     */
    memcpy(block, ptr, findEntry(ptr)->numBytes); // copy old data
    free(ptr);

    return block;
}

void* mycalloc(size_t numItems, size_t size,
    const char *filename, const int lineNumber)
{
    size_t numBytes = (numItems * size);

    void *block = malloc(numBytes);
    if(!block){
        return NULL;
    }

    memset(block, 0, numBytes); //clear any junk data

    return block;
}

void myfree(void *ptr, const char *filename, const int lineNumber)
{
    mementry *prev = NULL;
    mementry *curr = head;
    while(curr)
    {
        if(curr->ptr == ptr){
            if(curr->isFree){
                printf("ERROR pointer already free'd on line: %d, in file: %s\n",
                    lineNumber, filename);

                return;
            }
            curr->isFree = 1;

            // combine 2 free blocks if possible
            if(curr->next && curr->next->isFree){
                editMementry(curr,
                    (curr->numBytes + curr->next->numBytes + sizeof(mementry)),
                    1, curr->ptr, curr->next->next);
            }
            if(prev && prev->isFree){
                editMementry(prev,
                    (prev->numBytes + curr->numBytes + sizeof(mementry)),
                    1, prev->ptr, curr->next);
            }

            // heap cleanup
            cleanup(head, 0);

            return;
        }

        prev = curr;
        curr = curr->next;
    }

    printf("ERROR Pointer not given by malloc on line: %d, in file: %s\n",
        lineNumber, filename);

    return;
}

/******************** Helper Methods ********************/

/*
* change fields in a mementry
*/
void editMementry(mementry *m, size_t numBytes,
    int isFree, void *ptr, mementry *next)
{
    m->numBytes = numBytes;
    m->isFree = isFree;
    m->ptr = ptr;
    m->next = next;

    return;
}

/*
* find a mementry given the ptr to its data block
*/
mementry* findEntry(void *ptr)
{
    mementry *curr = head;
    while(curr)
    {
        if(curr->ptr == ptr){
            return curr;
        }

        curr = curr->next;
    }

    return NULL;
}

/* So if the user frees everything there should be no mem leaks right?*/
/* The following is a way of ensuring this*/
/* for future reference: 2 = null, 1 = free, 0 = used */
int cleanup(mementry *m, int place)
{
    if(!m){
        return 2;
    }

    int i = cleanup(m->next, place + 1);
    if(i == 0){ //thing above wasnt free
        return 0;
    }
    else if(i == 1){ //thing above was free
        sbrk( (m->next->numBytes + sizeof(mementry)) * -1 ); // remove from heap
        m->next = NULL; // remove from mementry linked list

        //im first, the thing above is free, im free, so i should free both
        if(m->isFree && place == 0){
            sbrk( (m->numBytes + sizeof(mementry)) * -1 );
            head = NULL;
            return 0;
        }

        return m->isFree;
    }

    if(m->isFree && place == 0){ //im the only block and free
        sbrk( (m->numBytes + sizeof(mementry)) * -1 );
        head = NULL;
        return 0;
    }

    return m->isFree; //thing above was null
}

/* prints the heap contents */
void printHeap()
{
    mementry *curr = head;
    printf("%s\n", (curr) ? "Heap contents:\n" : "Heap is Empty");
    int i = 1;
    while(curr)
    {
        printf("Block %d: free: %s\tbytes: %zu\n", i++,
            (curr->isFree) ? "yes" : "no",
            curr->numBytes);

        curr = curr->next;
    }

    return;
}
