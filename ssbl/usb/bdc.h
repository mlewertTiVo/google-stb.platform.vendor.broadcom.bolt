/***************************************************************************
 *     Copyright (c) 2012-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#define TRACE()     printf("%s: @%d\n", __func__, __LINE__)
#define FTRACE()    {if (Bdc_debug & DBG_FUNC_TRACE) printf("%s: @%d\n", \
			__func__, __LINE__); }

/*  *********************************************************************
    *  Some debug variable definitions
    ********************************************************************* */

#define DBG_CMD			0x00000001
#define DBG_BD_QUEUE		0x00000002
#define DBG_BD_RING		0x00000004
#define DBG_BD_STATS		0x00000008
#define DBG_PSC1		0x00000010
#define DBG_PSC2		0x00000020
#define DBG_CTL_STATE		0x00000040
#define DBG_CTL_DATA		0x00000080
#define DBG_CTL_SETUP		0x00000100
#define DBG_CTL_IN		0x00000200
#define DBG_CTL_OUT		0x00000400
#define DBG_CTL_STATUS		0x00000800
#define DBG_BULK_IN		0x00001000
#define DBG_BULK_IN2		0x00002000
#define DBG_BULK_OUT		0x00004000
#define DBG_BULK_OUT2		0x00008000
#define DBG_EVENT1		0x00010000
#define DBG_EVENT2		0x00020000
#define DBG_EVENT3		0x00040000
#define DBG_EVENT4		0x00080000
#define DBG_INIT1		0x00100000
#define DBG_INIT2		0x00200000
#define DBG_GEN1		0x01000000
#define DBG_GEN2		0x02000000
#define DBG_GEN3		0x04000000
#define DBG_DB			0x08000000
#define DBG_LPBK_MODE		0x40000000
#define DBG_FUNC_TRACE		0x80000000


/*  *********************************************************************
    *  Macros to muck with bitfields
    ********************************************************************* */

#define _BDC_MAKE32(x) ((uint32_t)(x))

/* Make a mask for 1 bit at position 'n' */
#define _BDC_MAKEMASK1(n) (_BDC_MAKE32(1) << _BDC_MAKE32(n))

/* Make a mask for 'v' bits at position 'n' */
#define _BDC_MAKEMASK(v, n) (_BDC_MAKE32((_BDC_MAKE32(1)<<(v))-1) <<	\
				_BDC_MAKE32(n))

/* Make a value with 'v' at bit position 'n' */
#define _BDC_MAKEVALUE(v, n) (_BDC_MAKE32(v) << _BDC_MAKE32(n))

/* Get a value from 'v' at bit position 'n' and mask 'm' */
#define _BDC_GETVALUE(v, n, m) ((_BDC_MAKE32(v) & _BDC_MAKE32(m)) >>	\
					_BDC_MAKE32(n))


/*  *********************************************************************
    *  Misc definitions
    ********************************************************************* */

#define LSB(x)			((x) & 0xff)
#define MSB(x)			(((x) & 0xff00) >> 8)

#define SRR0_SIZE		5		/* 2^(n+1) = 64 entries */
#define SRR0_ALIGN		16		/* bytes */

#define EP0BUF_SIZE		4096
#define EP0_MPS			64
#define EP0_NUM_BD		128
#define BULK_MPS		512
#define BULK_MPS_FS		64
#define BULKIN_EP		EP1IN
#define BULKIN_NUM_BD		128
#define BULKOUT_EP		EP2OUT
#define BULKOUT_NUM_BD		128

#define DEV_DESC_SIZE		18
#define CFG_DESC_SIZE		1024		/* change as needed */
#define MAX_STR_DESC_SIZE	256		/* max possible */
#define BDC_CMD_TIMEOUT		50		/* mS...change as needed */
#define BDC_COMM_TIMEOUT	100		/* mS...change as needed */
#define MAX_BULKOUT_LEN		65536		/* BD size */
#define MAX_LPBK_LEN		MAX_BULKOUT_LEN

/*  *********************************************************************
    *  Registers
    ********************************************************************* */

