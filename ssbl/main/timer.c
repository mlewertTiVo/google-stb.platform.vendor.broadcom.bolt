/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_printf.h"

#include "timer.h"

#include "bolt.h"
#include "bsp_config.h"
#include "board.h"

/*  *********************************************************************
    *  Data
    ********************************************************************* */

volatile bolt_timer_t bolt_ticks;	/* current system time */

static unsigned int timer_freq_hz = 27000000;	/* typical 27MHz */
static int32_t bolt_oldcount;	/* For keeping track of ticks */
static uint32_t bolt_remticks;
static int bolt_timer_initflg = 0;
static unsigned int timer_counts_per_usec;

#define TIMER_COUNTSPERTICK (timer_freq_hz/BOLT_HZ)

/*  *********************************************************************
    *  set_timer_speed()
    *
    *  Measure and record the timer frequency.
    *
    *  Input parameters:
    *	nothing
    *
    *  Return value:
    *	nothing
    ********************************************************************* */
void set_timer_speed(void)
{
	timer_freq_hz = arch_get_timer_freq_hz();

	timer_counts_per_usec = timer_freq_hz / 1000000;
	if (timer_counts_per_usec == 0)
		timer_counts_per_usec = 1;
}

/*  *********************************************************************
    *  bolt_timer_task()
    *
    *  This routine is called as part of normal device polling to
    *  update the system time.   We read the CP0 COUNT register,
    *  add the delta into our current time, convert to ticks,
    *  and keep track of the COUNT register overflow
    *
    *  Input parameters:
    *	nothing
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void bolt_timer_task(void *arg)
{
	int32_t count;
	int32_t deltaticks;
	uint32_t countspertick;

	countspertick = TIMER_COUNTSPERTICK;

	count = arch_getticks();

	if (count >= bolt_oldcount) {
		deltaticks = (count - bolt_oldcount) / countspertick;
		bolt_remticks += (count - bolt_oldcount) % countspertick;
	} else {
		deltaticks = (bolt_oldcount - count) / countspertick;
		bolt_remticks += (bolt_oldcount - count) % countspertick;
	}

	bolt_ticks += deltaticks + (bolt_remticks / countspertick);
	bolt_remticks %= countspertick;
	bolt_oldcount = count;
}

/*  *********************************************************************
    *  bolt_timer_init()
    *
    *  Initialize the timer module.
    *
    *  Input parameters:
    *	nothing
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

void bolt_timer_init(void)
{
	set_timer_speed();
	bolt_oldcount = arch_getticks();	/* get current COUNT register */
	bolt_ticks = 0;

	if (!bolt_timer_initflg) {
		/* add task for background polling */
		bolt_bg_add(bolt_timer_task, NULL);
		bolt_timer_initflg = 1;
	}
}

/*  *********************************************************************
    *  bolt_sleep(ticks)
    *
    *  Sleep for 'ticks' ticks.  Background tasks are processed while
    *  we wait.
    *
    *  Input parameters:
    *	ticks - number of ticks to sleep (note: *not* clocks!)
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

void bolt_sleep(unsigned int ticks)
{
	bolt_timer_t timer;

	TIMER_SET(timer, ticks);
	while (!TIMER_EXPIRED(timer))
		POLL();
}

/*  *********************************************************************
    *  bolt_usleep(us)
    *
    *  Sleep for approximately the specified number of microseconds.
    *
    *  Input parameters:
    *	us - number of microseconds to wait
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

void bolt_usleep(unsigned int us)
{
	uint32_t now;
	uint32_t gap;
	uint32_t elapsed;

	now = arch_getticks();
	gap = us * timer_counts_per_usec + 1;

	do {
		elapsed = arch_getticks() - now;
	} while (elapsed < gap);
}

/*  *********************************************************************
    *  bolt_msleep(ms)
    *
    *  Sleep for the specified number of milliseconds.
    *  Background tasks are processed while we wait.
    *
    *  Input parameters:
    *	ms - number of milliseconds to wait
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

void bolt_msleep(unsigned int ms)
{
	while (ms-- > 0) {
		bolt_usleep(1000);
		if (ms)		/* to avoid polling for 1 mS wait */
			POLL();
	}
}
