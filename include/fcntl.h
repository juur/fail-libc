#ifndef FCNTL_H
#define FCNTL_H

#define O_RDONLY       00
#define O_WRONLY       01
#define O_RDWR         02
#define O_CREAT      0100
#define O_TRUNC     01000
#define O_APPEND    02000

#include <sys/types.h>

int  creat(const char *, mode_t);
int  fcntl(int, int, ...);
int  open(const char *, int, ...);
int  openat(int, const char *, int, ...);

#endif
