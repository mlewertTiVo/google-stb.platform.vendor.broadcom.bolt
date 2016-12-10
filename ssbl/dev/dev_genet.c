/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#if CFG_GENET


/* ************************************
   * Broadcom settop GENET Driver
   * ...version 2 for new DMA
 * ***********************************/

#include "lib_types.h"
#include "lib_malloc.h"
#include "lib_string.h"
#include "lib_printf.h"
#include "lib_hexdump.h"

#include "iocb.h"
#include "ioctl.h"
#include "device.h"
#include "devfuncs.h"
#include "mii.h"
#include "error.h"

#include "bsp_config.h"
#include "dev_genet.h"
#include "timer.h"
#include "macutils.h"
#include "ui_command.h"
#include "env_subr.h"
#include "dev_bcmethsw.h"
#include "dev_bcmgphy.h"
#include "board.h"
#include "net_ebuf.h"
#include "net_ether.h"
#include "net_mdio.h"
#include "macaddr.h"
#include "otp_status.h"


#define MII_INT			INT_PHY_ADDR	/* default internal phy address */
/* override following macro in Makefile file to specify
   a different PHY type, mdio access, speed and addr.
ETH0_PHY: optional
 INT (default) - internal PHY
 MII - 10/100 MII (18-wire interface)
 RGMII - 14-wire RGMII interface, may run at 25 MHz or 125 MHz
RGMII_NO_ID - same as above, but disable phase shift on TXC clock

ETH0_MDIO_MODE: optional
 0  - standard MDIO is not available (3383)
 1 - (default) standard MDIO is available (ethernet PHY)
 boot - BOLT configure it only (ethernet switch)

ETH0_SPEED: optional
 100
 1000 (default)

ETH0_PHYADDR: optional
 1 (default) for internal PHY
 30 - Broadcom Robo switch PHY addr
 probe - probe and finding PHY addr
 */

#define UMAC_OFF(umac)		umac->cmd &= ~(CMD_TX_EN | CMD_RX_EN)
#define UMAC_ON(umac)		umac->cmd |= (CMD_TX_EN | CMD_RX_EN)
#define UMAC_ENABLED(umac)	(umac->cmd & (CMD_TX_EN | CMD_RX_EN))

/*
 * Buffers must be 4 byte aligned.  Because BOLT's buffer size is not,
 * we need to be sure outgoing and incoming traffic will fit into the
 * buffers we provide to the hardware.  Ordinarily our goal is to
 * ensure the RX buffers are smaller than BOLT's internal buffers and
 * that TX buffers are larger than BOLT's internal buffers.  However,
 * incoming traffic will contain an extra CRC which will be discarded.
 * Therefore we can conclude that rounding the size upwards will not
 * cause any problems.
 */
#define ENET_MAX_MTU_SIZE	1518
#define DMA_MAX_BURST_LENGTH	0x10
#define ENET_BUF_SIZE		2048
#define NUMRXBDS		64
#define NUMTXBDS		1

/* misc. configuration */
#define DMA_FC_THRESH_LO	5
#define DMA_FC_THRESH_HI	10

typedef struct genet_softc {
	volatile SysRegs	*sys;
	volatile uniMacRegs	*umac;
	volatile ExtRegs	*ext;
	volatile intrl2Regs *intrl2_0;
	volatile intrl2Regs *intrl2_1;
	volatile rbufRegs	*rbuf;
	volatile rDmaRegs	*rxDma;
	volatile tDmaRegs	*txDma;
	volatile DmaDesc	*RxBDs;
	volatile DmaDesc	*TxBD;

	uint8_t *RxBuffers;
	uint8_t rx_read_ptr;
	uint8_t *TxBuffer;

	uint8_t  macaddr[6];
	uint8_t  loopmode;
	uint32_t phyaddr;
	uint32_t ext_switch;
	uint32_t rgmii;
	uint32_t physpeed;
	char *phyintf;
	int instance;
	int switch_detected;
	mdio_info_t *mdio;
	uint32_t timeout_txdma;

} genet_softc;

static genet_softc *genet_softcs[NUM_GENET + 1];

static void genet_ether_probe(bolt_driver_t * drv, unsigned long probe_a,
			      unsigned long probe_b, void *probe_ptr);
static int genet_ether_open(bolt_devctx_t * ctx);
static int genet_ether_read(bolt_devctx_t * ctx, iocb_buffer_t * buffer);
static int genet_ether_inpstat(bolt_devctx_t * ctx, iocb_inpstat_t * inpstat);
static int genet_ether_write(bolt_devctx_t * ctx, iocb_buffer_t * buffer);
static int genet_ether_ioctl(bolt_devctx_t * ctx, iocb_buffer_t * buffer);
static int genet_ether_close(bolt_devctx_t * ctx);

static void genet_mdio_probe(bolt_driver_t * drv, unsigned long probe_a,
				unsigned long probe_b, void *probe_ptr);
static int genet_mdio_open(bolt_devctx_t * ctx);
static int genet_mdio_read(bolt_devctx_t * ctx, iocb_buffer_t * buffer);
static int genet_mdio_write(bolt_devctx_t * ctx, iocb_buffer_t * buffer);
static int genet_mdio_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer);
static int genet_mdio_close(bolt_devctx_t * ctx);

/* Set loopback mode */
static void loopback_disable(genet_softc * softc, uint8_t loopmode);
static void mii_internal_Loopback(genet_softc * softc, int bEnable);
static void mii_external_Loopback(genet_softc * softc, int bEnable);
static void ephy_workaround(genet_softc * softc);
static void mii_set_mdio_clock(mdio_info_t * mdio, uint32_t phy);

static const bolt_devdisp_t genet_ether_dispatch = {
	genet_ether_open,
	genet_ether_read,
	genet_ether_inpstat,
	genet_ether_write,
	genet_ether_ioctl,
	genet_ether_close,
	NULL,
	NULL
};

const bolt_driver_t genetdrv = {
	"GENET Internal Ethernet",
	"eth",
	BOLT_DEV_NETWORK,
	&genet_ether_dispatch,
	genet_ether_probe
};

static const bolt_devdisp_t genet_mdio_dispatch = {
	genet_mdio_open,
	genet_mdio_read,
	NULL,
	genet_mdio_write,
	genet_mdio_ioctl,
	genet_mdio_close,
	NULL,
	NULL
};

static bolt_driver_t genet_mdiodrv = {
	"GENET MDIO",
	"mdio",
	BOLT_DEV_OTHER,
	&genet_mdio_dispatch,
	genet_mdio_probe
};

static int Enet_debug;
#define DBG_RX			0x0010
#define DBG_RX_ERR		0x0020
#define DBG_TX			0x0040
#define DBG_MII_CMD		0x0100
#define DBG_MII_WRITE		0x0200
#define DBG_MII_READ		0x0400
#define DBG_MII_RD_FAIL		0x0800
#define DBG_EPHY_PROBE		0x1000

/* *********************************
 * Utility Functions
 * ********************************* */

/* Don't go up the IP stack as we could end up
  at a USB eth dongle. We *want* GENET.
*/
static void setenv_phy_probe_id(int id, int instance)
{
	char b[10];
	char env_var[255];

	xsprintf(b, "%d", id);
	xsprintf(env_var, "ETH%d_PHYPROBE_ID", instance);
	(void)env_setenv(env_var, b, ENV_FLG_BUILTIN);
}


