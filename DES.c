#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

//testing...

//Za sada ne radi :P
int64_t IP(int64_t input)
{
    int permutation[] = {58, 50, 42, 34, 26, 18, 10, 2, 60,\
                       52, 44, 36, 28, 20, 12, 4, 62, 54,\
                       46, 38, 30, 22, 14, 6, 64, 56, 48,\
                       40, 32, 24, 16, 8, 57, 49, 41, 33,\
                       25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3,\
                       61, 53, 45, 37, 29, 21, 13, 5,\
                       63, 55, 47, 39, 31, 23, 15, 7}, i;
    int64_t output = 0;
    for(i=0;i<64;i++)
    {
        if(input & (1<<(64-permutation[i])))
            output |= 1<<(63-i);
    }
    return output;
}

int main()
{
    int64_t test = 7;
    printf("%" PRId64 "\n", IP(test));
    return 0;
}
