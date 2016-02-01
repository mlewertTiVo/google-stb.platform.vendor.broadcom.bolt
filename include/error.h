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

#ifndef __ERROR_H__
#define __ERROR_H__

#define BOLT_OK			 0
#define BOLT_ERR                 -1	/* generic error */
#define BOLT_ERR_INV_COMMAND	-2
#define BOLT_ERR_EOF		-3
#define BOLT_ERR_IOERR		-4
#define BOLT_ERR_NOMEM		-5
#define BOLT_ERR_DEVNOTFOUND	-6
#define BOLT_ERR_DEVOPEN		-7
#define BOLT_ERR_INV_PARAM	-8
#define BOLT_ERR_ENVNOTFOUND	-9
#define BOLT_ERR_ENVREADONLY	-10

#define BOLT_ERR_BADEXE		-11
#define BOLT_ERR_NOT32BIT 	-12
#define BOLT_ERR_WRONGENDIAN 	-13
#define BOLT_ERR_BADELFVERS 	-14
#define BOLT_ERR_NOTMACHINE 	-15
#define BOLT_ERR_BADELFFMT 	-16
#define BOLT_ERR_BADADDR 	-17

#define BOLT_ERR_FILENOTFOUND	-18
#define BOLT_ERR_UNSUPPORTED	-19

#define BOLT_ERR_HOSTUNKNOWN	-20

#define BOLT_ERR_TIMEOUT		-21

#define BOLT_ERR_PROTOCOLERR	-22

#define BOLT_ERR_NETDOWN		-23
#define BOLT_ERR_NONAMESERVER	-24

#define BOLT_ERR_NOHANDLES	-25
#define BOLT_ERR_ALREADYBOUND	-26

#define BOLT_ERR_CANNOTSET	-27
#define BOLT_ERR_NOMORE		-28
#define BOLT_ERR_BADFILESYS	-29
#define BOLT_ERR_FSNOTAVAIL	-30

#define BOLT_ERR_INVBOOTBLOCK	-31
#define BOLT_ERR_WRONGDEVTYPE	-32
#define BOLT_ERR_BBCHECKSUM	-33
#define BOLT_ERR_BOOTPROGCHKSUM	-34

#define BOLT_ERR_LDRNOTAVAIL	-35

#define BOLT_ERR_NOTREADY	-36

#define BOLT_ERR_GETMEM          -37
#define BOLT_ERR_SETMEM          -38

#define BOLT_ERR_NOTCONN		-39
#define BOLT_ERR_ADDRINUSE	-40

#endif /* __ERROR_H__ */

