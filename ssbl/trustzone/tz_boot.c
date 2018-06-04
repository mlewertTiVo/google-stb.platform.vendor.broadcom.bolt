/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2018 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifdef TZ_SECURE_BOOT
#include "sec_boot.h"
#endif

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "error.h"
#include "board.h"
#include "bolt.h"
#include "loader.h"
#include "devtree.h"
#include "net_ebuf.h"
#include "net_ether.h"
#include "net_api.h"
#include "arch_ops.h"
#if CFG_MON64
#include "mon64.h"
#endif
#include "tz.h"
#include "tz_priv.h"


/* The function tz_go() is a trimmed version of bolt_go() with the
 * following differeneces:
 *
 *   - device tree address is retrived from tz_info;
 *   - the last unused argument now indicates the UART used by bolt.
 */
int tz_go(bolt_loadargs_t *la)
{
	struct tz_info *t;
#if CFG_MON64
	bolt_devtree_params_t p;
	void *dt_address;
#else
	struct fsbl_info *info;
#endif

	if (la->la_entrypt == 0) {
		xprintf("No program has been loaded.\n");
		return BOLT_ERR_INV_COMMAND;
	}

#if CFG_NETWORK
	if (!(la->la_flags & LOADFLG_NOCLOSE)) {
		if (net_getparam(NET_DEVNAME)) {
			xprintf("Closing network '%s'\n",
				(char *)net_getparam(NET_DEVNAME));
			net_uninit();
		}
	}
#endif

#if CFG_MON64
	/* use mon64 to boot */
	if (la->la_flags & LOADFLG_SECURE) {
		t = tz_info();
		if (!t)
			return BOLT_ERR;
		dt_address = t->dt_address;
	} else {
		bolt_devtree_getenvs(&p);
		if (!p.dt_address)
			return BOLT_ERR;
		dt_address = p.dt_address;
	}
	return (arch_booted64()) ?
		mon64_boot(la->la_flags, la->la_entrypt, dt_address) :
		psci_boot(la->la_flags, la->la_entrypt, dt_address);
#else
	t = tz_info();
	if (!t)
		return BOLT_ERR;

	info = board_info();
	if (!info)
		return BOLT_ERR;

#ifdef STUB64_START
	if (la->la_flags & LOADFLG_EL3_EXEC) {
		xprintf("TZ: Starting 64 bit EL3 monitor at %#lx\n",
				la->la_entrypt);
		bolt_start64_el3(la->la_entrypt, 0xffffffff,
		(unsigned int)t->dt_address, info->uart_base);
	} else
#endif
	{
		/* boot without psci */
		xprintf("TZ: Starting TZ program at %#lx (DTB @ %p, UART @ %#x)\n",
			la->la_entrypt, t->dt_address, info->uart_base);
		bolt_start(la->la_entrypt, 0xffffffff,
			(unsigned int)t->dt_address, info->uart_base);
	}

	return BOLT_OK;
#endif /* CFG_MON64 */
}


/* The function tz_boot() is a trimmed version of bolt_boot() with the
 * following differences:
 *
 *   - all batch related functionalities are removed (along with buffers);
 *   - at the end, instead of bolt_go(), tz_go() is called.
 */
int tz_boot(const char *ldrname, bolt_loadargs_t *la)
{
	int rc = BOLT_OK;
	int noise = la->la_flags & LOADFLG_NOISY;

	la->la_entrypt = 0;

	if (noise)
		xprintf("Loading: ");

#if (CFG_CMD_LEVEL >= 5)
		xprintf(" load_program() params:\n");
		xprintf("   ldrname  [%s]\n", ldrname);
		xprintf("   filename [%s]\n", la->la_filename);
		xprintf("   filesys [%s]\n", la->la_filesys);
		xprintf("   device [%s]\n", la->la_device);
		xprintf("   options [%s]\n", la->la_options);
		xprintf("   la_flags %#08x\n", la->la_flags);
		xprintf("   address %#08lx\n", la->la_address);
		xprintf("   maxsize %#08lx\n", la->la_maxsize);
		xprintf("   entrypt %#08lx\n", la->la_entrypt);
		xprintf("\n");
#endif
	rc = bolt_load_program(ldrname, la);

	if (rc < 0) {
		if (noise)
			xprintf("Failed.\n");
		goto out;
	}

#if CFG_MON64
	/* Install TZ program */
	if (la->la_flags & LOADFLG_SECURE) {
		long load_address = la->la_address;
		int  load_size = rc /* load size */;
		long install_address;

		rc = tz_install(load_address, load_size, &install_address);
		if (rc)
			goto out;

		la->la_address = install_address;
		la->la_entrypt = install_address;
	}

	/* In 64-bit, always call tz_go to update mon64 params */
	if (la->la_entrypt != 0)
		rc = tz_go(la);
#else

	/*
	 * Banzai!  Run the program.
	 */

	if ((la->la_flags & LOADFLG_EXECUTE) && (la->la_entrypt != 0)) {
		rc = tz_go(la);
	} else {
#ifdef STUB64_START
		/* On 64bit, set load addr and DT addr for Secure/NS worlds. */
		/* We don't need to set params when loading the EL3 image. */
		if (!(la->la_flags & LOADFLG_EL3_EXEC))
			tz_smm_set_params(la);
#endif
	}
#endif /* CFG_MON64 */
out:
	return rc;
}

#if CFG_MON64
int tz_install(long load_address, int load_size, long *install_address)
{
	struct tz_info *t;
	struct tz_mem_layout *mem_layout;
	long dest_address;
	int __maybe_unused dest_size;
	int __maybe_unused rc;

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	mem_layout = t->mem_layout;
	if (!mem_layout)
		return BOLT_ERR;

	dest_address = t->mem_addr + mem_layout->os_offset;
	dest_size = mem_layout->os_size;

	rc = sec_verify_tz((void *)load_address, load_size,
			   (void *)dest_address, dest_size);
	if (rc) {
		err_msg("failed to decrypt TZ program %d\n", rc);
		return BOLT_ERR;
	}

	if (install_address)
		*install_address = dest_address;

	return BOLT_OK;
}

#ifndef TZ_SECURE_BOOT
int sec_verify_tz(void *load_address, int load_size,
		  void *dest_address, int dest_size)
{
	/* Copy Astra from load address directly */
	memcpy((void *)dest_address, (void *)load_address, load_size);
	return BOLT_OK;
}
#endif
#endif /* CFG_MON64 */
