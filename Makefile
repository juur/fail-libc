SHELL := /bin/sh

srcdir := .
objdir := .

#.SUFFIXES:
#.SUFFIXES: .c .o .s .lo

DESTDIR		:=
CAT			:= cat
CC 			:= gcc
AR			:= ar
RANLIB		:= ranlib
CFLAGS 		:= \
	-pipe \
	-std=c11 \
	-ffreestanding \
	-nostdinc \
	-nostdlib \
	-fno-builtin \
	-ggdb \
	-O \
	-Wall \
	-Wextra \
	-Wformat=2 \
	-Wno-unused-parameter \
	-Wno-sign-compare \
	-pedantic
CPPFLAGS	:= -I$(srcdir)/include
CPPFLAGS	+= -MMD -MP
LDFLAGS 	:= 
LIBCC		:= -lgcc
VERSION		:= $(shell $(CAT) "$(srcdir)/misc/VERSION")
PACKAGE		:= $(shell $(CAT) "$(srcdir)/misc/PACKAGE")

SRC_DIRS 	:= $(addprefix $(srcdir)/,src crt)
CORE_GLOB	:= $(addsuffix /*.c,$(SRC_DIRS))
CPPS_GLOB	:= $(addsuffix /*.S,src)
CPPS_SRCS	:= $(CPPS_GLOB:%.s=%.S)
CORE_GLOB	+= $(addsuffix /*.[sS],$(SRC_DIRS))
CORE_SRCS	:= $(sort $(wildcard $(CORE_GLOB)))
CORE_OBJS	:= $(patsubst $(srcdir)/%,%.o,$(basename $(CORE_SRCS)))
ALL_OBJS	:= $(addprefix $(objdir)/obj/, $(sort $(CORE_OBJS)))

LIBC_OBJS	:= $(filter $(objdir)/obj/src/%,$(ALL_OBJS))
LDSO_OBJS	:= $(filter $(objdir)/obj/ldso/%,$(ALL_OBJS:%.o=%.lo))
CRT_OBJS	:= $(filter $(objdir)/obj/crt/%,$(ALL_OBJS))

AOBJS		:= $(LIBC_OBJS)
LOBJS		:= $(LIBC_OBJS:.o=.lo)

STATIC_LIBS	:= $(objdir)/lib/libc.a
SHARED_LIBS	:= $(objdir)/lib/libc.so
CRT_LIBS	:= $(addprefix $(objdir)/lib/,$(notdir $(CRT_OBJS)))
ALL_LIBS	:= $(CRT_LIBS) $(STATIC_LIBS) #$(SHARED_LIBS)

all: .d $(ALL_LIBS)

.d:
	@mkdir -p .d 2>/dev/null

OBJ_DIRS = $(sort $(patsubst %/,%,$(dir $(ALL_LIBS) $(ALL_OBJS))))

$(ALL_LIBS) $(ALL_OBJS) $(ALL_OBJS:%.o=%.lo): | $(OBJ_DIRS)

$(OBJ_DIRS):
	mkdir -p $@

clean:
	rm -rf $(objdir)/lib $(objdir)/obj

$(objdir)/crt/Scrt1.o:	CFLAGS += -fPIC -DDYN

$(LOBJS) $(LDSO_OBJS): CFLAGS += -fPIC -DDYN

CC_CMD = $(CC) $(CFLAGS) $(CPPFLAGS) -MF .d/$*.d -c -o $@ $<
AS_CMD = $(CC_CMD)

$(objdir)/obj/%.o:	$(srcdir)/%.c
	$(CC_CMD)

$(objdir)/obj/%.o:	$(srcdir)/%.s
	$(AS_CMD)

$(objdir)/obj/%.lo: $(srcdir)/%.s
	$(AS_CMD)
	
$(objdir)/obj/%.lo: $(srcdir)/%.c
	$(CC_CMD)

$(objdir)/lib/libc.so: $(LOBJS) $(LDSO_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -nostdlib -shared -Wl,-e,_dlstart -o $@ $(LOBJS) $(LDSO_OBJS) $(LIBCC)

$(objdir)/lib/libc.a: $(AOBJS)
	rm -f $@
	$(AR) rc $@ $(AOBJS)
	$(RANLIB) $@

$(objdir)/lib/%.o: $(objdir)/obj/crt/%.o
	cp $< $@

.PHONY: default

-include $($(addprefix obj/, $(sort $(CORE_GLOB))):%.c=.d/%.d)
