#ifndef _ERRNO_H
#define _ERRNO_H

#define errno (*__errno_location())

extern int *__errno_location(void);

#define EPERM      1
#define ENOENT     2
#define ESRCH      3
#define EINTR      4
#define EBADF      9
#define ENOMEM    12
#define EACCES    13
#define EFAULT    14
#define EBUSY     16
#define EEXIST    17
#define EINVAL    22
#define ENOTTY    25
#define EDEADLK   35
#define ENOSYS    38
#define EOVERFLOW 75

#endif
