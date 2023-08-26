SHELL := /bin/sh

srcdir := .
objdir := .

.SUFFIXES:
.SUFFIXES: .c .o .s .lo .S .y .l .tab.c .tab.h .yy.c .yy.h

PREFIX		:= 
DESTDIR		:=
CAT			:= cat
AS			:= $(PREFIX)gas
CPP      	:= $(PREFIX)cpp
LD			:= $(PREFIX)ld
CC 			:= $(PREFIX)gcc
AR			:= $(PREFIX)ar
RANLIB		:= $(PREFIX)ranlib
AFLAGS		:= \
	-ffreestanding \
	-nostdinc \
	-g \
	-Wall \
	-O0 \
	-Wextra \
	-pedantic \
	-fdiagnostics-color
CFLAGS 		:= \
	-std=c99 \
	-ffreestanding \
	-nostdinc \
	-fdiagnostics-color \
	-O0 \
	-ggdb3 \
	-Wall \
	-Wextra \
	-Wformat=2 \
	-Wno-unused-parameter \
	-Wmissing-field-initializers \
	-fno-asynchronous-unwind-tables \
	-Wno-sign-compare \
	-Wno-unused-but-set-variable \
	-pedantic

TEST_CFLAGS := $(filter-out -ffreestanding,$(CFLAGS))

SYSINCLUDE	:= $(shell $(CPP) -v /dev/null -o /dev/null 2>&1 | grep '^ .*gcc/.*include$$' | tr -d ' ')
CPPFLAGS	:= -isystem $(srcdir)/include -I$(objdir) -I$(objdir)/obj -I$(srcdir)/src -isystem $(SYSINCLUDE)
CPP_DEP		:= -MMD -MP
LDFLAGS 	:= -nostdlib
LIBCC		:= 
VERSION		:= $(shell $(CAT) "$(srcdir)/misc/VERSION")
PACKAGE		:= $(shell $(CAT) "$(srcdir)/misc/PACKAGE")

SRC_DIRS 	:= $(addprefix $(srcdir)/,src crt)
TEST_DIR    := $(addprefix $(srcdir)/,tests)

