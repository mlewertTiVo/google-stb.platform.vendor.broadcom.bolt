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

#define TRACE()         printf("%s: @%d\n", __FUNCTION__, __LINE__)
#define FTRACE()         {if (ehcidebug & SHOW_FUNC_TRACE) printf("%s: @%d\n", __FUNCTION__, __LINE__);}

/*  *********************************************************************
    *  Some debug variable definitions
    ********************************************************************* */

#define SHOW_EPT_ALLOC		0x00000001
#define SHOW_EPT_QH			0x00000002
#define SHOW_EPT_FREE		0x00000004
#define SHOW_EPT_STATS		0x00000008
#define SHOW_EPT_QUEUE		0x00000010
#define SHOW_XFER_ALLOC		0x00000100
#define SHOW_XFER_QTD		0x00000200
#define SHOW_XFER_FREE		0x00000400
#define SHOW_XFER_STATS		0x00000800
#define SHOW_XFER_QUEUE		0x00001000
#define SHOW_XFER_QUEUE1	0x00002000
#define SHOW_XFER_QUEUE2	0x00004000
#define SHOW_XFER_QUEUE3	0x00008000
#define SHOW_INIT_STATS		0x00010000
#define SHOW_RH_INFO1		0x00020000
#define SHOW_RH_INFO2		0x00040000
#define SHOW_RH_INFO3		0x00080000
#define SHOW_RH_INFO1		0x00020000
#define SHOW_DONE_XFER		0x01000000
#define SHOW_DONE_XFER1		0x02000000
#define SHOW_DONE_QTD		0x04000000
#define DBG_GEN1			0x08000000
#define SHOW_GEN_DBG1		0x10000000
#define SHOW_GEN_DBG2		0x20000000
#define SHOW_GEN_DBG  		0x40000000
#define SHOW_FUNC_TRACE		0x80000000
#define SHOW_DONE			(SHOW_DONE_QTD | SHOW_DONE_XFER)

/*  *********************************************************************
    *  Macros to muck with bitfields
    ********************************************************************* */

#define _EHCI_MAKE32(x) ((uint32_t)(x))

/*
 * Make a mask for 1 bit at position 'n'
 */

#define _EHCI_MAKEMASK1(n) (_EHCI_MAKE32(1) << _EHCI_MAKE32(n))

/*
 * Make a mask for 'v' bits at position 'n'
 */

#define _EHCI_MAKEMASK(v,n) (_EHCI_MAKE32((_EHCI_MAKE32(1)<<(v))-1) << _EHCI_MAKE32(n))

/*
 * Make a value at 'v' at bit position 'n'
 */

#define _EHCI_MAKEVALUE(v,n) (_EHCI_MAKE32(v) << _EHCI_MAKE32(n))
#define _EHCI_GETVALUE(v,n,m) ((_EHCI_MAKE32(v) & _EHCI_MAKE32(m)) >> _EHCI_MAKE32(n))

/*  *********************************************************************
    *  QH Descriptor
    ********************************************************************* */

#define EHCI_QH_ALIGN	32
enum { ITD, QH, SITD, FSTN };

typedef volatile struct ehci_qh_s {
	uint32_t qh_next;
	uint32_t qh_control;
	uint32_t qh_dw2;
	uint32_t qh_curr_qtd;
	uint32_t qh_next_qtd;
	uint32_t qh_alt_qtd;
	uint32_t qh_qtdctl;
	uint32_t qh_bufp[5];
	uint32_t qh_bufp_hi[5];
	uint32_t qh_pad[7];	/* to force 32 byte QH ptr alignment */
} ehci_qh_t;

#define M_EHCI_QH_PTR		0xFFFFFFF0
#define G_EHCI_QH_PTR(x)	(x & M_EHCI_QH_PTR)

#define M_EHCI_QH_T			_EHCI_MAKEMASK1(0)

#define S_EHCI_QH_TYP		1
#define M_EHCI_QH_TYP		_EHCI_MAKEMASK(2,S_EHCI_QH_TYP)
#define V_EHCI_QH_TYP(x)	_EHCI_MAKEVALUE(x,S_EHCI_QH_TYP)
#define G_EHCI_QH_TYP(x)	_EHCI_GETVALUE(x,S_EHCI_QH_TYP,M_EHCI_QH_TYP)

#define S_EHCI_QH_DA		0
#define M_EHCI_QH_DA		_EHCI_MAKEMASK(7,S_EHCI_QH_DA)
#define V_EHCI_QH_DA(x)		_EHCI_MAKEVALUE(x,S_EHCI_QH_DA)
#define G_EHCI_QH_DA(x)		_EHCI_GETVALUE(x,S_EHCI_QH_DA,M_EHCI_QH_DA)

#define S_EHCI_QH_EP		8
#define M_EHCI_QH_EP		_EHCI_MAKEMASK(4,S_EHCI_QH_EP)
#define V_EHCI_QH_EP(x)		_EHCI_MAKEVALUE(x,S_EHCI_QH_EP)
#define G_EHCI_QH_EP(x)		_EHCI_GETVALUE(x,S_EHCI_QH_EP,M_EHCI_QH_EP)

#define S_EHCI_QH_EPS		12
#define M_EHCI_QH_EPS		_EHCI_MAKEMASK(2,S_EHCI_QH_EPS)
#define V_EHCI_QH_EPS(x)	_EHCI_MAKEVALUE(x,S_EHCI_QH_EPS)
#define G_EHCI_QH_EPS(x)	_EHCI_GETVALUE(x,S_EHCI_QH_EPS,M_EHCI_QH_EPS)

#define M_EHCI_QH_DTC		_EHCI_MAKEMASK1(14)
#define M_EHCI_QH_H			_EHCI_MAKEMASK1(15)

#define S_EHCI_QH_MULT		30
#define M_EHCI_QH_MULT		_EHCI_MAKEMASK(2,S_EHCI_QH_MULT)
#define V_EHCI_QH_MULT(x)	_EHCI_MAKEVALUE(x,S_EHCI_QH_MULT)
#define G_EHCI_QH_MULT(x)	_EHCI_GETVALUE(x,S_EHCI_QH_MPS,M_EHCI_QH_MULT)

#define S_EHCI_QH_MPS		16
#define M_EHCI_QH_MPS		_EHCI_MAKEMASK(11,S_EHCI_QH_MPS)
#define V_EHCI_QH_MPS(x)	_EHCI_MAKEVALUE(x,S_EHCI_QH_MPS)
#define G_EHCI_QH_MPS(x)	_EHCI_GETVALUE(x,S_EHCI_QH_MPS,M_EHCI_QH_MPS)

#define M_EHCI_QH_C			EHCI_MAKEMASK1(27)

#define S_EHCI_QH_RL		28
#define M_EHCI_QH_RL		_EHCI_MAKEMASK(4,S_EHCI_QH_RL)
#define V_EHCI_QH_RL(x)		_EHCI_MAKEVALUE(x,S_EHCI_QH_RL)
#define G_EHCI_QH_RL(x)		_EHCI_GETVALUE(x,S_EHCI_QH_RL,M_EHCI_QH_RL)

/*  *********************************************************************
    *  Transfer Descriptor
    ********************************************************************* */

#define EHCI_QTD_ALIGN	32

typedef volatile struct ehci_qtd_s {
	uint32_t qtd_next;
	uint32_t qtd_alt;
	uint32_t qtd_control;
	uint32_t qtd_bufp[5];
	uint32_t qtd_bufp_hi[5];
	uint32_t qh_pad[3];	/* to force 32 byte QH ptr alignment */
} ehci_qtd_t;

#define M_EHCI_QTD_PTRMASK	0xFFFFFFF0

#define M_EHCI_QTD_T			_EHCI_MAKEMASK1(0)

#define S_EHCI_QTD_STATUS	0
#define M_EHCI_QTD_STATUS		_EHCI_MAKEMASK(8,S_EHCI_QTD_STATUS)
#define V_EHCI_QTD_STATUS(x)	_EHCI_MAKEVALUE(x,S_EHCI_QTD_STATUS)
#define G_EHCI_QTD_STATUS(x)	_EHCI_GETVALUE(x,S_EHCI_QTD_STATUS,M_EHCI_QTD_STATUS)

