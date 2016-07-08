/***************************************************************************
 *     Copyright (c) 2012-2016, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __BOOT_DEFINES_H__
#define __BOOT_DEFINES_H__

#include <stdint.h>
#include <compiler.h>

#if CFG_ZEUS4_2

#define BOOT_PARAMETER_OFFSET                   (0x00000040)
#define BOOT_PARAMETER_SIZE                     (0x00000100)

/* BFW */
#define BFW2_IMAGE_FLASH_OFFSET                 (0x00020000)
#define BFW1_IMAGE_FLASH_OFFSET                 (0x00100000)
#define BFW_SIGNATURE_OFFSET                    (0x00030000)
#define BFW_KEY3_OFFSET                         (0x00030100)
#define BFW_AUTHENTICATION_METHOD               (0x00030314)

/* AVS */
#define AVS2_IMAGE_FLASH_OFFSET                 (0x00051000)
#define AVS1_IMAGE_FLASH_OFFSET                 (0x00131000)


#if CFG_ZEUS4_2_1
#define AVS_PARAMETER_OFFSET                    (0x00004880)
#define AVS_SIGNATURE_OFFSET                    (0x00004894)
#define AVS_KEY4_OFFSET                         (0x000049A0)
#else /* Plain ol' Zeus 4.2 */
#define AVS_PARAMETER_OFFSET                    (0x00003C00)
#define AVS_SIGNATURE_OFFSET                    (0x00003C14)
#define AVS_KEY4_OFFSET                         (0x00003D20)
#endif

/* MemsysFW */
#define MEMSYS2_IMAGE_FLASH_OFFSET              (MEMSYS_TEXT_OFFS)
#define MEMSYS1_IMAGE_FLASH_OFFSET              (0x00135000)
#define MEMSYS_PARAMETER_OFFSET                 (MEMSYSFW_SIZE)
#define MEMSYS_SIGNATURE_OFFSET                 (MEMSYSFW_SIZE+0x14)
#define MEMSYS_KEY4_OFFSET                      (MEMSYSFW_SIZE+0x120)

#elif CFG_ZEUS4_1

#define BOOT_PARAMETER_OFFSET                   (0x00000880)
#define BOOT_PARAMETER_SIZE                     (0x00000400)

/* BFW */
#define BFW2_IMAGE_FLASH_OFFSET                 (0x00020000)
#define BFW1_IMAGE_FLASH_OFFSET                 (0x00100000)
#define BFW_SIGNATURE_OFFSET                    (0x00020000)
#define BFW_KEY3_OFFSET                         (0x00020100)
#define BFW_AUTHENTICATION_METHOD               (0x00020314)

/* AVS */
#define AVS2_IMAGE_FLASH_OFFSET                 (0x00051000)
#define AVS_PARAMETER_OFFSET                    (0x00003C00)
#define AVS_SIGNATURE_OFFSET                    (0x00003C10)
#define AVS_KEY4_OFFSET                         (0x00003D10)

#else

#define BOOT_PARAMETER_OFFSET                   (0x000001F0)
#define BOOT_PARAMETER_SIZE                     (0x00000AC0)

/* BFW */
#define BFW2_IMAGE_FLASH_OFFSET                 (0x00020000)
#define BFW_PARAMETER_FLASH_OFFSET              (0x00040000)
#define BFW_AUTHENTICATION_METHOD               (0x00040310)

#define BFW_PARAMETER_FLASH_ADDR (BOLT_TEXT_ADDR|BFW_PARAMETER_FLASH_OFFSET)
#define BFW_IMAGE_FLASH_ADDR     (BOLT_TEXT_ADDR|BFW2_IMAGE_FLASH_OFFSET)

/* AVS */
#define AVS2_IMAGE_FLASH_OFFSET                 (0x00051000)

/* MemsysFW */
#define MEMSYS2_IMAGE_FLASH_OFFSET              (0x00055000)

#endif

#define BFW_USE_EMMC_DATA_PART                  0x0
#define BFW_EMMC_DATA_PART_ADDR                 0x4000000

