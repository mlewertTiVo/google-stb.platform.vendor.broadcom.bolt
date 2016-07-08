/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
 
/* This feature is a subset of the engine required for the S3 powersave
 * mode. A software alternative is used if the DMA hardware based SHA
 * feature is not available.
 */
#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "ui_command.h"
#include "bolt.h"
#include "config.h"
#include "timer.h"
#include "error.h"
#include "sha2.h"
#include "../common/xpt_dma.h"

#define MCB_DESCRIPTOR_SIZE  0x100


static int ui_cmd_sha(ui_cmdline_t *cmd, int argc, char *argv[]);


int ui_init_shacmds(void)
{
	cmd_addcmd("sha", ui_cmd_sha, NULL,
		"Calculate SHA256 of a memory region",
		"sha -addr=<addr> -size=<size>"
#if CFG_HARDWARE_SHA
		" -channel=<n>\n"
		"If the optional -channel is not specified then a"
#else
		"\nA"
#endif
		" software SHA method is used.\n\n",
		"-addr=*;start address of the memory to SHA|"
		"-size=*;the size of the memory to SHA"
#if CFG_HARDWARE_SHA
		"|-channel=*;XPT_SHA20: channel 0, XPT_SHA21: channel 1"
#endif
	);

	return 0;
}


static void ssbl_die(char *msg)
{
	xprintf("sha fail '%s'\n", msg);
}


static int ui_cmd_sha(ui_cmdline_t *cmd, int argc, char *argv[])
{
	uint64_t addr = 0ULL;
	uint32_t size = 0U;
	const char *x;
	int i;
	signed int channel = -1;

	if (cmd_sw_value(cmd, "-addr", &x))
		addr = atoq(x);
	else
		return BOLT_ERR_INV_PARAM;

	if (cmd_sw_value(cmd, "-size", &x))
		size = atoi(x);
	else
		return BOLT_ERR_INV_PARAM;

#if CFG_HARDWARE_SHA
	if (cmd_sw_value(cmd, "-channel", &x)) {
		channel = atoi(x);
		if ((channel < 0) || (channel > 1))
			return BOLT_ERR_INV_PARAM;
	}
#endif
	if (CFG_HARDWARE_SHA && (channel >= 0)) {
		uint32_t sha[8];
		struct mcpb_dma_desc *desc;
		const struct memdma_initparams e = {
			ssbl_die, bolt_usleep, lib_memset, NULL};

		desc = KMALLOC(sizeof(*desc), MCB_DESCRIPTOR_SIZE);
		if (!desc)
			return BOLT_ERR_NOMEM;

		memdma_init(&e);

		xpt_dma_sha((dma_addr_t)addr, size, channel, desc, sha);

		KFREE(desc);

		xprintf("Hardware SHA channel %d:\n\t", channel);
		for (i = 0; i < 8; i++)
			xprintf("%08x", sha[7 - i]);
	} else {
		unsigned char digest[SHA256_DIGEST_SIZE];
		const unsigned char *message =
				(const unsigned char *)(uint32_t)addr;

		sha256(message, size, digest);

		xprintf("Software SHA:\n\t");
		for (i = 0; i < SHA256_DIGEST_SIZE; i++)
			xprintf("%02x", digest[i]);
	}

	xprintf("\n");

	return BOLT_OK;
}
