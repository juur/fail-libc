#ifndef _UTMPX_H
#define _UTMPX_H

#include <features.h>
#include <sys/types.h>
#include <sys/time.h>
#include <limits.h>

struct utmpx {
	char ut_user[16];  /* LOGIN_NAME_MAX */
	char ut_id[4];
	char ut_line[32];  /* ?NAME_MAX? */
	char ut_host[255]; /* HOST_NAME_MAX */

	pid_t ut_pid;
	short ut_type;

	struct timeval ut_tv;
};

#define EMPTY     0x00
#define BOOT_TIME 0x01
#define OLD_TIME  0x02
#define NEW_TIME  0x03
#define USER_PROCESS 0x04
#define INIT_PROCESS 0x05
#define LOGIN_PROCESS 0x06
#define DEAD_PROCESS 0x07

void          endutxent(void);
struct utmpx *getutxent(void);
struct utmpx *getutxid(const struct utmpx *);
struct utmpx *getutxline(const struct utmpx *);
struct utmpx *pututxline(const struct utmpx *);
void          setutxent(void);

#endif
// vim: set ft=c:
