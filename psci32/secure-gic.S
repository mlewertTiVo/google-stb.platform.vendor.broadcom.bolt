/*
 * gic.S - Secure gic initialisation for stand-alone Linux booting
 *
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE.txt file.
 */

#ifndef __ASSEMBLER__
#define __ASSEMBLER__
#endif

#include "arm-macros.h"
#include "armv8.h"

#define MPIDR_ID_BITS (MPIDR_AFF2 | MPIDR_AFF1 | MPIDR_AFF0)

/* from sub add_gic($$$) in config.pl for arm,cortex-a15-gic
 
interrupt-controller @ 0xffd00000

	0x0000	0x0000 - 0x0fff Reserved
	0x1000	0x1000 - 0x1fff Distributor
	0x2000	0x2000 - 0x3fff CPU interface
	0x4000	0x4000 - 0x4fff Virtual interface control (common base address)
	0x5000	0x5000 - 0x5fff Virtual interface control (processor-specific base address)
	0x6000	0x6000 - 0x7fff Virtual CPU interface

	ref: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0438c/BGBJFJJA.html
 */

#define INT_BASE_MAYBE	0xffd00000
#define GIC_DIST_OFFS	0x1000
#define GIC_CPU_OFFS	0x2000

.section .text
.align   4

ENTRY_PROC(gic_secure_init) /* uses: r0-r6 */
	push	{r0-r6, lr}

	ldr	r5, =#GIC_DIST_OFFS
	ldr	r6, =#GIC_CPU_OFFS

	/* read CBAR_EL1::PERIPHBASE[39:18]
	 * http://infocenter.arm.com/help/index.jsp?\
	 * topic=/com.arm.doc.ddi0500e/BABEBDAI.html
	 * lo bits are res0 so no cleaning out to do[1]
	 */
	mrc	p15, 1, r4, c15, c3, 0 /* Read CBAR */
	ldr	r1, =#~0xff /* [7:0] PERIPHBASE[39:32] */
	isb
	and	r4, r4, r1

	/*
	 * Check for the primary CPU to avoid a race on the distributor
	 * registers.
	 */
	mrc	p15, 0, r0, c0, c0, 5 /* read mpidr */
	isb
	ldr	r1, =MPIDR_ID_BITS
	tst	r0, r1
	bne	1f		/* secondary CPU */

	mov	r1, r4
	orr	r1, r1, r5	/* GICD_CTLR */
	mov	r0, #3		/* EnableGrp0 | EnableGrp1 */
	str	r0, [r1]

1:
	mov	r1, r4
	orr	r1, r1, r5
	add	r1, r1, #0x80	/* GICD_IGROUPR */

	mov	r0, #~0		/* Grp1 interrupts */
	str	r0, [r1]
	bne	2f		/* Only local interrupts for secondary CPUs */

	mov	r2, r4
	orr	r2, r2, r5
	add	r2, r2, #0x04	/* GICD_TYPER */

	ldr     r3, [r2]
	ands    r3, r3, #0x1f	/* ITLinesNumber */
	beq    2f

loop:
	str     r0, [r1, #4]!
	subs    r3, r3, #1
	bne    loop

2:	mov	r1, r4
	orr	r1, r1, r6	/* GICC_CTLR */

	mov	r0, #3		/* EnableGrp0 | EnableGrp1 */
	str	r0, [r1]

	mov	r0, #(1 << 7)	/* allow NS access to GICC_PMR */
	str	r0, [r1, #4]	/* GICC_PMR */

	pop	{r0-r6, pc}
END_PROC(gic_secure_init)


#ifdef PSCI32_GIC_UNINIT
ENTRY_PROC(gic_secure_uninit) /* uses: r0-r6 */
       push    {r0-r6, lr}

       set_secure_mode r0, r0

       ldr     r5, =#GIC_DIST_OFFS
       ldr     r6, =#GIC_CPU_OFFS

       mrc     p15, 1, r4, c15, c3, 0 /* Read CBAR */
       ldr     r1, =#~0xff /* [7:0] PERIPHBASE[39:32] */
       isb
       and     r4, r4, r1

       /*
	* Check for the primary CPU to avoid a race on the distributor
	* registers.
	*/
       mrc     p15, 0, r0, c0, c0, 5 /* read mpidr */
       isb
       ldr     r1, =MPIDR_ID_BITS
       tst     r0, r1
       bne     1f	       /* secondary CPU */

       mov     r1, r4
       orr     r1, r1, r5      /* GICD_CTLR */
       mov     r0, #0	       /* !EnableGrp0 | !EnableGrp1 */
       str     r0, [r1]

1:
       mov     r1, r4
       orr     r1, r1, r5
       add     r1, r1, #0x80   /* GICD_IGROUPR */

       mov     r0, #0	       /* !Grp1 interrupts */
       str     r0, [r1]
       bne     2f	       /* Only local interrupts for secondary CPUs */

       mov     r2, r4
       orr     r2, r2, r5      /* GIC_DIST_OFFS */
       add     r2, r2, #0x04   /* GICD_TYPER */

       ldr     r3, [r2]
       ands    r3, r3, #0x1f   /* ITLinesNumber */
       beq    2f

uloop:
       str     r0, [r1, #4]!
       subs    r3, r3, #1
       bne    uloop

2:     mov     r1, r4
       orr     r1, r1, r6      /* GICC_CTLR */

       mov     r0, #0	       /* !EnableGrp0 | !EnableGrp1 */
       str     r0, [r1]

       mov     r0, #(0 << 7)   /* !allow NS access to GICC_PMR */
       str     r0, [r1, #4]    /* GICC_PMR */

       set_nonsecure_mode r0, r0

       pop     {r0-r6, pc}
END_PROC(gic_secure_uninit)
#endif /* PSCI32_GIC_UNINIT */
