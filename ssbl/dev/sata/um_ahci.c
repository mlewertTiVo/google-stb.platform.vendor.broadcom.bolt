/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "um_ahci.h"
#include "fifo.h"
#include "mem_pool.h"

#if ((BSP_CFG_SATA_SPEED_ALLOWED > 3) || \
	(BSP_CFG_SATA_SPEED_ALLOWED < 0))
#error Bad value for BSP_CFG_SATA_SPEED_ALLOWED
#endif

/* Data */

static ahci_rx_fises_t     *rx_fises[MAX_SATA_PHY_PORTS];
static ahci_cmd_list_hdr_t *cmd_list_hdrs[MAX_SATA_PHY_PORTS];
static ahci_cmd_tbl_t      *cmd_tbls[MAX_SATA_PHY_PORTS];
static uint64_t            rx_fises_pa[MAX_SATA_PHY_PORTS];
static uint64_t cmd_list_hdrs_pa[MAX_SATA_PHY_PORTS][NUM_CMD_LIST_HDRS];
static uint64_t cmd_tbls_pa[MAX_SATA_PHY_PORTS][NUM_CMD_LIST_HDRS];
static sata_cdb_t          *tag_to_cdb[NUM_TAGS];
static cdb_pool_t          __cdb_pool;
static cdb_pool_t          *cdb_pool = &__cdb_pool;
static fifo_t              cdb_fifo;

struct task_state_t {
	thr_t thread;
};
struct task_state_t pt_state;
struct task_state_t cet_state;

uint32_t g_base;

/* Externs */

/* Static Functions */

/*
 * You must use the register access functions when manipulating SATA
 * core registers. Dereferencing SATA core pointers directly is not
 * recommended!
 */

/**
 *	__r32: Read a 32-bit value from addr
 */
static uint32_t __r32(void *addr)
{
	uint32_t val = *((volatile uint32_t *)addr);
#if (PRINT_REG_ACCESSES)
	func_printf("%p => %08xh\n", addr, val);
#endif
	return val;
}

/**
 *	__w32: Write a 32-bit value from addr
 */
static void __w32(void *addr, uint32_t val)
{
#if (PRINT_REG_ACCESSES)
	func_printf("%p <= %08xh\n", addr, val);
#endif
	barrier();
	*((volatile uint32_t *)addr) = val;
	barrier();
}

/**
 *	__rwb32: Read a 32-bit value from addr and write-back the same value
 *		to addr.
 */
static uint32_t __rwb32(void *addr)
{
	uint32_t val = __r32(addr);
	__w32(addr, val);
	return val;
}

static int is_queued(const sata_cdb_t *cdb)
{
	return (cdb->opcode == ATA_RFPDMA_Q || cdb->opcode == ATA_WFPDMA_Q);
}

static void init_h2d_fis_t(h2d_fis_t *fis, uint8_t cmd)
{
	fis->dw[0] = H2D_FIS_TYPE_VALUE;
	if (cmd)
		fis->dw[0] |= (1 << H2D_FIS_0_FLAGS_C) | (cmd << H2D_FIS_0_CMD);
}

static int wait_bits_timeout(void *reg, uint32_t msk, int clr, uint32_t timeout,
				uint32_t all)
{
	int            status  = ERR_TIMEOUT;
	uint32_t       t_start = _getticks();
	const uint32_t xor     = clr ? 0xffffffff : 0x0;

	do {
		uint32_t tmp = __r32(reg);
		if (all) {
			if (((tmp ^ xor) & msk) == msk) {
				status = 0;
				break;
			}
		} else {
			if ((tmp ^ xor) & msk) {
				status = 0;
				break;
			}
		}
		usleep(SECS_TO_TICKS(1));
	} while ((_getticks() - t_start) <= timeout);

	return status;
}

/**
 * tag_pop - Pops an unused NCQ tag associated with the given SATA device->port,
 * and attaches it to the provided CDB.
 *
 * NOTE: Must be under atomic context.
 *
 * @dev  - sata_dev_t
 * @port - port number
 * @cdb  - CDB
 */
static int tag_pop(sata_dev_t *dev, int port, sata_cdb_t *cdb)
{
	int status = 0;
	int tag;

	if (dev->non_q_barrier[port])
		status = ERR_BUSY_RETRY_LATER;

	if (!status) {
		for_each_tag(tag) {
			if (!(dev->tags_allocated[port] & BIT(tag))) {
				dev->tags_allocated[port] |= BIT(tag);
				break;
			}
		}

		if (tag == NUM_TAGS)
			status = ERR_NO_TAGS_FREE;
	}

	if (!status) {
		if (!is_queued(cdb))
			dev->non_q_barrier[port] = 1;

		cdb->tag = tag;
	}

	return status;
}

/**
 * tag_push - Pushes NCQ tag back into free set.
 *
 * NOTE: Must be under atomic context.
 *
 * @dev       - sata_dev_t
 * @port      - port number
 * @cdb       - CDB
 * @clr_start - Clear the 'tag_started' bitmask
 */
static void tag_push(sata_dev_t *dev, int port, sata_cdb_t *cdb, int clr_start)
{
	assert(cdb->tag >= 0);

	if (clr_start) {
		assert(dev->tags_started[port] & BIT(cdb->tag));
		dev->tags_started[port] &= ~BIT(cdb->tag);
	}

	assert(dev->tags_allocated[port] & BIT(cdb->tag));
	dev->tags_allocated[port] &= ~BIT(cdb->tag);

	cdb->tag = INV_TAG;
}

/**
 * init_cmd_list_hdr - Initialize a command list header
 */
static void init_cmd_list_hdr(ahci_cmd_list_hdr_t *cmd_list_hdr,
			      uint32_t bfr,
			      uint32_t bfr_len,
			      int is_write,
			      int is_reset,
			      int no_auto_clr_bsy,
			      int prdt_entries)
{
	uint32_t dw0 = 0;

	dw0 |= sizeof(h2d_fis_t) / sizeof(uint32_t);
	dw0 |= ((is_write ? 1 : 0)  << CLH_DW0_WRITE);
	dw0 |= ((is_reset ? 1 : 0)  << CLH_DW0_RESET);
	dw0 |= ((no_auto_clr_bsy ? 0 : 1) << CLH_DW0_CLR_BSY_ON_R_OK);
	dw0 |= ((prdt_entries & 0xFFFF) << CLH_DW0_PRDT_LEN);

	cmd_list_hdr->dw0 = dw0;
	cmd_list_hdr->prd_byte_cnt = 0;

	/* TODO */
	bfr = bfr;
	bfr_len = bfr_len;
}

/**
 * build_cmd - Prepare the descriptors needed to execute the given CDB.
 */
static int build_cmd(sata_cdb_t *cdb)
{
	int                status = 0;
	const int          port   = cdb->port;
	const int          tag    = cdb->tag;
	phys_region_desc_t *prdt  = &cmd_tbls[port][tag].prdt[0];
	h2d_fis_t          *cfis  = &cmd_tbls[port][tag].cfis;

	bfr_desc_t         *bd;
	int                sg_cnt;

	/* Build H2D FIS */
	init_h2d_fis_t((h2d_fis_t *)cfis, cdb->opcode);
	cfis->dw[0] |= (cdb->block_cnt & 0xFF) << 24;
	cfis->dw[1] = (0x40 << H2D_FIS_1_DEVICE) | (cdb->lba & 0xFFFFFF);
	cfis->dw[2] = (((cdb->block_cnt >> 8) & 0xFF) << 24) |
				((cdb->lba >> 24) & 0xFF);
	cfis->dw[3] = (0x08 << H2D_FIS_3_CONTROL) | ((tag & 0x1F) << 3);
	cfis->dw[4] = 0;

	/* Build PRDT */
	init_cmd_list_hdr(&cmd_list_hdrs[port][tag],
			  cdb->bd->pa,
			  cdb->bfr_len,
			  CMD_DIR(cdb->opcode),
			  0,
			  1,
			  1);

	/* This driver currently supports up to 8 scatter-gather chunks */
	sg_cnt = 0;
	for_each_bd(bd, cdb->bd) {
		assert(sg_cnt != MAX_PRDT);

		prdt[sg_cnt].dw[0] = bd->pa;
		prdt[sg_cnt].dw[1] = 0;
		prdt[sg_cnt].dw[2] = 0;
		prdt[sg_cnt].dw[3] = bd->sz;

		sg_cnt++;
	}

	cache_flush(&cmd_tbls[port][tag], sizeof(ahci_cmd_tbl_t));
	cache_flush(&cmd_list_hdrs[port][tag], sizeof(ahci_cmd_list_hdr_t));

	return status;
}

/**
 * start_cmd - Arm and start the command associated with the CDB.
 */
static void start_cmd(sata_cdb_t *cdb)
{
	ahci_port_regs_t *pr = cdb->dev->port_regs[cdb->port];

	/* Know which CDB to callback when an IRQ for tag completion occurs */
	tag_to_cdb[cdb->tag] = cdb;

	/* Last chance for data to make it to DRAM before we start accessing */
	barrier();

	mutex_lock(&cdb->dev->mtx);

	/* tags_started and sactive bitmaps must be updated atomically */
	cdb->dev->tags_started[cdb->port] |= BIT(cdb->tag);
	if (is_queued(cdb))
		__w32(&pr->sactive, BIT(cdb->tag));
	__w32(&pr->ci, BIT(cdb->tag));

	mutex_unlock(&cdb->dev->mtx);
}

