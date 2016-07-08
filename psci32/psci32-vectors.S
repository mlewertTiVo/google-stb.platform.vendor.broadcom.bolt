/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#include "arm-macros.h"
#include "armv8.h"
#include "psci.h"

.section .init
.align 5
.arm


ENTRY_PROC(mon_vectors)
	b	 unhandled	@ reset
	b	 unhandled	@ undefined instruction
	b	 do_psci_smc	@ software interrupt
	b	 unhandled	@ data abort
	b	 unhandled	@ prefetch (instruction) abort
	b	 unhandled	@ hypercall
	b	 unhandled	@ interrupt
	b	 unhandled	@ fast interrupt
END_PROC(mon_vectors)


.section .text
.align 4
.arm


ENTRY_PROC(unhandled)
	sub	sp, #80

	/* fill out and push a 'struct arm_regs' instance on the stack */
	stmia	sp!, {r0-r12}		/* push r0 thru r12 */

	mrs	r0, CPSR
	mov	r1, r0			/* backup CPSR in r1 */
	bic	r0, #CPSR_MODE_MASK
	orr	r0, #MODE_MON
	msr	CPSR_c, r0
	mov	r4, sp
	msr	CPSR_c, r1		/* go back to exception mode */
	mov	r5, lr
	stmia	sp!, {r4-r5}		/* push SP_mon, LR_<mode> */

	mov	r2, r1
	bic	r2, #~CPSR_MODE_MASK
	mov	r3, #MODE_ABT
	cmp	r2, r3			/* if ABT, calculate faulting PC */
	subeq	r2, r5, #8		/* if ABT, PC = LR - 8 */
	movne	r2, pc

	stmia	sp!, {r2}		/* push PC */
	and	r2, #CPSR_MODE_MASK

	stmia	sp!, {r0}		/* push copy of CPSR */

	mrc	p15, 0, r0, c5, c0, 0	/* read DFSR */
	mrc	p15, 0, r1, c6, c0, 0	/* read DFAR */
	mrc	p15, 0, r2, c6, c0, 2	/* read IFAR */
	stmia	sp!, {r0-r2}		/* push DFSR, DFAR, IFAR */

	sub	sp, #80
	mov	r0, sp
	blx	armv7_dump_registers
1:
	wfi
	b	1b
END_PROC(unhandled)


ENTRY_PROC(do_psci_smc)
	mrc	p15, 0, r3, c0, c0, 5
	isb
	and	r3, r3, #MPIDR_AFF0
	ldr	sp, =#1024
	mul	r3, r3, sp
	ldr	sp, =(PSCI_BASE + PSCI_SIZE)
	sub	sp, sp, r3
	bic	sp, sp, #7 /* paranoia: 8-byte alignment for ABI compliance */
	barrier
	push	{r1-r12, lr}

	/*  BRCM */
	ldr	r5, =OEM_PSCI_INIT
	cmp	r0, r5
	bne	1f
	blx	psci_init
1:
	/*  PSCI */
	blx	psci_decode
END_PROC(do_psci_smc)


ENTRY_PROC(eret32)
	ldmfd	sp!, {r1-r12, pc}^
END_PROC(eret32)


/* r0: linux_entry
 * r1: fdt
 * r2: bakery lock
 * r3: 1=secondary cpu
 */
ENTRY_PROC(exec32)
	bic	sp, sp, #7
	push	{r0-r3}

	/* 8-byte alignment for ABI compliance */

	/* Non-Secure Access Control Register,
	 * NSACR (EL2 [RO], EL3.NS=1/0 [RW])
	 */
	ldr	r9, =#(NSACR_NSASEDIS | NSACR_NSTRCDIS)
	ldr	r8, =#(NSACR_cp10 |  NSACR_cp11)
	mrc	p15, 0, r7, c1, c1, 2 /* Read NSACR, aka cptr_el3 */
	isb
	bic	r7, r7, r9
	orr	r7, r7, r8
	mcr	p15, 0, r7, c1, c1, 2 /* Write NSACR, aka cptr_el3 */
	isb

	bl	arch_init_counters_s
	bl	gic_secure_init
	bl	processor_specific_init_s
	bl 	security_scr_config
	bl	hypervisor_off_s

	pop	{r0-r3}

	set_nonsecure_mode	r7, r7

	/* Switch to HYP with exceptions masked CPSR when we eret
	 * out of PSCI
	 *
	 * Note: https://www.kernel.org/doc/Documentation/arm/Booting
	 * On CPUs supporting the ARM instruction set, the entry must be
	 * made in ARM state, even for a Thumb-2 kernel.
	 */
	ldr	r7, =(CPSR_FIQ | CPSR_IRQ | CPSR_ASYNC | MODE_HYP)
        msr     spsr_cxfs, r7

	mov	r7, sp
	ldr	r8, =drop_el
	barrier
	movs	pc, r8
	nop

	.align 4