#define M_EHCI_STATUS_ACTIVE	_EHCI_MAKEMASK1(7)
#define M_EHCI_STATUS_HALTED	_EHCI_MAKEMASK1(6)
#define M_EHCI_STATUS_BUFERR	_EHCI_MAKEMASK1(5)
#define M_EHCI_STATUS_BABBLE	_EHCI_MAKEMASK1(4)
#define M_EHCI_STATUS_XACTERR	_EHCI_MAKEMASK1(3)
#define M_EHCI_STATUS_PING		_EHCI_MAKEMASK1(0)
#define M_EHCI_STATUS_ERR		(M_EHCI_STATUS_HALTED | M_EHCI_STATUS_BUFERR | \
								 M_EHCI_STATUS_BABBLE | M_EHCI_STATUS_XACTERR)

#define S_EHCI_QTD_PID		8
#define M_EHCI_QTD_PID		_EHCI_MAKEMASK(2,S_EHCI_QTD_PID)
#define V_EHCI_QTD_PID(x)	_EHCI_MAKEVALUE(x,S_EHCI_QTD_PID)
#define G_EHCI_QTD_PID(x)	_EHCI_GETVALUE(x,S_EHCI_QTD_PID,M_EHCI_QTD_PID)

#define K_EHCI_QTD_OUT		0
#define K_EHCI_QTD_IN		1
#define K_EHCI_QTD_SETUP	2
#define K_EHCI_QTD_RESERVED	3

#define V_EHCI_TD_SETUP		V_EHCI_TD_PID(K_EHCI_TD_SETUP)
#define V_EHCI_TD_OUT		V_EHCI_TD_PID(K_EHCI_TD_OUT)
#define V_EHCI_TD_IN		V_EHCI_TD_PID(K_EHCI_TD_IN)
#define V_EHCI_TD_RESERVED	V_EHCI_TD_PID(K_EHCI_TD_RESERVED)

#define S_EHCI_QTD_CERR		10
#define M_EHCI_QTD_CERR		_EHCI_MAKEMASK(2,S_EHCI_QTD_CERR)
#define V_EHCI_QTD_CERR(x)	_EHCI_MAKEVALUE(x,S_EHCI_QTD_CERR)
#define G_EHCI_QTD_CERR(x)	_EHCI_GETVALUE(x,S_EHCI_QTD_CERR,M_EHCI_QTD_CERR)

#define S_EHCI_QTD_CPG		12
#define M_EHCI_QTD_CPG		_EHCI_MAKEMASK(3,S_EHCI_QTD_CPG)
#define V_EHCI_QTD_CPG(x)	_EHCI_MAKEVALUE(x,S_EHCI_QTD_CPG)
#define G_EHCI_QTD_CPG(x)	_EHCI_GETVALUE(x,S_EHCI_QTD_CPG,M_EHCI_QTD_CPG)

#define M_EHCI_QTD_IOC		_EHCI_MAKEMASK1(15)

#define S_EHCI_QTD_LEN		16
#define M_EHCI_QTD_LEN		_EHCI_MAKEMASK(15,S_EHCI_QTD_LEN)
#define V_EHCI_QTD_LEN(x)	_EHCI_MAKEVALUE(x,S_EHCI_QTD_LEN)
#define G_EHCI_QTD_LEN(x)	_EHCI_GETVALUE(x,S_EHCI_QTD_LEN,M_EHCI_QTD_LEN)

#define M_EHCI_QTD_DT		_EHCI_MAKEMASK1(31)

#define M_EHCI_QTD_BUFMASK	0xFFFFF000
#define EHCI_QTD_MAX_BUF	(4*1024)
#define EHCI_QTD_MAX_DATA	(20*1024)

/*  *********************************************************************
    *  Registers
    ********************************************************************* */

#define _EHCI_REGIDX(x)	((x)*4)

#define R_EHCI_CAPVER		_EHCI_REGIDX(0)
#define R_EHCI_HCSPARAMS	_EHCI_REGIDX(1)
#define R_EHCI_HCCPARAMS	_EHCI_REGIDX(2)
#define R_EHCI_USBCMD		_EHCI_REGIDX(4)
#define R_EHCI_USBSTS		_EHCI_REGIDX(5)
#define R_EHCI_USBINTR		_EHCI_REGIDX(6)
#define R_EHCI_FRINDEX		_EHCI_REGIDX(7)
#define R_EHCI_PERBASE		_EHCI_REGIDX(9)
#define R_EHCI_ASYNCBASE	_EHCI_REGIDX(10)
#define R_EHCI_CONFIG		_EHCI_REGIDX(20)
#define R_EHCI_PORTSC(x)	_EHCI_REGIDX(20+(x))	/* note: 1-based! */

