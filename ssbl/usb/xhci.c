/***************************************************************************
 *     Copyright (c) 2012-2016, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 *  For use with Broadcom XHCI controller only.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_printf.h"
#include "lib_string.h"
#include "lib_physio.h"
#include "timer.h"
#include "common.h"
#include "error.h"

#include "lib_malloc.h"
#include "lib_queue.h"
#include "usbchap9.h"
#include "usbd.h"
#include "xhci.h"
#include "usb_externs.h"

#include "env_subr.h"

/*  *********************************************************************
    *  Macros for dealing with hardware
    ********************************************************************* */

#define XHCI_VTOP(ptr)		((uint32_t)PHYSADDR((long)(ptr)))
#define XHCI_VTOP64(ptr)	((uint64_t)PHYSADDR((long)(ptr)))
#define U64(x)			((uint64_t)(long)(x))
#define U64P(x)			((uint64_t *)(long)(x))

#if CFG_CPU_COHERENT_DMA
#define XHCI_PTOV(ptr) ((void *)(CACADDR(ptr)))
#else
#define XHCI_PTOV(ptr) ((void *)(UNCADDR(ptr)))
#endif

#define XHCI_WRITECSR(softc, x, y) \
	DEV_WR((softc)->regs + (x), (y))

#define XHCI_READCSR(softc, x) \
	DEV_RD((softc)->regs + (x))


/*  *********************************************************************
    *  Macros to convert from "hardware" endpoint and transfer
    *  descriptors (trb_t) to "software" data structures
    *  (xhci_transfer_t).
    *
    *  Basically, there are two tables, indexed by the same value
    *  By subtracting the base of one pool from a pointer, we get
    *  the index into the other table.
    *
    *  We *could* have included the trb in the software
    *  data structures, but placing all the hardware stuff in one
    *  pool will make it easier for hardware that does not handle
    *  coherent DMA, since we can be less careful about what we flush
    *  and what we invalidate.
    ********************************************************************* */

#define xhci_transfer_from_trb(softc, trb) \
	((softc)->transfer_pool + ((trb) - (softc)->trb_pool))

/*  *********************************************************************
    *  Forward declarations
    ********************************************************************* */

static trb_t *xhci_alloc_ring(xhci_softc_t *softc);
static int xhci_intr(usbbus_t *bus);
static int xhci_roothub_xfer(usbbus_t *bus, usb_ept_t *uept, usbreq_t *ur);
static void xhci_roothub_statchg(xhci_softc_t *softc);

/*  *********************************************************************
    *  Globals
    ********************************************************************* */

static int xhcidebug;

/*  *********************************************************************
    *  Some debug routines
    ********************************************************************* */

void xhci_dumpportstat(int idx, uint32_t reg)
{
	printf("XHCI: Port %d: %08X\n", idx, reg);
}

void xhci_dump_trb(trb_t *trb)
{
	int i;
	uint32_t *p = (uint32_t *) trb;

	printf("XHCI: TRB @%p:", p);
	for (i = 0; i < 4; ++i)
		printf(" %08x", p[i]);
	printf("\n");
}

static void eptstats(xhci_softc_t *softc)
{
	int cnt;
	endpoint_t *e;

	cnt = 0;
	e = softc->endpoint_freelist;
	while (e) {
		e = e->next;
		cnt++;
	}
	printf("XHCI: EPT %d left, %d inuse\n", cnt, (EPTPOOL_SIZE - cnt));
}

static void ringstats(xhci_softc_t *softc)
{
	printf("XHCI: TRB %d in use\n", softc->ringcnt);
}

/*  *********************************************************************
    *  Helper functions
    ********************************************************************* */

static void *xhci_KUMALLOC(unsigned int size, unsigned int align)
{	/* need this function to zero out buffer */
	uint8_t *p;

	p = KUMALLOC(size, align);
	if (xhcidebug & SHOW_ALLOC)
		printf("KUMALLOC: size %d align %d @ %p\n", size, align, p);
	if (p)
		memset(p, 0, size);
	else
		printf("XHCI: KUMALLOC error p=%p\n", p);

	return p;
}

static void xhci_write_reg64(volatile uint64_t *p64, uint64_t val)
{
	volatile uint32_t *p32;

	p32 = (uint32_t *) p64;
	p32[0] = (uint32_t) (val & 0xffffffff);
	p32[1] = (uint32_t) ((val >> 32) & 0xffffffff);
}

static void xhci_set_link_trb(trb_t *ring, int num_trbs)
{
	trb_t *last_trb = &ring[num_trbs-1];

	last_trb->dw[0] = XHCI_VTOP(ring);
	last_trb->dw[1] = 0;
	last_trb->dw[2] = 0;
	/* cycle matches what h/w starts with */
	last_trb->dw[3] = V_TRB_CMD(LINK) + M_TRB_TC + M_TRB_CYC;
}

static int xhci_is_link_trb(trb_t *trb)
{
	int res;

	res = (G_TRB_ID((uint32_t *) trb) == LINK) &&
		(trb->dw[3] & M_TRB_TC);

	return res;
}

/* Mark (unused) addr_hi field only...rest kept for debug */
static void xhci_mark_trbs_free(trb_t *r, int cnt)
{
	int i;

	for (i = 0; i < cnt; ++i)
		r[i].dw[1] = 0xffffffff;
}

static int xhci_ev_done(int cc)
{
	return((cc == SUCCESS) || (cc == SHORT_PKT));
}

static dev_t *xhci_find_dev(xhci_softc_t *softc, int addr)
{
	xhci_t *xhci = softc->hc;
	int i;

	if (addr < 0)
		return 0;

	for (i = 0; i < xhci->nslots; ++i) {
		if (xhci->devs[i].addr == addr)
			break;
	}
	if (i >= xhci->nslots) {
		printf("XHCI EptCreate: address/context error\n");
		return 0;
	}

	return &xhci->devs[i];
}

/****************************************************************************/
static void xhci_init_ring(xhci_softc_t *softc, ring_t *r, trb_t *t)
{
	r->base = t;
	xhci_set_link_trb(t, NUM_TRBS);
	r->enq = t;
	r->cyc = M_TRB_CYC;	/* xHC starts @ 0, we start @ 1 */
}

/****************************************************************************/
static void xhci_ring_db(xhci_softc_t *softc, int slot, int ep, int in)
{
	xhci_t *xhci = softc->hc;
	int target;

	/* Make sure DMA information is flushed to memory */
	dmb();

	if (slot) {
		target = ep ? ((ep*2) + in) : 1;
		if (xhcidebug & SHOW_DBX) {
			printf(" - DB: slot=%d target=%d (ep=%d in=%d)\n",
				slot, target, ep, in);
		}
		xhci->db[slot] = target;
	} else {
		if (xhcidebug & SHOW_DB0)
			printf(" - DB: cmd\n");
		xhci->db[0] = 0;
	}
}

/****************************************************************************/
static event_trb_t *xhci_last_cmd_event(xhci_t *xhci)
{
	/* event structure updated by event handler (xhci_intr)
	   when command completes */
	return &xhci->last_cmd_event;
}

/****************************************************************************/
static int xhci_wait_cmd_done(xhci_softc_t *softc, int to)
{
	event_trb_t *ev;
	xhci_t *xhci = softc->hc;
	int res = -1;

	++softc->block_pcd;
	to *= 8;	/* mS to uFrame (125uS) units */
	while (to--) {
		xhci_intr(softc->bus);	/* force poll */
		ev = xhci_last_cmd_event(xhci);
		if (ev->ctrl) {
			if (xhci_ev_done(G_TRB_CC(ev)))
				res = 0;
			goto done;
		}
		bolt_usleep(125);
	}
	printf("XHCI: Event wait timeout!\n");

done:
	--softc->block_pcd;
	return res;
}

/****************************************************************************/
static void xhci_submit_trb(xhci_softc_t *softc, ring_t *tr, uint32_t dw0,
				uint32_t dw1, uint32_t dw2, uint32_t dw3)
{
	trb_t trb;

	trb.dw[0] = dw0;
	trb.dw[1] = dw1;
	trb.dw[2] = dw2;
	trb.dw[3] = dw3 + tr->cyc;

	/* Enqueue trb */
	memcpy(tr->enq, &trb, TRB_SIZE);
	if (xhcidebug & SHOW_XFER_TRB)
		xhci_dump_trb(tr->enq);

	/* Update TRB enqueue pointer */
	tr->enq += 1;
	if (xhci_is_link_trb(tr->enq)) {
		/* update link TRB with s/w's current TC to cause
		   the controller to wrap */
		tr->enq->dw[3] = (tr->enq->dw[3] & ~M_TRB_CYC) | tr->cyc;
		tr->enq = tr->base;
		tr->cyc ^= M_TRB_CYC;
		if (xhcidebug & SHOW_XFER_TRB)
			printf(" - TR wrap!\n");
	}
}

/****************************************************************************/
static int xhci_submit_cmd_trb(xhci_softc_t *softc, uint64_t dw01,
				uint32_t dw2, uint32_t dw3)
{
	trb_t trb;
	uint64_t *p64;
	xhci_t *xhci = softc->hc;

	p64 = U64P(&trb);
	*p64 = XHCI_VTOP64(dw01);
	trb.dw[2] = dw2;
	trb.dw[3] = dw3 + xhci->cmdr.cyc;	/* cmd + cycle bit */

	/* Enqueue trb */
	memcpy(xhci->cmdr.enq, &trb, TRB_SIZE);
	if (xhcidebug & SHOW_CMD_TRB)
		xhci_dump_trb(xhci->cmdr.enq);

	/* Update command ring enqueue pointer */
	xhci->cmdr.enq += 1;
	if (xhci_is_link_trb(xhci->cmdr.enq)) {
		/* update link TRB with s/w's current TC to cause the
		   controller to wrap */
		xhci->cmdr.enq->dw[3] = (xhci->cmdr.enq->dw[3] & ~M_TRB_CYC) |
					xhci->cmdr.cyc;
		xhci->cmdr.enq = xhci->cmdr.base;
		xhci->cmdr.cyc ^= M_TRB_CYC;
		if (xhcidebug & SHOW_CMD_TRB)
			printf(" - cmdr wrap!\n");
	}
	xhci_ring_db(softc, 0, 0, 0);
	memset(&xhci->last_cmd_event, 0, sizeof(event_trb_t));

	return xhci_wait_cmd_done(softc, XHCI_TIMEOUT);
}

