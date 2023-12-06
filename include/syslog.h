#ifndef _SYSLOG_H
#define _SYSLOG_H

#include <features.h>

#define LOG_PID    (1 << 0)
#define LOG_CONS   (1 << 1)
#define LOG_NDELAY (1 << 2)
#define LOG_ODELAY (1 << 3)
#define LOG_NOWAIT (1 << 4)

#define   LOG_KERN      (0<<3)
#define   LOG_USER      (1<<3)
#define   LOG_MAIL      (2<<3)
#define   LOG_NEWS      (3<<3)
#define   LOG_UUCP      (4<<3)
#define   LOG_DAEMON    (5<<3)
#define   LOG_AUTH      (6<<3)
#define   LOG_CRON      (7<<3)
#define   LOG_LPR       (8<<3)
#define   LOG_LOCAL0    (9<<3)
#define   LOG_LOCAL1   (10<<3)
#define   LOG_LOCAL2   (11<<3)
#define   LOG_LOCAL3   (12<<3)
#define   LOG_LOCAL4   (13<<3)
#define   LOG_LOCAL5   (14<<3)
#define   LOG_LOCAL6   (15<<3)
#define   LOG_LOCAL7   (16<<3)
/* Linux shit */
#define   LOG_AUTHPRIV (17<<3)
#define   LOG_SYSLOG   (18<<3)

#define   LOG_EMERG     0
#define   LOG_ALERT     1
#define   LOG_CRIT      2
#define   LOG_ERR       3
#define   LOG_WARNING   4
#define   LOG_NOTICE    5
#define   LOG_INFO      6
#define   LOG_DEBUG     7

#define LOG_MASK(x) ((x)&LOG_DEBUG)

extern void  closelog(void);
extern void  openlog(const char *, int, int);
extern int   setlogmask(int);
extern void  syslog(int, const char *, ...);

#include <stdarg.h>

extern void  vsyslog(int, const char *, va_list);

#endif
