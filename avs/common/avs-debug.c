/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2019 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "avs_bsu.h"
#include "avs-debug.h"
#include "avs_dvfs.h"
#include "avs_regs.h"
#include "avs_fw_interface.h"

#include <board.h>
#include <lib_printf.h>
#ifdef DVFS_SUPPORT
#include <pmap.h>
#endif

#include <stdint.h>

/* Local function prototypes */

enum AVS_ERROR avs_disable_avs_cmd(void)
{
	dvfs_send_command(CMD_DISABLE_AVS, 0, 0, 0, 0);

	return dvfs_wait_for_response(0, 0, 0, 0);
}

enum AVS_ERROR avs_enable_avs_cmd(void)
{
	dvfs_send_command(CMD_ENABLE_AVS, 0, 0, 0, 0);

	return dvfs_wait_for_response(0, 0, 0, 0);
}

enum AVS_ERROR avs_enter_s2_cmd(void)
{
	dvfs_send_command(CMD_STB_ENTER_S2, 0, 0, 0, 0);

	return dvfs_wait_for_response(0, 0, 0, 0);
}

enum AVS_ERROR avs_exit_s2_cmd(void)
{
	dvfs_send_command(CMD_STB_EXIT_S2, 0, 0, 0, 0);

	return dvfs_wait_for_response(0, 0, 0, 0);
}

enum AVS_ERROR avs_freeze_dac_cmd(void)
{
	dvfs_send_command(CMD_FREEZE_DAC, 0, 0, 0, 0);

	return dvfs_wait_for_response(0, 0, 0, 0);
}

enum AVS_ERROR avs_unfreeze_dac_cmd(void)
{
	dvfs_send_command(CMD_UNFREEZE_DAC, 0, 0, 0, 0);

	return dvfs_wait_for_response(0, 0, 0, 0);
}

enum AVS_ERROR avs_set_pmap_cmd(uint32_t mode, uint32_t init_state)
{
	uint32_t p0;
	uint32_t p3;

	p0 = mode;
	p3 = init_state;
	dvfs_send_command(CMD_SET_P_MAP, p0, 0, 0, p3);

	return dvfs_wait_for_response(0, 0, 0, 0);
}

enum AVS_ERROR avs_set_pstate_cmd(uint32_t core_index, uint32_t state,
	uint32_t num_of_clk_regs, uint32_t *addr_list, uint32_t *data_list,
	uint32_t *mask_list, uint32_t mode)
{
	/* Don't send an illegal state */
	if (state > P_STATE_4)
		return AVS_BAD_STATE; /* illegal state */

	dvfs_send_set_pstate(core_index, state, num_of_clk_regs, addr_list,
		data_list, mask_list, mode);
	return dvfs_wait_for_response(0, 0, 0, 0);
}

enum AVS_ERROR avs_get_core_pstate(uint32_t core_index, uint32_t *pstate,
	uint32_t *num_of_cores, uint32_t *num_of_pstates, uint32_t *core_domain)
{
	enum AVS_ERROR ret_code;
	uint32_t p0;
	uint32_t p1;
	uint32_t p2;
	uint32_t p3;

	dvfs_send_command(CMD_GET_P_STATE, core_index, 0, 0, 0);
	ret_code = dvfs_wait_for_response(&p0, &p1, &p2, &p3);

	if (ret_code != AVS_SUCCESS)
		return ret_code;

	if ((p1 & 0xff) != core_index)
		return AVS_BAD_PARAMETER;

	*pstate = p0;
	*num_of_cores = (p1 >> 16) & 0xff;
	*num_of_pstates = (p1 >> 8) & 0xff;
	*core_domain = p2;

	return ret_code;
}

enum AVS_ERROR avs_read_sensor(uint32_t sensor_index, uint32_t *value,
	uint32_t *num_of_sensors)
{
	enum AVS_ERROR ret_code;
	uint32_t p0;
	uint32_t p1;

	dvfs_send_command(CMD_READ_SENSOR, sensor_index, 0, 0, 0);
	ret_code = dvfs_wait_for_response(&p0, &p1, 0, 0);

	if (ret_code != AVS_SUCCESS)
		return ret_code;

	*value = p1;
	*num_of_sensors = (p0 >> 8) & 0xff;

	return ret_code;
}

enum AVS_ERROR avs_read_debug(uint32_t debug_index, uint32_t *value)
{
	enum AVS_ERROR ret_code;
	uint32_t p0;
	uint32_t p1;

	dvfs_send_command(CMD_READ_DEBUG, debug_index, 0, 0, 0);
	ret_code = dvfs_wait_for_response(&p0, &p1, 0, 0);

	if (ret_code != AVS_SUCCESS)
		return ret_code;

	if (p0 != debug_index)
		return AVS_BAD_PARAMETER;

	*value = p1;

	return ret_code;
}