/****************************************************************************/
static int xhci_find_parent_hub(xhci_softc_t *softc, uint32_t route, int rport)
{
	xhci_t *xhci = softc->hc;
	int i;

	for (i = 0; i < xhci->nslots; ++i) {
		if ((xhci->devs[i].route == route) &&
		    (xhci->devs[i].rport == rport)) {
			break;
		}
	}
	if (i >= xhci->nslots) {
		printf("XHCI: could not find parent hub slot\n");
		return -1;
	}
	return i;
}

/****************************************************************************/
static int xhci_find_tt_hubinfo(xhci_softc_t *softc, uint32_t route, int rport)
{
	xhci_t *xhci = softc->hc;
	uint32_t mask;
	int i, d = 0, port = 0, slot = 0;

	mask = 0xf0000;
	for (i = 4 ; i >= 0; --i) {
		if (route & mask) {
			port = (route & mask) >> (i * 4);
			route &= ~mask;		/* get parent route */
			d = xhci_find_parent_hub(softc, route, rport);
			if (d < 0)
				return -1;
			if (xhci->devs[d].speed == X_HS)
				break;
		}
		mask >>= 4;
	}
	if (i < 0) {
		printf("XHCI: route mask error (route %05x)\n", route);
		return -1;
	}
	slot = xhci->devs[d].slot;

	return ((port << 8) + slot);
}

/****************************************************************************/
static int xhci_init_dev_ctx(xhci_softc_t *softc, dev_t *dev, int mps)
{
	trb_t *tr;
	inp_ctx_t *ictx;
	int res, hport = 0, hslot = 0;
	uint32_t *p;

	tr = xhci_alloc_ring(softc);
	if (!tr)
		return -1;
	xhci_init_ring(softc, &dev->epr[0], tr);
	ictx = dev->ictx;
	memset(ictx, 0, sizeof(inp_ctx_t));
	ictx->ctl.add = 3;			/* add EP0 */
	if (dev->route && (dev->speed < X_HS)) {
		res = xhci_find_tt_hubinfo(softc, dev->route, dev->rport);
		if (res > 0) {
			hport = (res >> 8) & 0xff;
			hslot = res & 0xff;
		}
	}
	if (xhcidebug & SHOW_EPT_CFG1) {
		printf("cfgep[0]: spd %s rp %d slot %d ictx %p ectx %p mps %d tr %p route %05x\n",
			SPD_TYPE[dev->speed], dev->rport, dev->slot,
			ictx, &ictx->dctx.ep[0], mps, tr, dev->route);
		if (hslot)
			printf("          hport %d hslot %d\n", hport, hslot);
	}
	/* ContextEntries=1 (EP0), speed, route */
	ictx->dctx.slot.info1 = (1 << 27) | (dev->speed << 20) | dev->route;
	ictx->dctx.slot.info2 = dev->rport << 16;	/* RH port # */
	if (hslot)
		ictx->dctx.slot.tt = (hport << 8) + hslot;
	ictx->dctx.ep[0].info2 = (mps << 16) + (CTL_EP << 3) + (CERR << 1);
	ictx->dctx.ep[0].deq = XHCI_VTOP64(tr) + M_DCS;
	ictx->dctx.ep[0].info3 = 8;		/* Average EP0 TRB length */
	if (xhcidebug & SHOW_EPT_CFG3) {
		p = (uint32_t *) &ictx->dctx.slot;
		printf("cfgep[0] slot ctx: %08x %08x %08x %08x\n",
			p[0], p[1], p[2], p[3]);
		p = (uint32_t *) &ictx->dctx.ep[0];
		printf("cfgep[0] ep ctx:   %08x %08x %08x %08x %08x\n",
			p[0], p[1], p[2], p[3], p[4]);
	}

	return 0;
}

/****************************************************************************/
static int xhci_address_dev(xhci_softc_t *softc, dev_t *dev, int mps)
{
	inp_ctx_t *ictx;
	uint32_t dw3;

	ictx = dev->ictx;
	dw3 = (dev->slot << 24) + V_TRB_CMD(ADDR_DEV);
	if (mps) {
		ictx->ctl.add = 0;
		ictx->dctx.ep[0].info2 = (mps << 16) + (CTL_EP << 3) +
					 (CERR << 1);
		ictx->dctx.ep[0].deq = XHCI_VTOP64(dev->epr[0].enq) + M_DCS;
		if (xhcidebug & SHOW_EPT_CFG1)
			printf("cfgep[0]: ep0mps %d\n", mps);
	} else
		dw3 |= M_TRB_BSR;

	return xhci_submit_cmd_trb(softc, XHCI_VTOP(dev->ictx), 0, dw3);
}

/****************************************************************************/
static void xhci_deconfig_slot(xhci_softc_t *softc, dev_t *dev)
{
	xhci_t *xhci = softc->hc;
	uint32_t dw3;

	if (dev->config) {
		dw3 = (dev->slot << 24) + V_TRB_CMD(CONFIG) + M_TRB_DC;
		xhci_submit_cmd_trb(softc, 0, 0, dw3);
	}
	xhci_submit_cmd_trb(softc, 0, 0, (V_TRB_CMD(DIS_SLOT) +
			    (dev->slot << 24)));
	--softc->ctxcnt;
	--xhci->ndevs;
}

/****************************************************************************/
static int xhci_config_ep(xhci_softc_t *softc, dev_t *dev, int ep,
			int ep_type, int in, int mps, int add, int per,
			int hub)
{
	inp_ctx_t *ictx = dev->ictx;
	trb_t *tr;
	uint32_t dw3, mask, *p;
	int epidx, maxctx;
	int ttt = 0;

	epidx = V_EP_IDX(ep, in);
	mask = (1 << epidx);
	/* calc last context entry */
	maxctx = (ictx->dctx.slot.info1 >> 27) & 0x1f;
	if (epidx > maxctx)
		maxctx = epidx;
	if (hub && (dev->speed == X_HS))
		ttt = 3;		/* choose worst case */
	if (xhcidebug & SHOW_EPT_CFG2) {
		printf("cfgep[%d]: typ %d [%s] mps %d per %d epidx %d add %d mask %08x\n",
			ep, ep_type, EP_TYPE[ep_type], mps, per,
			epidx, add, mask);
		if (hub)
			printf("          hub %d ttt %d\n", hub, ttt);
	}
	if (add > 0) {
		ictx->ctl.add = mask;
		ictx->ctl.drop = 0;
		tr = xhci_alloc_ring(softc);
		if (!tr)
			return -1;
		xhci_init_ring(softc, &dev->epr[epidx], tr);
	} else if (add == 0) {
		ictx->ctl.add = 0;
		ictx->ctl.drop = mask;
	}
	if (xhcidebug & SHOW_EPT_CFG2) {
		printf("          maxctx %d ictx %p sctx %p ectx %p tr %p\n",
			maxctx, ictx, &ictx->dctx.slot,
			&ictx->dctx.ep[epidx-1], tr);
	}
	ictx->dctx.slot.info1 = (ictx->dctx.slot.info1 & ~(0x1f << 27)) |
				(hub << 26) | (maxctx << 27);
	if (hub) {
		ictx->dctx.slot.info2 |= (7 << 24);	/* num ports */
		ictx->dctx.slot.tt |= (ttt << 16);
	}
	ictx->dctx.ep[epidx-1].info1 = (per << 16);
	ictx->dctx.ep[epidx-1].info2 = (mps << 16) + (ep_type << 3) +
					(CERR << 1);
	ictx->dctx.ep[epidx-1].deq = XHCI_VTOP64(dev->epr[epidx].base) + M_DCS;
	/* ESIT, Average TRB len */
	ictx->dctx.ep[epidx-1].info3 = per ? (mps * 0x10001) : 8;
	dw3 = (dev->slot << 24) + V_TRB_CMD(CONFIG) +
		((add < 0) ? M_TRB_DC : 0);
	if (xhcidebug & SHOW_EPT_CFG3) {
		p = (uint32_t *) &ictx->dctx.slot;
		printf("cfgep slot ctx: %08x %08x %08x %08x\n",
			p[0], p[1], p[2], p[3]);
		p = (uint32_t *) &ictx->dctx.ep[epidx-1];
		printf("cfgep ep ctx:   %08x %08x %08x %08x %08x\n",
			p[0], p[1], p[2], p[3], p[4]);
	}

	return xhci_submit_cmd_trb(softc, XHCI_VTOP(ictx), 0, dw3);
}

/*  *********************************************************************
    *  xhci_alloc_ept(softc)
    *
    *  Allocate an endpoint data structure from the pool, and
    *  make it ready for use.  This structure is more to handle
    *  the top level driver interface and is not really needed by h/w.
    *
    *  Input parameters:
    *	softc - our XHCI controller
    *
    *  Return value:
    *	pointer to endpoint or NULL
    ********************************************************************* */

static endpoint_t *xhci_alloc_ept(xhci_softc_t *softc)
{
	endpoint_t *e;

	e = softc->endpoint_freelist;
	if (!e) {
		printf("XHCI: No endpoints left!\n");
		return NULL;
	}
	softc->endpoint_freelist = e->next;
	memset(e, 0, sizeof(endpoint_t));
	++softc->eptcnt;
	if (xhcidebug & SHOW_STATS) {
		printf("XHCI: AllocEpt: ");
		eptstats(softc);
	}

	return e;
}

/*  *********************************************************************
    *  xhci_alloc_ring(softc)
    *
    *  Allocate a TRB block for a ring.
    *
    *  Input parameters:
    *	softc - our XHCI controller
    *
    *  Return value:
    *	TRB ring base, or NULL
    ********************************************************************* */

static trb_t *xhci_alloc_ring(xhci_softc_t *softc)
{
	trb_t *t;
	int i;

	t = softc->trb_pool;
	i = TRBPOOL_SIZE;
	while (i > 0) {
		if (t->dw[1] == 0xffffffff)
			break;
		t += NUM_TRBS;
		i -= NUM_TRBS;
	}
	if (i <= 0) {
		printf("XHCI: No more TRB blocks!\n");
		return NULL;
	}
	memset((void *) t, 0, (NUM_TRBS * sizeof(trb_t)));

	++softc->ringcnt;
	if (xhcidebug & SHOW_STATS) {
		printf("XHCI AllocRing: ");
		ringstats(softc);
	}

	return t;
}

/*  *********************************************************************
    *  xhci_free_ring(softc, dev, ept)
    *
    *  Free a TRB ring by marking it for reuse.
    *
    *  Input parameters:
    *	softc - our XHCI controller
    *	dev - associated device
    *	e - associated endpoint
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void xhci_free_ring(xhci_softc_t *softc, dev_t *dev, endpoint_t *e)
{
	int in, epidx;

	in = (e->flags & UP_TYPE_IN) ? 1 : 0;
	epidx = V_EP_IDX(e->num, in);
	xhci_mark_trbs_free(dev->epr[epidx].base, NUM_TRBS);
	++softc->ringcnt;
}

/*  *********************************************************************
    *  xhci_freeept(softc,e)
    *
    *  Free an endpoint, returning it to the pool.
    *
    *  Input parameters:
    *	softc - our XHCI controller
    *	e - endpoint to free
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void xhci_freeept(xhci_softc_t *softc, endpoint_t *e)
{
	e->next = softc->endpoint_freelist;
	softc->endpoint_freelist = e;
	if (softc->hc->ndp) {	/* not during pool init */
		--softc->eptcnt;
		if (xhcidebug & SHOW_STATS) {
			printf("XHCI: FreeEpt[%p]: ", e);
			eptstats(softc);
		}
	}
}

/*  *********************************************************************
    *  xhci_init_data(softc)
    *
    *  Initialize the controller specific data structures.
    *
    *  Input parameters:
    *	softc - our XHCI controller
    *
    *  Return value:
    *	0 if ok
    *	else error code
    ********************************************************************* */

static void xhci_init_data(xhci_softc_t *softc, physaddr_t base)
{
	xhci_t *xhci = softc->hc;
	uint64_t *p64;
	int i, val;

	/* XHCI register addresses */
	xhci->base = (uint32_t *) base;
	xhci->caps = xhci->base;
	xhci->ops  = xhci->caps +
		(G_CAPVER_LEN(XHCI_READCSR(softc, R_CAPVER)) / 4);
	xhci->port = (port_t *) &xhci->ops[PORTSC_IDX];
	xhci->rts = (rts_t *) (xhci->caps +
				(XHCI_READCSR(softc, R_RTSOFF) / 4));
	xhci->db = (xhci->caps + (XHCI_READCSR(softc, R_DBOFF) / 4));
	val = G_PARAMS1_MSLS(XHCI_READCSR(softc, R_HCSPARAMS1));
	if (val <= MAX_SLOTS)
		xhci->nslots = val;
	else {
		printf("XHCI: overriding nslots from %d to %d!\n",
			val, MAX_SLOTS);
		xhci->nslots = MAX_SLOTS;
	}

	/* XHCI data structures */
	/*  DCBAA */
	xhci->pDCBAA = xhci_KUMALLOC(DCBAAP_LEN, DMA_BUF_ALIGN);

	/*  scratch pad */
	val = G_PARAMS2_MSBL(XHCI_READCSR(softc, R_HCSPARAMS2));
	if (val) {
		xhci->scratch_buf = U64(xhci_KUMALLOC((val*sizeof(uint64_t)),
							DMA_BUF_ALIGN));
		for (i = 0; i < val; ++i) {
			xhci->scratch[i] = U64(xhci_KUMALLOC(SCRATCH_BUF_SIZE,
								_KB(4)));
		}
		p64 = U64P(xhci->scratch_buf);
		p64[0] = XHCI_VTOP64(xhci->scratch[0]);
		p64[1] = XHCI_VTOP64(xhci->scratch[1]);
	}


	/*  command ring structure */
	xhci->cmdr.base = xhci_KUMALLOC(CMD_RING_LEN, DMA_BUF_ALIGN);

	/*  event ring structures */
	xhci->evr.base = xhci_KUMALLOC(EVENT_RING_LEN, DMA_BUF_ALIGN);
	xhci->pERST = xhci_KUMALLOC(sizeof(erst_t), DMA_BUF_ALIGN);
	xhci->pERST->size = NUM_TRBS;
	xhci->pERST->addr = XHCI_VTOP64(xhci->evr.base);

	/*  check AC64 & CSZ and report */
	val = XHCI_READCSR(softc, R_HCCPARAMS);
	if (!!(val & M_CPARAMS_CSZ) != !!CONTEXT_SIZE_64)
		printf("XHCI: context size mismatch!\n");

	/*  device slot entries */
	for (i = 0; i < xhci->nslots; ++i) {
		xhci->slots[i] = xhci_KUMALLOC(SLOT_SIZE, _KB(4));
		xhci->devs[i].ictx = xhci_KUMALLOC(sizeof(inp_ctx_t),
						   DMA_BUF_ALIGN);
	}

	xhci->ndp = G_PARAMS1_NPTS(XHCI_READCSR(softc, R_HCSPARAMS1));

	if (xhcidebug & SHOW_INIT_STATS) {
		printf("XHCI: cmdr %p evr %p slot0 %p ictx0 %p\n",
			xhci->cmdr.base, xhci->evr.base,
			xhci->slots[0], xhci->devs[0].ictx);
	}
}

/*  *********************************************************************
    *  xhci_initpools(softc)
    *
    *  Allocate and initialize the various pools of things that
    *  we use in the XHCI driver.  We do this by allocating some
    *  big chunks from the heap and carving them up.
    *
    *  Input parameters:
    *	softc - our XHCI controller
    *
    *  Return value:
    *	0 if ok
    *	else error code
    ********************************************************************* */

static int xhci_initpools(xhci_softc_t *softc)
{
	int idx;

	/* Do the TRB & transfer descriptor pool */
	softc->trb_pool =
	    xhci_KUMALLOC((TRBPOOL_SIZE * sizeof(trb_t)), DMA_BUF_ALIGN);
	softc->transfer_pool =
	    xhci_KUMALLOC((TRBPOOL_SIZE * sizeof(transfer_t)), DMA_BUF_ALIGN);
	if (!softc->transfer_pool || !softc->trb_pool) {
		printf("XHCI: Could not allocate TRB/transfer descriptors\n");
		return -1;
	}
	/* Use 0xff as a marker that a TRB block is available */
	xhci_mark_trbs_free(softc->trb_pool, TRBPOOL_SIZE);

	/* Do the endpoint descriptor pool and make all endpoint
	   descriptors available */
	softc->endpoint_pool =
	    xhci_KUMALLOC(EPTPOOL_SIZE * sizeof(endpoint_t), DMA_BUF_ALIGN);
	if (!softc->endpoint_pool) {
		printf("XHCI: Could not allocate endpoint descriptors\n");
		return -1;
	}
	softc->endpoint_freelist = NULL;
	for (idx = 0; idx < EPTPOOL_SIZE; idx++)
		xhci_freeept(softc, (softc->endpoint_pool + idx));
	if (xhcidebug & SHOW_INIT_STATS) {
		printf("XHCI InitEpt: ");
		eptstats(softc);
	}

	return 0;
}

/*  *********************************************************************
    *  xhci_stop(bus)
    *
    *  Stop the XHCI controller.
    *
    *  Input parameters:
    *	bus - our bus structure
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void xhci_stop(usbbus_t *bus)
{
	xhci_softc_t *softc = (xhci_softc_t *) bus->ub_hwsoftc;

	XHCI_WRITECSR(softc, R_USBCMD, 0);
	/* The HC is supposed to halt in 16 uFrames = 2mS */
	usb_delay_ms(bus, 5);
}

/*  *********************************************************************
    *  xhci_start(bus)
    *
    *  Start the XHCI controller.  After this routine is called,
    *  the hardware will be operational and ready to accept
    *  descriptors and interrupt calls.
    *
    *  Input parameters:
    *	bus - bus structure, from xhci_create
    *
    *  Return value:
    *	0 if ok
    *	else error code
    ********************************************************************* */

#define	USE_HCRESET	0	/* CSC gets cleared...must force CSC if WRC */
static int xhci_start(usbbus_t *bus)
{
	xhci_softc_t *softc = (xhci_softc_t *) bus->ub_hwsoftc;
	xhci_t *xhci = softc->hc;
	uint32_t reg;
	int i;

	/* Wait for controller to be ready */
	i = 50;
	while ((XHCI_READCSR(softc, R_USBSTS) & M_USBSTS_CNR) && i--)
		usb_delay_ms(bus, 1);
	if (!i) {
		printf("XHCI: Controller not ready!\n");
		return -1;
	}

	/* Halt the controller, if necessary */
	if (!(XHCI_READCSR(softc, R_USBSTS) & M_USBSTS_CHALT))
		xhci_stop(bus);

#if USE_HCRESET
	/* Reset the host controller.  */
	reg = XHCI_READCSR(softc, R_USBCMD) | M_USBCMD_HCR;
	XHCI_WRITECSR(softc, R_USBCMD, reg);
	reg = XHCI_READCSR(softc, R_USBCMD);
	for (i = 0; i < 5; i++) {
		if (!(XHCI_READCSR(softc, R_USBCMD) & M_USBCMD_HCR))
			break;
		usb_delay_ms(bus, 1);
	}

	/* HC Stop & Reset */
	if (XHCI_READCSR(softc, R_USBCMD) & M_USBCMD_HCR) {
		/* controller never came out of reset */
		printf("XHCI: Controller reset failed!\n");
		return -1;
	}
#endif

	/* CONFIG.MaxSlotsEn */
	XHCI_WRITECSR(softc, R_CONFIG, xhci->nslots);

	 /* Device Context base address...DCBAAP */
	XHCI_WRITECSR(softc, R_DCBAAPL, XHCI_VTOP(xhci->pDCBAA));
	XHCI_WRITECSR(softc, R_DCBAAPH, 0);

	 /* Device Context info[0]...scratch pad */
	xhci->pDCBAA[0] = XHCI_VTOP64(xhci->scratch_buf);

	 /* Device Context info[1]...slot 1 */
	for (i = 0; i < xhci->nslots; ++i)
		xhci->pDCBAA[i+1] = XHCI_VTOP64(xhci->slots[i]);

	/* Command ring...enqueue pointer & cycle bit */
	xhci->cmdr.enq = xhci->cmdr.base;
	xhci->cmdr.cyc = M_TRB_CYC;	/* xHC starts @ 0, we start @ 1 */
	xhci_set_link_trb(xhci->cmdr.base, NUM_TRBS);
	XHCI_WRITECSR(softc, R_CRCRL, 0);
	XHCI_WRITECSR(softc, R_CRCRH, 0);
	XHCI_WRITECSR(softc, R_CRCRL,
			(XHCI_VTOP(xhci->cmdr.base) + M_TRB_CYC));
	XHCI_WRITECSR(softc, R_CRCRH, 0);

	/* Event ring...enqueue pointer & cycle bit.
	   Note that there is no link TRB...goes by erstsz.size */
	xhci->evr.deq = xhci->evr.base;
	xhci->evr.cyc = 0;		/* xHC starts @ 1, we start @ 0 */
	xhci->rts->irs[0].erstsz = ERST_SIZE;	/* table size */
	xhci_write_reg64(&xhci->rts->irs[0].erstba, XHCI_VTOP64(xhci->pERST));
	xhci_write_reg64(&xhci->rts->irs[0].erdp, XHCI_VTOP64(xhci->evr.base));

	/* Enable interrupt management at lower (L2) level  for immediate */
	xhci->rts->irs[0].iman = 3;
	xhci->rts->irs[0].imod = 0;

	/* Remember how many ports we have */
	reg = XHCI_READCSR(softc, R_HCSPARAMS1);
	xhci->ndp = G_PARAMS1_NPTS(reg);

	/* Start the controller started */
	reg = XHCI_READCSR(softc, R_USBCMD);
	XHCI_WRITECSR(softc, R_USBCMD, (reg | M_USBCMD_RUN));
	usb_delay_ms(bus, 10);

#if USE_HCRESET
	/* Wait for one port to recover from controller reset */
	for (i = 1; i <= 500; i++) {
		if (!(XHCI_READCSR(softc, R_PORTSC(1)) & M_PORTSC_PR))
			break;
		usb_delay_ms(bus, 1);
	}
#endif

	/* Clear start up port reset flags */
	for (i = 1; i <= xhci->ndp; i++) {
		XHCI_WRITECSR(softc, R_PORTSC(i),
			(M_PORTSC_PP | M_PORTSC_WRC | M_PORTSC_PRC));
	}

	return 0;
}

