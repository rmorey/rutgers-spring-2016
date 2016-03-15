/**************************************************************
 *test4.c
 *
 *test fragmentation
 *
 *Authors: biggs and j-dawg
***************************************************************/

#include<stdlib.h>
#include<stdio.h>
#include "mymalloc.h"

int main(int argc, char const *argv[])
{
    void* p1 = malloc(50);
    printHeap();
    void* p2 = malloc(500);
    printHeap();
    void* p3 = malloc(50);
    printHeap();
    free(p2);
    printHeap();
    void* p4 = malloc(200);
    printHeap();
    void* p5 = malloc(210);
    printHeap();

    free(p1);
    printHeap();
    free(p3);
    printHeap();
    free(p4);
    printHeap();
    free(p5);
    printHeap();

    return EXIT_SUCCESS;
}