#define R_BDC_BDCCFG0		0x00
#define R_BDC_BDCCFG1		0x04
#define R_BDC_BDCCAP0		0x08
#define R_BDC_BDCCAP1		0x0c
#define R_BDC_CMDPAR0		0x10
#define R_BDC_CMDPAR1		0x14
#define R_BDC_CMDPAR2		0x18
#define R_BDC_CMDSC		0x1c
#define R_BDC_USPSC		0x20
#define R_BDC_USPPM2		0x2c
#define R_BDC_SPBBAL		0x38
#define R_BDC_SPBBAH		0x3c
#define R_BDC_BDCSC		0x40
#define R_BDC_MFNUM		0x48
#define R_BDC_XSFNTF		0x4c
#define R_BDC_DVCSA		0x50
#define R_BDC_DVCSB		0x54
#define R_BDC_SRRBAL0		0x200
#define R_BDC_SRRBAH0		0x204
#define R_BDC_SRRINT0		0x208
#define R_BDC_INTCLS0		0x20c

/* CFG0 defns */
#define S_CFG0_SPB		0
#define M_CFG0_SPB		_BDC_MAKEMASK(4, S_CFG0_SPB)
#define G_CFG0_SPB(x)		_BDC_GETVALUE(x, S_CFG0_SPB, M_CFG0_SPB)

/* CMDSC defns */
#define S_CMDPAR2_TYP		22
#define M_CMDPAR2_TYP		_BDC_MAKEMASK(2, S_CMDPAR2_TYP)
#define V_CMDPAR2_TYP(x)	_BDC_MAKEVALUE(x, S_CMDPAR2_TYP)
#define G_CMDPAR2_TYP(x)	_BDC_GETVALUE(x, S_CMDPAR2_TYP, M_CMDPAR2_TYP)
enum {CTRL, ISOC, BULK, INTR};
#define S_CMDPAR2_MPS		10
#define M_CMDPAR2_MPS		_BDC_MAKEMASK(11, S_CMDPAR2_MPS)
#define V_CMDPAR2_MPS(x)	_BDC_MAKEVALUE(x, S_CMDPAR2_MPS)
#define G_CMDPAR2_MPS(x)	_BDC_GETVALUE(x, S_CMDPAR2_MPS, M_CMDPAR2_MPS)

/* CMDSC defns */
#define S_CMDSC_CID		28
#define M_CMDSC_CID		_BDC_MAKEMASK(4, S_CMDSC_CID)
#define V_CMDSC_CID(x)		_BDC_MAKEVALUE(x, S_CMDSC_CID)
#define G_CMDSC_CID(x)		_BDC_GETVALUE(x, S_CMDSC_CID, M_CMDSC_CID)
#define M_CMDSC_SRD		_BDC_MAKEMASK1(27)
#define S_CMDSC_SRR		20
#define M_CMDSC_SRR		_BDC_MAKEMASK(5, S_CMDSC_SRR)
#define V_CMDSC_SRR(x)		_BDC_MAKEVALUE(x, S_CMDSC_SRR)
#define G_CMDSC_SRR(x)		_BDC_GETVALUE(x, S_CMDSC_SRR, M_CMDSC_SRR)
#define S_CMDSC_SUB		17
#define M_CMDSC_SUB		_BDC_MAKEMASK(3, S_CMDSC_SUB)
#define V_CMDSC_SUB(x)		_BDC_MAKEVALUE(x, S_CMDSC_SUB)
#define G_CMDSC_SUB(x)		_BDC_GETVALUE(x, S_CMDSC_SUB, M_CMDSC_SUB)
enum {ADR = 1, PAR};		/* for DVC */
enum {ACF = 1, DCF};		/* for EPC */
enum {RST = 1, STP, STL = 4, STS};	/* for EPO */
#define M_CMDSC_SNR		_BDC_MAKEMASK1(16)
#define S_CMDSC_EPN		10
#define M_CMDSC_EPN		_BDC_MAKEMASK(5, S_CMDSC_EPN)
#define V_CMDSC_EPN(x)		_BDC_MAKEVALUE(x, S_CMDSC_EPN)
#define G_CMDSC_EPN(x)		_BDC_GETVALUE(x, S_CMDSC_EPN, M_CMDSC_EPN)
#define S_CMDSC_CST		6
#define M_CMDSC_CST		_BDC_MAKEMASK(4, S_CMDSC_CST)
#define V_CMDSC_CST(x)		_BDC_MAKEVALUE(x, S_CMDSC_CST)
#define G_CMDSC_CST(x)		_BDC_GETVALUE(x, S_CMDSC_CST, M_CMDSC_CST)
enum {IDLE, SUCC, UNKW, PARA, STAT, FAIL, INTL, ABTD, BUSY = 15};
#define M_CMDSC_CWS		_BDC_MAKEMASK1(5)
#define S_CMDSC_CMD		0
#define M_CMDSC_CMD		_BDC_MAKEMASK(3, S_CMDSC_CMD)
#define V_CMDSC_CMD(x)		_BDC_MAKEVALUE(x, S_CMDSC_CMD)
#define G_CMDSC_CMD(x)		_BDC_GETVALUE(x, S_CMDSC_CMD, M_CMDSC_CMD)
enum {NOP, DVC, EPC, BLA, EPO, DNC = 6, ABT = 0xf};