static void write_hw_mac_address(volatile uniMacRegs *umac, uint8_t * addr)
{
	int enabled = UMAC_ENABLED(umac);

	if (enabled)
		UMAC_OFF(umac);

	umac->mac_0 = (addr[0] << 24) | (addr[1] << 16) |
					(addr[2] <<  8) | (addr[3]);
	umac->mac_1 = (addr[4] <<  8) | (addr[5]);

	if (enabled)
		UMAC_ON(umac);
}

static void genet_write_mac_address(genet_softc * softc,
						uint8_t * addr, int to_hw)
{
	memcpy(softc->macaddr, addr, 6);
	if (to_hw)
		write_hw_mac_address(softc->umac, addr);
}

static void genet_read_mac_address(genet_softc * softc, uint8_t * addr)
{
	memcpy(addr, softc->macaddr, 6);
}

static void genet_disable_dma(genet_softc * softc)
{
	softc->txDma->tdma_ctrl = 0;
	softc->rxDma->rdma_ctrl = 0;
	softc->umac->tx_flush = 1;
	bolt_usleep(10);
	softc->umac->tx_flush = 0;
}

static void reset_umac(volatile uniMacRegs *umac)
{
	/* disable MAC while updating its registers */
	umac->cmd = 0 ;

	/* issue soft reset, wait for it to complete */
	umac->cmd = CMD_SW_RESET;
	bolt_usleep(1000);
	umac->cmd = 0;
}

static char *genet_set_physpeed(genet_softc *softc, const enet_params *e)
{
	char env_var[255];
	char *physpeed = NULL;

	xsprintf(env_var, "ETH%d_SPEED", softc->instance);
	physpeed = env_getenv(env_var);
	if (!physpeed) {
		if (e && e->phy_speed)
			physpeed = e->phy_speed;
		else
			return NULL;
	}

	if (strcmp(physpeed, "10") == 0)
		softc->physpeed = 10;
	else if (strcmp(physpeed, "100") == 0)
		softc->physpeed = 100;
	else
		softc->physpeed = 1000;

	return physpeed;
}

/* Two slices per address */
#define MDF_ADDR_CNT	(ARRAY_SIZE(umac->mdf_addr) / 2) - 1

static void umac_set_mdf(volatile uniMacRegs *umac, const uint8_t *addr,
				unsigned int *i, unsigned int *mc)
{
	umac->mdf_addr[*i] = addr[0] << 8 | addr[1];
	umac->mdf_addr[*i + 1] = addr[2] << 24 | addr[3] << 16 |
				addr[4] << 8 | addr[5];
	umac->mdf_ctrl |= (1 << (MDF_ADDR_CNT - *mc));
	*i += 2;
	(*mc)++;
}

static void genet_ether_write_mac_address(struct genet_softc *softc)
{
	unsigned int i = 0;
	unsigned int mc = 0;

	/* Broadcast */
	umac_set_mdf(softc->umac, eth_broadcast, &i, &mc);
	/* Our OWN mac address */
	umac_set_mdf(softc->umac, softc->macaddr, &i, &mc);
}

/* *********************************
 * Init Functions
 * ********************************* */

static int init_umac(genet_softc * softc)
{
	volatile uniMacRegs *umac;
	volatile intrl2Regs *intrl2;
	mdio_config eMiiConfig;
	int speed = 10;
	int retval;

	umac = softc->umac;;
	intrl2 = softc->intrl2_0;

	/* Re-fetch the ETH%d_SPEED environment setting since
	 * it may have been changed by an user
	 */
	genet_set_physpeed(softc, NULL);

	reset_umac(umac);
	if (strcmp(softc->phyintf, "INT") == 0) {
		retval = mdio_phy_reset(softc->mdio, softc->phyaddr);
		if (retval != 0)
			return retval;
		mii_set_mdio_clock(softc->mdio, softc->phyaddr);
		mdio_set_advert(softc->mdio, softc->phyaddr, softc->physpeed);
		ephy_workaround(softc);
	} else {
		if (softc->ext_switch || softc->rgmii) {
			if (softc->physpeed == 100) {
				speed = UMAC_SPEED_100;
			} else {
				speed = UMAC_SPEED_1000;
			}
			umac->cmd &= ~CMD_HD_EN;
			umac->cmd = umac->cmd &
				~(CMD_SPEED_MASK << CMD_SPEED_SHIFT);
			umac->cmd = umac->cmd  | (speed << CMD_SPEED_SHIFT);
		} else {
			mdio_phy_reset(softc->mdio, softc->phyaddr);
			mdio_set_advert(softc->mdio, softc->phyaddr, softc->physpeed);
		}
	}

	/* clear tx/rx counter */
	umac->mib_ctrl = MIB_RESET_RX | MIB_RESET_TX | MIB_RESET_RUNT;
	umac->mib_ctrl = 0;

	umac->max_frame_len = ENET_MAX_MTU_SIZE;

	if (!softc->ext_switch && !softc->rgmii) {
		/*
		 * mii_init() will program speed/duplex/pause
		 * parameters into umac and rbuf registers.
		 */
		eMiiConfig = mdio_auto_configure(softc->mdio, softc->phyaddr);
		if (! (eMiiConfig & MDIO_AUTONEG)) {
			const bool is_link_up =
				(mdio_get_linkstatus(softc->mdio,
					softc->phyaddr) == 1);

			xprintf("Auto-negotiation timed-out%s\n",
				is_link_up ? "" : "...check cable/connection");
		}
		umac->cmd &= ~(CMD_SPEED_MASK << CMD_SPEED_SHIFT);
		if (eMiiConfig & MDIO_1000MBIT) {
			umac->cmd |= (UMAC_SPEED_1000 << CMD_SPEED_SHIFT);
			speed = 1000;
		} else {
			if (eMiiConfig & MDIO_100MBIT) {
				umac->cmd |=
					(UMAC_SPEED_100 << CMD_SPEED_SHIFT);
				speed = 100;
			} else {
				umac->cmd |=
					(UMAC_SPEED_10 << CMD_SPEED_SHIFT);
				speed = 10;
			}
		}
		xprintf("%d Mbps %s-Duplex\n", speed,
			((eMiiConfig & MDIO_FULLDUPLEX) ? "Full" : "Half"));
		if (eMiiConfig & MDIO_FULLDUPLEX) {
			umac->cmd &= ~CMD_HD_EN;
		} else {
			umac->cmd |= CMD_HD_EN;
		}
	}

	/* Configure our MDF filters */
	genet_ether_write_mac_address(softc);

	/* Mask all interrupts.*/
	intrl2->cpu_mask_set = 0xFFFFFFFF;
	intrl2->cpu_clear = 0xFFFFFFFF;
	intrl2->cpu_mask_clear = 0x0;

	/* Enable rx/tx engine.*/
	umac->cmd &= ~CMD_PROMISC;
	umac->cmd |= CMD_TX_EN | CMD_RX_EN;

	return 0;
}

static void init_edma(genet_softc * softc)
{
	volatile rDmaRingRegs *rdma;
	volatile tDmaRingRegs *tdma;

	/* init rDma, disable it first while updating register */
	softc->rxDma->rdma_ctrl = 0;
	softc->rxDma->rdma_scb_burst_size = DMA_MAX_BURST_LENGTH;
	rdma = &softc->rxDma->rDmaRings[DMA_RING_DESC_INDEX];
	/* by default, enable ring 16 (descriptor based) */
	rdma->rdma_write_pointer = 0;
	rdma->rdma_producer_index = 0;
	rdma->rdma_consumer_index = 0;
	rdma->rdma_ring_buf_size =
			(NUMRXBDS << DMA_RING_SIZE_SHIFT) | ENET_BUF_SIZE;
	rdma->rdma_start_addr = 0;
	rdma->rdma_end_addr =
			NUM_DESC_PER_BD*NUMRXBDS - 1;
	rdma->rdma_xon_xoff_threshold =
		(DMA_FC_THRESH_LO << DMA_XOFF_THRESHOLD_SHIFT) | DMA_FC_THRESH_HI;
	rdma->rdma_read_pointer = 0;

	/* Init tDma */
	softc->txDma->tdma_ctrl = 0;
	softc->txDma->tdma_scb_burst_size = DMA_MAX_BURST_LENGTH;
	tdma = &softc->txDma->tDmaRings[DMA_RING_DESC_INDEX];
	/* by default, enable ring DMA_RING_DESC_INDEX (descriptor based) */
	tdma->tdma_read_pointer = 0;
	tdma->tdma_producer_index = 0;
	tdma->tdma_consumer_index = 0;
	tdma->tdma_ring_buf_size =
		(NUMTXBDS << DMA_RING_SIZE_SHIFT) | ENET_BUF_SIZE;
	tdma->tdma_start_addr = 0;
	tdma->tdma_end_addr = NUM_DESC_PER_BD * NUMTXBDS - 1;
	tdma->tdma_mbuf_done_threshold = 0;
	/* Disable rate control for now */
	tdma->tdma_flow_period = 0;
	tdma->tdma_write_pointer = 0;
}

static int init_buffers(genet_softc * softc)
{
	int i;
	uint8_t *address;

	softc->RxBuffers = KMALLOC(NUMRXBDS * ENET_BUF_SIZE, DMA_BUF_ALIGN);

	if (softc->RxBuffers == NULL) {
		xprintf("GENET: Failed to allocate RxBuffer memory.\n");
		return 0;
	}

	softc->TxBuffer = KMALLOC(ENET_BUF_SIZE, DMA_BUF_ALIGN);
	if (softc->TxBuffer == NULL) {
		xprintf("GENET: Failed to allocate TxBuffer memory.\n");
		KFREE(softc->RxBuffers);
		softc->RxBuffers = NULL;
		return 0;
	}

	/* Init the Receive Buffer Descriptor Ring. */
	for (i = 0; i < NUMRXBDS; i++) {
		softc->RxBDs[i].length_status = ENET_BUF_SIZE << 16;
		address = softc->RxBuffers + i * ENET_BUF_SIZE;
		softc->RxBDs[i].address = PHYSADDR((uint32_t)address);
	}
	softc->rx_read_ptr = 0;

	/* Init Transmit Buffer Descriptor...using only one */
	softc->TxBD->length_status = (0 << 16);
	softc->TxBD->address = PHYSADDR((uint32_t)softc->TxBuffer);
	return 1;
}

/* probe for an external PHY via MDIO; return PHY address */
static int ephy_probe(genet_softc * softc)
{
	int addr;
	int phy_id;

	/*
	 * Enable RGMII to interface external PHY, power down internal 10/100 MII.
	 */
	GENET_RGMII_OOB_CTRL(softc) |= RGMII_MODE_EN;

#if CONFIG_BRCM_GENET_VERSION == 5
	softc->ext->ext_ephy_ctrl |=
		(EXT_EPHY_CNTRL_EXT_PWR_DOWN_PHY_EN |
		 EXT_EPHY_CNTRL_EXT_PWR_DOWN_PHY_RD |
		 EXT_EPHY_CNTRL_EXT_PWR_DOWN_PHY_SD |
		 EXT_EPHY_CNTRL_EXT_PWR_DOWN_PHY_RX |
		 EXT_EPHY_CNTRL_EXT_PWR_DOWN_PHY_TX |
		 EXT_EPHY_CNTRL_EXT_PWR_DOWN_BIAS |
		 EXT_EPHY_CNTRL_EXT_PWR_DOWN_DLL |
		 EXT_EPHY_CNTRL_IDDQ_GLOBAL_PWR);
#else
	softc->ext->ext_pwr_mgmt |=
		(EXT_PWR_DOWN_PHY | EXT_PWR_DOWN_DLL | EXT_PWR_DOWN_BIAS);
#endif

	addr = mdio_phy_find_first(softc->mdio, MII_INT);
	if (addr >= 0) {
		phy_id = mdio_get_phy_id(softc->mdio, addr);
		if (phy_id != 0xFFFF) {
			softc->phyaddr = addr;
			setenv_phy_probe_id(addr, softc->instance);
			return 0;
		}
	}
	if (addr < 0) {
		softc->phyaddr = PHY_MAX_ADDR + 1;
		xprintf ("***** ERROR: External PHY probe failed! *****\n\n");
	}
	setenv_phy_probe_id(-1, softc->instance);
	return -1;
}


/* *********************************
 * Driver Start Functions
 * ********************************* */

static void genet_ether_powerup(genet_softc * softc)
{
	/* Powering up applies to internal PHY only. */
	if (strcmp(softc->phyintf, "INT") != 0)
		return;

#if CONFIG_BRCM_GENET_VERSION < 4
	/* Prior to the GENET version 4, the phy was powered up by default.
	 * No need for powering up, do nothing but returning.
	 */
	return;
#elif CONFIG_BRCM_GENET_VERSION == 5
	/* Power up EPHY */
	softc->ext->ext_ephy_ctrl &=
		~(EXT_EPHY_CNTRL_EXT_PWR_DOWN_PHY_EN |
		EXT_EPHY_CNTRL_EXT_PWR_DOWN_PHY_RD |
		EXT_EPHY_CNTRL_EXT_PWR_DOWN_PHY_SD |
		EXT_EPHY_CNTRL_EXT_PWR_DOWN_PHY_RX |
		EXT_EPHY_CNTRL_EXT_PWR_DOWN_PHY_TX |
		EXT_EPHY_CNTRL_IDDQ_GLOBAL_PWR |
		EXT_EPHY_CNTRL_EXT_PWR_DOWN_BIAS |
		EXT_EPHY_CNTRL_EXT_PWR_DOWN_DLL);

	softc->ext->ext_ephy_ctrl |= EXT_EPHY_CNTRL_RESET;
	isb();
	bolt_usleep(60); /* 60us, for asserting RESET from RDB */

	softc->ext->ext_ephy_ctrl &= ~(EXT_EPHY_CNTRL_RESET);
	bolt_usleep(100); /* 100us, no PHY activity after reset from RDB */
#else
	/* GPHY is powered down by default. To power it up, GPHY AFE and BIAS
	 * need be out of power down, which requires reset.
	 */
	softc->ext->ext_gphy_ctrl &=
		~(EXT_GPHY_CNTRL_CFG_IDDQ_BIAS | EXT_GPHY_CNTRL_CFG_EXT_PWRDOWN);
	softc->ext->ext_gphy_ctrl |= EXT_GPHY_CNTRL_GPHY_RESET;
	bolt_usleep(60); /* 60us, for asserting RESET from RDB */
	softc->ext->ext_gphy_ctrl &= ~(EXT_GPHY_CNTRL_GPHY_RESET);
	bolt_usleep(20); /* 20us, no PHY activity after reset from RDB */
#endif

	/* dummy read to avoid the first read failure after powering up */
	(void) mdio_read(softc->mdio, softc->phyaddr, MII_BMSR);
}

static inline int genet_non_default_settings(const enet_params *e,
					     char *phy_type,
					     char *mdio_mode,
					     char *phy_speed,
					     char *phy_id)
{
	return enet_params_cmp(e, phy_type, mdio_mode, phy_speed, phy_id);
}

char *genet_get_phyintf(uint32_t base)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(genet_softcs); i++)
		if (genet_softcs[i] &&
		    ((uint32_t)genet_softcs[i]->sys) == base)
			return genet_softcs[i]->phyintf;

	return NULL;
}

static unsigned int mac_addr_inc;


static void port_ctl_set_physpeed(genet_softc *softc)
{
	bool onchip_phy = !!(!strcmp(softc->phyintf, "MOCA") ||
				!strcmp(softc->phyintf, "INT"));

	if (onchip_phy) { /* e.g. Eth phy or MoCA */
#if (CONFIG_BRCM_GENET_VERSION == 4)
		softc->sys->sys_port_ctrl = PORT_MODE_INT_GPHY;
#elif(CONFIG_BRCM_GENET_VERSION == 3) || (CONFIG_BRCM_GENET_VERSION == 5)
		softc->sys->sys_port_ctrl = PORT_MODE_INT_EPHY;
#else
#error GENET: Fix physpeed for the current CONFIG_BRCM_GENET_VERSION!
#endif
		return;
	}

	/* External PHY */

	switch (softc->physpeed) {
	case 100:
		if (strcmp(softc->phyintf, "MII") == 0)
			softc->sys->sys_port_ctrl = PORT_MODE_EXT_EPHY;
		else if (strcmp(softc->phyintf, "RVMII") == 0)
			softc->sys->sys_port_ctrl = PORT_MODE_EXT_RVMII_25;
		else
			softc->sys->sys_port_ctrl = PORT_MODE_EXT_GPHY;
		break;
	case 1000:
	default:
		softc->sys->sys_port_ctrl = PORT_MODE_EXT_GPHY;
		break;
	}
}

static void genet_ether_init(bolt_driver_t * drv, int instance)
{
	genet_softc *softc;
	char descr[100], *envmac;
	char *phyaddr;
	char *mdio_mode = NULL;
	char *physpeed = NULL;
	const enet_params *e = board_enet(instance);
	unsigned long base;
	char env_var[255];
	char b[10];

	if (instance > NUM_GENET) {
		xprintf("too many GENET\n");
		return;
	}

	if (!e)
		return;

	base = REG_ADDR(e->base);

	softc = genet_softcs[instance];
	if (softc && softc->mdio)
		mdio_uninit(softc->mdio);

	softc = (genet_softc *) KUMALLOC(sizeof(genet_softc), 0);
	if (softc == NULL) {
		xprintf("GENET: Failed to allocate softc memory.\n");
		return;
	} else {
		memset(softc, 0, sizeof(genet_softc));
		genet_softcs[instance] = softc;

		softc->instance = instance;
		softc->sys = (SysRegs *) base;
		softc->umac = (volatile uniMacRegs *)
			(base + UMAC_UMAC_REG_OFFSET);
		softc->ext = (ExtRegs *)(base + UMAC_EXT_REG_OFFSET);
		softc->intrl2_0 = (intrl2Regs *)
				(base + UMAC_INTRL2_0_REG_OFFSET);
		softc->intrl2_1 = (intrl2Regs *)
				(base + UMAC_INTRL2_1_REG_OFFSET);
		softc->rbuf = (rbufRegs *)
				(base + UMAC_RBUF_REG_OFFSET);
		softc->txDma = (tDmaRegs *)(base +
					UMAC_TDMA_REG_OFFSET +
			NUM_DMA_DESC * sizeof(unsigned long));
		softc->rxDma = (rDmaRegs *)(base +
					UMAC_RDMA_REG_OFFSET +
			NUM_DMA_DESC * sizeof(unsigned long));
		softc->RxBDs = (DmaDesc *)
				(base + UMAC_RDMA_REG_OFFSET);
		softc->TxBD = (DmaDesc *)
				(base + UMAC_TDMA_REG_OFFSET);

		/* set PHY type */
		xsprintf(env_var, "ETH%d_PHY", instance);
		softc->phyintf = env_getenv(env_var);
		if (!softc->phyintf) {
			if (e && e->phy_type)
				softc->phyintf = e->phy_type;
			else
				return;
		}

#ifdef OTP_OPTION_GPHY_DISABLE
		if (OTP_OPTION_GPHY_DISABLE() && !strcmp(softc->phyintf, "INT"))
			return;
#endif

		xsprintf(env_var, "ETH%d_PHYADDR", instance);
		phyaddr = env_getenv(env_var);
		if (!phyaddr) {
			if (e && e->phy_id)
				phyaddr = e->phy_id;
			else
				return;
		}
		if (strcmp(phyaddr, "probe") == 0)
			softc->phyaddr = -1;
		else {
			if (strcmp(phyaddr, "noprobe") == 0) {
				softc->phyaddr = 32;
				setenv_phy_probe_id(softc->phyaddr, softc->instance);
			}
			else
				softc->phyaddr = atoi(phyaddr);
		}
		envmac = env_getenv(MACADDR_ENVSTR);
		if (envmac) {
			MAC_str2bin(envmac, softc->macaddr);
			macaddr_increment(softc->macaddr, mac_addr_inc);
			mac_addr_inc += MACADDR_INCREMENT;

			if (!BSP_CFG_MOCA_MAC_EQ_ETH_MAC) {
				/* Keep the same allocation scheme as with
				 * Devicetree, MoCA eats one MAC address.
				 */
				if (e && (strcmp(e->phy_type, "MOCA") == 0))
					mac_addr_inc++;
			}
		}
		xsprintf(descr, "%s at %p", drv->drv_description,
			 softc->sys);
		bolt_attach_idx(drv, softc->instance, softc, NULL, descr);

		bolt_add_device(&genet_mdiodrv, 0, 0, softc);
	}

	genet_ether_powerup(softc);

#if CONFIG_BRCM_GENET_VERSION > 1
	GENET_RBUF_FLUSH_CTRL(softc) |= 0x02;
	bolt_usleep(10);
	GENET_RBUF_FLUSH_CTRL(softc) &= ~0x02;
	bolt_usleep(10);
#endif
	physpeed = genet_set_physpeed(softc, e);
	if (!physpeed)
		return;

	if (strcmp(softc->phyintf, "MOCA") == 0) {
#ifdef OTP_OPTION_MOCA_DISABLE
		if (!OTP_OPTION_MOCA_DISABLE())
#endif
			port_ctl_set_physpeed(softc);
		return;
	} else if (strcmp(softc->phyintf, "INT") != 0) {
		xsprintf(env_var, "ETH%d_MDIO_MODE", instance);
		mdio_mode = env_getenv(env_var);
		if (!mdio_mode) {
			if (e && e->mdio_mode)
				mdio_mode = e->mdio_mode;
			else /* Assign a default mdio_mode value */
				mdio_mode = "0";
		}
		if (strcmp(mdio_mode, "0") == 0) {
			softc->rgmii = 1;
			softc->phyaddr = -1;
		} else {
			if (strcmp(mdio_mode, "boot") == 0)
				softc->ext_switch = 1;
		}

		port_ctl_set_physpeed(softc);

#if CONFIG_BRCM_GENET_VERSION == 5
		softc->ext->ext_ephy_ctrl |=
			(EXT_EPHY_CNTRL_EXT_PWR_DOWN_PHY_EN |
			 EXT_EPHY_CNTRL_EXT_PWR_DOWN_PHY_RD |
			 EXT_EPHY_CNTRL_EXT_PWR_DOWN_PHY_SD |
			 EXT_EPHY_CNTRL_EXT_PWR_DOWN_PHY_RX |
			 EXT_EPHY_CNTRL_EXT_PWR_DOWN_PHY_TX |
			 EXT_EPHY_CNTRL_EXT_PWR_DOWN_BIAS |
			 EXT_EPHY_CNTRL_EXT_PWR_DOWN_DLL |
			 EXT_EPHY_CNTRL_IDDQ_GLOBAL_PWR);
#else
		softc->ext->ext_pwr_mgmt |=
			(EXT_PWR_DOWN_PHY | EXT_PWR_DOWN_DLL | EXT_PWR_DOWN_BIAS);

#endif
		/* All *MII variants require us to power on the RGMII block by
		 * setting RGMII_MODE_EN
		 */
		if (strstr(softc->phyintf, "MII")) {
			softc->ext->rgmii_oob_ctrl &= ~OOB_DISABLE;
			softc->ext->rgmii_oob_ctrl |= RGMII_LINK;
			softc->ext->rgmii_oob_ctrl |= RGMII_MODE_EN;
			if (strcmp(softc->phyintf, "RGMII_NO_ID") == 0) {
				/* Don't shift tx clock by 90 degree */
				softc->ext->rgmii_oob_ctrl |= ID_MODE_DIS;
			}
		}
	} else {
		mdio_mode = "1";
		port_ctl_set_physpeed(softc);
	}

	if (genet_non_default_settings(e, softc->phyintf, mdio_mode,
				physpeed, phyaddr))
		err_msg("GENET%d IS USING NON-DEFAULT SETTINGS", instance);

	if (softc->ext_switch) {
		if (ethsw_is_present(softc->mdio)) {
			softc->switch_detected = 1;
			ethsw_reset_ports(softc->mdio, 6,
					softc->phyintf, softc->physpeed);
			ethsw_switch_unmanage_mode(softc->mdio);
			ethsw_config_learning(softc->mdio);
			ethsw_setup_flow_control(softc->mdio);
		} else {
			err_msg("GENET%d: unable to detect switch on MDIO bus", instance);
			xsprintf(b, "%d", PHY_ID_NONE);
			xsprintf(env_var, "ETH%d_PHYADDR", instance);
			(void)env_setenv(env_var, b, ENV_FLG_BUILTIN);
			return;
		}
	}

	/* Probe for external PHY now */
	if (!softc->ext_switch && !softc->rgmii) {
		if ((int) softc->phyaddr < 0)
			ephy_probe(softc);
		if (softc->phyaddr >= PHY_MAX_ADDR)
		{
			xprintf("error: Invalid PHY address!\n");
			return;
		}
	}

	/* turn off UniMAC pause frames generation */
	softc->umac->pause_ctrl = 0;

	/* 5 milliseconds */
	softc->timeout_txdma = get_clocks_per_usec() * 5000;
}

