/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <arch.h>
#include <arch-mmu.h>
#include <arm-start.h>
#include <arm-macros.h>
#include <bchp_bsp_glb_control.h>
#include <bsp_config.h>
#include <common.h>
#include <ddr.h>
#include <hwuart.h>
#include <lib_types.h>
#include <ssbl-sec.h>

#include <stdbool.h>

/* The stack and heap must be SECTION (1MiB) aligned and
sized in multiples of it for us to reliably mark *only*
those areas and therefore prevent any overlap with something else.
*/
#if defined(ARM_V7) && (CFG_ALIGN_SIZE != SECTION_SIZE)
#error Cannot mark heap & stack as XN due to ALIGN_SIZE != SECTION_SIZE
#endif

#if CFG_STACK_PROTECT_SSBL
uintptr_t __stack_chk_guard = SSBL_STACK_CHECK_VAL;

void arch_call_bfw_main(unsigned long sp);

void __attribute__((noreturn)) __stack_chk_fail(void)
{
	unsigned long *sp = (unsigned long *)__getstack();

	puts("stack_chk_fail ");
	writehex(__stack_chk_guard);
	__puts(" STACK @ ");
	writehex((unsigned long)sp);
	__puts(" (");
	writehex(*(unsigned long *)sp);
	puts(")");
	while (1)
		;
}
#endif

struct fsbl_info *board_info(void)
{
	return _fsbl_info;
}

const uint32_t uart_base = (BCHP_PHYSICAL_OFFSET+BCHP_UARTA_REG_START);

static int putchar(int c)
{
	while (!(DEV_RD(uart_base + LSR_REG) & LSR_THRE))
		;
	DEV_WR(uart_base + THR_REG, c);

	return 0;
}

void __puts(const char *s)
{
	while (*s) {
		if (*s == '\n')
			putchar('\r');
		putchar(*(s++));
	}
}

int puts(const char *s)
{
	__puts(s);
	putchar('\r');
	putchar('\n');
	return 0;
}

void *memset(void *dest, int c, size_t cnt)
{
	unsigned char *d;

	d = dest;

	while (cnt) {
		*d++ = (unsigned char)c;
		cnt--;
	}

	return dest;
}

void __noisy_flush_caches(void)
{
	__puts("CACHE FLUSH ");
	clear_all_d_cache();
	invalidate_all_i_cache();
	puts("OK");
}

void writehex(uint32_t val)
{
	unsigned int i, c;

	for (i = 0; i < 8; i++, val <<= 4) {
		c = (val >> 28) + '0';
		if (c > '9')
			c += 'a' - '9' - 1;
		putchar(c);
	}
}

void __noreturn sys_die(const uint16_t die_code, const char *die_string)
{
	__puts("FATAL ERROR: ");
	writehex(die_code);
	putchar(';');
	puts(die_string);
	while (1)
		;
}

/* bolt_open, bolt_readblk , writeint and memcpy are included in bsl.a
 * but not used by ssbm. Just define some dummy functions
 * here so that there is no linking error.
 */
int bolt_open(char *flash)
{
	return 0;
}

int bolt_close(int fd)
{
	return 0;
}

int bolt_readblk(int fd, int offset, int addr, int size)
{
	return 0;
}

void memcpy(void)
{
}

void writeint(int n)
{
}

void itoa(int n)
{
}

int board_bootmode(void)
{
	return 0;
}

void *lib_memcpy(void *dest, const void *src, size_t cnt)
{
	return dest;
}

void arch_dump_registers(struct arm_regs *regs)
{
	int i;
	uint32_t cpsr;

	static const char * const names[] = {
		" r0     : ",
		" r1     : ",
		" r2     : ",
		" r3     : ",
		" r4     : ",
		" r5     : ",
		" r6     : ",
		" r7     : ",
		" r8     : ",
		" r9     : ",
		"r10     : ",
		"r11     : ",
		"r12     : ",
		"r13 (sp): ",
		"r14 (lr): ",
		"r15 (pc): ",
		"cpsr    : ",
		"dfsr    : ",
		"dfar    : ",
		"ifar    : ",
	};

	__puts("\n\nCPU exception: ");

	__asm__("mrs %0, CPSR" : "=r" (cpsr));
	switch (cpsr & CPSR_MODE_MASK) {
	case MODE_FIQ:
		__puts("FIQ\n");
		break;
	case MODE_IRQ:
		__puts("IRQ\n");
		break;
	case MODE_ABT:
		__puts("ABT\n");
		break;
	case MODE_UND:
		__puts("UND\n");
		break;
	default:
		__puts("unknown\n");
		break;
	}

	for (i = 0; i < 16; i++) {
		__puts(names[i]);
		writehex(regs->gprs.r[i]);
		__puts("\n");
	}

	__puts(names[16]);
	writehex(regs->cpsr);
	__puts("\n");

	__puts(names[17]);
	writehex(regs->dfsr);
	__puts("\n");

	__puts(names[18]);
	writehex(regs->dfar);
	__puts("\n");

	__puts(names[19]);
	writehex(regs->ifar);
	__puts("\n");
}

/* ssbm_main -- C entry point of SSBM, called from ssbm_init
 *
 * Parameters:
 *  _end   [in] end of SSBM in bytes
 *  _fbss  [in] start of BSS in bytes
 *  _ebss  [in] end of BSS in bytes
 *  _fdata [in] start of data section in bytes, and end of text section
 */
void ssbm_main(uint32_t _end, uint32_t _fbss, uint32_t _ebss, uint32_t _fdata)
{
	void (*reentry)(void *);
	unsigned long sp;

	puts("SSBM1");
	/* if cold boot, just jump to SSBL entry point */
	if (_fsbl_info != 0) {
#if CFG_ZEUS5_1
		/* For non-ZEUS 5.1, MICH is always disabled in FSBL.
		 *  For ZEUS 5.1, MICH is disabled in SSBM if cold boot;
		 *  MICH is disabled in FSBL if warm boot
		 */
		sec_mitch_check();
#endif

		reentry = (void (*)(void *))(uintptr_t)SSBL_RAM_ADDR;
		(*reentry)(_fsbl_info);
	}

	puts("SSBM2");
	__noisy_flush_caches();

	__puts("STACK @ ");
	sp = SSBL_RAM_ADDR - 0x100;

#if CFG_STACK_PROTECT_SSBL
	/* prime a fake previous guard value for bolt_main() in
	case -fstack-protector-all is used. */
	*((uint32_t *)(sp + sizeof(uint32_t))) = SSBL_STACK_CHECK_VAL;
#endif

	writehex(sp);
	puts("");

	__puts("ARCH: CONFIG");
	arch_config();
	puts(" OK");

	arch_call_bfw_main(sp);
}
