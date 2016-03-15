/**************************************************************
* "mymalloc" is a custom malloc library with methods
* for dynamically allocating, reallocting, and freeing memory from the heap.
*
* Authors: Jordan Lyon and Biggie Emmanuel
***************************************************************/

#ifndef my_malloc_h
#define my_malloc_h

#include <stdlib.h>

/*
* it mallocs
*/
void* mymalloc(size_t numBytes, const char *filename, const int lineNumber);

/*
* it reallocs
*/
void* myrealloc(void *ptr, size_t numBytes,
    const char *filename, const int lineNumber);

/*
* it callocs
*/
void* mycalloc(size_t numItems, size_t size,
    const char *filename, const int lineNumber);

/*
* it frees
*/
void myfree(void *ptr, const char *filename, const int lineNumber);

/*
* Macros which add the filename and line number to the arguments
* that are to be passed to these functions.
*/
#define malloc(x) mymalloc((x), __FILE__, __LINE__)
#define realloc(x, y) myrealloc((x), (y), __FILE__, __LINE__)
#define calloc(x, y) mycalloc((x), (y), __FILE__, __LINE__)
#define free(x) myfree((x), __FILE__, __LINE__)

#endif
