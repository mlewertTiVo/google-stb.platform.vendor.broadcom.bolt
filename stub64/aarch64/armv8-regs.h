/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __ARMV8_REGS_H__
#define __ARMV8_REGS_H__

/* SCTLR, System Control Register
SCTLR(NS) mapped to AArch64 register SCTLR_EL1
 EL1 (NS)
 EL2
 EL3 (SCR.NS=1)
SCTLR(S) may be mapped to SCTLR_EL3
 EL1 (S)
 EL3 (SCR.NS=0)
*/

/* SCTLR */
#define SCTLR_M 	(1 << 0) /* 1=enable EL1 & EL0 stage 1 address translation */
#define SCTLR_A 	(1 << 1) /* 1=alignment fault checking enabled */
#define SCTLR_C 	(1 << 2) /* 1=enable data and unified caches */
#define SCTLR_RES3 	(1 << 3) /* reserved */
#define SCTLR_RES4 	(1 << 4) /* reserved */
#define SCTLR_CP15BEN	(1 << 5) /* 1=enable dmb, dsb, and isb at EL0 & EL1 */
#define SCTLR_THEE	(1 << 6) /* deleted */
#define SCTLR_ITD	(1 << 7) /* 0=IT instructions available */
#define SCTLR_SED	(1 << 8) /* 0='setend' instruction available */
#define SCTLR_RES9	(1 << 9) /* reserved */
#define SCTLR_RES10	(1 << 10) /* reserved */
#define SCTLR_RES11	(1 << 11) /* reserved */
#define SCTLR_I		(1 << 12) /* 1=enable instruction cache */
#define SCTLR_V		(1 << 13) /* 1=select high exception vectors */
#define SCTLR_RES14	(1 << 14) /* reserved */
#define SCTLR_RES15	(1 << 15) /* reserved */
#define SCTLR_nTWI	(1 << 16) /* 1=execute 'wfi' ins as normal, 0=undef @ EL0 */
#define SCTLR_RES17	(1 << 17) /* reserved */
#define SCTLR_nTWE	(1 << 18) /* 1=execute 'wfe' ins as normal 0=undef @ EL0 */
#define SCTLR_WXN	(1 << 19) /* 1=regions with wr perms are forced to XN */
#define SCTLR_UWXN	(1 << 20) /* 1=regions with unprivilaged wr perms are forced to XN @ EL1 */
#define SCTLR_RES21	(1 << 21) /* reserved */
#define SCTLR_RES22	(1 << 22) /* reserved */
#define SCTLR_RES23	(1 << 23) /* reserved */
#define SCTLR_RES24	(1 << 24) /* reserved */
#define SCTLR_EE	(1 << 25) /* 0=exception endian is little */
#define SCTLR_RES26	(1 << 26) /* reserved */
#define SCTLR_RES27	(1 << 27) /* reserved */
#define SCTLR_TRE	(1 << 28) /* 1=TEX remap enable */
#define SCTLR_AFE	(1 << 29) /* short-descriptor translation table AP[0] mode */
#define SCTLR_TE	(1 << 30) /* 0=exceptions are taken in A32, 1=T32 */
#define SCTLR_RES31	(1 << 31) /* RES0 */

/* SCTLR EL1 */
#define SCTLR_EL1_M		SCTLR_M
#define SCTLR_EL1_A		SCTLR_A
#define SCTLR_EL1_C		SCTLR_C
#define SCTLR_EL1_SA		(1 << 4) /* stack alignment check enable */
#define SCTLR_EL1_CP15BEN	SCTLR_CP15BEN
#define SCTLR_EL1_THEE		SCTLR_THEE
#define SCTLR_EL1_ITD		SCTLR_ITD
#define SCTLR_EL1_SED		SCTLR_SED
#define SCTLR_EL1_UMA		(1 << 9) /* 1=Enable access to the interrupt masks from EL0 */
#define SCTLR_EL1_RES10 	(1 << 10) /* reserved 0 */
#define SCTLR_EL1_RES11 	(1 << 11) /* reserved 1 */
#define SCTLR_EL1_I		SCTLR_I	
#define SCTLR_EL1_DZE		(1 << 14) /* 1=DC ZVA ins allowed at EL0 */
#define SCTLR_EL1_UCT		(1 << 15) /* 1=enables EL0 access in AArch64 to the CTR_EL0 register */
#define SCTLR_EL1_nTWI		SCTLR_nTWI
#define SCTLR_EL1_nTWE		SCTLR_nTWE
#define SCTLR_EL1_WXN		SCTLR_WXN
#define SCTLR_EL1_E0E		(1 << 24) /* 0=explicit data accesses at EL0 are little-endian */
#define SCTLR_EL1_EE		SCTLR_EE
#define SCTLR_EL1_UCI		(1 << 26) /* 1= enable EL0/AArch64: DC CVAU, DC CIVAC, DC CVAC, and IC IVAU  */