/* MFNUM defns */
#define M_MFNUM_VLD		_BDC_MAKEMASK1(31)
#define S_MFNUM_MFC		0
#define M_MFNUM_MFC		_BDC_MAKEMASK(14, S_MFNUM_MFC)
#define G_MFNUM_MFC(x)		_BDC_GETVALUE(x, S_MFNUM_MFC, M_MFNUM_MFC)

/* USPSC defns */
#define M_USPSC_VBC		_BDC_MAKEMASK1(31)
#define M_USPSC_PRC		_BDC_MAKEMASK1(30)
#define M_USPSC_PCE		_BDC_MAKEMASK1(29)
#define M_USPSC_CFC		_BDC_MAKEMASK1(28)
#define M_USPSC_PCC		_BDC_MAKEMASK1(27)
#define M_USPSC_PSC		_BDC_MAKEMASK1(26)
#define M_USPSC_VBS		_BDC_MAKEMASK1(25)
#define M_USPSC_PRS		_BDC_MAKEMASK1(24)
#define M_USPSC_PCS		_BDC_MAKEMASK1(23)
#define S_USPSC_PSP		20
#define M_USPSC_PSP		_BDC_MAKEMASK(3, S_USPSC_PSP)
#define V_USPSC_PSP(x)		_BDC_MAKEVALUE(x, S_USPSC_PSP)
#define G_USPSC_PSP(x)		_BDC_GETVALUE(x, S_USPSC_PSP, M_USPSC_PSP)
enum {PSP_NC, PSP_FS, PSP_LS, PSP_HS, PSP_SS};
#define PORT_ENABLE		5
#define M_USPSC_SCN		_BDC_MAKEMASK1(8)
#define M_USPSC_SDC		_BDC_MAKEMASK1(7)
#define M_USPSC_SWS		_BDC_MAKEMASK1(4)
#define S_USPSC_PST		0
#define M_USPSC_PST		_BDC_MAKEMASK(4, S_USPSC_PST)
#define V_USPSC_PST(x)		_BDC_MAKEVALUE(x, S_USPSC_PST)
#define G_USPSC_PST(x)		_BDC_GETVALUE(x, S_USPSC_PST, M_USPSC_PST)
#define M_USPSC_ALLC		(M_USPSC_VBC|M_USPSC_PRC|M_USPSC_PCE| \
				 M_USPSC_CFC|M_USPSC_PCC|M_USPSC_PSC)

/* BDCSC defns */
#define S_BDCSC_COP		29
#define M_BDCSC_COP		_BDC_MAKEMASK(3, S_BDCSC_COP)
#define V_BDCSC_COP(x)		_BDC_MAKEVALUE(x, S_BDCSC_COP)
#define G_BDCSC_COP(x)		_BDC_GETVALUE(x, S_BDCSC_COP, M_BDCSC_COP)
#define SC_RST			1
#define SC_RUN			2
#define SC_STP			4
#define M_BDCSC_COS		_BDC_MAKEMASK1(28)
#define S_BDCSC_STS		20
#define M_BDCSC_STS		_BDC_MAKEMASK(3, S_BDCSC_STS)
#define V_BDCSC_STS(x)		_BDC_MAKEVALUE(x, S_BDCSC_STS)
#define G_BDCSC_STS(x)		_BDC_GETVALUE(x, S_BDCSC_STS, M_BDCSC_STS)
#define HALTED			1
#define NORMAL			2
#define OIP			7
#define M_BDCSC_SRMMCW		_BDC_MAKEMASK1(7)
#define M_BDCSC_GIE		_BDC_MAKEMASK1(1)
#define M_BDCSC_GIP		_BDC_MAKEMASK1(0)