/*  *********************************************************************
    *  xhci_cancel_xfer(bus,ept)
    *
    *  Cancels a transfer, typically due to endpoint deletion,
    *  by completing pending transfer for the endpoint.
    *  Note: Only one transfer per endpoint now.
    *
    *  Input parameters:
    *	bus - xhci bus structure
    *	ept - endpoint to remove
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void xhci_cancel_xfer(usbbus_t *bus, usb_ept_t *uept)
{
	endpoint_t *ept = (endpoint_t *) uept;
	usbreq_t *ur;
	transfer_t *xfer;

	xfer = ept->xfer;
	if (xfer) {
		ur = xfer->t_ref;
		if (xhcidebug & SHOW_FREE)
			printf("XHCI EptCancel: ept %p xfer %p\n", ept, xfer);
		if (ur) {
			ur->ur_status = K_XHCI_TD_CANCELLED;
			ur->ur_tdcount--;
			if (ur->ur_tdcount <= 0) {
				if (xhcidebug & SHOW_FREE) {
					printf("EHCI EptCancel: Completing request due to closed pipe: %p\n",
						ur);
				}
				usb_complete_request(ur, K_XHCI_TD_CANCELLED);
				/* NOTE: It is expected that the callee will
				free the usbreq. */
			}
		}
	}
}

/*  *********************************************************************
    *  xhci_xferdone(softc)
    *
    *  Process the completion of a data transfer.
    *
    *  Input parameters:
    *	softc - our XHCI controller
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static int xhci_xferdone(xhci_softc_t *softc, event_trb_t *ev)
{
	transfer_t *xfer;
	usbreq_t *ur;
	trb_t *trb;
	int val, res;

	trb = (trb_t *) XHCI_PTOV(ev->trbl);
	xfer = xhci_transfer_from_trb(softc, trb);
	ur = xfer->t_ref;
	if (xhcidebug & SHOW_XFER_DONE)
		printf("XHCI XferDone: trb %p xfer %p ur %p", trb, xfer, ur);

	if (xhci_ev_done(G_TRB_CC(ev)))
		val = 0;
	else {
		val = BOLT_ERR_IOERR;
		if (xfer->ept->num != -1) { /* if not a disconnect */
			printf("XHCI Transfer error: 0x%02x (ur=%p)\n",
				val, ur);
		}
	}

	/* See if it's time to call the callback */
	if (ur) {
		ur->ur_status = val;
		--ur->ur_tdcount;
		ur->ur_xferred = xfer->t_len;

		/* update request length with this TRB's transfer */
		res = G_TRB_LEN(ev->cc_len);
		if (res)
			ur->ur_xferred -= res;

		if (xhcidebug & SHOW_XFER_DONE)
			printf(" %d bytes", ur->ur_xferred);

		/* check if all transfers are done */
		if (ur->ur_tdcount == 0) {
			if (ur->ur_xferred) {
				CACHE_INVAL_RANGE(ur->ur_buffer,
						  ur->ur_xferred);
			}
			usb_complete_request(ur, val);
			xfer->ept->xfer = 0;
			xfer->ept = 0;
			xfer->t_ref = 0;
		}
	}
	if (xhcidebug & SHOW_XFER_DONE)
		printf("!\n");

	return val;
}

/*  *********************************************************************
    *  xhci_event(softc)
    *
    *  Process the completion of a event.
    *
    *  Input parameters:
    *	softc - our XHCI controller
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void xhci_event(xhci_softc_t *softc)
{
	xhci_t *xhci = softc->hc;
	static int i, wrap;
	int cmd, port, typ = 0, cc, discon_err = 0;
	event_trb_t *ev;
	uint32_t *ev1, *cmd_trb;

	/* Clear pending interrupt status */
	xhci->rts->irs[0].iman = 3;

	/* Process the completed event list */
	if (xhcidebug & SHOW_EVENTS4)
		printf("Event deq %p\n", xhci->evr.deq);
	ev = (event_trb_t *) xhci->evr.deq;
	while (ev->ctrl && ((ev->ctrl & M_TRB_CYC) != xhci->evr.cyc)) {
		ev1 = (uint32_t *) ev;
		if (xhcidebug & SHOW_EVENTS1)
			printf("Event %2d (@%p):", i, ev1);
		cmd = -1;
		cc = G_TRB_CC(ev);
		typ = G_TRB_ID(ev);
		switch (typ) {
		default:
			printf(" Trb=%08x", ev1[0]);
			break;

		case (XFER):
			if (xhcidebug & SHOW_EVENTS2)
				printf(" Trb=%08x Xfer\n", ev1[0]);
			discon_err = xhci_xferdone(softc, ev);
			break;
		case (CMD_COMP):
			memcpy(&xhci->last_cmd_event, ev, sizeof(event_trb_t));
			cmd_trb = XHCI_PTOV(ev1[0]);
			cmd = G_TRB_ID(cmd_trb);
			if (xhcidebug & SHOW_EVENTS2)
				printf(" Trb=%08x CmdComp", ev1[0]);
			break;
		case (PSC):	/* RH functions handle these */
			port = (ev1[0] >> 24) - 1;
			if (xhcidebug & SHOW_EVENTS2)
				printf("PSC...%d", port);
			break;
		}
		if (xhcidebug & SHOW_EVENTS2) {
			if (cmd >= 0) {
				printf(", CMD=%s (trb=%08x, ctl=%08x, sts=%08x)%s",
					TRB_TYPE[cmd], ev1[0], ev1[3], ev1[2],
					(xhci_ev_done(cc) ? "" : "  {ERR!} "));
			}
		} else if (!xhci_ev_done(cc) && !discon_err) {
			printf("XHCI: %s TRB failed! (cc=%02x, <%s>)\n",
			((typ == XFER) ? "XFER" : "CMD"), cc, TRB_CC_CODE[cc]);
		}
		++i;
		if (xhcidebug & SHOW_EVENTS)
			printf("\n");

		/* Update event dequeue pointer */
		if ((xhci->evr.deq - xhci->evr.base + 1) >= NUM_TRBS) {
			xhci->evr.deq = xhci->evr.base;
			xhci->evr.cyc ^= M_TRB_CYC;
			if (xhcidebug & SHOW_EVENTS4)
				printf("XHCI - EVR wrap %d\n", ++wrap);
		} else
			xhci->evr.deq += 1;
		ev = (event_trb_t *) xhci->evr.deq;
	}

	/* update dequeue ptr */
	xhci_write_reg64(&xhci->rts->irs[0].erdp,
			 (XHCI_VTOP64(xhci->evr.deq) | M_ERDP_EHB));

	if (xhcidebug & SHOW_EVENTS4)
		printf("XHCI: Event list processed\n");
}

/*  *********************************************************************
    *  xhci_intr(bus)
    *
    *  Process pending interrupts for the XHCI controller.
    *
    *  Input parameters:
    *	bus - our bus structure
    *
    *  Return value:
    *	0 if we did nothing
    *	nonzero if we did something.
    ********************************************************************* */

static int xhci_intr(usbbus_t *bus)
{
	uint32_t reg;
	xhci_softc_t *softc = (xhci_softc_t *) bus->ub_hwsoftc;

	/* Read the interrupt status register and don't bother
	   doing anything if nothing happened  */
	reg = XHCI_READCSR(softc, R_USBSTS);
	if (!softc->pending_pcd && (reg == 0))
		return 0;

	/* Write interrupt status value back to clear bits that were set */
	XHCI_WRITECSR(softc, R_USBSTS, reg);

	/* Event interrupt */
	if (reg & M_USBSTS_EINT)
		xhci_event(softc);

	/* Root Hub Status Change */
	if ((softc->block_pcd == 0) &&
	    (softc->pending_pcd || (reg & M_USBSTS_PCD))) {
		uint32_t lreg;

		if (xhcidebug & SHOW_RH_INFO1) {
			/* show only 1st port */
			lreg = XHCI_READCSR(softc, R_PORTSC(1));
			printf("XHCI RootHubStatusChange: %08x\n", lreg);
		}
		xhci_roothub_statchg(softc);
	}

	/* Host controller internal error */
	if ((reg & M_USBSTS_HCE) && !softc->error) {
		softc->error = 1;
		printf("XHCI: Host Controller ERROR\n");
	}

	/* System error...memory bus access */
	if ((reg & M_USBSTS_HSERR) && !softc->error) {
		softc->error = 1;
		printf("XHCI: Host System ERROR\n");
	}

	return 1;
}