#define BFW_2ND_PART_OFFSET                     (0x20000)
#define	BFW_LOADFROMDRAM                        0x00
#define	BFW_2NDTIERKEY                          0x03
#define	BFW_LOAD_PARAM                          ((BFW_LOADFROMDRAM << 8) | BFW_2NDTIERKEY)
#define	BFW_LOAD_PARAM_DRAM                     ((1 << 8) | BFW_2NDTIERKEY)

#define BFW_COLDBOOT_BUFFER_ADDR                (0x06000000)
#define BFW_WARMBOOT_BUFFER_ADDR                (0x3fc00000)
#define BFW_PAGELIST_BUFFER_SIZE				0x400

#define MEMC_S3_WARM_BOOT_EN                    0x4
#define MEMC_SSC_EN                             0x8

#define OTP_SECURE_BOOT_BIT                     0x4a
#define OTP_MICH_ENABLE_BIT                     0x4b
#define OTP_MEM_SCRAM_ENABLE_BIT                0x12
#define OTP_HOST_BOOT_CODE_DECRYPT_BIT          0x50
#define OTP_HOST_BOOT_CODE_DECRYPT_EXT_BIT      0x274
#define OTP_BSECK_RELOAD_BIT                    0x58
#define OTP_BSECK_ENABLE_BIT                    0x59
#define OTP_BSECKPointerInFlash_BIT             0xd
#define OTP_AVS_VERIFY_ENABLE_BIT               0x256
#define OTP_FORCE_DRAM_SCRAM_BIT                0x12
#define OTP_SAGE_SECURE_ENABLE_BIT              0xd4

#define BSP_S_FW_MAJOR_VERSION_SHIFT            18
#define BSP_S_FW_MINOR_VERSION_SHIFT            12
#define BSP_S_FW_SUB_MINOR_VERSION_SHIFT        8

#define ERR_BAD_IMAGE                           1
#define ERR_WARM_BOOT_FAILED                    2
#define ERR_VERIFY_FLASH_PARAMS_FAILED          3
#define ERR_DEFINE_REGION_FAILED                4
#define ERR_ENABLE_REGION_FAILED                5
#define ERR_VEREFY_REGION_FAILED                6
#define ERR_DISABLE_REGION_FAILED               7
#define ERR_MEMC_0_SHMOO_FAILED                 8
#define ERR_MEMC_1_SHMOO_FAILED                 9
#define ERR_NAND_ID_MISMATCH                    10
#define ERR_COPY_CODE_FAILED                    11
#define ERR_NAND_SSBL_NON_PG_ALIGN              12
#define ERR_REVERIFY_REGION_FAILED              13
#define ERR_REVERIFY_GlITCH                     14
#define ERR_GLITCH_TRACE_CHECK                  15
#define ERR_BSECK_IMAGE_ADDR_INVALID            16
#define ERR_MEM_SCRAM_FAILED                    17
#define ERR_TRACE_CHECK                         18
#define ERR_OTP_BIT_READ_FAILED                 19
#define ERR_MEMC_0_INIT_FAILED                  20
#define ERR_MEMC_1_INIT_FAILED                  21
#define ERR_S3_PARAM_HASH_FAILED                22
#define ERR_S3_DDR_HASH_FAILED                  23

/* boot status */
#define BOOT_BFW_STATUS_SHIFT                   0
#define BOOT_AVS_STATUS_SHIFT                   8
#define BOOT_MEMSYS_STATUS_SHIFT                16

#define BOOT_STATUS_MASK                        0x0000000F
#define BOOT_STATUS_FAIL                        0x0000000F
#define BOOT_STATUS_BOOTING                     0x0000000B
#define BOOT_STATUS_SUCCESS                     0x00000005
#define BOOT_IMAGE_MASK                         0x000000F0
#define BOOT_1ST_IMAGE                          0x00000010
#define BOOT_2ND_IMAGE                          0x00000020

#define BOOT_IMAGE_STATUS_MASK                  0x000000FF
#define BOOT_STATUS_1_BOOT                      0x0000001B
#define BOOT_STATUS_1_FAIL                      0x0000001F
#define BOOT_STATUS_1_SUCCESS                   0x00000015

#define BOOT_STATUS_2_BOOT                      0x0000002B
#define BOOT_STATUS_2_FAIL                      0x0000002F
#define BOOT_STATUS_2_SUCCESS                   0x00000025

