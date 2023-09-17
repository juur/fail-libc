#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H

#include <features.h>
#include <sys/types.h>
#include <signal.h>

typedef enum { P_ALL=3, P_PGID=2, P_PID=1 } idtype_t;

pid_t  wait(int *);
int    waitid(idtype_t, id_t, siginfo_t *, int);
pid_t  waitpid(pid_t, int *, int);

/* taken from musl */

#define WNOHANG    1
#define WNOTRACE   2
#define WEXITED    4
#define WCONTINUED 8

#define WEXITSTATUS(s) (((s) & 0xff00) >> 8)
#define WTERMSIG(s) ((s) & 0x7f)
#define WSTOPSIG(s) WEXITSTATUS(s)
#define WIFEXITED(s) (!WTERMSIG(s))
#define WIFSTOPPED(s) ((short)((((s)&0xffff)*0x10001)>>8) > 0x7f00)
#define WIFSIGNALED(s) (((s)&0xffff)-1U < 0xffu)

#endif
