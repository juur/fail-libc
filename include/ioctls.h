#ifndef _IOCTLS_H
#define _IOCTLS_H

#include <features.h>

#define TCGETS  0x5401
#define TCSETS  0x5402
#define TCSETSW 0x5403
#define TCSETSF 0x5404

#define TIOCGWINSZ 0x5413
#define TIOCSWINSZ 0x5414

#define TIOCNOTTY  0x5422

#endif
