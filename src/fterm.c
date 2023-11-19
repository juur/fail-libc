#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <err.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/ioctl.h>

static const struct {
    const char *const short_name;
    const char type;
} term_caps[] = {
{"bw",'b'},
{"am",'b'},
{"bce",'b'},
{"ccc",'b'},
{"xhp",'b'},
{"xhpa",'b'},
{"cpix",'b'},
{"crxm",'b'},
{"xt",'b'},
{"xenl",'b'},
{"eo",'b'},
{"gn",'b'},
{"hc",'b'},
{"chts",'b'},
{"km",'b'},
{"daisy",'b'},
{"hs",'b'},
{"hls",'b'},
{"in",'b'},
{"lpix",'b'},
{"da",'b'},
{"db",'b'},
{"mir",'b'},
{"msgr",'b'},
{"nxon",'b'},
{"xsb",'b'},
{"npc",'b'},
{"ndscr",'b'},
{"nrrmc",'b'},
{"os",'b'},
{"mc5i",'b'},
{"xvpa",'b'},
{"sam",'b'},
{"eslok",'b'},
{"hz",'b'},
{"ul",'b'},
{"xon",'b'},

{"bitwin",'#'},
{"bitype",'#'},
{"bufsz",'#'},
{"btns",'#'},
{"cols",'#'},
{"colors",'#'},
{"spinh",'#'},
{"spinv",'#'},
{"it",'#'},
{"lh",'#'},
{"lw",'#'},
{"lines",'#'},
{"lm",'#'},
{"ma",'#'},
{"xmc",'#'},
{"colors",'#'},
{"maddr",'#'},
{"mjump",'#'},
{"pairs",'#'},
{"wnum",'#'},
{"mcs",'#'},
{"mls",'#'},
{"ncv",'#'},
{"nlab",'#'},
{"npins",'#'},
{"orc",'#'},
{"orl",'#'},
{"orhi",'#'},
{"orvi",'#'},
{"pb",'#'},
{"cps",'#'},
{"vt",'#'},
{"widcs",'#'},
{"wsl",'#'},

{"acsc",'s'},
{"scesa",'s'},
{"cbt",'s'},
{"bel",'s'},
{"bicr",'s'},
{"binel",'s'},
{"birep",'s'},
{"cr",'s'},
{"cpi",'s'},
{"lpi",'s'},
{"chr",'s'},
{"cvr",'s'},
{"csr",'s'},
{"rmp",'s'},
{"csnm",'s'},
{"tbc",'s'},
{"mgc",'s'},
{"clear",'s'},
{"el1",'s'},
{"el",'s'},
{"ed",'s'},
{"csin",'s'},
{"colornm",'s'},
{"hpa",'s'},
{"cmdch",'s'},
{"cwin",'s'},
{"cup",'s'},
{"cud1",'s'},
{"home",'s'},
{"civis",'s'},
{"cub1",'s'},
{"mrcup",'s'},
{"cnorm",'s'},
{"cuf1",'s'},
{"ll",'s'},
{"cuu1",'s'},
{"cvvis",'s'},
{"defbi",'s'},
{"defc",'s'},
{"dch1",'s'},
{"dl1",'s'},
{"devt",'s'},
{"dial",'s'},
{"dsl",'s'},
{"dclk",'s'},
{"dispc",'s'},
{"hd",'s'},
{"enacs",'s'},
{"endbi",'s'},
{"smacs",'s'},
{"smam",'s'},
{"blink",'s'},
{"bold",'s'},
{"smcup",'s'},
{"smdc",'s'},
{"dim",'s'},
{"swidm",'s'},
{"sdrfq",'s'},
{"ehhlm",'s'},
{"smir",'s'},
{"sitm",'s'},
{"elhlm",'s'},
{"slm",'s'},
{"elohlm",'s'},
{"smicm",'s'},
{"snlq",'s'},
{"snrmq",'s'},
{"smpch",'s'},
{"prot",'s'},
{"rev",'s'},
{"erhlm",'s'},
{"smsc",'s'},
{"invis",'s'},
{"sshm",'s'},
{"smso",'s'},
{"ssubm",'s'},
{"ssupm",'s'},
{"ethlm",'s'},
{"smul",'s'},
{"sum",'s'},
{"evhlm",'s'},
{"smxon",'s'},
{"ech",'s'},
{"rmacs",'s'},
{"rmam",'s'},
{"sgr0",'s'},
{"rmcup",'s'},
{"rmdc",'s'},
{"rwidm",'s'},
{"rmir",'s'},
{"ritm",'s'},
{"rlm",'s'},
{"rmicm",'s'},
{"rmpch",'s'},
{"rmsc",'s'},
{"rshm",'s'},
{"rmso",'s'},
{"rsubm",'s'},
{"rsupm",'s'},
{"rmul",'s'},
{"rum",'s'},
{"rmxon",'s'},
{"pause",'s'},
{"hook",'s'},
{"flash",'s'},
{"ff",'s'},
{"fsl",'s'},
{"getm",'s'},
{"wingo",'s'},
{"hup",'s'},
{"is1",'s'},
{"is2",'s'},
{"is3",'s'},
{"if",'s'},
{"iprog",'s'},
{"initc",'s'},
{"initp",'s'},
{"ich1",'s'},
{"il1",'s'},
{"ip",'s'},
{"ka1",'s'},
{"ka3",'s'},
{"kb2",'s'},
{"kbs",'s'},
{"kbeg",'s'},
{"kcbt",'s'},
{"kc1",'s'},
{"kc3",'s'},
{"kcan",'s'},
{"ktbc",'s'},
{"kclr",'s'},
{"kclo",'s'},
{"kcmd",'s'},
{"kcpy",'s'},
{"kcrt",'s'},
{"kctab",'s'},
{"kdch1",'s'},
{"kdl1",'s'},
{"kcud1",'s'},
{"krmir",'s'},
{"kend",'s'},
{"kent",'s'},
{"kel",'s'},
{"ked",'s'},
{"kext",'s'},
    { "kf1"   , 's'} , 
    { "kf10"  , 's'} ,
    { "kf11"  , 's'} ,
    { "kf12"  , 's'} ,
    { "kf13"  , 's'} ,
    { "kf14"  , 's'} ,
    { "kf15"  , 's'} ,
    { "kf16"  , 's'} ,
    { "kf17"  , 's'} ,
    { "kf18"  , 's'} ,
    { "kf19"  , 's'} ,
    { "kf2"   , 's'} , 
    { "kf20"  , 's'} ,
    { "kf21"  , 's'} ,
    { "kf22"  , 's'} ,
    { "kf23"  , 's'} ,
    { "kf24"  , 's'} ,
    { "kf25"  , 's'} ,
    { "kf26"  , 's'} ,
    { "kf27"  , 's'} ,
    { "kf28"  , 's'} ,
    { "kf29"  , 's'} ,
    { "kf30"  , 's'} ,
    { "kf31"  , 's'} ,
    { "kf32"  , 's'} ,
    { "kf33"  , 's'} ,
    { "kf34"  , 's'} ,
    { "kf35"  , 's'} ,
    { "kf36"  , 's'} ,
    { "kf37"  , 's'} ,
    { "kf38"  , 's'} ,
    { "kf39"  , 's'} ,
    { "kf40"  , 's'} ,
    { "kf41"  , 's'} ,
    { "kf42"  , 's'} ,
    { "kf43"  , 's'} ,
    { "kf44"  , 's'} ,
    { "kf45"  , 's'} ,
    { "kf46"  , 's'} ,
    { "kf47"  , 's'} ,
    { "kf48"  , 's'} ,
    { "kf49"  , 's'} ,
    { "kf50"  , 's'} ,
    { "kf51"  , 's'} ,
    { "kf52"  , 's'} ,
    { "kf53"  , 's'} ,
    { "kf54"  , 's'} ,
    { "kf55"  , 's'} ,
    { "kf56"  , 's'} ,
    { "kf57"  , 's'} ,
    { "kf58"  , 's'} ,
    { "kf59"  , 's'} ,
    { "kf60"  , 's'} ,
    { "kf61"  , 's'} ,
    { "kf62"  , 's'} ,
    { "kf63"  , 's'} ,
    { "kf64"  , 's'} ,
    { "kf65"  , 's'} ,
    { "kf3"   , 's'} , 
    { "kf4"  , 's'} ,
    { "kf5"   , 's'} , 
    { "kf6"   , 's'} , 
    { "kf7"   , 's'} , 
    { "kf8"   , 's'} , 
    { "kf9"   , 's'} , 

{"kfnd",'s'},
{"khlp",'s'},
{"khome",'s'},
{"kich1",'s'},
{"kil1",'s'},
{"kcub1",'s'},
{"kll",'s'},
{"kmrk",'s'},
{"kmsg",'s'},
{"kmous",'s'},
{"kmov",'s'},
{"knxt",'s'},
{"knp",'s'},
{"kopn",'s'},
{"kopt",'s'},
{"kpp",'s'},
{"kprv",'s'},
{"kprt",'s'},
{"krdo",'s'},
{"kref",'s'},
{"krfr",'s'},
{"krpl",'s'},
{"krst",'s'},
{"kres",'s'},
{"kcuf1",'s'},
{"ksav",'s'},
{"kBEG",'s'},
{"kCAN",'s'},
{"kCMD",'s'},
{"kCPY",'s'},
{"kCRT",'s'},
{"kDC",'s'},
{"kDL",'s'},
{"kslt",'s'},
{"kEND",'s'},
{"kEOL",'s'},
{"kEXT",'s'},
{"kind",'s'},
{"kFND",'s'},
{"kHLP",'s'},
{"kHOM",'s'},
{"kIC",'s'},
{"kLFT",'s'},
{"kMSG",'s'},
{"kMOV",'s'},
{"kNXT",'s'},
{"kOPT",'s'},
{"kPRV",'s'},
{"kPRT",'s'},
{"kri",'s'},
{"kRDO",'s'},
{"kRPL",'s'},
{"kRIT",'s'},
{"kRES",'s'},
{"kSAV",'s'},
{"kSPD",'s'},
{"khts",'s'},
{"kUND",'s'},
{"kspd",'s'},
{"kund",'s'},
{"kcuu1",'s'},
{"rmkx",'s'},
{"smkx",'s'},
{"lf0",'s'},
{"lf1",'s'},
{"lf2",'s'},
{"lf3",'s'},
{"lf4",'s'},
{"lf5",'s'},
{"lf6",'s'},
{"lf7",'s'},
{"lf8",'s'},
{"lf9",'s'},
{"lf10",'s'},
{"fln",'s'},
{"rmln",'s'},
{"smln",'s'},
{"rmm",'s'},
{"smm",'s'},
{"mhpa",'s'},
{"mcud1",'s'},
{"mcub1",'s'},
{"mcuf1",'s'},
{"mvpa",'s'},
{"mcuu1",'s'},
{"minfo",'s'},
{"nel",'s'},
{"porder",'s'},
{"oc",'s'},
{"op",'s'},
{"pad",'s'},
{"dch",'s'},
{"dl",'s'},
{"cud",'s'},
{"mcud",'s'},
{"ich",'s'},
{"indn",'s'},
{"il",'s'},
{"cub",'s'},
{"mcub",'s'},
{"cuf",'s'},
{"mcuf",'s'},
{"rin",'s'},
{"cuu",'s'},
{"mcuu",'s'},
{"pctrm",'s'},
{"pfkey",'s'},
{"pfloc",'s'},
{"pfxl",'s'},
{"pfx",'s'},
{"pln",'s'},
{"mc0",'s'},
{"mc5p",'s'},
{"mc4",'s'},
{"mc5",'s'},
{"pulse",'s'},
{"qdial",'s'},
{"rmclk",'s'},
{"rep",'s'},
{"rfi",'s'},
{"reqmp",'s'},
{"rs1",'s'},
{"rs2",'s'},
{"rs3",'s'},
{"rf",'s'},
{"rc",'s'},
{"vpa",'s'},
{"sc",'s'},
{"scesc",'s'},
{"ind",'s'},
{"ri",'s'},
{"scs",'s'},
{"s0ds",'s'},
{"s1ds",'s'},
{"s2ds",'s'},
{"s3ds",'s'},
{"sgr1",'s'},
{"setab",'s'},
{"setaf",'s'},
{"sgr",'s'},
{"setb",'s'},
{"smgb",'s'},
{"smgbp",'s'},
{"sclk",'s'},
{"setcolor",'s'},
{"scp",'s'},
{"setf",'s'},
{"smgl",'s'},
{"smglp",'s'},
{"smglr",'s'},
{"slines",'s'},
{"slength",'s'},
{"smgr",'s'},
{"smgrp",'s'},
{"hts",'s'},
{"smgtb",'s'},
{"smgt",'s'},
{"smgtp",'s'},
{"wind",'s'},
{"sbim",'s'},
{"scsd",'s'},
{"rbim",'s'},
{"rcsd",'s'},
{"subcs",'s'},
{"supcs",'s'},
{"ht",'s'},
{"docr",'s'},
{"tsl",'s'},
{"tone",'s'},
{"u0",'s'},
{"u1",'s'},
{"u2",'s'},
{"u3",'s'},
{"u4",'s'},
{"u5",'s'},
{"u6",'s'},
{"u7",'s'},
{"u8",'s'},
{"u9",'s'},
{"uc",'s'},
{"hu",'s'},
{"wait",'s'},
{"xoffc",'s'},
{"xonc",'s'},
{"zerom",'s'},
{"meml",'s'},
{"memu",'s'},
{NULL,0}
};


