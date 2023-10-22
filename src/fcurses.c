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

	return ret;
}

SCREEN *newterm(char *type, FILE *out, FILE *in)
{
    setupterm(type ? type : getenv("TERM"), fileno(stdout), NULL);

	SCREEN *ret = NULL;

	if ((ret = calloc(1, sizeof(SCREEN))) == NULL)
		goto fail;

	ret->outfd = out;
	ret->infd = in;

	ret->_infd = fileno(in);
	ret->_outfd = fileno(out);

	if ((ret->stdscr = newwin(0,0,0,0)) == NULL)
		goto fail;

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

    return ERR;
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

    return ERR;
}

int clear(void)
{
    return wclear(stdscr);
}

int erase(void)
{
    return wclear(stdscr);
}

int werase(WINDOW *win)
{
    return wclear(win);
}

int wnoutrefresh(WINDOW *win)
{
    if (win == NULL)
        return ERR;

    return ERR;
}

int wrefresh(WINDOW *win)
{
    if (wnoutrefresh(win) == ERR)
        return ERR;

    if (win->clearok) {
        if (wclear(win) == ERR)
            return ERR;
        if (redrawwin(win) == ERR)
            return ERR;

        win->clearok = FALSE;
    }

    return doupdate();
}

int refresh(void)
{
    return wrefresh(stdscr);
}

int waddch(WINDOW *win, const chtype ch __attribute__(( unused )))
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

int addch(const chtype ch)
{
    return waddch(stdscr, ch);
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

WINDOW *initscr()
{
	if ((stdterm = newterm(getenv("TERM"), stdout, stdin)) == NULL)
		return NULL;
	stdscr = stdterm->stdscr;
    refresh();
    doupdate();
	return stdscr;
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
