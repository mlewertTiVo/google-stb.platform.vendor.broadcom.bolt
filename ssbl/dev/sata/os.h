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

#if !defined(__OS_H__)
#define __OS_H__

#if defined(_BOLT_)
#define __NONOS
#define __SINGLE_THREADED
#endif

#if !defined(__NONOS)
#include <assert.h>
#include <stdint.h>
#include <sys/time.h>
#include <pthread.h>
#include <sched.h>
#else
#include "stubs.h"
#if defined(_BOLT_)
#include "arch_ops.h"
#endif
#endif

#define MAYBE_UNUSED __attribute__((used))
#define WEAK __attribute__((weak))

#if defined(__NONOS)
typedef int mutex_t;
typedef int cond_t;
typedef int thr_t;
#else
typedef pthread_mutex_t mutex_t;
typedef pthread_cond_t cond_t;
typedef pthread_t thr_t;
#endif

#if defined(__NONOS)
static int MAYBE_UNUSED mutex_init(mutex_t *mutex)
{
	mutex = mutex;
	return -1;
}

static int MAYBE_UNUSED mutex_lock(mutex_t *mutex)
{
	mutex = mutex;
	return -1;
}

static int MAYBE_UNUSED mutex_unlock(mutex_t *mutex)
{
	mutex = mutex;
	return -1;
}

static int MAYBE_UNUSED cond_init(cond_t *cond)
{
	cond = cond;
	return -1;
}

static int MAYBE_UNUSED cond_wait(cond_t *cond, mutex_t *mutex)
{
	cond = cond;
	mutex = mutex;
	return -1;
}

static int MAYBE_UNUSED cond_broadcast(cond_t *cond)
{
	cond = cond;
	return -1;
}

static int MAYBE_UNUSED thr_create(thr_t *thr, void *(*f)(void *a), void *arg)
{
	thr = thr;
	f = f;
	arg = arg;
	return -1;
}

static void MAYBE_UNUSED thr_exit(void *retval)
{
	retval = retval;
}
#else
static int MAYBE_UNUSED mutex_init(mutex_t *mutex)
{
	return pthread_mutex_init(mutex, NULL);
}

static int MAYBE_UNUSED mutex_lock(mutex_t *mutex)
{
	return pthread_mutex_lock(mutex);
}

static int MAYBE_UNUSED mutex_unlock(mutex_t *mutex)
{
	return pthread_mutex_unlock(mutex);
}

static int MAYBE_UNUSED cond_init(cond_t *cond)
{
	return pthread_cond_init(cond, NULL);
}

static int MAYBE_UNUSED cond_wait(cond_t *cond, mutex_t *mutex)
{
	return pthread_cond_wait(cond, mutex);
}

static int MAYBE_UNUSED cond_broadcast(cond_t *cond)
{
	return pthread_cond_broadcast(cond);
}

static int MAYBE_UNUSED thr_create(thr_t *thr, void *(*f)(void *a), void *arg)
{
	return pthread_create(thr, NULL, f, arg);
}

static void MAYBE_UNUSED thr_exit(void *retval)
{
	pthread_exit(retval);
}

#endif /* __NONOS */

extern uint32_t _getticks(void);
extern void cache_flush(void *addr, uint32_t len);
extern void cache_inval(void *addr, uint32_t len);

#endif /* __OS_H__ */