drop_el:
	mov	sp, r7
	barrier

	push	{r0-r3}

	/* System Control Register
	 * SCTLR(NS): EL1(NS) EL2 EL3 (SCR.NS=1)
	 * Note this is a banked register determined by SCR.NS 
	 *
	 * Remap all exceptions to VBAR by setting SCTLR.V = 0
	 * CP15 DMB, DSB & ISB barrier ops enabled for EL0 and EL1
	 */
	ldr	r7, =#(SCTLR_RES1_BITS | SCTLR_CP15BEN)
	bic	r7, #SCTLR_V
	mcr	p15, 0, r7, c1, c0, 0
	isb

	bl	arch_init_config
	bl	hypervisor_off_ns
	bl	arch_init_counters
	bl	processor_specific_init_ns

	pop	{r0-r3}

	mov	r8, r2 /* copy the lock */
	mov	r9, r0 /* Linux entry point */

	cmp	r3, #1
	beq	secondary

	/* Primary cpu */
	mov	r2, r1 /* fdt/dtb address */
	ldr	r1, =#0xffffffff /* architecture ID */
	b	1f

secondary:
	/* A secondary cpu */
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	mov	r3, #0
	b	1f
1:
	/* unlock */
	mov	r4, #0
	str	r4, [r8]
	/* Data Cache line Clean and Invalidate by VA to PoC */
	mcr	p15, 0, r8, c7, c14, 1 /* DC CIMVAC */
	barrier
	bx	r9
END_PROC(exec32)


ENTRY_PROC(security_scr_config)
	/* Secure Configuration Register,
	 * SCR EL1(S), EL3.NS=0/1
	 */
	mrc	p15, 0, r8, c1, c1, 0
	isb

	/* 0=IRQs taken IRQ mode
	 * 0=FIQs taken FIQ mode
	 * 0=External aborts taken in ABT mode
	 * 0=Disable early permitted
	 * 0=SMC executes normally in NS state, doing a Secure Monitor Call
	 * 0=Secure state instruction fetches from NS memory are permitted
	 * 0=don't trap wfi & wfe to EL3/MON
	 */
	ldr	r7, =#(SCR_IRQ | SCR_FIQ | SCR_EA | SCR_nET | SCR_SCD | SCR_SIF | SCR_TWI | SCR_TWE)
	bic	r8, r8, r7

	/* 1=AW ABT's !taken to MON. !SCR.EA || HCR.AMO = this bit has no effect
	 * 1=FW FIQ's !taken to MON. !SCR.FIQ || HCR.FMO = this bit no effect
	 * 1=Allow HVC instruction
	 */
	orr	r8, r8, #(SCR_AW | SCR_FW | SCR_HCE)

	mcr	p15, 0, r8, c1, c1, 0	/* Write SCR */
	isb

	bx	 lr
END_PROC(security_scr_config)


ENTRY_PROC(hypervisor_off_s)
	/* Architectural Feature Access Control Register,
	 * CPACR (EL1, EL2, EL3)
	 */
	ldr	r8, =#(CPACR_cp10 | CPACR_cp11)
	mcr	p15, 0, r8, c1, c0, 2 /* Write CPACR, aka cpacr_el1 */
	isb

	bx	 lr
END_PROC(hypervisor_off_s)


