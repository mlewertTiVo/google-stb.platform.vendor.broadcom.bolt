/***************************************************************************
 *     Copyright (c) 2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
***************************************************************************/

#ifndef AVS_H__
#define AVS_H__

#include "avs_fw_interface.h"

/* Results from 'LoadFW' or 'WaitForFW' function: */
#define AVS_TIMEOUT		0	/* firmware failed to return any status */
#define AVS_SUCCESS		1	/* firmware successfully initialized and started */
#define AVS_FAILURE		2	/* firmware started but failed initialization */
#define AVS_DISABLED		4	/* OTP disable etc. */
#define AVS_VERIFY_FAILED	8	/* AVS region verification failed. */
#define AVS_LOADED		16	/* AVS ready to be started */


#ifdef __cplusplus
extern "C" {
#endif

/* outer API to FSBL */
void avs_init(void);
int avs_load(void);
int avs_start(int en, int pmap_id);

void avs_class_init(void);
int avs_common_load(void);
int avs_common_start(int en, int pmap_id);

void avs_warm_start(void);

unsigned int avs_firmware_rev(void);
int avs_check_disabled(void);

void setup_chip_for_avs_uart(void);

#ifdef __cplusplus
}
#endif

#endif /*AVS_H__*/
