/***************************************************************************
 *     Copyright (c) 2012-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "bsu-api.h"
#include "board.h"
#include "timer.h"
#include "arch_ops.h"
#include "cache_ops.h"
#include "net_api.h"
#include "error.h"
#include "../fsbl/fsbl-shmoo.h"
#include "bolt.h"
#include "avs_bsu.h"
#include "env_subr.h"
#include "timer.h"
#include "board_params.h"
#if CFG_SPLASH
#include "splash-api.h"
#endif

#define BSU_MEMINFOS (NUM_MEMC + 2)

/* WARNING: BOLT is single threaded so its the responsibility of the
 bsu app to make sure the BOLT resources that are called back into
 are protected by a mutex or semaphore lock to prevent races,
 deadlocks, livelocks, crashes and mystery bugs.
*/

/* ------------------------------------------------------------------------- */
/* libc like functions */

static void *bsu_malloc(size_t size)
{
	return KMALLOC(size, 0 /* align */);
}


static void bsu_free(void *ptr)
{
	KFREE(ptr);
}


static void *bsu_umalloc(size_t size)
{
	return KUMALLOC(size, 0 /* align */);
}


static void bsu_ufree(void *ptr)
{
	KUFREE(ptr);
}

static void *bsu_realloc(void *ptr, size_t size)
{
	void *q;
	size_t oldsz;

	if (NULL == ptr)
		return KMALLOC(size, 0);

	if (0 == size) { /* && ptr != NULL */
		KFREE(ptr);
		return NULL;
	}

	/* get malloc'd data size. 0 = bad ptr, or not in our pool.
	NOTE: It may be more than the original malloc requested
	size if any alignment bytes have been added. */
	oldsz = KMALLOC_USABLE_SIZE(ptr);
	if (0 == oldsz)
		return NULL;

	if (size == oldsz)
		return ptr;

	q = KMALLOC(size, 0);
	if (NULL == q)
		return NULL;

	lib_memcpy(q, ptr, min(size, oldsz));

	KFREE(ptr);

	return q;
}

static int bsu_xsnprintf(char *str, int size, const char *format, ...)
{
	va_list marker;
	int count;

	va_start(marker, format);
	count = xvsnprintf(str, size, format, marker);
	va_end(marker);

	return count;
}


/* ------------------------------------------------------------------------- */
/* misc support wrappers */

/* You need to call this (~100mSec) to
 keep BOLT drivers sane.
*/
static void bsu_poll_task(void)
{
	POLL();
}


/* add dummy values so we don't crash */
#if !CFG_NETWORK
static int bsu_dummy_net_init(char *devname)
{
	return BOLT_ERR_UNSUPPORTED;
}


static void bsu_dummy_net_uninit(void)
{
}


static int bsu_dummy_do_dhcp_request(char *devname)
{
	return BOLT_ERR_UNSUPPORTED;
}
#endif


/* To override, the real function should have exactly the same
prototype.
*/
void __weak aegis_linux_kernel_verify(unsigned long region_end_address,
	unsigned long sig_start_addr, int bverify, int bdecrypt)
{
}


static const struct memsys_params *find_memc(uint32_t idx)
{
	if (idx >= ARRAY_SIZE(shmoo_params))
		return NULL;

	return &shmoo_params[idx];
}

/* ------------------------------------------------------------------------- */
/* additional libc func used by Android BSU */

static char *bsu_strncat(char *dest, const char *src, size_t n)
{
	size_t i, len = lib_strlen(dest);

	for (i = 0; i < n; i++) {
		if ('\0' == *src)
			break;

		dest[len + i] = *src;
		src++;
	}

	dest[len + i] = '\0';

	return dest;
}

/* scan for a char being in a set (accept) of chars */
static inline int char_in_set(char c, const char *accept)
{
	while (*accept) {
		if (c == *accept)
			return 1;
		accept++;
	}
	return 0;
}