ENTRY_PROC(hypervisor_off_ns)
	/* Architectural Feature Access Control Register,
	 * CPACR (EL1, EL2, EL3)
	 */
	ldr	r8, =#(CPACR_cp10 | CPACR_cp11)
	mcr	p15, 0, r8, c1, c0, 2 /* Write CPACR, aka cpacr_el1 */

	/* Hypervisor Configuration Register,
	 * HCR (EL2, EL3.NS=1)
	 */
	mov	r8, #0
	mcr	p15, 4, r8, c1, c1, 0 /* Write HCR, aka hcr_el2 lower */
	mcr	p15, 4, r8, c1, c1, 4 /* Write HCR2, aka hcr_el2 upper */

	/* Hyp System Control Register,
	 * HSCTLR (EL2, EL3.NS=1)
	 *  - everything (MMU, cache etc.) off with only RES1 bits set.
	 */
	ldr	r7, =#(HSCTLR_RES1_BITS | HSCTLR_CP15BEN)
	mcr	p15, 4, r7, c1, c0, 0 /* Write HSCTLR, aka sctlr_el2 */

	/* Architectural Feature Trap Register,
	 * HCPTR (EL2, EL3.NS=1)
	 */
	ldr	r8, =#HCPTR_RES1_BITS
	mcr	p15, 4, r8, c1, c1, 2 /* Write HCPTR aka cptr_el2 */

	/* Hyp System Trap Register,
	 * HSTR (EL2, EL3.NS=1)
	 */
	mov	r8, #0
	mcr	p15, 4, r7, c1, c1, 3 /* Write HSTR */

	/* Debug Status and Control Register External View,
	 * DBGDSCRext (EL1, EL2, EL3)
	 */
	mov	r8, #0
	mcr	p14, 0, r8, c0, c2, 2 /* Write DBGDSCRext, aka mdscr_el1 */
	isb

	/* Hyp Vector Base Address Register,
	 * HVBAR (EL2, EL3 SCR.NS=1)
	 * Hyp is off, but map it in case somebody does
	 * the wrong thing and does a hyp call - we trap
	 * to 'unhandled' and the cpu dies.
	 */
	ldr	r8, =mon_vectors
	mcr	p15, 4, r8, c12, c0, 0 /* Write HVBAR */
	isb

	bx	 lr
END_PROC(hypervisor_off_ns)


ENTRY_PROC(processor_specific_init_s)
	check_is_orion	r7, r8, 1f

	/* ACTLR_EL3 can be mapped to AArch32 register
	 * ACTLR (S), but this is not mandated
	 */
	mrc	p15, 0, r7, c1, c0, 1 /* Read ACTLR */
	isb
	orr	r7, r7, #0x2 /* Allow access to CPUECTLR from EL1 */
	mcr	p15, 0, r7, c1, c0, 1 /* Write ACTLR */
	isb
1:
	bx	 lr
END_PROC(processor_specific_init_s)


ENTRY_PROC(processor_specific_init_ns)
	check_is_orion	r7, r8, 1f

	/* ACTLR_EL1 is mapped to AArch32 register
	 * ACTLR (NS)
	 */
	mrc	p15, 0, r7, c1, c0, 1 /* Read ACTLR */
	isb
	orr	r7, r7, #0x2
	mcr	p15, 0, r7, c1, c0, 1 /* Write ACTLR */

	/* ACTLR_EL2 is mapped to AArch32 register
	 * HACTLR
	 */
	mrc	p15, 4, r8, c1, c0, 1 /* Read HACTLR */
	isb
	orr	r8, r8, #0x2
	mcr	p15, 4, r8, c1, c0, 1 /* Write HACTLR */
	isb
1:
	bx	 lr
END_PROC(processor_specific_init_ns)


ENTRY_PROC(arch_init_config)
    	push	{r0-r2, lr}

	mrc	p15, 0, r0, c1, c0, 0
	isb

	/* Enable branch prediction (SCTLR.Z) */
	orr	r0, #(1 << 11)

	bic	r0, r0, #SCTLR_C /* disable d-cache */
	bic	r0, r0, #SCTLR_M /* disable mmu */
	mcr	p15, 0, r0, c1, c0, 0
	barrier

    	pop	{r0-r2, pc}