static int dev_mem_setup(int port, bfr_desc_t *bfr_desc)
{
	const int  cmd_list_hdrs_sz = sizeof(ahci_cmd_list_hdr_t) *
					NUM_CMD_LIST_HDRS;
	const int  cmd_tbls_tot_sz  = sizeof(ahci_cmd_tbl_t) *
					NUM_CMD_LIST_HDRS;
	const int  rx_fises_sz      = sizeof(ahci_rx_fises_t);
	const void *start_va        = bfr_desc->va;
	int        i;

	dbg_printf("va=%xh pa=%llxh\n", (unsigned int)bfr_desc->va, (unsigned long long)bfr_desc->pa);
	assert(((unsigned int)bfr_desc->va % PAGE_SIZE) == 0);

	/* Calculate VAs/PAs of command list headers */
	cmd_list_hdrs[port] = (ahci_cmd_list_hdr_t *)bfr_desc->va;
	for (i = 0; i < NUM_CMD_LIST_HDRS; i++) {
		cmd_list_hdrs_pa[port][i] = bfr_desc->pa;

		bfr_desc->va += sizeof(ahci_cmd_list_hdr_t);
		bfr_desc->pa += sizeof(ahci_cmd_list_hdr_t);
	}

	/*
	 * Although not entirely necessary, also align it to a page boundary
	 * to make things pretty.
	 *
	 * (It would be a nice test case to move these around...)
	 */
	if (bfr_desc->pa % PAGE_SIZE) {
		bfr_desc->pa = (bfr_desc->pa & ~(PAGE_SIZE - 1)) + PAGE_SIZE;
		bfr_desc->va = (void *)
			((uintptr_t)bfr_desc->va & ~(PAGE_SIZE - 1))
			+ PAGE_SIZE;
	}

	/* Calculate VAs/PAs of command tables */
	cmd_tbls[port] = (ahci_cmd_tbl_t *)bfr_desc->va;
	for (i = 0; i < NUM_CMD_LIST_HDRS; i++) {
		cmd_tbls_pa[port][i] = bfr_desc->pa;

		bfr_desc->va += sizeof(ahci_cmd_tbl_t);
		bfr_desc->pa += sizeof(ahci_cmd_tbl_t);
	}

	/* Calculate VAs/PAs for received FISes */
	rx_fises[port] = (ahci_rx_fises_t *)bfr_desc->va;
	rx_fises_pa[port] = bfr_desc->pa;
	bfr_desc->va += rx_fises_sz;
	bfr_desc->pa += rx_fises_sz;
	dbg_printf("fis[%d]: %08xh\n", port, (uint32_t)rx_fises_pa[port]);

	/*
	 * Although not entirely necessary, also align it to a page boundary
	 * to make things pretty.
	 *
	 * (It would be a nice test case to move these around...)
	 */
	if (bfr_desc->pa % PAGE_SIZE) {
		bfr_desc->pa = (bfr_desc->pa & ~(PAGE_SIZE - 1)) + PAGE_SIZE;
		bfr_desc->va = (void *)
			((uintptr_t)bfr_desc->va & ~(PAGE_SIZE - 1))
			+ PAGE_SIZE;
	}

	bfr_desc->sz -= bfr_desc->va - start_va;

	memset((void *)cmd_list_hdrs[port], 0, cmd_list_hdrs_sz);
	memset((void *)cmd_tbls[port], 0, cmd_tbls_tot_sz);
	memset((void *)rx_fises[port], 0, rx_fises_sz);

	/* Everything is contiguous and 1-to-1 for simplicity */
	for (i = 0; i < NUM_CMD_LIST_HDRS; i++) {
		ahci_cmd_list_hdr_t *hdr = &cmd_list_hdrs[port][i];
		hdr->cmd_tbl_base = (uint32_t)cmd_tbls_pa[port][i];
	}

	return 0;
}

static int wait_sata_reset(ahci_port_regs_t *pr)
{
	int             status    = ERR_TIMEOUT;
	uint32_t        t_start   = _getticks();
	pxis_int_stat_t pxie_bits = { .all = 0 };

	pxie_bits.bits.d2h_fis_int = 1;
	pxie_bits.bits.pio_setup_fis_int = 1;
	pxie_bits.bits.dma_setup_fis_int = 1;
	pxie_bits.bits.sdb_fis_int = 1;
	pxie_bits.bits.unk_fis_int = 1;
	pxie_bits.bits.iface_fatal_err_stat = 1;
	pxie_bits.bits.hbus_data_err_stat = 1;
	pxie_bits.bits.hbus_fatal_err_stat = 1;
	pxie_bits.bits.tf_err_stat = 1;

	do {
		if (!(__r32(&pr->ci) & BIT(DEF_NON_Q_TAG))) {
			status = 0;
			break;
		}
		if (__r32(&pr->int_status) & pxie_bits.all) {
			status = 0;
			break;
		}
		usleep(MS_TO_TICKS(50));
	} while ((_getticks() - t_start) <= SECS_TO_TICKS(2));

	return status;
}

static void hba_reset(sata_dev_t *dev)
{
	hba_ghc_cap_t ghc;

	dbg_printf("Resetting HBA...\n");

	/* Enable AHCI */
	ghc.all = __r32(&dev->hba_regs->ghc);
	ghc.bits.ahci_en = 1;
	__w32(&dev->hba_regs->ghc, ghc.all);

	/* Assert reset */
	ghc.all = __r32(&dev->hba_regs->ghc);
	ghc.bits.hba_rst = 1;
	__w32(&dev->hba_regs->ghc, ghc.all);

	/* Wait for reset to clear */
	/* TODO: Add timeout */
	do {
		ghc.all = __r32(&dev->hba_regs->ghc);
	} while (ghc.bits.hba_rst);

	/* Re-enable AHCI since it's cleared on reset */
	ghc.bits.ahci_en = 1;
	__w32(&dev->hba_regs->ghc, ghc.all);
}

static void port_reset(sata_dev_t *dev, int port)
{
	ahci_port_regs_t   *pr = dev->port_regs[port];
	pxcmd_cmd_n_stat_t pxcmd;

	dbg_printf("Resetting port %d\n", port);

	pxcmd.all = __r32(&pr->cmd_and_stat);
	pxcmd.bits.start = 0;
	pxcmd.bits.fis_rx_en = 0;
	__w32(&pr->cmd_and_stat, pxcmd.all);

	/*
	 * AHCI 10.1.2
	 * Software should wait at least 500 milliseconds for .CR to clear
	 * after clearing .ST
	 */
	usleep(MS_TO_TICKS(500));

	pxcmd.all = 0;
	pxcmd.bits.cmd_list_running = 1;
	pxcmd.bits.fis_rx_running = 1;
	while (__r32(&pr->cmd_and_stat) & pxcmd.all)
		;
}

static int port_phy_init(sata_dev_t *dev, int port, int power_up)
{
	int                    status   = 0;
	ahci_port_regs_t       *pr      = dev->port_regs[port];
	pxsctl_scr2_scontrol_t scontrol;
	pxserr_scr1_serror_t   serror;
	pxcmd_cmd_n_stat_t     pxcmd;
	pxssts_scr0_sstatus_t  sstatus;
	int timeout = (3 * 1000) / 20; /* 3 seconds */

	sstatus.all = 0;

	dbg_printf("Initializing PHY for port %d\n", port);

	if (power_up) {
		init_phy(port, dev->reg_base);

		/* Uncondtionally clear out all port errors */
		__rwb32(&pr->serror);
		__rwb32(&pr->int_status);
		__w32(&dev->hba_regs->is, BIT(port));
		serror.all = __r32(&pr->serror);
		if (serror.diag.bits.eXchanged) {
			serror.diag.bits.eXchanged = 1;
			__w32(&pr->serror, serror.all);
		}

		/* Ensure writes to reset registers have been sent */
		barrier();
	}

	/* Deassert DET */
	scontrol.all = __r32(&pr->scontrol);
	scontrol.bits.det = 0;
	__w32(&pr->scontrol, scontrol.all);

	barrier();

	usleep(MS_TO_TICKS(100));

	/* Assert SUD */
	pxcmd.all = __r32(&pr->cmd_and_stat);
	pxcmd.bits.spin_up = 1;
	__w32(&pr->cmd_and_stat, pxcmd.all);

	/* Ensure COMINIT is cleared before touching status */
	barrier();

	while (--timeout) {
		dmb();
		sstatus.all = __r32(&pr->sstatus);
		if (sstatus.bits.det == SCR2_DET_DEVICE_AND_COMM_OK)
			break;
		msleep(20);
	}

	if (sstatus.bits.det != SCR2_DET_DEVICE_AND_COMM_OK) {
		err_printf("Spin up fail. No disk?\n");
		dbg_printf("SUD: det:%d spd:%d ipm:%d)\n",
			sstatus.bits.det,
			sstatus.bits.spd,
			sstatus.bits.ipm);
		return ERR_NO_DRIVE;
	}

	/* Assert DET */
	scontrol.all = __r32(&pr->scontrol);
	scontrol.bits.ipm = 3;
	scontrol.bits.spd = BSP_CFG_SATA_SPEED_ALLOWED;
	scontrol.bits.det = 1;
	__w32(&pr->scontrol, scontrol.all);

	usleep(MS_TO_TICKS(100));

	/* Deassert DET */
	scontrol.all = __r32(&pr->scontrol);
	scontrol.bits.det = 0;
	__w32(&pr->scontrol, scontrol.all);

	usleep(MS_TO_TICKS(100));

	if (power_up) {
		dbg_printf("Clearing port interrupt status\n");
		__rwb32(&pr->int_status);
	}

	return status;
}

static void sata_test_setup(sata_dev_t *dev)
{
	uint32_t base          = dev->reg_base;
	uint32_t *top_bus_ctrl = (uint32_t *)(base + SATA_TOP_CTRL_BUS_CTRL);
	uint32_t pctrl, reg_value;
	int i;

	/* must write this before writing to IMPL reg */
	reg_value = __r32(top_bus_ctrl);
	reg_value |= 0x00010000;
	__w32(top_bus_ctrl, reg_value);

	/* Disable port multiplier support for now. */
	reg_value = __r32((uint32_t *)(base + SATA_AHCI_GHC_HBA_CAP));
	reg_value &= 0xfffdffff; /* Bit 17: Support_Port_Multiplier */
	__w32((uint32_t *)(base + SATA_AHCI_GHC_HBA_CAP), reg_value);

	pctrl = (uint32_t)dev->hba_regs + SATA_FIRST_PORT_CTRL;

	for (i = 0; i < MAX_SATA_PHY_PORTS; i++) {
		/* Adjust timeout to allow PLL sufficient time to lock
		 * while waking up from slumber mode.
		 */
		__w32((uint32_t *)SATA_PORT_PCTRL6(pctrl), 0xff1003fc);

		pctrl += SATA_NEXT_PORT_CTRL_OFFSET;
	}

	/* Must clear after use */
	reg_value = __r32(top_bus_ctrl);
	reg_value &= 0xfffeffff;
	__w32(top_bus_ctrl, reg_value);

	/* wait until bit gets cleared */
	do {
		reg_value = __r32(top_bus_ctrl);
		reg_value &= 0x00010000;
	} while (reg_value != 0);
}

static void handle_port_irq(sata_dev_t *sata_dev, int port)
{
	int               i;
	int               clear_barrier = 0;
	ahci_port_regs_t  *pr           = sata_dev->port_regs[port];
	pxis_int_stat_t   int_status    = { .all = __r32(&pr->int_status) };

	/* Handle non-fatal interface interrupt */
	if (int_status.all & BIT(PXIS_INFS)) {
		/* Get PxSERR */
		dbg_printf("P_%d_SERR=%xh\n", port, __r32(&pr->serror));
		assert(0);
		int_status.all &= ~BIT(PXIS_INFS);
	}

	/* Handle task file errors */
	if (int_status.all & BIT(PXIS_TFES)) {
		callback_t callback;

		tag_to_cdb[DEF_NON_Q_TAG]->tf_data.all = __r32(&pr->tf_data);

		callback = tag_to_cdb[DEF_NON_Q_TAG]->callback;
		if (callback) {
			callback(tag_to_cdb[DEF_NON_Q_TAG], ERR_TASKFILE);
			clear_barrier = 1;
		}

		int_status.all &= ~BIT(PXIS_TFES);
	}

	/* Handle SDB FIS reception */
	if (int_status.all & BIT(PXIS_SDBS)) {
		uint32_t tags_done;

		mutex_lock(&sata_dev->mtx);
		tags_done = ~__r32(&pr->sactive) &
					sata_dev->tags_started[port];
		mutex_unlock(&sata_dev->mtx);

		for_each_tag(i) {
			if (tags_done & BIT(i)) {
				if (tag_to_cdb[i]->callback)
					tag_to_cdb[i]->callback(tag_to_cdb[i],
								0);
			}
		}

		int_status.all &= ~BIT(PXIS_SDBS);
	}

	/* Handle PIO Setup FIS reception */
	if (int_status.all & BIT(PXIS_PSS)) {
		sata_cdb_t *cdb = tag_to_cdb[DEF_NON_Q_TAG];

		cdb->tf_data.all = __r32(&pr->tf_data);

		if (cdb->callback) {
			cdb->callback(cdb, 0);
			clear_barrier = 1;
		}

		int_status.all &= ~BIT(PXIS_PSS);
	}

	/* Handle D2H FIS reception */
	if (int_status.all & BIT(PXIS_DHRS)) {
		/* TODO */
		int_status.all &= ~BIT(PXIS_DHRS);
	}

	/* Handle PHY ready change status */
	if (int_status.all & BIT(PXIS_PRCS)) {
		pxssts_scr0_sstatus_t __maybe_unused sstatus;

		dbg_printf("PHY event on port %d\n", port);

		/*
		 * TODO: This needs to be handled this properly!
		 *
		 * If there are multiple cdbs in-flight, we need to invoke a
		 * freeze the new command pipe. Then, we can invoke the
		 * callbacks on the entire queue. Once that's done, gracefully
		 * reinitialize the port.
		 */
		port_reset(sata_dev, port);
		port_phy_init(sata_dev, port, 0);
		usleep(MS_TO_TICKS(100));

		sstatus.all = __r32(&pr->sstatus);
		__rwb32(&pr->serror);

#if (DEBUG)
		if (sstatus.bits.det != SCR2_DET_DEVICE_AND_COMM_OK)
			dbg_printf("No drive on port %d\n", port);
		else
			dbg_printf("Drive detected on port %d\n", port);
#endif

		int_status.all &= ~BIT(PXIS_PRCS);
	}

	/* Handle port change status */
	if (int_status.all & BIT(PXIS_PCS)) {
		dbg_printf("COMINIT received on port %d\n", port);

		int_status.all &= ~BIT(PXIS_PCS);
	}

	__w32(&pr->int_status, ~int_status.all);

	if (int_status.all) {
		err_printf("Unhandled interrupt! %xh\n", int_status.all);
		assert(0);
	}

	if (clear_barrier)
		sata_dev->non_q_barrier[port] = 0;
}

