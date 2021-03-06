/***************************************************************************
 *     Copyright (c) 2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#if CFG_SYSTEMPORT

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
#include "common.h"

#include "bsp_config.h"
#include "dev_systemport.h"
#include "timer.h"
#include "macutils.h"
#include "ui_command.h"
#include "env_subr.h"
#include "dev_bcmethsw.h"
#include "dev_sf2.h"
#include "board.h"
#include "net_ebuf.h"
#include "net_mdio.h"

#include "libfdt_env.h"
#include "fdt.h"
#include "devtree.h"

#define PFX	"SYSTEMPORT: "

/* Do not use the full RX buffers space */
#define NUM_RX_BUFFERS	(NUM_RX_DESC / 8)

/* Use the first transmit ring by default, nothing fancy */
#define TX_RING	0

typedef struct sysport_softc {
	physaddr_t base;

	uint8_t *rx_buffers;
	unsigned long rx_bds;
	uint8_t rx_read_ptr;
	uint16_t rx_c_index;

	struct dma_desc *tx_dma_desc;
	uint8_t *tx_buffer;

	uint8_t macaddr[6];
	int instance;
	mdio_info_t *mdio;
	phy_speed_t speed;
} sysport_softc;

#define SYSPORT_IO_MACRO(name, offset)					\
static inline uint32_t name##_readl(sysport_softc *softc, uint32_t off)	\
{									\
	uint32_t val = DEV_RD(softc->base + offset + (off));	\
	if (0) xprintf("R:%d " __stringify(name) ", offset: 0x%08x, val: 0x%08x\n", \
			__LINE__, softc->base + offset + off, val);	\
	return val;							\
}									\
									\
static inline void name##_writel(sysport_softc *softc, uint32_t val,	\
		uint32_t off)						\
{									\
	if (0) xprintf("W:%d " __stringify(name) ", offset: 0x%08x, val: 0x%08x\n", \
			__LINE__, softc->base + offset + off, val);	\
	DEV_WR(softc->base + offset + (off), (val));		\
}									\

SYSPORT_IO_MACRO(umac, SYS_PORT_UMAC_OFFSET);
SYSPORT_IO_MACRO(tdma, SYS_PORT_TDMA_OFFSET);
SYSPORT_IO_MACRO(rdma, SYS_PORT_RDMA_OFFSET);
SYSPORT_IO_MACRO(topctrl, SYS_PORT_TOPCTRL_OFFSET);
SYSPORT_IO_MACRO(rbuf, SYS_PORT_RBUF_OFFSET);

static int Enet_debug;
#define DBG_RX			0x0010
#define DBG_RX_ERR		0x0020
#define DBG_TX			0x0040
#define DBG_MII_CMD		0x0100
#define DBG_MII_WRITE		0x0200
#define DBG_MII_READ		0x0400
#define DBG_MII_RD_FAIL		0x0800
#define DBG_EPHY_PROBE		0x1000

static void unimac_reset(sysport_softc *softc)
{
	/* Disable MAC while updating configuration */
	umac_writel(softc, 0, UMAC_CMD);

	/* Perform a soft reset */
	umac_writel(softc, CMD_SW_RESET, UMAC_CMD);
	bolt_usleep(1);
	umac_writel(softc, 0, UMAC_CMD);
}

static void unimac_set_speed(sysport_softc *softc)
{
	uint32_t reg;

	/* Force the link to 1Gbits/sec */
	reg = umac_readl(softc, UMAC_CMD);
	reg &= ~CMD_HD_EN;
	reg &= ~(CMD_SPEED_MASK << CMD_SPEED_SHIFT);
	reg |= (CMD_SPEED_1000 << CMD_SPEED_SHIFT);
	umac_writel(softc, reg, UMAC_CMD);
}

static inline unsigned int unimac_enabled(sysport_softc *softc)
{
	return umac_readl(softc, UMAC_CMD) & (CMD_TX_EN | CMD_RX_EN);
}

static inline void unimac_on(sysport_softc *softc)
{
	uint32_t reg;

	reg = umac_readl(softc, UMAC_CMD);
	reg |= CMD_TX_EN | CMD_RX_EN;
	umac_writel(softc, reg, UMAC_CMD);
}

