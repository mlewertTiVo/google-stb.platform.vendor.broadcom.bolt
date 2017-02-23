/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 ***************************************************************************/

#if defined(CFG_NOSHMOO) || defined(CFG_EMULATION)
/* don't compile anything in this file
*/
#else

#include <aon_defs.h>
#include <boot_defines.h>
#include <fsbl.h>
#include <fsbl-shmoo.h>
#include <memsys.h>
#include <memsys-if.h>

static int wrap_putchar(char c)
{
	putchar((int)c);
	return 0;
}

static int wrap_delay_us(uint32_t us)
{
	udelay(us);
	return 0;
}

void do_shmoo(const struct memsys_interface *gmemsys,
	const struct ddr_info *ddr, uint32_t *mcb, unsigned int options)
{
	uint32_t ret;
	int memc = ddr->which;
	const struct memsys_params *p = &shmoo_params[memc];
	memsys_top_params_t params;
	memsys_system_callbacks_t cb;

	__puts(" MEMSYS-");
	putchar('0' + memc);
	report_hex("@ @ ", p->memc_reg_base);
	putchar(' ');

	memset(&params, 0, sizeof(params));
	memset(&cb, 0, sizeof(cb));

	cb.delay_us = wrap_delay_us,
	cb.putchar = wrap_putchar,
	cb.get_time_us = get_time_us,

	params.version = MEMSYS_FW_VERSION;
	params.edis_info = (EDIS_NPHY) | ((EDIS_OFFS) << 4);

	params.mem_test_size_bytes = _KB(128);
	params.phys_mem_test_base = _MB(ddr->base_mb);

	params.phys_memc_reg_base = p->memc_reg_base;
	params.phys_phy_reg_base  = p->phy_reg_base;
	params.phys_shim_reg_base = p->shim_reg_base;
	params.phys_edis_reg_base = p->edis_reg_base;
	params.saved_state_base = (uint32_t *)MEMSYS_STATE_REG_ADDR(memc);
	params.callbacks = &cb;
	params.mcb_addr = (const uint32_t *)mcb;

	params.options = MEMSYS_OPTION_SAVE_PHY_STATE |
			MEMSYS_OPTION_PREP_PHY_FOR_STANDBY |
			MEMSYS_OPTION_PHY_LOW_POWER_AT_STANDBY;

	if (options & SHMOO_OPTION_DISABLE_CONSOLE)
		params.options |= MEMSYS_OPTION_CONSOLE_OUTPUT_DISABLED;
	if (options & SHMOO_OPTION_WARMBOOT)
		params.options |= MEMSYS_OPTION_WARM_BOOT;

	ret = gmemsys->init(&params);
	if (ret != 0) {
		__puts("\nMEMSYS ERROR: ");
		writehex(ret);
		puts("\n");
		print_shmoo_error(&params.error);
		sec_memsys_set_status(0); /* bad */
		/* Does not return */
	}

	sec_memsys_set_status(1); /* success */
}

#endif	/* CFG_EMULATION || CFG_NOSHMOO */
