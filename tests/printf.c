#define _XOPEN_SOURCE 700

#include <stdio.h>

int main(void)
{
    const char *string = "sub-string";

    printf("Hello, world!\n");
    printf("Hello, %s.\n", string);
    printf("Hello, %c.\n", string[0]);
    printf("Hello, %c/%s.\n", string[0], string);
    printf("Hello, %#x\n", 0x1000);
    printf("Hello, %x\n", 0x1000);
    printf("Hello, %u\n", 0x1000);
    printf("Hello, %8u\n", 0x1000);
    printf("Hello, %08u\n", 0x1000);
    printf("Hello, %-8u\n", 0x1000);
    printf("Hello, %-#x\n", 0x1000);
    printf("empty string <%s>\n", "");
    printf("NULL string <%s>\n", NULL);
    printf("%g\n", 1e100);
    
    /*
    char buf[BUFSIZ];
    snprintf(buf, BUFSIZ, "Hello, world!");
    puts(buf);
    snprintf(buf, BUFSIZ, "Hello, %s.", string);
    puts(buf);
    snprintf(buf, BUFSIZ, "Hello, %c.", string[0]);
    puts(buf);
    snprintf(buf, BUFSIZ, "Hello, %c/%s.", string[0], string);
    puts(buf);
    */
    return 0;
}
