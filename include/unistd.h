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

long syscall(long number, ...);

#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>

int execve(const char *, char *const [], char *const []);
pid_t fork(void);
pid_t getpid(void);
pid_t getppid(void);

int access(const char *pathname, int mode);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
int close(int fd);

#include <sys/stat.h>

int stat(const char *pathname, struct stat *statbuf);
int fstat(int fd, struct stat *statbuf);
int lstat(const char *pathname, struct stat *statbuf);


#endif
// vim: set ft=c:
