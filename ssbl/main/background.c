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
#include "lib_string.h"
#include "lib_printf.h"

#include "timer.h"

/*  *********************************************************************
    *  Constants
    ********************************************************************* */

#define MAX_BACKGROUND_TASKS	16

/*  *********************************************************************
    *  Globals
    ********************************************************************* */

static void (*bolt_bg_tasklist[MAX_BACKGROUND_TASKS]) (void *);
static void *bolt_bg_args[MAX_BACKGROUND_TASKS];

/*  *********************************************************************
    *  bolt_bg_init()
    *
    *  Initialize the background task list
    *
    *  Input parameters:
    *      nothing
    *
    *  Return value:
    *      nothing
    ********************************************************************* */

void bolt_bg_init(void)
{
	memset(bolt_bg_tasklist, 0, sizeof(bolt_bg_tasklist));
}

/*  *********************************************************************
    *  bolt_bg_add(func,arg)
    *
    *  Add a function to be called periodically in the background
    *  polling loop.
    *
    *  Input parameters:
    *      func - function pointer
    *      arg - arg to pass to function
    *
    *  Return value:
    *      nothing
    ********************************************************************* */

void bolt_bg_add(void (*func) (void *x), void *arg)
{
	int idx;

	for (idx = 0; idx < MAX_BACKGROUND_TASKS; idx++) {
		if (bolt_bg_tasklist[idx] == NULL) {
			bolt_bg_tasklist[idx] = func;
			bolt_bg_args[idx] = arg;
			return;
		}
	}
}

/*  *********************************************************************
    *  bolt_bg_remove(func)
    *
    *  Remove a function from the background polling loop
    *
    *  Input parameters:
    *      func - function pointer
    *
    *  Return value:
    *      nothing
    ********************************************************************* */

void bolt_bg_remove(void (*func) (void *))
{
	int idx;

	for (idx = 0; idx < MAX_BACKGROUND_TASKS; idx++) {
		if (bolt_bg_tasklist[idx] == func)
			break;
	}

	if (idx == MAX_BACKGROUND_TASKS)
		return;

	for (; idx < MAX_BACKGROUND_TASKS - 1; idx++) {
		bolt_bg_tasklist[idx] = bolt_bg_tasklist[idx + 1];
		bolt_bg_args[idx] = bolt_bg_args[idx + 1];
	}

	bolt_bg_tasklist[idx] = NULL;
	bolt_bg_args[idx] = NULL;
}

/*  *********************************************************************
    *  background()
    *
    *  The main loop and other places that wait for stuff call
    *  this routine to make sure the background handlers get their
    *  time.
    *
    *  Input parameters:
    *      nothing
    *
    *  Return value:
    *      nothing
    ********************************************************************* */

void background(void)
{
	int idx;
	void (*func) (void *arg);

	for (idx = 0; idx < MAX_BACKGROUND_TASKS; idx++) {
		func = bolt_bg_tasklist[idx];
		if (func == NULL)
			break;
		(*func) (bolt_bg_args[idx]);
	}
}
