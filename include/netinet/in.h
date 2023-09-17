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
};

#define IPPROTO_IP	 0
#define IPPROTO_ICMP 1
#define IPPROTO_TCP  6
#define IPPROTO_UDP  17

#define INADDR_ANY       0x00000000
#define INADDR_BROADCAST 0xffffffff
#define INET_ADDRSTRLEN  16

#include <arpa/inet.h>

#endif
