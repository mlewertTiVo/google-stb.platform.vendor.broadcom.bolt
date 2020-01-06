/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description:
 *  This provides the methods used to pass DVFS parameters to the AVS firmware
 *
 ***************************************************************************/

#ifndef _AVS_DEBUG_H_
#define _AVS_DEBUG_H_

#include "avs_dvfs.h"

#include <stdint.h>

enum AVS_ERROR avs_disable_avs_cmd(void);

enum AVS_ERROR avs_enable_avs_cmd(void);

enum AVS_ERROR avs_enter_s2_cmd(void);

enum AVS_ERROR avs_exit_s2_cmd(void);

enum AVS_ERROR avs_freeze_dac_cmd(void);

enum AVS_ERROR avs_unfreeze_dac_cmd(void);

enum AVS_ERROR avs_set_pmap_cmd(uint32_t mode, uint32_t init_state);

enum AVS_ERROR avs_set_pstate_cmd(uint32_t core_index, uint32_t state,
	uint32_t num_of_clk_regs, uint32_t *addr_list, uint32_t *data_list,
	uint32_t *mask_list, uint32_t mode);

enum AVS_ERROR avs_get_core_pstate(uint32_t core_index, uint32_t *pstate,
	uint32_t *num_of_cores, uint32_t *num_of_pstates,
	uint32_t *core_domain);

enum AVS_ERROR avs_read_sensor(uint32_t sensor_index, uint32_t *value,
	uint32_t *num_of_sensors);

enum AVS_ERROR avs_read_debug(uint32_t debug_index, uint32_t *value);

enum AVS_ERROR avs_calc_freq(uint32_t pstate, uint32_t *value);

enum AVS_ERROR pmic_get_info(uint32_t *num_pmics, uint32_t *i2c_addr,
	uint32_t *num_gpio, uint32_t *num_regs,
	uint32_t *pmic_id, uint32_t *otp_index);

enum AVS_ERROR pmic_set_config(uint32_t pmic_index, uint32_t over_temp_thresh,
	uint32_t s3_regs_select);

enum AVS_ERROR pmic_get_status(uint32_t pmic_index, uint32_t *die_temp,
	uint32_t *ext_therm_temp, uint32_t *pwr_consump);

enum AVS_ERROR pmic_get_reg_info(uint32_t reg_index, uint32_t *nom_volt,
	uint32_t *over_i_thresh);

enum AVS_ERROR pmic_set_reg_config(uint32_t reg_index, uint32_t reg_volt,
	uint32_t over_i_thresh);

enum AVS_ERROR pmic_get_reg_status(uint32_t reg_index, uint32_t *reg_volt,
	uint32_t *reg_current);

enum AVS_ERROR pmic_reboot_power_cmd(uint32_t otp_enable);

#endif/* _AVS_DEBUG_H_ */


