#ifndef _STRINGS_H
#define _STRINGS_H

#include <features.h>
#include <sys/types.h>
#include <locale.h>

int strcasecmp(const char *, const char *);
int strcasecmp_l(const char *, const char *, locale_t);
int strncasecmp(const char *, const char *, size_t);
int strncasecmp_l(const char *, const char *, size_t, locale_t);

#endif