static void genet_ether_probe(bolt_driver_t * drv, unsigned long probe_a,
			      unsigned long probe_b, void *probe_ptr)
{
	if (!env_getenv("ETH_OFF"))
		genet_ether_init(drv, (int)probe_ptr);
}

static void genet_mdio_probe(bolt_driver_t * drv, unsigned long probe_a,
				unsigned long probe_b, void *probe_ptr)
{
	genet_softc * softc = probe_ptr;
	char buf[255];

	xsprintf(buf, "%s at %p", drv->drv_description, softc->umac);
	bolt_attach_idx(drv, softc->instance, softc, NULL, buf);

	xsprintf(buf, "mdio%d", softc->instance);
	softc->mdio = mdio_init(buf);
	if (!softc->mdio)
		err_msg("MDIO init failed on GENET%d", softc->instance);
}

/* *********************************
 * Network Layer Access Functions
 * ********************************* */

static int genet_ether_open(bolt_devctx_t * ctx)
{
	genet_softc *softc;
	int retval;

	if (ctx == NULL)
		return BOLT_ERR_INV_PARAM;
	softc = (genet_softc *) ctx->dev_softc;

	if (softc->ext_switch && softc->switch_detected == 0)
		return BOLT_ERR_IOERR;

	if ((Enet_debug = env_getval("ETH_DBG")) < 0)
		Enet_debug = 0;

	retval = init_umac(softc);
	if (retval != 0)
		return retval;

	genet_write_mac_address(softc, softc->macaddr, 1);
	genet_disable_dma(softc);

	init_buffers(softc);

	init_edma(softc);

	softc->rxDma->rdma_ctrl =
		(1 << (DMA_RING_DESC_INDEX + DMA_RING_BUF_EN_SHIFT) | DMA_EN);
	softc->txDma->tdma_ctrl =
		(1 << (DMA_RING_DESC_INDEX + DMA_RING_BUF_EN_SHIFT) | DMA_EN);

	return 0;
}

static int genet_ether_read(bolt_devctx_t * ctx, iocb_buffer_t * buffer)
{
	unsigned char * dstptr;
	unsigned char * srcptr;
	volatile DmaDesc * CurrentBDPtr;
	genet_softc * softc;
	uint16_t dmaFlag;
	unsigned int p_index = 0, c_index = 0;
	volatile rDmaRingRegs *rdma;
	int rc = 0;

	if (ctx == NULL) {
		xprintf("GENET: No context\n");
		return -1;
	}

	if (buffer == NULL) {
		xprintf("GENET: No dst buffer\n");
		return -1;
	}

	if (buffer->buf_length < (ENET_MAX_MTU_SIZE-4)) {
		xprintf("GENET: dst buffer too small.\n");
		xprintf("       actual size is %d\n", buffer->buf_length);
		return -1;
	}

	softc = (genet_softc *)ctx->dev_softc;
	if (softc == NULL) {
		xprintf("GENET: softc has not been initialized.\n");
		return -1;
	}

	rdma = &softc->rxDma->rDmaRings[DMA_RING_DESC_INDEX];
	p_index = DMA_PRODUCER_INDEX_MASK & rdma->rdma_producer_index;
	c_index = DMA_CONSUMER_INDEX_MASK & rdma->rdma_consumer_index;

	CurrentBDPtr = &softc->RxBDs[softc->rx_read_ptr];
	dmaFlag = (CurrentBDPtr->length_status & 0xffff);

	if (!(dmaFlag & DMA_EOP) || !(dmaFlag & DMA_SOP)) {
		if (Enet_debug & DBG_RX_ERR)
			xprintf("GENET: dropping fragmented packet\n");
		rc = -1;
		goto out;
	}

	if (dmaFlag & (DMA_RX_CRC_ERROR | DMA_RX_OV | DMA_RX_NO |
				DMA_RX_LG | DMA_RX_RXER)) {
		if (Enet_debug & DBG_RX_ERR)
			xprintf("GENET: dropping error packet\n");
		rc = -1;
		goto out;
	}

	dstptr = buffer->buf_ptr;

	/* Convert the length to native endian */
	buffer->buf_retlen = (CurrentBDPtr->length_status >> 16);

	if (buffer->buf_retlen < ENET_ZLEN) {
		if (Enet_debug & DBG_RX_ERR)
			xprintf("GENET: dropping runt packet\n");
		rc = -1;
		goto out;
	}

	srcptr = (unsigned char *)
		(CACADDR(CurrentBDPtr->address));

	/* Invalidate the cache */
	CACHE_INVAL_RANGE(srcptr, buffer->buf_retlen);

	if (Enet_debug & DBG_RX) {
		xprintf("*** RX: p_index=%d c_index=%d read_ptr=%d \
					len=%lu status=%#04x\n",
				 p_index, c_index, softc->rx_read_ptr,
				 (CurrentBDPtr->length_status>>16), dmaFlag);
		hexdump(srcptr, buffer->buf_retlen);
	}

	memcpy(dstptr, srcptr, buffer->buf_retlen);

out:
	rdma->rdma_consumer_index += 1;
	rdma->rdma_consumer_index &= DMA_CONSUMER_INDEX_MASK;
	softc->rx_read_ptr++;
	softc->rx_read_ptr &= (NUMRXBDS - 1);

	return rc;
}

