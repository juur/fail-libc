#ifndef _MNTENT_H
#define _MNTENT_H

#include <features.h>
#include <sys/mount.h>

#define MS_RDONLY (1<<0)
#define MS_NOSUID (1<<1)
#define MS_NODEV  (1<<2)
#define MS_NOEXEC (1<<3)
#define MS_SYNCHRONOUS (1<<4)
#define MS_NOATIME (1<<5)
#define MS_NODIRATIME (1<<6)
#define MS_MANDLOCK (1<<7)
#define MS_SILENT (1<<8)
#define MS_REMOUNT (1<<9)

struct mntent {
	char *mnt_fsname;
	char *mnt_dir;
	char *mnt_type;
	char *mnt_opts;
	int   mnt_freq;
	int   mnt_passno;
};

#include <stdio.h>

FILE *setmntent(const char *, const char *);
struct mntent *getmntent(FILE *);
int endmntent(FILE *);

#endif
