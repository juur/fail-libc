#ifndef SYS_STAT_H
#define SYS_STAT_H

#include <features.h>
#include <sys/types.h>
#include <time.h>


#define S_IFMT		0170000
#define S_IFDIR		0040000
#define S_IFCHR		0020000
#define S_IFBLK		0060000
#define S_IFREG		0100000
#define S_IFIFO		0010000
#define S_IFLNK		0120000
#define S_IFSOCK	0140000

#define S_ISUID       04000
#define S_ISGID       02000
#define S_ISVTX       01000
#define S_IRWXU        0700
#define S_IRUSR		   0400
#define S_IWUSR		   0200
#define S_IXUSR		   0100
#define S_IRWXG        0070
#define S_IRGRP		   0040
#define S_IWGRP		   0020
#define S_IXGRP		   0010
#define S_IRWXO        0007
#define S_IROTH		   0004
#define S_IWOTH		   0002
#define S_IXOTH		   0001

#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#define S_ISLNK(mode)  (((mode) & S_IFMT) == S_IFLNK)
#define S_ISBLK(mode)  (((mode) & S_IFMT) == S_IFBLK)
#define S_ISCHR(mode)  (((mode) & S_IFMT) == S_IFCHR)
#define S_ISFIFO(mode) (((mode) & S_IFMT) == S_IFIFO)
#define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)
#define S_ISSOCK(mode) (((mode) & S_IFMT) == S_IFSOCK)

struct stat {
	dev_t st_dev;
	ino_t st_ino;
	nlink_t st_nlink;
	mode_t st_mode;
	uid_t st_uid;
	gid_t st_gid;
	int __pad0; /* Linux */
	dev_t st_rdev;
	off_t st_size;
	blksize_t st_blksize;
	blkcnt_t st_blocks;

	struct timespec st_atim, st_mtim, st_cim;

	long __res[3];
};

#define st_atime st_atim.tv_sec
#define st_mtime st_mtim.tv_sec
#define st_ctime st_ctim.tv_sec

int    chmod(const char *, mode_t);
int    fchmod(int, mode_t);
int    fchmodat(int, const char *, mode_t, int);
int    fstat(int, struct stat *);
int    fstatat(int, const char *restrict, struct stat *restrict, int);
int    futimens(int, const struct timespec [2]);
int    lstat(const char *restrict, struct stat *restrict);
int    mkdir(const char *, mode_t);
int    mkdirat(int, const char *, mode_t);
int    mkfifo(const char *, mode_t);
int    mkfifoat(int, const char *, mode_t);
int    mknod(const char *, mode_t, dev_t);
int    mknodat(int, const char *, mode_t, dev_t);
int    stat(const char *restrict, struct stat *restrict);
mode_t umask(mode_t);
int    utimensat(int, const char *, const struct timespec [2], int);

#endif
