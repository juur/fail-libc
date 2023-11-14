#ifndef _TERM_H
#define _TERM_H

#include <stdbool.h>

typedef struct {
    int   fd;
    void *terminfo;
    int   lines, columns;
} TERMINAL;

extern TERMINAL *cur_term;

int del_curterm(TERMINAL *);
int putp(const char *);
int restartterm(char *, int, int *);
TERMINAL *set_curterm(TERMINAL *);
int setupterm(char *, int, int *);
int tigetflag(const char *);
int tigetnum(const char *);
char *tigetstr(const char *);
char *tiparm(const char *, ...);
char *tparm(const char *, long, long, long, long, long, long, long, long, long);
int tputs(const char *, int, int (*)(int));

/*
 * private structures, typedefs, etc.
 */

struct terminfo {
    struct terminfo *next;
    char *name; /* TODO names[] */
    char *desc;
    union {
        bool  bool_entry;
        int   int_entry;
        char *string_entry;
    } data[1024];
};


#endif
