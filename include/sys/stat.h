#ifndef SYS_STAT_H
#define SYS_STAT_H

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

#define S_IRUSR		   0400
#define S_IWUSR		   0200
#define S_IXUSR		   0100
#define S_IRGRP		   0040
#define S_IWGRP		   0020
#define S_IXGRP		   0010
#define S_IROTH		   0004
#define S_IWOTH		   0002
#define S_IXOTH		   0001

#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)

struct stat {
	mode_t st_mode;
	off_t st_size;

	struct timespec st_atim, st_mtim, st_cim;
};

#define st_atime st_atim.tv_sec
#define st_mtime st_mtim.tv_sec
#define st_ctime st_ctim.tv_sec

#endif
