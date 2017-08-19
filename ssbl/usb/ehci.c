/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_printf.h"
#include "lib_string.h"
#include "lib_physio.h"
#include "common.h"
#include "error.h"

#include "lib_malloc.h"
#include "lib_queue.h"
#include "usbchap9.h"
#include "usbd.h"
#include "ehci.h"
#include "usb_externs.h"

#include "env_subr.h"

#define INTR_PIPE
#define EHCI_NUM_MIN_FREE_EP	(EHCI_QHPOOL_SIZE - 4)

/*  *********************************************************************
    *  Macros for dealing with hardware
    *
    *  This is all yucky stuff that needs to be made more
    *  processor-independent.  It's mostly here now to help us with
    *  our test harness.
    ********************************************************************* */

#if defined(_CFE_) && defined(__MIPSEB) && defined(CFG_PCI)

#define BSWAP32(x) __swap32(x)
static inline uint32_t __swap32(uint32_t x)
{
	uint32_t y;

	y = ((x & 0xFF) << 24) |
	    ((x & 0xFF00) << 8) |
	    ((x & 0xFF0000) >> 8) | ((x & 0xFF000000) >> 24);

	return y;
}
#else
#define BSWAP32(x) (x)
#endif

#define EHCI_VTOP(ptr) ((uint32_t)PHYSADDR((long)(ptr)))

#if CFG_CPU_COHERENT_DMA
#define EHCI_PTOV(ptr) ((void *)(CACADDR(ptr)))
#else
#define EHCI_PTOV(ptr) ((void *)(UNCADDR(ptr)))
#endif

#define EHCI_WRITECSR(softc, x, y) \
	DEV_WR((softc)->ehci_regs + (x), (y))

#define EHCI_READCSR(softc, x) \
	DEV_RD((softc)->ehci_regs + (x))

#define NULL_INTR_QH BSWAP32(V_EHCI_QH_TYP(QH) + M_EHCI_QH_T)


/*  *********************************************************************
    *  Macros to convert from "hardware" endpoint and transfer
    *  descriptors (ehci_qh_t, ehci_qtd_t) to "software"
    *  data structures (ehci_transfer_t, ehci_endpoint_t).
    *
    *  Basically, there are two tables, indexed by the same value
    *  By subtracting the base of one pool from a pointer, we get
    *  the index into the other table.
    *
    *  We *could* have included the qh and qtd in the software
    *  data structures, but placing all the hardware stuff in one
    *  pool will make it easier for hardware that does not handle
    *  coherent DMA, since we can be less careful about what we flush
    *  and what we invalidate.
    ********************************************************************* */

#define ehci_qtd_from_transfer(softc, transfer) \
	((softc)->ehci_hwqtdpool + ((transfer) - (softc)->ehci_transfer_pool))

#define ehci_transfer_from_qtd(softc, qtd) \
	((softc)->ehci_transfer_pool + ((qtd) - (softc)->ehci_hwqtdpool))

#define ehci_qh_from_endpoint(softc, endpoint) \
	((softc)->ehci_hwqhpool + ((endpoint) - (softc)->ehci_endpoint_pool))

#define ehci_endpoint_from_qh(softc, qh) \
	((softc)->ehci_endpoint_pool + ((qh) - (softc)->ehci_hwqhpool))

/*  *********************************************************************
    *  Forward declarations
    ********************************************************************* */

static int ehci_roothub_xfer(usbbus_t *bus, usb_ept_t *uept, usbreq_t *ur);
static void ehci_roothub_statchg(ehci_softc_t *softc);

/*  *********************************************************************
    *  Globals
    ********************************************************************* */

int ehcidebug = 0;

/*  *********************************************************************
    *  Some debug routines
    ********************************************************************* */

void ehci_dumpportstat(int idx, uint32_t reg)
{
	printf("EHCI Port %d: %08X\n", idx, reg);
}

void ehci_dumpqtd(ehci_qtd_t *qtd)
{
	int i;
	uint32_t *p = (uint32_t *) qtd;

	printf("EHCI QTD @%p:", p);
	for (i = 0; i < 8; ++i)
		printf(" %08x", BSWAP32(p[i]));
	printf("\n");
}

void ehci_dumpqh(ehci_qh_t *qh)
{
	int i;
	uint32_t *p = (uint32_t *) qh;

	printf("EHCI QH @%p:", p);
	for (i = 0; i < 12; ++i)
		printf(" %08x", BSWAP32(p[i]));
	printf("\n");
}

#if 0
void ehci_dump_async(ehci_softc_t *softc)
{
	int active;
	ehci_endpoint_t *async = softc->ehci_async_list;
	ehci_qh_t *qh = ehci_qh_from_endpoint(softc, async);
	ehci_qtd_t *qtd;
	uint32_t pqh, pqtd, pqtd0;

	printf("\nEHCI Async list:\n");
	do {
		ehci_dumpqh(qh);
		pqtd = BSWAP32(qh->qh_curr_qtd) & M_EHCI_QH_PTR;
		active = BSWAP32(qh->qh_qtdctl) & 0x80;
		if (!pqtd || !active)
			pqtd = BSWAP32(qh->qh_next_qtd) & M_EHCI_QH_PTR;
		pqtd0 = pqtd;
		while (pqtd) {
			qtd = (ehci_qtd_t *) EHCI_PTOV(pqtd);
			ehci_dumpqtd(qtd);
			pqtd = BSWAP32(qtd->qtd_next) & M_EHCI_QTD_PTR;
			if (pqtd == pqtd0)
				pqtd = 0;
		}
		pqh = BSWAP32(qh->qh_next) & M_EHCI_QH_PTR;
		qh = (ehci_qh_t *) EHCI_PTOV(pqh);
	} while (pqh && (pqh != async->ep_phys));
}
#endif

static int eptused(ehci_softc_t *softc)
{
	int cnt;
	ehci_endpoint_t *e;

	cnt = 0;
	e = softc->ehci_endpoint_freelist;
	while (e) {
		e = e->ep_next;
		cnt++;
	}
	return cnt;
}

static void eptstats(ehci_softc_t *softc)
{
	int cnt;

	cnt = eptused(softc);
	printf("EHCI %d left, %d inuse\n", cnt, (EHCI_QHPOOL_SIZE - cnt));
}

static void xferstats(ehci_softc_t *softc)
{
	int cnt;
	ehci_transfer_t *t;

	cnt = 0;
	t = softc->ehci_transfer_freelist;
	while (t) {
		t = t->t_next;
		cnt++;
	}
	printf("EHCI %d left, %d inuse\n", cnt, (EHCI_QTDPOOL_SIZE - cnt));
}

/*  *********************************************************************
    *  _ehci_allocept(softc)
    *
    *  Allocate a QH endpoint data structure from the pool, and
    *  make it ready for use.  The endpoint is NOT attached to
    *  the hardware at this time. The QH points to itself.
    *
    *  Input parameters:
    *	softc - our EHCI controller
    *
    *  Return value:
    *	pointer to endpoint or NULL
    ********************************************************************* */

static ehci_endpoint_t *_ehci_allocept(ehci_softc_t *softc)
{
	ehci_endpoint_t *e;
	ehci_qh_t *qh;

	e = softc->ehci_endpoint_freelist;
	if (!e) {
		printf("EHCI No endpoints left!\n");
		return NULL;
	}

	softc->ehci_endpoint_freelist = e->ep_next;
	qh = ehci_qh_from_endpoint(softc, e);
	memset((void *)qh, 0, sizeof(ehci_qh_t));

	/* points to itself, QH type only */
	qh->qh_next = BSWAP32(EHCI_VTOP(qh) + V_EHCI_QH_TYP(QH));

	qh->qh_next_qtd = BSWAP32(M_EHCI_QH_T);
	qh->qh_alt_qtd = BSWAP32(M_EHCI_QH_T);

	e->ep_phys = EHCI_VTOP(qh);
	e->ep_next = NULL;
	e->qtd_list = NULL;

	if (ehcidebug & SHOW_EPT_ALLOC)
		printf("EHCI AllocEpt: ept %p  qh %p\n", e, qh);

	if (ehcidebug & SHOW_EPT_QH) {
		printf("EHCI AllocEpt: ");
		ehci_dumpqh(qh);
	}

	++softc->eptcnt;

	if (ehcidebug & SHOW_EPT_STATS) {
		printf("EHCI AllocEpt: ");
		eptstats(softc);
	}

	return e;
}

/*  *********************************************************************
    *  _ehci_allocxfer(softc)
    *
    *  Allocate a transfer descriptor.  It is prepared for use
    *  but not attached to the hardware.
    *
    *  Input parameters:
    *	softc - our EHCI controller
    *
    *  Return value:
    *	transfer descriptor, or NULL
    ********************************************************************* */

