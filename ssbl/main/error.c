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
#include "bolt.h"
#include "error.h"

/*  *********************************************************************
    *  Types
    ********************************************************************* */

typedef struct errmap_s {
	int errcode;
	const char *string;
} errmap_t;

/*  *********************************************************************
    *  Error code list
    ********************************************************************* */

errmap_t bolt_errorstrings[] = {
	{BOLT_OK, "No error"},
	{BOLT_ERR, "Error"},
	{BOLT_ERR_INV_COMMAND, "Invalid command"},
	{BOLT_ERR_EOF, "End of file reached"},
	{BOLT_ERR_IOERR, "I/O error"},
	{BOLT_ERR_NOMEM, "Insufficient memory"},
	{BOLT_ERR_DEVNOTFOUND, "Device not found"},
	{BOLT_ERR_DEVOPEN, "Device is open"},
	{BOLT_ERR_INV_PARAM, "Invalid parameter"},
	{BOLT_ERR_ENVNOTFOUND, "Environment variable not found"},
	{BOLT_ERR_ENVREADONLY, "Environment variable is read-only"},
	{BOLT_ERR_BADEXE, "Bad executable format"},
	{BOLT_ERR_NOT32BIT, "Not a 32-bit executable"},
	{BOLT_ERR_WRONGENDIAN, "Executable is wrong-endian"},
	{BOLT_ERR_BADELFVERS, "Invalid ELF file version"},
	{BOLT_ERR_NOTMACHINE, "Incorrect machine architecture"},
	{BOLT_ERR_BADELFFMT, "Invalid ELF file"},
	{BOLT_ERR_BADADDR, "Section would load outside available DRAM"},
	{BOLT_ERR_FILENOTFOUND, "File not found"},
	{BOLT_ERR_UNSUPPORTED, "Unsupported function"},
	{BOLT_ERR_HOSTUNKNOWN, "Host name unknown"},
	{BOLT_ERR_TIMEOUT, "Timeout occured"},
	{BOLT_ERR_PROTOCOLERR, "Network protocol error"},
	{BOLT_ERR_NETDOWN, "Network is down"},
	{BOLT_ERR_NONAMESERVER, "No name server configured"},
	{BOLT_ERR_NOHANDLES, "No more handles"},
	{BOLT_ERR_ALREADYBOUND, "Already bound"},
	{BOLT_ERR_CANNOTSET, "Cannot set network parameter"},
	{BOLT_ERR_NOMORE, "No more enumerated items"},
	{BOLT_ERR_BADFILESYS, "File system not recognized"},
	{BOLT_ERR_FSNOTAVAIL, "File system not available"},
	{BOLT_ERR_INVBOOTBLOCK, "Invalid boot block on disk"},
	{BOLT_ERR_WRONGDEVTYPE, "Device type is incorrect for boot method"},
	{BOLT_ERR_BBCHECKSUM, "Boot block checksum is invalid"},
	{BOLT_ERR_BOOTPROGCHKSUM, "Boot program checksum is invalid"},
	{BOLT_ERR_LDRNOTAVAIL, "Loader is not available"},
	{BOLT_ERR_NOTREADY, "Device is not ready"},
	{BOLT_ERR_GETMEM, "Cannot get memory at specified address"},
	{BOLT_ERR_SETMEM, "Cannot set memory at specified address"},
	{BOLT_ERR_NOTCONN, "Socket is not connected"},
	{BOLT_ERR_ADDRINUSE, "Address is in use"},
	{0, NULL}
};

/*  *********************************************************************
    *  bolt_errortext(err)
    *
    *  Returns the text corresponding to a BOLT error code
    *
    *  Input parameters:
    *      err - error code
    *
    *  Return value:
    *      string description of error
    ********************************************************************* */

const char *bolt_errortext(int err)
{
	errmap_t *e = bolt_errorstrings;

	while (e->string) {
		if (e->errcode == err)
			return e->string;
		e++;
	}

	return (const char *)"Unknown error";
}
