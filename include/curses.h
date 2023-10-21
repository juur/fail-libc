#ifndef _CURSES_H
#define _CURSES_H

#include <features.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <wchar.h>
#include <term.h>
#include <termios.h>
#include <stdint.h>

#define FALSE false
#define TRUE  true
#define ERR   (-1)
#define OK    (0)
#define _XOPEN_CURSES

#define WA_ALTCHARSET (1U << 16)
#define WA_BLINK      (1U << 17)
#define WA_BOLD       (1U << 18)
#define WA_DIM        (1U << 19)
#define WA_HORIZONTAL (1U << 20)
#define WA_INVIS      (1U << 21)
#define WA_LEFT       (1U << 22)
#define WA_LOW        (1U << 23)
#define WA_PROTECT    (1U << 24)
#define WA_REVERSE    (1U << 25)
#define WA_RIGHT      (1U << 26)
#define WA_STANDOUT   (1U << 27)
#define WA_TOP        (1U << 28)
#define WA_UNDERLINE  (1U << 29)
#define WA_VERTICAL   (1U << 30)

#define A_ALTCHARSET   WA_ALTCHARSET
#define A_BLINK        WA_BLINK
#define A_BOLD         WA_BOLD
#define A_DIM          WA_DIM
#define A_INVIS        WA_INVIS
#define A_PROECT       WA_PROTECT
#define A_REVERSE      WA_REVERSE
#define A_STANDOUT     WA_STANDOUT
#define A_UNDERLINE    WA_UNDERLINE

#define A_ATTRIBUTES   (~(0xffU))
#define A_CHARTEXT     (0x00ffU)
#define A_COLOR        (0xff00U)

#define COLOR_PAIR(n)  (((n) << 8) & A_COLOR)
#define PAIR_NUMBER(n) (((n) & A_COLOR) >> 8)

#define COLOR_BLACK     0
#define COLOR_BLUE      1
#define COLOR_GREEN     2
#define COLOR_CYAN      3
#define COLOR_RED       4
#define COLOR_MAGENTA   5
#define COLOR_YELLOW    6
#define COLOR_WHITE     7

#define KEY_CODE_YES    0x100

/* insert KEY_ variables here, starting at 0x101 */


typedef uint32_t attr_t;
typedef uint32_t chtype;

typedef struct {
    wchar_t chr;            /* this should probably be an array? */
    attr_t  attr;
    int     color_pair;
} cchar_t;

typedef struct _fc_window {
	size_t _cx, _cy;
	int x, y, lines, cols;
    bool clearok;
    bool idlok;
    bool leaveok;
    bool scrollok;
    struct {
        bool touched;
        char *line;
    } line_data[];
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
