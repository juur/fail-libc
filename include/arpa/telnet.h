#ifndef ARPA_TELNET_H
#define ARPA_TELNET_H

#define IAC     255
#define DONT    254
#define DO      253
#define WONT    252
#define WILL    251
#define SB      250
#define DM      242
#define NOP     241
#define SE      240

#define TELOPT_ECHO     1
#define TELOPT_SGA      3
#define TELOPT_STATUS   5
#define TELOPT_LOGOUT   18
#define TELOPT_DET      20
#define TELOPT_SNDLOC   23
#define TELOPT_TTYPE    24
#define TELOPT_OUTMRK   27
#define TELOPT_NAWS     31
#define TELOPT_TSPEED   32
#define TELOPT_LFLOW    33
#define TELOPT_XDISPLOC 35
#define TELOPT_NEW_ENVIRON  39

#endif
