#include <curses.h>
#include <term.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * public globals
 */

WINDOW *stdscr;
WINDOW *curscr;
int LINES = 0;
int COLS = 0;
int COLOR_PAIRS = 0;
int COLORS = 0;

/*
 * private globals
 */

static SCREEN *cur_screen;

/*
 * private functions
 */

static char *doupdate_bufptr;

static int _putchar_buffer(int c)
{
    *doupdate_bufptr++ = (char)c;

    return 0;
}

static int _putchar_cur_term(int c)
{
    unsigned char ch = (char)c;

    return write(cur_term->fd, &ch, 1);
}

/*
 * public functions
 */

bool has_colors(void)
{
    if (stdscr == NULL)
        return false;

    if (stdscr->scr->colors && stdscr->scr->pairs)
        return true;
    
    const int colors  = tigetnum("colors");
    const int pairs   = tigetnum("pairs");

    return (colors > 0 && pairs > 0);
}

static int init_extended_pair(int pair, int f, int b)
{
    if (stdscr == NULL)
        return ERR;

    if (pair < 0 || pair > stdscr->scr->pairs)
        return ERR;

    if (f < 0 || f > stdscr->scr->colors)
        return ERR;
    
    if (b < 0 || b > stdscr->scr->colors)
        return ERR;

    const char *initp = tigetstr("initp");

    if (initp == NULL || initp == (char *)-1)
        return ERR;
    
    return OK;
}

bool can_change_color(void)
{
    if (stdscr == NULL)
        return false;

    const bool ccc = tigetflag("ccc");

    if (ccc <= 0)
        return false;

    return true;
}

int init_pair(short pair, short f, short b)
{
    return init_extended_pair(pair, f, b);
}

int start_color(void)
{
    if (stdscr == NULL)
        return ERR;

    const int colors  = tigetnum("colors");
    const int pairs   = tigetnum("pairs");
    const char *initc = tigetstr("initc");

    if (colors <= 0 || pairs <= 0)
        return ERR;

    COLORS = colors;
    COLOR_PAIRS = pairs;

    stdscr->scr->colors = COLORS;
    stdscr->scr->pairs = COLOR_PAIRS;

    if (initc != NULL && initc != (char *)-1) {
    }

    //init_color(0, COLOR_WHITE, COLOR_BLACK);

    return OK;
}

WINDOW *newwin(int nlines, int ncols, int y, int x)
{
    WINDOW *ret;
    int lines = nlines ? nlines : (LINES - y);

    if ((ret = calloc(1, sizeof(WINDOW) + (sizeof(struct _fc_window_line_data) * (lines + 1)))) == NULL)
        return NULL;

    ret->x = x;
    ret->y = y;
    ret->lines = lines;
    ret->cols = ncols ? ncols : (COLS - x);
    ret->clearok = TRUE;

    for (int i = 0; i < (lines + 1); i++) {
        if ( (ret->line_data[i].line = calloc(1, sizeof(chtype) * (1 + ret->cols))) == NULL) {
            delwin(ret);
            return NULL;
        } else
            ret->line_data[i].touched = true;
    }

    return ret;
}

bool nc_use_env = TRUE;

void use_env(bool bf)
{
    nc_use_env = bf;
}

int keypad(WINDOW *win, bool bf)
{
    win->keypad = bf;

    return TRUE;
}

SCREEN *newterm(const char *type, FILE *out, FILE *in)
{
    if (out == NULL || in == NULL)
        return NULL;

    setvbuf(in, NULL, _IONBF, 0);
    setvbuf(out, NULL, _IONBF, 0);

    setupterm(type ? (char *)type : getenv("TERM"), fileno(out), NULL);

    SCREEN *ret = NULL;

    if ((ret = calloc(1, sizeof(SCREEN))) == NULL)
        goto fail;

    ret->term = cur_term;
    ret->outfd = out;
    ret->infd = in;

    ret->_infd = fileno(in);
    ret->_outfd = fileno(out);

    tcgetattr(ret->_infd, &ret->save_in);
    tcgetattr(ret->_outfd, &ret->save_out);

    if (tcgetattr(ret->_infd, &ret->shell_in) == -1) {
        goto fail;
    }

    if (tcgetattr(ret->_outfd, &ret->shell_out) == -1) {
        goto fail;
    }

    /* where should this go ? */
    struct termios tios;
    if (tcgetattr(ret->_outfd, &tios) == -1) {
        goto fail;
    }
    tios.c_oflag &= ~OCRNL;
    if (tcsetattr(ret->_outfd, 0, &tios) == -1) {
        goto fail;
    }

    if (tcgetattr(ret->_infd, &tios) == -1) {
        goto fail;
    }
    tios.c_iflag &= ~INPCK;
    if (tcsetattr(ret->_infd, 0, &tios) == -1) {
        goto fail;
    }

    if ((ret->defwin = newwin(0,0,0,0)) == NULL)
        goto fail;

    ret->buf_len = 16 * ret->defwin->lines * (ret->defwin->cols + 1);

    if ((ret->buffer = malloc(ret->buf_len)) == NULL) {
        goto fail;
    }

    return ret;

fail:
    if (ret) {
        if (ret->defwin)
            delwin(ret->defwin);
        free(ret);
    }

    return NULL;
}

int delwin(WINDOW *w)
{
    free(w);
    return OK;
}

/* The doupdate() function sends to the terminal the commands to perform any required changes. */
int doupdate(void)
{
    const char *home;

    if ((home = tiparm(tigetstr("home"))) == NULL || home == (char *)-1) {
        fprintf(stderr, "doupdate: home\n");
        return ERR;
    }

    const char *clear = tigetstr("clear");
    const char *el    = tigetstr("el");
    const char *civis = tigetstr("civis");
    const char *cnorm = tigetstr("cnorm");
    const char *vpa   = tigetstr("vpa");

    if (el == (char *)-1)
        el = NULL;
    if (civis == (char *)-1)
        civis = NULL;
    if (cnorm == (char *)-1)
        cnorm = NULL;
    if (vpa == (char *)-1)
        vpa = NULL;

    //memset(stdscr->scr->buffer, ' ', ret->buf_len);

    SCREEN *scr = stdscr->scr;
    doupdate_bufptr = scr->buffer;

    if (civis && cnorm)
        tputs(civis, 1, _putchar_buffer);

    if (clear)
        tputs(clear, 1, _putchar_buffer);
    else
        tputs(home, 1, _putchar_buffer);

    for (int i = 0; i < stdscr->lines; i++)
    {
        if (stdscr->line_data[i].touched) {
            for (int j = 0; j < stdscr->cols; j++) {
                if (stdscr->line_data[i].line[j])
                    *doupdate_bufptr++ = stdscr->line_data[i].line[j];
            }
            if (vpa) {
                *doupdate_bufptr++ = '\r';
                tputs(tiparm(vpa, i+1), 1, _putchar_buffer);
            } else
                *doupdate_bufptr++ = '\n';
            stdscr->line_data[i].touched = false;
        } else if (clear) {
            *doupdate_bufptr++ = '\n';
        } else if (el) {
            tputs(el, 1, _putchar_buffer);
            *doupdate_bufptr++ = '\n';
        } else {
            for (int j = 0; j < stdscr->cols; j++)
                *doupdate_bufptr++ = ' ';
            *doupdate_bufptr++ = '\n';
        }
    }

    if (cnorm)
        tputs(cnorm, 1, _putchar_buffer);

    scr->buf_ptr = doupdate_bufptr - scr->buffer;

    if (write(scr->_outfd, scr->buffer, scr->buf_ptr) < scr->buf_ptr) {
        fprintf(stderr, "doupdate: write\n");
        return ERR;
    }
    return OK;
}

int clearok(WINDOW *win, bool bf)
{
    win->clearok = bf;
    return OK;
}

int idlok(WINDOW *win, bool bf)
{
    win->idlok = bf;
    return OK;
}

