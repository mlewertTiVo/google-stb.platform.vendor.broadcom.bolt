/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#ifndef __ARMV8_H__
#define __ARMV8_H__

/* For use by A32 mode */

#define MPIDR_AFF0	0x000000ff
#define MPIDR_AFF1	0x0000ff00
#define MPIDR_AFF2	0x00ff0000
#define MPIDR_MT	0x01000000
#define MPIDR_U 	0x40000000

#define SCR_NS		(1 << 0) /* 0=cpu in S, 1=NS mode (!MON) */
#define SCR_IRQ		(1 << 1) /* 1=take IRQ in MON */
#define SCR_FIQ		(1 << 2) /* 1=take FIQ in MON */
#define SCR_EA		(1 << 3) /* 1=take ABT in MON */
#define SCR_FW		(1 << 4) /* A FIQ from NS state is !masked by CPSR.F, & taken to EL3 */
#define SCR_AW		(1 << 5) /* 0=Ext ABT taken from NS are !masked by CPSR.A, are taken to EL3 */
#define SCR_nET		(1 << 6) /* 0=Early termination permitted */
#define SCR_SCD		(1 << 7) /* 0=allow SMC */
#define SCR_HCE 	(1 << 8) /* 1=allow HVC */
#define SCR_SIF		(1 << 9) /* 0=sec state instruction fetches from NS mem allowed */
#define SCR_RES0	(2 << 10) /* 0=sec state instruction fetches from NS mem allowed */
#define SCR_TWI		(1 << 12) /* 0=don't trap WFI */
#define SCR_TWE 	(1 << 13) /* 0=don't trap WFE */

#define HSCTLR_M 	(1 << 0) /* 1=enable EL2 stage 1 address translation */
#define HSCTLR_A 	(1 << 1) /* 1=alignment fault checking enabled */
#define HSCTLR_C 	(1 << 2) /* 1=enable data and unified caches at EL2 */
#define HSCTLR_3RES1 	(1 << 3) /* reserved */
#define HSCTLR_4RES1 	(1 << 4) /* reserved */
#define HSCTLR_CP15BEN	(1 << 5) /* 1=enable dmb, dsb, and isb at EL2 */
#define HSCTLR_6RES0	(1 << 6) /* deleted */
#define HSCTLR_ITD	(1 << 7) /* 0=IT instructions available */
#define HSCTLR_SED	(1 << 8) /* 0='setend' instruction available */
#define HSCTLR_9RES0	(1 << 9) /* reserved */
#define HSCTLR_10RES0	(1 << 10) /* reserved */
#define HSCTLR_11RES1	(1 << 11) /* reserved */
#define HSCTLR_I	(1 << 12) /* 1=enable instruction cache @ EL2 */
#define HSCTLR_13RES0	(1 << 13) /* reserved */
#define HSCTLR_14RES0	(1 << 14) /* reserved */
#define HSCTLR_15RES0	(1 << 15) /* reserved */
#define HSCTLR_16RES1	(1 << 16) /* reserved */
#define HSCTLR_17RES0	(1 << 17) /* reserved */
#define HSCTLR_18RES1	(1 << 18) /* reserved */
#define HSCTLR_WXN	(1 << 19) /* 1=regions with wr perms are forced to XN */
#define HSCTLR_20RES0	(1 << 20) /* reserved */
#define HSCTLR_FI	(1 << 21) /* 1=Low interrupt latency configuration. Some performance features disabled. */
#define HSCTLR_22RES1	(1 << 22) /* reserved */
#define HSCTLR_23RES1	(1 << 23) /* reserved */
#define HSCTLR_24RES0	(1 << 24) /* reserved */
#define HSCTLR_EE	(1 << 25) /* 0=exception endian is little */
#define HSCTLR_26RES0	(1 << 26) /* reserved */
#define HSCTLR_27RES0	(1 << 27) /* reserved */
#define HSCTLR_28RES1	(1 << 28) /* reserved */
#define HSCTLR_29RES1	(1 << 29) /* reserved */
#define HSCTLR_TE	(1 << 30) /* 0=exceptions are taken in A32, 1=T32 */
#define HSCTLR_31RES0	(1 << 31) /* RES0 */

