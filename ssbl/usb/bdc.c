/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

/* **************************************************
   * BDC Driver for Fastboot
   *  - very basic support (mostly sunny day)
   *  -- operate at high speed only
   *  -- and enough for fastboot
   *  - no TEST_* support
 * *************************************************/

#include "lib_types.h"
#include "lib_malloc.h"
#include "lib_string.h"
#include "lib_printf.h"
#include "lib_physio.h"

#include "iocb.h"
#include "ioctl.h"
#include "device.h"
#include "devfuncs.h"
#include "error.h"

#include "bsp_config.h"
#include "usbchap9.h"
#include "bdc.h"
#include "timer.h"
#include "ui_command.h"
#include "env_subr.h"
#include "board.h"

/*  *********************************************************************
    *  Macros for dealing with hardware
    ********************************************************************* */

#define BDC_VTOP(ptr) ((uint32_t)PHYSADDR((long)(ptr)))

#define BDC_WRITECSR(softc, x, y) \
	DEV_WR((softc)->regs + (x), (y))

#define BDC_READCSR(softc, x) \
	DEV_RD((softc)->regs + (x))

/*  *********************************************************************
    *  Forward declarations
    ********************************************************************* */

static void bdc_probe(bolt_driver_t *drv, unsigned long probe_a,
			      unsigned long probe_b, void *probe_ptr);
static int bdc_open(bolt_devctx_t *ctx);
static int bdc_read(bolt_devctx_t *ctx, iocb_buffer_t *buffer);
static int bdc_inpstat(bolt_devctx_t *ctx, iocb_inpstat_t *inpstat);
static int bdc_write(bolt_devctx_t *ctx, iocb_buffer_t *buffer);
static int bdc_close(bolt_devctx_t *ctx);
static int bdc_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer);

/*  *********************************************************************
    *  Globals
    ********************************************************************* */

static const bolt_devdisp_t bdc_dispatch = {
	bdc_open,
	bdc_read,
	bdc_inpstat,
	bdc_write,
	bdc_ioctl,
	bdc_close,
	NULL,
	NULL
};

const bolt_driver_t bdc_drv = {
	"USB BDC",
	"usbdev",
	BOLT_DEV_NETWORK,
	&bdc_dispatch,
	bdc_probe
};

/* *********************************
 * Utility Functions
 * ********************************* */

static void dump_data(void *buf, int len)
{
	int i;
	uint8_t *p = (uint8_t *) buf;

	for (i = 0; i < len; ++i) {
		if (i % 16 == 0)
			xprintf("%s    ", (i ? "\n" : ""));
		xprintf(" %02x", *p++);
	}
	xprintf("\n");
}

static void dump_dw4(char *st, void *buf)
{
	uint32_t *p = (uint32_t *) buf;

	xprintf("%s %08x %08x %08x %08x\n", st, p[0], p[1], p[2], p[3]);
}


static void bdc_check_debug(bdc_softc *softc)
{
	softc->debug = env_getval("BDC_DBG");
	if (softc->debug < 0)
		softc->debug = 0;
	else
		printf("BDC: debug=0x%08x\n", softc->debug);
}
static void bdc_clear_bulk_state(bdc_softc *softc)
{
	softc->bulkout_rdy = 0;
	softc->bulkout_len = 0;
	softc->bulkout_queued = 0;
	softc->bulkin_done = 0;
}

static void bdc_change_vendev(bdc_softc *softc, uint32_t vendev)
{
	uint8_t *p1 = &softc->desc->dev.idVendorLow;
	uint8_t *p2 = (uint8_t *) &vendev;

	p1[0] = p2[2];
	p1[1] = p2[3];
	p1[2] = p2[0];
	p1[3] = p2[1];
}

static int bdc_mfnum(bdc_softc *softc)
{
	uint32_t val;

	val = BDC_READCSR(softc, R_BDC_MFNUM);
	if (val & M_MFNUM_VLD)
		val = G_MFNUM_MFC(val);
	else
		val = 0xffff;

	return val;
}

/* *********************************
 * Init/Cleanup Functions
 * ********************************* */

static int bdc_start(bdc_softc *softc, physaddr_t base)
{
	uint32_t reg, temp;
	unsigned int srr_buf_size;

	softc->regs = base;
	if (softc->debug & DBG_INIT1)
		printf("BDC: base: %08x\n", base);

	/* set up scratch pad */
	reg = BDC_READCSR(softc, R_BDC_BDCCFG0);
	temp = G_CFG0_SPB(reg);
	if (temp) {
		temp = 1 << (temp + 5);
		softc->scrpad = KUMALLOC(temp, DMA_BUF_ALIGN);
		if (softc->scrpad == NULL) {
			xprintf("BDC: Failed to allocate Scratchpad buffer\n");
			return 0;
		}
		memset(softc->scrpad, 0, temp);
		BDC_WRITECSR(softc, R_BDC_SPBBAL, BDC_VTOP(softc->scrpad));
		BDC_WRITECSR(softc, R_BDC_SPBBAH, 0);
		if (softc->debug & DBG_INIT1)
			printf("BDC: sp: %d bytes\n", temp);
	}

	/* set up SRR0 : #entries == 2 ** (SRR0_SIZE + 1) */
	softc->srr0_size = 1 << (SRR0_SIZE+1);
	srr_buf_size = softc->srr0_size * sizeof(bdc_sr_t);
	softc->srr0 = KUMALLOC(srr_buf_size, SRR0_ALIGN);
	if (softc->srr0 == NULL) {
		xprintf("BDC: Failed to allocate SRR buffer\n");
		return 0;
	}
	memset(softc->srr0, 0, srr_buf_size);
	softc->srr0_dq = 0;
	if (softc->debug & DBG_INIT1) {
		xprintf("BDC: srr0 addr=%p, #entries=%d, size=%d\n",
			softc->srr0, softc->srr0_size, srr_buf_size);
	}
	temp = BDC_VTOP(softc->srr0) + M_SRRBAL0_SWS +
		V_SRRBAL0_SIZ(SRR0_SIZE);
	BDC_WRITECSR(softc, R_BDC_SRRBAL0, temp);
	BDC_WRITECSR(softc, R_BDC_SRRBAH0, 0);
	BDC_WRITECSR(softc, R_BDC_SRRINT0, M_SRRINT_IE);	/* dqp=0 */
	BDC_WRITECSR(softc, R_BDC_INTCLS0, V_INTCLS_ICI(1));

	/* Enable global interrupt & get controller running */
	temp = V_BDCSC_COP(SC_RUN) + M_BDCSC_COS + M_BDCSC_SRMMCW +
		M_BDCSC_GIE;
	BDC_WRITECSR(softc, R_BDC_BDCSC, temp);
	bolt_msleep(10);

	/* check BDC state */
	reg = BDC_READCSR(softc, R_BDC_BDCSC);
	if (G_BDCSC_STS(reg) != NORMAL) {
		xprintf("BDC: Controller did not enter normal state (%08x)\n",
			 reg);
		return 0;
	}
	softc->dev_state = DEFAULT;

	return 1;
}

