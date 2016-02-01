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

#ifndef _VENDOR_XIOCB_H_
#define _VENDOR_XIOCB_H_

#include "iocb.h"

typedef struct bolt_vendor_xiocb_s
{
	bolt_xint_t placeholder;
}
bolt_vendor_xiocb_t;


bolt_int_t bolt_vendor_doxreq(bolt_vendor_xiocb_t *xiocb);

#endif /* _VENDOR_XIOCB_H_ */
