#ifndef _TERMIOS_H
#define _TERMIOS_H

#include <features.h>

#define VINTR    0
#define VQUIT    1
#define VERASE   2
#define VKILL    3
#define VEOF     4
#define VTIME    5
#define VMIN     6
#define VSTART   8
#define VSTOP    9
#define VSUSP    10
#define VEOL     11

/* mirror Linux */
#define NCCS    32

/* c_iflag */
#define IGNBRK  000001
#define BRKINT  000002
#define IGNPAR  000004
#define PARMRK  000010
#define INPCK   000020
#define ISTRIP  000040
#define INLCR   000100
#define IGNCR   000200
#define ICRNL   000400
#define IXON    002000
#define IXANY   004000
#define IXOFF   010000

/* c_oflag */
#define OPOST   0000001
#define ONLCR   0000004
#define OCRNL   0000010
#define ONOCR   0000020
#define ONLRET  0000040
#define OFILL   0000100
#define OFDEL   0000200
#define NL0     0000000
#define NL1     0000400
#define CR0     0000000
#define CR1     0001000
#define CR2     0002000
#define CR3     0003000
#define TAB0    0000000
#define TAB1    0004000
#define TAB2    0010000
#define TAB3    0014000
#define BS0     0000000
#define BS1     0020000
#define VT0     0000000
#define VT1     0040000
#define FF0     0000000
#define FF1     0100000

#define B0      0
#define B50     50
#define B75     75
#define B110    110
#define B134    134
#define B150    150
#define B200    200
#define B300    300
#define B600    600
#define B1200   1200
#define B1800   1800
#define B2400   2400
#define B4800   4800
#define B9600   9600
#define B19200  19200
#define B38400  38400

/* c_cflag */
#define CS5     0001
#define CS6     0002
#define CS7     0003
#define CS8     0004
#define CSTOPB  0010
#define CREAD   0020
#define PARENB  0040
#define PARODD  0100
#define HUPCL   0200
#define CLOCAL  0400

/* c_lflag */
#define ISIG    0000001
#define ICANON  0000002
#define ECHO    0000010
#define ECHOE   0000020
#define ECHOK   0000040
#define ECHONL  0000100
#define NOFLSH  0000200
#define TOSTOP  0000400
#define IEXTEN  0100000

#define TCSANOW   1
#define TCSADRAIN 2
#define TCSAFLUSH 3

#define TCIFLUSH  1
#define TCOFLUSH  2
#define TCIOFLUSH 3

#define TCIOFF 1
#define TCION  2
#define TCOOFF 3
#define TCOON  4

#define CSIZE   CS8
#define CRDLY   CR0
#define TABDLY  TAB3
#define BSDLY   BS0
#define VTDLY   VT0
#define FFDLY   FF0
#define NLDLY   NL0

/* TODO NL0/CR0/TAB0/BS0/VT0/FF0 */

typedef int cc_t;
typedef int speed_t;
typedef unsigned int tcflag_t;

struct termios {
	tcflag_t c_iflag;
	tcflag_t c_oflag;
	tcflag_t c_cflag;
	tcflag_t c_lflag;

	cc_t c_line;
	cc_t c_cc[NCCS];

	speed_t c_ispeed;
	speed_t c_ospeed;
};

struct winsize {
	unsigned short ws_row;
	unsigned short ws_col;
	unsigned short ws_xpixel, ws_ypixel;
};

#include <sys/types.h>
#include <ioctls.h>

speed_t cfgetispeed(const struct termios *);
speed_t cfgetospeed(const struct termios *);
int     cfsetispeed(struct termios *, speed_t);
int     cfsetospeed(struct termios *, speed_t);
int     tcdrain(int);
int     tcflow(int, int);
int     tcflush(int, int);
int     tcgetattr(int, struct termios *);
pid_t   tcgetsid(int);
int     tcsendbreak(int, int);
int     tcsetattr(int, int, const struct termios *);

#endif