/* SCTLR EL2 & EL3 */
#define SCTLR_EL2_M	SCTLR_M
#define SCTLR_EL2_A	SCTLR_A
#define SCTLR_EL2_C	SCTLR_C
#define SCTLR_EL2_I	SCTLR_I
#define SCTLR_EL2_WXN	SCTLR_WXN
#define SCTLR_EL2_EE	SCTLR_EE
/* RES1 bits */
#define SCTLR_EL2_RES4	(3 << 4)
#define SCTLR_EL2_RES11	(1 << 11)
#define SCTLR_EL2_RES16	(1 << 16)
#define SCTLR_EL2_RES18	(1 << 18)
#define SCTLR_EL2_RES22	(3 << 22)
#define SCTLR_EL2_RES28 (3 << 28)
#define SCTLR_EL2_RES1_BITS (SCTLR_EL2_RES28 | SCTLR_EL2_RES22 | \
	SCTLR_EL2_RES18 | SCTLR_EL2_RES16 | SCTLR_EL2_RES11 | SCTLR_EL2_RES4)


/* SPSR AARCH32 */
#define A32_SPSR_MODE	    (15 << 0) /* exception mode */
#define A32_SPSR_MODE_User	    0  /* 0b0000 */
#define A32_SPSR_MODE_FIQ	    1  /* 0b0001 */
#define A32_SPSR_MODE_IRQ	    2  /* 0b0010 */
#define A32_SPSR_MODE_Supervisor    3  /* 0b0011 */
#define A32_SPSR_MODE_Monitor	    6  /* 0b0110 */
#define A32_SPSR_MODE_Abort	    7  /* 0b0111 */
#define A32_SPSR_MODE_Hyp	    10 /* 0b1010 */
#define A32_SPSR_MODE_Undefined     11 /* 0b1011 */
#define A32_SPSR_MODE_System	    15 /* 0b1111 */
#define A32_SPSR_M4	    (1 << 4) /* 1=exception taken from AArch32 */
#define A32_SPSR_T	    (1 << 5) /* 1=in T32 state */
#define A32_SPSR_F	    (1 << 6) /* 1=FIQ exception masked  */
#define A32_SPSR_I	    (1 << 7) /* 1=IRQ exception masked */
#define A32_SPSR_A	    (1 << 8) /* 1= async data abort exception masked */
#define A32_SPSR_E	    (1 << 9) /* 0=little endian mode */
#define A32_SPSR_IT1	    (63 << 10) /* T32 IT state (part #1) */
#define A32_SPSR_GE	    (15 << 16) /* gt or eq flags (parallel addition) */
#define A32_SPSR_IL	    (1 << 20) /* illegal Execution State */
#define A32_SPSR_J	    (1 << 24) /* depricated */
#define A32_SPSR_IT2	    (3 << 25) /*  */ /* T32 IT state (part #2) */
#define A32_SPSR_Q	    (1 << 27)
#define A32_SPSR_V	    (1 << 28)
#define A32_SPSR_C	    (1 << 29)
#define A32_SPSR_Z	    (1 << 30)
#define A32_SPSR_N	    (1 << 31)

/* SPSR AARCH64 */
#define A64_SPSR_MODE	(15 << 0)
/* M[3:2] Exception Level, m[1] unused,
 M[0]: 0=SP is always SP0, 1=exception SP is determined by the EL */
#define A64_SPSR_MODE_EL0t	0  /* 0b0000 */
#define A64_SPSR_MODE_EL1t	4  /* 0b0100 */
#define A64_SPSR_MODE_EL1h	5  /* 0b0101 */
#define A64_SPSR_MODE_EL2t	8  /* 0b1000 */
#define A64_SPSR_MODE_EL2h	9  /* 0b1001 */
#define A64_SPSR_MODE_EL3t	12 /* 0b1100 */
#define A64_SPSR_MODE_EL3h	13 /* 0b1101 */
#define A64_SPSR_M4	(1 << 4) /* 0=exception taken from AArch64 */
#define A64_SPSR_RES5	(1 << 5) /* resereved */
#define A64_SPSR_F	(1 << 6) /* 1=FIQ exception are masked */
#define A64_SPSR_I	(1 << 7) /* 1=IRQ exception are masked */
#define A64_SPSR_A	(1 << 8) /* 1=system error exceptions are masked */
#define A64_SPSR_D	(1 << 9) /* 1=debug exceptions are masked  */
#define A64_SPSR_IL	(1 << 20) /* llegal Execution State */
#define A64_SPSR_SS	(1 << 21) /* software step */
#define A64_SPSR_V	(1 << 28)
#define A64_SPSR_C	(1 << 29)
#define A64_SPSR_Z	(1 << 30)
#define A64_SPSR_N	(1 << 31)