static ehci_transfer_t *_ehci_allocxfer(ehci_softc_t *softc)
{
	ehci_transfer_t *t;
	ehci_qtd_t *qtd;

	t = softc->ehci_transfer_freelist;
	if (!t) {
		printf("EHCI No more transfer descriptors!\n");
		return NULL;
	}

	softc->ehci_transfer_freelist = t->t_next;
	qtd = ehci_qtd_from_transfer(softc, t);
	memset((void *)qtd, 0, sizeof(ehci_qtd_t));
	qtd->qtd_next = BSWAP32(M_EHCI_QTD_T);
	qtd->qtd_alt = BSWAP32(M_EHCI_QTD_T);

	t->t_ref = NULL;
	t->t_next = NULL;
	if (ehcidebug & SHOW_XFER_ALLOC)
		printf("EHCI AllocXfer: qtd %p xfer %p\n", qtd, t);
	if (ehcidebug & SHOW_XFER_QTD) {
		printf("EHCI AllocXfer: ");
		ehci_dumpqtd(qtd);
	}
	++softc->xfercnt;
	if (ehcidebug & SHOW_XFER_STATS) {
		printf("EHCI AllocXfer: ");
		xferstats(softc);
	}

	return t;
}

/*  *********************************************************************
    *  _ehci_freeept(softc,e)
    *
    *  Free an endpoint, returning it to the pool.
    *
    *  Input parameters:
    *	softc - our EHCI controller
    *	e - endpoint descriptor to return
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void _ehci_freeept(ehci_softc_t *softc, ehci_endpoint_t *e)
{
	e->ep_next = softc->ehci_endpoint_freelist;
	softc->ehci_endpoint_freelist = e;
	--softc->eptcnt;
	if (softc->ehci_ndp) {	/* not during init */
		if (ehcidebug & SHOW_EPT_STATS) {
			printf("EHCI FreeEpt[%p]: ", e);
			eptstats(softc);
		}
	}
}

/*  *********************************************************************
    *  _ehci_freexfer(softc,t)
    *
    *  Free a transfer descriptor, returning it to the pool.
    *
    *  Input parameters:
    *	softc - our EHCI controller
    *	t - transfer descriptor to return
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void _ehci_freexfer(ehci_softc_t *softc, ehci_transfer_t *t)
{
	t->t_next = softc->ehci_transfer_freelist;
	softc->ehci_transfer_freelist = t;
	--softc->xfercnt;
	if (softc->ehci_ndp) {
		if (ehcidebug & SHOW_XFER_STATS) {
			printf("EHCI FreeXfer:  qtd %p xfer %p; ",
			       ehci_qtd_from_transfer(softc, t), t);
			xferstats(softc);
		}
	}
}

/*  *********************************************************************
    *  _ehci_initpools(softc)
    *
    *  Allocate and initialize the various pools of things that
    *  we use in the EHCI driver.  We do this by allocating some
    *  big chunks from the heap and carving them up.
    *
    *  Input parameters:
    *	softc - our EHCI controller
    *
    *  Return value:
    *	0 if ok
    *	else error code
    ********************************************************************* */

static int _ehci_initpools(ehci_softc_t *softc)
{
	int idx;

	/*
	 * Do the transfer descriptor pool
	 */

	softc->ehci_transfer_pool =
	    KUMALLOC((EHCI_QTDPOOL_SIZE * sizeof(ehci_transfer_t)),
		     DMA_BUF_ALIGN);
	softc->ehci_hwqtdpool =
	    KUMALLOC((EHCI_QTDPOOL_SIZE * sizeof(ehci_qtd_t)), DMA_BUF_ALIGN);

	/*
	 * In the case of noncoherent DMA, make these uncached addresses.
	 * This way all our descriptors will be uncached. Makes life easier,
	 * as we do not need to worry about flushing descriptors, etc.
	 */

	if (!softc->ehci_transfer_pool || !softc->ehci_hwqtdpool) {
		printf("EHCI Could not allocate transfer descriptors\n");
		return -1;
	}

	softc->ehci_transfer_freelist = NULL;

	for (idx = 0; idx < EHCI_QTDPOOL_SIZE; idx++)
		_ehci_freexfer(softc, softc->ehci_transfer_pool + idx);

	if (ehcidebug & SHOW_XFER_STATS) {
		printf("EHCI InitXfer: ");
		xferstats(softc);
	}

	/*
	 * Do the endpoint descriptor pool and
	 * make all endpoint descriptors available
	 */

	softc->ehci_endpoint_pool =
	    KUMALLOC(EHCI_QHPOOL_SIZE * sizeof(ehci_endpoint_t), DMA_BUF_ALIGN);
	softc->ehci_hwqhpool =
	    KUMALLOC(EHCI_QHPOOL_SIZE * sizeof(ehci_qh_t), DMA_BUF_ALIGN);

	if (!softc->ehci_endpoint_pool || !softc->ehci_hwqhpool) {
		printf("EHCI Could not allocate transfer descriptors\n");
		return -1;
	}

	softc->ehci_endpoint_freelist = NULL;

	for (idx = 0; idx < EHCI_QHPOOL_SIZE; idx++)
		_ehci_freeept(softc, (softc->ehci_endpoint_pool + idx));

	if (ehcidebug & SHOW_EPT_STATS) {
		printf("EHCI InitEpt: ");
		eptstats(softc);
	}
#ifdef INTR_PIPE
	/*
	 * Do the interrupt table
	 */

	softc->ehci_intr_table =
	    KUMALLOC((EHCI_INTR_TABLE_SIZE * sizeof(uint32_t)), 4096);

	if (!softc->ehci_intr_table) {
		printf("EHCI Could not allocate interrupt table\n");
		return -1;
	}
#endif

	return 0;
}

/*  *********************************************************************
    *  ehci_start(bus)
    *
    *  Start the EHCI controller.  After this routine is called,
    *  the hardware will be operational and ready to accept
    *  descriptors and interrupt calls.
    *
    *  Input parameters:
    *	bus - bus structure, from ehci_create
    *
    *  Return value:
    *	0 if ok
    *	else error code
    ********************************************************************* */

static int ehci_start(usbbus_t *bus)
{
	ehci_softc_t *softc = (ehci_softc_t *) bus->ub_hwsoftc;
	uint32_t reg;
	int idx;

	/*
	 * Halt the controller, if necessary
	 */

	if (!(EHCI_READCSR(softc, R_EHCI_USBSTS) & M_EHCI_USBSTS_HCH)) {
		EHCI_WRITECSR(softc, R_EHCI_USBCMD, 0);
		/* HC supposed halt in 16 uFrames = 2mS */
		usb_delay_ms(bus, 5);
	}

	/*
	 * Reset the host controller.
	 */

	reg = EHCI_READCSR(softc, R_EHCI_USBCMD) | M_EHCI_USBCMD_HCR;
	EHCI_WRITECSR(softc, R_EHCI_USBCMD, reg);
	for (idx = 0; idx < 5; idx++) {
		if (!(EHCI_READCSR(softc, R_EHCI_USBCMD) & M_EHCI_USBCMD_HCR))
			break;
		usb_delay_ms(bus, 1);
	}

	if (EHCI_READCSR(softc, R_EHCI_USBCMD) & M_EHCI_USBCMD_HCR) {
		/* controller never came out of reset */
		return -1;
	}

	/*
	 * Set up async and periodic lists.
	 */

	EHCI_WRITECSR(softc, R_EHCI_ASYNCBASE, softc->ehci_async_list->ep_phys);
	if (ehcidebug & SHOW_INIT_STATS)
		printf("EHCI Start: async: base %08x\n",
		       EHCI_READCSR(softc, R_EHCI_ASYNCBASE));
	EHCI_WRITECSR(softc, R_EHCI_PERBASE, EHCI_VTOP(softc->ehci_intr_table));
	if (ehcidebug & SHOW_INIT_STATS)
		printf("EHCI Start: per: base %08x\n",
		       EHCI_READCSR(softc, R_EHCI_PERBASE));

	/*
	 * Our driver is polled, turn off interrupts, clear status
	 */

	EHCI_WRITECSR(softc, R_EHCI_USBINTR, 0);
	EHCI_WRITECSR(softc, R_EHCI_USBSTS, M_EHCI_INT_ALL);

	/*
	 * Set up to run with 256 element frame list and configured.
	 */

	reg =
	    EHCI_READCSR(softc,
			 R_EHCI_USBCMD) | M_EHCI_USBCMD_RUN |
	    V_EHCI_USBCMD_FLS(2);
	EHCI_WRITECSR(softc, R_EHCI_USBCMD, reg);
	EHCI_WRITECSR(softc, R_EHCI_CONFIG, 1);

	/*
	 * Remember how many ports we have
	 */

	reg = EHCI_READCSR(softc, R_EHCI_HCSPARAMS);
	softc->ehci_ndp = G_EHCI_HCSPARAMS_NDP(reg);

	/*
	 * Enable port power
	 */

	for (idx = 1; idx <= softc->ehci_ndp; idx++)
		EHCI_WRITECSR(softc, R_EHCI_PORTSC(idx), M_EHCI_PORTSC_PP);
	usb_delay_ms(softc->ehci_bus, 100);

	return 0;
}

