/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#ifndef CFG_EMULATION
#include "arm-macros.h"
#include "bchp_common.h"
#include "bchp_bsp_glb_control.h"
#include "bchp_bsp_cmdbuf.h"

#define ERR_GLITCH_TRACE_CHECK 15
#define S3_TRACE_VALUE	0x712A0006
	.section .init
	.align   4
	.arm

	.macro sram_memset foo, reg_beg, reg_end, reg_val
\foo\()_memset_loop:
	str		\reg_val, [\reg_beg], #4
	cmp		\reg_beg, \reg_end
	blo		\foo\()_memset_loop
	.endm

@  We cannot rely on the compiler to properly
@ order these functions, instead we use
@ assembly code and also do a runtime
@ verification before this sequence is
@ called.


@ glitch_addr += glitch_trace;
@ glitch_entry = (void *) (glitch_addr << 1);
@ if (glitch_entry != glitch_addr1) handle_boot_err()
@ glitch_entry((struct fsbl_info *)glitch_info);
@ handle_boot_err()
@
ENTRY_PROC(anti_glitch_a)
	ldr		r1,	=glitch_trace
	ldr		r5,	[r1]
	ldr		r1,	=glitch_addr
	ldr		r4,	[r1]
	add		r4, r5, r4
	str		r4,	[r1]

	mov		r4, r4, LSL #1
	ldr		r1,	=glitch_entry
	str		r4,	[r1]

	ldr		r1,	=glitch_addr1
	ldr		r2,	[r1]
	cmp		r4,	r2
	beq		entry_ok

	mov		r0,	#ERR_GLITCH_TRACE_CHECK
	ldr		r2,	=handle_boot_err
	blx		r2

