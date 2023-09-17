#ifndef SIGNAL_H
#define SIGNAL_H

#include <features.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>

#define SIGHUP   1
#define SIGINT   2
#define SIGQUIT  3
#define SIGILL   4
#define SIGTRAP  5
#define SIGABRT  6
#define SIGBUS   7
#define SIGFPE   8
#define SIGKILL  9
#define SIGUSR1 10
#define SIGSEGV 11
#define SIGUSR2 12
#define SIGPIPE 13
#define SIGALRM 14
#define SIGTERM 15
#define SIGSTKFLT 16
#define SIGCHLD 17
#define SIGCONT 18
#define SIGSTOP 19
#define SIGTSTP 20
#define SIGTTIN 21
#define SIGTTOU 22
#define SIGURG  23
#define SIGXCPU 24
#define SIGXFSZ 25
#define SIGVTALRM 26
#define SIGPROF 27
#define SIGWINCH 28
#define SIGPOLL 29
#define SIGPWR 30
#define SIGSYS 31
#define NSIG 64

typedef unsigned long sigset_t;
typedef int sig_atomic_t;
typedef void (*__sighandler_t)(int);

#define SIG_ERR ((__sighandler_t)-1)
#define SIG_DFL ((__sighandler_t)0)
#define SIG_IGN ((__sighandler_t)1)

#define SIG_BLOCK   0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2

#define SA_NOCLDSTOP  0x00000001
#define SA_NOCLDWAIT  0x00000002
#define SA_SIGINFO    0x00000004
#define SA_ONSTACK    0x08000000
#define SA_RESTART    0x10000000
#define SA_NODEFER    0x40000000
#define SA_RESETHAND  0x80000000

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