static int genet_ether_inpstat(bolt_devctx_t * ctx, iocb_inpstat_t * inpstat)
{
	genet_softc * softc;
	unsigned int p_index = 0, c_index = 0;
	volatile rDmaRingRegs *rdma;

	if (ctx == NULL) {
		xprintf("GENET: No context\n");
		return -1;
	}

	if (inpstat == NULL) {
		xprintf("GENET: No inpstat buffer\n");
		return -1;
	}

	softc = (genet_softc *)ctx->dev_softc;
	if (softc == NULL) {
		xprintf("GENET: softc has not been initialized.\n");
		return -1;
	}

	rdma = &softc->rxDma->rDmaRings[DMA_RING_DESC_INDEX];
	p_index = DMA_PRODUCER_INDEX_MASK & rdma->rdma_producer_index;
	c_index = DMA_CONSUMER_INDEX_MASK & rdma->rdma_consumer_index;

	inpstat->inp_status = (p_index == c_index) ? 0 : 1;

	return 0;
}

static int genet_ether_write(bolt_devctx_t * ctx, iocb_buffer_t * buffer)
{
	unsigned int    srclen;
	unsigned char * dstptr;
	unsigned char * srcptr;
	genet_softc * softc;
	volatile DmaDesc * CurrentBDPtr;
	unsigned int p_index = 0, c_index = 0;
	volatile tDmaRingRegs *tdma;
	uint32_t start_time;
	uint32_t elapsed;

	if (ctx == NULL) {
		xprintf("GENET: No context\n");
		return -1;
	}

	if (buffer == NULL) {
		xprintf("GENET: No dst buffer\n");
		return -1;
	}

	if (buffer->buf_length > ENET_MAX_MTU_SIZE) {
		xprintf("GENET: src buffer too large.\n");
		xprintf("              size is %d\n", buffer->buf_length);
		return -1;
	}

	softc = (genet_softc *) ctx->dev_softc;
	if (softc == NULL) {
		xprintf("GENET: softc has not been initialized.\n");
		return -1;
	}

	tdma = &softc->txDma->tDmaRings[DMA_RING_DESC_INDEX];
	p_index = DMA_PRODUCER_INDEX_MASK & tdma->tdma_producer_index;
	c_index = DMA_CONSUMER_INDEX_MASK & tdma->tdma_consumer_index;
	CurrentBDPtr = softc->TxBD;

	if (Enet_debug & DBG_TX) {
		xprintf("*** TX: p_index=%d c_index=%d len=%d\n",
			p_index, c_index, buffer->buf_length);
		hexdump(buffer->buf_ptr, buffer->buf_length);
	}

	if (p_index != c_index) {
		xprintf("GENET: p_index=%d c_index=%d TX DMA in progress\n",
			p_index, c_index);
		return -1;
	}

	dstptr = (unsigned char *)CACADDR(CurrentBDPtr->address);
	srcptr = buffer->buf_ptr;
	srclen = buffer->buf_length;

	memcpy(dstptr, srcptr, srclen);

	CurrentBDPtr->length_status = srclen << 16;

	/* Flush the cache */
	CACHE_FLUSH_RANGE(dstptr, srclen);

	/* Set status of DMA BD to be transmitted. */
	CurrentBDPtr->length_status |= DMA_SOP | DMA_EOP | DMA_TX_APPEND_CRC;

	/* Enable DMA for this packet */
	softc->txDma->tDmaRings[DMA_RING_DESC_INDEX].tdma_producer_index += 1;

	start_time = arch_getticks();
	while (1) {
		p_index = DMA_PRODUCER_INDEX_MASK & tdma->tdma_producer_index;
		c_index = DMA_CONSUMER_INDEX_MASK & tdma->tdma_consumer_index;
		if (p_index == c_index)
			break;

		elapsed = arch_getticks() - start_time;
		if (elapsed > softc->timeout_txdma) {
			xprintf("GENET: p_index=%d c_index=%d TX DMA timeout\n",
				p_index, c_index);
			return BOLT_ERR_IOERR;
		}

		POLL(); /* give net_poll() chance to consume RX */
	}

	return 0;
}

