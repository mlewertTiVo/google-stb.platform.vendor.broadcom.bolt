/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __ARMV8_COP_H__
#define __ARMV8_COP_H__

#ifndef __ASSEMBLER__

#include <bitops.h>

#define _do_aarch64(XINSSR) __asm__ __volatile__ \
			(XINSSR "\n" : : : "memory")

#define _set_aarch64(X, INS) __asm__ __volatile__ \
			(INS ", %0\n" : : "r" (X) : "memory")

#define _get_copreg(X, COPREG) __asm__ __volatile__ \
			("mrs %0, " COPREG "\n": "=r" (X) : : "memory")

#define _set_copreg(X, COPREG) __asm__ __volatile__ \
			("msr " COPREG ", %0\n" : : "r" (X) : "memory")

#define isb64()	_do_aarch64("isb")
#define dsb64()	_do_aarch64("dsb sy")
#define dmb64()	_do_aarch64("dmb sy")
#define dsbish() _do_aarch64("dsb ish")

#define wfi()	_do_aarch64("wfi")
#define wfe()	_do_aarch64("wfe")
#define sev()	_do_aarch64("sev")

#define BARRIER64()	do { isb64(); dsb64(); } while (0)

/* instruction cache invalidate all to point of unification (PoU) */
#define ic_iallu()	_do_aarch64("ic iallu")

/* Instruction Cache Invalidate All to PoU, Inner Shareable */
#define ic_ialluis()	_do_aarch64("ic ialluis")

/* Data or unified Cache line Clean and Invalidate by VA to PoC [WO] 64 bit */
#define set_dccivac(X) _set_aarch64(X, "dc civac")

/*Data or unified Cache line Clean by VA to PoC [WO] 64 bit */
#define set_dccvac(X) _set_aarch64(X, "dc cvac")

/* Data or unified Cache line Invalidate by VA to PoC [WO] 64 bit */
#define set_dcivac(X) _set_aarch64(X, "dc ivac")

/* Data or unified Cache line Clean and Invalidate by Set/Way [WO] 64 bit */
#define set_dccisw(X) _set_aarch64(X, "dc cisw")

/* Data or unified Cache line Invalidate by Set/Way [WO] 64 bit */
#define set_dcisw(X) _set_aarch64(X, "dc isw")

/* Invalidate All entries in TLB for EL<n> */
#define tlbi_all_el1()   _do_aarch64("tlbi alle1")
#define tlbi_vmall_el1() _do_aarch64("tlbi vmalle1")
#define tlbi_all_el2()   _do_aarch64("tlbi alle2")
#define tlbi_all_el3()   _do_aarch64("tlbi alle3")

/*  TTBR - Translation Table Base Registers */
#define set_ttbr0_el3(X) _set_copreg(X, "ttbr0_el3")
#define set_ttbr0_el2(X) _set_copreg(X, "ttbr0_el2")
#define set_ttbr0_el1(X) _set_copreg(X, "ttbr0_el1")
#define set_ttbr1_el1(X) _set_copreg(X, "ttbr1_el1")
#define  get_ttbr0_el3(X) _get_copreg(X, "ttbr0_el3")
#define  get_ttbr0_el2(X) _get_copreg(X, "ttbr0_el2")
#define  get_ttbr0_el1(X) _get_copreg(X, "ttbr0_el1")
#define  get_ttbr1_el1(X) _get_copreg(X, "ttbr1_el1")

/* MAIR - Memory Attribute Indirection Registers (for long descriptors) */
#define set_mair_el3(X) _set_copreg(X, "mair_el3");
#define set_mair_el2(X) _set_copreg(X, "mair_el2");
#define set_mair_el1(X) _set_copreg(X, "mair_el1");