static void bdc_invalidate_next_enq(bdc_bd_t *next_enq)
{
	int i;
	uint32_t *enq = (uint32_t *) next_enq;

	for (i = 0; i < 4; ++i)
		enq[i] = 0xffffffff;
}

static void bdc_update_ep_enq(bdc_softc *softc, bdc_bd_t **ring,
				bdc_bd_t **enq, int num_bd)
{
	bdc_bd_t *bd;

	*enq += 1;
	bd = *enq;
	if (!(bd->ctl & M_BD_SBF) && (G_BD_TYP(bd->ctl) == CBD))
		*enq = *ring;
	bdc_invalidate_next_enq(*enq);

	if (softc->debug & DBG_BD_RING) {
		xprintf("BDC: update ring=%p, enq=%p (%d)\n",
				*ring, *enq, (*enq-*ring));
	}
}

static void bdc_reset_epring(bdc_softc *softc, bdc_bd_t *ring,
							bdc_bd_t **penq)
{
	*penq = ring;
	bdc_invalidate_next_enq(*penq);
}

static int bdc_alloc_epring(bdc_softc *softc, bdc_bd_t **pring,
				bdc_bd_t **penq, int num_bd)
{
	int size;
	bdc_bd_t *ring;

	size = num_bd * BD_SIZE;
	if (softc->debug & DBG_INIT1)
		xprintf("BDC: Ring alloc size %d", size);

	ring = (bdc_bd_t *) KUMALLOC(size, DMA_BUF_ALIGN);
	if (ring == NULL) {
		xprintf("...FAILED to allocate ring buffer\n");
		return 0;
	}
	memset(ring, 0, size);
	*pring = ring;
	bdc_reset_epring(softc, ring, penq);

	if (softc->debug & DBG_INIT1)
		xprintf(" ring %p end %p\n", ring, &ring[num_bd]);
	/* set up chain BD at end of ring */
	--num_bd;
	ring[num_bd].dpl = BDC_VTOP(ring);
	ring[num_bd].dph = 0;
	ring[num_bd].srrlen = 0;
	ring[num_bd].ctl = V_BD_TYP(CBD);
	if (softc->debug & DBG_INIT1)
		dump_dw4("BDC: CBD ", &ring[num_bd]);

	return 1;
}

static int bdc_init_buffers(bdc_softc *softc)
{
	int res;

	res = bdc_alloc_epring(softc, &softc->ep0_ring,
				&softc->ep0_enq, EP0_NUM_BD);
	if (!res)
		return 0;
	res = bdc_alloc_epring(softc, &softc->bulkin_ring,
				&softc->bulkin_enq, BULKIN_NUM_BD);
	if (!res)
		return 0;
	res = bdc_alloc_epring(softc, &softc->bulkout_ring,
				&softc->bulkout_enq, BULKOUT_NUM_BD);
	if (!res)
		return 0;

	softc->bulkout_buf =
		(uint8_t *) KMALLOC(MAX_BULKOUT_LEN, DMA_BUF_ALIGN);
	if (softc->bulkout_buf == NULL) {
		xprintf("...FAILED to allocate receive buffer\n");
		return 0;
	}
	memset(softc->bulkout_buf, 0, MAX_BULKOUT_LEN);

	softc->desc =
		(usb_descriptors_t *) KMALLOC(sizeof(usb_descriptors_t), 4);
	if (softc->desc == NULL) {
		xprintf("...FAILED to allocate USB descriptor buffer\n");
		return 0;
	}
	memcpy(softc->desc, &bdc_descriptors, sizeof(usb_descriptors_t));

	return res;
}

static void bdc_free_buffers(bdc_softc *softc)
{
	if (softc->scrpad)
		KUFREE(softc->scrpad);
	softc->scrpad = 0;
	if (softc->srr0)
		KUFREE(softc->srr0);
	softc->srr0 = 0;
	if (softc->ep0_ring)
		KUFREE(softc->ep0_ring);
	softc->ep0_ring = 0;
	if (softc->bulkin_ring)
		KUFREE(softc->bulkin_ring);
	softc->bulkin_ring = 0;
	if (softc->bulkout_ring)
		KUFREE(softc->bulkout_ring);
	softc->bulkout_ring = 0;
	if (softc->bulkout_buf)
		KFREE(softc->bulkout_buf);
	softc->bulkout_buf = 0;
	if (softc->desc)
		KFREE(softc->desc);
	softc->desc = 0;
}