static int genet_ether_ioctl(bolt_devctx_t * ctx, iocb_buffer_t * buffer)
{
	genet_softc * softc = ctx->dev_softc;
	volatile uniMacRegs *umac = softc->umac;
	int retval = 0;
	uint32_t bmcr, aux, tmp;
	mdio_config eConfig=0;
	int speed = 10;
	int to=1000;

	switch((int)buffer->buf_ioctlcmd) {
		case IOCTL_ETHER_GETHWADDR:
			genet_read_mac_address( softc, buffer->buf_ptr);
			break;
		case IOCTL_ETHER_SETHWADDR:
			genet_write_mac_address(softc, buffer->buf_ptr, 1);
			/* (re)Configure our MDF filters */
			genet_ether_write_mac_address(softc);
			break;
		case IOCTL_ETHER_GETSPEED:
			xprintf("GENET: GETSPEED not implemented.\n");
			retval = -1;
			break;
		case IOCTL_ETHER_SETSPEED:
			bmcr = mdio_read(softc->mdio, softc->phyaddr, MII_BMCR);
			tmp = bmcr & ~(BMCR_ANENABLE | BMCR_SPEED100 |
			       BMCR_SPEED1000 | BMCR_DUPLEX);
			switch(*buffer->buf_ptr) {
				case ETHER_SPEED_AUTO:
					/* restart autonegotiation */
					tmp |= (BMCR_RESTARTAN |
						BMCR_ANENABLE |
						BMCR_SPEED100 |
						BMCR_DUPLEX);
					break;
				case ETHER_SPEED_10HDX:
					break;
				case ETHER_SPEED_10FDX:
					tmp |= BMCR_DUPLEX;
					break;
				case ETHER_SPEED_100HDX:
					tmp |= BMCR_SPEED100;
					break;
				case ETHER_SPEED_100FDX:
					tmp |= (BMCR_SPEED100 | BMCR_DUPLEX);
					break;
				case ETHER_SPEED_1000HDX:
					tmp |= BMCR_SPEED1000;
					break;
				case ETHER_SPEED_1000FDX:
					tmp |= (BMCR_SPEED1000 | BMCR_DUPLEX);
					break;
				default:
					xprintf("GENET: undefined speed.\n");
					retval = -1;
					return retval;
			}
			if (bmcr != tmp) {
				/*
				 * set MDIX manual swap on if disable auto-neg
				 * so can use straight cable
				 */
				aux = mdio_read(softc->mdio, softc->phyaddr, 0x1C);
				if (tmp & BMCR_ANENABLE)
					aux &= ~0x1000;
				else
					aux |= 0x1000;
				mdio_write(softc->mdio, softc->phyaddr, 0x1C, aux);
				mdio_write(softc->mdio, softc->phyaddr, MII_BMCR, tmp);
			}
			if (tmp & BMCR_ANENABLE) {
				for (; to; --to) {
					bolt_usleep(1000);
					tmp = mdio_read(softc->mdio,
						softc->phyaddr, MII_BMSR);
					if (tmp & BMSR_ANCOMPLETE)
						break;
				}
				if (to)	{
					eConfig = mdio_get_config(softc->mdio, softc->phyaddr);
				}
			} else {
				/*
				 * force speed wait for two link integrity test
				 * pulse to deteck link up.
				 * link pulse is 16ms +/- 8ms = 8~24ms
				 * wait for 50ms > 2x24=48ms
				 */
				bolt_usleep(50000);
				eConfig = mdio_get_config(softc->mdio, softc->phyaddr);
			}

			UMAC_OFF(softc->umac);
			umac->cmd = umac->cmd &
				~(CMD_SPEED_MASK << CMD_SPEED_SHIFT);
			if (eConfig & MDIO_1000MBIT) {
				umac->cmd |=
					(UMAC_SPEED_1000 << CMD_SPEED_SHIFT);
				speed = 1000;
			} else {
				if (eConfig & MDIO_100MBIT) {
					umac->cmd |= (UMAC_SPEED_100
							<< CMD_SPEED_SHIFT);
					speed = 100;
				} else {
					umac->cmd |= (UMAC_SPEED_10
							<< CMD_SPEED_SHIFT);
					speed = 10;
				}
			}
			if (eConfig & MDIO_FULLDUPLEX) {
				umac->cmd &= ~CMD_HD_EN;
			} else {
				umac->cmd |= CMD_HD_EN;
			}
			UMAC_ON(softc->umac);
			xprintf("%d Mbps %s-Duplex\n", speed,
				(eConfig & MDIO_FULLDUPLEX) ? "Full" : "Half");
			retval = 0;
			break;
		case IOCTL_ETHER_GETLINK:
			if (buffer->buf_length != sizeof(int)) {
				retval = -1;
				break;
			}
			*((int *)buffer->buf_ptr) =
				mdio_get_linkstatus(softc->mdio, softc->phyaddr);
			retval = 0;
			break;
		case IOCTL_ETHER_GETLOOPBACK:
			*((int *)buffer) = softc->loopmode;
			retval = 0;
			break;
		case IOCTL_ETHER_SETLOOPBACK:
			switch(*(int *)(*((int *)(buffer->buf_ptr)))) {
			case ETHER_LOOPBACK_MAC_INT:
				if (softc->loopmode != ETHER_LOOPBACK_OFF)
					loopback_disable( softc, softc->loopmode);
				UMAC_OFF(softc->umac);
				softc->umac->cmd |= CMD_LCL_LOOP_EN;
				UMAC_ON(softc->umac);
				softc->loopmode = ETHER_LOOPBACK_MAC_INT;
				xprintf("GENET: Loopback mode set to " \
						"MAC internal loopback\n");
				break;
			case ETHER_LOOPBACK_OFF:
				loopback_disable(softc, softc->loopmode);
				softc->loopmode = ETHER_LOOPBACK_OFF;
				xprintf("GENET: Loopback mode is disabled\n");
				break;
			case ETHER_LOOPBACK_MAC_EXT:
				if (softc->loopmode != ETHER_LOOPBACK_OFF)
					loopback_disable(softc, softc->loopmode);
				UMAC_OFF(softc->umac);
				softc->umac->cmd |= (CMD_RMT_LOOP_EN |
							 CMD_PROMISC |
							 CMD_CRC_FWD);
				UMAC_ON(softc->umac);
				softc->loopmode = ETHER_LOOPBACK_MAC_EXT;
				xprintf("GENET: Loopback mode set to " \
						"MAC external loopback\n");
				break;
			case ETHER_LOOPBACK_PHY_INT:
				if (softc->loopmode != ETHER_LOOPBACK_OFF)
				    loopback_disable(softc, softc->loopmode);
				UMAC_OFF(softc->umac);
				mii_internal_Loopback(softc, TRUE);
				UMAC_ON(softc->umac);
				softc->loopmode = ETHER_LOOPBACK_PHY_INT;
				xprintf("GENET: Loopback mode set to " \
						"PHY internal loopback\n");
				break;
			case ETHER_LOOPBACK_PHY_EXT:
				if (softc->loopmode != ETHER_LOOPBACK_OFF)
				    loopback_disable(softc, softc->loopmode);
				UMAC_OFF(softc->umac);
				mii_external_Loopback(softc, TRUE);
				UMAC_ON(softc->umac);
				softc->loopmode = ETHER_LOOPBACK_PHY_EXT;
				xprintf("GENET: Loopback mode set to " \
						"PHY external loopback\n");
				break;
			default:
				xprintf("GENET: Invalid loopback setting.\n");
				retval = -1;
			}
			break;
		default:
			xprintf("GENET: Invalid IOCTL to genet_ether_ioctl\n");
			retval = -1;
	}

	return retval;
}

static int genet_ether_close(bolt_devctx_t * ctx)
{
	genet_softc * softc = (genet_softc *) ctx->dev_softc;

	UMAC_OFF(softc->umac);
	bolt_msleep(10);

	softc->txDma->tdma_ctrl = 0;
	softc->rxDma->rdma_ctrl = 0;
	bolt_msleep(2);
	softc->umac->tx_flush = 1;
	GENET_RBUF_FLUSH_CTRL(softc) = 1;
	bolt_msleep(1);
	softc->umac->tx_flush = 0;
	GENET_RBUF_FLUSH_CTRL(softc) = 0;

	/* And then free all allocated memory */
	KFREE(softc->TxBuffer);
	softc->TxBuffer = NULL;

	KFREE(softc->RxBuffers);
	softc->RxBuffers = NULL;

	return 0;
}

/* *********************************
 * MII Access Functions
 * ********************************* */

/*
 * mdio_access: Access MII via MDIO
 */
static uint32_t mdio_access(volatile uniMacRegs *umac,
		uint32_t opcode, uint32_t phy, uint32_t reg, uint32_t data)
{
	int cnt = 10;
	uint32_t cmd_data;

	cmd_data = (((MDIO_START_BUSY) | (opcode) |
		(phy << MDIO_PMD_SHIFT) | (reg << MDIO_REG_SHIFT)) + data);
	if (Enet_debug & DBG_MII_CMD)
		xprintf("*** MII: cmd_data=%08x)\n", cmd_data);
	umac->mdio_cmd = cmd_data;
	bolt_msleep(5);
	do {
		cmd_data = umac->mdio_cmd;
		if (cmd_data & MDIO_READ_FAIL) {
			if (Enet_debug & DBG_MII_RD_FAIL)
				xprintf("*** MII access error!\n");
			break;
		} else
			bolt_usleep(1000);
		--cnt;
	} while((cmd_data & MDIO_START_BUSY) && cnt);
	if (!cnt)
		 xprintf("*** MII communication timeout!\n");
	return(cmd_data & 0xffff);
}

static int genet_mdio_open(bolt_devctx_t * ctx)
{
	return 0;
}

static int genet_mdio_close(bolt_devctx_t * ctx)
{
	return 0;
}

static int genet_mdio_read(bolt_devctx_t * ctx, iocb_buffer_t * buffer)
{
	genet_softc * softc = (genet_softc *)ctx->dev_softc;
	mdio_xfer_t *xfer = (mdio_xfer_t *)buffer->buf_ptr;

	xfer->data = mdio_access(softc->umac, MDIO_RD, xfer->addr, xfer->regnum, 0);
	if (Enet_debug & DBG_MII_READ)
		xprintf("*** MII read:  phy=%d reg=%02x data=%04x)\n",
				xfer->addr, xfer->regnum, xfer->data);
	return 0;
}

static int genet_mdio_write(bolt_devctx_t * ctx, iocb_buffer_t * buffer)
{
	genet_softc * softc = (genet_softc *)ctx->dev_softc;
	mdio_xfer_t *xfer = (mdio_xfer_t *)buffer->buf_ptr;

	if (Enet_debug & DBG_MII_WRITE)
		xprintf("*** MII write: phy=%d reg=%02x data=%04x)\n",
				xfer->addr, xfer->regnum, xfer->data);
	mdio_access(softc->umac, MDIO_WR, xfer->addr, xfer->regnum, xfer->data);

	return 0;
}

