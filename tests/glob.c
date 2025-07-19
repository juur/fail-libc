#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <glob.h>

static const char *glob_error(int code)
{
    switch (code)
    {
        case 0:
            return "Success";
        case GLOB_NOSPACE:
            return "ENOMEM";
        case GLOB_ABORTED:
            return "EIO";
        case GLOB_NOMATCH:
            return "ESRCH";
        default:
            return "unknown error";
    }
}

int main(int argc, char *argv[])
{
    int rc;

    if (argc != 2)
        exit(EXIT_FAILURE);

    glob_t pglob;

    rc = glob(argv[1], 0, NULL, &pglob);

    if (rc != 0) {
        errx(EXIT_FAILURE, "glob returned an error: %s", glob_error(rc));
    } else {
        printf("gl_pathc=%lu\n", pglob.gl_pathc);
        for (size_t i = 0; i < pglob.gl_pathc; i++)
                printf(" gl_pathv[%lu]=%s\n", i, pglob.gl_pathv[i]);
    }

    globfree(&pglob);

    exit(EXIT_SUCCESS);
}
