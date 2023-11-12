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
    { "ht"    , 's'} , 
    { "hts"   , 's'} ,
    { "ich"  , 's'} ,
    { "if"  , 's'} ,
    { "il"  , 's'} ,
    { "il1"   , 's'} ,
    { "ind"   , 's'} , 
    { "ind"   , 's'} , 
    { "is2"  , 's'} ,
    { "it"    , '#'} , 
    { "ka1"   , 's'} , 
    { "ka3"   , 's'} , 
    { "kb2"   , 's'} , 
    { "kbs"   , 's'} , 
    { "kc1"   , 's'} , 
    { "kc3"   , 's'} , 
    { "kcub1" , 's'} , 
    { "kcud1" , 's'} , 
    { "kcuf1" , 's'} , 
    { "kcuu1" , 's'} , 
    { "kdch1"  , 's'} ,
    { "kent"  , 's'} ,
    { "kf0"   , 's'} , 
    { "kf1"   , 's'} , 
    { "kf10"  , 's'} ,
    { "kf11"  , 's'} ,
    { "kf12"  , 's'} ,
    { "kf13"  , 's'} ,
    { "kf14"  , 's'} ,
    { "kf17"  , 's'} ,
    { "kf18"  , 's'} ,
    { "kf19"  , 's'} ,
    { "kf2"   , 's'} , 
    { "kf20"  , 's'} ,
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
    { "kcub1"  , 's'} ,
    { "kll"  , 's'} ,
    { "kll"  , 's'} ,
    { "km"   , 'b'} ,
    { "knp"  , 's'} ,
    { "kopn"  , 's'} ,
    { "kopt"  , 's'} ,
    { "kpp"  , 's'} ,
    { "kprv"  , 's'} ,
    { "kprt"  , 's'} ,
    { "krdo"  , 's'} ,
    { "kref"  , 's'} ,
    { "krfr"  , 's'} ,
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
    { "nel"   , 's'} , 
    { "npc"   , 'b'} ,
    { "pairs"  , '#'} , 
    { "rc"  , 's'} ,
    { "rev"  , 's'} ,
    { "ri"    , 's'} , 
    { "rmacs" , 's'} , 
    { "rmam"  , 's'} ,
    { "rmir"  , 's'} ,
    { "rmkx"  , 's'} , 
    { "rmso"  , 's'} ,
    { "rmul"  , 's'} ,
    { "rs1"  , 's'} ,
    { "rs2"  , 's'} ,
    { "sc"  , 's'} ,
    { "sgr"  , 's'} ,
    { "sgr0"  , 's'} ,
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

static int _putchar(int c)
{
    char ch = (char)c;

    return write(cur_term->fd, &ch, 1);
}

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
                while (*src_ptr && *src_ptr != ';') 
                    src_ptr++;
                if (*src_ptr != ';')
                    goto fail;
                warnx("unsupported %%?");
                goto next;
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
    //printf("tputs(<%p[%d]>,%d,%p)\n", str, strlen(str), affcnt, putfunc);

    if (str == NULL || putfunc == NULL)
        return ERR;

    const char *ptr;

    ptr = str;

    while (*ptr)
    {
        putfunc(*ptr);

        ptr++;
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

    set_curterm(tmp_term);

    return OK;
}