END_PROC(arch_init_config)


ENTRY_PROC(arch_init_counters_s)
 	push	{r0-r4, lr}

	mov	r1, #0x1 /* Timers: interrupt enabled, !masked */

	/* Counter-timer Physical Timer Control register
	 * CNTP_CTL(S): EL1(S), EL3 (SCR.NS=1)
	 */
	mrc	p15, 0, r0, c14, c2, 1
	isb
	bic	r0, r0, #3
	orr	r0, r0, r1
	mcr	p15, 0, r0, c14, c2, 1
	isb

    	pop	{r0-r4, pc}
END_PROC(arch_init_counters_s)


/* Set counters and ID
 * Must be called with SCR.NS=1
 */
ENTRY_PROC(arch_init_counters)
   	push	{r0-r4, lr}

	/* Counter-timer Hyp Control register,
	 * CNTHCTL (EL2, EL3 SCR.NS=1)
	 */
	mrc	p15, 4, r0, c14, c1, 0 /* Read CNTHCTL, aka cnthctl_el2 */
	isb
	orr	r0, r0, #(CNTHCTL_EL1PCTEN | CNTHCTL_EL1PCEN)
	mcr	p15, 4, r0, c14, c1, 0 /* Write CNTHCTL */

	/* ---------------------------- */

	mov	r1, #0x1 /* Timers: interrupt enabled, masked */

	/* Counter-timer Hyp Physical Timer Control register,
	 * CNTHP_CTL (EL2, EL3.NS=0) 
	 */
	mrc     p15, 4, r0, c14, c2, 1 /* Read CNTHP_CTL aka     */
	isb
	bic     r0, r0, #3
	orr     r0, r0, r1
	mcr     p15, 4, r0, c14, c2, 1 /* Write CNTHP_CTL */
	isb

	/* Counter-timer Physical Timer Control register
	 * CNTP_CTL(NS): EL2, EL3 (SCR.NS=1)
	 */
	mrc	p15, 0, r0, c14, c2, 1
	isb
	bic	r0, r0, #3
	orr	r0, r0, r1
	mcr	p15, 0, r0, c14, c2, 1
	isb

	/* Counter-timer Virtual Timer Control register,
	 * CNTV_CTL  (EL<all>)
	 */
	mrc	p15, 0, r0, c14, c3, 1 /* Write CNTV_CTL */
	isb
	bic	r0, r0, #3
	orr	r0, r0, r1
	mcr	p15, 0, r0, c14, c3, 1 /* Write CNTV_CTL */
	isb

	/* ---------------------------- */

	mov	r0, #0
	mcrr	p15, 4, r0, r0, c14 /* Write 0x0 to CNTVOFF (EL2, EL3.NS=1) */
	isb

	mrc	p15, 0, r2, c0, c0, 0 /* Read MIDR */
	isb
	mcr	p15, 4, r2, c0, c0, 0 /* Write VPIDR (EL2, EL3.NS=1) */

	mrc	p15, 0, r1, c0, c0, 5 /* Read MPIDR */
	isb
	mcr	p15, 4, r1, c0, c0, 5 /* Write VMPIDR (EL2, EL3.NS=1)  */

    	pop	{r0-r4, pc}
END_PROC(arch_init_counters)


