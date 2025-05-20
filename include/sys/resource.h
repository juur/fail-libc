#ifndef _SYS_RESOURCE_H
#define _SYS_RESOURCE_H

#include <features.h>

#define PRIO_PROCESS 0
#define PRIO_PGRP    1
#define PRIO_USER    2

#define RLIM_INFINITY (~0UL)
#define RLIM_SAVED_MAX RLIM_INFINITY
#define RLIM_SAVED_CUR RLIM_INFINITY

#define RUSAGE_SELF 0
#define RUSAGE_CHILDREN 1

/* constants might need to change to match Linux API */
#define RLIMIT_CORE 0
#define RLIMIT_CPU 1
#define RLIMIT_DATA 2
#define RLIMIT_FSIZE 3
#define RLIMIT_NOFILE 4
#define RLIMIT_STACK 5
#define RLIMIT_AS 6

typedef unsigned int rlim_t;

#include <sys/types.h>

struct rlimit {
    rlim_t rlim_cur;
    rlim_t rlim_max;
};

#include <sys/time.h>

struct rusage {
    struct timeval ru_utime;
    struct timeval ru_stime;
};

int getpriority(int which, id_t who);
int setpriority(int which, id_t who, int prio);
int getrlimit(int, struct rlimit *);
int setrlimit(int, const struct rlimit *);
int getrusage(int, struct rusage *);


#endif
