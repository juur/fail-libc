#define _FAIL_LIBC_INTERNAL

/* 
 * includes
 */

#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <failos/syscall.h>
#include <asm/prctl.h>
#include <fcntl.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>
#include <utmpx.h>
#include <termios.h>
#include <utime.h>
#include <sys/ioctl.h>
#include <regex.h>
#include <err.h>
#include <pwd.h>
#include <grp.h>
#include <uchar.h>
#include <sys/socket.h>
#include <stdatomic.h>
#include <sys/statvfs.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <syslog.h>
#include <sys/un.h>
#include <iconv.h>
#include <mntent.h>
#include <netdb.h>
#include <wordexp.h>
#include <poll.h>
#include <sys/utsname.h>
#include <fmtmsg.h>
#include <glob.h>
#include <curses.h>
#include <assert.h>

#ifdef VALGRIND
#include <valgrind.h>
#include <memcheck.h>
#endif

/* 
 * preprocessor defines and macros
 */

#define hidden __attribute__((__visibility__("hidden")))

#define ARCH_SET_FS 0x1002

#define CLONE_VM            0x00000100
#define CLONE_FS            0x00000200
#define CLONE_FILES         0x00000400
#define CLONE_SIGHAND       0x00000800
#define CLONE_PTRACE        0x00002000
#define CLONE_VFORK         0x00004000
#define CLONE_PARENT        0x00008000
#define CLONE_THREAD        0x00010000
#define CLONE_NEWNS         0x00020000
#define CLONE_SYSVSEM       0x00040000
#define CLONE_SETTLS        0x00080000
#define CLONE_PARENT_SETTID 0x00100000
#define CLONE_CHILD_CLEARTID 0x00200000
#define CLONE_DETACHED      0x00400000
#define CLONE_UNTRACED      0x00800000
#define CLONE_CHILD_SETTID  0x01000000
#define CLONE_NEWCGROUP     0x02000000
#define CLONE_NEWUTS        0x04000000
#define CLONE_NEWIPC        0x08000000
#define CLONE_NEWUSER       0x10000000
#define CLONE_NEWPID        0x20000000
#define CLONE_NEWNET        0x40000000
#define CLONE_IO            0x80000000

/* getaddrinfo */

#define HDR_QR (1<<0)
#define HDR_SET_OPCODE(x) (((x)&0xf)<<1)
#define HDR_GET_OPCODE(x) (((x)>>1)&0xf)
#define HDR_AA (1<<6)
#define HDR_TC (1<<7)
#define HDR_RD (1<<8)
#define HDR_RA (1<<9)
#define HDR_SET_RCODE(x) (((x)&0xf)<<11)
#define HDR_GET_RCODE(x) (((x)>>11)&0xf)

#define CLASS_IN       1
#define CLASS_NONE     254
#define CLASS_ANY      255

/* internal memory */

#define MEM_MAGIC   0x61666c69UL

/* utf32toutf8 / utf8_to_utf32 */

#define UTF8_1BYTE 0x00
#define UTF8_2BYTE 0xc0
#define UTF8_3BYTE 0xe0
#define UTF8_4BYTE 0xf0
#define UTF8_NBYTE 0x80

#define UTF8_1BYTE_MASK 0x80
#define UTF8_2BYTE_MASK 0xe0
#define UTF8_3BYTE_MASK 0xf0
#define UTF8_4BYTE_MASK 0xf8
#define UTF8_NBYTE_MASK 0xc0

/* regexec() etc */

#undef RE_DEBUG

/* operator tokens taken from iso8859-1 */

// ·
#define CAT   0xB7
// ¦
#define OR    0xA6
// ¤
#define STAR  0xA4
// ¶
#define TERM  0xB6
// «
#define OPEN  0xAB
// »
#define CLOSE 0xBB
// ¿
#define OPT   0xBF
// ±
#define PLUS  0xB1
// Ø
#define NONE  0xD8
// å
#define ANY   0xE5
// ¹
#define BRACKET_OPEN 0xB9
// º
#define BRACKET_CLOSE 0xB0



/*
 * enums and enum typedefs
 */

enum dns_opcodes {
    OPCODE_QUERY  = 0,
    OPCODE_IQUERY = 1,
    OPCODE_STATUS = 2,
    OPCODE_NOTIFY = 4,
    OPCODE_UPDATE = 5,
};

enum dns_rcodes {
    RCODE_NOERROR  = 0,
    RCODE_FORMERR  = 1,
    RCODE_SERVFAIL = 2,
    RCODE_NXDOMAIN = 3,
    RCODE_NOTIMP   = 4,
    RCODE_REFUSED  = 5,
    RCODE_YXDOMAIN = 6,
    RCODE_YXRRSET  = 7,
    RCODE_NXRRSET  = 8,
    RCODE_NOTAUTH  = 9,
    RCODE_NOTZONE  = 10,
};


/* DNS lookups etc. */

enum dns_in_types {
    TYPE_A     = 1,
    TYPE_NS    = 2,
    TYPE_CNAME = 5,
    TYPE_SOA   = 6,
    TYPE_PTR   = 12,
    TYPE_MX    = 15,
    TYPE_TXT   = 16,
    TYPE_AAAA  = 28,
    TYPE_SRV   = 33,
    QTYPE_IXFR = 251,
    QTYPE_AXFR = 252,
    QTYPE_ALL  = 255
};

/* printf() etc */

typedef enum { JUSTIFY_NONE = 0, JUSTIFY_LEFT = 1, JUSTIFY_RIGHT = 2 } justify_t;

/* regexec() etc */

typedef enum {
    ET_PTRDIFF_T = 1,
    ET_VOID_T    = 2,
    ET_UINT8_T   = 3,
    ET_UINT16_T  = 4,
    ET_UINT32_T  = 5,
    ET_UINT64_T  = 6,
    ET_INT32_T   = 7
} etype_t;


/* parse_resolv_config() */

struct resolv {
    char **nameservers;
    in_addr_t *nameservers_in;
};

struct atexit_fun {
    struct atexit_fun *next;
    void (*function)(void);
};

struct mem_alloc {
    struct mem_alloc *next;
    struct mem_alloc *prev;
    uint32_t flags;
    uint32_t magic;
    size_t   len;
    void    *start;
    void    *end;
} __attribute__((packed));

#define MF_FREE (1<<0)

/* all internal ? */

#if __has_attribute(__counted_by__)
# define __counted_by(member)  __attribute__((__counted_by__(member)))
#else
# define __counted_by(member)
#endif

struct dns_result {
    int num_records;

    struct {
        int record_type;
        union {
            struct {
                char *mname;
                char *rname;
                long serial, refresh, retry, expire, minimum;
            } soa;
            struct in_addr in_v4;
            struct {
                int weight;
                char *string;
            } mx;
            char *string;
        }; 
    }rr[] __counted_by(num_records);
};

struct dns_header {
    union {
        struct {
            uint16_t ident;
            uint16_t flags;
            uint16_t num_questions;
            uint16_t num_answers;
            uint16_t num_rrs;
            uint16_t num_add_rrs;
        } __attribute__((packed));
        uint16_t words[6];
    } __attribute__((packed));
} __attribute__((packed));

struct dns_question {
    char *name;
    uint16_t type;
    uint16_t class;
};

struct dns_rr {
    bool allocated;
    unsigned char *name;
    union {
        struct {
            uint16_t type;
            uint16_t class;
            uint32_t ttl;
            uint16_t rdlength;
        } __attribute__((packed));
    } __attribute__((packed)) vals;
    void *additional;
};


/* iconv() */
struct iconv_private {
    int from;
    int to;

    char32_t buf[32];

    /* add additional state information here */
};

/* locale */
struct locale_t {
    int mask;
    char *locales[LC_ALL + 2];
};

typedef enum { TYPE_STACK = 1, TYPE_QUEUE = 2 } list_type_t;

/* regexec() */
typedef struct array_t {
    int len;
    int val[];
} array_t;

typedef struct node_t {
    struct node_t *root;
    struct node_t *left;
    struct node_t *right;

    array_t *firstpos;
    array_t *lastpos;
    array_t *followpos;

    int     pos_size;
    int     pos;
    bool    nullable;
    uint8_t type;

    array_t *start_groups;
    array_t *end_groups;

} node_t;

/* stack & queue headers */
typedef struct {
    void   *data;
    int     len;
    int     sp;
    int     pad0;
    list_type_t     type;
    etype_t etype;
} _re_stack_t; /* name clashes with <signal.h> */

typedef struct {
    void   *data;
    int     len;
    int     head;
    int     tail;
    list_type_t     type;
    etype_t etype;
} queue_t;


struct aug_state {
    _re_stack_t *in_vstack;
    _re_stack_t *out_vstack;
    uint8_t *are;
    uint8_t *are_ptr;
    uint8_t *is_match;
    ssize_t  are_len;
};

struct dfa_state_t;

typedef struct dfa_trans_t {
    struct dfa_state_t *to;

    array_t *start_capture;
    array_t *end_capture;

    uint8_t  match;
} dfa_trans_t;

typedef struct dfa_state_t {
    struct dfa_state_t *next;
    dfa_trans_t *(*trans)[];
    array_t     *state;

    int  num_trans;
    int  id;
    bool terminal;
    bool marked;
} dfa_state_t;

typedef union token_t {
    struct {
        uint8_t  token;
        uint8_t  pad0;
        uint16_t orig_pos;
        uint16_t pad1;
        int16_t group;
    } __attribute__((packed)) t;
    uint64_t val;
} __attribute__((packed)) token_t;

/* 
 * global variables 
 */

char **environ = NULL;
int daylight = 0;
long timezone = 0;
char *tzname[2] = {
    "GMT",
    "GMT"
};
WINDOW *stdscr = NULL;
WINDOW *curscr = NULL;
int LINES = 0;
int COLS = 0;
int COLOR_PAIRS = 0;
int COLORS = 0;


/* hidden global variables */

hidden FILE __stdout = {
    .fd = 1
};

hidden FILE __stdin = {
    .fd = 0
};

hidden FILE __stderr = {
    .fd = 2
};

FILE *stdout = &__stdout;
FILE *stdin = &__stdin;
FILE *stderr = &__stderr;

/* external function declarations */

/*
 * a reminder of syscall()
 *
 * %rdi    - number  -> %rax
 * %rsi    - 1st arg -> %rdi
 * %rdx    - 2nd arg -> %rsi
 * %rcx    - 3rd arg -> %rdx
 * %r8     - 4th arg -> %r10 (why?)
 * %r9     - 5th arg -> %r8
 * 8(%rsp) - 6th arg -> %r9
 */

extern int main(int, char *[], char *[]);

/*
 * library declarations 
 * use <system> #includes instead
 */


/*
 * local declarations 
 */

static int send_request(const char *name, void *result_out, int result_type);
static int vxscanf(const char *restrict src, FILE *restrict stream, const char *restrict format, va_list ap);
static int vxnprintf(char *restrict dst, FILE *restrict stream, size_t size, const char *restrict format, va_list ap);
static struct mem_alloc *alloc_mem(size_t size);
static void free_alloc(struct mem_alloc *buf);
static void check_mem();
static struct __pthread *__pthread_self(void);
static char *fgets_delim(char *s, int size, FILE *stream, int delim);
static int calc_base(const char **ptr);

/* 
 * anonymous local structs with constants 
 */

/* locale */

static struct locale_def {
    const char *const name;
} def_locale = {
    .name = "C",
};

static const struct locale_def *current_locale[LC_ALL + 2] = {
    [0]           = NULL,

    [LC_ALL]      = &def_locale,
    [LC_COLLATE]  = &def_locale,
    [LC_CTYPE]    = &def_locale,
    [LC_MESSAGES] = &def_locale,
    [LC_MONETARY] = &def_locale,
    [LC_NUMERIC]  = &def_locale,
    [LC_TIME]     = &def_locale,

    [LC_ALL + 1]  = NULL
};

/* 
 * local variables 
 */

/* curses */

static SCREEN *cur_screen = NULL;
static char *doupdate_bufptr = NULL;
static char *doupdate_bufend = NULL;

/* openlog()/syslog() etc */
static int unix_socket;
static int sl_options;
static int sl_facility;
static int sl_mask;
static const char *sl_ident;

/* getaddrinfo() etc */
static struct resolv *resolv;
static int which_ns;

/* getutxent() etc */
static struct utmpx utmpx_tmp;

static struct mem_alloc *tmp_first;
static struct mem_alloc *first;
static struct mem_alloc *last;
static void *_data_end, *_data_start;
static struct atexit_fun *global_atexit_list;

/* asctime() */
static struct tm localtime_tmp;
static struct tm gmtime_tmp;
static char asctime_tmp[28];

/* random()/srandom() */
static long random_state[31];
static int random_state_ptr = 0;

/* getmntent() etc */
static struct mntent mntent_ret;

/* constants */

/* debug strings for etype_t */
static const char *const etype_names[] = {
    NULL,
    "ET_PTRDIFF_T",
    "ET_VOID_T",
    "ET_UINT8_T",
    "ET_UINT16_T",
    "ET_UINT32_T",
    "ET_UINT64_T",
    "ET_INT32_T",
    NULL
};


/* stores the precedence (larger number is greater)
 * and if the operator is left_associative
 */
static const struct {
    const int  prec;
    const bool left_assoc;
} ops[256] = {
    [OPEN]  = {6,  false},
    [CLOSE] = {6,  false},
    [STAR]  = {5,  true},
    [PLUS]  = {5,  true},
    [OPT]   = {5,  true},
    [CAT]   = {4,  true},
    [OR]    = {3,  true},
    [TERM]  = {2,  true},
    [NONE]  = {2,  true},
    [ANY]   = {1,  true}
};

/* local function defintions */


/* 
 * inline functions 
 */

inline static long max(long a, long b)
{
    return a > b ? a : b;
}

inline static long min(long a, long b)
{
    return a < b ? a : b;
}

/* constants */

/* regerror() */
static const char *const regerrors[] = {
    [REG_SUCCESS]  = "REG_SUCCESS",
    [REG_NOMATCH]  = "REG_NOMATCH",
    [REG_BADPAT]   = "REG_BADPAT",
    [REG_ECOLLATE] = "REG_ECOLLATE",
    [REG_ECTYPE]   = "REG_ECTYPE",
    [REG_EESCAPE]  = "REG_EESCAPE",
    [REG_ESUBREG]  = "REG_ESUBREG",
    [REG_EBRACK]   = "REG_EBRACK",
    [REG_EPAREN]   = "REG_EPAREN",
    [REG_EBRACE]   = "REG_EBRACE",
    [REG_BADBR]    = "REG_BADBR",
    [REG_ERANGE]   = "REG_ERANGE",
    [REG_ESPACE]   = "REG_ESPACE",
    [REG_BADRPT]   = "REG_BADRPT",
    NULL
};
static const int num_regerrors = sizeof(regerrors)/sizeof(const char *);

/* fmtmsg() */
static const char *const mm_sevs[] = {
    "",
    "INFO",
    "WARNING",
    "ERROR",
    "HALT",
    NULL
};

/* getaddrinfo/gethostinfo */
[[maybe_unused]] static const char *const dns_qclass_strings[] = {
    [CLASS_IN]   = "IN",
    [CLASS_NONE] = "NONE",
    [CLASS_ANY]  = "ANY",
    NULL,
};

[[maybe_unused]] static const char *const dns_qtype_strings[] = {
    [TYPE_A]     = "A",
    [TYPE_NS]    = "NS",
    [TYPE_CNAME] = "CNAME",
    [TYPE_SOA]   = "SOA",
    [TYPE_PTR]   = "PTR",
    [TYPE_MX]    = "MX",
    [TYPE_TXT]   = "TXT",
    [TYPE_AAAA]  = "AAAA",
    [TYPE_SRV]   = "SRV",
    [QTYPE_IXFR] = "IXFR",
    [QTYPE_AXFR] = "AXFR",
    [QTYPE_ALL]  = "ALL",
    NULL
};

[[maybe_unused]] static const char *const dns_opcode_strings[] = {
    [OPCODE_QUERY]  = "QUERY",
    [OPCODE_IQUERY] = "IQUERY",
    [OPCODE_STATUS] = "STATUS",
    [OPCODE_NOTIFY] = "NOTIFY",
    [OPCODE_UPDATE] = "UPDATE",
    NULL
};

[[maybe_unused]] static const char *const dns_rcode_strings[] = {
    [RCODE_NOERROR]  = "No error condition",
    [RCODE_FORMERR]  = "Formet error",
    [RCODE_SERVFAIL] = "Server failure",
    [RCODE_NXDOMAIN] = "Name error",
    [RCODE_NOTIMP]   = "Not implemented",
    [RCODE_REFUSED]  = "Refused",
    [RCODE_YXDOMAIN] = "Domain exists",
    [RCODE_YXRRSET]  = "RR exists",
    [RCODE_NXRRSET]  = "RR missing",
    [RCODE_NOTAUTH]  = "Not authoritative",
    [RCODE_NOTZONE]  = "Not within zone",
    NULL
};

static const struct {
    const char *const short_name;
    const char type;
} term_caps[] = {
    {"bw",'b'    }, {"am",'b'   }, {"bce",'b'   }, {"ccc",'b'   }, {"xhp",'b'  }, 
    {"xhpa",'b'  }, {"cpix",'b' }, {"crxm",'b'  }, {"xt",'b'    }, {"xenl",'b' }, 
    {"eo",'b'    }, {"gn",'b'   }, {"hc",'b'    }, {"chts",'b'  }, {"km",'b'   }, 
    {"daisy",'b' }, {"hs",'b'   }, {"hls",'b'   }, {"in",'b'    }, {"lpix",'b' }, 
    {"da",'b'    }, {"db",'b'   }, {"mir",'b'   }, {"msgr",'b'  }, {"nxon",'b' }, 
    {"xsb",'b'   }, {"npc",'b'  }, {"ndscr",'b' }, {"nrrmc",'b' }, {"os",'b'   }, 
    {"mc5i",'b'  }, {"xvpa",'b' }, {"sam",'b'   }, {"eslok",'b' }, {"hz",'b'   }, 
    {"ul",'b'    }, {"xon",'b'  }, 

    {"bitwin",'#' }, {"bitype",'#' }, {"bufsz",'#'  }, {"btns",'#'  }, {"cols",'#'  },
    {"colors",'#' }, {"spinh",'#' }, {"spinv",'#' }, {"it",'#'     }, {"lh",'#'     },
    {"lw",'#'     }, {"lines",'#' }, {"lm",'#'    }, {"ma",'#'     }, {"xmc",'#'    },
    {"pairs",'#'  }, {"wnum",'#'   }, {"mcs",'#'    }, {"mls",'#'   }, {"ncv",'#'   }, 
    {"nlab",'#'   }, {"npins",'#'  }, {"orc",'#'    }, {"orl",'#'   }, {"orhi",'#'  }, 
    {"orvi",'#'   }, {"pb",'#'     }, {"cps",'#'    }, {"vt",'#'    }, {"widcs",'#' }, 
    {"wsl",'#'    }, {"colors",'#' }, {"maddr",'#' }, {"mjump",'#' }, 
    
    {"acsc",'s'   }, {"scesa",'s'   }, {"cbt",'s'     }, {"bel",'s'    }, {"bicr",'s'     }, 
    {"binel",'s'  }, {"birep",'s'   }, {"cr",'s'      }, {"cpi",'s'    }, {"lpi",'s'      }, 
    {"chr",'s'    }, {"cvr",'s'     }, {"csr",'s'     }, {"rmp",'s'    }, {"csnm",'s'     }, 
    {"tbc",'s'    }, {"mgc",'s'     }, {"clear",'s'   }, {"el1",'s'    }, {"el",'s'       }, 
    {"ed",'s'     }, {"csin",'s'    }, {"colornm",'s' }, {"hpa",'s'    }, {"cmdch",'s'    }, 
    {"cwin",'s'   }, {"cup",'s'     }, {"cud1",'s'    }, {"home",'s'   }, {"civis",'s'    }, 
    {"cub1",'s'   }, {"mrcup",'s'   }, {"cnorm",'s'   }, {"cuf1",'s'   }, {"ll",'s'       }, 
    {"cuu1",'s'   }, {"cvvis",'s'   }, {"defbi",'s'   }, {"defc",'s'   }, {"dch1",'s'     }, 
    {"dl1",'s'    }, {"devt",'s'    }, {"dial",'s'    }, {"dsl",'s'    }, {"dclk",'s'     }, 
    {"dispc",'s'  }, {"hd",'s'      }, {"enacs",'s'   }, {"endbi",'s'  }, {"smacs",'s'    }, 
    {"smam",'s'   }, {"blink",'s'   }, {"bold",'s'    }, {"smcup",'s'  }, {"smdc",'s'     }, 
    {"dim",'s'    }, {"swidm",'s'   }, {"sdrfq",'s'   }, {"ehhlm",'s'  }, {"smir",'s'     }, 
    {"sitm",'s'   }, {"elhlm",'s'   }, {"slm",'s'     }, {"elohlm",'s' }, {"smicm",'s'    }, 
    {"snlq",'s'   }, {"snrmq",'s'   }, {"smpch",'s'   }, {"prot",'s'   }, {"rev",'s'      }, 
    {"erhlm",'s'  }, {"smsc",'s'    }, {"invis",'s'   }, {"sshm",'s'   }, {"smso",'s'     }, 
    {"ssubm",'s'  }, {"ssupm",'s'   }, {"ethlm",'s'   }, {"smul",'s'   }, {"sum",'s'      }, 
    {"evhlm",'s'  }, {"smxon",'s'   }, {"ech",'s'     }, {"rmacs",'s'  }, {"rmam",'s'     }, 
    {"sgr0",'s'   }, {"rmcup",'s'   }, {"rmdc",'s'    }, {"rwidm",'s'  }, {"rmir",'s'     }, 
    {"ritm",'s'   }, {"rlm",'s'     }, {"rmicm",'s'   }, {"rmpch",'s'  }, {"rmsc",'s'     }, 
    {"rshm",'s'   }, {"rmso",'s'    }, {"rsubm",'s'   }, {"rsupm",'s'  }, {"rmul",'s'     }, 
    {"rum",'s'    }, {"rmxon",'s'   }, {"pause",'s'   }, {"hook",'s'   }, {"flash",'s'    }, 
    {"ff",'s'     }, {"fsl",'s'     }, {"getm",'s'    }, {"wingo",'s'  }, {"hup",'s'      }, 
    {"is1",'s'    }, {"is2",'s'     }, {"is3",'s'     }, {"if",'s'     }, {"iprog",'s'    }, 
    {"initc",'s'  }, {"initp",'s'   }, {"ich1",'s'    }, {"il1",'s'    }, {"ip",'s'       }, 
    {"ka1",'s'    }, {"ka3",'s'     }, {"kb2",'s'     }, {"kbs",'s'    }, {"kbeg",'s'     }, 
    {"kcbt",'s'   }, {"kc1",'s'     }, {"kc3",'s'     }, {"kcan",'s'   }, {"ktbc",'s'     }, 
    {"kclr",'s'   }, {"kclo",'s'    }, {"kcmd",'s'    }, {"kcpy",'s'   }, {"kcrt",'s'     }, 
    {"kctab",'s'  }, {"kdch1",'s'   }, {"kdl1",'s'    }, {"kcud1",'s'  }, {"krmir",'s'    }, 
    {"kend",'s'   }, {"kent",'s'    }, {"kel",'s'     }, {"ked",'s'    }, {"kext",'s'     }, 
    {"kf1",'s'    }, {"kf10",'s'    }, {"kf11",'s'    }, {"kf12",'s'   }, {"kf13",'s'     }, 
    {"kf14",'s'   }, {"kf15",'s'    }, {"kf16",'s'    }, {"kf17",'s'   }, {"kf18",'s'     }, 
    {"kf19",'s'   }, {"kf2",'s'     }, {"kf20",'s'    }, {"kf21",'s'   }, {"kf22",'s'     }, 
    {"kf23",'s'   }, {"kf24",'s'    }, {"kf25",'s'    }, {"kf26",'s'   }, {"kf27",'s'     }, 
    {"kf28",'s'   }, {"kf29",'s'    }, {"kf30",'s'    }, {"kf31",'s'   }, {"kf32",'s'     }, 
    {"kf33",'s'   }, {"kf34",'s'    }, {"kf35",'s'    }, {"kf36",'s'   }, {"kf37",'s'     }, 
    {"kf38",'s'   }, {"kf39",'s'    }, {"kf40",'s'    }, {"kf41",'s'   }, {"kf42",'s'     }, 
    {"kf43",'s'   }, {"kf44",'s'    }, {"kf45",'s'    }, {"kf46",'s'   }, {"kf47",'s'     }, 
    {"kf48",'s'   }, {"kf49",'s'    }, {"kf50",'s'    }, {"kf51",'s'   }, {"kf52",'s'     }, 
    {"kf53",'s'   }, {"kf54",'s'    }, {"kf55",'s'    }, {"kf56",'s'   }, {"kf57",'s'     }, 
    {"kf58",'s'   }, {"kf59",'s'    }, {"kf60",'s'    }, {"kf61",'s'   }, {"kf62",'s'     }, 
    {"kf63",'s'   }, {"kf64",'s'    }, {"kf65",'s'    }, {"kf3",'s'    }, {"kf4",'s'      }, 
    {"kf5",'s'    }, {"kf6",'s'     }, {"kf7",'s'     }, {"kf8",'s'    }, {"kf9",'s'      }, 
    {"kfnd",'s'   }, {"khlp",'s'    }, {"khome",'s'   }, {"kich1",'s'  }, {"kil1",'s'     }, 
    {"kcub1",'s'  }, {"kll",'s'     }, {"kmrk",'s'    }, {"kmsg",'s'   }, {"kmous",'s'    }, 
    {"kmov",'s'   }, {"knxt",'s'    }, {"knp",'s'     }, {"kopn",'s'   }, {"kopt",'s'     }, 
    {"kpp",'s'    }, {"kprv",'s'    }, {"kprt",'s'    }, {"krdo",'s'   }, {"kref",'s'     }, 
    {"krfr",'s'   }, {"krpl",'s'    }, {"krst",'s'    }, {"kres",'s'   }, {"kcuf1",'s'    }, 
    {"ksav",'s'   }, {"kBEG",'s'    }, {"kCAN",'s'    }, {"kCMD",'s'   }, {"kCPY",'s'     }, 
    {"kCRT",'s'   }, {"kDC",'s'     }, {"kDL",'s'     }, {"kslt",'s'   }, {"kEND",'s'     }, 
    {"kEOL",'s'   }, {"kEXT",'s'    }, {"kind",'s'    }, {"kFND",'s'   }, {"kHLP",'s'     }, 
    {"kHOM",'s'   }, {"kIC",'s'     }, {"kLFT",'s'    }, {"kMSG",'s'   }, {"kMOV",'s'     }, 
    {"kNXT",'s'   }, {"kOPT",'s'    }, {"kPRV",'s'    }, {"kPRT",'s'   }, {"kri",'s'      }, 
    {"kRDO",'s'   }, {"kRPL",'s'    }, {"kRIT",'s'    }, {"kRES",'s'   }, {"kSAV",'s'     }, 
    {"kSPD",'s'   }, {"khts",'s'    }, {"kUND",'s'    }, {"kspd",'s'   }, {"kund",'s'     }, 
    {"kcuu1",'s'  }, {"rmkx",'s'    }, {"smkx",'s'    }, {"lf0",'s'    }, {"lf1",'s'      }, 
    {"lf2",'s'    }, {"lf3",'s'     }, {"lf4",'s'     }, {"lf5",'s'    }, {"lf6",'s'      }, 
    {"lf7",'s'    }, {"lf8",'s'     }, {"lf9",'s'     }, {"lf10",'s'   }, {"fln",'s'      }, 
    {"rmln",'s'   }, {"smln",'s'    }, {"rmm",'s'     }, {"smm",'s'    }, {"mhpa",'s'     }, 
    {"mcud1",'s'  }, {"mcub1",'s'   }, {"mcuf1",'s'   }, {"mvpa",'s'   }, {"mcuu1",'s'    }, 
    {"minfo",'s'  }, {"nel",'s'     }, {"porder",'s'  }, {"oc",'s'     }, {"op",'s'       }, 
    {"pad",'s'    }, {"dch",'s'     }, {"dl",'s'      }, {"cud",'s'    }, {"mcud",'s'     }, 
    {"ich",'s'    }, {"indn",'s'    }, {"il",'s'      }, {"cub",'s'    }, {"mcub",'s'     }, 
    {"cuf",'s'    }, {"mcuf",'s'    }, {"rin",'s'     }, {"cuu",'s'    }, {"mcuu",'s'     }, 
    {"pctrm",'s'  }, {"pfkey",'s'   }, {"pfloc",'s'   }, {"pfxl",'s'   }, {"pfx",'s'      }, 
    {"pln",'s'    }, {"mc0",'s'     }, {"mc5p",'s'    }, {"mc4",'s'    }, {"mc5",'s'      }, 
    {"pulse",'s'  }, {"qdial",'s'   }, {"rmclk",'s'   }, {"rep",'s'    }, {"rfi",'s'      }, 
    {"reqmp",'s'  }, {"rs1",'s'     }, {"rs2",'s'     }, {"rs3",'s'    }, {"rf",'s'       }, 
    {"rc",'s'     }, {"vpa",'s'     }, {"sc",'s'      }, {"scesc",'s'  }, {"ind",'s'      }, 
    {"ri",'s'     }, {"scs",'s'     }, {"s0ds",'s'    }, {"s1ds",'s'   }, {"s2ds",'s'     }, 
    {"s3ds",'s'   }, {"sgr1",'s'    }, {"setab",'s'   }, {"setaf",'s'  }, {"sgr",'s'      }, 
    {"setb",'s'   }, {"smgb",'s'    }, {"smgbp",'s'   }, {"sclk",'s'   }, {"setcolor",'s' }, 
    {"scp",'s'    }, {"setf",'s'    }, {"smgl",'s'    }, {"smglp",'s'  }, {"smglr",'s'    }, 
    {"slines",'s' }, {"slength",'s' }, {"smgr",'s'    }, {"smgrp",'s'  }, {"hts",'s'      }, 
    {"smgtb",'s'  }, {"smgt",'s'    }, {"smgtp",'s'   }, {"wind",'s'   }, {"sbim",'s'     }, 
    {"scsd",'s'   }, {"rbim",'s'    }, {"rcsd",'s'    }, {"subcs",'s'  }, {"supcs",'s'    }, 
    {"ht",'s'     }, {"docr",'s'    }, {"tsl",'s'     }, {"tone",'s'   }, {"u0",'s'       }, 
    {"u1",'s'     }, {"u2",'s'      }, {"u3",'s'      }, {"u4",'s'     }, {"u5",'s'       }, 
    {"u6",'s'     }, {"u7",'s'      }, {"u8",'s'      }, {"u9",'s'     }, {"uc",'s'       }, 
    {"hu",'s'     }, {"wait",'s'    }, {"xoffc",'s'   }, {"xonc",'s'   }, {"zerom",'s'    }, 
    {"meml",'s'   }, {"memu",'s'    }, 

    {NULL,0}
};


/*
 * constants
 */

static const char *terminfo_location = "/usr/share/terminfo/";
//static const char terminfo_location[] = "terminfo/";

/*
 * private globals
 */

static struct terminfo *termdb = NULL;
static char tiparm_ret[BUFSIZ];
extern bool nc_use_env;


/*
 * public globals
 */

TERMINAL *cur_term;



static void dump_one_mem(const struct mem_alloc *const mem)
{
    __builtin_printf( "mem @ %p [prev=%p,next=%p,free=%d,len=%lu]\n",
            (void *)mem,
            (void *)mem->prev,
            (void *)mem->next,
            mem->flags & MF_FREE,
            mem->len);
}

__attribute__((unused)) static void dump_mem()
{
    const struct mem_alloc *tmp;
    int i;

    printf("start: ");
    dump_one_mem(first);
    printf("\n");

    printf("last:  ");
    dump_one_mem(last);
    printf("\n");

    for (i = 0, tmp = first; tmp; tmp = tmp->next, i++) {
        printf("[%d] ", i);
        dump_one_mem(tmp);
        printf("\n");
    }
}

__attribute__((unused)) static void dump_mem_stats(void)
{
    printf( "\n"
            "tmp_first = %p\n"
            "first     = %p\n"
            "last      = %p\n",
            tmp_first,
            first,
            last);

    size_t blocks = 0, block_free = 0;
    size_t bytes = 0, bytes_free = 0;

    for (struct mem_alloc *tmp = first; tmp; tmp = tmp->next)
    {
        blocks++;
        bytes += tmp->len;

        if (tmp->flags & MF_FREE) {
            block_free++;
            bytes_free+=tmp->len;
        }

    }

    printf( "blocks    = %8lu (avg %lu)\n"
            "..free    = %8lu (avg %lu)\n"
            "bytes     = %8lu (%lu MiB)\n"
            "..free    = %8lu (%lu MiB)\n",
            blocks, blocks ? bytes/blocks : 0,
            block_free, block_free ? bytes_free/block_free : 0,
            bytes, bytes / (1024*1024),
            bytes_free, bytes_free / (1024*1024));
}

[[gnu::nonnull]] static int utf32toutf8(const char32_t from, char *to)
{
    if (from < 0x80) {
        to[0] = ( (from >>  0) & ~UTF8_1BYTE_MASK ) | UTF8_1BYTE;
        return 1;

    } else if (from < 0x800) {
        to[0] = ( (from >>  6) & ~UTF8_2BYTE_MASK ) | UTF8_2BYTE;
        to[1] = ( (from >>  0) & ~UTF8_NBYTE_MASK ) | UTF8_NBYTE;
        return 2;

    } else if (from < 0x10000) {
        to[0] = ( (from >> 12) & ~UTF8_3BYTE_MASK ) | UTF8_3BYTE;
        to[1] = ( (from >>  6) & ~UTF8_NBYTE_MASK ) | UTF8_NBYTE;
        to[2] = ( (from >>  0) & ~UTF8_NBYTE_MASK ) | UTF8_NBYTE;
        return 3;

    } else if (from < 0x110000) {
        to[0] = ( (from >> 18) & ~UTF8_4BYTE_MASK ) | UTF8_4BYTE;
        to[1] = ( (from >> 12) & ~UTF8_NBYTE_MASK ) | UTF8_NBYTE;
        to[2] = ( (from >>  6) & ~UTF8_NBYTE_MASK ) | UTF8_NBYTE;
        to[3] = ( (from >>  0) & ~UTF8_NBYTE_MASK ) | UTF8_NBYTE;
        return 4;

    }

    return -1;
}

[[gnu::nonnull]] static int utf8toutf32(const char *orig_from, char32_t *to)
{
    const uint8_t *from = (const uint8_t *)orig_from;

    if (       (from[0] & UTF8_1BYTE_MASK) == UTF8_1BYTE) {
        /* 1 byte */
        *to = 0U;
        *to |= ((from[0] & ~UTF8_1BYTE_MASK) << 0);
        return 1;

    } else if ((from[0] & UTF8_2BYTE_MASK) == UTF8_2BYTE) {
        /* 2 byte */
        *to  = 0U;
        *to |= ((from[0] & ~UTF8_2BYTE_MASK) << 6);
        *to |= ((from[1] & ~UTF8_NBYTE_MASK) << 0);
        return 2;

    } else if ((from[0] & UTF8_3BYTE_MASK) == UTF8_3BYTE) {
        /* 3 byte */
        *to  = 0U;
        *to |= ((from[0] & ~UTF8_3BYTE_MASK) << 12);
        *to |= ((from[1] & ~UTF8_NBYTE_MASK) << 6);
        *to |= ((from[2] & ~UTF8_NBYTE_MASK) << 0);
        return 3;

    } else if ((from[0] & UTF8_4BYTE_MASK) == UTF8_4BYTE) {
        /* 4 byte */
        *to  = 0U;
        *to |= ((from[0] & ~UTF8_4BYTE_MASK) << 18);
        *to |= ((from[1] & ~UTF8_NBYTE_MASK) << 12);
        *to |= ((from[2] & ~UTF8_NBYTE_MASK) << 6);
        *to |= ((from[3] & ~UTF8_NBYTE_MASK) << 0);
        return 4;

    } else {
        errno = EINVAL;
        return -1;
    }
}

static size_t ascii_to_utf32(const char *src, char32_t *dst) {
    const char *src_ptr = src;
    char32_t   *dst_ptr = dst;

    errno = EINVAL;

    if (*src_ptr <= 0)
        return -1;

    *dst_ptr++ = *src_ptr++;

    errno = 0;
    return 0;
}

static size_t utf32_to_ascii(const char32_t *src, char *dst) {
    const char32_t *src_ptr = src;
    char           *dst_ptr = dst;

    errno = EINVAL;

    if (*src_ptr == 0)
        return -1;
    else if (*src_ptr > 0x7f)
        *dst_ptr++ = ' ';
    else
        *dst_ptr++ = *src_ptr;

    src_ptr++;

    errno = 0;
    return 0;
}

static size_t utf32_to_utf8(const char32_t *src, char *dst) {
    return utf32toutf8(*src, dst);
}

static size_t utf8_to_utf32(const char *src, char32_t *dst) {
    return utf8toutf32(src, dst);
}

static size_t utf32_in(const char *src, char32_t *dst) {
    *dst = *((char32_t *)src);
    return 4;
}

static size_t utf32_out(const char32_t *src, char *dst) {
    *(char32_t *)dst = *src;
    return 4;
}

static struct {
    const char *name;
    uint16_t    id;

    size_t   (*to_utf32)(const char     *src, char32_t *dst);
    size_t (*from_utf32)(const char32_t *src, char     *dst);
} iconv_codesets[] = {
    { "ascii",       367, ascii_to_utf32,     utf32_to_ascii    },
    { "us-ascii",    367, ascii_to_utf32,     utf32_to_ascii    },
    { "cp367",       367, ascii_to_utf32,     utf32_to_ascii    },
    { "iso646-us",   367, ascii_to_utf32,     utf32_to_ascii    },
    { "ibm367",      367, ascii_to_utf32,     utf32_to_ascii    },
    { "utf8",       1209, utf8_to_utf32,      utf32_to_utf8     },
    { "utf-8",      1209, utf8_to_utf32,      utf32_to_utf8     },
    { "utf32",         0, utf32_in,           utf32_out         },
    { "utf-32",        0, utf32_in,           utf32_out         },
    //  { "iso-8859-1",  819, iso88591_to_utf32,  utf32_to_iso88591 },

    {NULL, 0, NULL, NULL}
};

static int find_iconv_codeset(const char *name)
{
    for (int i = 0; iconv_codesets[i].name; i++) {
        if (!(strcasecmp(name, iconv_codesets[i].name)))
            return i;
    }

    return -1;
}

size_t iconv(iconv_t cd, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft)
{
    struct iconv_private *state = cd;

    if (state == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* may need to update the iconv_codeset entries to include functions for
     * state reset & shift sequence */
    if ( inbuf == NULL || *inbuf == NULL ) {
        /* set init state ... */
        memset(state->buf, 0, sizeof(state->buf));
        printf("init ");

        if ( outbuf != NULL || (outbuf && *outbuf != NULL )) {
            printf("shift");
            /* ... and store shift sequence */
        }
        printf("\n");
        return 0;
    }

    /* normal conversion */

    char *src_ptr = *inbuf;
    char *dst_ptr = *outbuf;
    size_t done = 0;

    //printf("loop start: %p[%x] -> %p[%x]\n", src_ptr, *inbytesleft, dst_ptr, *outbytesleft);
    while (*src_ptr && *inbytesleft && *outbytesleft) {
        //printf("loop 0\n");

        done = iconv_codesets[state->from].to_utf32(src_ptr, state->buf);
        //printf("loop 0 = %x\n", done);
        if (done == (size_t)-1)
            return -1;
        if (done == 0)
            break;

        src_ptr      += done;
        *inbytesleft -= done;

        //printf("loop 1\n");

        done = iconv_codesets[state->to].from_utf32(state->buf, dst_ptr);
        if (done == (size_t)-1)
            return -1;
        if (done == 0)
            break;

        /* TODO undo src_ptr / inbytesleft if done == 0 ? */

        dst_ptr       += done;
        *outbytesleft -= done;
    }

    return 0;
}

iconv_t iconv_open(const char *tocode, const char *fromcode)
{
    int to   = find_iconv_codeset(tocode);
    int from = find_iconv_codeset(fromcode);

    if ( to == -1 || from == -1 ) {
        errno = EINVAL;
        return (iconv_t)-1;
    }

    struct iconv_private *ret;

    if ((ret = calloc(1, sizeof(struct iconv_private))) == NULL)
        return (iconv_t)-1;

    ret->from = from;
    ret->to   = to;

    return ret;
}

int iconv_close(iconv_t cd)
{
    if (cd != NULL)
        free(cd);

    return 0;
}

int system(const char *command)
{
    pid_t child;
    int wstatus = 0;

    if (command == NULL) {
        if (access("/bin/sh", R_OK|X_OK) == 0)
            return 1;
        return 0;
    }

    struct sigaction *oldint = NULL, *oldquit = NULL;
    sigset_t oset, set;
    bool restore_chld = true;

    if ((sigprocmask(0, NULL, &oset)) == -1)
        return -1;

    if (sigismember(&oset, SIGCHLD))
        restore_chld = false;

    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_BLOCK, &set, NULL);

    sigaction(SIGINT, &(struct sigaction) {.sa_handler = SIG_IGN}, oldint);
    sigaction(SIGQUIT, &(struct sigaction) {.sa_handler = SIG_IGN}, oldquit);

    if ((child = fork()) == 0) {
        if (execl("/bin/sh", "sh", "-c", command, NULL) == -1)
            _exit(127);
    } else if (child == -1) {
        return wstatus = -1;;
    }

    if (wstatus == 0)
        waitpid(child, &wstatus, 0);

    if (restore_chld) {
        sigemptyset(&set);
        sigaddset(&set, SIGCHLD);
        sigprocmask(SIG_UNBLOCK, &set, NULL);
    }

    sigaction(SIGINT, oldint, NULL);
    sigaction(SIGQUIT, oldquit, NULL);

    return wstatus;
}

pid_t waitpid(pid_t pid, int *wstatus, int options)
{
    return syscall(__NR_wait4, pid, wstatus, options, NULL);
}

int execve(const char *path, char *const argv[], char *const envp[])
{
    //int rc = 0;
    //printf("execve: path=%s, argv=%p, envp=%p, envp[0]=%p, envp[0]=%s\n", path, argv, envp, envp[0], envp[0]);
    return syscall(__NR_execve, (uint64_t)path, (uint64_t)argv, (uint64_t)envp);
    //printf("execve: returned %u\n", rc);
    //return rc;
}

static const char *exec_find(const char *file)
{
    char buf[PATH_MAX];

    if (access(file, R_OK|X_OK) == 0)
        return strdup(file);

    const char *pathtmp = getenv("PATH");

    if (pathtmp == NULL) {
        printf("exec_find: PATH is NULL\n");
        errno = ENOENT;
        return NULL;
    }

    char *path, *res;

    if ((path = strdup(pathtmp)) == NULL)
        return NULL;

    char *saveptr = NULL;

    res = strtok_r(path, ":", &saveptr);

    while (res)
    {
        snprintf(buf, sizeof(buf), "%s/%s", res, file);

        if (access(buf, X_OK|R_OK) == 0) {
            free(path);
            return strdup(buf);
        }

        res = strtok_r(NULL, ":", &saveptr);
    }

    free(path);
    errno = ENOENT;
    return NULL;
}

int execl(const char *path, const char *arg0, ...)
{
    int argc = 1;
    va_list ap;
    char *tmp, **argv;

    va_start(ap, arg0);
    while ((tmp = va_arg(ap, char *)) != NULL)
        argc++;
    va_end(ap);

    if ((argv = calloc(argc + 1, sizeof(char *))) == NULL)
        return -1;

    argv[0] = strdup(arg0);

    va_start(ap, arg0);
    for (int i = 1; i < argc; i++)
        argv[i] = va_arg(ap, char *);
    va_end(ap);

    return execve(path, argv, environ);
}

int execlp(const char *file, const char *arg0, ...)
{
    const char *pathname;

    pathname = exec_find(file);

    if (pathname == NULL)
        return -1;

    int argc = 1;
    va_list ap;
    char *tmp, **argv;

    va_start(ap, arg0);
    while ((tmp = va_arg(ap, char *)) != NULL)
        argc++;
    va_end(ap);

    if ((argv = calloc(argc + 1, sizeof(char *))) == NULL)
        return -1;

    argv[0] = strdup(arg0);

    va_start(ap, arg0);
    for (int i = 1; i < argc; i++)
        argv[i] = va_arg(ap, char *);
    va_end(ap);

    return execve(pathname, argv, environ);
}

int execle(const char *pathname, const char *arg, ...)
{
    int argc = 1;
    va_list ap;
    char *tmp, **argv, **envp;

    va_start(ap, arg);
    while ((tmp = va_arg(ap, char *)) != NULL)
        argc++;
    envp = va_arg(ap, char **);
    va_end(ap);

    if ((argv = calloc(argc + 1, sizeof(char *))) == NULL)
        return -1;

    argv[0] = strdup(pathname);
    va_start(ap, arg);
    for (int i = 1; i < argc; i++)
        argv[i] = va_arg(ap, char *);
    va_end(ap);

    return execve(pathname, argv, envp);
}

int execv(const char *pathname, char *const argv[])
{
    return execve(pathname, argv, environ);
}

int execvp(const char *file, char *const argv[])
{
    const char *pathname;

    pathname = exec_find(file);

    if (pathname == NULL)
        return -1;

    return execve(pathname, argv, environ);
}


int execvpe(const char *file, char *const argv[], char *const envp[])
{
    const char *pathname;

    pathname = exec_find(file);

    if (pathname == NULL)
        return -1;

    return execve(pathname, argv, envp);
}

int getpriority(int which, id_t who)
{
    return syscall(__NR_getpriority, which, who);
}

    __attribute__((nonnull))
static size_t _qsort_partition(void *_base, size_t width, int (*comp)(const void *, const void *),
        ssize_t begin, ssize_t end)
{
    uint8_t *base = _base;
    uint8_t *pivot = base + (end * width);
    ssize_t i = (begin - 1);
    uint8_t swap_temp[width];// = malloc(width);

    //if (swap_temp == NULL)
    //    return begin - 1;

    for (ssize_t j = begin; j < end; j++)
    {
        if (comp(base + (j * width), pivot) <= 0) {
            i++;

            memcpy(swap_temp, base + (i * width), width);
            memcpy(base + (i * width), base + (j * width), width);
            memcpy(base + (j * width), swap_temp, width);
        }
    }

    i++;

    memcpy(swap_temp, base + (i * width), width);
    memcpy(base + (i * width), base + (end * width), width);
    memcpy(base + (end * width), swap_temp, width);

    //free(swap_temp);

    return i;
}

    __attribute__((nonnull))
static void _qsort(void *base, size_t width, int (*comp)(const void *, const void *),
        ssize_t begin, ssize_t end)
{
    if (begin < end) {
        ssize_t idx = _qsort_partition(base, width, comp, begin, end);
        if (idx < begin)
            return;

        _qsort(base, width, comp, begin, idx - 1);
        _qsort(base, width, comp, idx + 1, end);
    }
}

void qsort(void *base, size_t nel, size_t width, int (*comp)(const void *, const void *))
{
    if (base == NULL || comp == NULL || width == 0) {
        errno = EINVAL;
        return;
    }

    if (nel < 2)
        return;

    _qsort(base, width, comp, 0, nel - 1);
}

int setpriority(int which, id_t who, int pri)
{
    return syscall(__NR_setpriority, which, who, pri);
}

int nice(int inc)
{
    return setpriority(PRIO_PROCESS, 0, getpriority(PRIO_PROCESS, 0) + inc);
}

char *stpcpy(char *restrict dest, const char *restrict src)
{
    if (dest == NULL || src == NULL)
        goto fail;

    size_t i;
    for (i = 0; src[i]; i++)
        dest[i] = src[i];

    dest[i] = '\0';

    return &dest[i];

fail:
    return NULL;
}

char *strcpy(char *dest, const char *src)
{
    if (dest == NULL || src == NULL)
        goto fail;

    size_t i;
    for (i = 0; src[i]; i++)
        dest[i] = src[i];

    dest[i] = '\0';

fail:
    return dest;
}

size_t strlcpy(char *restrict dest, const char *restrict src, size_t dstsize)
{
    size_t cnt = 0;

    while (*src && cnt < dstsize)
    {
        *dest++ = *src++;
        cnt++;
    }

    return cnt;
}

size_t strlcat(char *restrict dest, const char *restrict src, size_t dstsize)
{
    size_t cnt = 0;

    while (*dest && cnt < dstsize) {
        dest++;
        cnt++;
    }

    while (*src && cnt < dstsize)
    {
        *dest++ = *src++;
        cnt++;
    }

    return cnt;
}

char *strncpy(char *restrict dest, const char *restrict src, size_t n)
{
    if (dest == NULL || src == NULL)
        goto fail;

    size_t i;

    /* copy the characters in src (up to n) */
    for (i = 0; src[i] && i < n; i++)
        dest[i] = src[i];

    /* pad with NUL any remaining characters up to n */
    for (     ;           i < n; i++)
        dest[i] = '\0';

fail:
    return dest;
}

/* Linux specific */
int arch_prctl(int code, unsigned long addr)
{
    return syscall(__NR_arch_prctl, code, addr);
}

[[gnu::alias("_exit")]] void _Exit(int status);
[[gnu::noreturn]] void _exit(int status)
{
    if (environ) {
        for (size_t i = 0; environ[i]; i++)
            if (environ[i])
                free(environ[i]);
        free(environ);
    }

    void *npt;
    arch_prctl(ARCH_GET_FS, (uintptr_t)&npt);
    free(npt);

    check_mem();
    syscall(__NR_exit_group, status);
    for (;;) __asm__ volatile("hlt");
}

[[gnu::noreturn]] void exit(int status)
{
    for (struct atexit_fun *node = global_atexit_list; node; node = node->next) {
        node->function();
    }
    _exit(status);
}

static unsigned long rand_seed;

int rand(void)
{
    rand_seed = rand_seed * 1103515245 + 12345;
    return ((unsigned)(rand_seed/(RAND_MAX * 2)) % RAND_MAX);
}

void srand(unsigned int seed)
{
    rand_seed = seed;
}

long random(void)
{
    long ret;
    random_state[random_state_ptr] = random_state[random_state_ptr] * 1103515245 + 12345;
    ret = ((uint32_t)(random_state[random_state_ptr]/(0xffffffff * 2)) % 0xffffffff);
    random_state_ptr++;
    if (random_state_ptr >= 31)
        random_state_ptr = 0;
    return ret;
}

void srandom(unsigned seed)
{
    for (int i = 0; i < 31; i++)
        random_state[i] = seed * i + i;
}

int mkstemp(char *template)
{
    if (!template || strlen(template) < 6)
        return -1;

    char *xxx;

    if (strcmp((xxx = template + strlen(template) - 6), "XXXXXX"))
        return -1;

    char tmp[6];
    srand((uintptr_t)template + time(NULL));

    for (int i = 0; i < 6; i++)
        switch(rand()%(3-1)+1)
        {
            case 1:  tmp[i] = rand()%('9'-'0')+'0'; break;
            case 2:  tmp[i] = rand()%('Z'-'A')+'A'; break;
            default: tmp[i] = rand()%('z'-'a')+'a'; break;
        }

    memcpy(xxx, tmp, 6);

    int fd;

    if ((fd = open(template, O_CREAT|O_EXCL|O_NOCTTY|O_RDWR, 0600)) == -1)
        return -1;

    unlink(template);

    return fd;
}

    __attribute__((pure))
char *strchr(const char *const s, const int c)
{
    if (s == NULL) return NULL;

    const char *tmp;

    for (tmp = s;; tmp++) {
        if (*tmp == '\0')
            return NULL;
        if (*tmp == c)
            return (char *)tmp;
    }
    return NULL;
}

size_t strcspn(const char *s, const char *accept)
{
    const char *ptr;
    const size_t len = strlen(accept);
    size_t i;
    size_t ret;

    ret = 0;

    for (ptr = s; *ptr; ptr++)
    {
        for (i = 0; i < len; i++)
            if (*ptr == accept[i])
                goto fail;
        ret++;
        continue;
fail:
        break;
    }

    return ret;
}

size_t strspn(const char *s, const char *accept)
{
    const char *ptr;
    const size_t len = strlen(accept);
    size_t i;
    size_t ret;

    ret = 0;

    for (ptr = s; *ptr; ptr++)
    {
        for (i = 0; i < len; i++)
            if (*ptr == accept[i])
                goto next;
        break;
next:
        ret++;
    }

    return ret;
}

    __attribute__((pure))
char *strrchr(const char *const s, const int c)
{
    if (s == NULL) return NULL;

    const char *tmp;

    tmp = (s + (strlen(s) - 1));

    while (tmp >= s && *tmp != c) tmp--;
    if (tmp < s) return NULL;
    return (char *)tmp;
}

static char *strtok_state;

char *strtok(char *restrict s, const char *restrict sep)
{
    return strtok_r(s, sep, &strtok_state);
}

char *strtok_r(char *restrict str, const char *restrict delim, char **restrict saveptr)
{
    char *tmp, *ret;

    if (saveptr == NULL || delim == NULL)
        return NULL;

    if (str)
        *saveptr = str;

    if (!*saveptr)
        return NULL;

    while (**saveptr && *(*saveptr+1) && strchr(delim, **saveptr))
        *(*saveptr)++ = '\0';

    tmp = *saveptr;

    while (*tmp && !strchr(delim, *tmp))
        tmp++;

    while (*tmp && *(tmp+1) && strchr(delim, *(tmp+1)))
        *tmp++ = '\0';

    if (tmp == *saveptr)
        return (*saveptr = NULL);

    if (!*tmp) {
        ret = *saveptr;
        *saveptr = NULL;
        return ret;
    }

    *tmp = '\0';
    ret = *saveptr;

    *saveptr = ++tmp;
    return ret;
}

ssize_t write(int fd, const void *buf, size_t count)
{
    return syscall(__NR_write, fd, buf, count);
}

int getsockname(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len)
{
    return syscall(__NR_getsockname, socket, address, address_len);
}

ssize_t recvfrom(int socket, void *restrict buffer, size_t length, int flags, struct sockaddr *restrict address, socklen_t *restrict address_len)
{
    return syscall(__NR_recvfrom, socket, buffer, length, flags, address, address_len);
}

ssize_t read(int fd, void *buf, size_t count)
{
    return syscall(__NR_read, fd, buf, count);
}

int symlink(const char *path1, const char *path2)
{
    return syscall(__NR_symlink, path1, path2);
}

int link(const char *path1, const char *path2)
{
    return syscall(__NR_link, path1, path2);
}

int open(const char *pathname, int flags, ...)
{
    mode_t mode = 0;

    if (flags & O_CREAT) {
        va_list ap;
        va_start(ap, flags);
        mode = va_arg(ap, mode_t);
        va_end(ap);
    }

    return syscall(__NR_open, pathname, flags, mode);
}

int access(const char *pathname, int mode)
{
    return syscall(__NR_access, pathname, mode);
}

int close(int fd)
{
    return syscall(__NR_close, fd, 0 ,0 ,0 ,0 ,0, 0);
}

int chmod(const char *path, mode_t mode)
{
    return syscall(__NR_chmod, path, mode);
}

int lchown(const char *pathname, uid_t owner, gid_t group)
{
    return syscall(__NR_lchown, pathname, owner, group);
}

int chown(const char *pathname, uid_t owner, gid_t group)
{
    return syscall(__NR_chown, pathname, owner, group);
}

int lstat(const char *pathname, struct stat *statbuf)
{
    int fd, rc;

    if ((fd = open(pathname, O_RDONLY|O_NOFOLLOW)) == -1)
        return -1;

    rc = fstat(fd, statbuf);

    close(fd);

    return rc;
}

int fstat(int fd, struct stat *buf)
{
    return syscall(__NR_fstat, fd, buf);
}

int utime(const char *path, const struct utimbuf *times)
{
    return syscall(__NR_utime, path, times);
}

int utimes(const char *path, const struct timeval times[2])
{
    return syscall(__NR_utimes, path, times);
}

int unlink(const char *path)
{
    return syscall(__NR_unlink, path);
}

int stat(const char *restrict pathname, struct stat *restrict statbuf)
{
    return syscall(__NR_stat, pathname, statbuf);
}

off_t lseek(int fd, off_t offset, int whence)
{
    return syscall(__NR_lseek, fd, offset, whence);
}

int fileno(FILE *stream)
{
    if (stream == NULL || stream->mem) {
        errno = EBADF;
        return -1;
    }

    return stream->fd;
}

int fclose(FILE *stream)
{
    int ret = 0;

    if (!stream) {
        return 0;
    }

    if (!stream->mem && stream->fd != -1) {
        close(stream->fd);
    }

    if (stream->buf) {
        free(stream->buf);
    }
    free(stream);

    return ret;
}

[[gnu::nonnull]]
static void itoa(char *buf, int base, unsigned long d, __attribute__((unused)) bool pad, __attribute__((unused)) int size)
{
    char *p = buf, *p1, *p2;
    unsigned long ud = d;
    unsigned long divisor = 10;
    unsigned long remainder;

    if (base=='d' && (long)d < 0)
    {
        *p++ = '-';
        buf++;
        ud = -d;
    } else if (base=='x') {
        divisor = 16;
    }

    do {
        remainder = ud % divisor;
        *p++ = (char)((remainder < 10) ? remainder + '0' : remainder + 'a' - 10);
    } while (ud /= divisor);

    *p = 0;

    p1 = buf;
    p2 = p - 1;

    while (p1<p2)
    {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}

[[gnu::format(printf,3,4)]]
int snprintf(char *restrict str, size_t size, const char *restrict format, ...)
{
    int ret;
    va_list ap;
    va_start(ap, format);
    ret = vsnprintf(str, size, format, ap);
    va_end(ap);
    return ret;
}

[[gnu::format(printf,2,3)]] int sprintf(char *restrict s, const char *restrict format, ...)
{
    int ret;
    va_list ap;
    va_start(ap, format);
    ret = vsprintf(s, format, ap);
    va_end(ap);
    return ret;
}

[[gnu::format(printf,2,3)]] int fprintf(FILE *restrict stream, const char *restrict format, ...)
{
    int ret;
    va_list ap;
    va_start(ap, format);
    ret = vfprintf(stream, format, ap);
    va_end(ap);
    return ret;
}

[[gnu::format(printf,1,2)]] int printf(const char *restrict format, ...)
{
    if (stdout == NULL || format == NULL)
        return 0;

    int ret;
    va_list ap;
    va_start(ap, format);
    ret = vfprintf(stdout, format, ap);
    va_end(ap);
    return ret;
}

int vfscanf(FILE *restrict stream, const char *restrict format, va_list arg)
{
    return vxscanf(NULL, stream, format, arg);
}

int vscanf(const char *restrict format, va_list arg)
{
    if (stdin == NULL)
        return 0;

    return vfscanf(stdin, format, arg);
}

int vsscanf(const char *restrict s, const char *restrict format, va_list arg)
{
    return vxscanf(s, NULL, format, arg);
}

int fscanf(FILE *restrict stream, const char *restrict format, ...)
{
    int ret;
    va_list ap;
    va_start(ap, format);
    ret = vfscanf(stream, format, ap);
    va_end(ap);
    return ret;
}

int sscanf(const char *restrict s, const char *restrict format, ...)
{
    int ret;

    va_list ap;
    va_start(ap, format);
    ret = vsscanf(s, format, ap);
    va_end(ap);
    return ret;
}

int scanf(const char *restrict format, ...)
{
    int ret;
    va_list ap;
    va_start(ap, format);
    ret = vscanf(format, ap);
    va_end(ap);
    return ret;
}

int vsprintf(char *restrict dst, const char *restrict format, va_list ap)
{
    return vxnprintf(dst, NULL, 0, format, ap);
}

int vsnprintf(char *restrict dst, size_t size, const char *restrict format, va_list ap)
{
    return vxnprintf(dst, NULL, size, format, ap);
}

int vfprintf(FILE *restrict stream, const char *format, va_list ap)
{
    return vxnprintf(NULL, stream, 0, format, ap);
}

int vprintf(const char *restrict format, va_list ap)
{
    return vfprintf(stdout, format, ap);
}

#define _CHAR   1
#define _SHORT  2
#define _INT    4
#define _LONG   8
#define _LLONG  16

static const char *lastss;
static const char *ss;
static bool ss_invert;

static bool is_valid_scanset(const char *scanset, char c, char negate)
{
    ss = lastss = scanset;

    if (*ss == negate) {
        ss++;
        ss_invert = true;
    } else
        ss_invert = false;

    return strchr(ss, c) ? !ss_invert : ss_invert;
}

/* need to support glob(3) which doesn't use '^' */
[[gnu::nonnull]] static char *expand_scanset(char *orig, char negate)
{
    /* 1a803c69406f9e9e60754a9c9728e03572965850 change to preallocated buffer as malloc go boom */
    char ret[BUFSIZ];
    size_t off = 0;
    const char *sptr = NULL;
    char from = 0, to = 0;

    //memset(ret, 0, sizeof(ret));

    sptr = orig;
    if (*sptr && *sptr == negate)
        ret[off++] = *sptr++;

    while (*sptr)
    {
        if (*(sptr + 1) == '-' && *(sptr + 2)) {
            from = *sptr;
            to = *(sptr + 2);

            for (char t = from; t <= to; t++)
                ret[off++] = t;
            sptr += 3;
        } else
            ret[off++] = *sptr++;
    }

    ret[off] = '\0';
    strcpy(orig, ret);
    return orig;
}

static int vxscanf(const char *restrict src, FILE *restrict stream, const char *restrict format, va_list ap)
{
    char c=0, chr_in=0;
    const char *restrict save;
    //const char *restrict p;
    //char *scanset = NULL;
    char scanset[BUFSIZ];
    char buf[64] = {0};
    char   s_buf[BUFSIZ];
    bool is_file = stream ? true : false;
    bool do_scanset = false;
    int bytes_scanned = 0, rc = -1;
    unsigned buf_idx;

    if (format == NULL || (src == NULL && stream == NULL))
        return -1;

    while ((c = *format++) != 0)
    {
        if (is_file && (feof(stream) || ferror(stream))) {
            goto fail;
        } else if (!is_file && !*src) {
            goto fail;
        } else if (isspace(c)) {
            while (isspace(*format)) format++;

            do {
                int tmp;
                if (is_file) {
                    if ((tmp = fgetc(stream)) == EOF)
                        break;
                    chr_in = (char)tmp;
                } else {
                    if (*src == '\0')
                        break;
                    chr_in = *src++;
                }

                if (chr_in == '\0')
                    break;

                if (isspace(chr_in))
                    continue;

                if (is_file)
                    ungetc(chr_in, stream);
                else
                    src--;

                break;
            } while(1);
        } else if (c != '%') {
            int tmp;

            if (is_file) {
                if ((tmp = fgetc(stream)) == EOF)
                    break;
                chr_in = (char)tmp;
            } else {
                if (*src == '\0')
                    break;
                chr_in = *src++;
            }

            if (chr_in == '\0') {
                break;
            }

            if (c != chr_in) {
                break;
            }
        } else {
            /* %[*]['][m][max_field_width][type_modifier]conversion_specifier */

            int len = _INT, str_limit = 0, sub_read = 0;
            bool do_malloc = false;
            bool do_not_store = false;
            int base = 10;
next:
            c = *format++;

            if (isdigit((unsigned char)c)) {
                str_limit *= 10;
                str_limit += c - '0';
                goto next;
            }

            /* TODO this mixes all the different conversion specificiations, we
             * should only parse * as the first, for example */
            switch(c)
            {
                case 'h':
                    len = (len == _SHORT ? _CHAR : _SHORT);
                    goto next;

                case 'l':
                    len = (len == _LONG ? _LLONG : _LONG);
                    goto next;

                case 'j':
                    len = _INT;
                    goto next;

                case 'z':
                    len = _LONG;
                    goto next;

                case 't':
                    len = _LONG;
                    goto next;

                case 'm':
                    do_malloc = true;
                    goto next;

                case '*':
                    do_not_store = true;
                    goto next;

                case 'x':
                    base = 16;
                    goto do_num_scan;
                case 'd':
                case 'u':
                case 'i':
                    base = 10;
do_num_scan:
                    buf_idx = 0;
                    do {
                        /* read all the digits into buf & set last to \0 */
                        int tmp;

                        if (is_file) {
                            if ((tmp = fgetc(stream)) == EOF)
                                break;
                            chr_in = (char)tmp;
                        } else {
                            if (*src == '\0')
                                break;
                            chr_in = *src++;
                        }

                        if (chr_in == '\0')
                            break;

                        if (!isdigit(chr_in)) {
                            if (is_file) { ungetc(chr_in, stream); } else { src--; }
                            break;
                        }

                        //printf(".d.got: %c\n", chr_in);

                        buf[buf_idx++] = chr_in;
                    } while(buf_idx < sizeof(buf)-1);
                    /* TODO size modifiers {hh,h,l,ll,j,z,t} */
                    /* TODO check for buf overflow */
                    buf[buf_idx] = '\0';

                    switch(c) {
                        case 'x':
                            base = 16;
                            /* fall through */
                        case 'u':
                            switch (len) {
                                case _CHAR:
                                    if (!do_not_store)
                                        *(unsigned char*)(va_arg(ap, unsigned char *)) = strtoul(buf, NULL, base);
                                    bytes_scanned++;
                                    break;
                                case _SHORT:
                                    if (!do_not_store)
                                        *(unsigned short *)(va_arg(ap, unsigned short *)) = strtoul(buf, NULL, base);
                                    bytes_scanned++;
                                    break;
                                case _INT:
                                    if (!do_not_store)
                                        *(unsigned *)(va_arg(ap, unsigned *)) = strtoul(buf, NULL, base);
                                    bytes_scanned++;
                                    break;
                                case _LONG:
                                    if (!do_not_store)
                                        *(unsigned long *)(va_arg(ap, unsigned long*)) = strtoul(buf, NULL, base);
                                    bytes_scanned++;
                                    break;
                                case _LLONG:
                                    if (!do_not_store)
                                        *(unsigned long long*)(va_arg(ap, unsigned long long*)) = strtoull(buf, NULL, base);
                                    bytes_scanned++;
                                    break;
                            }
                            break;

                        case 'i':
                            base = 0;
                            /* fall through */
                        case 'd':
                            switch (len) {
                                case _CHAR:
                                    if (!do_not_store)
                                        *(char *)(va_arg(ap, char *)) = strtol(buf, NULL, base);
                                    bytes_scanned++;
                                    break;
                                case _SHORT:
                                    if (!do_not_store)
                                        *(short *)(va_arg(ap, short *)) = strtol(buf, NULL, base);
                                    bytes_scanned++;
                                    break;
                                case _INT:
                                    if (!do_not_store)
                                        *(int *)(va_arg(ap, int *)) = strtol(buf, NULL, base);
                                    bytes_scanned++;
                                    break;
                                case _LONG:
                                    if (!do_not_store)
                                        *(long *)(va_arg(ap, long *)) = strtol(buf, NULL, base);
                                    bytes_scanned++;
                                    break;
                                case _LLONG:
                                    if (!do_not_store)
                                        *(long long *)(va_arg(ap, long long *)) = strtoll(buf, NULL, base);
                                    bytes_scanned++;
                                    break;
                            }
                            break;
                    }
                    break; /* case d: case u: */

                case '[':
                    {
                        do_scanset = false;

                        /* read the scan set up to ] */
                        save = format;

                        if (*format == '^') format++;
                        if (*format == '\0') goto fail;
                        if (*format == ']') format++;

                        while((c = *format++) != '\0')
                            if (c == ']')
                                break;

                        if (c == '\0')
                            goto fail;

                        /* 1a803c69406f9e9e60754a9c9728e03572965850 move to buffer as malloc is shit */
                        memset(scanset, 0, sizeof(scanset));

                        strncat(scanset, save, format - save - 1);
                        if ((expand_scanset(scanset, '^')) == NULL)
                            goto fail;
                        do_scanset = true;
                    }
                    /* fall through */

                case 's':
                    {
                        char  *dst   = NULL;
                        char **m_ptr = NULL;

                        if (!do_not_store) {
                            if (do_malloc) {
                                memset(s_buf, 0, sizeof(s_buf));
                                dst = s_buf;

                                m_ptr = (char **)(va_arg(ap, char **));
                                if (m_ptr == NULL)
                                    goto fail;
                            } else {
                                dst = (char *)(va_arg(ap, char *));
                                if (dst == NULL)
                                    goto fail;
                            }
                        }

                        /* this bit should apply to all 'read me some stuff' ? */

                        /* TODO this seems to fail for %s where the string is "" */

                        /* how many bytes for this string have we matched? */
                        sub_read = 0;
                        do {
                            int tmp;

                            if (is_file) {
                                if ((tmp = fgetc(stream)) == EOF)
                                    break;
                                chr_in = (char)tmp;
                            } else {
                                /* FIXME something before this is advancing to the end of
                                 * the string, but not exiting */
                                if (*src == '\0')
                                    break;

                                chr_in = *src++;
                            }

                            if (chr_in == '\0') {
                                break;
                            }

                            /* if we have read passed the end of the matchable string,
                             * back off a character */
                            if (    (chr_in == *format) ||
                                    ( do_scanset && !is_valid_scanset(scanset, chr_in, '^')) ||
                                    (!do_scanset && isspace(chr_in))
                               ) {

                                if (is_file) { ungetc(chr_in, stream); } else { src--; }
                                break;
                            }

                            if (!do_not_store)
                                *dst++ = chr_in;
                            sub_read++;

                            if ((str_limit && sub_read >= str_limit) || (do_malloc && sub_read >= BUFSIZ) || sub_read > 1000)
                                break;

                        } while(1);

                        if (!do_not_store)
                            *dst = '\0';

                        do_scanset = false;

                        if (!do_not_store && do_malloc && sub_read) {
                            if ((*m_ptr = strdup(s_buf)) == NULL)
                                goto fail;

                            bytes_scanned++;
                        } else if (!do_not_store && do_malloc && !sub_read) {
                            *m_ptr = NULL;
                        } else
                            bytes_scanned++;
                    }
                    break; /* case 's' */
            } /* switch(c) */
        }
    } /* while ((c = *format++) != 0) */


fail:
    rc = bytes_scanned;

    return rc;
}

    __attribute__ ((access (write_only, 1), access (read_only, 4)))
static int vxnprintf(char *restrict dst, FILE *restrict stream, size_t size, const char *restrict format, va_list ap)
{
    char c;
    const char *p = NULL;
    char buf[64] = {0};
    ssize_t i = 0, l = 0;
    size_t off = 0, wrote = 0, remainder = 0;
    const bool is_file   = stream ? true : false;
    const bool is_string = dst    ? true : false;

    if (format == NULL)// || (dst == NULL && stream == NULL))
        return -1;

    //memset(buf2, '0', 63);
    //memset(buf,  '0', 63);

    while ((c = *format++) != 0 && (size == 0 || off < size))
    {
        if (is_string)
            dst[off] = '\0';

        if (is_file && (feof(stream) || ferror(stream)))
            return -1;

        if (c!= '%') {
            const char *tformat = format - 1;
            while (*tformat && *tformat != '%') tformat++;
            ssize_t tlen = tformat - (format - 1);
            tlen = size ? min(size-off, tlen) : tlen;

            if (is_file) {
                if (fwrite(format-1, tlen, 1, stream) != 1)
                    return -1;
            } else if (is_string)
                memcpy(dst + off, format-1, tlen);

            off   += tlen;
            wrote += tlen;
            format = tformat;
        } else {

            /* %[$][flags][width][.precision][length modifier]conversion */

            int     lenmod_size = _INT;
            ssize_t field_width = 0;
            ssize_t buflen = 0;
            ssize_t precision = 0;

            bool zero_pad  = false;
            bool done_flag = false;
            bool done_fwid = false;
            bool done_prec = false;
            bool has_prec  = false;
            bool alternate_form = false;

            bool printed = false;

            justify_t justify = JUSTIFY_RIGHT;

next:
            c = *format++;
            p = buf;

            if (done_prec)
                goto skip_prec;

            if (done_fwid)
                goto skip_fwid;

            if (done_flag)
                goto skip_flag;

            switch (c)
            {
                case '#':
                    alternate_form = true;
                    goto next;
                case '-':
                    justify = JUSTIFY_LEFT;
                    zero_pad = false;
                    goto next;
                case '0':
                    if (justify != JUSTIFY_LEFT) {
                        zero_pad = true;
                        justify = JUSTIFY_RIGHT;
                    }
                    goto next;
            }

            done_flag = true;
skip_flag:
            if (c == '*' && field_width) {
                errno = EINVAL;
                return -1;
            } else if (c == '*') {
                field_width = (unsigned int)va_arg(ap, unsigned int);
            } else if (isdigit((unsigned char)c)) {
                field_width *= 10;
                field_width += c - '0';
                goto next;
            }

            done_fwid = true;
skip_fwid:

            if (c == '.') {
                has_prec  = true;
                goto next;
            } else if (!has_prec) {
                done_prec = true;
                goto skip_prec;
            } else if (isdigit(c)) {
                precision *= 10;
                precision += c - '0';
                goto next;
            }

            done_prec = true;

skip_prec:
            switch (c)
            {
                case '%':
                    goto chr;

                case 'p':
                    lenmod_size = _LONG;
                    c = 'x';
                    itoa(buf,c,(uintptr_t)va_arg(ap, void *), zero_pad, lenmod_size);
                    goto forcex;

                case 'h':
                    lenmod_size = (lenmod_size == _SHORT ? _CHAR : _SHORT);
                    goto next;

                case 'l':
                    lenmod_size = (lenmod_size == _LONG ? _LLONG : _LONG);
                    goto next;

                case 'j':
                    lenmod_size = _INT;
                    goto next;

                case 'z':
                    lenmod_size = _LONG;
                    goto next;

                case 't':
                    lenmod_size = _LONG;
                    goto next;

                case 'u':
                case 'x':
                case 'X': /* TODO upper case [A-F] */
                    {
                        int shift = 0;
                        if (alternate_form && (c == 'x' || c == 'X')) {
                            sprintf(buf, "0x");
                            shift += 2;
                        }

                        switch(lenmod_size) {
                            case _CHAR:
                                itoa(buf+shift,c,(unsigned long)va_arg(ap, unsigned int), zero_pad, lenmod_size);
                                break;
                            case _SHORT:
                                itoa(buf+shift,c,(unsigned long)va_arg(ap, unsigned int), zero_pad, lenmod_size);
                                break;
                            case _INT:
                                itoa(buf+shift,c,(unsigned long)va_arg(ap, unsigned int), zero_pad, lenmod_size);
                                break;
                            case _LONG:
                                itoa(buf+shift,c,(unsigned long)va_arg(ap, unsigned long), zero_pad, lenmod_size);
                                break;
                            case _LLONG:
                                errno = ENOSYS;
                                return -1;
                        }
forcex:
                        precision = 0;

                        goto padcheck;
                    }
                case 'i':
                case 'd':
                    switch(lenmod_size) {
                        case _CHAR:
                            itoa(buf,c,(unsigned long)va_arg(ap, int), zero_pad, lenmod_size);
                            break;
                        case _SHORT:
                            itoa(buf,c,(unsigned long)va_arg(ap, int), zero_pad, lenmod_size);
                            break;
                        case _INT:
                            itoa(buf,c,(unsigned long)va_arg(ap, int), zero_pad, lenmod_size);
                            break;
                        case _LONG:
                            itoa(buf,c,(unsigned long)va_arg(ap, long), zero_pad, lenmod_size);
                            break;
                        case _LLONG:
                            errno = ENOSYS;
                            return -1;
                            break;
                    }

                    /* TODO somewhere around here need to handle alternate_form */

                    precision = 0;
                    //fputs("\nvxnprintf: entered pre-padcheck", stdout);
padcheck:
                    //fputs("\nvxnprintf: entered padcheck", stdout);
                    if (justify == JUSTIFY_RIGHT) {
leftpadcheck:
                        //fputs("\nvxnprintf: entered leftpadcheck with: ", stdout);
                        //fputs(p, stdout);
                        //fputs("\n", stdout);
                        buflen = strlen(p);
                        char pad_chr = zero_pad ? '0' : ' ';
                        /*
                           char tmp[64];
                           putchar('"'); puts(p); putchar('"');
                           puts(" field_width="); itoa(tmp,'d',field_width,false,8); puts(tmp);
                           puts(" tmplen="); itoa(tmp,'d',buflen,false,8); puts(tmp);
                           putchar('\n');
                           */

                        /* TODO handle precision/field_width == 0 but zero_pad, so
                         * need to use lenmod_size is the default */

                        for (i = 0, l = field_width - buflen; l && (i < l) && (!size || off < size); i++, off++, wrote++) {
                            if (is_file)
                                putc(pad_chr, stream);
                            else if (is_string)
                                dst[off] = pad_chr;
                        }
                        zero_pad = false;
                        justify = JUSTIFY_RIGHT;
                    } else {
                        //fputs("\nvxnprintf: entered non-leftpadcheck with: ", stdout);
                        //fputs(p, stdout);
                        //fputs("\n", stdout);
                    }

                    lenmod_size = _INT;
                    if (!printed) {
                        //fputs("vxnprintf: not-printed\n", stdout);
                        goto string;
                    }
                    break;

                case 's':
                    p = va_arg(ap, const char *);
                    goto padcheck;
string:
                    if (printed)
                        break;

                    printed = true;

                    /* handle the case string is NULL */
                    int plen = (p != NULL) ? strlen(p) : 6;

                    if (size) {
                        remainder = size - off;
                        remainder = (precision>0) ? (size_t)min((size_t)precision, remainder) : remainder;
                        remainder = min(plen, remainder);
                    } else
                        remainder = (precision>0) ? min(precision, plen) : plen;

                    if (p == NULL) {
                        /* handle the case our string is a NULL pointer */
                        if (is_file) {
                            if (fwrite("(null)", remainder, 1, stream) < 1)
                                return -1;
                        } else if (is_string) {
                            strncat(dst + off, "(null)", remainder);
                        }
                        off   += remainder;
                        wrote += remainder;
                        if (justify == JUSTIFY_LEFT)
                            goto leftpadcheck;
                    } else {
                        /*
                           char tmpb[64];
                           puts("strlen(p)="); itoa(tmpb,'d',strlen(p),false,8); puts(tmpb);
                           puts(" size="); itoa(tmpb,'d',size,false,8); puts(tmpb);
                           puts(" off="); itoa(tmpb,'d',off,false,8); puts(tmpb);
                           puts(" precision="); itoa(tmpb,'d',precision,false,8); puts(tmpb);
                           puts(" remainder="); itoa(tmpb,'d',remainder,false,8); puts(tmpb);
                           putchar('\n');
                           */

                        /* check allows for "%s", "" empty strings */
                        if (remainder) {
                            if (is_file) {
                                if (fwrite(p, remainder, 1, stream) < 1)
                                    return -1;
                            } else if (is_string) {
                                strncat(dst + off, p, remainder);
                            }
                        }
                        off   += remainder; /* TODO does this put off > size ? */
                        wrote += remainder;
                        if (justify == JUSTIFY_LEFT)
                            goto leftpadcheck;
                    }
                    break;

                case 'c':
                    c = va_arg(ap, int);
chr:
                    if (is_file) {
                        if (fputc(c, stream) == EOF) //isprint(c) ? c : ' ', stream);
                        return -1;
                    } else if (is_string) {
                        dst[off] = c;//isprint(c) ? c : ' ';
                    }

                    off++;
                    wrote++;
                    break;

                case 'n':
                    *(va_arg(ap, int *)) = wrote;
                    break;

                default:
                    errno = ENOSYS;
                    return -1;
            }
        }
    }

    if (is_string) {
        if (off == size)
            dst[off] = '\0';
        else {
            dst[off++] = '\0';
        }

        /* this looks like it might be off-by-1 in the case above ? FIXME */

        return off;
    }

    return wrote;
}

#undef _LONG
#undef _SHORT
#undef _INT
#undef _LLONG
#undef _CHAR

int fcntl(int fd, int cmd, ...)
{
    unsigned long arg;
    va_list ap;

    va_start(ap, cmd);
    arg = va_arg(ap, unsigned long);
    va_end(ap);

    return syscall(__NR_fcntl, fd, cmd, arg);
}

    __attribute__((nonnull(1), access(read_only, 1), access(write_only, 2)))
static int parse_fopen_flags(const char *mode, bool *seek_end)
{
    int want_flags = 0;
    if (seek_end)
        *seek_end  = false;

    if      (!strncmp(mode, "r+", 2)) want_flags = O_RDWR;
    else if (!strncmp(mode, "w+", 2)) want_flags = O_RDWR|O_TRUNC|O_CREAT;
    else if (!strncmp(mode, "a+", 2)) want_flags = O_RDWR|O_CREAT;
    else if (*mode == 'r')            want_flags = O_RDONLY;
    else if (*mode == 'w')            want_flags = O_WRONLY|O_TRUNC|O_CREAT;
    else if (*mode == 'a')           {want_flags = O_WRONLY|O_CREAT; if (seek_end) *seek_end = true;}

    return want_flags;
}

FILE *fdopen(int fd, const char *mode)
{
    if (!mode || fd < 0) {
        return NULL;
    }

    FILE *ret = calloc(1, sizeof(FILE));
    if (ret == NULL) {
        return NULL;
    }
    ret->fd = fd;

    int flags;

    if ((flags = fcntl(fd, F_GETFL)) == -1)
        goto fail;

    bool seek_end  = false;
    const int want_flags = parse_fopen_flags(mode, &seek_end);

    if (fcntl(fd, F_SETFL, want_flags) == -1)
        goto fail;

    if (seek_end)
        fseek(ret, 0, SEEK_END);

    return ret;
fail:
    if (ret)
        free(ret);

    return NULL;
}

int isdigit(int c)
{
    if (c >= '0' && c <= '9') return true;
    return false;
}

int isxdigit(int c)
{
    if (isdigit(c))
        return true;
    if (c >= 'a' && c <= 'f')
        return true;
    if (c >= 'A' && c <= 'F')
        return true;

    return false;
}

static const char *const posix_punct="!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

int ispunct(int c)
{
    if (strchr(posix_punct, c) != NULL) return true;

    return false;
}

int isalnum(int c)
{
    if (isalpha(c)) return true;
    if (isdigit(c)) return true;

    return false;
}

int isblank(int c)
{
    switch(c)
    {
        case ' ':
        case '\t':
            return true;
    }

    return false;
}

int iscntrl(int c)
{
    if (c < 0x20) return true;
    if (c == 0x7f) return true;
    return false;
}

int isprint(int c)
{
    return !iscntrl(c);
}

int isgraph(int c)
{
    if (isalnum(c)) return true;
    if (ispunct(c)) return true;

    return false;
}

int isalpha(int c)
{
    if (isupper(c)) return true;
    if (islower(c)) return true;

    return false;
}

int isupper(int c)
{
    if (c >= 'A' && c <= 'Z') return true;

    return false;
}

int islower(int c)
{
    if (c >= 'a' && c <= 'z') return true;

    return false;
}

int isascii(int c)
{
    if (c <= 0x7f && c >= 0x00) return true;

    return false;
}

int isspace(int c)
{
    switch(c)
    {
        case ' ':
        case '\f':
        case '\n':
        case '\r':
        case '\t':
        case '\v':
            return true;
        default:
            return false;
    }
}

int tolower(int c)
{
    if (!isupper(c)) return c;

    return(c - ('a' - 'A'));
}

int toupper(int c)
{
    if (!islower(c)) return c;

    return(c + ('a' - 'A'));
}

int ferror(FILE *stream)
{
    return(stream->error != 0);
}

int feof(FILE *stream)
{
    return(stream->eof);
}

FILE *fopen(const char *pathname, const char *modestr)
{
    int mode = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH;
    bool seek_end;

    if (!pathname || !modestr)
        return NULL;

    const int flags = parse_fopen_flags(modestr, &seek_end);
    int fd = open(pathname, flags, mode);
    if (fd < 0)
        goto fail;

    FILE *ret;
    if ((ret = calloc(1, sizeof(FILE))) == NULL)
        goto fail_close;

    if ((ret->buf = calloc(1, BUFSIZ)) == NULL)
        goto fail_free;

    if (isatty(fd))
        ret->buf_mode = _IOLBF;
    else
        ret->buf_mode = _IOFBF;

    ret->bpos = 0;
    ret->bhas = 0;
    ret->blen = BUFSIZ;
    ret->fd = fd;
    ret->flags = flags;
    return ret;

fail_free:
    free(ret);
fail_close:
    close(fd);
fail:
    return NULL;
}

[[maybe_unused]] static void dump_fd(const FILE *fd)
{
    printf("fd @ %p\n"
            "flags: %x\n"
            "buf_mode: %x\n"
            "bpos: %x\n"
            "bhas: %x\n"
            "mem: %p\n"
            "mem_size: %lx\n"
            "offset: %lx\n",
            fd,
            fd->flags,
            fd->buf_mode,
            fd->bpos,
            fd->bhas,
            fd->mem,
            fd->mem_size,
            fd->offset);
}

FILE *fmemopen(void *buf, size_t size, const char *mode)
{
    errno = ENOMEM;

    if (buf == NULL)
        errno = EINVAL;

    FILE *ret;
    bool seek_end;

    if ((ret = calloc(1, sizeof(FILE))) == NULL)
        return NULL;

    //if ((ret->buf = calloc(1, BUFSIZ)) == NULL)
    //    goto fail_free;

    ret->flags = parse_fopen_flags(mode, &seek_end);
    ret->buf_mode = _IOFBF;
    ret->bpos = 0;
    ret->bhas = 0;
    //ret->blen = BUFSIZ;
    ret->mem = buf;
    ret->mem_size = size;
    ret->offset = 0;
    ret->fd = -1;

    //dump_fd(ret);

    return ret;

    //fail_free:
    //    free(ret);
    //    return NULL;
}

DIR *opendir(const char *dirname)
{
    DIR *ret;

    if (dirname == NULL)
        return NULL;

    int fd = open(dirname, O_RDONLY|O_DIRECTORY, 0);
    if (fd < 0)
        return NULL;

    if ((ret = calloc(1, sizeof(DIR))) == NULL) {
        close(fd);
        return NULL;
    }

    ret->fd = fd;
    ret->idx = NULL;
    ret->end = (struct dirent *)(ret->buf + sizeof(ret->buf));

    return ret;
}

ssize_t getdents64(int fd, void *dirp, size_t count)
{
    return (ssize_t)syscall(__NR_getdents64, fd, dirp, count);
}

struct dirent *readdir(DIR *dp)
{
    struct dirent *ret;

    if (dp == NULL) {
        errno = EBADF;
        return NULL;
    }

    errno = 0;
    ssize_t rc;

    if (dp->idx == NULL)
        goto get;

    if (dp->idx < dp->end && dp->idx->d_reclen) {
ok:
        ret = dp->idx;
        /*
        printf("readdir: d_ino: %lu d_off: %u d_reclen: %u d_type: %u: d_name: \"%s\"\n",
              ret->d_ino,
              ret->d_off,
              ret->d_reclen,
              ret->d_type,
              ret->d_name);*/
        dp->idx = (struct dirent *)(((char *)ret) + ret->d_reclen);
        return ret;
    }

get:
    //printf("readdir: getdents64()\n");
    rc = getdents64(dp->fd, dp->buf, sizeof(dp->buf));
    //printf("readdir: getdents64() returned %ld\n", rc);

    if (rc == -1) {
        //printf("readdir: rc<0\n");
        return NULL;
    } else if (rc == 0) {
        //printf("readdir: rc==0\n");
        return NULL;
    } else {
        dp->idx = (struct dirent *)dp->buf;
        //printf("readdir: rc=%d d_reclen:%d\n",
          //    rc,
            //  dp->idx->d_reclen);
        dp->end = (void *)(dp->buf + rc);
        goto ok;
    }
}

int closedir(DIR *dir)
{
    if (close(dir->fd) == -1)
        return -1;

    free(dir);

    return 0;
}

size_t strlen(const char *s)
{
    register const char *t = s;

    if (s == NULL)
        return 0;

    while (*t != '\0')
        t++;

    return t - s;
}

ssize_t getline(char **restrict lineptr, size_t *restrict n, FILE *restrict stream)
{
    return getdelim(lineptr, n, '\n', stream);
}

ssize_t getdelim(char **restrict lineptr, size_t *restrict n, int delimiter, FILE *restrict stream)
{
    if (!lineptr || !n || !stream) {
        errno = EINVAL;
        return -1;
    }

    /*
       printf("getdelim(%p[%p], %p[%lu], %x, %p)\n",
       lineptr,
       lineptr ? *lineptr : 0,
       n,
       n ? *n : 0,
       delimiter,
       stream);
       */

    char buf[LINE_MAX] = {0};

    if (fgets_delim(buf, sizeof(buf), stream, delimiter) == NULL)
        return ferror(stream) ? -1 : 0;

    size_t len = strlen(buf);

    if (*lineptr == NULL) {
        *lineptr = strdup(buf);
        *n = len;
        if (*lineptr == NULL)
            return -1;
    } else {
        if (*n < len) {
            *lineptr = realloc(*lineptr, len + 1);
            if (*lineptr == NULL)
                return -1;
        }
        strcpy(*lineptr, buf);
        *n = len;
    }

    return len;
}

int setsockopt(int fd, int level, int name, const void *value, socklen_t len)
{
    return syscall(__NR_setsockopt, fd, level, name, value, len);
}

int getsockopt(int fd, int level, int name, void *restrict value, socklen_t *restrict len)
{
    return syscall(__NR_getsockopt, fd, level, name, value, len);
}

int accept(int fd, struct sockaddr *restrict addr, socklen_t *restrict addrlen)
{
    return syscall(__NR_accept, fd, addr, addrlen);
}

int connect(int fd, const struct sockaddr *address, socklen_t len)
{
    return syscall(__NR_connect, fd, address, len);
}

int listen(int fd, int backlog)
{
    return syscall(__NR_listen, fd, backlog);
}

int shutdown(int sockfd, int how)
{
    return syscall(__NR_shutdown, sockfd, how);
}

int socket(int domain, int type, int proto)
{
    return syscall(__NR_socket, domain, type, proto);
}

int bind(int fd, const struct sockaddr *saddr, socklen_t len)
{
    return syscall(__NR_bind, fd, saddr, len);
}

int abs(int i)
{
    return (i > 0) ? i : -i;
}

int pipe(int pipefd[2])
{
    return syscall(__NR_pipe, pipefd);
}

int poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
    return syscall(__NR_poll, fds, nfds, timeout);
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
    return syscall(__NR_select, nfds, readfds, writefds, exceptfds, timeout);
}

int gettimeofday(struct timeval *tv, void *tz)
{
    return syscall(__NR_gettimeofday, (long)tv, (long)tz, 0, 0, 0, 0, 0);
}

time_t time(time_t *tloc)
{
    return syscall(__NR_time, tloc);
}

int setvbuf(FILE *, char *, int , size_t )
{
    return 0;
}

static bool is_valid_locale(const char *locale)
{
    if (!strlen(locale)) return true;
    else if (!strcasecmp("C", locale)) return true;
    else if (!strcasecmp("POSIX", locale)) return true;

    return false;
}

locale_t newlocale(int category_mask, const char *locale, locale_t base)
{
    struct locale_t *ret = base;

    if (locale == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (~LC_ALL_MASK & category_mask) {
        errno = EINVAL;
        return NULL;
    }

    if (!is_valid_locale(locale)) {
        errno = ENOENT;
        return NULL;
    }

    if (ret == NULL && (ret = calloc(1, sizeof(struct locale_t))) == NULL)
        return NULL;

    ret->mask = category_mask;

    for (int i = 0; i < LC_ALL; i++) {
        if (category_mask & (1 << i)) {
            if ((ret->locales[i+1] = strdup(locale)) == NULL)
                goto fail;
        } else {
            if ((ret->locales[i+1] = strdup("C")) == NULL)
                goto fail;
        }
    }

    return ret;

fail:
    if (ret)
        freelocale(ret);

    return NULL;
}

void freelocale(locale_t locobj)
{
    struct locale_t *ret = locobj;

    for (int i = 0; i < LC_ALL + 2; i++)
        if (ret->locales[i])
            free(ret->locales[i]);

    free(locobj);
}

char *setlocale(int category, const char *locale)
{
    switch (category) {
        case LC_ALL:
        case LC_COLLATE:
        case LC_CTYPE:
        case LC_MESSAGES:
        case LC_MONETARY:
        case LC_NUMERIC:
        case LC_TIME:
            break;
        default:
            return NULL;
    }

    if (locale == NULL) {
done:
        return (char *)current_locale[category]->name;

    } else if (!strlen(locale)) {
        current_locale[category] = &def_locale;
        goto done;

    } else if (!strcasecmp("C", locale) || !strcasecmp("POSIX", locale)) {
        current_locale[category] = &def_locale;
        goto done;

    }

    return NULL;
}

int nanosleep(const struct timespec *req, struct timespec *rem)
{
    return syscall(__NR_nanosleep, req, rem);
}

static const char *const wday_name[7] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char *const wday_name_long[7] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

static const char *const mon_name[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static const char *const mon_names_long[12] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

static const char *const ampm_name[2] = {
    "am", "pm"
};

#define int_process(min, max, adjust, target) \
    if (!isdigit(*src_ptr)) \
    goto done; \
    \
    sscanf(src_ptr, "%u", &tmp_int); \
    \
    if (tmp_int < (min) || tmp_int > (max)) \
    goto done; \
    \
    target = tmp_int - (adjust); \
    \
    while(isdigit(*src_ptr)) src_ptr++; \

char *strptime(const char *restrict s, const char *restrict format, struct tm*restrict tm)
{
    const char *src_ptr = s;
    const char *fmt_ptr = format;
    bool found = false;
    int century = -1;
    int year2 = -1;
    int ampm = -1;
    int hours = -1;

    while (*fmt_ptr)
    {
        if (!*src_ptr) {
            if (found)
                goto done;
            else {
                /* TODO set found somewhere? */
                return NULL;
            }
        }

        if (isspace(*fmt_ptr)) {
            while (isspace(*src_ptr))
                src_ptr++;
            goto next_fmt;
        }

        if (*fmt_ptr != '%') {
again:
            if (*fmt_ptr == '%')
                continue;
percent:
            if (*fmt_ptr != *src_ptr)
                goto done;

            fmt_ptr++;
            src_ptr++;
            goto again;
        }

        /* must be % */

        int tmp_int;

        switch(*(++fmt_ptr))
        {
            case 'Z':
                while(isalpha(*src_ptr))
                    src_ptr++;
                break;

            case '\0':
                errno = EINVAL;
                return NULL;

            case 'w':
                int_process(0,6,0,tm->tm_wday);
                break;

            case 'a':
            case 'A':
                for (int i = 0; i < 7; i++)
                    if (!strncasecmp(src_ptr, wday_name[i], 3)) {
                        tm->tm_wday = i;
                        src_ptr += 3;
                        goto next_fmt;
                    }
                for (int i = 0; i < 7; i++)
                    if (!strncasecmp(src_ptr, wday_name_long[i], strlen(wday_name_long[i]))) {
                        tm->tm_wday = i;
                        src_ptr += 3;
                        goto next_fmt;
                    }
                goto done;

            case 'p':
                if (!strncasecmp(src_ptr, ampm_name[0], 2))
                    ampm = 1;
                else if (!strncasecmp(src_ptr, ampm_name[1], 2))
                    ampm = 2;
                else
                    goto done;
                src_ptr += 2;
                break;

            case 'b':
            case 'B':
            case 'h':
                for (int i = 0; i < 12; i++)
                    if (!strncasecmp(src_ptr, mon_name[i], 3)) {
                        tm->tm_mon = i;
                        src_ptr += 3;
                        goto next_fmt;
                    }
                for (int i = 0; i < 12; i++)
                    if (!strncasecmp(src_ptr, mon_names_long[i], strlen(mon_names_long[i]))) {
                        tm->tm_mon = i;
                        src_ptr += 3;
                        goto next_fmt;
                    }
                goto done;

            case 'C':
                int_process(0,99,0,century);
                break;

            case 'd':
            case 'e':
                int_process(1,31,0,tm->tm_mday);
                break;

            case 'I':
                hours = 12;
                int_process(1,12,0,tm->tm_hour);
                break;

            case 'H':
                hours = 24;
                int_process(0,23,0,tm->tm_hour);
                break;

            case 'm':
                int_process(1,12,1,tm->tm_mon);
                break;

            case 'M':
                int_process(0,59,0,tm->tm_min);
                break;

            case 'S':
                int_process(0,60,0,tm->tm_sec);
                break;

            case 'T':
            case 'R':
                hours = 24;
                int_process(0,23,0,tm->tm_hour);
                if (*src_ptr != ':')
                    goto done;
                src_ptr++;
                int_process(0,59,0,tm->tm_min);

                if (*fmt_ptr == 'R')
                    break;

                if (*src_ptr != ':')
                    goto done;
                src_ptr++;
                int_process(0,60,0,tm->tm_sec);
                break;

            case 'j':
                int_process(1,366,0,tm->tm_yday);
                break;

            case 'U': /* week number, starting Sunday */
            case 'W': /* week number, starting Monday */
                /* FIXME what to do ? */
                errno = ENOSYS;
                break;

            case 't':
                if (isspace(*src_ptr))
                    src_ptr++;
                else
                    goto done;
                break;

            case 'y':
                int_process(0,99,0,year2);
                break;

            case 'Y':
                int_process(0,9999,0,tm->tm_year);
                tm->tm_year -= 1900;
                break;

            case '%':
                goto percent;

            default:
                warnx("unknown fmt %%%c", *fmt_ptr);
                return NULL;
        }

next_fmt:
        fmt_ptr++;
    }

done:
    if (hours == 12 && ampm == 2)
        tm->tm_hour += 12;

    if (century == -1) {
        tm->tm_year = year2;
        if (tm->tm_year <= 68)
            tm->tm_year += 100;
    } else {
        tm->tm_year = (century * 1000) - 1900;
        if (year2 != -1)
            tm->tm_year += year2;
    }
    /* TODO calculate yday etc. if not provided? */
    return (char *)src_ptr;
}


size_t strftime(char *restrict s, size_t max, const char *restrict fmt, const struct tm *restrict tm)
{
    const char *src = fmt;
    char *dst = s, *end = (s + max);

    //printf("strftime: fmt=<%s> max=%d\n", fmt, max);

    while (dst < (s + max) && *src)
    {
        if (*src == '%') {
            if (*++src == 0) {
                return -1;
            }

            int remain = end - dst;
            int add = 0;

            switch(*src) {
                case 'a':
                    add = snprintf(dst, remain, "%s", wday_name[tm->tm_wday]);
                    break;
                case 'b':
                    add = snprintf(dst, remain, "%s", mon_name[tm->tm_mon]);
                    break;
                case 'e':
                    add = snprintf(dst, remain, "%02u", tm->tm_mday);
                    break;
                case 'H':
                    add = snprintf(dst, remain, "%02u", tm->tm_hour);
                    break;
                case 'M':
                    add = snprintf(dst, remain, "%02u", tm->tm_min);
                    break;
                case 'S':
                    add = snprintf(dst, remain, "%02u", tm->tm_sec);
                    break;
                case 'Z':
                    add = snprintf(dst, remain, "UTC");
                    break;
                case 'Y':
                    add = snprintf(dst, remain, "%4u", tm->tm_year + 1900);
                    break;
                case 'z':
                    add = snprintf(dst, remain, "+0000"); /* TODO */
                    break;
                case 'F':
                    add = snprintf(dst, remain, "%04u-%02u-%02u", tm->tm_year, tm->tm_mon, tm->tm_mday);
                    break;
                case 'c':
                    add = snprintf(dst, remain, "%4u-%02u-%02uT%02u:%02u:%02u%s",
                            tm->tm_year + 1900, tm->tm_mon, tm->tm_mday,
                            tm->tm_hour, tm->tm_min, tm->tm_sec,
                            "+0000");
                    break;
                default:
                    printf("UNKNOWN: %c\n", *src);
            }

            //printf("add=%d\n", add);

            dst += add - 1;
            src++;
        } else {
            *dst++ = *src++;
        }
    }

    return (dst-s);
}

/* TODO buffering? */
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t ret;
    const char *tmp_ptr = ptr;
    ssize_t res;

    if (ptr == NULL || stream == NULL || size == 0 || nmemb == 0)
        return 0;

    for (ret = 0; ret < nmemb; ret++)
    {
        if ( stream->mem ) {
            /* TODO */
            stream->error = ENOSYS;
            return ret;
        } else if ( (res = write(stream->fd, tmp_ptr, size)) != (ssize_t)size ) {
            if (res >= 0)
                stream->eof = true;
            else
                stream->error = errno;
            return ret;
        }
        tmp_ptr += size;
    }

    return ret;
}

#if 0
//static size_t _fread(void *ptr, size_t size, FILE *stream)
//{
//  size_t rem = size;
//  char *dst = ptr;
//
//  while (rem)
//  {
//      if (!stream->blen) {
//      }
//
//      memcpy(dst, stream->fd
//
//              }
//              }
#endif

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t nmemb_read;
    char *tmp_ptr = ptr;
    ssize_t res;
    ssize_t to_read;
    ssize_t tmp;
    size_t mem_left;

    if (ptr == NULL || stream == NULL || size == 0 || nmemb == 0)
        return 0;

    for (nmemb_read = 0; nmemb_read < nmemb; nmemb_read++)
    {
        to_read = size;

        if ( stream->has_unwind ) {
            *tmp_ptr++ = stream->unwind;
            stream->has_unwind = false;
            to_read--;
        }

        if (!to_read) {
            goto done;
        }
more:
        mem_left = stream->mem_size - stream->offset;

        /* process buffer first (if any) */
        if (stream->buf && stream->blen) {
            /* get the most we can, that we need, from the buffer */
            tmp = min(to_read, stream->bhas - stream->bpos);

            /* if the buffer has any, deliver it */
            if (tmp > 0) {
                memcpy(tmp_ptr, stream->buf + stream->bpos, tmp);
                tmp_ptr += tmp;
                to_read -= tmp;
                stream->bpos += tmp;
            }

            /* have we exhausted the buffer ? */
            if (stream->bpos >= stream->bhas) {
                /* res will contain the actual bytes read into the buffer*/
                res = 0;

                /* handle fmemopen() */
                if (stream->mem) {
                    size_t mem_read = stream->blen;

                    if (mem_read > mem_left) {
                        stream->eof = 1;
                        if (mem_left == 0)
                            return 0;
                        mem_read = mem_left;
                    }

                    memcpy(stream->buf, (void *)((uintptr_t)stream->mem + (uintptr_t)stream->offset), mem_read);
                    stream->offset += mem_read;
                    mem_left = stream->mem_size - stream->offset;

                    res = mem_read;

                } else if (stream->fd != -1) { /* handle fopen() */
                    if ((res = read(stream->fd, stream->buf, stream->blen)) <= 0) {
                        if (res == -1)
                            stream->error = 1;
                        else
                            stream->eof = 1;
                        return nmemb_read;
                    }
                }

                stream->bpos = 0;
                stream->bhas = res;
            }

            /* have we read a full record? */
            if (to_read)
                goto more;

            continue;
        } /* else, no buffering */

        /* handle fmemopen() */
        if (stream->mem) {
            size_t mem_read = to_read;

            if (mem_read > mem_left) {
                stream->eof = 1;
                if (mem_left == 0)
                    return 0;
                mem_read = mem_left;
            }

            memcpy(tmp_ptr, (void *)((uintptr_t)stream->mem + (uintptr_t)stream->offset), mem_read);
            stream->offset += mem_read;
            mem_left = stream->mem_size - stream->offset;

            if (mem_read != (size_t)to_read)
                return nmemb_read; /* FIXME is this correct for nmemb ? */

        } else if (stream->fd != -1) { /* handle fopen() */
            if ((res = read(stream->fd, tmp_ptr, to_read)) != to_read) {
                if (res == 0)
                    stream->eof = true;
                else
                    stream->error = errno;
                return nmemb_read;
            }
        }

done:
        tmp_ptr += to_read;
    }

    return nmemb_read;
}

int fflush(FILE *stream)
{
    if (stream == NULL) {
        errno = EINVAL;
        return -1;
    }
    return 0;
}

int fputs(const char *s, FILE *stream)
{
    return fwrite(s, strlen(s), 1, stream);
}

int puts(const char *s)
{
    if (fputs(s, stdout) == EOF)
        return EOF;
    if (putc('\n', stdout) == EOF)
        return EOF;

    return 0;
}

char *fgets(char *restrict s, int size, FILE *restrict stream)
{
    if (!s || !stream)
        return NULL;

    return fgets_delim(s, size, stream, '\n');
}

int statvfs(const char *restrict path, struct statvfs *restrict buf)
{
    return syscall(__NR_statfs, path, buf);
}

int fstatvfs(int fd, struct statvfs *buf)
{
    return syscall(__NR_fstatfs, fd, buf);
}

int fgetc(FILE *stream)
{
    char ch;

    if ( fread(&ch, 1, 1, stream) != 1 )
        return EOF;

    return (int)(ch);
}

int ungetc(int c, FILE *stream)
{
    if (!stream)
        return EOF;

    stream->has_unwind = true;
    stream->unwind = c;

    return c;
}

int getc(FILE *stream)
{
    return fgetc(stream);
}

int getchar(void)
{
    return getc(stdin);
}

char *strcat(char *dest, const char *src)
{
    if (dest == NULL || src == NULL) return dest;

    size_t dest_len,i;
    dest_len = strlen(dest);

    for (i = 0; src[i] != '\0'; i++)
        dest[dest_len + i] = src[i];

    dest[dest_len + i] = '\0';

    return dest;
}

char *strncat(char *dest, const char *src, size_t n)
{
    if (dest == NULL || src == NULL) return dest;

    size_t i, dest_len;

    dest_len = strlen(dest);

    for (i = 0 ; i < n && src[i] != '\0' ; i++)
        dest[dest_len + i] = src[i];

    dest[dest_len + i] = '\0';

    return dest;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    if (s1 == NULL || s2 == NULL) return 0;

    size_t i = 0;
    while (i < n)
    {
        if (s1[i] != s2[i]) return 1;
        if (s1[i] == '\0') break;
        i++;
    }
    return 0;
}

int strcmp(const char *s1, const char *s2)
{
    if (s1 == NULL || s2 == NULL) return 0;

    size_t i = 0;
    while (true)
    {
        if (s1[i] != s2[i]) return 1;
        if (s1[i] == '\0' || s2[i] == '\0') break;
        i++;
    }
    return 0;
}

int strcasecmp(const char *s1, const char *s2)
{
    if (s1 == NULL || s2 == NULL) return 0;

    size_t i = 0;

    while (true)
    {
        if (tolower(s1[i]) != tolower(s2[i])) return 1;
        if (s1[i] == '\0' || s2[i] == '\0') break;
        i++;
    }

    return 0;
}

int strncasecmp(const char *s1, const char *s2, size_t n)
{
    if (s1 == NULL || s2 == NULL) return 0;

    size_t i = 0;

    while (i < n)
    {
        if (tolower(s1[i]) != tolower(s2[i])) return 1;
        if (s1[i] == '\0' || s2[i] == '\0') break;
        i++;
    }

    return 0;
}

char *strstr(const char *heystack, const char *needle)
{
    if (heystack == NULL || needle == NULL) return NULL;

    const char *ret = heystack;
    size_t len = strlen(needle);

    while (*ret)
    {
        if (*ret != *needle) {
            ret++;
            continue;
        }

        if (!strncmp(needle, ret, len))
            return (char *)ret;

        ret++;
    }

    return NULL;
}

void free(void *ptr)
{
#ifdef VALGRIND
    VALGRIND_FREELIKE_BLOCK(ptr, 0);
#endif
    if (ptr == NULL)
        return;

    struct mem_alloc *buf = (struct mem_alloc *)((char *)ptr - sizeof(struct mem_alloc));
    
#ifdef VALGRIND
    VALGRIND_MAKE_MEM_DEFINED(buf, sizeof(struct mem_alloc));
#endif

    if (buf < first || buf > last) {
        exit(100);
    }
    if (buf->magic != MEM_MAGIC) {
        exit(101);
    }
    if ((buf->flags & MF_FREE) == 1) {
        exit(102);
    }

    free_alloc(buf);

#ifdef VALGRIND
    VALGRIND_MAKE_MEM_NOACCESS(buf, sizeof(struct mem_alloc));
#endif
}

    __attribute__((malloc))
void *malloc(size_t size)
{
    void *ret_ptr;

    if (size == 0)
        return NULL;

    struct mem_alloc *ret = NULL;

    if ((ret = alloc_mem(size)) == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    ret_ptr = ((char *)ret->start) + sizeof(struct mem_alloc);
#ifdef VALGRIND
    VALGRIND_MAKE_MEM_NOACCESS(ret, sizeof(struct mem_alloc));
    VALGRIND_MALLOCLIKE_BLOCK(ret_ptr, size, 0, 0);
#endif
    return ret_ptr;
}

    __attribute__((malloc))
void *realloc(void *ptr, size_t size)
{
    if (ptr == NULL) {
        return malloc(size);
    }
    
    const struct mem_alloc *old;
    old = ptr - sizeof(struct mem_alloc);
    size_t old_size;
#ifdef VALGRIND
    VALGRIND_MAKE_MEM_DEFINED(old, sizeof(struct mem_alloc));
#endif
    if (old->magic != MEM_MAGIC)
        errx(EXIT_FAILURE, "realloc: memory at %p missing magic", ptr);

    old_size = old->len - sizeof(struct mem_alloc);
    void *new;
    
    if ((new = malloc(size)) == NULL)
        goto done;

    memcpy(new, ptr, size < old_size ? size : old_size);
    free(ptr);
done:
#ifdef VALGRIND
    VALGRIND_MAKE_MEM_NOACCESS(old, sizeof(struct mem_alloc));
#endif
    return new;
}

#if 0
void *memset(void *s, int c, size_t n)
{
    for (size_t i = 0; i < n; i++)
        ((unsigned char *)s)[i] = (unsigned char)c;
    return s;
}
#endif

#if 0
void *memset(void *s, int _c, size_t _n)
{
    const char c = _c;
    register size_t n = _n;

    if (s == NULL) return s;

    register unsigned long long *restrict l_ptr;
    unsigned long long blah;
    char *s_ptr;

    if (n > sizeof(blah) ) {
        memset(&blah, (char)c, sizeof(blah));
        l_ptr = s;
        for (;n > sizeof(blah); n -= sizeof(blah))
            *(l_ptr++) = blah;

        s_ptr = (void *)l_ptr;
    } else
        s_ptr = s;


    for (size_t i = 0; i < n; i++)
        *(s_ptr++) = c;

    return s;
}
#endif

void *memset(void *s, int c, size_t n)
{
    //unsigned char *ptr = s;
    //const unsigned char *end = ptr + n;

    /* move to first aligned qword */
    //while (((uintptr_t)ptr % sizeof(uint64_t)) && ptr != end)
    //    *(ptr++) = c;

    //if (ptr == end)
    //    return ptr;

    const uint64_t byte = (uint8_t)c;
    //const uint64_t val = (byte<<56UL)|(byte<<48UL)|(byte<<40)|(byte<<32)|(byte<<24)|(byte<<16)|(byte<<8)|byte;

    //const size_t qwords = (end - ptr)/sizeof(uint64_t);

    __asm__( "cld; pushq %%rdi; pushq %%rcx; push %%rax; rep stosb; popq %%rax; popq %%rcx; popq %%rdi;"
            :: "D" (s), "a" (byte), "c" (n));

    //ptr += qwords * sizeof(uint64_t);

    /* fill in any remaining non-qwords */
    //while (ptr < end)
    //    *(ptr++) = c;

    return s;
}

    __attribute__((malloc))
void *calloc(size_t nmemb, size_t size)
{
    void *ret;
    size_t len = nmemb * size;

    if (len == 0)
        return NULL;

    if ((ret = malloc(len)) == NULL)
        return NULL;

    memset(ret, 0, len);
    return ret;
}

int fputc(int c, FILE *stream)
{
    unsigned char ch = c;
    return fwrite(&ch, 1, 1, stream);
}

int mkdir(const char *path, mode_t mode)
{
    return syscall(__NR_mkdir, path, mode);
}

int mknod(const char *pathname, mode_t mode, dev_t dev)
{
    return syscall(__NR_mknod, pathname, mode, dev);
}

pid_t setsid(void)
{
    return syscall(__NR_setsid);
}

int mkfifo(const char *path, mode_t mode)
{
    /* TODO & ~(something) for mode? */
    return mknod(path, S_IFIFO|mode, 0);
}

int dup(int oldfd)
{
    return syscall(__NR_dup, oldfd);
}

int putchar(int c)
{
    return putc(c, stdout);
}

int putchar_unlocked(int c)
{
    return(putchar(c));
}

int atexit(void (*function)(void))
{
    if (function == NULL) {
        errno = EINVAL;
        return -1;
    }

    struct atexit_fun *node;
    if ((node = calloc(1, sizeof(struct atexit_fun))) == NULL) {
        return -1;
    }

    node->next = global_atexit_list;
    node->function = function;
    global_atexit_list = node;

    return 0;
}

int kill(pid_t pid, int sig)
{
    return syscall(__NR_kill, pid, sig);
}

pid_t getpid(void)
{
    return syscall(__NR_getpid);
}

uid_t getuid(void)
{
    return syscall(__NR_getuid);
}

uid_t geteuid(void)
{
    return syscall(__NR_getuid);
}

gid_t getgid(void)
{
    return syscall(__NR_getgid);
}

gid_t getegid(void)
{
    return syscall(__NR_getegid);
}

int raise(int sig)
{
    return pthread_kill(pthread_self(), sig);
    //return kill(getpid(), sig);
}

static FILE *pw = NULL;
static struct passwd pass = {
    .pw_name   = NULL,
    .pw_passwd = NULL,
    .pw_gecos  = NULL,
    .pw_shell  = NULL,
    .pw_dir    = NULL
};

static void free_pwnam()
{
    if (pass.pw_name)   { free(pass.pw_name);   pass.pw_name   = NULL; }
    if (pass.pw_passwd) { free(pass.pw_passwd); pass.pw_passwd = NULL; }
    if (pass.pw_gecos)  { free(pass.pw_gecos);  pass.pw_gecos  = NULL; }
    if (pass.pw_shell)  { free(pass.pw_shell);  pass.pw_shell  = NULL; }
    if (pass.pw_dir)    { free(pass.pw_dir);    pass.pw_dir    = NULL; }
}

static struct passwd *getpw(const char *name, uid_t uid)
{
    if (pw == NULL) {
        if ((pw = fopen("/etc/passwd","r")) == NULL)
            return NULL;
    }

    rewind(pw);

    size_t   len   = 0;
    ssize_t  bytes = 0;
    char    *line  = NULL;
    int      rc;

    do {
        bytes = getline(&line, &len, pw);

        if (line == NULL || len <=0 || bytes <= 0 || feof(pw) || ferror(pw)) {
            if (ferror(pw)) {
                fclose(pw);
                pw = NULL;
            }

            if (line) {
                free(line);
                line = NULL;
            }
            return NULL;
        }

        free_pwnam();
        rc = sscanf(line, " %ms:%ms:%d:%d:%m[-_,.0-9a-zA-Z ]:%ms:%ms ",
                &pass.pw_name,
                &pass.pw_passwd,
                &pass.pw_uid,
                &pass.pw_gid,
                &pass.pw_gecos,
                &pass.pw_dir,
                &pass.pw_shell
                );

        free(line);
        line = NULL;

        if (rc == EOF && ferror(pw))
            goto skip;

        if (rc < 4)
            goto skip;

        if (name) {
            if (!strcmp(name, pass.pw_name))
                return &pass;
        } else {
            if (uid == pass.pw_uid)
                return &pass;
        }
skip:
        free_pwnam();
    } while(1);

    //fail:
    free_pwnam();
    if (pw)
        fclose(pw);
    if (line)
        free(line);
    return NULL;
}

struct passwd *getpwnam(const char *name)
{
    return getpw(name, 0);
}

struct passwd *getpwuid(uid_t uid)
{
    return getpw(NULL, uid);
}

struct group *getgrnam(const char *name)
{
    struct group *tmp;
    setgrent();
    
    while ((tmp = getgrent()) != NULL)
    {
        if (!strcmp(name, tmp->gr_name))
            return tmp;
    }
    
    return NULL;
}

struct group *getgrgid(gid_t gid)
{
    struct group *tmp;
    setgrent();
    
    while ((tmp = getgrent()) != NULL)
    {
        if (gid == tmp->gr_gid)
            return tmp;
    }
    
    return NULL;
}

int getgroups(int size, gid_t list[])
{
    errno = ENOMEM;
    if (list == NULL || size <= 0)
        errno = EINVAL;
    return 0; /* TODO */
}

static FILE *gr = NULL; 

void setgrent(void)
{
    if (gr == NULL) {
        if ((gr = fopen("/etc/group", "r")) == NULL)
            return;
    } else {
        rewind(gr);
    }
}

void endgrent(void)
{
    if (gr != NULL) {
        fclose(gr);
        gr = NULL;
    }
}

static struct group grpret = {
    .gr_name = NULL,
    .gr_passwd = NULL,
    .gr_mem = NULL,
    .gr_gid = -1,
};

struct group *getgrent(void)
{
    char *line;
    ssize_t rc;
    struct group *ret = NULL;
    size_t len;

    line = NULL;
    len = BUFSIZ;

    if ((rc = getline(&line, &len, gr)) == -1) {
        if (line)
            free(line);
        return NULL;
    }

    if (feof(gr) || ferror(gr)) {
        if (ferror(gr))
            endgrent();
        goto done;
    }

    if (grpret.gr_passwd)
        free(grpret.gr_passwd);
    if (grpret.gr_name)
        free(grpret.gr_name);
    if (grpret.gr_mem) {
        for (size_t i = 0; grpret.gr_mem[i]; i++) {
            free(grpret.gr_mem[i]);
            grpret.gr_mem[i] = NULL;
        }
        free(grpret.gr_mem);
    }

    memset(&grpret, 0, sizeof(grpret));

    char *members;

    rc = sscanf(line, " %ms:%ms:%d:%ms ",
            &grpret.gr_name,
            &grpret.gr_passwd,
            &grpret.gr_gid,
            &members);

    if (rc < 3) {
        goto done;
    }

    if (members && *members) {
        size_t numgrps;
        char *grp, *saveptr;
        char **newmembers;

        numgrps = 0;
        saveptr = NULL;

        if ((grp = strtok_r(members, ",", &saveptr)) == NULL)
            goto out;

        grpret.gr_mem = malloc((++numgrps + 1) * sizeof(char *));
        grpret.gr_mem[0] = strdup(grp); /* TODO error check */
        grpret.gr_mem[1] = NULL;

        while ((grp = strtok_r(NULL, ",", &saveptr)) != NULL)
        {
            if ((newmembers = realloc(grpret.gr_mem, (++numgrps + 1) * sizeof(char *))) == NULL)
                goto done;

            grpret.gr_mem = newmembers;
            grpret.gr_mem[numgrps] = NULL;
            grpret.gr_mem[numgrps-1] = strdup(grp); /* TODO error check */
        }
    }
out:

    ret = &grpret;
done:
    if (line)
        free(line);
    return ret;
}

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
    return syscall(__NR_sigprocmask, how, set, oldset, sizeof(sigset_t));
}

[[gnu::noreturn]] void abort(void)
{
    sigset_t signal_mask;

    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, SIGABRT);
    sigprocmask(SIG_UNBLOCK, &signal_mask, NULL);

    raise(SIGABRT);

    const struct sigaction sa = {
        .sa_handler = SIG_DFL,
        .sa_mask = 0,
        .sa_flags = 0
    };

    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, SIGABRT);
    sigprocmask(SIG_UNBLOCK, &signal_mask, NULL);
    sigaction(SIGABRT, &sa, NULL);
    raise(SIGABRT);

    /* FIXME this should restore the defaul signal handler for SIGABRT,
     * then raise again */
    exit(1);
}

int sigismember(const sigset_t *set, int signum)
{
    return *set & (1UL << signum);
}

int sigemptyset(sigset_t *set)
{
    if (!set)
        return -1;

    *set = 0;
    return 0;
}

int sigfillset(sigset_t *set)
{
    *set = ~0UL;
    return 0;
}

int sigaddset(sigset_t *set, int signo)
{
    if (!set || signo <= 0 || signo > 63) {
        errno = EINVAL;
        return -1;
    }

    *set |= (1UL << signo);

    return 0;
}

int sigdelset(sigset_t *set, int signo)
{
    if (!set || signo <= 0 || signo > 63) {
        errno = EINVAL;
        return -1;
    }

    *set &= ~(1UL << signo);

    return 0;
}

void perror(const char *s)
{
    if (s && *s)
        fprintf(stderr, "%s: %s\n", strerror(errno), s);
    else
        fprintf(stderr, "%s\n", strerror(errno));
}

char *strerror(int errnum)
{
    //static char buf[64];

    switch(errnum)
    {
        case 0:
            return "ENONE";
        case EPERM:
            return "EPERM";
        case ENOENT:
            return "ENOENT";
        case ESRCH:
            return "ESRCH";
        case EINTR:
            return "EINTR";
        case EBADF:
            return "EBADF";
        case ENOMEM:
            return "ENOMEM";
        case EACCES:
            return "EACCES";
        case EBUSY:
            return "EBUSY";
        case EEXIST:
            return "EEXIST";
        case EINVAL:
            return "EINVAL";
        case ENOTTY:
            return "ENOTTY";
        case EDEADLK:
            return "EDEADLK";
        case ENOSYS:
            return "ENOSYS";
        case EOVERFLOW:
            return "EOVERFLOW";
        case EXDEV:
            return "EXDEV";
        case ECONNREFUSED:
            return "ECONNREFUSED";
        case ENODEV:
            return "ENODEV";
        case ENOTDIR:
            return "ENOTDIR";
        case ENOTCONN:
            return "ENOTCONN";
        case EAFNOSUPPORT:
            return "EAFNOSUPPORT";
        case ENOTSOCK:
            return "ENOTSOCK";
        case EOPNOTSUPP:
            return "EOPNOTSUPP";
        default:
            return "unknown";
            errno = EINVAL;
    }
}

void *memccpy(void *dest, const void *src, int c, size_t n)
{
    register const unsigned char *s_ptr = src;
    register const unsigned char cmp = (unsigned char)c;
    register unsigned char *d_ptr = dest;
    register size_t cnt = n;

    while (*s_ptr && cnt)
    {
        if (*s_ptr == cmp)
            return ++d_ptr;
        *d_ptr++ = *s_ptr++;
        cnt--;
    }

    return NULL;
}

void *memmove(void *dest, const void *src, size_t n)
{
    void *tmp;

    if ((tmp = malloc(n)) == NULL)
        return NULL;

    memcpy(tmp, src, n);
    memcpy(dest, tmp, n);

    free(tmp);
    return dest;
}

wchar_t *wmemcpy(wchar_t *dest, const wchar_t *src, size_t n)
{
    size_t cnt;

    for (cnt = 0; cnt < n; cnt++)
        dest[cnt] = src[cnt];

    return dest;
}

wchar_t *wmemmove(wchar_t *dest, const wchar_t *src, size_t n)
{
    wchar_t *tmp;

    if ((tmp = malloc(n * sizeof(wchar_t))) == NULL)
        return NULL;

    wmemcpy(tmp, src, n);
    wmemcpy(dest, tmp, n);

    free(tmp);
    return dest;
}

#if 0
void *memcpy(void *dest, const void *src, size_t n)
{
    for (size_t i = 0; i < n; i++)
        ((unsigned char *)dest)[i] = ((unsigned char *)src)[i];
    return dest;
}
#endif

void *memcpy(void *dst, const void *src, size_t n)
{
    if (n == 0)
        return NULL;

    if (dst == NULL || src == NULL) {
        printf("memcpy: attempt to copy from 0x%lx to 0x%lx %lu bytes\n", (uintptr_t)src, (uintptr_t)dst, n);
        return NULL;
    }

    for(size_t i = 0; i < n; i++)
        ((char *)dst)[i] = ((const char *)src)[i];

    return dst;

    /*
       const unsigned char *src_ptr = src;
       const unsigned char *const src_end = src + n;
       unsigned char       *dst_ptr = dst;

       const size_t qwords = n / sizeof(uint64_t);
       const size_t bytes  = qwords * sizeof(uint64_t);*/

    __asm__( "cld; pushq %%rsi; pushq %%rdi; pushq %%rcx; rep movsb; popq %%rcx; popq %%rdi; popq %%rsi;"
            :
            : "S" (src), "D" (dst), "c" (n)//qwords)
        : "memory"
            );

    return dst;
    /*
       src_ptr = src + bytes;
       dst_ptr = dst + bytes;

       while (src_ptr < src_end)
       {
     *dst_ptr = *src_ptr;
     dst_ptr++;
     src_ptr++;
     }

     return dst;
     */
}

#if 0
#define LONG_SIZE sizeof(unsigned long long)
void *memcpy(void *dest, const void *src, size_t n)
{
    register size_t todo = n;

    if (dest == NULL)
        return dest;

    register const unsigned long long *src_ptr;
    register unsigned long long *dst_ptr;
    const unsigned char *s_ptr;
    unsigned char *d_ptr;

    s_ptr = src;
    d_ptr = dest;

    src_ptr = src;
    dst_ptr = dest;

    if (todo > LONG_SIZE) {
        for (;todo > LONG_SIZE; todo -= LONG_SIZE) {
            *(dst_ptr++) = *(src_ptr++);
        }

        s_ptr += (n - todo);
        d_ptr += (n - todo);
    }

    for (size_t i = 0; i < todo; i++) {
        *(d_ptr++) = *(s_ptr++);
    }

    return dest;
}
#undef LONG_SIZE
#endif

char *strdup(const char *s)
{
    if (s == NULL) {
        return NULL;
    }

    char *ret;
    size_t len = strlen(s) + 1;

    if ((ret = calloc(1, len)) == NULL)
        return NULL;

    memcpy(ret, s, len);
    return ret;
}

char *strndup(const char *s, size_t n)
{
    char *ret;
    size_t len = strlen(s);
    if (len > n) len = n;
    n++;

    if ((ret = malloc(len)) == NULL)
        return NULL;

    ret[len] = '\0';

    memcpy(ret, s, len);
    return ret;
}

static struct servent *netdb = NULL;
static int netdb_size = -1;
static bool netdb_keepopen = 1;
static int netdb_current_record = 0;

static void free_servent(struct servent *db, int size)
{
    if (db == NULL)
        return;

    for (int i = 0; i < size; i++)
    {
        if (db[i].s_name)
            free(db[i].s_name);
        if (db[i].s_aliases)
            for (int j = 0; db[i].s_aliases[j]; j++)
                free(db[i].s_aliases[j]);
        free(db[i].s_aliases);
        if (db[i].s_proto)
            free(db[i].s_proto);
    }

    free(db);
}

static int process_netdb(void)
{
    if (netdb) {
        free_servent(netdb, netdb_size);
        netdb = NULL;
        netdb_size = 0;
    }

    int fp = -1;
    char *buf = NULL;
    struct stat sb;
    ssize_t rc;
    FILE *fbuf = NULL;
    struct servent *servent = NULL;
    char *lineptr = NULL;
    size_t n = 0;
    bool running = true;
    int valid_lines = 0;

    if ((fp = open("services", O_RDONLY)) == -1)
        return -1;

    if (fstat(fp, &sb) == -1)
        goto fail;

    if ((buf = calloc(1, sb.st_size + 1)) == NULL)
        goto fail;

    rc = read(fp, buf, sb.st_size);

    if (rc == -1 || rc < sb.st_size)
        goto fail;

    if ((fbuf = fmemopen(buf, sb.st_size, "r")) == NULL)
        goto fail;

    while (running)
    {
        if (getline(&lineptr, &n, fbuf) < 0 || n == 0 || lineptr == NULL) {
            running = false;
            goto next;
        }

        if (*lineptr == '#' || *lineptr == ';')
            goto next;

        valid_lines++;
next:
        if (lineptr)
            free(lineptr);

        n = 0;
        lineptr = NULL;
    }

    if (valid_lines == 0)
        goto done;

    if ((servent = calloc(valid_lines, sizeof(struct servent))) == NULL)
        goto fail;

    //printf("allocated %d entries\n", valid_lines);

    running = true;
    valid_lines = 0;
    n = 0;
    lineptr = NULL;

    rewind(fbuf);
    netdb_size = 0;

    while (running)
    {
        char *buf = NULL;
        char *service_name = NULL, *service_proto = NULL;
        int service_port = -1;
        char *service_aliases = NULL;
        int offset = -1;
        char **alias_list = NULL;

        lineptr = NULL;
        n = 0;

        if (getline(&lineptr, &n, fbuf) < 0 || n == 0 || lineptr == NULL) {
            //warn("noline1");
            running = false;
            goto next2;
        }

        if (*lineptr == '#' || *lineptr == ';' || *lineptr == '\n')
            goto next2;

        //printf("lineptr=<%s>\n", lineptr);

        if (sscanf(lineptr, "%m[^\n#]", &buf) != 1 || buf == NULL) {
            warnx("scanf fail");
            running = false;
            goto next2;
        }
        //printf("buf=<%s>\n", buf);

        offset = strlen(buf) - 1;

        while (buf[offset] && isspace(buf[offset]))
            buf[offset--] = '\0';

        int rc;

        //printf("buf=<%s>\n", buf);

        if ((rc = sscanf(buf, "%ms %u/%ms %m[^\n]",
                        &service_name, &service_port, &service_proto, &service_aliases)) < 3) {
            warnx("scanf2 fail: %u <%s>", rc, buf);
            exit(0);
            running = false;
            goto next2;
        }

        servent[netdb_size].s_name = service_name;
        servent[netdb_size].s_proto = service_proto;
        servent[netdb_size].s_port = service_port;

        /* process aliases (if any) */
        if (service_aliases && strlen(service_aliases)) {
            int count = 1;
            char *ptr = service_aliases;

            for (; *ptr; ptr++)
                if (isspace(*ptr))
                    count++;

            /* this will over estimate in some cases, but who cares */
            if ((alias_list = calloc(count + 1, sizeof(char *))) == NULL) {
                running = false;
                goto next2;
            }

            ptr = strtok(service_aliases, " \t");
            count = 0;

            while (ptr)
            {
                if ((alias_list[count] = strdup(ptr)) == NULL) {
                    ptr = NULL;
                    continue;
                }
                ptr = strtok(NULL, " \t");
                count++;
            }

            alias_list[count] = NULL;

            servent[netdb_size].s_aliases = alias_list;
            alias_list = NULL;
        } else {
            servent[netdb_size].s_aliases = NULL;
        }

        netdb_size++;

        /* NULL so we don't free those stashed above */
        service_name = NULL;
        service_proto = NULL;

next2:
        if (alias_list) {
            for (int i = 0; alias_list[i]; i++)
                free(alias_list[i]);
            free(alias_list);
        }
        if (buf)
            free(buf);
        if (service_name)
            free(service_name);
        if (service_proto)
            free(service_proto);
        if (service_aliases)
            free(service_aliases);
        if (lineptr)
            free(lineptr);

        n = 0;
        lineptr = NULL;
    }

done:
    close(fp);
    fclose(fbuf);
    free(buf);
    netdb = servent;

    return 0;

fail:
    warnx("fail");
    if (fp != -1)
        close(fp);
    if (fbuf)
        fclose(fbuf);
    if (buf)
        free(buf);
    if (servent)
        free_servent(servent, netdb_size);

    return -1;
}

static int open_netdb(void)
{
    if (!netdb_keepopen || netdb == NULL)
        return process_netdb();
    return 0;
}

struct servent *getservbyname(const char *name, const char *proto)
{
    if (open_netdb() == -1)
        return NULL;

    for (int i = 0; i < netdb_size; i++)
    {
        /* check the primary name */
        if (!strcasecmp(name, netdb[i].s_name)) {
            if (!proto || !strcasecmp(proto, netdb[i].s_proto)) {
                return &netdb[i];
            }
        } else if (netdb[i].s_aliases) {
            /* check all aliases, if they exist */
            for (int j = 0; netdb[i].s_aliases[j]; j++)
                if (!strcasecmp(name, netdb[i].s_aliases[j]))
                    if (!proto || !strcasecmp(proto, netdb[i].s_proto)) {
                        return &netdb[i];
                    }
        }
    }

    return NULL;
}

struct servent *getservent(void)
{
    if (open_netdb() == -1) {
        return NULL;
    }

    if (netdb_current_record >= netdb_size) {
        return NULL;
    }

    return &netdb[netdb_current_record++];
}

void setservent(int stayopen)
{
    netdb_keepopen = stayopen;
    netdb_current_record = 0;
    process_netdb();
}


void endservent(void)
{
    netdb_current_record = 0;
    if (netdb)
        free_servent(netdb, netdb_size);
    netdb = NULL;
    netdb_size = 0;
}


FILE *setmntent(const char *file, const char *type) {
    FILE *ret;

    if ((ret = fopen(file, type)) == NULL)
        return NULL;

    return ret;
}

static void free_mntent(struct mntent *me)
{
    if (me) {
        if (me->mnt_fsname)
            free(me->mnt_fsname);
        if (me->mnt_opts)
            free(me->mnt_opts);
        if (me->mnt_type)
            free(me->mnt_type);
        if (me->mnt_dir)
            free(me->mnt_dir);
    }
}

struct mntent *getmntent(FILE *stream) {
    char *lineptr;
    size_t len;

    errno = 0;

    if (stream == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (feof(stream))
        return NULL;

    if (ferror(stream)) {
        errno = EBADF;
        return NULL;
    }

    lineptr = NULL;

    while(true)
    {
        if (lineptr) {
            free(lineptr);
            lineptr = NULL;
        }

        len = 0;

        if ((getline(&lineptr, &len, stream)) == -1)
            goto done;

        if (!lineptr || !*lineptr || *lineptr == '#')
            continue;

        free_mntent(&mntent_ret);
        memset(&mntent_ret, 0, sizeof(mntent_ret));

        if (sscanf(lineptr, "%ms %ms %ms %ms %d %d",
                    &mntent_ret.mnt_fsname,
                    &mntent_ret.mnt_dir,
                    &mntent_ret.mnt_type,
                    &mntent_ret.mnt_opts,
                    &mntent_ret.mnt_freq,
                    &mntent_ret.mnt_passno) < 4) {
            continue;
        }

        /* TODO expand escaped characters */

        break;
    }
done:
    if (lineptr)
        free(lineptr);

    return &mntent_ret;
}

int addmntent(FILE *stream, const struct mntent *mnt)
{
    if (stream == NULL || mnt == NULL || mnt->mnt_fsname == NULL ||
            mnt->mnt_opts == NULL || mnt->mnt_dir == NULL || mnt->mnt_type == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* TODO */

    errno = ENOSYS;
    return -1;
}

int endmntent(FILE *stream)
{
    /* TODO spec compliant? */
    free_mntent(&mntent_ret);

    if (stream)
        return fclose(stream);
    else {
        errno = EINVAL;
        return -1;
    }
    return 0;
}

/* replace back with stderr here and perror etc. */

void err(int eval, const char *fmt, ...)
{
    int en = errno;
    if (fmt != NULL) {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
        fprintf(stderr, ": ");
    }
    fprintf(stderr, "%s\n", strerror(en));
    exit(eval);
}

void errx(int eval, const char *fmt, ...)
{
    if (fmt != NULL) {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
    }
    fprintf(stderr, "\n");
    exit(eval);
}

void warn(const char *fmt, ...)
{
    int en = errno;
    if (fmt != NULL) {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
        fprintf(stderr, ": ");
    }
    fprintf(stderr, "%s\n", strerror(en));
}

void warnx(const char *fmt, ...)
{
    if (fmt != NULL) {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
    }
    fprintf(stderr, "\n");
}

int pthread_kill(pthread_t thread, int sig)
{
    return syscall(__NR_tkill, thread->my_tid, sig);
}

int pthread_rwlock_destroy(pthread_rwlock_t *)
{
    return ENOMEM;
}

int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock, const pthread_rwlockattr_t *restrict )
{
    memset(rwlock, 0, sizeof(pthread_rwlock_t));
    return ENOMEM;
}

int pthread_rwlock_unlock(pthread_rwlock_t *)
{
    return ENOMEM;
}

int pthread_rwlock_trywrlock(pthread_rwlock_t *)
{
    return EBUSY;
}

int pthread_rwlock_rdlock(pthread_rwlock_t *)
{
    return ENOMEM;
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t *)
{
    return EBUSY;
}

int pthread_rwlock_wrlock(pthread_rwlock_t *)
{
    return ENOMEM;
}

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    return (void *)syscall(__NR_mmap, addr, length, prot, flags, fd, offset);
}

#define STACK_SIZE (1024 * 1024)

/* invoked from clone.S */
int __start_thread(int (*fn)(void *), void *arg)
{
    _exit(fn(arg));
}

pid_t fork(void)
{
    pid_t ret;
    //printf("fork: pre environ=%p environ[0]=%p environ[0]=%s\n", environ, environ[0], environ[0]);
    ret = (pid_t)syscall(__NR_fork);
    //printf("fork: post[%d] environ=%p environ[0]=%p environ[0]=%s\n", ret, environ, environ[0], environ[0]);
    return ret;
}

extern int _clone(unsigned long flags, void *stack, void *parent_id,
        void *child_tid, unsigned long newtls, int (*fn)(void *), void *arg);

/* the c-library wrapper */
int clone(int (*fn)(void *), void *stack, int flags, void *arg, ...)
{
    int parent_id = gettid();
    int child_tid = 0;
    int ret;

    ret = _clone(
            flags,
            stack,
            &parent_id,
            &child_tid,
            0,
            fn,
            arg
            );

    if (ret < 0) {
        return ret;
    } else if (ret == 0) {
        for (;;) ;
    } else
        return child_tid;
}

pthread_t pthread_self(void)
{
    return __pthread_self();
}

static const pthread_attr_t default_pthread_attr = {
    .stackaddr = NULL,
    .stacksize = STACK_SIZE,
};

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg)
{
    int clone_flags = CLONE_VM|CLONE_FS|CLONE_FILES
        |CLONE_SIGHAND|CLONE_THREAD|CLONE_SYSVSEM|CLONE_SETTLS
        |CLONE_PARENT_SETTID|CLONE_CHILD_CLEARTID|CLONE_DETACHED;

    __attribute__((unused)) struct __pthread *self;
    struct __pthread *new;
    void *stack;
    const pthread_attr_t *at = attr ? attr : &default_pthread_attr;

    if ((new = malloc(sizeof(struct __pthread))) == NULL) {
        errno = ENOMEM;
        return -1;
    }

    memcpy(&new->attrs, at, sizeof(pthread_attr_t));

    if (at->stackaddr) {
        stack = at->stackaddr;
    } else if ((stack = mmap(NULL, at->stacksize, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON|MAP_STACK, -1, 0)) == MAP_FAILED) {
        free(new);
        return -1;
    }

    self = __pthread_self();
    new->self = new;
    new->errnum = 0;
    new->start_routine = start_routine;
    new->start_arg = arg;
    new->stack = stack;
    new->stack_size = STACK_SIZE;
    new->parent_tid = gettid();
    new->my_tid = 0;
    new->attrs.stackaddr = stack;
    *thread = new;

    _Pragma("GCC diagnostic push")
        _Pragma("GCC diagnostic ignored \"-Wincompatible-pointer-types\"")
        int ret = _clone(
                clone_flags,
                (char *)stack + STACK_SIZE,
                &new->parent_tid,
                &new->my_tid,
                (unsigned long)new,
                /* i have no idea how to handle the fact clone wants int return.
                 * but pthread create void return */
                start_routine,
                arg
                );
    _Pragma("GCC diagnostic pop")

        if (ret < 0) {
            *thread = NULL;
            return ret;
        }

    return 0;
}

int tcsendbreak(int fd, int duration)
{
    return ioctl(fd, TCSBRKP, duration);
}

void cfmakeraw(struct termios *tio)
{
    tio->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tio->c_oflag &= ~OPOST;
    tio->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tio->c_cflag &= ~(CSIZE | PARENB);
    tio->c_cflag |= CS8;
}

char *ctermid(char *s)
{
    if (s == NULL)
        s = "/dev/tty";
    else
        strcpy(s, "/dev/tty");

    return s;
}

int tcgetattr(int fd, struct termios *tio)
{
    return ioctl(fd, TCGETS, tio);
}

speed_t cfgetospeed(const struct termios *tio)
{
    return tio->c_ospeed;
}

speed_t cfgetispeed(const struct termios *tio)
{
    return tio->c_ispeed;
}
int tcsetattr(int fd, int optional_actions, const struct termios *tio)
{
    int sc;

    switch (optional_actions)
    {
        case 0:
        case TCSANOW:
            sc = TCSETS;
            break;
        case TCSADRAIN:
            sc = TCSETSW;
            break;
        case TCSAFLUSH:
            sc = TCSETSF;
            break;
        default:
            errno = EINVAL;
            return -1;
    }

    return ioctl(fd, sc, tio);
}

/* in the abscence of a standard, use Linux's */
int mount(const char *source, const char *target, const char *fstype, unsigned long flags, const void *data)
{
    return syscall(__NR_mount, source, target, fstype, flags, data);
}

static char ttyname_string[NAME_MAX];

int isatty(int fd)
{
    struct termios tio;
    const int rc = tcgetattr(fd, &tio);

    if (rc == -1 && errno == ENOTTY)
        return 0;
    else if (rc == -1)
        return -1;
    else
        return 0;
}

char *ttyname(int fd)
{
    if (!isatty(fd)) {
        errno = ENOTTY;
        return NULL;
    }

    char buf[64];
    ssize_t len;

    snprintf(buf, sizeof(buf), "/proc/self/fd/%d", fd);

    if ((len = readlink(buf, ttyname_string, sizeof(ttyname_string))) == -1)
        return NULL;

    ttyname_string[len] = 0;
    return ttyname_string;
}

static struct resolv *parse_resolv_config(void)
{
    FILE *fp = NULL;
    struct resolv *ret = NULL;
    int rc = 0;

    if ((fp = fopen("/etc/resolv.conf", "r")) == NULL) {
        warn("Could not open /etc/resolv.conf");
        goto fail;
    }

    char *lineptr = NULL;
    size_t len = 0;

    if ((ret = calloc(1, sizeof(struct resolv))) == NULL)
        goto fail;

    char *tmp;

    while (!ferror(fp) && !feof(fp))
    {
        if (lineptr) {
            free(lineptr);
            lineptr = NULL;
        }

        if (getline(&lineptr, &len, fp) == -1)
            break;

        if (!lineptr || !*lineptr || !len || *lineptr == '#' || *lineptr == ';')
            continue;

        if (lineptr == strstr(lineptr, "nameserver ")) {
            if ((rc = sscanf(lineptr, "%*s %ms ", &tmp)) != 2) {
                warn("parse_resolv_config: nameserver sscanf returned %d for <%s>\n", rc, lineptr);
                continue;
            }
            char *tmp_ptr, **tmp_ns;
            int num_ns = 0;
            ret->nameservers = NULL;

            if ((tmp_ptr = strtok(tmp, " ")) == NULL) {
                warnx("parse_resolv_config: invalid nameserver line");
                continue;
            }

            do {
                if ((tmp_ns = realloc(ret->nameservers, (num_ns + 2) * sizeof(char *))) == NULL) {
                    warn("parse_resolv_config: realloc");
                    continue;
                }
                ret->nameservers = tmp_ns;
                ret->nameservers[num_ns++] = strdup(tmp_ptr);
                ret->nameservers[num_ns] = NULL;
                tmp_ptr = strtok(NULL, " ");
            } while(tmp_ptr);

            if ((ret->nameservers_in = malloc(sizeof(in_addr_t) * num_ns)) == NULL) {
                warn("parse_resolv_config: malloc");
                continue;
            }

            for (int i = 0; i < num_ns; i++) {
                const struct addrinfo hints = {
                    .ai_family = AF_INET,
                    .ai_flags = AI_NUMERICHOST,
                };
                struct addrinfo *result;

                ret->nameservers_in[i] = 0;
                if ((rc = getaddrinfo(ret->nameservers[i], NULL, &hints, &result)) != 0) {
                    warnx("parse_resolv_config: getaddrinfo: %s", gai_strerror(rc));
                    goto next;
                }

                ret->nameservers_in[i] = ((struct sockaddr_in *)result->ai_addr)->sin_addr.s_addr;
                freeaddrinfo(result);
next:
                free(ret->nameservers[i]);
                ret->nameservers[i] = NULL;
            }
            free(ret->nameservers);
            ret->nameservers = NULL;
        } else if (lineptr == strstr(lineptr, "domain ")) {
            if (sscanf(lineptr, "%*s %ms ", &tmp) != 2)
                continue;
        } else if (lineptr == strstr(lineptr, "search ")) {
            if (sscanf(lineptr, "%*s %ms ", &tmp) != 2)
                continue;
        } else if (lineptr == strstr(lineptr, "options ")) {
            char *ptr, *tmp_ptr;
            for (tmp_ptr = lineptr + 8; *tmp_ptr && isspace(*tmp_ptr); tmp_ptr++)
                ;
            
            ptr = strtok(tmp_ptr, " ");

            while(ptr)
            {
                if (ptr == strstr(ptr, "ndots:")) {
                } else if (ptr == strstr(ptr, "rotate")) {
                } else if (ptr == strstr(ptr, "timeout:")) {
                } else if (ptr == strstr(ptr, "attempts:")) {
                } else
                    warnx("unknown resolv.conf option <%s>\n", ptr);
                ptr = strtok(NULL, " ");
            }
        } else
            warnx("unknown resolv.conf config <%s>\n", lineptr);

        free(lineptr);
        lineptr = NULL;
        len = 0;
    }

    return ret;

fail:
    printf("fail\n");
    if (fp)
        fclose(fp);
    if (ret)
        free(ret);

    return NULL;
}

int getaddrinfo(const char *restrict nodename, const char *restrict servname,
        const struct addrinfo *restrict hints, struct addrinfo **restrict res)
{
    if (!nodename && !servname)
        return EAI_NONAME;

    if (!res)
        return EAI_SYSTEM;

    struct addrinfo defaults = {
        .ai_flags = AI_V4MAPPED|AI_ADDRCONFIG,
        .ai_family = AF_UNSPEC,
        .ai_socktype = 0,
        .ai_protocol = 0,
    };
    
    if (hints) {
        if (hints->ai_flags & AI_CANONNAME && nodename == NULL)
            return EAI_BADFLAGS;

        if (hints->ai_family)
            defaults.ai_family = hints->ai_family;
        if (hints->ai_socktype)
            defaults.ai_socktype = hints->ai_socktype;
        if (hints->ai_protocol)
            defaults.ai_protocol = hints->ai_protocol;
        if (hints->ai_flags)
            defaults.ai_flags = hints->ai_flags;
    }

    if ((*res = calloc(1, sizeof(struct addrinfo))) == NULL)
        return EAI_MEMORY;

    if ((defaults.ai_flags & AI_NUMERICHOST) == 0) {
        if (nodename == NULL) /* TODO handle services too */
            return EAI_NONAME;

        if (resolv == NULL && ((resolv = parse_resolv_config()) == NULL))
            return EAI_SYSTEM;

        if (defaults.ai_family == AF_INET || defaults.ai_family == AF_UNSPEC) {
            if (((*res)->ai_addr = malloc(sizeof(struct sockaddr_in))) == NULL) {
                free(*res);
                *res = NULL;
                return EAI_MEMORY;
            }

            (*res)->ai_addrlen = sizeof(struct sockaddr_in);
            (*res)->ai_addr->sa_family = AF_INET;
            (*res)->ai_next = NULL;

            int rc = send_request(nodename, &((struct sockaddr_in *)(*res)->ai_addr)->sin_addr.s_addr, TYPE_A);

            if (rc == -1) {
                free((*res)->ai_addr);
                free(*res);
                *res = NULL;
                return EAI_FAIL;
            }

            return 0;
        }

        return EAI_FAMILY;
    }

    if ((defaults.ai_flags & AI_NUMERICHOST) == AI_NUMERICHOST) {
        if (nodename == NULL)
            return EAI_NONAME;

        if (defaults.ai_family == AF_INET || defaults.ai_family == AF_UNSPEC) {
            if (((*res)->ai_addr = malloc(sizeof(struct sockaddr_in))) == NULL) {
                free(*res);
                *res = NULL;
                return EAI_MEMORY;
            }

            (*res)->ai_addrlen = sizeof(struct sockaddr_in);

            ((struct sockaddr_in *)(*res)->ai_addr)->sin_addr.s_addr = inet_addr(nodename);
            (*res)->ai_addr->sa_family = AF_INET;

            (*res)->ai_next = NULL;

            return 0;
        }

        return EAI_FAMILY;
    }

    return EAI_SYSTEM;
}

void freeaddrinfo(struct addrinfo *ai)
{
    struct addrinfo *next;

    while(ai)
    {
        next = ai->ai_next;
        if (ai->ai_addr)
            free(ai->ai_addr);
        if (ai->ai_canonname)
            free(ai->ai_canonname);
        free(ai);
        ai = next;
    }
}

const char *gai_strerror(int ecode)
{
    static char buf[BUFSIZ];

    const char *const codes[] = {
        [0]             = "success",
        [-EAI_AGAIN]    = "EAI_AGAIN",
        [-EAI_BADFLAGS] = "EAI_BADFLAGS",
        [-EAI_FAIL]     = "EAI_FAIL",
        [-EAI_FAMILY]   = "EAI_FAMILY",
        [-EAI_MEMORY]   = "EAI_MEMORY",
        [-EAI_NONAME]   = "EAI_NONAME",
        [-EAI_SERVICE]  = "EAI_SERVICE",
        [-EAI_SOCKTYPE] = "EAI_SOCKTYPE",
        [-EAI_SYSTEM]   = "EAI_SYSTEM",
        [-EAI_OVERFLOW] = "EAI_OVERFLOW",
    };

    if (ecode > 0)
        return "unknown";
    else if (-ecode > (int)(sizeof(codes)/sizeof(char *)))
        return "unknown";
    else if (codes[-ecode] == NULL)
        return "unknown";
    else if (ecode == EAI_SYSTEM) {
        snprintf(buf, sizeof(buf), "EAI_SYSTEM: %s", strerror(errno));
        return buf;
    } else return codes[-ecode];
}

void closelog(void)
{
}

static void open_syslog(void)
{
    if ((unix_socket = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
        return;

    struct sockaddr_un sun = {
        .sun_family = AF_UNIX,
        .sun_path   = "/dev/log"
    };

    if (connect(unix_socket, (struct sockaddr *)&sun, sizeof(sun)) == -1) {
        close(unix_socket);
        unix_socket = -1;
        return;
    }
}

void openlog(const char *ident, int logopt, int facility)
{
    sl_facility = facility;
    sl_options  = logopt;

    if (sl_ident)
        free((void *)sl_ident);

    sl_ident = strdup(ident);

    if ((logopt & LOG_NDELAY))
        open_syslog();
}

int setlogmask(int maskpri)
{
    int old_mask = sl_mask;
    sl_mask = maskpri;
    return old_mask;
}

void syslog(int priority, const char *message, ...)
{
    va_list vararg;
    va_start(vararg, message);
    vsyslog(priority, message, vararg);
    va_end(vararg);
}

void vsyslog(int priority, const char *message, va_list ap)
{
    if (unix_socket == -1)
        open_syslog();

    if (unix_socket == -1 && !(sl_options & LOG_CONS))
        return;

    int fd = unix_socket;
    int con_fd = -1;

    if (unix_socket == -1 || (sl_options & LOG_CONS))
        if ((con_fd = open("/dev/console", O_WRONLY)) == -1) {
            perror("open /dev/console");
            return;
        }

    if (unix_socket == -1)
        fd = con_fd;

    char t_mess[BUFSIZ];
    char t_date[18]; /* Apr 10 00:00:00 */
    char t_log[sizeof(t_mess) + sizeof(t_date) + 1];

    struct tm *tmp;
    time_t t = time(NULL);
    tmp = localtime(&t);

    if (!tmp)
        return;

    strftime(t_date, sizeof(t_date), "%b %e %H:%M:%S", tmp);
    vsnprintf(t_mess, sizeof(t_mess), message, ap);

    if ((sl_options & LOG_PID))
        snprintf(t_log, sizeof(t_log), "<%u>%s %s[%u]: %s\n", sl_facility|priority, t_date, sl_ident, getpid(), t_mess);
    else
        snprintf(t_log, sizeof(t_log), "<%u>%s %s: %s\n",      sl_facility|priority, t_date, sl_ident,           t_mess);

    size_t len = strlen(t_log);

    if (write(fd, t_log, len) == -1 || unix_socket == -1)
        close(fd);

    if ((sl_options & LOG_CONS) && (fd != con_fd)) {
        write(con_fd, t_log, len);
        close(con_fd);
    }
}

ssize_t readlink(const char *pathname, char *buf, size_t siz)
{
    return syscall(__NR_readlink, pathname, buf, siz);
}

long sysconf(int name)
{
    switch(name)
    {
        case _SC_2_VERSION:      return _POSIX2_VERSION;
        case _SC_CLK_TCK:        return 100;
        case _SC_HOST_NAME_MAX:  return HOST_NAME_MAX;
        case _SC_LOGIN_NAME_MAX: return LOGIN_NAME_MAX;
        case _SC_NGROUPS_MAX:    return NGROUPS_MAX;
        case _SC_NSIG:           return SIGSYS + 1; /* Should NSIG be this? */
        case _SC_OPEN_MAX:       return OPEN_MAX;
        case _SC_PAGESIZE:       return PAGESIZE;
        case _SC_PAGE_SIZE:      return PAGE_SIZE;
        case _SC_VERSION:        return _POSIX_VERSION;
        case _SC_XOPEN_VERSION:  return _XOPEN_VERSION;
        default:
            errno = EINVAL;
            return -1;
    }
}

int killpg(int pgrp, int sig)
{
    return syscall(__NR_kill, -pgrp, sig);
}

int pthread_setcancelstate(int state, int *oldstate)
{
    pthread_t self = pthread_self();

    switch (state)
    {
        case PTHREAD_CANCEL_ENABLE:
        case PTHREAD_CANCEL_DISABLE:
            if (oldstate)
                *oldstate = self->attrs.cancelstate;
            self->attrs.cancelstate = state;
            return 0;

        default:
            return EINVAL;
    }

}

int pthread_setcanceltype(int type, int *oldtype)
{
    pthread_t self = pthread_self();

    switch(type)
    {
        case PTHREAD_CANCEL_DEFERRED:
        case PTHREAD_CANCEL_ASYNCHRONOUS:
            if (oldtype)
                *oldtype = self->attrs.canceltype;
            self->attrs.canceltype = type;
            return 0;

        default:
            return EINVAL;
    }
}

int pthread_join(pthread_t, void **)
{
    return ESRCH;
}

int pthread_attr_init(pthread_attr_t *attr)
{
    memcpy(attr, &default_pthread_attr, sizeof(pthread_attr_t));
    return 0;
}

int pthread_attr_destroy(pthread_attr_t *attr)
{
    memset(attr, 0, sizeof(pthread_attr_t));
    return 0;
}

int pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict )
{
    memset(mutex, 0, sizeof(pthread_mutex_t));
    return 0;
}

int pthread_mutex_trylock(pthread_mutex_t *)
{
    return 0;
}

int pthread_mutex_lock(pthread_mutex_t *)
{
    return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *)
{
    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *)
{
    return 0;
}

int strerror_r(int errnum, char *buf, size_t buflen)
{

    char *tmp = strerror(errnum);

    if (tmp) {
        snprintf(buf, buflen, "%s", strerror(errnum));
        return 0;
    }

    errno = EINVAL;
    return -1;
}

double sinh(double )
{
    return 0;
}

double cosh(double )
{
    return 0;
}

double tanh(double )
{
    return 0;
}

float fmodf(float x, float y)
{
    if (isnan(x) || isnan(y))
        return NAN;

    if (isinf(x)) {
        errno = EDOM;
        return NAN;
    }

    if (y == 0.0) {
        errno = EDOM;
        return NAN;
    }

    return x - (x/y) * y;
}

int __signbitf(float val)
{
    union {float x; uint32_t y;} i = {i.x = val};
    return i.y & (1U<<31);
}

int __signbitd(double val)
{
    union {double x; uint64_t y;} i = {i.x = val};
    return i.y & (1UL<<63);
}

int __fpclassifyf(float val)
{
    union {float x; uint32_t y;} i = {i.x = val};
    short exp = i.y >> 23 & 0xff;
    int frac = i.y & ((1UL<<23)-1);
    //bool sign = i.y >> 31;

    /* TODO figure out how to process the fraction */

    if (exp == 0) {
        /* signed zero or subnormal */
        if (frac)
            return FP_SUBNORMAL;
        else
            return FP_ZERO;
    } else if (exp == 0xff) {
        /* infinity or NaN */
        if (frac)
            return FP_NAN;
        else
            return FP_INFINITE;
    } else
        return FP_NORMAL;
}

int __fpclassifyd(double val)
{
    union {double x; uint64_t y;} i = {val};
    int exp = i.y >> 52 & 0x7ff;
    long frac = i.y & ((1UL<<52)-1);
    //bool sign = i.y >> 63;

    /* TODO figure out how to process the fraction */

    if (exp == 0) {
        /* signed zero or subnormal */
        if (frac)
            return FP_SUBNORMAL;
        else
            return FP_ZERO;
    } else if (exp == 0x7ff) {
        /* infinity or NaN */
        if (frac)
            return FP_NAN;
        else
            return FP_INFINITE;
    } else
        return FP_NORMAL;
}


double fmod(double x, double y)
{
    if (isnan(x) || isnan(y))
        return NAN;

    if (isinf(x)) {
        errno = EDOM;
        return NAN;
    }

    if (y == 0.0) {
        errno = EDOM;
        return NAN;
    }

    return x - (x/y) * y;
}

double sin(double x)
{
    if (isnan(x))
        return NAN;

    if (isinf(x)) {
        errno = EDOM;
        return NAN;
    }

    x = fmod(x,  (2 * M_PI));

    while (x < 0)
        x += 2 * M_PI;

    while (x > 2 * M_PI)
        x -= 2 * M_PI;

    double epsilon = 0.1e-16;
    double sinus = 0.0;
    double sign = 1.0;
    double term = x;
    double n = 1;

    while (term > epsilon) {
        sinus += sign * term;
        sign = -sign;
        term *= x * x / (n+1) / (n+2);
        n += 2;
    }

    return sinus;
}

static double fac(int a)
{
    double ret = 1;
    for (int i = 1; i <= a; i++)
        ret *= i;
    return ret;
}

double cos(double x)
{
    x = fmod(x,  (2 * M_PI));

    while (x < 0)
        x += 2 * M_PI;

    while (x > 2 * M_PI)
        x -= 2 * M_PI;

    double epsilon = 0.1e-16;
    double cosus = 1.0;
    double sign = 1.0;
    double term = 0;
    double n = 2;

    do {
        cosus += sign * term;
        sign = -sign;
        term = pow(x, n) / fac(n);
        n += 2;
    } while (term > epsilon);

    return cosus;
}

double acos(double )
{
    return 0;
}

double asin(double )
{
    return 0;
}

double atan2(double , double )
{
    return 0;
}

double fabs(double x)
{
    double res;
    __asm__("fabs" : "=t" (res) : "0" (x));
    return res;
}

float fabsf(float x)
{
    float res;
    __asm__("fabs" : "=t" (res) : "0" (x));
    return res;
}

/* log e */
double log(double )
{
    return 0;
}

double log10(double )
{
    return 0;
}

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */


#define __HI(x) *(1+(int*)&x)
#define __LO(x) *(int*)&x
#define __HIp(x) *(1+(int*)x)
#define __LOp(x) *(int*)x

double copysign(double x, double y)
{
    __HI(x) = (__HI(x)&0x7fffffff)|(__HI(y)&0x80000000);
    return x;
}

static const double
two54   =  1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */
        twom54  =  5.55111512312578270212e-17, /* 0x3C900000, 0x00000000 */
        huge   = 1.0e+300,
        tiny   = 1.0e-300;

double scalbn (double x, int n)
{
    int  k,hx,lx;
    hx = __HI(x);
    lx = __LO(x);
    k = (hx&0x7ff00000)>>20;        /* extract exponent */
    if (k==0) {             /* 0 or subnormal x */
        if ((lx|(hx&0x7fffffff))==0) return x; /* +-0 */
        x *= two54;
        hx = __HI(x);
        k = ((hx&0x7ff00000)>>20) - 54;
        if (n< -50000) return tiny*x;   /*underflow*/
    }
    if (k==0x7ff) return x+x;       /* NaN or Inf */
    k = k+n;
    if (k >  0x7fe) return huge*copysign(huge,x); /* overflow  */
    if (k > 0)              /* normal result */
    {__HI(x) = (hx&0x800fffff)|(k<<20); return x;}
    if (k <= -54) {
        if (n > 50000) {    /* in case integer overflow in n+k */
            return huge*copysign(huge,x);   /*overflow*/
        } else return tiny*copysign(tiny,x);    /*underflow*/
    }
    k += 54;                /* subnormal result */
    __HI(x) = (hx&0x800fffff)|(k<<20);
    return x*twom54;
}

/* @(#)s_atan.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 *
 */

/* atan(x)
 * Method
 *   1. Reduce x to positive by atan(x) = -atan(-x).
 *   2. According to the integer k=4t+0.25 chopped, t=x, the argument
 *      is further reduced to one of the following intervals and the
 *      arctangent of t is evaluated by the corresponding formula:
 *
 *      [0,7/16]      atan(x) = t-t^3*(a1+t^2*(a2+...(a10+t^2*a11)...)
 *      [7/16,11/16]  atan(x) = atan(1/2) + atan( (t-0.5)/(1+t/2) )
 *      [11/16.19/16] atan(x) = atan( 1 ) + atan( (t-1)/(1+t) )
 *      [19/16,39/16] atan(x) = atan(3/2) + atan( (t-1.5)/(1+1.5t) )
 *      [39/16,INF]   atan(x) = atan(INF) + atan( -1/t )
 *
 * Constants:
 * The hexadecimal values are the intended ones for the following
 * constants. The decimal values may be used, provided that the
 * compiler will convert from decimal to binary accurately enough
 * to produce the hexadecimal values shown.
 */

static const double atanhi[] = {
    4.63647609000806093515e-01, /* atan(0.5)hi 0x3FDDAC67, 0x0561BB4F */
    7.85398163397448278999e-01, /* atan(1.0)hi 0x3FE921FB, 0x54442D18 */
    9.82793723247329054082e-01, /* atan(1.5)hi 0x3FEF730B, 0xD281F69B */
    1.57079632679489655800e+00, /* atan(inf)hi 0x3FF921FB, 0x54442D18 */
};

static const double atanlo[] = {
    2.26987774529616870924e-17, /* atan(0.5)lo 0x3C7A2B7F, 0x222F65E2 */
    3.06161699786838301793e-17, /* atan(1.0)lo 0x3C81A626, 0x33145C07 */
    1.39033110312309984516e-17, /* atan(1.5)lo 0x3C700788, 0x7AF0CBBD */
    6.12323399573676603587e-17, /* atan(inf)lo 0x3C91A626, 0x33145C07 */
};

static const double aT[] = {
    3.33333333333329318027e-01, /* 0x3FD55555, 0x5555550D */
    -1.99999999998764832476e-01, /* 0xBFC99999, 0x9998EBC4 */
    1.42857142725034663711e-01, /* 0x3FC24924, 0x920083FF */
    -1.11111104054623557880e-01, /* 0xBFBC71C6, 0xFE231671 */
    9.09088713343650656196e-02, /* 0x3FB745CD, 0xC54C206E */
    -7.69187620504482999495e-02, /* 0xBFB3B0F2, 0xAF749A6D */
    6.66107313738753120669e-02, /* 0x3FB10D66, 0xA0D03D51 */
    -5.83357013379057348645e-02, /* 0xBFADDE2D, 0x52DEFD9A */
    4.97687799461593236017e-02, /* 0x3FA97B4B, 0x24760DEB */
    -3.65315727442169155270e-02, /* 0xBFA2B444, 0x2C6A6C2F */
    1.62858201153657823623e-02, /* 0x3F90AD3A, 0xE322DA11 */
};

static const double one   = 1.0;

double atan(double x)
{
    double w,s1,s2,z;
    int ix,hx,id;

    hx = __HI(x);
    ix = hx&0x7fffffff;
    if(ix>=0x44100000) {	/* if |x| >= 2^66 */
        if(ix>0x7ff00000||
                (ix==0x7ff00000&&(__LO(x)!=0)))
            return x+x;		/* NaN */
        if(hx>0) return  atanhi[3]+atanlo[3];
        else     return -atanhi[3]-atanlo[3];
    } if (ix < 0x3fdc0000) {	/* |x| < 0.4375 */
        if (ix < 0x3e200000) {	/* |x| < 2^-29 */
            if(huge+x>one) return x;	/* raise inexact */
        }
        id = -1;
    } else {
        x = fabs(x);
        if (ix < 0x3ff30000) {		/* |x| < 1.1875 */
            if (ix < 0x3fe60000) {	/* 7/16 <=|x|<11/16 */
                id = 0; x = (2.0*x-one)/(2.0+x);
            } else {			/* 11/16<=|x|< 19/16 */
                id = 1; x  = (x-one)/(x+one);
            }
        } else {
            if (ix < 0x40038000) {	/* |x| < 2.4375 */
                id = 2; x  = (x-1.5)/(one+1.5*x);
            } else {			/* 2.4375 <= |x| < 2^66 */
                id = 3; x  = -1.0/x;
            }
        }}
    /* end of argument reduction */
    z = x*x;
    w = z*z;
    /* break sum from i=0 to 10 aT[i]z**(i+1) into odd and even poly */
    s1 = z*(aT[0]+w*(aT[2]+w*(aT[4]+w*(aT[6]+w*(aT[8]+w*aT[10])))));
    s2 = w*(aT[1]+w*(aT[3]+w*(aT[5]+w*(aT[7]+w*aT[9]))));
    if (id<0) return x - x*(s1+s2);
    else {
        z = atanhi[id] - ((x*(s1+s2) - atanlo[id]) - x);
        return (hx<0)? -z:z;
    }
}

static const double
bp[] = {1.0, 1.5,},
    dp_h[] = { 0.0, 5.84962487220764160156e-01,}, /* 0x3FE2B803, 0x40000000 */
    dp_l[] = { 0.0, 1.35003920212974897128e-08,}, /* 0x3E4CFDEB, 0x43CFD006 */
    zero    =  0.0,
    two =  2.0,
    two53   =  9007199254740992.0,  /* 0x43400000, 0x00000000 */
    /* poly coefs for (3/2)*(log(x)-2s-2/3*s**3 */
    L1  =  5.99999999999994648725e-01, /* 0x3FE33333, 0x33333303 */
    L2  =  4.28571428578550184252e-01, /* 0x3FDB6DB6, 0xDB6FABFF */
    L3  =  3.33333329818377432918e-01, /* 0x3FD55555, 0x518F264D */
    L4  =  2.72728123808534006489e-01, /* 0x3FD17460, 0xA91D4101 */
    L5  =  2.30660745775561754067e-01, /* 0x3FCD864A, 0x93C9DB65 */
    L6  =  2.06975017800338417784e-01, /* 0x3FCA7E28, 0x4A454EEF */
    P1   =  1.66666666666666019037e-01, /* 0x3FC55555, 0x5555553E */
    P2   = -2.77777777770155933842e-03, /* 0xBF66C16C, 0x16BEBD93 */
    P3   =  6.61375632143793436117e-05, /* 0x3F11566A, 0xAF25DE2C */
    P4   = -1.65339022054652515390e-06, /* 0xBEBBBD41, 0xC5D26BF1 */
    P5   =  4.13813679705723846039e-08, /* 0x3E663769, 0x72BEA4D0 */
    lg2  =  6.93147180559945286227e-01, /* 0x3FE62E42, 0xFEFA39EF */
    lg2_h  =  6.93147182464599609375e-01, /* 0x3FE62E43, 0x00000000 */
    lg2_l  = -1.90465429995776804525e-09, /* 0xBE205C61, 0x0CA86C39 */
    ovt =  8.0085662595372944372e-0017, /* -(1024-log2(ovfl+.5ulp)) */
    cp    =  9.61796693925975554329e-01, /* 0x3FEEC709, 0xDC3A03FD =2/(3ln2) */
    cp_h  =  9.61796700954437255859e-01, /* 0x3FEEC709, 0xE0000000 =(float)cp */
    cp_l  = -7.02846165095275826516e-09, /* 0xBE3E2FE0, 0x145B01F5 =tail of cp_h*/
    ivln2    =  1.44269504088896338700e+00, /* 0x3FF71547, 0x652B82FE =1/ln2 */
    ivln2_h  =  1.44269502162933349609e+00, /* 0x3FF71547, 0x60000000 =24b 1/ln2*/
    ivln2_l  =  1.92596299112661746887e-08; /* 0x3E54AE0B, 0xF85DDF44 =1/ln2 tail*/

double sqrt(double x)
{
    double ret = 0.0;
    __asm__ ("fsqrt" : "=t" (ret) : "0" (x));
    return ret;
}

static double __ieee754_pow(double x, double y)
{
    double z,ax,z_h,z_l,p_h,p_l;
    double y1,t1,t2,r,s,t,u,v,w;
    int i0,__attribute__((unused)) i1,i,j,k,yisint,n;
    int hx,hy,ix,iy;
    unsigned lx,ly;

    i0 = ((*(int*)&one)>>29)^1; i1=1-i0;
    hx = __HI(x); lx = __LO(x);
    hy = __HI(y); ly = __LO(y);
    ix = hx&0x7fffffff;  iy = hy&0x7fffffff;

    /* y==zero: x**0 = 1 */
    if ((iy|ly)==0) return one;

    /* +-NaN return x+y */
    if (ix > 0x7ff00000 || ((ix==0x7ff00000)&&(lx!=0)) ||
            iy > 0x7ff00000 || ((iy==0x7ff00000)&&(ly!=0)))
        return x+y;

    /* determine if y is an odd int when x < 0
     * yisint = 0   ... y is not an integer
     * yisint = 1   ... y is an odd int
     * yisint = 2   ... y is an even int
     */
    yisint  = 0;
    if (hx<0) {
        if (iy>=0x43400000) yisint = 2; /* even integer y */
        else if (iy>=0x3ff00000) {
            k = (iy>>20)-0x3ff;    /* exponent */
            if (k>20) {
                j = ly>>(52-k);
                if ((unsigned)(j<<(52-k))==ly) /* change to (unsigned) unsure if this breaks anything */
                    yisint = 2-(j&1);
            } else if (ly==0) {
                j = iy>>(20-k);
                if ((j<<(20-k))==iy) yisint = 2-(j&1);
            }
        }
    }

    /* special value of y */
    if (ly==0) {
        if (iy==0x7ff00000) {   /* y is +-inf */
            if (((ix-0x3ff00000)|lx)==0)
                return  y - y;  /* inf**+-1 is NaN */
            else if (ix >= 0x3ff00000)/* (|x|>1)**+-inf = inf,0 */
                return (hy>=0)? y: zero;
            else            /* (|x|<1)**-,+inf = inf,0 */
                return (hy<0)?-y: zero;
        }
        if (iy==0x3ff00000) {   /* y is  +-1 */
            if (hy<0) return one/x; else return x;
        }
        if (hy==0x40000000) return x*x; /* y is  2 */
        if (hy==0x3fe00000) {   /* y is  0.5 */
            if (hx>=0)  /* x >= +0 */
                return sqrt(x);
        }
    }

    ax   = fabs(x);
    /* special value of x */
    if (lx==0) {
        if (ix==0x7ff00000||ix==0||ix==0x3ff00000){
            z = ax;         /*x is +-0,+-inf,+-1*/
            if (hy<0) z = one/z;    /* z = (1/|x|) */
            if (hx<0) {
                if (((ix-0x3ff00000)|yisint)==0) {
                    z = (z-z)/(z-z); /* (-1)**non-int is NaN */
                } else if (yisint==1)
                    z = -z;     /* (x<0)**odd = -(|x|**odd) */
            }
            return z;
        }
    }

    n = (hx>>31)+1;

    /* (x<0)**(non-int) is NaN */
    if ((n|yisint)==0) return NAN;//(x-x)/(x-x);

    s = one; /* s (sign of result -ve**odd) = -1 else = 1 */
    if ((n|(yisint-1))==0) s = -one;/* (-ve)**(odd int) */

    /* |y| is huge */
    if (iy>0x41e00000) { /* if |y| > 2**31 */
        if (iy>0x43f00000){ /* if |y| > 2**64, must o/uflow */
            if (ix<=0x3fefffff) return (hy<0)? huge*huge:tiny*tiny;
            if (ix>=0x3ff00000) return (hy>0)? huge*huge:tiny*tiny;
        }
        /* over/underflow if x is not close to one */
        if (ix<0x3fefffff) return (hy<0)? s*huge*huge:s*tiny*tiny;
        if (ix>0x3ff00000) return (hy>0)? s*huge*huge:s*tiny*tiny;
        /* now |1-x| is tiny <= 2**-20, suffice to compute
           log(x) by x-x^2/2+x^3/3-x^4/4 */
        t = ax-one;     /* t has 20 trailing zeros */
        w = (t*t)*(0.5-t*(0.3333333333333333333333-t*0.25));
        u = ivln2_h*t;  /* ivln2_h has 21 sig. bits */
        v = t*ivln2_l-w*ivln2;
        t1 = u+v;
        __LO(t1) = 0;
        t2 = v-(t1-u);
    } else {
        double ss,s2,s_h,s_l,t_h,t_l;
        n = 0;
        /* take care subnormal number */
        if (ix<0x00100000)
        {ax *= two53; n -= 53; ix = __HI(ax); }
        n  += ((ix)>>20)-0x3ff;
        j  = ix&0x000fffff;
        /* determine interval */
        ix = j|0x3ff00000;      /* normalize ix */
        if (j<=0x3988E) k=0;        /* |x|<sqrt(3/2) */
        else if (j<0xBB67A) k=1;    /* |x|<sqrt(3)   */
        else {k=0;n+=1;ix -= 0x00100000;}
        __HI(ax) = ix;

        /* compute ss = s_h+s_l = (x-1)/(x+1) or (x-1.5)/(x+1.5) */
        u = ax-bp[k];       /* bp[0]=1.0, bp[1]=1.5 */
        v = one/(ax+bp[k]);
        ss = u*v;
        s_h = ss;
        __LO(s_h) = 0;
        /* t_h=ax+bp[k] High */
        t_h = zero;
        __HI(t_h)=((ix>>1)|0x20000000)+0x00080000+(k<<18);
        t_l = ax - (t_h-bp[k]);
        s_l = v*((u-s_h*t_h)-s_h*t_l);
        /* compute log(ax) */
        s2 = ss*ss;
        r = s2*s2*(L1+s2*(L2+s2*(L3+s2*(L4+s2*(L5+s2*L6)))));
        r += s_l*(s_h+ss);
        s2  = s_h*s_h;
        t_h = 3.0+s2+r;
        __LO(t_h) = 0;
        t_l = r-((t_h-3.0)-s2);
        /* u+v = ss*(1+...) */
        u = s_h*t_h;
        v = s_l*t_h+t_l*ss;
        /* 2/(3log2)*(ss+...) */
        p_h = u+v;
        __LO(p_h) = 0;
        p_l = v-(p_h-u);
        z_h = cp_h*p_h;     /* cp_h+cp_l = 2/(3*log2) */
        z_l = cp_l*p_h+p_l*cp+dp_l[k];
        /* log2(ax) = (ss+..)*2/(3*log2) = n + dp_h + z_h + z_l */
        t = (double)n;
        t1 = (((z_h+z_l)+dp_h[k])+t);
        __LO(t1) = 0;
        t2 = z_l-(((t1-t)-dp_h[k])-z_h);
    }

    /* split up y into y1+y2 and compute (y1+y2)*(t1+t2) */
    y1  = y;
    __LO(y1) = 0;
    p_l = (y-y1)*t1+y*t2;
    p_h = y1*t1;
    z = p_l+p_h;
    j = __HI(z);
    i = __LO(z);
    if (j>=0x40900000) {                /* z >= 1024 */
        if (((j-0x40900000)|i)!=0)          /* if z > 1024 */
            return s*huge*huge;         /* overflow */
        else {
            if (p_l+ovt>z-p_h) return s*huge*huge;  /* overflow */
        }
    } else if ((j&0x7fffffff)>=0x4090cc00 ) {   /* z <= -1075 */
        if (((j-0xc090cc00)|i)!=0)      /* z < -1075 */
            return s*tiny*tiny;     /* underflow */
        else {
            if (p_l<=z-p_h) return s*tiny*tiny; /* underflow */
        }
    }
    /*
     * compute 2**(p_h+p_l)
     */
    i = j&0x7fffffff;
    k = (i>>20)-0x3ff;
    n = 0;
    if (i>0x3fe00000) {     /* if |z| > 0.5, set n = [z+0.5] */
        n = j+(0x00100000>>(k+1));
        k = ((n&0x7fffffff)>>20)-0x3ff; /* new k for n */
        t = zero;
        __HI(t) = (n&~(0x000fffff>>k));
        n = ((n&0x000fffff)|0x00100000)>>(20-k);
        if (j<0) n = -n;
        p_h -= t;
    }
    t = p_l+p_h;
    __LO(t) = 0;
    u = t*lg2_h;
    v = (p_l-(t-p_h))*lg2+t*lg2_l;
    z = u+v;
    w = v-(z-u);
    t  = z*z;
    t1  = z - t*(P1+t*(P2+t*(P3+t*(P4+t*P5))));
    r  = (z*t1)/(t1-two)-(w+z*w);
    z  = one-(r-z);
    j  = __HI(z);
    j += (n<<20);
    if ((j>>20)<=0) z = scalbn(z,n);    /* subnormal output */
    else __HI(z) += (n<<20);
    return s*z;
}


double pow(double x, double y)
{
    return __ieee754_pow(x, y);
}

double log1p(double )
{
    return 0;
}

double exp(double )
{
    return 0;
}

double hypot(double , double )
{
    return 0;
}

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 *
 */

/* __ieee754_atan2(y,x)
 * Method :
 *	1. Reduce y to positive by atan2(y,x)=-atan2(-y,x).
 *	2. Reduce x to positive by (if x and y are unexceptional):
 *		ARG (x+iy) = arctan(y/x)   	   ... if x > 0,
 *		ARG (x+iy) = pi - arctan[y/(-x)]   ... if x < 0,
 *
 * Special cases:
 *
 *	ATAN2((anything), NaN ) is NaN;
 *	ATAN2(NAN , (anything) ) is NaN;
 *	ATAN2(+-0, +(anything but NaN)) is +-0  ;
 *	ATAN2(+-0, -(anything but NaN)) is +-pi ;
 *	ATAN2(+-(anything but 0 and NaN), 0) is +-pi/2;
 *	ATAN2(+-(anything but INF and NaN), +INF) is +-0 ;
 *	ATAN2(+-(anything but INF and NaN), -INF) is +-pi;
 *	ATAN2(+-INF,+INF ) is +-pi/4 ;
 *	ATAN2(+-INF,-INF ) is +-3pi/4;
 *	ATAN2(+-INF, (anything but,0,NaN, and INF)) is +-pi/2;
 *
 * Constants:
 * The hexadecimal values are the intended ones for the following
 * constants. The decimal values may be used, provided that the
 * compiler will convert from decimal to binary accurately enough
 * to produce the hexadecimal values shown.
 */

static const double
pi_o_4  = 7.8539816339744827900E-01, /* 0x3FE921FB, 0x54442D18 */
        pi_o_2  = 1.5707963267948965580E+00, /* 0x3FF921FB, 0x54442D18 */
        pi      = 3.1415926535897931160E+00, /* 0x400921FB, 0x54442D18 */
        pi_lo   = 1.2246467991473531772E-16; /* 0x3CA1A626, 0x33145C07 */

double __ieee754_atan2(double y, double x)
{
    double z;
    int k,m,hx,hy,ix,iy;
    unsigned lx,ly;

    hx = __HI(x); ix = hx&0x7fffffff;
    lx = __LO(x);
    hy = __HI(y); iy = hy&0x7fffffff;
    ly = __LO(y);
    if(((ix|((lx|-lx)>>31))>0x7ff00000)||
            ((iy|((ly|-ly)>>31))>0x7ff00000))	/* x or y is NaN */
        return x+y;
    if(((hx-0x3ff00000)|lx)==0) return atan(y);   /* x=1.0 */
    m = ((hy>>31)&1)|((hx>>30)&2);	/* 2*sign(x)+sign(y) */

    /* when y = 0 */
    if((iy|ly)==0) {
        switch(m) {
            case 0:
            case 1: return y; 	/* atan(+-0,+anything)=+-0 */
            case 2: return  pi+tiny;/* atan(+0,-anything) = pi */
            case 3: return -pi-tiny;/* atan(-0,-anything) =-pi */
        }
    }
    /* when x = 0 */
    if((ix|lx)==0) return (hy<0)?  -pi_o_2-tiny: pi_o_2+tiny;

    /* when x is INF */
    if(ix==0x7ff00000) {
        if(iy==0x7ff00000) {
            switch(m) {
                case 0: return  pi_o_4+tiny;/* atan(+INF,+INF) */
                case 1: return -pi_o_4-tiny;/* atan(-INF,+INF) */
                case 2: return  3.0*pi_o_4+tiny;/*atan(+INF,-INF)*/
                case 3: return -3.0*pi_o_4-tiny;/*atan(-INF,-INF)*/
            }
        } else {
            switch(m) {
                case 0: return  zero  ;	/* atan(+...,+INF) */
                case 1: return -zero  ;	/* atan(-...,+INF) */
                case 2: return  pi+tiny  ;	/* atan(+...,-INF) */
                case 3: return -pi-tiny  ;	/* atan(-...,-INF) */
            }
        }
    }
    /* when y is INF */
    if(iy==0x7ff00000) return (hy<0)? -pi_o_2-tiny: pi_o_2+tiny;

    /* compute y/x */
    k = (iy-ix)>>20;
    if(k > 60) z=pi_o_2+0.5*pi_lo; 	/* |y/x| >  2**60 */
    else if(hx<0&&k<-60) z=0.0; 	/* |y|/x < -2**60 */
    else z=atan(fabs(y/x));		/* safe to do y/x */
    switch (m) {
        case 0: return       z  ;	/* atan(+,+) */
        case 1: __HI(z) ^= 0x80000000;
                return       z  ;	/* atan(-,+) */
        case 2: return  pi-(z-pi_lo);/* atan(+,-) */
        default: /* case 3 */
                return  (z-pi_lo)-pi;/* atan(-,-) */
    }
}

#undef __HI
#undef __LO
#undef __HIp
#undef __LOp

[[gnu::noreturn]] void __assert_fail(const char *assertion, const char *file, int line, const char *func)
{
    fprintf(stdout, "assert (%s) failed in %s at %s:%d\n",
            assertion, func, file, line);
    abort();
}

char *inet_ntoa(struct in_addr in)
{
    static char name[17] = {0};

    uint32_t host = ntohl(in.s_addr);

    snprintf(name, 17, "%d.%d.%d.%d",
            (host >> 24 & 0xff),
            (host >> 16 & 0xff),
            (host >> 8 & 0xff),
            (host  & 0xff));
    return name;
}

in_addr_t inet_addr(const char *cp)
{
    int a,b,c,d;

    sscanf(cp, " %u.%u.%u.%u ", &a, &b, &c, &d);

    if (a > 255 || b > 255 || c > 255 || d > 255)
        return (in_addr_t)-1;

    if (a < 0 || b < 0 || c < 0 || d < 0)
        return (in_addr_t)-1;

    in_addr_t ret;

    ret = (a<<24)|(b<<16)|(c<<8)|d;
    return htonl(ret);
}

uint32_t htonl(uint32_t hostlong)
{
    unsigned char data[4] = {0};
    uint32_t ret;

    data[0] = hostlong >> 24;
    data[1] = hostlong >> 16;
    data[2] = hostlong >> 8;
    data[3] = hostlong;

    memcpy(&ret, &data, sizeof(data));

    return ret;
}

uint16_t htons(uint16_t hostshort)
{
    unsigned char data[2] = {0};
    uint16_t ret;

    data[0] = hostshort >> 8;
    data[1] = hostshort;

    memcpy(&ret, &data, sizeof(data));

    return ret;
}

uint32_t ntohl(uint32_t net)
{
    return ((net & 0x000000ff) << 24)
        |  ((net & 0x0000ff00) << 8)
        |  ((net & 0x00ff0000) >> 8)
        |  ((net & 0xff000000) >> 24);
    /*
       unsigned char data[4] = {};
       memcpy(&data, &net, sizeof(data));

       return ((uint32_t) data[3] << 0)
       | ((uint32_t) data[2] << 8)
       | ((uint32_t) data[1] << 16)
       | ((uint32_t) data[0] << 24);*/
}

uint16_t ntohs(uint16_t net)
{
    return ((net & 0x00ff) << 8)
        |  ((net & 0xff00) >> 8);
}

int brk(void *addr)
{
    void *newbrk = (void *)syscall(__NR_brk, (long)addr);
    if (newbrk == NULL || newbrk == _data_end || newbrk != addr) {
        errno = ENOMEM;
        return -1;
    }
    _data_end = newbrk;
    return 0;
}

void *sbrk(intptr_t increment)
{
    void *ret = _data_end;

    if (increment == 0)
        return _data_end;
    if (brk((char *)_data_end + increment))
        return NULL;

    return ret;
}

char *optarg = NULL;
int opterr = 1, optind = 1, optopt = 0;
static int optoff = 1;

int getopt(int argc, char *const argv[], const char *optstring)
{
again:
    if (       optind >= argc
            || optind == 0
            || argv[optind] == NULL
            || *argv[optind] != '-'
            || !strcmp(argv[optind], "-")
       ) {
        optoff = 1;
        optarg = NULL;
        return -1;
    }

    if (!strcmp(argv[optind], "--")) {
        optind++;
        optoff = 1;
        optarg = NULL;
        return -1;
    }

    bool optstring_colon = false;

    /* special handling of first char of optstring */
    if (*optstring == ':') {
        opterr = 0;
        optstring++;
        optstring_colon = true;
    }

    char opt = argv[optind][optoff++];

    if (opt == 0) {
        /* we've exhausted this -options so see if there is another */
        optind++;
        optoff = 1;
        optarg = NULL;
        goto again;
    }

    char *match;
    char ret;

    /* see if the option is valid */
    if ((match = strchr(optstring, opt)) == NULL) {
        if (opterr)
            fprintf(stderr, "%s: illegal option -- %c\n", argv[0], isprint(opt) ? opt : '?');

        ret = '?';
        optoff = 1;
        optopt = opt;
        optarg = NULL;
        goto done;
    }

    optopt = 0;
    ret = *match;

    /* handle the case we have an optstring */
    if (*(match + 1) == ':') {
        /* .. part of existing argv */
        if (argv[optind][optoff]) {
            optarg = &argv[optind++][optoff];
        } else {
            /* must be the next argv */
            optarg = argv[++optind];
            optind++;
        }

        /* failure to find an optarg */
        if (!optarg || !*optarg) {
            optoff = 1;
            optarg = NULL;

            if (opterr)
                fprintf(stderr, "%s: option requires an argument -- %c\n", argv[0], isprint(opt) ? opt : '?');

            if (optstring_colon)
                ret = ':';
            else
                ret = '?';
        }

        optoff = 1;
    } else
        optarg = NULL;

done:
    return ret;
}

char *basename(char *path)
{
    int i = strlen(path) - 1;

    while (i > 0)
    {
        if (path[i-1] == '/')
            return path + i;
        i--;
    }

    return path;
}

char *dirname(char *path)
{
    int i = strlen(path) - 1;
    while (i > 0)
    {
        if (path[i] == '/') {
            path[i] = '\0';
            return path;
        }
        i--;
    }

    return ".";
}

int ioctl(int fd, int request, ...)
{
    int ret;
    void *arg;

    va_list ap;
    va_start(ap, request);
    arg = va_arg(ap, void *);
    va_end(ap);

    ret = (int)syscall(__NR_ioctl, (long)fd, (long)request, (long)arg, 0, 0, 0, 0, 0);

    return ret;
}

/*
   typedef union {
   float f;
   struct {
   unsigned int mantisa : 23;
   unsigned int exponent : 8;
   unsigned int sign : 1;
   } parts;
   } float_t;

   typedef union {
   double d;
   struct {
   unsigned long mantisa : 52;
   unsigned int exponent : 11;
   unsigned int sign : 1;
   } parts;
   } double_t;

   typedef union {
   long double ld;
   struct {
   unsigned long mantisa : 63;
   unsigned int integer : 1;
   unsigned int exponent : 15;
   unsigned int sign : 1;
   } parts;
   } long_double_t;
   */

double strtod(const char *restrict nptr, char **restrict endptr)
{
    const char *src_ptr = nptr;

    while (*src_ptr && isspace(*src_ptr))
        src_ptr++;

    if (*src_ptr == '\0') {
invalid:
        if (endptr)
            *endptr = (char *)src_ptr;
        errno = EINVAL;
        return 0;
    }

    int sign = 0;

    if (*src_ptr == '+') {
        sign = 1;
        src_ptr++;
    } else if (*src_ptr  == '-') {
        sign = -1;
        src_ptr++;
    }

    int state = 0;
    bool got_digit = false;

    unsigned long pre_radix  = 0;
    unsigned long post_radix = 0;
    unsigned long exponent   = 0;

    int exp_sign = 0;

    while (true)
    {
        if (*src_ptr == '\0')
            goto done;

        if (isdigit(*src_ptr)) {
            got_digit = true;
            switch(state)
            {
                case 0: pre_radix *= 10;  pre_radix  += (*src_ptr - '0'); break;
                case 1: post_radix *= 10; post_radix += (*src_ptr - '0'); break;
                case 2: exponent *= 10;   exponent   += (*src_ptr - '0'); break;
            }
        } else if (*src_ptr == '.') {
            if (state == 0)
                state = 1;
            else
                goto invalid;
        } else if (tolower(*src_ptr) == 'e') {
            if (state == 0 || state == 1)
                state = 2;
            else
                goto invalid;
        } else if (*src_ptr == '-') {
            if (state == 2 && exponent == 0)
                exp_sign = -1;
            else
                goto invalid;
        } else
            goto done;

        src_ptr++;
    }
done:
    if (got_digit == false)
        goto invalid;

    printf("strtod=%c%lu.%luE%c%lu\n",
            (sign == 0) ? '0' : (sign == -1) ? '-' : '+',
            pre_radix,
            post_radix,
            exp_sign ? '-' : '0',
            exponent
          );

    return 0;
}

long strtol(const char *restrict nptr, char **restrict endptr, int base)
{
    long ret = 0;
    long neg = 1;
    long oldret;

    if (nptr == NULL || base < 0 || base == 1 || base > 36) {
invalid:
        errno = EINVAL;
        return 0;
    }

    const char *ptr = nptr;

    while (isspace(*ptr)) ptr++;

    if (*ptr == '-' || *ptr == '+') {
        neg = *ptr == '-' ? -1 : 1;
        ptr++;
    }

    if (base == 0) {
        base = calc_base(&ptr);
        if (base == 0)
            goto invalid;
    }

    while (*ptr)
    {
        char c = tolower(*ptr);

        if (isdigit(c)) c = c - '0';
        else if (isalpha(c)) c = 10 + (c - 'a');
        else break;

        oldret = ret;
        ret *= base;
        ret += c;
        if (oldret < 0 && ret > 0) {
            errno = ERANGE;
            ret = LONG_MIN;
            break;
        } else if (oldret > 0 && ret < 0) {
            errno = ERANGE;
            ret = LONG_MAX;
            break;
        }

        ptr++;
    }

    if (endptr) {
        *endptr = (char *)ptr;
    }

    return ret * neg;
}

unsigned long strtoul(const char *restrict nptr, char **restrict endptr, int base)
{
    unsigned long ret = 0, oldret;

    if (nptr == NULL || base < 0 || base == 1 || base > 36) {
invalid:
        errno = EINVAL;
        return 0;
    }

    const char *ptr = nptr;

    while (isspace(*ptr)) ptr++;

    if (base == 0) {
        base = calc_base(&ptr);
        if (base == 0)
            goto invalid;
    }

    while (*ptr)
    {
        char c = tolower(*ptr);

        if (isdigit(c)) c = c - '0';
        else if (isalpha(c)) c = 10 + (c - 'a');
        else break;

        oldret = ret;
        ret *= base;
        ret += c;
        if (ret < oldret) {
            errno = ERANGE;
            ret = ULONG_MAX;
            break;
        }

        ptr++;
    }

    if (endptr)
        *endptr = (char *)ptr;

    return ret;
}

long long strtoll(const char *restrict nptr, char **restrict endptr, int base)
{
    /* FIXME */
    return strtol(nptr, endptr, base);
}

unsigned long long strtoull(const char *restrict nptr, char **restrict endptr, int base)
{
    /* FIXME */
    return strtoul(nptr, endptr, base);
}

double atof(const char *nptr)
{
    return(strtod(nptr, NULL));
}

int atoi(const char *nptr)
{
    return(strtol(nptr, NULL, 10));
}

long atol(const char *nptr)
{
    return(strtol(nptr, NULL, 10));
}

long long atoll(const char *nptr)
{
    return strtoll(nptr, NULL, 10);
}

    __attribute__((nonnull(1), access(read_only, 1), access(write_only, 2)))
static size_t findenv(const char *name, size_t *nlen)
{
    size_t i;
    size_t len;

    if (environ == NULL) {
        return -1;
    }

    len = strlen(name);
    if (nlen)
        *nlen = len;

    for (i = 0; environ[i]; i++)
    {
        if (strncmp(environ[i], name, len))
            continue;

        if (environ[i][len] != '=')
            continue;

        return i;
    }

    return -1;
}

char *getenv(const char *name)
{
    int i;
    size_t len;

    if (name == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if ((i = findenv(name, &len)) == -1) {
        return NULL;
    }

    return (environ[i] + len + 1);
}

static int environ_size(void)
{
    if (environ == NULL)
        return 0;

    int ret = 0;

    while (environ[ret])
        ret++;
    return ret;
}

int setenv(const char *name, const char *value, int overwrite)
{
    if (name == NULL) {
        errno = EINVAL;
        return -1;
    }

    size_t i, es;
    char *new, **tmp;
    size_t len = 0;

    if ((i = findenv(name, &len)) == (size_t)-1) {
        if ((tmp = realloc(environ, ((es = environ_size()) + 2) * sizeof(char *))) == NULL)
            return -1;

        environ = tmp;
        i = es;

        environ[i]   = NULL;
        environ[i+1] = NULL;
        goto set;
    } else {
set:
        if (overwrite == 0)
            return 0;
        len = len + ((value != NULL) ? strlen(value) : 0) + 2;
        if ((new = calloc(1, len)) == NULL)
            return -1;
        snprintf(new, len, "%s=%s", name, value ? value : "");
        environ[i] = new;
    }

    return 0;
}

int unsetenv(const char *name)
{
    if (name == NULL) {
        errno = EINVAL;
        return -1;
    }

    int i,j;

    if ((i = findenv(name, NULL)) == -1)
        return 0;

    for (j = i + 1; environ[j]; i++, j++)
        environ[i] = environ[j];

    environ[i] = NULL;

    char **tmp;
    if ((tmp = realloc(environ, (environ_size() + 2) * sizeof(char *))) == NULL)
        return -1;

    environ = tmp;
    return 0;
}

int chdir(const char *path)
{
    return syscall(__NR_chdir, path);
}

mode_t umask(mode_t mask)
{
    return syscall(__NR_umask, mask);
}

char *getcwd(char *buf, size_t size)
{
    return (char *)syscall(__NR_getcwd, buf, size);
}

int putenv(char *string)
{
    if (string == NULL)
        return -1;

    char *eq, **tmp;
    size_t len, i;

    if ((eq = strchr(string, '=')) == NULL)
        return -1;

    len = eq - string;

    for (i = 0; environ[i]; i++)
    {
        if (strncmp(environ[i], string, len))
            continue;

        if (environ[i][len] != '=')
            continue;

        environ[i] = string;
        return 0;
    }

    if ((tmp = realloc(environ, (i+2) * sizeof(char *))) == NULL)
        return -1;

    environ = tmp;
    environ[i] = string;
    environ[i+1] = NULL;

    return 0;
}

void clearerr(FILE *fp)
{
    fp->eof = false;
    fp->error = 0;
}

void rewind(FILE *fp)
{
    fseek(fp, 0L, SEEK_SET);
    clearerr(fp);
}

int fseek(FILE *fp, long offset, int whence)
{
    if (fp->mem) {
        off_t newoff;

        switch(whence)
        {
            case SEEK_SET:
                newoff = offset;
                break;
            case SEEK_CUR:
                newoff = fp->offset + offset;
                break;
            case SEEK_END:
                newoff = fp->mem_size;
                break;
            default:
                errno = EINVAL;
                return -1;
        }

        if (newoff < 0L || (size_t)newoff > fp->mem_size) {
            errno = EINVAL;
            return -1;
        }

        fp->offset = newoff;
        return 0;
    }

    /* real file */

    off_t rc;

    if ((rc = lseek(fp->fd, offset, whence)) == -1) {
        fp->error = errno;
        return -1;
    }

    fp->offset = rc;
    return 0;
}

static FILE *utx  = NULL;
static int utx_rw = 0;

static int try_open_utx()
{
    if (utx != NULL) {
        if (ferror(utx)) {
            fclose(utx);
            utx = NULL;
            goto try;
        }

        return 0;
    }

try:
    if ((utx = fopen("/run/utmp", "r")) == NULL)
        return -1;

    utx_rw = 0;

    return 0;
}

void setutxent()
{
    if (try_open_utx() == -1)
        return;

    rewind(utx);
}

void endutxent()
{
    if (utx) {
        fclose(utx);
        utx = NULL;
        utx_rw = 0;
    }
}

struct utmpx *getutxent()
{
    if (try_open_utx() == -1)
        return NULL;


    if (fread(&utmpx_tmp, 1, sizeof(utmpx_tmp), utx) != sizeof(utmpx_tmp))
        return NULL;

    return &utmpx_tmp;
}

char *asctime(const struct tm *tm)
{
    if (tm->tm_wday > 6 || tm->tm_wday < 0 || tm->tm_mon > 11 || tm->tm_mon < 0)
        return NULL;

    /* snprintf might overflow the last set of digits */
    uint16_t safe_year = 1900 + tm->tm_year;

    if (safe_year > 9999) safe_year = 9999;

    snprintf(asctime_tmp,
            sizeof(asctime_tmp),
            "%.3s %.3s%3d %.2d:%.2d:%.2d %4u\n",
            wday_name[tm->tm_wday],
            mon_name[tm->tm_mon],
            tm->tm_mday,
            tm->tm_hour,
            tm->tm_min,
            tm->tm_sec,
            safe_year);

    return asctime_tmp;
}

/* Unix time: number of seconds since 1970-01-01T00:00:00Z */

struct tm *gmtime(const time_t *const now)
{
    long secs, days, years, hours, mins, rem_secs;
    long yday, mday = 0;

    if (*now > UINT_MAX || *now < 0) {
        errno = EOVERFLOW;
        return NULL;
    }

    secs = *now;

    /* figure out the values not impacted by the year */
    days = secs / 86400;
    rem_secs = secs % 86400;

    hours = rem_secs / 3600;
    rem_secs = rem_secs % 3600;

    mins = rem_secs / 60;
    rem_secs = rem_secs % 60;

    /* initial view of the year ignoring leap days */
    years = days / 365;
    yday = days - (years * 365);
    years += 1970;

    /* correct for additional leap days */
    yday -= (years-1)/4   - (1970-1)/4;
    yday += (years-1)/100 - (1970-1)/100;
    yday -= (years-1)/400 - (1970-1)/400;

    long cnt = 0, month = 0;
    bool leap = false;

    /* handle the case we've overshot the year */
    if (yday >= 0) {
        leap = (!(years % 4) || !(years % 100)) && (years % 400);
    } else while (yday < 0) {
        years--;
        /* as the year has changed, recalculate if it is a leap year */
        leap = (!(years % 4) || !(years % 100)) && (years % 400);

        yday = 365 + yday + (leap ? 1 : 0);
    }

    /* this is here due to February */
    const long d_in_m[12] = {31,leap ? 29 : 28,31,30,31,30,31,31,30,31,30,31};

    /* figure out which calendar month we're in */
    for (long i = 0; i < 12; i++, month++ ) {
        if (cnt + d_in_m[i] > yday) break;
        cnt += d_in_m[i];
    }

    /* now figure out the day of the month as the 'remainder' */
    mday = yday - cnt + 1;

    /* populate structure to return */
    gmtime_tmp.tm_yday  = yday;
    gmtime_tmp.tm_mday  = mday;
    gmtime_tmp.tm_mon   = month;
    gmtime_tmp.tm_hour  = hours;
    gmtime_tmp.tm_min   = mins;
    gmtime_tmp.tm_year  = years - 1900;
    gmtime_tmp.tm_sec   = rem_secs;
    gmtime_tmp.tm_isdst = 0;

    /* done! */
    return &gmtime_tmp;
}

struct tm *localtime_r(const time_t *restrict timer, struct tm *restrict result)
{
    struct tm *gmt = gmtime(timer);

    memcpy(result, gmt, sizeof(struct tm));
    return result;
}

struct tm *localtime(const time_t *now)
{
    struct tm *gmt = gmtime(now);

    if (gmt == NULL) {
        return NULL;
    }

    memcpy(&localtime_tmp, gmt, sizeof(struct tm));

    return &localtime_tmp;
}

unsigned int sleep(unsigned seconds)
{
    /* TODO */

    struct timespec rem, req = {
        .tv_sec  = seconds,
        .tv_nsec = 0
    };

    if (nanosleep(&req, &rem) == 0)
        return 0;

    return rem.tv_sec;
}

int setpgid(pid_t pid, pid_t pgid)
{
    return syscall(__NR_setpgid, pid, pgid);
}

pid_t setpgrp(void)
{
    setpgid(0, 0);
    return getpgrp();
}

pid_t getpgrp(void)
{
    return syscall(__NR_getpgrp);
}

static void sig_restore(void)
{
    puts("sig_restore\n");
    syscall(__NR_sigreturn);
}

int fsync(int fd)
{
    return syscall(__NR_fsync, fd);
}

int fdatasync(int fd)
{
    return syscall(__NR_fdatasync, fd);
}

int uname(struct utsname *buf)
{
    return syscall(__NR_uname, buf);
}

[[gnu::nonnull(1)]] static char *decode_qname(const unsigned char *qname, ssize_t max_len, ssize_t *used, const unsigned char *root)
{
    const unsigned char *src = NULL;
    const unsigned char *end = NULL;
    ssize_t ret_len;
    char *ret = NULL;
    char *tmp_str = NULL;
    char *ret_ptr = NULL;

    //printf("decode_qname: max_len is %ld\n", max_len);

    /* pass 1 - obtain length of string */
    src = qname;
    end = (void *)(uintptr_t)qname + max_len;
    ret_len = 0;

    while (*src && src < end)
    {
        if (*src == 0xc0) {
            src++;
            if (root) {
                tmp_str = decode_qname(root + *src, max_len, NULL, NULL);
                ret_len += strlen(tmp_str);
                free(tmp_str);
            }
            src++;
            break;
        }
            
        ret_len += *src + 1;
        src += *src;
        src++;
    }

    if (used)
        *used = (src - qname);

    //printf("decode_qname: ret_len is %ld\n", ret_len);

    if ((ret = calloc(1, ret_len + 1)) == NULL)
        return NULL;

    /* pass 2 - convert to an ASCII string */
    src = qname;
    ret_ptr = ret;
    while (*src && src < end)
    {
        if (*src == 0xc0) {
            src++;
            if (root) {
                tmp_str = decode_qname(root + *src, max_len, NULL, NULL);
                strcat(ret, tmp_str);
                ret_ptr += strlen(tmp_str);
                *ret_ptr++ = '.';
                free(tmp_str);
            }
            src++;
            break;
        }
        memcpy(ret_ptr, (char *)src + 1, *src);
        ret_ptr += *src;
        *ret_ptr++ = '.';
        src += *src;
        src++;
    }
    /* remove trailing "." */
    *--ret_ptr = '\0';

    //printf("decode_qname: returning <%s>\n", ret);
    return ret;
}

/* take a hostname:
 *    www.google.com. 
 * and convert to (q)name format:
 *    \003 w w w \006 g o o g l e \003 c o m \000
 */
[[gnu::nonnull(1)]] static unsigned char *encode_qname(const char *name, int *len_out)
{
    char *src_cpy = NULL;
    unsigned char *ret = NULL;

    if ((src_cpy = strdup(name)) == NULL)
        return NULL;

    char *ptr = src_cpy + strlen(src_cpy) - 1;

    /* remove any trailing dot(s) */
    while (*ptr && *ptr == '.')
        *ptr-- = '\0';

    if (!*ptr || !strlen(ptr))
        goto fail;

    ptr = src_cpy;

    /* check the name is valid */
    while (*ptr)
    {
        if (!isalnum(*ptr) && *ptr != '.' && *ptr != '-') {
            errno = EINVAL;
            goto fail;
        }
        ptr++;
    }

    /* +1 - terminating null length root label 
     * +1 - to cover the last label, that has no .
     */
    int name_len = strlen(src_cpy) + 2;
    if ((ret = calloc(1, name_len)) == NULL)
        goto fail;

    int offset = 0;
    const char *tmp = src_cpy;
    const char *old_tmp = src_cpy;

    bool running = true;

    /* pack into (q)name format:
     * (LEN{1} [-A-Z0-9]*)* \000
     */
    while(running)
    {
        int len;
        tmp = strchr(old_tmp, '.');
        if (tmp == NULL) {
            tmp = old_tmp;
            running = false;
            len = src_cpy + name_len - 2 - tmp;
        } else
            len = tmp - old_tmp;

        if (len <= 0 || len > 63) {
            errno = ENAMETOOLONG;
            goto fail;
        }

        /* top two bits are used for something other than len */
        ret[offset++] = (uint8_t)(len & 0x3f);
        memcpy(&ret[offset], old_tmp, len);

        offset += len;
        old_tmp = ++tmp;
    }

    free(src_cpy);
    if (len_out)
        *len_out = name_len;
    return ret;

fail:
    if (src_cpy)
        free(src_cpy);
    if (ret)
        free(ret);
    return NULL;
}

static void free_dns_result(struct dns_result *dr)
{
    for (int i = 0; i < dr->num_records; i++)
        switch (dr->rr[i].record_type)
        {
            case TYPE_SOA:
                if (dr->rr[i].soa.mname)
                    free(dr->rr[i].soa.mname);
                if (dr->rr[i].soa.rname)
                    free(dr->rr[i].soa.rname);
                break;
            case TYPE_MX:
                if (dr->rr[i].mx.string)
                    free(dr->rr[i].mx.string);
                break;
            case TYPE_TXT:
            case TYPE_PTR:
            case TYPE_CNAME:
            case TYPE_NS:
                if (dr->rr[i].string)
                    free(dr->rr[i].string);
                break;
        }

    free(dr);
}

[[gnu::malloc]] static struct dns_result *build_result(const struct dns_rr *rr, int num_rrs, void *root)
{
    struct dns_result *res;

    if ((res = calloc(1, sizeof(struct dns_result) +
                    (sizeof(res->rr[0]) * num_rrs))) == NULL)
        return NULL;

    res->num_records = num_rrs;

    for (int i = 0; i < num_rrs; i++)
    {
        const struct dns_rr *tmp_rr = &rr[i];

        if (tmp_rr->vals.class != CLASS_IN) {
            errno = EINVAL;
            goto fail;
        }

        res->rr[i].record_type = rr[i].vals.type;

        switch(res->rr[i].record_type)
        {
            case TYPE_SOA:
                ssize_t lena, lenb;
                res->rr[i].soa.mname = decode_qname(tmp_rr->additional, tmp_rr->vals.rdlength, &lena, root);
                if (res->rr[i].soa.mname == NULL)
                    goto fail;
                res->rr[i].soa.rname = decode_qname(tmp_rr->additional + lena, tmp_rr->vals.rdlength - lena, &lenb, root);
                if (res->rr[i].soa.rname == NULL)
                    goto fail;
                res->rr[i].soa.serial = ntohl(*(uint32_t *)(tmp_rr->additional + lena + lenb));
                res->rr[i].soa.refresh = ntohl(*(uint32_t *)(tmp_rr->additional + lena + lenb + 4));
                res->rr[i].soa.retry = ntohl(*(uint32_t *)(tmp_rr->additional + lena + lenb + 8));
                res->rr[i].soa.expire = ntohl(*(uint32_t *)(tmp_rr->additional + lena + lenb + 16));
                res->rr[i].soa.minimum = ntohl(*(uint32_t *)(tmp_rr->additional + lena + lenb + 24));
                break;
            case TYPE_MX:
                res->rr[i].mx.weight = ntohs(*(uint16_t *)(tmp_rr->additional));
                res->rr[i].mx.string = decode_qname(tmp_rr->additional + sizeof(uint16_t),
                        tmp_rr->vals.rdlength - sizeof(uint16_t), NULL, root);
                break;
            case TYPE_TXT:
            case TYPE_PTR:
            case TYPE_CNAME:
            case TYPE_NS:
                res->rr[i].string = decode_qname(rr[i].additional, rr[i].vals.rdlength, NULL, root);
                if (res->rr[i].string == NULL)
                    goto fail;
                break;
            case TYPE_A:
                memcpy(&res->rr[i].in_v4.s_addr, rr[i].additional,
                        sizeof(res->rr[i].in_v4.s_addr));
                break;
            default:
                warnx("build_result: unknown type %u\n",
                        res->rr[i].record_type);
                break;
        }
    }

    return res;
fail:
    /* TODO */
    free_dns_result(res);
    return NULL;
}


__attribute__((nonnull))
static void free_dns_rr(struct dns_rr *blk, bool free_it)
{
    if (blk->name)
        free(blk->name);

    if (blk->additional)
        free(blk->additional);

    if (free_it)
        free(blk);
}

__attribute__((nonnull))
static void free_dns_rr_block(struct dns_rr *blk)
{
    int i = 0;

    while(blk[i].allocated)
        free_dns_rr(&blk[i++], false);

    free(blk);

    return;
}

/* TODO make return struct dns_question like process_rr_block */
/* TODO is processing of the _question_block_ actually required? 
 * for now, remove anything but skipping */
[[maybe_unused,gnu::nonnull]] static void process_question_block(const char **const inbuf_ptr,int num_qs, const unsigned char *)
{
    for (int count = 0; count < num_qs; count++)
    {
        //const char *name_start = *inbuf_ptr;

        while(**inbuf_ptr)
        {
            if (**inbuf_ptr & 0xc0) {
                (*inbuf_ptr) += 2;
                goto comp_skip;
            }

            (*inbuf_ptr) += **inbuf_ptr + 1;
        }
        (*inbuf_ptr)++;

comp_skip:
        /* already skiped 0xc0+len above */
        
        /*unsigned char *tmp_qname;

        int len = (*inbuf_ptr) - name_start;

        if ((tmp_qname = calloc(1, len)) == NULL)
            err(EXIT_FAILURE, "calloc");
        memcpy(tmp_qname, name_start, len);*/

        [[maybe_unused]] uint16_t qtype  = ntohs(*((uint16_t *)*inbuf_ptr)); (*inbuf_ptr) += 2;
        [[maybe_unused]] uint16_t qclass = ntohs(*((uint16_t *)*inbuf_ptr)); (*inbuf_ptr) += 2;

        //free(tmp_qname);
    }
}

[[gnu::nonnull]] static struct dns_rr *process_rr_block(const char **const inbuf_ptr, int num_rrs, const unsigned char *)
{
    struct dns_rr *ret = NULL;

    if ((ret = calloc(num_rrs + 1, sizeof(struct dns_rr))) == NULL)
        goto outer_fail;

    for (int count = 0; count < num_rrs; count++)
    {
        /* TODO does this zero each time? */
        struct dns_rr tmp_rr = {0};

        tmp_rr.allocated = true;

        const char *name_start = *inbuf_ptr;

        /* consume each label */
        while(**inbuf_ptr)
        {
            if (**inbuf_ptr & 0xc0) {
                (*inbuf_ptr) += 2;
                goto comp_skip;
            }

            /* skip over the label */
            (*inbuf_ptr) += **inbuf_ptr + 1;
        }

        /* skip over root */
        (*inbuf_ptr)++;

comp_skip:
        int name_len = (*inbuf_ptr) - name_start;

        if ((tmp_rr.name = calloc(1, name_len)) == NULL)
            goto fail;

        /* copy the full (q)name */
        memcpy(tmp_rr.name, name_start, name_len);

        /* parse the rest of the fields */
        tmp_rr.vals.type     = ntohs(*((uint16_t *)*inbuf_ptr)); (*inbuf_ptr) += 2;
        tmp_rr.vals.class    = ntohs(*((uint16_t *)*inbuf_ptr)); (*inbuf_ptr) += 2;
        tmp_rr.vals.ttl      = ntohs(*((uint32_t *)*inbuf_ptr)); (*inbuf_ptr) += 4;
        tmp_rr.vals.rdlength = ntohs(*((uint16_t *)*inbuf_ptr)); (*inbuf_ptr) += 2;

        /* allocate and read the additional block, if any */
        if (tmp_rr.vals.rdlength) {
            if ((tmp_rr.additional = calloc(1, tmp_rr.vals.rdlength)) == NULL)
                goto fail;
            memcpy(tmp_rr.additional, *inbuf_ptr, tmp_rr.vals.rdlength);

            /* skip over it */
            (*inbuf_ptr) += tmp_rr.vals.rdlength;
        }

        /* add to the array */
        memcpy(&ret[count], &tmp_rr, sizeof(tmp_rr));
        continue;
fail:
        if (tmp_rr.name) {
            free(tmp_rr.name);
            tmp_rr.name = NULL;
        }

        if (tmp_rr.additional) {
            free(tmp_rr.additional);
            tmp_rr.additional = NULL;
        }

        goto outer_fail;
    }

    return ret;

outer_fail:
    if (ret)
        free_dns_rr_block(ret);
    return NULL;
}

__attribute__((nonnull))
static char *build_request(const char *name, int *pack_len, int type)
{
    char *buf = NULL;
    struct dns_header hdr = {0};
    unsigned char *qname = NULL;
    int name_len;

    hdr.flags = HDR_RD; /* Recursion Desired */
    hdr.ident = random(); /* TODO we need to check the reply contains this? */
    hdr.num_questions = 1;

    for (int i = 0; i < 6; i++)
        hdr.words[i] = htons(hdr.words[i]);

    if ((qname = encode_qname(name, &name_len)) == NULL)
        goto fail;

    /* TODO none of this is actually needed? just vals? */
    const struct dns_rr rr = {
        .name          = qname,
        .vals.type     = htons(type), /* TYPE_A */
        .vals.class    = htons(CLASS_IN),
        .vals.ttl      = htonl(0),
        .vals.rdlength = htons(0),
        .allocated     = false,
        .additional    = NULL,
    };

    *pack_len = sizeof(hdr) + sizeof(rr.vals) + name_len;

    if ((buf = malloc(*pack_len)) == NULL)
        goto fail;

    memcpy(buf, &hdr, sizeof(hdr));
    memcpy(buf + sizeof(hdr), rr.name, name_len);
    memcpy(buf + sizeof(hdr) + name_len, &rr.vals, sizeof(rr.vals));

    free(qname);
    return buf;

fail:
    if (buf)
        free(buf);
    if (qname)
        free(qname);

    return NULL;
}

/* returns the raw packet data via inbuf & returns length */
static int send_udp4_dns_request(in_addr_t server, in_port_t port, char *inbuf, size_t inlen,
        const char *qstring, int qtype)
{
    int sock_fd = -1;
    int rc = 0;
    char *buf = NULL;
    int pack_len = 0;

    if ((buf = build_request(qstring, &pack_len, qtype)) == NULL) {
        errno = EINVAL;
        rc = -1;
        goto fail;
    }

    if ((rc = sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        goto fail;

    const struct sockaddr_in sin = {
        .sin_family      = AF_INET,
        .sin_port        = htons(port),
        .sin_addr.s_addr = htonl(server),
    };

    if ((rc = connect(sock_fd, (struct sockaddr *)&sin, sizeof(sin))) == -1)
        goto fail;

    rc = write(sock_fd, buf, pack_len);

    if (rc == -1)
        goto fail;
    else if (rc < pack_len) {
        errno = EIO;
        rc = -1;
        goto fail;
    }

    rc = read(sock_fd, inbuf, inlen);

    if (rc == -1)
        goto fail;
    else if ((size_t)rc < sizeof(struct dns_header)) {
        errno = EIO;
        rc = -1;
        goto fail;
    }

fail:
    if (buf)
        free(buf);

    if (sock_fd != -1) {
        shutdown(sock_fd, SHUT_RDWR);
        close(sock_fd);
    }

    return rc;
}

[[maybe_unused]] static long lcg(long modulus, long a, long c, long /* seed */ )
{
    static long lcg_seed = 0;

    lcg_seed = (a * lcg_seed +c ) % modulus;
    return lcg_seed;
}

[[gnu::nonnull(1)]] static int send_request(const char *name, void *result_out, int result_type)
{
    char inbuf[1500];
    int rc;

    if (result_type != TYPE_A) {
        errno = EINVAL;
        return -1;
    }

    if (resolv == NULL && ((resolv = parse_resolv_config()) == NULL)) {
        errno = EIO;
        return -1;
    }

    if (resolv->nameservers_in[which_ns++] == 0)
        which_ns = 0;

    if ((rc = send_udp4_dns_request(resolv->nameservers_in[which_ns], 53,
                    inbuf, sizeof(inbuf), name, TYPE_A)) == -1)
        return -1;

    struct dns_header *hdr = (void *)inbuf;
    for (int count = 0; count < 6; count++)
        hdr->words[count] = ntohs(hdr->words[count]);

    const char *inbuf_end = inbuf + rc;
    const char *inbuf_ptr = inbuf + sizeof(struct dns_header);
    void *tmp = NULL;;
    struct dns_result *result = NULL;

    if (hdr->num_questions) {
        if (inbuf_ptr > inbuf_end) {
            errno = EIO;
            return -1;
        }
        
        process_question_block(&inbuf_ptr, hdr->num_questions, (void *)inbuf);
    }

    if (hdr->num_answers) {
        if (inbuf_ptr > inbuf_end) {
            errno = EIO;
            return -1;
        }

        if ((tmp = process_rr_block(&inbuf_ptr, hdr->num_answers, (void *)inbuf)) == NULL)
            return -1;

        if ((result = build_result(tmp, hdr->num_answers, inbuf)) == NULL) {
            free_dns_rr_block(tmp);
            return -1;
        }
        free_dns_rr_block(tmp);
    }

    /* ignore hdr->num_rrs */
    /* ignore hdr->num_add_rrs */

    if (result) {
        if (result->num_records)
            *(in_addr_t *)result_out = result->rr[0].in_v4.s_addr;
        free_dns_result(result);
    } else {
        return EAI_NONAME;
    }

    return 0;
}

int gethostname(char *name, size_t len)
{
    struct utsname buf;
    int rc;

    if ((rc = uname(&buf)) == -1)
        return rc;

    strlcpy(name, buf.nodename, len);

    if (strlen(name) > len) {
        errno = ENAMETOOLONG;
        return -1;
    }
    
    return 0;
}

int sigaction(int sig, const struct sigaction *restrict act, struct sigaction *restrict oact)
{
    if (act) {
        struct sigaction int_act;
        memcpy(&int_act, act, sizeof(struct sigaction));
        int_act.sa_trampoline = sig_restore;
        /*
        if (int_act.sa_flags & SA_SIGINFO)
            printf("sigaction: { [%02d] sa_sigaction = %p }\n",
                    sig,
                    int_act.sa_sigaction);
        else
            printf("sigaction: { [%02d] sa_handler   = %p }\n",
                    sig,
                    int_act.sa_handler);
                    */
        
        return syscall(__NR_sigaction, sig, &int_act, oact);
    }
    
    return syscall(__NR_sigaction, sig, act, oact);
}

int setitimer(int which, const struct itimerval *restrict value,
        struct itimerval *restrict ovalue)
{
    return syscall(__NR_setitimer, which, value, ovalue);
}

time_t mktime(struct tm *t)
{
    return t->tm_sec + t->tm_min*60 + t->tm_hour*3600 + t->tm_yday*86400 + (t->tm_year-70)*31536000 + ((t->tm_year-69)/4)*86400 - ((t->tm_year-1)/100)*86400 + ((t->tm_year+299)/400)*86400;
}

unsigned int alarm(unsigned int seconds)
{
    struct itimerval new = {
        .it_value = {
            .tv_sec = seconds,
            .tv_usec = 0,
        },
        .it_interval = {0}
    };

    struct itimerval old;

    setitimer(ITIMER_REAL, &new, &old);

    return old.it_value.tv_sec;
}

__sighandler_t signal(int num, __sighandler_t func)
{
    struct sigaction osa, sa = {
        .sa_handler = func,
        .sa_flags = 0,
        .sa_mask = 0,
    };

    if (sigaction(num, &sa, &osa) == -1)
        return NULL;

    return osa.sa_handler;
}

size_t regerror(int errcode, const regex_t *restrict /* preg */ , char *restrict errbuf, size_t size)
{
    int ret;

    if (errcode < 0 || errcode > num_regerrors)
        return 0;

    ret = regerrors[errcode] ? strlen(regerrors[errcode]) : 0;

    if (size && errbuf && ret)
        snprintf(errbuf, size, "%s", regerrors[errcode]);

    return ret;
}

pid_t gettid(void)
{
    return syscall(__NR_gettid);
}

int sigsuspend(const sigset_t *mask)
{
    return syscall(__NR_sigsuspend, mask, (size_t)sizeof(sigset_t));
}

int sigpending(sigset_t *set)
{
    return syscall(__NR_sigpending, set, (size_t)sizeof(sigset_t));
}

int sigtimedwait(const sigset_t *set, siginfo_t *info, const struct timespec *timeout)
{
    return syscall(__NR_sigtimedwait, set, info, timeout, (size_t)sizeof(sigset_t));
}

int sigwait(const sigset_t *set, int *sig)
{
    int rc;
    rc = sigtimedwait(set, NULL, NULL);

    if (rc >= 0) {
        if (sig)
            *sig = rc;

        return 0;
    }

    return errno;
}

int sigwaitinfo(const sigset_t *set, siginfo_t *info)
{
    return sigtimedwait(set, info, NULL);
}

int sigqueue(pid_t pid, int sig, const union sigval value)
{
    siginfo_t uinfo = {
        .si_signo = sig,
        .si_pid   = getpid(),
        .si_uid   = getuid(),
        .si_value = value,
        .si_code  = SI_QUEUE
    };

    return syscall(__NR_sigqueueinfo, pid, sig, &uinfo);
}

int mbsinit(const mbstate_t *ps)
{
    if (ps == NULL)
        return -1;

    if (ps->state == 0)
        return 1;

    return 0;
}

size_t mbstowcs(wchar_t *restrict dest, const char *restrict src, size_t n)
{
    size_t count = n;

    if (src == NULL)
        return (size_t)-1;

    while (*src && count < n)
    {
        /* TODO */
        if (dest) {
            *dest = *src;
            dest++;
        }

        src++;
        count++;
    }

    return count;
}

void wordfree(wordexp_t *p)
{
    if (p->we_wordv) {
        for (int i = 0; p->we_wordv[i]; i++) {
            free(p->we_wordv[i]);
            p->we_wordv[i] = NULL;
        }

        free(p->we_wordv);
        p->we_wordv = NULL;
        p->we_wordc = -1;
        p->flags = 0;
        p->we_offs = -1;
    }
}

[[gnu::nonnull]] static inline bool is_arth(const char *str)
{
    if (str[0] == '$' && str[1] == '(' && str[2] == '(')
        return true;
    return false;
}

[[gnu::nonnull]] static inline bool is_quoted(const char *str)
{
    if (*str == '\'' || *str == '"')
        return true;
    return false;
}

[[gnu::nonnull]] static const char *skip_quoted(const char *start)
{
    if (*start == '"') {
       start++;
       while (*start)
       {
           if (*start == '\\')
               start += 2;
           else if (*start == '"') {
               break;
           } else
               start++;
       }
       if (*start == '"')
           return ++start;
       return NULL;
    }

    start++;
    while (*start && *start != '\'')
        start++;
    if (*start == '\'')
        return ++start;

    return NULL;
}


[[gnu::nonnull]] static bool is_shell(const char *str)
{
    if (*str) {
        if (str[0] == '$' && str[1] == '(')
            return true;
        if (*str == '`')
            return true;
    }

    return false;
}

[[gnu::nonnull]] const char *skip_shell(const char *start)
{
    const char *str = start;
    bool close_tick = false;

    if (str[0] == '`') {
        close_tick = true;
        str++;
        goto begin;
    }

    if (str[0] == '$' && str[1] == '(') {
        str += 2;
        goto begin;
    }

    return start;

begin:

    while (*str)
    {
        if (*str == '\\') {
            str++;
            goto next;
        }

        if (close_tick && *str == '`')
            return ++str;
        if (!close_tick && *str == ')')
            return ++str;

        if (str[0] == '$' && str[1] == '(') {
            str = skip_shell(str);
            continue;
        }
next:
        str++;
    }

    return NULL;
}

[[gnu::nonnull]] static int wrde_tilde(const char **str, wordexp_t * /* p */)
{
    char *dst_ptr, *dst, *newstr;
    char name[256], path[PATH_MAX];
    const char *src_ptr, *tmp, *old_ptr;
    const struct passwd *ent;
    int rc;
    size_t name_len;

    rc = 0;

    if (strchr(*str, '~') == NULL)
        return 0;

    if ((dst_ptr = dst = malloc(strlen(*str))) == NULL) {
        rc = WRDE_NOSPACE;
        goto fail;
    }
    *dst = '\0';

    size_t len;

    for (src_ptr = *str; *src_ptr; src_ptr++, dst_ptr++)
    {
again:
        if (*src_ptr == '\\') {
            if (src_ptr[1] == '\0') {
                rc = WRDE_SYNTAX;
                goto fail;
            }
            *dst_ptr++ = *src_ptr++;
            goto copy;
        } else if (is_shell(src_ptr)) {
            old_ptr = src_ptr;
            src_ptr = skip_shell(src_ptr);
copy_block:
            len = src_ptr - old_ptr;
            memcpy(dst_ptr, old_ptr, len);
            dst_ptr += len;
            *dst_ptr = '\0';
            if (!*src_ptr)
                break;
            goto again;
        } else if (is_quoted(src_ptr)) {
            old_ptr = src_ptr;
            src_ptr = skip_quoted(src_ptr);
            goto copy_block;
            /*
               } else if (*src_ptr == '"') {
               while (*src_ptr) {
               if (*src_ptr == '\\') {
               src_ptr++;
               if (*src_ptr == '"')
               src_ptr++;
               } else if (*src_ptr == '"') {
               break;
               }
               }
               */
        } else if (*src_ptr == '~') {
            for (tmp = src_ptr + 1; *tmp && isascii(*tmp) && !isspace(*tmp); tmp++) {
                /* skip */
                if (*tmp == '\\' && tmp[1])
                    tmp += 2;
                else if (*tmp == '$' || *tmp == '`')
                    break;
            }

            /* ~ on its own */
            if (tmp == src_ptr + 1)
                continue;

            name_len = tmp - src_ptr - 1;
            if (name_len > sizeof(name)) {
                rc = WRDE_NOSPACE;
                goto fail;
            }

            /* skip ~ and extract the username */
            memcpy(name, ++src_ptr, name_len);
            name[name_len] = '\0';

            /* lookup in passwd */
            if ((ent = getpwnam(name)) == NULL) {
                /* no match, copy is literal string */
                printf("fail <%s>\n", name);
                src_ptr--;
                goto copy;
            }

            const size_t path_len = strlen(ent->pw_dir);
            const size_t len = dst_ptr - dst;

            if (path_len > sizeof(path)) {
                rc = WRDE_NOSPACE;
                goto fail;
            }
            /* extract the home dir */
            strcpy(path, ent->pw_dir);

            /* grow the string */
            if ((newstr = realloc(dst, strlen(dst) + path_len + 1)) == NULL) {
                rc = WRDE_NOSPACE;
                goto fail;
            }

            dst = newstr;
            strcpy(dst + len, path);

            /* -1 because loop will ++ */
            dst_ptr  = dst + len + path_len - 1;
            src_ptr += name_len - 1;

        } /* if ~ */ else {
copy:
            *dst_ptr = *src_ptr;
        }
    } /* for */

    free((void *)*str);
    *str = dst;

fail:
    if (rc && dst)
        free(dst);

    return rc;

}

[[gnu::nonnull]] static int wrde_var(const char **str, wordexp_t * /* p */)
{
    char *dst_ptr, *dst, *newstr;
    char name[256];
    const char *src_ptr, *tmp, *old_ptr;
    int rc;
    size_t len;

    rc = 0;

    if ((strchr(*str, '$')) == NULL)
        return 0;

    if ((dst_ptr = dst = malloc(strlen(*str))) == NULL) {
        rc = WRDE_NOSPACE;
        goto fail;
    }
    *dst = '\0';

    for (src_ptr = *str; *src_ptr; src_ptr++, dst_ptr++)
    {
again:
        if (*src_ptr == '\\') {
            src_ptr++;
            goto normal;
        }
        if (is_shell(src_ptr)) {
            old_ptr = src_ptr;
            src_ptr = skip_shell(src_ptr);
copy_block:
            len = src_ptr - old_ptr;
            memcpy(dst_ptr, old_ptr, len);
            dst_ptr += len;
            *dst_ptr = '\0';
            if (!*src_ptr)
                break;
            goto again;
        }
        if (*src_ptr == '\'') {
            old_ptr = src_ptr;
            src_ptr = skip_quoted(src_ptr);
            goto copy_block;
        }
        if (*src_ptr == '$') {
            bool need_brace = false;

            /*if (*(src_ptr + 1) == '(')
                goto normal;*/

            if (*++src_ptr == '{') {
                src_ptr++;
                need_brace = true;
            }

            tmp = src_ptr;

            /* find the end of the variable name */
            while (*tmp) {
                if (need_brace && *tmp == '}')
                    break;
                if (!isalnum(*tmp) && *tmp != '_') /* TODO check this is right */
                    break;
                tmp++;
            }

            /* bail of unclosed } */
            if (need_brace && *tmp != '}') {
                rc = WRDE_SYNTAX;
                goto fail;
            }
            const size_t name_len = tmp - src_ptr;

            if (name_len > sizeof(name)) {
                rc = WRDE_BADVAL;
                goto fail;
            }

            /* get the var name */
            memcpy(name, src_ptr, name_len);
            name[name_len] = '\0';

            /* skip over the var name and optional } */
            src_ptr += name_len - 1; /* loop does ++ */
            if (need_brace)
                src_ptr++;

            /* get the value of the variable, append if one is found */
            const char *val = getenv(name);
            const size_t val_len = val ? strlen(val) : 0;

            if (val && val_len) {
                const size_t len = dst_ptr - dst;
                if ((newstr = realloc(dst, strlen(dst) + val_len + 1)) == NULL) {
                    rc = WRDE_NOSPACE;
                    goto fail;
                }
                dst = newstr;
                dst_ptr = stpcpy(dst + len, val);
            }

            /* loop does ++ */
            dst_ptr--; 
        } else {
normal:
            *dst_ptr = *src_ptr;
        }
    }

    rc = 0;
    free((void *)*str);
    *str = dst;
fail:
    if (rc && dst)
        free(dst);

    return rc;
}

static int wrde_cmd(const char **str, wordexp_t * /*p*/)
{
    const char *src_ptr, *old_ptr;
    char *dst_ptr, *dst;
    int rc;
    size_t len;

    if ((dst_ptr = dst = malloc(strlen(*str))) == NULL) {
        rc = WRDE_NOSPACE;
        goto fail;
    }

    for (src_ptr = *str; *src_ptr; src_ptr++, dst_ptr++)
    {
again:
        if (*src_ptr == '\\') {
            *dst_ptr++ = *src_ptr++;
            goto normal;
        } else if (is_arth(src_ptr)) {
            goto normal;
        } else if (*src_ptr == '\'') {
            old_ptr = src_ptr;
            src_ptr = skip_quoted(src_ptr);
            len = src_ptr - old_ptr;
            memcpy(dst_ptr, old_ptr, len);
            dst_ptr += len;
            *dst_ptr = '\0';
            if (!*src_ptr)
                break;
            goto again;
        } else if (is_shell(src_ptr)) {
            /* TODO - handle the shell here, but recursively */
            src_ptr = skip_shell(src_ptr);
            if (!*src_ptr)
                break;
            goto again;
        } else {
normal:
            *dst_ptr = *src_ptr;
        }
    }
    rc = 0;
    free((void *)*str);
    *str = dst;

fail:
    return rc;
}

static int wrde_arth(const char **str, wordexp_t * /*p*/)
{
    char *dst_ptr, *dst;//, *newstr;
    char buf[BUFSIZ];
    const char *src_ptr, *tmp, *old_ptr;
    int rc;
    size_t len;

    rc = 0;

    if ((strstr(*str, "$((")) == NULL)
        return 0;

    if ((dst_ptr = dst = malloc(strlen(*str))) == NULL) {
        rc = WRDE_NOSPACE;
        goto fail;
    }
    *dst = '\0';

    for (src_ptr = *str; *src_ptr; src_ptr++, dst_ptr++)
    {
again:
        /* TODO check for escape here and in other wrde functions */
        if (*src_ptr == '\\') {
            if (src_ptr[1] == '\0') {
                rc = WRDE_SYNTAX;
                goto fail;
            }
            *dst_ptr++ = *src_ptr++;
            goto copy;
        }
        if (*src_ptr == '\'') {
            old_ptr = src_ptr;
            src_ptr = skip_quoted(src_ptr);
            len = src_ptr - old_ptr;
            memcpy(dst_ptr, old_ptr, len);
            dst_ptr += len;
            *dst_ptr = '\0';
            if (!*src_ptr)
                break;
            goto again;
        }
        if (!strncmp(src_ptr, "$((", 3)) {
            tmp = src_ptr + 3;

            while (*tmp) {
                if (!strncmp(tmp, "))", 2))
                    break;
                tmp++;
            }

            if (!*tmp) {
                rc = WRDE_SYNTAX;
                goto fail;
            }

            src_ptr += 3;

            const size_t buf_len = tmp - src_ptr;

            if (buf_len > sizeof(buf)) {
                rc = WRDE_BADVAL;
                goto fail;
            }

            memcpy(buf, src_ptr, buf_len);
            buf[buf_len] = '\0';

            /* TODO expand arithmetic in buf to dst_ptr */
            
            src_ptr += buf_len + 2; /* loop does ++ */
            dst_ptr--;
        } else {
copy:
            *dst_ptr = *src_ptr;
        }
    }

    rc = 0;
    free((void *)*str);
    *str = dst;

fail:
    if (rc && dst)
        free(dst);

    return rc;
}

static int wrde_field(const char **str, wordexp_t *p, const char *delim_override)
{
    const char *src_ptr, *delim;
    const char *field_start = NULL;
    const char *field_end = NULL;
    size_t field_len = 0;
    int rc = 0;

    if (p->we_offs && p->we_wordv == NULL)
        return WRDE_NOSPACE;

    if (delim_override != NULL) {
        delim = delim_override;
    } else if ((delim = getenv("IFS")) == NULL) {
        delim = " \t\n";
    } else if (strlen(delim) == 0) {
        /* is this right? */
        p->we_wordc = 1;
        if ((p->we_wordv = calloc(2, sizeof(char *))) == NULL) {
            rc = WRDE_NOSPACE;
            goto fail;
        }

        if ((p->we_wordv[0] = strdup(*str)) == NULL) {
            rc = WRDE_NOSPACE;
            goto fail;
        }
        return 0;
    }

    src_ptr = *str;

    while (1)
    {
        /* if we have an IFS (or we have got to the end) prepare to extract */
        if ((strchr(delim, *src_ptr) != NULL) || *src_ptr == '\0') {

            /* only extract, if we've actually got anything to extract */
            if (field_start != NULL) {
                field_end = src_ptr;
                field_len = field_end - field_start;
                
                p->we_wordc++;
                
                if (p->we_wordc > p->we_offs) {
                    /* grow the we_wordv array */
                    char **tmp_array;
                    if ((tmp_array = realloc(p->we_wordv, (p->we_wordc + 1) * sizeof(char **))) == NULL) {
                        rc = WRDE_NOSPACE;
                        goto fail;
                    }
                    p->we_wordv = tmp_array;
                }
            
                if ((p->we_wordv[p->we_wordc - 1] = malloc(field_len + 1)) == NULL) {
                    rc = WRDE_NOSPACE;
                    goto fail;
                }

                /* copy the field out */
                memcpy(p->we_wordv[p->we_wordc-1], field_start, field_len);
                
                /* don't forget to NULL terminate the string */
                p->we_wordv[p->we_wordc - 1][field_len] = '\0';

                /* reset for next field (if any) */
                field_start = NULL;
                field_end = NULL;
                field_len = 0;
            }

            /* skip multiple IFS - TODO is this correct? */
            while (*src_ptr && (strchr(delim, *src_ptr) != NULL)) {
                src_ptr++;
            }

            if (*src_ptr == '\0')
                break;

            /* more data, so start a new field */
            field_start = src_ptr;
            continue;
        }
        
        /* first iteration */
        if (field_start == NULL)
            field_start = src_ptr;

        if (*src_ptr == '\0') {
            /* handle end of the string */
            break;
        } else if (*src_ptr == '\\') {
            /* handle escaped character */
            src_ptr += 2;
        } else if (*src_ptr == '"') {
            /* handle double quoting character */
            
            src_ptr++;
            
            while (*src_ptr)
            {
                if (*src_ptr == '\\') {
                    /* ... including nested escaped characters */
                    src_ptr += 2;
                } else if (*src_ptr == '"') {
                    src_ptr++;
                    goto double_quote_out;
                } else {
                    /* ... trailing " */
                    src_ptr++;
                }
            }
double_quote_out:
        } else if (*src_ptr == '\'') {
            /* handle single quoting character */
            
            src_ptr++;

            while (*src_ptr && *src_ptr != '\'')
                src_ptr++;
            /* ... trailing ' */
            src_ptr++;
        } else
            /* handle anything else */
            src_ptr++;
    }

fail:
    return rc;
}

/* TODO: this should operate in each member of p, as wrde_field will have split */
static ssize_t wrde_wildcard(wordexp_t *p)
{
    int rc = 0;
    size_t we_cnt;
    for (we_cnt = 0; rc == 0 && we_cnt < p->we_wordc; we_cnt++)
    {
        //printf("wrde_wildcard: [%d/%ld] (%s)\n", we_cnt, p->we_wordc, p->we_wordv[we_cnt]);

        glob_t pglob = {
            .gl_pathv = NULL,
            .gl_pathc = 0,
            .gl_offs = 0,
        };

        if ((rc = glob(p->we_wordv[we_cnt], GLOB_NOCHECK, NULL, &pglob)) != 0) {
            switch (rc)
            {
                case GLOB_NOSPACE:
                    rc = WRDE_NOSPACE;
                    break;
                default:
                    rc = WRDE_SYNTAX;
                    break;
            }
            //warnx("wrde_wildcard: glob failed on <%s>", p->we_wordv[we_cnt]);
            goto fail;
        }

        if (pglob.gl_pathc == 0)
            continue;

        //printf("wrde_wildcard: got %ld entries\n", pglob.gl_pathc);

        char **new_we_wordv;
        size_t new_size;
        new_size = p->we_wordc + pglob.gl_pathc;
        //printf("wrde_wildcard: resizing to %ld\n", new_size);
        if ((new_we_wordv = realloc(p->we_wordv, (new_size+1) * sizeof(char *))) == NULL) {
            warn("wrde_wildcard: realloc");
            rc = WRDE_NOSPACE;
            goto fail;
        }
        new_we_wordv[p->we_wordc + pglob.gl_pathc] = NULL;
        //printf("wrde_wildcard: realloc OK\n");
        
        free(p->we_wordv[we_cnt]);

        /*
         * 0      -> we_cnt-1: do nothing
         * we_cnt -> we_cnt+gl_pathc-1: replace we_cnt & insert others
         * we_cnt+gl_pathc -> we_wordc+gl_pathc-1: move we_cnt+1 here
         */

        /* move subsequent wordv[]s to the end */
        for (size_t i = we_cnt + pglob.gl_pathc, j = we_cnt + 1; i < p->we_wordc + pglob.gl_pathc - 1; i++, j++) {
            //printf("wrde_wildcard: moving %d(%s) to %d\n", j, new_we_wordv[j], i);
            new_we_wordv[i] = new_we_wordv[j];
        }
        //printf("wrde_wildcard: move 1 ok\n");

        /* insert globv[]s */
        for (size_t i = we_cnt, j = 0; j < pglob.gl_pathc; i++, j++)
        {
            //printf("wrde_wildcard: setting wordv[%d] to pathv[%d] (%s)\n", i, j, pglob.gl_pathv[j]);
            if ((new_we_wordv[i] = strdup(pglob.gl_pathv[j])) == NULL) {
                warn("wrde_wildcard: strdup");
                rc = WRDE_NOSPACE;
                goto fail;
            }
        }
        //printf("wrde_wildcard: move 2 ok\n");

        p->we_wordc += pglob.gl_pathc - 1;
        p->we_wordv = new_we_wordv;
        we_cnt += pglob.gl_pathc;

        //printf("wrde_wildcard: now [%d/%ld]\n", we_cnt, p->we_wordc);
fail:
        if (pglob.gl_pathc)
            globfree(&pglob);
    }
    //printf("wrde_wildcard: done\n");

    return rc ? rc : (ssize_t)we_cnt;
}

static int wrde_rmquote(wordexp_t *p)
{
    int rc = 0;
    for (size_t i = 0; rc == 0 && i < p->we_wordc; i++)
    {
        const char *src_ptr;
        char *dst_ptr, *dst;
        char *str = p->we_wordv[i];
        size_t len = strlen(str);

        if ((dst_ptr = dst = malloc(len)) == NULL) {
            rc = WRDE_NOSPACE;
            goto fail;
        }

        for (src_ptr = str; *src_ptr; src_ptr++, dst_ptr++) 
        {
            if (*src_ptr == '\\') {
                *dst_ptr = *++src_ptr;
            } else if (*src_ptr == '\'') {
                src_ptr++;
                while (*src_ptr && *src_ptr != '\'')
                    *dst_ptr++ = *src_ptr++;
                if (*src_ptr == '\0') {
                    rc = WRDE_SYNTAX;
                    goto fail;
                }
                dst_ptr--;
            } else if (*src_ptr == '"') {
                src_ptr++;
                while (*src_ptr) {
                    if (*src_ptr == '\\') {
                        *dst_ptr++ = *++src_ptr;
                    } else if (*src_ptr == '"') {
                        break;
                    } else {
                        *dst_ptr++ = *src_ptr++;
                    }
                }
                if (*src_ptr == '\0') {
                    rc = WRDE_SYNTAX;
                    goto fail;
                }
                dst_ptr--;
            } else
                *dst_ptr = *src_ptr;
        }
        *dst_ptr = '\0';

        rc = 0;
        free(p->we_wordv[i]);
        p->we_wordv[i] = dst;
        dst_ptr = dst = NULL;

fail:
    }
    return rc;
}

int fmtmsg(long classification, const char *label, int severity, const char *text,
        const char *action, const char *tag)
{
    int rc = MM_OK;
    FILE *output[2] = {
        NULL,
        NULL
    };

    if (classification & MM_PRINT)
        output[0] = stderr;

    if (classification & MM_CONSOLE)
        if ((output[1] = fopen("/dev/console", "a")) == NULL)
            rc = MM_NOCON;

    if ( (classification & (MM_PRINT|MM_CONSOLE)) == 0 )
        output[0] = stderr;

    if (strchr(label, ':') == NULL) {
        rc = MM_NOTOK;
        goto fail;
    }

    if (severity < 0 || severity > MM_HALT) {
        rc = MM_NOTOK;
        goto fail;
    }

    const bool has_to_fix = action || tag;
    FILE *fp;

    for (int i = 0; i < 2; i++)
    {
        if ( (fp = output[i]) == NULL)
            continue;

        if (fprintf(fp, "%s %s: %s\n",
                    label ? label : "",
                    mm_sevs[severity],
                    text ? text : "") < 0)
            rc = fp == stderr ? MM_NOMSG : MM_NOCON;

        if (has_to_fix) {
            if (fprintf(fp, "TO FIX: %s %s\n",
                        action ? action : "",
                        tag ? tag : "") < 0)
                rc = fp == stderr ? MM_NOMSG : MM_NOCON;
        }
    }

fail:
    if (output[1])
        fclose(output[1]);
    return rc;
}

int wordexp(const char *restrict s, wordexp_t *restrict p, int flags)
{
    int rc = 0;
    errno = -ENOSYS;
    const char *tmp;

    tmp = NULL;
    p->flags = flags;
    p->we_wordv = NULL;

    if (flags & WRDE_DOOFFS) {
        if ((p->we_wordv = calloc(p->we_offs, sizeof(char *))) == NULL)
            return WRDE_NOSPACE;
    } else {
        p->we_wordv = NULL;
        p->we_offs = 0;
    }

    if ((tmp = strdup(s)) == NULL) {
        rc = WRDE_NOSPACE;
        goto fail;
    }

    printf("wrde_start:  <%s>\n", tmp);
    if ((rc = wrde_tilde(&tmp, p)) < 0)
        goto fail;
    printf("wrde_tilde:  <%s>\n", tmp);
    if ((rc = wrde_var(&tmp, p)) < 0)
        goto fail;
    printf("wrde_var:    <%s>\n", tmp);
    if (!(flags & WRDE_NOCMD)) {
        if ((rc = wrde_cmd(&tmp, p)) < 0)
            goto fail;
        //printf("wrde_cmd:    <%s>\n", tmp);
    }
    if ((rc = wrde_arth(&tmp, p)) < 0)
        goto fail;
    printf("wrde_arth:   <%s>\n", tmp);
    if (flags & WRDE_PRIVATE_SHELL)
        rc = wrde_field(&tmp, p, " \t");
    else
        rc = wrde_field(&tmp, p, NULL);

    if (rc < 0)
        goto fail;

    if (p->we_wordc && p->we_wordv == NULL) {
        rc = WRDE_NOSPACE;
        goto fail;
    }
    for (size_t i = 0; i < p->we_wordc; i++)
        printf("wrde_field [%lu]: <%s>\n", i, p->we_wordv[i]);
    
    if ((rc = wrde_wildcard(p)) < 0)
        goto fail;
    for (size_t i = 0; i < p->we_wordc; i++)
        printf("wrde_wildcd[%lu]: <%s>\n", i, p->we_wordv[i]);

    if ((rc = wrde_rmquote(p)) < 0)
        goto fail;
    for (size_t i = 0; i < p->we_wordc; i++)
        printf("wrde_rmquot[%lu]: <%s>\n", i, p->we_wordv[i]);

    rc = 0;

fail:
    if (rc < 0)
        wordfree(p);

    if (tmp)
        free((void *)tmp);

    return rc;
}

static int glob_check_one(const char *glb, const char *str, bool match_all, const glob_t *pglob)
{
    const char *glb_ptr = glb;
    const char *str_ptr = str;
    bool escape = false;
    errno = 0;

    /* is this right? */
    if (*glb_ptr == '"' || *glb_ptr == '\'')
        return strncmp(glb_ptr + 1, str_ptr, strlen(glb_ptr) - 2);

    while(*glb_ptr && *str_ptr)
    {
        if (escape)
            goto normal;

        switch (*glb_ptr)
        {
            case '\\':
                if ((pglob->glp_flags & GLOB_NOESCAPE))
                    goto normal;
                escape = true;
                goto next_noescape;
            case '[':
                {
                    const char *tmp_ptr;
                    char scanset[BUFSIZ];
                    size_t len;
                    tmp_ptr = glb_ptr+1;

                    /* find the end of the scanset */
                    while (*tmp_ptr)
                    {
                        if ((!(pglob->glp_flags & GLOB_NOESCAPE)) && *tmp_ptr == '\\' && *(tmp_ptr+1) == ']')
                            tmp_ptr++;
                        else if (*tmp_ptr == ']')
                            break;
                        tmp_ptr++;
                    }

                    /* we use EAGAIN to signal that more characters might fix
                     * the issue */
                    if (*tmp_ptr == '\0') {
                        errno = EAGAIN;
                        return -1;
                    }

                    len = tmp_ptr - glb_ptr - 1;
                    strlcpy(scanset, glb_ptr + 1, len);
                    scanset[len] = '\0';
                    expand_scanset(scanset, '!');

                    /* need to update to switch from '^' and fix the shit static use*/
                    if (!is_valid_scanset(scanset, *str_ptr, '!'))
                        goto match_fail;

                    glb_ptr = tmp_ptr;
                    goto next;
                }
                break;
            case '*':
                {
                    char match_char;
                    glb_ptr++;

                    /* gobble up *** */
                    while (*glb_ptr == '*')
                        glb_ptr++;
                    
                    /* if * is end of string, just accept */
                    if (*glb_ptr == '\0')
                        goto match_ok;

                    /* if escaped, take match_char literally */
                    if ((!(pglob->glp_flags & GLOB_NOESCAPE)) && *glb_ptr == '\\')
                        match_char = *(++glb_ptr);
                    /* if it's a ? then break the * parsing  */
                    else if (*glb_ptr == '?')
                        continue;
                    /* otherwise, the get the char after * */
                    else
                        match_char = *glb_ptr;
                    
                    /* skip over the match_char */
                    glb_ptr++;
                    
                    /* keep going till we run out of input or match_char */
                    while (*str_ptr) {
                        if (*str_ptr == match_char) {
                            /* * is gready, so gobble up all but the last match_char */
                            while (*(str_ptr+1) && *(str_ptr+1) == match_char) {
                                str_ptr++;
                            }
                            break;
                        }
                        str_ptr++;
                    }

                    /* we failed to match the match_char */
                    if (*str_ptr == '\0') {
                        goto match_fail;
                    }

                    /* skip the match_char as we matched already */
                    str_ptr++;

                    /* bypass normal: */
                    continue;
                }
            case '?':
                goto next;
            default:
normal:
                if (*glb_ptr != *str_ptr)
                    goto match_fail;
                break;
        }
next:
        escape = false;
next_noescape:
        glb_ptr++;
        str_ptr++;
    }

    if (match_all && (*glb_ptr || *str_ptr)) {
match_fail:
        return 1;
    }

match_ok:
    return 0;
}

/* TODO add in the err function ptr from glob() so individual readdir failures
 * can be reported? */

/* expands a single directory path segment using globbing rules, returning 
 * the result array of char * in split */
static int glob_expand_entry(const char *ent, char **split[], const glob_t *pglob)
{
    static const int grow_size = 100;
    DIR *dir;
    struct dirent *dent;
    char **dir_list = NULL;
    int num_entries = 0;
    int act_entries = 0;
    int rc = 0;

    if ((dir = opendir(".")) == NULL)
        return GLOB_ABORTED;

    /* obtain a list of all entries in the current working directory */
    errno = 0;
    while ((dent = readdir(dir)) != NULL)
    {
        //printf("expand_entry: readdir found <%s>[%d]\n", dent->d_name, dent->d_reclen);
        if (dent->d_reclen == 0 || !strcmp(".", dent->d_name) || !strcmp("..", dent->d_name))
            continue;
        if (act_entries == num_entries) {
            //printf("expand_entry: expanding %d/%d\n", act_entries, num_entries);
            char **tmp_list;
            if ((tmp_list = realloc(dir_list, sizeof(char *) * (num_entries + grow_size))) == NULL) {
                rc = GLOB_NOSPACE;
                closedir(dir);
                goto fail;
            }
            dir_list = tmp_list;
            
            /* ensure the traling entries are NULL, as realloc() does not claer */
            for (int i = num_entries; i < num_entries + grow_size; i++)
                dir_list[i] = NULL;
            
            num_entries += grow_size;
        }

        if ((dir_list[act_entries++] = strdup(dent->d_name)) == NULL) {
            rc = GLOB_NOSPACE;
            closedir(dir);
            goto fail;
        }
    }
    /* we've finished with the DIR now */
    closedir(dir);

    char buf[BUFSIZ];
    const char *src; 
    char *dst;

    /* iterate over each directory entry, comparing with ent */
    for (src = ent, dst = buf; *src; src++, dst++) {

        /* next character is escaped */
        /* not sure this is needed ...
        if (*src == '\\') {
            src++;
            goto check;
        }
        */

        /* TODO logic for stuff that needs escaping here? */
//check:
        *dst = *src;
        *(dst+1) = '\0';

        /* compare the 1..n characters so far copied to buf against this entry */
        for (int i = 0; i < num_entries; i++) {
            /* skip entries already discounted */
            if (dir_list[i] == NULL)
                continue;

            if ((rc = glob_check_one(buf, dir_list[i], *(src+1) == '\0', pglob))) {
                /* the entry doesn't match, so remove it for the next check */
                if (rc == -1 && errno == EAGAIN && (*src == '\0' || *(src+1) == '\0')) {
                    rc = GLOB_ABORTED;
                    goto fail;
                } else if (rc == -1 && errno == EAGAIN) {
                    continue;
                } else if (rc == -1) {
                    rc = GLOB_ABORTED;
                    goto fail;
                }
                act_entries--;
                free(dir_list[i]);
                dir_list[i] = NULL;
            }
        }
    }

    /* nothing matched */
    if (act_entries == 0) {
        rc = 0; //GLOB_NOMATCH;
        goto fail;
    }

    /* prepare the array of matched strings */
    if ((*split = calloc(act_entries + 1, sizeof(char *))) == NULL) {
        rc = GLOB_NOSPACE;
        goto fail;
    }

    /* extract each matched string */
    for (int idx = 0, i = 0; i < num_entries; i++) {
        if (dir_list[i] == NULL)
            continue;

        /* 'move' the malloc() pointer from dir_list to split */
        (*split)[idx++] = dir_list[i];
        dir_list[i] = NULL;
    }

    *dst = '\0';
    rc = act_entries;

    /* TODO this is wrong ent shouldn't be touched any more? */
    //free((void *)*ent);
    //if ((*ent = strdup(buf)) == NULL)
    //    return GLOB_NOSPACE;
fail:

    /* in success, dir_list will be full of NULLs */
    if (dir_list) {
        for (int i = 0; i < num_entries; i++)
            if (dir_list[i]) {
                free(dir_list[i]);
                dir_list[i] = NULL;
            }

        free(dir_list);
    }
    return rc;
}

static const char **glob_do_part(int part, glob_t *pglob)
{
    const char **ret_list = NULL;
    const char ***child_ents = NULL;
    bool *include = NULL;
    int num_ents = 0;
    int rc = 0;
    //int ret_list_cnt = 0;

    if (pglob->glp_presplit == NULL || pglob->glp_presplit[part] == NULL) {
        pglob->glp_rc = GLOB_NOMATCH; // ??
        return NULL;
    }

    //const char *part_name = pglob->glp_presplit[part]; // for debugging
    //printf("glob_do_part: %d:%10s\n", part, part_name);
    
    /* not sure how better to handle this, as no other
     * entries will have / in it? */
    if (!strcmp(pglob->glp_presplit[part], "/")) {
        pglob->glp_postsplit[part] = calloc(2, sizeof(char *));
        pglob->glp_postsplit[part][0] = strdup("/");
        num_ents = rc = 1;
        goto skip;
    }

    //printf("glob_do_part: glob_expand_entry(%s)\n",
            //pglob->glp_presplit[part]);
    if ((num_ents = rc = glob_expand_entry(pglob->glp_presplit[part],
                    &pglob->glp_postsplit[part], pglob)) <= 0) {
        //printf("glob_do_part: glob_expand_entry fail: %d\n", rc);
        goto fail;
    }

skip:
    char buf[PATH_MAX];

    if (getcwd(buf, sizeof(buf)) == NULL) {
        warn("glob_do_part: getcwd");
        rc = GLOB_ABORTED;
        goto fail;
    }

    bool is_last = part == (pglob->glp_presplit_cnt - 1);

    /* this array stores if the entry is to be included (or not) */
    if ((include = calloc(1, num_ents * sizeof(bool))) == NULL) {
        warn("glob_do_part: malloc(include)");
        rc = GLOB_NOSPACE;
        goto fail;
    }

    /* this array stores an array-pointer to 1..n recursive results */
    if ((child_ents = calloc(1, num_ents * sizeof(char **))) == NULL) {
        warn("glob_do_part: malloc(child_ents)");
        rc = GLOB_NOSPACE;
        goto fail;
    }

    /* everything in this list matched, but we need to check this is a
     * partial path match vs full*/
    for (int j = 0; j < num_ents; j++)
    {
        include[j] = false;
        child_ents[j] = NULL;

        /* if we're not the last path part, attempt to chdir (to see if it's
         * a directory */
        //printf("glob_do_part: checking %s\n", pglob->glp_postsplit[part][j]);
        if (!is_last) {
            if (chdir(pglob->glp_postsplit[part][j]) == 0) {
                //printf("glob_do_part: now in %s\n", pglob->glp_postsplit[part][j]);
                pglob->glp_rc = 0;
                const char **valid_ents = glob_do_part(part + 1, pglob);

                if (chdir(buf) == -1)
                    err(EXIT_FAILURE, "glob_do_part: unable to chdir back to %s", buf);
                
                if (valid_ents) {
                    include[j] = true;
                    child_ents[j] = valid_ents;
                }  else if (pglob->glp_rc) {
                    //printf("glob_do_part: global error\n");
                    rc = pglob->glp_rc;
                    goto fail;
                }
                
                continue;
            } 

            if (errno != ENOTDIR && !(pglob->glp_flags & GLOB_ERR)) {
                //printf("glob_do_part: error\n");
                rc = GLOB_ABORTED;
                goto fail;
            }
        } else
            /* successful matches on the last path-part must be included */
            include[j] = true;
        
        /* non-last path-parts that are also non-directories are a non-match ? */
    }

    /*
    ret_list_cnt = 0;
    for (int j = 0; j < num_ents; j++) {
        if (include[j])
            ret_list_cnt++;
    }
    */

    /* prepare the initial list of returns */
    //if ( (ret_list = malloc((ret_list_cnt + 1) * sizeof(char *))) == NULL) {
    if ( (ret_list = malloc((num_ents + 1) * sizeof(char *))) == NULL) {
        rc = GLOB_NOSPACE;
        goto fail;
    }

    /* dupe them */
    for (int j = 0; j < num_ents; j++)
    {
        if (include[j])
            //ret_list[i++] = strdup(pglob->glp_postsplit[part][j]);
            ret_list[j] = strdup(pglob->glp_postsplit[part][j]);
        else {
            ret_list[j] = NULL;
        }
        free(pglob->glp_postsplit[part][j]);
        pglob->glp_postsplit[part][j] = NULL;
    }
    free(pglob->glp_postsplit[part]);
    pglob->glp_postsplit[part] = NULL;

    ret_list[num_ents/*ret_list_cnt*/] = NULL;

fail:
    if (include) {
        free(include);
        include = NULL;
    }

    if (rc < 0 || ret_list == NULL) {
        if (ret_list) {
            for (int i = 0; i < num_ents /*ret_list_cnt*/; i++)
            {
                if (ret_list[i]) {
                    free((void *)ret_list[i]);
                    ret_list[i] = NULL;
                }
            }
            free(ret_list);
            ret_list = NULL;
        }
        
        if (child_ents) {
            for (int i = 0; i < num_ents; i++)
            {
                if (child_ents[i]) {
                    free(child_ents[i]);
                    child_ents[i] = NULL;
                }
            }
            free(child_ents);
            child_ents = NULL;
        }
    }
    if (ret_list) {
        /* calculate total number of entries to return, including those
         * from recursive calls */
        int new_cnt = 0;
        for (int i = 0; i < num_ents; i++)
        {
            /* invalid as different size to child_ents? */
            //if (ret_list[i] == NULL)
            //    continue;

            if (child_ents[i] != NULL) {
                for (int j = 0; child_ents[i][j]; j++)
                    new_cnt++;
            } else {
                new_cnt++;
            }
        }

        const char **new_ret_list;
        if ((new_ret_list = malloc((new_cnt + 1) * sizeof(char *))) == NULL) {
            rc = GLOB_NOSPACE;
            goto fail;
        }

        new_ret_list[new_cnt] = NULL;
        int pos = 0;

        /* build the list of this + recursive results */
        
        //printf("ret_list - part=%d:\n", part);
        //for (int i = 0; i < num_ents /*ret_list_cnt*/; i++)
        /*    printf("ret_list[%d]=%s\n", i, ret_list[i]);

        for (int i = 0; i < num_ents; i++)
            if (child_ents[i])
                for (int j = 0; child_ents[i][j]; j++)
                    printf("child_ents[%d][%d]=%s\n", i, j, child_ents[i][j]);
            else
                printf("child_ents[%d]=NULL\n", i);*/

        for (int i = 0; i < num_ents /*ret_list[i]*/; i++)
        {
            /* TODO better way of doing this? */
            const bool add_slash = strcmp("/", ret_list[i]);
            
            if (child_ents[i]) {
                /* merge in all child results, concat path-parts */
                for (int j = 0; child_ents[i][j]; j++)
                {
                    snprintf(buf, sizeof(buf), "%s%s%s", ret_list[i], add_slash ? "/" : "", child_ents[i][j]);
                    
                    free((void *)child_ents[i][j]);
                    child_ents[i][j] = NULL;
                    
                    new_ret_list[pos++] = strdup(buf);
                }
                //free(child_ents[i]);
                //child_ents[i] = NULL;

                /* TODO confirm this tidy up is needed */
                if (ret_list[i]) {
                    free((void *)ret_list[i]);
                    ret_list[i] = NULL;
                }
            } else if (ret_list[i]) {
                /* 'move' the pointer */
                new_ret_list[pos++] = ret_list[i];
                ret_list[i] = NULL;
            }
            /* skip ret_list[i] == NULL */
        }
        new_ret_list[pos] = NULL;
        
        for (int i = 0; i < num_ents; i++)
            if (child_ents[i]) {
                free(child_ents[i]);
                child_ents[i] = NULL;
            }
        free(child_ents);
        //printf("glob_do_part: returning %d\n", pos);

        free(ret_list);
        ret_list = new_ret_list;

        if (part == 0) {
            pglob->gl_pathc = pos;
            pglob->gl_pathv = (char **)new_ret_list;
        }
        
        rc = 0;
    }

    //printf("glob_do_part: setting rc to %d\n", rc);
    pglob->glp_rc = rc;
    return ret_list;
}

static void globfree_private(glob_t *pglob)
{
    if (pglob->glp_postsplit) {
        for (int i = 0; pglob->glp_postsplit[i]; i++)
        {
            for (int j = 0; pglob->glp_postsplit[i][j]; j++) 
            {
                free(pglob->glp_postsplit[i][j]);
                pglob->glp_postsplit[i][j] = NULL;
            }
            free(pglob->glp_postsplit[i]);
            pglob->glp_postsplit[i] = NULL;
        }
        free(pglob->glp_postsplit);
        pglob->glp_postsplit = NULL;
    }

    if (pglob->glp_presplit) {
        for (int i = 0; i < pglob->glp_presplit_cnt; i++)
        {
            if (pglob->glp_presplit[i]) {
                free(pglob->glp_presplit[i]);
                pglob->glp_presplit[i] = NULL;
            }
        }
        free(pglob->glp_presplit);
        pglob->glp_presplit = NULL;
    }

}

int glob(const char *restrict pattern, int flags, int (*)(const char *epath, int eerrno), glob_t *restrict pglob)
{
    char buf[BUFSIZ];
    char cwd[PATH_MAX];
    const char *ptr;
    char *tmp;
    char **new_p;
    int rc = 0;

    pglob->gl_pathc = 0;
    pglob->glp_presplit_cnt = 0;
    pglob->glp_presplit = NULL;
    pglob->glp_postsplit = NULL;
    pglob->glp_flags = flags;
    pglob->glp_rc = 0;
    /* TODO */
    pglob->gl_pathv = NULL;
    pglob->gl_pathc = 0;

    if (getcwd(cwd, sizeof(cwd)) == NULL)
        return GLOB_NOSPACE;

    if (pattern == NULL)
        return GLOB_ABORTED;

    memset(buf, 0, sizeof(buf));
    tmp = buf;

    /* FIXME //// handling is shit */
    for (ptr = pattern; *ptr;)
    {
        /* TODO handle quoted parts: for " \ only escapes "
         * for ' \ escapes nothing */

        /* only escape path splits, else \* turns into * which isn't what the 
         * user wanted */
        if ((!(pglob->glp_flags & GLOB_NOESCAPE)) && *ptr == '\\' && *(ptr+1) == '/') {
            ptr++;
            goto copy;
        }

        if (*(ptr+1) == '\0') {
            *tmp++ = *ptr;
            goto comp;
        }

        if (*ptr == '/') {
            /* skip all but the last '/' */
            if (ptr != pattern)
                while (*(ptr+1) == '/')
                    ptr++;
comp:
            /* grow the path component array */
            if ((new_p = realloc(pglob->glp_presplit, sizeof(char *) * (pglob->glp_presplit_cnt + 2))) == NULL) {
                warn("realloc");
                rc = GLOB_NOSPACE;
                goto fail;
            }
            pglob->glp_presplit = new_p;
            size_t len = tmp - buf;
            char *pre_str = NULL;

            /* extract the current path-part from buf[] */
            if (ptr == pattern && *ptr == '/') {
                /* skip all but the last '/' */
                while (*(ptr+1) == '/')
                    ptr++;

                len = 0;
                pre_str = NULL;
                /* if this is the first '/' we do some kludge and keep the literal '/' */
                if ((pglob->glp_presplit[pglob->glp_presplit_cnt] = strdup("/")) == NULL) {
                    warn("strdup");
                    rc = GLOB_NOSPACE;
                    goto fail;
                }
            } else if (len && (pre_str = pglob->glp_presplit[pglob->glp_presplit_cnt] = strndup(buf, len)) == NULL) {
                /* otherwise we extract the path-part (this also gives the goto above) */
                warn("glob: strndup(%d)", len);
                rc = GLOB_NOSPACE;
                goto fail;
            }
            
            /* TODO these two if {} blocks might be redundant / crap */
            if (len && pre_str && !strcmp("/", pre_str)) {
                free(pre_str);
                pre_str = NULL;
                continue;
            }
            if (len>1 && pre_str) {
                char *tmp;
                tmp = pre_str + len - 1;
                while (tmp > pre_str && *tmp == '/')
                {
                    if (tmp > pre_str && *(tmp - 1) == '\\')
                        break;

                    *tmp-- = '\0';
                }
                while (*(ptr+1) == '/')
                    ptr++;
            }

            /* advance beyond '/' */
            ptr++;
            pglob->glp_presplit_cnt++;

            /* reset */
            memset(buf, 0, sizeof(buf));
            tmp = buf;

            continue;
        }
copy:
        *tmp++ = *ptr++;
    }

    if ((pglob->glp_postsplit = calloc(pglob->glp_presplit_cnt + 1, sizeof(char **))) == NULL) {
        rc = GLOB_NOSPACE;
        goto fail;
    }
    pglob->glp_postsplit_cnt = pglob->glp_presplit_cnt;

    //for (int i = 0; i < pglob->glp_presplit_cnt; i++)
      //  printf("glob: presplit[%d]=%s\n", i, pglob->glp_presplit[i]);

    void *ret;

    if ((ret = glob_do_part(0, pglob)) == NULL) {
        rc = pglob->glp_rc;
        goto fail;
    }

    if (pglob->glp_flags & GLOB_NOCHECK) {
        rc = 0;
    } else if (pglob->gl_pathc == 0) {
        rc = GLOB_NOMATCH;
    }

    globfree_private(pglob);
fail:
    if (chdir(cwd) == -1)
        err(EXIT_FAILURE, "glob: unable to chdir to original location (%s)", cwd);

    if (rc)
        globfree(pglob);

    return rc;
}

void globfree(glob_t *pglob)
{
    if (pglob->gl_pathv) {
        for (size_t i = 0; i < pglob->gl_pathc; i++) {
            if (pglob->gl_pathv[i])
                free(pglob->gl_pathv[i]);
            pglob->gl_pathv[i] = NULL;
        }

        free(pglob->gl_pathv);
        pglob->gl_pathv = NULL;
    }
    globfree_private(pglob);
}


__attribute__((malloc))
static array_t *alloc_array(int elements)
{
    array_t *ret;

    if ((ret = malloc((sizeof(int) * (size_t)elements) + sizeof(array_t))) == NULL)
        return NULL;

    ret->len = elements;
    for (int i = 0; i < elements; ret->val[i++] = -1) ;

    return ret;
}

__attribute__((nonnull))
static int array_copy(array_t *dst, const array_t *src)
{
    if (dst->len != src->len) {
        errno = EOVERFLOW;
        return -1;
    }

    memcpy(dst->val, src->val, sizeof(int) * (size_t)dst->len);
    return 0;
}

/* remove all values */
__attribute__((nonnull))
static void array_clear(array_t *vec)
{
    for (int i = 0; i < vec->len; i++)
        vec->val[i] = -1;
}

static void free_node(node_t *node)
{
    if (!node)
        return;

    if (node->right)
        free_node(node->right);
    if (node->left)
        free_node(node->left);

    if (node->firstpos)
        free(node->firstpos);
    if (node->lastpos)
        free(node->lastpos);
    if (node->followpos)
        free(node->followpos);
    if (node->end_groups)
        free(node->end_groups);
    if (node->start_groups)
        free(node->start_groups);

    memset(node, 0, sizeof(node_t));

    free(node);
}

__attribute__((malloc))
static node_t *alloc_node(int len)
{
    node_t *ret;

    if ((ret = calloc(1, sizeof(node_t))) == NULL)
        return NULL;

    if ((ret->firstpos = alloc_array(len)) == NULL)
        goto fail;
    if ((ret->lastpos = alloc_array(len)) == NULL)
        goto fail;
    if ((ret->followpos = alloc_array(len)) == NULL)
        goto fail;

    ret->pos_size      = len;
    ret->nullable      = false;
    if ((ret->start_groups  = alloc_array(10)) == NULL)
        goto fail;
    if ((ret->end_groups    = alloc_array(10)) == NULL)
        goto fail;

    //printf("alloc_node: len=%d @0x%p\n", len, (void *)ret);

    return ret;

fail:
    if (ret->firstpos)
        free(ret->firstpos);
    if (ret->lastpos)
        free(ret->lastpos);
    if (ret->followpos)
        free(ret->followpos);
    if (ret->start_groups)
        free(ret->start_groups);
    if (ret->end_groups)
        free(ret->end_groups);
    free(ret);
    return NULL;
}

__attribute__((nonnull,warn_unused_result))
static int add_trans(dfa_state_t *state, dfa_trans_t *trans)
{
    dfa_trans_t *(*new_trans)[];

    if ((new_trans = realloc(state->trans, sizeof(dfa_trans_t *[1]) * (size_t)(state->num_trans + 1))) == NULL)
        return -1;

    state->trans = new_trans;
    (*state->trans)[state->num_trans++] = trans;

    return 0;
}

static void free_dfa_trans(dfa_trans_t *trans)
{
    if (!trans)
        return;

    if (trans->start_capture)
        free(trans->start_capture);
    if (trans->end_capture)
        free(trans->end_capture);

    free(trans);
}

__attribute__((nonnull,warn_unused_result,malloc))
static dfa_trans_t *new_dfa_trans(dfa_state_t *to,
        uint8_t match, array_t *start_capture, array_t *end_capture)
{
    dfa_trans_t *ret;

    if ((ret = malloc(sizeof(dfa_trans_t))) == NULL)
        return NULL;

    ret->to            = to;
    ret->match         = match;
    if ((ret->start_capture = alloc_array(start_capture->len)) == NULL)
        goto fail;

    if ((ret->end_capture   = alloc_array(end_capture->len)) == NULL)
        goto fail;

    array_copy(ret->start_capture, start_capture);
    array_copy(ret->end_capture, end_capture);

    return ret;
fail:
    if (ret->start_capture)
        free(ret->start_capture);
    if (ret->end_capture)
        free(ret->end_capture);
    free(ret);

    return NULL;
}

static void free_dfa_state(dfa_state_t *state)
{
    if (!state)
        return;

    if (state->trans) {
        for (int i = 0; i < state->num_trans; i++) {
            free_dfa_trans((*state->trans)[i]);
            (*state->trans)[i] = NULL;
        }
        free(state->trans);
        state->trans = NULL;
    }

    if (state->state) {
        free(state->state);
        state->state = NULL;
    }

    free(state);
}

__attribute__((nonnull,warn_unused_result,malloc))
static dfa_state_t *new_dfa_state(array_t *state, bool terminal)
{
    dfa_state_t *ret;

    if ((ret = malloc(sizeof(dfa_state_t))) == NULL)
        return NULL;

    ret->terminal = terminal;

    if ((ret->state = alloc_array(state->len)) == NULL)
        goto fail;

    array_copy(ret->state, state);

    ret->num_trans = 0;
    ret->trans     = NULL;
    ret->marked    = false;
    ret->next      = NULL;

    return ret;

fail:
    if (ret->state)
        free(ret->state);
    if (ret)
        free(ret);

    return NULL;
}

/* stack functions - 0xff is empty, false is error, errno is set */

__attribute__((nonnull,warn_unused_result))
static int push(_re_stack_t *stack, ...)
{
    if (stack->sp == stack->len - 1) {
        errno = ENOSPC;
        return false;
    }

    va_list ap;
    va_start(ap, stack);
    switch(stack->etype)
    {
        case ET_PTRDIFF_T: ((ptrdiff_t *)stack->data)[++stack->sp] = va_arg(ap, ptrdiff_t); break;
        case ET_VOID_T:    ((void **)stack->data)[++stack->sp]     = va_arg(ap, void *); break;
        case ET_UINT8_T:   ((uint8_t *)stack->data)[++stack->sp]   = (uint8_t)va_arg(ap, int); break;
        case ET_UINT16_T:  ((uint16_t *)stack->data)[++stack->sp]  = (uint16_t)va_arg(ap, int); break;
        case ET_UINT32_T:  ((uint32_t *)stack->data)[++stack->sp]  = va_arg(ap, uint32_t); break;
        case ET_INT32_T:   ((int32_t *)stack->data)[++stack->sp]   = va_arg(ap, int32_t); break;
        case ET_UINT64_T:  ((uint64_t *)stack->data)[++stack->sp]  = va_arg(ap, uint64_t); break;
        default:
            va_end(ap);
            errno = EINVAL;
            return false;
    }

    va_end(ap);
    return true;
}

__attribute__((nonnull,warn_unused_result))
static bool peek(const _re_stack_t *stack, ...)
{
    if (stack->sp == -1)
        return false;

    va_list ap;
    va_start(ap, stack);
    switch(stack->etype)
    {
        case ET_PTRDIFF_T: *va_arg(ap, ptrdiff_t *) = ((ptrdiff_t *)stack->data)[stack->sp]; break;
        case ET_VOID_T:    *va_arg(ap, void **)     = ((void **)stack->data)[stack->sp]; break;
        case ET_UINT8_T:   *va_arg(ap, uint8_t *)   = ((uint8_t *)stack->data)[stack->sp]; break;
        case ET_UINT16_T:  *va_arg(ap, uint16_t *)  = ((uint16_t *)stack->data)[stack->sp]; break;
        case ET_UINT32_T:  *va_arg(ap, uint32_t *)  = ((uint32_t *)stack->data)[stack->sp]; break;
        case ET_INT32_T:   *va_arg(ap, int32_t *)   = ((int32_t *)stack->data)[stack->sp]; break;
        case ET_UINT64_T:  *va_arg(ap, uint64_t *)  = ((uint64_t *)stack->data)[stack->sp]; break;
        default:
            va_end(ap);
            errno = EINVAL;
            return false;
    }

            va_end(ap);
    return true;
}

__attribute__((nonnull,warn_unused_result))
static bool pop(_re_stack_t *stack, ...)
{
    if (stack->sp == -1) {
        return false;
    }

    assert(stack->sp >= 0);

    va_list ap;
    va_start(ap, stack);
    switch(stack->etype)
    {
        case ET_PTRDIFF_T: *va_arg(ap, ptrdiff_t *) = ((ptrdiff_t *)stack->data)[stack->sp--]; break;
        case ET_VOID_T:    *va_arg(ap, void **)     = ((void **)stack->data)[stack->sp--]; break;
        case ET_UINT8_T:   *va_arg(ap, uint8_t *)   = ((uint8_t *)stack->data)[stack->sp--]; break;
        case ET_UINT16_T:  *va_arg(ap, uint16_t *)  = ((uint16_t *)stack->data)[stack->sp--]; break;
        case ET_UINT32_T:  *va_arg(ap, uint32_t *)  = ((uint32_t *)stack->data)[stack->sp--]; break;
        case ET_INT32_T:   *va_arg(ap, int32_t *)   = ((int32_t *)stack->data)[stack->sp--]; break;
        case ET_UINT64_T:  *va_arg(ap, uint64_t *)  = ((uint64_t *)stack->data)[stack->sp--]; break;
        default:
            va_end(ap);
            errno = EINVAL;
            return false;
    }

    va_end(ap);
    return true;
}

/* queue functions - 0xff is empty, false is error, errno is set */

__attribute__((nonnull,warn_unused_result))
static bool enqueue(queue_t *queue, ...)
{
    if (queue->head == 0) {
        errno = ENOSPC;
        return false;
    }
    va_list ap;
    va_start(ap, queue);
    switch(queue->etype)
    {
        case ET_PTRDIFF_T: ((ptrdiff_t *)queue->data)[queue->head--] = va_arg(ap, ptrdiff_t); break;
        case ET_VOID_T:    ((void **)queue->data)[queue->head--]     = va_arg(ap, void *); break;
        case ET_UINT8_T:   ((uint8_t *)queue->data)[queue->head--]   = (uint8_t)(va_arg(ap, int)); break;
        case ET_UINT16_T:  ((uint16_t *)queue->data)[queue->head--]  = (uint16_t)(va_arg(ap, int)); break;
        case ET_UINT32_T:  ((uint32_t *)queue->data)[queue->head--]  = (va_arg(ap, uint32_t)); break;
        case ET_UINT64_T:  ((uint64_t *)queue->data)[queue->head--]  = (va_arg(ap, uint64_t)); break;
        default:
            va_end(ap);
            errno = EINVAL;
            return false;
    }
    va_end(ap);
    return true;
}

__attribute__((nonnull, unused))
static bool tail(const queue_t *queue, ...)
{
    if (queue->head == queue->tail) {
        return false;
    }

    va_list ap;
    va_start(ap, queue);
    switch(queue->etype)
    {
        case ET_PTRDIFF_T: *va_arg(ap, ptrdiff_t *) = ((ptrdiff_t *)queue->data)[queue->tail]; break;
        case ET_VOID_T:    *va_arg(ap, void **)     = ((void **)queue->data)[queue->tail]; break;
        case ET_UINT8_T:   *va_arg(ap, uint8_t *)   = ((uint8_t *)queue->data)[queue->tail]; break;
        case ET_UINT16_T:  *va_arg(ap, uint16_t *)  = ((uint16_t *)queue->data)[queue->tail]; break;
        case ET_UINT32_T:  *va_arg(ap, uint32_t *)  = ((uint32_t *)queue->data)[queue->tail]; break;
        case ET_UINT64_T:  *va_arg(ap, uint64_t *)  = ((uint64_t *)queue->data)[queue->tail]; break;
        default:
            va_end(ap);
            errno = EINVAL;
            return false;
    }
    va_end(ap);
    return true;
}

__attribute__((nonnull,warn_unused_result,unused))
static bool dequeue(queue_t *queue, ...)
{
    if (queue->head == queue->tail) {
        return false;
    }

    va_list ap;
    va_start(ap, queue);
    switch(queue->etype)
    {
        case ET_PTRDIFF_T: *va_arg(ap, ptrdiff_t *) = ((ptrdiff_t *)queue->data)[queue->tail--]; break;
        case ET_VOID_T:    *va_arg(ap, void **)     = ((void **)queue->data)[queue->tail--]; break;
        case ET_UINT8_T:   *va_arg(ap, uint8_t *)   = ((uint8_t *)queue->data)[queue->tail--]; break;
        case ET_UINT16_T:  *va_arg(ap, uint16_t *)  = ((uint16_t *)queue->data)[queue->tail--]; break;
        case ET_UINT32_T:  *va_arg(ap, uint32_t *)  = ((uint32_t *)queue->data)[queue->tail--]; break;
        case ET_INT32_T:   *va_arg(ap, int32_t *)   = ((int32_t *)queue->data)[queue->tail--]; break;
        case ET_UINT64_T:  *va_arg(ap, uint64_t *)  = ((uint64_t *)queue->data)[queue->tail--]; break;
        default:
            va_end(ap);
            errno = EINVAL;
            return false;
    }
    va_end(ap);
    return true;
}

/* queue/stack allocation functions */
__attribute__((noclone))
static void free_stack(_re_stack_t *stack)
{
    if (!stack)
        return;

    if (stack->data)
        free(stack->data);

    free((void *)stack);
}

__attribute__((malloc,warn_unused_result,noclone))
static _re_stack_t *alloc_stack(int size, etype_t etype)
{
    _re_stack_t *ret = NULL;
    ssize_t len = 0;

    switch (etype) {
        case ET_UINT8_T:   len = sizeof(uint8_t); break;
        case ET_VOID_T:    len = sizeof(void *); break;
        case ET_PTRDIFF_T: len = sizeof(ptrdiff_t); break;
        case ET_UINT16_T:  len = sizeof(uint16_t); break;
        case ET_UINT32_T:  len = sizeof(uint32_t); break;
        case ET_INT32_T:   len = sizeof(int32_t); break;
        case ET_UINT64_T:  len = sizeof(uint64_t); break;
        default:
            errno = EINVAL;
            return NULL;
    }

    if ((ret = malloc(sizeof(_re_stack_t))) == NULL)
        return NULL;

    if ((ret->data = malloc((size_t)len * (size_t)++size)) == NULL)
        goto fail;

    ret->len = size;
    ret->sp  = -1;
    ret->etype = etype;
    ret->type = TYPE_STACK;

    return ret;

fail:
    if (ret)
        free(ret);

    return NULL;
}

static void free_queue(queue_t *queue)
{
    if (!queue)
        return;

    if (queue->data)
        free(queue->data);

    free((void *)queue);
}

/* TODO merge with other alloc_ for a generic one */
__attribute__((malloc,warn_unused_result, noclone))
static queue_t *alloc_queue(int size, etype_t etype)
{
    queue_t *ret = NULL;
    ssize_t len = 0;

    switch (etype) {
        case ET_UINT8_T:   len = sizeof(uint8_t); break;
        case ET_VOID_T:    len = sizeof(void *); break;
        case ET_PTRDIFF_T: len = sizeof(ptrdiff_t); break;
        case ET_UINT16_T:  len = sizeof(uint16_t); break;
        case ET_UINT32_T:  len = sizeof(uint32_t); break;
        case ET_UINT64_T:  len = sizeof(uint64_t); break;
        default:
            errno = EINVAL;
            return NULL;
    }

    if ((ret = malloc(sizeof(queue_t))) == NULL)
        return NULL;

    if ((ret->data = malloc((size_t)(len * ++size))) == NULL)
        goto fail;

    ret->len  = size;
    ret->head = size - 1;
    ret->tail = size - 1;
    ret->etype = etype;
    ret->type = TYPE_QUEUE;

    return ret;

fail:
    if (ret)
        free(ret);

    return NULL;
}

static void print_array(const array_t *array)
{
    if (!array) {
        printf("NULL");
        return;
    }

    for (int i = 0; i < array->len; i++)
    {
        if (array->val[i] == -1)
            break;

        printf("%d", array->val[i]);

        if (i + 1 < array->len && array->val[i+1] != -1)
            printf(",");
    }
}

__attribute__((nonnull,unused))
static void print_token(const token_t *t)
{
    printf("token=%c orig_pos=%2d group=%2d\n",
            t->t.token,
            t->t.orig_pos,
            t->t.group
            );
}

__attribute__((nonnull))
static void print_node(const node_t *n, int indent)
{
    if (indent == 0)
        printf("root : ");
    printf("%c [pos:%3d: nullable:%u ",
            n->type,
            n->pos,
            n->nullable);
    printf("start:[");
    print_array(n->start_groups);
    printf("] end:[");
    print_array(n->end_groups);
    printf("]");

    printf(" firstpos(");
    print_array(n->firstpos);
    printf("), lastpos(");
    print_array(n->lastpos);
    printf("), followpos(");
    print_array(n->followpos);
    printf(")]\n");

    if (n->left) {
        for (int i = 0; i < indent; i+=2)
            printf("| ");
        printf("+-left : ");
        print_node(n->left, indent+2);
    }
    if (n->right) {
        for (int i = 0; i < indent; i+=2)
            printf("| ");
        printf("+-right: ");
        print_node(n->right, indent+2);
    }
}

__attribute__((nonnull,unused))
static void dump_node_queue(queue_t *queue)
{
    printf("len:%d head:%d tail:%d type:%d etype:%s\n",
            queue->len, queue->head, queue->tail,
            queue->type, etype_names[queue->etype]);
    for (int i = 0; i < queue->len; i++) {
        printf("\tpos[%2d]=", i);
        token_t node;
        switch(queue->etype)
        {
            case ET_UINT64_T:
                node.val = ((uint64_t *)queue->data)[i];
                printf("0x%08lx p=%2d t=%c", node.val, node.t.orig_pos, node.t.token ? node.t.token : '_');
                break;
            default: break;
        }
        printf(" %s%s\n",
                i == queue->head ? "HEAD" : "",
                i == queue->tail ? "TAIL" : ""
              );
    }
}

__attribute__((nonnull,unused))
static void dump_node_stack(_re_stack_t *stack)
{
    printf("len:%d sp:%d\n", stack->len, stack->sp);
    node_t *n;

    while (pop(stack, &n))
    {
        printf("root: ");
        print_node(n, 0);
    }
}

__attribute__((warn_unused_result))
static bool is_operator(uint8_t op)
{
    switch(op)
    {
#ifdef NCONV
        case PLUS:
        case OPT:
#endif
        case CAT:
        case OR:
        case STAR:
            return true;

        default:
            return false;
    }
}

/* this function implements the shunting yard algorithm as described on the
 * wikipedia page. the queue_t returned is hopefully a syntax tree in somewhat
 * reverse-polish notation. the token_list submitted is iso8859-1 but only in
 * the sense some non-ASCII codepoints are used to store operators
 *
 * the second arg will be set to a pointer to an array of group_t unions
 * these store each match-group alongwith the first non-parenthesis character
 * position afer the OPEN and CLOSE
 *
 * the returned queue_t will contain token_t unions which have both
 * the uint8_t token and the character posistion
 *
 * this combined with the groups arg can ensure match groups can be
 * preserved in the abscence of parenthesis.
 */
__attribute__((nonnull,malloc,warn_unused_result))
static queue_t *yard(const uint8_t *token_list, const uint8_t *is_match, int max)
{
    queue_t *output_queue   = NULL;
    _re_stack_t *operator_stack = NULL;
    _re_stack_t *output_stack   = NULL;
    _re_stack_t *group_stack    = NULL;

    token_t os;
    uint8_t token;
    int tl_idx;
    int len;

    errno  = 0;
    tl_idx = 0;
    token  = token_list[tl_idx];
    len    = (int)strlen((const char *)token_list);

    /* allocate our output queue (FIFO) & operator stack (LIFO)
     * these are actually unions, but of the same size */
    if ((output_queue = alloc_queue(len, ET_UINT64_T)) == NULL)
        return NULL;

    if ((operator_stack = alloc_stack(len, ET_UINT64_T)) == NULL)
        goto fail;

    if ((output_stack = alloc_stack(len, ET_UINT64_T)) == NULL)
        goto fail;

    if ((group_stack = alloc_stack(20, ET_INT32_T)) == NULL)
        goto fail;

    int32_t group_num = 0, cur_group = -1;

    while(token)
    {
        assert(tl_idx < max);

        //printf("yard: cur_group=%d group_stack: sp=%d\n",
        //      cur_group,
        //    group_stack->sp
        //  );

        token_t newtok = {
            .t.token    = token,
            .t.orig_pos = tl_idx,
            .t.group    = -1
        };

        token_t o1, o2;
        uint64_t dummy = 0;

        switch(token)
        {
            case OPEN:
                /* Handle (non-)matching groups */
                if (is_match[tl_idx]) {
                    if (cur_group != -1) {
                        if (!push(group_stack, cur_group))
                            goto fail;
                    }

                    cur_group = group_num;
                    newtok.t.group = group_num++;
                }

                if (!push(operator_stack, newtok.val))
                    goto fail;

                if (!enqueue(output_queue, newtok))
                    goto fail;

                //printf("yard: "); print_token(&newtok);
                break;

            case CLOSE:
                /* Handle (non-)matching groups */
                if (is_match[tl_idx]) {
                    newtok.t.group = cur_group;

                    if (peek(group_stack, &dummy)) {
                        if (!pop(group_stack, &cur_group))
                            goto fail;
                    } else
                        cur_group = -1;
                }

                /* while the operator at the top of the operator stack is not a left
                 * parenthesis: pop the operator from the operator stack into the
                 * output queue */
                while(peek(operator_stack, &os) && os.t.token != OPEN ) {
                    if (!pop(operator_stack, &os))
                        goto fail;
                    if (!enqueue(output_queue, os))
                        goto fail;
                }

                /* If the stack runs out without finding a left parenthesis, then
                 * there are mismatched parentheses. */
                if (!peek(operator_stack, &dummy)) {
                    errno = EINVAL;
                    goto fail;
                }

                /* {assert there is a left parenthesis at the top of the operator stack}
                 * the discard OPEN */
                if (!peek(operator_stack, &os) || os.t.token != OPEN || !pop(operator_stack, &os))
                    goto fail;

                /* TODO ?? if there is a function on the op stack, pop it to the output queue ?? */
                if (!enqueue(output_queue, newtok))
                  goto fail;

                //printf("yard: "); print_token(&newtok);
                break;

#ifdef NCONV
            case PLUS:
            case OPT:
#endif
            case CAT:
            case OR:
            case STAR:
                {
                    o1 = newtok;
                    if (!peek(operator_stack, &o2)) {
                        goto skip;
                    }

                    /* there is an operator o2 other than the left parenthesis at the top
                       of the operator stack, and (o2 has greater precedence than o1
                       or they have the same precedence and o1 is left-associative) */

                    while(  o2.t.token != OPEN &&
                            ( (ops[o2.t.token].prec >  ops[o1.t.token].prec) ||
                              (ops[o2.t.token].prec == ops[o1.t.token].prec && ops[o1.t.token].left_assoc) ))
                    {
                        token_t tmp_op;

                        if (!pop(operator_stack, &tmp_op))
                            goto fail;

                        if (!enqueue(output_queue, tmp_op))
                            goto fail;

                        if (!peek(operator_stack, &o2))
                            break;
                    }
skip:
                    if (!push(operator_stack, o1))
                        goto fail;
                }
                break;

            case TERM:
            case NONE:
            default:

                if (!enqueue(output_queue, newtok))
                    goto fail;

                if (!push(output_stack, newtok))
                    goto fail;

                break;
        }

        token = token_list[++tl_idx];
    }

    //printf("loop done\n");

    while(peek(operator_stack, &os))
    {
        /* {assert the operator on top of the stack is not a (left) parenthesis} */
        if (os.t.token == OPEN) {
            errno = EINVAL;
            goto fail;
        }
        if (!pop(operator_stack, &os))
            goto fail;

        if (!enqueue(output_queue, os))
            goto fail;
    }

done:
    free_stack(operator_stack);
    free_stack(output_stack);
    free_stack(group_stack);

    return output_queue;

fail:
    if (errno == 0)
        errno = EINVAL;
    free_queue(output_queue);
    output_queue = NULL;
    goto done;
}

/* return he used length of the array */
__attribute__((nonnull))
static int array_len(const array_t *v1)
{
    for (int i = 0; ;i++)
        if (v1->val[i] == -1)
            return i;
}

/* check if a array has a value */
__attribute__((nonnull, warn_unused_result))
static bool array_has(const array_t *vec, int value)
{
    for (int i = 0; i < vec->len; i++) {
        if (vec->val[i] == -1)
            break;
        if (vec->val[i] == value)
            return true;
    }
    return false;
}

/* compare two (unsorted) arrays */
__attribute__((nonnull,warn_unused_result))
static bool array_compare(const array_t *v1, const array_t *v2)
{
    int len = array_len(v1);
    if (array_len(v2) != len)
        return false;

    if (len == 0)
        return true;

    for (int i = 0; i < len; i++)
        if (!array_has(v2, v1->val[i]))
            return false;

    return true;
}

/* append single value to array */
__attribute__((nonnull,warn_unused_result))
static int array_append(array_t *dst, int value)
{
    if (array_has(dst, value))
        return 0;

    for (int i = 0; i < dst->len-1; i++) {
        if (dst->val[i] == -1) {
            dst->val[i] = value;
            dst->val[i+1] = -1;
            return 0;
        }
    }

    errno = ENOSPC;
    return -1;
}

/* combine two arrays a and b to dst.
 * a can be NULL or a == dst
 */
__attribute__((nonnull(1,3)))
static int array_union(array_t *dst, array_t *a, array_t *b)
{
    int i,j;

    if (a && a != dst) {
        if (dst->len < a->len) {
            errno = EOVERFLOW;
            return -1;
        }
        for (j = 0, i = 0; i < a->len; i++, j++) {
            if (a->val[i] == -1)
                break;
            dst->val[i] = a->val[i];
        }
    } else
        for (j = 0; j < dst->len && dst->val[j] != -1; j++) ;

    for (i = 0; i < b->len; i++) {
        if (b->val[i] == -1 )
            break;
        if (array_has(dst, b->val[i]))
            continue;
        if (j+1 == dst->len) {
            errno = EOVERFLOW;
            return -1;
        }
        dst->val[j++] = b->val[i];
    }
    dst->val[j] = -1;
    return 0;
}


#define BUF_INCR 32L

__attribute__((nonnull, warn_unused_result))
static bool grow_buffer(struct aug_state *state, off_t growth)
{
    uint8_t *old_are, *old_is_match;
    off_t offset = state->are_ptr - state->are;
    state->are_len += growth;
    old_are = state->are;
    old_is_match = state->is_match;

    if ((state->are = realloc(old_are, (size_t)state->are_len)) == NULL)
        return false;
    if ((state->is_match = realloc(old_is_match, (size_t)state->are_len)) == NULL)
        return false;

    state->are_ptr = state->are + offset;

    for (int i = 0; i < growth; i++)
        state->is_match[offset + i] = false;

    return true;
}

/* calculate followpos */
__attribute__((nonnull))
static void fix_followpos(node_t *root, node_t *(*node_lookup)[])
{
    /*
     * 1. for each node n in the tree do
     * 2.  if n is a concatenation node with left child c1 and right child c2 then
     * 3.   for each i in lastpos(c1) do
     * 4.    followpos(i) := followpos(i) » firstpos(c2)
     *      end do
     * 5.  else if n is a *-node
     * 6.   for each i in lastpos(n) do
     * 7.    followpos(i) := followpos(i) » firstpos(n)
     *      end do
     *     end if
     *    end do
     */

    node_t *inode = NULL;
    int node_id;
#ifdef RE_DEBUG
    printf("fix_followpos: pos=%2d type=%c left[%c] right[%c] start[",
            root->pos, root->type,
            root->left ? root->left->type : ' ',
            root->right ? root->right->type : ' ');

    print_array(root->start_groups);
    printf("] end[");
    print_array(root->end_groups);
    printf("]\n");
#endif


    /* if n is a concat node ... */
    if (root->type == CAT && (root->right && root->left)) {

        /* for each i in lastpos(left) */
        for (int i = 0; ; i++)
        {
            if ((node_id=root->left->lastpos->val[i]) == -1)
                break;

            /* lookup inode based on i and union with firstpos(right) */
            if ((inode = (*node_lookup)[node_id]) == NULL) {
                printf("Can't find inode %d\n", node_id);
                continue;
            }

            /* followpos(i) = followpos(u) U firstpos(right) */
            array_union(inode->followpos, NULL,
                    root->right->firstpos);
        }

        array_union(root->right->end_groups, NULL, root->left->end_groups);
        array_clear(root->left->end_groups);
    } else if (root->type == STAR) {
        /* else if n is a *-node ... */

        /* for each i in lastpos(n) */
        for (int i = 0; ; i++)
        {
            if ((node_id = root->lastpos->val[i]) == -1)
                break;

            /* lookup inode based on i and union with firstpos(root) */
            if ((inode = (*node_lookup)[node_id]) == NULL) {
                printf("Can't find inode %d\n", node_id);
                continue;
            }

            /* followpos(i) = followpos(i) U firstpos(n) */
            array_union(inode->followpos, NULL,
                    root->firstpos);
        }

    }

    if (root->right)
        fix_followpos(root->right, node_lookup);
    if (root->left)
        fix_followpos(root->left, node_lookup);
}

/* install 'node->root' and calc nullable, firstpos and lastpos */
__attribute__((nonnull))
static void fix_parents(node_t *root)
{
    if (root->left) {
        root->left->root = root;
        fix_parents(root->left);

    }

    if (root->right) {
        fix_parents(root->right);
        root->right->root = root;

    }

    switch (root->type)
    {
        case NONE:
            root->nullable = true;
            root->firstpos->val[0] = root->pos;
            root->firstpos->val[1] = -1;
            root->lastpos->val[0] = root->pos;
            root->lastpos->val[1] = -1;
            break;

        case STAR:
            /* TODO check that right is the correct one to use - in one
             * example it is 'c1' and the other 'c1' are left */
            if (root->right) {
                root->nullable = true;
                array_copy(root->firstpos, root->right->firstpos);
                array_copy(root->lastpos, root->right->lastpos);
            }
            break;

        case CAT:
            if (root->right && root->left) {
                root->nullable = root->right->nullable && root->left->nullable;

                if (root->left->nullable) {
                    array_union(root->firstpos,
                            root->left->firstpos,
                            root->right->firstpos);
                } else {
                    array_copy(root->firstpos, root->left->firstpos);
                }

                if (root->right->nullable) {
                    array_union(root->lastpos,
                            root->left->lastpos,
                            root->right->lastpos);
                } else {
                    array_copy(root->lastpos, root->right->lastpos);
                }
            }
            break;

        case OR:
            root->nullable = root->right->nullable || root->left->nullable;

            array_union(root->firstpos,
                    root->left->firstpos,
                    root->right->firstpos);

            array_union(root->lastpos,
                    root->left->lastpos,
                    root->right->lastpos);
            break;

        default:
            root->nullable = false;
            root->firstpos->val[0] = root->pos;
            root->firstpos->val[1] = -1;
            root->lastpos->val[0] = root->pos;
            root->lastpos->val[1] = -1;
            break;

    }
}

static struct {
    const char *const chr_class;
    const char *const expn;
} chr_classes[] = {
    { "[:lower:]", "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)" },
    { "[:upper:]", "(A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)" },
    { "[:alpha:]", "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)" },
    { "[:alnum:]", "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|0|1|2|3|4|5|6|7|8|9)" },
    { "[:digit:]", "(0|1|2|3|4|5|6|7|8|9)" },
    { "[:space:]", "( |\t|\v|\r|\n)" },
    { NULL, NULL }
};

/* ensures that a [range] is well formed */
__attribute__((nonnull,warn_unused_result))
static int validate_range(const char *range)
{
    if (!*range)
        return -1;

    //printf("check: %s\n", range);

    const char *ptr = range;
    const char *tmp = NULL;
    const char *chr_class_start = NULL;

    if (*ptr == '-')
        ptr++;

    bool had_alnum = false;
    bool has_start = false;

    while (*ptr) {
        //printf("check: %c alnum:%d start:%d\n", isprint(*ptr) ? *ptr : ' ', had_alnum, has_start);
        if (*ptr == '-') {
            if (has_start) goto fail;
            if (!had_alnum) goto fail;
            has_start = true;
        } else if (!strncmp("[:", ptr, 2)) {
            chr_class_start = ptr;
            ptr += 2;
            if ((tmp = strstr(ptr, ":]")) == NULL)
                return -1;
            tmp--;
            while (*tmp && tmp >= ptr)
                if (!isalpha(*tmp--))
                    return -1;

            for (int i = 0; chr_classes[i].chr_class; i++)
                if (!strncmp(chr_classes[i].chr_class, chr_class_start, 9))
                    goto found;
            return -1;
found:
            ptr = tmp + 1;
        } else if (isalnum(*ptr)) {
            had_alnum = true;
            if (has_start) {
                has_start = false;
                had_alnum = false;
            }
        }

        ptr++;
    }

    if (has_start)
        goto fail;

    return 0;

fail:
    return -1;
}

/* this function augments an ASCII ERE via:
 * appending concat and terminal at the end
 * inserting an explict concat operator
 * replacing other ASCII operators with alternatives from iso8859-1
 * the following ERE operators are suppored: ()|*+?
 * unless NCONV is defined at compile time, the expression is simplfied:
 *  a+   becomes a.a*
 *  a?   becomes a|NONE
 *  (a)? becomes ((a)|NONE)
 * .     is retained as the ANY char
 * escaping via \ is also supported for literals
 *
 * TODO:
 * {,m}:  a{,4}  becomes a?a?a?a?
 * {n,}:  a{4,}  becomes aaaaa*
 * {n,m}: a{1,3} becomes aa?a?
 * [a-z]: expand to (a|b|c|d|e|f|....|z) ?
 * [[:class:]] expand to (a|b|c|....|z) ?
 * ^$:    tag the RE as being anchored?
 *
 */
__attribute__((nonnull,malloc,warn_unused_result, noclone))
static uint8_t *augment(const char *re, uint8_t **is_match_out, size_t *is_match_len)
{
    const char *re_ptr;
    bool running;
    bool previous;
    uint64_t dummy;

    struct aug_state state;

    memset(&state, 0, sizeof(state));

    re_ptr        = re;
    state.are_len = BUF_INCR;
    running       = true;
    previous      = false;
    *is_match_len = 0;

    if ((state.are_ptr = state.are = malloc((size_t)state.are_len)) == NULL)
        return NULL;

    if ((state.is_match = calloc(1, (size_t)state.are_len)) == NULL)
        goto fail;

    if ((state.in_vstack = alloc_stack(20L, ET_PTRDIFF_T)) == NULL)
        goto fail;

    if ((state.out_vstack = alloc_stack(20L, ET_PTRDIFF_T)) == NULL)
        goto fail;

    /* handle match group 0 */
    *state.are_ptr++ = OPEN;
    *state.is_match = true;

    while (running)
    {
        const off_t offset = state.are_ptr - state.are;

        if (!state.are_len || offset >= (state.are_len - 6))
            if (!grow_buffer(&state, BUF_INCR))
                goto fail;

        switch (*re_ptr)
        {
            case '?':
#ifdef NCONV
                *state.are_ptr++ = OPT;
#else
                /* replace ()? with (()|NONE) */
                if (*(state.are_ptr - 1) == CLOSE) {
                    /* TODO this might not work */
                    ptrdiff_t open;
                    ssize_t   len;
                    uint8_t  *new, *new_match;

                    if (!peek(state.out_vstack, &open)) {
                        errno = EOVERFLOW; goto fail;
                    }
                    len = (state.are_ptr - (state.are + open));

                    if (!grow_buffer(&state, len + 6))
                        goto fail;
                    /* pointers may have changed */

                    if (!peek(state.out_vstack, &open))
                        goto fail;
                    state.are_ptr = state.are + open;

                    if (!pop(state.out_vstack, &open)) {
                        errno = EOVERFLOW; goto fail;
                    }

                    new       = (uint8_t *)strndup((const char *)(state.are + open), (size_t)len);
                    new_match = malloc(len);
                    memcpy(new_match, (state.is_match + open), (size_t)len);

                    if (!pop(state.in_vstack, &dummy)) {
                        errno = EOVERFLOW; goto fail;
                    }

                    *state.are_ptr++ = OPEN;
                    memcpy(state.are_ptr, new, (size_t)len);
                    memcpy(&state.is_match[state.are_ptr - state.are], new_match, (size_t)len);
                    /* () becomes (() so correct to .() */
                    state.is_match[state.are_ptr - state.are - 1] = false;
                    state.are_ptr += len;

                    free(new);
                    free(new_match);

                } else {
                    /* replace .? with (.|NONE) */
                    uint8_t tmp = *(state.are_ptr - 1);
                    *(state.are_ptr - 1) = OPEN;
                    *state.are_ptr++ = tmp;
                }
                *state.are_ptr++ = OR;
                *state.are_ptr++ = NONE;
                *state.are_ptr++ = CLOSE;
#endif
                break;

            case '{':
                {
                    /* TODO add support for (...){n.m} */
                    if (re_ptr == re) {
                        /* not valid at the start of a RE */
                        fprintf(stderr, "augment: {n.m} not valid at start of RE\n");
                        errno = EINVAL; goto fail;
                    }

                    int from = 0;
                    int to = 0;

                    /* skip over { */
                    re_ptr++;

                    /* read n */
                    while (*re_ptr && isdigit(*re_ptr)) {
                        from *= 10;
                        from += (*re_ptr - '0');
                        re_ptr++;
                    }

                    if (*re_ptr == '}') {
                        to = from;
                        goto skip_m;
                    }

                    if (*re_ptr++ != ',') {
                        fprintf(stderr, "augment: missing , in {n,m}: got %c\n", *(re_ptr - 1));
                        errno = EINVAL; goto fail;
                    }

                    /* read m */
                    while (*re_ptr && isdigit(*re_ptr)) {
                        to *= 10;
                        to += (*re_ptr - '0');
                        re_ptr++;
                    }
skip_m:

                    if (*re_ptr != '}' ||
                            (from == 0 && to == 0) ||
                            (to != 0 && from > to) ) {
                        fprintf(stderr, "augment: invalid format {n,m}\n");
                        errno = EINVAL; goto fail;
                    }

                    /* FIXME this won't work for (...){n,m} */
                    /* FIXME 6 is just the first number it doesn't segfault */
                    /* FIXME is the handling of {6,} to==0 OK? */
                    if (!grow_buffer(&state, ((to ? to : (from + 1)) - from)*6))
                        goto fail;

                    /* This won't work properly for OPEN/CLOSE */
                    uint8_t prev = *(state.are_ptr-1);

                    if (prev != OPEN)
                        state.are_ptr--;

                    to -= from;

                    /* Output prev nCAT times e.g. a{4,8} => a.a.a.a */
                    for (int i = 0; i < from; i++)
                    {
                        if (prev == OPEN) {
                            fprintf(stderr, "augment: OPEN prev nCAT\n");
                            errno = EINVAL; goto fail;
                        } else {
                            *(state.are_ptr++) = prev;
                        }

                        if (i+1 != from)
                            *(state.are_ptr++) = CAT;
                        previous = true;
                    }

                    /* Output lots of CAT(a|NONE) m times */
                    for (int i = 0; i < to; i++)
                    {
                        if (i == 0 && previous)
                            *(state.are_ptr++) = CAT;

                        *(state.are_ptr++) = OPEN;

                        if (prev == OPEN) {
                            fprintf(stderr, "augment: OPEN CAT\n");
                            errno = EINVAL; goto fail;
                        } else {
                            *(state.are_ptr++) = prev;
                        }

                        *(state.are_ptr++) = OR;
                        *(state.are_ptr++) = NONE;
                        *(state.are_ptr++) = CLOSE;

                        if (i+1 != to)
                            *(state.are_ptr++) = CAT;
                        previous = true;
                    }
                }

                break;

            case '[':
                {
                    *state.are_ptr++ = BRACKET_OPEN;
                    re_ptr++;

                    const char *bracket_start = re_ptr;
                    const char *bracket_end = NULL;

                    if (!strncmp("[:", re_ptr, 2)) {
                        const char *tmp = re_ptr;
                        re_ptr = strstr(re_ptr, ":]");
                        if (re_ptr == NULL)
                            goto fail;
                        re_ptr+=2;
                        if (!grow_buffer(&state, BUF_INCR))
                            goto fail;
                        memcpy(state.are_ptr, tmp, re_ptr - tmp);
                        bracket_end = re_ptr;
                        goto skip;
                    }

                    bool found = false;
                    while(*re_ptr && !found) {
                        if ((state.are_ptr - state.are) >= (state.are_len - 6))
                            if (!grow_buffer(&state, BUF_INCR))
                                goto fail;
                        if (*re_ptr == ']')
                            found = true;
                        else
                            *state.are_ptr++ = *re_ptr++;
                    }

                    if (!found) {
                        fprintf(stderr, "augment: no BRACKET_CLOSE\n");
                        errno = EINVAL; goto fail;
                    }

                    bracket_end = re_ptr;
skip:
                    char *range;

                    if ((range = calloc(1, (bracket_end - bracket_start) + 1)) == NULL) {
                        goto fail;
                    }
                    memcpy(range, bracket_start, (bracket_end - bracket_start));

                    if (validate_range(range)) {
                        fprintf(stderr, "augment: invalid range [%s]\n", range);
                        free(range);
                        errno = EINVAL; goto fail;
                    }
                    free(range);

                    *state.are_ptr++ = BRACKET_CLOSE;

                }
                break;

            case '(':
                {
                    ptrdiff_t ptr;
                    if (previous)
                        *state.are_ptr++ = CAT;

                    ptr = (state.are_ptr - state.are);
                    if (!push(state.out_vstack, ptr))
                        goto fail;

                    ptr = (re_ptr - re);
                    if (!push(state.in_vstack, ptr))
                        goto fail;

                    *state.are_ptr = OPEN;
                    state.is_match[state.are_ptr - state.are] = true;
                    state.are_ptr++;

                    previous = false;
                }
                break;

            case ')':
                *state.are_ptr = CLOSE;
                state.is_match[state.are_ptr - state.are] = true;
                state.are_ptr++;

                switch(*(re_ptr + 1)) {
                    case '+':
                    case '?':
                        break;
                    default:
                        if (!pop(state.out_vstack, &dummy) ||
                                !pop(state.in_vstack, &dummy))
                            goto fail;
                }
                previous = true;
                break;

            case '\0':
                /* handle match group 0 */
                *state.are_ptr = CLOSE;
                state.is_match[state.are_ptr - state.are] = true;
                state.are_ptr++;

                *state.are_ptr++ = CAT;
                *state.are_ptr++ = TERM;
                *state.are_ptr   = '\0';
                running = false;
                continue;

            case '+':
#ifdef NCONV
                *state.are_ptr++ = PLUS;
#else
                /* replace ()+ with ().()* */
                if (*(state.are_ptr - 1) == CLOSE) {
                    ptrdiff_t open;
                    ssize_t    len;
                    uint8_t  *new, *new_match;

                    if (!peek(state.out_vstack, &open)) {
                        errno = EOVERFLOW; goto fail;
                    }
                    len = (state.are_ptr - (state.are + open));

                    if (!grow_buffer(&state, len + 6))
                        goto fail;
                    /* pointers invalidated here */

                    if (!peek(state.out_vstack, &open))
                        goto fail;
                    state.are_ptr = state.are + open;

                    if (!pop(state.out_vstack, &open)) {
                        errno = EOVERFLOW; goto fail;
                    }

                    new       = (uint8_t *)strndup((const char *)(state.are + open), (size_t)len);
                    if (new == NULL)
                        goto fail;

                    new_match = (uint8_t *)malloc(len);
                    if (!new_match) {
                        free(new);
                        goto fail;
                    }

                    memcpy(new_match, (state.is_match + open), len);

                    if (!pop(state.in_vstack, &dummy)) {
                        errno = EOVERFLOW; goto fail;
                    }

                    memcpy(state.are_ptr, new, (size_t)len);
                    memcpy(&state.is_match[state.are_ptr - state.are], new_match, (size_t)len);
                    state.are_ptr += len;
                    *state.are_ptr++ = CAT;
                    memcpy(state.are_ptr, new, (size_t)len);
                    memcpy(&state.is_match[state.are_ptr - state.are], new_match, (size_t)len);
                    state.are_ptr += len;

                    free(new);
                    free(new_match);

                } else {

                    /* replace .+ with ..* */
                    *state.are_ptr++ = CAT;
                    *state.are_ptr = *(state.are_ptr - 2);
                    state.are_ptr++;
                }
                *state.are_ptr++ = STAR;
#endif
                break;

            case '*':
                *state.are_ptr++ = STAR;
                break;

            case '|':
                *state.are_ptr++ = OR;
                if (!*(re_ptr + 1) || *(re_ptr + 1) == ')') {
                    *state.are_ptr++ = NONE;
                }
                previous = false;
                break;

            case '.':
                if (previous)
                    *state.are_ptr++ = CAT;
                *state.are_ptr++ = ANY;
                previous = true;
                break;

            case '\\':
                re_ptr++;
                /* fall-through */

            default:
                if (previous)
                    *state.are_ptr++ = CAT;
                *state.are_ptr++ = (uint8_t)*re_ptr;
                previous = true;
                break;
                }

                re_ptr++;
        }

done:

    free_stack(state.in_vstack);
    free_stack(state.out_vstack);
    *is_match_out = state.is_match;
    *is_match_len = state.are_len;
    return state.are;

fail:
    if (state.is_match) {
        free(state.is_match);
        state.is_match = NULL;
    }

    if (state.are) {
        free(state.are);
        state.are = NULL;
        state.are_ptr = NULL;
    }

    goto done;
}
#undef BUF_INCR

__attribute__((nonnull))
static void trim_dfa(dfa_state_t *list)
{
    for (dfa_state_t *st = list; st; st=st->next) {
        if (st->state) {
            free(st->state);
            st->state = NULL;
        }
    }
}

__attribute__((nonnull,unused))
static void print_state(const dfa_state_t *state)
{
    printf("D[%02d] trans=%d: {",
            state->id,
            state->num_trans);
    if (state->state)
        print_array(state->state);
    printf("}\n");
    for (int i = 0; i < state->num_trans; i++) {
        dfa_trans_t *trans = (*state->trans)[i];
        printf(" Dstate %c => D[%02d] ",
                trans->match,
                trans->to->id);
        if (array_len(trans->start_capture)) {
            printf(" start:");
            print_array(trans->start_capture);
        }
        if (array_len(trans->end_capture)) {
            printf("   end:");
            print_array(trans->end_capture);
        }
        printf("\n");
    }
}

/* locate a state based on the array provided */
__attribute__((nonnull, pure))
static dfa_state_t *find_state(dfa_state_t *restrict list, const array_t *restrict def)
{
    for (dfa_state_t *st = list; st; st=st->next)
        if (array_compare(def, st->state))
            return st;
    return NULL;
}

__attribute__((nonnull))
static struct dfa_state_t *build_dfa(node_t *root, node_t *(*node_lookup)[])
{
    int snum = 0;
    dfa_state_t *list = NULL;
    dfa_state_t *s0   = NULL;
    bool has_unmarked;
    bool has_failed = false;

#ifdef RE_DEBUG
    printf("build_dfa:\n");
    print_node(root, 0);
#endif

    /* allocate Dstate[00] - the initial state */
    if ((s0 = new_dfa_state(root->firstpos, root->type == TERM)) == NULL)
        goto fail;
    list = s0;
    s0->id = snum++;


    /* nested for() loops are awkward */
    do {
        has_unmarked = false;

        const int vec_sz = root->pos_size + 1;
        array_t *tmp_vec, *done_vec, *tgt_state, *tmp_start_group, *tmp_end_group;

        tmp_vec         = alloc_array(vec_sz);
        done_vec        = alloc_array(vec_sz);
        tgt_state       = alloc_array(vec_sz);
        tmp_start_group = alloc_array(20);
        tmp_end_group   = alloc_array(20);

        if (!tmp_vec || !done_vec || !tgt_state || !tmp_start_group || !tmp_end_group)
            goto inner_fail;

        /* keep going until every dfa state is marked */
        for (dfa_state_t *st = list; st; st=st->next)
        {
            if (st->marked)
                continue;
#ifdef RE_DEBUG
            printf("build_dfa: processing state %d\n", st->id);
#endif
            has_unmarked = true;
            array_clear(done_vec);
            array_clear(tmp_start_group);
            array_clear(tmp_end_group);

            /* iterate over each AST node in this state */
            for (int i = 0; i < st->state->len; i++)
            {
                int node_id = st->state->val[i];


                if (node_id == -1)
                    break;

                node_t *node = (*node_lookup)[node_id];
#ifdef RE_DEBUG
                printf("build_dfa: checking node %d (%c)\n", node_id, node->type);
#endif
                /* we don't add NULL leaf nodes */
                if (node->type == NONE)
                    continue;

                /* if we have already done this node type (char) skip */
                if (array_has(done_vec, node->type))
                    continue;

                /* note we've already looked at this type (char) */
                if (array_append(done_vec, (int)node->type))
                    goto fail;
                array_clear(tmp_vec);

                /* find all the other AST nodes, from this one,
                 * matching the same type/char */
                for (int j = i; j < st->state->len; j++) {
                    if (st->state->val[j] <= -1)
                        break;

                    node_t *tmp_node = (*node_lookup)[st->state->val[j]];

                    if (tmp_node->type != node->type)
                        continue;

                    if (array_append(tmp_vec, st->state->val[j]))
                        goto fail;

                }

                array_clear(tgt_state);

                /* build a target state e.g. {1,2,3} from the matching
                 * ones */
                for (int j = 0; j < tmp_vec->len; j++) {
                    if (tmp_vec->val[j] == -1)
                        break;
                    const node_t *add = (*node_lookup)[tmp_vec->val[j]];
                    array_union(tgt_state, NULL, add->followpos);
                    array_union(tmp_start_group, NULL, add->start_groups);
                    array_union(tmp_end_group, NULL, add->end_groups);
                }

                /* see if this state already exists */
                dfa_state_t *tgt = find_state(list, tgt_state);

                if (!tgt) {
                    /* build it, if not */
                    if ((tgt = new_dfa_state(tgt_state, false)) == NULL)
                        goto inner_fail;
                    tgt->id = snum++;
                    /* FIXME terminal ?? */
                    tgt->next = list;
                    list = tgt;
                }

                /* create and attach the DFA state transistion */
                //dfa_trans_t *tran = new_dfa_trans(tgt, node->type,
                //        node->start_groups, node->end_groups);
                dfa_trans_t *tran = new_dfa_trans(tgt, node->type,
                        tmp_start_group, tmp_end_group);

                if (tran == NULL)
                    goto inner_fail;

                if (add_trans(st, tran) == -1)
                    goto inner_fail;

            }

            st->marked = true;
        }

inner_end:
        if (tmp_vec)
            free(tmp_vec);
        if (done_vec)
            free(done_vec);
        if (tgt_state)
            free(tgt_state);
        if (tmp_start_group)
            free(tmp_start_group);
        if (tmp_end_group)
            free(tmp_end_group);

        continue;
inner_fail:
        has_unmarked = false;
        has_failed   = true;
        goto inner_end;

    } while(has_unmarked);

    if (!has_failed) {
        trim_dfa(list);
        return list;
    }

fail:
    {
        dfa_state_t *next = NULL;
        for (dfa_state_t *st = list; st; st = next)
        {
            next = st->next;
            free_dfa_state(st);
        }
        return NULL;
    }
}



/* global function defintions */

/* regerror is defined in libc.c */

void regfree(regex_t *preg)
{
    dfa_state_t *state = preg->priv;
    if (state)
        free_dfa_state(state);

    preg->priv = 0;
}

int regexec(const regex_t *restrict preg, const char *restrict string, size_t len,
        regmatch_t pmatch[restrict], __attribute__((unused)) int eflags)
{
    if (!preg || !string || !preg->priv)
        return -1;

    const dfa_state_t *state = preg->priv;
    const char *p = string;
    bool found = false;

    for (size_t i = 0; i < len; i++)
        pmatch[i].rm_so = pmatch[i].rm_eo = -1;

    while (1)
    {
#ifdef RE_DEBUG
        printf("regexec: \"%c\" [%2d] state=%d",
                isprint(*p) ? *p : ' ',
                (int)(p - string),
                state->id
                );
#endif
        /* Check each transition from this state */
        int i;
        int has_any = -1;
        dfa_trans_t *trans;

        /*
        for (i = 0; i < state->num_trans; i++)
        {
            trans = (*state->trans)[i];
            printf(" '%c'-[%d,%d]->%d",
                    trans->match,
                    array_len(trans->start_capture),
                    array_len(trans->end_capture),
                    trans->to->id
                    );
        }
        */

        for (i = 0; i < state->num_trans; i++)
        {
            trans = (*state->trans)[i];

#ifdef RE_DEBUG
            if (trans->match == OPEN) {
                printf("*** OPEN found\n");
            } else if (trans->match == CLOSE) {
                printf("*** CLOSE found\n");
            } else
#endif
            if (trans->match == ANY && (*p && *p != '\n')) {
#ifdef RE_DEBUG
                printf(" has_any");
                printf(" moving to %d", trans->to->id);
#endif
                has_any = i;
            } else
            /* See if we match this and shift to it */
            if (*p && (trans->match == *p)) {
#ifdef RE_DEBUG
                printf(" matched");
                printf(" moving to %d", trans->to->id);
#endif
matched:
                if (pmatch && array_len(trans->start_capture))
                    for (int j = 0; j < trans->start_capture->len; j++)
                        if (trans->start_capture->val[j] != -1) {
                            /* record only the first group enter */
                            if (pmatch[trans->start_capture->val[j]].rm_so == -1) {
                                pmatch[trans->start_capture->val[j]].rm_so = (p - string);
#ifdef RE_DEBUG
                                printf(" enter group %d", trans->start_capture->val[j]);
#endif
                            }
                        }

                if (pmatch && array_len(trans->end_capture))
                    for (int j = 0; j < trans->end_capture->len; j++)
                        if (trans->end_capture->val[j] != -1) {
                            /* update so we get the last group exit */
                            pmatch[trans->end_capture->val[j]].rm_eo = (p - string);
#ifdef RE_DEBUG
                            printf(" exit group %d", trans->end_capture->val[j]);
#endif
                        }

                state = trans->to;
#ifdef RE_DEBUG
                printf("\n");
#endif
                goto next;
            }
            /* Also check if we have an exit option */
            else if (((!*p || *p == '\n') || !has_any) && (trans->match == TERM)) {
#ifdef RE_DEBUG
                printf(" term               ");
#endif
                if (pmatch && array_len(trans->end_capture))
                    for (int j = 0; j < trans->end_capture->len; j++)
                        if (trans->end_capture->val[j] != -1) {
                            pmatch[trans->end_capture->val[j]].rm_eo = (p - string);
#ifdef RE_DEBUG
                            printf(" exit group %d", trans->end_capture->val[j]);
#endif
                        }
                if (pmatch)
                    pmatch[0].rm_eo = (p - string);

#ifdef RE_DEBUG
                printf(" term '%c' '%c' %d '%s'\n", *p, *(p+1), has_any, string);
#endif
                found = true;
                goto done;
            }
        }

#ifdef RE_DEBUG
        printf(" out has_any=%d", has_any);
#endif

        if (has_any != -1 && (*p && *p != '\n')) {
            //printf(" any");
            i = has_any;
            goto matched;
        }

#ifdef RE_DEBUG
        printf(" no match\n");
#endif
        state = preg->priv;

next:
        /* Prevent going beyond the string termination */
        if (*p == 0 || *p == '\n')
            break;
        p++;
    }

done:
    if (pmatch)
        for (size_t i = 0; i < len; i++)
            if (pmatch[i].rm_eo == -1)
                pmatch[i].rm_so = -1;
    return found ? 0 : REG_NOMATCH;
}

int regcomp(regex_t *restrict preg, const char *restrict regex, int cflags)
{
    node_t *(*node_lookup)[] = NULL;
    node_t *node = NULL;
    node_t *root = NULL;
    _re_stack_t *node_stack = NULL;
    queue_t *q = NULL;
    dfa_state_t *list = NULL;
    uint8_t *tmp_are = NULL;
    token_t qn;
    int i = 1;

    if (preg == NULL || regex == NULL) {
        return -1;
    }

    memset(preg, 0, sizeof(regex_t));

    preg->cflags = cflags;

#ifdef RE_DEBUG
    printf("0. regex:     %s\n", regex);
#endif
    uint8_t *is_match = NULL;

    /* process argv[1] which contains ASCII ERE */
    size_t is_match_len;
    if ((tmp_are = augment(regex, &is_match, &is_match_len)) == NULL || is_match == NULL)
        goto fail;

#ifdef RE_DEBUG
    printf("1. tokenised: %s\n", tmp_are);
    printf("    is_match: ");
    for (size_t i = 0; i < strlen((char *)tmp_are); i++) {
        if (is_match[i])
            printf("M");
        else
            printf(".");
    }
    printf("\n");
#endif

    /* convert to RPN */
    if ((q = yard(tmp_are, is_match, is_match_len)) == NULL)
        goto fail;

#ifdef RE_DEBUG
    printf("2. RPN:       ", q->head, q->tail);
    for (int i = q->head + 1; i <= q->tail; i++) {
        token_t tmp;
        tmp.val = (((uint64_t *)q->data)[i]);
        printf("%c", tmp.t.token);
        if(tmp.t.group != -1) {
            printf("{");
            if (tmp.t.token == OPEN) printf("%d", tmp.t.group);
            printf(",");
            if (tmp.t.token == CLOSE) printf("%d", tmp.t.group);
            printf("} ");
        } else
            printf(" ");
    }
    printf("\n");
#endif

    if ((node_stack = alloc_stack(q->len, ET_VOID_T)) == NULL)
        goto fail;
    if ((node_lookup = malloc(sizeof(node_t *) * (size_t)(q->len + 1))) == NULL)
        goto fail;
    memset(node_lookup, 0, sizeof(node_t *) * (size_t)(q->len + 1));

#ifdef RE_DEBUG
    printf("3. build AST\n");
#endif

    /* Build the Abstract Syntax Tree */
    while (dequeue(q, &qn))
    {
#ifdef RE_DEBUG
        printf("build_ast: pop: ");
        print_token(&qn);
#endif

        if ((node = alloc_node(q->len)) == NULL)
            goto fail;

        while (qn.t.token == OPEN || qn.t.token == CLOSE) {
            if (qn.t.token == OPEN) {
                if (array_append(node->start_groups, qn.t.group))
                    goto fail;
            } else { /* CLOSE */
                if (array_append(node->end_groups, qn.t.group))
                    goto fail;
            }

            if (!dequeue(q, &qn))
                goto fail;
#ifdef RE_DEBUG
            printf("build_ast: pop: ");
            print_token(&qn);
#endif
        }

        node->type = qn.t.token;
#ifdef RE_DEBUG
        print_node(node, 0);
#endif

        if (is_operator(qn.t.token))
        {
            node->pos = -1;

            if (!pop(node_stack, &node->right))
                goto fail;

            /* STAR only has c1 not c1 & c2 */
            if (qn.t.token != STAR && !pop(node_stack, &node->left))
                goto fail;
#ifdef RE_DEBUG
            printf("build_ast: set right to %c",
                    node->right->type);
            if (node->left)
                printf(" and left to %c",
                        node->left->type);
            printf("\n");
#endif
        }
        else
        {
            (*node_lookup)[i] = node;
            node->pos = i++;
        }

        if (!push(node_stack, node))
            goto fail;

    }

    if (!peek(node_stack, &root))
        goto fail;

    /* fill in parent, calc firstpos, lastpos, nullable */
    fix_parents(root);
    /* then calculate follow pos */
    fix_followpos(root, node_lookup);

#ifdef RE_DEBUG
    printf("3. build DFA\n");
#endif
    /* construct the DFA */
    if ((list = build_dfa(root, node_lookup)) == NULL)
        goto fail;

#ifdef RE_DEBUG
    printf("4. Final Dstates\n");
    for(dfa_state_t *tdfa = list; tdfa; tdfa=tdfa->next)
        print_state(tdfa);
    printf("\n");
#endif

    errno = 0;

fail:
    if (errno) for (dfa_state_t *next = NULL, *st = list; st; st = next) {
        next = st->next;
        free_dfa_state(st);
        list = NULL;
    }

    if (node_lookup)
        free(node_lookup);

    if (errno != 0)
        perror("main");

    if (tmp_are)
        free(tmp_are);

    if (q)
        free_queue(q);

    if (node_stack) {
        while (pop(node_stack, &node))
            free_node(node);

        free_stack(node_stack);
    }

    while(list && list->next)
        list = list->next;

    preg->priv = list;

#ifdef RE_DEBUG
    printf("errno=%d\n", errno);
#endif
    return errno ? -1 : 0;
}
/* vim: set expandtab ts=4 sw=4: */

[[maybe_unused]] static void hexdump(const char *tmp)
{
    if (tmp && tmp != (char *)-1)
        while (*tmp)
        {
            if (isprint(*tmp)) printf("%c", *tmp);
            else
                printf("0x%03x", *tmp);

            tmp++;
            if (*tmp)
                printf(" ");
        }
}


[[gnu::nonnull]] static int get_termcap_idx(const char *capname, char type)
{
    for (int i = 0; term_caps[i].short_name; i++)
    {
        if (strcmp(capname, term_caps[i].short_name))
            continue;

        if (type && type != term_caps[i].type)
            continue;

        return i;
    }

    return -1;
}

[[gnu::nonnull]] void _fc_free_terminfo(struct terminfo *term)
{
    if (term->name)
        free(term->name);
    if (term->desc)
        free(term->desc);

    for (int i = 0; term_caps[i].short_name; i++)
        switch (term_caps[i].type)
        {
            case 's':
                if (term->data[i].string_entry)
                    free(term->data[i].string_entry);
                break;
            case '#':
            case 'b':
                break;
        }

    free(term);
}

[[gnu::nonnull(1)]] static struct terminfo *parse_terminfo(const char *term_name, int *errret)
{
    FILE *tinfo;
    char  buf[BUFSIZ];
    char  tmpbuf[BUFSIZ];
    char *ptr, *tok, *desc;
    int   rc;

    struct terminfo *ret;

    ret   = NULL;
    rc    = 0;
    tinfo = NULL;
    ptr   = NULL;
    tok   = buf;
    desc  = NULL;

    memset(buf, 0, sizeof(buf));
    memset(tmpbuf, 0, sizeof(tmpbuf));

    if (snprintf(buf, sizeof(buf), "%s%c/%s", terminfo_location, term_name[0], term_name) >= ((int)sizeof(buf) - 2)) {
        if (errret)
            *errret = 0;
        else
            warnx("Overflow on terminfo file location");

        return NULL;
    }

    if ((tinfo = fopen(buf, "r")) == NULL) {
        if (errret)
            *errret = 0;
        else
            warn("Unable to open terminfo <%s>", buf);

        return NULL;
    }

    rc = fread(buf, sizeof(buf), 1, tinfo);

    if (rc >= (int)(sizeof(buf) - 2) || ferror(tinfo))
        goto malformed;

    /* this section needs improving to handle more complex
     * white space & split line arrangements */

    while (*tok && isspace(*tok))
        tok++;

    if (!*tok)
        goto malformed;

    if (*tok == '#') {
        while (*tok && *tok != '\n') tok++;
        if (*tok == '\n') tok++;
        if (!*tok) goto malformed;
    }

    /* find the first comma (delimiting the name(s) from attributes */
    if ((ptr = strtok(tok, ",")) == NULL)
        goto malformed;

    if ((ret = calloc(1, sizeof(struct terminfo))) == NULL) {
        if (errret)
            *errret = 0;
        else
            warn("calloc");

        goto error;
    }

    /* vt100|vt100-am|dec vt100 (w/advanced video), */
    if ((desc = strrchr(ptr, '|')) != NULL) {
        ret->desc = strdup(desc + 1);
        ret->name = strndup(ptr, desc - ptr);
    } else {
        ret->name = strdup(ptr);
        ret->desc = strdup("");
    }


    /* xon, * cols#80, * bold=\E[1m$<2>, */
    while((ptr = strtok(NULL, ",")) != NULL)
    {
        while (*ptr && isspace(*ptr)) ptr++;
        if (!*ptr)
            continue;

        char type;
        char escstr[BUFSIZ];
        char *tmpptr;

        if ((tok = strchr(ptr, '=')) != NULL) {
            tok++;
            memset(tmpbuf, 0, sizeof(tmpbuf));
            strncat(tmpbuf, ptr, tok - ptr - 1);
            tmpbuf[tok-ptr-1] = '\0';

            int offset = 0;

            /* expand ^. and \. */
            for (tmpptr = tok; *tmpptr; )
            {
                if (*tmpptr == '^') {
                    tmpptr++;
                    if (*tmpptr >= 'A' && *tmpptr <= 'Z')
                        escstr[offset++] = *tmpptr - 'A' + 1;
                    else
                        switch(*tmpptr)
                        {
                            case '?': escstr[offset++] = 127; break;
                            case '@': escstr[offset++] = 0; break;
                            case '[': escstr[offset++] = 27; break;
                            case '\\': escstr[offset++] = 28; break;
                            case ']': escstr[offset++] = 29; break;
                            case '^': escstr[offset++] = 30; break;
                            case '-': escstr[offset++] = 31; break;
                            default:
                                      goto malformed;
                        }
                    tmpptr++;
                } else if (*tmpptr =='\\') {
                    tmpptr++;
                    if (isdigit(*tmpptr) &&
                            *(tmpptr+1) && isdigit(*(tmpptr+1)) &&
                            *(tmpptr+2) && isdigit(*(tmpptr+2))) {
                        char oct[4] = {0};
                        strncat(oct, tmpptr, 3);
                        oct[3] = '\0';
                        /* TODO error checking */
                        escstr[offset++] = strtol(oct, NULL, 8);
                        break;
                    }
                    switch(*tmpptr)
                    {
                        case 'E':
                        case 'e':
                            escstr[offset++] = 27;
                            break;
                        case 'n': escstr[offset++] = '\n'; break;
                        case 'l': escstr[offset++] = '\n'; break;
                        case 'r': escstr[offset++] = '\r'; break;
                        case 't': escstr[offset++] = '\t'; break;
                        case 'b': escstr[offset++] = '\b'; break;
                        case 'f': escstr[offset++] = '\f'; break;
                        case 's': escstr[offset++] = ' '; break;
                        case '0': escstr[offset++] = (char)0200; break;
                        case '\\':
                        case '^':
                        case ',':
                        case ':':
                                  escstr[offset++] = *tmpptr;
                                  break;
                        default:
                                  warnx("malformed <%c>", *tmpptr);
                                  goto malformed;
                    }
                    tmpptr++;
                } else
                    escstr[offset++] = *tmpptr++;

            }
            escstr[offset] = '\0';
            type = 's';
            /* str_entry */
        } else if ((tok = strchr(ptr, '#')) != NULL) {
            tok++;
            memset(tmpbuf, 0, sizeof(tmpbuf));
            strncat(tmpbuf, ptr, tok - ptr - 1);
            tmpbuf[tok-ptr-1] = '\0';
            type = '#';
            /* int_entry */
        } else {
            snprintf(tmpbuf, sizeof(tmpbuf), "%s", ptr);
            type = 'b';
            /* bool_entry */
        }

        bool found;
        int i;

        for (found = false, i = 0; term_caps[i].short_name; i++)
            if (!strcmp(tmpbuf, term_caps[i].short_name)) {
                found = true;
                break;
            }

        if (!found) {
            warnx("%s not found", tmpbuf);
            goto malformed;
        }

        switch(type)
        {
            case 'b':
                ret->data[i].bool_entry = true;
                break;
            case 's':
                ret->data[i].string_entry = strdup(escstr);
                break;
            case '#':
                if (!strncmp("0x", tok, 2))
                    ret->data[i].int_entry = strtol(tok + 2, NULL, 16);
                else if (isdigit(*tok))
                    ret->data[i].int_entry = strtol(tok, NULL, 10);
                else
                    goto malformed;
                break;
        }
    }

done:
    if (tinfo)
        fclose(tinfo);

    return ret;

malformed:
    if (errret)
        *errret = 0;
    else
        warnx("Malformed terminfo <%s>", ptr);

error:
    if (ret)
        _fc_free_terminfo(ret);

    ret = NULL;
    goto done;

}

[[gnu::nonnull(1)]] static struct terminfo *load_terminfo(const char *name, int *errret)
{
    struct terminfo *ret;

    for (ret = termdb; ret; ret=ret->next)
        if (!strcmp(name, ret->name))
            return ret;

    if ((ret = parse_terminfo(name, errret)) == NULL)
        return NULL;

    ret->next = termdb;
    termdb = ret;

    return ret;
}

/*
 * public functions
 */

int tigetflag(const char *capname)
{
    int idx;

    if (capname == NULL)
        goto fail;

    if ((idx = get_termcap_idx(capname, 'b')) == -1)
        goto fail;

    return ((struct terminfo *)cur_term->terminfo)->data[idx].bool_entry;
fail:
    return -1;
}

int tigetnum(const char *capname)
{
    int idx;

    if (capname == NULL)
        goto fail;

    if ((idx = get_termcap_idx(capname, '#')) == -1)
        goto fail;

    return ((struct terminfo *)cur_term->terminfo)->data[idx].int_entry;
fail:
    return -2;
}

char *tigetstr(const char *capname)
{
    int idx;

    if (capname == NULL)
        goto fail;

    if ((idx = get_termcap_idx(capname, 's')) == -1)
        goto fail;

    return ((struct terminfo *)cur_term->terminfo)->data[idx].string_entry;
fail:
    return (char *)-1;
}

char *tiparm(const char *cap, ...)
{
    if (cap == NULL)
        return NULL;

    //int idx;

    //if ((idx = get_termcap_idx(cap, 0)) < 0)
    //    return NULL;

    memset(tiparm_ret, 0, sizeof(tiparm_ret));

    if (cur_term == NULL || cur_term->terminfo == NULL) {
        warnx("tiparm: <%s>: fail", cap);
        return NULL;
    }

    //const char *src = ((struct terminfo *)cur_term->terminfo)->data[idx].string_entry;
    const char *src_ptr = cap;
    char *dst_ptr = tiparm_ret;

    if (src_ptr == NULL)
        return NULL;

    char *str_arg[10];
    int int_arg[10];
    int num_arg = 0;
    va_list ap;

    va_start(ap, cap);
    memset(str_arg, 0, sizeof(str_arg));
    memset(int_arg, 0, sizeof(int_arg));

    union stack_ent {
        char *str;
        int   val;
    };

    const int max_stack = 100;
    int cur_stack = 0;

    union stack_ent stack[max_stack];

    while (*src_ptr)
    {
        //printf("tiparm_ret: <");
        //hexdump(tiparm_ret);
        //printf(">\n");
        //printf("dst_ptr: <");
        //hexdump(dst_ptr);
        //printf(">\n");

        if (*src_ptr != '%') {
            //printf("copying <%c>\n", *src_ptr);
            *dst_ptr = *src_ptr;
            dst_ptr++;
            goto next;
        }

        if (!*(++src_ptr))
            goto fail;

        //printf("src_ptr = %c\n", *src_ptr);
        switch (*src_ptr)
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '.':
            case ':':
                //case '+': /* this clashes with %+ ? */
            case '#':
                goto next;
            case 'd':
                {
                    //printf("%%d: cur_stack=%d\n", cur_stack);
                    if (cur_stack == 0)
                        goto fail;
                    cur_stack--;
                    int len = snprintf(dst_ptr, sizeof(tiparm_ret), "%d", stack[cur_stack].val);
                    //printf("%%d: stack=%d len=%d val=%d dst_ptr=%s\n", cur_stack, len, stack[cur_stack].val, dst_ptr);
                    dst_ptr += len - 1;
                    //printf("%%d: new_dst_ptr: %s\n", dst_ptr);
                }
                goto next;
            case 'o':
            case 'x':
            case 'X':
                //case 's': /* this clashes with %s ? */
                warnx("unsupported printf expansion");
                goto next;
            case '%':
                *dst_ptr++ = '%';
                goto next;
            case 'c': /* pop() printf %c arg */
                if (cur_stack == 0)
                    goto fail;
                *dst_ptr++ = (char)stack[cur_stack--].val;
                goto next;
            case 's': /* pop() printf %s arg */
                if (cur_stack == 0)
                    goto fail;
                int len = strlen(stack[cur_stack].str);
                strcpy(dst_ptr, stack[cur_stack--].str);
                dst_ptr += len;
                goto next;
            case 'l': /* pop() printf strlen(%s) arg */
                if (cur_stack == 0)
                    goto fail;
                dst_ptr += snprintf(dst_ptr, dst_ptr - tiparm_ret, "%lu", strlen(stack[cur_stack--].str));
                goto next;
            case '{':
                {
                    src_ptr++;
                    const char *from = src_ptr;
                    while (*src_ptr && isdigit(*src_ptr))
                        src_ptr++;
                    if (*src_ptr != '}')
                        goto fail;
                    long val = strtol(from, NULL, 10);
                    stack[cur_stack].val = val;
                    cur_stack++;
                    goto next;
                }
            case 'i': /* +1 to first 2 parameters */
                while (num_arg < 2) {
                    int_arg[num_arg] = va_arg(ap, int);
                    //printf("%%i set [%d]=[%d]\n", num_arg, int_arg[num_arg]);
                    num_arg++;
                }
                int_arg[0]++;
                int_arg[1]++;
                //printf("%%i: set to [%d,%d]\n", int_arg[0], int_arg[1]);
                goto next;
            case '?': /* %? expr %t thenpart %e elsepart %; */
                {
                    if (*(++src_ptr) != '%')
                        goto fail;

                    if (!*(++src_ptr))
                        goto fail;

                    int val = 0;

                    /* parse expr */
next_expr:
                    printf("if: parse expr <%s>\n", src_ptr);
                    switch(*src_ptr) {
                        /* TODO add other things here? somehow reuse the main? */
                        case 'p':
                            src_ptr++;
                            if (!isdigit(*src_ptr))
                                goto fail;
                            int digit = *src_ptr++ - '0' - 1;
                            if (digit < 0 || digit > 8)
                                goto fail;
                            while (num_arg <= digit) {
                                int_arg[num_arg] = va_arg(ap, int);
                                num_arg++;
                            }
                            if (cur_stack >= max_stack)
                                goto fail;
                            stack[cur_stack].val = int_arg[num_arg];
                            cur_stack++;
                            break;

                        case '|':
                            {
                                src_ptr++;
                                if (cur_stack < 2)
                                    goto fail;
                                int a = stack[cur_stack--].val;
                                int b = stack[cur_stack--].val;
                                stack[cur_stack++].val = a|b;
                                break;
                            }

                        default:
                            goto fail;
                    }
                    printf("if: parse post expr <%s>\n", src_ptr);

                    if (*src_ptr == '%' && *(src_ptr+1) && *(src_ptr+1) != 't' && *(src_ptr+1) != 'e') {
                        src_ptr++;
                        goto next_expr;
                    }

                    /* thenpart */
                    if (cur_stack == 0)
                        goto fail;

                    val = stack[cur_stack--].val;
                    printf("if: then part val=%d\n", val);

                    if (val) {
                        printf("if: true <%s>\n", src_ptr);
                        if (*src_ptr++ != '%')
                            goto fail;
                        if (*src_ptr++ != 't')
                            goto fail;
                        printf("if: <%s>\n", src_ptr);
                        while (*src_ptr && *src_ptr != '%')
                        {
if_again:
                            *dst_ptr++ = *src_ptr++;
                        }
                        if (!*src_ptr)
                            goto fail;
                        if (*(++src_ptr) == '%')
                            goto if_again;
                        if (*src_ptr == ';') {
                            goto next;
                        } else if(*src_ptr == 'e') {
                            printf("if: else_skip <%s>\n", src_ptr);
                            /* skip over the elsepart */
                            while (*src_ptr && *src_ptr != '%')
                            {
else_skip_again:
                                src_ptr++;
                            }
                            if (!*src_ptr)
                                goto fail;
                            printf("if: else not null\n");
                            if (*(++src_ptr) == '%')
                                goto else_skip_again;
                            else if (*src_ptr == ';')
                                goto next;
                            else {
                                printf("if: else fail <%s>\n", src_ptr);
                                goto fail;
                            }
                            printf("then done: <%s>\n", src_ptr);
                        }
                    } else /* elsepart */ {
                        printf("if: elsepart\n");
                        /* skip over thenpart */
                        if (*src_ptr++ != '%')
                            goto fail;
                        if (*src_ptr++ != 't')
                            goto fail;
                        printf("if: elsepart skipping then <%s>\n", src_ptr);
                        while (*src_ptr && *src_ptr != '%')
                        {
else_then_skip:
                            src_ptr++;
                        }
                        printf("if: elsepart skipped then <%s>\n", src_ptr);
                        if (!*src_ptr)
                            goto fail;
                        src_ptr++;
                        if (*src_ptr == '%')
                            goto else_then_skip;
                        else if (*src_ptr == ';')
                            goto no_else_end;
                        else if (*src_ptr !='e')
                            goto fail;
                        src_ptr++;
                        /* now we're at the elsepart */
                        printf("if: at elsepart <%s>\n", src_ptr);
                        while (*src_ptr && *src_ptr != '%')
                        {
else_again:
                            *dst_ptr++ = *src_ptr++;
                        }
                        if (!*src_ptr)
                            goto fail;
                        src_ptr++;
                        if (*src_ptr == '%')
                            goto else_again;
                        else if (*src_ptr == ';') {
no_else_end:
                            printf("if: FINISHED\n");
                            goto next;
                        } else
                            goto fail;
                    }
                    goto next;
                }
            case 'p':
                //printf("%%p\n");
                src_ptr++;
                //printf("%%p: %c\n", *src_ptr);
                if (isdigit(*src_ptr)) {
                    int digit = *src_ptr - '0' - 1;
                    if (digit < 0 || digit > 8)
                        goto fail;
                    while (num_arg <= digit) {
                        /* WTF to do here? */
                        int_arg[num_arg] = va_arg(ap, int);
                        //printf("%%p: [%d] = [%d]\n", num_arg, int_arg[num_arg]);
                        num_arg++;
                    }
                    if (cur_stack >= max_stack)
                        goto fail;
                    //printf("%%p: push %d[%d] to %d\n", digit, int_arg[digit], cur_stack);
                    stack[cur_stack].val = int_arg[digit];
                    cur_stack++;
                    goto next;
                } else if (isupper(*src_ptr)) {
                    src_ptr++;
                } else if (islower(*src_ptr)) {
                    src_ptr++;
                } else {
                    warnx("unsupported %%p <%s>", src_ptr);
                    goto fail;
                }
                goto next;
            case '|':
                {
                    if (cur_stack < 2)
                        goto fail;
                    int a = stack[cur_stack--].val;
                    int b = stack[cur_stack--].val;
                    stack[cur_stack++].val = a|b;
                    goto next;
                }
            case '+':
                {
                    if (cur_stack < 2)
                        goto fail;
                    int a = stack[cur_stack--].val;
                    int b = stack[cur_stack--].val;
                    stack[cur_stack++].val = a+b;
                    goto next;
                }
            case '*':
                {
                    if (cur_stack < 2)
                        goto fail;
                    int a = stack[cur_stack--].val;
                    int b = stack[cur_stack--].val;
                    stack[cur_stack++].val = a*b;
                    goto next;
                }
            case '/':
                {
                    if (cur_stack < 2)
                        goto fail;
                    int a = stack[cur_stack--].val;
                    int b = stack[cur_stack--].val;
                    stack[cur_stack++].val = a/b;
                    goto next;
                }


            default:
                warnx("unsupported command <%c>", *src_ptr);
                goto fail;
        }
next:
        src_ptr++;
    }

    va_end(ap);
    //printf("tiparm_ret = <%s>\n", tiparm_ret);
    return tiparm_ret;
fail:
    va_end(ap);
    printf("tiparm: fail\n");
    return NULL;
}

int tputs(const char *str, int affcnt __attribute__((unused)), int (*putfunc)(int))
{
    if (str == NULL || putfunc == NULL)
        return ERR;

    register const char *ptr = str;
    while (*ptr)
    {
        putfunc(*ptr++);
    }

    return OK;
}

int putp(const char *str)
{
    return tputs(str, 1, putchar);
}

TERMINAL *set_curterm(TERMINAL *nterm)
{
    TERMINAL *oterm;

    oterm = cur_term;
    cur_term = nterm;

    return oterm;
}

int setupterm(char *term, int fildes, int *errret)
{
    const char *term_name;

    if (term && strlen(term))
        term_name = term;
    else if ((term_name = getenv("TERM")) == NULL || !strlen(term_name))
        term_name = "unknown";

    TERMINAL *tmp_term;
    struct terminfo *tinfo;

    if ((tinfo = load_terminfo(term_name, errret)) == NULL)
        return ERR;

    if ((tmp_term = malloc(sizeof(TERMINAL))) == NULL) {
        if (errret)
            *errret = 0;
        else
            warn("setupterm: malloc");
        return ERR;
    }

    tmp_term->fd = fildes;
    tmp_term->terminfo = tinfo;

    TERMINAL *oterm = NULL;

    oterm = set_curterm(tmp_term);

    struct winsize ws;

    if (ioctl(tmp_term->fd, TIOCGWINSZ, &ws) != -1) {
        char new_lines[32], new_columns[32];
        snprintf(new_lines,   sizeof(new_lines),   "%u", ws.ws_row);
        snprintf(new_columns, sizeof(new_columns), "%u", ws.ws_col);
        setenv("LINES", new_lines, true);
        setenv("COLUMNS", new_columns, true);
    }

    if (nc_use_env && getenv("LINES") != NULL)
        tmp_term->lines = atoi(getenv("LINES"));
    else if (tigetnum("lines") != -1)
        tmp_term->lines = tigetnum("lines");
    else
        goto fail;

    if (nc_use_env && getenv("COLUMNS") != NULL)
        tmp_term->columns = atoi(getenv("COLUMNS"));
    else if (tigetnum("cols") != -1)
        tmp_term->columns = tigetnum("cols");
    else
        goto fail;

    LINES = tmp_term->lines;
    COLS = tmp_term->columns;

    const struct {
        const char *const key;
        const int         id;
    } keys[] = {
        { "ka1"    ,  KEY_A1        },
        { "ka3"    ,  KEY_A3        },
        { "kb2"    ,  KEY_B2        },
        { "kbs"    ,  KEY_BACKSPACE },
        { "kc1"    ,  KEY_C1        },
        { "kc3"    ,  KEY_C3        },
        { "kcub1"  ,  KEY_LEFT      },
        { "kcud1"  ,  KEY_DOWN      },
        { "kcuf1"  ,  KEY_RIGHT     },
        { "kcuu1"  ,  KEY_UP        },
        { "kend"   ,  KEY_END       },
        { "kent"   ,  KEY_ENTER     },
        { "kf0"    ,  KEY_F0        },
        { "kf1"    ,  KEY_F(1)      },
        { "kf2"    ,  KEY_F(2)      },
        { "kf3"    ,  KEY_F(3)      },
        { "kf4"    ,  KEY_F(4)      },
        { "kf5"    ,  KEY_F(5)      },
        { "kf6"    ,  KEY_F(6)      },
        { "kf7"    ,  KEY_F(7)      },
        { "kf8"    ,  KEY_F(8)      },
        { "kf9"    ,  KEY_F(9)      },
        { "kf10"   ,  KEY_F(10)     },
        { "kf11"   ,  KEY_F(11)     },
        { "kf12"   ,  KEY_F(12)     },
        { "khome"  ,  KEY_HOME      },
        { "knp"    ,  KEY_NPAGE     },
        { "kpp"    ,  KEY_PPAGE     },

        { NULL, -1 }
    };

    for (int i = 0; keys[i].key != NULL; i++)
    {
        char *tmp;

        if ((tmp = tigetstr(keys[i].key)) == NULL || tmp == (char *)-1) {
            //fprintf(stderr, "setupterm: fail on '%s' res %s\n",
            //        keys[i].key, tmp == (char *)-1 ? "-1" : tmp);
            continue;
        }

        tmp_term->keys[keys[i].id].id = tmp;
        tmp_term->keys[keys[i].id].len = strlen(tmp);
    }

    return OK;

fail:
    set_curterm(oterm);
    if (tmp_term)
        free(tmp_term);
    if (errret)
        *errret = 0;
    else
        warnx("setupterm: cannot find line or column information");
    return ERR;
}

static int _getch(TERMINAL *term, int out[1])
{
    char buf[8] = {0};
    ssize_t len;

    len = read(term->fd, buf, sizeof(buf));

    //fprintf(stderr, "_getch: read: %d\n", len);

    if (len == -1)
        return -1;

    if (len == 0) {
        *out = 0;
        return 0;
    }

    if (len == 1) {
        *out = buf[0];
        return 0;
    }

    if (buf[0] == 0x1b) {
        //fprintf(stderr, "_getch: checking ESC key match\n");
        //fprintf(stderr, "_getch: <");
        //hexdump(stderr, buf);
        //fprintf(stderr, ">\n");

        for (int i = 0; i < NUM_KEYS; i++)
        {
            if (cur_term->keys[i].id == NULL)
                continue;

            //fprintf(stderr, "_getch: comparing to <");
            //hexdump(stderr, cur_term->keys[i].id);
            //fprintf(stderr, ">\n");

            if (strncmp(buf, cur_term->keys[i].id, cur_term->keys[i].len))
                continue;

            //fprintf(stderr, "_getch: found %d\n", i);

            *out = i;

            if (cur_term->keys[i].len != len) {
                ; /* handle unread characters TODO */
            }

            return 0;
        }

        return -1;

        //fprintf(stderr, "_getch: found a match!\n");
    }

    return -1;
}


static int init_extended_pair(int pair, int f, int b)
{
    if (stdscr == NULL)
        return ERR;

    if (pair < 0 || pair > stdscr->scr->pairs)
        return ERR;

    if (f < 0 || f > stdscr->scr->colors)
        return ERR;
    
    if (b < 0 || b > stdscr->scr->colors)
        return ERR;

    const char *initp = tigetstr("initp");

    if (initp == NULL || initp == (char *)-1)
        return ERR;
    
    return OK;
}


static int _putchar_buffer(int c)
{
    if (doupdate_bufptr == NULL || doupdate_bufptr >= doupdate_bufend)
        return -1;

    *doupdate_bufptr++ = (char)c;

    return 0;
}

static int _putchar_cur_term(int c)
{
    if (cur_term == NULL || cur_term->fd == -1)
        return -1;

    unsigned char ch = (char)c;

    return write(cur_term->fd, &ch, 1);
}

[[maybe_unused]] static void hexdump_fp(FILE *fp, const char *tmp, ssize_t len)
{
    ssize_t pos = 0;

    if (tmp && tmp != (char *)-1)
        while (*tmp && (!len || pos < len))
        {
            if (*tmp == '\n')
                fprintf(fp, "\n> ");
            else if(ispunct(*tmp) || *tmp == ' ' || isalnum(*tmp))
                fprintf(fp, "%c", *tmp);
            else
                fprintf(fp, "0x%03x", *tmp);

            tmp++;
            pos++;
            if (*tmp)
                fprintf(fp, " ");
        }
}


/*
 * public functions
 */

int def_prog_mode(void)
{
    /* TODO */
    return OK;
}

bool has_colors(void)
{
    if (stdscr == NULL)
        return false;

    if (stdscr->scr->colors && stdscr->scr->pairs)
        return true;
    
    const int colors  = tigetnum("colors");
    const int pairs   = tigetnum("pairs");

    return (colors > 0 && pairs > 0);
}

bool can_change_color(void)
{
    if (stdscr == NULL)
        return false;

    const bool ccc = tigetflag("ccc");

    if (ccc <= 0)
        return false;

    return true;
}

int init_pair(short pair, short f, short b)
{
    return init_extended_pair(pair, f, b);
}

int start_color(void)
{
    if (stdscr == NULL)
        return ERR;

    const int colors  = tigetnum("colors");
    const int pairs   = tigetnum("pairs");
    const char *initc = tigetstr("initc");

    if (colors <= 0 || pairs <= 0)
        return ERR;

    COLORS = colors;
    COLOR_PAIRS = pairs;

    stdscr->scr->colors = COLORS;
    stdscr->scr->pairs = COLOR_PAIRS;

    if (initc != NULL && initc != (char *)-1) {
        tputs(initc, 1, _putchar_buffer);
    }

    //init_color(0, COLOR_WHITE, COLOR_BLACK);

    return OK;
}

/* clearok: unspecified
 * idlok: FALSE
 * leaveok: FALSE
 * scrollok: FALSE
 */
WINDOW *newwin(int nlines, int ncols, int y, int x)
{
    WINDOW *ret;
    int lines = nlines ? nlines : (LINES - y);

    if ((ret = calloc(1, sizeof(WINDOW) + (sizeof(struct _fc_window_line_data) * (lines + 1)))) == NULL)
        return NULL;

    ret->x = x;
    ret->y = y;
    ret->lines = lines;
    ret->cols = ncols ? ncols : (COLS - x);
    ret->clearok = TRUE;
    ret->leaveok = FALSE;
    ret->scrollok = FALSE;
    ret->idlok = FALSE;

    for (int i = 0; i < (lines + 1); i++) {
        if ( (ret->line_data[i].line = calloc(1, sizeof(chtype) * (1 + ret->cols))) == NULL) {
            delwin(ret);
            return NULL;
        }
        memset(ret->line_data[i].line, ' ', ret->cols);
        ret->line_data[i].touched = true;
    }

    return ret;
}

bool nc_use_env = TRUE;

void use_env(bool bf)
{
    nc_use_env = bf;
}

int keypad(WINDOW *win, bool bf)
{
    win->keypad = bf;

    const char *smkx = tigetstr("smkx");
    const char *rmkx = tigetstr("rmkx");

    if (bf) {
        if (smkx)
            tputs(smkx, 1, _putchar_cur_term);
    } else {
        if (rmkx)
            tputs(rmkx, 1, _putchar_cur_term);
    }

    return TRUE;
}

SCREEN *newterm(const char *type, FILE *out, FILE *in)
{
    int rc;
    SCREEN *ret = NULL;


    if (out == NULL || in == NULL)
        return NULL;

    setvbuf(in, NULL, _IONBF, 0);
    setvbuf(out, NULL, _IONBF, 0);

    rc = setupterm(type ? (char *)type : getenv("TERM"), fileno(out), NULL);

    if (rc == ERR)
        goto fail;

    if ((ret = calloc(1, sizeof(SCREEN))) == NULL)
        goto fail;

    ret->term = cur_term;
    ret->outfd = out;
    ret->infd = in;

    ret->_infd = fileno(in);
    ret->_outfd = fileno(out);

    if (tcgetattr(ret->_infd, &ret->shell_in) == -1) {
        goto fail;
    }

    if (tcgetattr(ret->_outfd, &ret->shell_out) == -1) {
        goto fail;
    }
    
    memcpy(&ret->save_in, &ret->shell_in, sizeof(ret->save_in));
    memcpy(&ret->save_out, &ret->shell_out, sizeof(ret->save_in));

    /* where should this go ? */
    struct termios tios;
    if (tcgetattr(ret->_outfd, &tios) == -1) {
        goto fail;
    }
    tios.c_oflag &= ~OCRNL;
    if (tcsetattr(ret->_outfd, 0, &tios) == -1) {
        goto fail;
    }

    if (tcgetattr(ret->_infd, &tios) == -1) {
        goto fail;
    }
    tios.c_iflag &= ~INPCK;
    if (tcsetattr(ret->_infd, 0, &tios) == -1) {
        goto fail;
    }

    if ((ret->defwin = newwin(0,0,0,0)) == NULL)
        goto fail;

    ret->buf_len = 16 * ret->defwin->lines * (ret->defwin->cols + 1);

    if ((ret->buffer = malloc(ret->buf_len)) == NULL) {
        goto fail;
    }

    return ret;

fail:
    if (ret) {
        if (ret->defwin)
            delwin(ret->defwin);
        if (ret->term)
            del_curterm(ret->term);
        free(ret);
    }

    return NULL;
}

int delwin(WINDOW *w)
{
    free(w);
    return OK;
}

/* The doupdate() function sends to the terminal the commands to perform any required changes. */
int doupdate(void)
{
    const char *home;

    if ((home = tigetstr("home")) == NULL || home == (char *)-1) {
        fprintf(stderr, "doupdate: home\n");
        return ERR;
    }

    const char *clear = tigetstr("clear");
    const char *el    = tigetstr("el");
    const char *civis = tigetstr("civis");
    const char *cnorm = tigetstr("cnorm");
    const char *vpa   = tigetstr("vpa");

    if (el == (char *)-1)
        el = NULL;
    if (civis == (char *)-1)
        civis = NULL;
    if (cnorm == (char *)-1)
        cnorm = NULL;
    if (vpa == (char *)-1)
        vpa = NULL;

    //memset(stdscr->scr->buffer, ' ', ret->buf_len);

    SCREEN *scr = stdscr->scr;
    doupdate_bufptr = scr->buffer;
    doupdate_bufend = scr->buffer + scr->buf_len - 1;

    //if (civis && cnorm)
      //  tputs(civis, 1, _putchar_buffer);

    if (clear)
        tputs(clear, 1, _putchar_buffer);
    else
        tputs(home, 1, _putchar_buffer);

    for (int i = 0; i < stdscr->lines; i++)
    {
        bool last_line = (i == stdscr->lines - 1);

        if (stdscr->line_data[i].touched) {
            for (int j = 0; j < stdscr->cols; j++) {
                if (stdscr->line_data[i].line[j])
                    *doupdate_bufptr++ = stdscr->line_data[i].line[j];
                else
                    *doupdate_bufptr++ = ' ';
            }
            if (!last_line) {
                if (vpa) {
                    *doupdate_bufptr++ = '\r';
                    tputs(tiparm(vpa, i+1), 1, _putchar_buffer);
                } else
                    *doupdate_bufptr++ = '\n';
            }
            stdscr->line_data[i].touched = false;
        } else if (clear) {
            if (!last_line)
                *doupdate_bufptr++ = '\n';
        } else if (el) {
            tputs(el, 1, _putchar_buffer);
            if (!last_line)
                *doupdate_bufptr++ = '\n';
        } else {
            for (int j = 0; j < stdscr->cols; j++)
                *doupdate_bufptr++ = ' ';
            if (!last_line)
                *doupdate_bufptr++ = '\n';
        }
    }

    //if (cnorm)
      //  tputs(cnorm, 1, _putchar_buffer);

    scr->buf_ptr = doupdate_bufptr - scr->buffer;

    //fprintf(stderr, "doupdate: about to dump: ");
    //hexdump(stderr, scr->buffer, scr->buf_ptr);
    //fprintf(stderr, "\n");

    if (write(scr->_outfd, scr->buffer, scr->buf_ptr) < scr->buf_ptr) {
        fprintf(stderr, "doupdate: write\n");
        return ERR;
    }
    return OK;
}

int clearok(WINDOW *win, bool bf)
{
    win->clearok = bf;
    return OK;
}

int idlok(WINDOW *win, bool bf)
{
    win->idlok = bf;
    return OK;
}

int leaveok(WINDOW *win, bool bf)
{
    win->leaveok = bf;
    return OK;
}

int scrollok(WINDOW *win, bool bf)
{
    win->scrollok = bf;
    return OK;
}

/* The redrawwin() and wredrawln() functions inform the implementation that some or all of the information physically displayed for the specified window may have been corrupted. The redrawwin() function marks the entire window; wredrawln() marks only num_lines lines starting at line number beg_line. The functions prevent the next refresh operation on that window from performing any optimization based on assumptions about what is physically displayed there */
int wredrawln(WINDOW *win, int beg_line, int num_lines)
{
    if (win == NULL || beg_line > num_lines || num_lines > win->lines || beg_line < 0)
        return ERR;

    for (int i = beg_line; i < num_lines; i++)
        win->line_data[i].touched = true;

    return OK;
}

int redrawwin(WINDOW *win)
{
    if (win == NULL)
        return ERR;

    return wredrawln(win, 0, win->lines);
}

int wclear(WINDOW *win)
{
    if (win == NULL)
        return ERR;

    clearok(win, TRUE);

    return OK;
}

int clear(void)
{
    return wclear(stdscr);
}

int erase(void)
{
    return werase(stdscr);
}

int werase(WINDOW *win)
{
    for (int i = 0; i < win->lines - 1; i++) {
        memset(win->line_data[i].line, ' ', sizeof(chtype) * win->cols);
        win->line_data[i].touched = true;
    }

    return OK;
}

/* The wnoutrefresh() function determines which parts of the terminal may need updating */
int wnoutrefresh(WINDOW *win)
{
    if (win == NULL)
        return ERR;

    return OK;
}

static void f_clearscr(void)
{
    const char *tmp;

    tmp = tigetstr("clear");
    if (tmp != NULL)
        tputs(tmp, 1, _putchar_cur_term);
}

/* The refresh() and wrefresh() functions refresh the current or specified window. The functions position the terminal’s cursor at the cursor position of the window, except that if the leaveok() mode has been enabled, they may leave the cursor at an arbitrary position.*/
int wrefresh(WINDOW *win)
{
    if (wnoutrefresh(win) == ERR) {
        fprintf(stderr, "wrefresh: wnoutrefresh: ERR\n");
        return ERR;
    }

    if (win->clearok) {
        win->clearok = FALSE;

        if (redrawwin(win) == ERR) {
            fprintf(stderr, "wrefresh: redrawwin: ERR\n");
            return ERR;
        }

        if (win != curscr)
            f_clearscr();

        win->clearok = FALSE;
    }

    if (win == curscr)
        f_clearscr();

    if (doupdate() == ERR)
        return ERR;

    if (win->leaveok == FALSE)
        wmove(win, win->y, win->x);

    return OK;
}

/* spec is silent on which what this uses, but ncurses says stdscr */
int refresh(void)
{
    return wrefresh(stdscr);
}

int waddch(WINDOW *win, const chtype ch)
{
    if (win == NULL)
        return ERR;

    win->line_data[win->y].line[win->x++] = ch;
    win->line_data[win->y].touched = true;

    if (win->x > win->cols) {
        win->x = 0;
        win->y++;
    }
    wmove(win, win->y, win->x);

    return OK;
}

int mvwaddch(WINDOW *win, int y, int x, const chtype ch)
{
    if (wmove(win, y, x) == ERR)
        return ERR;

    return waddch(win, ch);
}

int addch(const chtype ch)
{
    return waddch(stdscr, ch);
}

int mvaddch(int y, int x, const chtype ch)
{
    return mvwaddch(stdscr, y, x, ch);
}

int echochar(const chtype ch)
{
    if (waddch(stdscr, ch) == ERR)
        return ERR;

    return wrefresh(stdscr);
}

int wechochar(WINDOW *win, const chtype ch)
{
    if (waddch(win, ch) == ERR)
        return ERR;

    return wrefresh(win);
}

void delscreen(SCREEN *sp)
{
    if (sp) {
        free(sp);
    }
}

bool isendwin(void)
{
    if (stdscr == NULL)
        return FALSE;

    return cur_screen->isendwin;
}

int endwin(void)
{
    if (cur_screen == NULL)
        return ERR;

    tcsetattr(cur_screen->_infd, TCSANOW, &cur_screen->save_in);
    tcsetattr(cur_screen->_outfd, TCSANOW, &cur_screen->save_out);

    return TRUE;
}

/* The initscr() function is equivalent to:
 * 
 *  newterm(getenv("TERM"), stdout, stdin);
 *  return stdscr;
 * 
 * The initscr() and newterm() functions initialize the cur_term external variable.
 * 
 * Upon successful completion, the initscr() function returns a pointer to stdscr. Otherwise, it does
 * not return.
 * Upon successful completion, the newterm() function returns a pointer to the specified terminal.
 * Otherwise, it returns a null pointer.
 */

WINDOW *initscr()
{
    if ((cur_screen = newterm(getenv("TERM"), stdout, stdin)) == NULL)
        return NULL;
    def_prog_mode();

    // ncurses doesn't do any of this here, but various things in newterm?
    curscr = stdscr = cur_screen->defwin;
    stdscr->scr = cur_screen;
    const char *smcup = tigetstr("smcup");
    tputs(smcup, 1, _putchar_cur_term);
    const char *sgr0 = tigetstr("sgr0");
    tputs(sgr0, 1, _putchar_cur_term);
    const char *rmir = tigetstr("rmir");
    tputs(rmir, 1, _putchar_cur_term);
    const char *smam = tigetstr("smam");
    tputs(smam, 1, _putchar_cur_term);
    const char *clear = tigetstr("clear");
    tputs(clear, 1, _putchar_cur_term);
    refresh();
    doupdate();
    /*
    fprintf(stderr, "cur_screen[%d,%d]\nstdscr[%d,%d]\nLINES=%d COLS=%d\n",
            cur_screen->term->columns,
            cur_screen->term->lines,
            stdscr->cols,
            stdscr->lines,
            LINES,
            COLS);*/
    return stdscr;
}

int beep(void)
{
    char *tp;

    if ((tp = tigetstr("bel")) != NULL)
        return tputs(tp, 1, _putchar_cur_term);
    else if ((tp = tigetstr("flash")) != NULL)
        return tputs(tp, 1, _putchar_cur_term);
    else
        return ERR;
}

int wmove(WINDOW *win, int y, int x)
{
    if (win == NULL || y < 0 || x < 0 || y > win->lines || x > win->cols)
        return ERR;

    win->x = x;
    win->y = y;

    char *tp;

    if ((tp = tiparm(tigetstr("cup"), y, x)) == NULL)
        return ERR;

    return tputs(tp, 1, _putchar_cur_term);
}

int move(int y, int x)
{
    return wmove(stdscr, y, x);
}

/* add a wide-character string */

int waddnwstr(WINDOW *win, const wchar_t *wstr, int n)
{
    if (win == NULL || wstr == NULL || n == 0)
        return ERR;
    /* TODO */
    return OK;
}

int waddwstr(WINDOW *win, const wchar_t *wstr)
{
    return waddnwstr(win, wstr, -1);
}

/* add a string of multi-byte characters */

int waddnstr(WINDOW *win, const char *str, int n)
{
    //wchar_t *dest = NULL;
    int rc = ERR;

    /*
    len = mbstowcs(NULL, str, (n == -1) ? 0 : n);

    if (len == (size_t)-1)
        goto done;

    if ((dest = calloc(1, len + 1)) == NULL)
        goto done;

    if (mbstowcs(dest, str, (n == -1) ? 0 : n) == (size_t)-1)
        goto done;

    rc = waddwstr(win, dest);

done:
    if (dest)
        free(dest);
    */

    const char *tmp = str;
    int cnt = n;

    while (cnt && *tmp)
    {
        waddch(win, *tmp);
        tmp++; cnt--;
    }

    return rc;
}

int waddstr(WINDOW *win, const char *str)
{
    return waddnstr(win, str, -1);
}

int mvwaddnstr(WINDOW *win, int y, int x, const char *str, int n)
{
    if (wmove(win, y, x) == ERR)
        return ERR;
    return waddnstr(win, str, n);
}

int addstr(const char *str)
{
    return waddnstr(stdscr, str, -1);
}

int mvaddstr(int y, int x, const char *str)
{
    return mvwaddnstr(stdscr,y,x,str,-1);
}

/* add string of single-byte characters and renditions */

int waddchnstr(WINDOW *win, const chtype *chstr, int n)
{
    if (win == NULL || chstr == NULL || n == 0)
        return ERR;
    /* TODO */
    return ERR;
}

int mvwaddchnstr(WINDOW *win, int y, int x, const chtype *chstr,
int n)
{
    if (wmove(win, y, x) == ERR)
        return ERR;

    return waddchnstr(win, chstr, n);
}

int mvwaddchstr(WINDOW *win, int y, int x, const chtype *chstr)
{
    if (wmove(win, y, x) == ERR)
        return ERR;

    return waddchnstr(win, chstr, -1);
}

int waddchstr(WINDOW *win, const chtype *chstr)
{
    return waddchnstr(win, chstr, -1);
}

int addchstr(const chtype *chstr)
{
    return waddchnstr(stdscr, chstr, -1);
}

/* attribute */

int wattroff(WINDOW *win, int attrs)
{
    win->attr &= ~attrs;

    return OK;
}

int wattr_off(WINDOW *win, attr_t attrs __attribute((unused)), void *opts __attribute((unused)))
{
    if (win == NULL)
        return ERR;

    return OK;
}

int wattr_on(WINDOW *win, attr_t attrs __attribute((unused)), void *opts __attribute((unused)))
{
    if (win == NULL)
        return ERR;

    return OK;
}

int wattron(WINDOW *win, int attrs)
{
    if (win == NULL)
        return ERR;

    win->attr |= attrs;

    return OK;
}

int wattrset(WINDOW *win, int attrs)
{
    if (win == NULL)
        return ERR;

    win->attr = attrs;

    return OK;
}

int attroff(int attrs)
{
    return wattroff(stdscr, attrs);
}

int attron(int attrs)
{
    return wattron(stdscr, attrs);
}

int attrset(int attrs)
{
    return wattrset(stdscr, attrs);
}

int wclrtoeol(WINDOW *win)
{
    for (int i = win->x; i < win->cols; i++)
        win->line_data[win->y].line[i] = ' ';

    win->line_data[win->y].touched = true;

    return OK;
}

int clrtoeol(void)
{
    return wclrtoeol(stdscr);
}

int mvwprintw(WINDOW *win, int y, int x, const char *fmt, ...)
{
    if (wmove(win, y, x) == ERR)
        return ERR;

    char buf[BUFSIZ];

    va_list ap;
    va_start (ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end (ap);

    return waddstr(win, buf);
}

int mvprintw(int y, int x, const char *fmt, ...)
{
    if (wmove(stdscr, y, x) == ERR)
        return ERR;

    char buf[BUFSIZ];

    va_list ap;
    va_start (ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end (ap);

    return waddstr(stdscr, buf);
}

int wprintw(WINDOW *win, const char *fmt, ...)
{
    char buf[BUFSIZ];

    va_list ap;
    va_start (ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end (ap);

    return waddstr(win, buf);
}

int printw(const char *fmt, ...)
{
    char buf[BUFSIZ];

    va_list ap;
    va_start (ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end (ap);

    return waddstr(stdscr, buf);
}

int del_curterm(TERMINAL *oterm)
{
    if (oterm == NULL)
        return ERR;

    extern void _fc_free_terminfo(struct terminfo *);

    if (oterm == cur_term)
        cur_term = NULL;

    if (oterm->terminfo) {
        _fc_free_terminfo(oterm->terminfo);
        oterm->terminfo = NULL;
    }
    free(oterm);

    return OK;
}

int wgetch(WINDOW *win)
{
    int ret = 0;

    if (win == NULL)
        return ERR;

    if (_getch(win->scr->term, &ret) == -1)
        return ERR;

    //fprintf(stderr, "wgetch: returning %d\n", ret);

    return ret;
}

int getch()
{
    return wgetch(stdscr);
}

int halfdelay(int tenths)
{
    struct termios tios;

    if (cur_term == NULL)
        return ERR;

    if (tcgetattr(cur_term->fd, &tios) == -1)
        return ERR;

    tios.c_cc[VTIME] = tenths;

    if (tcsetattr(cur_term->fd, 0, &tios) == -1)
        return ERR;

    return OK;
}

int baudrate(void)
{
    struct termios tio;

    if (cur_term == NULL)
        return ERR;

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return ERR;

    return (int) cfgetospeed(&tio);
}

char *termname(void)
{
    if (cur_term == NULL)
        return NULL;

    return ((struct terminfo *)cur_term->terminfo)->name;
}

/* The nl() function enables a mode in which <carriage-return> is translated to <newline> on input. */
int nl(void)
{
    struct termios tio;

    if (stdscr == NULL)
        return ERR;

    if (tcgetattr(stdscr->scr->_infd, &tio) == -1)
        return ERR;

    tio.c_iflag |= ICRNL;

    if (tcsetattr(stdscr->scr->_infd, TCSANOW, &tio) == -1)
        return ERR;

    stdscr->nl = TRUE;

    return OK;
}

int nonl(void)
{
    struct termios tio;

    if (cur_term == NULL)
        return ERR;

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return ERR;

    tio.c_iflag &= ~ICRNL;

    if (tcsetattr(cur_term->fd, 0, &tio) == -1)
        return ERR;

    stdscr->nl = FALSE;

    return OK;
}

int meta(WINDOW *win, bool bf)
{
    struct termios tio;
    char *cap;

    if (cur_term == NULL)
        return ERR;

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return ERR;

    if (bf == TRUE) {
        if ((cap = tigetstr("smm")) != NULL)
            tputs(cap, 1, _putchar_cur_term);
        tio.c_cflag &= ~CSIZE;
        tio.c_cflag |= CS8;
    } else if (bf == FALSE) {
        if ((cap = tigetstr("rmm")) != NULL)
            tputs(cap, 1, _putchar_cur_term);
        tio.c_cflag &= ~CSIZE;
        tio.c_cflag |= CS7;
    } else {
        warnx("meta: unknown bool value");
        return ERR;
    }

    if (tcsetattr(cur_term->fd, 0, &tio) == -1) {
        warn("meta: tcsetattr");
        return ERR;
    }

    win->meta = bf;

    return OK;
}

int raw(void)
{
    struct termios tio;

    if (stdscr == NULL)
        return ERR;

    if (tcgetattr(stdscr->scr->_infd, &tio) == -1)
        return ERR;

    tio.c_lflag &= ~(ICANON|ECHO|ISIG);
    tio.c_iflag &= ~(IXON);
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 1;

    /*  The ISIG and IXON flags are cleared upon entering this mode. */

    if (tcsetattr(stdscr->scr->_infd, TCSANOW, &tio) == -1)
        return ERR;

    return OK;
}


int cbreak(void)
{
    struct termios tio;

    if (stdscr == NULL)
        return ERR;

    if (tcgetattr(stdscr->scr->_infd, &tio) == -1)
        return ERR;

    /* per X/Open: 
     * This mode achieves the same
     * effect as non-canonical-mode, Case B input processing (with
     * MIN set to 1 and ICRNL cleared) 
     */

    tio.c_lflag &= ~(ICANON|ECHO);
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 1;

    if (tcsetattr(stdscr->scr->_infd, TCSANOW, &tio) == -1)
        return ERR;

    return OK;

}

int nocbreak(void)
{
    struct termios tio;

    if (cur_term == NULL)
        return ERR;

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return ERR;

    tio.c_lflag |= (ICANON|IEXTEN);

    if (tcsetattr(cur_term->fd, 0, &tio) == -1)
        return ERR;

    return 0;

}

char erasechar(void)
{
    struct termios tio;

    if (cur_term == NULL)
        return ERR;

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return 0;

    return tio.c_cc[VERASE];
}

int echo(void)
{
    struct termios tio;

    if (cur_term == NULL)
        return ERR;

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return ERR;

    tio.c_lflag |= ECHO;

    if (tcsetattr(cur_term->fd, 0, &tio) == -1)
        return ERR;

    return 0;
}

int noecho(void)
{
    struct termios tio;

    if (cur_term == NULL)
        return ERR;

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return ERR;

    tio.c_lflag &= ~ECHO;

    if (tcsetattr(cur_term->fd, 0, &tio) == -1)
        return ERR;

    return 0;
}

char killchar(void)
{
    struct termios tio;

    if (cur_term == NULL)
        return ERR;

    if (tcgetattr(cur_term->fd, &tio) == -1)
        return 0;

    return tio.c_cc[VKILL];
}
/* vim: set expandtab ts=4 sw=4: */

/* End of public library routines */

static int calc_base(const char **ptr)
{
    int base = 0;

    if (!**ptr)
        return 0;

    if (**ptr == '0' && isdigit(*(*ptr)+1)) {
        base = 8;
        (*ptr)++;
    } else if (**ptr == '0' && tolower(*(*ptr)+1) == 'x') {
        base = 16;
        (*ptr) += 2;
    } else
        base = 10;

    return base;
}

#define SBRK_GROW_SIZE  (1<<21)

static void init_mem()
{
    const size_t len = SBRK_GROW_SIZE * 4;

    if ( (tmp_first = first = last = sbrk(len)) == NULL ) {
        printf("init_mem: unable to sbrk(%lu)\n", len);
        _exit(2);
    }

    first->next = NULL;
    first->prev = NULL;
    first->start = first;
    first->end = ((char *)first->start) + len;
    first->flags = MF_FREE;
    first->len = len;
    first->magic = MEM_MAGIC;
}

static void mem_compress()
{
    struct mem_alloc *buf, *next;

    for (buf = tmp_first; buf;)
    {
#ifdef VALGRIND
        VALGRIND_MAKE_MEM_DEFINED(buf, sizeof(struct mem_alloc));
#endif
        if (!(buf->flags & MF_FREE))
            goto next;

        next = buf->next;
#ifdef VALGRIND
        if (next)
            VALGRIND_MAKE_MEM_DEFINED(next, sizeof(struct mem_alloc));
#endif

        if (next && (next->flags & MF_FREE)) {

            if (next == tmp_first)
                tmp_first = buf;

            buf->len  = (buf->len + next->len);
            buf->end  = next->end;
            buf->next = next->next;

            if (next->next) {
#ifdef VALGRIND
            VALGRIND_MAKE_MEM_DEFINED(next->next, sizeof(struct mem_alloc));
#endif
                next->next->prev = buf;
#ifdef VALGRIND
            VALGRIND_MAKE_MEM_NOACCESS(next->next, sizeof(struct mem_alloc));
#endif
            }

            memset(next, 0, sizeof(struct mem_alloc));

            if (buf->next == NULL)
                last = buf;
#ifdef VALGRIND
            VALGRIND_MAKE_MEM_NOACCESS(next, sizeof(struct mem_alloc));
#endif
        }
next:
        buf = buf->next;
#ifdef VALGRIND
        VALGRIND_MAKE_MEM_NOACCESS(buf, sizeof(struct mem_alloc));
#endif
    }
}

static void free_alloc(struct mem_alloc *tmp)
{
    if (tmp) {
        tmp->flags |= MF_FREE;
        if (tmp < tmp_first)
            tmp_first = tmp;
    }
}

static struct mem_alloc *grow_pool(void)
{
    mem_compress();

    const size_t len = SBRK_GROW_SIZE;
    struct mem_alloc *new_last;
    struct mem_alloc *old_last = last;

    if ((new_last = sbrk(len)) == NULL)
        exit(3);
#ifdef VALGRIND
    VALGRIND_CREATE_MEMPOOL(new_last, 0, 0);
#endif

    if ((last->flags & MF_FREE)) {
        last->end = (char *)last->end + len;
        last->len += len;
    } else {
        old_last->next = new_last;

        new_last->prev = old_last;
        new_last->next = NULL;
        new_last->len = len;
        new_last->magic = MEM_MAGIC;
        new_last->flags = MF_FREE;
        new_last->start = new_last;

        new_last->end = (char *)new_last->start + len;

        last = new_last;
    }
    return last;
}

static void check_mem(void)
{
    if (first == NULL) {
        __builtin_printf( "check_mem: first is null\n");
        _exit(1);
    }
    if (last == NULL) {
        __builtin_printf( "last is null\n");
        _exit(1);
    }

    struct mem_alloc *tmp, *prev;

    for (tmp = first, prev = NULL; tmp;)
    {
#ifdef VALGRIND
        VALGRIND_MAKE_MEM_DEFINED(tmp, sizeof(struct mem_alloc));
#endif
        if (tmp < first || tmp > last) {
            __builtin_printf( "check_mem: %p out of range [prev=%p]\n", (void *)tmp, (void *)prev);
            _exit(1);
        }
        if (tmp->magic != MEM_MAGIC) {
            __builtin_printf( "check_mem: %p has bad magic [prev=%p]\n", (void *)tmp, (void *)prev);
            _exit(1);
        }
        if (tmp->next == tmp || tmp->prev == tmp) {
            __builtin_printf( "check_mem: %p circular {<%p,%p>}\n", (void *)tmp, (void *)tmp->prev, (void *)tmp->next);
            _exit(1);
        }

        prev = tmp;
        tmp = tmp->next;
#ifdef VALGRIND
        VALGRIND_MAKE_MEM_NOACCESS(tmp, sizeof(struct mem_alloc));
#endif
    }
}

[[gnu::hot]] inline static struct mem_alloc *find_free(size_t size)
{
    struct mem_alloc *tmp;
    const size_t seek = size + (sizeof(struct mem_alloc) * 2);

    for (tmp = tmp_first; tmp;)
    {
#ifdef VALGRIND
        struct mem_alloc *prev;
        VALGRIND_MAKE_MEM_DEFINED(tmp, sizeof(struct mem_alloc));
#endif
        if (tmp < first || tmp > last)
            _exit(200);
        if (tmp->next == tmp)
            _exit(201);
        if (tmp->magic != MEM_MAGIC)
            _exit(202);

        if ((tmp->flags & MF_FREE) && tmp->len >= seek) {
#ifdef VALGRIND
        VALGRIND_MAKE_MEM_NOACCESS(tmp, sizeof(struct mem_alloc));
#endif
            return tmp;
        }

#ifdef VALGRIND
        prev = tmp;
        tmp = tmp->next;
        VALGRIND_MAKE_MEM_DEFINED(prev, sizeof(struct mem_alloc));
#else
        tmp = tmp->next;
#endif
    }

    return grow_pool();
}

[[gnu::hot, gnu::nonnull]] inline static struct mem_alloc *split_alloc(struct mem_alloc *old, size_t size)
{
    size_t seek;
    struct mem_alloc *rem = NULL;

    seek = size + (sizeof(struct mem_alloc) * 2);
#ifdef VALGRIND
    VALGRIND_MAKE_MEM_DEFINED(old, sizeof(struct mem_alloc));
#endif

    if (!size)
        goto fail;
    if (!(old->flags & MF_FREE) || !old->len)
        goto fail;
    if (old->len < seek)
        goto fail;

    if (!old->next && last != old)
        exit(42);
    if (!old->prev && first != old)
        exit(43);
    if (old != old->start)
        exit(40);
    if ((char *)old->start + old->len != old->end)
        exit(41);
    if (old->magic != MEM_MAGIC)
        exit(44);

    rem = (struct mem_alloc *)(((char *)old->start) + sizeof(struct mem_alloc) + size);
    if (rem == NULL || (void *)rem < _data_start || (void *)rem > _data_end) {
        fputs("mem_alloc: corruption\n", stderr);
        abort();
    }
#ifdef VALGRIND
    VALGRIND_MAKE_MEM_DEFINED(rem, sizeof(struct mem_alloc));
#endif
    rem->magic = MEM_MAGIC;

    if (old->next) {
#ifdef VALGRIND
    VALGRIND_MAKE_MEM_DEFINED(old->next, sizeof(struct mem_alloc));
#endif
        old->next->prev = rem;
#ifdef VALGRIND
    VALGRIND_MAKE_MEM_NOACCESS(old->next, sizeof(struct mem_alloc));
#endif
    }

    rem->prev = old;
    rem->next = old->next;
    old->next = rem;

    rem->flags |= MF_FREE;
    old->flags = ~(MF_FREE);

    rem->len = old->len - (sizeof(struct mem_alloc) + size);
    rem->start = rem;
    rem->end = (char *)rem->start + rem->len;

    old->len = sizeof(struct mem_alloc) + size;
    old->end = ((char *)old->start) + old->len;

    if (old->prev == NULL)
        first = old;
    if (rem->next == NULL)
        last = rem;

#ifdef VALGRIND
        VALGRIND_MAKE_MEM_NOACCESS(rem, sizeof(struct mem_alloc));
        VALGRIND_MAKE_MEM_NOACCESS(old, sizeof(struct mem_alloc));
#endif
    return old;
fail:
#ifdef VALGRIND
        VALGRIND_MAKE_MEM_NOACCESS(rem, sizeof(struct mem_alloc));
        VALGRIND_MAKE_MEM_NOACCESS(old, sizeof(struct mem_alloc));
#endif
    return NULL;
}

[[gnu::hot]] inline static struct mem_alloc *alloc_mem(size_t req_size)
{
    static uint64_t cnt = 0;
    struct mem_alloc *ret;
    size_t size;

    if (cnt++ >= 512) {
        mem_compress();
        cnt = 0;
    }

    if (req_size == 0)
        return NULL;

    if (req_size < 64) {
        size = 64;
    } else {
        size = req_size;
    }

    if ((ret = find_free(size)) == NULL)
        return NULL;

    if ((split_alloc(ret, size)) == NULL)
        return NULL;

#ifdef VALGRIND
    VALGRIND_MAKE_MEM_DEFINED(ret, sizeof(struct mem_alloc));
#endif
    tmp_first = ret->next;
#ifdef VALGRIND
    VALGRIND_MAKE_MEM_DEFINED(tmp_first, sizeof(struct mem_alloc));
#endif

    while (tmp_first && ((tmp_first->flags & MF_FREE) == 0)) {
        struct mem_alloc *next = tmp_first->next;
#ifdef VALGRIND
        VALGRIND_MAKE_MEM_DEFINED(next, sizeof(struct mem_alloc));
        VALGRIND_MAKE_MEM_NOACCESS(tmp_first, sizeof(struct mem_alloc));
#endif
        tmp_first = next;
    }

    return ret;
}

/* this explodes if not inline */
inline static struct __pthread *__pthread_self(void)
{
    struct __pthread *ret = NULL;
    __asm__("movq %%fs:0, %0":"=r" (ret));
    return ret;
}

[[gnu::nonnull, gnu::access(write_only, 1)]] static char *fgets_delim(char *s, const int size,
        FILE *const restrict stream, const int delim)
{
    if (feof(stream) || ferror(stream))
        return NULL;

    int len = 0;
    char in;

    while (len < (size - 1))
    {
        if ((in = fgetc(stream)) == EOF)
            break;

        if ((s[len++] = in) == delim) {
            s[len] = '\0';
            break;
        }
    }
    if (len == 0 || ferror(stream) )
        return NULL;
    return s;
}


/* Special functions */

int *__errno_location(void)
{
    return &__pthread_self()->errnum;
}

    __attribute__((constructor))
void init(void)
{
}

    __attribute__((destructor))
void fini(void)
{
}

typedef struct {
    long a_type;
    union {
        long a_val;
        void *a_ptr;
        void (*a_fnc)();
    } a_un;
} __attribute__((packed)) auxv_t;

/*
   AT_NULL 0 ignored
   AT_IGNORE 1 ignored
   AT_EXECFD 2 a_val
   AT_PHDR 3 a_ptr
   AT_PHENT 4 a_val
   AT_PHNUM 5 a_val
   AT_PAGESZ 6 a_val
   AT_BASE 7 a_ptr
   AT_FLAGS 8 a_val
   AT_ENTRY 9 a_ptr
   AT_NOTELF 10 a_val
   AT_UID 11 a_val
   AT_EUID 12 a_val
   AT_GID 13 a_val
   AT_EGID 14 a_val
   */
#define AT_NULL 0
#define AT_IGNORE 1
#define AT_EXECFD 2
#define AT_PHDR 3
#define AT_PHENT 4
#define AT_PHNUM 5
#define AT_PAGESZ 6
#define AT_BASE 7
#define AT_FLAGS 8
#define AT_ENTRY 9
#define AT_NOTELF 10
#define AT_UID 11
#define AT_EUID 12
#define AT_GID 13
#define AT_EGID 14

/* Linux extensions? */
#define AT_PLATFORM 15
#define AT_HWCAP 16
#define AT_CLKTCK 17
#define AT_SECURE 23
#define AT_BASE_PLATFORM 24
#define AT_RANDOM 25
#define AT_HWCAP2 26
#define AT_EXECFN 31
#define AT_SYSINFO_EHDR 33

[[gnu::nonnull, maybe_unused]] static void debug_aux(const auxv_t *aux)
{
    switch (aux->a_type)
    {
        case AT_IGNORE:
            printf("AT_IGNORE\n");
            break;
        case AT_EXECFD:
            printf("AT_EXECFD: %ld\n", aux->a_un.a_val);
            break;
        case AT_PAGESZ:
            printf("AT_PAGESZ: %ld\n", aux->a_un.a_val);
            break;
        case AT_PHDR:
            printf("AT_PHDR:   0x%p\n", aux->a_un.a_ptr);
            break;
        case AT_PHENT:
            printf("AT_PHENT:  %ld\n", aux->a_un.a_val);
            break;
        case AT_PHNUM:
            printf("AT_PHNUM:  %ld\n", aux->a_un.a_val);
            break;
        case AT_BASE:
            printf("AT_BASE:   0x%p\n", aux->a_un.a_ptr);
            break;
        case AT_FLAGS:
            printf("AT_FLAGS:  %ld\n", aux->a_un.a_val);
            break;
        case AT_ENTRY:
            printf("AT_ENTRY:  0x%p\n", aux->a_un.a_ptr);
            break;
        case AT_NOTELF:
            printf("AT_NOTELF: %ld\n", aux->a_un.a_val);
            break;
        case AT_UID:
            printf("AT_UID:    %ld\n", aux->a_un.a_val);
            break;
        case AT_EUID:
            printf("AT_EUID:   %ld\n", aux->a_un.a_val);
            break;
        case AT_GID:
            printf("AT_GID:    %ld\n", aux->a_un.a_val);
            break;
        case AT_EGID:
            printf("AT_EGID:   %ld\n", aux->a_un.a_val);
            break;

            /* Linux specific outside of ABI specification */
        case AT_SECURE:
            printf("AT_SECURE: %ld\n", aux->a_un.a_val);
            break;
        case AT_PLATFORM:
            printf("AT_PLTFRM: %s\n", (char *)aux->a_un.a_ptr);
            break;
        case AT_EXECFN:
            printf("AT_EXECFN: %s\n", (char *)aux->a_un.a_ptr);
            break;
        case AT_HWCAP:
            printf("AT_HWCAP:  %lu\n", aux->a_un.a_val);
            break;
        case AT_HWCAP2:
            printf("AT_HWCAP2: %lu\n", aux->a_un.a_val);
            break;
        case AT_CLKTCK:
            printf("AT_CLKTCK: %ld\n", aux->a_un.a_val);
            break;
        case AT_RANDOM:
            printf("AT_RANDOM: 0x%p\n", aux->a_un.a_ptr);
            break;

            /* Linux x86_64 specific outside of ABI specification */
        case AT_SYSINFO_EHDR:
            printf("AT_SYSINF: 0x%p\n", aux->a_un.a_ptr);
            break;

        default:
            printf("Unknown:   %ld.0x%lx\n", aux->a_type, aux->a_un.a_val);
            break;
    }
}

[[gnu::noreturn]] void __libc_start_main(int ac, char *av[], char **envp, auxv_t *aux)
{
    struct __pthread tmp = {
        .errnum = 0,
        .parent_tid = 0,
        .self = &tmp
    };

    syscall(__NR_arch_prctl, ARCH_SET_FS, (uint64_t)&tmp);

    if (__pthread_self() != &tmp)
        _exit(1);

    _data_start = _data_end = (void *)syscall(__NR_brk, 0);
    if (_data_end == (void *)-1UL)
        _Exit(EXIT_FAILURE);

    global_atexit_list = NULL;

    first = NULL;
    last  = NULL;

    /* FailOS doesn't seem to initialise .data properly yet */

    errno = 0;
    tmp_first = NULL;
    optoff = 1; // init isn't working in FailOS ?
    optarg = NULL;
    opterr = 1;
    optind = 1;
    optopt = 0;
    utx = NULL;
    utx_rw = 0;
    rand_seed = 1;
    random_state_ptr = 0;
    lastss = NULL;
    ss = NULL;
    ss_invert = false;
    pw = NULL;
    netdb = NULL;
    netdb_size = -1;
    netdb_keepopen = 1;
    netdb_current_record = 0;
    unix_socket = -1;
    timezone = 0;
    daylight = 0;
    tzname[0] = "GMT";
    tzname[1] = "GMT";
    sl_options = 0;
    sl_facility = LOG_USER;
    sl_mask = 0;
    sl_ident = NULL;
    strtok_state = NULL;
    resolv = NULL;
    which_ns = 0;

    memset(random_state, 0, sizeof(random_state));
    memset(&pass, 0, sizeof(struct passwd));
    memset(&grpret, 0, sizeof(struct group));
    memset(&mntent_ret, 0, sizeof(struct mntent));
    memset(ttyname_string, 0, sizeof(ttyname_string));
    memset(&utmpx_tmp, 0, sizeof(struct utmpx));
    memset(&localtime_tmp, 0, sizeof(struct tm));
    memset(&gmtime_tmp, 0, sizeof(struct tm));
    memset(asctime_tmp, 0, sizeof(asctime_tmp));
    gr = NULL;


    init_mem();

    int i = 0;
    while (aux[i].a_type)
    {
        /* TODO process AUX here */

        i++;
    }

    struct __pthread *npt = malloc(sizeof(struct __pthread));

    if (npt == NULL)
        _Exit(EXIT_FAILURE);

    npt->parent_tid = 0;
    npt->self = npt;
    npt->errnum = 0;

    arch_prctl(ARCH_SET_FS, (uint64_t)npt);

    npt->my_tid = gettid();
    check_mem();

    size_t env_count;

    for (env_count = 0; envp[env_count] != NULL; )
        env_count++;

    if ((environ = calloc(1, sizeof(char *) * (env_count + 1))) == NULL)
        err(EXIT_FAILURE, "__libc_start_main: calloc");

    char *new_env;
    for (size_t i = 0; i < env_count; i++) {

        if (envp[i] == NULL)
            continue;

        if ((new_env = strdup(envp[i])) == NULL) {
            err(EXIT_FAILURE, "__libc_start_main: strdup");
        }

        environ[i] = new_env;
    }

    srand(rand()+time(NULL));
    srandom(rand()+time(NULL));
    srand(random());
    srandom(rand());
    
#ifdef VALGRIND
    if (RUNNING_ON_VALGRIND)
        printf("*** running in valgrind ***\n");
#endif

    exit(main(ac, av, environ));
}

/* vim: set expandtab ts=4 sw=4: */