static char *bsu_strtok_r(char *str, const char *delim, char **saveptr)
{
	char *p, *s;

	if (NULL != str) /* first pass */
		*saveptr = s = str;
	else
		s = *saveptr;

	while (*s) {
		if (char_in_set(*s, delim)) {
			*s = '\0';
			s++;
			break;
		}
		s++;
	}

	p = *saveptr; /* save original str start pos */

	*saveptr = s;

	return (*p) ? p : NULL;
}

size_t bsu_strspn(const char *s, const char *accept)
{
	size_t c = 0;

	while (*s) {
		if (NULL == strchr(accept, *s))
			break;
		c++; s++;
	}
	return c;
}


static long int __bsu_xtol(const char *dest, char **ep)
{
	long int x = 0;
	unsigned int digit;

	if ((*dest == '0') && (*(dest + 1) == 'x'))
		dest += 2;

	while (*dest) {
		if ((*dest >= '0') && (*dest <= '9'))
			digit = *dest - '0';
		else if ((*dest >= 'A') && (*dest <= 'F'))
			digit = 10 + *dest - 'A';
		else if ((*dest >= 'a') && (*dest <= 'f'))
			digit = 10 + *dest - 'a';
		else
			break;

		x *= 16;
		x += digit;
		dest++;
	}

	if (ep)
		*ep = (char *)dest;

	return x;
}

#define bsu_isspace(x) (((x) == ' ') || ((x) == '\t'))


static int checkbase(char c, int base)
{
	char a = '0' + (unsigned)(base);

	return (c < '0') || (c >= a);
}

static long int bsu_strtol(const char *nptr, char **endptr, int base)
{
	long int x = 0;
	int neg  = 0, digit;
	int maybehex = ((base == 16) || (base == 0));

	while (1) {
		*endptr = (char *)nptr;

		if (*nptr == '\0')
			return 0;

		if (!bsu_isspace(*nptr))
			break;
		nptr++;
	}

	if (*nptr == '-') {
		neg = 1;
		nptr++;
	} else if (*nptr == '+') {
		nptr++;
	} else {
	}

	if (*nptr == '0') {
		nptr++;
		if (*nptr == 'x') {
			if (!maybehex) {
				*endptr = (char *)nptr;
				return 0;
			} else {
				nptr++;
				x = __bsu_xtol(nptr, endptr);
				return  neg ? -x : x;
			}
		}

		if (0 == base)
			base = 8;
	}

	if (16 == base) {
		x = __bsu_xtol(nptr, endptr);
		return  neg ? -x : x;
	}

	if (0 == base)
		base = 10;

	if ((base < 2) || (base > 36)) {
		*endptr = NULL;
		return 0;
	}

	while (*nptr) {
		if (!checkbase(*nptr, base))
			digit = *nptr - '0';
		else
			break;

		x *= base;
		x += digit;
		nptr++;
	}

	*endptr = (char *)nptr;

	return neg ? -x : x;
}


static unsigned long int bsu_strtoul(const char *nptr, char **endptr, int base)
{
	return (unsigned long int)bsu_strtol(nptr, endptr, base);
}


static const ssbl_board_params *bsu_board_params(void)
{
	struct fsbl_info *inf = board_info();

	if (!inf || (inf->board_idx >= FSBLINFO_N_BOARDS(inf->n_boards)))
		return NULL;

	return &board_params[inf->board_idx];
}

/* ------------------------------------------------------------------------- */

static struct bsu_meminfo xfd_mem[BSU_MEMINFOS];

static struct bsu_api bsu_xapi = {

	.xfd_signature = BSU_SIGNATURE,
	.xfd_api_version = BSU_API_VERSION,

	/* memc0 may be split (BOLT in the middle)
	 and an extra entry to mark the end (and
	 as a guard.)
	*/
	.xfd_num_mem = BSU_MEMINFOS,
	.xfd_mem = xfd_mem,

	.xfn_arch_get_timer_freq_hz = arch_get_timer_freq_hz,
	.xfn_arch_getticks = arch_getticks,
	.xfn_clear_d_cache = clear_d_cache,

