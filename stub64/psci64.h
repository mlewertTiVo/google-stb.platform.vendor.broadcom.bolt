/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2018 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __PSCI64_H__
#define __PSCI64_H__

#include <stddef.h>
#include <bitops.h>
#include <bchp_common.h>

#ifndef __noreturn
#define __noreturn __attribute__((noreturn))
#endif

/* clean and invalidate by dcache level */
#define UPTO_L1 1
#define UPTO_POU 7 /* Max A53 cache levels */

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
	uint64_t		entry_point_address;
	uint64_t		context_id;
} __attribute__((aligned(64)));

/* Do not move 'nr_cpu' - its offset is referenced
 * in the 'calc_per_cpu_stack' assembly macro.
 */
struct psci_cfg {
	unsigned int nr_cpu;
	unsigned int nr_cluster;
	unsigned int all_cpus; /* save a mul */
	unsigned int debug;
	unsigned int locked_sticky;
	unsigned int rac_master;
	struct per_cpu *cpu;
} __attribute__((aligned(64)));


static inline uint32_t rdb_read(uint64_t reg)
{
	return *(volatile uint32_t *)(reg + (unsigned)BCHP_PHYSICAL_OFFSET);
}

static inline void rdb_write(uint64_t reg, uint32_t value)
{
	*(volatile uint32_t *)(reg + (unsigned)BCHP_PHYSICAL_OFFSET) = value;
}

static inline uint64_t rdb_read64(uint64_t reg)
{
	return *(volatile uint64_t *)(reg + (unsigned)BCHP_PHYSICAL_OFFSET);
}

static inline void rdb_write64(uint64_t reg, uint64_t value)
{
	*(volatile uint64_t *)(reg + (unsigned)BCHP_PHYSICAL_OFFSET) = value;
}

/* stub-uart */
void uart_init(void);
void uart_putc(int c);

/* smm-minilib */
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

/* smm-powerctl */
void set_cpu_boot_addr(unsigned int cpu, uint64_t boot_addr);
uint32_t cpu_enable(unsigned int cpu, int en);
uint32_t power_up_cpu(unsigned int cpu);
uint32_t power_down_cpu(unsigned int cpu);
void reboot(void);
unsigned int cpu_is_on(unsigned int cpu);

/* asm */
void __smp_on(uint64_t x0, uint64_t x1);
#define	smp_on() __smp_on(0, 0)

void __noreturn smp_off_unlock(volatile uint32_t *lock);
uint64_t get_mpidr(void);
void cpu_init32(void);
void cpu_init64(void);
void cpu_boot32(uint64_t at_addr, uint64_t per_cpu_stacktop, uint64_t context,
		volatile uint32_t *lock);
void cpu_boot64(uint64_t at_addr, uint64_t per_cpu_stacktop, uint64_t context,
		volatile uint32_t *lock);
void gic_bypass(void);

/* smm-cache */
void clean_invalidate_dcache(int to_cache_level);
void invalidate_dcache(int to_cache_level);
void invalidate_icache(void);
void icache_enable(int yes);
unsigned long disable_all_caches_and_mmu(void);

void rac_flush(void);
uint32_t rac_disable_and_flush(void);
int rac_enable(uint32_t rac_cfg0);
int rac_is_enabled(void);

/* Trustzone Monitor */
void tz_sm_set_loadaddr(uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3,
			uint64_t x4 /* current stack pointer */);

/* smm-psci */
struct psci_cfg *get_config(void);
unsigned int get_cpu_idx(uint64_t mpidr);

void psci_decode(uint64_t x0, uint64_t x1, uint64_t x2,	uint64_t x3,
			uint64_t x4 /* current stack pointer */);

void psci_init(uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3,
			uint64_t x4 /* current stack pointer */);

void exec64(uint64_t fdt, uint64_t linux_entry);
void exec64_el3(uint64_t fdt, uint64_t linux_entry);
void exec32(uint64_t fdt, uint64_t linux_entry);

void eret64(uint64_t x0 /* psci return code */,
		uint64_t x4 /* saved stack pointer */);

void per_cpu_start32(uint64_t per_cpu_stacktop);
void per_cpu_start64(uint64_t per_cpu_stacktop);

unsigned int debug(void);
unsigned int rac_master(void);

#endif /* __PSCI64_H__ */
