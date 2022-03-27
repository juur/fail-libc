#ifndef _CURSES_H
#define _CURSES_H

#include <sys/types.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct _fc_window {
	size_t _cx, _cy;
	int x, y, lines, cols;
} WINDOW;

typedef struct _fc_screen {
	int _infd, _outfd;
	FILE *outfd, *infd;
	WINDOW *stdscr;
} SCREEN;

extern WINDOW *initscr(void);
extern int endwin(void);
extern int cbreak(void);
extern int halfdelay(int);
extern int noecho(void);
extern int nonl(void);
extern int meta(WINDOW *, bool);
extern int idlok(WINDOW *, bool);
extern int scrollok(WINDOW *, bool);
extern int keypad(WINDOW *, bool);
extern WINDOW *newwin(int,int,int,int);
extern SCREEN *newterm(char *type, FILE *outfd, FILE *infd);
extern int delwin(WINDOW *win);

extern WINDOW *stdscr;

#endif