static inline void unimac_off(sysport_softc *softc)
{
	uint32_t reg;

	reg = umac_readl(softc, UMAC_CMD);
	reg &= ~(CMD_TX_EN | CMD_RX_EN);
	umac_writel(softc, reg, UMAC_CMD);
}

static void unimac_init(sysport_softc *softc)
{
	uint32_t reg;

	unimac_reset(softc);
	unimac_set_speed(softc);

	/* Clear MIB RX/TX counters */
	reg = MIB_RX_CNT_RST | MIB_TX_CNT_RST | MIB_RUNT_CNT_RST;
	umac_writel(softc, reg, UMAC_MIB_CTRL);
	umac_writel(softc, 0, UMAC_MIB_CTRL);

	/* Configure maximum acceptable packet size */
	umac_writel(softc, UMAC_MAX_MTU_SIZE, UMAC_MAX_FRAME_LEN);

	/* Enable promiscuous mode since we might receive packets
	 * from the switch which will forward packets not necessarily
	 * for our CPU interface only.
	 */
	reg = umac_readl(softc, UMAC_CMD);
	reg |= CMD_PROMISC;
	umac_writel(softc, reg, UMAC_CMD);
}



static void unimac_write_hwaddr(sysport_softc *softc, uint8_t *addr)
{
	unsigned int enabled = unimac_enabled(softc);

	memcpy(softc->macaddr, addr, 6);

	if (enabled)
		unimac_off(softc);

	umac_writel(softc, (addr[0] << 24 | addr[1] << 16 |
				addr[2] << 8 | addr[3]), UMAC_MAC0);
	umac_writel(softc, addr[4] << 8 | addr[5], UMAC_MAC1);

	if (enabled)
		unimac_on(softc);
}

static void topctrl_flush(sysport_softc *softc)
{
	/* Flush TX and RX at TOPCTRL level, wait for at least 1.5ms
	 * for a full-sized packet to be drained.
	 */
	topctrl_writel(softc, RX_FLUSH, RX_FLUSH_CNTL);
	topctrl_writel(softc, TX_FLUSH, TX_FLUSH_CNTL);
	bolt_msleep(2);
	topctrl_writel(softc, 0, RX_FLUSH_CNTL);
	topctrl_writel(softc, 0, TX_FLUSH_CNTL);
}

static void rbuf_init(sysport_softc *softc)
{
	uint32_t reg;

	reg = rbuf_readl(softc, RBUF_CONTROL);
	reg &= ~(RBUF_4B_ALGN | RBUF_RSB_EN);
	rbuf_writel(softc, reg, RBUF_CONTROL);
}

static int init_buffers(sysport_softc *softc)
{
	unsigned int i;
	uint32_t addr;

	softc->rx_bds = softc->base + SYS_PORT_RDMA_OFFSET;
	softc->rx_read_ptr = 0;
	softc->rx_c_index = 0;
	softc->rx_buffers = KMALLOC(NUM_RX_BUFFERS * RX_BUF_LENGTH, DMA_BUF_ALIGN);
	if (!softc->rx_buffers) {
		xprintf(PFX "Failed to allocate RX buffers\n");
		return -1;
	}

	for (i = 0; i < NUM_RX_BUFFERS; i++) {
		addr = PHYSADDR((uint32_t)(softc->rx_buffers + i * RX_BUF_LENGTH));
		BDEV_WR(softc->rx_bds + (i * DESC_SIZE) + DESC_ADDR_LO,
				addr);
	}

	/* Bounce buffer for packet contents */
	softc->tx_buffer = KMALLOC(RX_BUF_LENGTH, DMA_BUF_ALIGN);
	if (!softc->tx_buffer) {
		xprintf(PFX "Failed to allocate TX buffer\n");
		KFREE(softc->rx_buffers);
		return -1;
	}

	/* Transmit descriptor where buffers are DMA'd from
	 * Note that Tx descriptor is allocated from uncached memory
	 * as this is not performance critical. */
	softc->tx_dma_desc = KUMALLOC(sizeof(*softc->tx_dma_desc), DMA_BUF_ALIGN);
	if (!softc->tx_dma_desc) {
		xprintf(PFX "Failed to allocate TX DMA descriptor\n");
		KFREE(softc->rx_buffers);
		KFREE(softc->tx_buffer);
		return -1;
	}

	return 0;
}

