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

#ifndef AVS_BSU_H__
#define AVS_BSU_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Prototypes */

/* Init AVS features for use in BOLT SSBL.
 */
void avs_ssbl_init(void);

/* Print the current AVS status.
 */
void avs_info_print(void);


/* This function provides the BSU environment with an ability to get the
 * current voltage and temperature.  It also indicates whether the firmware
 * is running or not.  This could be important to know whether the voltage
 * is a converged voltage.
 *
 *           voltage [out] current voltage in mV
 *       temperature [out] current temp in 1000th
 * firmware_runnings [out] is firmware running
 *
 */
int avs_get_data(unsigned int *voltage, signed int *temperature,
		bool *firmware_runnings);


/* This function chooses if an overtemp event will cause a hardware reboot.
 *                en [in] 1=enable reboot on overtemp, 0=disable reboot.
 */
void avs_do_reset_on_overtemp(int en);

#ifdef __cplusplus
}
#endif

#endif /*AVS_BSU_H__*/
