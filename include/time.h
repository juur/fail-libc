#ifndef TIME_H
#define TIME_H

#include <sys/types.h>
#include <stddef.h>

struct tm {
	int tm_sec,tm_min,tm_hour,tm_mday;
	int	tm_mon,tm_year,tm_wday,tm_yday,tm_isdst;
};

struct timespec {
	time_t tv_sec;
	long   tv_nsec;
};

int nanosleep(const struct timespec *req, struct timespec *rem);

#endif
