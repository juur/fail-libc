#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H

#include <features.h>
#include <sys/types.h>
#include <stdint.h>

struct timeval {
	time_t      tv_sec;
	suseconds_t tv_usec;
};

#define FD_SETSIZE	1024
#define _FD_SET_FDS	(FD_SETSIZE/sizeof(uint64_t) + 1)

typedef uint64_t _fds_t;

typedef struct {
	_fds_t fds[_FD_SET_FDS];
} fd_set;

#include <signal.h>
#include <time.h>

extern int pselect(int, fd_set *restrict, fd_set *restrict, fd_set *restrict,
		           const struct timespec *restrict, const sigset_t *restrict);
extern int  select(int, fd_set *restrict, fd_set *restrict, fd_set *restrict,
                   struct timeval *restrict);

#include <string.h>

#define BIT_IDX(x)			( (x)/(sizeof(_fds_t)*8) )
#define BIT_OFFSET(x)		( ((sizeof(_fds_t)*8)-1)-((x)%(sizeof(_fds_t)*8)) )

#define FD_SET(y, x)	( (x)->fds[BIT_IDX(y)] |= BIT_OFFSET(y) )
#define FD_CLR(y, x)	( (x)->fds[BIT_IDX(y)] &= ~(BIT_OFFSET(y)) )
#define FD_ISSET(y, x)	( ((x)->fds[BIT_IDX(y)] & BIT_OFFSET(y)) == BIT_OFFSET(y) )
#define FD_ZERO(x)		( (void)memset((x)->fds, 0, sizeof((x)->fds)) )

#endif
