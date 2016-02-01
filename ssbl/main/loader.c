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

#include "lib_types.h"
#include "lib_string.h"
#include "lib_printf.h"

#include "error.h"

#include "bolt.h"
#include "fileops.h"

#include "loader.h"

/*  *********************************************************************
    *  Externs
    ********************************************************************* */

extern const bolt_loader_t elfloader;
extern const bolt_loader_t rawloader;
extern const bolt_loader_t srecloader;
extern const bolt_loader_t zimgloader;

/*  *********************************************************************
    *  Loader list
    ********************************************************************* */

const bolt_loader_t *const bolt_loaders[] = {
#if CFG_ELF_LDR
	&elfloader,
#endif
#if CFG_RAW_LDR
	&rawloader,
#endif
#if CFG_SREC_LDR
	&srecloader,
#endif
#if CFG_ZIMG_LDR
	&zimgloader,
#endif
	NULL
};

const bolt_loader_t *bolt_user_loader;

/*  *********************************************************************
    *  bolt_addloader(loader)
    *
    *  Add a user provided loader
    *
    *  Input parameters:
    *      loader - loader details
    *
    *  Return value:
    *      0 on success
    ********************************************************************* */

int bolt_addloader(const bolt_loader_t *loader)
{
	if (loader == NULL)
		return BOLT_ERR_INV_PARAM;

	if ((loader->name == NULL) || (loader->loader == NULL))
		return BOLT_ERR_INV_PARAM;

	bolt_user_loader = loader;

	return BOLT_OK;
}

/*  *********************************************************************
    *  bolt_findloader(name)
    *
    *  Find a loader by name
    *
    *  Input parameters:
    *      name - name of loader to find
    *
    *  Return value:
    *      pointer to loader structure or NULL
    ********************************************************************* */

const bolt_loader_t *bolt_findloader(const char *name)
{
	const bolt_loader_t *const *ldr;

	ldr = bolt_loaders;

	while (*ldr) {
		if (strcmp(name, (*ldr)->name) == 0)
			return *ldr;
		ldr++;
	}

	if (bolt_user_loader != NULL) {
		if (strcmp(name, bolt_user_loader->name) == 0)
			return bolt_user_loader;
	}

	return NULL;
}

/*  *********************************************************************
    *  bolt_load_progam(name,args)
    *
    *  Look up a loader and call it.
    *
    *  Input parameters:
    *      name - name of loader to run
    *      args - arguments to pass
    *
    *  Return value:
    *      return value
    ********************************************************************* */

int bolt_load_program(const char *name, bolt_loadargs_t *la)
{
	const bolt_loader_t *ldr;
	int res;

	ldr = bolt_findloader(name);
	if (!ldr)
		return BOLT_ERR_LDRNOTAVAIL;

	res = LDRLOAD(ldr, la);

	return res;
}
