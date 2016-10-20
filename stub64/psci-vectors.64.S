/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __ASSEMBLER__
#define __ASSEMBLER__
#endif

#include "aarch64/armv8-regs.h"
#include "aarch64/armv8-macros.h"
#include <config.h>
#include <psci.h>

#define PSCI_STACK_SIZE		2048
#define A32_FIXED_ONMODE	A32_SPSR_MODE_Hyp
#define A64_FIXED_ONMODE	A64_SPSR_MODE_EL2h


/* http://infocenter.arm.com/help/index.jsp?
 * topic=/com.arm.doc.den0024a/CHDEEDDC.html
 *
 * -> Each entry in the vector table is 16 instructions long,
 * i.e. 64 bytes, but we don't stuff instructions there
 * but simply jump to a handler.
 */
.macro	vec	label
.align	7
	b	\label
.endm


.section .init
.align 11

/* aarch64 EL3 handler for exceptions, especially 
 * System Management Mode (EL3) features. Can add PSCI later on.
 *
 * ==> VBAR_EL3 should have already been setup in FSBL
 *
 */
ENTRY_PROC(smm_start)
	/* Current EL with SP0 */
	vec	ex_unhandled	/* Synchronous */
	vec	ex_unhandled	/* IRQ/vIRQ  */
	vec	ex_unhandled	/* FIQ/vFIQ  */
	vec	ex_unhandled	/* SError/vSError  */

	/* Current EL with SPx  */
	vec	ex_unhandled	/* Synchronous */
	vec	ex_unhandled	/* IRQ/vIRQ  */
	vec	ex_unhandled	/* FIQ/vFIQ  */
	vec	ex_unhandled	/* SError/vSError  */

	/* Lower EL using AArch64  */
	vec	le_sync64	/* Synchronous (smc: psci etc.) */
	vec	ex_unhandled	/* IRQ/vIRQ  */
	vec	ex_unhandled	/* FIQ/vFIQ  */
	vec	ex_unhandled	/* SError/vSError  */

	/* Lower EL using AArch32  */
	vec	le_sync32	/* Synchronous  (smc: psci etc.) */
	vec	ex_unhandled	/* IRQ/vIRQ  */
	vec	ex_unhandled	/* FIQ/vFIQ  */
	vec	ex_unhandled	/* SError/vSError  */
END_PROC(smm_start)


.section .text
.align 4


.macro fail_param cop
	putc ':'
	mrs	x0, \cop
	barrier
	bl	writehex64
.endm

ENTRY_PROC(ex_unhandled)
	putc '?'
	putc 'U'
	b	die_msg
END_PROC(ex_unhandled)


ENTRY_PROC(fail)
	putc '?'
	putc 'F'
	b	die_msg
END_PROC(fail)


ENTRY_PROC(die_msg)
	putc '\r'
	putc '\n'
	putc 'E'
	putc '1'
	putc ' '
	fail_param	mpidr_el1
	fail_param	currentel
	fail_param	far_el1
	fail_param	spsr_el1
	fail_param	sctlr_el1
	fail_param	elr_el1
	putc '\r'
	putc '\n'

	putc '\r'
	putc '\n'
	putc 'E'
	putc '2'
	putc ' '
	fail_param	far_el2
	fail_param	spsr_el2
	fail_param	sctlr_el2
	fail_param	elr_el2

	putc '\r'
	putc '\n'
	putc 'E'
	putc '3'
	putc ' '
	fail_param	far_el3
	fail_param	spsr_el3
	fail_param	sctlr_el3
	fail_param	elr_el3

	putc '\r'
	putc '\n'

1:	/* The emulation tarmac log may stop short
	 * if wfi is encountered.
	 */
#ifndef CFG_FULL_EMULATION
	wfi
#endif
	b	1b
END_PROC(die_msg)


