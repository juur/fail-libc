#ifndef UNISTD_H
#define UNISTD_H

#include <features.h>

#define _POSIX_VERSION	202405L
#define _POSIX2_VERSION	202505L
#define _XOPEN_UNIX 1
#define _XOPEN_VERSION	800

#define STDIN_FILENO	0
#define STDOUT_FILENO	1
#define STDERR_FILENO	2

#define R_OK 4
#define W_OK 2
#define X_OK 1
#define F_OK 0

#define F_LOCK 0
#define F_TEST 1
#define F_TLOCK 2
#define F_ULOCK 3

#define _PC_2_SYMLINKS 1
#define _PC_ALLOC_SIZE_MIN 2
#define _PC_ASYNC_IO 3
#define _PC_CHOWN_RESTRICTED 4
#define _PC_FALLOC 5
#define _PC_FILESIZEBITS 6
#define _PC_LINK_MAX 7
#define _PC_MAX_CANON 8
#define _PC_MAX_INPUT 9
#define _PC_NAME_MAX 10
#define _PC_NO_TRUNC 11
#define _PC_PATH_MAX 12
#define _PC_PIPE_BUF 13
#define _PC_PRIO_IO 14
#define _PC_REC_INCR_XFER_SIZE 15
#define _PC_REC_MAX_XFER_SIZE 16
#define _PC_REC_MIN_XFER_SIZE 17
#define _PC_REC_XFER_ALIGN 18
#define _PC_SYMLINK_MAX 19
#define _PC_SYNC_IO 20
#define _PC_TEXTDOMAIN_MAX 21
#define _PC_TIMESTAMP_RESOLUTION 22
#define _PC_VDISABLE 23

#define _SC_AIO_LISTIO_MAX 1
#define _SC_AIO_MAX 2
#define _SC_AIO_PRIO_DELTA_MAX 3
#define _SC_ARG_MAX 4
#define _SC_ATEXIT_MAX 5
#define _SC_BC_BASE_MAX 6
#define _SC_BC_DIM_MAX 7
#define _SC_BC_SCALE_MAX 8
#define _SC_BC_STRING_MAX 9
#define _SC_CHILD_MAX 10
#define _SC_CLK_TCK 11
#define _SC_COLL_WEIGHTS_MAX 12
#define _SC_DELAYTIMER_MAX 13
#define _SC_EXPR_NEST_MAX 14
#define _SC_HOST_NAME_MAX 15
#define _SC_IOV_MAX 16
#define _SC_LINE_MAX 17
#define _SC_LOGIN_NAME_MAX 18
#define _SC_NGROUPS_MAX 19
#define _SC_GETGR_R_SIZE_MAX 20
#define _SC_GETPW_R_SIZE_MAX 21
#define _SC_MQ_OPEN_MAX 22
#define _SC_MQ_PRIO_MAX 23
#define _SC_NPROCESSORS_CONF 24
#define _SC_NPROCESSORS_ONLN 25
#define _SC_NSIG 26
#define _SC_OPEN_MAX 27
#define _SC_PAGE_SIZE 28
#define _SC_PAGESIZE 29
#define _SC_THREAD_DESTRUCTOR_ITERATIONS 30
#define _SC_THREAD_KEYS_MAX 31
#define _SC_THREAD_STACK_MIN 32
#define _SC_THREAD_THREADS_MAX 33
#define _SC_RE_DUP_MAX 34
#define _SC_RTSIG_MAX 35
#define _SC_SEM_NSEMS_MAX 36
#define _SC_SEM_VALUE_MAX 37
#define _SC_SIGQUEUE_MAX 38
#define _SC_STREAM_MAX 39
#define _SC_SYMLOOP_MAX 40
#define _SC_TIMER_MAX 41
#define _SC_TTY_NAME_MAX 42
#define _SC_TZNAME_MAX 43
#define _SC_ADVISORY_INFO 44
#define _SC_BARRIERS 45
#define _SC_ASYNCHRONOUS_IO 46
#define _SC_CLOCK_SELECTION 47
#define _SC_CPUTIME 48
#define _SC_DEVICE_CONTROL 49
#define _SC_FSYNC 50
#define _SC_IPV6 51
#define _SC_JOB_CONTROL 52
#define _SC_MAPPED_FILES 53
#define _SC_MEMLOCK 54
#define _SC_MEMLOCK_RANGE 55
#define _SC_MEMORY_PROTECTION 56
#define _SC_MESSAGE_PASSING 57
#define _SC_MONOTONIC_CLOCK 58
#define _SC_PRIORITIZED_IO 59
#define _SC_PRIORITY_SCHEDULING 60
#define _SC_RAW_SOCKETS 61
#define _SC_READER_WRITER_LOCKS 62
#define _SC_REALTIME_SIGNALS 63
#define _SC_REGEXP 64
#define _SC_SAVED_IDS 65
#define _SC_SEMAPHORES 66
#define _SC_SHARED_MEMORY_OBJECTS 67
#define _SC_SHELL 68
#define _SC_SPAWN 69
#define _SC_SPIN_LOCKS 70
#define _SC_SPORADIC_SERVER 71
#define _SC_SS_REPL_MAX 72
#define _SC_SYNCHRONIZED_IO 73
#define _SC_THREAD_ATTR_STACKADDR 74
#define _SC_THREAD_ATTR_STACKSIZE 75
#define _SC_THREAD_CPUTIME 76
#define _SC_THREAD_PRIO_INHERIT 77
#define _SC_THREAD_PRIO_PROTECT 78
#define _SC_THREAD_PRIORITY_SCHEDULING 79
#define _SC_THREAD_PROCESS_SHARED 80
#define _SC_THREAD_ROBUST_PRIO_INHERIT 81
#define _SC_THREAD_ROBUST_PRIO_PROTECT 82
#define _SC_THREAD_SAFE_FUNCTIONS 83
#define _SC_THREAD_SPORADIC_SERVER 84
#define _SC_THREADS 85
#define _SC_TIMEOUTS 86
#define _SC_TIMERS 87
#define _SC_TYPED_MEMORY_OBJECTS 88
#define _SC_VERSION 89
#define _SC_V8_ILP32_OFF32 90
#define _SC_V8_ILP32_OFFBIG 91
#define _SC_V8_LP64_OFF64 92
#define _SC_V8_LPBIG_OFFBIG 93
#define _SC_V7_ILP32_OFF32 94
#define _SC_V7_ILP32_OFFBIG 95
#define _SC_V7_LP64_OFF64 96
#define _SC_V7_LPBIG_OFFBIG 97
#define _SC_2_C_BIND 98
#define _SC_2_C_DEV 99
#define _SC_2_CHAR_TERM 100
#define _SC_2_FORT_RUN 101
#define _SC_2_LOCALEDEF 102
#define _SC_2_SW_DEV 103
#define _SC_2_UPE 104
#define _SC_2_VERSION 105
#define _SC_XOPEN_CRYPT 106
#define _SC_XOPEN_ENH_I18N 107
#define _SC_XOPEN_REALTIME 108
#define _SC_XOPEN_REALTIME_THREADS 109
#define _SC_XOPEN_SHM 110
#define _SC_XOPEN_UNIX 111
#define _SC_XOPEN_UUCP 112
#define _SC_XOPEN_VERSION 113

