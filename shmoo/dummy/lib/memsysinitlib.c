/***************************************************************************
 *     Copyright (c) 2012-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

/* see: fsbl/fsbl-shmoo.c 
*/
#include "../../fsbl/fsbl.h"

typedef enum MemsysErrorCode
{
   memsysErrorNone = 0,
   memsysErrorBadMemory,
   memsysErrorInvalidConfig,
   memsysErrorInvalidRegBase,
   memsysErrorBadChecksum,
   memsysErrorBadVersion,
   memsysErrorNoSystemPll,
   memsysErrorPowerUpTimeout,
   memsysErrorShimPllTimeout,
   memsysErrorDramInitTimeout,
   memsysErrorInvalidSscOption,
} 
MemsysErrorCode;


MemsysErrorCode memsys_init(uint32_t *mcb_addr, uint32_t *options)
{
	return memsysErrorNone;
}


MemsysErrorCode memsys_init_get_version(
  int32_t  *major,
  int32_t  *minor,
  int32_t  *exp_ver,
  char    **ver_str)
{
	*major = 0;
	*minor = 0;
	*exp_ver = 0;
	*ver_str = "DUMMY";

	return memsysErrorNone;
}