enum AVS_ERROR avs_calc_freq(uint32_t pstate, uint32_t *value)
{
	enum AVS_ERROR ret_code;
	uint32_t p0;
	uint32_t p1;

	dvfs_send_command(CMD_CALC_FREQ, pstate, 0, 0, 0);
	ret_code = dvfs_wait_for_response(&p0, &p1, 0, 0);

	if (ret_code != AVS_SUCCESS)
		return ret_code;

	if (p0 != pstate)
		return AVS_BAD_PARAMETER;

	*value = p1;

	return ret_code;
}

enum AVS_ERROR pmic_get_info(uint32_t *num_pmics, uint32_t *i2c_addr,
	uint32_t *num_gpio, uint32_t *num_regs,
	uint32_t *pmic_id, uint32_t *otp_index)
{
	enum AVS_ERROR ret_code;
	uint32_t p0;
	uint32_t p1;
	uint32_t p2;
	uint32_t p3;

	dvfs_send_command(CMD_GET_PMIC_INFO, 0, 0, 0, 0);
	ret_code = dvfs_wait_for_response(&p0, &p1, &p2, &p3);
	if (ret_code != AVS_SUCCESS) {
		p0 = 0;
		p1 = 0;
		p2 = 0;
		p3 = 0;
	}
	*num_pmics = p0 & 0xff;
	*num_regs = (p0 >> 8) & 0xff;
	*num_gpio = p0 >> 16;
	*i2c_addr = p1;
	*pmic_id = p2;
	*otp_index = p3;

	return ret_code;
}

enum AVS_ERROR pmic_set_config(uint32_t pmic_index, uint32_t over_temp_thresh,
	uint32_t s3_regs_select)
{
	enum AVS_ERROR ret_code;
	uint32_t p0;
	uint32_t p1;
	uint32_t p2;

	p0 = pmic_index;
	p1 = over_temp_thresh;
	p2 = s3_regs_select;
	dvfs_send_command(CMD_SET_PMIC_CONFIG, p0, p1, p2, 0);
	ret_code = dvfs_wait_for_response(0, 0, 0, 0);

	return ret_code;
}

enum AVS_ERROR pmic_get_status(uint32_t pmic_index, uint32_t *die_temp,
	uint32_t *ext_therm_temp, uint32_t *pwr_consump)
{
	enum AVS_ERROR ret_code;
	uint32_t p0;
	uint32_t p1;
	uint32_t p2;
	uint32_t p3;

	p0 = pmic_index;
	dvfs_send_command(CMD_GET_PMIC_STATUS, p0, 0, 0, 0);
	ret_code = dvfs_wait_for_response(0, &p1, &p2, &p3);

	if (ret_code != AVS_SUCCESS)
		return ret_code;

	*die_temp = p1;
	*ext_therm_temp = p2;
	*pwr_consump = p3;

	return ret_code;
}

enum AVS_ERROR pmic_get_reg_info(uint32_t reg_index, uint32_t *nom_volt,
	uint32_t *over_i_thresh)
{
	enum AVS_ERROR ret_code;
	uint32_t p0;
	uint32_t p1;

	p0 = reg_index;
	dvfs_send_command(CMD_GET_PMIC_REG_INFO, p0, 0, 0, 0);
	ret_code = dvfs_wait_for_response(0, &p1, 0, 0);

	if (ret_code != AVS_SUCCESS)
		return ret_code;

	*nom_volt = p1 & 0xffff;
	*over_i_thresh = p1 >> 16;

	return ret_code;
}

enum AVS_ERROR pmic_set_reg_config(uint32_t reg_index, uint32_t reg_volt,
	uint32_t over_i_thresh)
{
	enum AVS_ERROR ret_code;
	uint32_t p0;
	uint32_t p1;

	p0 = reg_index;
	p1 = reg_volt | (over_i_thresh << 16);
	dvfs_send_command(CMD_SET_PMIC_REG_CONFIG, p0, p1, 0, 0);
	ret_code = dvfs_wait_for_response(0, 0, 0, 0);

	return ret_code;
}

enum AVS_ERROR pmic_get_reg_status(uint32_t reg_index, uint32_t *reg_volt,
	uint32_t *reg_current)
{
	enum AVS_ERROR ret_code;
	uint32_t p0;
	uint32_t p1;

	p0 = reg_index;
	dvfs_send_command(CMD_GET_PMIC_REG_STATUS, p0, 0, 0, 0);
	ret_code = dvfs_wait_for_response(0, &p1, 0, 0);

	if (ret_code != AVS_SUCCESS)
		return ret_code;

	*reg_volt = p1 & 0xffff;
	*reg_current = p1 >> 16;

	return ret_code;
}

enum AVS_ERROR pmic_reboot_power_cmd(uint32_t otp_enable)
{
	dvfs_send_command(CMD_REBOOT_POWER, otp_enable, 0, 0, 0);

	return dvfs_wait_for_response(0, 0, 0, 0);
}
