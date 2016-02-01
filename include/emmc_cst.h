/***************************************************************************
 *     Copyright (c) 2016-, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#ifndef _CST_H
#define _CST_H


/* eMMC device state */
#define CST_ADDRESS_OUT_OF_RANGE_MASK  0x80000000
#define CST_ADDRESS_MISALIGN_MASK      0x40000000
#define CST_BLOCK_LEN_ERROR_MASK       0x20000000
#define CST_ERASE_SEQ_ERROR_MASK       0x10000000
#define CST_ERASE_PARAM_MASK           0x08000000
#define CST_WP_VIOLATION_MASK          0x04000000
#define CST_DEVICE_IS_LOCKED_MASK      0x02000000
#define CST_LOCK_UNLOCK_FAILED_MASK    0x01000000
#define CST_COM_CRC_ERROR_MASK         0x00800000
#define CST_ILLEGAL_COMMAND_MASK       0x00400000
#define CST_DEVICE_ECC_FAILED_MASK     0x00200000
#define CST_CC_ERROR_MASK              0x00100000
#define CST_ERROR_MASK                 0x00080000
#define CST_CID_CSD_OVERWRITE_MASK     0x00010000
#define CST_WP_ERASE_SKIP_MASK         0x00008000
#define CST_ERASE_RESET_MASK           0x00002000
#define CST_CURRENT_STATE_MASK         0x00001E00
#define CST_READY_FOR_DATA_MASK        0x00000100
#define CST_SWITCH_ERROR_MASK          0x00000080
#define CST_EXCEPTION_EVENT_MASK       0x00000040
#define CST_APP_CMD_MASK               0x00000020
#define CST_ADDRESS_OUT_OF_RANGE_SHIFT 31
#define CST_ADDRESS_MISALIGN_SHIFT     30
#define CST_BLOCK_LEN_ERROR_SHIFT      29
#define CST_ERASE_SEQ_ERROR_SHIFT      28
#define CST_ERASE_PARAM_SHIFT          27
#define CST_WP_VIOLATION_SHIFT         26
#define CST_DEVICE_IS_LOCKED_SHIFT     25
#define CST_LOCK_UNLOCK_FAILED_SHIFT   24
#define CST_COM_CRC_ERROR_SHIFT        23
#define CST_ILLEGAL_COMMAND_SHIFT      22
#define CST_DEVICE_ECC_FAILED_SHIFT    21
#define CST_CC_ERROR_SHIFT             20
#define CST_ERROR_SHIFT                19
#define CST_CID_CSD_OVERWRITE_SHIFT    16
#define CST_WP_ERASE_SKIP_SHIFT        15
#define CST_ERASE_RESET_SHIFT          13
#define CST_CURRENT_STATE_SHIFT         9
#define CST_READY_FOR_DATA_SHIFT        8
#define CST_SWITCH_ERROR_SHIFT          7
#define CST_EXCEPTION_EVENT_SHIFT       6
#define CST_APP_CMD_SHIFT               5
#define CST_STATE_SLP  10
#define CST_STATE_BTST  9
#define CST_STATE_DIS   8
#define CST_STATE_PRG   7
#define CST_STATE_RCV   6
#define CST_STATE_DATA  5
#define CST_STATE_TRAN  4
#define CST_STATE_STBY  3
#define CST_STATE_IDEN  2
#define CST_STATE_REDY  1
#define CST_STATE_IDLE  0

/* Device Status */
#define Idx_CST_ADDRESS_OUT_OF_RANGE 0x80000000
#define Idx_CST_ADDRESS_MISALIGN     0x40000000
#define Idx_CST_BLOCK_LEN_ERROR      0x20000000
#define Idx_CST_ERASE_SEQ_ERROR      0x10000000
#define Idx_CST_ERASE_PARAM          0x08000000
#define Idx_CST_WP_VIOLATION         0x04000000
#define Idx_CST_DEVICE_IS_LOCKED     0x02000000
#define Idx_CST_LOCK_UNLOCK_FAILED   0x01000000
#define Idx_CST_COM_CRC_ERROR        0x00800000
#define Idx_CST_ILLEGAL_COMMAND      0x00400000
#define Idx_CST_DEVICE_ECC_FAILED    0x00200000
#define Idx_CST_CC_ERROR             0x00100000
#define Idx_CST_ERROR                0x00080000
#define Idx_CST_CID_CSD_OVERWRITE    0x00010000
#define Idx_CST_WP_ERASE_SKIP        0x00008000
#define Idx_CST_ERASE_RESET          0x00002000
#define Idx_CST_CURRENT_STATE        0x00001E00
#define Idx_CST_READY_FOR_DATA       0x00000100
#define Idx_CST_SWITCH_ERROR         0x00000080
#define Idx_CST_EXCEPTION_EVENT      0x00000040
#define Idx_CST_APP_CMD              0x00000020

/* Register : Extended CSD */
/* Properties Segment */
#define Idx_ExtCSD_S_CMD_SET                    504
#define Idx_ExtCSD_HPI_FEATURES                 503
#define Idx_ExtCSD_BKOPS_SUPPORT                502
#define Idx_ExtCSD_MAX_PACKED_READS             501
#define Idx_ExtCSD_MAX_PACKED_WRITES            500
#define Idx_ExtCSD_DATA_TAG_SUPPORT             499
#define Idx_ExtCSD_TAG_UNIT_SIZE                498
#define Idx_ExtCSD_TAG_RES_SIZE                 497
#define Idx_ExtCSD_CONTEXT_CAPABILITIES         496
#define Idx_ExtCSD_LARGE_UNIT_SIZE_M1           495
#define Idx_ExtCSD_EXT_SUPPORT                  494
#define Idx_ExtCSD_CACHE_SIZE                   249
#define Idx_ExtCSD_GENERIC_CMD6_TIME            248
#define Idx_ExtCSD_POWER_OFF_LONG_TIME          247
#define Idx_ExtCSD_BKOPS_STATUS                 246
#define Idx_ExtCSD_CORRECTLY_PRG_SECTORS_NUM    242
#define Idx_ExtCSD_INI_TIMEOUT_AP               241
#define Idx_ExtCSD_PWR_CL_DDR_52_360            239
#define Idx_ExtCSD_PWR_CL_DDR_52_195            238
#define Idx_ExtCSD_PWR_CL_200_360               237
#define Idx_ExtCSD_PWR_CL_200_195               236
#define Idx_ExtCSD_MIN_PERF_DDR_W_8_52          235
#define Idx_ExtCSD_MIN_PERF_DDR_R_8_52          234
#define Idx_ExtCSD_TRIM_MULT                    232
#define Idx_ExtCSD_SEC_FEATURE_SUPPORT          231
#define Idx_ExtCSD_SEC_ERASE_MULT               230
#define Idx_ExtCSD_SEC_TRIM_MULT                229
#define Idx_ExtCSD_BOOT_INFO                    228
#define Idx_ExtCSD_BOOT_SIZE_MULT               226
#define Idx_ExtCSD_ACC_SIZE                     225
#define Idx_ExtCSD_HC_ERASE_GRP_SIZE            224
#define Idx_ExtCSD_ERASE_TIMEOUT_MULT           223
#define Idx_ExtCSD_REL_WR_SEC_C                 222
#define Idx_ExtCSD_HC_WP_GRP_SIZE               221
#define Idx_ExtCSD_S_C_VCC                      220
#define Idx_ExtCSD_S_C_VCCQ                     219
#define Idx_ExtCSD_S_A_TIMEOUT                  217
#define Idx_ExtCSD_SEC_COUNT                    212
#define Idx_ExtCSD_MIN_PERF_W_8_52              210
#define Idx_ExtCSD_MIN_PERF_R_8_52              209
#define Idx_ExtCSD_MIN_PERF_W_8_26_4_52         208
#define Idx_ExtCSD_MIN_PERF_R_8_26_4_52         207
#define Idx_ExtCSD_MIN_PERF_W_4_26              206
#define Idx_ExtCSD_MIN_PERF_R_4_26              205
#define Idx_ExtCSD_PWR_CL_26_360                203
#define Idx_ExtCSD_PWR_CL_52_360                202
#define Idx_ExtCSD_PWR_CL_26_195                201
#define Idx_ExtCSD_PWR_CL_52_195                200
#define Idx_ExtCSD_PARTITION_SWITCH_TIME        199
#define Idx_ExtCSD_OUT_OF_INTERRUPT_TIME        198
#define Idx_ExtCSD_DRIVER_STRENGTH              197
#define Idx_ExtCSD_DEVICE_TYPE                  196
#define Idx_ExtCSD_CSD_STRUCTURE                194
#define Idx_ExtCSD_EXT_CSD_REV                  192