/* SCR, Secure Configuration Register
EL3 (SCR.NS=1)
EL3 (SCR.NS=0)
*/
#define SCR_NS		(1 << 0) /* 1=EL0 and EL1 cannot access sec memory */
#define SCR_IRQ		(1 << 1) /* 1=take IRQ in EL3 */
#define SCR_FIQ		(1 << 2) /* 1=take FIQ in EL3 */
#define SCR_EA		(1 << 3) /* 1=take ext abort in EL3 */
#define SCR_RES4	(3 << 4) /* Reserved, RES1  */
#define SCR_RES6	(1 << 6) /* Reserved, RES0  */
#define SCR_SCD		(1 << 7) /* 0=allow SMC in EL1, EL2, or EL3*/
#define SCR_HCE 	(1 << 8) /* 1=allow HVC in non-secure EL1 modes */
#define SCR_SIF		(1 << 9) /* 0=sec state instruction fetches from NS mem allowed */
#define SCR_RW		(1 << 10) /* 0=Lower levels are all AArch32 */
#define SCR_ST		(1 << 11) /* 0=only EL3 access[1] for CNTPS_TVAL_EL1, CNTPS_CTL_EL1 & CNTPS_CVAL_EL1 */
#define SCR_TWI		(1 << 12) /* 0=don't trap WFI */
#define SCR_TWE 	(1 << 13) /* 0=don't trap WFE*/
/* 14..31: reserved RES0 */
/*[1] 1=These registers are accessible in EL3 and also in EL1 when SCR_EL3.NS==0 */


/* Architectural Feature Trap Register
EL2
EL3 (SCR.NS=1)
EL3 (SCR.NS=0)
*/
#define CPTR_TFP (1 << 10) /* 1=trap fp & SIMD instructions */
#define CPTR_TTA (1 << 20) /* 1=trap trace functionality */
#define CPTR_TCPAC (1 << 31) /*  1=trap CPACR accesses */

/* Architectural Feature Trap Register
EL1
*/
#define CPACR_FPEN_MASK (3 << 20) /* 11=don't trap FPEN/SIMD ins */
#define CPACR_TTA	(1 << 28) /* 1=trap traces to EL1 from El1 or EL0 */


/* CurrentEL */
#define CURRENT_EL_MASK 0x0c
#define CURRENT_EL_SHIFT 2
#define CURRENT_E0_MASK (0 << CURRENT_EL_SHIFT)
#define CURRENT_E1_MASK (1 << CURRENT_EL_SHIFT)
#define CURRENT_E2_MASK (2 << CURRENT_EL_SHIFT)
#define CURRENT_E3_MASK (3 << CURRENT_EL_SHIFT)


/* CNTKCTL, AKA CNTKCTL_EL1 */
#define CNTKCTL_EL0PCTEN	0x001 /* EL0 access to CNTPCT & CNTFRQ */
#define CNTKCTL_EL0VCTEN	0x002 /* EL0 access to CNTVCT & CNTFRQ */
#define CNTKCTL_EVNTEN		0x004 /* Enables the generation of an event stream */
#define CNTKCTL_EVNTDIR_MASK	0x0F0 /* Select event (0 to 15) trigger*/
#define CNTKCTL_EL0VTEN		0x100 /* EL0 access to CNTV_CVAL, CNTV_CTL, and CNTV_TVAL */
#define CNTKCTL_EL0PTEN		0x200 /* EL0 access to CNTP_CVAL, CNTP_CTL, and CNTP_TVAL */

/* CNTHCTL_EL2 */
#define CNTHCTL_EL2_EL1PCTEN	(1 << 0)
#define CNTHCTL_EL2_EL1PCEN	(1 << 1)
#define CNTHCTL_EL2_EVNTEN	(1 << 2)
#define CNTHCTL_EL2_EVNTDIR	(1 << 3)
#define CNTHCTL_EL2_EVNTI	(0xf << 4)

/* CNTP_CTL */
#define CNTP_CTL_ENABLE  0x1
#define CNTP_CTL_IMASK   0x2
#define CNTP_CTL_ISTATUS 0x4


/* ICC_SRE_EL3 */
#define ICC_SRE_ELx_SRE 	(1 << 0) /* 1=sys reg i/f to ICH_* & the EL1, EL2, and EL3 ICC_[] for EL3 */
#define ICC_SRE_ELx_DFB 	(1 << 1) /* 1=disable FIQ bypass */
#define ICC_SRE_ELx_DIB 	(1 << 2) /* 1=disable IRQ bypass */
#define ICC_SRE_ELx_Enable	(1 << 3) /* 1=EL1 and EL2 accesses to ICC_SRE_EL1 or ICC_SRE_EL2 trap to EL3 */