static int genet_mdio_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	genet_softc *softc = ctx->dev_softc;
	int retval = 0;
	struct ether_phy_info *phy_info;

	switch ((int)buffer->buf_ioctlcmd) {
	case IOCTL_ETHER_SET_PHY_DEFCONFIG:
		ephy_workaround(softc);
		break;

	case IOCTL_ETHER_GET_PORT_PHYID:
		/* We ignore port (ioctl() param bolt_offset_t offset)
		 * for GENET devcies and return a common value.
		 */
		*(int *)(buffer->buf_ptr) = softc->phyaddr;
		buffer->buf_retlen = sizeof(int);
		break;

	case IOCTL_ETHER_GET_MDIO_PHYID:
		if (!softc->mdio) {
			retval = BOLT_ERR_DEVNOTFOUND;
			break;
		}
		*(int *)(buffer->buf_ptr) = (int)softc->mdio->phy_id;
		buffer->buf_retlen = sizeof(int);
	break;

	case IOCTL_ETHER_GET_PHY_INFO:
		phy_info = (struct ether_phy_info *)buffer->buf_ptr;
#if CONFIG_BRCM_GENET_VERSION == 5
		phy_info->type = ETH_EPHY;
		phy_info->phyaddr = (unsigned long *)&softc->ext->ext_ephy_ctrl;
#else
		phy_info->type = ETH_GPHY;
		phy_info->phyaddr = (unsigned long *)&softc->ext->ext_gphy_ctrl;
#endif
		phy_info->version = CONFIG_BRCM_GENET_VERSION;
		buffer->buf_retlen = sizeof(struct ether_phy_info);
	break;

	default:
		xprintf("GENET: Invalid IOCTL to genet_mdio_ioctl\n");
		retval = -1;
	}

	return retval;
}

static void loopback_disable(genet_softc * softc, uint8_t loopmode)
{
	volatile uniMacRegs *umac = softc->umac;

	if (loopmode != ETHER_LOOPBACK_OFF) {
		UMAC_OFF(umac);
		switch (loopmode) {
		case ETHER_LOOPBACK_MAC_INT:
			umac->cmd &= ~CMD_LCL_LOOP_EN;
			break;
		case ETHER_LOOPBACK_MAC_EXT:
			umac->cmd &=
				~(CMD_RMT_LOOP_EN | CMD_PROMISC | CMD_CRC_FWD);
			break;
		case ETHER_LOOPBACK_PHY_INT:
			mii_internal_Loopback(softc, FALSE);
			break;
		case ETHER_LOOPBACK_PHY_EXT:
			mii_external_Loopback(softc, FALSE);
			break;
		}
		UMAC_ON(umac);
	}
}

/*
 * mii_internal_Loopback: Set the MII internal loopback mode
 */
static void mii_internal_Loopback(genet_softc * softc, int bEnable)
{
	uint32_t uData;
	uint32_t uStatus;
	int to=1000;

	uStatus = mdio_read(softc->mdio, MII_INT, MII_BMSR) & 0xffff;
	if (bEnable) {
		uData = mdio_read(softc->mdio, MII_INT, MII_BMCR) & 0xffff;

		/* Disable autonegotiation */
		uData &= ~BMCR_ANENABLE;
		/* Enable Loopback */
		uData |= (BMCR_LOOPBACK | BMCR_DUPLEX);
		if (uStatus & (BMSR_100BT_FDX | BMSR_100BT_HDX)) {
			/* Enable Force 100 if 100TX */
			mdio_write(softc->mdio, MII_INT, MII_BMCR, uData);
			uData = mdio_read(softc->mdio, MII_INT, MII_PTEST) & 0xffff;
			uData |= (PTEST_FORCE_TX_LINK | PTEST_FORCE_TX_LOCK);
			mdio_write(softc->mdio, MII_INT, MII_PTEST, uData);
		} else {
			mdio_write(softc->mdio, MII_INT, MII_BMCR, uData);
			uData = mdio_read(softc->mdio, MII_INT, MII_AUXCTL) & 0xffff;
			uData |= (AUXCTL_FORCE_LINK | AUXCTL_EDGE_RATE_4NS);
			mdio_write(softc->mdio, MII_INT, MII_AUXCTL, uData);
		}
	} else {
		if (uStatus & (BMSR_100BT_FDX | BMSR_100BT_HDX)) {
			uData = mdio_read(softc->mdio, MII_INT, MII_PTEST) & 0xffff;
			uData &= ~(PTEST_FORCE_TX_LINK | PTEST_FORCE_TX_LOCK);
			mdio_write(softc->mdio, MII_INT, MII_PTEST, uData);
		} else {
			uData = mdio_read(softc->mdio, MII_INT, MII_AUXCTL) & 0xffff;
			uData &= ~(AUXCTL_FORCE_LINK);
			mdio_write(softc->mdio, MII_INT, MII_AUXCTL, uData);
		}
		uData = mdio_read(softc->mdio, MII_INT, MII_BMCR) & 0xffff;
		/* enable autonegotiation */
		uData &= ~BMCR_LOOPBACK;
		uData |= (BMCR_ANENABLE | BMCR_RESTARTAN);
		mdio_write(softc->mdio, MII_INT, MII_BMCR, uData);
		/* wait for it to finish */
		for (; to; --to) {
			bolt_usleep(1000);
			uData = mdio_read(softc->mdio, MII_INT, MII_BMSR);
			if (uData & BMSR_ANCOMPLETE)
				break;
		}
	}
}

/*
 * mii_external_Loopback: Set the MII external loopback mode
 */
static void mii_external_Loopback(genet_softc * softc, int bEnable)
{
	uint32_t uData;

	/* enable shadow register */
	uData = mdio_read(softc->mdio, MII_INT, MII_BRCMTEST) & 0xffff;
	uData |= BRCMTEST_SHADOW_ENABLE;
	mdio_write(softc->mdio, MII_INT, MII_BRCMTEST, uData);

	uData = mdio_read(softc->mdio, MII_INT, MII_AUXMODE_3) & 0xffff;
	/* configure remote loopback */
	if (bEnable) {
		uData |= AUXMODE_3_REMOTE_LOOPBACK;
	} else {
		uData &= ~AUXMODE_3_REMOTE_LOOPBACK;
	}
	mdio_write(softc->mdio, MII_INT, MII_AUXMODE_3, uData);

	/* disable shadow register */
	uData = mdio_read(softc->mdio, MII_INT, MII_BRCMTEST) & 0xffff;
	uData &= ~BRCMTEST_SHADOW_ENABLE;
	mdio_write(softc->mdio, MII_INT, MII_BRCMTEST, uData);
}


static void ephy_workaround(genet_softc * softc)
{
	int phy_id = softc->phyaddr;

	/* internal phy only */
	if (strcmp(softc->phyintf, "INT"))
		return;

	/* apply the GPHY workaround for our chip */
	bcm_gphy_workaround(softc->mdio, &phy_id, 1);
}

/*
 * Set MDIO to 64 clock transactions
 */
static void mii_set_mdio_clock(mdio_info_t * mdio, uint32_t phy)
{
	uint32_t uData;

	/* set MDIO reg 0x1D bit 12 */
	uData = (1 << 12);
	mdio_write(mdio, phy, 0x1d, uData);
	/* do read operation to ensure mode has been set */
	uData = mdio_read(mdio, phy, 0x1d);
}

#endif /* CFG_GENET */