/* Modes Segment */
#define Idx_ExtCSD_CMD_SET                      191
#define Idx_ExtCSD_CMD_SET_REV                  189
#define Idx_ExtCSD_POWER_CLASS                  187
#define Idx_ExtCSD_HS_TIMING                    185
#define Idx_ExtCSD_BUS_WIDTH                    183
#define Idx_ExtCSD_ERASED_MEM_CONT              181
#define Idx_ExtCSD_PARTITION_CONFIG             179
#define   PCFG_BOOT_ACK				0x40
#define   PCFG_BOOT_PARTITION_ENABLE_MASK	0x38
#define   PCFG_BOOT_PARTITION_ENABLE_NONE	0x00
#define   PCFG_BOOT_PARTITION_ENABLE_BOOT1	0x08
#define   PCFG_BOOT_PARTITION_ENABLE_BOOT2	0x10
#define   PCFG_BOOT_PARTITION_ENABLE_DATA	0x38
#define   PCFG_PARTITION_ACCESS_MASK		0x07
#define   PCFG_PARTITION_ACCESS_DATA		0x00
#define   PCFG_PARTITION_ACCESS_BOOT1		0x01
#define   PCFG_PARTITION_ACCESS_BOOT2		0x02
#define   PCFG_PARTITION_ACCESS_RPMB		0x03
#define   PCFG_PARTITION_ACCESS_GP0		0x04
#define   PCFG_PARTITION_ACCESS_GP1		0x05
#define   PCFG_PARTITION_ACCESS_GP2		0x06
#define   PCFG_PARTITION_ACCESS_GP3		0x07
#define Idx_ExtCSD_BOOT_CONFIG_PROT             178
#define Idx_ExtCSD_BOOT_BUS_CONDITIONS          177
#define Idx_ExtCSD_ERASE_GROUP_DEF              175
#define Idx_ExtCSD_BOOT_WP_STATUS               174
#define Idx_ExtCSD_BOOT_WP                      173
#define Idx_ExtCSD_USER_WP                      171
#define Idx_ExtCSD_FW_CONFIG                    169
#define Idx_ExtCSD_RPMB_SIZE_MULT               168
#define Idx_ExtCSD_WR_REL_SET                   167
#define Idx_ExtCSD_WR_REL_PARAM                 166
#define Idx_ExtCSD_SANITIZE_START               165
#define Idx_ExtCSD_BKOPS_START                  164
#define Idx_ExtCSD_BKOPS_EN                     163
#define Idx_ExtCSD_RST_n_FUNCTION               162
#define Idx_ExtCSD_HPI_MGMT                     161
#define Idx_ExtCSD_PARTITIONING_SUPPORT         160
#define Idx_ExtCSD_MAX_ENH_SIZE_MULT            157
#define Idx_ExtCSD_PARTITIONS_ATTRIBUTE         156
#define Idx_ExtCSD_PARTITION_SETTING_COMPLETED  155
#define Idx_ExtCSD_GP_SIZE_MULT                 143
#define Idx_ExtCSD_ENH_SIZE_MULT                140
#define Idx_ExtCSD_ENH_START_ADDR               136
#define Idx_ExtCSD_SEC_BAD_BLK_MGMNT            134
#define Idx_ExtCSD_TCASE_SUPPORT                132
#define Idx_ExtCSD_PERIODIC_WAKEUP              131
#define Idx_ExtCSD_PROGRAM_CID_CSD_DDR_SUPPORT  130
#define Idx_ExtCSD_VENDOR_SPECIFIC_FIELD         64
#define Idx_ExtCSD_NATIVE_SECTOR_SIZE            63
#define Idx_ExtCSD_USE_NATIVE_SECTOR             62
#define Idx_ExtCSD_DATA_SECTOR_SIZE              61
#define Idx_ExtCSD_INI_TIMEOUT_EMU               60
#define Idx_ExtCSD_CLASS_6_CTRL                  59
#define Idx_ExtCSD_DYNCAP_NEEDED                 58
#define Idx_ExtCSD_EXCEPTION_EVENTS_CTRL         56
#define Idx_ExtCSD_EXCEPTION_EVENTS_STATUS       54
#define Idx_ExtCSD_EXT_PARTITIONS_ATTRIBUTE      52
#define Idx_ExtCSD_CONTEXT_CONF                  37
#define Idx_ExtCSD_PACKED_COMMAND_STATUS         36
#define Idx_ExtCSD_PACKED_FAILURE_INDEX          35
#define Idx_ExtCSD_POWER_OFF_NOTIFICATION        34
#define Idx_ExtCSD_CACHE_CTRL                    33
#define Idx_ExtCSD_FLUSH_CACHE                   32

#define Idx_ExtCSD_ACC_CMD  0	/* EXT_CSD Access mode : Command Set */
#define Idx_ExtCSD_ACC_SET  1	/* EXT_CSD Access mode : Set Bits */
#define Idx_ExtCSD_ACC_CLR  2	/* EXT_CSD Access mode : Clear Bits */
#define Idx_ExtCSD_ACC_WRB  3	/* EXT_CSD Access mode : Write Byte */

/* OCR */
#define OCR_READY	0x80000000
#define OCR_SECTOR_MODE 0x40000000
#define OCR_VDD_33_34   0x00200000
#define OCR_VDD_17_195  0x00000080

#endif /* _CST_H */
