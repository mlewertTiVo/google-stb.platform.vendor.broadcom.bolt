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
#include "lib_physio.h"
#include "error.h"
#include "exchandler.h"

/*  *********************************************************************
    *  mem_peek(d,addr,type)
    *
    *  Read memory of the specified type at the specified address.
    *  Exceptions are caught in the case of a bad memory reference.
    *
    *  Input parameters:
    *	d - pointer to where data should be placed
    *	addr - address to read
    *	type - type of read to do (MEM_BYTE, etc.)
    *
    *  Return value:
    *	0 if ok
    *	else error code
    ********************************************************************* */

int mem_peek(void *d, long addr, int type)
{

	switch (type) {
	case MEM_BYTE:
		PHYSIO_MEM_U8(d) = PHYSIO_MEM_U8(addr);
		break;
	case MEM_HALFWORD:
		PHYSIO_MEM_U16(d) = PHYSIO_MEM_U16(addr);
		break;
	case MEM_WORD:
		PHYSIO_MEM_U32(d) = PHYSIO_MEM_U32(addr);
		break;
	case MEM_QUADWORD:
		PHYSIO_MEM_U64(d) = PHYSIO_MEM_U64(addr);
		break;
	default:
		return BOLT_ERR_INV_PARAM;
	}

	return 0;
}

/* *********************************************************************
   *  Write memory of type at address addr with value val.
   *  Exceptions are caught, handled (error message) and function
   *  returns with 0.
   *
   *  1 success
   *  0 failure
   ********************************************************************* */

int mem_poke(long addr, uint64_t val, int type)
{

	switch (type) {
	case MEM_BYTE:
		PHYSIO_MEM_U8(addr) = (uint8_t)val;
		break;
	case MEM_HALFWORD:
		PHYSIO_MEM_U16(addr) = (uint16_t)val;
		break;
	case MEM_WORD:
		PHYSIO_MEM_U32(addr) = (uint32_t)val;
		break;
	case MEM_QUADWORD:
		PHYSIO_MEM_U64(addr) = (uint64_t)val;
		break;
	default:
		return BOLT_ERR_INV_PARAM;
	}

	return 0;
}
