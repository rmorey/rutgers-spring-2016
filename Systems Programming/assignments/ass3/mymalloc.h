#ifndef my_malloc_h
#define my_malloc_h

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

struct mementry_
{
    size_t numBytes;
    int isFree;
    void *ptr;
    struct mementry_ *next;
}
typedef struct mementry_ mementry;

#define malloc(x) mymalloc((x), __FILE__, __LINE__)
#define realloc(x, y) myrealloc((x), (y), __FILE__, __LINE__)
#define calloc(x, y) mycalloc((x), (y), __FILE__, __LINE__)
#define free(x) myfree((x), __FILE__, __LINE__)

#define MIN_BLOCK_SIZE (sizeof(mementry) + 20)

mymalloc(size_t numBytes, const char *filename, const int lineNumber);

myrealloc(void *ptr, size_t numBytes, const char *filename, const int lineNumber);

mycalloc(size_t numItems, size_t size, const char *filename, const int lineNumber);

myfree(void *ptr, const char *filename, const int lineNumber);

#endif