/* ---------------------------------------------------------------------
 * ABI call standard for the ARM 64-bit Architecture (AArch64)
 * Doc: ARM IHI 0055B, AArch64 ABI release 1.0, 22nd May 2013
 *
 * Reg		Special	Role
 * SP			The Stack Pointer.
 * x30		LR	The Link Register.
 * x29		FP	The Frame Pointer
 * x19...x28		CALLEE-SAVED REGISTERS
 * x18			Platform Reg, otherwise a temp register.
 * x17		IP1	Second intra-procedure-call temporary register
 * x16		IP0	First intra-procedure-call scratch register
 * x9...x15		Temporary registers
 * x8			Indirect result location register
 * x0...x7		Parameter/result registers
 * ---------------------------------------------------------------------
 * As we may ret to Linux save the registers and also
 * do it per cpu as any of them could call us.
 * x0...x3: params.
 *
 * SP_EL3 = STACK_TOP - (nKiB * (AFF0 + (AFF1 x NR_CPUS_PER_CLUSTER)))
 *
 */
.macro calc_per_cpu_stack
	mov	sp, x4		/* save x4 in the EL3 SP */
	msr	tpidr_el3, x5	/* save x5 in the EL3 sofware thread ID reg */

	/* The first element of struct psci_cfg @__config_info
	 * is "unsigned int nr_cpu" which is the MAX number of cpus
	 * per cluster. When we init this it should be zero and
	 * we set it up using cpu #0  - so a don't care for a very
	 * first call to function OEM_PSCI_INIT.
	 */
	ldr	x4, =__config_info	/* x4 = NR_CPUS_PER_CLUSTER */
	ldr	w4, [x4]
	and	x4, x4, #0xff		/* limit to h/w max clusters */

	mrs	x5, mpidr_el1		/* x5 = AFF1 */
	isb
	lsl	x5, x5, #48		/* mask to get AFF1 only */
	lsr	x5, x5, #56

	mul	x4, x4, x5		/* x4 = NR_CPUS_PER_CLUSTER x AFF1 */

	mrs	x5, mpidr_el1		/* x5 = AFF0 */
	isb
	and	x5, x5, #MPIDR_AFF0

	add	x4, x4, x5		/* x4 += AFF0 */

	mov	x5, #PSCI_STACK_SIZE
	mul	x4, x4, x5		/* x4 *= PSCI_STACK_SIZE (nKiB) */

	ldr	x5, =(PSCI_BASE + PSCI_SIZE)
	sub	x5, x5, x4		/* x5 = stacktop - x4 */

	mov	x4, sp		/* restore x4 */
	mov	sp, x5		/* WARN: spsel=1, so SP_EL3 used */
	mrs	x5, tpidr_el3	/* restore x5 */
	isb
	dsb	ish
.endm


.macro save_per_cpu_stack
	calc_per_cpu_stack

	xpush2	x29, x30
	xpush2	x27, x28
	xpush2	x25, x26
	xpush2	x23, x24
	xpush2	x21, x22
	xpush2	x19, x20
	xpush2	x17, x18
	xpush2	x15, x16
	xpush2	x13, x14
	xpush2	x11, x12
	xpush2	 x9, x10
	xpush2	 x7,  x8
	xpush2	 x5,  x6
	xpush2	 x3,  x4

	mov	x4, sp /* save for psci_decode() */
.endm


/* return from exception for c code
 * x0: Return code from PSCI
 * x1: Saved stack pointer of original params
 */
ENTRY_PROC(eret64)
	mov	 sp, x1 /* restore from psci_decode() */

	xpop2	 x3,  x4
	xpop2	 x5,  x6
	xpop2	 x7,  x8
	xpop2	 x9, x10
	xpop2	x11, x12
	xpop2	x13, x14
	xpop2	x15, x16
	xpop2	x17, x18
	xpop2	x19, x20
	xpop2	x21, x22
	xpop2	x23, x24
	xpop2	x25, x26
	xpop2	x27, x28
	xpop2	x29, x30
	eret
END_PROC(eret64)


ENTRY_PROC(get_mpidr)
	mrs	x0, mpidr_el1
	barrier
	ret
END_PROC(get_mpidr)


ENTRY_PROC(le_sync64)
	/*  sp & x4 now point to our per cpu stack */
	save_per_cpu_stack

	/*  BRCM */
	ldr	x5, =OEM_PSCI_INIT
	cmp	x0, x5
	b.eq	psci_init

	/*  PSCI */
	b	psci_decode
