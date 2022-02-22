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

int alphasort(const struct dirent **, const struct dirent **);
int closedir(DIR *);
int dirfd(DIR *);
DIR *fdopendir(int);
DIR *opendir(const char *);
struct dirent *readdir(DIR *);
int readdir_r(DIR *restrict, struct dirent *restrict, struct dirent **restrict);
void rewinddir(DIR *);
int scandir(const char *, struct dirent ***, int (*)(const struct dirent *), int (*)(const struct dirent **, const struct dirent **));

#endif