/* XSFNTF defns */
#define S_XSFNTF_EPN		0
#define M_XSFNTF_EPN		_BDC_MAKEMASK(5, S_XSFNTF_EPN)
#define V_XSFNTF_EPN(x)		_BDC_MAKEVALUE(x, S_XSFNTF_EPN)
#define G_XSFNTF_EPN(x)		_BDC_GETVALUE(x, S_XSFNTF_EPN, M_XSFNTF_EPN)
enum {EP0 = 1, EP1OUT, EP1IN, EP2OUT, EP2IN};

/* SRRBAL0 defns */
#define S_SRRBAL0_BAL		4
#define M_SRRBAL0_BAL		_BDC_MAKEMASK(28, S_SRRBAL0_BAL)
#if 0
#define V_SRRBAL0_BAL(x)	_BDC_MAKEVALUE(x, S_SRRBAL0_BAL)
#define G_SRRBAL0_BAL(x)	_BDC_GETVALUE(x, S_SRRBAL0_BAL, M_SRRBAL0_BAL)
#endif
#define M_SRRBAL0_SWS		_BDC_MAKEMASK1(3)
#define S_SRRBAL0_SIZ		0
#define M_SRRBAL0_SIZ		_BDC_MAKEMASK(3, S_SRRBAL0_SIZ)
#define V_SRRBAL0_SIZ(x)	_BDC_MAKEVALUE(x, S_SRRBAL0_SIZ)
#define G_SRRBAL0_SIZ(x)	_BDC_GETVALUE(x, S_SRRBAL0_SIZ, M_SRRBAL0_SIZ)

/* SRRINT defns */
#define S_SRRINT_EQP		24
#define M_SRRINT_EQP		_BDC_MAKEMASK(8, S_SRRINT_EQP)
#define V_SRRINT_EQP(x)		_BDC_MAKEVALUE(x, S_SRRINT_EQP)
#define G_SRRINT_EQP(x)		_BDC_GETVALUE(x, S_SRRINT_EQP, M_SRRINT_EQP)
#define S_SRRINT_DQP		16
#define M_SRRINT_DQP		_BDC_MAKEMASK(8, S_SRRINT_DQP)
#define V_SRRINT_DQP(x)		_BDC_MAKEVALUE(x, S_SRRINT_DQP)
#define G_SRRINT_DQP(x)		_BDC_GETVALUE(x, S_SRRINT_DQP, M_SRRINT_DQP)
#define M_SRRINT_RST		_BDC_MAKEMASK1(3)
#define M_SRRINT_ISR		_BDC_MAKEMASK1(2)
#define M_SRRINT_IE		_BDC_MAKEMASK1(1)
#define M_SRRINT_IP		_BDC_MAKEMASK1(0)

/* INTCLS defns */
#define S_INTCLS_ICC		16
#define M_INTCLS_ICC		_BDC_MAKEMASK(16, S_INTCLS_ICC)
#define V_INTCLS_ICC(x)		_BDC_MAKEVALUE(x, S_INTCLS_ICC)
#define G_INTCLS_ICC(x)		_BDC_GETVALUE(x, S_INTCLS_ICC, M_INTCLS_ICC)
#define S_INTCLS_ICI		0
#define M_INTCLS_ICI		_BDC_MAKEMASK(16, S_INTCLS_ICI)
#define V_INTCLS_ICI(x)		_BDC_MAKEVALUE(x, S_INTCLS_ICI)
#define G_INTCLS_ICI(x)		_BDC_GETVALUE(x, S_INTCLS_ICI, M_INTCLS_ICI)

/* DRD_STATUS defns */
#define S_DRD_STATE		0
#define M_DRD_STATE		_BDC_MAKEMASK(2, S_DRD_STATE)
#define G_DRD_STATE(x)		_BDC_GETVALUE(x, S_DRD_STATE, M_DRD_STATE)
#define DEV_MODE		2

/*  *********************************************************************
    *  Status Report
    ********************************************************************* */

