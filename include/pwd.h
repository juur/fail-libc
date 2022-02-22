#ifndef _PWD_H
#define _PWD_H

#include <sys/types.h>

struct passwd {
	char *pw_name;
	char *pw_dir;
	char *pw_shell;
	uid_t pw_uid;
	gid_t pw_gid;
};

void           endpwent(void);
struct passwd *getpwent(void);
struct passwd *getpwnam(const char *);
int            getpwnam_r(const char *, struct passwd *, char *, size_t, struct passwd **);
struct passwd *getpwuid(uid_t);
int            getpwuid_r(uid_t, struct passwd *, char *, size_t, struct passwd **);
void           setpwent(void);

#endif
