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
#include <bchp_hif_cpubiuctrl.h>

.section .init
.align   4


/* This stub is to allow the cpu to boot in aarch64 mode
 * and then switch to aarch32
 */
ENTRY_PROC(stub_start)
	bl	1f
bootmode:
	.word 0x00000000
1:
#ifndef SECURE_BOOT
	/* see if we're running from SRAM (Boot 2.0)
	 * or flash (Boot 1.0)
	 */
	mov	x0, x30
	ldr	x3, =SRAM_ADDR
	ldr	x2, =0xffff0000 /* ~SRAM_ADDR */
	and	x0, x0, x2
	and	x1, x3, x2
	cmp	x0, x1
	beq	3f

	icache_op 1

	/* nope, need to copy FLASH -> SRAM
	 * x0 = flash (src)
	 * x3 = SRAM  (dest)
	 * x1 = SRAM  (dest end)
	 */
	ldr	x1, =SRAM_STACK
2:
	ldr	w2, [x0]
	str	w2, [x3]
	add	x0, x0, #0x4
	add	x3, x3, #0x4
	cmp	x3, x1
	b.ne	2b

	icache_op 0

	/* use "ldr" instead of "adr" to
	 * force an absolute SRAM address
	 */
	ldr	x0, =3f
	br	x0
3:
#endif /* SECURE_BOOT */

	/* now that we are in sram go mark this as
	 * an A64 boot so A32 FSBL has a clue.
	 */
	mov	w0, #0xffff0A53
	ldr	x1, =bootmode
	str	w0, [x1]

	/* zero out .bss
	 */
	mov	w4, #0
	ldr	x0, =_stub_zero_start
	ldr	x1, =_stub_zero_end
	sub	x2, x1, x0
	mov	x3, #0
4:
	cmp	x3, x2
	b.eq	5f
	strb    w4, [x0, x3]
	add	x3, x3, #1
	b	4b
5:
	/* commence early init */

	/* 0 = use SP_EL0 at all exception levels,
	 * else SP_EL<LEVEL>
	 */
	mov     x5, #1
	msr     spsel, x5
	isb

	/* --- setup stack --- */

	ldr	x5, =SRAM_STACK
	mov	sp, x5

	bl	uart_init

	putc	'A'

	/* Check EL3.
	 * If we're not in EL3 then its unhappy time.
	 */
	mrs	x9, CurrentEL
	and	x9, x9, #CURRENT_EL_MASK
	cmp	x9, #CURRENT_E3_MASK
	b.ne	not_in_el3

	putc	'3'

	/* --- EL3 register access only --- */

	/* Secure Configuration Register.
	 *   [NS]   EL0 and EL1 cannot access sec memory.
	 *  [HCE]   HVC instructions are enabled at EL1 and above.
	 *   [RW]=0 Lower levels are *ALL* AArch32 <-- NOTE!
	 *  [SMD]=0 SMC instructions are enabled at EL1 and above.
	 */
	mov	x0, #SCR_RES4
	orr	x0, x0, #SCR_NS
	orr	x0, x0, #SCR_HCE
	msr	scr_el3, x0 /* Secure Configuration Register */

	/* Architectural Feature Trap Register (EL3) NS=0, NS=1.
	 *  disable all traps (TFP, TTA & TCPAC) to EL3
	 */
	msr	cptr_el3, xzr

	/* setup stack for other ELs,
	 * only accesssable like this at EL3
	 */
	ldr	x5, =SRAM_STACK
	msr	sp_el2, x5
	msr	sp_el1, x5
	msr	sp_el0, x5


	/* --- general register access (>= EL2) --- */

	/* Architectural Feature Access Control Register
	 *  [FPEN] bits [21:20] = b11 = no traps,
	 *  don't trap FP, SIMD (FPEN) or trace (TTA)
	 */
	mov	x0, #CPACR_FPEN_MASK
	msr	cpacr_el1, x0

	/* Monitor Debug System Control Register
	 *  disable all monitor debug
	 */
	msr	mdscr_el1, xzr

#if defined(CONFIG_BCM7260A0)
	/* limit0: RBUS 0x0_FFD0_0000..0x0_FFFF_FFFF */
	ldr	x0, =(0xd0000000+BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_LLIMT0)
	ldr	w1, =0x00ffd000
	str	w1, [x0, #0]
#endif

	bl	gic_secure_init

	putc	'G'

	/* Hypervisor Configuration Register
	 *
	 * If HCR_EL2.TSC = SCR_EL3.SMD = 0, execution of an SMC
	 * instruction at EL1 or higher generates a Secure Monitor Call
	 * exception, using the EC value 0x17, that is taken to EL3.
	 * Note 1: When EL3 is using AArch32, this exception is taken to
	 * Monitor mode.
	 * Note 2: HCR_EL2_RW is unset (lower levels are aarch32)
	 */
	msr	hcr_el2, xzr

	/* --- drop to aarch32 --- */

	/* System Control Register
	 *  everything off with only RES1 bits set.
	 */
	ldr	x0, =SCTLR_EL2_RES1_BITS
	msr	sctlr_el2, x0

	/* final step - switch to A32 mode */
	putc	'E'

	/* Where we start aarch32 at. This should branch back again
	* to the reset vector but now we're in A32 mode so the code
	* path will now take us into FSBL proper.
	*/
	ldr	x0, =SRAM_ADDR

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
	ldr	x2, =PSCI_BASE
	msr	vbar_el3, x2
	isb

	/* Saved Program Status Register (@ EL3)
	 *  [M3:0]   AArch32 mode
	 *    [M4]=1 Exception taken from AArch32
	 *     [T]=0 Exception taken from A32 state (1=T32.)
	 *     [F]=1 FIQ  Exception masked
	 *     [I]=1 IRQ  Exception masked
	 *     [A]=1 Async Exception masked
	 *     [E]=0 Little endian
	 */
	mov	x1, #(A32_SPSR_MODE_Supervisor | A32_SPSR_M4 | A32_SPSR_F | A32_SPSR_I | A32_SPSR_A)

	/* do it!
	 */
	drop_el3_to_elx	x1, x0

	/* backstop for fails */
	putc	'?'
	wfi

not_in_el3:
	putc	'X'
	wfi
END_PROC(stub_start)

.end
