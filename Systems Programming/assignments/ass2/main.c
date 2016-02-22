#include <stdio.h>
#include <stdlib.h>
#include "sorted-list.h"

// Debug Macros
#define debugc(x) printf(#x " is: %c\n", x)
#define debugs(x) printf(#x " is: %s\n", x)
#define debugi(x) printf(#x " is: %d\n", x)
#define debugf(x) printf(#x " is: %f\n", x)

// Debug Methods
void printList(SortedListPtr sl)
{
    Node *ptr = sl->list;

    printf( (ptr) ? "List contents:\n" : "The list is empty" );

    while(ptr)
    {
        char *i = (char*) ptr->data;
        printf("DATA ADDRESS: %s\tNEXT ADDRESS: %p\n", i, ptr->next);

        ptr = ptr->next;
    }
}

void printNode(Node* ptr)
{
    printf( (ptr) ? "List contents:\n" : "The list is empty" );

    while(ptr)
    {
        char *i = (char*) ptr->data;
        printf("DATA: %s\tNEXT ADDRESS: %p\n", i, ptr->next);

        ptr = ptr->next;
    }
}

/*
 * compare function
 */
int cmp(void* a, void* b)
{
    char *first = (char*) a;
    char *sec = (char*) b;

    if((*first) == (*sec))
    {
        return 0;
    }

    return ((*first) > (*sec)) ? 1 : -1;
}

/*
 * destroy function
 */
void des(void* a)
{
    return;
}

void printList(SortedListPtr slp);

int main(int argc, char* argv[])
{
    const char *one = "apple";
    const char *two = "bear";
    const char *three = "cat";
    const char *four = "dog";

    SortedListPtr slp = SLCreate(cmp, des);

    SLInsert(slp, (void*) one);
    SLInsert(slp, (void*) two);
    SLInsert(slp, (void*) three);
    SLInsert(slp, (void*) four);

    SortedListIteratorPtr iter = SLCreateIterator(slp);

    debugs((char*) SLNextItem(iter)); debugs((char*) SLNextItem(iter));
    SLRemove(slp, (void*) three);
    SLRemove(slp, (void*) two);
    printNode(iter->currentItem); // should be cat

    debugs((char*) SLNextItem(iter)); //should break or bear
    debugs((char*) SLNextItem(iter)); //apple

    SLDestroy(slp);
    return EXIT_SUCCESS;
}
