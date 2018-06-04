/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2018 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef _TZ_H_
#define _TZ_H_

#include "loader.h"

int tz_init(void);

int tz_devtree_init(void);
int tz_devtree_init_nwos(void);

int tz_devtree_set(void);
int tz_devtree_set_nwos(void);

int tz_go(bolt_loadargs_t *la);
int tz_boot(const char *ldrname, bolt_loadargs_t *la);

#if CFG_MON64
int tz_install(long load_address, int load_size, long *install_address);

#ifndef TZ_SECURE_BOOT
int sec_verify_tz(void *load_address, int load_size,
		  void *dest_address, int dest_size);
#endif
#endif

#endif /* _TZ_H_ */
