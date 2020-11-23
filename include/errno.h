#ifndef ERRNO_H
#define ERRNO_H

#define errno (*__errno_location())

#define ESRCH	3
#define ENOMEM	12
#define EACCES	13
#define EINVAL	22
#define EDEADLK	35

#endif
