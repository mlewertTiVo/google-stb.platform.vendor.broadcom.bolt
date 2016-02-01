/***************************************************************************
 *     Copyright (c) 2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#include <fsbl-common.h>
#include <fsbl.h>

#include <bchp_common.h>
#include <bchp_sun_top_ctrl.h>
#include <bchp_aon_pm_l2.h>
#include <bchp_mpm_cpu_data_mem.h>


/* MHL spec 2.2, section 7.9.1.2, p143 */
#define MHL_VER_MINOR 0x0f
#define MHL_VER_MAJOR 0xf0
#define MHL_VER_MINOR_SHIFT 0
#define MHL_VER_MAJOR_SHIFT 4


/* MHL spec 2.2, section 7.9.1.3, p143 */

/* At least 1 bit must be set for this field to be valid. */
#define MHL_DEV_CAT_DEV_TYPE_MASK    0x0f
#define MHL_DEV_CAT_DEV_TYPE_ILLEGAL 0x00
#define MHL_DEV_CAT_DEV_TYPE_SINK    0x01
#define MHL_DEV_CAT_DEV_TYPE_SOURCE  0x02
#define MHL_DEV_CAT_DEV_TYPE_DONGLE  0x03
#define MHL_DEV_CAT_DEV_TYPE_MAX     MHL_DEV_CAT_DEV_TYPE_DONGLE

/*  PLIM      Power avail  MHL versions
 *  {0,0,0}   500mA        1.x, 2.x, 3.0
 *  {0,0,1}   900mA        2.x, 3.0
 *  {0,1,0}   1.5A         2.x, 3.0
 *  {0,1,1}   Dongle       2.x, 3.0
 *  {1,0,0}   2A           3.0 [1]
 */

/* POW=1 If the device can ouptut power across the MHL VBUS. */
#define MHL_DEV_CAT_POW   BIT(4)
#define MHL_DEV_CAT_PLIM0 BIT(5)
#define MHL_DEV_CAT_PLIM1 BIT(6)
#define MHL_DEV_CAT_PLIM2 BIT(7) /* MHL >= 3.0 only (reserved in 2.0) */

#define MHL_DEV_CAT_PLIM_MHL_V2_MASK  (MHL_DEV_CAT_PLIM0 | MHL_DEV_CAT_PLIM1)
#define MHL_DEV_CAT_PLIM_MHL_V3_MASK  (MHL_DEV_CAT_PLIM_MHL_V2_MASK | \
					MHL_DEV_CAT_PLIM2)
#define MHL_DEV_CAT_PLIM_SHIFT 5


/* The interface (AKA 'mailbox') to the MPM.
 * CBUS sends bytes, and the MPM firmware packs these
 * into u32 values at specific places in its data memory.
 */
#define MPM_MAILBOX_SRC_DCAP_BASE  13
#define MPM_MAILBOX_SRC_DCAP_STATE (MPM_MAILBOX_SRC_DCAP_BASE + 0)
#define MPM_MAILBOX_SRC_DCAP_VER   (MPM_MAILBOX_SRC_DCAP_BASE + 1)
#define MPM_MAILBOX_SRC_DCAP_CAT   (MPM_MAILBOX_SRC_DCAP_BASE + 2)

/* Sink date (e.g. from a TV) is offset 16 *bytes* from src (STB/Dongle) */
#define MPM_MAILBOX_SINK_DCAP_BASE  (MPM_MAILBOX_SRC_DCAP_BASE + 16)
#define MPM_MAILBOX_SINK_DCAP_STATE (MPM_MAILBOX_SINK_DCAP_BASE + 0)
#define MPM_MAILBOX_SINK_DCAP_VER   (MPM_MAILBOX_SINK_DCAP_BASE + 1)
#define MPM_MAILBOX_SINK_DCAP_CAT   (MPM_MAILBOX_SINK_DCAP_BASE + 2)


/*  The MPM firmware interface (mailbox) consists of an array of u32
 * registers containing u8 fields at specific offsets (CBUS data is
 * u8.) The STB ARM cpu (little-endian) reads this as:
 *
 * word field
 * 0    3 2 1 0
 * 1    7 6 5 4
 * 2    b a 9 8
 *
 * To read a field:
 *
 * 1. Get the register address by u32 aligning (masking off) the
 * byte offset.
 *
 * 2. Fields are in ascending order so right shift and mask 0xff to
 * get the field. To get the shift (in multiples of u8) get the field
 * offset bits (0..3) and multiply by 8 (<< 3).
 */
static uint8_t mailbox_read_dcap(uint32_t offset)
{
	uint32_t data = BDEV_RD(BCHP_MPM_CPU_DATA_MEM_WORDi_ARRAY_BASE +
			(offset & (~0x3)));

	return (uint8_t)((data >> ((offset & 0x3) << 3)) & 0xff);
}


uint32_t read_mhl_power_config(struct fsbl_info *info)
{
	uint8_t dev_cat, mhl_ver;
	uint32_t pwr = FSBL_RUNFLAG_MHL_BOOT_Nope; /* 0 */

#if defined(OTP_OPTION_MHL_MPM_DISABLE)
	if (OTP_OPTION_MHL_MPM_DISABLE())
		goto out;
#endif
	/* Test to see if strapped for MPM wakup. If strap_mhl_powerup is high
	 * the chip boots in S3 mode (AON supplies up / ONOFF supplies down),
	 * MPM boots, negotiates with TV and wakes up the chip when done (this
	 * is a wakeup event in the AON PM interrupt controller.)
	 */
	if (!(BDEV_RD_F(SUN_TOP_CTRL_STRAP_VALUE_1, strap_mhl_powerup)))
		goto out;

	/* Test to see if we *were* woken up by MPM.
	 * Note: AON_RESET_HISTORY may also set 's3_wakeup' *plus*
	 * 'power_on' or 'software_master'.
	 */
	if (!(BDEV_RD_F(AON_PM_L2_CPU_STATUS, MPM_INTR)))
		goto out;

	mhl_ver = mailbox_read_dcap(MPM_MAILBOX_SINK_DCAP_VER);
#if (CFG_CMD_LEVEL >= 3)
	__puts("MHL sink v");
	writeint((mhl_ver & MHL_VER_MAJOR) >> MHL_VER_MAJOR_SHIFT);
	putchar('.');
	writeint((mhl_ver & MHL_VER_MINOR) >> MHL_VER_MINOR_SHIFT);
	__puts("\n");
#endif
	/* MHL (Mobile High-definition Link) Specification, p144/264.
	 * Sink indicates it does not supply power?
	 */
	dev_cat = mailbox_read_dcap(MPM_MAILBOX_SINK_DCAP_CAT);
	if (!(dev_cat & MHL_DEV_CAT_POW))
		goto out;

	/* Check PLIM2 is valid */
	if (mhl_ver >= 0x30)
		pwr = (dev_cat & MHL_DEV_CAT_PLIM_MHL_V3_MASK);
	else
		pwr = (dev_cat & MHL_DEV_CAT_PLIM_MHL_V2_MASK);

	/* We add one to align with FSBL_RUNFLAG_MHL_BOOT_* values
	so to make sure a non-zero value indicates a MHL derived
	power negotiation */
	pwr = (pwr >> MHL_DEV_CAT_PLIM_SHIFT) + 1;

	info->runflags |= (pwr << FSBL_RUNFLAG_MHL_SHIFT);
out:
	return pwr;
}
