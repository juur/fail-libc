#ifndef _DIRENT_H
#define _DIRENT_H

#include <features.h>
#include <sys/types.h>
#include <limits.h>

struct dirent {
	ino_t d_ino;
	off_t d_off; /* Linux */
	unsigned short int d_reclen; /* Linux */
	unsigned char d_type; /* Linux */
	char d_name[]; /* Linux */
};

#define DT_UNKNOWN 0
#define DT_FIFO    1
#define DT_CHR     2
#define DT_DIR     4
#define DT_BLK     6
#define DT_REG     8
#define DT_LNK     10
#define DT_SOCK    12
#define DT_WHT     14


typedef struct {
	int fd;
	int error;
	struct dirent *idx;
	struct dirent *end;
	char buf[8192];
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
