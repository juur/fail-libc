#ifndef _SYS_TIME_H
#define _SYS_TIME_H

#include <features.h>
#include <sys/types.h>
#include <sys/select.h>

struct itimerval {
    struct timeval it_interval;
    struct timeval it_value;
};

#define ITIMER_REAL 0
#define ITIMER_VIRTUAL 1
#define ITIMER_PROF 2

extern int gettimeofday(struct timeval *tv, void *tz);
extern int getitimer(int which, struct itimerval *curr_value);
extern int setitimer(int which, const struct itimerval *restrict new_value, struct itimerval *restrict old_value);

#endif
