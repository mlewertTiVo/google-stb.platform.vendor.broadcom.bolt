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

#include "bolt.h"
#include "lib_printf.h"
#include "env_subr.h"
#include "devtree.h"
#include "zimage.h"
#include "lib_string.h"

int bolt_zimage_setenv_end(unsigned int address)
{
	int error = DT_OK;
	char buffer[40];

	xsprintf(buffer, "%x", address);
	error = env_setenv("ZIMAGE_END", buffer, ENV_FLG_BUILTIN);
	if (error)
		xprintf("setenv ZIMAGE_END failed!n");
	return error;
}

char *bolt_zimage_getenv_end(void)
{
	return env_getenv("ZIMAGE_END");
}
