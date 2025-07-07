#ifndef ARPA_INET_H
#define ARPA_INET_H

#include <features.h>
#include <stdint.h>

uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);

#include <netinet/in.h>

in_addr_t inet_addr(const char *cp);
char *inet_ntoa(struct in_addr in);
const char  *inet_ntop(int, const void *restrict, char *restrict, socklen_t);
int          inet_pton(int, const char *restrict, void *restrict);
#endif
