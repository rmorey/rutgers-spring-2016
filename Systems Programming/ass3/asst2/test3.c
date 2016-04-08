/**************************************************************
 *test3.c
 *
 *test free
 *
 *Authors: Biggie and Jordano
***************************************************************/

#include<stdlib.h>
#include<stdio.h>
#include "mymalloc.h"

int main(int argc, char const *argv[])
{
    void* ptr = malloc(sizeof(double) * 30);
    void *ptr2 = malloc(40);
    free(ptr);
    printHeap();

    free(ptr); //double free

    printHeap();

    int x = 7;
    free(&x); //incorrect free
    free(ptr2);
    printHeap();


    return EXIT_SUCCESS;
}
