#ifndef _UCHAR_H
#define _UCHAR_H

#include <sys/types.h>

typedef unsigned short char16_t;
typedef unsigned int   char32_t;

#include <wchar.h>

extern size_t mbrtoc16(char16_t *restrict pc16, const char *restrict s, size_t n, mbstate_t *restrict ps);
extern size_t c16rtomb(char *restrict s, char16_t c16, mbstate_t *restrict ps);
extern size_t mbrtoc32(char32_t *restrict pc32, const char *restrict s, size_t n, mbstate_t *restrict ps);
extern size_t c32rtomb(char *restrict s, char32_t c32, mbstate_t *restrict ps);

#endif