/*
 * R_EHCI_CAPVER
 */

#define S_EHCI_CAPVER_LEN		0
#define M_EHCI_CAPVER_LEN		_EHCI_MAKEMASK(8,S_EHCI_CAPVER_LEN)
#define G_EHCI_CAPVER_LEN(x)	_EHCI_GETVALUE(x,S_EHCI_CAPVER_LEN,M_EHCI_CAPVER_LEN)

#define S_EHCI_CAPVER_VER		16
#define M_EHCI_CAPVER_VER		_EHCI_MAKEMASK(16,S_EHCI_CAPVER_VER)
#define G_EHCI_CAPVER_VER(x)	_EHCI_GETVALUE(x,S_EHCI_CAPVER_VER,M_EHCI_CAPVER_VER)
#define K_EHCI_VER				0x20

/*
 * R_EHCI_HCSPARAMS
 */

#define S_EHCI_HCSPARAMS_NDP	0
#define M_EHCI_HCSPARAMS_NDP	_EHCI_MAKEMASK(4,S_EHCI_HCSPARAMS_NDP)
#define G_EHCI_HCSPARAMS_NDP(x)	_EHCI_GETVALUE(x,S_EHCI_HCSPARAMS_NDP,M_EHCI_HCSPARAMS_NDP)

/*
 * R_EHCI_USBCMD
 */

#define M_EHCI_USBCMD_RUN	_EHCI_MAKEMASK1(0)
#define M_EHCI_USBCMD_HCR	_EHCI_MAKEMASK1(1)
#define M_EHCI_USBCMD_PSE	_EHCI_MAKEMASK1(4)
#define M_EHCI_USBCMD_ASE	_EHCI_MAKEMASK1(5)
#define M_EHCI_USBCMD_IAAD	_EHCI_MAKEMASK1(6)

#define S_EHCI_USBCMD_FLS	2
#define M_EHCI_USBCMD_FLS		_EHCI_MAKEMASK(2,S_EHCI_USBCMD_FLS)
#define V_EHCI_USBCMD_FLS(x)	_EHCI_MAKEVALUE(x,S_EHCI_USBCMD_FLS)

/*
 * R_EHCI_USBSTS
 */

#define M_EHCI_USBSTS_INT	_EHCI_MAKEMASK1(0)
#define M_EHCI_USBSTS_ERR	_EHCI_MAKEMASK1(1)
#define M_EHCI_USBSTS_PSC	_EHCI_MAKEMASK1(2)
#define M_EHCI_USBSTS_FLR	_EHCI_MAKEMASK1(3)
#define M_EHCI_USBSTS_HSE	_EHCI_MAKEMASK1(4)
#define M_EHCI_USBSTS_IAA	_EHCI_MAKEMASK1(5)
#define M_EHCI_USBSTS_HCH	_EHCI_MAKEMASK1(12)
#define M_EHCI_USBSTS_REC	_EHCI_MAKEMASK1(13)
#define M_EHCI_USBSTS_PSS	_EHCI_MAKEMASK1(14)
#define M_EHCI_USBSTS_ASS	_EHCI_MAKEMASK1(15)
#define M_EHCI_INT_ALL		(M_EHCI_USBSTS_INT | M_EHCI_USBSTS_ERR | \
							 M_EHCI_USBSTS_PSC | M_EHCI_USBSTS_HSE )

/*
 * R_EHCI_FRINDEX
 */

#define S_EHCI_FRINDEX		0
#define M_EHCI_FRINDEX		_EHCI_MAKEMASK(14,S_EHCI_FRINDEX)
#define V_EHCI_FRINDEX(x)	_EHCI_MAKEVALUE(x,S_EHCI_FRINDEX)

/*
 * R_EHCI_PORTSC
 */

