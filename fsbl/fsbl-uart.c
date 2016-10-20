/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "fsbl.h"
#include "hwuart.h"

static unsigned long uart_base;

unsigned long get_uart_base(void)
{
	return uart_base;
}


void uart_init(unsigned long base)
{
#if defined(CFG_EMULATION) || defined(CFG_FULL_EMULATION)
	/* The Veloce environment utilizes a divider of 1, so reassert
	 * the value here.
	 */
	unsigned int div = 1;
#else
	/* round to nearest value */
	unsigned int div = (UART_BASE_CLK + CFG_SERIAL_BAUD_RATE * 8) /
		(CFG_SERIAL_BAUD_RATE * 16);

	/* setup all hard wired pinmux here, just
	  before we use the uart.
	 */
	fsbl_pinmux();
#endif

	uart_base = BVIRTADDR(base);

	/* SWBOLT-1848: Even if the UART is only used by
	 * Linux and not BOLT we should fully flush it after
	 * any kind of reset to prevent the possibility of
	 * IIR.IID being "character timeout indication",
	 * which the Linux 8250 driver can't handle.
	 */

	/* Disable interrupts. */
	DEV_WR(uart_base + IER_REG, 0x00);
	dmb();

	/* Flush & disable FIFO. */
	DEV_WR(uart_base + FCR_REG, 0x06);

	/* Read to clear status. */
	(void)DEV_RD(uart_base + LSR_REG);
	(void)DEV_RD(uart_base + RBR_REG);
	(void)DEV_RD(uart_base + IIR_REG);

	/* Set Baud rate. */
	DEV_WR(uart_base + LCR_REG, 0x83);
	DEV_WR(uart_base + DLL_REG, div & 0xff);
	DEV_WR(uart_base + DLH_REG, div >> 8);
	DEV_WR_RB(uart_base + LCR_REG, 0x03);
	dmb();

	/* enable FIFO. */
	DEV_WR(uart_base + FCR_REG, 0x01);
}


int putchar(int c)
{
	if (!CFG_FSBL_CONSOLE)
		return 0;

	while (!(DEV_RD(uart_base + LSR_REG) & LSR_THRE))
		;
	DEV_WR(uart_base + THR_REG, c);
	return 0;
}


int getchar(void)
{
	if (!CFG_FSBL_CONSOLE)
		return 0;

	while (!(DEV_RD(uart_base + LSR_REG) & LSR_DR))
		;
	return DEV_RD(uart_base + RBR_REG) & 0xff;
}


#ifndef SECURE_BOOT
int getc(void)
{
	if (!CFG_FSBL_CONSOLE)
		return 0;

	if (!(DEV_RD(uart_base + LSR_REG) & LSR_DR))
		return 0;

	return DEV_RD(uart_base + RBR_REG) & 0xff;
}
#endif