END_PROC(le_sync64)


ENTRY_PROC(le_sync32)
	/* Sanitize, in case upper 32 bits are set in aarch32 */
	and	x0, x0, #0x00000000ffffffff
	and	x1, x1, #0x00000000ffffffff
	and	x2, x2, #0x00000000ffffffff
	and	x3, x3, #0x00000000ffffffff
	b	le_sync64
END_PROC(le_sync32)


ENTRY_PROC(init_counters)
	/* Counter-timer Hypervisor Control register NS
	*  [EL1PCEN]=1 NS EL0/EL1 access to CNTP_CTL_EL0, CNTP_CVAL_EL0 &
	*      CNTP_TVAL_EL0 not trapped to EL2.
	*  [EL1PCTEN]=1 NS EL0/EL1 access to CNTPCT_EL0 not trapped to EL2
	*/
	mrs	x7, cnthctl_el2
	isb
	orr	x7, x7, #(CNTHCTL_EL2_EL1PCTEN | CNTHCTL_EL2_EL1PCEN)
	msr	cnthctl_el2, x7

	/* ---------------------------- */

	mov	x7, #0x1 /* Timers: interrupt enabled, masked */

	/* Counter-timer Hyp Physical Timer Control register,
	 * CNTHP_CTL (EL2, EL3) 
	 */
	mrs	x8, cnthp_ctl_el2
	isb
	bic     x8, x8, #3
	orr     x8, x8, x7
	msr	cnthp_ctl_el2, x8
	isb

	/* Counter-timer Physical Timer Control register
	 * CNTP_CTL(NS): EL2, EL3 (SCR.NS=1)
	 */
	mrs	x8, cntp_ctl_el0
	isb
	bic     x8, x8, #3
	orr     x8, x8, x7
	msr	cntp_ctl_el0, x8
	isb

	/* Counter-timer Virtual Timer Control register,
	 * CNTV_CTL  (EL<all>)
	 */
	mrs	x8, cntv_ctl_el0
	isb
	bic     x8, x8, #3
	orr     x8, x8, x7
	msr	cntv_ctl_el0, x8
	isb

	/* ---------------------------- */

	/* MIDR_EL1: Main ID Reg -> Virtualization Processor ID Reg
	 * MPIDR_EL1: Multiprocessor Affinity Reg -> Virtualization MP ID Reg
	 */
	mrs x7, midr_el1    /* Read only reg */
	mrs x8, mpidr_el1   /* Read only reg */
	isb

	msr vpidr_el2, x7
	msr vmpidr_el2, x8
	isb

	ret
END_PROC(init_counters)


/* x0: fdt
 * x1: linux_entry
 */
ENTRY_PROC(exec64)
	/* Secure Configuration Register.
	 *   [NS]   EL0 and EL1 cannot access sec memory.
	 *  [HCE]   HVC instructions are enabled at EL1 and above.
	 *   [RW]=1 Lower levels are *ALL* AArch64 <-- NOTE!
	 *  [SMD]=0 SMC instructions are enabled at EL1 and above.
	 */
	mov	x7, #SCR_RES4
	orr	x7, x7, #SCR_NS
	orr	x7, x7, #SCR_HCE
	orr	x7, x7, #SCR_RW
	msr	scr_el3, x7
	isb

	/* Bit #31: 0=Lower levels are all AArch32, 1=EL1 is AArch64 */
	mov	x7, #HCR_EL2_RW
	bl	hypervisor_off	/* uses: x7, x8 */
	bl	init_counters

	/* Where we start EL2 at. */
	ldr	x7, =do_exec

	/* mask IRQ, FIQ, system & debug exceptions
	in SPSR EL3 (SPSR_mon.) Step down to EL2 with
	exception SP determined by the EL.
	 */
	mov	x8, #(A64_SPSR_A | A64_SPSR_D | A64_SPSR_I | A64_SPSR_F | A64_SPSR_MODE_EL2h)

	/* drop to EL2 */
	drop_el3_to_elx	x8, x7

