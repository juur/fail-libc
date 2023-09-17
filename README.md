fail-libc is an AMD64 specific libc implementation _broadly_ aligned 
to POSIX.1-2017.

It assumes, vaguely, the Linux AMD64 ABI (for syscalls), and thus uses the 
System V AMD64 ABI.

There is a partial implemention of regex(7).
Very few math.h functions are implemented.

The malloc implemention is extremely basic.
