#ifndef SYS_SOCKET_H
#define SYS_SOCKET_H

typedef unsigned int   socklen_t;
typedef unsigned short sa_family_t;

struct sockaddr {
	sa_family_t sa_family;
	char sa_data[];
};

#include <sys/uio.h>

struct msghdr {
	void *msg_name;
	socklen_t msg_namelen;
	struct iovec *msg_iov;
	void *msg_control;
	socklen_t msg_controllen;
	int msg_flags;
};

struct cmsghdr {
	socklen_t cmsg_len;
	int csmg_level;
	int csmg_type;
};

struct linger {
	int l_onoff;
	int l_linger;
};

#define SOCK_DGRAM     1
#define SOCK_RAW       2
#define SOCK_SEQPACKET 3
#define SOCK_STREAM    4

#define SOL_SOCKET     1

#define SO_ACCEPTCONN	1
#define SO_BROADCAST	2
#define SO_DEBUG		3
#define SO_DONTROUTE	4
#define SO_ERROR		5
#define SO_KEEPALIVE	6
#define SO_LINGER		7
#define SO_OOBINLINE	8
#define SO_RCVBUF		9
#define SO_RCVLOWAT		10
#define SO_RCVTIMEO		11
#define SO_SNDBUF		12
#define SO_SNDLOWAT		13
#define SO_SNDTIMEO		14
#define SO_TYPE			15

#define SOMAXCONN		32

#define AF_INET		1
#define AF_INET6	2
#define AF_UNIX		3
#define AF_UNSPEC	4

#include <sys/types.h>

int     accept(int, struct sockaddr *, socklen_t *);
int     bind(int, const struct sockaddr *, socklen_t);
int     connect(int, const struct sockaddr *, socklen_t);
int     getpeername(int, struct sockaddr *, socklen_t *);
int     getsockname(int, struct sockaddr *, socklen_t *);
int     getsockopt(int, int, int, void *, socklen_t *);
int     listen(int, int);
ssize_t recv(int, void *, size_t, int);
ssize_t recvfrom(int, void *, size_t, int, struct sockaddr *, socklen_t *);
ssize_t recvmsg(int, struct msghdr *, int);
ssize_t send(int, const void *, size_t, int);
ssize_t sendmsg(int, const struct msghdr *, int);
ssize_t sendto(int, const void *, size_t, int, const struct sockaddr *,
        socklen_t);
int     setsockopt(int, int, int, const void *, socklen_t);
int     shutdown(int, int);
int     sockatmark(int);
int     socket(int, int, int);
int     socketpair(int, int, int, int [2]);
#endif
