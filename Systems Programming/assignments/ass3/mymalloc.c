#include "mymalloc.h"

static mementry *head;

void* mymalloc(size_t numBytes, const char *filename, const int lineNumber)
{
    if(!head){
        // jordano
    }

    mementry *prev = NULL;
    mementry *curr = head;
    char *nextBlock = NULL;
    while(curr)
    {
        size_t diff = curr->numBytes - numBytes;

        if(curr->isFree){
            if(diff == 0){
                curr->isFree = 0;
                return curr->ptr;
            }
            else if(diff > MIN_BLOCK_SIZE){
                nextBlock = (char*) curr->ptr;
                nextBlock += numBytes;
                nextBlock = (mementry*) nextBlock;

                editMementry(nextBlock, (curr->numBytes) - (numBytes) - (sizeof(mementry)),
                    1, nextBlock + sizeof(mementry), curr->next);

                editMementry(curr, numBytes, 0, curr->ptr, nextBlock);

                return curr->ptr;
            }
            else if(diff > 0){
                curr->isFree = 0;
                return curr->ptr;
            }
        }

        prev = curr;
        curr = curr->next;
    }

    curr = (mementry*) sbrk(sizeof(mementry) + numBytes);

    if(errno == 12){
        return NULL;
    }

    editMementry(curr, numBytes, 0, (curr +1), NULL);

    prev->next = curr;

    return curr->ptr;
}

myrealloc(void *ptr, size_t numBytes, const char *filename, const int lineNumber)
{

}

mycalloc(size_t numItems, size_t size, const char *filename, const int lineNumber)
{

}

myfree(void *ptr, const char *filename, const int lineNumber)
{

}

void editMementry(mementry *m, size_t numBytes, int isFree, void *ptr, mementry *next)
{
    m->numBytes = numBytes;
    m->isFree = isFree;
    m->ptr = ptr;
    m->next = next;
}