/* boot param mask and shift */
#define PARAM_PART_SIZE_MASK                    0x001FFFFF
#define PARAM_PART_SIZE_SHIFT                   0
#define PARAM_IMAGE_CS_MASK                     0x70000000
#define PARAM_IMAGE_CS_SHIFT                    28
#define PARAM_CTRL_WORD_ENABLE_MASK             0x80000000
#define PARAM_CTRL_MEMSYS_DISABLE_SHIFT         21

/* boot param for second image  */
#if CFG_ZEUS4_2
#define PARAM_2ND_BFW_PART                      0x0000006C
#define PARAM_2ND_BFW_PART_OFFSET               0x00000070
#define PARAM_2ND_BFW_CTRL                      0x00000074
#define PARAM_2ND_AVS_PART                      0x00000078
#define PARAM_2ND_AVS_PART_OFFSET               0x0000007C
#define PARAM_2ND_AVS_CTRL                      0x00000088
#define PARAM_2ND_MEMSYS_PART                   0x0000008C
#define PARAM_2ND_MEMSYS_PART_OFFSET            0x00000090
#define PARAM_2ND_MEMSYS_CTRL                   0x00000098

/* boot param for first image  */
#define PARAM_1ST_BFW_PART                      0x0000009C
#define PARAM_1ST_BFW_PART_OFFSET               0x000000A0
#define PARAM_1ST_BFW_CTRL                      0x000000A4
#define PARAM_1ST_AVS_PART                      0x000000A8
#define PARAM_1ST_AVS_PART_OFFSET               0x000000AC
#define PARAM_1ST_AVS_CTRL                      0x000000B8
#define PARAM_1ST_MEMSYS_PART                   0x000000BC
#define PARAM_1ST_MEMSYS_PART_OFFSET            0x000000C0
#define PARAM_1ST_MEMSYS_CTRL                   0x000000C8

#else
#define PARAM_2ND_BFW_PART                      0x00000B74
#define PARAM_2ND_BFW_PART_OFFSET               0x00000B78
#define PARAM_2ND_BFW_CTRL                      0x00000B7C
#define PARAM_2ND_AVS_PART                      0x00000B80
#define PARAM_2ND_AVS_PART_OFFSET               0x00000B84
#define PARAM_2ND_AVS_CTRL                      0x00000B90

/* boot param for first image  */
#define PARAM_1ST_BFW_PART                      0x00000BA4
#define PARAM_1ST_BFW_PART_OFFSET               0x00000BA8
#define PARAM_1ST_BFW_CTRL                      0x00000BAC
#define PARAM_1ST_AVS_PART                      0x00000BB0
#define PARAM_1ST_AVS_PART_OFFSET               0x00000BB4
#define PARAM_1ST_AVS_CTRL                      0x00000BC0
#endif

/* boot param for SSBL */
#if CFG_ZEUS4_2
#define PARAM_SSBL_SIZE                         0x0000004C
#define PARAM_SSBL_PART                         0x000000CC
#define PARAM_SSBL_PART_OFFSET                  0x000000D0
#define PARAM_SSBL_CTRL                         0x000000E0
#else
#define PARAM_SSBL_SIZE                         0x0000040C
#define PARAM_SSBL_PART                         0x00000BD4
#define PARAM_SSBL_PART_OFFSET                  0x00000BD8
#define PARAM_SSBL_CTRL                         0x00000BE8
#endif

#define AON_FW_TYPE_MASK                        0x3F000000
#define AON_FW_TYPE_AVS                         0x2A000000
#define AON_FW_TYPE_BFW                         0x1B000000
#define AON_FW_TYPE_MEMSYS                      0x3D000000

#define BOOT_BSP_RESET_MASK                     0xC0000000
#define BOOT_BSP_RESET_FLAG                     0x80000000

#define TRACE_VALID_MARKER          0x712A0000

/* common to secure/non-secure cold/warm boot
*/
#define TRACE_CHECK_SECURE_BOOT_BIT 0

#define TRACE_DEFINE_REGION_BIT     1	/* secure cold boot */
#define TRACE_ENABLE_REGION_BIT     2	/* secure cold boot */
#define TRACE_VERIFY_REGION_BIT     3	/* secure cold boot */
#define TRACE_REVERIFY_REGION_BIT   4	/* secure cold boot */

