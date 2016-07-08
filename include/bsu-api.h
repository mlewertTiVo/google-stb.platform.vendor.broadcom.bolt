/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __BSU_API_H__
 #define __BSU_API_H__

/* BOLT should never define 'BSU' as these are
 the headers required by the BSU app itself.
*/
#ifdef BSU

#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "fileops.h"
#include "ui_command.h"
#include "ui_gpio.h"
#include "devfuncs.h"

#ifdef DROID_BSU
#include "loader.h"
#else
/* Fake out bolt_loader_t type to get BSU app that does not depend on other
 * BOLT data structure to compile with this header file. Use this typedef
 * to maintain sizeof(struct bsu_api).*/
typedef void bolt_loader_t;
#endif /* DROID_BSU */

#else

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "common.h"
#include "bolt.h"
#include "fileops.h"
#include "console.h"
#include "ui_command.h"
#include "ui_gpio.h"
#include "net_ebuf.h"
#include "net_ether.h"
#include "net_api.h"
#include "../ssbl/main/nvram_subr.h"
#include "avs_bsu.h"
#include "ssbl-common.h"
#include "loader.h"
#include "devfuncs.h"
#endif


/* If the API is changed in any way then
you must bump the version

- BSU_API_VERSION is for the BSU portion only: from the first struct element
to the line "--- diagnostics sub-api ---"

- BSU_DIAGS_VERSION is for everything after the line
" --- diagnostics sub-api --- "
*/
#define BSU_API_VERSION 9
#define BSU_DIAGS_VERSION 3

#define BSU_SIGNATURE 0x62737531

#define BSU_MEMINFO_INVALID -1


/* ------------------------------------------------------------------------- */

extern void bolt_launch_bsu(unsigned long ept, unsigned long param1,
			unsigned long param2, unsigned long param3);

void bolt_start_bsu(unsigned long ept, unsigned long param1,
			unsigned long param2);

/* ------------------------------------------------------------------------- */

struct bsu_meminfo {
	uint32_t memc;
	uint32_t base;
	uint32_t top;
};

struct bsu_api {

	uint32_t xfd_signature;
	uint32_t xfd_api_version;

	uint32_t xfd_num_mem;
	struct bsu_meminfo *xfd_mem; /* array */

	/* misc */
	uint32_t (*xfn_arch_get_timer_freq_hz)(void);
	uint32_t (*xfn_arch_getticks)(void);
	void (*xfn_clear_d_cache)(void *addr, unsigned int len);

	/* lib */
	int  (*xfn_printf)(const char *template, ...);
	int (*xfn_sprintf)(char *buf, const char *templat, ...);
	int (*xfn_vprintf)(const char *templat, va_list marker);
	int (*xfn_vsprintf)(char *outbuf,
			const char *templat, va_list marker);
	int (*xfn_vsnprintf)(char *outbuf, int n,
			const char *templat, va_list marker);


	char *(*xfn_strcpy)(char *dest, const char *src);
	int (*xfn_strcmp)(const char *dest, const char *src);
	size_t (*xfn_strlen)(const char *str);

	int (*xfn_memcmp)(const void *dest, const void *src, size_t cnt);
	void *(*xfn_memcpy)(void *dest, const void *src, size_t cnt);
	void *(*xfn_memset)(void *dest, int c, size_t cnt);
	int (*xfn_atoi)(const char *dest);
	uint64_t (*xfn_xtoq)(const char *dest);


	/* std malloc */
	void *(*xfn_malloc)(size_t size);
	void  (*xfn_free)(void *ptr);
	/* uncached malloc */
	void *(*xfn_umalloc)(size_t size);
	void  (*xfn_ufree)(void *ptr);

	/* bolt */
	int (*xfn_bolt_docommands)(const char *buf);
	void (*xfn_poll_task)(void);

	/* console i/o */
	int (*xfn_console_open)(char *name);
	int (*xfn_console_close)(void);
	int (*xfn_console_read)(unsigned char *buffer, int length);
	int (*xfn_console_write)(/* arm unsigned */ char *buffer, int length);
	int (*xfn_console_status)(void);
	int (*xfn_console_readkey)(void);
	int (*xfn_console_readline)(char *prompt, char *str, int len);
	int (*xfn_console_readline_noedit)(char *prompt, char *str, int len);

	/* fileops */
	int (*xfn_fs_init)(const char *fsname,
				fileio_ctx_t **fsctx, void *device);
	int (*xfn_fs_uninit)(fileio_ctx_t *);
	int (*xfn_fs_open)(fileio_ctx_t*,
				void **ref, const char *filename, int mode);
	int (*xfn_fs_close)(fileio_ctx_t*, void *ref);
	int (*xfn_fs_read)(fileio_ctx_t*,
				void *ref, uint8_t *buffer, int len);
	int (*xfn_fs_write)(fileio_ctx_t*,
				void *ref, uint8_t *buffer, int len);
	int (*xfn_fs_seek)(fileio_ctx_t*,
				void *ref, int offset, int how);
	int (*xfn_fs_hook)(fileio_ctx_t *fsctx, const char *fsname);

	/* net */
	int  (*xfn_net_init)(char *devname);
	void (*xfn_net_uninit)(void);
	int  (*xfn_do_dhcp_request)(char *devname);