/*  *********************************************************************
    *  xhci_delete(bus)
    *
    *  Remove an XHCI bus structure and all resources allocated to
    *  it (used when shutting down USB)
    *
    *  Input parameters:
    *	bus - our USB bus structure
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void xhci_delete(usbbus_t *bus)
{
	/* xxx fill in later. */
}

/*  *********************************************************************
    *  xhci_create(addr)
    *
    *  Create a USB bus structure and associate it with our XHCI
    *  controller device.
    *
    *  Input parameters:
    *	addr - physical address of controller
    *
    *  Return value:
    *	usbbus structure pointer
    ********************************************************************* */

static usbbus_t *xhci_create(physaddr_t addr)
{
	int res;
	xhci_softc_t *softc;
	usbbus_t *bus;

	xhcidebug = env_getval("XHCIDBG");

	if (xhcidebug == -1)
		xhcidebug = 0;
	else
		printf("*** xhcidebug: %x\n", xhcidebug);

	softc = xhci_KUMALLOC(sizeof(xhci_softc_t), DMA_BUF_ALIGN);
	if (!softc)
		return NULL;
	memset(softc, 0, sizeof(xhci_softc_t));

	bus = xhci_KUMALLOC(sizeof(usbbus_t), DMA_BUF_ALIGN);
	if (!bus) {
		KUFREE(softc);
		return NULL;
	}
	memset(bus, 0, sizeof(usbbus_t));

	softc->hc = xhci_KUMALLOC(sizeof(xhci_t), DMA_BUF_ALIGN);
	if (!softc->hc)
		return NULL;
	memset(softc->hc, 0, sizeof(xhci_t));

	bus->ub_hwsoftc = (usb_hc_t *) softc;
	bus->ub_hwdisp = &xhci_driver;
	bus->ub_flags = UB_FLG_USB30;

	q_init(&(softc->rh_intrq));

	softc->regs = addr;
	softc->rh_newaddr = -1;
	softc->bus = bus;

	res = xhci_initpools(softc);
	if (res != 0)
		goto error;
	xhci_init_data(softc, addr);

	softc->ringcnt = 0;

	return bus;

error:
	KUFREE(softc);
	KUFREE(bus);
	return NULL;
}

/*  *********************************************************************
    *  xhci_ept_create(bus,usbaddr,eptnum,mps,flags)
    *
    *  Create a hardware endpoint structure and attach it to
    *  the hardware's endpoint list.  The hardware manages lists
    *  of queues, and this routine adds a new queue to the appropriate
    *  list of queues for the endpoint in question.  It roughly
    *  corresponds to the information in the XHCI specification.
    *  Data toggle is always handled in the QH overlay.
    *
    *  Input parameters:
    *	bus - the USB bus we're dealing with
    *	usbaddr - USB address (0 means default address)
    *	eptnum - the endpoint number
    *	mps - the packet size for this endpoint
    *	flags - various flags to control endpoint creation
    *
    *  Return value:
    *	endpoint structure pointer, or NULL
    ********************************************************************* */

static usb_ept_t *xhci_ept_create(usbbus_t *bus,
				  int addr, int epnum, int mps, int flags)
{
	xhci_softc_t *softc = (xhci_softc_t *) bus->ub_hwsoftc;
	endpoint_t *ept;
	dev_t *dev;
	xhci_t *xhci = softc->hc;
	int res, slot, in, eptype, hub, iper = 0;

	ept = xhci_alloc_ept(softc);
	ept->flags = flags;
	ept->mps = mps;
	ept->num = epnum;
	ept->addr = addr;	/* for context/EPT sync */
	hub = (flags & UP_TYPE_HUB) ? 1 : 0;

	if (xhcidebug & SHOW_EPT_CREATE) {
		printf("XHCI EptCreate: ep %d addr %d flags %08x mps %d hub %d\n",
			epnum, addr, flags, mps, hub);
	}
	/* Device context only for devices. If root hub already addressed or
	   address is not 1 then this request is not for RH */
	if (softc->rh_addr && (addr != 1)) {
		if (epnum == 0) {
			/* Create a device context, get a slot, config EP0
			   and set up device address */
			if (xhci->ndevs >= xhci->nslots) {
				printf("XHCI: Too many devices...max=%d!\n",
					xhci->nslots);
				ept->addr = -1;
				goto done;
			}
			res = xhci_submit_cmd_trb(softc, 0, 0,
						  V_TRB_CMD(EN_SLOT));
			if (res < 0) {
				printf("XHCI: Enable slot failed!\n");
				ept->addr = -1;
				goto done;
			}
			slot = xhci->last_cmd_event.ctrl >> 24;
			dev = &xhci->devs[slot-1];
			if (flags & UP_TYPE_SUPRSPEED) {
				dev->speed = X_SS;
				ept->mps = 512;
			} else if (flags & UP_TYPE_HIGHSPEED) {
				dev->speed = X_HS;
				ept->mps = 64;
			} else if (flags & UP_TYPE_LOWSPEED)
				dev->speed = X_LS;
			else
				dev->speed = X_FS;
			dev->slot = slot;
			dev->config = 0;
			dev->error = 0;
			dev->rport = (flags >> UP_RPORT_SHIFT) & UP_RPORT_MASK;
			++dev->rport;	/* keep it one based */
			dev->route = (flags >> UP_ROUTE_SHIFT) & UP_ROUTE_MASK;
			dev->addr = addr;	/* for context/EPT sync */
			ept->dev = dev;		/* maintain cross-link */
			dev->pipes = 1;
			res = xhci_init_dev_ctx(softc, dev, ept->mps);
			if (res < 0) {
				printf("XHCI: Device context init failed!\n");
				dev->error = 1;
				goto done;
			}
			res = xhci_address_dev(softc, dev, 0);
			if (res < 0) {
				printf("XHCI: Address device (a) failed!\n");
				dev->error = 1;
				goto done;
			}
			++xhci->ndevs;
		} else {
			dev = xhci_find_dev(softc, addr);
			if (!dev || (dev->error))
				goto done;
			++dev->pipes;
			dev->hub = hub;
			ept->mps = mps;
			in = (flags & UP_TYPE_IN) ? 1 : 0;
			if (flags & UP_TYPE_CONTROL)
				eptype = CTL_EP;
			else if (flags & UP_TYPE_BULK)
				eptype = in ? BULK_IN_EP : BULK_OUT_EP;
			else if (flags & UP_TYPE_INTR) {
				eptype = in ? INTR_IN_EP : INTR_OUT_EP;
				iper = 10;	/* period = 128mS */
			} else if (flags & UP_TYPE_ISOC)
				eptype = in ? ISOC_IN_EP : ISOC_OUT_EP;
			else {
				printf("XHCI EptCreate: invalid EP type\n");
				goto done;
			}
			res = xhci_config_ep(softc, dev, epnum, eptype,
					     in, ept->mps, 1, iper, dev->hub);
			if (res < 0) {
				ept->error = 1;
				printf("XHCI: EP %d config failed!\n", epnum);
			}
			dev->config = 1;
		}
		if (xhcidebug & SHOW_EPT_CREATE) {
			printf("XHCI EptCreate: dev %p devs %d slot %d rp %d pipes %d\n",
				dev, xhci->ndevs, dev->slot, dev->rport,
				dev->pipes);
		}
	}
done:

	return (usb_ept_t *) ept;
}

/*  *********************************************************************
    *  xhci_ept_setaddr(bus,ept,usbaddr)
    *
    *  With XHCI, the device address is done by h/w.  So, we use this
    *  incoming address for internal sync between EPTs and device
    *  contexts.
    *
    *  Input parameters:
    *	bus - usb bus structure
    *	ept - an open endpoint descriptor
    *	addr - new address from upper layer
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void xhci_ept_setaddr(usbbus_t *bus, usb_ept_t *uept, int addr)
{
	endpoint_t *ept = (endpoint_t *) uept;

	ept->addr = addr;
	if (addr > 1)	/* not for RH */
		ept->dev->addr = addr;
}

/*  *********************************************************************
    *  xhci_ept_setmps(bus,ept,mps)
    *
    *  Set the maximum packet size of this endpoint.  This is
    *  normally used during the processing of endpoint 0 (default
    *  pipe) after we find out how big ep0's packets can be.
    *
    *  Input parameters:
    *   bus - our USB bus structure
    *   ept - endpoint structure
    *   mps - new packet size
    *
    *  Return value:
    *   nothing
    *
    ********************************************************************* */

static void xhci_ept_setmps(usbbus_t *bus, usb_ept_t *uept, int mps)
{
	xhci_softc_t *softc = (xhci_softc_t *) bus->ub_hwsoftc;
	endpoint_t *ept = (endpoint_t *) uept;
	dev_t *dev;
	int res;

	if (softc->rh_addr) {	/* for non-root hub devices only */
		dev = xhci_find_dev(softc, ept->addr);
		if (!dev)
			return;
		if (!dev->error && !ept->error) {
			res = xhci_address_dev(softc, dev, mps);
			if (res < 0) {
				printf("XHCI: Address device (b) failed!\n");
				dev->error = 1;
			}
		}
	}
}

/*  *********************************************************************
    *  xhci_ept_cleartoggle(bus,ept)
    *
    *  Not used for this controller.
    *
    ********************************************************************* */

static void xhci_ept_cleartoggle(usbbus_t *bus, usb_ept_t *uept)
{
}

/*  *********************************************************************
    *  xhci_ept_delete(bus,ept)
    *
    *  Deletes an endpoint from the EHCI controller.  This
    *  routine also completes pending transfers for the
    *  endpoint and gets rid of the hardware ept (queue base).
    *
    *  Input parameters:
    *	bus - xhci bus structure
    *	ept - endpoint to remove
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void xhci_ept_delete(usbbus_t *bus, usb_ept_t *uept)
{
	xhci_softc_t *softc = (xhci_softc_t *) bus->ub_hwsoftc;
	endpoint_t *ept = (endpoint_t *) uept;
	dev_t *dev;

	if (xhcidebug & SHOW_EPT_DEL)
		printf("XHCI EptDel: addr %d ep %d ", ept->addr, ept->num);

	/* Remove endpoint from queue and complete transfers
	   and return endpoint to free pool */
	dev = xhci_find_dev(softc, ept->addr);

	if (dev) {
		if (xhcidebug & SHOW_EPT_DEL) {
			printf("dev %p slot %d pipes %d config %d\n",
			dev, dev->slot, dev->pipes, dev->config);
		}
		xhci_cancel_xfer(bus, uept);
		xhci_free_ring(softc, dev, ept);
		ept->num = -1; /* mark for disconnect processing */
		xhci_freeept(softc, ept);
		--dev->pipes;
		if (dev->pipes == 0)
			xhci_deconfig_slot(softc, dev);
	} else
		xhci_freeept(softc, ept);
	if (xhcidebug & SHOW_EPT_DEL)
		printf("\n");
}

/*  *********************************************************************
    *  xhci_xfer(bus,ept,ur)
    *
    *  Queue a transfer for the specified endpoint.  Depending on
    *  the transfer type, the transfer may go on one of many queues.
    *  When the transfer completes, a callback will be called.
    *
    *  New QTDs are appended to the tail of the QTD list.  The head will
    *  always be the dummy QTD.
    *
    *  NOTE: Incoming buffer must be aligned to wMaxPacketSize when size
    *        is such that multiple QTDs are used.
    *
    *  Input parameters:
    *	bus - bus structure
    *	ept - endpoint descriptor
    *	ur - request (includes pointer to user buffer)
    *
    *  Return value:
    *	0 if ok
    *	else error
    ********************************************************************* */

static int xhci_xfer(usbbus_t *bus, usb_ept_t *uept, usbreq_t *ur)
{
	xhci_softc_t *softc = (xhci_softc_t *) bus->ub_hwsoftc;
	endpoint_t *ept = (endpoint_t *) uept;
	dev_t *dev;
	usb_device_request_t *req;
	transfer_t *xfer = 0;
	trb_t *trb;
	uint8_t *buf;
	int len, wVal, wIdx, epidx;
	uint32_t dw0, dw1, dw2, dw3, trt;

	/* If the destination USB address matches the address of the
	   root hub, shunt the request over to our root hub emulation */
	if (ur->ur_dev->ud_address == softc->rh_addr)
		return xhci_roothub_xfer(bus, uept, ur);

	dev = xhci_find_dev(softc, ept->addr);
	if (!dev)
		return 0;
	if (dev->error || ept->error) {
		usb_complete_request(ur, -1);
		return 0;
	}

	buf = ur->ur_buffer;
	len = ur->ur_length;
	if (len)
		CACHE_FLUSH_RANGE(buf, len);

	/* Set up TRB based on the request type */
	if (ur->ur_flags & UR_FLAG_SETUP) {
		req = (usb_device_request_t *) buf;
		wVal = GETUSBFIELD(req, wValue);
		wIdx = GETUSBFIELD(req, wIndex);
		dw0 = (wVal << 16) + (req->bRequest << 8) + req->bmRequestType;
		dw1 = (GETUSBFIELD(req, wLength) << 16) + wIdx;
		dw2 = 0x00000008;	/* TRB xfer len = setup packet len */
		if (len) {
			trt = (req->bmRequestType & USBREQ_DIR_IN) ?
				IN_DATA : OUT_DATA;
		} else
			trt = NO_DATA;
		dw3 = V_TRB_TRT(trt) + V_TRB_CMD(CTL_SETUP) +
			M_TRB_IDT + M_TRB_IOC;
		trb = dev->epr[0].enq;
		xhci_submit_trb(softc, &dev->epr[0], dw0, dw1, dw2, dw3);
		xhci_ring_db(softc, dev->slot, 0, 0);
	} else if (ur->ur_flags & UR_FLAG_IN) {
		dw2 = len;
		if (ept->num == 0) {
			dw3 = M_TRB_EP0_DIR_IN + V_TRB_CMD(CTL_DATA) +
				M_TRB_IOC;
			trb = dev->epr[0].enq;
			xhci_submit_trb(softc, &dev->epr[0],
					XHCI_VTOP(buf), 0, dw2, dw3);
			xhci_ring_db(softc, dev->slot, 0, 0);
		} else {
			dw3 = V_TRB_CMD(NORM) + M_TRB_IOC;
			epidx = V_EP_IDX(ept->num, 1);
			trb = dev->epr[epidx].enq;
			xhci_submit_trb(softc, &dev->epr[epidx],
					XHCI_VTOP(buf), 0, dw2, dw3);
			xhci_ring_db(softc, dev->slot, ept->num, 1);
		}
	} else if (ur->ur_flags & UR_FLAG_OUT) {
		dw2 = len;
		if (ept->num == 0) {
			dw3 = M_TRB_EP0_DIR_OUT + V_TRB_CMD(CTL_DATA) +
				M_TRB_IOC;
			trb = dev->epr[0].enq;
			xhci_submit_trb(softc, &dev->epr[0],
					XHCI_VTOP(buf), 0, dw2, dw3);
			xhci_ring_db(softc, dev->slot, 0, 0);
		} else {
			dw3 = V_TRB_CMD(NORM) + M_TRB_IOC;
			epidx = V_EP_IDX(ept->num, 0);
			trb = dev->epr[epidx].enq;
			xhci_submit_trb(softc, &dev->epr[epidx],
					XHCI_VTOP(buf), 0, dw2, dw3);
			xhci_ring_db(softc, dev->slot, ept->num, 0);
		}
	} else if (ur->ur_flags & UR_FLAG_STATUS_OUT) {
		dw3 = M_TRB_EP0_DIR_OUT + V_TRB_CMD(CTL_STATUS) + M_TRB_IOC;
		trb = dev->epr[0].enq;
		xhci_submit_trb(softc, &dev->epr[0], 0, 0, 0, dw3);
		xhci_ring_db(softc, dev->slot, 0, 0);
	} else if (ur->ur_flags & UR_FLAG_STATUS_IN) {
		dw3 = M_TRB_EP0_DIR_IN + V_TRB_CMD(CTL_STATUS) + M_TRB_IOC;
		trb = dev->epr[0].enq;
		xhci_submit_trb(softc, &dev->epr[0], 0, 0, 0, dw3);
		xhci_ring_db(softc, dev->slot, 0, 0);
	} else {
		printf("XHCI: Shouldn't happen...invalid ur_flags!\n");
		return 0;
	}
	ur->ur_tdcount = 1;	/* just one TRB (64kb) is enough for now */
	xfer = xhci_transfer_from_trb(softc, trb);
	xfer->t_ref = ur;
	xfer->t_len = len;
	xfer->ept = ept;	/* set up cross-links */
	ept->xfer = xfer;
	if (xhcidebug & SHOW_XFER_QUEUE2)
		printf("XHCI Xfer: trb %p xfer %p ur %p\n", trb, xfer, ur);

	if (xhcidebug & SHOW_XFER_QUEUE1) {
		printf("XHCI Xfer: ur %p addr %d pipe %02X buf %p len %d\n",
			ur, ur->ur_dev->ud_address, ur->ur_pipe->up_num,
			buf, len);
	}

	return 0;
}

/*  *********************************************************************
    *  xhci_reset(bus)
    *
    *  Reset XHCI
    *
    *  Input parameters:
    *	bus - our USB bus structure
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void xhci_reset(usbbus_t *bus)
{
	xhci_softc_t *softc = (xhci_softc_t *) bus->ub_hwsoftc;

	xhci_stop(bus);
	XHCI_WRITECSR(softc, R_USBCMD, M_USBCMD_HCR);
}

/*  *********************************************************************
    *  Driver structure
    ********************************************************************* */

usb_hcdrv_t xhci_driver = {
	xhci_create,
	xhci_delete,
	xhci_start,
	xhci_stop,
	xhci_intr,
	xhci_ept_create,
	xhci_ept_delete,
	xhci_ept_setmps,
	xhci_ept_setaddr,
	xhci_ept_cleartoggle,
	xhci_xfer,
	xhci_reset
};

/*  *********************************************************************
    *  Root Hub - 3.0
    *
    *  Data structures and functions
    ********************************************************************* */

#define USB_HUB_DESCRIPTOR_SS_TYPE           0x2A

/*
 * Data structures and routines to emulate the 3.0 root hub.
 */
static usb_device_descr_t xhci_root_devdsc = {
	sizeof(usb_device_descr_t),	/* bLength */
	USB_DEVICE_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USBWORD(0x0300),	/* bcdUSB */
	USB_DEVICE_CLASS_HUB,	/* bDeviceClass */
	0,			/* bDeviceSubClass */
	3,			/* bDeviceProtocol */
	64,			/* bMaxPacketSize0 */
	USBWORD(0),		/* idVendor */
	USBWORD(0),		/* idProduct */
	USBWORD(0x0100),	/* bcdDevice */
	1,			/* iManufacturer */
	2,			/* iProduct */
	0,			/* iSerialNumber */
	1			/* bNumConfigurations */
};

static usb_config_descr_t xhci_root_cfgdsc = {
	sizeof(usb_config_descr_t),	/* bLength */
	USB_CONFIGURATION_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USBWORD(sizeof(usb_config_descr_t) +
		sizeof(usb_interface_descr_t) +
		sizeof(usb_endpoint_descr_t)),	/* wTotalLength */
	1,			/* bNumInterfaces */
	1,			/* bConfigurationValue */
	0,			/* iConfiguration */
	USB_CONFIG_SELF_POWERED,	/* bmAttributes */
	0			/* MaxPower */
};

static usb_interface_descr_t xhci_root_ifdsc = {
	sizeof(usb_interface_descr_t),	/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,	/* bDescriptorType */
	0,			/* bInterfaceNumber */
	0,			/* bAlternateSetting */
	1,			/* bNumEndpoints */
	USB_INTERFACE_CLASS_HUB,	/* bInterfaceClass */
	0,			/* bInterfaceSubClass */
	0,			/* bInterfaceProtocol */
	0			/* iInterface */
};

static usb_endpoint_descr_t xhci_root_epdsc = {
	sizeof(usb_endpoint_descr_t),	/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType */
	(USB_ENDPOINT_DIRECTION_IN | 1),	/* bEndpointAddress */
	USB_ENDPOINT_TYPE_INTERRUPT,	/* bmAttributes */
	USBWORD(8),		/* wMaxPacketSize */
	9			/* bInterval...(i.e. 256 for USB 3.0 hub) */
};