#define M_EHCI_PORTSC_CCS	_EHCI_MAKEMASK1(0)
#define M_EHCI_PORTSC_CSC	_EHCI_MAKEMASK1(1)
#define M_EHCI_PORTSC_PED	_EHCI_MAKEMASK1(2)
#define M_EHCI_PORTSC_PESC	_EHCI_MAKEMASK1(3)
#define M_EHCI_PORTSC_OCA	_EHCI_MAKEMASK1(4)
#define M_EHCI_PORTSC_OCC	_EHCI_MAKEMASK1(5)
#define M_EHCI_PORTSC_RSM	_EHCI_MAKEMASK1(6)
#define M_EHCI_PORTSC_PSS	_EHCI_MAKEMASK1(7)
#define M_EHCI_PORTSC_PR	_EHCI_MAKEMASK1(8)
#define M_EHCI_PORTSC_PP	_EHCI_MAKEMASK1(12)
#define M_EHCI_PORTSC_PO	_EHCI_MAKEMASK1(13)

#define S_EHCI_PORTSC_LS	10
#define M_EHCI_PORTSC_LS	_EHCI_MAKEMASK(2,S_EHCI_PORTSC_LS)
#define G_EHCI_PORTSC_LS(x)	_EHCI_GETVALUE(x,S_EHCI_PORTSC_LS,M_EHCI_PORTSC_LS)
#define K_EHCI_LS_SE0		0
#define K_EHCI_LS_K			1
#define K_EHCI_LS_J			2
#define K_EHCI_LS_SE1		3

#define M_EHCI_PORTSC_ALLC	(M_EHCI_PORTSC_CSC | M_EHCI_PORTSC_PESC | M_EHCI_PORTSC_OCC)
#define M_EHCI_PORTSC_KEEP	(M_EHCI_PORTSC_PP | M_EHCI_PORTSC_PO | M_EHCI_PORTSC_PED)

/*  *********************************************************************
    *  EHCI Structures
    ********************************************************************* */

//#define beginningof(ptr,type,field) ((type *) (((int) (ptr)) - ((int) ((type *) 0)->field)))

#define EHCI_INTR_TABLE_SIZE	256
#define EHCI_QHPOOL_SIZE		128
/* This should allow a max of 32-40 devices */
#define EHCI_QTDPOOL_SIZE		128

typedef struct ehci_endpoint_s {
	struct ehci_endpoint_s *ep_next;
	ehci_qtd_t *qtd_list;	/* used for processing completed QTDs */
	uint32_t ep_phys;
	int ep_flags;
	int ep_mps;
	int ep_num;
} ehci_endpoint_t;

typedef struct ehci_transfer_s {
	void *t_ref;
	int t_length;
	struct ehci_transfer_s *t_next;
} ehci_transfer_t;

typedef struct ehci_softc_s {
	ehci_endpoint_t *ehci_async_list;	/* points to dummy QH */
	ehci_endpoint_t *ehci_periodic_list;
	ehci_endpoint_t *ehci_endpoint_pool;
	ehci_transfer_t *ehci_transfer_pool;
	ehci_qh_t *ehci_hwqhpool;
	ehci_qtd_t *ehci_hwqtdpool;
	uint32_t *ehci_intr_table;
	ehci_endpoint_t *ehci_endpoint_freelist;
	ehci_transfer_t *ehci_transfer_freelist;
	physaddr_t ehci_regs;
	int ehci_ndp;
	long ehci_addr;
	uint32_t ehci_intdisable;

	int ehci_rh_newaddr;	/* Address to be set on next status update */
	int ehci_rh_addr;	/* address of root hub */
	int ehci_rh_conf;	/* current configuration # */
	uint8_t ehci_rh_buf[128];	/* buffer to hold hub responses */
	uint8_t *ehci_rh_ptr;	/* pointer into buffer */
	int ehci_rh_len;	/* remaining bytes to transfer */
	queue_t ehci_rh_intrq;	/* Interrupt request queue */
	usbbus_t *ehci_bus;	/* owning usbbus structure */

	int eptcnt, xfercnt;	/* debug stats */
} ehci_softc_t;

/*
 * Misc stuff
 */
#define EHCI_RESET_DELAY		10
#define K_EHCI_TD_CANCELLED		0xFF
