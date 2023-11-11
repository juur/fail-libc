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

/* insert KEY_ variables here, starting at 0x101 
 * Source: X/Open Curses, Issue 7 pg 309+
 */

#define   KEY_A1          0x101
#define   KEY_A3          0x102
#define   KEY_B2          0x103
#define   KEY_BACKSPACE   0x104
#define   KEY_BEG         0x105
#define   KEY_BREAK       0x106
#define   KEY_BTAB        0x107
#define   KEY_C1          0x108
#define   KEY_C3          0x109
#define   KEY_CANCEL      0x10a
#define   KEY_CATAB       0x10b
#define   KEY_CLEAR       0x10c
#define   KEY_CLOSE       0x10d
#define   KEY_COMMAND     0x10e
#define   KEY_COPY        0x10f
#define   KEY_CREATE      0x110
#define   KEY_CTAB        0x111
#define   KEY_DC          0x112
#define   KEY_DL          0x113
#define   KEY_DOWN        0x114
#define   KEY_EIC         0x115
#define   KEY_END         0x116
#define   KEY_ENTER       0x117
#define   KEY_EOL         0x118
#define   KEY_EOS         0x119
#define   KEY_EXIT        0x11a
#define   KEY_F0          0x11b
#define   KEY_F(n)        (KEY_F0 + (n))
#define   KEY_FIND        0x15b
#define   KEY_HELP        0x15c
#define   KEY_HOME        0x15d
#define   KEY_IC          0x15e
#define   KEY_IL          0x15f
#define   KEY_LEFT        0x160
#define   KEY_LL          0x161
#define   KEY_MARK        0x162
#define   KEY_MAX         0x163
#define   KEY_MESSAGE     0x164
#define   KEY_MIN         0x165
#define   KEY_MOUSE       0x166
#define   KEY_MOVE        0x167
#define   KEY_NEXT        0x168
#define   KEY_NPAGE       0x169
#define   KEY_OPEN        0x16a
#define   KEY_OPTIONS     0x16b
#define   KEY_PPAGE       0x16c
#define   KEY_PREVIOUS    0x16d
#define   KEY_PRINT       0x16e
#define   KEY_REDO        0x16f
#define   KEY_REFERENCE   0x170
#define   KEY_REFRESH     0x171
#define   KEY_REPLACE     0x172
#define   KEY_RESET       0x173
#define   KEY_RESIZE      0x174
#define   KEY_RESTART     0x175
#define   KEY_RESUME      0x176
#define   KEY_RIGHT       0x177
#define   KEY_SAVE        0x178
#define   KEY_SBEG        0x179
#define   KEY_SCANCEL     0x17a
#define   KEY_SCOMMAND    0x17b
#define   KEY_SCOPY       0x17c
#define   KEY_SCREATE     0x17d
#define   KEY_SDC         0x17e
#define   KEY_SDL         0x17f
#define   KEY_SELECT      0x180
#define   KEY_SEND        0x181
#define   KEY_SEOL        0x182
#define   KEY_SEXIT       0x183
#define   KEY_SF          0x184
#define   KEY_SFIND       0x185
#define   KEY_SHELP       0x186
#define   KEY_SHOME       0x187
#define   KEY_SIC         0x188
#define   KEY_SLEFT       0x189
#define   KEY_SMESSAGE    0x18a
#define   KEY_SMOVE       0x18b
#define   KEY_SNEXT       0x18c
#define   KEY_SOPTIONS    0x18d
#define   KEY_SPREVIOUS   0x18e
#define   KEY_SPRINT      0x18f
#define   KEY_SR          0x18a
#define   KEY_SREDO       0x18b
#define   KEY_SREPLACE    0x18c
#define   KEY_SRESET      0x18d
#define   KEY_SRIGHT      0x18e
#define   KEY_SRSUME      0x18f
#define   KEY_SSAVE       0x190
#define   KEY_SSUSPEND    0x191
#define   KEY_STAB        0x192
#define   KEY_SUNDO       0x193
#define   KEY_SUSPEND     0x194
#define   KEY_UNDO        0x195
#define   KEY_UP          0x196


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
    bool meta;
    bool nl;
    struct {
        bool touched;
        chtype *line;
    } line_data[];
} WINDOW;

typedef struct _fc_screen {
	int _infd, _outfd;
	FILE *outfd, *infd;
	WINDOW *stdscr;
} SCREEN;

#define getmaxyx(win,y,x) { x = (win)->lines; y = (win)->cols; }

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
extern int addch(const chtype ch);
extern int waddch(WINDOW *win, const chtype ch);
extern int mvaddch(int y, int x, const chtype ch);
extern int mvwaddch(WINDOW *win, int y, int x, const chtype ch);
extern int echochar(const chtype ch);
extern int wechochar(WINDOW *win, const chtype ch);
extern int move(int y, int x);
extern int wmove(WINDOW *win, int y, int x);

extern WINDOW *stdscr;

#endif
