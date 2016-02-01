/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
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
	bl		invalidate_all_i_cache
	bl		disable_caches
	pop		{r0-r3}
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
