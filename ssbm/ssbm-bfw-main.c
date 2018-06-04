/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <lib_types.h> /* required by arm-start.h */
#include <arm-start.h>
#include <aon_defs.h>
#include <boot_defines.h>
#include <config.h>

#if CFG_MON64
void arch_launch_mon64(uint32_t uart_base);
extern const uint32_t uart_base;
#endif

/* bfw_main -- entry point
 *
 * Input parameters:
 *   a,b - not used
 */
void bfw_main(int a, int b)
{
#if CFG_MON64
	uint32_t flags;
#endif
	void (*reentry)();

#if CFG_ZEUS5_1
	sec_bfw_verify(BFW_RAM_BASE);
	puts("SSBM: BFW Done");
#endif

#if CFG_MON64
	flags = AON_REG(AON_REG_MAGIC_FLAGS);
	if (flags & S3_FLAG_BOOTED64) {
		arch_launch_mon64(uart_base);
	} else
#endif
	{
		reentry = (void (*)(void *))(uintptr_t)PSCI_BASE;
		(*reentry)();
	}
}
