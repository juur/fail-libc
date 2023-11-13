#ifndef _CURSES_H
#define _CURSES_H

#define _XOPEN_CURSES

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

typedef struct _fc_screen SCREEN;
typedef struct _fc_window WINDOW;

struct _fc_window_line_data {
    bool touched;
    chtype *line;
};

typedef struct _fc_window {
	size_t _cx, _cy;
	int x, y, lines, cols;
    bool clearok;
    bool idlok;
    bool leaveok;
    bool scrollok;
    bool meta;
    bool nl;
    bool keypad;
    int attr;
    SCREEN *scr;
    WINDOW *parent;
    struct _fc_window_line_data line_data[];
} WINDOW;

typedef struct _fc_screen {
	int _infd, _outfd;
	FILE *outfd, *infd;
	WINDOW *stdscr;
    TERMINAL *term;
    bool isendwin;
    struct termios shell_in;
    struct termios shell_out;
    struct termios save_in;
    struct termios save_out;
} SCREEN;

#define getmaxyx(win,y,x) { y = ((win)->lines); x = ((win)->cols); }
#define getyx(win,y,x) { x = (win)->x; y = (win)->y; }

extern WINDOW *stdscr;
extern WINDOW *curscr;
extern int COLOR_PAIRS;
extern int COLORS;
extern int COLS;
extern int LINES;

int addch(const chtype);
int addchnstr(const chtype *, int);
int addchstr(const chtype *);
int addnstr(const char *, int);
int addnwstr(const wchar_t *, int);
int addstr(const char *);
int add_wch(const cchar_t *);
int add_wchnstr(const cchar_t *, int);
int add_wchstr(const cchar_t *);
int addwstr(const wchar_t *);
int attroff(int);
int attron(int);
int attrset(int);
int attr_get(attr_t *, short *, void *);
int attr_off(attr_t, void *);
int attr_on(attr_t, void *);
int attr_set(attr_t, short, void *);
int baudrate(void);
int beep(void);
int bkgd(chtype);
void bkgdset(chtype);
int bkgrnd(const cchar_t *);
void bkgrndset(const cchar_t *);
int border(chtype, chtype, chtype, chtype, chtype, chtype, chtype, chtype);
int border_set(const cchar_t *, const cchar_t *,
const cchar_t *, const cchar_t *, const cchar_t *,
const cchar_t *, const cchar_t *, const cchar_t *);
int box(WINDOW *, chtype, chtype);
int box_set(WINDOW *, const cchar_t *, const cchar_t *);
bool can_change_color(void);
int cbreak(void);
int chgat(int, attr_t, short, const void *);
int clearok(WINDOW *, bool);
int clear(void);
int clrtobot(void);
int clrtoeol(void);
int color_content(short, short *, short *, short *);
int color_set(short,void *);
int copywin(const WINDOW *, WINDOW *, int, int, int, int, int, int, int);
int curs_set(int);
int def_prog_mode(void);
int def_shell_mode(void);
int delay_output(int);
int delch(void);
int deleteln(void);
void delscreen(SCREEN *);
int delwin(WINDOW *);
WINDOW *derwin(WINDOW *, int, int, int, int);
int doupdate(void);
WINDOW *dupwin(WINDOW *);
int echo(void);
int echochar(const chtype);
int echo_wchar(const cchar_t *);
int endwin(void);
char erasechar(void);
int erase(void);
int erasewchar(wchar_t *);
void filter(void);
int flash(void);
int flushinp(void);
chtype getbkgd(WINDOW *);
int getbkgrnd(cchar_t *);
int getcchar(const cchar_t *, wchar_t *, attr_t *, short *, void *);
int getch(void);
int getnstr(char *, int);
int getn_wstr(wint_t *, int);
int getstr(char *);
int get_wch(wint_t *);
WINDOW *getwin(FILE *);
int get_wstr(wint_t *);
int halfdelay(int);
bool has_colors(void);
bool has_ic(void);
bool has_il(void);
int hline(chtype, int);
int hline_set(const cchar_t *, int);
void idcok(WINDOW *, bool);
int idlok(WINDOW *, bool);
void immedok(WINDOW *, bool);
chtype inch(void);
int inchnstr(chtype *, int);
int inchstr(chtype *);
WINDOW *initscr(void);
int init_color(short, short, short, short);
int init_pair(short, short, short);
int innstr(char *, int);
int innwstr(wchar_t *, int);
int insch(chtype);
int insdelln(int);
int insertln(void);
int insnstr(const char *, int);
int ins_nwstr(const wchar_t *, int);
int insstr(const char *);
int instr(char *);
int ins_wch(const cchar_t *);
int ins_wstr(const wchar_t *);
int intrflush(WINDOW *, bool);
int in_wch(cchar_t *);
int in_wchnstr(cchar_t *, int);
int in_wchstr(cchar_t *);
int inwstr(wchar_t *);
bool isendwin(void);
bool is_linetouched(WINDOW *, int);
bool is_wintouched(WINDOW *);
char *keyname(int);
char *key_name(wchar_t);
int keypad(WINDOW *, bool);
char killchar(void);
int killwchar(wchar_t *);
int leaveok(WINDOW *, bool);
char *longname(void);
int meta(WINDOW *, bool);
int move(int, int);
int mvaddch(int, int, const chtype);
int mvaddchnstr(int, int, const chtype *, int);
int mvaddchstr(int, int, const chtype *);
int mvaddnstr(int, int, const char *, int);
int mvaddnwstr(int, int, const wchar_t *, int);
int mvaddstr(int, int, const char *);
int mvadd_wch(int, int, const cchar_t *);
int mvadd_wchnstr(int, int, const cchar_t *, int);
int mvadd_wchstr(int, int, const cchar_t *);
int mvaddwstr(int, int, const wchar_t *);
int mvchgat(int, int, int, attr_t, short, const void *);
int mvcur(int, int, int, int);
int mvdelch(int, int);
int mvderwin(WINDOW *, int, int);
int mvgetch(int, int);
int mvgetnstr(int, int, char *, int);
int mvgetn_wstr(int, int, wint_t *, int);
int mvgetstr(int, int, char *);
int mvget_wch(int, int, wint_t *);
int mvget_wstr(int, int, wint_t *);
int mvhline(int, int, chtype, int);
int mvhline_set(int, int, const cchar_t *, int);
chtype mvinch(int, int);
int mvinchnstr(int, int, chtype *, int);
int mvinchstr(int, int, chtype *);
int mvinnstr(int, int, char *, int);
int mvinnwstr(int, int, wchar_t *, int);
int mvinsch(int, int, chtype);
int mvinsnstr(int, int, const char *, int);
int mvins_nwstr(int, int, const wchar_t *, int);
int mvinsstr(int, int, const char *);
int mvinstr(int, int, char *);
int mvins_wch(int, int, const cchar_t *);
int mvins_wstr(int, int, const wchar_t *);
int mvin_wch(int, int, cchar_t *);
int mvin_wchnstr(int, int, cchar_t *, int);
int mvin_wchstr(int, int, cchar_t *);
int mvinwstr(int, int, wchar_t *);
int mvprintw(int, int, const char *, ...);
int mvscanw(int, int, const char *, ...);
int mvvline(int, int, chtype, int);
int mvvline_set(int, int, const cchar_t *, int);
int mvwaddch(WINDOW *, int, int, const chtype);
int mvwaddchnstr(WINDOW *, int, int, const chtype *, int);
int mvwaddchstr(WINDOW *, int, int, const chtype *);
int mvwaddnstr(WINDOW *, int, int, const char *, int);
int mvwaddnwstr(WINDOW *, int, int, const wchar_t *, int);
int mvwaddstr(WINDOW *, int, int, const char *);
int mvwadd_wch(WINDOW *, int, int, const cchar_t *);
int mvwadd_wchnstr(WINDOW *, int, int, const cchar_t *, int);
int mvwadd_wchstr(WINDOW *, int, int, const cchar_t *);
int mvwaddwstr(WINDOW *, int, int, const wchar_t *);
int mvwchgat(WINDOW *, int, int, int, attr_t, short,const void *);
int mvwdelch(WINDOW *, int, int);
int mvwgetch(WINDOW *, int, int);
int mvwgetnstr(WINDOW *, int, int, char *, int);
int mvwgetn_wstr(WINDOW *, int, int, wint_t *, int);
int mvwgetstr(WINDOW *, int, int, char *);
int mvwget_wch(WINDOW *, int, int, wint_t *);
int mvwget_wstr(WINDOW *, int, int, wint_t *);
int mvwhline(WINDOW *, int, int, chtype, int);
int mvwhline_set(WINDOW *, int, int, const cchar_t *, int);
int mvwin(WINDOW *, int, int);
chtype mvwinch(WINDOW *, int, int);
int mvwinchnstr(WINDOW *, int, int, chtype *, int);
int mvwinchstr(WINDOW *, int, int, chtype *);
int mvwinnstr(WINDOW *, int, int, char *, int);
int mvwinnwstr(WINDOW *, int, int, wchar_t *, int);
int mvwinsch(WINDOW *, int, int, chtype);
int mvwinsnstr(WINDOW *, int, int, const char *, int);
int mvwins_nwstr(WINDOW *, int, int, const wchar_t *, int);
int mvwinsstr(WINDOW *, int, int, const char *);
int mvwinstr(WINDOW *, int, int, char *);
int mvwins_wch(WINDOW *, int, int, const cchar_t *);
int mvwins_wstr(WINDOW *, int, int, const wchar_t *);
int mvwin_wch(WINDOW *, int, int, cchar_t *);
int mvwin_wchnstr(WINDOW *, int, int, cchar_t *, int);
int mvwin_wchstr(WINDOW *, int, int, cchar_t *);
int mvwinwstr(WINDOW *, int, int, wchar_t *);
int mvwprintw(WINDOW *, int, int, const char *, ...);
int mvwscanw(WINDOW *, int, int, const char *, ...);
int mvwvline(WINDOW *, int, int, chtype, int);
int mvwvline_set(WINDOW *, int, int, const cchar_t *, int);
int napms(int);
WINDOW *newpad(int, int);
SCREEN *newterm(const char *, FILE *, FILE *);
WINDOW *newwin(int, int, int, int);
int nl(void);
int nocbreak(void);
int nodelay(WINDOW *, bool);
int noecho(void);
int nonl(void);
void noqiflush(void);
int noraw(void);
int notimeout(WINDOW *, bool);
int overlay(const WINDOW *, WINDOW *);
int overwrite(const WINDOW *, WINDOW *);
int pair_content(short, short *, short *);
int pechochar(WINDOW *, chtype);
int pecho_wchar(WINDOW *, const cchar_t*);
int pnoutrefresh(WINDOW *, int, int, int, int, int, int);
int prefresh(WINDOW *, int, int, int, int, int, int);
int printw(const char *, ...);
int putp(const char *);
int putwin(WINDOW *, FILE *);
void qiflush(void);
int raw(void);
int redrawwin(WINDOW *);
int refresh(void);
int reset_prog_mode(void);
int reset_shell_mode(void);
int resetty(void);
int ripoffline(int, int (*)(WINDOW *, int));
int savetty(void);
int scanw(const char *, ...);
int scr_dump(const char *);
int scr_init(const char *);
int scrl(int);
int scroll(WINDOW *);
int scrollok(WINDOW *, bool);
int scr_restore(const char *);
int scr_set(const char *);
int setcchar(cchar_t*, const wchar_t*, const attr_t, short, const void*);
int setscrreg(int, int);
SCREEN *set_term(SCREEN *);
int slk_attr_off(const attr_t, void *);
int slk_attroff(const chtype);
int slk_attr_on(const attr_t, void *);
int slk_attron(const chtype);
int slk_attr_set(const attr_t, short, void *);
int slk_attrset(const chtype);
int slk_clear(void);
int slk_color(short);
int slk_init(int);
char *slk_label(int);
int slk_noutrefresh(void);
int slk_refresh(void);
int slk_restore(void);
int slk_set(int, const char *, int);
int slk_touch(void);
int slk_wset(int, const wchar_t *, int);
int standend(void);
int standout(void);
int start_color(void);
WINDOW *subpad(WINDOW *, int, int, int, int);
WINDOW *subwin(WINDOW *, int, int, int, int);
int syncok(WINDOW *, bool);
chtype termattrs(void);
attr_t term_attrs(void);
char *termname(void);
int tigetflag(const char *);
int tigetnum(const char *);
char *tigetstr(const char *);
void timeout(int);
int touchline(WINDOW *, int, int);
int touchwin(WINDOW *);
char *tiparm(const char *, ...);
char *tparm(const char *, long, long, long, long, long, long, long, long, long);
int typeahead(int);
int ungetch(int);
int unget_wch(const wchar_t);
int untouchwin(WINDOW *);
void use_env(bool);
int vid_attr(attr_t, short, void *);
int vidattr(chtype);
int vid_puts(attr_t, short, void *, int (*)(int));
int vidputs(chtype, int (*)(int));
int vline(chtype, int);
int vline_set(const cchar_t *, int);
int vw_printw(WINDOW *, const char *, va_list);
int vw_scanw(WINDOW *, const char *, va_list);
int waddch(WINDOW *, const chtype);
int waddchnstr(WINDOW *, const chtype *, int);
int waddchstr(WINDOW *, const chtype *);
int waddnstr(WINDOW *, const char *, int);
int waddnwstr(WINDOW *, const wchar_t *, int);
int waddstr(WINDOW *, const char *);
int wadd_wch(WINDOW *, const cchar_t *);
int wadd_wchnstr(WINDOW *, const cchar_t *, int);
int wadd_wchstr(WINDOW *, const cchar_t *);
int waddwstr(WINDOW *, const wchar_t *);
int wattroff(WINDOW *, int);
int wattron(WINDOW *, int);
int wattrset(WINDOW *, int);
int wattr_get(WINDOW *, attr_t *, short *, void *);
int wattr_off(WINDOW *, attr_t, void *);
int wattr_on(WINDOW *, attr_t, void *);
int wattr_set(WINDOW *, attr_t, short, void *);
int wbkgd(WINDOW *, chtype);
void wbkgdset(WINDOW *, chtype);
int wbkgrnd(WINDOW *, const cchar_t *);
void wbkgrndset(WINDOW *, const cchar_t *);
int wborder(WINDOW *, chtype, chtype, chtype, chtype, chtype,
chtype, chtype, chtype);
int wborder_set(WINDOW *, const cchar_t *, const cchar_t *,
const cchar_t *, const cchar_t *, const cchar_t *,
const cchar_t *, const cchar_t *, const cchar_t *);
int wchgat(WINDOW *, int, attr_t, short, const void *);
int wclear(WINDOW *);
int wclrtobot(WINDOW *);
int wclrtoeol(WINDOW *);
void wcursyncup(WINDOW *);
int wcolor_set(WINDOW *, short, void *);
int wdelch(WINDOW *);
int wdeleteln(WINDOW *);
int wechochar(WINDOW *, const chtype);
int wecho_wchar(WINDOW *, const cchar_t *);
int werase(WINDOW *);
int wgetbkgrnd(WINDOW *, cchar_t *);
int wgetch(WINDOW *);
int wgetnstr(WINDOW *, char *, int);
int wgetn_wstr(WINDOW *, wint_t *, int);
int wgetstr(WINDOW *, char *);
int wget_wch(WINDOW *, wint_t *);
int wget_wstr(WINDOW *, wint_t *);
int whline(WINDOW *, chtype, int);
int whline_set(WINDOW *, const cchar_t *, int);
chtype winch(WINDOW *);
int winchnstr(WINDOW *, chtype *, int);
int winchstr(WINDOW *, chtype *);
int winnstr(WINDOW *, char *, int);
int winnwstr(WINDOW *, wchar_t *, int);
int winsch(WINDOW *, chtype);
int winsdelln(WINDOW *, int);
int winsertln(WINDOW *);
int winsnstr(WINDOW *, const char *, int);
int wins_nwstr(WINDOW *, const wchar_t *, int);
int winsstr(WINDOW *, const char *);
int winstr(WINDOW *, char *);
int wins_wch(WINDOW *, const cchar_t *);
int wins_wstr(WINDOW *, const wchar_t *);
int win_wch(WINDOW *, cchar_t *);
int win_wchnstr(WINDOW *, cchar_t *, int);
int win_wchstr(WINDOW *, cchar_t *);
int winwstr(WINDOW *, wchar_t *);
int wmove(WINDOW *, int, int);
int wnoutrefresh(WINDOW *);
int wprintw(WINDOW *, const char *, ...);
int wredrawln(WINDOW *, int, int);
int wrefresh(WINDOW *);
int wscanw(WINDOW *, const char *, ...);
int wscrl(WINDOW *, int);
int wsetscrreg(WINDOW *, int, int);
int wstandend(WINDOW *);
int wstandout(WINDOW *);
void wsyncup(WINDOW *);
void wsyncdown(WINDOW *);
void wtimeout(WINDOW *, int);
int wtouchln(WINDOW *, int, int, int);
wchar_t *wunctrl(cchar_t *);
int wvline(WINDOW *, chtype, int);
int wvline_set(WINDOW *, const cchar_t *, int);


#endif