/*  MPIDR (mapped to MPIDR_EL1) */
#define MPIDR_AFF0_SHIFT	0
#define MPIDR_AFF1_SHIFT	8
#define MPIDR_AFF2_SHIFT	16
#define MPIDR_AFF3_SHIFT	32
#define MPIDR_AFF0	(0xff << MPIDR_AFF0_SHIFT)
#define MPIDR_AFF1	(0xff << MPIDR_AFF1_SHIFT)
#define MPIDR_AFF2	(0xff << MPIDR_AFF2_SHIFT)
#define MPIDR_MT 	(1 << 24)
#define MPIDR_RES0_0 	(0x1f << 25)
#define MPIDR_U 	(1 << 30)
#define MPIDR_RES1 	(1 << 31)
#define MPIDR_AFF3	(0xff << MPIDR_AFF3_SHIFT)
#define MPIDR_RES0_1 	(0xffffff << 40)
#define MPIDR_AFFINITY_MASK (MPIDR_AFF0 | MPIDR_AFF1 | MPIDR_AFF2 | MPIDR_AFF3)
#define MPIDR_AFFINITY_MASK32 (MPIDR_AFF0 | MPIDR_AFF1 | MPIDR_AFF2)

/*  MAIR - Memory Attribute Indirection Registers */
#define MAIR_ATTR0_SHIFT	(0)
#define MAIR_ATTR1_SHIFT	(8)
#define MAIR_ATTR2_SHIFT	(16)
#define MAIR_ATTR3_SHIFT	(24)
#define MAIR_ATTR4_SHIFT	(32)
#define MAIR_ATTR5_SHIFT	(40)
#define MAIR_ATTR6_SHIFT	(48)
#define MAIR_ATTR7_SHIFT	(56)
/* mair regions for long descriptor type
translation table entry. We should not require
these for SECTION short descriptors */
#define MAIR_ATTR0	(0xff << MAIR_ATTR0_SHIFT)
#define MAIR_ATTR1	(0xff << MAIR_ATTR1_SHIFT)
#define MAIR_ATTR2	(0xff << MAIR_ATTR2_SHIFT)
#define MAIR_ATTR3	(0xff << MAIR_ATTR3_SHIFT)
#define MAIR_ATTR4	(0xff << MAIR_ATTR4_SHIFT)
#define MAIR_ATTR5	(0xff << MAIR_ATTR5_SHIFT)
#define MAIR_ATTR6	(0xff << MAIR_ATTR6_SHIFT)
#define MAIR_ATTR7	(0xff << MAIR_ATTR7_SHIFT)

/* for the long descriptor Attr[n][7:4] */
#define MAIR_F_DEVICE		(0x0 << 4) /* 0b0000 - device memory */
#define MAIR_F_WT_TRAN		(0x3 << 4) /* 0b00rw - normal memory, outer write-through transient (rw not b00) */
#define MAIR_F_NONCACHED	(0x4 << 4) /* 0b0100 - normal memory, outer non-cacheable */
#define MAIR_F_WB_TRAN	 	(0x7 << 4) /* 0b01rw - normal memory, outer write-back transient (rw not b00) */
#define MAIR_F_WT		(0xb << 4) /* 0b10rw - normal memory, outer write-through non-transient */
#define MAIR_F_WB		(0xf << 4) /* 0b11rw - normal memory, outer write-back non-transient */
/* Attr[n][0:3], [7:4] == 0b0000 (MAIR_F_DEVICE) */
#define MAIR_F_nGnRnE	(0x0 << 0) /* 0b0000 - Device-nGnRnE memory  */
#define MAIR_F_nGnRE	(0x4 << 0) /* 0b0100 - Device-nGnRE memory */
#define MAIR_F_nGRE	(0x8 << 0) /* 0b1000 - Device-nGRE memory */
#define MAIR_F_GRE	(0xc << 0) /* 0b1100 - Device-GRE memory */
/* Attr[n][0:3], [7:4] != 0b0000 */
/* TBD (normal memory, inner) */


