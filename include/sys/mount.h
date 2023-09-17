#ifndef _SYS_MOUNT
#define _SYS_MOUNT

#include <features.h>

extern int mount(const char *source, const char *target, const char *fstype, unsigned long flags, const void *data);

#define MS_NOSUID   (1<<1)
#define MS_NODEV    (1<<2)
#define MS_NOEXEC	(1<<3)
#define MS_RELATIME (1<<21)

#endif