/*  *********************************************************************
    *  _ehci_setupepts(softc)
    *
    *  Set up the endpoint tree.
    *  Basically the hardware knows how to scan lists of lists,
    *  so we build a tree where each level is pointed to by two
    *  parent nodes.  We can choose our scanning rate by attaching
    *  endpoints anywhere within this tree.
    *
    *  Interrupt pipe is handled in a very simplistic way since this is
    *  not a full fledged OS. Each interrupt QH is entered only in the
    *  interrupt table.  i.e. no tree. This is more than sufficient to
    *  handle hubs which is the primary target device.
    *
    *  Input parameters:
    *	softc - our EHCI controller
    *
    *  Return value:
    *	0 if ok
    *	else error (out of descriptors)
    ********************************************************************* */

static int _ehci_setupepts(ehci_softc_t *softc)
{
	ehci_endpoint_t *e;
#ifdef INTR_PIPE
	int idx;
#endif

	/*
	 * Set up the initial list head for the async transfer lists.
	 * Subsequent QHs will be appended to the head.
	 *
	 */

	e = _ehci_allocept(softc);
	softc->ehci_async_list = e;

#ifdef INTR_PIPE
	/* Fill h/w table with null entries and create list
	with placeholder endpoint */
	for (idx = 0; idx < EHCI_INTR_TABLE_SIZE; idx++)
		softc->ehci_intr_table[idx] = NULL_INTR_QH;

	e = _ehci_allocept(softc);
	softc->ehci_periodic_list = e;
#endif

	return 0;
}

/*  *********************************************************************
    *  ehci_stop(bus)
    *
    *  Stop the EHCI hardware.
    *
    *  Input parameters:
    *	bus - our bus structure
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void ehci_stop(usbbus_t *bus)
{
	ehci_softc_t *softc = (ehci_softc_t *) bus->ub_hwsoftc;

	EHCI_WRITECSR(softc, R_EHCI_USBCMD, 0);
}

/*  *********************************************************************
    *  _ehci_queueept(softc,queue,e)
    *
    *  Append an endpoint to a list of endpoints.
    *
    *  Input parameters:
    *	queue - endpoint descriptor for head of queue (which is dummy)
    *	e - endpoint to append to queue
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void _ehci_queueept(ehci_softc_t *softc, ehci_endpoint_t *queue,
			   ehci_endpoint_t *newept)
{
	ehci_qh_t *head;
	ehci_qh_t *newqh;
	uint32_t head_next;

	head = ehci_qh_from_endpoint(softc, queue);
	newqh = ehci_qh_from_endpoint(softc, newept);

	head_next = newqh->qh_next;	/* get ptr + type before overwriting */
	newept->ep_next = queue->ep_next;
	newqh->qh_next = head->qh_next;

	queue->ep_next = newept;
	head->qh_next = head_next;

	if (ehcidebug & SHOW_EPT_QUEUE)
		ehci_dumpqh(newqh);
}

/*  *********************************************************************
    *  _ehci_deqept(queue,e)
    *
    *  Find and remove an endpoint from the list of endpoints.
    *
    *  Input parameters:
    *	queue - base of queue to look for endpoint on
    *	e - endpoint to remove
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void _ehci_deqept(ehci_softc_t *softc, ehci_endpoint_t *queue,
			 ehci_endpoint_t *e)
{
	ehci_endpoint_t *cur;
	ehci_qh_t *curqh;
	ehci_qh_t *qh;

	cur = queue;
	while (cur && (cur->ep_next != e))
		cur = cur->ep_next;

	if (cur == NULL) {
		printf("EHCI Could not remove EP %08X: not on the list!\n",
		       (uint32_t) (intptr_t) e);
		return;
	}

	/*
	 * Remove from our regular list
	 */

	cur->ep_next = e->ep_next;

	/*
	 * now remove from the hardware's list
	 */

	curqh = ehci_qh_from_endpoint(softc, cur);
	qh = ehci_qh_from_endpoint(softc, e);

	curqh->qh_next = qh->qh_next;
}

/*  *********************************************************************
    *  _ehci_cancel_xfer(bus,ept)
    *
    *  Cancels a transfer, typically due to endpoint deletion,
    *  by completing pending transfers for the endpoint.
    *
    *  Input parameters:
    *	bus - ehci bus structure
    *	ept - endpoint to remove
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void _ehci_cancel_xfer(usbbus_t *bus, usb_ept_t *uept)
{
	ehci_softc_t *softc = (ehci_softc_t *) bus->ub_hwsoftc;
	ehci_endpoint_t *ept = (ehci_endpoint_t *) uept;
	usbreq_t *ur;
	ehci_qtd_t *qtd;
	ehci_transfer_t *transfer;

	qtd = ept->qtd_list;
	while (1) {
		transfer = ehci_transfer_from_qtd(softc, qtd);

		if (BSWAP32(EHCI_VTOP(qtd)) & M_EHCI_QTD_T)
			break;

		ur = transfer->t_ref;

		if (ehcidebug & SHOW_EPT_FREE) {
			printf("EHCI EptDelete: ");
			printf("qtd %p xfer %p\n", qtd, transfer);
		}

		if (ur) {
			ur->ur_status = K_EHCI_TD_CANCELLED;
			ur->ur_tdcount--;

			if (ur->ur_tdcount == 0) {

				if (ehcidebug & SHOW_EPT_FREE) {
					printf("EHCI EptDelete: Completing ");
					printf("request due to closed pipe:");
					printf(" %p\n", ur);
				}

				usb_complete_request(ur, K_EHCI_TD_CANCELLED);
				/* NOTE: It is expected that the callee will
				free the usbreq. */
			}
		}
		_ehci_freexfer(softc, transfer);
		qtd = (ehci_qtd_t *) EHCI_PTOV(BSWAP32(qtd->qtd_next));
	}
}

/*  *********************************************************************
    *  _ehci_async_ept_delete(bus,ept)
    *
    *  Deletes an async endpoint from the EHCI controller.  This
    *  routine also completes pending transfers for the
    *  endpoint and gets rid of the hardware ept (queue base).
    *
    *  Input parameters:
    *	bus - ehci bus structure
    *	ept - endpoint to remove
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void _ehci_async_ept_delete(usbbus_t *bus, usb_ept_t *uept)
{
	ehci_softc_t *softc = (ehci_softc_t *) bus->ub_hwsoftc;
	ehci_endpoint_t *ept = (ehci_endpoint_t *) uept;
	uint32_t reg;

	/* Stop the async list */
	reg = EHCI_READCSR(softc, R_EHCI_USBCMD) & ~M_EHCI_USBCMD_ASE;
	EHCI_WRITECSR(softc, R_EHCI_USBCMD, reg);

	/* mark this EP to avoid dual processing in transfer done processing */
	ept->ep_num = -1;

	/*
	 * Wait for atleast two SOFs (16 uSOFs) to guarantee async list
	 * has stopped.  Check anyways!
	 */

	usb_delay_ms(softc->ehci_bus, 8);
	if (EHCI_READCSR(softc, R_EHCI_USBSTS) & M_EHCI_USBSTS_ASS) {
		printf("EHCI ERROR: ");
		printf("Async list did not stop (EHCI status %08x)\n", reg);
	}

	/* Remove endpoint from queue and complete transfers
	and return endpoint to free pool */

	_ehci_deqept(softc, softc->ehci_async_list, ept);
	_ehci_cancel_xfer(bus, uept);
	_ehci_freeept(softc, ept);

	/* Restart the async list */
	reg = EHCI_READCSR(softc, R_EHCI_USBCMD) | M_EHCI_USBCMD_ASE;
	EHCI_WRITECSR(softc, R_EHCI_USBCMD, reg);
}

