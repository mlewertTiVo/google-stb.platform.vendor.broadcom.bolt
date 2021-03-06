/***************************************************************************
 *     Copyright (c) 2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
***************************************************************************/

#ifndef AVS_TEMP_RESET_H__
#define AVS_TEMP_RESET_H__

#ifdef __cplusplus
extern "C" {
#endif


/* AVS Monitors, not applicable to all devices
 */
#define STB_DEVICE 0
#define DCD_DEVICE 1


/* Use to enable the over temperature reset
 *
 * device: use 0 for STB and 1 for DCS(CM) island
 *     en: enable (1) or disable (0) reste on overtemp
 */
void avs_set_temp_threshold(unsigned int device, int en);


/* Use to check whether current reset was due to over temperature condition
 *
 *   quiet: set to true to disable printing message on over-temp
 *
 * returns: true if we reset due to an overtemp condition.
 */
bool avs_check_temp_reset(bool quiet);


#ifdef __cplusplus
}
#endif

#endif /* AVS_TEMP_RESET_H__ */

