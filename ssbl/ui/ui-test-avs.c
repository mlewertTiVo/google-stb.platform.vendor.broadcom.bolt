/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2019 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <avs-debug.h>
#include <console.h>
#include <devfuncs.h>
#include <device.h>
#include <error.h>
#include <iocb.h>
#include <ioctl.h>
#include <lib_malloc.h>
#include <lib_printf.h>
#include <lib_queue.h>
#include <lib_string.h>
#include <lib_types.h>
#include <timer.h>
#include <ui_command.h>
#include <ui_init.h>

enum {
	AVS_DEBUG_VAL_VOLT0	=  2,
	AVS_DEBUG_VAL_TEMP0	=  3,
	AVS_DEBUG_VAL_PV0	=  4,
	AVS_DEBUG_VAL_MV0	=  5,
	AVS_DEBUG_VAL_REVISION	= 10,
	AVS_DEBUG_VAL_STATE	= 11,
	AVS_DEBUG_VAL_HEARTBEAT	= 12,
	AVS_DEBUG_VAL_MAGIC	= 13,
	AVS_DEBUG_VAL_SIGMA	= 14,
	AVS_DEBUG_VAL_VOLT1	= 16,
	AVS_DEBUG_VAL_TEMP1	= 17,
	AVS_DEBUG_VAL_PV1	= 18,
	AVS_DEBUG_VAL_MV1	= 19,
	AVS_DEBUG_VAL_FREQ	= 20,
	AVS_DEBUG_VAL_SIZE
};