#ifdef INTR_PIPE
/*  *********************************************************************
    *  _ehci_periodic_ept_delete(bus,ept)
    *
    *  Deletes an periodic endpoint from the EHCI controller.  This
    *  routine also completes pending transfers for the
    *  endpoint and gets rid of the hardware ept (queue base).
    *
    *  Input parameters:
    *	bus - ehci bus structure
    *	ept - endpoint to remove
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void _ehci_periodic_ept_delete(usbbus_t *bus, usb_ept_t *uept)
{
	ehci_softc_t *softc = (ehci_softc_t *) bus->ub_hwsoftc;
	ehci_endpoint_t *ept = (ehci_endpoint_t *) uept;
	uint32_t reg;
	int idx, found = 0;

	/* Stop the async list */
	reg = EHCI_READCSR(softc, R_EHCI_USBCMD) & ~M_EHCI_USBCMD_PSE;
	EHCI_WRITECSR(softc, R_EHCI_USBCMD, reg);

	/* mark this EP to avoid dual processing in transfer done processing */
	ept->ep_num = -1;

	/*
	 * Wait for atleast two SOFs (16 uSOFs) to guarantee periodic list
	 * has stopped.  Check anyways!
	 */

	usb_delay_ms(softc->ehci_bus, 8);
	if (EHCI_READCSR(softc, R_EHCI_USBSTS) & M_EHCI_USBSTS_PSS) {
		printf("EHCI ERROR: ");
		printf("Periodic list did not stop (EHCI status %08x)\n", reg);
	}

	/*
	 * Remove endpoint from periodic table
	 */
	for (idx = 0; idx < EHCI_INTR_TABLE_SIZE; idx++) {
		if (G_EHCI_QH_PTR(softc->ehci_intr_table[idx]) ==
			ept->ep_phys) {
			softc->ehci_intr_table[idx] = NULL_INTR_QH;
			found = 1;
		}
	}
	if (!found)
		printf
		    ("EHCI Could not find periodic endpoint (%08x %08x %08x)\n",
		     ept->ep_phys, softc->ehci_intr_table[0],
		     G_EHCI_QH_PTR(softc->ehci_intr_table[0]));

	/* Remove endpoint from queue and complete transfers
	and return endpoint to free pool */

	_ehci_deqept(softc, softc->ehci_periodic_list, ept);
	_ehci_cancel_xfer(bus, uept);
	_ehci_freeept(softc, ept);

	/* Restart the periodic list */
	reg = EHCI_READCSR(softc, R_EHCI_USBCMD) | M_EHCI_USBCMD_PSE;
	EHCI_WRITECSR(softc, R_EHCI_USBCMD, reg);
}
#endif

/*  *********************************************************************
    *  ehci_ept_xferdone(softc)
    *
    *  Process the completion of a transfer for an endpoint.
    *  The completed transfer's list should be intact such that
    *  we can examine the results.
    *
    *  Input parameters:
    *	softc - our EHCI controller
    *	ept - endpoint to scan
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void ehci_ept_xferdone(ehci_softc_t *softc, ehci_endpoint_t *ept)
{
	ehci_qh_t *qh;
	ehci_transfer_t *transfer;
	ehci_qtd_t *qtd;
	usbreq_t *ur;
	int val;
	uint32_t control;

	/*
	 *  Scan through this endpoint's QTD lists for completed transfers
	 */

	while (1) {
		qh = ehci_qh_from_endpoint(softc, ept);	/* next QH */
		qtd = ept->qtd_list;
		while (1) {	/* process next QTD in current QH */

			/* - not this QH...still active */
			if (BSWAP32(qh->qh_qtdctl) & M_EHCI_STATUS_ACTIVE)
				break;

			/* - end of list? */
			if (!qtd || BSWAP32(EHCI_VTOP(qtd)) & M_EHCI_QTD_T)
				break;

			transfer = ehci_transfer_from_qtd(softc, qtd);
			ur = transfer->t_ref;
			control = BSWAP32(qtd->qtd_control);

			if (!control) /* dummy qtd */
				break;

			/* still active...likely a queued IN request */
			if (control & M_EHCI_STATUS_ACTIVE)
				break;	/*  allow list to finish */

			val = control & M_EHCI_STATUS_ERR;
			if (val) {

				/* set during device disconnect processing */
				if (ept->ep_num == -1)
					val = K_EHCI_TD_CANCELLED;
				else if (((val != M_EHCI_STATUS_HALTED) ||
					  (ept->ep_num == 0)) &&
					 (ehcidebug & SHOW_XFER_ERR)) {
						/* show stalls only on EP0 */
					printf("EHCI Transfer error: 0x%02x %p %d\n",
						val, ur, ept->ep_num);
					val = BOLT_ERR_IOERR;
				}
			}

			if (ehcidebug & SHOW_DONE_QTD) {
				printf("EHCI XferDone(%d): ",
						ur ? ur->ur_tdcount : -1);
				ehci_dumpqtd(qtd);
			}

			/* See if it's time to call the callback */
			if (ur) {
				ur->ur_status = val;
				ur->ur_tdcount--;

				/* update USB total request length
				with this QTD's transfer */
				ur->ur_xferred += transfer->t_length -
					BSWAP32(G_EHCI_QTD_LEN(control));

				if (ehcidebug & SHOW_DONE_XFER1)
					printf("EHCI XferDone: %d bytes\n",
					       ur->ur_xferred);

				/* check if all transfers are done */
				if (ur->ur_tdcount == 0) {
					/* Noncoherent DMA: need to invalidate,
					since data is in phys mem */
					if (ur->ur_xferred)
						CACHE_INVAL_RANGE(ur->ur_buffer,
								  ur->
								  ur_xferred);
					usb_complete_request(ur, val);
					if (ehcidebug & SHOW_DONE_XFER)
						printf("EHCI XferDone: %d bytes\n",
						       ur->ur_xferred);
				}
			}

			/* Advance to the next request before
			freeing this one */
			qtd = (ehci_qtd_t *) EHCI_PTOV(BSWAP32(qtd->qtd_next));

			/* Unlink the processed QTD from
			the endpoint's QTD list */
			ept->qtd_list = (ehci_qtd_t *) qtd;

			/* Free up the request */
			_ehci_freexfer(softc, transfer);
		}

		/* Advance to next QH and stop if end of list */
		ept = ept->ep_next;
		if (!ept)
			break;
	}
	if (ehcidebug & SHOW_DONE)
		printf("EHCI XferDone: ept done\n\n");
}

/*  *********************************************************************
    *  ehci_intr_xferdone(softc)
    *
    *  Process the completion of a transfer for this ehci controller.
    *
    *  Input parameters:
    *	softc - our EHCI controller
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void ehci_intr_xferdone(ehci_softc_t *softc)
{
	/* Scan through the async and periodic endpoints
	for completed transfers */

	ehci_ept_xferdone(softc, softc->ehci_async_list);
#ifdef INTR_PIPE
	ehci_ept_xferdone(softc, softc->ehci_periodic_list);
#endif

	if (ehcidebug & SHOW_DONE)
		printf("EHCI XferDone: exit\n\n");
}

/*  *********************************************************************
    *  ehci_intr(bus)
    *
    *  Process pending interrupts for the EHCI controller.
    *
    *  Input parameters:
    *	bus - our bus structure
    *
    *  Return value:
    *	0 if we did nothing
    *	nonzero if we did something.
    ********************************************************************* */

static int ehci_intr(usbbus_t *bus)
{
	uint32_t reg;
	ehci_softc_t *softc = (ehci_softc_t *) bus->ub_hwsoftc;

	/*
	 * Read the interrupt status register.
	 */

	reg = EHCI_READCSR(softc, R_EHCI_USBSTS) & M_EHCI_INT_ALL;

	/*
	 * Don't bother doing anything if nothing happened.
	 */
	if (!softc->pending_psc && (reg == 0))
		return 0;

	/*
	 * Data transfer done or error...initiate async doorbell sequence to
	 * ensure that QTDs are updated by h/w before processing them.
	 */
	if (reg & (M_EHCI_USBSTS_INT | M_EHCI_USBSTS_ERR)) {
		EHCI_WRITECSR(softc, R_EHCI_USBCMD,
			      (EHCI_READCSR(softc, R_EHCI_USBCMD) |
			       M_EHCI_USBCMD_IAAD));
		if ((ehcidebug & SHOW_GEN_DBG) && (reg & M_EHCI_USBSTS_ERR))
			printf("EHCI ERRINT\n");
	}

	/* Root Hub Status Change */
	if (!(softc->ehci_intdisable & M_EHCI_USBSTS_PSC) &&
		(softc->pending_psc || (reg & M_EHCI_USBSTS_PSC))) {
		uint32_t lreg;

		if (ehcidebug & SHOW_RH_INFO1) {
			/* show only 1st port */
			lreg = EHCI_READCSR(softc, R_EHCI_PORTSC(1));
			printf("EHCI RootHubStatusChange: %08x\n", lreg);
		}
		ehci_roothub_statchg(softc);
	}

	/* Frame list rollover */
	if (reg & M_EHCI_USBSTS_FLR) {
		/* printf("EHCI Frame list rollover\n"); */
		/* don't be noisy about this */
	}

	/* System error...memory bus access */
	if (reg & M_EHCI_USBSTS_HSE)
		printf("EHCI Host System ERROR\n");

	/* IAA...time to process completed QTDs */
	if (reg & M_EHCI_USBSTS_IAA)
		ehci_intr_xferdone(softc);

	/*
	 * Write the value back to the interrupt
	 * register to clear the bits that were set.
	 */

	EHCI_WRITECSR(softc, R_EHCI_USBSTS, reg);

	return 1;
}

