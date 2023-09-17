#ifndef ARPA_INET_H
#define ARPA_INET_H

#include <features.h>
#include <stdint.h>

uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);
#endif
