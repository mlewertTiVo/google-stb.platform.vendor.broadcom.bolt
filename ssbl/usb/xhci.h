/***************************************************************************
 *     Copyright (c) 2012-2017, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

/*  *********************************************************************
    *  Some general definitions
    *   - To make for easier TRB ring management, all rings
    *     MUST be of the same size.
    ********************************************************************* */

#define CONTEXT_SIZE_64		1	/* BRCM controller uses 64 byte CE's */
#define MAX_EP			30	/* for now...incl EP0 */
#define XHCI_TIMEOUT		1000	/* mS */
#define MAX_SLOTS		32
#define SLOT_SIZE		4096
#define ERST_SIZE		1	/* number of segments */
#define TRB_SIZE		(4*sizeof(uint32_t)) /* 16 bytes */
#define NUM_TRBS		32	/* per ring...see above note */
#define DCBAAP_LEN		(MAX_SLOTS*sizeof(uint64_t))
#define CMD_RING_LEN		(NUM_TRBS*TRB_SIZE)
#define EVENT_RING_LEN		(NUM_TRBS*TRB_SIZE)
#define EP0_RING_LEN		(NUM_TRBS*TRB_SIZE)
#define EPX_RING_LEN		(NUM_TRBS*TRB_SIZE)
#define	MAX_SCRATCH		32
#define SCRATCH_BUF_SIZE	4096

/* Pool defns: MD=32 devices, 4 EPs & (NT*4) TRBs per device */
#define CTXPOOL_SIZE		MAX_SLOTS
#define NUM_DEV_EPS		4
#define EPTPOOL_SIZE		(MAX_SLOTS*NUM_DEV_EPS)
#define TRBPOOL_SIZE		(EPTPOOL_SIZE*NUM_TRBS)

#define K_XHCI_TD_CANCELLED	0xFF

enum {
	ISOC_OUT_EP = 1, BULK_OUT_EP, INTR_OUT_EP, CTL_EP,
	ISOC_IN_EP, BULK_IN_EP, INTR_IN_EP
};
static char *EP_TYPE[] =  {
	"?0?", "ISOC_OUT", "BULK_OUT", "INTR_OUT",
	"CTL", "ISOC_IN", "BULK_IN", "INTR_IN"
};
enum XHCI_SPD { X_FS = 1, X_LS, X_HS, X_SS };
static char *SPD_TYPE[] =  {
	"S0?", "FS", "LS", "HS", "SS", "S5?", "S6?", "S7?"
};

/*  *********************************************************************
    *  Some debug variable definitions
    ********************************************************************* */

#define SHOW_EPT_CREATE		0x00000001
#define SHOW_EPT_CFG1		0x00000002
#define SHOW_EPT_CFG2		0x00000004
#define SHOW_EPT_CFG3		0x00000008
#define SHOW_EPT_DEL		0x00000010
#define SHOW_DB0		0x00000040
#define SHOW_DBX		0x00000080
#define SHOW_CMD_TRB		0x00000100
#define SHOW_XFER_TRB		0x00000200
#define SHOW_XFER_QUEUE1	0x00000400
#define SHOW_XFER_QUEUE2	0x00000800
#define SHOW_XFER_DONE		0x00001000
#define SHOW_RH_INFO1		0x00010000
#define SHOW_RH_INFO2		0x00020000
#define SHOW_RH_INFO3		0x00040000
#define SHOW_GEN_DBG0		0x00080000
#define SHOW_ALLOC		0x00100000
#define SHOW_FREE		0x00200000
#define SHOW_STATS		0x00400000
#define SHOW_INIT_STATS		0x00800000
#define SHOW_EVENTS1		0x01000000
#define SHOW_EVENTS2		0x02000000
#define SHOW_EVENTS3		0x04000000
#define SHOW_EVENTS4		0x08000000
#define SHOW_GEN_DBG1		0x10000000
#define SHOW_GEN_DBG2		0x20000000
#define SHOW_GEN_DBG3		0x40000000
#define SHOW_GEN_DBG4		0x80000000

#define SHOW_EVENTS		0x0f000000

/*  *********************************************************************
    *  Macros to muck with bitfields
    ********************************************************************* */

