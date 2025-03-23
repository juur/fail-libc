#ifndef _WORDEXP_H
#define _WORDEXP_H

#include <stddef.h>

typedef struct {
    size_t we_wordc;
    char **we_wordv;
    size_t we_offs;
    int flags;
} wordexp_t;

#define WRDE_APPEND     (1<<0)
#define WRDE_DOOFFS     (1<<1)
#define WRDE_NOCMD      (1<<2)
#define WRDE_REUSE      (1<<3)
#define WRDE_SHOWERR    (1<<4)
#define WRDE_UNDEF      (1<<5)

#define WRDE_BADCHAR    (-1)
#define WRDE_BADVAL     (-2)
#define WRDE_CMDSUB     (-3)
#define WRDE_NOSPACE    (-4)
#define WRDE_SYNTAX     (-5)

int wordexp(const char *restrict, wordexp_t *restrict, int);
void wordfree(wordexp_t *);
#endif
