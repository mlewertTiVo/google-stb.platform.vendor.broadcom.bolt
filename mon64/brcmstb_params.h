/******************************************************************************
 * Copyright (C) 2017 Broadcom.  The term "Broadcom" refers to Broadcom Limited and/or its subsidiaries.
 *
 * This program is the proprietary software of Broadcom and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *****************************************************************************/

#ifndef _BRCMSTB_PARAMS_H_
#define _BRCMSTB_PARAMS_H_

#include "mon_params.h"

/* Current brcmstb parameters version */
#define BRCMSTB_PARAMS_VERSION  0x03

/* Brcmstb SOC register group enums */
enum {
	BRCMSTB_RGROUP_BOOTSRAM = 0,
	BRCMSTB_RGROUP_SUN_TOP_CTRL,
	BRCMSTB_RGROUP_HIF_CPUBIUCTRL,
	BRCMSTB_RGROUP_HIF_CONTINUATION,
	BRCMSTB_RGROUP_HIF_CPU_INTR1,
	BRCMSTB_RGROUP_AVS_CPU_DATA_MEM,
	BRCMSTB_RGROUP_AVS_HOST_L2,
	BRCMSTB_RGROUP_AVS_CPU_L2,
	BRCMSTB_RGROUP_SCPU_GLOBALRAM,
	BRCMSTB_RGROUP_SCPU_HOST_INTR2,
	BRCMSTB_RGROUP_CPU_IPI_INTR2,
	BRCMSTB_RGROUP_AON_CTRL,
	BRCMSTB_RGROUP_LAST
};

/* Brcmstb interrupts */
enum {
	BRCMSTB_INTR_AVS_CPU,
	BRCMSTB_INTR_SCPU_CPU,
	BRCMSTB_INTR_LAST
};

/* Brcmstb SOC register group descriptor */
typedef struct brcmstb_rgroup {
	/* Revision is bumped in case the offsets and/or formats
	 * of any relevent registers in the group have changed
	 */
	uint32_t rev;
	uint32_t base;
	uint32_t size;
} brcmstb_rgroup_t;

/*
 * Platform parameters
 */
typedef struct brcmstb_params {
	param_hdr_t hdr;

	/* Brcmstb SOC register group descriptors */
	brcmstb_rgroup_t rgroups[BRCMSTB_RGROUP_LAST];

	/* Brcmstb interrupts */
	uint8_t intrs[BRCMSTB_INTR_LAST];

} brcmstb_params_t;

#endif /* _BRCMSTB_PARAMS_H_ */
