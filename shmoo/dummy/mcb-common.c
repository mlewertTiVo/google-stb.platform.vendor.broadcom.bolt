
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



#include <fsbl-common.h>


#define SHMOO_ARRAY_SIZE 0x00000004

#define SHMOO_ARRAY_ELEMENTS 0x00000001

/* shmoo/B100_E001/mcb/memsys_config_1000_4Gx16_le.mcb 
*/
static const uint32_t memsys_config_template[SHMOO_ARRAY_ELEMENTS] __attribute__ ((section(".mcbdata"))) = {
	0xffffffff,	/* 0 */
};


const struct memsys_info __maybe_unused shmoo_data[] __attribute__ ((section(".mcbtable"))) = {
	{ NULL, MCB_MAGIC1, MCB_MAGIC2, MCB_MAGIC3, -1, NULL },
	{ memsys_config_template,	MCB_MAGIC1,	SHMOO_ARRAY_SIZE,	SHMOO_ARRAY_ELEMENTS, -1, NULL },
	{ NULL, 0, 0, 0, 0, NULL }
};
