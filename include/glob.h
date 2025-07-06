#ifndef _GLOB_H
#define _GLOB_H

#include <sys/types.h>

#define GLOB_APPEND   (1<<0)
#define GLOB_DOOFFS   (1<<1)
#define GLOB_ERR      (1<<2)
#define GLOB_MARK     (1<<3)
#define GLOB_NOCHECK  (1<<4)
#define GLOB_NOESCAPE (1<<5)
#define GLOB_NOSORT   (1<<6)

#define GLOB_ABORTED  -1
#define GLOB_NOMATCH  -2
#define GLOB_NOSPACE  -3

typedef struct {
    size_t    gl_pathc;
    char    **gl_pathv;
    size_t    gl_offs;

    int       glp_flags;
    int       glp_rc;
    int       glp_presplit_cnt;
    int       glp_postsplit_cnt;
    
    char    **glp_presplit;
    char   ***glp_postsplit;
} glob_t;

extern int  glob(const char *restrict, int, int(*)(const char *, int), glob_t *restrict);
extern void globfree(glob_t *);

#endif
