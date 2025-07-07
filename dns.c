#define _XOPEN_SOURCE 700

#include <stdint.h>
#include <sys/socket.h>
#include <err.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <locale.h>
#include <strings.h>
#include <arpa/inet.h>

#define HDR_QR (1<<0)
#define HDR_SET_OPCODE(x) (((x)&0xf)<<1)
#define HDR_GET_OPCODE(x) (((x)>>1)&0xf)
#define HDR_AA (1<<6)
#define HDR_TC (1<<7)
#define HDR_RD (1<<8)
#define HDR_RA (1<<9)
#define HDR_SET_RCODE(x) (((x)&0xf)<<11)
#define HDR_GET_RCODE(x) (((x)>>11)&0xf)

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

#define TYPE_A       1
#define TYPE_NS      2
#define TYPE_CNAME   5
#define TYPE_SOA     6
#define TYPE_PTR     12
#define TYPE_MX      15
#define TYPE_TXT     16
#define TYPE_AAAA    28
#define TYPE_SRV     33

#define QTYPE_IXFR    251
#define QTYPE_AXFR    252
#define QTYPE_ALL     255

#define CLASS_IN       1
#define CLASS_NONE     254
#define CLASS_ANY      255

[[maybe_unused]] static const char *qclasses[] = {
    [CLASS_IN]   = "IN",
    [CLASS_NONE] = "NONE",
    [CLASS_ANY]  = "ANY",
    [0xffff]     = NULL,
};

#define   OPCODE_QUERY     0
#define   OPCODE_IQUERY    1
#define   OPCODE_STATUS    2

#define   OPCODE_NOTIFY    4

#define   OPCODE_UPDATE    5

#define   RCODE_NOERROR    0
#define   RCODE_FORMERR    1
#define   RCODE_SERVFAIL   2
#define   RCODE_NXDOMAIN   3
#define   RCODE_NOTIMP     4
#define   RCODE_REFUSED    5

#define   RCODE_YXDOMAIN   6
#define   RCODE_YXRRSET    7
#define   RCODE_NXRRSET    8
#define   RCODE_NOTAUTH    9
#define   RCODE_NOTZONE    10

static const char *qtypes[] = {
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
    [0xffff]     = NULL
};

[[maybe_unused]] static const char *opcodes[] = {
    [OPCODE_QUERY]  = "QUERY",
    [OPCODE_IQUERY] = "IQUERY",
    [OPCODE_STATUS] = "STATUS",
    [OPCODE_NOTIFY] = "NOTIFY",
    [OPCODE_UPDATE] = "UPDATE",
    [0xf]           = NULL
};

