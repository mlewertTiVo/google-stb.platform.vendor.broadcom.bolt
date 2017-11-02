/*****************************************************************************
*
* Copyright 2014 - 2015 Broadcom Corporation.  All rights reserved.
*
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2, available at
* http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
*
* Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a
* license other than the GPL, without Broadcom's express prior written
* consent.
*
*****************************************************************************/

#ifndef _ANDROID_BSU_H
#define _ANDROID_BSU_H

#include <compiler.h>
#include <bsu-api.h>

/* BSU API shortcuts */
#define cmd_sw_value	bsuapi->xfn_cmd_sw_value
#define cmd_sw_isset	bsuapi->xfn_cmd_sw_isset
#define cmd_getarg	bsuapi->xfn_cmd_getarg
#define cmd_gpio_btn_state	bsuapi->xfn_cmd_gpio_btn_state
#define ui_showerror	bsuapi->xfn_ui_showerror
#define fs_init		bsuapi->xfn_fs_init
#define fs_uninit	bsuapi->xfn_fs_uninit
#define fs_open		bsuapi->xfn_fs_open
#define fs_close	bsuapi->xfn_fs_close
#define fs_read		bsuapi->xfn_fs_read
#define fs_seek		bsuapi->xfn_fs_seek
#define os_strlen	bsuapi->xfn_strlen
#define os_malloc	bsuapi->xfn_malloc
#define os_free		bsuapi->xfn_free
#define os_strcpy	bsuapi->xfn_strcpy
#define os_strncpy	bsuapi->xfn_strncpy
#define os_memset	bsuapi->xfn_memset
#define os_memcmp	bsuapi->xfn_memcmp
#define os_memcpy       bsuapi->xfn_memcpy
#define os_sprintf	bsuapi->xfn_sprintf
#define os_strcmp	bsuapi->xfn_strcmp
#define os_strncmp	bsuapi->xfn_strncmp
#define os_strtok_r	bsuapi->xfn_strtok_r
#define os_strtoul	bsuapi->xfn_strtoul
#define os_strstr	bsuapi->xfn_strstr
#define os_atoi	bsuapi->xfn_atoi
#define os_printf	bsuapi->xfn_printf
#define cmd_addcmd	bsuapi->xfn_cmd_addcmd
#define bolt_docommands	bsuapi->xfn_bolt_docommands
#define bolt_getdevinfo	bsuapi->xfn_bolt_getdevinfo
#define bolt_ioctl	bsuapi->xfn_bolt_ioctl
#define bolt_open	bsuapi->xfn_bolt_open
#define bolt_close	bsuapi->xfn_bolt_close
#define bolt_readblk	bsuapi->xfn_bolt_readblk
#define bolt_writeblk	bsuapi->xfn_bolt_writeblk
#define bolt_inpstat	bsuapi->xfn_bolt_inpstat
#define tz_smm_set_params	bsuapi->xfn_tz_smm_set_params
#define bolt_load_program	bsuapi->xfn_bolt_load_program
#define lib_crc32	bsuapi->xfn_crc32
#define poll_task	bsuapi->xfn_poll_task
#define msleep		bsuapi->xfn_msleep
#define boot_addloader	bsuapi->xfn_boot_addloader
#define env_getenv	bsuapi->xfn_env_getenv
#define env_getval      bsuapi->xfn_env_getval
#define env_save      bsuapi->xfn_env_save
#define env_setenv	bsuapi->xfn_env_setenv
#define console_status	bsuapi->xfn_console_status
#define arch_getticks64	bsuapi->xfn_getticks64
#define arch_get_timer_freq_hz  bsuapi->xfn_arch_get_timer_freq_hz
#define aon_reset_string	bsuapi->xfn_aon_reset_string
#if CFG_SPLASH
#define splash_feedback	bsuapi->xfn_splash_feedback
#endif

#define DEBUG
#if defined(DEBUG)
#define DLOG(fmt, args...)	os_printf(fmt, ##args)
#else
#define DLOG(fmt, args...)
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define BOOT_ARGS_SIZE 512

extern struct bsu_api *bsuapi;

extern int android_boot(ui_cmdline_t *cmd, int argc, char *argv[]);
extern int android_fastboot(ui_cmdline_t *cmd, int argc, char *argv[]);
extern int android_trusty_boot(ui_cmdline_t *cmd, int argc, char *argv[]);

extern void boot_reason_reg_set(uint32_t reg_val);
extern uint32_t boot_reason_reg_get(void);

extern int android_get_boot_partition(ui_cmdline_t *cmd, char *boot_partition,
				      int *is_legacy_boot);
extern int android_override_bootargs(char* bootargs, char *bootargs_buf);
extern int android_boot_addloader(void);
#if defined(BCHP_WKTMR_REG_START)
extern void android_wktmr_adjust(void);
#endif
extern void android_start_wdt(void);
extern void fastboot_process_canned_gpt(uint32_t boot_reason);
#endif /* _ANDROID_BSU_H */
