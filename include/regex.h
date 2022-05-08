#ifndef _REGEX_H
#define _REGEX_H

typedef long regoff_t;

#include <sys/types.h>

typedef struct {
	size_t  re_nsub;
	void   *priv;
	int     cflags;
} regex_t;

typedef struct {
	regoff_t rm_so;
	regoff_t rm_eo;
} regmatch_t;

#define REG_EXTENDED 0x1
#define REG_ICASE    0x2
#define REG_NEWLINE  0x4
#define REG_NOSUB    0x8

#define REG_NOTBOL   0x1
#define REG_NOTEOL   0x2

#define REG_NOMATCH   -1
#define REG_BADPAT    -2
#define REG_ECOLLATE  -3
#define REG_ECTYPE    -4
#define REG_EESCAPE   -5
#define REG_ESUBREG   -6
#define REG_EBRACK    -7
#define REG_EPAREN    -8
#define REG_EBRACE    -9
#define REG_BADBR    -10
#define REG_ERANGE   -11
#define REG_ESPACE   -12
#define REG_BADRPT   -13

extern int    regcomp(regex_t *restrict, const char *restrict, int);
extern size_t regerror(int, const regex_t *restrict, char *restrict, size_t);
extern int    regexec(const regex_t *restrict, const char *restrict, size_t, regmatch_t [restrict], int);
extern void   regfree(regex_t *);

#endif
