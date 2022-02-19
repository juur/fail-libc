#ifndef ERRNO_H
#define ERRNO_H

#define errno (*__errno_location())

extern int *__errno_location(void);

#define ESRCH	3
#define EINTR	4
#define ENOMEM	12
#define EACCES	13
#define EINVAL	22
#define EDEADLK	35

#endif