/*  *********************************************************************
    *  ehci_delete(bus)
    *
    *  Remove an EHCI bus structure and all resources allocated to
    *  it (used when shutting down USB)
    *
    *  Input parameters:
    *	bus - our USB bus structure
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void ehci_delete(usbbus_t *bus)
{
	/* xxx fill in later. */
}

/*  *********************************************************************
    *  ehci_create(addr)
    *
    *  Create a USB bus structure and associate it with our EHCI
    *  controller device.
    *
    *  Input parameters:
    *	addr - physical address of controller
    *
    *  Return value:
    *	usbbus structure pointer
    ********************************************************************* */

static usbbus_t *ehci_create(physaddr_t addr)
{
	int res;
	ehci_softc_t *softc;
	usbbus_t *bus;

	ehcidebug = env_getval("EHCIDBG");
	if (ehcidebug < 0)
		ehcidebug = 0;
	else
		printf("*** ehcidebug: %x\n", ehcidebug);

	softc = KUMALLOC(sizeof(ehci_softc_t), DMA_BUF_ALIGN);
	if (!softc)
		return NULL;

	bus = KUMALLOC(sizeof(usbbus_t), DMA_BUF_ALIGN);
	if (!bus) {
		KUFREE(softc);
		return NULL;
	}

	memset(softc, 0, sizeof(ehci_softc_t));
	memset(bus, 0, sizeof(usbbus_t));

	bus->ub_hwsoftc = (usb_hc_t *) softc;
	bus->ub_hwdisp = &ehci_driver;
	bus->ub_flags = UB_FLG_USB20;

	q_init(&(softc->ehci_rh_intrq));

	softc->ehci_regs = addr;
	softc->ehci_rh_newaddr = -1;
	softc->ehci_bus = bus;

	res = _ehci_initpools(softc);
	if (res != 0)
		goto error;

	softc->eptcnt = 0;
	softc->xfercnt = 0;

	res = _ehci_setupepts(softc);
	if (res != 0)
		goto error;

	return bus;

error:
	KUFREE(softc);
	KUFREE(bus);
	return NULL;
}

/*  *********************************************************************
    *  ehci_ept_create(bus,usbaddr,eptnum,mps,flags)
    *
    *  Create a hardware endpoint structure and attach it to
    *  the hardware's endpoint list.  The hardware manages lists
    *  of queues, and this routine adds a new queue to the appropriate
    *  list of queues for the endpoint in question.  It roughly
    *  corresponds to the information in the EHCI specification.
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

static usb_ept_t *ehci_ept_create(usbbus_t *bus,
				  int usbaddr, int eptnum, int mps, int flags)
{
	uint32_t eptflags, hport = 0, haddr = 0, spd, reg;
	ehci_endpoint_t *ept;
	ehci_qh_t *qh;
	ehci_transfer_t *dummyxfer;
	ehci_qtd_t *dummyqtd;
	ehci_softc_t *softc = (ehci_softc_t *) bus->ub_hwsoftc;
#ifdef INTR_PIPE
	int idx, pte;
#endif

	/* Extract split transaction and speed info */
	hport = (flags >> UP_RPORT_SHIFT) & UP_RPORT_MASK;
	haddr = (flags >> UP_ROUTE_SHIFT) & UP_ROUTE_MASK;
	if (flags & UP_TYPE_HIGHSPEED)
		spd = HS;
	else if (flags & UP_TYPE_LOWSPEED)
		spd = LS;
	else
		spd = FS;

	/*
	 * Set up functional address, endpoint number, and packet size
	 */
	if (mps < 64)
		mps = 64;
	eptflags = V_EHCI_QH_DA(usbaddr) | V_EHCI_QH_EP(eptnum) |
	    V_EHCI_QH_EPS(spd) | V_EHCI_QH_MPS(mps);
	if ((spd != HS) && (flags & UP_TYPE_CONTROL))
		eptflags |= M_EHCI_QH_C;

	if (ehcidebug & SHOW_INIT_STATS) {
		printf
		    ("EHCI EptCreate: Create endpoint %d addr %d flags %08X mps %d\n",
		     eptnum, usbaddr, eptflags, mps);
	}

	/*
	 * Allocate the QH and placeholder QTD
	 */

	ept = _ehci_allocept(softc);
	qh = ehci_qh_from_endpoint(softc, ept);

	dummyxfer = _ehci_allocxfer(softc);
	dummyqtd = ehci_qtd_from_transfer(softc, dummyxfer);

	/*
	 * Transfer main info into the endpoint descriptor.
	 * Allocate a dummy TD to the endpoint.
	 */

	qh->qh_control = BSWAP32(eptflags);
	ept->ep_flags = flags;
	ept->ep_mps = mps;
	ept->ep_num = eptnum;
	qh->qh_dw2 = BSWAP32(V_EHCI_QH_MULT(1) |
			     V_EHCI_QH_STPN(hport) |
			     V_EHCI_QH_STHA(haddr));
	qh->qh_next_qtd = BSWAP32(EHCI_VTOP(dummyqtd));
	ept->qtd_list = dummyqtd;

	/*
	 * Put it on the right queue
	 */

	if (flags & (UP_TYPE_CONTROL | UP_TYPE_BULK))
		_ehci_queueept(softc, softc->ehci_async_list, ept);

#ifdef INTR_PIPE
	else if ((flags & UP_TYPE_INTR) && (usbaddr != 1)) {
		/* no need to queue root hub intr requests */

		/* queue into first empty entry and also into subsequent 64
		   entry offets to get INTR period of 64mS needed for HID */
		for (pte = 0; pte < EHCI_INTR_TABLE_SIZE; pte++) {
			if (softc->ehci_intr_table[pte] == NULL_INTR_QH)
				break;
		}
		if (pte < EHCI_INTR_TABLE_SIZE) {
			qh->qh_next = NULL_INTR_QH;

			qh->qh_dw2 |= BSWAP32(V_EHCI_QH_STSM(0x01));
			if (spd != HS)
				qh->qh_dw2 |= BSWAP32(V_EHCI_QH_STCM(0x06));
			for (idx = 0; idx < 4; idx++) {
				softc->ehci_intr_table[pte+(idx*64)] =
				    BSWAP32(ept->ep_phys + V_EHCI_QH_TYP(QH));
			}
			_ehci_queueept(softc, softc->ehci_periodic_list, ept);
		} else
			printf("EHCI No INTR entries available!\n");
	}
#endif

	/* Enable async and periodic lists if device connected */
	if (eptused(softc) < EHCI_NUM_MIN_FREE_EP) {
		reg = EHCI_READCSR(softc, R_EHCI_USBCMD);
		if (!(reg & M_EHCI_USBCMD_ASE))
			reg |= M_EHCI_USBCMD_ASE;
#ifdef INTR_PIPE
		if (!(reg & M_EHCI_USBCMD_PSE))
			reg |= M_EHCI_USBCMD_PSE;
#endif
		EHCI_WRITECSR(softc, R_EHCI_USBCMD, reg);
	}

	return (usb_ept_t *) ept;
}

/*  *********************************************************************
    *  ehci_ept_setaddr(bus,ept,usbaddr)
    *
    *  Change the functional address for a USB endpoint.  We do this
    *  when we switch the device's state from DEFAULT to ADDRESSED
    *  and we've already got the default pipe open.  This
    *  routine mucks with the descriptor and changes its address
    *  bits.
    *
    *  Input parameters:
    *	bus - usb bus structure
    *	ept - an open endpoint descriptor
    *	usbaddr - new address for this endpoint
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void ehci_ept_setaddr(usbbus_t *bus, usb_ept_t *uept, int usbaddr)
{
	uint32_t eptflags;
	ehci_endpoint_t *ept = (ehci_endpoint_t *) uept;
	ehci_softc_t *softc = (ehci_softc_t *) bus->ub_hwsoftc;
	ehci_qh_t *qh = ehci_qh_from_endpoint(softc, ept);

	eptflags = BSWAP32(qh->qh_control);
	eptflags &= ~M_EHCI_QH_DA;
	eptflags |= V_EHCI_QH_DA(usbaddr);
	qh->qh_control = BSWAP32(eptflags);
}

/*  *********************************************************************
    *  ehci_ept_setmps(bus,ept,mps)
    *
    *  Set the maximum packet size of this endpoint.  This is
    *  normally used during the processing of endpoint 0 (default
    *  pipe) after we find out how big ep0's packets can be.
    *
    *  Input parameters:
    *	bus - our USB bus structure
    *	ept - endpoint structure
    *	mps - new packet size
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void ehci_ept_setmps(usbbus_t *bus, usb_ept_t *uept, int mps)
{
	uint32_t eptflags;
	ehci_softc_t *softc = (ehci_softc_t *) bus->ub_hwsoftc;
	ehci_endpoint_t *ept = (ehci_endpoint_t *) uept;
	ehci_qh_t *qh = ehci_qh_from_endpoint(softc, ept);

	eptflags = BSWAP32(qh->qh_control);
	eptflags &= ~M_EHCI_QH_MPS;
	eptflags |= V_EHCI_QH_MPS(mps);
	qh->qh_control = BSWAP32(eptflags);
	ept->ep_mps = mps;

}

/*  *********************************************************************
    *  ehci_ept_settoggle(bus,ept)
    *
    *  Sets the data toggle for the specified endpoint.
    *
    *  Input parameters:
    *	ept - endpoint structure
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void ehci_ept_settoggle(ehci_softc_t *softc, ehci_endpoint_t *ept)
{
	uint32_t eptflags;
	ehci_qh_t *qh = ehci_qh_from_endpoint(softc, ept);

	eptflags = BSWAP32(qh->qh_control);
	eptflags |= M_EHCI_QTD_DT;	/* in QH overlay */
	qh->qh_qtdctl = BSWAP32(eptflags);
}