static int ui_cmd_avs_disable_avs(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_avs_enable_avs(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_avs_enter_s2(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_avs_exit_s2(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_avs_freeze_dac(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_avs_unfreeze_dac(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_avs_set_pmap(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_avs_set_pstate(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_avs_get_pstate(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_avs_read_sensor(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_avs_read_debug(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_avs_read_all(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_avs_calc_freq(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_avs_test_pstate(ui_cmdline_t *cmd, int argc, char *argv[]);

static char *help_set_pstate_description =
"avs set_pstate <cpu_pstate>\n"
"avs set_pstate <core_index> <core_pstate>\n"
"avs set_pstate <core_index> <core_pstate> <addr0 data0 mask0> ...\n"
"avs set_pstate <core_index> <core_pstate> <mode> <addr0 data0 mask0> ...\n";

static char *help_get_pstate_description =
"avs get_pstate\n"
"avs get_pstate <core_index>\n";

static char *core_domain_string[] = {"STB", "CPU", "***"};

int ui_init_avstestcmds(void)
{
	cmd_addcmd("avs disable",
		   ui_cmd_avs_disable_avs,
		   NULL,
		   "Send DISABLE_AVS",
		   "avs disable",
		   "");
	cmd_addcmd("avs enable",
		   ui_cmd_avs_enable_avs,
		   NULL,
		   "Send ENABLE_AVS",
		   "avs enable",
		   "");
	cmd_addcmd("avs enter_s2",
		   ui_cmd_avs_enter_s2,
		   NULL,
		   "Send ENTER_S2",
		   "avs enter_s2",
		   "");
	cmd_addcmd("avs exit_s2",
		   ui_cmd_avs_exit_s2,
		   NULL,
		   "Send EXIT_S2",
		   "avs exit_s2",
		   "");
	cmd_addcmd("avs freeze",
		   ui_cmd_avs_freeze_dac,
		   NULL,
		   "Send FREEZE_DAC",
		   "avs freeze",
		   "");
	cmd_addcmd("avs unfreeze",
		   ui_cmd_avs_unfreeze_dac,
		   NULL,
		   "Send UNFREEZE_DAC",
		   "avs unfreeze",
		   "");
	cmd_addcmd("avs set_map",
		   ui_cmd_avs_set_pmap,
		   NULL,
		   "Send SET_PMAP to AVS",
	"avs set_pmap <mode> <initial pstate> ; 0=AVS, 1=DFS, 2=DVS, 3=DVFS",
		   "");
	cmd_addcmd("avs set_pstate",
		   ui_cmd_avs_set_pstate,
		   NULL,
		   "Send SET_PSTATE to AVS",
		   help_set_pstate_description,
		   "");
	cmd_addcmd("avs get_pstate",
		   ui_cmd_avs_get_pstate,
		   NULL,
		   "Send GET_PSTATE to AVS",
		   help_get_pstate_description,
		   "");
	cmd_addcmd("avs read_sensor",
		   ui_cmd_avs_read_sensor,
		   NULL,
		   "Send READ_SENSOR to AVS",
		   "avs read_sensor <sensor_index>",
		   "");
	cmd_addcmd("avs read_debug",
		   ui_cmd_avs_read_debug,
		   NULL,
		   "Send READ_DEBUG to AVS",
		   "avs read_debug <debug_index>",
		   "");
	cmd_addcmd("avs read_all",
		   ui_cmd_avs_read_all,
		   NULL,
		   "Read PMAP, PSTATE, and sensor info",
		   "avs read_all",
		   "");
	cmd_addcmd("avs calc_freq",
		   ui_cmd_avs_calc_freq,
		   NULL,
		   "Return the calculated cpu frequency",
		   "avs calc_freq",
		   "");
	cmd_addcmd("avs test_pstate",
		   ui_cmd_avs_test_pstate,
		   NULL,
		   "run it multiple times",
		   "avs test_pstate",
		   "");

	return 0;
}

static int ui_cmd_avs_disable_avs(ui_cmdline_t *cmd, int argc, char *argv[])
{
	enum AVS_ERROR ret_code;

	ret_code = avs_disable_avs_cmd();

	if (ret_code != AVS_SUCCESS)
		xprintf("Error. ret_code=0x%X\n", ret_code);

	return ret_code;
}

static int ui_cmd_avs_enable_avs(ui_cmdline_t *cmd, int argc, char *argv[])
{
	enum AVS_ERROR ret_code;

	ret_code = avs_enable_avs_cmd();

	if (ret_code != AVS_SUCCESS)
		xprintf("Error. ret_code=0x%X\n", ret_code);

	return ret_code;
}

static int ui_cmd_avs_enter_s2(ui_cmdline_t *cmd, int argc, char *argv[])
{
	enum AVS_ERROR ret_code;

	ret_code = avs_enter_s2_cmd();

	if (ret_code != AVS_SUCCESS)
		xprintf("Error. ret_code=0x%X\n", ret_code);

	return ret_code;
}

static int ui_cmd_avs_exit_s2(ui_cmdline_t *cmd, int argc, char *argv[])
{
	enum AVS_ERROR ret_code;

	ret_code = avs_exit_s2_cmd();

	if (ret_code != AVS_SUCCESS)
		xprintf("Error. ret_code=0x%X\n", ret_code);

	return ret_code;
}

static int ui_cmd_avs_freeze_dac(ui_cmdline_t *cmd, int argc, char *argv[])
{
	enum AVS_ERROR ret_code;

	ret_code = avs_freeze_dac_cmd();

	if (ret_code != AVS_SUCCESS)
		xprintf("Error. ret_code=0x%X\n", ret_code);

	return ret_code;
}

static int ui_cmd_avs_unfreeze_dac(ui_cmdline_t *cmd, int argc, char *argv[])
{
	enum AVS_ERROR ret_code;

	ret_code = avs_unfreeze_dac_cmd();

	if (ret_code != AVS_SUCCESS)
		xprintf("Error. ret_code=0x%X\n", ret_code);

	return ret_code;
}

static int ui_cmd_avs_set_pmap(ui_cmdline_t *cmd, int argc, char *argv[])
{
	enum AVS_ERROR ret_code;
	uint32_t pmap_value;
	uint32_t init_state;

	if (argc != 2) {
		xprintf("require 2 parameters: mode, init_state\n");
		return 0;
	}
	pmap_value = atoi(cmd_getarg(cmd, 0));
	init_state = atoi(cmd_getarg(cmd, 1));

	ret_code = avs_set_pmap_cmd(pmap_value, init_state);

	if (ret_code != AVS_SUCCESS)
		xprintf("Error. ret_code=0x%X\n", ret_code);

	return ret_code;
}

static int is_domain_value(uint32_t *core_domain, char *input_string)
{
	if ((strcmpi(input_string, "STB") == 0) ||
	    (strcmpi(input_string, "S")   == 0)) {
		*core_domain = 0;
		return 1;
	}
	if ((strcmpi(input_string, "CPU") == 0) ||
	    (strcmpi(input_string, "C")   == 0)) {
		*core_domain = 1;
		return 1;
	}
	return 0;
}

static void handle_set_pstate_2args(ui_cmdline_t *cmd, int argc, char *argv[])
{
	enum AVS_ERROR ret_code;
	uint32_t new_state;
	uint32_t core_index;
	uint32_t pstate;
	uint32_t num_of_cores;
	uint32_t num_of_pstates;
	uint32_t selected_domain;
	uint32_t core_domain;

	if (is_domain_value(&selected_domain, cmd_getarg(cmd, 0)) == 1) {
		new_state = atoi(cmd_getarg(cmd, 1));
		ret_code = avs_get_core_pstate(0, &pstate,
			&num_of_cores, &num_of_pstates, &core_domain);

		if (ret_code != AVS_SUCCESS) {
			xprintf(
		"Error. Could not get num_of_core. ret_code=0x%X\n",
				ret_code);
			return;
		}
		for (core_index = 0; core_index < num_of_cores;
				core_index++) {
			uint32_t core_num_of_cores;
			uint32_t core_num_of_pstates;

			ret_code = avs_get_core_pstate(core_index,
				&pstate, &core_num_of_cores,
				&core_num_of_pstates, &core_domain);
			if (ret_code != AVS_SUCCESS) {
				xprintf(
"Error. Could not get information on core_index=%d. ret_code=0x%X\n",
					core_index, ret_code);
				return;
			}
			if (core_domain == selected_domain) {
				ret_code = avs_set_pstate_cmd(
					core_index, new_state, 0, 0, 0, 0, 0);
			}
		}
	}
	else {
		core_index = atoi(cmd_getarg(cmd, 0));
		new_state = atoi(cmd_getarg(cmd, 1));
		ret_code = avs_set_pstate_cmd(core_index, new_state, 0, 0, 0,
				0, 0);
		if (ret_code != AVS_SUCCESS)
			xprintf("Error. ret_code=0x%X\n", ret_code);
	}
}

#define NUM_OF_CLK_REGS	3
static int ui_cmd_avs_set_pstate(ui_cmdline_t *cmd, int argc, char *argv[])
{
	enum AVS_ERROR ret_code;

	int arg_cnt = 0;
	uint32_t new_state = 0;
	uint32_t core_index = 0;

	uint32_t mode = 0;
	uint32_t addr_list[NUM_OF_CLK_REGS];
	uint32_t data_list[NUM_OF_CLK_REGS];
	uint32_t mask_list[NUM_OF_CLK_REGS];
	int num_of_clk_regs = 0;

	if (argc == 0) {
		xprintf("Error. Missing parameters");
		return 0;
	}
	else if (argc == 2) {
		handle_set_pstate_2args(cmd, argc, argv);
		return 0;
	}

	if (argc == 1) {
		new_state = atoi(cmd_getarg(cmd, arg_cnt++));
	}
	else if (argc > 1) {
		core_index = atoi(cmd_getarg(cmd, arg_cnt++));
		new_state = atoi(cmd_getarg(cmd, arg_cnt++));
	}

	while ((num_of_clk_regs < NUM_OF_CLK_REGS) && (argc >= (arg_cnt+3))) {
		addr_list[num_of_clk_regs] = lib_atoq(cmd_getarg(cmd, arg_cnt++));
		data_list[num_of_clk_regs] = lib_atoq(cmd_getarg(cmd, arg_cnt++));
		mask_list[num_of_clk_regs] = lib_atoq(cmd_getarg(cmd, arg_cnt++));
		num_of_clk_regs++;
	}

	if (argc == (arg_cnt+1)) {
		mode = lib_atoq(cmd_getarg(cmd, arg_cnt++));
	}

	ret_code = avs_set_pstate_cmd(core_index, new_state, num_of_clk_regs,
		addr_list, data_list, mask_list, mode);

	if (ret_code != AVS_SUCCESS)
		xprintf("Error. ret_code=0x%X\n", ret_code);

	return 0;
}

static int ui_cmd_avs_get_pstate(ui_cmdline_t *cmd, int argc, char *argv[])
{
	enum AVS_ERROR ret_code;
	uint32_t pstate;
	uint32_t num_of_cores;
	uint32_t num_of_pstates;
	uint32_t core_domain;
	uint32_t selected_domain = 0xff; /* all domains are selected */
	int selected_index;

	if (argc == 0) {
		selected_domain = 0xff;
		selected_index = -1;	/* -1, no core_index selected */
	} else if (argc == 1) {
		if (is_domain_value(&selected_domain, cmd_getarg(cmd, 0))
			== 1) {
			selected_index = -1;	/* -1, no core_index selected */
		} else {
			selected_domain = 0xff;
			selected_index = atoi(cmd_getarg(cmd, 0));
		}
	} else {
		xprintf(
"Error. Missing core or domain selection (0-n, CPU, or STB). None will return all\n"
			);
		return 0;
	}

	ret_code = avs_get_core_pstate(0, &pstate, &num_of_cores,
		&num_of_pstates, &core_domain);
	if (ret_code != AVS_SUCCESS) {
		xprintf("Error. Could not get num_of_cores. ret_code=0x%X\n",
			ret_code);
		return 0;
	}
	xprintf("num_of_cores:   %d\n\n", num_of_cores);

	xprintf("index   domain  pstate  #_pstates\n");
	xprintf("======  ======  ======  =========\n");
	int start_index;
	int last_index;
	int core_index;

	if (selected_index == -1) { /* -1, signifies no core_index selected */
		start_index = 0;
		last_index = num_of_cores;
	} else {
		start_index = selected_index;
		last_index = selected_index + 1;
	}
	for (core_index = start_index; core_index < last_index; core_index++) {
#if defined(CONFIG_BCM7255)
		if (core_index == 1)
			continue;
#endif
		ret_code = avs_get_core_pstate(core_index, &pstate,
			&num_of_cores, &num_of_pstates, &core_domain);
		if (ret_code != AVS_SUCCESS) {
			xprintf(
	"Error. Could not get core_domain for index=%d. ret_code=0x%X\n",
				core_index, ret_code);
			return 0;
		}

		if ((selected_domain != 0xff) &&
			(selected_domain != core_domain))
			continue;

		xprintf("%6d  %6s  %6d  %6d\n", core_index,
			core_domain_string[core_domain > 1 ? 2 : core_domain], pstate,
			num_of_pstates);
	}

	return 0;
}

static int ui_cmd_avs_read_sensor(ui_cmdline_t *cmd, int argc, char *argv[])
{
	enum AVS_ERROR ret_code;
	int index;
	uint32_t value;
	uint32_t num_of_sensors;

	if (argc == 0) {
		xprintf("missing sensor index\n");
		return 0;
	}
	index = atoi(cmd_getarg(cmd, 0));
	ret_code = avs_read_sensor(index, &value, &num_of_sensors);
	if (ret_code != AVS_SUCCESS) {
		xprintf("Error. ret_code=0x%X\n", ret_code);
		return 0;
	}
	switch (index) {
	case 0:
		xprintf("temperature: %d\n", value);
		break;
	default:
		xprintf("voltage: %d\ncurrent: %d\n", value & 0xffff,
			value >> 16);
		break;
	}
	xprintf("num_of_sensors: %d\n", num_of_sensors);
	return 0;
}

static int ui_cmd_avs_read_debug(ui_cmdline_t *cmd, int argc, char *argv[])
{
	enum AVS_ERROR ret_code;
	int index;
	uint32_t value;

	if (argc == 0) {
		xprintf("missing debug index\n");
		return 0;
	}
	index = atoi(cmd_getarg(cmd, 0));
	ret_code = avs_read_debug(index, &value);
	if (ret_code != AVS_SUCCESS) {
		xprintf("Error. ret_code=0x%X\n", ret_code);
		return 0;
	}
	xprintf("value: 0x%08X (%d)\n", value, value);

	return 0;
}

static int ui_cmd_avs_read_all(ui_cmdline_t *cmd, int argc, char *argv[])
{
	enum AVS_ERROR ret_code;
	unsigned int core_index;
	uint32_t pstate;
	uint32_t num_of_cores;
	uint32_t num_of_pstates;
	uint32_t core_domain;
	uint32_t num_of_sensors;
	uint32_t value;

	ret_code = avs_get_core_pstate(0, &pstate, &num_of_cores,
		&num_of_pstates, &core_domain);
	if (ret_code != AVS_SUCCESS) {
		xprintf("Error. Could not get num_of_cores. ret_code=0x%X\n",
			ret_code);
		return 0;
	}
#ifdef avs_get_current_pmap
	xprintf("\nCurrent PMAP index: %d\n\n", avs_get_current_pmap());
#endif
	xprintf("number of cores: %d\n", num_of_cores);
	xprintf("index   domain  pstate  #_pstates\n");
	xprintf("======  ======  ======  =========\n");
	for (core_index = 0 ; core_index < num_of_cores ; core_index++) {
#if defined(CONFIG_BCM7255)
		if (core_index == 1)
			continue;
#endif
		ret_code = avs_get_core_pstate(core_index, &pstate,
			&num_of_cores, &num_of_pstates, &core_domain);
		if (ret_code != AVS_SUCCESS) {
			xprintf(
	"Error. Could not get information on core_index=%d. ret_code=0x%X\n",
				core_index, ret_code);
			return 0;
		}
		xprintf("%6d  %6s  %6d  %6d\n", core_index,
			core_domain_string[core_domain > 1 ? 2 : core_domain], pstate,
			num_of_pstates);
	}
	xprintf("\n");

	ret_code = avs_read_sensor(0, &value, &num_of_sensors);
	if (ret_code != AVS_SUCCESS) {
		xprintf("Error. ret_code=0x%X\n", ret_code);
		return 0;
	}
	xprintf("temperature: %6d\n", value);
	ret_code = avs_read_sensor(1, &value, &num_of_sensors);
	if (ret_code != AVS_SUCCESS) {
		xprintf("Error. ret_code=0x%X\n", ret_code);
		return 0;
	}
	xprintf("island 0:    %6d  current: %6d\n", value & 0xffff,
		value >> 16);
	if (num_of_sensors > 2) {
		ret_code = avs_read_sensor(2, &value, &num_of_sensors);
		if (ret_code != AVS_SUCCESS) {
			xprintf("Error. ret_code=0x%X\n", ret_code);
			return 0;
		}
		xprintf("island 1:    %6d  current: %6d\n", value & 0xffff,
			value >> 16);
	}
	xprintf("\n");

	ret_code = avs_read_debug(AVS_DEBUG_VAL_PV0, &value);
	if (ret_code != AVS_SUCCESS) {
		xprintf("Error. ret_code=0x%X\n", ret_code);
		return 0;
	}
	xprintf("PV0: 0x%08X (%d)\n", value, value);
	ret_code = avs_read_debug(AVS_DEBUG_VAL_MV0, &value);
	if (ret_code != AVS_SUCCESS) {
		xprintf("Error. ret_code=0x%X\n", ret_code);
		return 0;
	}
	xprintf("MV0: 0x%08X (%d)\n", value, value);
	ret_code = avs_read_debug(AVS_DEBUG_VAL_REVISION, &value);
	if (ret_code != AVS_SUCCESS) {
		xprintf("Error. ret_code=0x%X\n", ret_code);
		return 0;
	}
	xprintf("Revision: 0x%08X\n", value);
	xprintf("\n");

	return 0;
}

static int ui_cmd_avs_calc_freq(ui_cmdline_t *cmd, int argc, char *argv[])
{
	enum AVS_ERROR ret_code;
	int pstate;
	uint32_t value;

	if (argc == 0)
		pstate = 0;
	else
		pstate = atoi(cmd_getarg(cmd, 0));

	ret_code = avs_calc_freq(pstate, &value);
	if (ret_code != AVS_SUCCESS) {
		xprintf("Error. ret_code=0x%X\n", ret_code);
		return 0;
	}
	xprintf("frequency: %d\n", value);

	return 0;
}

static int ui_cmd_avs_test_pstate(ui_cmdline_t *cmd, int argc, char *argv[])
{
	enum AVS_ERROR ret_code;
	int pstate0;
	int pstate1;

	if (argc == 0) {
		pstate0 = 4;
		pstate1 = 0;
	} else if (argc == 2) {
		pstate0 = atoi(cmd_getarg(cmd, 0));
		pstate1 = atoi(cmd_getarg(cmd, 1));
	} else {
		xprintf("Error. Either no pstates or 2 pstates\n");
		return 0;
	}

	for (unsigned int i = 0 ; i < 500 ; i++) {
		ret_code = avs_set_pstate_cmd(0, pstate1, 0, 0, 0, 0, 0);
		if (ret_code != AVS_SUCCESS) {
			xprintf("Error. ret_code=0x%X\n", ret_code);
			return 0;
		}
		ret_code = avs_set_pstate_cmd(0, pstate0, 0, 0, 0, 0, 0);
		if (ret_code != AVS_SUCCESS) {
			xprintf("Error. ret_code=0x%X\n", ret_code);
			return 0;
		}
	}
	return 0;
}