#define _XHCI_MAKE32(x) ((uint32_t)(x))

/* Make a mask for 1 bit at position 'n' */
#define _XHCI_MAKEMASK1(n) (_XHCI_MAKE32(1) << _XHCI_MAKE32(n))

/* Make a mask for 'v' bits at position 'n' */
#define _XHCI_MAKEMASK(v, n) (_XHCI_MAKE32((_XHCI_MAKE32(1)<<(v))-1) << \
			     _XHCI_MAKE32(n))

/* Make a value at 'v' at bit position 'n' */
#define _XHCI_MAKEVALUE(v, n) (_XHCI_MAKE32(v) << _XHCI_MAKE32(n))
#define _XHCI_GETVALUE(v, n, m) ((_XHCI_MAKE32(v) & _XHCI_MAKE32(m)) >> \
				_XHCI_MAKE32(n))

/*  *********************************************************************
    *  Registers
    ********************************************************************* */

#define _XHCI_REGIDX(x)	((x)*4)

#define R_CAPVER		_XHCI_REGIDX(0)
#define R_HCSPARAMS1		_XHCI_REGIDX(1)
#define R_HCSPARAMS2		_XHCI_REGIDX(2)
#define R_HCSPARAMS3		_XHCI_REGIDX(3)
#define R_HCCPARAMS		_XHCI_REGIDX(4)
#define R_DBOFF			_XHCI_REGIDX(5)
#define R_RTSOFF		_XHCI_REGIDX(6)
#define R_HCCPARAMS2		_XHCI_REGIDX(7)
#define R_USBCMD		_XHCI_REGIDX(8)
#define R_USBSTS		_XHCI_REGIDX(9)
#define R_PAGESIZ		_XHCI_REGIDX(10)
#define R_DNCTRL		_XHCI_REGIDX(13)
#define R_CRCRL			_XHCI_REGIDX(14)
#define R_CRCRH			_XHCI_REGIDX(15)
#define R_DCBAAPL		_XHCI_REGIDX(20)
#define R_DCBAAPH		_XHCI_REGIDX(21)
#define R_CONFIG		_XHCI_REGIDX(22)
#define R_PORTSC(x)		(0x410+(16*x))	/* note: 1-based */
#define PORTSC_IDX		(0x400/4)	/* 32 bit index offset */

/* R_CAPVER */
#define S_CAPVER_LEN		0
#define M_CAPVER_LEN		_XHCI_MAKEMASK(8, S_CAPVER_LEN)
#define G_CAPVER_LEN(x)		_XHCI_GETVALUE(x, S_CAPVER_LEN, M_CAPVER_LEN)

#define S_CAPVER_VER		16
#define M_CAPVER_VER		_XHCI_MAKEMASK(16, S_CAPVER_VER)
#define G_CAPVER_VER(x)		_XHCI_GETVALUE(x, S_CAPVER_VER, M_CAPVER_VER)
#define K_XHCI_VER		0x100

/* R_HCSPARAMS1 */
#define S_PARAMS1_NPTS		24
#define M_PARAMS1_NPTS		_XHCI_MAKEMASK(8, S_PARAMS1_NPTS)
#define G_PARAMS1_NPTS(x)	_XHCI_GETVALUE(x, S_PARAMS1_NPTS, \
						M_PARAMS1_NPTS)

#define S_PARAMS1_MSLS		0
#define M_PARAMS1_MSLS		_XHCI_MAKEMASK(8, S_PARAMS1_MSLS)
#define G_PARAMS1_MSLS(x)	_XHCI_GETVALUE(x, S_PARAMS1_MSLS, \
						M_PARAMS1_MSLS)

/* R_HCSPARAMS2 */
#define S_PARAMS2_MSBL		27
#define M_PARAMS2_MSBL		_XHCI_MAKEMASK(5, S_PARAMS2_MSBL)
#define G_PARAMS2_MSBL(x)	_XHCI_GETVALUE(x, S_PARAMS2_MSBL, \
						M_PARAMS2_MSBL)

