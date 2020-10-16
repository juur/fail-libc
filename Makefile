CC := gcc
CFLAGS := \
	-std=c99 \
	-O1 \
	-Wall \
	-Wextra \
	-pedantic \
	-fno-builtin \
	-Wno-unused-parameter \
	-nostdlib
LDFLAGS := -lgcc -nostdlib

default:	libc.a

clean:
	$(RM) libc.a *.o

libc.a:	libc.o syscall.o start.o
	$(AR) rs $@ $<

%.o:	src/%.c
	$(CC) $(CFLAGS) -c $<

%.o:	src/%.s
	$(CC) $(CFLAGS) -c $<

.PHONY: default
