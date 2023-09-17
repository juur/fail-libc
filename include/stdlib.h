#ifndef STDLIB_H
#define STDLIB_H

#include <features.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define MB_CUR_MAX (__get_mb_cur_max())

typedef struct {
	int quot;
	int rem;
} div_t;

typedef struct {
	long quot;
	long rem;
} ldiv_t;

typedef struct {
	long long quot;
	long long rem;
} lldiv_t;

#include <stddef.h>
#include <sys/wait.h>
#include <limits.h>
#include <math.h>

extern __attribute__((noreturn)) void _Exit(int);
extern long          a64l(const char *);
extern void          abort(void);
extern int           abs(int);
extern int           atexit(void (*)(void));
extern double        atof(const char *);
extern int           atoi(const char *);
extern long          atol(const char *);
extern long long     atoll(const char *);
extern void         *bsearch(const void *, const void *, size_t, size_t, int (*)(const void *, const void *));
extern void         *calloc(size_t, size_t);
extern div_t         div(int, int);
extern double        drand48(void);
extern double        erand48(unsigned short [3]);
extern __attribute__((noreturn)) void exit(int);
extern void          free(void *);
extern char         *getenv(const char *);
extern int           getsubopt(char **, char *const *, char **);
extern int           grantpt(int);
extern char         *initstate(unsigned, char *, size_t);
extern long          jrand48(unsigned short [3]);
extern char         *l64a(long);
extern long          labs(long);
extern void          lcong48(unsigned short [7]);
extern ldiv_t        ldiv(long, long);
extern long long     llabs(long long);
extern lldiv_t       lldiv(long long, long long);
extern long          lrand48(void);
extern void         *malloc(size_t);
extern int           mblen(const char *, size_t);
extern size_t        mbstowcs(wchar_t *, const char *, size_t);
extern int           mbtowc(wchar_t *, const char *, size_t);
extern char         *mkdtemp(char *);
extern int           mkstemp(char *);
extern long          mrand48(void);
extern long          nrand48(unsigned short [3]);
extern int           posix_memalign(void **, size_t, size_t);
extern int           posix_openpt(int);
extern char         *ptsname(int);
extern int           putenv(char *);
extern void          qsort(void *, size_t, size_t, int (*)(const void *, const void *));
extern int           rand(void);
extern int           rand_r(unsigned *);
extern long          random(void);
extern void         *realloc(void *, size_t);
extern char         *realpath(const char *, char *);
extern unsigned short *seed48(unsigned short [3]);
extern int           setenv(const char *, const char *, int);
extern void          setkey(const char *);
extern char         *setstate(char *);
extern void          srand(unsigned);
extern void          srand48(long);
extern void          srandom(unsigned);
extern double        strtod(const char *, char **);
extern float         strtof(const char *, char **);
extern long          strtol(const char *, char **, int);
extern long double   strtold(const char *, char **);
extern long long     strtoll(const char *, char **, int);
extern unsigned long strtoul(const char *, char **, int);
extern unsigned long long strtoull(const char *, char **, int);
extern int           system(const char *);
extern int           unlockpt(int);
extern int           unsetenv(const char *);
extern size_t        wcstombs(char *, const wchar_t *, size_t);
extern int           wctomb(char *, wchar_t);
#endif
