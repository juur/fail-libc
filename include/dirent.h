#ifndef _DIRENT_H
#define _DIRENT_H

#include <sys/types.h>
#include <limits.h>

struct dirent {
	ino_t d_ino;
	char d_name[NAME_MAX];
};


typedef struct {
	int fd;
	int error;
	int idx, max;
	struct dirent buf[200];
} DIR;

extern int alphasort(const struct dirent **, const struct dirent **);
extern int closedir(DIR *);
extern int dirfd(DIR *);
extern DIR *fdopendir(int);
extern DIR *opendir(const char *);
extern struct dirent *readdir(DIR *);
extern int readdir_r(DIR *restrict, struct dirent *restrict, struct dirent **restrict);
extern void rewinddir(DIR *);
extern int scandir(const char *, struct dirent ***, int (*)(const struct dirent *), int (*)(const struct dirent **, const struct dirent **));
#endif