/* R_HCCPARAMS */
#define M_CPARAMS_AC64		_XHCI_MAKEMASK1(0)
#define M_CPARAMS_CSZ		_XHCI_MAKEMASK1(2)

/* R_USBCMD */
#define M_USBCMD_RUN		_XHCI_MAKEMASK1(0)
#define M_USBCMD_HCR		_XHCI_MAKEMASK1(1)

/* R_USBSTS */
#define M_USBSTS_CHALT		_XHCI_MAKEMASK1(0)
#define M_USBSTS_HSERR		_XHCI_MAKEMASK1(2)
#define M_USBSTS_EINT		_XHCI_MAKEMASK1(3)
#define M_USBSTS_PCD		_XHCI_MAKEMASK1(4)
#define M_USBSTS_CNR		_XHCI_MAKEMASK1(11)
#define M_USBSTS_HCE		_XHCI_MAKEMASK1(12)
#define M_INT_ALL		(M_USBSTS_CHALT | M_USBSTS_HSERR | \
				 M_USBSTS_EINT | M_USBSTS_PCD | \
				 M_USBSTS_CNR | M_USBSTS_HCE)

/* R_PORTSC */
#define M_PORTSC_CCS		_XHCI_MAKEMASK1(0)
#define M_PORTSC_PED		_XHCI_MAKEMASK1(1)
#define M_PORTSC_OCA		_XHCI_MAKEMASK1(3)
#define M_PORTSC_PR		_XHCI_MAKEMASK1(4)
#define M_PORTSC_PP		_XHCI_MAKEMASK1(9)
#define M_PORTSC_PIC		_XHCI_MAKEMASK1(14)
#define M_PORTSC_LWS		_XHCI_MAKEMASK1(16)
#define M_PORTSC_CSC		_XHCI_MAKEMASK1(17)
#define M_PORTSC_PEC		_XHCI_MAKEMASK1(18)
#define M_PORTSC_WRC		_XHCI_MAKEMASK1(19)
#define M_PORTSC_OCC		_XHCI_MAKEMASK1(20)
#define M_PORTSC_PRC		_XHCI_MAKEMASK1(21)
#define M_PORTSC_PLC		_XHCI_MAKEMASK1(22)
#define M_PORTSC_CEC		_XHCI_MAKEMASK1(23)
#define M_PORTSC_CAC		_XHCI_MAKEMASK1(24)
#define M_PORTSC_WPR		_XHCI_MAKEMASK1(31)

#define S_PORTSC_PLS		5
#define M_PORTSC_PLS		_XHCI_MAKEMASK(4, S_PORTSC_PLS)
#define G_PORTSC_PLS(x)		_XHCI_GETVALUE(x, S_PORTSC_PLS, M_PORTSC_PLS)
#define K_PLS_U0		0
#define K_PLS_U1		1
#define K_PLS_U2		2
#define K_PLS_U3		3
#define K_PLS_INACTIVE		4
#define K_PLS_RXDETECT		5
#define K_PLS_DISABLED		6
#define K_PLS_POLLING		7
#define K_PLS_RECOVERY		8
#define K_PLS_HOTRESET		9
#define K_PLS_TXCOMP		0xA
#define K_PLS_RXCOMP		0xB
#define K_PLS_RESUME		0xF

#define S_PORTSC_PS		13
#define M_PORTSC_PS		_XHCI_MAKEMASK(4, S_PORTSC_PS)
#define G_PORTSC_PS(x)		_XHCI_GETVALUE(x, S_PORTSC_PS, M_PORTSC_PS)
#define K_PS_XX			0
#define K_PS_FS			1
#define K_PS_LS			2
#define K_PS_HS			3
#define K_PS_SS			4

#define M_PORTSC_ALLC		(M_PORTSC_CSC | M_PORTSC_PEC | M_PORTSC_WRC | \
				 M_PORTSC_OCC | M_PORTSC_PRC | M_PORTSC_PLC | \
				 M_PORTSC_CEC | M_PORTSC_CAC)
#define M_PORTSC_STAT		0x1fff
#define M_PORTSC_KEEP		(M_PORTSC_PP)
#define M_PORTSC_CHECK		(M_PORTSC_CSC | M_PORTSC_WRC | M_PORTSC_OCC | \
				 M_PORTSC_PRC)