/* Boot secondary CPU and is expected to start in SVC_s mode. */
ENTRY_PROC(cpu_init_secondary)
	set_secure_mode	r7, r7 /* make sure */
	
	/* A2 errata: Dummy write to L2CTLR to propagate modified default
	 * register settings to L2 consuming logic
	 */
	mrc	p15, 1, r2, c9, c0, 2
	mcr	p15, 1, r2, c9, c0, 2

	ldr	r0, =(PSCI_BASE + PSCI_SIZE)
	ldr	r6, =#1024

	mrc	p15, 0, r7, c0, c0, 5
	isb
	and	r7, r7, #MPIDR_AFF0

	mul	r6, r6, r7	/* 1024 x MPIDR */
	sub	r0, r0, r6	/* stack top - (1024 x MPIDR) */
	bic	r0, r0, #7	/* 8-byte alignment for ABI compliance */
	mov	sp, r0

	/* System Control Register
	 * SCTLR(S) EL1(S), EL3 (SCR.NS=0)
	 * Note this is a banked register determined by SCR.NS 
	 *
	 * Leave I-cache enable to per_cpu_start() - after smp_on()
	 * Remap all exceptions to VBAR by setting SCTLR.V = 0
	 * CP15 DMB, DSB & ISB barrier ops enabled for EL0 and EL1
	 */
	ldr	r2, =#(SCTLR_RES1_BITS | SCTLR_CP15BEN)
	bic	r2, #SCTLR_V
	mcr	p15, 0, r2, c1, c0, 0
	isb

	/* Setup VBAR */
	ldr	r8, =mon_vectors
	mcr	p15, 0, r8, c12, c0, 0

	/* CPSR to apply mode mask to */
	mrs	r1, CPSR

	set_mode_sp	r1, r0, MODE_FIQ
	set_mode_sp	r1, r0, MODE_IRQ
	set_mode_sp	r1, r0, MODE_ABT
	set_mode_sp	r1, r0, MODE_UND
	set_mode_sp	r1, r0, MODE_SYS
	set_mode_sp	r1, r0, MODE_SVC

	/* Switch mode & setup stack for smc */
	set_mode_sp	r1, r0, MODE_MON

	/* MVBAR setup is valid only if EL3 is AArch32 
	 * EL1(S), EL3 SCR.NS=0|1
	 */
	mcr	p15, 0, r8, c12, c0, 1
	isb

	blx	per_cpu_start
END_PROC(cpu_init_secondary)


ENTRY_PROC(smp_on)
	push	{r0-r4}
	check_is_orion	r0, r1, 1f

	set_secure_mode	r0, r4

	/* ACTLR_EL3 can be mapped to AArch32 register
	 * ACTLR (S), but this is not mandated
	 */
	mrc	p15, 0, r0, c1, c0, 1 /* Read ACTLR */
	isb
	bic	r0, r0, #0x2 /* Allow access to CPUECTLR from EL1 */
	mcr	p15, 0, r0, c1, c0, 1 /* Write ACTLR */
	isb

	set_nonsecure_mode r0, r0

	/* ACTLR_EL2 is mapped to AArch32 register
	 * HACTLR
	 */
	mrc	p15, 4, r0, c1, c0, 1 /* Read HACTLR */
	isb
	bic	r0, r0, #0x2
	mcr	p15, 4, r0, c1, c0, 1 /* Write HACTLR */
	isb

	/* ACTLR_EL1 is mapped to AArch32 register
	 * ACTLR (NS)
	 */
	mrc	p15, 0, r0, c1, c0, 1 /* Read ACTLR */
	isb
	bic	r0, r0, #0x2
	mcr	p15, 0, r0, c1, c0, 1 /* Write ACTLR */
	isb

	revert_secure_mode r4

	mrrc	p15, 1, r0, r1, c15 /* Read CPU Extended Control Register */
	isb
	orr	r0, r0, #CPUECTLR_EL1_SMPEN
	mrrc	p15, 1, r0, r1, c15 /* Write */
	barrier
1:
	pop	{r0-r4}
	bx	lr
END_PROC(smp_on)


/* This is a tidied up disassemby of
 * _flush_cache(int level, int do_clean) built in A32
 * mode. Stack usage has been removed and ldr/str have
 * been worked around.
 *
 * r0: cache level (L1, L2)
 * r1: clean & invalidate, or invalidate only.
 * uses r0...r9
 */