typedef struct bdc_sr_s {
	uint32_t	bph;
	uint32_t	bpl;
	uint32_t	len;
	uint32_t	dw3;
} bdc_sr_t;

enum {
	SR_XFR, SR_CMD, SR_BIA, SR_MCW,		/* Status Report Types */
	SR_PSC, SR_BDC, SR_TNE, SR_BDE,
	SR_MAX
};

enum {
	STS_SUC = 1, STS_BNA, STS_SHT, STS_BAB,	/* Transfer Status */
	STS_STP, STS_SPR, STS_DSS, STS_SSS,
	STS_SIB, STS_UTE = 11, STS_MAX
};

/* DW2...len, etc */
#define S_SR_LMD			24
#define M_SR_LMD			_BDC_MAKEMASK(2, S_SR_LMD)
#define V_SR_LMD(x)			_BDC_MAKEVALUE(x, S_SR_LMD)
#define G_SR_LMD(x)			_BDC_GETVALUE(x, S_SR_LMD, M_SR_LMD)
#define S_SR_LEN			0
#define M_SR_LEN			_BDC_MAKEMASK(24, S_SR_LEN)
#define V_SR_LEN(x)			_BDC_MAKEVALUE(x, S_SR_LEN)
#define G_SR_LEN(x)			_BDC_GETVALUE(x, S_SR_LEN, M_SR_LEN)

/* DW3 */
#define S_SR_STS			28
#define M_SR_STS			_BDC_MAKEMASK(4, S_SR_STS)
#define V_SR_STS(x)			_BDC_MAKEVALUE(x, S_SR_STS)
#define G_SR_STS(x)			_BDC_GETVALUE(x, S_SR_STS, M_SR_STS)
#define S_SR_MFC			10
#define M_SR_MFC			_BDC_MAKEMASK(14, S_SR_MFC)
#define V_SR_MFC(x)			_BDC_MAKEVALUE(x, S_SR_MFC)
#define G_SR_MFC(x)			_BDC_GETVALUE(x, S_SR_MFC, M_SR_MFC)
#define M_SR_R				_BDC_MAKEMASK1(9)
#define S_SR_EPN			4
#define M_SR_EPN			_BDC_MAKEMASK(5, S_SR_EPN)
#define V_SR_EPN(x)			_BDC_MAKEVALUE(x, S_SR_EPN)
#define G_SR_EPN(x)			_BDC_GETVALUE(x, S_SR_EPN, M_SR_EPN)
#define S_SR_XSF			0
#define M_SR_XSF			_BDC_MAKEMASK(4, S_SR_XSF)
#define V_SR_XSF(x)			_BDC_MAKEVALUE(x, S_SR_XSF)
#define G_SR_XSF(x)			_BDC_GETVALUE(x, S_SR_XSF, M_SR_XSF)


/*  *********************************************************************
    *  Buffer Descriptor
    ********************************************************************* */

#define BDC_BD_ALIGN		32
#define BDC_BD_MAX_BUF		(64*1024)
#define BDC_BD_MAX_DATA		(256*1024*1024)
#define BD_SIZE			sizeof(bdc_bd_t)
#define BDC_BD_INCR		(BD_SIZE/sizeof(uint32_t))

typedef struct bdc_bd_s {
	uint32_t dpl;
	uint32_t dph;
	uint32_t srrlen;
	uint32_t ctl;
} bdc_bd_t;

/* DW2 */
#define M_BD_LTF		_BDC_MAKEMASK1(25)

/* DW3 */
#define M_BD_SBF		_BDC_MAKEMASK1(31)
#define M_BD_IOC		_BDC_MAKEMASK1(30)
#define M_BD_ISP		_BDC_MAKEMASK1(29)
#define M_BD_ACC		_BDC_MAKEMASK1(28)
#define M_BD_EOT		_BDC_MAKEMASK1(27)
#define M_BD_SOT		_BDC_MAKEMASK1(26)
#define M_BD_DIRIN		_BDC_MAKEMASK1(25)
#define S_BD_TYP		0
#define M_BD_TYP		_BDC_MAKEMASK(4, S_BD_TYP)
#define V_BD_TYP(x)		_BDC_MAKEVALUE(x, S_BD_TYP)
#define G_BD_TYP(x)		_BDC_GETVALUE(x, S_BD_TYP, M_BD_TYP)
enum {XBD, DBD, SBD, CBD = 15};
#define S_BD_TFS		4
#define M_BD_TFS		_BDC_MAKEMASK(21, S_BD_TFS)
#define V_BD_TFS(x)		_BDC_MAKEVALUE(x, S_BD_TFS)
#define G_BD_TFS(x)		_BDC_GETVALUE(x, S_BD_TFS, M_BD_TFS)