int leaveok(WINDOW *win, bool bf)
{
    win->leaveok = bf;
    return OK;
}

int scrollok(WINDOW *win, bool bf)
{
    win->scrollok = bf;
    return OK;
}

/* The redrawwin() and wredrawln() functions inform the implementation that some or all of the information physically displayed for the specified window may have been corrupted. The redrawwin() function marks the entire window; wredrawln() marks only num_lines lines starting at line number beg_line. The functions prevent the next refresh operation on that window from performing any optimization based on assumptions about what is physically displayed there */
int wredrawln(WINDOW *win, int beg_line, int num_lines)
{
    if (win == NULL || beg_line > num_lines || num_lines > win->lines || beg_line < 0)
        return ERR;

    return OK;
}

int redrawwin(WINDOW *win)
{
    if (win == NULL)
        return ERR;

    return wredrawln(win, 0, win->lines);
}

int wclear(WINDOW *win)
{
    if (win == NULL)
        return ERR;

    clearok(win, TRUE);

    return OK;
}

int clear(void)
{
    return wclear(stdscr);
}

int erase(void)
{
    return werase(stdscr);
}

int werase(WINDOW *win)
{
    for (int i = 0; i < win->lines; i++) {
        memset(win->line_data[i].line, 0, sizeof(chtype) * win->cols);
        win->line_data[i].touched = true;
    }

    return OK;
}

/* The wnoutrefresh() function determines which parts of the terminal may need updating */
int wnoutrefresh(WINDOW *win)
{
    if (win == NULL)
        return ERR;

    return OK;
}

static void f_clearscr(void)
{
    const char *tmp;

    tmp = tiparm(tigetstr("clear"));
    if (tmp != NULL)
        tputs(tmp, 1, _putchar_cur_term);
}

/* The refresh() and wrefresh() functions refresh the current or specified window. The functions position the terminal’s cursor at the cursor position of the window, except that if the leaveok() mode has been enabled, they may leave the cursor at an arbitrary position.*/
int wrefresh(WINDOW *win)
{
    if (wnoutrefresh(win) == ERR) {
        fprintf(stderr, "wrefresh: wnoutrefresh: ERR\n");
        return ERR;
    }

    if (win->clearok) {
        win->clearok = FALSE;

        if (redrawwin(win) == ERR) {
            fprintf(stderr, "wrefresh: redrawwin: ERR\n");
            return ERR;
        }

        if (win != curscr)
            f_clearscr();

        win->clearok = FALSE;
    }

    if (win == curscr)
        f_clearscr();

    if (doupdate() == ERR)
        return ERR;

    if (win->leaveok == FALSE)
        wmove(win, win->y, win->x);

    return OK;
}

int refresh(void)
{
    return wrefresh(stdscr);
}

int waddch(WINDOW *win, const chtype ch)
{
    if (win == NULL)
        return ERR;

    win->line_data[win->y].line[win->x++] = ch;
    win->line_data[win->y].touched = true;

    if (win->x > win->cols) {
        win->x = 0;
        win->y++;
    }
    wmove(win, win->y, win->x);

    return OK;
}

int mvwaddch(WINDOW *win, int y, int x, const chtype ch)
{
    if (wmove(win, y, x) == ERR)
        return ERR;

    return waddch(win, ch);
}

int addch(const chtype ch)
{
    return waddch(stdscr, ch);
}

int mvaddch(int y, int x, const chtype ch)
{
    return mvwaddch(stdscr, y, x, ch);
}

int echochar(const chtype ch)
{
    if (waddch(stdscr, ch) == ERR)
        return ERR;

    return wrefresh(stdscr);
}

int wechochar(WINDOW *win, const chtype ch)
{
    if (waddch(win, ch) == ERR)
        return ERR;

    return wrefresh(win);
}

void delscreen(SCREEN *sp)
{
    if (sp) {
        free(sp);
    }
}

bool isendwin(void)
{
    if (stdscr == NULL)
        return FALSE;

    return cur_screen->isendwin;
}