static void init_dma(sysport_softc *softc)
{
	/* Initialize RDMA first */
	rdma_writel(softc, 0, RDMA_WRITE_PTR_LO);
	rdma_writel(softc, 0, RDMA_WRITE_PTR_HI);
	rdma_writel(softc, 0, RDMA_PROD_INDEX);
	rdma_writel(softc, 0, RDMA_CONS_INDEX);
	rdma_writel(softc, NUM_RX_BUFFERS << RDMA_RING_SIZE_SHIFT |
				RX_BUF_LENGTH, RDMA_RING_BUF_SIZE);
	rdma_writel(softc, 0, RDMA_START_ADDR_HI);
	rdma_writel(softc, 0, RDMA_START_ADDR_LO);
	rdma_writel(softc, 0, RDMA_END_ADDR_HI);
	rdma_writel(softc, NUM_RX_BUFFERS * WORDS_PER_DESC - 1, RDMA_END_ADDR_LO);

	/* Initialize TDMA */
	tdma_writel(softc, TDMA_LL_RAM_INIT_BUSY, TDMA_STATUS);
	tdma_writel(softc, 0, TDMA_DESC_RING_COUNT(TX_RING));
	tdma_writel(softc, 0, TDMA_DESC_RING_INTR_CONTROL(TX_RING));
	tdma_writel(softc, 0, TDMA_DESC_RING_PROD_CONS_INDEX(TX_RING));
	tdma_writel(softc, 0, TDMA_DESC_RING_PCP_DEI_VID(TX_RING));

	/* Ignore congestion information from the switch */
	tdma_writel(softc, RING_IGNORE_STATUS, TDMA_DESC_RING_MAPPING(TX_RING));

	/* Set a high enough threshold, otherwise TDMA will assert
	 * the over hysteresis bits and won't produce packets
	 */
	tdma_writel(softc, 16 << RING_HYST_THRESH_SHIFT | 32,
			TDMA_DESC_RING_MAX_HYST(TX_RING));

	/* Enable ring */
	tdma_writel(softc, RING_EN, TDMA_DESC_RING_HEAD_TAIL_PTR(TX_RING));

	/* Enable TX_RING (0) at the Tier-1 Arbiter */
	tdma_writel(softc, (1 << TX_RING), TDMA_TIER1_ARB_0_QUEUE_EN);
}

static int rdma_enable_set(sysport_softc *softc, unsigned int enable)
{
	unsigned int timeout = 1000;
	uint32_t reg;

	reg = rdma_readl(softc, RDMA_CONTROL);
	if (enable)
		reg |= RDMA_EN;
	else
		reg &= ~RDMA_EN;
	rdma_writel(softc, reg, RDMA_CONTROL);

	/* Poll for DMA disabling completion */
	do {
		reg = rdma_readl(softc, RDMA_STATUS);
		if (!!(reg & RDMA_DISABLED) == !enable)
			return 0;

		bolt_usleep(10);
	} while (timeout-- > 0);

	err_msg("%s: timeout waiting for RDMA to finish!", __func__);

	return -1;
}

static int tdma_enable_set(sysport_softc *softc, unsigned int enable)
{
	unsigned int timeout = 1000;
	uint32_t reg;

	reg = tdma_readl(softc, TDMA_CONTROL);
	if (enable)
		reg |= TDMA_EN;
	else
		reg &= ~TDMA_EN;
	tdma_writel(softc, reg, TDMA_CONTROL);

	/* Poll for DMA enabling/disabling completion */
	do {
		reg = tdma_readl(softc, TDMA_STATUS);
		if (!!(reg & TDMA_DISABLED) == !enable)
			return 0;

		bolt_usleep(10);
	} while (timeout-- > 0);

	err_msg("%s: timeout waiting for TDMA to finish!", __func__);

	return -1;
}