/*
 * constants
 */

//static const char *terminfo_location = "/usr/share/terminfo/";
static const char terminfo_location[] = "terminfo/";

/* 
 * private globals 
 */

static struct terminfo *termdb = NULL;
static char tiparm_ret[BUFSIZ];

/*
 * public globals
 */

TERMINAL *cur_term;

/*
 * private functions
 */

__attribute__(( nonnull ))
static int get_termcap_idx(const char *capname, char type)
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

__attribute__(( nonnull ))
static void free_terminfo(struct terminfo *term)
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

__attribute__(( nonnull(1) ))
static struct terminfo *parse_terminfo(const char *term_name, int *errret)
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

    //printf("terminfo_location=%p\n", terminfo_location);

    snprintf(buf, sizeof(buf), "%s%c/%s", terminfo_location, term_name[0], term_name);

    if ((tinfo = fopen(buf, "r")) == NULL) {
        if (errret)
            *errret = 0;
        else
            warn("Unable to open terminfo <%s>", buf);

        return NULL;
    }

    rc = fread(buf, sizeof(buf), 1, tinfo);

    if (rc == sizeof(buf) || ferror(tinfo))
        goto malformed;

    /* this section needs improving to handle more complex
     * white space & split line arrangements */

    while (*tok && isspace(*tok))
        tok++;

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
        free_terminfo(ret);

    ret = NULL;
    goto done;

}

    __attribute__(( nonnull(1) ))
static struct terminfo *load_terminfo(const char *name, int *errret)
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

#if 0
static void    hexdump(const char *tmp)
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
#endif

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

extern bool nc_use_env;

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

    fprintf(stderr, "setting LINES=%d COLS=%d\n", LINES, COLS);

    const struct {
        const char *key;
        const int   id;
    } keys[] = {
        { "kuu1", KEY_UP },
        { "kcud1", KEY_DOWN },
        { "kcub1", KEY_LEFT },
        { "kcuf1", KEY_RIGHT },
        { "kpp", KEY_PPAGE },
        { "knp", KEY_NPAGE },
        { "khome", KEY_HOME },
        { "kend", KEY_END },
        { NULL, -1 }
    };

    for (int i = 0; keys[i].key != NULL; i++)
    {
        char *tmp;

        if ((tmp = tigetstr(keys[i].key)) == NULL || tmp == (char *)-1)
            continue;

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