#define M_PORTSC_OTHER		(M_PORTSC_PP | M_PORTSC_PEC | M_PORTSC_PLC | \
				 M_PORTSC_CEC | M_PORTSC_CAC)
#define PORTSC_USB30		2

/* R_ERDP */
#define M_ERDP_EHB		_XHCI_MAKEMASK1(3)


/*  *********************************************************************
    *  XHCI Structures
    ********************************************************************* */

#define CTL_MPS		512
#define CERR		3


enum {	/* Transfer Ring TRBs */
	NORM = 1, CTL_SETUP, CTL_DATA, CTL_STATUS,	/*  0..8 */
	ISOCH, LINK, EV_DATA, NO_OP };
enum {	/* Command Ring TRBs...LINK allowed too */
	EN_SLOT = 9, DIS_SLOT, ADDR_DEV, CONFIG,	/*  9..12 */
	EVAL_CTX, RESET_EP, STOP_EP,			/* 13..15 */
	SET_TR_DEQ, RESET_DEV, FORCE_EV, NEG_BW,	/* 16..19 */
	SET_LAT_TOL, GET_BW, FORCE_HDR, NO_OP_CMD,	/* 20..23 */
	};
enum {	/* Event Ring TRBs */
	XFER = 32, CMD_COMP, PSC, BW_REQ, DB, HC, DEV_NOTIF, MFINDEX_WRAP
	};

typedef struct trb_s {
	uint32_t	dw[4];
} trb_t;

#define M_TRB_CYC		_XHCI_MAKEMASK1(0)
#define M_TRB_TC		_XHCI_MAKEMASK1(1)
#define M_TRB_IDT		_XHCI_MAKEMASK1(6)
#define M_TRB_IOC		_XHCI_MAKEMASK1(5)
#define M_TRB_ISP		_XHCI_MAKEMASK1(2)
#define M_TRB_ENT		_XHCI_MAKEMASK1(1)
#define M_TRB_CH		_XHCI_MAKEMASK1(4)
#define M_TRB_DC		_XHCI_MAKEMASK1(9)
#define M_TRB_BSR		_XHCI_MAKEMASK1(9)
#define M_TRB_EP0_DIR_IN	_XHCI_MAKEMASK1(16)
#define M_TRB_EP0_DIR_OUT	0

#define S_TRB_CMD		10
#define M_TRB_CMD		_XHCI_MAKEMASK(6, S_TRB_CMD)
#define V_TRB_CMD(x)		_XHCI_MAKEVALUE(x, S_TRB_CMD)
#define G_TRB_CMD(x)		_XHCI_GETVALUE(x, S_TRB_CMD, M_TRB_CMD)

#define S_TRB_TRT		16	/* SETUP TRB */
#define M_TRB_TRT		_XHCI_MAKEMASK(2, S_TRB_TRT)
#define V_TRB_TRT(x)		_XHCI_MAKEVALUE(x, S_TRB_TRT)
#define G_TRB_TRT(x)		_XHCI_GETVALUE(x, S_TRB_TRT, M_TRB_TRT)
enum { NO_DATA, TRT_RSVD, OUT_DATA, IN_DATA };

typedef struct event_trb_s {
	uint32_t	trbl;
	uint32_t	trbh;
	uint32_t	cc_len;		/* cc=31:24; len=23:0 */
	uint32_t	ctrl;
} event_trb_t;

#define G_TRB_CC(x)	(((uint32_t) ((uint32_t *) (x))[2] >> 24) & 0xff)
#define G_TRB_LEN(x)	((x) & 0xffffff)
#define G_TRB_ID(x)	(((uint32_t) ((uint32_t *) (x))[3] >> 10) & 0x3f)
enum {
	INVALID, SUCCESS, DATA_BUF_ERR, BABBLE,
	XACT_ERR, TRB_ERR, STALL, RESOURCE_ERR,
	BW_ERR, NO_SLOTS, INV_STREAM_TYPE, SLOT_NOT_EN,
	EP_NOT_ENAB, SHORT_PKT, RING_UNDERRUN, RING_OVRRUN,
	VF_EV_RING_FULL, PARAM_ERR, BW_OVR_ERR, CTX_STATE_ERR,
	NO_PING_RESP, EV_RING_FULL, INCOMPAT_DEV, MISSED_SVC,
	CMD_ABORTED, STOPPED, STOPPED_LEN, STOP_SHORT_PKT,
	MAX_EXIT_LAT_HI, RSVD_ERR, ISOC_OVRRUN, EV_LOST,
	UNDEFINED, INV_STREAM_ID, SEC_BW_ERR, SPL_XACT_ERR
	};