extern long syscall(unsigned long number, ...);

#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>
#include <fcntl.h>

extern char  *optarg;
extern int    opterr, optind, optopt;

extern char        *crypt(const char *, const char *);
extern char        *getcwd(char *, size_t);
extern char        *getlogin(void);
extern char        *ttyname(int);
extern gid_t        getegid(void);
extern gid_t        getgid(void);
extern int          access(const char *, int);
extern int          chdir(const char *);
extern int          chown(const char *, uid_t, gid_t);
extern int          close(int);
extern int          dup(int);
extern int          dup2(int, int);
extern int          execl(const char *, const char *, ...);
extern int          execle(const char *, const char *, ...);
extern int          execlp(const char *, const char *, ...);
extern int          execv(const char *, char *const []);
extern int          execve(const char *, char *const [], char *const []);
extern int          execvp(const char *, char *const []);
extern int          faccessat(int, const char *, int, int);
extern int          fchdir(int);
extern int          fchown(int, uid_t, gid_t);
extern int          fchownat(int, const char *, uid_t, gid_t, int);
extern int          fdatasync(int);
extern int          fexecve(int, char *const [], char *const []);
extern int          fsync(int);
extern int          ftruncate(int, off_t);
extern int          getgroups(int, gid_t []);
extern int          gethostname(char *, size_t);
extern int          getlogin_r(char *, size_t);
extern int          getopt(int, char *const [], const char *);
extern int          isatty(int);
extern int          lchown(const char *, uid_t, gid_t);
extern int          link(const char *, const char *);
extern int          linkat(int, const char *, int, const char *, int);
extern int          lockf(int, int, off_t);
extern int          nice(int);
extern int          pause(void);
extern int          pipe(int [2]);
extern int          rmdir(const char *);
extern int          setegid(gid_t);
extern int          seteuid(uid_t);
extern int          setgid(gid_t);
extern int          setpgid(pid_t, pid_t);
extern int          setregid(gid_t, gid_t);
extern int          setreuid(uid_t, uid_t);
extern int          setuid(uid_t);
extern int          symlink(const char *, const char *);
extern int          symlinkat(const char *, int, const char *);
extern int          tcsetpgrp(int, pid_t);
extern int          truncate(const char *, off_t);
extern int          ttyname_r(int, char *, size_t);
extern int          unlink(const char *);
extern int          unlinkat(int, const char *, int);
extern long         fpathconf(int, int);
extern long         gethostid(void);
extern long         pathconf(const char *, int);
extern long         sysconf(int);
extern off_t        lseek(int, off_t, int);
extern pid_t        fork(void);
extern pid_t        getpgid(pid_t);
extern pid_t        getpgrp(void);
extern pid_t        getpid(void);
extern pid_t        getppid(void);
extern pid_t        getsid(pid_t);
extern pid_t        setpgrp(void);
extern pid_t        setsid(void);
extern pid_t        tcgetpgrp(int);
extern size_t       confstr(int, char *, size_t);
extern ssize_t      pread(int, void *, size_t, off_t);
extern ssize_t      pwrite(int, const void *, size_t, off_t);
extern ssize_t      read(int, void *, size_t);
extern ssize_t      readlink(const char *restrict, char *restrict, size_t);
extern ssize_t      readlinkat(int, const char *restrict, char *restrict, size_t);
extern ssize_t      write(int, const void *, size_t);
extern uid_t        geteuid(void);
extern uid_t        getuid(void);
extern unsigned     alarm(unsigned);
extern unsigned     sleep(unsigned);
extern void         _exit(int);
extern void         encrypt(char [64], int);
extern void         swab(const void *restrict, void *restrict, ssize_t);
extern void         sync(void);

#endif
// vim: set ft=c:

