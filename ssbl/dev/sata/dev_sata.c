/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2018 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_malloc.h"
#include "lib_printf.h"
#include "lib_string.h"
#include "iocb.h"
#include "ioctl.h"
#include "device.h"
#include "devfuncs.h"
#include "error.h"
#include "common.h"
#include "config.h"
#include "bsp_config.h"
#include "bchp_common.h"
#include "dev_sata.h"

#define VERBOSE 0
#define SECTOR_SZ 512
#define MAX_SECTS 256
#define TMP_BFR_NUM_SECTS 8
#define ZERO_COPY 0


typedef struct sata_softc {
	uint32_t tmp_bfr[TMP_BFR_NUM_SECTS * SECTOR_SZ];
	int ready;
	int initialized;
	sata_dev_t sata_dev;
	struct sata_softc *root_ctx;
	int port;
} sata_softc;

static void __sata_callback(sata_cdb_t *cdb, int status)
{
	sata_cmd_cleanup(cdb);
}

static int __sata_id(sata_softc *ctx)
{
	sata_cdb_t *cdb     = NULL;
	int        status   = 0;
	bfr_desc_t bfr_desc;

	status = sata_cdb_pop(&ctx->sata_dev, ctx->port, &cdb);
	if (status)
		return status;

	bfr_desc.pa = (uint32_t)ctx->tmp_bfr;
	bfr_desc.va = ctx->tmp_bfr;
	bfr_desc.sz = SECTOR_SZ;
	bfr_desc.next = NULL;

	cdb->opcode    = ATA_IDENTIFY_DEVICE;
	cdb->block_cnt = 1;
	cdb->bd        = &bfr_desc;
	cdb->bfr_len   = bfr_desc.sz;
	cdb->callback  = __sata_callback;

	status = sata_cmd_issue(cdb);
	assert(status == 0);

	CACHE_INVAL_RANGE(cdb->bd->va, cdb->bd->sz);

	if (status)
		sata_cdb_push(cdb);

	return status;
}

static int __sata_rw(sata_softc *ctx, uint32_t lba, uint32_t b, uint32_t *bfr,
		     uint32_t bfr_sz, int write)
{
	sata_cdb_t *cdb     = NULL;
	int        status   = 0;
	bfr_desc_t bfr_desc;

#if (VERBOSE)
	xprintf("lba: %u, b: %u, bfr: %p, bfr_sz: %u, w: %d\n",
		lba,
		b,
		bfr,
		bfr_sz,
		write);
#endif

	status = sata_cdb_pop(&ctx->sata_dev, ctx->port, &cdb);
	if (status)
		return status;

	if (!bfr || bfr_sz < (b * SECTOR_SZ)) {
		status = -2;
		goto done;
	}

	bfr_desc.pa = (uint32_t)bfr;
	bfr_desc.va = bfr;
	bfr_desc.sz = bfr_sz;
	bfr_desc.next = NULL;

	cdb->opcode    = write ? ATA_WFPDMA_Q : ATA_RFPDMA_Q;
	cdb->block_cnt = b;
	cdb->bd        = &bfr_desc;
	cdb->bfr_len   = bfr_desc.sz;
	cdb->callback  = __sata_callback;
	cdb->lba       = lba;

	CACHE_INVAL_RANGE(cdb->bd->va, cdb->bd->sz);

	status = sata_cmd_issue(cdb);
	assert(status == 0);

done:
	if (status)
		sata_cdb_push(cdb);

	return status;
}

static int irq_wait(void)
{
	return 0;
}

static void irq_en(int n)
{
	n = n;
}

#define SATA_TOP_CTRL_REG_LENGTH		0x24
#define SATA_TOP_CTRL_PHY_CTRL_OFS		0xc
#define SATA_TOP_CTRL_PHY_CTRL_LEN		0x8
#define SATA_TOP_CTRL_PHY_CTRL_1		0x0
#define SATA_TOP_CTRL_PHY_CTRL_2		0x4
#define SATA_TOP_CTRL_REGS_PER_PORT		2
#define SATA_TOP_CTRL_1_PHY_DEFAULT_POWER_STATE	BIT(14)
#define SATA_TOP_CTRL_2_PHY_GLOBAL_RESET	BIT(14)

/* put the core back to power down state */
static int dev_sata_uninit_phy(sata_softc *ctx)
{
	/* yfzhang@broadcom.com has stated that the core will only have (2)
	 * ports. Further, the RDB currently lacks documentation for these
	 * registers. So just keep a map of which port corresponds to these
	 * magic registers.
	 */
	const uint32_t top_ctrl = BCHP_PHYSICAL_OFFSET +
		BCHP_SATA_TOP_CTRL_REG_START;
	const uint32_t port_to_phy_ctrl_ofs[NUM_SATA_PHY_CTL] = {
		SATA_TOP_CTRL_PHY_CTRL_OFS + (0 * SATA_TOP_CTRL_PHY_CTRL_LEN),
#if NUM_SATA_PHY_CTL > 1
		SATA_TOP_CTRL_PHY_CTRL_OFS + (1 * SATA_TOP_CTRL_PHY_CTRL_LEN),
#endif
#if NUM_SATA_PHY_CTL > 2
#error Need to specify another port ctrl register set.
#endif
	};
	int port;
	uint32_t reg;

	for (port = 0; port < NUM_SATA_PHY_CTL; port++) {
		uint32_t p;

		/* clear PHY_DEFAULT_POWER_STATE */
		p = top_ctrl + port_to_phy_ctrl_ofs[port] +
			SATA_TOP_CTRL_PHY_CTRL_1;
		reg = DEV_RD(p) | SATA_TOP_CTRL_1_PHY_DEFAULT_POWER_STATE;
		DEV_WR(p, reg);
	}

	return 0;
}

static int dev_sata_init_phy(sata_softc *ctx)
{
	/* yfzhang@broadcom.com has stated that the core will only have (2)
	 * ports. Further, the RDB currently lacks documentation for these
	 * registers. So just keep a map of which port corresponds to these
	 * magic registers.
	 */
	const uint32_t top_ctrl = BCHP_PHYSICAL_OFFSET +
		BCHP_SATA_TOP_CTRL_REG_START;
	const uint32_t port_to_phy_ctrl_ofs[NUM_SATA_PHY_CTL] = {
		SATA_TOP_CTRL_PHY_CTRL_OFS + (0 * SATA_TOP_CTRL_PHY_CTRL_LEN),
#if NUM_SATA_PHY_CTL > 1
		SATA_TOP_CTRL_PHY_CTRL_OFS + (1 * SATA_TOP_CTRL_PHY_CTRL_LEN),
#endif
#if NUM_SATA_PHY_CTL > 2
#error Need to specify another port ctrl register set.
#endif
	};
	int port;
	uint32_t reg;

	for (port = 0; port < NUM_SATA_PHY_CTL; port++) {
		uint32_t p;

		/* clear PHY_DEFAULT_POWER_STATE */
		p = top_ctrl + port_to_phy_ctrl_ofs[port] +
			SATA_TOP_CTRL_PHY_CTRL_1;
		reg = DEV_RD(p) & ~SATA_TOP_CTRL_1_PHY_DEFAULT_POWER_STATE;
		DEV_WR(p, reg);

		/* toggle PHY_GLOBAL_RST */
		p = top_ctrl + port_to_phy_ctrl_ofs[port] +
			SATA_TOP_CTRL_PHY_CTRL_2;
		reg = DEV_RD(p) | SATA_TOP_CTRL_2_PHY_GLOBAL_RESET;
		DEV_WR(p, reg);
		reg &= ~SATA_TOP_CTRL_2_PHY_GLOBAL_RESET;
		DEV_WR(p, reg);
	}

	return 0;
}

static int dev_sata_init(sata_softc *ctx)
{
	int      status;
	uint32_t *mem;
	uint32_t mem_sz = 512 * 1024;

	status = dev_sata_init_phy(ctx);
	if (status)
		return status;

	if (!IS_ROOT_CTX(ctx))
		return -1;

	mem = KMALLOC(mem_sz, 4096);
	if (mem != NULL) {
		um_ahci_cfg_t cfg;

		/*
		 * Initialize internal state, including memory maps for all
		 * descriptors and the memory pool
		 */
		cfg.dev         = &ctx->sata_dev;
		cfg.reg_base    = BPHYSADDR(BCHP_SATA_GRB_REG_START);
		cfg.mem_base_va = mem;
		cfg.mem_base_pa = (uint32_t)mem;
		cfg.mem_base_sz = mem_sz;
		cfg.irq_wait    = irq_wait;
		cfg.irq_en      = irq_en;

		status = um_ahci_core_init(&cfg);
	}

	return status;
}

static void dev_sata_probe(bolt_driver_t *drv, unsigned long port,
				unsigned long probe_b, void *probe_ptr)
{
	sata_softc *ctx;

	ctx = KMALLOC(sizeof(sata_softc), SECTOR_SZ);
	if (!ctx)
		return;

	memset(ctx, 0, sizeof(sata_softc));

	if (port != 0) {
		/*
		 * The sata0 device serves as the root context for the other
		 * ports. All other devices are dummies in order to fit in the
		 * bolt device driver model.
		 */
		bolt_device_t *bolt_dev = bolt_finddev("sata0");

		if (!bolt_dev) {
			xprintf("%s: couldn't locate sata0\n", __func__);
			KFREE(ctx);
			return;
		}

		if (!bolt_dev->dev_softc) {
			xprintf("%s: couldn't locate sata0 ctx\n", __func__);
			KFREE(ctx);
			return;
		} else
			ctx->root_ctx = (sata_softc *)bolt_dev->dev_softc;

		ctx->port = port;
	}

	(void) bolt_attach_idx(drv, port, ctx, NULL, "SATA3 AHCI Device");
}

static int dev_sata_open(bolt_devctx_t *bctx)
{
	int status;
	sata_softc *root_ctx;
	sata_softc *port_ctx;

#if !defined(CFG_SATA)
	return -1;
#endif

	root_ctx = port_ctx = (sata_softc *)bctx->dev_softc;
	if (port_ctx->root_ctx)
		root_ctx = port_ctx->root_ctx;

	if (!root_ctx->initialized) {
		status = dev_sata_init(root_ctx);
		if (!status) {
			root_ctx->initialized = 1;
		}
	}

	if (port_ctx != root_ctx)
		port_ctx->sata_dev = root_ctx->sata_dev;

	if (root_ctx->initialized && !port_ctx->ready) {
		uint32_t port_mask;
		/*
		 * Scan for drives
		 */
		if (sata_scan(&root_ctx->sata_dev, BIT(port_ctx->port), &port_mask)) {
			/* Some drives take longer to power up.  */
			status = dev_sata_uninit_phy(root_ctx);
			if (status)
				return status;
			msleep(100);
			status = dev_sata_init_phy(root_ctx);
			if (status)
				return status;
			status = sata_scan(&root_ctx->sata_dev, BIT(port_ctx->port), &port_mask);
			if (status)
				return status;
		}
		if (port_mask & (1 << port_ctx->port)) {
			__sata_id(port_ctx);
			port_ctx->ready = 1;
		}
	}

	return port_ctx->ready ? 0 : -1;
}

static int dev_sata_close(bolt_devctx_t *bctx)
{
	return 0;
}

static int dev_sata_read(bolt_devctx_t *bctx, iocb_buffer_t *buffer)
{
	int        status = 0;
	uint32_t   *dst_bfr;
	uint32_t   dst_bfr_left;
	uint64_t   cur_ofs;
	sata_softc *port_ctx;

	port_ctx = (sata_softc *)bctx->dev_softc;

	if (!port_ctx->ready)
		return -1;

	if (buffer->buf_length == 0)
		return -1;

	dst_bfr      = (uint32_t *)buffer->buf_ptr;
	dst_bfr_left = buffer->buf_length;
	cur_ofs      = buffer->buf_offset;

	/* handle head partials */
	if (cur_ofs % SECTOR_SZ) {
		uint32_t lba = cur_ofs / SECTOR_SZ;
		uint32_t pad = cur_ofs % SECTOR_SZ;
		uint32_t cnt;

		if (pad + dst_bfr_left < SECTOR_SZ)
			cnt = dst_bfr_left;
		else
			cnt = SECTOR_SZ - pad;

		status = __sata_rw(port_ctx, lba, 1, port_ctx->tmp_bfr,
					SECTOR_SZ, 0);
		if (!status) {
			memcpy(dst_bfr, ((uint8_t *)&port_ctx->tmp_bfr[0]) + pad,
				cnt);

			cur_ofs += cnt;
			dst_bfr += cnt;
			dst_bfr_left -= cnt;
		}
	}

	if (!status) {
		/* handle fully-aligned */
		while (dst_bfr_left >= SECTOR_SZ && status == 0) {
			uint32_t lba = cur_ofs / SECTOR_SZ;
			uint32_t b = dst_bfr_left / SECTOR_SZ;
			uint32_t *bfr;

#if (ZERO_COPY)
			if (b >= MAX_SECTS)
				b = MAX_SECTS;
			bfr = dst_bfr;
#else
			if (b >= TMP_BFR_NUM_SECTS)
				b = TMP_BFR_NUM_SECTS;
			bfr = port_ctx->tmp_bfr;
#endif

			status = __sata_rw(port_ctx, lba, b, bfr, b * SECTOR_SZ,
						0);
			if (!status) {
				if (!ZERO_COPY)
					memcpy(dst_bfr, port_ctx->tmp_bfr,
						b * SECTOR_SZ);

				cur_ofs += b * SECTOR_SZ;
				dst_bfr += b * SECTOR_SZ;
				dst_bfr_left -= b * SECTOR_SZ;
			}
		}
	}

	if (!status) {
		/* handle tail partials */
		if (dst_bfr_left != 0) {
			uint32_t lba = cur_ofs / SECTOR_SZ;

			status = __sata_rw(port_ctx, lba, 1, port_ctx->tmp_bfr,
						SECTOR_SZ, 0);
			if (!status) {
				memcpy(dst_bfr, port_ctx->tmp_bfr, dst_bfr_left);

				cur_ofs += dst_bfr_left;
				dst_bfr += dst_bfr_left;
				dst_bfr_left -= dst_bfr_left;
			}
		}
	}

	if (!status)
		buffer->buf_retlen = buffer->buf_length - dst_bfr_left;

	return status;
}

static int dev_sata_write(bolt_devctx_t *bctx, iocb_buffer_t *buffer)
{
	xprintf("%s: writing unimplemented\n", __func__);
	return -1;
}

static int dev_sata_inpstat(bolt_devctx_t *bctx, iocb_inpstat_t *inpstat)
{
	inpstat->inp_status = 1;
	return 0;
}

static const bolt_devdisp_t sata_devdisp = {
	dev_sata_open,
	dev_sata_read,
	dev_sata_inpstat,
	dev_sata_write,
	NULL,
	dev_sata_close,
	NULL,
	NULL
};

bolt_driver_t satadrv = {
	"Broadcom SATA3 AHCI Driver",
	"sata",
	BOLT_DEV_DISK,
	&sata_devdisp,
	dev_sata_probe,
};
