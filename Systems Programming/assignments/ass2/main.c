/*
 * Authors: Olaolu Emmanuel and Jordan Lyon
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sorted-list.h"

// Debug Macros
#define debugc(x) printf(#x " is: %c\n", x)
#define debugs(x) printf(#x " is: %s\n", x)
#define debugi(x) printf(#x " is: %d\n", x)
#define debugf(x) printf(#x " is: %f\n", x)

/////////////////////////////////// STRUCT TESTING UTILS //////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Point
{
    int *x;
    int *y;
};
typedef struct Point Point;

int cmpStruct(void* a, void* b)
{
    Point *c = (Point*) a;
    Point *d = (Point*) b;

    int sum1 = *(c->x) + *(c->y);
    int sum2 = *(d->x) + *(d->y);

    if(sum1 == sum2)
    {
        return 0;
    }

    return (sum1 > sum2) ? 1 : -1;
}

void desStruct(void* a)
{
    Point *b = (Point*) a;
    free(b->x);
    free(b->y);
    free(b);

    return;
}

/////////////////////////////////// STRING TESTING UTILS //////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int cmpString(void* a, void* b)
{
    char *c = (char*) a;
    char *d = (char*) b;

    if(strcmp(c, d) == 0)
    {
        return 0;
    }

    return (strcmp(c, d) > 0) ? 1 : -1;
}

void desString(void* a)
{
    char *b = (char*) a;
    free(b);

    return;
}

/////////////////////////////////// MAIN ///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    /*********** start string testing *************/
    printf("START STRING TEST SIM:\n");

    char *a = malloc(sizeof(char) * 100);
    strcpy(a, "apples");

    char *b = malloc(sizeof(char) * 100);
    strcpy(b, "riggityrow");

    char *c = malloc(sizeof(char) * 100);
    strcpy(c, "jazzyjeff");

    char *d = malloc(sizeof(char) * 100);
    strcpy(d, "barrel");

    SortedListPtr sl = SLCreate(cmpString, desString);

    //test insertion
    SLInsert(sl, (void*) c);
    SLInsert(sl, (void*) b);
    SLInsert(sl, (void*) d);
    SLInsert(sl, (void*) a);

    //test Iterator
    SortedListIteratorPtr si = SLCreateIterator(sl);
    debugs((char*) SLNextItem(si)); //riggityrow
    debugs((char*) SLNextItem(si)); //jazzyjeff
    SLRemove(sl, "barrel"); // iterator would normallly return this next but now will return apple
    debugs((char*) SLNextItem(si)); //apple
    debugs((char*) SLGetItem(si)); //apple
    debugs((char*) SLGetItem(si)); //apple
    SLDestroyIterator(si);

    SLDestroy(sl);

    printf("END STRING TEST SIM:\n");
    /*********** end string testing *************/

    /*********** start struct testing *************/
    printf("START STRUCT TEST SIM:\n");

    Point *e = (Point*) malloc(sizeof(Point));
    e->x = malloc(sizeof(int));
    e->y = malloc(sizeof(int));
    *(e->x) = 3;
    *(e->y) = 2;

    Point *f = (Point*) malloc(sizeof(Point));
    f->x = malloc(sizeof(int));
    f->y = malloc(sizeof(int));
    *(f->x) = 1;
    *(f->y) = 7;

    Point *g = (Point*) malloc(sizeof(Point));
    g->x = malloc(sizeof(int));
    g->y = malloc(sizeof(int));
    *(g->x) = 23;
    *(g->y) = 98;

    Point *h = (Point*) malloc(sizeof(Point));
    h->x = malloc(sizeof(int));
    h->y = malloc(sizeof(int));
    *(h->x) = 30;
    *(h->y) = 30;

    sl = SLCreate(cmpStruct, desStruct);

    //test insertion
    SLInsert(sl, (void*) h);
    SLInsert(sl, (void*) e);
    SLInsert(sl, (void*) f);
    SLInsert(sl, (void*) g);

    //test iterator
    si = SLCreateIterator(sl);
    Point *temp = (Point*) SLNextItem(si);
    printf("x is: %d\ty is: %d\n", *(temp->x), *(temp->y)); // 23,98
    SortedListIteratorPtr si2 = SLCreateIterator(sl);
    temp = (Point*) SLNextItem(si2);
    printf("x is: %d\ty is: %d\n", *(temp->x), *(temp->y)); // 23,98
    temp = (Point*) SLNextItem(si2);
    printf("x is: %d\ty is: %d\n", *(temp->x), *(temp->y)); // 30,30
    temp = (Point*) SLNextItem(si2);
    printf("x is: %d\ty is: %d\n", *(temp->x), *(temp->y)); // 1,7
    SLRemove(sl, (void*) h);
    SLRemove(sl, (void*) e);
    temp = (Point*) SLNextItem(si);
    printf("x is: %d\ty is: %d\n", *(temp->x), *(temp->y)); // 1,7
    temp = (Point*) SLNextItem(si2);
    printf("%p", temp); // nil
    temp = (Point*) SLNextItem(si2);
    printf("%p", temp); // nil

    SLDestroyIterator(si);
    SLDestroyIterator(si2);
    SLDestroy(sl);

    printf("\nEND STRUCT TEST SIM:\n");
    /*********** end struct testing *************/

    return EXIT_SUCCESS;
}

/* Other Debug Methods
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
} */
