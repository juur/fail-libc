#include <stdlib.h>
#include <stdio.h>

int comp(const void *a, const void *b)
{
    int l = *(int *)a;
    int r = *(int *)b;

    return l - r;
}

int main(void)
{
    int array[10] = {5,4,2,3,6,1,10,2,3,4};

    for (int i = 0; i < 10; i++)
        printf("%d ", array[i]);
    printf("\n");

    qsort(array, 10, sizeof(int), comp);

    for (int i = 0; i < 10; i++)
        printf("%d ", array[i]);
    printf("\n");

    exit(EXIT_SUCCESS);
}
