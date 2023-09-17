#ifndef ERR_H
#define ERR_H

#include <features.h>

extern __attribute__((noreturn)) void err(int eval, const char *fmt, ...);
extern __attribute__((noreturn)) void errx(int eval, const char *fmt, ...);
extern void warn(const char *fmt, ...);
extern void warnx(const char *fmt, ...);
#endif
