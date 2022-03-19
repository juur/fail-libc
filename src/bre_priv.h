#ifndef __BRE_PRIV_H
#define __BRE_PRIV_H

enum n_type { 
	N_DUPL    =  1, /* chr=* or 0, num=DUP from, to=DUP to    */
	N_SIMPLE  =  2,	/* arg0=node, arg1 = dupl?,   next/prev   */
	N_ROOT    =  3, /* arg0 = first node, num: bit0=^ bit1=$  */
	N_CLASS   =  4,	/* num: 1=end_range,2=char_class,3=equiv  */
	N_EXP     =  5, /* single exp: arg0=exp next/prev         */
	N_RANGE   =  6, /* num: bit0=- arg0=start arg1=end?       */
	N_CHAR    =  7, /* chr, num: 0=ORD_CHAR, 1=QUOTED[2], 0=. */
	N_MATCH   =  8, /* arg0 = node inside \( \), num=backref  */
	N_BACKREF =  9, /* num=backref                            */
	N_LIST    = 10, /* arg0 = 1st, num: bit0=-], bit1=[^      */
	N_STRING  = 11, /* str                                    */
};

enum n_subtype {
	N_DUPL_ALL = 1,
	N_DUPL_N   = 2,
	N_DUPL_NP  = 3,
	N_DUPL_N2M = 4
};

struct node {
    struct node   *prev;
	struct node   *next;
    
	struct node   *arg0;
	struct node	  *arg1;
	struct node   *arg2;

    char          *str;
    
	long           num;
	long           to;
    
	enum n_type    type;
	enum n_subtype subtype;
    
	unsigned char  pri;
	char           chr;
};

extern struct node *newNode(enum n_type type);
extern struct node *first_node(struct node *n);
extern void print_node(const struct node *n, int indent);
extern int do_match(const struct node *root, const char *string);

extern int backref_cnt;

#endif
