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

#ifndef __ENV_SUBR_H__
#define __ENV_SUBR_H__

/*  *********************************************************************
    *  Constants
    ********************************************************************* */

/*
 * TLV types.  These codes are used in the "type-length-value"
 * encoding of the items stored in the NVRAM device (flash or EEPROM)
 *
 * The layout of the flash/nvram is as follows:
 *
 * <type> <length> <data ...> <type> <length> <data ...> <type_end>
 *
 * The type code of "ENV_TLV_TYPE_END" marks the end of the list.
 * The "length" field marks the length of the data section, not
 * including the type and length fields.
 *
 * Environment variables are stored as follows:
 *
 * <type_env> <length> <flags> <name> = <value>
 *
 * If bit 0 (low bit) is set, the length is an 8-bit value.
 * If bit 0 (low bit) is clear, the length is a 16-bit value
 * 
 * Bit 7 set indicates "user" TLVs.  In this case, bit 0 still
 * indicates the size of the length field.  
 *
 * Flags are from the constants below:
 *
 */

#define ENV_LENGTH_16BITS	0x00	/* for low bit */
#define ENV_LENGTH_8BITS	0x01

#define ENV_TYPE_USER		0x80

#define ENV_CODE_SYS(n,l) (((n)<<1)|(l))
#define ENV_CODE_USER(n,l) ((((n)<<1)|(l)) | ENV_TYPE_USER)

#define ENV_MAX_RECORD		0xff

/*
 * The actual TLV types we support
 */

#define ENV_TLV_TYPE_END	0x00
#define ENV_TLV_TYPE_ENV	ENV_CODE_SYS(0,ENV_LENGTH_8BITS)

/*
 * Environment variable flags 
 */

#define ENV_FLG_NORMAL		0x00	/* normal read/write */
#define ENV_FLG_BUILTIN		0x01	/* builtin - not stored in flash */
#define ENV_FLG_READONLY	0x02	/* read-only - cannot be changed */

#define ENV_FLG_MASK		0xFF	/* mask of attributes we keep */
#define ENV_FLG_ADMIN		0x100	/* lets us internally override permissions */

/*  *********************************************************************
    *  Prototypes
    ********************************************************************* */

int env_delenv(const char *name);
char *env_getenv(const char *name);
int env_setenv(const char *name, const char *value, int flags);
int env_load(void);
int env_save(void);
int env_enum(int idx, char *name, int *namelen, char *val, int *vallen);
int env_envtype(const char *name);
int env_getval(const char *name);

#endif /* __ENV_SUBR_H__ */

