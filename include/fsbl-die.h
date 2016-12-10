/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#ifndef __FSBL_DIE_H__
#define __FSBL_DIE_H__


#define DIE_CATEGORY_MASK	0xff00
#define DIE_CODE_MASK		0x00ff


/* Category */

#define DIE_AVS(d)	(d | 0x0100)
#define DIE_BOARD(d)	(d | 0x0200)
#define DIE_FLASH(d)	(d | 0x0300)
#define DIE_SHMOO(d)	(d | 0x0400)
#define DIE_GENERIC(d)	(d | 0x0500)
#define DIE_GENSEC(d)	(d | 0x0600)
#define DIE_SSBL(d)	(d | 0x0700)
#define DIE_BFW(d)	(d | 0x0800)
#define DIE_MEMSYS(d)	(d | 0x0900)
#define DIE_PM(d)	(d | 0x0A00)


/* Fail codes */

/* AVS */
#define	DIE_NO_AVS_IMAGE_INFO			DIE_AVS(1)
#define	DIE_AVS_CODE_LOAD_FROM_EMMC_FAILED	DIE_AVS(2)
#define	DIE_AVS_CODE_LOAD_FAILED		DIE_AVS(3)
#define	DIE_AVS_DATA_LOAD_FAILED		DIE_AVS(4)
#define	DIE_AVS_REVERIFY			DIE_AVS(5)
#define	DIE_AVS_TRACE				DIE_AVS(6)
#define	DIE_AVS_VERIFY_LOAD			DIE_AVS(7)
#define	DIE_AVS_VERIFY_KEY2			DIE_AVS(8)
#define	DIE_AVS_VERIFY_REGION			DIE_AVS(9)

/* Board */
#define	DIE_BOARD_NVM_LOAD			DIE_BOARD(1)
#define	DIE_BOARD_NVM_MAGIC			DIE_BOARD(2)
#define	DIE_BOARD_LIST_LOAD			DIE_BOARD(3)
#define	DIE_BOARD_LIST_MAGIC			DIE_BOARD(4)

/* Flash */
#define	DIE_UNRECOGNIZED_NAND_CHIP		DIE_FLASH(1)
#define	DIE_NAND_FLASH_IS_CORRUPT		DIE_FLASH(2)
#define	DIE_CHIP_SELECT_NOT_SUPPORTED		DIE_FLASH(3)
#define	DIE_UNALIGNED_ACCESS_TO_NAND_FLASH	DIE_FLASH(4)

/* Security (generic) */
#define	DIE_UNKNOWN_IMAGE_TYPE			DIE_GENSEC(1)
#define	DIE_BOOT_2ND_IMAGE_FAILED		DIE_GENSEC(2)
#define	DIE_MEMSYS_NOT_READY			DIE_GENSEC(3)
#define	DIE_OTP_READ_FAILED			DIE_GENSEC(4)
#define	DIE_GET_BOOT_PARAM_FAILED		DIE_GENSEC(5)
#define	DIE_BOOT_PARAMETER_READ_FAILURE		DIE_GENSEC(6)
#define	DIE_NO_BFW_IMAGE			DIE_GENSEC(7)
#define	DIE_NO_MEMSYS_FW_IMAGE			DIE_GENSEC(8)
#define	DIE_MEMSYS_ALT_NOT_SUPPORTED		DIE_GENSEC(9)
#define	DIE_MEMSYS_LOAD_FAILURE			DIE_GENSEC(10)
#define	DIE_NO_AVS_IMAGE			DIE_GENSEC(11)

/* SHMOO */
#define	DIE_SHMOO_LOAD				DIE_SHMOO(1)
#define	DIE_MCB_NOT_FOUND			DIE_SHMOO(2)
#define	DIE_PATCH_MCB				DIE_SHMOO(3)
#define	DIE_MCB_CHECKSUM			DIE_SHMOO(4)
#define	DIE_BAD_BOARD_MSYS_MAGIC		DIE_SHMOO(5)
#define	DIE_MISSING_SHMOO_DATA			DIE_SHMOO(6)
#define	DIE_BAD_MCB_MAGIC			DIE_SHMOO(7)
#define	DIE_BAD_MCB_TABLE			DIE_SHMOO(8)
#define	DIE_WARM_BOOT_NOT_SUPPORTED		DIE_SHMOO(9)

/* Uncategorised */
#define	DIE_OLD_STYLE				DIE_GENERIC(1)
#define	DIE_FLASH_READ_FAILURE			DIE_GENERIC(2)
#define	DIE_XPT_DMA_NOT_DONE			DIE_GENERIC(3)
#define	DIE_STACK_CHECK_FAIL			DIE_GENERIC(4)

/* SSBL */
#define	DIE_SSBL_VERIFY				DIE_SSBL(1)
#define	DIE_SSBL_REVERIFY			DIE_SSBL(2)
#define	DIE_SSBL_DISABLE_REGION			DIE_SSBL(3)
#define	DIE_SSBL_WAIT_REGION			DIE_SSBL(4)
#define	DIE_SSBL_TRACE				DIE_SSBL(5)
#define	DIE_SSBL_LOAD				DIE_SSBL(6)
#define	DIE_SSBL_KEY_1				DIE_SSBL(7)
#define	DIE_SSBL_WRONG_REGION			DIE_SSBL(8)

/* BFW */
#define	DIE_BFW_OTP_READ			DIE_BFW(1)
#define	DIE_BFW_LOAD				DIE_BFW(2)
#define	DIE_BFW_LOAD_EMMC			DIE_BFW(3)
#define	DIE_BFW_OFFSET				DIE_BFW(4)

/* MemsysFW */
#define	DIE_MEMSYS_NO_IMAGE			DIE_MEMSYS(1)
#define	DIE_MEMSYS_TRACE			DIE_MEMSYS(2)
#define	DIE_MEMSYS_DISABLE_REGION		DIE_MEMSYS(3)
#define	DIE_MEMSYS_LOCK				DIE_MEMSYS(4)
#define	DIE_MEMSYS_LOAD				DIE_MEMSYS(5)

/* PM */
#define	DIE_PM_RET_FROM_S3			DIE_PM(1)
#define	DIE_PM_NO_S3_SUPPORT			DIE_PM(2)

#endif /* __FSBL_DIE_H__ */
