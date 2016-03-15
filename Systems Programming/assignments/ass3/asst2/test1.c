/**************************************************************
 *test1.c
 *
 *test realloc method
 *
 *Authors: Jordan and BIggie
***************************************************************/

#include<stdlib.h>
#include<stdio.h>
#include "mymalloc.h"

int main(int argc, char const *argv[])
{
    int *nums = (int*) calloc(10, sizeof(int));

    int i;
    for (i = 0; i < 10; i++) {
        nums[i] = i;
    }

    nums = (int*) realloc(nums, sizeof(int) * 20);
    for (;i < 20; i++) {
        nums[i] = i;
    }

    for (i = 0; i < 20; i++) {
        printf("%s\n", (nums[i] == i) ?
            "Realloc was successful" :
            "Realloc UNSUCCESSFUL");
    }


    return EXIT_SUCCESS;
}
