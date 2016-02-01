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
#include "bolt.h"
#include "common.h"

#include "bchp_common.h"
#include "bchp_sun_gisb_arb.h"
#include "bchp_sun_l2.h"
#include "bchp_sun_top_ctrl.h"

void bolt_gisb_task(void *arg);


#define MIN_GISB_TIMEOUT 0x50


/* Removed: bolt_gisb_init(). Do nothing instead as FSBL should
have set SUN_GISB_ARB_TIMER up already, or Linux will recover
it (SWLINUX-3317) as we emerge from S3 powersave. */


/*  *********************************************************************
    *  bolt_gisb_task()
    *
    *  Monitor GISB timeouts
    *
    *  Input parameters:
    *      nothing
    *
    *  Return value:
    *      nothing
    ********************************************************************* */

void bolt_gisb_task(void *arg)
{
	uint32_t reg;

	reg = BDEV_RD(BCHP_SUN_L2_CPU_STATUS);

	if (reg & (BCHP_SUN_L2_CPU_STATUS_GISB_TIMEOUT_INTR_MASK |
		    BCHP_SUN_L2_CPU_STATUS_GISB_TEA_INTR_MASK)) {

		/* print debug info */
		xprintf("\n\nERROR: GISB %s\n",
			reg & BCHP_SUN_L2_CPU_STATUS_GISB_TIMEOUT_INTR_MASK ?
			"Timeout" : "TAE");

		xprintf("GISB Address = %#08x\n",
			BDEV_RD(BCHP_SUN_GISB_ARB_ERR_CAP_ADDR));

		xprintf("GISB Data = %#08x\n",
			BDEV_RD(BCHP_SUN_GISB_ARB_ERR_CAP_DATA));

		/* clear timeout interrupt */
		BDEV_WR(BCHP_SUN_GISB_ARB_ERR_CAP_CLR,
			BCHP_SUN_GISB_ARB_ERR_CAP_CLR_clear_MASK);

		BDEV_WR_F(SUN_L2_CPU_CLEAR, GISB_TEA_INTR, 1);
		BDEV_WR_F(SUN_L2_CPU_CLEAR, GISB_TIMEOUT_INTR, 1);
	}
}

uint32_t bolt_gisb_read(void)
{
	return BDEV_RD(BCHP_SUN_GISB_ARB_TIMER);
}

void bolt_gisb_write(uint32_t value)
{
	/* prevent hammering bolt_gisb_task() if the
	timeout is too low. A special case is made for
	value=0 to disable this timer. */
	if ((value > 0) && (value < MIN_GISB_TIMEOUT))
		warn_msg("gisb: timeout of 0x%x is too low! (!0 and < 0x%x)",
			value, MIN_GISB_TIMEOUT);
	else
		BDEV_WR(BCHP_SUN_GISB_ARB_TIMER, value);
}
