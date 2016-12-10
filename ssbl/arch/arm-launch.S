/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/


/* declare variables we use here
*/
#include "arm-macros.h"
#include "initdata.h"

.section .data
.align 4
.arm

DECLARE_INITVAR(mem_textreloc)
    DECLARE_INITVAR(mem_textbase)
    DECLARE_INITVAR(mem_textsize)
    DECLARE_INITVAR(mem_totalsize)
    DECLARE_INITVAR(mem_topofmem)
    DECLARE_INITVAR(mem_heapstart)
    DECLARE_INITVAR(mem_bottomofmem)
    DECLARE_INITVAR(mem_datareloc)
    DECLARE_INITVAR(cpu_prid)


.section .text
.align 4


# r0 - start address for jump
# r1 - architecture i.d.  (0)
# r2 - ATAGS pointer (unused.) ATAGS can be at addr 0x100 as well. (0)
#     Default for BOLT is DTB only, we don't support ATAGS.
#
# r3 is unused

ENTRY_PROC(bolt_launch)
	/* use SRAM stack as this goes bye-bye on pop.
	 we could also save into other registers...
	 if careful!
	 */
	ldr		sp, =SRAM_STACK
	push	{r0-r3}
	bl		clear_all_d_cache
	pop	{r4-r7}
	bl		invalidate_all_i_cache
	bl		disable_caches
	mov	r0, r4
	mov	r1, r5
	mov	r2, r6
	mov	r3, r7
	mov 	pc, r0
END_PROC(bolt_launch)

ENTRY_PROC(bolt_launch_bsu)
	push	{lr}
	push	{r0-r3}
	barrier
	bl		clear_all_d_cache
	bl		invalidate_all_i_cache
	barrier
	pop		{r0-r3}
#  An app should save lr and sp on entry, then
# restore sp and pc (lr) to get back to
# our caller.
	mov 	pc, r0
END_PROC(bolt_launch_bsu)


#ifdef STUB64_START
#include "psci.h"
	/* r0: la->la_entrypt
	 * r1: 0xffffffff (unused)
	 * r2: (unsigned int)p.dt_address
	 * r3: 0 (unused)
	 *
	 * PSCI call
	 * r0: SMM_FUNC_EXEC64
	 * r1: Linux address
	 * r2: DTB address
	 *
	 * Linux64 gets (Rn == Wn)
	 * r0: DTB
	 * r1: zero
	 * r2: zero
	 * r3: zero
	 *
	 * Linux32 gets (Rn == Wn)
	 * r0: ?
	 * r1: architecture i.d
	 * r2: DTB
	 * r3: ?
	 */
ENTRY_PROC(psci_exec)
	ldr	sp, =SRAM_STACK
	push	{r0-r4}
	bl	clear_all_d_cache
	pop	{r4-r8}
	bl	invalidate_all_i_cache
	bl	disable_caches

	mov	r1, r4
	mov	r2, r6
	mov	r3, r7
	mov	r0, r8 /* PSCI func */
	isb
	bl	psci
1:	wfi	/* booting Linux: we should not get back here */
	b	1b
END_PROC(psci_exec)


ENTRY_PROC(bolt_launch64)
	ldr	r4, =OEM_FUNC_EXEC64
	b	psci_exec
END_PROC(bolt_launch64)


ENTRY_PROC(bolt_launch64_el3)
	ldr	r4, =OEM_FUNC_EXEC64_EL3
	b	psci_exec
END_PROC(bolt_launch64_el3)


ENTRY_PROC(bolt_launch32)
	ldr	r4, =OEM_FUNC_EXEC32
	b	psci_exec
END_PROC(bolt_launch32)


ENTRY_PROC(psci)
	smc	#0 /* do the PSCI call */
	isb
	bx	lr
END_PROC(psci)

#endif /* STUB64_START */
