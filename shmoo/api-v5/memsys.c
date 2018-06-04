/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2018 Broadcom. All rights reserved.
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

	cb.putchar = wrap_putchar;

	params.version = MEMSYS_FW_VERSION;
	params.mem_test_size_bytes = _KB(128);
	/* phys_mem_test_base[0..MEMSYS_MAX_RANKS-1]
	 * From Memsys Library v5.1.0.1 Integratin Guide:
	 * "For most cases, this should be set to 0 for each rank."
	 */
	params.phys_memc_reg_base = p->memc_reg_base;
	params.phys_phy_reg_base  = p->phy_reg_base;
	params.saved_state_base = (uint32_t *)MEMSYS_STATE_REG_ADDR(memc);
	params.callbacks = &cb;
	params.mcb_addr = mcb;

	params.options = MEMSYS_OPTION_SAVE_PHY_STATE |
			MEMSYS_OPTION_PREP_PHY_FOR_STANDBY |
			MEMSYS_OPTION_PHY_LOW_POWER_AT_STANDBY;

	if (options & SHMOO_OPTION_DISABLE_CONSOLE)
		params.options |= MEMSYS_OPTION_CONSOLE_OUTPUT_DISABLED;
	if (options & SHMOO_OPTION_WARMBOOT)
		params.options |= MEMSYS_OPTION_WARM_BOOT;
	params.options |= (ddr->ddr_size & DDRINFO_NUM_RANKS_MASK) <<
		MEMSYS_OPTION_RANKS_SHIFT;

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