static int tdma_ll_ram_poll(sysport_softc *softc)
{
	unsigned int timeout = 1000;
	uint32_t reg;

	do {
		reg = tdma_readl(softc, TDMA_STATUS);
		if (!(reg & TDMA_LL_RAM_INIT_BUSY))
			return 0;

		bolt_usleep(10);
	} while (timeout-- > 0);

	err_msg("%s: timeout waiting for DESC_RAM to clear!", __func__);

	return -1;
}

static int sysport_ether_open(bolt_devctx_t *ctx)
{
	sysport_softc *softc = ctx->dev_softc;
	int ret;

	/* Run the switch initialization function */
	bcm_sf2_init();

	if ((Enet_debug = env_getval("ETH_DBG")) < 0)
		Enet_debug = 0;

	rbuf_init(softc);

	init_buffers(softc);

	init_dma(softc);

	unimac_init(softc);

	unimac_write_hwaddr(softc, softc->macaddr);

	unimac_on(softc);

	/* Kick DMA engine */
	ret = rdma_enable_set(softc, 1);
	if (ret)
		return ret;

	return tdma_enable_set(softc, 1);
}

#define DMA_RX_ERR_MASK		(RX_STATUS_ERR | \
				RX_STATUS_OVFLOW)

static int sysport_ether_read(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	sysport_softc *softc = ctx->dev_softc;
	uint8_t *dst_ptr, *src_ptr;
	uint32_t dma_flags_len, addr;
	uint16_t dma_flags, len;
	uint16_t p_index, c_index;
	int rc = 0;

	p_index = rdma_readl(softc, RDMA_PROD_INDEX) & RDMA_PROD_INDEX_MASK;
	c_index = rdma_readl(softc, RDMA_CONS_INDEX) & RDMA_CONS_INDEX_MASK;

	addr = BDEV_RD(softc->rx_bds +
			(softc->rx_read_ptr * DESC_SIZE) + DESC_ADDR_LO);
	dma_flags_len = BDEV_RD(softc->rx_bds +
			(softc->rx_read_ptr * DESC_SIZE) +
			DESC_ADDR_HI_STATUS_LEN);

	/* Extract DMA flags and length */
	dma_flags = (dma_flags_len >> DESC_STATUS_SHIFT) & DESC_STATUS_MASK;
	len = (dma_flags_len >> DESC_LEN_SHIFT) & DESC_LEN_MASK;

	if (dma_flags & DMA_RX_ERR_MASK) {
		if (Enet_debug & DBG_RX_ERR)
			xprintf(PFX "dropping error packet with: 0x%04x\n",
					dma_flags & DMA_RX_ERR_MASK);
		rc = -1;
		goto out;
	}

	dst_ptr = buffer->buf_ptr;
	buffer->buf_retlen = len;

	if (buffer->buf_retlen < ENET_ZLEN) {
		if (Enet_debug & DBG_RX_ERR)
			xprintf(PFX "dropping small packet: %d\n", len);
		rc = -1;
		goto out;
	}

	src_ptr = (uint8_t *)CACADDR(addr);
	CACHE_INVAL_RANGE(src_ptr, buffer->buf_retlen);

	if (Enet_debug & DBG_RX) {
		xprintf("*** RX: p_index=%d c_index=%d read_ptr=%d len=%d status=%#04x\n",
				p_index, c_index, softc->rx_read_ptr,
				len, dma_flags);
		hexdump(src_ptr, buffer->buf_retlen);
	}

	memcpy(dst_ptr, src_ptr, buffer->buf_retlen);
out:
	softc->rx_c_index++;
	softc->rx_c_index &= RDMA_CONS_INDEX_MASK;
	rdma_writel(softc, softc->rx_c_index, RDMA_CONS_INDEX);

	/* Advance read pointer */
	softc->rx_read_ptr++;
	softc->rx_read_ptr &= (NUM_RX_BUFFERS - 1);

	return rc;
}

static int sysport_ether_inpstat(bolt_devctx_t *ctx, iocb_inpstat_t *inpstat)
{
	sysport_softc *softc = ctx->dev_softc;
	uint32_t p_index = 0, c_index = 0;

	if (!softc || !inpstat)
		return -1;

	p_index = rdma_readl(softc, RDMA_PROD_INDEX) & RDMA_PROD_INDEX_MASK;
	c_index = rdma_readl(softc, RDMA_CONS_INDEX) & RDMA_CONS_INDEX_MASK;

	inpstat->inp_status = (p_index != c_index);

	return 0;
}

