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
#include <boot_defines.h>
#include <config.h>

/* bfw_main -- entry point
 *
 * Input parameters:
 *   a,b - not used
 */
void bfw_main(int a, int b)
{
	void (*reentry)();

	sec_bfw_verify(BFW_RAM_BASE);
	puts("SSBM: BFW Done");
	reentry = (void (*)(void *))(uintptr_t)PSCI_BASE;
	(*reentry)();
}
