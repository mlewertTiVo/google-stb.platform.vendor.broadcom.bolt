/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __SSBL_SEC_H__
#define __SSBL_SEC_H__

#include "lib_types.h"

#define SEC_MAX_NUM_RANDOM_NUMBERS 10

const char *sec_get_bsp_version(void);
int sec_get_random_num(uint32_t *dest, int num);
uint32_t sec_enable_debug_ports(void);
void ssbl_main_sec(void);
void sec_mitch_check(void);
uint32_t sec_get_aon_boot_status(uint32_t aon_fw_type);
void sec_set_aon_boot_status(uint32_t aon_fw_type, int success);

#endif /* __SSBL_SEC_H__ */