CORE_GLOB	:= $(addsuffix /*.c,$(SRC_DIRS))
CORE_GLOB	+= $(addsuffix /*.S,$(SRC_DIRS))
YACC_GLOB	:= $(addsuffix /*.y,$(SRC_DIRS))
LEX_GLOB	:= $(addsuffix /*.l,$(SRC_DIRS))
TEST_GLOB	:= $(addsuffix /*.c,$(TEST_DIR))

CORE_SRCS	:= $(sort $(wildcard $(CORE_GLOB)))
YACC_SRCS	:= $(sort $(wildcard $(YACC_GLOB)))
LEX_SRCS	:= $(sort $(wildcard $(LEX_GLOB)))
TEST_SRCS	:= $(sort $(wildcard $(TEST_GLOB)))

YACC_INT	:= $(addprefix $(objdir)/obj/, $(patsubst %,%.tab.c,$(notdir $(basename $(YACC_SRCS)))))
LEX_INT		:= $(addprefix $(objdir)/obj/, $(patsubst %,%.yy.c,$(notdir $(basename $(LEX_SRCS)))))

CORE_OBJS	:= $(patsubst $(srcdir)/%,%.o,$(basename $(CORE_SRCS)))
YACC_OBJS	:= $(addprefix $(objdir)/obj/,$(patsubst %.tab.c,%.tab.o,$(notdir $(YACC_INT))))
LEX_OBJS	:= $(addprefix $(objdir)/obj/,$(patsubst %.yy.c,%.yy.o,$(notdir $(LEX_INT))))
TEST_OBJS	:= $(patsubst $(srcdir)/%,%.o,$(basename $(TEST_SRCS)))

ALL_OBJS	:= $(addprefix $(objdir)/obj/, $(sort $(CORE_OBJS)))
ALL_OBJS	+= $(sort $(YACC_OBJS))
ALL_OBJS	+= $(sort $(LEX_OBJS))
TEST_OBJS	:= $(addprefix $(objdir)/obj/, $(sort $(TEST_OBJS)))

LIBC_OBJS	:= $(filter $(objdir)/obj/src/%,$(ALL_OBJS))
LIBC_OBJS	+= $(YACC_OBJS) $(LEX_OBJS)
LDSO_OBJS	:= $(filter $(objdir)/obj/ldso/%,$(ALL_OBJS:%.o=%.lo))
CRT_OBJS	:= $(filter $(objdir)/obj/crt/%,$(ALL_OBJS))

AOBJS		:= $(LIBC_OBJS)
LOBJS		:= $(LIBC_OBJS:.o=.lo)

STATIC_LIBS	:= $(objdir)/lib/libc.a
SHARED_LIBS	:= $(objdir)/lib/libc.so.$(VERSION)
CRT_LIBS	:= $(addprefix $(objdir)/lib/,$(notdir $(CRT_OBJS)))
ALL_LIBS	:= $(CRT_LIBS) $(STATIC_LIBS) #$(SHARED_LIBS)
OBJ_DIRS    := $(sort $(patsubst %/,%,$(dir $(ALL_LIBS) $(ALL_OBJS) $(TEST_OBJS))))
ALL_TESTS	:= $(TEST_OBJS:.o=)

all: .d $(ALL_LIBS) $(ALL_TESTS)


print:
	@#echo "YACC_GLOB=$(YACC_GLOB)"
	@#echo "YACC_SRCS=$(YACC_SRCS)"
	@#echo "YACC_INT=$(YACC_INT)"
	@#echo "YACC_OBJS=$(YACC_OBJS)"
	@#echo "LEX_GLOB=$(LEX_GLOB)"
	@#echo "LEX_SRCS=$(LEX_SRCS)"
	@#echo "LEX_INT=$(LEX_INT)"
	@#echo "LEX_OBJS=$(LEX_OBJS)"
	@#echo "LIBC_OBJS=$(LIBC_OBJS)"
	@echo "ALL_OBJS=$(ALL_OBJS)"
	@echo "OBJ_DIRS=$(OBJ_DIRS)"
	@echo "LOBJS=$(LOBJS)"
	@echo "LDSO_OBJS=$(LDSO_OBJS)"

.d .d/src .d/crt .d/tests:
	@[[ -d .d ]] || mkdir -p .d/{src,crt,tests} 2>/dev/null


$(ALL_LIBS) $(ALL_OBJS) $(ALL_OBJS:%.o=%.lo): | $(OBJ_DIRS)

$(OBJ_DIRS):
	@[[ -d $@ ]] || mkdir -p $@

clean:
	rm -rf $(objdir)/lib $(objdir)/obj $(objdir)/.d

$(objdir)/crt/Scrt1.o: CFLAGS += -fPIC -DDYN

$(LOBJS) $(LDSO_OBJS): CFLAGS += -fPIC -DDYN

CC_CMD    = $(CC) $(CFLAGS) $(CPPFLAGS) $(CPP_DEP) -MF .d/$*.d -c -o $@ $<
AS_CMD    = $(CC) $(AFLAGS) $(CPPFLAGS) $(CPP_DEP) -MF .d/$*.d -c -o $@ $<
CCYY_CMD  = $(CC) $(subst -fanalyzer,,$(CFLAGS)) $(CPPFLAGS) -MM -MG -MF $(objdir)/.d/$(<F:%.l=%.yy.d) $(objdir)/obj/$(<F:%.l=%.yy.c)
CCTAB_CMD = $(CC) $(subst -fanalyzer,,$(CFLAGS)) $(CPPFLAGS) -MM -MG -MF $(objdir)/.d/$(<F:%.y=%.tab.d) $(objdir)/obj/$(<F:%.y=%.tab.c)
LDYY_CMD  = $(CC) -c $(subst -fanalyzer,,$(CFLAGS)) $(CPPFLAGS) -Wno-unused-function $< -o $@
LEX_CMD   = $(LEX) $(LFLAGS) -o $(objdir)/obj/$(<F:%.l=%.yy.c) --header-file=$(objdir)/obj/$(<F:%.l=%.yy.h) $<
YACC_CMD  = $(YACC) $(YFLAGS) -t -o $(objdir)/obj/$(<F:%.y=%.tab.c) -d -p $(*F) $<

$(objdir)/re:	$(objdir)/obj/tests/re.o $(objdir)/lib/crt1.o $(objdir)/lib/libc.a
	$(CC) $(LDFLAGS) $< $(objdir)/lib/crt1.o $(objdir)/lib/libc.a -o $@

$(objdir)/obj/%.yy.h  $(objdir)/obj/%.yy.c  $(objdir)/.d/%.yy.d:  $(srcdir)/src/%.l $(objdir)/.d
	$(LEX_CMD)
	$(CCYY_CMD)

$(objdir)/obj/%.tab.h $(objdir)/obj/%.tab.c $(objdir)/.d/%.tab.d: $(srcdir)/src/%.y $(objdir)/.d
	$(YACC_CMD)
	$(CCTAB_CMD)

$(objdir)/obj/%.tab.o: $(objdir)/obj/%.tab.c $(objdir)/obj/%.tab.h $(objdir)/.d/%.tab.d
	$(LDYY_CMD)

$(objdir)/obj/%.tab.lo: $(objdir)/obj/%.tab.c $(objdir)/obj/%.tab.h $(objdir)/.d/%.tab.d
	$(LDYY_CMD)

$(objdir)/obj/%.yy.o: $(objdir)/obj/%.yy.c $(objdir)/obj/%.yy.h $(objdir)/.d/%.yy.d
	$(LDYY_CMD)

$(objdir)/obj/%.yy.lo: $(objdir)/obj/%.yy.c $(objdir)/obj/%.yy.h $(objdir)/.d/%.yy.d
	$(LDYY_CMD)



$(objdir)/obj/%.s:	$(srcdir)/%.S
	$(CC) -E $(CPPFLAGS) $(CPP_DEP) -MF .d/$*.d -o $@ $< 

$(objdir)/obj/%.o:	$(srcdir)/%.c
	$(CC_CMD)

$(objdir)/obj/tests/%.o:	$(srcdir)/%.c
	$(CC_CMD)

$(objdir)/obj/%.o:	$(objdir)/obj/%.s
	$(AS_CMD)

$(objdir)/obj/%.lo: $(objdir)/obj/%.s
	$(AS_CMD)
	
$(objdir)/obj/%.lo: $(srcdir)/%.c
	$(CC_CMD)

$(objdir)/lib/libc.so.$(VERSION): $(LOBJS) $(LDSO_OBJS)
	$(CC) $(LDFLAGS) -Wl,-soname,libc.so.1 -shared -o $@ $(LOBJS) $(LDSO_OBJS) $(LIBCC)

# was -Wl,-e,_dlstart

$(objdir)/lib/libc.a: $(AOBJS)
	rm -f $@
	$(AR) rcD $@ $(AOBJS)
	$(RANLIB) -D $@

$(objdir)/lib/%.o: $(objdir)/obj/crt/%.o
	cp $< $@

.PHONY: default

-include $($(sort $(CORE_GLOB)):%.c=.d/%.d)
