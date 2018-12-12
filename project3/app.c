#include <stdio.h>
#include <stdlib.h>

int main()
{
    int i, *ptr, sum = 0;

    ptr = (int*) malloc(n * sizeof(int));
    if(ptr == NULL)                     
    {
        printf("Error! memory not allocated.");
        exit(0);
    }

    for(i = 0; i < 100; ++i)
    {
        scanf("%d", ptr + i);
        sum += *(ptr + i);
    }

    free(ptr);
    return 0;
}
