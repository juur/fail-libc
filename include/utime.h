#ifndef _UTIME_H
#define _UTIME_H

#include <features.h>
#include <sys/types.h>

struct utimbuf {
	time_t actime;
	time_t modtime;
};

int utime(const char *, const struct utimbuf *);

#endif