#define HSCTLR_RES1_BITS (HSCTLR_3RES1 | HSCTLR_4RES1 | HSCTLR_11RES1 | HSCTLR_16RES1 | HSCTLR_18RES1 | HSCTLR_22RES1 | HSCTLR_23RES1 | HSCTLR_28RES1 | HSCTLR_29RES1)


/* SCTLR(S):  EL1(S)      EL3 (SCR.NS=0)
 * SCTLR(NS): EL1(NS) EL2 EL3 (SCR.NS=1)
 *
 * SCTLR(NS) is mapped to A64 register SCTLR_EL1
 * SCTLR(S) can be mapped to A64 register SCTLR_EL3, but is not mandated.
 *
 * If EL3 is A32, there there are separate S & NS instances of this register.
 */
#define SCTLR_M 	(1 << 0) /* 1=enable EL1 & EL0 stage 1 address translation */
#define SCTLR_A 	(1 << 1) /* 1=alignment fault checking enabled */
#define SCTLR_C 	(1 << 2) /* 1=enable data and unified caches */
#define SCTLR_3RES1 	(1 << 3) /* reserved */
#define SCTLR_4RES1 	(1 << 4) /* reserved */
#define SCTLR_CP15BEN	(1 << 5) /* 1=enable dmb, dsb, and isb at EL0 & EL1 */
#define SCTLR_THEE	(1 << 6) /* 1=T32EE is enabled */
#define SCTLR_ITD	(1 << 7) /* 0=IT instructions available */
#define SCTLR_SED	(1 << 8) /* 0='setend' instruction available */
#define SCTLR_9RES0	(1 << 9) /* reserved */
#define SCTLR_10RES0	(1 << 10) /* reserved */
#define SCTLR_11RES1	(1 << 11) /* reserved */
#define SCTLR_I		(1 << 12) /* 1=enable instruction cache */
#define SCTLR_V		(1 << 13) /* 1=select high exception vectors */
#define SCTLR_14RES0	(1 << 14) /* reserved */
#define SCTLR_15RES0	(1 << 15) /* reserved */
#define SCTLR_nTWI	(1 << 16) /* 1=execute 'wfi' ins as normal, 0=undef @ EL0 */
#define SCTLR_17RES0	(1 << 17) /* reserved */
#define SCTLR_nTWE	(1 << 18) /* 1=execute 'wfe' ins as normal 0=undef @ EL0 */
#define SCTLR_WXN	(1 << 19) /* 1=regions with wr perms are forced to XN */
#define SCTLR_UWXN	(1 << 20) /* 1=regions with unprivilaged wr perms are forced to XN @ EL1 */
#define SCTLR_21RES0	(1 << 21) /* reserved */
#define SCTLR_22RES1	(1 << 22) /* reserved */
#define SCTLR_23RES1	(1 << 23) /* reserved */
#define SCTLR_24RES0	(1 << 24) /* reserved */
#define SCTLR_EE	(1 << 25) /* 0=exception endian is little */
#define SCTLR_26RES0	(1 << 26) /* reserved */
#define SCTLR_27RES0	(1 << 27) /* reserved */
#define SCTLR_TRE	(1 << 28) /* 1=TEX remap enable */
#define SCTLR_AFE	(1 << 29) /* short-descriptor translation table AP[0] mode */
#define SCTLR_TE	(1 << 30) /* 0=exceptions are taken in A32, 1=T32 */
#define SCTLR_31RES30	(1 << 31) /* RES0 */

#define SCTLR_RES1_BITS (SCTLR_3RES1 | SCTLR_4RES1 | SCTLR_11RES1 | SCTLR_22RES1 | SCTLR_23RES1)