/* *********************************
 * BDC commands
 * ********************************* */

static int bdc_cmd_status(bdc_softc *softc, int to)
{
	uint32_t val = 0;

	to *= 10;	/* mS -> 100uS units */
	do {
		val = G_CMDSC_CST(BDC_READCSR(softc, R_BDC_CMDSC));
		if (val != BUSY) {
			if (val != SUCC) {
				xprintf("BDC: Command error (status=%d)\n",
					val);
				val = 0;
			} else if (softc->debug & DBG_CMD) {
				xprintf("BDC: Command successful [%04x]\n",
					bdc_mfnum(softc));
			}
			return val;
		}
		bolt_usleep(100);
	} while (to--);
	xprintf("BDC: Command timeout (status=%d)\n", val);

	return 0;
}

static int bdc_submit_cmd(bdc_softc *softc, uint32_t par0, uint32_t par1,
				uint32_t par2, uint32_t sc)
{
	BDC_WRITECSR(softc, R_BDC_CMDPAR0, par0);
	BDC_WRITECSR(softc, R_BDC_CMDPAR1, par1);
	BDC_WRITECSR(softc, R_BDC_CMDPAR2, par2);
	sc += V_CMDSC_CID(softc->cid) + M_CMDSC_CWS;
	BDC_WRITECSR(softc, R_BDC_CMDSC, sc);
	++softc->cid;

	if (softc->debug & DBG_CMD) {
		xprintf("BDC: CMD = %08x %08x %08x %08x [%04x]\n",
		par0, par1, par2, sc, bdc_mfnum(softc));
	}

	return bdc_cmd_status(softc, BDC_CMD_TIMEOUT);
}

static int bdc_ep_op(bdc_softc *softc, int epn, int op, int misc)
{
	uint32_t sc;

	if (softc->debug & DBG_INIT2)
		xprintf("BDC: EPO epn=%d op=%d misc=%08x\n", epn, op, misc);

	sc = V_CMDSC_SUB(op) + V_CMDSC_CMD(EPO) + V_CMDSC_EPN(epn) + misc;

	return bdc_submit_cmd(softc, 0, 0, 0, sc);
}

static int bdc_config_ep(bdc_softc *softc, int typ, int epn, void *ring,
				bdc_bd_t **penq, int action, int mps)
{
	uint32_t par2, sc;

	if (action == ACF)
		bdc_reset_epring(softc, ring, penq);

	if (softc->debug & DBG_INIT2) {
		xprintf("BDC: Config EP %d typ=%d action=%d",
			epn, typ, action);
		if (action == ACF)
			xprintf(" mps=%d ring=%p enq=%p\n", mps, ring, *penq);
		else
			xprintf("\n");
	}

	par2 = V_CMDPAR2_TYP(typ) + V_CMDPAR2_MPS(mps);
	sc = V_CMDSC_SUB(action) + V_CMDSC_CMD(EPC) + V_CMDSC_EPN(epn);

	return bdc_submit_cmd(softc, BDC_VTOP(ring), 0, par2, sc);
}

static int bdc_set_addr(bdc_softc *softc, uint32_t addr)
{
	uint32_t sc;

	if (softc->debug & DBG_INIT2)
		xprintf("BDC: Set up device address %02x\n", addr);
	sc = V_CMDSC_SUB(ADR) + V_CMDSC_CMD(DVC);

	return bdc_submit_cmd(softc, 0, 0, addr, sc);
}

static void bdc_ring_db(bdc_softc *softc, int db)
{
	uint32_t val;

	/* Make sure DMA information is flushed to memory */
	dmb();
	val = V_XSFNTF_EPN(db);
	BDC_WRITECSR(softc, R_BDC_XSFNTF, val);
	if (softc->debug & DBG_DB) {
		xprintf("BDC: Rang Doorbell for EP %d [%04x]\n",
		val, bdc_mfnum(softc));
	}
}

static int bdc_clear_bulk_stall(bdc_softc *softc, int epa)
{
	int epn;

	epn = (epa == bdc_descriptors.bulkin_ep.bEndpointAddress) ?
		BULKIN_EP : BULKOUT_EP;
	bdc_ep_op(softc, epn, STL, 0);
	bdc_ep_op(softc, epn, RST, M_CMDSC_SNR);
	bdc_ring_db(softc, epn);

	return 0;
}


/* *********************************
 * BDC Bulk transactions
 *  - TODO
 *  -- multi-BDs
 *  -- ZLP (?)
 * ********************************* */

static int bdc_bd_tfs(int len)
{
	int tfs;

	tfs = (len / BULK_MPS) + ((len % BULK_MPS) != 0);

	return tfs;
}

static void bdc_bulkin_data(bdc_softc *softc, uint8_t *buf, int len)
{
	bdc_bd_t *bd = softc->bulkin_enq;

	if (softc->debug & DBG_BULK_IN) {
		xprintf("BDC: Queue BULK IN buffer (len=%d, enq=%p)\n",
				len, softc->bulkin_enq);
	}

	/* Flush the cache */
	CACHE_FLUSH_RANGE(buf, len);

	bd->dpl = BDC_VTOP(buf);
	bd->dph = 0;
	bd->srrlen = M_BD_LTF + len;
	bd->ctl = M_BD_IOC + M_BD_ISP + M_BD_EOT + M_BD_SOT +
			  V_BD_TFS(bdc_bd_tfs(len)) + V_BD_TYP(XBD);
	if (softc->debug & DBG_BD_QUEUE)
		dump_dw4("BDC: BULK IN BD ", bd);
	bdc_update_ep_enq(softc, &softc->bulkin_ring, &
					softc->bulkin_enq, BULKIN_NUM_BD);
	bdc_ring_db(softc, BULKIN_EP);
}

static void bdc_bulkout_data(bdc_softc *softc, uint8_t *buf, int len)
{
	bdc_bd_t *bd = softc->bulkout_enq;

	if (softc->debug & DBG_BULK_OUT) {
		xprintf("BDC: Queue BULK OUT buffer (len=%d, enq=%p)\n",
				len, softc->bulkout_enq);
	}

	softc->bulkout_len = len;	/* queued length */

	/* Invalidate the cache */
	CACHE_INVAL_RANGE(buf, len);

	bd->dpl = BDC_VTOP(buf);
	bd->dph = 0;
	bd->srrlen = M_BD_LTF + len;
	bd->ctl = M_BD_IOC + M_BD_ISP + M_BD_EOT + M_BD_SOT +
		  V_BD_TFS(bdc_bd_tfs(len)) + V_BD_TYP(XBD);
	if (softc->debug & DBG_BD_QUEUE)
		dump_dw4("BDC: BULK OUT BD ", bd);
	bdc_update_ep_enq(softc, &softc->bulkout_ring,
			  &softc->bulkout_enq, BULKOUT_NUM_BD);
	bdc_ring_db(softc, BULKOUT_EP);
	softc->bulkout_queued = 1;
}

static void bdc_setup_lpbk_pkt(bdc_softc *softc, int bulkin)
{
	static int patt;

	if (bulkin && !softc->lpbk.len) {  /* nothing rx, so just bulk in */
		memset(softc->lpbk.buf, patt++, 127);
		softc->lpbk.len = 127;
	}
	if (bulkin) {
		bdc_bulkin_data(softc, softc->lpbk.buf, softc->lpbk.len);
		softc->lpbk.len = 0;
	} else {
		bdc_bulkout_data(softc, softc->lpbk.buf, MAX_LPBK_LEN);
	}
}

static void bdc_complete_bulkin(bdc_softc *softc, bdc_sr_t *xr)
{
	if (softc->lpbk.on)
		bdc_setup_lpbk_pkt(softc, 0);
	softc->bulkin_done = 1;
}

static void bdc_complete_bulkout(bdc_softc *softc, bdc_sr_t *xr)
{
	softc->bulkout_len -= G_SR_LEN(xr->len);	/* actual rx length */
	if (softc->debug & DBG_BULK_OUT)
		xprintf("BDC: Actual BULK OUT len=%d\n", softc->bulkout_len);

	if (softc->lpbk.on) {
		softc->lpbk.len = softc->bulkout_len;
		bdc_setup_lpbk_pkt(softc, 1);
	}
	softc->bulkout_rdy = 1;		/* buffer received */
	softc->bulkout_queued = 0;
}

static int bdc_bulkin_done(bdc_softc *softc)
{
	int res;

	res = softc->bulkin_done;
	softc->bulkin_done = 0;

	return res;
}

static int bdc_bulkout_queued(bdc_softc *softc)
{
	return softc->bulkout_queued;
}

static int bdc_bulkout_done(bdc_softc *softc)
{
	int res;

	res = softc->bulkout_rdy;
	softc->bulkout_rdy = 0;

	return res;
}


/* *********************************
 * BDC EP0 transactions
 *  - TODO (as needed)
 *  - other needed Ch9 commands
 *  -- multi-BDs
 *  -- ZLP
 * ********************************* */

static void bdc_send_ctrl_in_data(bdc_softc *softc, uint8_t *buf, int len)
{
	bdc_bd_t *bd = (bdc_bd_t *) softc->ep0_enq;

	if (softc->debug & DBG_CTL_IN) {
		xprintf("BDC: Control IN Data Stage (len=%d, enq=%p)\n",
				len, softc->ep0_enq);
	}

	/* Flush the cache */
	CACHE_FLUSH_RANGE(buf, len);

	bd->dpl = BDC_VTOP(buf);
	bd->dph = 0;
	bd->srrlen = M_BD_LTF + len;
	bd->ctl = M_BD_IOC + M_BD_ISP + M_BD_EOT + M_BD_SOT +
			  M_BD_DIRIN + V_BD_TFS(1) + V_BD_TYP(DBD);
	if (softc->debug & DBG_BD_QUEUE)
		dump_dw4("BDC: Control IN BD ", bd);
	if (softc->debug & DBG_CTL_DATA)
		dump_data(buf, len);

	bdc_update_ep_enq(softc, &softc->ep0_ring,
			  &softc->ep0_enq, EP0_NUM_BD);
	bdc_ring_db(softc, EP0);
}

static void bdc_ctrl_status(bdc_softc *softc, int in)
{
	bdc_bd_t *bd = (bdc_bd_t *) softc->ep0_enq;

	if (softc->debug & DBG_CTL_STATUS) {
		xprintf("BDC: Control Status %s (enq=%p)\n",
				(in ? "IN" : "OUT"), softc->ep0_enq);
	}

	in = in ? M_BD_DIRIN : 0;
	bd->dpl = 0;
	bd->dph = 0;
	bd->srrlen = M_BD_LTF;
	bd->ctl = M_BD_IOC + M_BD_EOT + M_BD_SOT +
			  in + V_BD_TFS(0) + V_BD_TYP(SBD);
	if (softc->debug & DBG_BD_QUEUE)
		dump_dw4("BDC: Control STATUS BD ", bd);

	bdc_update_ep_enq(softc, &softc->ep0_ring,
			  &softc->ep0_enq, EP0_NUM_BD);
	bdc_ring_db(softc, EP0);
}

static void bdc_ctrl_out(bdc_softc *softc)
{
	switch (softc->setup.bRequest) {
	case(USB_REQUEST_SET_ADDRESS):
		softc->dev_state = ADDRESS;
		bdc_set_addr(softc, softc->setup.wValue);
		break;
	case(USB_REQUEST_SET_CONFIGURATION):
		softc->config_val = softc->setup.wValue;
		softc->dev_state = CONFIG;
		break;
	case(USB_REQUEST_CLEAR_FEATURE):
		if ((softc->setup.bmRequestType & 0x1f) ==
				USBREQ_REC_ENDPOINT) {
			bdc_clear_bulk_stall(softc, softc->setup.wIndex);
		}
		break;
	default:
		break;
	}
}

static void bdc_handle_setup(bdc_softc *softc, bdc_sr_t *xr)
{
	if (softc->ep0_state != WAIT_FOR_SETUP) {
		xprintf("BDC: SETUP overrun!\n");
		xprintf("BDC: EP0 state = %s, Dev state = %s\n",
			EP0_STATE[softc->ep0_state],
			DEV_STATE[softc->dev_state]);
		/* some cleanup to do...??? */
	}

	/* Parse the (little endian) setup packet */
	softc->setup.bmRequestType = xr->bph & 0xff;
	softc->setup.bRequest = (xr->bph >> 8) & 0xff;
	softc->setup.wValue = (xr->bph >> 16) & 0xffff;
	softc->setup.wIndex = xr->bpl & 0xffff;
	softc->setup.wLength = (xr->bpl >> 16)  & 0xffff;

	if (softc->debug & DBG_CTL_SETUP) {
		xprintf("BDC: SETUP: %02x %02x %04x %04x %04x\n",
				softc->setup.bmRequestType,
				softc->setup.bRequest, softc->setup.wValue,
				softc->setup.wIndex, softc->setup.wLength
			);
	}
	/* minimal SETUP handling for now */
	if (softc->setup.bmRequestType & USBREQ_DIR_IN) {
		softc->ep0_state = WAIT_FOR_DATA_IN;
	} else {	/* CTL-OUT */
		bdc_ctrl_out(softc);
		softc->ep0_state = WAIT_FOR_STATUS_IN;
	}
	if (softc->debug & DBG_CTL_STATE) {
		xprintf("BDC: EP0 state = %s, Dev state = %s\n",
			EP0_STATE[softc->ep0_state],
			DEV_STATE[softc->dev_state]);
	}
}

static int bdc_get_string_desc(bdc_softc *softc, uint8_t *buf)
{
	uint8_t len = 0;
	char *st = "";
	int i, j, id;

	buf[1] = USB_STRING_DESCRIPTOR_TYPE;
	id = softc->setup.wValue & 0xff;
	switch (id) {
	case(0):
		len = 4;
		buf[2] = LSB(DEVICE_STRING_LANGUAGE_ID);
		buf[3] = MSB(DEVICE_STRING_LANGUAGE_ID);
		break;
	case(DEVICE_STRING_PRODUCT_INDEX):
		st = DEVICE_STRING_PRODUCT;
		break;
	case(DEVICE_STRING_SERIAL_NUMBER_INDEX):
		st = env_getenv("BOARD_SERIAL");
		if (!st)
			st = DEVICE_STRING_SERIAL_NUMBER;
		break;
	case(DEVICE_STRING_CONFIG_INDEX):
		st = DEVICE_STRING_CONFIG;
		break;
	case(DEVICE_STRING_INTERFACE_INDEX):
		st = DEVICE_STRING_INTERFACE;
		break;
	case(DEVICE_STRING_MANUFACTURER_INDEX):
		st = DEVICE_STRING_MANUFACTURER;
		break;
	default:
		xprintf("BDC: Invalid string descriptor request %d\n", id);
		id = 0;
		break;
	}

	if (id) {
		len = 2;
		j = 2;
		for (i = 0; i < (int) strlen(st); ++i) {  /* to unicode */
			buf[j++] = st[i];
			buf[j++] = 0;
			len += 2;
		}
	}
	buf[0] = len;

	return len;
}

static void bdc_ctrl_in_data(bdc_softc *softc)
{
	uint8_t *buf;
	int	data, len = 0;

	switch (softc->setup.bRequest) {
	case(USB_REQUEST_GET_DESCRIPTOR):
		switch (softc->setup.wValue >> 8) {
		default:
		case(USB_DEVICE_DESCRIPTOR_TYPE):
			buf = (uint8_t *) &softc->desc->dev;
			len = sizeof(usb_device_descr_t);
			break;
		case(USB_CONFIGURATION_DESCRIPTOR_TYPE):
			buf = (uint8_t *) &softc->desc->cfg;
			len = TOTAL_CONFIG_LEN;
			break;
		case(USB_STRING_DESCRIPTOR_TYPE):
			buf = softc->str_desc;
			len = bdc_get_string_desc(softc, buf);
			break;
		}
		len = (len < softc->setup.wLength) ? len :
			softc->setup.wLength;
		bdc_send_ctrl_in_data(softc, buf, len);
		break;
	case(USB_REQUEST_GET_CONFIGURATION):
		buf = (uint8_t *) &softc->config_val;
		bdc_send_ctrl_in_data(softc, buf, 1);
		break;
	case(USB_REQUEST_GET_STATUS):
		if ((softc->setup.bmRequestType & 0x1f) == USBREQ_REC_DEVICE)
			data = 1;
		else
			data = 0;
		buf = (uint8_t *) &data;
		bdc_send_ctrl_in_data(softc, buf, 2);
		break;
	case(USB_REQUEST_GET_INTERFACE):
		data = 0;
		buf = (uint8_t *) &data;
		bdc_send_ctrl_in_data(softc, buf, 1);
		break;
	default:
		/* for now, send ZLP for unknown commands */
		buf = (uint8_t *) &len;
		bdc_send_ctrl_in_data(softc, buf, 0);
		break;
	}
}