int endwin(void)
{
    if (cur_screen == NULL)
        return ERR;

    tcsetattr(cur_screen->_infd, TCSANOW, &cur_screen->save_in);
    tcsetattr(cur_screen->_outfd, TCSANOW, &cur_screen->save_out);

    return TRUE;
}

WINDOW *initscr()
{
    if ((cur_screen = newterm(getenv("TERM"), stdout, stdin)) == NULL)
        return NULL;
    curscr = stdscr = cur_screen->defwin;
    stdscr->scr = cur_screen;
    refresh();
    doupdate();
    fprintf(stderr, "cur_screen[%d,%d]\nstdscr[%d,%d]\nLINES=%d COLS=%d\n",
            cur_screen->term->columns,
            cur_screen->term->lines,
            stdscr->cols,
            stdscr->lines,
            LINES,
            COLS);
    return stdscr;
}

int beep(void)
{
    char *tp;

    if ((tp = tiparm(tigetstr("bel"))) != NULL)
        return tputs(tp, 1, _putchar_cur_term);
    else if ((tp = tiparm(tigetstr("flash"))) != NULL)
        return tputs(tp, 1, _putchar_cur_term);
    else
        return ERR;
}

int wmove(WINDOW *win, int y, int x)
{
    if (win == NULL || y < 0 || x < 0 || y > win->lines || x > win->cols)
        return ERR;

    win->x = x;
    win->y = y;

    char *tp;

    if ((tp = tiparm(tigetstr("cup"), y, x)) == NULL)
        return ERR;

    return tputs(tp, 1, _putchar_cur_term);
}

int move(int y, int x)
{
    return wmove(stdscr, y, x);
}

/* add a wide-character string */

int waddnwstr(WINDOW *win, const wchar_t *wstr, int n)
{
    if (win == NULL || wstr == NULL || n == 0)
        return ERR;
    /* TODO */
    return OK;
}

int waddwstr(WINDOW *win, const wchar_t *wstr)
{
    return waddnwstr(win, wstr, -1);
}

/* add a string of multi-byte characters */

int waddnstr(WINDOW *win, const char *str, int n)
{
    //wchar_t *dest = NULL;
    int rc = ERR;

    /*
    len = mbstowcs(NULL, str, (n == -1) ? 0 : n);

    if (len == (size_t)-1)
        goto done;

    if ((dest = calloc(1, len + 1)) == NULL)
        goto done;

    if (mbstowcs(dest, str, (n == -1) ? 0 : n) == (size_t)-1)
        goto done;

    rc = waddwstr(win, dest);

done:
    if (dest)
        free(dest);
    */

    const char *tmp = str;
    int cnt = n;

    while (cnt && *tmp)
    {
        waddch(win, *tmp);
        tmp++; cnt--;
    }

    return rc;
}

int waddstr(WINDOW *win, const char *str)
{
    return waddnstr(win, str, -1);
}

int mvwaddnstr(WINDOW *win, int y, int x, const char *str, int n)
{
    if (wmove(win, y, x) == ERR)
        return ERR;
    return waddnstr(win, str, n);
}

int addstr(const char *str)
{
    return waddnstr(stdscr, str, -1);
}

int mvaddstr(int y, int x, const char *str)
{
    return mvwaddnstr(stdscr,y,x,str,-1);
}

/* add string of single-byte characters and renditions */

int waddchnstr(WINDOW *win, const chtype *chstr, int n)
{
    if (win == NULL || chstr == NULL || n == 0)
        return ERR;
    /* TODO */
    return ERR;
}

int mvwaddchnstr(WINDOW *win, int y, int x, const chtype *chstr,
int n)
{
    if (wmove(win, y, x) == ERR)
        return ERR;

    return waddchnstr(win, chstr, n);
}

int mvwaddchstr(WINDOW *win, int y, int x, const chtype *chstr)
{
    if (wmove(win, y, x) == ERR)
        return ERR;

    return waddchnstr(win, chstr, -1);
}

int waddchstr(WINDOW *win, const chtype *chstr)
{
    return waddchnstr(win, chstr, -1);
}