#define TRACE_VERIFY_WARM_0_BIT     1	/* secure warm boot */
#define TRACE_VERIFY_WARM_1_BIT     2	/* secure warm boot */
#define TRACE_VERIFY_WARM_2_BIT     3	/* secure warm boot */
#define TRACE_VERIFY_WARM_3_BIT     4	/* secure warm boot */


/* common to secure/non-secure cold/warm boot
*/
#define TRACE_RECHECK_SECURE_BOOT_BIT       5
#define TRACE_REVERIFY_MASK         0x000000F0

#define TRACE_SECURE_COLD_BOOT_VALUE	\
			((TRACE_VALID_MARKER) | \
			(1 << TRACE_CHECK_SECURE_BOOT_BIT) | \
		    (1 << TRACE_DEFINE_REGION_BIT) | \
		    (1 << TRACE_ENABLE_REGION_BIT) | \
		    (1 << TRACE_VERIFY_REGION_BIT) | \
		    (1 << TRACE_REVERIFY_REGION_BIT) | \
		    (1 << TRACE_RECHECK_SECURE_BOOT_BIT))

#define TRACE_NON_SECURE_COLD_BOOT_VALUE \
		((TRACE_VALID_MARKER) | \
		(1 << TRACE_CHECK_SECURE_BOOT_BIT) | \
		(1 << TRACE_RECHECK_SECURE_BOOT_BIT))


#define TRACE_SECURE_S3_WARM_BOOT_VALUE \
		((TRACE_VALID_MARKER) | \
		(1 << TRACE_CHECK_SECURE_BOOT_BIT) | \
		(1 << TRACE_VERIFY_WARM_0_BIT) | \
		(1 << TRACE_VERIFY_WARM_1_BIT) | \
		(1 << TRACE_VERIFY_WARM_2_BIT) | \
		(1 << TRACE_VERIFY_WARM_3_BIT) | \
		(1 << TRACE_RECHECK_SECURE_BOOT_BIT))

#define TRACE_NON_SECURE_S3_WARM_BOOT_VALUE \
		((TRACE_VALID_MARKER) | \
		(1 << TRACE_CHECK_SECURE_BOOT_BIT) | \
		(1 << TRACE_RECHECK_SECURE_BOOT_BIT))

#define TRACE_SECURE_BOOT_VALUE     TRACE_SECURE_COLD_BOOT_VALUE
#define TRACE_NON_SECURE_BOOT_VALUE TRACE_NON_SECURE_COLD_BOOT_VALUE


#if defined(S_UNITTEST)
/*
 * The following defines are used to define the start/end address
 * parameters to the MemAuth command that sets up R2 (aka SSBL).
 * The Start Address parameter includes the region number (2)
 * which is extracted by the define_regionX function to determine
 * the desired region number.
 * The End Address parameter will be updated during compilation
 * with the proper ecode size from ssbl.map if you use the perl
 * script to compile them (make_aegis_cfe.pl).
 */

/*
 * The following defines are used to define the start/end address
 * parameters to the MemAuth command that sets up R2 (aka SSBL).
 * The Start Address parameter includes the region number (2)
 * which is extracted by the define_regionX function to determine
 * the desired region number.
 * The End Address parameter will be updated during compilation
 * with the proper ecode size from ssbl.map if you use the perl
 * script to compile them (make_aegis_cfe.pl).
 */
#define	R2_REGION_ID                    2
#define	R2_START_ADDR                   (0x07008000)

/* ssbl.map (ecode-1) must agree with e2e regression config
*/
#define	R2_END_ADDR                     (R2_START_ADDR + SSBL_TEXT_SIZE - 1)

#define R2_SIG_START_ADDR               (0xe0088500)
#define R2_KEYID_CODERULE_EPOCH         (0x01ff0000)

#define	SECOND_TIER_KEY2_FLASH_ADDR     (0xe0020000) /*(0xe005f000)*/
#define	BFW_BSECK_FIXED_FLASH_ADDR      (0xe0068000)	/* as of Zeus 4.1 */