static usb_hub_descr_t xhci_root_hubdsc = {
	USB_HUB_DESCR_SIZE,	/* bLength */
	USB_HUB_DESCRIPTOR_SS_TYPE,	/* bDescriptorType */
	0,			/* bNumberOfPorts */
	USBWORD(0),		/* wHubCharacteristics */
	0,			/* bPowreOnToPowerGood */
	0,			/* bHubControl Current */
	{0}			/* bRemoveAndPowerMask */
};

/*  *********************************************************************
    *  xhci_roothb_strdscr(ptr,str)
    *
    *  Construct a string descriptor for root hub requests
    *
    *  Input parameters:
    *	ptr - pointer to where to put descriptor
    *	str - regular string to put into descriptor
    *
    *  Return value:
    *	number of bytes written to descriptor
    ********************************************************************* */

static int xhci_roothub_strdscr(uint8_t *ptr, char *str)
{
	uint8_t *p = ptr;

	*p++ = strlen(str) * 2 + 2;	/* Unicode strings */
	*p++ = USB_STRING_DESCRIPTOR_TYPE;
	while (*str) {
		*p++ = *str++;
		*p++ = 0;
	}
	return (p - ptr);
}

/*  *********************************************************************
    *  xhci_hub_status(portstat)
    *
    *  Build 32-bit hub status from XHCI port status.
    *
    *  Input parameters:
    *	ps - XHCI port status
    *
    *  Return value:
    *	full hub status
    ********************************************************************* */

static int xhci_hub_status(uint32_t ps)
{
	uint32_t hubstat = 0;

	/* First assemble port state */
	hubstat = ps & M_PORTSC_STAT;

	/* Now assemble port status change */
	hubstat |= ((ps & M_PORTSC_CSC) >> 1) |
		   ((ps & M_PORTSC_WRC) << 2) |
		   ((ps & M_PORTSC_OCC) >> 1) |
		   (ps & M_PORTSC_PRC) |
		   (ps & M_PORTSC_PLC) |
		   (ps & M_PORTSC_CEC);

	return hubstat;
}

/*  *********************************************************************
    *  xhci_roothub_req(softc,req)
    *
    *  Handle a descriptor request on the control pipe for the
    *  root hub.  We pretend to be a real root hub here and
    *  return all the standard descriptors.
    *
    *  Input parameters:
    *	softc - our XHCI controller
    *	req - a usb request (completed immediately)
    *
    *  Return value:
    *	0 if ok
    *	else error code
    ********************************************************************* */

static int xhci_roothub_req(xhci_softc_t *softc, usb_device_request_t *req)
{
	uint8_t *ptr;
	uint16_t wLength;
	uint16_t wValue;
	uint16_t wIndex;
	usb_port_status_t ups;
	usb_hub_descr_t hdsc;
	uint32_t status;
	uint32_t statport;
	uint32_t tmpval;
	int res = 0;

	ptr = softc->rh_buf;

	wLength = GETUSBFIELD(req, wLength);
	wValue = GETUSBFIELD(req, wValue);
	wIndex = GETUSBFIELD(req, wIndex);

	if (xhcidebug & SHOW_RH_INFO3) {
		printf("XHCI RH-SETUP: bmRequestType %02x bRequest %02x ",
			req->bmRequestType, req->bRequest);
		printf("wIndex %04x wValue %04x wLength %04x\n",
			wIndex, wValue, wLength);
	}

	switch (REQSW(req->bRequest, req->bmRequestType)) {

	case REQCODE(USB_REQUEST_GET_STATUS,
		USBREQ_DIR_IN, USBREQ_TYPE_STD, USBREQ_REC_DEVICE):
		*ptr++ = (USB_GETSTATUS_SELF_POWERED & 0xFF);
		*ptr++ = (USB_GETSTATUS_SELF_POWERED >> 8);
		break;

	case REQCODE(USB_REQUEST_GET_STATUS,
		USBREQ_DIR_IN, USBREQ_TYPE_STD, USBREQ_REC_ENDPOINT):
	case REQCODE(USB_REQUEST_GET_STATUS,
		USBREQ_DIR_IN, USBREQ_TYPE_STD, USBREQ_REC_INTERFACE):
		*ptr++ = 0;
		*ptr++ = 0;
		break;

	case REQCODE(USB_REQUEST_GET_STATUS,
		USBREQ_DIR_IN, USBREQ_TYPE_CLASS, USBREQ_REC_OTHER):
		status = XHCI_READCSR(softc, (R_PORTSC(wIndex)));
		if (xhcidebug & SHOW_RH_INFO2) {
			printf("XHCI RHGetStatus: ");
			xhci_dumpportstat(wIndex, status);
		}
		status = xhci_hub_status(status);
		if (xhcidebug & SHOW_RH_INFO2)
			printf("XHCI endcoded port status: %08x\n", status);
		PUTUSBFIELD((&ups), wPortStatus, (status & 0xFFFF));
		PUTUSBFIELD((&ups), wPortChange, (status >> 16));
		memcpy(ptr, &ups, sizeof(ups));
		ptr += sizeof(ups);
		break;

	case REQCODE(USB_REQUEST_GET_STATUS,
		USBREQ_DIR_IN, USBREQ_TYPE_CLASS, USBREQ_REC_DEVICE):
		*ptr++ = 0;
		*ptr++ = 0;
		*ptr++ = 0;
		*ptr++ = 0;
		break;

	case REQCODE(USB_REQUEST_CLEAR_FEATURE,
		USBREQ_DIR_OUT, USBREQ_TYPE_STD, USBREQ_REC_DEVICE):
	case REQCODE(USB_REQUEST_CLEAR_FEATURE,
		USBREQ_DIR_OUT, USBREQ_TYPE_STD, USBREQ_REC_INTERFACE):
	case REQCODE(USB_REQUEST_CLEAR_FEATURE,
		USBREQ_DIR_OUT, USBREQ_TYPE_STD, USBREQ_REC_ENDPOINT):
		/* do nothing, not supported */
		break;

	case REQCODE(USB_REQUEST_CLEAR_FEATURE,
		USBREQ_DIR_OUT, USBREQ_TYPE_CLASS, USBREQ_REC_OTHER):
		statport = R_PORTSC(wIndex);
		status = XHCI_READCSR(softc, statport) & M_PORTSC_KEEP;
		if (xhcidebug & SHOW_RH_INFO2) {
			printf("XHCI RHClearFeature(%d): ", wValue);
			xhci_dumpportstat(wIndex,
					  XHCI_READCSR(softc, statport));
		}
		switch (wValue) {
		case USB_PORT_FEATURE_CONNECTION:
			break;
		case USB_PORT_FEATURE_ENABLE:
			XHCI_WRITECSR(softc, statport,
				      (status & ~M_PORTSC_PED));
			break;
		case USB_PORT_FEATURE_SUSPEND:
			XHCI_WRITECSR(softc, statport,
				      (status | M_PORTSC_LWS | K_PLS_U0));
			break;
		case USB_PORT_FEATURE_OVER_CURRENT:
			break;
		case USB_PORT_FEATURE_RESET:
			break;
		case USB_PORT_FEATURE_POWER:
			status &= ~M_PORTSC_PP;
			XHCI_WRITECSR(softc, statport, status);
			break;
		case USB_PORT_FEATURE_C_PORT_CONNECTION:
			XHCI_WRITECSR(softc, statport,
				      (status | M_PORTSC_CSC | M_PORTSC_PLC));
			break;
		case USB_PORT_FEATURE_C_PORT_ENABLE:
			break;
		case USB_PORT_FEATURE_C_PORT_SUSPEND:
			break;
		case USB_PORT_FEATURE_C_PORT_OVER_CURRENT:
			XHCI_WRITECSR(softc, statport,
				      (status | M_PORTSC_OCC));
			break;
		case USB_PORT_FEATURE_C_PORT_RESET:
			if (xhcidebug & SHOW_RH_INFO3)
				printf(" -- RESET\n");
			XHCI_WRITECSR(softc, statport,
				      (status | M_PORTSC_WRC | M_PORTSC_PRC));
			softc->rh_enum_port = wIndex;
			break;
		}

		/*
		 * If we've cleared all of the conditions that
		 * want our attention on the port status,
		 * then we can accept port status interrupts again.
		 */

		if ((wValue >= USB_PORT_FEATURE_C_PORT_CONNECTION) &&
		    (wValue <= USB_PORT_FEATURE_C_PORT_RESET)) {
			status = XHCI_READCSR(softc, statport);
			if ((status & M_PORTSC_ALLC) == 0) {
				if (xhcidebug & SHOW_RH_INFO3)
					printf(" -- PCD enabled\n");
				if (softc->block_pcd)
					--softc->block_pcd;
			}
		}
		break;

	case REQCODE(USB_REQUEST_SET_FEATURE,
		USBREQ_DIR_OUT, USBREQ_TYPE_STD, USBREQ_REC_DEVICE):
	case REQCODE(USB_REQUEST_SET_FEATURE,
		USBREQ_DIR_OUT, USBREQ_TYPE_STD, USBREQ_REC_INTERFACE):
	case REQCODE(USB_REQUEST_SET_FEATURE,
		USBREQ_DIR_OUT, USBREQ_TYPE_STD, USBREQ_REC_ENDPOINT):
		res =
		    -1;
		break;

	case REQCODE(USB_REQUEST_SET_FEATURE,
		USBREQ_DIR_OUT, USBREQ_TYPE_CLASS, USBREQ_REC_DEVICE):
		/* nothing */
		break;

	case REQCODE(USB_REQUEST_SET_FEATURE,
		USBREQ_DIR_OUT, USBREQ_TYPE_CLASS, USBREQ_REC_OTHER):
		statport = R_PORTSC(wIndex);
		status = XHCI_READCSR(softc, statport) & M_PORTSC_KEEP;
		if (xhcidebug & SHOW_RH_INFO2) {
			printf("XHCI RHSetFeature(%d): ", wValue);
			xhci_dumpportstat(wIndex,
					  XHCI_READCSR(softc, statport));
		}
		switch (wValue) {
		case USB_PORT_FEATURE_CONNECTION:
			break;
		case USB_PORT_FEATURE_ENABLE:
			break;
		case USB_PORT_FEATURE_SUSPEND:
			status &= M_PORTSC_KEEP;
			XHCI_WRITECSR(softc, statport,
				      (status | M_PORTSC_LWS | K_PLS_U3));
			break;
		case USB_PORT_FEATURE_OVER_CURRENT:
			break;
		case USB_PORT_FEATURE_RESET:
			if (xhcidebug & SHOW_RH_INFO3)
				printf(" -- RESET\n");
			XHCI_WRITECSR(softc, statport,
				      (status | M_PORTSC_PR));
			usb_delay_ms(softc->bus, 100);
			break;
		case USB_PORT_FEATURE_POWER:
			if (xhcidebug & SHOW_RH_INFO3)
				printf(" -- POWER\n");
			XHCI_WRITECSR(softc, statport, M_PORTSC_PP);
			break;
		case USB_PORT_FEATURE_LOW_SPEED:
			break;
		case USB_PORT_FEATURE_C_PORT_CONNECTION:
			break;
		case USB_PORT_FEATURE_C_PORT_ENABLE:
			break;
		case USB_PORT_FEATURE_C_PORT_SUSPEND:
			break;
		case USB_PORT_FEATURE_C_PORT_OVER_CURRENT:
			break;
		case USB_PORT_FEATURE_C_PORT_RESET:
			break;
		}
		break;

	case REQCODE(USB_REQUEST_SET_ADDRESS,
		USBREQ_DIR_OUT, USBREQ_TYPE_STD, USBREQ_REC_DEVICE):
		if (xhcidebug & SHOW_RH_INFO2)
			printf("XHCI RHSetAddr(%d)\n", wValue);
		softc->rh_newaddr = wValue;
		break;

	case REQCODE(USB_REQUEST_GET_DESCRIPTOR,
		USBREQ_DIR_IN, USBREQ_TYPE_STD, USBREQ_REC_DEVICE):
		switch (wValue >> 8) {
		case USB_DEVICE_DESCRIPTOR_TYPE:
			memcpy(ptr, &xhci_root_devdsc,
			       sizeof(xhci_root_devdsc));
			ptr += sizeof(xhci_root_devdsc);
			break;
		case USB_CONFIGURATION_DESCRIPTOR_TYPE:
			memcpy(ptr, &xhci_root_cfgdsc,
			       sizeof(xhci_root_cfgdsc));
			ptr += sizeof(xhci_root_cfgdsc);
			memcpy(ptr, &xhci_root_ifdsc,
				sizeof(xhci_root_ifdsc));
			ptr += sizeof(xhci_root_ifdsc);
			memcpy(ptr, &xhci_root_epdsc,
				sizeof(xhci_root_epdsc));
			ptr += sizeof(xhci_root_epdsc);
			break;
		case USB_STRING_DESCRIPTOR_TYPE:
			switch (wValue & 0xFF) {
			case 1:
				ptr += xhci_roothub_strdscr(ptr, "Generic");
				break;
			case 2:
				ptr += xhci_roothub_strdscr(ptr, "Root Hub");
				break;
			default:
				*ptr++ = 0;
				break;
			}
			break;
		default:
			res = -1;
			break;
		}
		break;

	case REQCODE(USB_REQUEST_GET_DESCRIPTOR,
		USBREQ_DIR_IN, USBREQ_TYPE_CLASS, USBREQ_REC_DEVICE):
		memcpy(&hdsc, &xhci_root_hubdsc, sizeof(hdsc));
		hdsc.bNumberOfPorts = softc->hc->ndp;
		tmpval = USB_HUBCHAR_PWR_IND;
		PUTUSBFIELD((&hdsc), wHubCharacteristics, tmpval);
		hdsc.bPowerOnToPowerGood = 10;	/* 20 mS...per spec */
		hdsc.bDescriptorLength = USB_HUB_DESCR_SIZE + 1;
		hdsc.bRemoveAndPowerMask[0] =
		    (softc->hc->ndp == 2 ? 0x60000 : 0x20000);
		memcpy(ptr, &hdsc, sizeof(hdsc));
		ptr += sizeof(hdsc);
		break;

	case REQCODE(USB_REQUEST_SET_DESCRIPTOR,
		USBREQ_DIR_OUT, USBREQ_TYPE_CLASS, USBREQ_REC_DEVICE):
		/* nothing */
		break;

	case REQCODE(USB_REQUEST_GET_CONFIGURATION,
		USBREQ_DIR_IN, USBREQ_TYPE_STD, USBREQ_REC_DEVICE):
		*ptr++ = softc->rh_conf;
		break;

	case REQCODE(USB_REQUEST_SET_CONFIGURATION,
		USBREQ_DIR_OUT, USBREQ_TYPE_STD, USBREQ_REC_DEVICE):
		softc->rh_conf = wValue;
		break;

	case REQCODE(USB_REQUEST_GET_INTERFACE,
		USBREQ_DIR_IN, USBREQ_TYPE_STD, USBREQ_REC_INTERFACE):
		*ptr++ = 0;
		break;

	case REQCODE(USB_REQUEST_SET_INTERFACE,
		USBREQ_DIR_OUT, USBREQ_TYPE_STD, USBREQ_REC_INTERFACE):
		/* nothing */
		break;

	case REQCODE(USB_REQUEST_SYNC_FRAME,
		USBREQ_DIR_OUT, USBREQ_TYPE_STD, USBREQ_REC_ENDPOINT):
		/* nothing */
		break;
	}

	softc->rh_ptr = softc->rh_buf;
	softc->rh_len = ptr - softc->rh_buf;

	return res;
}