#if defined(__SINGLE_THREADED)
static int task_irqpoll(void *arg)
{
	const int  is_single_threaded = 1;
#else
static void *task_irqpoll(void *arg)
{
	const int  is_single_threaded = 0;
#endif
	sata_dev_t *sata_dev = (sata_dev_t *)arg;
	int irq_num = 0, prev_irq_num, inner_timeout;

#if (DEBUG)
	if (!is_single_threaded)
		dbg_printf("Start %s\n", __func__);
#endif
	for (;;) {

		if (is_single_threaded) {
			dbg_printf("Outer POLL %s\n", __func__);
		}

		if (!is_single_threaded) {
			prev_irq_num = irq_num;
			irq_num = sata_dev->irq_wait();

			/* Detect missed interrupts */
			if (prev_irq_num + 1 != irq_num) {
				fprintf(stderr, "missed %d irqs\n",
					(irq_num - prev_irq_num));
			}
		}

		/* Command (drive response) within 5 seconds. */
		inner_timeout = 5000;

		for (;;) {
			int      port;
			uint32_t is = __r32(&sata_dev->hba_regs->is);

			if (is_single_threaded) {
				dbg_printf("Inner POLL %s\n", __func__);
			}

			if (is) {
				for_each_port(port) {
					if (is & BIT(port))
						handle_port_irq(sata_dev,
								port);
				}

				__w32(&sata_dev->hba_regs->is, is);
			} else {
				int cmds_outstanding = 0;

				if (!is_single_threaded)
					break;

				/* Single-thread mode requires polling */
				for_each_port(port) {
					if (sata_dev->tags_started[port])
						cmds_outstanding = 1;
				}

				if (!cmds_outstanding)
					break;
			}

			if (is_single_threaded) {
				if (0 >= inner_timeout--) {
					dbg_printf("Inner POLL TIMEOUT!%s\n",
						__func__);
#if defined(__SINGLE_THREADED)
					return ERR_TIMEOUT;
#else
					break;
#endif
				}
				msleep(1);
			}
		}

		if (is_single_threaded)
			break;
		else {
			/* Re-enable interrupts */
			sata_dev->irq_en(1);
		}
	}

	if (!is_single_threaded) {
		dbg_printf("irqpoll task is exiting\n");
		thr_exit(NULL);
	}

#if defined(__SINGLE_THREADED)
	return 0;
#else
	return NULL;
#endif
}

static int task_irqpoll_init(sata_dev_t *dev)
{
	int status = 0;

#if !defined(__SINGLE_THREADED)
	status = thr_create(&pt_state.thread, &task_irqpoll, dev);
	if (status)
		err_printf("couldn't create polling task (%d)\n", status);
#else
	dev = dev;
#endif

	return status;
}

static void *task_cdb_exec(void *arg)
{
	sata_dev_t *dev = (sata_dev_t *)arg;
	int        status;

	dev = dev;

	for (;;) {
		int        cdb_idx = INV_CDB_IDX;
		sata_cdb_t *cdb    = NULL;

		status = fifo_pop(&cdb_fifo, &cdb_idx);
		if (!status) {
			assert(cdb_idx >= 0 && cdb_idx < NUM_CDBS);
			cdb = &cdb_pool->cdbs[cdb_idx];
		} else {
#if defined(__SINGLE_THREADED)
			break;
#else
			continue;
#endif
		}

		status = build_cmd(cdb);
		if (status) {
			/* problem building command */
			if (cdb->callback)
				cdb->callback(cdb, status);
			else {
				assert(0);
			}
		}

		start_cmd(cdb);
	}

	return NULL;
}

static int task_cdb_exec_init(sata_dev_t *dev)
{
	int status = 0;

#if !defined(__SINGLE_THREADED)
	status = thr_create(&cet_state.thread, &task_cdb_exec, dev);
	if (status)
		err_printf("couldn't create cdb exec task (%d)\n", status);
#else
	dev = dev;
#endif

	return status;
}

static int sata_reset_phase1(ahci_port_regs_t *pr)
{
	int                   status;
	pxssts_scr0_sstatus_t sstatus;
	pxcmd_cmd_n_stat_t    cmd_and_stat;

	/* Make sure the PxCMD.ST is cleared before doing anything */
	cmd_and_stat.all = __r32(&pr->cmd_and_stat);
	cmd_and_stat.bits.start = 0;
	__w32(&pr->cmd_and_stat, cmd_and_stat.all);

	/* Don't bother doing anything if no drive is attached */
	status = wait_bits_timeout(&pr->sstatus.all,
				   0xF,
				   0,
				   SECS_TO_TICKS(1),
				   0);
	if (status)
		return ERR_NO_DRIVE;

	sstatus.all = __r32(&pr->sstatus);
	if (sstatus.bits.det == SCR2_DET_PERFORM_COMINIT) {
		/* A device is present but requires a COMINIT sequence */
		status = wait_bits_timeout(&pr->sstatus.all,
					   SCR2_DET_DEVICE_AND_COMM_OK,
					   0,
					   TIMEOUT_TICKS_5S * 10,
					   1);
	} else if (sstatus.bits.det != SCR2_DET_DEVICE_AND_COMM_OK) {
		/* Unhandled */
		return ERR_NO_DRIVE;
	}

	return status;
}

/*  *********************************************************************
    *  Global Functions
    ********************************************************************* */

/**
 * sata_reset - Asserts reset on port
 *
 * @dev        - Pointer to sata_dev_t instance
 * @port       - Port
 * @reset_type - Type of reset to perform
 */
int sata_reset(sata_dev_t *dev, int port, int reset_type)
{
	int              status  = 0;
	ahci_port_regs_t *pr     = NULL;

	if (!dev || !dev->hba_regs || !dev->port_regs[port]) {
		status = ERR_INV_PTR;
		goto done;
	}

	if (!dev->status.is_ready) {
		status = ERR_DEV_NOT_INIT;
		goto done;
	}

	pr = dev->port_regs[port];

	status = sata_reset_phase1(pr);
	if (status)
		goto done;

	if (reset_type == RESET_SW) {
		int                i;
		pxcmd_cmd_n_stat_t cmd_and_stat;

		/*
		 * Mask off interrupts for this port since this code needs full
		 * control of the state machine.
		 */
		__w32(&pr->int_enable, 0);

		/* Force clear BSY and DRQ using CLO before enabling */
		cmd_and_stat.all = __r32(&pr->cmd_and_stat);
		cmd_and_stat.bits.start = 0;
		cmd_and_stat.bits.cmd_list_override = 1;
		__w32(&pr->cmd_and_stat, cmd_and_stat.all);

		status = wait_bits_timeout(&pr->cmd_and_stat,
					   BIT(PXCMD_CLO),
					   1,
					   TIMEOUT_TICKS_5S,
					   0);
		if (status)
			goto done;

		cmd_and_stat.all = __r32(&pr->cmd_and_stat);
		cmd_and_stat.bits.start = 1;
		__w32(&pr->cmd_and_stat, cmd_and_stat.all);

		usleep(SECS_TO_TICKS(1));

		/* Blindly clear all errors */
		__rwb32(&pr->int_status);
		__rwb32(&pr->serror);

		/* Toggle SRST */
		for (i = 0; i < 2 && !status; i++) {
			h2d_fis_t *cfis =
			    &cmd_tbls[port][DEF_NON_Q_TAG].cfis;

			dbg_printf("srst cfis: %p\n", (uint32_t *)cfis);

			usleep(MS_TO_TICKS(1));

			/* Build a SRST H2D FIS */
			memset(cfis, 0, sizeof(h2d_fis_t));
			init_h2d_fis_t((h2d_fis_t *)cfis, 0);
			cfis->fields.control.bits.srst = i ? 0 : 1;

			init_cmd_list_hdr(&cmd_list_hdrs[port][DEF_NON_Q_TAG],
					  0,
					  0,
					  0,
					  !i,
					  i,
					  0);

			cache_flush(&cmd_tbls[port][DEF_NON_Q_TAG],
				    sizeof(ahci_cmd_tbl_t));
			cache_flush(&cmd_list_hdrs[port][DEF_NON_Q_TAG],
				    sizeof(ahci_cmd_list_hdr_t));

			/* Wait for tag clear, then fire */
			status = wait_bits_timeout(&pr->ci,
						   BIT(DEF_NON_Q_TAG),
						   1,
						   TIMEOUT_TICKS_5S,
						   0);
			if (status)
				break;

			__w32(&pr->ci, BIT(DEF_NON_Q_TAG));

			status = wait_sata_reset(pr);
			if (status)
				break;
		}

		if (!status)
			status = wait_bits_timeout((uint32_t *)&pr->tf_data,
						   BIT(PXTFD_TF_STS_BSY),
						   1,
						   TIMEOUT_TICKS_5S,
						   0);

		/* Enable all port interrupts */
		__w32(&pr->int_enable, 0xFFFFFFFF);
	}

done:
	return status;
}

/**
 * sata_scan - Scans for SATA devices on each port
 *
 * @dev        - Pointer to sata_dev_t instance
 * @devs_found - (out) A bitwise mask of ports that have devices attached
 *
 */
int sata_scan(sata_dev_t *dev, uint32_t ports_to_scan, uint32_t *devs_found)
{
	int status = 0;
	int port;

	if (!dev) {
		status = ERR_INV_PTR;
		goto done;
	}

	if (!dev->status.is_ready) {
		status = ERR_DEV_NOT_INIT;
		goto done;
	}

	if (devs_found)
		*devs_found = 0;

	dev->irq_en(1);

	for_each_port(port) {
		if (!(ports_to_scan & BIT(port)))
			continue;

		dbg_printf("Scanning port %d\n", port);

		status = port_phy_init(dev, port, 1);
		if (status) {
			err_printf("PHY init failed (%d)\n", status);
			status = 0;
			continue;
		}

		status = sata_reset(dev, port, RESET_SW);
		if (!status) {
			dbg_printf("Drive found on port %d\n", port);
			if (devs_found)
				*devs_found |= BIT(port);
		} else {
			err_printf("Reset failed (%d)\n", status);
		}
	}

done:
	return status;
}

/**
 * um_ahci_core_init - Initialize the user-mode AHCI driver context
 *
 * @cfg - Pointer to configuration block
 *
 */
int um_ahci_core_init(const um_ahci_cfg_t *cfg)
{
	int           status   = 0;
	int           port;
	bfr_desc_t    bfr_desc;
	bfr_desc_t    mem_pool_info;
	sata_dev_t    *dev     = cfg->dev;
	hba_ghc_cap_t ghc_cap;

	if (!dev) {
		err_printf("null device ptr\n");
		status = ERR_INV_PTR;
		goto done;
	}

	bfr_desc.va = cfg->mem_base_va;
	bfr_desc.pa = cfg->mem_base_pa;
	bfr_desc.sz = cfg->mem_base_sz;

	status = mutex_init(&dev->mtx);
	assert(status == 0);

	dbg_printf("reg_base: %xh\n", cfg->reg_base);
	g_base = cfg->reg_base; /* HACK */

	memset(dev, 0, sizeof(sata_dev_t));
	dev->reg_base = cfg->reg_base;
	dev->hba_regs = (void *)(cfg->reg_base + SATA_AHCI_GHC_HBA_CAP);

	fifo_init(&cdb_fifo);

	sata_test_setup(dev);

	/* Reset the entire AHCI controller */
	hba_reset(dev);

	/* The ports_implemented field is offset-by-1 */
	if ((dev->hba_regs->cap.bits.num_of_ports + 1) != MAX_SATA_PHY_PORTS) {
		err_msg("Port mismatch: %u != %u",
			dev->hba_regs->cap.bits.num_of_ports + 1,
			MAX_SATA_PHY_PORTS);
		status = ERR_PORT_MISMATCH;
		goto done;
	}

	/* Point controller to freshly alloc'ed memory */
	for_each_port(port) {
		ahci_port_regs_t *pr = NULL;
		pxcmd_cmd_n_stat_t cmd_and_stat;

		/* Prep device memory */
		dev_mem_setup(port, &bfr_desc);

		dev->port_regs[port] =
		    (((void *)dev->hba_regs)
		     + sizeof(ahci_hba_ghc_regs_t)
		     + (port * sizeof(ahci_port_regs_t)));
		pr = dev->port_regs[port];

		port_reset(dev, port);

		/*
		 * At this point, the controller should be ready to accept
		 * the device memory. (Use physical addresses!!)
		 */
		__w32(&pr->cmd_list_base, (uint32_t)cmd_list_hdrs_pa[port][0]);
		__w32(&pr->fis_base, (uint32_t)rx_fises_pa[port]);
		cmd_and_stat.all = __r32(&pr->cmd_and_stat);
		cmd_and_stat.bits.fis_rx_en = 1;
		__w32(&pr->cmd_and_stat, cmd_and_stat.all);
	}

	/* setup irq handling task */
	dev->irq_wait = cfg->irq_wait;
	dev->irq_en = cfg->irq_en;
	task_irqpoll_init(dev);

	/* setup cdb executor task */
	task_cdb_exec_init(dev);

	/* Enable controller interrupts */
	dbg_printf("Enabling HBA interrupts\n");
	ghc_cap.all = __r32(&dev->hba_regs->ghc);
	ghc_cap.bits.int_en = 1;
	__w32(&dev->hba_regs->ghc, ghc_cap.all);

	/*
	 * Create cluster memory (heap) for read/write data
	 */
	status = mem_pool_init(&bfr_desc);
	if (status) {
		err_printf("mem_pool_init failure (%d)\n", status);
		goto done;
	}

	/*
	 * Initialize the CDB free pool
	 */
	memset((void *)cdb_pool, 0, sizeof(cdb_pool_t));
	assert(mutex_init(&cdb_pool->mtx) == 0);

	dev->status.is_ready = 1;

	mem_pool_get_pool_info(&mem_pool_info);

#if (DEBUG)
	printf("+----------------------------------------------+\n");
	printf("| Region             | PA         | VA         |\n");
	printf("+----------------------------------------------+\n");
	printf("| Register Base      | %#08x | %#08x |\n",
			(unsigned int)VIRT_TO_PHYS(dev->reg_base), (unsigned int)dev->reg_base);
	printf("| HBA Global         | %#08x | %#08x |\n",
			(unsigned int)VIRT_TO_PHYS(dev->hba_regs), (unsigned int)dev->hba_regs);
	printf("| Port 0             | %#08x | %#08x |\n",
			(unsigned int)VIRT_TO_PHYS(dev->port_regs[0]), (unsigned int)dev->port_regs[0]);
#if MAX_SATA_PHY_PORTS > 1
	printf("| Port 1             | %#08x | %#08x |\n",
			(unsigned int)VIRT_TO_PHYS(dev->port_regs[1]), (unsigned int)dev->port_regs[1]);
#endif
	printf("| Pool               | %#08llx | %#08x |\n",
			(unsigned long long)mem_pool_info.pa, (unsigned int)mem_pool_info.va);
	printf("+----------------------------------------------+\n");
#endif

	goto done;

done:
	return status;
}

/**
 * sata_cdb_pop - Get a sata_cdb_t instance from the free pool
 *
 * @dev  - A SATA device struct
 * @port - Port number
 * @cdb  - (out) A pointer-pointer to the sata_cdb_t if alloc is successful
 */
int sata_cdb_pop(sata_dev_t *dev, int port, sata_cdb_t **cdb)
{
	int i;
	int status = 0;

	assert(cdb);
	assert(dev);

	mutex_lock(&cdb_pool->mtx);

	for (i = 0; i < NUM_CDBS; i++) {
		if (!(cdb_pool->bitmap & (1UL << i))) {
			cdb_pool->bitmap |= 1UL << i;
			break;
		}
	}

	mutex_unlock(&cdb_pool->mtx);

	if (i == NUM_CDBS) {
		status = ERR_NOMEM;
		goto done;
	}

	*cdb = &cdb_pool->cdbs[i];

	(*cdb)->cdb_idx = i;
	(*cdb)->dev = dev;
	(*cdb)->port = port;
	(*cdb)->tag = INV_TAG;

done:
	return status;
}

/**
 * sata_cdb_push - Returns a sata_cdb_t instance to the free pool
 *
 * @cdb - The sata_cdb_t to return
 */
int sata_cdb_push(sata_cdb_t *cdb)
{
	int status = 0;

	if (!cdb)
		return ERR_INV_PTR;

	assert(cdb->tag == INV_TAG);

	mutex_lock(&cdb_pool->mtx);
	cdb_pool->bitmap &= ~(1UL << cdb->cdb_idx);
	mutex_unlock(&cdb_pool->mtx);

	return status;
}

/**
 * sata_cmd_issue - Submit a CDB to the execution queue
 *
 * This routine will prepare and submit a CDB for execution by the executor
 * thread. A SATA tag will be allocated and attached to the CDB prior to
 * submission to the queue.
 *
 * @cdb - A pointer to a sata_cdb_t instance
 */
int sata_cmd_issue(sata_cdb_t *cdb)
{
	int status = 0;

	if (!cdb || !cdb->dev)
		return ERR_INV_PTR;

	cdb->start = _getticks();

	mutex_lock(&cdb->dev->mtx);
	status = tag_pop(cdb->dev, cdb->port, cdb);
	mutex_unlock(&cdb->dev->mtx);

	if (status)
		status = ERR_BUSY_NO_TAGS;
	else {
		fifo_push(&cdb_fifo, cdb->cdb_idx);
#if defined(__SINGLE_THREADED)
		task_cdb_exec(cdb->dev);
		status = task_irqpoll(cdb->dev);
#endif
	}

	return status;
}

/**
 * sata_cmd_cleanup - Clean up a CDB after execution.
 *
 * Relinquishes the SATA tag that was allocated by the issue() function.
 *
 * @cdb - A pointer to a sata_cdb_t instance
 */
int sata_cmd_cleanup(sata_cdb_t *cdb)
{
	mutex_lock(&cdb->dev->mtx);
	tag_push(cdb->dev, cdb->port, cdb, 1);
	mutex_unlock(&cdb->dev->mtx);

	return 0;
}
