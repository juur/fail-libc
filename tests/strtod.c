#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char *str = "-1.23456e-123";
    strtod(str, NULL);
    str = "1";
    strtod(str, NULL);
    str = "1.000000000023";
    strtod(str, NULL);
    str = "1e100";
    strtod(str, NULL);

    exit(EXIT_FAILURE);
}
