#ifndef _NETDB_H
#define _NETDB_H

#include <netinet/in.h>
#include <inttypes.h>
#include <errno.h>

struct hostent {
	char  *h_name;
	char **h_aliases;
	int    h_addrtype;
	int    h_length;
	char **h_addr_list;
};

struct netent {
	char    *n_name;
	char   **n_aliases;
	int      n_addrtype;
	uint32_t n_net;
};

struct protoent {
	char  *p_name;
	char **p_aliases;
	int    p_proto;
};

struct servent {
	char  *s_name;
	char **s_aliases;
	int    s_port;
	char  *s_proto;
};

struct addrinfo {
	int ai_flags;
	int ai_family;
	int ai_socktype;
	int ai_protocol;
	socklen_t ai_addrlen;
	struct sockaddr *ai_addr;
	char *ai_canonname;
	struct addrinfo *ai_next;
};

#define h_errno (*__errno_location())

extern void              endhostent(void);
extern void              endnetent(void);
extern void              endprotoent(void);
extern void              endservent(void);
extern void              freeaddrinfo(struct addrinfo *);
extern const char       *gai_strerror(int);
extern int               getaddrinfo(const char *restrict, const char *restrict,
		const struct addrinfo *restrict,
		struct addrinfo **restrict);
extern struct hostent   *gethostent(void);
extern int               getnameinfo(const struct sockaddr *restrict, socklen_t,
		char *restrict, socklen_t, char *restrict,
		socklen_t, int);
extern struct netent    *getnetbyaddr(uint32_t, int);
extern struct netent    *getnetbyname(const char *);
extern struct netent    *getnetent(void);
extern struct protoent  *getprotobyname(const char *);
extern struct protoent  *getprotobynumber(int);
extern struct protoent  *getprotoent(void);
extern struct servent   *getservbyname(const char *, const char *);
extern struct servent   *getservbyport(int, const char *);
extern struct servent   *getservent(void);
extern void              sethostent(int);
extern void              setnetent(int);
extern void              setprotoent(int);
extern void              setservent(int);

#endif