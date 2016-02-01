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

/* FIXME:  char_t required for memsys_init_api.h */
typedef char char_t;
#include "memsys_init_api.h"


#ifdef MEMSYS_TOP_API_H__

struct memsys_interface {
	uint32_t signature;
	void (*setup)(void);
	int (*get_version)(memsys_version_t *ver);
	int (*init)(memsys_top_params_t *params);
	struct memsys_info *shmoo_data;
};


#else /* original version (V1) */

typedef int      (*putchar_t)(int c);
typedef	void     (*udelay_t)(uint32_t us);
typedef	uint64_t (*get_syscount_t)(void);

struct memsys_setup_params {
	putchar_t console;
	udelay_t udt;
	get_syscount_t gst;
	int first;
};

struct memsys_interface {
	uint32_t signature;
	void (*setup)(struct memsys_setup_params *p);
	uint32_t (*get_version)(int32_t *major, int32_t *minor,
				int32_t *exp_ver, char **ver_str);
	uint32_t (*init)(uint32_t *mcb_addr, uint32_t *options);
	struct memsys_info *shmoo_data;
};
#endif /* __MEMSYS_API_V? */

#endif /* _MEMSYS_IF_H */
