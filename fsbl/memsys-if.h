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

#ifndef _MEMSYS_IF_H
 #define _MEMSYS_IF_H

#include "memsys_top_api.h"

struct memsys_interface {
	uint32_t signature;
	void (*setup)(void);
	int (*get_version)(memsys_version_t *ver);
	int (*init)(memsys_top_params_t *params);
	struct memsys_info *shmoo_data;
};

#endif /* _MEMSYS_IF_H */