/* TCR - Translation Control Registers */
#define set_tcr_el3(X) _set_copreg(X, "tcr_el3")
#define set_tcr_el2(X) _set_copreg(X, "tcr_el2")
#define set_tcr_el1(X) _set_copreg(X, "tcr_el1")
#define  get_tcr_el3(X) _get_copreg(X, "tcr_el3")
#define  get_tcr_el2(X) _get_copreg(X, "tcr_el2")
#define  get_tcr_el1(X) _get_copreg(X, "tcr_el1")

/* SCTLR - System Control Register */
#define set_sctlr_el3(X) _set_copreg(X, "sctlr_el3")
#define set_sctlr_el2(X) _set_copreg(X, "sctlr_el2")
#define set_sctlr_el1(X) _set_copreg(X, "sctlr_el1")
#define  get_sctlr_el3(X) _get_copreg(X, "sctlr_el3")
#define  get_sctlr_el2(X) _get_copreg(X, "sctlr_el2")
#define  get_sctlr_el1(X) _get_copreg(X, "sctlr_el1")

/* misc */
#define  get_cbar_el1(X) _get_copreg(X, "S3_1_C15_C3_0")
#define  get_l2ctlr_el1(X) _get_copreg(X, "S3_1_C11_C0_2")

/* Reset Management Register */
#define set_rmr_el1(X) _set_copreg(X, "rmr_el1")
#define set_rmr_el2(X) _set_copreg(X, "rmr_el2")
#define set_rmr_el3(X) _set_copreg(X, "rmr_el3")

/* Vector Base Address Register  */
#define set_vbar_el1(X) _set_copreg(X, "vbar_el1")
#define set_vbar_el2(X) _set_copreg(X, "vbar_el2")
#define set_vbar_el3(X) _set_copreg(X, "vbar_el3")

#define get_vbar_el1(X) _get_copreg(X, "vbar_el1")
#define get_vbar_el2(X) _get_copreg(X, "vbar_el2")
#define get_vbar_el3(X) _get_copreg(X, "vbar_el3")

/* Reset Vector Base Address Register  (RO) */
#define get_rvbar_el1(X) _get_copreg(X, "rvbar_el1")
#define get_rvbar_el2(X) _get_copreg(X, "rvbar_el2")
#define get_rvbar_el3(X) _get_copreg(X, "rvbar_el3")

/* Exception Syndrome Register */
#define get_esr_el1(X) _get_copreg(X, "esr_el1")
#define get_esr_el2(X) _get_copreg(X, "esr_el2")
#define get_esr_el3(X) _get_copreg(X, "esr_el3")

/* Interrupt Status Register */
#define get_isr_el1(X) _get_copreg(X, "isr_el1")

/* Saved Program Status Register
 * Holds the saved process state when an exception is taken to ELx.
 */
#define get_spsr_el1(X) _get_copreg(X, "spsr_el1")
#define get_spsr_el2(X) _get_copreg(X, "spsr_el2")
#define get_spsr_el3(X) _get_copreg(X, "spsr_el3")

/* MIDR_EL1, Main ID Register */
#define get_midr_el1(X) _get_copreg(X, "midr_el1")

/* CPU Extended Control Register [RW] 64 bit */
#define get_cpuectlr_el1(X) _get_copreg(X, "S3_1_C15_C2_1")
#define set_cpuectlr_el1(X) _set_copreg(X, "S3_1_C15_C2_1")

/* Cache Size Selection Register [RW] 32 bit */
#define set_csselr_el1(X) _set_copreg(X, "csselr_el1")

/* Current Cache Size ID Register [RO] 32 bit */
#define get_ccsidr_el1(X) _get_copreg(X, "ccsidr_el1")

/* Cache Level ID Register [RO] 64 bit */
#define get_clidr_el1(X) _get_copreg(X, "clidr_el1")
#define set_clidr_el1(X) _set_copreg(X, "clidr_el1")

static inline uint32_t get_current_el(void)
{
	uint32_t n;

	_get_copreg(n, "CurrentEL");
	return (CURRENT_EL_MASK & n) >> CURRENT_EL_SHIFT;
}


#endif /* __ASSEMBLER__ */
#endif /* __ARMV8_COP_H__ */
