#define _XOPEN_SOURCE 700

#include <stdio.h>

int main(void)
{
    const char *string = "sub-string";
    char buf[BUFSIZ];

    printf("Hello, world!\n");
    printf("Hello, %s.\n", string);
    printf("Hello, %c.\n", string[0]);
    printf("Hello, %c/%s.\n", string[0], string);
    
    snprintf(buf, BUFSIZ, "Hello, world!");
    puts(buf);
    snprintf(buf, BUFSIZ, "Hello, %s.", string);
    puts(buf);
    snprintf(buf, BUFSIZ, "Hello, %c.", string[0]);
    puts(buf);
    snprintf(buf, BUFSIZ, "Hello, %c/%s.", string[0], string);
    puts(buf);
    return 0;
}
