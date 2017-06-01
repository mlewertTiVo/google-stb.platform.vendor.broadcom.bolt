/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __LIB_QUEUE_H__
#define __LIB_QUEUE_H__

/*  *********************************************************************
    *  Macros
    ********************************************************************* */

#define q_init(q)	((q)->q_prev = (q), (q)->q_next = (q))
#define q_isempty(q)	((q)->q_next == (q))
#define q_getfirst(q)	((q)->q_next)
#define q_getlast(q)	((q)->q_prev)

/*  *********************************************************************
    *  Types
    ********************************************************************* */

typedef struct queue_s {
	struct queue_s *q_next;
	struct queue_s *q_prev;
} queue_t;

/*  *********************************************************************
    *  Prototypes
    ********************************************************************* */

void q_enqueue(queue_t *, queue_t *);
void q_dequeue(queue_t *);

queue_t *q_deqnext(queue_t *qb);
queue_t *q_deqprev(queue_t *qb);
void q_enqnext(queue_t *qe, queue_t *item);
void q_enqprev(queue_t *qe, queue_t *item);

int q_map(queue_t * qb, int (*func) (queue_t *, unsigned int, unsigned int),
	  unsigned int a, unsigned int b);

int q_count(queue_t *);
int q_find(queue_t *, queue_t *);

#endif /* __LIB_QUEUE_H__ */