static char *TRB_CC_CODE[] = {
	"INVALID", "SUCCESS", "DATA_BUF_ERR", "BABBLE",
	"XACT_ERR", "TRB_ERR", "STALL", "RESOURCE_ERR",
	"BW_ERR", "NO_SLOTS", "INV_STREAM_TYPE", "SLOT_NOT_EN",
	"EP_NOT_ENAB", "SHORT_PKT", "RING_UNDERRUN", "RING_OVRRUN",
	"VF_EV_RING_FULL", "PARAM_ERR", "BW_OVR_ERR", "CTX_STATE_ERR",
	"NO_PING_RESP", "EV_RING_FULL", "INCOMPAT_DEV", "MISSED_SVC",
	"CMD_ABORTED", "STOPPED", "STOPPED_LEN", "STOP_SHORT_PKT",
	"MAX_EXIT_LAT_HI", "RSVD_ERR", "ISOC_OVRRUN", "EV_LOST",
	"UNDEFINED", "INV_STREAM_ID", "SEC_BW_ERR", "SPL_XACT_ERR"
};

static char *TRB_TYPE[64] = {
	"Rsvd", "Norm", "Setup", "Data",	/* 0..3   */
	"Status", "Isoch", "Link", "EvData",	/* 4..7   */
	"Noop", "EnSlot", "DisSlot", "AddrDev",	/* 8..11  */
	"Config", "EvalCtx", "RstEP", "StopEP",	/* 12..15 */
	"TrDeq", "RstDev", "FrcEv", "NegBW",	/* 16..19 */
	"SetLat", "GetBW", "FrcHdr", "Noop",	/* 20..23 */
	"Rsvd", "Rsvd", "Rsvd", "Rsvd",		/* 24..27 */
	"Rsvd", "Rsvd", "Rsvd", "Rsvd",		/* 28..31 */
	"Xfer", "CmdComp", "PSC", "BWReq",	/* 32..35 */
	"DB", "HC", "DevNotf", "MFIdx",		/* 36..39 */
	"Rsvd", "Rsvd", "Rsvd", "Rsvd",		/* 40..43 */
	"Rsvd", "Rsvd", "Rsvd", "Rsvd",		/* 44..47 */
	"Vend", "Vend", "Vend", "Vend",		/* 48..51 */
	"Vend", "Vend", "Vend", "Vend",		/* 52..55 */
	"Vend", "Vend", "Vend", "Vend",		/* 56..59 */
	"Vend", "Vend", "Vend", "Vend",		/* 60..63 */
};

typedef struct ring_s {
	trb_t			*enq;
	trb_t			*deq;
	volatile uint32_t	cyc;
	trb_t			*base;
} ring_t;

typedef struct erst_s {
	volatile uint64_t	addr;
	volatile uint32_t	size;
	uint32_t		rsvd;
} erst_t;

typedef struct irs_s {
	volatile uint32_t	iman;
	volatile uint32_t	imod;
	volatile uint32_t	erstsz;
	volatile uint32_t	rsvd;
	volatile uint64_t	erstba;
	volatile uint64_t	erdp;
} irs_t;

typedef struct rts_s {
	volatile uint32_t	mfindex;
	uint32_t		rsvdZ[7];
	irs_t			irs[1024];
} rts_t;

typedef struct dba_s {
	volatile uint32_t	*target;
} dba_t;

typedef struct port_s {
	volatile uint32_t	sc;
	volatile uint32_t	pmsc;
	volatile uint32_t	li;
	volatile uint32_t	hlpmc;
} port_t;
#define PORT_OFS	(sizeof(port_t) / sizeof(uint32_t))

