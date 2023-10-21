#include <curses.h>
#include <term.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

WINDOW *stdscr;
static SCREEN *stdterm;

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

int wrefresh(WINDOW *win)
{
    if (win == NULL)
        return ERR;
    return OK;
}

int refresh(void)
{
    return wrefresh(stdscr);
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

WINDOW *initscr()
{
	if ((stdterm = newterm(getenv("TERM"), stdout, stdin)) == NULL)
		return NULL;
	stdscr = stdterm->stdscr;
    refresh();
    doupdate();
	return stdscr;
}
