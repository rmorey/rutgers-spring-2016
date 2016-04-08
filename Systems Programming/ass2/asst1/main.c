/**************************************************************
 *filename
 *
 *description
 *
 *Authors: names
***************************************************************/

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "sorted-list.h"

int cp(void* a, void* b){
    char *a1 = (char*) a;
    char *b1 = (char*) b;

    if((*a1) == (*b1)){
        return 0;
    }

    return ((*a1) > (*b1)) ? 1 : -1;
}

void d(void* a){
    return;
}

int main(int argc, char const *argv[])
{
    SortedListPtr s = SLCreate(cp, d);

    char x = 'a';
    SLInsert(s, (void*) (&x));
    char y = 'x';
    SLInsert(s, (void*) (&y));
    char z = 'k';
    SLInsert(s, (void*) (&z));
    char f = 'l';
    SLInsert(s, (void*) (&f));
    char h = 'j';
    SLInsert(s, (void*) (&h));

    /*SLRemove(s, (void*) (&y));
    double k = 10.97;
    SLInsert(s, (void*) (&k));*/

    SortedListIteratorPtr i = SLCreateIterator(s);

    /*char * test = (char*) SLNextItem(i);
    printf("From next item: %c\n", *test);

    test = (char*) SLGetItem(i);
    printf("From  GetGtem: %c\n", *test);*/


    char * test = (char*) SLNextItem(i);
    while(test != NULL){
        printf("%c\n", *test);
        test = (char*) SLNextItem(i);
    }

    return EXIT_SUCCESS;
}