static int sysport_ether_write(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	sysport_softc *softc = ctx->dev_softc;
	struct dma_desc *desc;
	uint32_t addr_status_len;
	uint32_t p_index = 0, c_index = 0;
	unsigned int timeout = 1000;

	if (!softc || !buffer)
		return -1;

	desc = softc->tx_dma_desc;

	if (buffer->buf_length > UMAC_MAX_MTU_SIZE) {
		if (Enet_debug & DBG_TX)
			xprintf(PFX "buffer is too large: %d\n",
					buffer->buf_length);
		return -1;
	}

	p_index = tdma_readl(softc, TDMA_DESC_RING_PROD_CONS_INDEX(TX_RING));
	c_index = (p_index >> RING_CONS_INDEX_SHIFT);
	p_index &= RING_PROD_INDEX_MASK;
	c_index &= RING_CONS_INDEX_MASK;

	if (Enet_debug & DBG_TX) {
		xprintf("*** TX: p_index=%d c_index=%d len=%d\n",
				p_index, c_index, buffer->buf_length);
		hexdump(buffer->buf_ptr, buffer->buf_length);
	}

	if (p_index != c_index) {
		xprintf(PFX "Transmit DMA operation in progress.\n");
		return -1;
	}

	memcpy(softc->tx_buffer, buffer->buf_ptr, buffer->buf_length);

	CACHE_FLUSH_RANGE(softc->tx_buffer, buffer->buf_length);

	desc->addr_lo = (uint32_t)CACADDR(softc->tx_buffer);
	addr_status_len = (buffer->buf_length << DESC_LEN_SHIFT);
	addr_status_len |= (DESC_SOP | DESC_EOP | TX_STATUS_APP_CRC) <<
				DESC_STATUS_SHIFT;
	/* Make sure the pending writes to the descriptors are complete
	 * before attempting to write that descriptor address into the
	 * TX Ring write port
	 */
	dsb();
	desc->addr_status_len = addr_status_len;
	dsb();

	/* Write the two 32-bits words into the write port pairs */
	tdma_writel(softc, desc->addr_status_len, TDMA_WRITE_PORT_HI(TX_RING));
	tdma_writel(softc, desc->addr_lo, TDMA_WRITE_PORT_LO(TX_RING));

	/* Poll TDMA for completion */
	do {
		p_index = tdma_readl(softc, TDMA_DESC_RING_PROD_CONS_INDEX(TX_RING));
		c_index = (p_index >> RING_CONS_INDEX_SHIFT);
		p_index &= RING_PROD_INDEX_MASK;
		c_index &= RING_CONS_INDEX_MASK;
	} while (p_index != c_index && timeout--);

	if (timeout == 0) {
		if (Enet_debug & DBG_TX)
			xprintf("*** TX: p_index=%d, c_index=%d, timeout\n",
					p_index, c_index);
		return -1;
	}

	return 0;
}

static int sysport_ether_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	sysport_softc *softc = ctx->dev_softc;
	int cmd = (int)buffer->buf_ioctlcmd;
	int rc = 0;

	switch (cmd) {
	case IOCTL_ETHER_GETHWADDR:
		memcpy(buffer->buf_ptr, softc->macaddr, 6);
		break;

	case IOCTL_ETHER_SETHWADDR:
		unimac_write_hwaddr(softc, buffer->buf_ptr);
		break;

	case IOCTL_ETHER_GETSPEED:
		rc = SPEED_1000;
		break;

	case IOCTL_ETHER_SETSPEED:
		xprintf(PFX "cannot change link speed\n");
		rc = -1;
		break;

	case IOCTL_ETHER_GETLINK:
		rc = 1;
		break;

	default:
		xprintf(PFX "Unhandled ioctl value: %d\n", cmd);
		break;
	}

	return rc;
}

