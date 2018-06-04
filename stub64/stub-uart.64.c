/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <aarch64/armv8-regs.h>
#include <aarch64/armv8-cop.h>

/* FIXME: redo the whole of FSBL & a portion of SSBL to rm hwuart.h #includes */
#define __BOLT_ARCH_H
#include <hwuart.h>

#include <bchp_common.h>
#include <bchp_cntcontrolbase.h>
#include <bitops.h>
#include "stub.64.h"

/* do *not* include fsbl.h or lib_physio.h! */

const uint64_t uart_base  = (unsigned)BCHP_UARTA_REG_START;


void uart_init(void)
{
	static uint32_t cntfreq;

#if defined(CFG_EMULATION) || defined(CFG_FULL_EMULATION)
	/* The Veloce environment utilizes a divider of 1, so reassert
	 * the value here.
	 */
	const unsigned const int div = 1;
#else
	/* round to nearest value */
	const unsigned const int div =
		(UART_BASE_CLK + CFG_SERIAL_BAUD_RATE * 8) /
		(CFG_SERIAL_BAUD_RATE * 16);
#endif

	/* Stop CPU System Counter, and clear counter value
	*/
	rdb_write(BCHP_CNTControlBase_CNTCR, 0);
	rdb_write(BCHP_CNTControlBase_CNTCV_LO, 0);
	rdb_write(BCHP_CNTControlBase_CNTCV_HI, 0);

	/* Set the ARM generic timer frequency using the CPU system
	 * counter. system counter clock @ BCHP_CNTControlBase_CNTFID0
	 * (27MHz - default)CNTFRQ
	 *
	 * ==> IMPORTANT: Even if you don't want the uart, we *must* setup
	 * cntfreq for the rest of the system.
	 *
	 */
	cntfreq = rdb_read(BCHP_CNTControlBase_CNTFID0);

	__asm__ __volatile__("msr cntfrq_el0, %0\n"
		: : "r" (cntfreq) : "memory");

	BARRIER64();

	/* Start CPU System Counter with the 1st entry of the frequency modes
	*/
	rdb_write(BCHP_CNTControlBase_CNTCR,
			((1 << 0) << BCHP_CNTControlBase_CNTCR_FCREQ_SHIFT) |
			BCHP_CNTControlBase_CNTCR_EN_MASK);

	rdb_write(uart_base + LCR_REG, 0x83);
	rdb_write(uart_base + DLL_REG, div & 0xff);
	rdb_write(uart_base + DLH_REG, div >> 8);
	rdb_write(uart_base + LCR_REG, 0x03);

	rdb_write(uart_base + FCR_REG, 0x07);
}


void uart_putc(int c)
{
	if (!CFG_FSBL_CONSOLE)
		return;

	while (!(rdb_read(uart_base + LSR_REG) & LSR_THRE))
		;

	rdb_write(uart_base + THR_REG, c);
}
