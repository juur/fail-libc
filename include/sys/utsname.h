#ifndef _SYS_UTSNAME_H
#define _SYS_UTSNAME_H

#include <features.h>

struct utsname {
	char sysname[64];
	char nodename[64];
	char release[64];
	char version[64];
	char machine[64];
	char domainname[256]; /* Linux */
};

int uname(struct utsname *);

#endif
