#ifndef FCNTL_H
#define FCNTL_H

/* sync with fail-os/src/file.h */
#define O_RDONLY        01
#define O_WRONLY        02
#define O_RDWR          (O_RDONLY|O_WRONLY)
#define O_EXEC          04
#define O_SEARCH        O_EXEC

#define O_APPEND      0100
#define O_CLOEXEC     0200
#define O_CREAT       0400
#define O_DIRECTORY  01000
#define O_DSYNC      02000
#define O_EXCL       04000
#define O_NOCTTY    010000
#define O_NOFOLLOW  020000
#define O_NONBLOCK  040000
#define O_RSYNC    0100000
#define O_TRUNC    0200000
#define O_TTY_INIT 0400000

#include <sys/types.h>

int  creat(const char *, mode_t);
int  fcntl(int, int, ...);
int  open(const char *, int, ...);
int  openat(int, const char *, int, ...);

#endif