/*  *********************************************************************
    *  xhci_roothub_statchg(softc)
    *
    *  This routine is called from the interrupt service routine
    *  (well, polling routine) for the xhci controller.  If the
    *  controller notices a root hub status change, it dequeues an
    *  interrupt transfer from the root hub's queue and completes
    *  it here.
    *
    *  Input parameters:
    *	softc - our XHCI controller
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void xhci_roothub_statchg(xhci_softc_t *softc)
{
	usbreq_t *ur;
	uint32_t status;
	uint8_t portstat = 0;
	int idx;

	/* Note: this only works up to 8 ports */
	for (idx = 1; idx <= softc->hc->ndp; idx++) {
		status = XHCI_READCSR(softc, R_PORTSC(idx));
		/* Process main change bits and clear others */
		XHCI_WRITECSR(softc, R_PORTSC(idx), (status & M_PORTSC_OTHER));
		if (status & M_PORTSC_CHECK)
			portstat = (1 << idx);
	}

	if (portstat != 0) {
		ur = (usbreq_t *)q_deqnext(&(softc->rh_intrq));
		if (!ur) {	/* no requests pending, postpone action */
			softc->pending_pcd = 1;
			return;
		}

		ur->ur_buffer[0] = portstat;
		ur->ur_xferred = 1;

		usb_complete_request(ur, 0);
		++softc->block_pcd;
	}
	softc->pending_pcd = 0;
}

/*  *********************************************************************
    *  xhci_roothub_xfer(softc,req)
    *
    *  Handle a root hub xfer - xhci_xfer transfers control here
    *  if we detect the address of the root hub - no actual transfers
    *  go out on the wire, we just handle the requests directly to
    *  make it look like a hub is attached.
    *
    *  This seems to be common practice in the USB world, so we do
    *  it here too.
    *
    *  Input parameters:
    *	softc - our EHCI controller structure
    *	req - usb request destined for host controller
    *
    *  Return value:
    *	0 if ok
    *	else error
    ********************************************************************* */

static int xhci_roothub_xfer(usbbus_t *bus, usb_ept_t *uept, usbreq_t *ur)
{
	xhci_softc_t *softc = (xhci_softc_t *) bus->ub_hwsoftc;
	endpoint_t *ept = (endpoint_t *) uept;
	int res;

	switch (ept->num) {

		/*
		 * CONTROL ENDPOINT
		 */
	case 0:

		/*
		 * Three types of transfers:  OUT (SETUP), IN (data), or STATUS.
		 * figure out which is which.
		 */

		if (ur->ur_flags & UR_FLAG_SETUP) {
			/*
			 * SETUP packet - this is an OUT request to the control
			 * pipe.  We emulate the hub request here.
			 */
			usb_device_request_t *req;

			req = (usb_device_request_t *) ur->ur_buffer;

			res = xhci_roothub_req(softc, req);
			if (res != 0)
				printf("XHCI: Root hub request error\n");

			ur->ur_xferred = ur->ur_length;
			ur->ur_status = 0;
			usb_complete_request(ur, 0);
		}

		else if (ur->ur_flags & UR_FLAG_STATUS_IN) {
			/*
			 * STATUS IN : it's sort of like a dummy IN request
			 * to acknowledge a SETUP packet that otherwise has no
			 * status.  Just complete the usbreq.
			 */

			if (softc->rh_newaddr != -1) {
				softc->rh_addr = softc->rh_newaddr;
				softc->rh_newaddr = -1;
			}

			ur->ur_status = 0;
			ur->ur_xferred = 0;
			usb_complete_request(ur, 0);
		}

		else if (ur->ur_flags & UR_FLAG_STATUS_OUT) {
			/*
			 * STATUS OUT : it's sort of like a dummy OUT request
			 */
			ur->ur_status = 0;
			ur->ur_xferred = 0;
			usb_complete_request(ur, 0);
		}

		else if (ur->ur_flags & UR_FLAG_IN) {
			/*
			 * IN : return data from the root hub
			 */
			int amtcopy;

			amtcopy = softc->rh_len;
			if (amtcopy > ur->ur_length)
				amtcopy = ur->ur_length;

			memcpy(ur->ur_buffer, softc->rh_ptr, amtcopy);

			softc->rh_ptr += amtcopy;
			softc->rh_len -= amtcopy;

			ur->ur_status = 0;
			ur->ur_xferred = amtcopy;
			usb_complete_request(ur, 0);
		}

		else {
			printf("XHCI: Unknown root hub transfer type\n");
			return -1;
		}
		break;

		/*
		 * INTERRUPT ENDPOINT
		 */

	case 1:		/* interrupt pipe */
		if (ur->ur_flags & UR_FLAG_IN)
			q_enqueue(&(softc->rh_intrq), (queue_t *) ur);
		break;

	}

	return 0;
}

