#include <curses.h>
#include <stdio.h>
#include <stdlib.h>

WINDOW *stdscr;
static SCREEN *stdterm;

SCREEN *newterm(char *type, FILE *out, FILE *in)
{
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

WINDOW *initscr()
{
	if ((stdterm = newterm(NULL, stdout, stdin)) == NULL)
		return NULL;
	stdscr = stdterm->stdscr;
	return stdscr;
}