ENTRY_PROC(die_flush_cache)
	dmb	sy
	/* cache = ((level - 1) << 1) | (icache ? 0x1 : 0x0) */
	sub	r0, r0, #1
	lsl	r0, r0, #1

	/* CSSELR, Cache Size Selection Register */
	mcr	15, 2, r0, c0, c0, 0
	isb	sy

	/* CCSIDR, Current Cache Size ID Register */
	mrc	15, 1, r3, c0, c0, 0

	and	r8, r3, #7
	mov	r2, #4
	add	r8, r8, #2
	ubfx	r5, r3, #13, #15
	add	r5, r5, #1
	ubfx	r9, r3, #3, #6
	lsl	r8, r2, r8
	add	r9, r9, #1
	mov	r3, #0
	clz	r6, r9
	mul	r5, r5, r8
	add	r6, r6, #1
 4:
	lsl	r7, r3, r6
	mov	r2, #0
 3:
	orr	r4, r2, r0
	orr	r4, r4, r7

	cmp	r1, #0
	beq	1f

	/* DC CISW */
	mcr	15, 0, r4, c7, c14, 2
	b	2f
 1:
 	/* DC ISW */
	mcr	15, 0, r4, c7, c6, 2
 2:
 	isb	sy
	add	r2, r2, r8
	cmp	r2, r5
	bcc	3b

	add	r3, r3, #1
	cmp	r3, r9
	bcc	4b

	barrier
	bx	lr
END_PROC(die_flush_cache)

/* Sequence as described in:
 * http://infocenter.arm.com/help/
 * index.jsp?topic=/com.arm.doc.ddi0464d/CACJFAJC.html
 * Section: "Individual processor shutdown mode"
 *
 * Additional paranoia about touching any memory (especially
 * the stack) while we do this shutdown process.
 *
 * r0: bakery *lock
 */
ENTRY_PROC(smp_off_unlock)
	mov	r10, r0
	mov	r12, #0

	check_is_orion	r1, r2, 1f
	mov	r12, #1
1:
	/* mmu & all caches off */
	mrc	p15, 0, r1, c1, c0, 0
	isb
	bic	r1, #(SCTLR_M | SCTLR_C)
	bic	r1, #SCTLR_I
	mcr	p15, 0, r1, c1, c0, 0
	barrier

	mov	r0, #1	/* flush to L1 */
	mov	r1, #1	/* clean & invalidate */
	bl	die_flush_cache

	/* Clear any dangling exclusive locks we
	 * might be holding.
	 */
	clrex

	cmp	r12, #1
	bne	2f	/* not an orion B53 */

	/* SMP off */
	mrrc	p15, 1, r7, r8, c15 /* Read CPU Extended Control Register */
	isb
	bic	r7, r7, #CPUECTLR_EL1_SMPEN
	mrrc	p15, 1, r7, r8, c15 /* Write */
	barrier

	set_secure_mode	r8, r8

	/* ACTLR_EL3 can be mapped to AArch32 register
	 * ACTLR (S), but this is not mandated
	 */
	mrc	p15, 0, r7, c1, c0, 1	/* Read ACTLR */
	isb
	bic	r7, r7, #0x2		/* Deny access to CPUECTLR from EL1 */
	mcr	p15, 0, r7, c1, c0, 1	/* Write ACTLR */
	isb

	set_nonsecure_mode r8, r8

	/* ACTLR_EL1 is mapped to AArch32 register
	 * ACTLR (NS)
	 */
	mrc	p15, 0, r7, c1, c0, 1	/* Read ACTLR */
	isb
	bic	r7, r7, #0x2		/* Deny access */
	mcr	p15, 0, r7, c1, c0, 1	/* Write ACTLR */
	isb

	/* ACTLR_EL2 is mapped to AArch32 register
	 * HACTLR
	 */
	mrc	p15, 4, r7, c1, c0, 1	/* Read HACTLR */
	isb
	bic	r7, r7, #0x2		/* Deny access */
	mcr	p15, 4, r7, c1, c0, 1	/* Write HACTLR */
	isb

	/* bakery unlock, our ONLY non-instruction and
	 * non-RDB memory access.
	 */
	mov	r1, #0
	str	r1, [r10]
	barrier

	/* No point in doing a DCCIMVAC (Data Cache line Clean and Invalidate
	 * by VA to PoC) here as we are out of coherence. Let it
	 * filter donwn of its own accord. Have to revisit if we have
	 * cache & MMU on in MON mode, which is unlikely at the moment.
	 */
2:
	wfi
	b	2b
END_PROC(smp_off_unlock)

.end
