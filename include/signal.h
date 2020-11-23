#ifndef SIGNAL_H
#define SIGNAL_H

#include <sys/types.h>
#include <time.h>
#include <pthread.h>

#define SIGABRT 6

typedef unsigned long sigset_t;

typedef struct {
	void *ss_sp;
	size_t ss_size;
	int ss_flags;
} stack_t;

union sigval {
	int    sival_int;
	void  *sival_ptr;
};

typedef struct {
	int           si_signo;
	int           si_code;
	int           si_errno;
	pid_t         si_pid;
	uid_t         si_uid;
	void         *si_addr;
	int           si_status;
	long          si_band;
	union sigval  si_value;
} siginfo_t;

struct sigaction {
	void   (*sa_handler)(int);
	sigset_t sa_mask;
	int      sa_flags;
	void   (*sa_sigaction)(int, siginfo_t *, void *);
};

int    kill(pid_t, int);
int    killpg(pid_t, int);
void   psiginfo(const siginfo_t *, const char *);
void   psignal(int, const char *);
int    pthread_kill(pthread_t, int);
int    pthread_sigmask(int, const sigset_t *, sigset_t *);
int    raise(int);
int    sigaction(int, const struct sigaction *, struct sigaction *);
int    sigaddset(sigset_t *, int);
int    sigaltstack(const stack_t *, stack_t *);
int    sigdelset(sigset_t *, int);
int    sigemptyset(sigset_t *);
int    sigfillset(sigset_t *);
int    sighold(int);
int    sigignore(int);
int    siginterrupt(int, int);
int    sigismember(const sigset_t *, int);
void (*signal(int, void (*)(int)))(int);
int    sigpause(int);
int    sigpending(sigset_t *);
int    sigprocmask(int, const sigset_t *, sigset_t *);
int    sigqueue(pid_t, int, union sigval);
int    sigrelse(int);
void (*sigset(int, void (*)(int)))(int);
int    sigsuspend(const sigset_t *);
int    sigtimedwait(const sigset_t *, siginfo_t *, const struct timespec *);
int    sigwait(const sigset_t *, int *);
int    sigwaitinfo(const sigset_t *, siginfo_t *);

#endif
