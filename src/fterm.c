#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <err.h>
#include <stdarg.h>

/*
 * private structures, typedefs, etc.
 */

struct terminfo {
    struct terminfo *next;
    char *name; /* TODO names[] */
    char *desc;
    union {
        bool  bool_entry;
        int   int_entry;
        char *string_entry;
    } data[_CURSES_NUM_DATA];
};

/*
 * constants
 */

static const struct {
    const char *short_name;
    const char type;
} term_caps[_CURSES_NUM_DATA] = {
    { "acsc"  , 's'} , 
    { "am"    , 'b'} , 
    { "bel"   , 's'} , 
    { "clear" , 's'} , 
    { "cols"  , '#'} , 
    { "cr"    , 's'} , 
    { "cub1"  , 's'} , 
    { "cud1"  , 's'} , 
    { "cuf1"  , 's'} , 
    { "cup"   , 's'} , 
    { "cuu1"  , 's'} , 
    { "ed"    , 's'} , 
    { "el"    , 's'} , 
    { "home"  , 's'} , 
    { "ht"    , 's'} , 
    { "ind"   , 's'} , 
    { "ind"   , 's'} , 
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
    { "kf0"   , 's'} , 
    { "kf1"   , 's'} , 
    { "kf2"   , 's'} , 
    { "kf3"   , 's'} , 
    { "kf5"   , 's'} , 
    { "kf6"   , 's'} , 
    { "kf7"   , 's'} , 
    { "kf8"   , 's'} , 
    { "kf9"   , 's'} , 
    { "lines" , '#'} , 
    { "nel"   , 's'} , 
    { "ri"    , 's'} , 
    { "rmacs" , 's'} , 
    { "rmkx"  , 's'} , 
    { "smacs" , 's'} , 
    { "smkx"  , 's'} , 
    { "u8"    , 's'} , 
    { "u9"    , 's'} , 
};

//static const char *terminfo_location = "/usr/share/terminfo/";
static const char *terminfo_location = "";

/* 
 * private globals 
 */

static struct terminfo *termdb = NULL;

/*
 * public globals
 */

TERMINAL *cur_term;

/*
 * private functions
 */

static int get_termcap_idx(const char *capname, char type)
{
    for (int i = 0; i < _CURSES_NUM_DATA; i++)
    {
        if (strcmp(capname, term_caps[i].short_name))
            continue;

        if (type && type != term_caps[i].type)
            continue;

        return i;
    }

    return -1;
}

static void free_terminfo(struct terminfo *term)
{
    if (term->name)
        free(term->name);
    if (term->desc)
        free(term->desc);

    for (int i = 0; i < _CURSES_NUM_DATA; i++)
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
    char buf[BUFSIZ];
    char tmpbuf[BUFSIZ];
    char *ptr, *tok, *desc;
    int rc;
    struct terminfo *ret;

    ret   = NULL;
    rc    = 0;
    tinfo = NULL;
    ptr   = NULL;
    tok   = buf;
    desc  = NULL;

    memset(buf, 0, sizeof(buf));
    memset(tmpbuf, 0, sizeof(tmpbuf));

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
                        /* TODO parse octal string */
                        goto malformed;
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

        for (found = false, i = 0; i < _CURSES_NUM_DATA; i++)
            if (!strcmp(tmpbuf, term_caps[i].short_name)) {
                found = true;
                break;
            }

        if (!found)
            goto malformed;

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
        warnx("Malformed terminfo <%s>");

error:
    if (ret)
        free_terminfo(ret);

    ret = NULL;
    goto done;

}

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

    if ((idx = get_termcap_idx(capname, 'b')) == -1)
        goto fail;

    return ((struct terminfo *)cur_term->terminfo)->data[idx].bool_entry;
fail:
    return -1;
}

int tigetnum(const char *capname)
{
    int idx;

    if ((idx = get_termcap_idx(capname, '#')) == -1)
        goto fail;

    return ((struct terminfo *)cur_term->terminfo)->data[idx].int_entry;
fail:
    return -2;
}

char *tigetstr(const char *capname)
{
    int idx;

    if ((idx = get_termcap_idx(capname, 's')) == -1)
        goto fail;

    return ((struct terminfo *)cur_term->terminfo)->data[idx].string_entry;
fail:
    return (char *)-1;
}

static char tiparm_ret[BUFSIZ];

char *tiparm(const char *cap, ...)
{
    int idx;

    if ((idx = get_termcap_idx(cap, 0)) == -1)
        return NULL;

    memset(tiparm_ret, 0, sizeof(tiparm_ret));

    const char *src = term_caps[idx].short_name;
    const char *src_ptr = src;
    //char *dst = tiparm_ret;
    char *dst_ptr = tiparm_ret;

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
            case '%':
                *dst_ptr++ = '%';
                goto next;
            case 'c': /* pop() printf %c arg */
            case 's': /* pop() printf %s arg */
            case 'l': /* pop() printf strlen(%s) arg */
                goto fail;
                break;
            case '?': /* %? expr %t thenpart %e elsepart %; */
                goto fail;
            case 'p':
                src_ptr++;
                if (isdigit(*src_ptr)) {
                    src_ptr++;
                } else if (isupper(*src_ptr)) {
                    src_ptr++;
                } else if (islower(*src_ptr)) {
                    src_ptr++;
                } else
                    goto fail;
                goto next;
            default:
                goto fail;
        }
next:
        src_ptr++;
    }

    return tiparm_ret;
fail:
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
    char *term_name;

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


