#include <stdio.h>
#include <stdlib.h>

// Debug Macros
#define debugc(x) printf(#x " is: %c\n", x)
#define debugs(x) printf(#x " is: %s\n", x)
#define debugi(x) printf(#x " is: %d\n", x)
#define debugf(x) printf(#x " is: %f\n", x)

int main(int argc, char* argv[])
{
    int i = 1;
    while(i < 100) { printf("%d", i++); };

    return EXIT_SUCCESS;
}
