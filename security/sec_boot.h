/***************************************************************************
 *     Copyright (c) 2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __SECURE_BOOT_H__
#define __SECURE_BOOT_H__

#include <boot_defines.h>

#define SRAM_ADDR_OF(x)							(SRAM_ADDR | x)

#define PARAM_CTRL_WORD_KEY0_MASK               (0x08000000)
#define PARAM_CTRL_WORD_FORCE_SIGNED_BFW_MASK	(0x04000000)

#if CFG_ZEUS4_2
#define PARAM_SWIZZLE                           (0x00000050)
#define PARAM_2ND_TIER_KEY1                     (0x00000340)

/* boot param for second image  */
#define PARAM_2ND_AVS_CODE_SIZE                 (0x00000080)
#define PARAM_2ND_AVS_DATA_SIZE                 (0x00000084)
#define PARAM_2ND_MEMSYSFW_SIZE                 (0x00000094)

/* boot param for first image  */
#define PARAM_1ST_AVS_CODE_SIZE                 (0x000000B0)
#define PARAM_1ST_AVS_DATA_SIZE                 (0x000000B4)
#define PARAM_1ST_MEMSYSFW_SIZE                 (0x000000C4)
#else
#define PARAM_SWIZZLE                           (0x00000410)
#define PARAM_2ND_TIER_KEY1                     (0x00000640)

#define PARAM_2ND_AVS_CODE_SIZE                 (0x00000B88)
#define PARAM_2ND_AVS_DATA_SIZE                 (0x00000B8C)

/* boot param for first image  */
#define PARAM_1ST_AVS_CODE_SIZE                 (0x00000BB8)
#define PARAM_1ST_AVS_DATA_SIZE                 (0x00000BBC)
#endif

#define PARAM_CUSTOMER_INIT_ADDR1               (0x000000F8)
#define PARAM_CUSTOMER_INIT_DATA1               (0x000000FC)

#define PARAM_1ST_MEMSYSFW_HASH                 (0x00000128)
#define PARAM_2ND_MEMSYSFW_HASH                 (0x0000012C)

/* Actual definition is not preceded by PARAM_ */
#define SECOND_TIER_KEY_SIZE                    (256+20+256)
#define AVS_IMEM_SIZE                           0x3000
#define AVS_DMEM_SIZE                           0xc00

/* use scrach area instead of buffer to save fsbl size */
#define SCRATCH_MEM_LOCATION                    (SSBL_BOARDINFO - 4096)

/* struct for AVS/Memsys/SSBL region info */
struct region_info {
	uint32_t region_num;
	uint32_t key_id;
	uint32_t code_rule;
	uint32_t market_id;
	uint32_t market_id_mask;
	uint32_t epoch;
	uint32_t signing_version;
	uint32_t partial_hash;
	uint32_t cpu1_starting_addr;
	uint32_t cpu1_ending_addr;
};

uint32_t sec_define_regionX_boot(uint32_t region_start_address,
			uint32_t region_end_address,
			uint32_t sig_start_addr,
			uint32_t *region_info);
uint32_t sec_enable_regions_boot(void);
uint32_t sec_wait_for_verify_regionX_boot(uint32_t region_num);
uint32_t sec_decrypt_ssbl(uint32_t ssbl_size);

uint32_t sec_verify_avs_impl(image_info *info);
uint32_t sec_verify_memsys_impl(image_info *info);
uint32_t sec_reverify_region(uint32_t region, int *secure_otp);
uint32_t sec_memsys_region_disable_impl(void);
uint32_t sec_disable_region(uint32_t region_num);
uint32_t sec_wait_for_disable_region(uint32_t region_num);
uint32_t sec_verify_ssbl_impl(void);
uint32_t sec_lock_memc_impl(int memc);


#endif /* __SECURE_BOOT_H__ */
