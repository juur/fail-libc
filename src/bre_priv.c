#define _XOPEN_SOURCE 700

#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "bre_priv.h"

int backref_cnt = 0;

struct node *newNode(const enum n_type type)
{
    struct node *ret;

    if ((ret = calloc(1, sizeof(struct node))) == NULL)
        err(EXIT_FAILURE, "newNode.calloc");

    ret->type = type;

    return ret;
}

inline static char *node_name(const enum n_type type)
{
	switch(type)
	{
		case 1:  return "N_DUPL   ";
		case 2:  return "N_SIMPLE ";
		case 3:  return "N_ROOT   ";
		case 4:  return "N_CLASS  ";
		case 5:  return "N_EXP    ";
		case 6:  return "N_RANGE  ";
		case 7:  return "N_CHAR   ";
		case 8:  return "N_MATCH  ";
		case 9:  return "N_BACKREF";
		case 10: return "N_LIST   ";
		case 11: return "N_STRING ";
		default: return "!!ERROR!!";
	}
}

#define MAX_BACKREF	9

struct state {
	struct {
		const char *from;
		size_t      len;
	} backrefs[MAX_BACKREF];

	const char *ptr;
	const char *str;
	const char *str_end;

	size_t str_len;
	long   backref_cnt;
};

static int match_node(struct state *restrict s, const struct node *restrict n, const bool backwards)
{
	bool need_re;
	int  rc = 0;
	int  cnt = 0;

	switch(n->type)
	{
		case N_RANGE:
			if (n->arg0->type != N_CHAR || n->arg1->type != N_CHAR)
				return -1;

			printf("n_range: %c between [%c-%c]\n",
					*s->ptr, n->arg0->chr, n->arg1->chr);
					
			if (*s->ptr >= n->arg0->chr && *s->ptr <= n->arg1->chr)
				rc = 1;
			break;

		case N_LIST:
			for (const struct node *tmp = n->arg0; tmp && !rc; tmp = tmp->next)
				if ((rc = match_node(s, tmp, backwards)) == -1)
					return -1;
			printf("n_list concluded: %d\n", rc);
			break;

		case N_MATCH:
			s->backrefs[n->num].from = s->ptr;
			rc = match_node(s, n->arg0, backwards);
			if (!rc) break;
			s->backrefs[n->num].len = s->ptr - s->backrefs[n->num].from;
			printf("backref[%ld] [%p,%ld] '%.*s'\n",
					n->num,
					s->backrefs[n->num].from,
					s->backrefs[n->num].len,
					(int)s->backrefs[n->num].len,
					s->backrefs[n->num].from
				  );
			break;

		case N_CHAR:
			printf("n_char: %c == %c\n", n->chr, *s->ptr);
			if (n->chr == *(s->ptr)) {
				return 1;
			}
			return 0;
			break;

		case N_SIMPLE:
			{
				need_re = n->arg1 ? true : false;
				printf("n_simple: need_re: %d\n", need_re);
				cnt = 0;

				do {
					if ((rc = match_node(s, n->arg0, backwards)) == -1)
						return -1;

					printf("n_simple: s->ptr:%2x[%c] need_re:%d cnt:%d rc:%d\n", *s->ptr, *s->ptr, need_re, cnt, rc);

					if (rc) {
						cnt++;
						s->ptr++;
					}

					if (need_re) {
						switch (n->arg1->subtype) 
						{
							case N_DUPL_ALL:
								if (!rc || *s->ptr == 0) {
									need_re = false;
									rc = 1;
								}
								break;
							case N_DUPL_N2M:
								printf("n_simple: N_DUPL_N2M: cnt:%d\n", cnt);
								if (cnt >= n->arg1->num && cnt <= n->arg1->to && !rc) {
									rc = 1;
									need_re = false;
								} else if (!rc)
									need_re = false;
								break;
							case N_DUPL_NP:
								printf("n_simple: N_DUPL_NP: cnt:%d\n", cnt);
								if (cnt >= n->arg1->num && !rc) {
									rc = 1;
									need_re = false;
								} else if (!rc)
									need_re = false;
								break;
							case N_DUPL_N:
								printf("n_simple: N_DUPL_N: cnt:%d\n", cnt);
								if (!rc || cnt == n->arg1->num)
									need_re = false;
								break;
						}
					} else {
						break;
					}
				} while(need_re);

				/* i think this is wrong depending on the re */
				if (!rc)
					return 0;

				if (n->next)
					rc = match_node(s, n->next, backwards);
			}
			break;

		default:
			warnx("unimp");
			return -1;
	}

	return rc;
}

int do_match(const struct node *restrict root, const char *restrict string)
{
	struct state *st;
	const char *start;

	if ((st = calloc(1, sizeof(struct state))) == NULL)
		return -1;

	start       = string;
	st->str_len = strlen(string);
	st->str_end = string + st->str_len;
	st->str     = string;

again:
	st->ptr = start;

	printf("\ndo_match.match_node [%p,%p,%p,%p,%lu]\n", string, st->ptr, start, st->str_end, st->str_len);
	int rc = match_node(st, root->arg0, false);
	printf("do_match: rc=%d\n", rc);

	if (((root->num & 1) != 1) && rc == 0 && (++start < st->str_end) )
		goto again;

	if (rc == 1)
		printf("match at %lu\n", start - string);

	free(st);
	return rc;
}

void print_node(const struct node *restrict n, const int indent)
{
	printf("%*snode[%s]", indent, "", node_name(n->type));
	switch(n->type)
	{
		case N_DUPL:
			if (n->chr == '*')
				printf(" *");
			else if (n->to == 0)
				printf(" {%lu}", n->num);
			else if (n->to == -1)
				printf(" {%lu,}", n->num);
			else
				printf(" {%lu,%lu}", n->num, n->to);
			break;

		case N_BACKREF:
			printf(" \\%ld", n->num);
			break;

		case N_ROOT:
			printf(" %s%s:\n", 
					(n->num&1)?"^":"",
					(n->num&2)?"$":""
				  );
			print_node(n->arg0, indent + 1);
			return;

		case N_CHAR:
			if (!n->chr)
				printf(" DOT");
			else
				printf(" '%c'", n->chr);
			break;

		case N_STRING:
			printf(" '%s'", n->str);
			break;

		case N_LIST:
			printf(" %s%s:\n", 
					(n->num&1)?"-] ":"",
					(n->num&2)?"[^":""
				  );
			print_node(n->arg0, indent + 1);
			return;

		case N_MATCH:
			printf(" ref=%lu:\n", n->num);
			print_node(n->arg0, indent + 1);
			return;

		case N_EXP:
			if (n->arg0) {
				printf(" :\n");
				print_node(n->arg0, indent + 1);
			}
			if (n->next)
				print_node(n->next, indent);
			return;

		case N_RANGE:
			printf("%s\n%*s start: ",
					(n->num&1)?"-":"",
					indent, "");
			if (n->arg0)
				print_node(n->arg0, 0);
			else
				printf(" NULL\n");

			printf("%*s   end: ", indent, "");
			if (n->arg1)
				print_node(n->arg1, 0);
			else
				printf(" NULL\n");

			if (n->next)
				print_node(n->next, indent);

			return;

		case N_SIMPLE:
			if (n->arg0) {
				printf(" RE:\n");
				print_node(n->arg0, indent + 1);
			} else {
				printf(" RE MISSING\n");
			}
			if (n->arg1) {
				printf("%*s has duplicate:\n", indent, "");
				print_node(n->arg1, indent + 2);
			}
			if (n->next)
				print_node(n->next, indent);
			return;

		case N_CLASS:
			printf(" [%ld] [:%s:]", n->num, n->str);
			break;

		default:
			printf(" Unknown");
	}
	printf("\n");
}

struct node *first_node(struct node *restrict n)
{
	struct node *tmp = n;
	while (tmp->prev) 
		tmp = tmp->prev;
	return tmp;
}