/*  *********************************************************************
    *  ehci_ept_cleartoggle(bus,ept)
    *
    *  Clear the data toggle for the specified endpoint.
    *  Also, clears ping state.
    *  Typically called when stalled endpoints are cleared.
    *
    *  Input parameters:
    *	bus - our USB bus structure
    *	ept - endpoint structure
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void ehci_ept_cleartoggle(usbbus_t *bus, usb_ept_t *uept)
{
	uint32_t eptflags;
	ehci_softc_t *softc = (ehci_softc_t *) bus->ub_hwsoftc;
	ehci_endpoint_t *ept = (ehci_endpoint_t *) uept;
	ehci_qh_t *qh = ehci_qh_from_endpoint(softc, ept);

	eptflags = BSWAP32(qh->qh_control);
	eptflags &= ~(M_EHCI_QTD_DT | M_EHCI_STATUS_PING); /* in QH overlay */
	qh->qh_control = BSWAP32(eptflags);
}

/*  *********************************************************************
    *  ehci_ept_delete(bus,ept)
    *
    *  Deletes an endpoint from the EHCI controller.  This
    *  routine also completes pending transfers for the
    *  endpoint and gets rid of the hardware ept (queue base).
    *
    *  Input parameters:
    *	bus - ehci bus structure
    *	ept - endpoint to remove
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void ehci_ept_delete(usbbus_t *bus, usb_ept_t *uept)
{
	ehci_softc_t *softc = (ehci_softc_t *) bus->ub_hwsoftc;
	ehci_endpoint_t *ept = (ehci_endpoint_t *) uept;
	uint32_t reg;

	if (ept->ep_flags & (UP_TYPE_CONTROL | UP_TYPE_BULK))
		_ehci_async_ept_delete(bus, uept);
#ifdef INTR_PIPE
	else if (ept->ep_flags & UP_TYPE_INTR)
		_ehci_periodic_ept_delete(bus, uept);
#endif
	else
		printf("EHCI Invalid endpoint\n");

	/* Disable async and periodic lists if no devices connected */
	if (eptused(softc) >= EHCI_NUM_MIN_FREE_EP) {
		reg = EHCI_READCSR(softc, R_EHCI_USBCMD);
		reg &= ~(M_EHCI_USBCMD_ASE | M_EHCI_USBCMD_PSE);
		EHCI_WRITECSR(softc, R_EHCI_USBCMD, reg);
	}
}

/*  *********************************************************************
    *  ehci_xfer(bus,ept,ur)
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

static int ehci_xfer(usbbus_t *bus, usb_ept_t *uept, usbreq_t *ur)
{
	ehci_softc_t *softc = (ehci_softc_t *) bus->ub_hwsoftc;
	ehci_endpoint_t *ept = (ehci_endpoint_t *) uept;
	ehci_transfer_t *newtransfer = 0;
	ehci_qtd_t *newhead, *newtail, *lastqtd = 0;
	ehci_transfer_t *curtransfer;
	ehci_qtd_t *curqtd;
	uint8_t *ptr;
	int i, len, qtdlen;
	int p0l, p0u;
	int amtcopy;
	int pktlen;
	uint32_t x, qtdcontrol = 0;

	/*
	 * If the destination USB address matches
	 * the address of the root hub, shunt the request
	 * over to our root hub emulation.
	 */

	if (ur->ur_dev->ud_address == softc->ehci_rh_addr)
		return ehci_roothub_xfer(bus, uept, ur);

	/*
	 * Set up the QTD flags based on the
	 * request type.
	 */

	if (ur->ur_flags & UR_FLAG_SETUP) {
		qtdcontrol = V_EHCI_QTD_PID(K_EHCI_QTD_SETUP) |
				V_EHCI_QTD_CERR(3); /* DT=0 */
	} else if (ur->ur_flags & UR_FLAG_IN) {
		qtdcontrol = V_EHCI_QTD_PID(K_EHCI_QTD_IN) |
				V_EHCI_QTD_CERR(3);
	} else if (ur->ur_flags & UR_FLAG_OUT) {
		qtdcontrol = V_EHCI_QTD_PID(K_EHCI_QTD_OUT) |
				V_EHCI_QTD_CERR(3);
	} else if (ur->ur_flags & UR_FLAG_STATUS_OUT) {
		ehci_ept_settoggle(softc, ept);
		qtdcontrol = V_EHCI_QTD_PID(K_EHCI_QTD_OUT) |
				V_EHCI_QTD_CERR(3);
	} else if (ur->ur_flags & UR_FLAG_STATUS_IN) {
		ehci_ept_settoggle(softc, ept);
		qtdcontrol = V_EHCI_QTD_PID(K_EHCI_QTD_IN) |
				V_EHCI_QTD_CERR(3);
	} else {
		printf("EHCI Shouldn't happen...invalid ur_flags!\n");
	}

	ptr = ur->ur_buffer;
	len = ur->ur_length;
	ur->ur_tdcount = 0;

	if (ehcidebug & SHOW_XFER_QUEUE) {
		printf("EHCI Xfer: addr %d pipe %02X ",
			ur->ur_dev->ud_address, ur->ur_pipe->up_num);
		printf("QH %08X ptr %p control %08x length %d\n",
			ept->ep_phys, ptr, qtdcontrol, len);
	}

	curqtd = ept->qtd_list;
	while (curqtd->qtd_next & M_EHCI_QTD_PTRMASK)	/* find the tail */
		curqtd = (ehci_qtd_t *) EHCI_PTOV(curqtd->qtd_next);
	newhead = curqtd;
	curtransfer = ehci_transfer_from_qtd(softc, curqtd);

	if (len == 0) {
		newtransfer = _ehci_allocxfer(softc);
		newtail = ehci_qtd_from_transfer(softc, newtransfer);

		/* old tail becomes head of new list */
		curqtd->qtd_control = BSWAP32(qtdcontrol);
		curqtd->qtd_next = BSWAP32(EHCI_VTOP(newtail));
		curtransfer->t_next = newtransfer;
		curtransfer->t_ref = ur;
		curtransfer->t_length = 0;

		if (ehcidebug & SHOW_XFER_QUEUE) {
			printf("EHCI Xfer0: ");
			printf("qtd %p xfer %p\n", curqtd, curtransfer);
		}

		ur->ur_tdcount++;
		lastqtd = curqtd;
		if (ehcidebug & SHOW_XFER_QUEUE2) {
			printf("EHCI Xfer ");
			ehci_dumpqtd(curqtd);
		}
	} else {
		pktlen = EHCI_QTD_MAX_DATA;

		/* Noncoherent DMA: need to flush user
		 buffer to real memory first */
		if (len)
			CACHE_FLUSH_RANGE(ptr, len);

		while (len > 0) {
			amtcopy = len;
			if (amtcopy > pktlen)
				amtcopy = pktlen;
			newtransfer = _ehci_allocxfer(softc);
			newtail = ehci_qtd_from_transfer(softc, newtransfer);
			curqtd->qtd_bufp[0] = BSWAP32(EHCI_VTOP(ptr));
			x = (uint32_t)ptr;
			if (!(x & ~M_EHCI_QTD_BUFMASK)) { /* 4k aligned? */
				p0l = (amtcopy < EHCI_QTD_MAX_BUF) ?
					amtcopy : EHCI_QTD_MAX_BUF;
				p0u = 0;
			}
			/* 4k page crossing? */
			else if ((x + amtcopy) > ((x & M_EHCI_QTD_BUFMASK)
					+ EHCI_QTD_MAX_BUF)) {

				/* unused space in page 0 */
				p0u = (uint32_t)ptr % EHCI_QTD_MAX_BUF;

				/* length in page 0 based on buffer alignment */
				p0l = EHCI_QTD_MAX_BUF - p0u;
			} else { /* less than 4k for page 0 */
				p0l = amtcopy;
				p0u = 0;
			}

			/* if using all QTD buffers then
			 adjust this qtd's length */
			if (amtcopy == EHCI_QTD_MAX_DATA)
				amtcopy -= p0u;

			qtdlen = amtcopy;
			curqtd->qtd_control =
			    BSWAP32(qtdcontrol | V_EHCI_QTD_LEN(qtdlen));
			ptr += p0l;
			qtdlen -= p0l;
			for (i = 1; (qtdlen > 0) && (i < 5); ++i) {
				curqtd->qtd_bufp[i] =
				    BSWAP32(EHCI_VTOP(ptr) &
					    M_EHCI_QTD_BUFMASK);
				ptr += EHCI_QTD_MAX_BUF;
				qtdlen -= EHCI_QTD_MAX_BUF;
			}
			if (ehcidebug & SHOW_XFER_QUEUE2) {
				printf("EHCI Xfer ");
				ehci_dumpqtd(curqtd);
			}
			curqtd->qtd_next = BSWAP32(EHCI_VTOP(newtail));
			curtransfer->t_next = newtransfer;
			curtransfer->t_ref = ur;
			curtransfer->t_length = amtcopy;
			if (ehcidebug & SHOW_XFER_QUEUE1)
				printf("EHCI Xfer: qtd %p xfer %p\n", curqtd,
				       curtransfer);
			len -= amtcopy;
			ur->ur_tdcount++;
			lastqtd = curqtd;
			curqtd = newtail;
			curtransfer = newtransfer;

			/* mark active for subsequent QTDs */
			qtdcontrol |= M_EHCI_STATUS_ACTIVE;

			if (ehcidebug & SHOW_XFER_QUEUE3) {
				printf("EHCI Xfer ");
				ehci_dumpqtd(lastqtd);
			}
		}
	}

	/* last one in new list should interrupt */
	lastqtd->qtd_control |= BSWAP32(M_EHCI_QTD_IOC);

	/* this is where h/w acts on our new QTD list */
	newhead->qtd_control |= BSWAP32(M_EHCI_STATUS_ACTIVE);

	return 0;
}