do_exec:
	mov	x7, x1	/* aarch64 executable (Linux) entry address */
	mov	x1, xzr
	mov	x2, xzr
	mov	x3, xzr
	barrier
	br	x7	/* go! */
END_PROC(exec64)


/* x0: fdt
 * x1: linux_entry
 */
ENTRY_PROC(exec32)
	mov	x2, x0
	/* Secure Configuration Register.
	 *   [NS]   EL0 and EL1 cannot access sec memory.
	 *  [HCE]   HVC instructions are enabled at EL1 and above.
	 *   [RW]=0 Lower levels are *ALL* AArch32 <-- NOTE!
	 *  [SMD]=0 SMC instructions are enabled at EL1 and above.
	 */
	mov	x7, #SCR_RES4
	orr	x7, x7, #SCR_NS
	orr	x7, x7, #SCR_HCE /* revisit? */
	bic	x7, x7, #SCR_RW
	msr	scr_el3, x7

	/* Bit #31: 0=Lower levels are all AArch32, 1=EL1 is AArch64 */
	mov	x7, #0
	bl	hypervisor_off	/* uses: x7, x8 */
	bl	init_counters

	/* mask IRQ, FIQ, system & debug exceptions
	 * and step down to HYP.
	 */
	mov	x8, #(A32_SPSR_MODE_Hyp | A32_SPSR_M4 | A32_SPSR_F | A32_SPSR_I | A32_SPSR_A)

	/* drop to HYP */
	drop_el3_to_elx	x8, x1

	/* backstop for fails */
	putc	'?'
	wfi

END_PROC(exec32)

/* ----------------------------------------------------
 * Setup and boot a cpu in either A32 or A64 modes
 * TBD: consolidate them
 * ---------------------------------------------------- */


ENTRY_PROC(cpu_init32)
	calc_per_cpu_stack
	mov	x0, sp
	b	per_cpu_start32
END_PROC(cpu_init32)


ENTRY_PROC(cpu_init64)
	calc_per_cpu_stack
	mov	x0, sp
	b	per_cpu_start64
END_PROC(cpu_init64)


/* x7: HCR_EL2 value to set
 */
ENTRY_PROC(hypervisor_off)
	/* Architectural Feature Access Control Register
	 *  [FPEN] bits [21:20] = b11 = no traps,
	 *  don't trap FP, SIMD (FPEN) or trace (TTA)
	 */
	mov	x8, #CPACR_FPEN_MASK
	msr	cpacr_el1, x8

	/* Hypervisor Configuration Register
	 * If HCR_EL2.TSC = SCR_EL3.SMD = 0, execution of an SMC
	 * instruction at EL1 or higher generates a Secure Monitor Call
	 * exception, using the EC value 0x87, that is taken to EL3.
	 * Note 1: When EL3 is using AArch32, this exception is taken to
	 * Monitor mode.
	 * Note 2: HCR_EL2_RW is set (lower levels are aarch64) and EL0
	 * is determined by the Execution state described in the current
	 *  process state when executing at EL0, i.e. match SCR_RW.
	 */
	msr	hcr_el2, x7

	/* SCTLR_EL2, System Control Register (EL2)
	 * aka HSCTLR in A32 mode.
	 * Everything (MMU, cache etc.) off with only RES1 bits set.
	 */
	ldr	x7, =SCTLR_EL2_RES1_BITS
	msr	sctlr_el2, x7

	/* Architectural Feature Trap Register (EL2) NS
	 * aka HCPTR in A32 mode.
	 * [TCPAC]=0 NS EL1 access to CPACR_EL1 & CPACR are not trapped to EL2
	 *   [TTA]=0 NS access to the trace registers are not trapped to EL2
	 *   [TFP]=0 EL0/EL1 SIMD & FP register access not trapped to EL2
	 */
	mrs	x7, cptr_el2
	ldr	x8, =~(CPTR_TFP | CPTR_TTA | CPTR_TCPAC)
	isb
	and	x7, x7, x8
	msr	cptr_el2, x7

	/* Architectural Feature Trap Register (EL3) NS=0, NS=1.
	 *  disable all traps (TFP, TTA & TCPAC) to EL3
	 */
	msr	cptr_el3, xzr

	/* Hypervisor System Trap Register
	* Disable coprocessor traps to EL2.
	*/
	msr	hstr_el2, xzr

	/* Monitor Debug Configuration Register (EL2)
	 * Disable traps to EL2, but preserve the HPMN value.
	 */
	mov	x7, #0x1f
	msr	mdcr_el2, x7

	/* Monitor Debug System Control Register
	 *  disable all monitor debug
	 */
	msr	mdscr_el1, xzr

	ret
