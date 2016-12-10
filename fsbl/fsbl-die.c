/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <common.h>
#include <stdbool.h>
#include "fsbl.h"


static void die_report(uint16_t die_code, const char *die_string)
{
	sec_set_errcode(die_code);

	__puts("DIE: ");
	__puts(die_string);
	report_hex(" ", (uint32_t)die_code);
}


/* API */

void __noreturn sys_die(const uint16_t die_code, const char *die_string)
{
	die_report(die_code, die_string);
	loop_forever();
	while (1)
		;
}


void __noreturn memsys_die(const uint16_t die_code, const char *die_string)
{
	__puts("MEMSYS_");
	die_report(die_code, die_string);
	sec_memsys_set_status(0 /* Fail */);
	while (1)
		;
}
