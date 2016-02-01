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

#ifndef __TIMER_H__
#define __TIMER_H__

void bolt_bg_init(void);
void bolt_bg_add(void (*func) (void *), void *arg);
void bolt_bg_remove(void (*func) (void *));

void background(void);

#define POLL() background()

typedef int64_t bolt_timer_t;

void bolt_timer_init(void);
extern volatile bolt_timer_t bolt_ticks;

void bolt_sleep(unsigned int ticks);
void bolt_usleep(unsigned int us);
void bolt_msleep(unsigned int ms);

#define BOLT_HZ 10		/* ticks per second */

#define TIMER_SET(x,v)  x = bolt_ticks + (v)
#define TIMER_EXPIRED(x) ((x) && (bolt_ticks > (x)))
#define TIMER_CLEAR(x) x = 0
#define TIMER_RUNNING(x) ((x) != 0)

#endif /* __TIMER_H__ */

