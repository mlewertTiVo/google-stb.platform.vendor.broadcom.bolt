/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
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

int tz_boot(const char *ldrname, bolt_loadargs_t *la);

#endif /* _TZ_H_ */
