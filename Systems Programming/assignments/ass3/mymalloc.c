/**************************************************************
* This is the source code for the custom malloc library "mymalloc".
* Authors: Jordan Lyon and Biggie Emmanuel
***************************************************************/

#include <unistd.h>
#include "mymalloc.h"

/*
* This number represents the smallest # of bytes for a block of memory
* that can be broken off from a larger block of free memory.
*
* This is done because creating really small chunks of memory
* in between allocated blocks can lead to more fragmentation.
*/
#define MIN_BLOCK_SIZE 100

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

void* mymalloc(size_t numBytes, const char *filename, const int lineNumber)
{
    if(!head){
        head = (mementry*) sbrk(sizeof(mementry) + numBytes);
        editMementry(head, numBytes, 1, head + sizeof(mementry), NULL);
        return head->ptr;
    }

    mementry *prev = NULL;
    mementry *curr = head;
    char *nextBlock = NULL;
    while(curr)
    {
        size_t diff = curr->numBytes - numBytes;

        if(curr->isFree)
        {
            if(diff == 0){
                curr->isFree = 0;
            }
            else if(diff > MIN_BLOCK_SIZE){
                nextBlock = (char*) curr->ptr;
                nextBlock += numBytes;
                nextBlock = (mementry*) nextBlock;

                editMementry(nextBlock,
                    (curr->numBytes) - (numBytes) - (sizeof(mementry)),
                    1,
                    nextBlock + sizeof(mementry), curr->next);

                editMementry(curr, numBytes, 0, curr->ptr, nextBlock);
            }
            else if(diff > 0){
                curr->isFree = 0;
            }

            return curr->ptr;
        }

        prev = curr;
        curr = curr->next;
    }

    curr = (mementry*) sbrk(sizeof(mementry) + numBytes);
    if(curr == (void*)-1){
        return NULL;
    }

    editMementry(curr, numBytes, 0, (curr + 1), NULL);

    prev->next = curr;

    return curr->ptr;
}

void* myrealloc(void *ptr, size_t numBytes,
    const char *filename, const int lineNumber)
{
    void* block = malloc(numBytes);
    if(!block){
        return NULL;
    }

    memcpy(block, ptr, findEntry(ptr)->numBytes);
    free(ptr);

    return block;
}

void* mycalloc(size_t numItems, size_t size,
    const char *filename, const int lineNumber)
{
    size_t numBytes = (numItems * size);

    void *block = malloc(numBytes);
    if(!block){
        return NULL:
    }

    memset(block, 0, numBytes);

    return block;
}

void myfree(void *ptr, const char *filename, const int lineNumber)
{
    mementry *curr = head;
    while(curr)
    {
        if(curr->ptr == ptr){
            curr->ptr->isFree = 1;
        }

        curr = curr->next;
    }

    return;
}

/******************** Helper Methods ********************/

/*
*
*/
void editMementry(mementry *m, size_t numBytes,
    int isFree, void *ptr, mementry *next)
{
    m->numBytes = numBytes;
    m->isFree = isFree;
    m->ptr = ptr;
    m->next = next;
}

/*
*
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