#define HCPTR_0RES1	(0x3ff << 0) /* reserved */
#define HCPTR_TCP10	(1 << 10) /* 1=trap valid NS access to CP<n> to Hyp mode */
#define HCPTR_TCP11	(1 << 11) /* 1=trap valid NS access to CP<n> to Hyp mode */
#define HCPTR_12RES1	(3 << 12) /* reserved */
#define HCPTR_14RES0	(1 << 14) /* reserved */
#define HCPTR_TASE	(1 << 15) /* 1=trap valid NS access to SIMD functionality */
#define HCPTR_16RES0	(0x0f << 16) /* reserved */
#define HCPTR_TTA	(1 << 20) /* 1=trap trace functionality */
#define HCPTR_21RES0	(0x3ff << 21) /* reserved */
#define HCPTR_TCPAC	(1 << 31) /*  1=trap CPACR accesses */

#define HCPTR_RES1_BITS (HCPTR_0RES1 | HCPTR_12RES1)

#define NSACR_cp10	(1 << 10) /* 1=copro 10 access from any security state */
#define NSACR_cp11	(1 << 11) /* 1=copro 11 access from any security state */
#define NSACR_NSASEDIS	(1 << 15) /* 1=disable Non-secure Advanced SIMD functionality */
#define NSACR_NSTRCDIS	(1 << 20) /* 1=disable Non-secure access to CP14 trace registers */

#define CPACR_cp10	(3 << 20) /* b11=Full access */
#define CPACR_cp11	(3 << 22) /* b11=Full access */
#define CPACR_TRCDIS	(1 << 28) /* 1=disable CP14 access to trace registers */
#define CPACR_ASEDIS	(1 << 31) /* 1=disable Advanced SIMD functionality */

#define CNTHCTL_EL1PCTEN	(1 << 0) /* 1=CNTPCT access from NS EL1 & EL0 */
#define CNTHCTL_EL1PCEN 	(1 << 1) /* 1=NS CNTP_CVAL, CNTP_TVAL & CNTP_CTL access from NS EL1 & EL0 */
#define CNTHCTL_EVNTEN		(1 << 2) /* 1=EN event stream */
#define CNTHCTL_EVNTDIR 	(1 << 3) /* 1=1->0 transition of the trigger bit triggers an event, else 0->1 */
#define CNTHCTL_EVNTI		(0xf << 4) /* Select which bit (0 to 15) of CNTPCT or CNTVCT is the event stream trigger */

/* CPUECTLR_EL1 CPU Extended Control Register [RW] 64 bit */
#define CPUECTLR_EL1_RETENTION_CTL		(0x7 << 0)
#define CPUECTLR_EL1_ASIMD_FP_RETENTION_CTL	(0x7 << 3)
#define CPUECTLR_EL1_SMPEN			(0x1 << 6)

/* CNTKCTL, AKA CNTKCTL_EL1 */
#define CNTKCTL_EL0PCTEN	0x001 /* EL0 access to CNTPCT & CNTFRQ */
#define CNTKCTL_EL0VCTEN	0x002 /* EL0 access to CNTVCT & CNTFRQ */
#define CNTKCTL_EVNTEN		0x004 /* Enables the generation of an event stream */
#define CNTKCTL_EVNTDIR_MASK	0x0F0 /* Select event (0 to 15) trigger */
#define CNTKCTL_EL0VTEN		0x100 /* EL0 access to CNTV_CVAL, CNTV_CTL, and CNTV_TVAL */
#define CNTKCTL_EL0PTEN		0x200 /* EL0 access to CNTP_CVAL, CNTP_CTL, and CNTP_TVAL */

#ifdef __ASSEMBLER__

/* SCR.NS=0 */
.macro set_secure_mode tmp_reg, saveit_reg
	mrc	p15, 0, \tmp_reg, c1, c1, 0
	isb
	mov	\saveit_reg, \tmp_reg
	bic	\tmp_reg, \tmp_reg, #SCR_NS
	mcr	p15, 0, \tmp_reg, c1, c1, 0
	isb