int addchstr(const chtype *chstr)
{
    return waddchnstr(stdscr, chstr, -1);
}

/* attribute */

int wattroff(WINDOW *win, int attrs)
{
    win->attr &= ~attrs;

    return OK;
}

int wattr_off(WINDOW *win, attr_t attrs __attribute((unused)), void *opts __attribute((unused)))
{
    if (win == NULL)
        return ERR;

    return OK;
}

int wattr_on(WINDOW *win, attr_t attrs __attribute((unused)), void *opts __attribute((unused)))
{
    if (win == NULL)
        return ERR;

    return OK;
}

int wattron(WINDOW *win, int attrs)
{
    if (win == NULL)
        return ERR;

    win->attr |= attrs;

    return OK;
}

int wattrset(WINDOW *win, int attrs)
{
    if (win == NULL)
        return ERR;

    win->attr = attrs;

    return OK;
}

int attroff(int attrs)
{
    return wattroff(stdscr, attrs);
}

int attron(int attrs)
{
    return wattron(stdscr, attrs);
}

int attrset(int attrs)
{
    return wattrset(stdscr, attrs);
}

int wclrtoeol(WINDOW *win)
{
    for (int i = win->x; i < win->cols; i++)
        win->line_data[win->y].line[i] = 0;

    win->line_data[win->y].touched = true;

    return OK;
}

int clrtoeol(void)
{
    return wclrtoeol(stdscr);
}

