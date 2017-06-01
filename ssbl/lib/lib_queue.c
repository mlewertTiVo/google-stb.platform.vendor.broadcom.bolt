/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <lib_queue.h>
#include <stddef.h>

/*  *********************************************************************
    *  Q_ENQUEUE(qb,item)
    *
    *  Add item to a queue
    *
    *  Input Parameters:
    *		qb - queue block
    *		item - item to add
    *
    *  Return Value:
    *		Nothing.
    ********************************************************************* */

void q_enqueue(queue_t *qb, queue_t *item)
{
	qb->q_prev->q_next = item;
	item->q_next = qb;
	item->q_prev = qb->q_prev;
	qb->q_prev = item;
}

/*  *********************************************************************
    *  Q_DEQUEUE(element)
    *
    *  Remove an element from the queue
    *
    *  Input Parameters:
    *		element - element to remove
    *
    *  Return Value:
    *		Nothing.
    ********************************************************************* */

void q_dequeue(queue_t *item)
{
	item->q_prev->q_next = item->q_next;
	item->q_next->q_prev = item->q_prev;
}

/*  *********************************************************************
    *  Q_DEQNEXT(qb)
    *
    *  Dequeue next element from the specified queue
    *
    *  Input Parameters:
    *		qb - queue block
    *
    *  Return Value:
    *		next element, or NULL
    ********************************************************************* */

queue_t *q_deqnext(queue_t *qb)
{
	if (qb->q_next == qb)
		return NULL;

	qb = qb->q_next;

	qb->q_prev->q_next = qb->q_next;
	qb->q_next->q_prev = qb->q_prev;

	return qb;
}

/*  *********************************************************************
    *  Q_DEQPREV(qb)
    *
    *  Dequeue previous element from the specified queue
    *
    *  Input Parameters:
    *		qb - queue block
    *
    *  Return Value:
    *		previous element, or NULL
    ********************************************************************* */

queue_t *q_deqprev(queue_t *qb)
{
	if (qb->q_prev == qb)
		return NULL;

	qb = qb->q_prev;

	qb->q_prev->q_next = qb->q_next;
	qb->q_next->q_prev = qb->q_prev;

	return qb;
}

/*  *********************************************************************
 * q_enqnext -- adds a new item right after the specified queue element
 *
 * Parameters:
 *  qe   : pointer to a queue element after which a new item is added
 *  item : pointer to an item to add
 *  ********************************************************************* */
void q_enqnext(queue_t *qe, queue_t *item)
{
	item->q_next = qe->q_next;
	item->q_prev = qe;
	qe->q_next->q_prev = item;
	qe->q_next = item;
}

/*  *********************************************************************
 * q_enqprev -- insert a new item right before the specified queue element
 *
 * Parameters:
 *  qe   : pointer to a queue element before which a new item is inserted
 *  item : pointer to an item to insert
 *  ********************************************************************* */
void q_enqprev(queue_t *qe, queue_t *item)
{
	item->q_prev = qe->q_prev;
	item->q_next = qe;
	qe->q_prev->q_next = item;
	qe->q_prev = item;
}

/*  *********************************************************************
    *  Q_MAP(qb)
    *
    *  "Map" a queue, calling the specified function for each
    *  element in the queue
    *
    *  If the function returns nonzero, q_map will terminate.
    *
    *  Input Parameters:
    *		qb - queue block
    *		fn - function pointer
    *		a,b - parameters for the function
    *
    *  Return Value:
    *		return value from function, or zero if entire queue
    *		was mapped.
    ********************************************************************* */

int q_map(queue_t *qb, int (*func) (queue_t *, unsigned int, unsigned int),
	  unsigned int a, unsigned int b)
{
	queue_t *qe;
	queue_t *nextq;
	int res;

	qe = qb;

	qe = qb->q_next;

	while (qe != qb) {
		nextq = qe->q_next;
		res = (*func) (qe, a, b);
		if (res)
			return res;
		qe = nextq;
	}

	return 0;
}

/*  *********************************************************************
    *  Q_COUNT(qb)							*
    *									*
    *  Counts the elements on a queue (not interlocked)			*
    *									*
    *  Input Parameters:						*
    *		qb - queue block					*
    *									*
    *  Return Value:							*
    *		number of elements					*
    ********************************************************************* */
int q_count(queue_t *qb)
{
	queue_t *qe;
	int res = 0;

	qe = qb;

	while (qe->q_next != qb) {
		qe = qe->q_next;
		res++;
	}

	return res;
}

/*  *********************************************************************
    *  Q_FIND(qb,item)
    *
    *  Determines if a particular element is on a queue.
    *
    *  Input Parameters:
    *		qb - queue block
    *		item - queue element
    *
    *  Return Value:
    *		0 - not on queue
    *		>0 - position on queue
    ********************************************************************* */
int q_find(queue_t *qb, queue_t *item)
{
	queue_t *q;
	int res = 1;

	q = qb->q_next;

	while (q != item) {
		if (q == qb)
			return 0;
		q = q->q_next;
		res++;
	}

	return res;
}
