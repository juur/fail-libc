#define _XOPEN_SOURCE 700

#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
    int opt;

    setlocale(LC_ALL, "C");

    while( (opt = getopt(argc, argv, "ab:d")) != -1)
    {
        printf("optind:%d optopt:%02x[%c] optarg:%p <%s> opt:%02x[%c] opterr:%d\n",
                optind, 
                optopt, isprint(optopt) ? optopt : ' ',
                optarg, optarg ? optarg : " ", 
                opt,    isprint(opt) ? opt : ' ',
                opterr);

            
    }

    while (optind < argc)
    {
        printf("argv[%u]=<%s>\n", optind, argv[optind]);
        optind++;
    }

    return 0;
}