	/* nvram */
	int (*xfn_nvram_getsize)(void);
	int (*xfn_nvram_read)(unsigned char *buffer, int, int);
	int (*xfn_nvram_write)(unsigned char *buffer, int, int);
	int (*xfn_nvram_open)(void);
	int (*xfn_nvram_close)(void);
	int (*xfn_nvram_erase)(void);

	/* nvram_subr */
	void (*xfn_bolt_set_envdevname)(char *name);
	void (*xfn_bolt_free_envdevname)(void);

	/* error */
	const char *(*xfn_bolt_errortext)(int err); /* bolt_errortext */

	/* security */
	void (*xfn_region_verify)(unsigned long region_end_address,
		unsigned long sig_start_addr, int bverify, int bdecrypt);

	/* lib (v4) */
	char *(*xfn_strncpy)(char *dest, const char *src, size_t cnt);

	/* lib (v5) */
	char *(*xfn_strcat)(char *dest, const char *src);
	char *(*xfn_strchr)(const char *dest, int c);
	char *(*xfn_strdup)(const char *str);
	char *(*xfn_strncat)(char *dest, const char *src, size_t n);
	char *(*xfn_strrchr)(const char *dest, int c);
	int   (*xfn_strncmp)(const char *dest, const char *src, size_t cnt);
	char *(*xfn_strstr)(const char *dest, const char *find);
	char *(*xfn_strtok_r)(char *str, const char *delim, char **saveptr);
	size_t (*xfn_strspn)(const char *s, const char *accept);
	unsigned long int (*xfn_strtoul)(const char *nptr, char **endptr, int base);
	long int (*xfn_strtol)(const char *nptr, char **endptr, int base);
	unsigned int (*xfn_crc32)(const unsigned char *buf, unsigned int len);

	/* lib (v6) */
	/* WARNING: Do *NOT* use realloc with a umalloc'd ptr. */
	void *(*xfn_realloc)(void *ptr, size_t size);
	void (*xfn_msleep)(unsigned int ms);
	int (*xfn_snprintf)(char *str, int size, const char *format, ...);

	/* lib (v7) */
	int (*xfn_bolt_getdevinfo)(char *name);
	int (*xfn_bolt_open)(char *name);
	int (*xfn_bolt_close)(int handle);
	int (*xfn_bolt_readblk)(int handle, bolt_offset_t offset,
					unsigned char *buffer, int length);
	int (*xfn_bolt_writeblk)(int handle, bolt_offset_t offset,
					unsigned char *buffer, int length);
	int (*xfn_bolt_inpstat)(int handle);

	char *(*xfn_aon_reset_string)(void);

	/* features (v5) */
	int (*xfn_boxmode)(void);

	/* features (v6) */
	uint8_t xfd_board_id;

	int (**xfn_xprinthook) (const char *);

	/* misc (v8) */
	void (*xfn_invalidate_d_cache)(void *addr, unsigned int len);

	/* --- diagnostics sub-api --- */

	/* Additions for memory diagnostics program.
	Can be useful for other things. The main BSU
	app should not care beyond this point. */

	/* user interface */
	uint32_t xfd_diags_version;

	int (*xfn_cmd_sw_value)(ui_cmdline_t *cmd, char *swname,
			const char **swvalue);

	int (*xfn_cmd_sw_isset)(ui_cmdline_t *cmd, char *swname);

	char *(*xfn_cmd_getarg)(ui_cmdline_t *cmd, int argnum);

	int (*xfn_cmd_addcmd)(char *command,
		int (*func)(ui_cmdline_t *, int argc, char *argv[]),
		void *ref, char *help, char *usage, char *switches);

	int (*xfn_ui_showerror)(int errcode, char *tmplt, ...);

	int (*xfn_cmd_gpio_btn_state)(char* bank, int pin);

#ifndef BSU /* export BOLT inner system info */
	struct board_type *(*xfn_board_thisboard)(void);

	struct ddr_info *(*xfn_board_find_ddr)(struct board_type *b,
				uint32_t idx);

	const struct memsys_params *(*xfn_find_memc)(uint32_t idx);
#else /* maintain sizeof(struct bsu_api) */

	void *(*xfn_board_thisboard)(void);

	void *(*xfn_board_find_ddr)(void *b, uint32_t idx);

	void *(*xfn_find_memc)(uint32_t idx);
#endif
	uint32_t xfd_edis_offset;
	uint32_t xfd_edis_nphy;

	/* avs */
	int (*xfn_avs_get_data)(unsigned int *voltage,
			signed int *temperature, bool *firmware_running);
	/* timers */
	uint64_t (*xfn_getticks64)(void);

	/* boot */
	int (*xfn_boot_addloader)(const bolt_loader_t *loader);

	/* env */
	char *(*xfn_env_getenv)(const char *name);

	/* diags v2 */
	int (*xfn_env_setenv)(const char *name, const char *value, int flags);
	int (*xfn_env_getval)(const char *name);
	int (*xfn_env_save)(void);

	/* Eth PHY diags (v9) */
	int (*xfn_bolt_gethandle)(char *name);
	int (*xfn_bolt_ioctl)(int handle, unsigned int ioctlnum, void *buffer,
	      size_t length, size_t *retlen, unsigned long long offset);
	void (*xfn_usleep)(unsigned int us);
#ifndef BSU
	const ssbl_board_params *(*xfn_board_params)(void);
#else
	const void *(*xfn_board_params)(void);
#endif
};

#endif /* __BSU_API_H__ */