static void bdc_ctrl_out_data(bdc_softc *softc)
{
	/* handle DATA OUT later */
	softc->ep0_state = WAIT_FOR_DATA_OUT;
}

static void bdc_handle_ctrl_data(bdc_softc *softc)
{
	if (softc->ep0_state == WAIT_FOR_DATA_IN)
		bdc_ctrl_in_data(softc);
	else if (softc->ep0_state == WAIT_FOR_DATA_OUT)
		bdc_ctrl_out_data(softc);
	else
		xprintf("BDC: Bad EP0 data state (%d)!\n", softc->ep0_state);
}

static void bdc_handle_ep0_state_transition(bdc_softc *softc)
{
	if (softc->debug & DBG_CTL_STATE) {
		xprintf("BDC: State[in]  EP0 = %s, Dev = %s\n",
			EP0_STATE[softc->ep0_state],
			DEV_STATE[softc->dev_state]);
		}

	switch (softc->ep0_state) {
	case(WAIT_FOR_DATA_IN):
		softc->ep0_state = WAIT_FOR_STATUS_OUT;
		break;
	case(WAIT_FOR_DATA_OUT):
		softc->ep0_state = WAIT_FOR_STATUS_IN;
		break;
	case(WAIT_FOR_STATUS_IN):
		switch (softc->dev_state) {
		case(ADDRESS):
			softc->dev_state = ADDRESSED;
			softc->ep0_state = WAIT_FOR_SETUP;
			break;
		case(CONFIG):
			softc->dev_state = CONFIGURED;
			softc->ep0_state = WAIT_FOR_SETUP;
			bdc_config_ep(softc, BULK, BULKIN_EP,
				softc->bulkin_ring,
				&softc->bulkin_enq,
				ACF, BULK_MPS);
			bdc_config_ep(softc, BULK, BULKOUT_EP,
				softc->bulkout_ring,
				&softc->bulkout_enq,
				ACF, BULK_MPS);
			if (softc->lpbk.on)
				bdc_setup_lpbk_pkt(softc, 0);
			break;
		case(CONFIGURED):
			softc->ep0_state = WAIT_FOR_SETUP;
			break;
		default:
			break;
		}
		break;
	case(WAIT_FOR_STATUS_OUT):
		softc->ep0_state = WAIT_FOR_SETUP;
		break;
	default:
		break;
	}

	if (softc->debug & DBG_CTL_STATE) {
		xprintf("BDC: State[out] EP0 = %s, Dev = %s\n",
			EP0_STATE[softc->ep0_state],
			DEV_STATE[softc->dev_state]);
		}
}


/* *********************************
 * BDC event response
 *  - TODO
 *  -- ep0 data out
 *  --- get status
 *  --- stalls
 * ********************************* */

static void bdc_transfer_status(bdc_softc *softc, bdc_sr_t *xr,
				int sts, int epn)
{
	switch (sts) {
	case (STS_SUC):
		if (softc->debug & DBG_EVENT3)
			xprintf("BDC: Event Success\n");

		if (softc->ep0_state != WAIT_FOR_SETUP)
			bdc_handle_ep0_state_transition(softc);
		else if (epn == BULKOUT_EP)
			bdc_complete_bulkout(softc, xr);
		else if (epn == BULKIN_EP)
			bdc_complete_bulkin(softc, xr);
		break;

	case (STS_SPR):
		bdc_handle_setup(softc, xr);
		break;

	case (STS_DSS):
		if (epn == EP0)
			bdc_handle_ctrl_data(softc);
		else
			xprintf("BDC: DSS request for unknown EP (%d)\n",
				epn);
		break;

	case (STS_SSS):
		if (softc->ep0_state == WAIT_FOR_STATUS_IN)
			bdc_ctrl_status(softc, 1);
		else if (softc->ep0_state == WAIT_FOR_STATUS_OUT)
			bdc_ctrl_status(softc, 0);
		else
			xprintf("BDC: Bad EP0 status state (%d)!\n",
				softc->ep0_state);
		break;

	case (STS_SHT):
		if (epn == BULKOUT_EP)
			bdc_complete_bulkout(softc, xr);
		else if (epn == BULKIN_EP)
			bdc_complete_bulkin(softc, xr);
		else
			xprintf("BDC: SHT report for unknown EP (%d)\n",
				epn);
		break;

	case (STS_STP):
		break;

	default:
		if (sts < STS_MAX)
			xprintf("BDC: Event status %s\n", STS_ST[sts]);
		else
			xprintf("BDC: Unknown Event status %d\n", sts);
		break;
	}
}

static int bdc_handle_vbc(bdc_softc *softc, uint32_t psc)
{
	int on;

	on = psc & M_USPSC_VBS;
	if (softc->debug & DBG_PSC1)
		xprintf("BDC: VBUS is %s\n", (on ? "ON" : "OFF"));

	if (on) {
		softc->dev_state = ATTACHED;
		softc->ep0_state = WAIT_FOR_SETUP;
	} else {
		/* deconfigure the endpoints and/or reset cmd to BDC */
		if (softc->dev_state >= RESET) {
			if (bdc_ep_op(softc, EP0, STP, 0))
				bdc_config_ep(softc, CTRL, EP0, 0, 0, DCF, 0);
		}
#if 0	/* FIXME: stop ep not working yet */
		if (softc->dev_state == CONFIGURED) {
			if (bdc_ep_op(softc, BULKIN_EP, STP, 0)) {
				bdc_config_ep(softc, BULK, BULKIN_EP,
						0, 0, DCF, 0);
			}
			if (bdc_ep_op(softc, BULKOUT_EP, STP, 0)) {
				bdc_config_ep(softc, BULK, BULKOUT_EP,
				0, 0, DCF, 0);
			}
		}
#endif
		bdc_clear_bulk_state(softc);
		softc->dev_state = DEFAULT;
	}

	return on;
}

static void bdc_psc(bdc_softc *softc)
{
	uint32_t psc;
	int scn = 0;

	psc = BDC_READCSR(softc, R_BDC_USPSC);
	if (softc->debug & DBG_PSC2)
		xprintf("BDC: USPSC %08x\n", psc);
	if (psc & M_USPSC_VBC)
		scn = bdc_handle_vbc(softc, psc);
	/* reset done? */
	if ((psc & (M_USPSC_PRC | M_USPSC_PRS)) == M_USPSC_PRC) {
		softc->dev_speed = G_USPSC_PSP(psc);
		if (softc->debug & DBG_PSC1)
			xprintf("BDC: Port Reset (speed=%s)\n",
				SPEED_ST[softc->dev_speed]);
		if ((softc->dev_speed == PSP_HS) ||
		    (softc->dev_speed == PSP_FS)) {
			bdc_config_ep(softc, CTRL, EP0, softc->ep0_ring,
					  &softc->ep0_enq, ACF, EP0_MPS);
		}
		softc->dev_state = RESET;
	}
	if (psc & M_USPSC_PCE)
		xprintf("BDC: Port Connect FAILED\n");
	if (psc & M_USPSC_CFC)
		xprintf("BDC: Port Config ERROR\n");
	if ((psc & M_USPSC_PCC) && (softc->debug & DBG_PSC1))
		xprintf("BDC: Port Connect change occurred\n");
	if ((psc & M_USPSC_PSC) && (softc->debug & DBG_PSC1))
		xprintf("BDC: Suspend/Resume change occurred\n");
	if (scn)
		psc |= M_USPSC_SCN;
	else
		psc &= M_USPSC_ALLC;
	BDC_WRITECSR(softc, R_BDC_USPSC, psc);
}

static void bdc_handle_events(void *arg)
{
	uint32_t val, ev, epn, sts, mfc;
	bdc_sr_t *evp;
	bdc_softc *softc = (bdc_softc *) arg;

	/* BDC is clock-gated while in host mode during DRD operation.
	   So, access regsiters only when in device mode */
	if (softc->drd_status) {
		val = DEV_RD(softc->drd_status);
		if (G_DRD_STATE(val) != DEV_MODE)
			return;
	}

	val = BDC_READCSR(softc, R_BDC_SRRINT0);
	if (val & M_SRRINT_IP) {
		/* check if something to deq srrint0(eqp != dqp)...
		   index into event buffer */
		evp = &softc->srr0[softc->srr0_dq];
		ev = G_SR_XSF(evp->dw3);
		epn = G_SR_EPN(evp->dw3);
		sts = G_SR_STS(evp->dw3);
		mfc = G_SR_MFC(evp->dw3);
		if (softc->debug & DBG_EVENT2) {
			xprintf("BDC: Event %d srrdq=%d evp=%p "
				"srrint0=%08x mfc=%04x\n",
				ev, softc->srr0_dq, evp, val, mfc);
			if (softc->debug & DBG_EVENT4)
				dump_dw4("    ", evp);
		}
		BDC_WRITECSR(softc, R_BDC_SRRINT0, val);	/* clear IP */
		if (ev == SR_XFR) {
			if (softc->debug & DBG_EVENT1) {
				xprintf("BDC: XFR event, sts=%d, epn=%d\n",
					sts, epn);
			}
			bdc_transfer_status(softc, evp, sts, epn);
		} else if (ev == SR_PSC) {
			bdc_psc(softc);
		} else if (ev < SR_MAX) {
			if (softc->debug & DBG_EVENT1)
				xprintf("BDC: %s event\n", SR_ST[ev]);
		} else {
			xprintf("BDC: Unhandled event (sts=%d)\n", ev);
			val &= ~M_SRRINT_IE;
		}
		++softc->srr0_dq;
		if (softc->srr0_dq >= softc->srr0_size)
			softc->srr0_dq = 0;

		/* update DQP */
		val &= ~(M_SRRINT_DQP + M_SRRINT_ISR + M_SRRINT_IP);
		val += V_SRRINT_DQP(softc->srr0_dq);
		BDC_WRITECSR(softc, R_BDC_SRRINT0, val);
	}
}


/* *********************************
 * Driver Start Functions
 * ********************************* */

static void bdc_probe(bolt_driver_t *drv, unsigned long probe_a,
		      unsigned long probe_b, void *probe_ptr)
{
	bdc_softc *softc;
	physaddr_t base;
	char descr[100];
	uint32_t vendev;

	if (env_getenv("BDCOFF"))
		return;

	softc = (bdc_softc *) KMALLOC(sizeof(bdc_softc), 0);
	if (softc == NULL) {
		xprintf("BDC: Failed to allocate softc memory.\n");
		return;
	}
	memset(softc, 0, sizeof(bdc_softc));
	bdc_check_debug(softc);
	if (softc->debug & DBG_LPBK_MODE) {
		softc->lpbk.on = 1;
		printf("BDC: LOOPBACK mode\n");
	}

	base = probe_a;
	if (bdc_init_buffers(softc)) {
		vendev = env_getval("BDC_VENDEV");
		if (vendev != 0xffffffff) {
			printf("BDC: Vendor/Device ID=0x%08x\n",
				vendev);
			bdc_change_vendev(softc, vendev);
		}
		if (bdc_start(softc, base)) {
			softc->drd_status = probe_b;
			xsprintf(descr, "%s at 0x%08x",
				drv->drv_description, base);
			bolt_attach(drv, softc, NULL, descr);
			bolt_bg_add(bdc_handle_events, softc);

			/* Success */
			return;
		}
	}

	/* Failed, free resources */
	bdc_free_buffers(softc);
	KFREE(softc);
}

