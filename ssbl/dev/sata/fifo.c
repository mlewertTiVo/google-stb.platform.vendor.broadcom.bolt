/*
 * Copyright (c) 2013, Marc Carino
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of the  nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "fifo.h"
#include "os.h"

static void fifo_pend(fifo_t *fifo)
{
	for (;;) {
		int event_received = 0;

		for (;;) {
			if (fifo->evt) {
				fifo->evt = 0;
				event_received = 1;
				break;
			} else
				cond_wait(&fifo->cnd, &fifo->mtx);
		}

		if (event_received)
			break;
	}
}

static void fifo_post(fifo_t *fifo)
{
	mutex_lock(&fifo->mtx);
	fifo->evt = 1;
	cond_broadcast(&fifo->cnd);
	mutex_unlock(&fifo->mtx);
}

int fifo_init(fifo_t *fifo)
{
	memset(fifo, 0, sizeof(fifo_t));
	assert(!mutex_init(&fifo->mtx));
	assert(!cond_init(&fifo->cnd));
	return 0;
}

int fifo_pop(fifo_t *fifo, int *num)
{
	unsigned int rc = 0;

	assert(fifo);
	assert(num);

	mutex_lock(&fifo->mtx);

	if (fifo->count == 0)
		fifo_pend(fifo);

	if (fifo->count == 0)
		rc = -1;
	else {
		*num = fifo->next[fifo->head];
		fifo->head = (fifo->head + 1) % NUM_CDBS;
		fifo->count--;
	}

	mutex_unlock(&fifo->mtx);

	return rc;
}

int fifo_push(fifo_t *fifo, int num)
{
	unsigned int rc = 0;
	int do_post = 0;

	assert(num >= 0 && num < NUM_CDBS);
	assert(fifo);
	assert(fifo->count < NUM_CDBS);

	mutex_lock(&fifo->mtx);

	if (fifo->count == NUM_CDBS)
		rc = -1;
	else {
		fifo->next[fifo->tail] = num;
		fifo->tail = (fifo->tail + 1) % NUM_CDBS;
		fifo->count++;
		if (fifo->count == 1)
			do_post = 1;
	}

	mutex_unlock(&fifo->mtx);

	if (do_post)
		fifo_post(fifo);

	return rc;
}
