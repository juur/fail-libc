#ifndef _CTYPE_H
#define _CTYPE_H

#include <features.h>
#include <locale.h>

extern int   isalnum(int);
extern int   isalnum_l(int, locale_t);
extern int   isalpha(int);
extern int   isalpha_l(int, locale_t);
extern int   isascii(int);
extern int   isblank(int);
extern int   isblank_l(int, locale_t);
extern int   iscntrl(int);
extern int   iscntrl_l(int, locale_t);
extern int   isdigit(int);
extern int   isdigit_l(int, locale_t);
extern int   isgraph(int);
extern int   isgraph_l(int, locale_t);
extern int   islower(int);
extern int   islower_l(int, locale_t);
extern int   isprint(int);
extern int   isprint_l(int, locale_t);
extern int   ispunct(int);
extern int   ispunct_l(int, locale_t);
extern int   isspace(int);
extern int   isspace_l(int, locale_t);
extern int   isupper(int);
extern int   isupper_l(int, locale_t);
extern int   isxdigit(int);
extern int   isxdigit_l(int, locale_t);
extern int   toascii(int);
extern int   tolower(int);
extern int   tolower_l(int, locale_t);
extern int   toupper(int);
extern int   toupper_l(int, locale_t);

#endif
