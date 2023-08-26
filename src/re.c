#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <locale.h>
#include <sys/types.h>
#include <stddef.h>
#include <ctype.h>
#include <stdarg.h>
#include <regex.h>
#include <assert.h>



/* common pre-processor macros */


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



/* typedef structures & unions and directly related pre-processor macros */

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

typedef enum {
    ET_PTRDIFF_T = 1,
    ET_VOID_T    = 2,
    ET_UINT8_T   = 3,
    ET_UINT16_T  = 4,
    ET_UINT32_T  = 5,
    ET_UINT64_T  = 6,
	ET_INT32_T   = 7
} etype_t;

/* stack & queue headers */
typedef struct {
    void   *data;
    int     len;
    int     sp;
    int     pad0;
    int     type;
    etype_t etype;
} stack_t;

typedef struct {
    void   *data;
    int     len;
    int     head;
    int     tail;
    int     type;
    etype_t etype;
} queue_t;

#define TYPE_STACK  1
#define TYPE_QUEUE  2

struct aug_state {
    stack_t *in_vstack;
    stack_t *out_vstack;
    uint8_t *are;
    uint8_t *are_ptr;
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



/* constants */

/* debug strings for etype_t */
static const char *etype_names[] = {
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

__attribute__((malloc(free, 1)))
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

    memset(node, 0, sizeof(node_t));

    free(node);
}

__attribute__((malloc(free_node, 1)))
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
    ret->start_groups  = alloc_array(10);
    ret->end_groups    = alloc_array(10);

    //printf("alloc_node: len=%d @0x%p\n", len, (void *)ret);

    return ret;

fail:
    if (ret) {
        if (ret->firstpos)
            free(ret->firstpos);
        if (ret->lastpos)
            free(ret->lastpos);
        if (ret->followpos)
            free(ret->followpos);
        free(ret);
    }
    return NULL;
}

__attribute__((nonnull,warn_unused_result))
static int add_trans(dfa_state_t *state, dfa_trans_t *trans)
{
    dfa_trans_t *(*new_trans)[];

    if ((new_trans = realloc(state->trans, sizeof(dfa_trans_t *) * (size_t)(state->num_trans + 1))) == NULL)
        return -1;

    state->trans = new_trans;
    (*state->trans)[state->num_trans++] = trans;

    return 0;
}

static void free_dfa_trans(dfa_trans_t *trans)
{
    if (!trans)
        return;

    free(trans);
}

