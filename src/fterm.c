#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <err.h>
#include <stdarg.h>
#include <unistd.h>

/*
 * constants
 */

static const struct {
    const char *const short_name;
    const char type;
} term_caps[] = {

    { "acsc"  , 's'} , 
    { "am"    , 'b'} , 
    { "bce"   , 'b'} ,
    { "bel"   , 's'} , 
    { "blink" , 's'} ,
    { "bold"  , 's'} ,
    { "cbt"   , 's'} ,
    { "ccc"   , 'b'} ,
    { "civis" , 's'} ,
    { "clear" , 's'} , 
    { "cnorm" , 's'} ,
    { "colors"  , '#'} , 
    { "cols"  , '#'} , 
    { "cr"    , 's'} , 
    { "csr"   , 's'} ,
    { "cub"   , 's'} ,
    { "cub1"  , 's'} , 
    { "cud"   , 's'} ,
    { "cud1"  , 's'} , 
    { "cuf"   , 's'} ,
    { "cuf1"  , 's'} , 
    { "cup"   , 's'} , 
    { "cuu"   , 's'} ,
    { "cuu1"  , 's'} , 
    { "cvvis"  , 's'} , 
    { "dch"   , 's'} ,
    { "dch1"  , 's'} ,
    { "dim"   , 's'} ,
    { "dl"    , 's'} ,
    { "dl1"   , 's'} ,
    { "ech"  , 's'} ,
    { "ed"    , 's'} , 
    { "el"    , 's'} , 
    { "el1"   , 's'} ,
    { "enacs" , 's'} ,
    { "flash"  , 's'} ,
    { "home"  , 's'} , 
    { "hpa"   , 's'} ,
    { "ht"    , 's'} , 
    { "hts"   , 's'} ,
    { "ich"  , 's'} ,
    { "if"  , 's'} ,
    { "il"  , 's'} ,
    { "il1"   , 's'} ,
    { "ind"   , 's'} , 
    { "ind"   , 's'} , 
    { "indn"   , 's'} ,
    { "initc"   , 's'} ,
    { "invis"   , 's'} ,
    { "is2"  , 's'} ,
    { "it"    , '#'} , 
    { "ka1"   , 's'} , 
    { "ka3"   , 's'} , 
    { "kb2"   , 's'} , 
    { "kbs"   , 's'} , 
    { "kc1"   , 's'} , 
    { "kc3"   , 's'} , 
    { "kcbt" , 's'} , 
    { "kcub1" , 's'} , 
    { "kcud1" , 's'} , 
    { "kcuf1" , 's'} , 
    { "kcuu1" , 's'} , 
    { "kdch1"  , 's'} ,
    { "kend"  , 's'} ,
    { "kent"  , 's'} ,
    { "kf0"   , 's'} , 
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
    { "kfnd"  , 's'} ,
    { "khlp"  , 's'} ,
    { "khome"  , 's'} ,
    { "kich1"  , 's'} ,
    { "kil1"  , 's'} ,
    { "kind"  , 's'} ,
    { "kcub1"  , 's'} ,
    { "kll"  , 's'} ,
    { "kll"  , 's'} ,
    { "kDC"  , 's'} ,
    { "kEND"  , 's'} ,
    { "kHOM"  , 's'} ,
    { "kIC"  , 's'} ,
    { "kLFT"  , 's'} ,
    { "kNXT"  , 's'} ,
    { "kPRV"  , 's'} ,
    { "kRIT"  , 's'} ,
    { "km"   , 'b'} ,
    { "kmous"  , 's'} ,
    { "knp"  , 's'} ,
    { "kopn"  , 's'} ,
    { "kopt"  , 's'} ,
    { "kpp"  , 's'} ,
    { "kprv"  , 's'} ,
    { "kprt"  , 's'} ,
    { "krdo"  , 's'} ,
    { "kref"  , 's'} ,
    { "krfr"  , 's'} ,
    { "kri"  , 's'} ,
    { "krpl"  , 's'} ,
    { "krst"  , 's'} ,
    { "kcuf1"  , 's'} ,
    { "ksav"  , 's'} ,
    { "kslt"  , 's'} ,
    { "lf1"  , 's'} ,
    { "lf2"  , 's'} ,
    { "lf3"  , 's'} ,
    { "lf4"  , 's'} ,
    { "lines" , '#'} , 
    { "mc0"  , 's'} ,
    { "mc4"  , 's'} ,
    { "mc5"  , 's'} ,
    { "mc5i"  , 'b'} ,
    { "mir"   , 'b'} ,
    { "msgr"  , 'b'} ,
    { "meml"  , 's'} ,
    { "memu"  , 's'} ,
    { "mgc"  , 's'} ,
    { "nel"   , 's'} , 
    { "oc"   , 's'} , 
    { "op"   , 's'} , 
    { "npc"   , 'b'} ,
    { "pairs"  , '#'} , 
    { "rc"  , 's'} ,
    { "rev"  , 's'} ,
    { "rep"  , 's'} ,
    { "ri"    , 's'} , 
    { "rin"    , 's'} , 
    { "ritm"    , 's'} , 
    { "rmacs" , 's'} , 
    { "rmam"  , 's'} ,
    { "rmm"  , 's'} ,
    { "rmir"  , 's'} ,
    { "rmkx"  , 's'} , 
    { "rmso"  , 's'} ,
    { "rmcup"  , 's'} ,
    { "rmul"  , 's'} ,
    { "rs1"  , 's'} ,
    { "rs2"  , 's'} ,
    { "sc"  , 's'} ,
    { "setab"  , 's'} ,
    { "setaf"  , 's'} ,
    { "sgr"  , 's'} ,
    { "sgr0"  , 's'} ,
    { "sitm"  , 's'} ,
    { "smcup"  , 's'} ,
    { "smglr"  , 's'} ,
    { "smm"  , 's'} ,
    { "smacs" , 's'} , 
    { "smam"  , 's'} ,
    { "smir"  , 's'} ,
    { "smkx"  , 's'} , 
    { "smso"  , 's'} ,
    { "smul"  , 's'} ,
    { "tbc"  , 's'} ,
    { "u6"    , 's'} , 
    { "u7"    , 's'} , 
    { "u8"    , 's'} , 
    { "u9"    , 's'} , 
    { "vt"    , '#'} ,
    { "vpa"    , 's'} ,
    { "xenl"  , 'b'} ,
    { "xon"   , 'b'} ,
    { NULL    , 0  }
};

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

    printf("terminfo_location=%p\n", terminfo_location);

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
            strncpy(tmpbuf, ptr, tok - ptr - 1);

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
                        char oct[4];
                        strncpy(oct, tmpptr, 3);
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
            strncpy(tmpbuf, ptr, tok - ptr - 1);
            type = '#';
            /* int_entry */
        } else {
            strcpy(tmpbuf, ptr);
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
                ret->data[i].int_entry = atoi(tok);
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

char *tiparm(const char *cap, ...)
{
    if (cap == NULL)
        return NULL;

    int idx;

    if ((idx = get_termcap_idx(cap, 0)) == -1)
        return NULL;

    memset(tiparm_ret, 0, sizeof(tiparm_ret));

    const char *src = ((struct terminfo *)cur_term->terminfo)->data[idx].string_entry;
    const char *src_ptr = src;
    char *dst_ptr = tiparm_ret;

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
        if (*src_ptr != '%') {
            *dst_ptr++ = *src_ptr;
            goto next;
        }

        if (!*(++src_ptr))
            goto fail;

        switch (*src_ptr)
        {
            case ':':
                //case '+': /* this clashes with %+ ? */
            case '#':
                goto next;
            case 'd':
                if (cur_stack == 0)
                    goto fail;
                dst_ptr += snprintf(dst_ptr, dst_ptr - tiparm_ret, "%d", stack[cur_stack--].val);
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
            case 'i': /* +1 to first 2 parameters */
                while (num_arg < 2) {
                    int_arg[num_arg++] = va_arg(ap, int);
                }
                int_arg[0]++;
                int_arg[1]++;
                goto next;
            case '?': /* %? expr %t thenpart %e elsepart %; */
                {
                    if (*(++src_ptr) != '%')
                        goto fail;

                    if (!*(++src_ptr))
                        goto fail;
                    
                    int val = 0;
                    
                    /* parse expr */
                    switch(*src_ptr) {
                        /* TODO add other things here? somehow reuse the main? */
                        case 'p':
                            if (!isdigit(*(++src_ptr)))
                                goto fail;
                            int digit = *src_ptr++ - '0';
                            while (num_arg < digit)
                                int_arg[num_arg++] = va_arg(ap, int);
                            val = int_arg[num_arg-1];
                            break;
                        default:
                            goto fail;
                    }

                    /* thenpart */
                    if (val) {
                        if (*src_ptr++ != '%')
                            goto fail;
                        if (*src_ptr++ != 't')
                            goto fail;
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
                            /* skip over the elsepart */
                            while (*src_ptr && *src_ptr != '%')
                            {
else_skip_again:
                                src_ptr++;
                            }
                            if (!*src_ptr)
                                goto fail;
                            if (*(++src_ptr) == '%')
                                goto else_skip_again;
                            else if (*src_ptr == ';')
                                goto next;
                            else
                                goto fail;
                        }
                    } else /* elsepart */ {
                        /* skip over thenpart */
                        if (*src_ptr++ != '%')
                            goto fail;
                        if (*src_ptr++ != 't')
                            goto fail;
                        while (*src_ptr && *src_ptr != '%')
                        {
else_then_skip:
                            src_ptr++;
                        }
                        if (!*src_ptr) 
                            goto fail;
                        src_ptr++;
                        if (*src_ptr == '%')
                            goto else_then_skip;
                        else if (*src_ptr !='e')
                            goto fail;
                        src_ptr++;
                        /* now we're at the elsepart */
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
                        else if (*src_ptr == ';')
                            goto next;
                        else
                            goto fail;
                    }
                    goto next;
                }
            case 'p':
                src_ptr++;
                if (isdigit(*src_ptr)) {
                    int digit = *src_ptr - '0';
                    src_ptr++;
                    while (num_arg < digit) {
                        /* WTF to do here? */
                        int_arg[num_arg++] = va_arg(ap, int);
                    }
                    if (cur_stack >= max_stack)
                        goto fail;
                    stack[++cur_stack].str = str_arg[digit];
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
            default:
                warnx("unsupported command <%c>", *src_ptr);
                goto fail;
        }
next:
        src_ptr++;
    }

    va_end(ap);
    return tiparm_ret;
fail:
    va_end(ap);
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

    set_curterm(tmp_term);

    if (tigetnum("lines") != -1)
        tmp_term->lines = tigetnum("lines");
    else if (nc_use_env && getenv("LINES") != NULL)
        tmp_term->lines = atoi(getenv("LINES"));

    if (tigetnum("cols") != -1)
        tmp_term->columns = tigetnum("cols");
    else if (nc_use_env && getenv("COLUMNS") != NULL)
        tmp_term->columns = atoi(getenv("COLUMNS"));

    LINES = tmp_term->lines;
    COLS = tmp_term->columns;

    return OK;
}