entry_ok:
#if !defined(CFG_EMULATION) && !defined(CFG_FULL_EMULATION) && defined(CFG_ZEUS4_2)
	ldr		r1,	=g_bsp_version
	ldr		r8,	[r1]
	cmp		r8,	#3
	bls		clear_sram

	@ Store JTAG unblock command data at (BCHP_BSP_OTP_SCRATCH_REG_START + 128).
	@ JTAG unblock command is sent after data section is clear, need to store
	@ it somewhere.
	@
	@ r6: (BCHP_BSP_OTP_SCRATCH_REG_START + 128)
	@
	@ 0:  BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE
	@ 4:  BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE + 0x194
	@ 8:  BCHP_BSP_GLB_CONTROL_GLB_IRDY
	@ 12: BCHP_BSP_GLB_CONTROL_GLB_OLOAD1
	@ 16: BCHP_BSP_GLB_CONTROL_GLB_ILOAD1
	@ 20: BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS
	@ 24: 0xabcdef00
	@ 28: 0xbe55aa41
	@ 32: 0x789a0004
	@ 36: 0x1057a675
	@ 40: status

	ldr		r2,	=BCHP_PHYSICAL_OFFSET
	ldr		r1,	=BCHP_BSP_OTP_SCRATCH_REG_START
	add		r1, r1, r2
	mov		r6,	#128
	add		r6, r1, r6
	ldr		r1,	=BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE
	add		r1, r1, r2
	str		r1,	[r6]
	mov		r3,	#404	@0x194
	add		r1, r1, r3
	str		r1,	[r6, #4]
	ldr		r1,	=BCHP_BSP_GLB_CONTROL_GLB_IRDY
	add		r1, r1, r2
	str		r1,	[r6, #8]
	ldr		r1,	=BCHP_BSP_GLB_CONTROL_GLB_OLOAD1
	add		r1, r1, r2
	str		r1,	[r6, #12]
	ldr		r1,	=BCHP_BSP_GLB_CONTROL_GLB_ILOAD1
	add		r1, r1, r2
	str		r1,	[r6, #16]
	ldr		r1,	=BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS
	add		r1, r1, r2
	str		r1,	[r6, #20]
	ldr		r1,	=0xabcdef00
	str		r1,	[r6, #24]
	ldr		r1,	=0xbe55aa41
	str		r1,	[r6, #28]
	ldr		r1,	=0x789a0004
	str		r1,	[r6, #32]
	ldr		r1,	=0x1057a675
	str		r1,	[r6, #36]
	ldr		r1,	=0x12347890
	str		r1,	[r6, #40]
#endif
clear_sram:
	@ clear SRAM from start of sram to pc
	ldr		r1,	=SRAM_ADDR	@0xffe0_0000
	mov		r2, 	pc
	mov		r3, 	#0
	sram_memset	clear1, r1, r2, r3

	@ store entry and fsbl_info into r4 and r0
	ldr		r1,	=glitch_info
	ldr		r0,	[r1]
	ldr		r1,	=glitch_entry
	ldr		r4,	[r1]

	@ store pointer to handle_boot_err()
	ldr		r5,	=handle_boot_err

	@clear anti_glitch_b to end of SRAM
	ldr		r1, =anti_glitch_b

	@ A15:0xffe1_0000, A53:0xffe2_0000
	ldr		r2, =(SRAM_ADDR + SRAM_LEN)
	mov  		r3, #0
	sram_memset	clear2, r1, r2, r3

#if !defined(CFG_EMULATION) && !defined(CFG_FULL_EMULATION) && defined(CFG_ZEUS4_2)
	cmp		r8,	#3
	bls		call_entry

	@ =================
	@ send JTAG unblock
	@ =================

	@ check for IRDY1
	ldr		r2,	[r6, #8]
irdy1:
	ldr		r3,	[r2]
	lsls		r1, r3, #31
	bpl		irdy1

	@ r6: (BCHP_BSP_OTP_SCRATCH_REG_START + 128)
	@ r7: BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE
	ldr		r7,	[r6]

	@ REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE) = 0x10
	movs		r2, 	#16
	str		r2,	[r7]
	@ REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+4) = 0xdb
	movs		r2,	#219
	str		r2,	[r7, #4]
	@ REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+8) = 0xabcdef00
	ldr		r2,	[r6, #24]
	str		r2,	[r7, #8]
	@ REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+12) = 0xbe55aa41
	ldr		r2,	[r6, #28]
	str		r2,	[r7, #12]
	@ REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+16) = 0x789a0004
	ldr		r2,	[r6, #32]
	str		r2,	[r7, #16]
	@ REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+20) = 0x1057a675
	ldr		r2,	[r6, #36]
	str		r2,	[r7, #20]

	@ REG(BCHP_BSP_GLB_CONTROL_GLB_ILOAD1) = 0x1
	ldr		r2,	[r6, #16]
	movs		r3, 	#1
	str		r3,	[r2]

	@ check for OLOAD1
	ldr		r2,	[r6, #12]
oload1:
	ldr		r3,	[r2]
	lsls		r3, r3, #31
	bpl		oload1

	@ REG(BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS) = 0x00
	ldr		r2,	[r6, #20]
	movs		r3, 	#0
	str		r3,	[r2]

	@ REG(BCHP_BSP_GLB_CONTROL_GLB_OLOAD1) = 0x00
	ldr		r2,	[r6, #12]
	str		r3,	[r2]

	@ status = REG(BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE+0x194)
	ldr		r1,	[r6, #4]
	ldr		r2,	[r1]
	str		r2,	[r6, #40]
#endif
call_entry:
	@ r4: ssbl_entry, r5: handle_boot_err, r0: fsbl_info
	@ call ssbl_entry(*fsbl_info)
	blx		r4
	mov		r0,	#ERR_GLITCH_TRACE_CHECK
	blx		r5
END_PROC(anti_glitch_a)

@ glitch_trace = 12345;
@ sec_verify_ssbl();
@ glitch_addr -= glitch_trace;
@ sec_mitch_check();
@ anti_glitch_a();
@ handle_boot_err();
@
ENTRY_PROC(anti_glitch_b)
	movw		r0,	#12345
	ldr		r1,	=glitch_trace
	str		r0,	[r1]

	ldr		r2,	=sec_verify_ssbl
	blx		r2

	ldr		r1,	=glitch_trace
	ldr		r5,	[r1]
	ldr		r1,	=glitch_addr
	ldr		r4,	[r1]
	rsb		r4, r5, r4
	str		r4,	[r1]

	ldr		r2,	=sec_mitch_check
	blx		r2

	ldr		r2,	=anti_glitch_a
	blx		r2

	mov		r0,	#ERR_GLITCH_TRACE_CHECK
	ldr		r2,	=handle_boot_err
	blx		r2
END_PROC(anti_glitch_b)

@ glitch_addr = (uint32_t) copy_code();
@ glitch_addr1 = glitch_addr;
@ glitch_addr = glitch_addr >> 1;
@ mmu_disable();
@ i_cache_config(0);
@ anti_glitch_b();
@ handle_boot_err();
@
ENTRY_PROC(anti_glitch_c)
	ldr		r2,	=copy_code
	blx		r2
	ldr		r1,	=glitch_addr1
	str		r0,	[r1]
	mov		r0, r0, LSR #1
	ldr		r1,	=glitch_addr
	str		r0,	[r1]

	ldr		r2,	=mmu_disable
	blx		r2

	ldr		r2,	=i_cache_config
	mov		r0,	#0
	blx		r2

	ldr		r2,	=anti_glitch_b
	blx		r2

	mov		r0,	#ERR_GLITCH_TRACE_CHECK
	ldr		r2,	=handle_boot_err
	blx		r2
END_PROC(anti_glitch_c)

@ glitch_entry = (void *) (glitch_addr << 1);
@ glitch_entry();
@ handle_boot_err()
@
ENTRY_PROC(anti_glitch_d)
	ldr		r1,	=glitch_addr
	ldr		r2,	[r1]
	mov		r2, r2, LSL #1
	ldr		r3,	=handle_boot_err

	@ call linux_entry()
	blx		r2
	mov		r0,	#ERR_GLITCH_TRACE_CHECK
	blx		r3
END_PROC(anti_glitch_d)

#if CFG_PM_S3
@ store glitch_addr content to r0
@ anti_glitch_b();
@ handle_boot_err();
@
ENTRY_PROC(anti_glitch_e)

@	ldr		r0,	=BCHP_PHYSICAL_OFFSET
@	ldr		r1,	=BCHP_HIF_TOP_CTRL_SCRATCH


	ldr		r1,	=glitch_trace
	ldr		r0,	[r1]	
	ldr		r2,	=S3_TRACE_VALUE

	cmp		r0,	r2
	beq		trace_ok

	mov		r0,	#ERR_GLITCH_TRACE_CHECK
	ldr		r2,	=handle_boot_err
	blx		r2

trace_ok:
	ldr		r2,	=anti_glitch_d
	blx		r2

	mov		r0,	#ERR_GLITCH_TRACE_CHECK
	ldr		r2,	=handle_boot_err
	blx		r2
END_PROC(anti_glitch_e)
#endif

@ r1: addr of register to set
@ r4: value to set
@
ENTRY_PROC(return_from_check_return_val)
	str		r4,	[r1]
	pop		{r4-r6, pc}
END_PROC(return_from_check_return_val)

@ check return value and set trace bit
@ r0: val to check, 0: ok, other: failure
@ r1: addr of register to set
@ r2: trace bit to set
@ r3: error code
@
ENTRY_PROC(check_return_val)
	push		{r4-r6, lr}
	ldr		r4,	[r1]
	mov		r5, 	#1
	lsl		r6, 	r5, r2
	orr		r4, 	r4, r6
	cmp		r0, 	#0
	beq		return_from_check_return_val

	mov		r0,	r3
	ldr		r2,	=handle_boot_err
	blx		r2
END_PROC(check_return_val)

.end

#endif /* !CFG_EMULATION */