static int sysport_ether_close(bolt_devctx_t *ctx)
{
	sysport_softc *softc = ctx->dev_softc;
	uint32_t reg;
	int ret;

	/* Disable UniMAC RX */
	reg = umac_readl(softc, UMAC_CMD);
	reg &= ~CMD_RX_EN;
	umac_writel(softc, reg, UMAC_CMD);

	/* Poll for RDMA disabling */
	ret = rdma_enable_set(softc, 0);
	if (ret)
		return ret;

	ret = tdma_enable_set(softc, 0);
	if (ret)
		return ret;

	/* Disable UniMAC TX */
	reg = umac_readl(softc, UMAC_CMD);
	reg &= ~CMD_TX_EN;
	umac_writel(softc, reg, UMAC_CMD);

	/* Wait for a full-sized packet to be drained */
	bolt_msleep(2);

	ret = tdma_ll_ram_poll(softc);
	if (ret)
		return ret;

	topctrl_flush(softc);

	KFREE(softc->rx_buffers);
	softc->rx_buffers = NULL;
	KFREE(softc->tx_buffer);
	softc->tx_buffer = NULL;
	KUFREE(softc->tx_dma_desc);
	softc->tx_dma_desc = NULL;

	/* Run the switch exit function */
	bcm_sf2_exit();

	return 0;
}

static unsigned long sysport_dt_init(sysport_softc *softc)
{
	bolt_devtree_params_t params;
	const struct fdt_property *prop;
	int offset = 0;
	int proplen;
	char *node_name = "ethernet@";
	const char *name;
	int depth;
	void *current_dtb;

	bolt_devtree_getenvs(&params);
	current_dtb = params.dt_address;
	depth = 0;
	offset = 0;
	offset = fdt_next_node(current_dtb, offset, &depth);

	while (1) {
		if ((offset < 0) || (depth < 1))
			break;

		name = fdt_get_name(current_dtb, offset, NULL);
		if (strncmp(name, node_name, strlen(node_name)) == 0) {
			prop = fdt_get_property(current_dtb, offset,
						"compatible", NULL);

			if (strcmp(prop->data, "brcm,systemport-v1.00") == 0) {
				prop = fdt_get_property(current_dtb, offset,
							"reg", &proplen);
				if (proplen < (int)((sizeof(uint32_t) * 2)))
					return 0;

				return DT_PROP_DATA_TO_U32(prop->data, 0);
			}
		}
		offset = fdt_next_node(current_dtb, offset, &depth);
	}

	return 0;
}

static void sysport_ether_init(bolt_driver_t *drv, int instance)
{
	sysport_softc *softc;
	char desc[100], *envmac;

	softc = KUMALLOC(sizeof(*softc), 0);
	if (!softc) {
		xprintf(PFX "Failed to allocate softc\n");
		return;
	}
	memset(softc, 0, sizeof(*softc));

	softc->instance = instance;
	softc->base = sysport_dt_init(softc);
	if (!softc->base) {
		xprintf(PFX "Failed to find base address\n");
		KUFREE(softc);
		return;
	}

	envmac = env_getenv(MACADDR_ENVSTR);
	if (envmac)
		MAC_str2bin(envmac, softc->macaddr);

	xsprintf(desc, "%s at 0x%08x", drv->drv_description, softc->base);
	bolt_attach(drv, softc, NULL, desc);

	/* Initialize the switch MDIO bus for early access */
	bcm_sf2_mdio_init();
}

static void sysport_ether_probe(bolt_driver_t *drv, unsigned long probe_a,
				unsigned long probe_b, void *probe_ptr)
{
	if (!env_getenv("ETH_OFF"))
		sysport_ether_init(drv, (int)probe_ptr);
}

static const bolt_devdisp_t sysport_ether_dispatch = {
	.dev_open = sysport_ether_open,
	.dev_read = sysport_ether_read,
	.dev_inpstat = sysport_ether_inpstat,
	.dev_write = sysport_ether_write,
	.dev_ioctl = sysport_ether_ioctl,
	.dev_close = sysport_ether_close,
};

const bolt_driver_t sysportdrv = {
	"SYSTEMPORT Internal Ethernet",
	"eth",
	BOLT_DEV_NETWORK,
	&sysport_ether_dispatch,
	sysport_ether_probe
};

#endif /* CFG_SYSTEMPORT */
