#ifndef _ERRNO_H
#define _ERRNO_H

#define errno (*__errno_location())

extern int *__errno_location(void);

#define EPERM      1
#define ENOENT     2
#define ESRCH      3
#define EINTR      4
#define EIO        5
#define ENOEXEC    8
#define EBADF      9
#define ECHILD    10
#define EAGAIN    11
#define EWOULDBLOCK EAGAIN
#define ENOMEM    12
#define EACCES    13
#define EFAULT    14
#define EBUSY     16
#define EEXIST    17
#define ENOTDIR   20
#define EISDIR    21
#define EINVAL    22
#define ENOTTY    25
#define ESPIPE    29
#define ERANGE    34
#define EDEADLK   35
#define ENAMETOOLONG 36
#define ENOSYS    38
#define ELOOP     40
#define EOVERFLOW 75
#define EAFNOSUPPORT 97

#endif