/*  *********************************************************************
    *  ehci_reset(bus)
    *
    *  Reset EHCI
    *
    *  Input parameters:
    *	bus - our USB bus structure
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void ehci_reset(usbbus_t *bus)
{
	ehci_softc_t *softc = (ehci_softc_t *) bus->ub_hwsoftc;

	EHCI_WRITECSR(softc, R_EHCI_USBCMD, 0);
	EHCI_WRITECSR(softc, R_EHCI_USBCMD, M_EHCI_USBCMD_HCR);
}

/*  *********************************************************************
    *  Driver structure
    ********************************************************************* */

usb_hcdrv_t ehci_driver = {
	ehci_create,
	ehci_delete,
	ehci_start,
	ehci_stop,
	ehci_intr,
	ehci_ept_create,
	ehci_ept_delete,
	ehci_ept_setmps,
	ehci_ept_setaddr,
	ehci_ept_cleartoggle,
	ehci_xfer,
	ehci_reset
};

/*  *********************************************************************
    *  Root Hub
    *
    *  Data structures and functions
    ********************************************************************* */

/*
 * Data structures and routines to emulate the root hub.
 */
static usb_device_descr_t ehci_root_devdsc = {
	sizeof(usb_device_descr_t),	/* bLength */
	USB_DEVICE_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USBWORD(0x0200),	/* bcdUSB */
	USB_DEVICE_CLASS_HUB,	/* bDeviceClass */
	0,			/* bDeviceSubClass */
	0,			/* bDeviceProtocol */
	64,			/* bMaxPacketSize0 */
	USBWORD(0),		/* idVendor */
	USBWORD(0),		/* idProduct */
	USBWORD(0x0100),	/* bcdDevice */
	1,			/* iManufacturer */
	2,			/* iProduct */
	0,			/* iSerialNumber */
	1			/* bNumConfigurations */
};