END_PROC(hypervisor_off)


/* Setup a new cpu in A32/T32 mode.
 * x0: Lower EL boot address
 * x1: Default (mini) stack
 * x2: context_id
 * x3: *lock
 */
ENTRY_PROC(cpu_boot32)
	mov	x18, x0 /* R14 (LR) SVC */
	mov	x19, x1 /* R13 (SP) SVC */
	mov	x20, x2 /* R14 (LR) ABT */
	mov	x22, x3 /* R14 (LR) UND */

	/* 0 = use SP_EL0 at all exception levels,
	 * else SP_EL<LEVEL>
	 */
	mov     x0, #1
	msr     spsel, x0
	isb
	mov	sp, x19

	/* Secure Configuration Register.
	 *   [NS]   EL0 and EL1 cannot access sec memory.
	 *  [HCE]   HVC instructions are enabled at EL1 and above.
	 *   [RW]=0 Lower levels are *ALL* AArch32 <-- NOTE!
	 *  [SMD]=0 SMC instructions are enabled at EL1 and above.
	 */
	mov	x0, #SCR_RES4
	orr	x0, x0, #SCR_NS
	orr	x0, x0, #SCR_HCE
	msr	scr_el3, x0

	/* Setup stack for other ELs,
	 * only accesssable like this at EL3
	 */
	msr	sp_el2, x19
	msr	sp_el1, x19
	msr	sp_el0, x19
	mov	x13, x19 /* R13 USR */
	mov	x15, x19 /* R13 HYP */
	mov	x17, x19 /* R13 IRQ */
	mov	x21, x19 /* R13 ABT */
	mov	x23, x19 /* R13 UND */
	mov	x29, x19 /* R13 FIQ */

	/* uses: x0 x1 x2 x3 x4 */
	bl	gic_secure_init

	/* uses: x7, x8 */
	mov	x7, #0
	bl	hypervisor_off
	bl	init_counters

	/* Vector Base Address Register
	 *  vector base address for any exception that is taken to EL3
	 *
	 * ==> Must be 2KiB aligned as bits [10:0] = res0
	 *
	 */
	ldr	x0, =PSCI_BASE
	msr	vbar_el3, x0

	/* final step - switch to A32 mode */

	/* Saved Program Status Register (@ EL3)
	 *  [M3:0]   AArch32 mode
	 *    [M4]=1 Exception taken from AArch32
	 *     [T]=? Exception taken from A32 state (1=T32.)
	 *     [F]=1 FIQ  Exception masked
	 *     [I]=1 IRQ  Exception masked
	 *     [A]=1 Async Exception masked
	 *     [E]=0 Little endian
	 */
	mov	x1, #(A32_SPSR_M4 | A32_SPSR_F | A32_SPSR_I | A32_SPSR_A)

	/* T32 address? */
	tst	x18, #1
	b.eq	1f
	orr	x1, x1, #A32_SPSR_T
1:
	ldr	x4, =A32_FIXED_ONMODE
	orr	x1, x1, x4
	mov	x19, x1
#if 0
	mov	x0, x20 /* context id */
#else
	/* https://www.kernel.org/doc/Documentation/arm64/booting.txt */
	mov	x0, xzr
	mov	x1, xzr
	mov	x2, xzr
	mov	x3, xzr
#endif
	mov	w7, #0
	str	w7, [x22]
	/* Data Cache line Clean and Invalidate by VA to PoC */
	dc	civac, x22
	barrier

	drop_el3_to_elx	x19, x18 /* do it! */

	b	fail /* backstop for fails */