[[maybe_unused]] static const char *rcodes[] = {
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
    [0xf]            = NULL
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


#ifdef DEBUG
[[gnu::nonnull,maybe_unused]] static void dump_qname(const unsigned char *qname, const unsigned char *packet)
{
    printf("qname=");
    const unsigned char *tmp = qname;

    if (*tmp == 0xc0) {
        printf("offset=0x%02x [", tmp[1]);
        dump_qname(packet + tmp[1], packet);
        printf("] ");
        return;
    } else if (*tmp & 0xc0) {
        printf("UNSUPPORTED");
        return;
    }

    while (*tmp)
    {
        if (isprint(*tmp))
            printf("%c ", *tmp);
        else
            printf("\\%03o ", *tmp);

        tmp++;
    }
}
#endif

static char *decode_qname(const unsigned char *qname, ssize_t max_len, ssize_t *used, const unsigned char *root)
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

    ret = calloc(1, ret_len + 1);

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
__attribute__((nonnull(1)))
static unsigned char *encode_qname(const char *name, int *len_out)
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

#ifdef DEBUG
[[gnu::nonnull,maybe_unused]] static void hexdump(const char *buf, int len)
{
    for(int i = 0; i < len; i++)
        printf("%02x ", (unsigned char)buf[i]);
}
#endif

static void free_dns_result(struct dns_result *dr)
{
    free(dr);
}

[[gnu::malloc(free_dns_result)]] static struct dns_result *build_result(const struct dns_rr *rr, int num_rrs, void *root)
{
    struct dns_result *res;

    if ((res = calloc(1, sizeof(struct dns_result) +
                    (sizeof(res->rr[0]) * num_rrs))) == NULL)
        return NULL;

    res->num_records = num_rrs;

    for (int i = 0; i < num_rrs; i++)
    {
        res->rr[i].record_type = rr[i].vals.type;
        const struct dns_rr *tmp_rr = &rr[i];

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
__attribute__((nonnull))
static void process_question_block(const char **const inbuf_ptr, int num_qs, const unsigned char *)
{
    for (int count = 0; count < num_qs; count++)
    {
        const char *name_start = *inbuf_ptr;

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
        unsigned char *tmp_qname;

        int len = (*inbuf_ptr) - name_start;

        if ((tmp_qname = calloc(1, len)) == NULL)
            err(EXIT_FAILURE, "calloc");
        memcpy(tmp_qname, name_start, len);

        [[maybe_unused]] uint16_t qtype  = ntohs(*((uint16_t *)*inbuf_ptr)); (*inbuf_ptr) += 2;
        [[maybe_unused]] uint16_t qclass = ntohs(*((uint16_t *)*inbuf_ptr)); (*inbuf_ptr) += 2;

#ifdef DEBUG
        printf("process_qblck: type=0x%02x[%4s] class=0x%02x[%3s]\n ", 
                qtype, 
                qtypes[qtype] ? qtypes[qtype] : "", 
                qclass,
                qclasses[qclass] ? qclasses[qclass] : "");
        //dump_qname(tmp_qname, root);
        /* FIXME */
#endif
        free(tmp_qname);
        //printf("\n");
    }
}

__attribute__((nonnull))
static struct dns_rr *process_rr_block(const char **const inbuf_ptr, int num_rrs, const unsigned char *)
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

#ifdef DEBUG
        printf("process_block: type=0x%02x[%4s] class=0x%02x[%3s] ttl=0x%02d rdlength=0x%03x\n ",
                tmp_rr.vals.type,
                qtypes[tmp_rr.vals.type] ? qtypes[tmp_rr.vals.type] : "", 
                tmp_rr.vals.class,
                qclasses[tmp_rr.vals.class] ? qclasses[tmp_rr.vals.class] : "",
                tmp_rr.vals.ttl,
                tmp_rr.vals.rdlength
              );

        //dump_qname(tmp_rr.name, root);

        if (tmp_rr.vals.rdlength && tmp_rr.additional) {
            char buf[64], *tmp;
            //hexdump(tmp_rr.additional, tmp_rr.vals.rdlength);
            switch (tmp_rr.vals.type)
            {
                case TYPE_SOA:
                    ssize_t lena, lenb;
                    tmp = decode_qname(tmp_rr.additional, tmp_rr.vals.rdlength, &lena, root);
                    printf(" rd.mname=%s", tmp);
                    free(tmp);
                    tmp = decode_qname(tmp_rr.additional + lena, tmp_rr.vals.rdlength - lena, &lenb, root);
                    printf(" rd.rname=%s", tmp);
                    free(tmp);
                    printf(" rd.serial=%u", ntohl(*(uint32_t *)(tmp_rr.additional + lena + lenb)));
                    printf(" rd.refresh=%u", ntohl(*(uint32_t *)(tmp_rr.additional + lena + lenb + 4)));
                    printf(" rd.retry=%u", ntohl(*(uint32_t *)(tmp_rr.additional + lena + lenb + 8)));
                    printf(" rd.expire=%u", ntohl(*(uint32_t *)(tmp_rr.additional + lena + lenb + 16)));
                    printf(" rd.minimum=%u", ntohl(*(uint32_t *)(tmp_rr.additional + lena + lenb + 24)));
                    break;
                case TYPE_A:
                    struct in_addr in;
                    memcpy(&in.s_addr, tmp_rr.additional, sizeof(in.s_addr));
                    inet_ntop(AF_INET, &in, buf, sizeof(buf));
                    printf(" rd.ipv4=%s", buf);
                    break;
                case TYPE_MX:
                    printf(" rd.mx=%u", ntohs(*(uint16_t *)(tmp_rr.additional)));
                    tmp = decode_qname(tmp_rr.additional + sizeof(uint16_t),
                            tmp_rr.vals.rdlength - sizeof(uint16_t), NULL, root);
                    printf(" rd.str=%s", tmp);
                    free(tmp);
                    break;
                case TYPE_TXT:
                case TYPE_PTR:
                case TYPE_CNAME:
                case TYPE_NS:
                    tmp = decode_qname(tmp_rr.additional, tmp_rr.vals.rdlength, NULL, root);
                    printf(" rd.str=%s", tmp);
                    free(tmp);
                    break;
            }
        }
        printf("\n");
#endif

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

    hdr.ident = htons(random());
    hdr.flags |= HDR_RD; /* Recursion Desired */
    hdr.flags = htons(hdr.flags);
    hdr.num_questions = htons(1);

    if ((qname = encode_qname(name, &name_len)) == NULL)
        goto fail;

    const struct dns_rr rr = {
        .name = qname,
        .vals.type = htons(type), /* TYPE_A */
        .vals.class = htons(CLASS_IN),
        .vals.ttl = htonl(0),
        .vals.rdlength = htons(0),
        .allocated = false,
        .additional = NULL,
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

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "POSIX");

    if (argc != 3)
        errx(EXIT_FAILURE, "Usage: %s NAME RR_TYPE", argv[0]);

    int sock_fd;

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        err(EXIT_FAILURE, "socket");

    const struct sockaddr_in sin = {
        .sin_family      = AF_INET,
        .sin_port        = htons(53),
        .sin_addr.s_addr = htonl(INADDR_LOOPBACK),
    };

    if (connect(sock_fd, (struct sockaddr *)&sin, sizeof(sin)) == -1)
        err(EXIT_FAILURE, "connect");

    struct timeval tv;
    gettimeofday(&tv, NULL);

    srandom(tv.tv_usec);
    srandom(random());
    srandom(random());

    ssize_t ret;
    char *buf;
    int pack_len;
    int qtype;

    for (qtype = 0; qtype < 0xffff; qtype++) {
        if (qtypes[qtype] == NULL)
            continue;
        if (!strcasecmp(qtypes[qtype], argv[2]))
            break;
    }

    if (qtype == 0xffff)
        errx(EXIT_FAILURE, "unknown qtype %s", argv[2]);

    if ((buf = build_request(argv[1], &pack_len, qtype)) == NULL)
        err(EXIT_FAILURE, "build_request");

    if ((ret = write(sock_fd, buf, pack_len)) == -1)
        err(EXIT_FAILURE, "write");
    else if (ret == 0)
        errx(EXIT_FAILURE, "write: 0");
    else if (ret != pack_len)
        errx(EXIT_FAILURE, "write: short");

    free(buf);
    char inbuf[1500];

    if ((ret = read(sock_fd, &inbuf, 1500)) == -1)
        err(EXIT_FAILURE, "read");
    else if (ret == 0)
        errx(EXIT_FAILURE, "read: 0");
    else if (ret < (int)sizeof(struct dns_header))
        errx(EXIT_FAILURE, "read: short");

    struct dns_header hdr;
    memcpy(&hdr, inbuf, sizeof(hdr));
    int count;

    for (count = 0; count < 6; count++)
        hdr.words[count] = ntohs(hdr.words[count]);

    //printf("read: %lu\n", ret);

#ifdef DEBUG
    printf("*** Header:\nident=%#x flags=%#x (%s%s%s%s%sOPCODE=%#x[%s] RCODE=%#x[%s]) ans=%d que=%d rrs=%d add_rrs=%d\n",
            hdr.ident,
            hdr.flags,
            hdr.flags & HDR_QR ? "QR " : "",
            hdr.flags & HDR_AA ? "AA " : "",
            hdr.flags & HDR_TC ? "TC " : "",
            hdr.flags & HDR_RD ? "RD " : "",
            hdr.flags & HDR_RA ? "RA " : "",
            HDR_GET_OPCODE(hdr.flags),
            opcodes[HDR_GET_OPCODE(hdr.flags)],
            HDR_GET_RCODE(hdr.flags),
            rcodes[HDR_GET_RCODE(hdr.flags)],
            hdr.num_answers,
            hdr.num_questions,
            hdr.num_rrs,
            hdr.num_add_rrs);
#endif

    const char *inbuf_end = inbuf + ret;
    const char *inbuf_ptr = inbuf + sizeof(hdr);
    void *tmp;
    
    [[maybe_unused]] struct dns_result *result;

    //printf("*** Questions:\n");
    if (hdr.num_questions) {
        if (inbuf_ptr > inbuf_end)
            errx(EXIT_FAILURE, "read: short buffer");

        process_question_block(&inbuf_ptr, hdr.num_questions, (const unsigned char *)inbuf);
    }

    if (hdr.num_answers) {
        //printf("*** Answers:\n");
        
        if (inbuf_ptr > inbuf_end)
            errx(EXIT_FAILURE, "read: short buffer");

        if ((tmp = process_rr_block(&inbuf_ptr, hdr.num_answers, (const unsigned char *)inbuf)) == NULL)
            err(EXIT_FAILURE, "process_rr_block");
        result = build_result(tmp, hdr.num_answers, inbuf);
        free_dns_rr_block(tmp);
    }

    if (hdr.num_rrs) {
        //printf("*** RRs:\n");
        
        if (inbuf_ptr > inbuf_end)
            errx(EXIT_FAILURE, "read: short buffer");

        if ((tmp = process_rr_block(&inbuf_ptr, hdr.num_rrs, (const unsigned char *)inbuf)) == NULL)
            err(EXIT_FAILURE, "process_rr_block");
        free_dns_rr_block(tmp);
    }
    
    if (hdr.num_add_rrs) {
        //printf("*** Additional RRs:\n");
        
        if (inbuf_ptr > inbuf_end)
            errx(EXIT_FAILURE, "read: short buffer");

        if ((tmp = process_rr_block(&inbuf_ptr, hdr.num_add_rrs, (const unsigned char *)inbuf)) == NULL)
            err(EXIT_FAILURE, "process_rr_block");
        free_dns_rr_block(tmp);
    }

    shutdown(sock_fd, SHUT_RDWR);
    close(sock_fd);
    exit(EXIT_SUCCESS);
}
