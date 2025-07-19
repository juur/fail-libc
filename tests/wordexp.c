#define _XOPEN_SOURCE 700

#include <wordexp.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <err.h>

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");

    if (argc != 2)
        exit(EXIT_FAILURE);

    wordexp_t we;

    memset(&we, 0, sizeof(we));

    int rc = wordexp(argv[1], &we, 0);

    printf("rc:           %d\n", rc);

    if (rc < 0) {
        const char *msg;
        switch (rc) 
        {
            case WRDE_NOSPACE: msg = "WRDE_NOSPACE";   break;
            case WRDE_SYNTAX:  msg = "WRDE_SYNTAX";    break;
            default:           msg = "WRDE_UNKNOWN";   break;
        }
        errx(EXIT_FAILURE, "wordexp: %s", msg);
    }

    printf("we_wordc:     %ld\n", we.we_wordc);
    printf("we_offs:      %ld\n", we.we_offs);

    for (size_t i = 0; i < we.we_wordc; i++)
        printf("we_wordv[%02ld]: '%s'\n", i, we.we_wordv[i]);

    exit(EXIT_SUCCESS);
}
