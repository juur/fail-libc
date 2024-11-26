#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdio.h>
#include <term.h>
#include <curses.h>
#include <ctype.h>

static void    hexdump(const char *tmp)
{
    if (tmp && tmp != (char *)-1)
        while (*tmp)
        {
            if (isprint(*tmp)) printf("%c", *tmp);
            else
                printf("0x%03x", *tmp);

            tmp++;
            if (*tmp)
                printf(" ");
        }
}

int main(int ac, char *av[])
{
    int rc;

    const char *cap_name;

    printf("setupterm=%d\n", rc = setupterm(NULL, fileno(stdout), NULL));

    if (rc == ERR)
        exit(EXIT_FAILURE);

    cap_name = ac>1 ? av[1] : "clear";

    printf("tigetflag = %d\n", tigetflag(cap_name));

    printf("tigetnum  = %d\n", tigetnum(cap_name));
   
    printf("tigetstr  = <");

    char *ret = tigetstr(cap_name);
    if (ret == (char *)-1) {
        printf("(not a string)");
        ret = NULL;
    } else if (ret == NULL) {
    } else
        hexdump(ret);
    printf(">\n");

    if (ret) {
    char *parm = tiparm(tigetstr(cap_name),1,2,3,4,5,6,7,8,9);
    printf("tiparm    = <");
    hexdump(parm);
    printf(">\n");
    }
 
    //if (parm && parm != (char *)-1)
    //    putp(parm);

    exit(EXIT_SUCCESS);
}