END_PROC(cpu_boot32)


/* Setup a new cpu in A64 mode.
 * x0: Lower EL boot address
 * x1: Default (mini) stack
 * x2: context_id
 * x3: *lock
 */
ENTRY_PROC(cpu_boot64)
	mov	x19, x0
	mov	x20, x1
	mov	x21, x2
	mov	x22, x3

	/* 0 = use SP_EL0 at all exception levels,
	 * else SP_EL<LEVEL>
	 */
	mov     x0, #1
	msr     spsel, x0
	isb
	mov	sp, x20

	/* Secure Configuration Register.
	 *   [NS]=1 EL0 and EL1 cannot access sec memory.
	 *  [HCE]=1 HVC instructions are enabled at EL1 and above.
	 *   [RW]=1 Lower levels are *ALL* AArch64 <-- NOTE!
	 *  [SMD]=0 SMC instructions are enabled at EL1 and above.
	 */
	mov	x7, #SCR_RES4
	orr	x7, x7, #SCR_NS
	orr	x7, x7, #SCR_HCE
	orr	x7, x7, #SCR_RW
	msr	scr_el3, x7

	/* Re-setup stack for other ELs,
	 * only accesssable like this at EL3
	 */
	mov	sp, x20
	msr	sp_el2, x20
	msr	sp_el1, x20
	msr	sp_el0, x20

	/* uses: x0 x1 x2 x3 x4 */
	bl	gic_secure_init

	/* uses: x7, x8 */
	mov	x7, #HCR_EL2_RW /* Needs to be set for EL1 */
	bl	hypervisor_off
	bl	init_counters

	/* Vector Base Address Register
	 *  vector base address for any exception that is taken to EL3
	 *
	 * ==> Must be 2KiB aligned as bits [10:0] = res0
	 *
	 *  We'll have to insert the vectory handler in SSBL
	 * as we can't (a) guarantee a space in FSBL for it
	 * due to memory limitations and reserved security adresses
	 * getting in the way, (b) FSBL gets wiped anyway just
	 * before we jump to SSBL.
	 */
	ldr	x0, =PSCI_BASE
	msr	vbar_el3, x0

	/* final step - switch to A64 ELx mode */
	mov	x4, #(A64_FIXED_ONMODE | A64_SPSR_A | A64_SPSR_D | A64_SPSR_I | A64_SPSR_F)
#if 0
	/* Power State Coordination Interface (PSCI)
	 * Document number: ARM DEN 0022C
	 */
	mov	x0, x21 /* context id */
#else
	/* https://www.kernel.org/doc/Documentation/arm64/booting.txt */
	mov	x0, xzr
	mov	x1, xzr
	mov	x2, xzr
	mov	x3, xzr
#endif

	mov	w7, #0
	str	w7, [x22]
	/* Data Cache line Clean and Invalidate by VA to PoC */
	dc	civac, x22
	barrier

	drop_el3_to_elx	x4, x19 /* do it! */

	b	fail /* backstop for fails */
END_PROC(cpu_boot64)


ENTRY_PROC(__smp_on)
	check_is_orion	x0, x1, 1f

	/* ACTLR_EL3 / ACTLR (S) */
	mrs     x0, actlr_el3
	isb
	orr     x0, x0, #0x2	/* Allow access to CPUECTLR from EL1 */
	msr     actlr_el3, x0

	/* ACTLR_EL2 / HACTLR */
	mrs     x0, actlr_el2
	isb
	orr     x0, x0, #0x2
	msr     actlr_el2, x0

	/* ACTLR_EL1 / ACTLR (NS) */
	mrs     x0, actlr_el1
	isb
	orr     x0, x0, #0x2
	msr     actlr_el1, x0

	mrs     x0, S3_1_C15_C2_1
	isb
	orr     x0, x0, #CPUECTLR_EL1_SMPEN	/* enable SMP bit */
	msr     S3_1_C15_C2_1, x0
1:
	barrier
	ret
END_PROC(__smp_on)


