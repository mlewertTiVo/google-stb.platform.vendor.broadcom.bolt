/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __BCHP_CHIPID_H__
#define __BCHP_CHIPID_H__

#include <stdint.h>

/* supplement to bchp_sun_top_ctrl.h */

#define CHIPID_MAJOR_REV_MASK 0xF0
#define CHIPID_MINOR_REV_MASK 0x0F
#define CHIPID_REV_MASK       (CHIPID_MAJOR_REV_MASK | CHIPID_MINOR_REV_MASK)

static uint32_t __maybe_unused chipid_without_rev(const uint32_t chip_id)
{
	/* 4-digit part numbers look like: 0x7445_0000
	 * 5-digit part numbers look like: 0x0743_7100
	 */
	return chip_id & 0xf0000000 ? chip_id >> 16 : chip_id >> 8;
}

#endif /* __BCHP_CHIPID_H__ */