/* TTBCR - Translation Table Base Control Registers (aarch32)
 Non-secure TTBCR is mapped to the aarch64 TCR_EL1[31:0]
     Secure TTBCR is mapped to the aarch64 TCR_EL3[31:0]
 ===> Not for Hyp mode! <===
*/
/* when TTBCR.EAE is 0 */
#define TTBCR_EAE0_N 		(0x7 << 0) /* width of the base addr in TTBR0 (31:14-N) */
#define TTBCR_EAE0_RES0_0 	(1 << 3)
#define TTBCR_EAE0_PD0		(1 << 4) /* TTBR0: 1=TLB miss faults, 0=walk tbl */
#define TTBCR_EAE0_PD1		(1 << 5) /* TTBR1: 1=TLB miss faults, 0=walk tbl */
#define TTBCR_EAE0_RES0_1	(0x1FFFFFF << 6)
#define TTBCR_EAE0_EAE		(1 << 31) /* 0=Use 32bit with short descriptor table fmt */
/*
	when TTBCR.EAE is 1
 */
#define TTBCR_EAE1_T0SZ		(0x7 << 0) /* size offs mem region addressed by TTBR0 (2^32-T0SZ) */
#define TTBCR_EAE1_RES0_0	(0xf << 3)
#define TTBCR_EAE1_EPD0 	(1 << 7) /* TTBR0: 1=TLB miss faults, 0=walk tbl */
#define TTBCR_EAE1_IRGN0	(0x3 << 8) /* inner cacheability attribute (TTBR0) */
#define TTBCR_EAE1_ORGN0	(0x3 << 10) /* outer cacheability attribute (TTBR0) */
#define TTBCR_EAE1_SH0		(0x3 << 12) /* shareability attrib (TTBR0) */
#define TTBCR_EAE1_RES0_1	(0x3 << 14)
#define TTBCR_EAE1_T1SZ		(0x7 << 16) /* size offs mem region addressed by TTBR1 (2^32-T1SZ)  */
#define TTBCR_EAE1_RES0_2	(0x7 << 19)
#define TTBCR_EAE1_A1		(1 << 22) /* 1=TTBR1.asid defines the ASID, else TTBR0 */
#define TTBCR_EAE1_EPD1		(1 << 23) /* TTBR1: 1=TLB miss faults, 0=walk tbl */
#define TTBCR_EAE1_IRGN1	(0x3 << 24) /* inner cacheability attribute (TTBR1) */
#define TTBCR_EAE1_ORGN1	(0x3 << 26) /* outer cacheability attribute (TTBR1) */
#define TTBCR_EAE1_SH1		(0x3 << 28) /* shareability attrib (TTBR1) */
#define TTBCR_EAE1_RES0_3	(1 << 30)
#define TTBCR_EAE0_EAE		(1 << 31) /* 1= use 40 bit translation, with long descriptor translation table fmt */


/* HTCR - Hyp Translation Control Register (aarch32) EL2 / EL3(NS) 
 HTCR is architecturally mapped to aarch64 register TCR_EL2
*/
#define HTCR_T0SZ	(0x7 << 0) /* size offs mem region addressed by TTBR0 (2^32-TSIZE) */
#define HTCR_RES0_0  	(0x1f << 3)
#define HTCR_IRGN0   	(0x3 << 8) /* inner cacheability attribute (TTBR0) */
#define HTCR_ORGN0   	(0x3 << 10) /* outer cacheability attribute (TTBR0) */
#define HTCR_SH0	(0x3 << 12) /* shareability attrib (TTBR0) */
#define HTCR_RES0_1	(0x1ff << 14)
#define HTCR_RES1_2	(1 << 23) /* [[1=TLB miss faults]] */
#define HTCR_RES0_3	(0x7f << 24)
#define HTCR_RES1_4	(1 << 31) /* [[EAE]] */

/* TCR_EL1 (EL1 / EL0) 
 TCR_EL1 is architecturally mapped to aarch32 register TTBCR(NS) 
*/
#define TCR_EL1_T0SZ	(0x3f << 0) /* size offs mem region addressed by TTBR0_EL1 (2^32-T0SZ)  */
#define TCR_EL1_RES0_0	(1 << 6)
#define TCR_EL1_EPD0	(1 << 7) /* 0=table walk using TTBR0_EL1, 1=TLB miss generates a Translation fault */
#define TCR_EL1_IRGN0	(0x3 << 8) /* inner cacheability attribute (TTBR0_EL1) */
#define TCR_EL1_ORGN0	(0x3 << 10) /* outer cacheability attribute (TTBR0_EL1) */
#define TCR_EL1_SH0	(0x3 << 12) /* shareability attrib (TTBR0_EL1) */
#define TCR_EL1_TG0	(0x3 << 14) /* TTBR0_EL1 granule size 0=4k, 2=64k */
#define TCR_EL1_T1SZ	(0x3f << 16) /* size offs mem region addressed by TTBR1_EL1 (2^32-T1SZ)  */
#define TCR_EL1_A1	(1 << 22) /* 1=TTBR1_EL1.asid defines the ASID, else TTBR0_EL1 */
#define TCR_EL1_EPD1	(1 << 23) /* TTBR1_EL1: 1=TLB miss faults, 0=walk tbl */
#define TCR_EL1_IRGN1	(0x3 << 24) /* inner cacheability attribute (TTBR1_EL1) */
#define TCR_EL1_ORGN1	(0x3 << 26) /* outer cacheability attribute (TTBR1_EL1) */
#define TCR_EL1_SH1	(0x3 << 28) /* shareability attrib (TTBR1_EL1) */
#define TCR_EL1_TG1	(0x3 << 30) /* TTBR1_EL1 granule size 0=4k, 2=64k */
#define TCR_EL1_IPS	(0x7 << 32) /* intermediate physical address size 0=32bit, 1=36, 2=40 */
#define TCR_EL1_RES0_1	(1 << 35)
#define TCR_EL1_AS	(1 << 36) /* ASID size, 0=8 bit, 1=16 bit */
#define TCR_EL1_TBI0	(1 << 37) /* TTBR0_EL1, 1=top byte ignored in the address calculation */
#define TCR_EL1_TBI1	(1 << 37) /* TTBR1_EL1, 1=top byte ignored in the address calculation */
#define TCR_EL1_RES0_2	(0x1FFFFFF << 39) /* [63:39] */

