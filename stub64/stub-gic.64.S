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

#include "aarch64/armv8-regs.h"
#include "aarch64/armv8-macros.h"

#define MPIDR_ID_BITS	(0xff00ffffff)

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

ENTRY_PROC(gic_secure_init) /* uses: x0 x1 x2 x3 x4 */

	/* read CBAR_EL1::PERIPHBASE[39:18]
	 * http://infocenter.arm.com/help/index.jsp?\
	 * topic=/com.arm.doc.ddi0500e/BABEBDAI.html
	 * lo bits are res0 so no cleaning out to do[1]
	 */
	mrs	x4, S3_1_C15_C3_0
	ldr	x1, =0xffffff00 /* [7:0] PERIPHBASE[39:32] */
	isb
	and	x4, x4, x1

	/*
	 * Check for the primary CPU to avoid a race on the distributor
	 * registers.
	 */
	mrs	x0, mpidr_el1
	ldr	x1, =MPIDR_ID_BITS
	tst	x0, x1
	b.ne	1f		/* secondary CPU */

	mov	x1, x4
	orr	x1, x1, GIC_DIST_OFFS	/* GICD_CTLR */
	mov	w0, #3		/* EnableGrp0 | EnableGrp1 */
	str	w0, [x1]

1:
	mov	x1, x4
	orr	x1, x1, GIC_DIST_OFFS
	add	x1, x1, 0x80	/* GICD_IGROUPR */

	mov	w0, #~0		/* Grp1 interrupts */
	str	w0, [x1]
	b.ne	2f		/* Only local interrupts for secondary CPUs */

	mov	x2, x4
	orr	x2, x2, GIC_DIST_OFFS
	add	x2, x2, 0x04	/* GICD_TYPER */

	ldr     w3, [x2]
	ands    w3, w3, #0x1f	/* ITLinesNumber */
	b.eq    2f

loop:	str     w0, [x1, #4]!
	subs    w3, w3, #1
	b.ne    loop

2:	mov	x1, x4
	orr	x1, x1, GIC_CPU_OFFS	/* GICC_CTLR */

	mov	w0, #3		/* EnableGrp0 | EnableGrp1 */
	str	w0, [x1]

	mov	w0, #1 << 7	/* allow NS access to GICC_PMR */
	str	w0, [x1, #4]	/* GICC_PMR */

	ret
END_PROC(gic_secure_init)


ENTRY_PROC(gic_bypass)
	xpush2	 x0,  x1

	mrs	x0, S3_1_C15_C3_0	/* Read CBAR_EL1 */
	ldr	x1, =0xffffff00		/* [7:0] PERIPHBASE[39:32] */
	isb
	and	x0, x0, x1
	orr	x0, x0, GIC_CPU_OFFS	/* GICC_CTLR */

	mov	w1, #0			/* DisableGrp0 | DisableGrp1 */
	str	w1, [x0]

	xpop2	 x0,  x1
	ret
END_PROC(gic_bypass)
