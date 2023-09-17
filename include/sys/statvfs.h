#ifndef _SYS_STATVS_H
#define _SYS_STATVS_H

#include <features.h>
#include <sys/types.h>

#define ST_RDONLY (1<<0)
#define ST_NOSUID (1<<1)

struct statvfs {
	unsigned long f_bsize;
	unsigned long f_frsize;
	fsblkcnt_t  f_blocks;
	fsblkcnt_t  f_bfree;
	fsblkcnt_t  f_bavail;
	fsfilcnt_t  f_files;
	fsfilcnt_t  f_ffree;
	fsfilcnt_t f_favail;
	unsigned long f_fsid;
	unsigned long f_flag;
	unsigned long f_namemax;
	/* Linux compat */
	int __f_spare[6];
};

int fstatvfs(int, struct statvfs *);
int statvfs(const char *restrict, struct statvfs *restrict);

#endif