	.xfn_printf = printf,
	.xfn_sprintf = sprintf,
	.xfn_vprintf = vprintf,
	.xfn_vsprintf = xvsprintf,
	.xfn_vsnprintf = vsnprintf,
	.xfn_strcpy = lib_strcpy,
	.xfn_strcmp = lib_strcmp,
	.xfn_strlen = lib_strlen,
	.xfn_memcmp = lib_memcmp,
	.xfn_memcpy = lib_memcpy,
	.xfn_memset = lib_memset,
	.xfn_atoi = lib_atoi,
	.xfn_xtoq = lib_xtoq,
	.xfn_malloc = bsu_malloc,
	.xfn_free = bsu_free,
	.xfn_umalloc = bsu_umalloc,
	.xfn_ufree = bsu_ufree,

	.xfn_bolt_docommands = bolt_docommands,
	.xfn_poll_task = bsu_poll_task,

	.xfn_console_open = console_open,
	.xfn_console_close = console_close,
	.xfn_console_read = console_read,
	.xfn_console_write = console_write,
	.xfn_console_status = console_status,
#if !CFG_MIN_CONSOLE
	.xfn_console_readkey = console_readkey,
#else
	.xfn_console_readkey = NULL,
#endif
	.xfn_console_readline = console_readline,
	.xfn_console_readline_noedit = console_readline_noedit,

	.xfn_fs_init = fs_init,
	.xfn_fs_uninit = fs_uninit,
	.xfn_fs_open = fs_open,
	.xfn_fs_close = fs_close,
	.xfn_fs_read = fs_read,
	.xfn_fs_write = fs_write,
	.xfn_fs_seek = fs_seek,
	.xfn_fs_hook = fs_hook,

#if CFG_NETWORK
	.xfn_net_init = net_init,
	.xfn_net_uninit = net_uninit,
	.xfn_do_dhcp_request = do_dhcp_request,
#else
	.xfn_net_init = bsu_dummy_net_init,
	.xfn_net_uninit = bsu_dummy_net_uninit,
	.xfn_do_dhcp_request = bsu_dummy_do_dhcp_request,
#endif
	.xfn_nvram_getsize = nvram_getsize,
	.xfn_nvram_read = nvram_read,
	.xfn_nvram_write = nvram_write,
	.xfn_nvram_open = nvram_open,
	.xfn_nvram_close = nvram_close,
	.xfn_nvram_erase = nvram_erase,

	.xfn_bolt_set_envdevname = bolt_set_envdevname,
	.xfn_bolt_free_envdevname = bolt_free_envdevname,

	.xfn_bolt_errortext = bolt_errortext,

	.xfn_region_verify = aegis_linux_kernel_verify,

	/* v4 */
	.xfn_strncpy = lib_strncpy,

	/* features */
	.xfn_boxmode = board_init_current_rts_boxmode,

	/* v5 */
	.xfn_strcat = lib_strcat,
	.xfn_strchr = lib_strchr,
	.xfn_strdup = lib_strdup,
	.xfn_strncat = bsu_strncat,
	.xfn_strrchr = lib_strrchr,
	.xfn_strncmp = lib_strncmp,
	.xfn_strstr = lib_strstr,
	.xfn_strtok_r = bsu_strtok_r,
	.xfn_strspn = bsu_strspn,
	.xfn_strtoul = bsu_strtoul,
	.xfn_strtol = bsu_strtol,
	.xfn_crc32 = lib_crc32,

	/* v6 */
	.xfn_realloc = bsu_realloc,
	.xfn_msleep = bolt_msleep,
	.xfn_snprintf = bsu_xsnprintf,

	.xfn_xprinthook = &xprinthook,

	/* v7 */
	.xfn_bolt_getdevinfo = bolt_getdevinfo,
	.xfn_bolt_open = bolt_open,
	.xfn_bolt_close = bolt_close,
	.xfn_bolt_readblk = bolt_readblk,
	.xfn_bolt_writeblk = bolt_writeblk,
	.xfn_bolt_inpstat = bolt_inpstat,

	.xfn_aon_reset_string = aon_reset_as_string,

	/* v8 */
	.xfn_invalidate_d_cache = invalidate_d_cache,

	/* --- diagnostics sub-api --- */

	.xfd_diags_version = BSU_DIAGS_VERSION,