/*  *********************************************************************
    *  USB Chapter 9 stuff
    ********************************************************************* */

typedef struct usb_setup_s {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
} usb_setup_t;

enum {DEFAULT, ATTACHED, RESET, ADDRESS, ADDRESSED,
	  CONFIG, CONFIGURED, SUSPEND};
enum {WAIT_FOR_SETUP, WAIT_FOR_DATA_IN, WAIT_FOR_DATA_OUT,
	  WAIT_FOR_STATUS_IN, WAIT_FOR_STATUS_OUT
};

typedef struct usb_descriptors_s {
	usb_device_descr_t	dev;
	usb_config_descr_t	cfg;
	usb_interface_descr_t	intf;
	usb_endpoint_descr_t	bulkin_ep;
	usb_endpoint_descr_t	bulkout_ep;
} usb_descriptors_t;
#define TOTAL_CONFIG_LEN	(sizeof(usb_descriptors_t) - \
				 sizeof(usb_device_descr_t))


/*  *********************************************************************
    *  BDC bulk loopback test feature
    ********************************************************************* */

typedef struct bdc_lpbk_s {
	uint8_t		buf[4096];
	int		len;
	int		on;
} bdc_lpbk_t;


/*  *********************************************************************
    *  BDC softc
    ********************************************************************* */

typedef struct bdc_softc {
	physaddr_t		regs;
	bdc_sr_t		*srr0;
	uint32_t		srr0_dq;
	uint32_t		srr0_size;
	bdc_bd_t		*ep0_ring;
	bdc_bd_t		*ep0_enq;
	bdc_bd_t		*bulkin_ring;
	bdc_bd_t		*bulkin_enq;
	int			bulkin_done;
	bdc_bd_t		*bulkout_ring;
	bdc_bd_t		*bulkout_enq;
	uint8_t			*bulkout_buf;
	int			bulkout_len;
	int			bulkout_queued;
	int			bulkout_rdy;

	uint32_t		*scrpad;
	int			cid;
	int			need_zlp;

	usb_setup_t		setup;
	int			ep0_state;
	int			dev_state;
	int			dev_speed;
	int			config_val;

	usb_descriptors_t	*desc;
	uint8_t			str_desc[MAX_STR_DESC_SIZE];

	int			debug;
	bdc_lpbk_t		lpbk;

	physaddr_t		drd_status;
} bdc_softc;


/*  *********************************************************************
    *  Print strings
    ********************************************************************* */

static char *SPEED_ST[] = {
	"NC", "FS", "LS", "HS", "SS",
	"NC", "NC", "NC", "NC", "NC"
};

static char *SR_ST[] = {
	"XFR", "CMD", "BIA", "MCW", "PSC", "BDC", "TNE", "BDE"
};

static char *STS_ST[] = {
	"000", "SUC", "BNA", "SHT", "BAB",
	"STP", "SPR", "DSS", "SSS", "SIB",
	"AAA", "UTE"
};

static char *DEV_STATE[] = {
	"DEFAULT", "ATTACHED", "RESET", "ADDRESS",
	"ADDRESSED", "CONFIG", "CONFIGURED", "SUSPEND"
};

static char *EP0_STATE[] = {
	"WAIT_FOR_SETUP", "WAIT_FOR_DATA_IN", "WAIT_FOR_DATA_OUT",
	"WAIT_FOR_STATUS_IN", "WAIT_FOR_STATUS_OUT"
};


/*  *********************************************************************
    *  USB descriptors
    ********************************************************************* */

#if defined(CONFIG_DEVICE_VENDOR_ID) && defined(CONFIG_DEVICE_PRODUCT_ID)
	#define DEVICE_VENDOR_ID CONFIG_DEVICE_VENDOR_ID
	#define DEVICE_PRODUCT_ID CONFIG_DEVICE_PRODUCT_ID
