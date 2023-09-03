#ifndef _LOCALE_H
#define _LOCALE_H

struct lconv {
	char    *currency_symbol;
	char    *decimal_point;
	char     frac_digits;
	char    *grouping;
	char    *int_curr_symbol;
	char     int_frac_digits;
	char     int_n_cs_precedes;
	char     int_n_sep_by_space;
	char     int_n_sign_posn;
	char     int_p_cs_precedes;
	char     int_p_sep_by_space;
	char     int_p_sign_posn;
	char    *mon_decimal_point;
	char    *mon_grouping;
	char    *mon_thousands_sep;
	char    *negative_sign;
	char     n_cs_precedes;
	char     n_sep_by_space;
	char     n_sign_posn;
	char    *positive_sign;
	char     p_cs_precedes;
	char     p_sep_by_space;
	char     p_sign_posn;
	char    *thousands_sep;
};

#include <stddef.h>

#define LC_COLLATE	1
#define LC_CTYPE	2
#define LC_MESSAGES	3
#define LC_MONETARY	4
#define LC_NUMERIC	5
#define LC_TIME		6
#define LC_ALL		7

#define LC_COLLATE_MASK  (1 << (LC_COLLATE-1))
#define LC_CTYPE_MASK    (1 << (LC_CTYPE-1))
#define LC_MESSAGES_MASK (1 << (LC_MESSAGES-1))
#define LC_MONETARY_MASK (1 << (LC_MONETARY-1))
#define LC_NUMERIC_MASK  (1 << (LC_NUMERIC-1))
#define LC_TIME_MASK     (1 << (LC_TIME-1))
#define LC_ALL_MASK      ((1 << (LC_ALL-1))-1)

typedef void * locale_t;

extern locale_t      duplocale(locale_t);
extern void          freelocale(locale_t);
extern struct lconv *localeconv(void);
extern locale_t      newlocale(int, const char *, locale_t);
extern char         *setlocale(int, const char *);
extern locale_t      uselocale (locale_t);

#endif
