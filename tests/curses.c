#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <curses.h>

int main(void)
{
    //setlocale(LC_ALL, "");
    initscr();
    printw("Hello, world!");
    refresh();
    sleep(2);
    endwin();
    return(EXIT_SUCCESS);
}