#else
	#define DEVICE_VENDOR_ID	0x18D1		/* Google */
	#define DEVICE_PRODUCT_ID	0x4EE0		/* nexus four */
#endif

/* String 0 is the language id */
#define DEVICE_STRING_PRODUCT_INDEX		1
#define DEVICE_STRING_SERIAL_NUMBER_INDEX	2
#define DEVICE_STRING_CONFIG_INDEX		3
#define DEVICE_STRING_INTERFACE_INDEX		4
#define DEVICE_STRING_MANUFACTURER_INDEX	5
#define DEVICE_STRING_MAX_INDEX			DEVICE_STRING_MANUFACTURER_INDEX
#define DEVICE_STRING_LANGUAGE_ID		0x0409		/* English */
#define DEVICE_STRING_PRODUCT			"BDC device"
#define DEVICE_STRING_SERIAL_NUMBER		"S/N: #######"
#define DEVICE_STRING_CONFIG			"Config string"
#define DEVICE_STRING_INTERFACE			"I/F string"
#define DEVICE_STRING_MANUFACTURER		"Broadcom Corp"

#define DEVICE_INTERFACE_CLASS			0xff
#define DEVICE_INTERFACE_SUB_CLASS		0x42
#define DEVICE_INTERFACE_PROTOCOL		0x03

static usb_descriptors_t bdc_descriptors = {
	.dev = {
		.bLength = sizeof(usb_device_descr_t),
		.bDescriptorType = USB_DEVICE_DESCRIPTOR_TYPE,
		USBWORD(0x200),				/* bcdUSB */
		.bDeviceClass = 0x00,
		.bDeviceSubClass = 0x00,
		.bDeviceProtocol = 0x00,
		.bMaxPacketSize0 = 64,
		USBWORD(DEVICE_VENDOR_ID),
		USBWORD(DEVICE_PRODUCT_ID),
		USBWORD(0x100),				/* bcdDevice */
		.iManufacturer = DEVICE_STRING_MANUFACTURER_INDEX,
		.iProduct = DEVICE_STRING_PRODUCT_INDEX,
		.iSerialNumber = DEVICE_STRING_SERIAL_NUMBER_INDEX,
		.bNumConfigurations = 0x1,
	},

	.cfg = {
		.bLength = sizeof(usb_config_descr_t),
		.bDescriptorType = USB_CONFIGURATION_DESCRIPTOR_TYPE,
		USBWORD(TOTAL_CONFIG_LEN),	/* wTotalLength */
		.bNumInterfaces = 1,
		.bConfigurationValue = 1,
		.iConfiguration = DEVICE_STRING_CONFIG_INDEX,
		.bmAttributes = CONFIG_SELF_POWERED,
		.MaxPower = 0x32
	},

	.intf = {
		.bLength = sizeof(usb_interface_descr_t),
		.bDescriptorType = USB_INTERFACE_DESCRIPTOR_TYPE,
		.bInterfaceNumber = 0x0,
		.bAlternateSetting = 0x0,
		.bNumEndpoints = 0x2,
		.bInterfaceClass = DEVICE_INTERFACE_CLASS,
		.bInterfaceSubClass = DEVICE_INTERFACE_SUB_CLASS,
		.bInterfaceProtocol = DEVICE_INTERFACE_PROTOCOL,
		.iInterface = DEVICE_STRING_INTERFACE_INDEX
	},

	.bulkin_ep = {
		.bLength = sizeof(usb_endpoint_descr_t),
		.bDescriptorType = USB_ENDPOINT_DESCRIPTOR_TYPE,
		.bEndpointAddress = (USB_ENDPOINT_DIRECTION_IN + (BULKIN_EP)/2),
		.bmAttributes = USB_ENDPOINT_TYPE_BULK,
		USBWORD(BULK_MPS),	/* wMaxPacketSize */
		.bInterval = 0x00
	},

	.bulkout_ep = {
		.bLength = sizeof(usb_endpoint_descr_t),
		.bDescriptorType = USB_ENDPOINT_DESCRIPTOR_TYPE,
		.bEndpointAddress = (BULKOUT_EP/2),
		.bmAttributes = USB_ENDPOINT_TYPE_BULK,
		USBWORD(BULK_MPS),	/* wMaxPacketSize */
		.bInterval = 10		/* NAK poll period of 12.5usec for HS */
	}
};