/* TCR_EL2 - Translation Control Register (aarch64) EL2 / EL3(NS & S) */
#define TCR_EL2_T0SZ	(0x3f << 0) /* size offs mem region addressed by TTBR0_EL2 (2^32-T0SZ)  */
#define TCR_EL2_RES0_0	(0x3 << 6)
#define TCR_EL2_IRGN0	(0x3 << 8) /* inner cacheability attribute (TTBR0_EL2) */
#define TCR_EL2_ORGN0	(0x3 << 10) /* outer cacheability attribute (TTBR0_EL2) */
#define TCR_EL2_SH0	(0x3 << 12) /* shareability attrib (TTBR0_EL2) */
#define TCR_EL2_TG0	(0x3 << 14) /* TTBR0_EL2 granule size 0=4k, 2=64k */
#define TCR_EL2_PS	(0x7 << 16) /* physical address size 0=32bit, 1=36, 2=40 */
#define TCR_EL2_RES0_1	(1 << 19)
#define TCR_EL2_TBI	(1 << 20) /* 1=top byte ignored in the address calculation */
#define TCR_EL2_RES0_2	(0x3 << 21)
#define TCR_EL2_RES1_3	(1 << 23)
#define TCR_EL2_RES0_4	(0x7f << 24)
#define TCR_EL2_RES1_5	(1 << 31)

/* TCR_EL3 */
#define TCR_EL3_T0SZ	(0x3f << 0) /* size offs mem region addressed by TTBR0_EL3 (2^32-T0SZ)  */
#define TCR_EL3_RES0_0	(0x3 << 6) /* [7:6] */
#define TCR_EL3_IRGN0	(0x3 << 8) /* inner cacheability attribute (TTBR0_EL3) */
#define TCR_EL3_ORGN0	(0x3 << 10) /* outer cacheability attribute (TTBR0_EL3) */
#define TCR_EL3_SH0	(0x3 << 12) /* shareability attrib (TTBR0_EL3) */
#define TCR_EL3_TG0	(0x3 << 14) /* TTBR0_EL3 granule size 0=4k, 2=64k */
#define TCR_EL3_PS	(0x7 << 16) /* physical address size 0=32bit, 1=36, 2=40 */
#define TCR_EL3_RES0_1	(1 << 19)
#define TCR_EL3_TBI	(1 << 20) /* 1=top byte ignored in the address calculation */
#define TCR_EL3_RES0_2	(0x3 << 21)
#define TCR_EL3_RES1_3	(1 << 23)
#define TCR_EL3_RES0_4	(0x7f << 24)
#define TCR_EL3_RES1_5	(1 << 31)