int mvwprintw(WINDOW *win, int y, int x, const char *fmt, ...)
{
    if (wmove(win, y, x) == ERR)
        return ERR;

    char buf[BUFSIZ];

    va_list ap;
    va_start (ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end (ap);

    return waddstr(win, buf);
}

int mvprintw(int y, int x, const char *fmt, ...)
{
    if (wmove(stdscr, y, x) == ERR)
        return ERR;

    char buf[BUFSIZ];

    va_list ap;
    va_start (ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end (ap);

    return waddstr(stdscr, buf);
}

int wprintw(WINDOW *win, const char *fmt, ...)
{
    char buf[BUFSIZ];

    va_list ap;
    va_start (ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end (ap);

    return waddstr(win, buf);
}

/*
static void hexdump(FILE *fp, const char *tmp)
{
    if (tmp && tmp != (char *)-1)
        while (*tmp)
        {
            if (isprint(*tmp)) fprintf(fp, "%c", *tmp);
            else
                fprintf(fp, "0x%03x", *tmp);

            tmp++;
            if (*tmp)
                fprintf(fp, " ");
        }
}
*/

static int _getch(TERMINAL *term, int out[1])
{
    char buf[8] = {0};
    ssize_t len;

    len = read(term->fd, buf, sizeof(buf));

    //fprintf(stderr, "_getch: read: %d\n", len);

    if (len == -1)
        return -1;

    if (len == 0) {
        *out = 0;
        return 0;
    }

    if (len == 1) {
        *out = buf[0];
        return 0;
    }

    if (buf[0] == '\e') {
        //fprintf(stderr, "_getch: checking ESC key match\n");
        //fprintf(stderr, "_getch: <");
        //hexdump(stderr, buf);
        //fprintf(stderr, ">\n");

        for (int i = 0; i < NUM_KEYS; i++)
        {
            if (cur_term->keys[i].id == NULL)
                continue;

            if (strncmp(buf, cur_term->keys[i].id, cur_term->keys[i].len))
                continue;

            *out = i;

            if (cur_term->keys[i].len != len) {
                ; /* handle unread characters TODO */
            }

            return 0;
        }

        return -1;

        //fprintf(stderr, "_getch: found a match!\n");
    }

    return -1;
}

int wgetch(WINDOW *win)
{
    int ret = 0;

    if (win == NULL)
        return ERR;

    if (_getch(win->scr->term, &ret) == -1)
        return ERR;

    fprintf(stderr, "wgetch: returning %d\n", ret);

    return ret;
}

int getch()
{
    return wgetch(stdscr);
}

int halfdelay(int tenths)
{
    struct termios tios;

    if (cur_term == NULL)
        return ERR;

    if (tcgetattr(cur_term->fd, &tios) == -1)
        return ERR;

    tios.c_cc[VTIME] = tenths;

    if (tcsetattr(cur_term->fd, 0, &tios) == -1)
        return ERR;

    return OK;
}

int baudrate(void)
{
    struct termios tio;

    if (cur_term == NULL)
        return ERR;

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return ERR;

    return (int) cfgetospeed(&tio);
}

char *termname(void)
{
    if (cur_term == NULL)
        return NULL;

    return ((struct terminfo *)cur_term->terminfo)->name;
}

int cbreak(void)
{
    struct termios tio;

    memset(&tio, 0, sizeof(tio));

    if (stdscr == NULL)
        return ERR;

    if (tcgetattr(stdscr->scr->_infd, &tio) == -1)
        return ERR;

    //tio.c_lflag &= ~(ICANON|IEXTEN);

    tio.c_lflag &= ~(ICANON|ECHO);
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 1;

    if (tcsetattr(stdscr->scr->_infd, TCSANOW, &tio) == -1)
        return ERR;

    return OK;

}

int nl(void)
{
    struct termios tio;

    if (stdscr == NULL)
        return ERR;

    if (tcgetattr(stdscr->scr->_infd, &tio) == -1)
        return ERR;

    tio.c_iflag |= ICRNL;

    if (tcsetattr(stdscr->scr->_infd, TCSANOW, &tio) == -1)
        return ERR;

    stdscr->nl = TRUE;

    return OK;
}

int nonl(void)
{
    struct termios tio;

    if (cur_term == NULL)
        return ERR;

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return ERR;

    tio.c_iflag &= ~ICRNL;

    if (tcsetattr(cur_term->fd, 0, &tio) == -1)
        return ERR;

    stdscr->nl = FALSE;

    return OK;
}

int meta(WINDOW *win, bool bf)
{
    struct termios tio;
    char *cap;

    if (cur_term == NULL)
        return ERR;

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return ERR;

    if (bf == TRUE) {
        if ((cap = tiparm(tigetstr("smm"))) != NULL)
            tputs(cap, 1, _putchar_cur_term);
        tio.c_cflag &= ~CSIZE;
        tio.c_cflag |= CS8;
    } else if (bf == FALSE) {
        if ((cap = tiparm(tigetstr("rmm"))) != NULL)
            tputs(cap, 1, _putchar_cur_term);
        tio.c_cflag &= ~CSIZE;
        tio.c_cflag |= CS7;
    } else {
        warnx("meta: unknown bool value");
        return ERR;
    }

    if (tcsetattr(cur_term->fd, 0, &tio) == -1) {
        warn("meta: tcsetattr");
        return ERR;
    }

    win->meta = bf;

    return OK;
}

int nocbreak(void)
{
    struct termios tio;

    if (cur_term == NULL)
        return ERR;

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return ERR;

    tio.c_lflag |= (ICANON|IEXTEN);

    if (tcsetattr(cur_term->fd, 0, &tio) == -1)
        return ERR;

    return 0;

}

char erasechar(void)
{
    struct termios tio;

    if (cur_term == NULL)
        return ERR;

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return 0;

    return tio.c_cc[VERASE];
}

int echo(void)
{
    struct termios tio;

    if (cur_term == NULL)
        return ERR;

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return ERR;

    tio.c_lflag |= ECHO;

    if (tcsetattr(cur_term->fd, 0, &tio) == -1)
        return ERR;

    return 0;
}

int noecho(void)
{
    struct termios tio;

    if (cur_term == NULL)
        return ERR;

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return ERR;

    tio.c_lflag &= ~ECHO;

    if (tcsetattr(cur_term->fd, 0, &tio) == -1)
        return ERR;

    return 0;
}

char killchar(void)
{
    struct termios tio;

    if (cur_term == NULL)
        return ERR;

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return 0;

    return tio.c_cc[VKILL];
}

/* vim: set expandtab ts=4 sw=4: */