/* This is a tidied up disassemby of
 * _flush_cache(int level, int clean) built in A64
 * mode.
 *
 * x0: cache level (0=L1, 1=L2)
 * x1: 1=clean & invalidate, or 0=invalidate only.
 * uses x0 up to x10, depending upon what gcc
 * outputs.
 */
ENTRY_PROC(die_flush_cache)
	dmb	sy
	lsl	w0, w0, #1

	msr	csselr_el1, x0
	isb

	mrs	x2, ccsidr_el1
	isb

	ubfx	x4, x2, #3, #10
	ubfx	x8, x2, #13, #15
	and	w2, w2, #0x7
	add	w4, w4, #0x1
	mov	w3, #0x1f
	add	w2, w2, #0x2
	mov	w6, #0x4
	clz	w7, w4
	lsl	w6, w6, w2
	clz	w6, w6
	sub	w6, w3, w6
	sub	w3, w7, w3
	add	w8, w8, #0x1
	sxtw	x0, w0
	mov	x2, #0x0
	mov	w5, w4
	add	w7, w3, #0x20
6:
	cmp	x2, x5
	b.cs	1f

	lsl	x9, x2, x7
	orr	x9, x9, x0
	mov	x3, #0x0
5:
	cmp	x3, x8
	b.cs	2f

	lsl	x4, x3, x6
	orr	x4, x4, x9

	cbz	w1, 3f
	dc	cisw, x4
	b	4f
3:
	dc	isw, x4
4:
	add	x3, x3, #0x1
	b	5b

2:
	add	x2, x2, #0x1
	b	6b
1:
	isb
	dsb	sy
	ret
END_PROC(die_flush_cache)


/* Sequence as described in:
 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0500d/CACJFAJC.html
 * Section: "Individual processor shutdown mode"
 *
 * Additional paranoia about touching any memory (especially
 * the stack) while we do this shutdown process.
 *
 * x0: bakery *lock
 */
ENTRY_PROC(smp_off_unlock)
	mov	x11, x0
	mov	x12, #0

	check_is_orion	x0, x1, 1f
	mov	x12, #1
1:
	/* mmu & all caches off */
	mrs	x1, sctlr_el3
	isb
	bic	x1, x1, #SCTLR_EL2_M
	bic	x1, x1, #SCTLR_EL2_C
	bic	x1, x1, #SCTLR_EL2_I
	msr	sctlr_el3, x1
	barrier

	mov	x0, #0	/* flush to L1 */
	mov	x1, #1	/* clean & invalidate */
	bl	die_flush_cache

	/* Clear any dangling exclusive locks we
	 * might be holding.
	 */
	clrex

	cmp	x12, #1
	b.ne	2f	/* not an orion B53 */

	/* SMP off */
	mrs     x0, S3_1_C15_C2_1
	isb
	bic     x0, x0, #CPUECTLR_EL1_SMPEN /* disable SMP bit */
	msr     S3_1_C15_C2_1, x0
	isb
	dsb	ish

	/* ACTLR_EL3 / ACTLR (S) */
	mrs     x0, actlr_el3
	isb
	bic     x0, x0, #0x2	/* Deny access to CPUECTLR from EL1 */
	msr     actlr_el3, x0

	/* ACTLR_EL1 / ACTLR (NS) */
	mrs     x0, actlr_el1
	isb
	bic     x0, x0, #0x2
	msr     actlr_el1, x0
	isb

	/* ACTLR_EL2 / HACTLR */
	mrs     x0, actlr_el2
	isb
	bic     x0, x0, #0x2
	msr     actlr_el2, x0

2:
	/* bakery unlock, our ONLY non-instruction and
	 * non-RDB memory access.
	 */
	mov	w1, #0
	str	w1, [x11]
	barrier

	/* No point in doing a DCCIMVAC (Data Cache line Clean and Invalidate
	 * by VA to PoC) here as we are out of coherence. Let it
	 * filter donwn of its own accord. Have to revisit if we have
	 * cache & MMU on in MON mode, which is unlikely at the moment.
	 */
3:
	wfi
	b	3b
END_PROC(smp_off_unlock)

.end
