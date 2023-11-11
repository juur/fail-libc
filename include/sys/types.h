#ifndef SYS_TYPES_H
#define SYS_TYPES_H

#include <features.h>

typedef int           clock_t;
typedef int           clockid_t;
typedef int           pid_t;
typedef int           gid_t;
typedef int           uid_t;
typedef int           id_t;
typedef long          blkcnt_t;
typedef long          blksize_t;
typedef long          off_t;
typedef long          ssize_t;
typedef long          suseconds_t;
typedef long          time_t;
typedef unsigned long fsblkcnt_t;
typedef unsigned long fsfilcnt_t;
typedef unsigned int  mode_t;
typedef unsigned int  dev_t;
typedef unsigned int  socklen_t;
typedef unsigned long ino_t;
typedef unsigned long nlink_t;
typedef unsigned long size_t;
typedef void *        timer_t;

typedef struct {
    void   *stackaddr;
    size_t  stacksize;
    int     cancelstate;
    int     canceltype;
} pthread_attr_t;

typedef struct {
    int dummy;
} pthread_cond_t;

pid_t gettid(void);

#endif
