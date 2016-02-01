/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
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
#if defined(CFG_EMULATION)
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

	DEV_WR(uart_base + LCR_REG, 0x83);
	DEV_WR(uart_base + DLL_REG, div & 0xff);
	DEV_WR(uart_base + DLH_REG, div >> 8);
	DEV_WR_RB(uart_base + LCR_REG, 0x03);

	DEV_WR(uart_base + FCR_REG, 0x07);
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

