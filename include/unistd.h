#ifndef UNISTD_H
#define UNISTD_H

#define _POSIX_VERSION	200809L
#define _POSIX2_VERSION	200809L
#define _XOPEN_VERSION	700

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

#define _SC_NGROUPS_MAX 1

long syscall(long number, ...);

#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/stat.h>

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
extern int          getopt(int, char * const [], const char *);
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
