/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "bchp_common.h"
#include "bchp_sun_top_ctrl.h"

#include "error.h"
#include "lib_types.h"
#include "arch.h"
#include "lib_physio.h"


#if defined(BCHP_DDR34_PHY_BYTE_LANE_0_0_REG_START)
 /*
  * 7250b0, 7364a0, 7364b0, 7364c0, 74371a0, 7260a0,
  * [7366c0 7439b0, 7445d0, 7445e0].
  */
#include "bchp_ddr34_phy_byte_lane_0_0.h"
#define	PHY_BYTE_LANE_0		BCHP_DDR34_PHY_BYTE_LANE_0_0_BL_SPARE_REG
#elif defined(BCHP_DDR34_PHY_BYTE_LANE_0_A_0_REG_START)
 /*
  * 7268a0, 7271a0.
  */
#include "bchp_ddr34_phy_byte_lane_0_a_0.h"
#define	PHY_BYTE_LANE_0		BCHP_DDR34_PHY_BYTE_LANE_0_A_0_BL_SPARE_REG
#else
/*
 * Other chip def, or we are missing the RDB header.
 */
#error ddr_ssc: PHY_BYTE_LANE undefined!
#endif


#if defined(BCHP_DDR34_PHY_BYTE_LANE_0_1_REG_START)
 /*
  * 7366c0, 7439b0, [7445d0, 7445e0].
  */
#include "bchp_ddr34_phy_byte_lane_0_1.h"
#define	PHY_BYTE_LANE_1		BCHP_DDR34_PHY_BYTE_LANE_0_1_BL_SPARE_REG
#endif


#if defined(BCHP_DDR34_PHY_BYTE_LANE_0_2_REG_START)
 /*
  * 7445d0, 7445e0.
  */
#include "bchp_ddr34_phy_byte_lane_0_2.h"
#define	PHY_BYTE_LANE_2		BCHP_DDR34_PHY_BYTE_LANE_0_2_BL_SPARE_REG
#endif


/*
 * BL_SPARE_REG is set by FSBL memsys (shmoo'ing) code to
 * the following specification:
 *
 * bits[11:00] DDR speed in MHz Valid range is 1 to 4095MHz.
 *
 * bits[13:12] DDR width (physical width), valid choices are:
 *	0: 8 bits        1: 16 bit        2: 32 bit        3: 36 bit (32+ecc)
 *
 * bits[15:14] Reserved
 *
 * bits[23:16] Device size, in units of Gbit, this is per DRAM.
 *	Note 0 means 512Mbit. The total capacity per interface is
 *	( DDR_width / device_width ) x device_size.
 *
 * bits[25:24] Device width, this is per DRAM.
 *	0: 8 bit	1: 16 bit	2: reserved for 32 bit	3: reserved
 *
 * bits[31:26] Reserved.
 *
 */

int memsys_ssbl_ddr_info(uint32_t idx, uint32_t *clock, uint32_t *phy_width,
			   uint32_t *dev_size, uint32_t *dev_width)
{
	uint32_t val, t_size, reg = 0;

	switch (idx) {
	case 0:
#ifdef PHY_BYTE_LANE_0
		reg = PHY_BYTE_LANE_0;
#endif
		break;
	case 1:
#ifdef PHY_BYTE_LANE_1
		reg = PHY_BYTE_LANE_1;
#endif
		break;
	case 2:
#ifdef PHY_BYTE_LANE_2
		reg = PHY_BYTE_LANE_2;
#endif
		break;
	default:
		break;
	}

	if (0 == reg)
		return BOLT_ERR;

	val = BDEV_RD(reg);

	if (clock)
		*clock = val & 0xfff;

	if (phy_width) {
		switch ((val >> 12) & 0x3) {
		case 0:
			*phy_width = 8;
			break;
		case 1:
			*phy_width = 16;
			break;
		case 2:
			*phy_width = 32;
			break;
		case 3:
		default:
			*phy_width = 36;
			break;
		}
	}

	t_size = (val >> 16) & 0xff;

	if (dev_size) {
		switch (t_size) {
		case 0:
			*dev_size = 512;
			break;
		default:
			*dev_size = t_size * 1024;
			break;
		}
	}

	if (dev_width) {
		switch ((val >> 24) & 3) {
		case 0:
			*dev_width = 8;
			break;
		case 1:
			*dev_width = 16;
			break;
		case 2:
			*dev_width = 32;
			break;
		case 3:
		default:
			*dev_width = 0;
		}
	}

	return BOLT_OK;
}

