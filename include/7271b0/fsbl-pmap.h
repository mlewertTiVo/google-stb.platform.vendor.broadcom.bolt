/***************************************************************************
 *     Copyright (c) 2012-2017, Broadcom
 *     All Rights Reserved
 *     Confidential Property of Broadcom
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

/***************************************************************************
*
* This file is generated by generate_pmap_file.pl for supporting platforms.
*
* This file contains a list of PMAPs and the corresponding register settings
*
***************************************************************************/

#ifndef BCHP_FSBL_PMAP_PRIV_H__
#define BCHP_FSBL_PMAP_PRIV_H__

#define AVS_RDB_DATE 20170126 /* %y%m%d */
#define AVS_RDB_TIME 2045 /* %h%m */

const struct pmap_entry pmapTable[]
	__attribute__ ((section(".pmaptable") unused)) = {
	{
		.cpu = { .ndiv = 184, .pdiv = 3, .mdiv = 2 },
		.scb = { .ndiv =   0, .pdiv = 0, .mdiv = 8 },
		.sysif_mdiv = 3,
	},
	{
		.cpu = { .ndiv = 167, .pdiv = 3, .mdiv = 2 },
		.scb = { .ndiv =   0, .pdiv = 0, .mdiv = 9 },
		.sysif_mdiv = 4,
	},
	{
		.cpu = { .ndiv = 184, .pdiv = 3, .mdiv = 2 },
		.scb = { .ndiv =   0, .pdiv = 0, .mdiv = 8 },
		.sysif_mdiv = 3,
	},
};
#endif