/* HCR_EL2 */
#define HCR_EL2_VM	(1 << 0)  /* 1=EL1 and EL0 stage 2 address translation en */
#define HCR_EL2_SWIO	(1 << 1)  /* 1=AArch32: DCISW is executed as DCCISW */
#define HCR_EL2_PTW	(1 << 2)  /* 0=normal walk, 1=memory access generates a stage 2 Permission fault. */
#define HCR_EL2_FMO	(1 << 3)  /* 0=For exception levels below EL2, FIQ is not taken to EL2. */
#define HCR_EL2_IMO	(1 << 4)  /* 0=For exception levels below EL2, IRQ is not taken to EL2. */
#define HCR_EL2_AMO	(1 << 5)  /* 0=For exception levels below EL2, Asynchronous External Abort is not taken to EL2. */
#define HCR_EL2_VF	(1 << 6)  /* 1=Virtual FIQ is pending by this mechanism */
#define HCR_EL2_VI	(1 << 7)  /* 1=Virtual IRQ is pending by this mechanism */
#define HCR_EL2_VSE	(1 << 8)  /* 1=Virtual  Asynchronous External Abort is pending by this mechanism */
#define HCR_EL2_FB	(1 << 9)  /* Force broadcast (Inner Sharable) TLBxx & ICIALLU instruction. */
#define HCR_EL2_BSU	(3 << 10) /* Barrier Shareability upgrade (11=full, 10=outer, 01=inner, 00=no effect */
#define HCR_EL2_DC	(1 << 12) /* Default Cacheable. for 0: if the stage 1 MMU off, default mem attribute is Device-nGnRnE */
#define HCR_EL2_TWI	(1 << 13) /* 0=Non-secure EL0 or EL1 execution of WFI instructions not trapped to EL2 */
#define HCR_EL2_TWE	(1 << 14) /* 0=Non-secure EL0 or EL1 execution of WFE instructions not trapped to EL2 */
#define HCR_EL2_TID0	(1 << 15) /* Trap ID group 0 */
#define HCR_EL2_TID1	(1 << 16) /* Trap ID group 1 */
#define HCR_EL2_TID2	(1 << 17) /* Trap ID group 2 */
#define HCR_EL2_TID3	(1 << 18) /* Trap ID group 3 */
#define HCR_EL2_TSC	(1 << 19) /* 0=NS EL1 execution of SMC instructions are not trapped to EL2 */
#define HCR_EL2_TIDCP	(1 << 20) /* Trap Implementation Dependent functionality */
#define HCR_EL2_TACR	(1 << 21) /* Trap Auxiliary Control Registers */
#define HCR_EL2_TSW	(1 << 22) /* 1=Trap data or unified cache maintenance operations by Set/Way */
#define HCR_EL2_TPC	(1 << 23) /* 1=Trap data or unified cache maintenance operations to Point of Coherency */
#define HCR_EL2_TPU	(1 << 24) /* 1=Trap cache maintenance instructions to Point of Unification */
#define HCR_EL2_TTLB	(1 << 25) /* 1=Trap TLB maintenance instructions */
#define HCR_EL2_TVM	(1 << 26) /* 1=Trap Virtual Memory controls */
#define HCR_EL2_TGE	(1 << 27) /* 1=Trap General Exceptions */
#define HCR_EL2_TDZ	(1 << 28) /* 1=Trap DC ZVA instructions */
#define HCR_EL2_HCD	(1 << 29) /* 1=Hypervisor Call instruction disable */
#define HCR_EL2_TRVM	(1 << 30) /* 1=Trap Reads of Virtual Memory controls */
#define HCR_EL2_RW	(1 << 31) /* 0=Lower levels are all AArch32, 1=EL1 is AArch64 */
#define HCR_EL2_CD	(1 << 32) /* Stage 2 Data cache disable */
#define HCR_EL2_ID	(1 << 33) /* Stage 2 Instruction cache disable */
#define HCR_EL2_RES0_a	(15 << 34)
#define HCR_EL2_MIOCNCE (1 << 38) /* Mismatched Inner/Outer Cacheable Non-Coherency Enable */
#define HCR_EL2_RES_b	(0x1FFFFFF << 39)

/* TTBCR & TCR: for X being IRGN0, ORGN0, IRGN1 or ORGN1 fields */
#define TTBCR_REGION_NONCACHE	0x0 /* normal memory, x non-cacheable */
#define TTBCR_REGION_WRBACK	0x1 /* normal memory, x write back write allocate cacheable */
#define TTBCR_REGION_WRTHRU	0x2 /* normal memory, x write through cacheable */
#define TTBCR_REGION_WRBACK_NA	0x3 /* normal memory, x write back - no write allocate cacheable */

/* TTBCR & TCR: for SH0 or SH1 */
#define TTBCR_SHARE_NON		0x0 /* non-shareable */
#define TTBCR_SHARE_OUTER	0x1 /* outer shareable */
#define TTBCR_SHARE_INNER	0x2 /* inner shareable */
#define TTBCR_SHARE_RES 	0x3 /* reserved */


/* ESR_ELx, Exception Syndrome Register (ELx) */
#define ESR_ELx_ISS (0x1FFFFFF << 0)	/* [24:0] Instruction Specific Syndrome */
#define ESR_ELx_IL  (1 << 25)		/* 0=16-bit, 1=32-bit instruction trapped */
#define ESR_ELx_EC (0x3f << 26)		/* [31:26] Exception Class */

