/**************************************************************
 *test2.c
 *
 *test malloc
 *
 *Authors: BIggie and Jordan
***************************************************************/

#include<stdlib.h>
#include<stdio.h>
#include "mymalloc.h"

int main(int argc, char const *argv[])
{
    void* ptr = malloc(sizeof(double) * 30);
    printf("%s\n", (ptr) ? "Successful malloc" : "UNSUCCESSFUL malloc");

    ptr = malloc(sizeof(double) * 30);
    printf("%s\n", (ptr) ? "Successful malloc" : "UNSUCCESSFUL malloc");

    ptr = malloc(sizeof(double) * 30);
    printf("%s\n", (ptr) ? "Successful malloc" : "UNSUCCESSFUL malloc");

    ptr = malloc(sizeof(double) * 30);
    printf("%s\n", (ptr) ? "Successful malloc" : "UNSUCCESSFUL malloc");

    return EXIT_SUCCESS;
}
