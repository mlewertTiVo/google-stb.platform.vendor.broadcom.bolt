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

/*
 * fsbl.h - internal FSBL definitions
 */

#ifndef _FSBL_H
#define _FSBL_H

#include <fsbl-common.h>
#include <stdbool.h>

#define DIV10(a) ((a)/10)
#define MOD10(a) ((a)-(10*DIV10(a)))

#define INITSEG	__attribute__ ((section(".init")))
#define INITSEG_RODATA __attribute__ ((section(".init.rodata")))

/* Generate special function prologues */
#define EXCEPTION(x) __attribute__((interrupt(x)))

/* Do not generate function prologue */
#define NAKED __attribute__((naked))

#if defined(MEMC_ALT) && (MEMSYS_ALT_SIZE > MEMSYS_SIZE)
 #define SHMOO_SRAM_ADDR (MEMSYS_SRAM_ADDR + MEMSYS_ALT_SIZE)
#else
 #define SHMOO_SRAM_ADDR (MEMSYS_SRAM_ADDR + MEMSYS_SIZE)
#endif

/* architecture specific
*/
void late_cpu_init(void);
void mmu_initialize_pagetable(void);
void mmu_add_pages(unsigned long base, unsigned long size);
void mmu_enable(void);
void mmu_disable(void);
int get_num_cpus(void);


/* flash
*/
struct fsbl_flash_partition {
	/*
	 * Chip-select flash is connected to (CS0 represents the boot flash,
	 * even for non-EBI flash like eMMC)
	 */
	int cs;
	/* Base offset of the partition within the flash */
	uint32_t part_offs;
	/* Size of the partition; 0 for unbounded (fill to device) */
	uint32_t part_size;
};

void get_flash_info(struct fsbl_info *info);

/* Assumes reading from the boot flash, BOLT partition */
int load_from_flash(void *dst, uint32_t flash_offs, size_t len);

/*
 * Provided for flexibility; may die() or return negative error code if
 * features are not implemented
 */
int load_from_flash_ext(void *dst, uint32_t flash_offs, size_t len,
			struct fsbl_flash_partition *part);

/* IMPORTANT: see comments for documentation */
int get_flash_offset(uint32_t *ret_offs, uint32_t text_offs,
		     struct fsbl_flash_partition *part);
/* main
*/
void fsbl_print_version(int vermax, int vermin);

/* console/uart
*/
void uart_init(unsigned long base);
int putchar(int c);
int getchar(void);
unsigned long get_uart_base(void);
#ifndef SECURE_BOOT
int getc(void);
#endif

/* hardware timer
*/
uint64_t get_syscount(void);
uint64_t get_systime(void);
void udelay(uint32_t micro_seconds);
void sleep_ms(uint32_t ms);
uint64_t get_time_diff(uint64_t was);

/* misc
*/
void fsbl_pinmux(void); /* generated */
void *copy_code(void);


/* minilib
*/
void memcpy4(uint32_t *dst, const uint32_t *src, size_t bytes);
void fastcopy(uint32_t *dst, const uint32_t *src, size_t bytes);
void memcpy(void *dst, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void __noreturn die(char *msg);
void __puts(const char *s);
int puts(const char *s);
void writehex(uint32_t val);
char *itoa(int n);
#define writeint(n) __puts(itoa(n))

/* pwr
*/
#if CFG_BATTERY_BACKUP
int fsbl_docsis_booted(uint32_t *aon_reset_history, uint32_t *pm_ctrl);
int fsbl_booted_on_battery(void);
int fsbl_C1_check(struct fsbl_info *info);
void inform_ecm(uint32_t rstate, uint32_t pstate);
/* Return true if booted on battery because of "dying gasp" situation. */
static inline int fsbl_dying_gasp(void)
{
	return (fsbl_docsis_booted(0, 0) && !fsbl_booted_on_battery());
}
#endif


/* shmoo + saved board, memsys.
*/
void shmoo_load(void);
void memsys_load(void);
#if CFG_PM_S3 && !(defined(CFG_NOSHMOO) || defined(CFG_EMULATION))
void memsys_warm_restart(int num_ddr);
#else
static inline void memsys_warm_restart(int num_ddr) { }
#endif
void shmoo_set(const struct ddr_info *ddr, bool warm_boot);
void shmoo_menu(struct board_nvm_info *nvm);
struct ddr_info *shmoo_ddr_by_index(uint32_t d);
void set_loud_shmoo(void);


/* board list
*/
struct board_nvm_info *nvm_load(void);
void load_boards(struct fsbl_info *info, uint32_t dst);
int board_select(struct fsbl_info *info, uint32_t dst);
struct board_type *get_tmp_board(void);
void board_try_patch_ddr(struct board_nvm_info *nvm);


/* sec
*/
#ifdef CFG_EMULATION
static inline void sec_config_print(void) { }
static inline void sec_init(void) { }
static inline void sec_bfw_load(bool warm_boot) { }
static inline void sec_scramble_sdram(bool warm_boot) { }
static inline void sec_lock_memc(int memc) { }
static inline void sec_verify_ssbl(void) { }
static inline int sec_verify_avs(void) { return 0; }
static inline void sec_avs_set_status(int success) { }
#else
void sec_config_print(void);
void sec_init(void);
void sec_bfw_load(bool warm_boot);
void sec_scramble_sdram(bool warm_boot);
void sec_lock_memc(int memc);
void sec_verify_ssbl(void);
int sec_verify_avs(void);
void sec_avs_set_status(int success);
#endif
uint32_t sec_avs_select_image(void);
void sec_verify_memsys(void);
void sec_memsys_region_disable(void);
void sec_memsys_set_status(int success);
void sec_set_memc(struct fsbl_info *pinfo);
void sec_mitch_check(void);
void sec_print_version(void);
void __noreturn fsbl_end(struct fsbl_info *info);

/* glitch
*/
void anti_glitch_a(void);
void anti_glitch_b(void);
void anti_glitch_c(void);
#if CFG_PM_S3
void anti_glitch_d(void);
void anti_glitch_e(void);
#endif

/* i2c
*/
#if CFG_BOARD_ID
uint8_t get_ext_board_id(void);
#else
static inline int get_ext_board_id(void) { return 0; }
#endif


/* MHL
 *  Returns an (unshifted runflag) FSBL_RUNFLAG_MHL_BOOT_
 * enum with !=0 indicating we did start via an MPM
 * firmware negotiated MHL (sink) derived power src.
 */
#if CFG_MHL
uint32_t read_mhl_power_config(struct fsbl_info *info);
#else
static inline uint32_t read_mhl_power_config(struct fsbl_info *info)
{
	return FSBL_RUNFLAG_MHL_BOOT_Nope;
}
#endif

#endif /* _FSBL_H */
