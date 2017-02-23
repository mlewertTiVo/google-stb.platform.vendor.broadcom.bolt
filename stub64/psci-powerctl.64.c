/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <psci64.h>
#include <aarch64/armv8-regs.h>
#include <aarch64/armv8-cop.h>

#include <bchp_common.h>
#include <bchp_cntcontrolbase.h>
#include <bchp_hif_continuation.h>
#include <bchp_hif_cpubiuctrl.h>
#include <bchp_sun_top_ctrl.h>

#include <psci.h>
#include <error.h>


/* Macro works as long as the h/w ip blocks have the same offsets. Either
 next to each other, or as part of a larger block. Have to revisit and use
 an array if hardware locates them at non-identical offsets from each other.
*/
#define R_PZONE(x) (BCHP_HIF_CPUBIUCTRL_CPU0_PWR_ZONE_CNTRL_REG + \
			((x) * (BCHP_HIF_CPUBIUCTRL_CPU1_PWR_ZONE_CNTRL_REG - \
				BCHP_HIF_CPUBIUCTRL_CPU0_PWR_ZONE_CNTRL_REG)))

#define F_PZONE(field) \
		(BCHP_HIF_CPUBIUCTRL_CPU0_PWR_ZONE_CNTRL_REG_##field##_MASK)

#define BPCM_TIMEOUT_US 500000 /* 50 mS */
#define BPCM_WAIT_US 10


/* -----------------
 * Support functions
 * -----------------
 */

static inline void reg_bit_set(uint64_t reg, uint32_t mask)
{
	rdb_write(reg, rdb_read(reg) | mask);
	BARRIER64();
}


static inline void reg_bit_clr(uint64_t reg, uint32_t mask)
{
	rdb_write(reg, rdb_read(reg) & (~mask));
	BARRIER64();
}


static int wait_bit_set(uint64_t reg, uint32_t mask)
{
	int to = BPCM_TIMEOUT_US / BPCM_WAIT_US;

	while (to--) {
		if ((rdb_read(reg) & mask))
			return 0;
		udelay(BPCM_WAIT_US);
	}
	return BOLT_ERR_TIMEOUT;
}


static int wait_bit_clr(uint64_t reg, uint32_t mask)
{
	int to = BPCM_TIMEOUT_US / BPCM_WAIT_US;

	while (to--) {
		if (!(rdb_read(reg) & mask))
			return 0;
		udelay(BPCM_WAIT_US);
	}
	return BOLT_ERR_TIMEOUT;
}


/* -------------
 * core features
 * -------------
 */

void set_cpu_boot_addr(unsigned int cpu, uint64_t boot_addr)
{
	uint32_t base;

	/* 2 x 32 bit registers per entry, or:
	 * HIF_CONTINUATION_STB_BOOT_HI_ADDR0	07:00
	 * HIF_CONTINUATION_STB_BOOT_ADDR0	31:00
	 *
	 * 1 x 64 bit register per entry:
	 * HIF_CONTINUATION_STB_BOOT_ADDR0	39:00
	 */
	base = BCHP_HIF_CONTINUATION_REG_START + (cpu << 3);

	if (debug()) {
		msg_cpu("ADDR", cpu);
		__puts(" ");
		writehex64(boot_addr);
		__puts(" @ RDB: ");
		writehex(base);
		puts("");
	}

#ifndef BCHP_HIF_CONTINUATION_STB_BOOT_HI_ADDR0
	rdb_write64(base, boot_addr);
#else
	/* high 8 bits address */
	rdb_write(base, (uint32_t)((boot_addr >> 32) & 0x00000000000000ffULL));
	base += sizeof(uint32_t); /* and, low 32 bits address */
	rdb_write(base, (uint32_t) (boot_addr        & 0x00000000ffffffffULL));
#endif
	BARRIER64();
}


uint32_t cpu_enable(unsigned int cpu, int en)
{
	uint32_t reg, m = 0xffffffff;

	reg = BCHP_HIF_CPUBIUCTRL_CPU_RESET_CONFIG_REG;

	/* TBD: m  = (1 << cpu) */
	switch (cpu) {
#ifdef BCHP_HIF_CPUBIUCTRL_CPU_RESET_CONFIG_REG_CPU0_RESET_MASK
	case 0:
		m = BCHP_HIF_CPUBIUCTRL_CPU_RESET_CONFIG_REG_CPU0_RESET_MASK;
		break;
#endif
#ifdef BCHP_HIF_CPUBIUCTRL_CPU_RESET_CONFIG_REG_CPU1_RESET_MASK
	case 1:
		m = BCHP_HIF_CPUBIUCTRL_CPU_RESET_CONFIG_REG_CPU1_RESET_MASK;
		break;
#endif
#ifdef BCHP_HIF_CPUBIUCTRL_CPU_RESET_CONFIG_REG_CPU2_RESET_MASK
	case 2:
		m = BCHP_HIF_CPUBIUCTRL_CPU_RESET_CONFIG_REG_CPU2_RESET_MASK;
		break;
#endif
#ifdef BCHP_HIF_CPUBIUCTRL_CPU_RESET_CONFIG_REG_CPU3_RESET_MASK
	case 3:
		m = BCHP_HIF_CPUBIUCTRL_CPU_RESET_CONFIG_REG_CPU3_RESET_MASK;
		break;
#endif
	default:
		break;
	}

	if (debug()) {
		msg_cpu((en) ? "ON" : "OFF", cpu);
		__puts(" ");
		writehex(m);
		__puts(" @ ");
		writehex(reg);
		puts("");
	}

	if (m == 0xffffffff)
		return PSCI_ERR_INVALID_PARAMETERS;
	
	BARRIER64();

	if (en)
		reg_bit_clr(BCHP_HIF_CPUBIUCTRL_CPU_RESET_CONFIG_REG, m);
	else
		reg_bit_set(BCHP_HIF_CPUBIUCTRL_CPU_RESET_CONFIG_REG, m);

	return PSCI_SUCCESS;
}


unsigned int cpu_is_on(unsigned int cpu)
{
	uint32_t reg, mask;

	reg = rdb_read(BCHP_HIF_CPUBIUCTRL_CPU_RESET_CONFIG_REG);
	mask = (1 << cpu);

	return !!(reg & mask);
	
}


void reboot(void)
{
	reg_bit_set(BCHP_SUN_TOP_CTRL_RESET_SOURCE_ENABLE,
	BCHP_SUN_TOP_CTRL_RESET_SOURCE_ENABLE_sw_master_reset_enable_MASK);

	reg_bit_set(BCHP_SUN_TOP_CTRL_SW_MASTER_RESET,
	BCHP_SUN_TOP_CTRL_SW_MASTER_RESET_chip_master_reset_MASK);
}


static int is_powered_up(unsigned int cpu)
{
	uint32_t val;

	val = rdb_read(R_PZONE(cpu));

	/* 1 = reset asserted */
	return !(val & F_PZONE(ZONE_RESET_STATE));
}


uint32_t power_up_cpu(unsigned int cpu)
{
	int rc = 0, rd = 0;

	if (debug())
		msg_cpu("PWR_UP", cpu);

	if (is_powered_up(cpu)) {
		if (debug())
			puts("ALRDY_DONE");
		return PSCI_SUCCESS;
	}

	/* Its important to do ZONE_MAN_ISO_CNTL *first* */
	reg_bit_set(R_PZONE(cpu), F_PZONE(ZONE_MAN_ISO_CNTL));
	reg_bit_set(R_PZONE(cpu), F_PZONE(ZONE_MANUAL_CONTROL));
	reg_bit_set(R_PZONE(cpu), F_PZONE(reserved1));
	reg_bit_set(R_PZONE(cpu), F_PZONE(ZONE_MAN_MEM_PWR));

	rc = wait_bit_set(R_PZONE(cpu), F_PZONE(ZONE_MEM_PWR_STATE));
	if (rc) {
		if (debug())
			puts("u1");
		return PSCI_ERR_INTERNAL_FAILURE;
	}

	reg_bit_set(R_PZONE(cpu), F_PZONE(ZONE_MAN_CLKEN));

	rd = wait_bit_set(R_PZONE(cpu), F_PZONE(ZONE_DPG_PWR_STATE));
	if (rd) {
		if (debug())
			puts("u2");
		return PSCI_ERR_INTERNAL_FAILURE;
	}

	reg_bit_clr(R_PZONE(cpu), F_PZONE(ZONE_MAN_ISO_CNTL));
	reg_bit_set(R_PZONE(cpu), F_PZONE(ZONE_MAN_RESET_CNTL));

	if (debug())
		puts("OK");
	return PSCI_SUCCESS;
}


uint32_t power_down_cpu(unsigned int cpu)
{
	int rc = 0, rd = 0;

	if (debug())
		msg_cpu("PWR_DOWN", cpu);

	if (!is_powered_up(cpu)) {
		if (debug())
			puts("ALRDY_DONE");
		return PSCI_SUCCESS;
	}

	/* Its important to do ZONE_MAN_ISO_CNTL *first* */
	reg_bit_set(R_PZONE(cpu), F_PZONE(ZONE_MAN_ISO_CNTL));

	reg_bit_set(R_PZONE(cpu), F_PZONE(ZONE_MANUAL_CONTROL));
	reg_bit_clr(R_PZONE(cpu), F_PZONE(ZONE_MAN_RESET_CNTL));
	reg_bit_clr(R_PZONE(cpu), F_PZONE(ZONE_MAN_CLKEN));
	reg_bit_clr(R_PZONE(cpu), F_PZONE(ZONE_MAN_MEM_PWR));

	rc = wait_bit_clr(R_PZONE(cpu), F_PZONE(ZONE_MEM_PWR_STATE));
	if (rc) {
		if (debug())
			puts("d1");
		return PSCI_ERR_INTERNAL_FAILURE;
	}

	/* This is actually DPG. */
	reg_bit_clr(R_PZONE(cpu), F_PZONE(reserved1));

	rd = wait_bit_clr(R_PZONE(cpu), F_PZONE(ZONE_DPG_PWR_STATE));
	if (rd) {
		if (debug())
			puts("d2");
		return PSCI_ERR_INTERNAL_FAILURE;
	}

	/* If the pwr zone IP block goes wrong then
	 * it can read zero, so we catch that.
	 */
	if (0 == rdb_read(R_PZONE(cpu))) {
		if (debug())
			puts("d3");
		return PSCI_ERR_INTERNAL_FAILURE;
	}

	if (debug())
		puts("OK");
	return PSCI_SUCCESS;
}
