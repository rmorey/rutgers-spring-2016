things to mention:

Extra Credit:
----------------------------------------------
- we did realloc

- we did calloc

- we dealt lessened the fragmentation by giving the user an entire mem block as opposed to a segment if it meant that the free segment would be rly small and unlikely to be malloc'd for

- we use sbrk as opposed to a static char array
- (maybe this will be EC?) we actually free the heap as well using sbrk so there are no mem leaks if the user free everything


How it works
------------------------------------------
-we use mementries as descriptors for each block of memory in the heap, free or allocated, and they form a linked list so that we can traverse them

malloc:
- we use sbrk to extend the heap and fill malloc requests (this is only done if we cant find a free block in the heap initially)

calloc:
- we use memset to 0 out the data we malloc

realloc:
- we use malloc and memcpy to copy the data over

free:
- we search for the pointer they gave us in the mementries
- after a successful free we look at the top of the heap for any free memory and remove it from the heap using sbrk. this way if everything gets freed there will be no memory leaks.
