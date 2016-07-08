/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __PSCI32_H__
#define __PSCI32_H__

#include <stddef.h>
#include <bitops.h>
#include <bchp_common.h>
#include <compiler.h>

/* clean and invalidate by dcache level */
#define UPTO_L1 0
#define UPTO_POU 1 /* Max A15 cache levels */

/* In the pending cases another cpu must
 * check and progress the PEND to its
 * conclusion as the cpu can't take itself
 * fully down.
 */
enum cpu_state {
	CPU_OFF = 0,
	CPU_PEND_OFF,
	CPU_FAIL_OFF,
	CPU_PEND_ON,
	CPU_FAIL_ON,
	CPU_ON,
	CPU_IDLE,
	CPU_NOT_PRESENT,
};

/* target_cpu is an index into an array of these */
struct per_cpu {
	volatile uint32_t	lock; /* Bakery lock */
	enum cpu_state		state;
	uint32_t		entry_point_address;
	uint32_t		context_id;
} __attribute__((aligned(64)));

/* Do not move 'nr_cpu' - its offset is referenced
 * in the 'calc_per_cpu_stack' assembly macro.
 */
struct psci_cfg {
	unsigned int nr_cpu;
	unsigned int nr_cluster;
	unsigned int all_cpus; /* save a mul */
	unsigned int debug;
	unsigned int bootonce;
	unsigned int locked_sticky;
	unsigned int rac_master;
	struct per_cpu *cpu;
} __attribute__((aligned(64)));


static inline uint32_t rdb_read(uint32_t reg)
{
	return *(volatile uint32_t *)(reg | (unsigned)BCHP_PHYSICAL_OFFSET);
}

static inline void rdb_write(uint32_t reg, uint32_t value)
{
	*(volatile uint32_t *)(reg | (unsigned)BCHP_PHYSICAL_OFFSET) = value;
}


/* uart */
void uart_init(void);
void uart_putc(int c);

/* minilib */
void __puts(const char *s);
int puts(const char *s);
void udelay(uint32_t us);
void mdelay(uint32_t ms);
void writehex(uint32_t val);
void writehex64(uint64_t val);
void memcpy(void *dst, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void writeint(int n);
void msg_cpu(const char *s, unsigned int cpu /* cpu+cluster index */);
void hexdump(uint32_t mem, int numitems);

/* powerctl */
void set_cpu_boot_addr(unsigned int cpu, uint32_t boot_addr);
uint32_t cpu_enable(unsigned int cpu, int en);
uint32_t power_up_cpu(unsigned int cpu);
uint32_t power_down_cpu(unsigned int cpu);
void reboot(void);
unsigned int cpu_is_on(unsigned int cpu);

/* asm */
void smp_on(void);
void __noreturn smp_off_unlock(volatile uint32_t *lock);
void cpu_init_secondary(void);
void exec32(uint32_t linux_entry, uint32_t fdt,
		volatile uint32_t *lock, int secondary);

/* cache */
void clean_invalidate_dcache(int to_cache_level);
void invalidate_dcache(int to_cache_level);
void invalidate_icache(void);
void icache_enable(int yes);
unsigned long disable_all_caches_and_mmu(void);
void set_sys_ctl_reg(unsigned long sctlr);
unsigned long get_sys_ctl_reg(void);

uint32_t rac_disable_and_flush(void);
void rac_enable(uint32_t rac_cfg0);
int rac_is_enabled(void);

/* secure psci */
void gic_secure_uninit(void);

/* psci */
struct psci_cfg *get_config(void);
unsigned int get_cpu_idx(uint32_t mpidr);

void psci_decode(uint32_t r0, uint32_t r1, uint32_t r2,	uint32_t r3);
void psci_init(uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3);
void eret32(uint32_t r0); /* psci return code */
void __noreturn per_cpu_start(uint32_t per_cpu_stacktop);

unsigned int debug(void);
unsigned int rac_master(void);

#endif /* __PSCI32_H__ */
