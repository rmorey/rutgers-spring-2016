#include <stdio.h>
#include <stdlib.h>
#include "sorted-list.h"

// Debug Macros           
#define debugc(x) printf(#(x) " is: %c\n", (x))
#define debugs(x) printf(#(x) " is: %s\n", (x))
#define debugi(x) printf(#(x) " is: %d\n", (x))
#define debugf(x) printf(#(x) " is: %f\n", (x))                 

// Debug Methods          
void printList(SortedListPointer sl)
{                         
    Node *ptr = sl->list;

    printf( (ptr) ? "List contents:\n" : "The list is empty" );
    
    while(ptr)
    {
        printf("DATA ADDRESS: %p\tNEXT ADDRESS: %p\n", ptr->data, ptr->next);

        ptr = ptr->next;
    }
}   

int main(int argc, char* argv[])
{
    
}