__attribute__((nonnull,warn_unused_result,malloc(free_dfa_trans,1)))
static dfa_trans_t *new_dfa_trans(dfa_state_t *to,
        uint8_t match, array_t *start_capture, array_t *end_capture)
{
    dfa_trans_t *ret;

    if ((ret = malloc(sizeof(dfa_trans_t))) == NULL)
        return NULL;

    ret->to            = to;
    ret->match         = match;
    ret->start_capture = alloc_array(start_capture->len);
    ret->end_capture   = alloc_array(end_capture->len);

    array_copy(ret->start_capture, start_capture);
    array_copy(ret->end_capture, end_capture);

    return ret;
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

__attribute__((nonnull,warn_unused_result,malloc(free_dfa_state,1)))
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
static int push(stack_t *stack, ...)
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
static bool peek(const stack_t *stack, ...)
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
static bool pop(stack_t *stack, ...)
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
static void free_stack(stack_t *stack)
{
    if (!stack)
        return;

    if (stack->data)
        free(stack->data);

    free((void *)stack);
}

__attribute__((malloc(free_stack, 1),warn_unused_result))
static stack_t *alloc_stack(int size, etype_t etype)
{
    stack_t *ret;

    if ((ret = malloc(sizeof(stack_t))) == NULL)
        return NULL;

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
__attribute__((malloc(free_queue, 1),warn_unused_result))
static queue_t *alloc_queue(int size, etype_t etype)
{
    queue_t *ret;

    if ((ret = malloc(sizeof(queue_t))) == NULL)
        return NULL;

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

__attribute__((nonnull))
static void print_token(const token_t *t)
{
    printf("raw=0x%016lx token=%c orig_pos=%2d group=%2d\n",
            t->val,
            t->t.token,
            t->t.orig_pos,
            t->t.group
            );
}

__attribute__((nonnull))
static void print_node(const node_t *n, int indent)
{
	if (indent == 0)
		printf(" root: ");
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
		for (int i = 0; i < indent+2; i++)
			printf(" ");
        printf(" left: ");
        print_node(n->left, indent+2);
    }
    if (n->right) {
		for (int i = 0; i < indent+2; i++)
			printf(" ");
        printf("right: ");
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
static void dump_node_stack(stack_t *stack)
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
__attribute__((nonnull,malloc(free_queue,1),warn_unused_result))
static queue_t *yard(const uint8_t *token_list)
{
    queue_t *output_queue   = NULL;
    stack_t *operator_stack = NULL;
    stack_t *output_stack   = NULL;
	stack_t *group_stack    = NULL;

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
		printf("yard: cur_group=%d group_stack: sp=%d\n", 
				cur_group,
				group_stack->sp
				);

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
				if (cur_group != -1) {
					if (!push(group_stack, cur_group))
						goto fail;
				}

                cur_group = group_num;
                newtok.t.group = group_num++;

                if (!push(operator_stack, newtok.val))
                    goto fail;

                if (!enqueue(output_queue, newtok))
                    goto fail;

                printf("yard: "); print_token(&newtok);
                break;

            case CLOSE:
                /* while the operator at the top of the operator stack is not a left
                 * parenthesis: pop the operator from the operator stack into the
                 * output queue */
                newtok.t.group = cur_group;

				if (peek(group_stack, &dummy)) {
					if (!pop(group_stack, &cur_group))
						goto fail;
				} else
					cur_group = -1;

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

                printf("yard: "); print_token(&newtok);
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
    uint8_t *old_are;
    off_t offset = state->are_ptr - state->are;
    state->are_len += growth;
    old_are = state->are;

    if ((state->are = realloc(old_are, (size_t)state->are_len)) == NULL)
        return false;

    state->are_ptr = state->are + offset;

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

/* this function augments an ASCII ERE via:
 * appending concat and terminal at the end
 * inserting an explict concat operator
 * replacing other ASCII operators with alternatives from iso8859-1
 * the following ERE operators are suppored: ()|*+?
 * unless NCONV is defined at compile time, the expression is simplfied:
 *  a+ becomes a.a*
 *  a? becomes a|NONE
 * escaping via \ is also supported for literals
 *
 * TODO:
 * {,m}:  a{,4}  becomes a?a?a?a?
 * {n,}:  a{4,}  becomes aaaaa*
 * {n,m}: a{1,3} becomes aa?a?
 * [a-z]: expand to (a|b|c|d|e|f|....|z) ?
 * ^$:    tag the RE as being anchored?
 * .:     is retained as the ANY char
 *
 * match groups?? are they impossible in a simplifed DFA?
 */
__attribute__((nonnull,malloc(free, 1),warn_unused_result))
static uint8_t *augment(const char *re)
{
    const char *re_ptr;
    bool running;
    bool previous;
    uint64_t dummy;

    struct aug_state state;

    memset(&state, 0, sizeof(state));

    re_ptr   = re;
    state.are_len  = BUF_INCR;
    running  = true;
    previous = false;

    if ((state.are = malloc((size_t)state.are_len)) == NULL)
        return NULL;

    if ((state.in_vstack = alloc_stack(20L, ET_PTRDIFF_T)) == NULL)
        goto fail;

    if ((state.out_vstack = alloc_stack(20L, ET_PTRDIFF_T)) == NULL)
        goto fail;

    *state.are = OPEN;
    state.are_ptr = state.are + 1;

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
                    ssize_t    len;
                    uint8_t  *new;

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

                    new = (uint8_t *)strndup((const char *)(state.are + open), (size_t)len);
                    if (!pop(state.in_vstack, &dummy)) {
                        errno = EOVERFLOW; goto fail;
                    }

                    *state.are_ptr++ = OPEN;
                    memcpy(state.are_ptr, new, (size_t)len);
                    state.are_ptr += len;

                    free(new);

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

                    if (*re_ptr != '}' ||
                            (from == 0 && to == 0) ||
                            (to != 0 && from > to) ) {
                        errno = EINVAL; goto fail;
                    }

                    /* FIXME this won't work for (...){n,m} */
                    /* FIXME 6 is just the first number it doesn't segfault */
                    if (!grow_buffer(&state, (to - from)*6))
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

                    *state.are_ptr++ = OPEN;
                    previous = false;
                }
                break;

            case ')':
                *state.are_ptr++ = CLOSE;
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
                *state.are_ptr++ = CLOSE;
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
                    uint8_t  *new;

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

                    new = (uint8_t *)strndup((const char *)(state.are + open), (size_t)len);
                    if (!pop(state.in_vstack, &dummy)) {
                        errno = EOVERFLOW; goto fail;
                    }

                    memcpy(state.are_ptr, new, (size_t)len);
                    state.are_ptr += len;
                    *state.are_ptr++ = CAT;
                    memcpy(state.are_ptr, new, (size_t)len);
                    state.are_ptr += len;

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
    return state.are;

fail:
    if (state.are)
        free(state.are);

    state.are = NULL;
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

__attribute__((nonnull))
static void print_state(const dfa_state_t *state)
{
    printf("D[%02d]: {",
            state->id);
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

    /* allocate Dstate[00] - the initial state */
    if ((s0 = new_dfa_state(root->firstpos, root->type == TERM)) == NULL)
        goto fail;
    list = s0;
    s0->id = snum++;

    printf("Input list:\n");

    print_node(root, 0);

    /* nested for() loops are awkward */
    do {
        has_unmarked = false;

        const int vec_sz = root->pos_size + 1;
        array_t *tmp_vec, *done_vec, *tgt_state, *tmp_start_group, *tmp_end_group;

        tmp_vec = alloc_array(vec_sz);
        done_vec = alloc_array(vec_sz);
        tgt_state = alloc_array(vec_sz);
		tmp_start_group = alloc_array(20);
		tmp_end_group = alloc_array(20);

        if (!tmp_vec || !done_vec || !tgt_state)
            goto inner_fail;

        /* keep going until every dfa state is marked */
        for (dfa_state_t *st = list; st; st=st->next)
        {
            if (st->marked)
                continue;

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

int regexec(const regex_t *restrict preg, const char *restrict string, __attribute__((unused)) size_t len,
        __attribute__((unused)) regmatch_t pmatch[], __attribute__((unused)) int eflags)
{
    if (!preg || !string || !preg->priv)
        return -1;

    const dfa_state_t *state = preg->priv;
    const char *p = string;
    bool found = false;

    while (1)
    {
        printf("regexec: \"%c\" [%2d] state=%d",
				isprint(*p) ? *p : ' ',
				(int)(p - string),
				state->id
				);
        /* Check each transition from this state */
        int i;
        int has_any = 0;
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

            if (trans->match == OPEN) {
                printf("*** OPEN found\n");
            } else if (trans->match == CLOSE) {
                printf("*** CLOSE found\n");
			/*
            } else if (trans->match == ANY && (*p && *p != '\n')) {
                printf(" has_any");
                printf(" moving to %d", trans->to->id);
                has_any = i;
			*/
            } else
            /* See if we match this and shift to it */
            if ( (trans->match == ANY && *p && *p != '\n') || 
					(*p && (trans->match == *p)) ) {
                printf(" matched");
                printf(" moving to %d", trans->to->id);
//matched:
                if (pmatch && array_len(trans->start_capture))
                    for (int j = 0; j < trans->start_capture->len; j++)
                        if (trans->start_capture->val[j] != -1) {
                            pmatch[trans->start_capture->val[j]].rm_so = (p - string);
							printf(" enter group %d", trans->start_capture->val[j]);
						}

                if (pmatch && array_len(trans->end_capture))
                    for (int j = 0; j < trans->end_capture->len; j++)
                        if (trans->end_capture->val[j] != -1) {
                            pmatch[trans->end_capture->val[j]].rm_eo = (p - string);
							printf("  exit group %d", trans->end_capture->val[j]);
						}
                            
                state = trans->to;
				printf("\n");
                goto next;
            }
            /* Also check if we have an exit option */
            else if (((!*p || *p == '\n') || !has_any) && (trans->match == TERM)) {
                printf(" term               ");
                if (pmatch && array_len(trans->end_capture))
                    for (int j = 0; j < trans->end_capture->len; j++)
                        if (trans->end_capture->val[j] != -1) {
                            pmatch[trans->end_capture->val[j]].rm_eo = (p - string);
							printf("  exit group %d", trans->end_capture->val[j]);
						}
				if (pmatch)
					pmatch[0].rm_eo = (p - string);
                            
                printf(" term '%c' '%c' %u '%s'\n", *p, *(p+1), has_any, string);
                found = true;
                goto done;
            }
        }
		printf("\n");
		/*
        if (has_any && (*p && *p != '\n')) {
            printf("any\n");
            i = has_any;
            goto matched;
        }
		*/

next:
        /* Prevent going beyond the string termination */
        if (*p == 0 || *p == '\n')
            break;
        p++;
    }

done:
    return found ? 0 : REG_NOMATCH;
}



/* global function defintions */

int regcomp(regex_t *restrict preg, const char *restrict regex, int cflags)
{
    node_t *(*node_lookup)[] = NULL;
    node_t *node = NULL;
    node_t *root = NULL;
    stack_t *node_stack = NULL;
    queue_t *q = NULL;
    dfa_state_t *list = NULL;
    uint8_t *tmp_are = NULL;
    token_t qn;
    int i = 1;

    if (preg == NULL || regex == NULL)
        return -1;

    preg->cflags = cflags;

    printf("0. regex:     %s\n", regex);

    /* process argv[1] which contains ASCII ERE */
    if ((tmp_are = augment(regex)) == NULL)
        goto fail;

    printf("1. tokenised: %s\n", tmp_are);

    /* convert to RPN */
    if ((q = yard(tmp_are/*, &groups*/)) == NULL)
        goto fail;

    //printf("queue\n");

    if ((node_stack = alloc_stack(q->len, ET_VOID_T)) == NULL)
        goto fail;
    if ((node_lookup = malloc(sizeof(node_t *) * (size_t)(q->len + 1))) == NULL)
        goto fail;
    memset(node_lookup, 0, sizeof(node_t *) * (size_t)(q->len + 1));

    printf("2. build AST\n");

    /* Build the Abstract Syntax Tree */
    while (dequeue(q, &qn))
    {
        printf("build_ast: token: ");
        print_token(&qn);

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

			printf("build_ast: token: ");
			print_token(&qn);
        }

        node->type = qn.t.token;

        if (is_operator(qn.t.token))
        {
            node->pos = -1;

            if (!pop(node_stack, &node->right))
                goto fail;

            /* STAR only has c1 not c1 & c2 */
            if (qn.t.token != STAR && !pop(node_stack, &node->left))
                goto fail;

			printf("set left to %x and right to %x\n", node->left, node->right);
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

    printf("3. build DFA\n");
    /* construct the DFA */
    if ((list = build_dfa(root, node_lookup)) == NULL)
        goto fail;

    printf("4. Final Dstates\n");
    for(dfa_state_t *tdfa = list; tdfa; tdfa=tdfa->next)
        print_state(tdfa);
    printf("\n");

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

    printf("errno=%d\n", errno);
    return errno ? -1 : 0;
}
