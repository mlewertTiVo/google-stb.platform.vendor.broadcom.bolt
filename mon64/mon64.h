/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __MON64_H__
#define __MON64_H__

#include <stddef.h>
#include <stdint.h>
#include "loader.h"

/* SGI used for mailboxes */
#define MON64_NW_MBOX_SGI 13
#define MON64_TZ_MBOX_SGI 14

void mon64_init(void);

void mon64_install(long load_address, int load_size, long *install_address);

#ifndef TZ_SECURE_BOOT
int sec_verify_mon64(void *load_address, int load_size,
		    void *dest_address, int dest_size);
#endif

int mon64_boot(unsigned int la_flags, long la_entrypt, void *dt_address);

#endif /* __MON64_H__ */