static usb_config_descr_t ehci_root_cfgdsc = {
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

static usb_interface_descr_t ehci_root_ifdsc = {
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

static usb_endpoint_descr_t ehci_root_epdsc = {
	sizeof(usb_endpoint_descr_t),	/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType */
	(USB_ENDPOINT_DIRECTION_IN | 1),	/* bEndpointAddress */
	USB_ENDPOINT_TYPE_INTERRUPT,	/* bmAttributes */
	USBWORD(8),		/* wMaxPacketSize */
	255			/* bInterval */
};

static usb_hub_descr_t ehci_root_hubdsc = {
	USB_HUB_DESCR_SIZE,	/* bLength */
	USB_HUB_DESCRIPTOR_TYPE,	/* bDescriptorType */
	0,			/* bNumberOfPorts */
	USBWORD(0),		/* wHubCharacteristics */
	0,			/* bPowreOnToPowerGood */
	0,			/* bHubControl Current */
	{0}			/* bRemoveAndPowerMask */
};

/*  *********************************************************************
    *  ehci_roothb_strdscr(ptr,str)
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

static int ehci_roothub_strdscr(uint8_t *ptr, char *str)
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
    *  ehci_hub_status(portstat)
    *
    *  Build 32-bit hub status from EHCI port status.
    *
    *  Input parameters:
    *	ps - EHCI port status
    *
    *  Return value:
    *	full bub status
    ********************************************************************* */

static int ehci_hub_status(uint32_t ps)
{
	uint32_t hubstat = 0;

	/* First assemble port state */
	hubstat = (ps & M_EHCI_PORTSC_CCS) | ((ps & M_EHCI_PORTSC_PED) >> 1) |
	    ((ps & M_EHCI_PORTSC_PSS) >> 5) | ((ps & M_EHCI_PORTSC_PR) >> 4) |
	    ((ps & M_EHCI_PORTSC_PP) >> 4);
	if (ps & M_EHCI_PORTSC_PED)
		hubstat |= USB_PORT_STATUS_HIGHSPD;

	/* Now assemble port status change */
	hubstat |=
	    ((ps & M_EHCI_PORTSC_CSC) << 15) | ((ps & M_EHCI_PORTSC_PESC) << 14)
	    | ((ps & M_EHCI_PORTSC_OCC) << 14);

	return hubstat;
}

/*  *********************************************************************
    *  ehci_roothub_req(softc,req)
    *
    *  Handle a descriptor request on the control pipe for the
    *  root hub.  We pretend to be a real root hub here and
    *  return all the standard descriptors.
    *
    *  Input parameters:
    *	softc - our EHCI controller
    *	req - a usb request (completed immediately)
    *
    *  Return value:
    *	0 if ok
    *	else error code
    ********************************************************************* */

static int ehci_roothub_req(ehci_softc_t *softc, usb_device_request_t *req)
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

	ptr = softc->ehci_rh_buf;

	wLength = GETUSBFIELD(req, wLength);
	wValue = GETUSBFIELD(req, wValue);
	wIndex = GETUSBFIELD(req, wIndex);

	if (ehcidebug & SHOW_RH_INFO3) {
		printf("EHCI RH-SETUP: bmRequestType %02x bRequest %02x ",
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
		status = EHCI_READCSR(softc, (R_EHCI_PORTSC(wIndex)));
		if (ehcidebug & SHOW_RH_INFO2) {
			printf("EHCI EHCI RHGetStatus: ");
			ehci_dumpportstat(wIndex, status);
		}
		status = ehci_hub_status(status);
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
		statport = R_EHCI_PORTSC(wIndex);
		status = EHCI_READCSR(softc, statport) & M_EHCI_PORTSC_KEEP;
		if (ehcidebug & SHOW_RH_INFO2) {
			printf("EHCI RHClearFeature(%d): ", wValue);
			ehci_dumpportstat(wIndex,
					  EHCI_READCSR(softc, statport));
		}
		switch (wValue) {
		case USB_PORT_FEATURE_CONNECTION:
			break;
		case USB_PORT_FEATURE_ENABLE:
			EHCI_WRITECSR(softc, statport,
				      (status & ~M_EHCI_PORTSC_PED));
			break;
		case USB_PORT_FEATURE_SUSPEND:
			break;
		case USB_PORT_FEATURE_OVER_CURRENT:
			break;
		case USB_PORT_FEATURE_RESET:
			break;
		case USB_PORT_FEATURE_POWER:
			status &= ~M_EHCI_PORTSC_PP;
			EHCI_WRITECSR(softc, statport, status);
			break;
		case USB_PORT_FEATURE_C_PORT_CONNECTION:
			EHCI_WRITECSR(softc, statport,
				      (status | M_EHCI_PORTSC_CSC));
			break;
		case USB_PORT_FEATURE_C_PORT_ENABLE:
			EHCI_WRITECSR(softc, statport,
				      (status | M_EHCI_PORTSC_PESC));
			break;
		case USB_PORT_FEATURE_C_PORT_SUSPEND:
			break;
		case USB_PORT_FEATURE_C_PORT_OVER_CURRENT:
			EHCI_WRITECSR(softc, statport,
				      (status | M_EHCI_PORTSC_OCC));
			break;
		case USB_PORT_FEATURE_C_PORT_RESET:
			break;
		}

		/*
		 * If we've cleared all of the conditions that
		 * want our attention on the port status,
		 * then we can accept port status interrupts again.
		 */

		if ((wValue >= USB_PORT_FEATURE_C_PORT_CONNECTION) &&
		    (wValue <= USB_PORT_FEATURE_C_PORT_RESET)) {
			status = EHCI_READCSR(softc, statport);

			if ((status & M_EHCI_PORTSC_ALLC) == 0)
				softc->ehci_intdisable &= ~M_EHCI_USBSTS_PSC;
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
		statport = R_EHCI_PORTSC(wIndex);
		status = EHCI_READCSR(softc, statport);
		if (ehcidebug & SHOW_RH_INFO2) {
			printf("EHCI RHSetFeature(%d): ", wValue);
			ehci_dumpportstat(wIndex,
					  EHCI_READCSR(softc, statport));
		}
		switch (wValue) {
		case USB_PORT_FEATURE_CONNECTION:
			break;
		case USB_PORT_FEATURE_ENABLE:
			break;
		case USB_PORT_FEATURE_SUSPEND:
			status &= M_EHCI_PORTSC_KEEP;
			EHCI_WRITECSR(softc, statport,
				      (status | M_EHCI_PORTSC_PSS));
			break;
		case USB_PORT_FEATURE_OVER_CURRENT:
			break;
		case USB_PORT_FEATURE_RESET:
			if (ehcidebug & SHOW_RH_INFO2)
				printf("EHCI RHSetFeature(RESET)\n");
			if (G_EHCI_PORTSC_LS(status) == K_EHCI_LS_K)
				EHCI_WRITECSR(softc, statport, (status |
					M_EHCI_PORTSC_PO));
					/* must be LS device */
			else {
				EHCI_WRITECSR(softc, statport,
					      (status | M_EHCI_PORTSC_PR));
				usb_delay_ms(softc->ehci_bus, 50);
				EHCI_WRITECSR(softc, statport, status);
				usb_delay_ms(softc->ehci_bus, 50);

				if (!(EHCI_READCSR(softc, statport) &
					M_EHCI_PORTSC_PED))
					EHCI_WRITECSR(softc, statport,
					(status | M_EHCI_PORTSC_PO));
					/* must be FS device */

				usb_delay_ms(softc->ehci_bus, 100);
			}
			break;
		case USB_PORT_FEATURE_POWER:
			if (ehcidebug & SHOW_RH_INFO2)
				printf("EHCI RHSetFeature(POWER)\n");
			EHCI_WRITECSR(softc, statport, M_EHCI_PORTSC_PP);
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
		if (ehcidebug & SHOW_RH_INFO2)
			printf("EHCI RHSetAddr(%d)\n", wValue);
		softc->ehci_rh_newaddr = wValue;
		break;

	case REQCODE(USB_REQUEST_GET_DESCRIPTOR,
		USBREQ_DIR_IN, USBREQ_TYPE_STD, USBREQ_REC_DEVICE):
		switch (wValue >> 8) {
		case USB_DEVICE_DESCRIPTOR_TYPE:
			memcpy(ptr, &ehci_root_devdsc,
			       sizeof(ehci_root_devdsc));
			ptr += sizeof(ehci_root_devdsc);
			break;
		case USB_CONFIGURATION_DESCRIPTOR_TYPE:
			memcpy(ptr, &ehci_root_cfgdsc,
			       sizeof(ehci_root_cfgdsc));
			ptr += sizeof(ehci_root_cfgdsc);
			memcpy(ptr, &ehci_root_ifdsc, sizeof(ehci_root_ifdsc));
			ptr += sizeof(ehci_root_ifdsc);
			memcpy(ptr, &ehci_root_epdsc, sizeof(ehci_root_epdsc));
			ptr += sizeof(ehci_root_epdsc);
			break;
		case USB_STRING_DESCRIPTOR_TYPE:
			switch (wValue & 0xFF) {
			case 1:
				ptr += ehci_roothub_strdscr(ptr, "Generic");
				break;
			case 2:
				ptr += ehci_roothub_strdscr(ptr, "Root Hub");
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
		memcpy(&hdsc, &ehci_root_hubdsc, sizeof(hdsc));
		hdsc.bNumberOfPorts = softc->ehci_ndp;
		tmpval = USB_HUBCHAR_PWR_IND;
		PUTUSBFIELD((&hdsc), wHubCharacteristics, tmpval);
		hdsc.bPowerOnToPowerGood = 10;	/* 20 mS...per spec */
		hdsc.bDescriptorLength = USB_HUB_DESCR_SIZE + 1;
		hdsc.bRemoveAndPowerMask[0] =
		    (softc->ehci_ndp == 2 ? 0x60000 : 0x20000);
		memcpy(ptr, &hdsc, sizeof(hdsc));
		ptr += sizeof(hdsc);
		break;

	case REQCODE(USB_REQUEST_SET_DESCRIPTOR,
		USBREQ_DIR_OUT, USBREQ_TYPE_CLASS, USBREQ_REC_DEVICE):
		/* nothing */
		break;

	case REQCODE(USB_REQUEST_GET_CONFIGURATION,
		USBREQ_DIR_IN, USBREQ_TYPE_STD, USBREQ_REC_DEVICE):
		*ptr++ = softc->ehci_rh_conf;
		break;

	case REQCODE(USB_REQUEST_SET_CONFIGURATION,
		USBREQ_DIR_OUT, USBREQ_TYPE_STD, USBREQ_REC_DEVICE):
		softc->ehci_rh_conf = wValue;
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

	softc->ehci_rh_ptr = softc->ehci_rh_buf;
	softc->ehci_rh_len = ptr - softc->ehci_rh_buf;

	return res;
}

/*  *********************************************************************
    *  ehci_roothub_statchg(softc)
    *
    *  This routine is called from the interrupt service routine
    *  (well, polling routine) for the ehci controller.  If the
    *  controller notices a root hub status change, it dequeues an
    *  interrupt transfer from the root hub's queue and completes
    *  it here.
    *
    *  Input parameters:
    *	softc - our EHCI controller
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static void ehci_roothub_statchg(ehci_softc_t *softc)
{
	usbreq_t *ur;
	uint32_t status;
	uint8_t portstat = 0;
	int idx;

	/* Note: this only works up to 8 ports */
	for (idx = 1; idx <= softc->ehci_ndp; idx++) {
		status = EHCI_READCSR(softc, R_EHCI_PORTSC(idx));
		if (status & M_EHCI_PORTSC_ALLC)
			portstat = (1 << idx);
	}

	if (portstat != 0) {
		ur = (usbreq_t *)q_deqnext(&(softc->ehci_rh_intrq));
		if (!ur) {	/* no requests pending, postpone action */
			softc->pending_psc = 1;
			return;
		}

		memset(ur->ur_buffer, 0, ur->ur_length);
		ur->ur_buffer[0] = portstat;
		ur->ur_xferred = ur->ur_length;

		usb_complete_request(ur, 0);
		softc->ehci_intdisable |= M_EHCI_USBSTS_PSC;
	}
	softc->pending_psc = 0;
}

/*  *********************************************************************
    *  ehci_roothub_xfer(softc,req)
    *
    *  Handle a root hub xfer - ehci_xfer transfers control here
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

static int ehci_roothub_xfer(usbbus_t *bus, usb_ept_t *uept, usbreq_t *ur)
{
	ehci_softc_t *softc = (ehci_softc_t *) bus->ub_hwsoftc;
	ehci_endpoint_t *ept = (ehci_endpoint_t *) uept;
	int res;

	switch (ept->ep_num) {

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

			res = ehci_roothub_req(softc, req);
			if (res != 0)
				printf
				    ("EHCI Root hub request returned an error\n");

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

			if (softc->ehci_rh_newaddr != -1) {
				softc->ehci_rh_addr = softc->ehci_rh_newaddr;
				softc->ehci_rh_newaddr = -1;
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

			amtcopy = softc->ehci_rh_len;
			if (amtcopy > ur->ur_length)
				amtcopy = ur->ur_length;

			memcpy(ur->ur_buffer, softc->ehci_rh_ptr, amtcopy);

			softc->ehci_rh_ptr += amtcopy;
			softc->ehci_rh_len -= amtcopy;

			ur->ur_status = 0;
			ur->ur_xferred = amtcopy;
			usb_complete_request(ur, 0);
		}

		else {
			printf("EHCI Unknown root hub transfer type\n");
			return -1;
		}
		break;

		/*
		 * INTERRUPT ENDPOINT
		 */

	case 1:		/* interrupt pipe */
		if (ur->ur_flags & UR_FLAG_IN)
			q_enqueue(&(softc->ehci_rh_intrq), (queue_t *) ur);
		break;

	}

	return 0;
}