#define ESR_ELx_EC_SMC32 (0x13 << 26) /* 010011, Exception from SMC ins execution in AArch32 state */
#define ESR_ELx_EC_SMC64 (0x17 << 26) /* 010111, Exception from SMC ins execution in AArch64 state */


/* CPUECTLR_EL1 CPU Extended Control Register [RW] 64 bit */
#define CPUECTLR_EL1_RETENTION_CTL		(0x7 << 0)
#define CPUECTLR_EL1_ASIMD_FP_RETENTION_CTL	(0x7 << 3)
#define CPUECTLR_EL1_SMPEN 			(0x1 << 6)

/* MIDR_EL1, Main ID Register [RO] 32 bit */
#define MIDR_EL1_REV		(0xf << 0)
#define MIDR_EL1_PARTNUM	(0xfff << 4)
#define MIDR_EL1_ARCH		(0xf << 16)
#define MIDR_EL1_VARIANT	(0xf << 20)
#define MIDR_EL1_IMPLEMENTER	(0xff << 24)

/* OEM */
#define MIDR_MAJOR_PART_MASK	(MIDR_EL1_IMPLEMENTER | MIDR_EL1_ARCH | MIDR_EL1_PARTNUM)
#define MIDR_IMPLEMENTER_BRCM	(0x42 << 24)
#define MIDR_PartOrionB53	0x420f1000

/* CSSELR_EL1, Cache Size Selection Register */
#define CSSELR_EL1_InD		(1 << 0) /* 1=Instruction or 0=Data */

#define CSSELR_EL1_InD_DCACHE	0
#define CSSELR_EL1_InD_ICACHE	1

#define CSSELR_EL1_Level_SHIFT	1 /* Cache level */
#define CSSELR_EL1_Level	(7 << CSSELR_EL1_Level_SHIFT)

#define CSSELR_EL1_L1		(0 << CSSELR_EL1_Level_SHIFT)
#define CSSELR_EL1_L2		(1 << CSSELR_EL1_Level_SHIFT)
#define CSSELR_EL1_L3		(2 << CSSELR_EL1_Level_SHIFT)
#define CSSELR_EL1_L4		(3 << CSSELR_EL1_Level_SHIFT)
#define CSSELR_EL1_L5		(4 << CSSELR_EL1_Level_SHIFT)
#define CSSELR_EL1_L6		(5 << CSSELR_EL1_Level_SHIFT)
#define CSSELR_EL1_L7		(6 << CSSELR_EL1_Level_SHIFT)
#define CSSELR_EL1_RSVD		(7 << CSSELR_EL1_Level_SHIFT)

/* CCSIDR_EL1, Current Cache Size ID Register */
#define CCSIDR_EL1_LineSize_SHIFT	0 /* Log2(Number of bytes in cache line) -4 */
#define CCSIDR_EL1_LineSize		(0x7 << 0)

#define CCSIDR_EL1_Associativity_SHIFT	3 /* Associativity of cache -1 */
#define CCSIDR_EL1_Associativity	(0x3ff << CCSIDR_EL1_Associativity_SHIFT)

#define CCSIDR_EL1_NumSets_SHIFT	13
#define CCSIDR_EL1_NumSets		(0x7FFF << CCSIDR_EL1_NumSets_SHIFT)

#define CCSIDR_EL1_WA			(1 << 28) /* 1=Write-allocation supported */
#define CCSIDR_EL1_RA			(1 << 29) /* 1=Read-allocation supported */
#define CCSIDR_EL1_WB			(1 << 30) /* 1=Write-back supported */
#define CCSIDR_EL1_WT			(1 << 31) /* 1=Write-through supported */

/* CLIDR_EL1, Cache Level ID Register [RO] 64 bit */
#define CLIDR_EL1_CTYPES		(0x1FFFFF << 0)
#define CLIDR_EL1_CTYPES_MASK		0x07
#define CLIDR_EL1_CTYPES_SHIFT		3

#define CLIDR_EL1_CTYPE_NOCACHE 	0
#define CLIDR_EL1_CTYPE_IC_ONLY 	1
#define CLIDR_EL1_CTYPE_DC_ONLY 	2
#define CLIDR_EL1_CTYPE_SEPARATE_ID	3
#define CLIDR_EL1_CTYPE_UINFIED_ID	4
#define CLIDR_EL1_CTYPE_RES5		5
#define CLIDR_EL1_CTYPE_RES6		6
#define CLIDR_EL1_CTYPE_RES7		7

#define CLIDR_EL1_LoUIS 		(0x7 << 21)
#define CLIDR_EL1_LoC			(0x7 << 24)
#define CLIDR_EL1_LoUU			(0x7 << 27)
#define CLIDR_EL1_ICB			(0x7 << 30)

#endif /* __ARMV8_REGS_H__ */
