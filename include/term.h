#ifndef _TERM_H
#define _TERM_H

#include <stdbool.h>

#define _CURSES_NUM_DATA 46

typedef struct {
    int   fd;
    void *terminfo;
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

#endif
