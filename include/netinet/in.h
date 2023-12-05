#ifndef _NETINET_IN_H
#define _NETINET_IN_H

#include <features.h>
#include <inttypes.h>

typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;

struct in_addr {
	in_addr_t s_addr;
};

#include <sys/socket.h>

struct sockaddr_in {
	sa_family_t sin_family;
	in_port_t   sin_port;
	struct in_addr sin_addr;
    /* attempt to make the same size as sockaddr else things break */
    unsigned char sin_zero[8];
};

#define IPPROTO_IP	 0
#define IPPROTO_ICMP 1
#define IPPROTO_TCP  6
#define IPPROTO_UDP  17

#define INADDR_ANY       ((in_addr_t)0x00000000)
#define INADDR_BROADCAST ((in_addr_t)0xffffffff)
#define INADDR_LOOPBACK  ((in_addr_t)0x7f000001)
#define INET_ADDRSTRLEN  16

#include <arpa/inet.h>

#endif
