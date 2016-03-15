#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

// test calloc
int main(int argc, char *argv[])
{
    int *nums = (int*) calloc(10, sizeof(int));
    if(!nums){
        printf("%s\n", "Not malloc'd properly");
        return EXIT_FAILURE;
    }

    printHeap();

    int i;
    for(i = 0; i < 10; i++)
    {
        printf("%s\n", (!nums[i]) ?
            "It was calloc'd properly" :
            "It was not calloc'd properly");

        nums[i] = i;
    }

    for(i = 0; i < 10; i++)
    {
        printf("%s\n", (nums[i] == i) ?
            "values were assigned properly" :
            "values were not assigned properly");
    }

    free(nums);
    printHeap();

    return EXIT_SUCCESS;
}
