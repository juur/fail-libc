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

/*
 * private globals
 */

static SCREEN *stdterm;

/*
 * private functions
 */

static int _putchar(int c)
{
    char ch = (char)c;

    return write(cur_term->fd, &ch, 1);
}

/*
 * public functions
 */

WINDOW *newwin(int nlines, int ncols, int y, int x)
{
	WINDOW *ret;

	if ((ret = calloc(1, sizeof(WINDOW))) == NULL)
		return NULL;

	ret->x = x;
	ret->y = y;
	ret->lines = nlines;
	ret->cols = ncols;
    ret->clearok = TRUE;

	return ret;
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


	if ((ret->stdscr = newwin(0,0,0,0)) == NULL)
		goto fail;

    ret->stdscr->scr = ret;

	return ret;

fail:
	if (ret) {
		if (ret->stdscr)
			delwin(ret->stdscr);
		free(ret);
	}

	return NULL;
}

int delwin(WINDOW *w)
{
	free(w);
	return 0;
}

int doupdate(void)
{
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

int wredrawwin(WINDOW *win, int beg_line, int num_lines)
{
    if (win == NULL || beg_line > num_lines || num_lines > win->lines || beg_line < 0)
        return ERR;

    if (win->clearok) {
        win->clearok = FALSE;
    }

    return OK;
}

int redrawwin(WINDOW *win)
{
    if (win == NULL)
        return ERR;

    return wredrawwin(win, 0, win->lines);
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

int wnoutrefresh(WINDOW *win)
{
    if (win == NULL)
        return ERR;

    return OK;
}

static void f_clearscr(void)
{
    const char *tmp;

    tmp = tiparm("clear");
    if (tmp != NULL)
        putp(tmp);
}

int wrefresh(WINDOW *win)
{
    if (wnoutrefresh(win) == ERR)
        return ERR;

    if (win->clearok) {
        win->clearok = FALSE;

        if (redrawwin(win) == ERR)
            return ERR;

        if (win != curscr)
            f_clearscr();

        win->clearok = FALSE;
    }

    if (win->leaveok == FALSE)
        wmove(win, win->y, win->x);

    if (win == curscr)
        f_clearscr();

    return doupdate();
}

int refresh(void)
{
    return wrefresh(stdscr);
}

int waddch(WINDOW *win, const chtype ch)
{
    if (win == NULL)
        return ERR;

    win->line_data[win->y].line[win->x] = ch;

    win->x++;
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

    return stdterm->isendwin;
}

int endwin(void)
{
    tcsetattr(stdterm->_infd, 0, &stdterm->save_in);
    tcsetattr(stdterm->_outfd, 0, &stdterm->save_out);

    return TRUE;
}

WINDOW *initscr()
{
	if ((stdterm = newterm(getenv("TERM"), stdout, stdin)) == NULL)
		return NULL;
	curscr = stdscr = stdterm->stdscr;
    refresh();
    doupdate();
	return stdscr;
}

int beep(void)
{
    char *tp;

    if ((tp = tiparm("bel")) != NULL)
        return tputs(tp, 1, _putchar);
    else if ((tp = tiparm("flash")) != NULL)
        return tputs(tp, 1, _putchar);
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

    if ((tp = tiparm("cup", y, x)) == NULL)
        return ERR;

    return tputs(tp, 1, _putchar);
}

int move(int y, int x)
{
    return wmove(stdscr, y, x);
}

/* add a wide-character string */

int waddnwstr(WINDOW *win, const wchar_t *wstr, int n)
{
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
    size_t len = 0;
    wchar_t *dest = NULL;
    int rc = ERR;

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

int wattron(WINDOW *win, int attrs)
{
    win->attr |= attrs;

    return OK;
}

int wattrset(WINDOW *win, int attrs)
{
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

int wgetch(WINDOW *win)
{
    char ret;

    if (read(win->scr->_infd, &ret, 1) == -1)
        return ERR;

    return (int)ret;
}

int getch()
{
    return wgetch(stdscr);
}

int halfdelay(int tenths)
{
    struct termios tios;

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

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return ERR;

    return (int) cfgetospeed(&tio);
}

char *termname(void)
{
    return ((struct terminfo *)cur_term->terminfo)->name;
}

int cbreak(void)
{
    struct termios tio;

    memset(&tio, 0, sizeof(tio));

    if (tcgetattr(stdscr->scr->_infd, &tio) == -1)
        return ERR;

    //tio.c_lflag &= ~(ICANON|IEXTEN);

    tio.c_lflag &= ~(ICANON|ECHO);
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 0;

    if (tcsetattr(stdscr->scr->_infd, TCSANOW, &tio) == -1)
        return ERR;

    return OK;

}

int nl(void)
{
    struct termios tio;

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

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return ERR;

    if (bf == TRUE) {
        if ((cap = tiparm("smm")) != NULL)
            tputs(cap, 1, _putchar);
        tio.c_cflag &= ~CSIZE;
        tio.c_cflag |= CS8;
    } else if (bf == FALSE) {
        if ((cap = tiparm("rmm")) != NULL)
            tputs(cap, 1, _putchar);
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

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return 0;

    return tio.c_cc[VERASE];
}

int echo(void)
{
    struct termios tio;

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

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return 0;

    return tio.c_cc[VKILL];
}