/* TBD can we put this as part of BFW image as well???
*/
#define	BFW_BSECK_PTR_FLASH_ADDR        (0xe0098000)
#define	BFW_SIGNATURE_FLASH_ADDR        (0xe00c0000)


/* To make things cleaner, I am making the assumption at the least that the
  BFW image is followed by BFW  sig. then followed by the key3, key 3 param
  and key3 sign. This would enable us to use the FSBL BBM. It would be even
  cleaner if we have the same BFW layout with the other parameters.
*/

/*
 * We use the byte @ flash offset 0x404 for passing flags to CFE
 * via flash tweaks.
 *
 *	bit 0: 0 = NoR2	(NOT IMPLEMENTED YET)
 *	       1 = R2 (NOT IMPLEMENTED YET)
 *
 *	bit 1: 0 = Load BFW
 *	       1 = Do NOT load BFW (stay in BBL).
 *
 *	bit 2: 1 = BFW is hash locked
 *	       0 = Verify BFW with signature
 *		   Note that this can be overridden by security mode.
 *		   If security mode indicates hash-locked only, then
 *		   specifying signature verification here will be ignored.
 *
 *	bit 3: 0 = Don't force DRAM scrambling
 *	       1 = force DRAM scrambling (even when the OTP bit is not set)
 *
 *	bit 4: 0 = Enter normal infinite while-loop when ready
 *	       1 = Drop into command line prompt (normally just for special
 *						RNG testing).
 */
#define	FLASH_CFE_PARAM                (0xe0000404)

#define	FLASH_CFE_PARAM_R2_MASK         0x01
#define	FLASH_CFE_PARAM_BBL_MASK        0x02
#define	FLASH_CFE_PARAM_BFWSIG_MASK     0x04
#define	FLASH_CFE_PARAM_DDR_SCRAM_MASK  0x08
#define	FLASH_CFE_PARAM_DO_PROMPT_MASK  0x10

#define	FLASH_CFE_PARAM_UNINITIALIZED	((uint32_t)-1)

extern uint32_t check_flash_cfe_param(uint32_t param_mask);

#else /* S_UNITTEST */

/* ssbl.map (ecode-1) must agree with e2e regression config
*/
#define	R2_START_ADDR                  (0x07008000)
#define R2_KEYID_CODERULE_EPOCH        (0x01000000)

#endif

typedef enum {
	IMAGE_TYPE_BFW = 1,
	IMAGE_TYPE_AVS,
	IMAGE_TYPE_MEMSYS
} image_type;

typedef struct image_info_s {
	image_type	image_type;
	uint8_t		secure_boot;
	uint32_t	addr_offset;
	uint32_t	ctrl_word;
	struct fsbl_flash_partition flash;
} image_info;

uint32_t sec_read_otp_bit(uint32_t field, uint32_t *otp_value);
void handle_boot_err(uint32_t err_code);
void check_return_val(uint32_t val, uint32_t reg,
			uint32_t bit2set, uint32_t err_code);

#if (CFG_ZEUS4_2 || CFG_ZEUS4_1)
uint32_t sec_bfw_load_impl(image_info *info, uint32_t pagelist);
#else
uint32_t bseck_reload(void);
#endif
uint32_t sec_verify_second_tier_key(
	uint32_t second_tier_key,
	uint32_t key0,
	uint32_t address);
uint32_t sec_second_stage_code_load(
	uint32_t code_ptr_addr1,
	uint32_t code_ptr_addr2,
	uint32_t key3_addr,
	uint32_t key_select);

uint32_t sec_scramble_sdram_impl(bool warm_boot);
uint32_t sec_disable_MICH(void);

uint32_t sec_get_bootparam(uint32_t *param);

void     sec_handle_boot_status(int aon_reg_idx, int success);
uint32_t sec_get_aon_boot_status(int aon_reg_idx);
void     sec_set_aon_boot_status(int aon_reg_idx, int success);
int      select_image(image_info *info);

int      get_image_info(image_info *info);
int      set_image_info(image_info *info);

void     sec_print_bsp_version(void);
void     sec_print_bsp_debug_info(void);
void     set_trace(uint32_t val);

#if CFG_ZEUS4_2
uint32_t sec_memsys_ready(void);
#endif

#endif /* __BOOT_DEFINES_H__ */