.endm


/* SCR.NS=1 */
.macro set_nonsecure_mode tmp_reg, saveit_reg
	mrc	p15, 0, \tmp_reg, c1, c1, 0
	isb
	mov	\saveit_reg, \tmp_reg
	orr	\tmp_reg, \tmp_reg, #SCR_NS
	mcr	p15, 0, \tmp_reg, c1, c1, 0
	isb
.endm

/* SCR.NS=reg (up to you to make sure <reg> is preserved
 * or if tmp_reg == saveit_reg then that is a NOP.
 */
.macro revert_secure_mode saveit_reg
	mcr	p15, 0, \saveit_reg, c1, c1, 0
	isb
.endm


.macro check_is_orion midr tmp brfail
	/* MIDR_EL1 is architecturally mapped to AArch32 register MIDR */
	mrc	p15, 0, \midr, c0, c0, 0 /* Read MIDR */
	isb
	/* Mask off variable fields */
	ldr     \tmp, =#0xff0ffff0
	and     \midr, \midr, \tmp

	ldr     \tmp, =#0x420f1000 /* Orion B53 */
	cmp     \midr, \tmp
	bne    \brfail
.endm


/* debugging helper */
.macro probe an_int
	push	{r0-r8, lr}
	ldr	r0, =#'>'
	blx	uart_putc
	ldr	r0, =#\an_int
	blx	writeint
	ldr	r0, =#0x0d
	blx	uart_putc
	ldr	r0, =#0x0a
	blx	uart_putc
	barrier
	pop	{r0-r8, lr}
.endm


#else /* !__ASSEMBLER__ */


#include "arm/arm.h"
#include <stddef.h>
#include <bitops.h>

#define wfe()	{ __asm__ __volatile__ ("wfe" : : : "memory"); }
#define wfi()	{ __asm__ __volatile__ ("wfi" : : : "memory"); }
#define sev()	{ __asm__ __volatile__ ("sev" : : : "memory"); }
#define dsbish() { __asm__ __volatile__ ("dsb ish" : : : "memory"); }
#define clrex() { __asm__ __volatile__ ("clrex" : : : "memory"); }


static inline uint32_t get_mpidr(void)
{
	uint32_t mpidr;

	__asm__ __volatile__ (
		"mrc	p15, 0, %0, c0, c0, 5\n"
		"isb	sy\n"
		"dsb	sy\n"
		: "=r" (mpidr));

	return mpidr;
}

/* Interrupt Status Register
 * ISR (RO, EL1, EL2, EL3)
 */
static inline uint32_t get_isr(void)
{
	uint32_t pending_irq;

	__asm__ __volatile__ (
		"mrc	p15, 0, %0, c12, c1, 0\n"
		: "=r" (pending_irq));

	return pending_irq;
}

static inline void set_dccvac(uint32_t addr)
{
	/* DCCMVAC: Data cache CLEAN by address, PoC */
	__asm__ __volatile__(
		" dmb	sy\n"
		" mcr	p15, 0, %0, c7, c10, 1\n"
		: /* no outputs */
		: "r" (addr)
		: "memory");
}

static inline void set_dcivac(uint32_t addr)
{
	/* DCIMVAC: Data cache INVALIDATE by address, PoC */
	__asm__ __volatile__(
		" dmb	sy\n"
		" mcr	p15, 0, %0, c7, c6, 1\n"
		: /* no outputs */
		: "r" (addr)
		: "memory");
}

static inline void set_dccivac(uint32_t addr)
{
	/* DCCIMVAC: Data cache CLEAN and INVALIDATE by address, PoC */
	__asm__ __volatile__(
		" dmb	sy\n"
		" mcr	p15, 0, %0, c7, c14, 1\n"
		: /* no outputs */
		: "r" (addr)
		: "memory");
}

#endif /* __ASSEMBLER__ */

#endif /* __ARMV8_H__ */