int usb_init_bdc(physaddr_t base, physaddr_t ctrl)
{
	bolt_add_device((bolt_driver_t *) &bdc_drv, base, ctrl, 0);

	return 0;
}

/* *********************************
 * Application Layer Access Functions
 * ********************************* */

static int bdc_open(bolt_devctx_t *ctx)
{
	bdc_softc *softc = (bdc_softc *) ctx->dev_softc;

	bdc_check_debug(softc);
	if (softc->dev_speed != PSP_HS)
		return BOLT_ERR_UNSUPPORTED;
	if (softc->dev_state != CONFIGURED)
		return BOLT_ERR_DEVNOTFOUND;

	return 0;
}

static int bdc_read(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	bdc_softc *softc;

	if (ctx == NULL) {
		xprintf("BDC: No context\n");
		return -1;
	}
	if (buffer == NULL) {
		xprintf("BDC: No dst buffer\n");
		return -1;
	}
	softc = (bdc_softc *) ctx->dev_softc;
	if (softc == NULL) {
		xprintf("BDC: softc has not been initialized.\n");
		return -1;
	}
	if (softc->dev_state != CONFIGURED)
		return BOLT_ERR_DEVNOTFOUND;

	/* Return lesser of received and requested lengths.
	   BUT, caller must ask for the length was queued or expected.
	   Otherwise, data will be thrown away */
	if (softc->bulkout_len < (int) buffer->buf_length)
		buffer->buf_retlen = softc->bulkout_len;
	else
		buffer->buf_retlen = buffer->buf_length;
	memcpy(buffer->buf_ptr, softc->bulkout_buf, buffer->buf_retlen);
	if (softc->debug & DBG_BULK_OUT2) {
		xprintf("*** RX:\n");
		dump_data(buffer->buf_ptr, buffer->buf_retlen);
	}

	return 0;
}

static int bdc_inpstat(bolt_devctx_t *ctx, iocb_inpstat_t *inpstat)
{
	bdc_softc *softc;

	if (ctx == NULL) {
		xprintf("BDC: No context\n");
		return -1;
	}
	if (inpstat == NULL) {
		xprintf("BDC: No inpstat buffer\n");
		return -1;
	}
	softc = (bdc_softc *) ctx->dev_softc;
	if (softc == NULL) {
		xprintf("BDC: softc has not been initialized.\n");
		return -1;
	}
	if (softc->dev_state != CONFIGURED)
		return BOLT_ERR_DEVNOTFOUND;

	bdc_handle_events(softc);	/* have to poll */
	inpstat->inp_status = bdc_bulkout_done(softc);

	return 0;
}

static int bdc_write(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	int to = BDC_COMM_TIMEOUT*10;	/* mS -> 100uS units */
	bdc_softc *softc;

	if (ctx == NULL) {
		xprintf("BDC: No context\n");
		return -1;
	}
	if (buffer == NULL) {
		xprintf("BDC: No dest buffer\n");
		return -1;
	}
	softc = (bdc_softc *) ctx->dev_softc;
	if (softc == NULL) {
		xprintf("BDC: softc has not been initialized.\n");
		return -1;
	}
	if (softc->dev_state != CONFIGURED)
		return BOLT_ERR_DEVNOTFOUND;

	if (softc->debug & DBG_BULK_IN2) {
		xprintf("*** TX: len=%d\n", buffer->buf_length);
		dump_data(buffer->buf_ptr, buffer->buf_length);
	}

	bdc_bulkin_data(softc, buffer->buf_ptr, buffer->buf_length);

	/* block till send is done or timeout */
	do {
		bdc_handle_events(softc);	/* have to poll */
		if (--to == 0)
			return BOLT_ERR_TIMEOUT;
		bolt_usleep(100);
	} while (!bdc_bulkin_done(softc));

	return 0;
}

static int bdc_close(bolt_devctx_t *ctx)
{
	bdc_softc *softc = ctx->dev_softc;

	bdc_clear_bulk_state(softc);

	/* nothing else for now...BOLT exit will reset BDC */

	return 0;
}

static int bdc_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	bdc_softc *softc = ctx->dev_softc;
	int retval = 0, len;

	switch ((int)buffer->buf_ioctlcmd) {
	case IOCTL_USBBDC_GET_BULKOUT_MINSIZE:
		*(int *) buffer->buf_ptr = (softc->dev_speed == PSP_HS) ?
					   BULK_MPS : BULK_MPS_FS;
		break;
	case IOCTL_USBBDC_GET_BULKOUT_MAXSIZE:
		*(int *) buffer->buf_ptr = MAX_BULKOUT_LEN;
		break;
	case IOCTL_USBBDC_QUEUE_BULKOUT:
		len = *(int *) buffer->buf_ptr;
		if (len > MAX_BULKOUT_LEN)
			retval = BOLT_ERR_INV_PARAM;
		else if (bdc_bulkout_queued(softc))
			retval = BOLT_ERR_ALREADYBOUND;
		else
			bdc_bulkout_data(softc, softc->bulkout_buf, len);
		break;
	default:
		retval = BOLT_ERR_INV_COMMAND;
	}

	return retval;
}