/* Device structures...64 byte contexts only */
typedef struct slot_ctx_s {
	volatile uint32_t	info1;
	volatile uint32_t	info2;
	volatile uint32_t	tt;
	volatile uint32_t	state;
	uint32_t		rsvd1[4];
#if CONTEXT_SIZE_64
	uint32_t		rsvd2[8];
#endif
} slot_ctx_t;

typedef struct ep_ctx_s {
	volatile uint32_t	info1;
	volatile uint32_t	info2;
	volatile uint64_t	deq;
	volatile uint32_t	info3;
	uint32_t		rsvd1[3];
#if CONTEXT_SIZE_64
	uint32_t		rsvd2[8];
#endif
} ep_ctx_t;
#define M_DCS		1		/* TRB ring deq */

typedef struct dev_ctx_s {
	slot_ctx_t	slot;
	ep_ctx_t	ep[31];
} dev_ctx_t;
/* map endpoint according device context index...Fig 73 */
#define V_EP_IDX(e, i)	(((e) * 2) + (i))

typedef struct inp_ctl_ctx_s {
	volatile uint32_t	drop;
	volatile uint32_t	add;
	uint32_t		rsvd1[6];
#if CONTEXT_SIZE_64
	uint32_t		rsvd2[8];
#endif
} inp_ctl_ctx_t;

typedef struct inp_ctx_s {
	inp_ctl_ctx_t		ctl;
	dev_ctx_t		dctx;
} inp_ctx_t;

typedef struct dev_s {
	inp_ctx_t	*ictx;
	ring_t		epr[MAX_EP];	/* EP rings */
	int		addr;		/* usbd's addr as internal id */
	int		rport;		/* root port */
	int		speed;
	int		slot;
	int		hub;
	uint32_t	route;		/* if via external hub */
	int		pipes;
	int		config;
	int		error;
} dev_t;

typedef struct xhci_s {
	/* xHCI registers */
	volatile uint32_t	*base;
	volatile uint32_t	*caps;
	volatile uint32_t	*ops;
	port_t			*port;
	rts_t			*rts;
	volatile uint32_t	*db;
	volatile uint64_t	*pDCBAA;

	/* xHCI data structures */
	erst_t			*pERST;
	ring_t			cmdr;
	ring_t			evr;
	volatile uint8_t	*slots[MAX_SLOTS];
	dev_t			devs[MAX_SLOTS];
	void		*scratch_buf;	/* holds address of array */
	volatile uint64_t	scratch[MAX_SCRATCH];
	int			nslots;
	int			ndp;
	int			ndevs;

	event_trb_t		last_cmd_event;
} xhci_t;

typedef struct transfer_s {
	void			*t_ref;
	int			t_len;
	struct endpoint_s	*ept;	/* cross-link */
} transfer_t;

typedef struct endpoint_s {
	struct endpoint_s *next;
	int		flags;
	int		mps;
	int		num;
	int		slot;
	int		addr;
	int		error;
	dev_t		*dev;
	transfer_t	*xfer;
} endpoint_t;

typedef struct xhci_softc_s {
	xhci_t		*hc;

	endpoint_t	*endpoint_pool;
	trb_t		*trb_pool;
	transfer_t	*transfer_pool;
	endpoint_t	*endpoint_freelist;
	trb_t		*trb_freelist;

	physaddr_t	regs;
	int		block_pcd;
	int		pending_pcd;
	int		error;

	int		rh_newaddr;	/* to be set on next status update */
	int		rh_addr;	/* address of root hub */
	int		rh_conf;	/* current configuration # */
	uint8_t		rh_buf[128];	/* buffer to hold hub responses */
	uint8_t		*rh_ptr;	/* pointer into buffer */
	int		rh_len;		/* remaining bytes to transfer */
	int		rh_enum_port;	/* port being enumerated */
	queue_t		rh_intrq;	/* Interrupt request queue */
	usbbus_t	*bus;		/* owning usbbus structure */

	int		eptcnt;		/* debug stats */
	int		ctxcnt;
	int		ringcnt;
} xhci_softc_t;

