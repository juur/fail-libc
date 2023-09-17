#ifndef _SYS_TIME_H
#define _SYS_TIME_H

#include <features.h>
#include <sys/types.h>
#include <sys/select.h>

extern int gettimeofday(struct timeval *tv, void *tz);

#endif
