/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_printf.h"

#include "error.h"
#include "board.h"
#include "mmap-dram.h"
#include "chipid.h"

#include "bchp_common.h"
#include "bchp_sun_top_ctrl.h"

#include "tz.h"
#include "tz_priv.h"


struct tz_reg_group s_tz_reg_groups[] = {
	/* Add new ones on top, so it appears at bottom in device tree */
	{
		.compatible = "brcm,brcmstb-memc-tracelog",
		.start = BCHP_MEMC_TRACELOG_0_0_REG_START,
		.end   = BCHP_MEMC_TRACELOG_0_0_REG_END
	},
	{
		.compatible = "brcm,brcmstb-memc-sentinel",
		.start = BCHP_MEMC_SENTINEL_0_0_REG_START,
		.end   = BCHP_MEMC_SENTINEL_0_0_REG_END
	},
	{
		.compatible = "brcm,brcmstb-cpu-biu-arch",
		.start = BCHP_HIF_CPUBIUARCH_REG_START,
		.end   = BCHP_HIF_CPUBIUARCH_REG_END
	},
	{
		.compatible = "brcm,brcmstb-cpu-biu-ctrl",
		.start = BCHP_HIF_CPUBIUCTRL_REG_START,
		.end   = BCHP_HIF_CPUBIUCTRL_REG_END
	},
	{
		.compatible = "brcm,brcmstb-hif-continuation",
		.start = BCHP_HIF_CONTINUATION_REG_START,
		.end   = BCHP_HIF_CONTINUATION_REG_END
	},
	{
		.compatible = "brcm,brcmstb-sun-top-ctrl",
		.start = BCHP_SUN_TOP_CTRL_REG_START,
		.end   = BCHP_SUN_TOP_CTRL_REG_END
	},
	{
		.compatible = NULL,
		.start = 0,
		.end   = 0
	}
};


struct tz_mem_layout s_tz_mem_layout_32MB = {
	.tzioc_offset   = 0x00000000,
	.tzioc_size     = 0x01000000,

	.os_offset      = 0x01000000,
	.os_size        = 0x01000000,

	.t2n_offset     = 0x00001000,
	.t2n_size       = 0x001ff000,
	.n2t_offset     = 0x00200000,
	.n2t_size       = 0x001ff000,

	.tzheaps_offset = 0x00400000,
	.tzheaps_size   = 0x00400000,
	.nwheaps_offset = 0x00800000,
	.nwheaps_size   = 0x00800000
};


struct tz_mem_layout s_tz_mem_layout_16MB = {
	.tzioc_offset   = 0x00000000,
	.tzioc_size     = 0x00600000,

	.os_offset      = 0x00600000,
	.os_size        = 0x00a00000,

	.t2n_offset     = 0x00001000,
	.t2n_size       = 0x000ff000,
	.n2t_offset     = 0x00100000,
	.n2t_size       = 0x000ff000,

	.tzheaps_offset = 0x00200000,
	.tzheaps_size   = 0x00200000,
	.nwheaps_offset = 0x00400000,
	.nwheaps_size   = 0x00200000
};


int tz_config_init(void)
{
	struct tz_info *t;

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	/* UART for TZOS is off by default */
	t->uart_base = 0;

	/* Use UARTA (UART0) for NWOS by default */
	t->uart_nwos = BCHP_PHYSICAL_OFFSET + BCHP_UARTA_REG_START;

	return 0;
}


int tz_config_uart(int uart)
{
	uint32_t __maybe_unused regval;
	struct tz_info *t;

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	if (uart == -1) {
		xprintf("TZ console defaults to UART1\n");
		uart = 1;
	}

	switch (uart) {
	case 1:
		xprintf("TZ console uses UART1\n");
		t->uart_base = (BCHP_PHYSICAL_OFFSET +
				BCHP_UARTB_REG_START);
		xprintf("Please set pin-mux manually for UART1\n");
		break;
	case 2:
		xprintf("TZ console uses UART2\n");
		t->uart_base = (BCHP_PHYSICAL_OFFSET +
				BCHP_UARTC_REG_START);
		xprintf("Please set pin-mux manually for UART2\n");
		break;
	default:
		xprintf("Invalid console uart selection\n");
		return BOLT_ERR;
	}

	return 0;
}

/* EOF */