	.xfn_cmd_sw_value = cmd_sw_value,
	.xfn_cmd_sw_isset = cmd_sw_isset,
	.xfn_cmd_getarg = cmd_getarg,
	.xfn_cmd_addcmd = cmd_addcmd,

	.xfn_cmd_gpio_btn_state = cmd_gpio_btn_state,
	.xfn_ui_showerror = ui_showerror,

	.xfn_board_thisboard = board_thisboard,
	.xfn_board_find_ddr = board_find_ddr,
	.xfn_find_memc = find_memc,

	.xfd_edis_offset = EDIS_OFFS,
	.xfd_edis_nphy = EDIS_NPHY,

	.xfn_avs_get_data = avs_get_data,

	.xfn_getticks64 = arch_getticks64,

	.xfn_boot_addloader = bolt_addloader,

	.xfn_env_getenv = env_getenv,
	.xfn_env_setenv = env_setenv,
	.xfn_env_getval = env_getval,
	.xfn_env_save = env_save,

	/* Eth PHY diags (v9) */
	.xfn_bolt_gethandle = bolt_gethandle,
	.xfn_bolt_ioctl = bolt_ioctl,
	.xfn_usleep = bolt_usleep,
	.xfn_board_params = bsu_board_params,

#if CFG_TRUSTZONE_MON
	.xfn_tz_smm_set_params = tz_smm_set_params,
	.xfn_bolt_load_program = bolt_load_program,
#endif
#if CFG_SPLASH
	.xfn_splash_feedback = splash_feedback,
#endif
};

#define RESERVE_FOR_LINUX 10UL

void bolt_start_bsu(unsigned long ept, unsigned long param1,
					unsigned long param2)
{
	int i, ix = 0;
	struct board_type *b = board_thisboard();
	struct ddr_info *ddr;

	/* Reset all entries to unused/non-valid
	*/
	for (i = 0; i < BSU_MEMINFOS; i++) {
		bsu_xapi.xfd_mem[i].memc = BSU_MEMINFO_INVALID;
		bsu_xapi.xfd_mem[i].base = BSU_MEMINFO_INVALID;
		bsu_xapi.xfd_mem[i].top = BSU_MEMINFO_INVALID;
	}

	for (i = 0; i < NUM_MEMC; i++) {
		ddr = board_find_ddr(b, i);
		if (!ddr)
			break;

		/* Special case for (split) memc0 with BOLT in the middle
		 This gives you memory RESERVE_FOR_LINUX up to BOLT start
		to play with. Note that this does not include your app if
		it happens to live in this area! It can live in memory
		below RESERVE_FOR_LINUX but a 'load linux' command
		might nuke it as BOLT loads the Linux code over yours.
		*/
		if (ddr->which == 0) {
			bsu_xapi.xfd_mem[ix].memc = ddr->which;
			bsu_xapi.xfd_mem[ix].base = _MB(ddr->base_mb);

			/* find the lowest address in bolt based on
			our defaults. */
			bsu_xapi.xfd_mem[ix].top =
					min(SSBL_BOARDINFO, SSBL_PAGE_TABLE);

			if (ddr->base_mb < RESERVE_FOR_LINUX)
				bsu_xapi.xfd_mem[ix].base +=
					_MB(RESERVE_FOR_LINUX);

			/* do 2nd half of memc0?
			*/
			if ((ddr->base_mb + ddr->size_mb) > 256) {
				ix++;
				bsu_xapi.xfd_mem[ix].memc = ddr->which;
				bsu_xapi.xfd_mem[ix].base = _MB(256);
				bsu_xapi.xfd_mem[ix].top =
					_MB(ddr->base_mb + ddr->size_mb);
			}
		} else {
			bsu_xapi.xfd_mem[ix].memc = ddr->which;
			bsu_xapi.xfd_mem[ix].base = _MB(ddr->base_mb);
			bsu_xapi.xfd_mem[ix].top =
					_MB(ddr->base_mb + ddr->size_mb);
		}
		ix++;
	}

	bsu_xapi.xfd_board_id = b->bid;

	bolt_launch_bsu(ept, param1, param2, (unsigned long)&bsu_xapi);
}


