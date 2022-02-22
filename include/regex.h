#ifndef _REGEX_H
#define _REGEX_H

typedef long regoff_t;

typedef struct {
	size_t re_nsub;
} regex_t;

typedef struct {
	regoff_t rm_so;
	regoff_t rm_eo;
} regmatch_t;

#define REG_EXTENDED 0x1
#define REG_ICASE    0x2
#define REG_NEWLINE  0x4
#define REG_NOSUB    0x8
#endif
