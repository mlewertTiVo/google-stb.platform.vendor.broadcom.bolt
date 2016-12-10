/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <aarch64/armv8-regs.h>
#include <aarch64/armv8-cop.h>
#include <psci64.h>
#include <psci.h>
#include <bchp_cntcontrolbase.h>
#include <bitops.h>
#include <bakery_lock64.h>
#include <aon_defs.h>


/* TBD:
 *  S3
 *  Spinlocks need EL3 MMU on (MMU tables!) so use bakery for now, ref:
 *   https://lists.linaro.org/pipermail/linaro-dev/2011-July/005823.html
 */


#define PSCI_INT_VERSION 0x02

#define INILOCK_UNLOCKED	0x40545752
#define INILOCK_LOCKED		0x00000000

#define OEM_INI_PARM(x, field) \
	(((x) & OEM_PSCI_INIT_##field##_MASK) \
	>> OEM_PSCI_INIT_##field##_SHIFT)

#define POWER_STATE_PARM(x, field) \
	(((x) & PSCI_POWER_STATE_##field##_MASK) \
	>> PSCI_POWER_STATE_##field##_SHIFT)

#define _LOCK(_me)	bakery_lock_get(_me, &(config->cpu[_me].lock))
#define _UNLOCK(_me)	bakery_lock_release(&(config->cpu[_me].lock))
#define _HASLOCK(_me)	bakery_lock_status(&(config->cpu[_me].lock))

/* From ld script */
extern void *__config_info[];
extern void *__per_cpu_info[];

static struct psci_cfg * const config = (struct psci_cfg *)__config_info;

static unsigned int inilock = INILOCK_UNLOCKED;


/* At the moment we support just affinity levels 0 & 1
 * and no higher.
 */
unsigned int get_cpu_idx(uint64_t mpidr)
{
	unsigned int cluster, cpu;

	cpu = mpidr & MPIDR_AFF0;
	cluster = (mpidr & MPIDR_AFF1) >> 8;

	return (config->nr_cpu * cluster) + cpu;
}


static int affinity_ok(uint64_t mpidr)
{
	if (mpidr & MPIDR_AFF2)
		return 0;

	if (get_cpu_idx(mpidr) >= config->all_cpus)
		return 0;

	return 1;
}


unsigned int debug(void)
{
	return config->debug;
}


unsigned int rac_master(void)
{
	return config->rac_master;
}


struct psci_cfg *get_config(void)
{
	return (struct psci_cfg *)config;
}


/* For conditions where we should not ever get to. */
static void __noreturn backstop(const char *str, int me)
{
	if (config->debug)
		msg_cpu(str, me);

	while (1)
		wfi();
}


static void dump_cpu_states(unsigned int me)
{
	unsigned int i;

	__puts(" cpu idx:");
	writeint(me);
	puts("");

	for (i = 0; i < config->all_cpus; i++) {
		__puts("cpu ");
		writeint(i);
		__puts(" state:");
		writeint(config->cpu[i].state);
		puts("");
	}
}


void psci_init(uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3,
			uint64_t x4)
{
	unsigned int me;
	int rc = PSCI_SUCCESS;

	/* The loader is responsible for clearing all memory used
	 * by this code to 0x00 i.e. clear data areas and .bss
	 * before we get called to init things here.
	 */

	if (inilock != INILOCK_UNLOCKED) {
		rc = PSCI_ERR_DENIED;
		goto out;
	}

	/* smp: caches must be off before we leave or join (we are in EL3) */
	(void)disable_all_caches_and_mmu();
	smp_on();
	invalidate_icache();
	invalidate_dcache(UPTO_POU);
	icache_enable(1); /* At EL3 only */

	if (x1 & OEM_PSCI_INIT_LOCK_MASK)
		inilock = INILOCK_LOCKED;

	config->debug = (x1 & OEM_PSCI_INIT_DEBUG_MASK) ? 1 : 0;
	config->rac_master = (x1 & OEM_PSCI_INIT_IS_RAC_MASTER) ? 1 : 0;

	if (config->debug) {
		__puts("PSCI: config @ ");
		writehex64((uint64_t)config);
	}

	config->nr_cpu = OEM_INI_PARM(x1, NR_CPU);
	config->nr_cluster = OEM_INI_PARM(x1, NR_CLUST);

	if (!config->nr_cpu || !config->nr_cluster) {
		rc = PSCI_ERR_INVALID_PARAMETERS;
		goto out;
	}

	config->all_cpus = config->nr_cpu * config->nr_cluster;

	if (config->debug) {
		__puts(" cpus:");
		writeint(config->all_cpus);
	}

	config->cpu = (struct per_cpu *)__per_cpu_info;

	/* Mark that the cpu doing this is up */
	me = get_cpu_idx(get_mpidr());
	config->cpu[me].state = CPU_ON;

	if (config->debug)
		dump_cpu_states(me);
out:
	eret64(rc, x4);
}


/* A cpu must not call poweroff_single_cpu()
 * for itself as it will not complete and hang
 * at the cpu_enable() point in the code.
 */
static void poweroff_single_cpu(unsigned int i)
{
	unsigned int rc;

	if (config->debug)
		msg_cpu("PWROFF", i);

	if (config->cpu[i].state == CPU_NOT_PRESENT)
		return;

	rc = power_down_cpu(i);
	if (rc != PSCI_SUCCESS) {
		config->cpu[i].state = CPU_FAIL_OFF;
		if (config->debug) {
			__puts("rc=");
			writehex(rc);
			puts("");
		}
	} else {
		config->cpu[i].state = CPU_OFF;
		if (config->debug) {
			msg_cpu("ISOFF", i);
			puts("");
		}
	}

	mdelay(1); /* Settling */

	/* Even if we fail to power it down
	 * we put it in reset anyway, if not
	 * already done by PSCI_CPU_OFF.
	 */
	cpu_enable(i, 0);
}


static inline int is_aarch32_smc_exception(void)
{
	uint32_t esr;

	get_esr_el3(esr);
	BARRIER64();

	if ((esr & ESR_ELx_EC) == ESR_ELx_EC_SMC32)
		return 1;

	/* Could catch it not being !ESR_ELx_EC_SMC64 */
	return 0;
}


static void late_cpu_init(void)
{
	uint32_t cntfreq;
	uint64_t tmp = 0ULL;

	cntfreq = rdb_read(BCHP_CNTControlBase_CNTFID0);
	BARRIER64(); /* This must complete before setting cntfrq_el0 */

	__asm__ __volatile__("msr cntfrq_el0, %0\n"
		: : "r" (cntfreq) : "memory");

	__asm__ __volatile__("msr cntvoff_el2, %0\n"
		: : "r" (tmp) : "memory");

	BARRIER64();
}


static int __noreturn per_cpu_common_init(uint64_t per_cpu_stacktop, int is_64)
{
	int me = get_cpu_idx(get_mpidr());

	/* smp: caches must be off before we leave or join */
	(void)disable_all_caches_and_mmu();

	/*
	 * POWER STATE COORDINATION INTERFACE (PSCI)
	 * Document number: ARM DEN 0022C
	 *
	 * 5.6 CPU_ON
	 *
	 *  5.6.3 Implementation responsibilities: Cache management
	 *
	 *   For CPU_ON, the PSCI implementation must:
	 *
	 *    => Perform invalidation of caches on boot,
	 *       unless this invalidation is automatically
	 *       performed by the hardware.
	 *
	 *    =>  Manage coherency.
	 */
	invalidate_icache();
	invalidate_dcache(UPTO_L1);
	/*
	 * Now we can allow cache events to filter down
	 * and not worry about the RAC getting confused.
	 */
	smp_on();

	_LOCK(me);

	icache_enable(1); /* At EL3 only */

	late_cpu_init();

	config->cpu[me].state = CPU_ON;

	if (config->debug) {
		msg_cpu("GO", me);
		__puts("@ ");
		writehex64(config->cpu[me].entry_point_address);
		puts("");
	}

	if (is_64) {
		cpu_boot64(config->cpu[me].entry_point_address,
				per_cpu_stacktop,
				config->cpu[me].context_id,
				&(config->cpu[me].lock));
	} else {
		cpu_boot32(config->cpu[me].entry_point_address,
				per_cpu_stacktop,
				config->cpu[me].context_id,
				&(config->cpu[me].lock));
	}
	
	backstop("EXEC_RTN!", me);
}


/* cpu_init -> per_cpu_start?? -> per_cpu_common_init ->
 * cpu_boot -> [entry_point_address]
 */
void __noreturn per_cpu_start32(uint64_t per_cpu_stacktop)
{
	per_cpu_common_init(per_cpu_stacktop, 0);
}


void __noreturn per_cpu_start64(uint64_t per_cpu_stacktop)
{
	per_cpu_common_init(per_cpu_stacktop, 1);
}


static uint32_t do_cpu_on(int me, uint64_t target_id,
				uint64_t ep_addr, uint64_t context_id)
{
	int rc, other;

	if (!affinity_ok(target_id))
		return PSCI_ERR_INVALID_PARAMETERS;

	other = get_cpu_idx(target_id);

	if (config->cpu[other].state == CPU_NOT_PRESENT)
		return PSCI_ERR_NOT_PRESENT;

	if (me == other)
		return PSCI_ERR_ALREADY_ON;

	if ((config->cpu[other].state == CPU_ON) ||
		(config->cpu[other].state == CPU_IDLE))
		return PSCI_ERR_ALREADY_ON;

	if (config->cpu[other].state != CPU_OFF)
		return PSCI_ERR_INTERNAL_FAILURE;

	config->cpu[other].entry_point_address = ep_addr;
	config->cpu[other].context_id = context_id;

	rc = power_up_cpu(other);
	if (rc != PSCI_SUCCESS) {
		config->cpu[other].state = CPU_FAIL_ON;
		return PSCI_ERR_INTERNAL_FAILURE;
	}
	
	if (config->debug)
		__puts("BOOT");

	if (is_aarch32_smc_exception()) {
		if (config->debug)
			puts("32");
		set_cpu_boot_addr(other, (uint64_t)cpu_init32);
	} else {
		if (config->debug)
			puts("64");
		set_cpu_boot_addr(other, (uint64_t)cpu_init64);
	}

	config->cpu[other].state = CPU_PEND_ON;

	BARRIER64(); /* Make sure all RDB writes are done */

	cpu_enable(other, 1); /* Away the other cpu goes! */

	return PSCI_SUCCESS;
}


static void do_system_off(unsigned int me)
{
	unsigned int i;

	for (i = 0; i < config->all_cpus; i++) {

		if (i == me)
			continue;

		if (config->cpu[i].state == CPU_NOT_PRESENT)
			continue;

		/* Unlike check_pending() if the cpu
		 * is not properly marked as being off
		 * we try to force it off anyway.
		 */
		if (config->cpu[i].state != CPU_OFF)
			poweroff_single_cpu(i);
	}

	/* We can't fully do the poweroff sequence for ourselves
	 * so just go into reset. Later on we could use S3 with
	 * no wakeup events set to put us in a lower power mode.
	 */
	cpu_enable(me, 0);
}


static void check_pending(unsigned int me)
{
	unsigned int i;

	for (i = 0; i < config->all_cpus; i++) {

		if (i == me) /* Can't do stuff for myself */
			continue;

		if (config->cpu[i].state == CPU_PEND_OFF) {
			poweroff_single_cpu(i);
		}
	}
}


static uint32_t do_affinity_info(uint64_t target_affinity,
				uint64_t lowest_affinity_level)
{
	int other;

	 /* From v1.0 onwards and in
	  * Linux cpu_psci_cpu_kill()
	  */
	if (lowest_affinity_level != 0)
		return PSCI_ERR_NOT_SUPPORTED;

	if (!affinity_ok(target_affinity))
		return PSCI_ERR_INVALID_PARAMETERS;

	other = get_cpu_idx(target_affinity); /* x1: target_affinity */

	/* Idling cores that have called CPU_SUSPEND
	 * are considered to be in the ON state.
	 * Sec 5.7.1 ARM DENC 0022C.
	 */
	if ((config->cpu[other].state == CPU_ON) ||
		(config->cpu[other].state == CPU_IDLE))
		return 0; /* ON */

	if (config->cpu[other].state == CPU_PEND_ON)
		return 2; /* ON_PENDING */

	return 1; /* OFF, FAIL or NOT_PRESENT states */
}


static int i_am_last_cpu_on(void)
{
	unsigned int i, ons = 0;

	for (i = 0; i < config->all_cpus; i++) {
		switch (config->cpu[i].state) {
		case CPU_ON:
		case CPU_IDLE:
		case CPU_PEND_ON: /* Just comming up */
			ons++;
			break;
		/* case CPU_OFF:
		 * case CPU_PEND_OFF:
		 * case CPU_FAIL_OFF:
		 * case CPU_FAIL_ON:
		 * case CPU_NOT_PRESENT:
		 */
		default:
			break;
		}
	}

	return (ons == 1);
}


static void __noreturn bakery_release_pend_off(int me)
{
	/* Once the lock is released another cpu
	 * can shut us down on its next PSCI call.
	 */
	config->cpu[me].state = CPU_PEND_OFF;

	/* relinquish exclusive PSCI access
	 * lock if we had it.
	 */
	config->locked_sticky = 0;

	rac_flush();

	smp_off_unlock(&config->cpu[me].lock);

	backstop("CPU_OFF_FAIL!", me);
}


static uint32_t do_cpu_off(int me)
{
	/* We can't fully do the poweroff sequence for ourselves
	 * so just go into reset. Later on we could use S3 with
	 * no wakeup events set to put us in a lower power mode.
	 * No need to release the lock as we're the last cpu up.
	 * NOTE: For S3 we need to dcache clean & inval to PoC.
	 */
	if (i_am_last_cpu_on())
		cpu_enable(me, 0);

	bakery_release_pend_off(me);

	/* We should not get here */
	return PSCI_ERR_INTERNAL_FAILURE;
}


static void acpi_enter_S3(int me)
{
	/* TODO FIXME TBD */

	if (config->debug) {
		msg_cpu("Pretend S3", me);
		dump_cpu_states(me);
	}

	bakery_release_pend_off(me);
}


static void cpu_standby(int me)
{
	uint64_t pending_interrupts;

	/* Check IRQ, FIQ or SError */
	get_isr_el1(pending_interrupts);
	isb64();

	/* Can't sleep - something went ping! */
	if (pending_interrupts)
		return;

	if (config->debug)
		msg_cpu("WFI", me);

	config->cpu[me].state = CPU_IDLE;

	_UNLOCK(me);
	/* Now wait for something to happen. May have
	 * sticky lock so could block other PSCI calls
	 * and ops until its woken up.
	 */
#if defined(CFG_EMULATION) || defined(CFG_FULL_EMULATION)
		while (1)
#else
		wfi()
#endif
			;
	/* Woken up */
	_LOCK(me);

	config->cpu[me].state = CPU_ON;
}


static uint32_t do_cpu_suspend(int me, uint64_t power_state,
				uint64_t ep_addr, uint64_t context_id)
{
	unsigned int last, level, type;

	type = POWER_STATE_PARM(power_state, TYPE);

	if (type == PSCI_POWER_STATE_TYPE_STANDBY) {
		cpu_standby(me);
		return PSCI_SUCCESS;
	}

	/* Power me down, or S3 it. */

	level = POWER_STATE_PARM(power_state, LEVEL);

	/* TBD if we need to support cluster powerdown. */
	if ((level == PSCI_POWER_STATE_LEVEL_CLUSTER) ||
			(level >= PSCI_POWER_STATE_LEVEL_UNDEF))
		return PSCI_ERR_INVALID_PARAMETERS;

	/* If we end up doing S3, remember
	 * where should resume.
	 */
	config->cpu[me].entry_point_address = ep_addr;
	config->cpu[me].context_id = context_id;

	last = i_am_last_cpu_on();
	if (!last) {
		if (level == PSCI_POWER_STATE_LEVEL_CORE)
			bakery_release_pend_off(me);

		/* We were not the last cpu running but
		 * system shutdown was requested.
		 */
		return PSCI_ERR_DENIED;
	}

	/* relinquish exclusive PSCI access
	 * lock if we had it.
	 */
	config->locked_sticky = 0;

	/* PSCI_POWER_STATE_LEVEL_SYSTEM or last cpu standing. */

	acpi_enter_S3(me);

	/* We should not get here. */

	return PSCI_ERR_INTERNAL_FAILURE;
}


static int boot_linux(uint64_t fn, uint64_t fdt,
			uint64_t linux_entry, uint64_t x4)
{
	int me = get_cpu_idx(get_mpidr());

	if (config->debug) {
		__puts("PSCI: ");
		if (fn == OEM_FUNC_EXEC64_EL3) {
			__puts("Secure monitor");
		} else {
			__puts("DTB @ ");
			writehex64(fdt);
			__puts(", Linux");
		}
		__puts(" entry @ ");
		writehex64(linux_entry);
		puts("");
	}

	/* smp: caches must be off before we leave or join */
	(void)disable_all_caches_and_mmu();
	invalidate_icache();
	invalidate_dcache(UPTO_L1);
	smp_on();
	late_cpu_init();

	_UNLOCK(me);
	BARRIER64(); /* Make sure we are all done here */

	/* https://www.kernel.org/doc/Documentation/arm64/booting.txt
	 * We can keep i-cache and SMP coherency on.
	 */
	if (fn == OEM_FUNC_EXEC64_EL3) {
#if CFG_TRUSTZONE_MON
		exec64_el3(fdt, linux_entry);
#endif
	} else if (fn == OEM_FUNC_EXEC64)
		exec64(fdt, linux_entry);
	else
		exec32(fdt, linux_entry);

	/* Should not return here after exec??() */
	return PSCI_ERR_INTERNAL_FAILURE;
}


void resume_from_s3(void)
{
	struct brcmstb_s3_params *params;
	uintptr_t addr;
	uint32_t flags;
	uint64_t spsr_el3, fn;

	if (!is_aarch32_smc_exception()) {
		if (config->debug)
			puts("PSCI: S3 bad resume: A64");
		goto failed_resume;
	}

	get_spsr_el3(spsr_el3);
	isb64();

	if ((spsr_el3 & A32_SPSR_MODE) != A32_SPSR_MODE_Supervisor) {
		if (config->debug) {
			__puts("PSCI: S3 bad resume, spsr: ");
			writehex64(spsr_el3);
			puts("");
		}
		goto failed_resume;
	}

	flags = AON_REG(AON_REG_MAGIC_FLAGS);
	if (flags & S3_FLAG_PSCI_BOOT) {

		fn = (flags & S3_FLAG_BOOTED64) ?
			OEM_FUNC_EXEC64 : OEM_FUNC_EXEC32;

		addr = AON_REG(AON_REG_CONTROL_LOW);
		addr |= shift_left_32((uintptr_t)AON_REG(AON_REG_CONTROL_HIGH));

		params = (struct brcmstb_s3_params *)addr;

		if (config->debug) {
			__puts("PSCI: S3 resume @ ");
			writehex(params->reentry);
			puts("");
		}

		(void)boot_linux(fn, 0 /* No DTB */, params->reentry, 0);
	}

	if (config->debug) {
		__puts("PSCI: S3 bad resume, flags: ");
		writehex(flags);
		puts("");
	}

failed_resume:
	reboot(); /* Backstop */
}


void psci_decode(uint64_t x0, uint64_t x1, uint64_t x2,	uint64_t x3,
			uint64_t x4)
{
	uint32_t rc = PSCI_ERR_NOT_SUPPORTED;
	uint64_t cpu;
	unsigned int me;

	cpu = get_mpidr();
	me = get_cpu_idx(cpu);

	/* OEM */
	if ((x0 == OEM_PSCI_UNLOCK) && rac_master()) {
		if (_HASLOCK(me)) {
			config->locked_sticky = 0;
			_UNLOCK(me);
		}
		rc = PSCI_SUCCESS;
		goto out;
	}

	if ((x0 == OEM_PSCI_LOCK) && rac_master()) {
		if (!_HASLOCK(me)) {
			_LOCK(me);
			config->locked_sticky = 1;
		}
		rc = PSCI_SUCCESS;
		goto out;
	}

	if (!_HASLOCK(me))
		_LOCK(me);

	switch (x0) {

	/* OEM */
	case OEM_PSCI_RAC_DISABLE:
		rc = rac_disable_and_flush();
		break;

	case OEM_PSCI_RAC_ENABLE:
		rc = rac_enable(x1);
		break;

	case OEM_FUNC_S3_RESUME:
		resume_from_s3();
		break;

	case OEM_FUNC_EXEC64:
	case OEM_FUNC_EXEC64_EL3:
	case OEM_FUNC_EXEC32:
		rc = PSCI_ERR_DENIED;
		if (me != 0)
			break;
		rc = boot_linux(x0, x2, x1, x4);
		/* May or may not return here */
		break;

	case OEM_FUNC_LEGACY_GIC_BYPASS:
		if (me == 0) {
			gic_bypass();
			rc = PSCI_SUCCESS;
		} else
			rc = PSCI_ERR_DENIED;
		break;

	/* v0.1 */
	case PSCI_CPU_SUSPEND:
	case PSCI_CPU_SUSPEND_32:
		rc = do_cpu_suspend(me, x1, x2, x3);
		break;

	case PSCI_CPU_OFF:
		rc = do_cpu_off(me);
		break;

	case PSCI_CPU_ON:
	case PSCI_CPU_ON_32:
		rc = do_cpu_on(me, x1, x2, x3);
		break;

	/* v0.2 */
	case PSCI_VERSION:
		rc = PSCI_INT_VERSION;
		break;

	case PSCI_AFFINITY_INFO:
	case PSCI_AFFINITY_INFO_32:
		rc = do_affinity_info(x1, x2);
		check_pending(me);
		break;

	case PSCI_MIGRATE_INFO_TYPE:
		/* Trusted OS is not present, or is MP capable.
		 * No need to use PSCI_MIGRATE.
		 * See: Sec 5.1.7, 5.9.1 ARM DENC 0022C.
		 */
		rc = 2;
		break;

	case PSCI_MIGRATE_INFO_UP_CPU:
	case PSCI_MIGRATE_INFO_UP_CPU_32:
		/* UNDEFINED return value if MIGRATE_INFO_TYPE
		 * returns 2, which it does.
		 * See: Sec 5.1.8 ARM DENC 0022C.
		 */
		break;

	case PSCI_MIGRATE:
	case PSCI_MIGRATE_32:
		/* Migration not requried.
		 * See: Sec 5.8.2 ARM DENC 0022C.
		 */
		rc = PSCI_ERR_NOT_SUPPORTED;
		break;

	case PSCI_SYSTEM_OFF:
		do_system_off(me);
		/* We should not get here */
		rc = PSCI_ERR_INTERNAL_FAILURE;
		break;

	case PSCI_SYSTEM_RESET:
		reboot();
		/* should not return */
		rc = PSCI_ERR_INTERNAL_FAILURE;
		break;

	/* v1.0 */

	case PSCI_PSCI_FEATURES:

	case PSCI_CPU_FREEZE:

	case PSCI_CPU_DEFAULT_SUSPEND:
	case PSCI_CPU_DEFAULT_SUSPEND_32:

	case PSCI_NODE_HW_STATE:
	case PSCI_NODE_HW_STATE_32:

	case PSCI_SYSTEM_SUSPEND: /* S3/suspend to ram (v1.0) */
	case PSCI_SYSTEM_SUSPEND_32:

	case PSCI_PSCI_SET_SUSPEND_MODE:

	case PSCI_PSCI_STAT_RESIDENCY:
	case PSCI_PSCI_STAT_RESIDENCY_32:

	case PSCI_PSCI_STAT_COUNT:
	case PSCI_PSCI_STAT_COUNT_32:
	default:
		break;
	}

	if (!config->locked_sticky)
		_UNLOCK(me);
out:
	eret64(rc, x4);
}
