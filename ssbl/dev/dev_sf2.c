/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "common.h"
#include "timer.h"
#include "net_mdio.h"
#include "iocb.h"
#include "ioctl.h"
#include "device.h"
#include "devfuncs.h"
#include "mii.h"
#include "error.h"
#include "bsp_config.h"
#include "board.h"

#include "bchp_switch_reg.h"
#include "dev_sf2.h"

#include "dev_bcmethsw.h"
#include "dev_bcmgphy.h"

#include "lib_printf.h"

#define MDIO_CMD		0x00
#define MDIO_CFG		0x04

#define MDIO_START_BUSY		(1 << 29)
#define MDIO_READ_FAIL		(1 << 28)
#define MDIO_RD			(2 << 26)
#define MDIO_WR			(1 << 26)
#define MDIO_PMD_SHIFT		21
#define MDIO_PMD_MASK		0x1F
#define MDIO_REG_SHIFT		16
#define MDIO_REG_MASK		0x1F

#define DBG_MII_READ		0x0001
#define DBG_MII_WRITE		0x0002

#define PFX			"SF2-mdio: "

typedef struct sf2_softc {
	unsigned int ext_switch_port;
	const enet_params *int_phy_params[NUM_SWITCH_PHY];
	unsigned int debug;
	unsigned long base;
	mdio_info_t *mdio;
} sf2_softc;

static sf2_softc sf2_mdio_softc;

static void sf2_force_link(const enet_params *e)
{
	unsigned int speed;
	unsigned long addr, reg = 0;

	speed = atoi(e->phy_speed);

	switch (e->switch_port) {
	case 0:
		addr = BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0;
		break;
	case 1:
		addr = BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_1;
		break;
	case 2:
		addr = BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_2;
		break;
#ifdef BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_3
	case 3:
		addr = BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_3;
		break;
#endif
#ifdef BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_4
	case 4:
		addr = BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_4;
		break;
#endif
#ifdef BCHP_SWITCH_CORE_STS_OVERRIDE_P5
	case 5:
		if (speed == SPEED_2000)
			reg |= BCHP_SWITCH_CORE_STS_OVERRIDE_P5_GMII_SPEED_UP_2G_MASK;
		addr = BCHP_SWITCH_CORE_STS_OVERRIDE_P5;
		break;
#endif
#ifdef BCHP_SWITCH_CORE_STS_OVERRIDE_P7
	case 7:
		if (speed == SPEED_2000)
			reg |= BCHP_SWITCH_CORE_STS_OVERRIDE_P7_SPEED_MASK;
		addr = BCHP_SWITCH_CORE_STS_OVERRIDE_P7;
		break;
#endif
	default:
		return;
	}

	reg |= BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0_SW_OVERRIDE_MASK;
	reg |= BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0_LINK_STS_MASK;

	if (!strcmp(e->phy_type, "MOCA"))
		reg &= ~BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0_DUPLX_MODE_MASK;
	else
		reg |= BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0_DUPLX_MODE_MASK;

	reg &= ~BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0_SPEED_MASK;
	reg &= ~BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0_RXFLOW_CNTL_MASK;
	reg &= ~BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0_TXFLOW_CNTL_MASK;

	switch (speed) {
	case SPEED_10:
		reg |= (0 << BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0_SPEED_SHIFT);
		break;
	case SPEED_100:
		reg |= (1 << BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0_SPEED_SHIFT);
		break;
	case SPEED_2000:
	case SPEED_1000:
		reg |= (2 << BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0_SPEED_SHIFT);
		break;
	default:
		break;
	}

	BDEV_WR(addr, reg);
}

void sf2_port_isolate_setup(const enet_params *e, unsigned int port_mask)
{
	unsigned long addr, reg = 0;

	switch (e->switch_port) {
	case 0:
		addr = BCHP_SWITCH_CORE_PORT_VLAN_CTL_Port_0;
		break;
	case 1:
		addr = BCHP_SWITCH_CORE_PORT_VLAN_CTL_Port_1;
		break;
	case 2:
		addr = BCHP_SWITCH_CORE_PORT_VLAN_CTL_Port_2;
		break;
#ifdef BCHP_SWITCH_CORE_PORT_VLAN_CTL_Port_3
	case 3:
		addr = BCHP_SWITCH_CORE_PORT_VLAN_CTL_Port_3;
		break;
#endif
#ifdef BCHP_SWITCH_CORE_PORT_VLAN_CTL_Port_4
	case 4:
		addr = BCHP_SWITCH_CORE_PORT_VLAN_CTL_Port_4;
		break;
#endif
#ifdef BCHP_SWITCH_CORE_PORT_VLAN_CTL_Port_5
	case 5:
		addr = BCHP_SWITCH_CORE_PORT_VLAN_CTL_Port_5;
		break;
#endif
	case 7:
		addr = BCHP_SWITCH_CORE_PORT_VLAN_CTL_P7;
		break;
	default:
		return;
	}

	/* Set this port and the IMP to be in the default VLAN */
	reg = BDEV_RD(addr);
	reg &= ~(BCHP_SWITCH_CORE_PORT_VLAN_CTL_Port_0_PORT_EGRESS_EN_MASK);
	if (port_mask)
		reg |= port_mask;
	else
		reg |= (1 << e->switch_port | 1 << 8 | port_mask);
	BDEV_WR(addr, reg);
}

static void sf2_sgphy_init(int phy_id)
{
	uint32_t reg;
#if defined(BCHP_SWITCH_REG_QPHY_CNTRL)
	static int phyad = 32;
	int phy_port;
#endif

#if defined(BCHP_SWITCH_REG_SPHY_CNTRL)
	/* Turn on SGPHY */
	reg = BDEV_RD(BCHP_SWITCH_REG_SPHY_CNTRL);
	reg |= BCHP_SWITCH_REG_SPHY_CNTRL_phy_reset_MASK;
	reg &= ~(BCHP_SWITCH_REG_SPHY_CNTRL_ext_pwr_down_MASK |
		BCHP_SWITCH_REG_SPHY_CNTRL_iddq_bias_MASK |
		BCHP_SWITCH_REG_SPHY_CNTRL_iddq_global_pwr_MASK);

	/* Set phy ID (only makes sense in qphy case */
	reg &= ~(BCHP_SWITCH_REG_SPHY_CNTRL_phy_phyad_MASK);
	reg |= (phy_id << BCHP_SWITCH_REG_SPHY_CNTRL_phy_phyad_SHIFT) &
		BCHP_SWITCH_REG_SPHY_CNTRL_phy_phyad_MASK;

	BDEV_WR(BCHP_SWITCH_REG_SPHY_CNTRL, reg);
	bolt_usleep(21);
	reg = BDEV_RD(BCHP_SWITCH_REG_SPHY_CNTRL);
	reg &= ~BCHP_SWITCH_REG_SPHY_CNTRL_phy_reset_MASK;
	BDEV_WR(BCHP_SWITCH_REG_SPHY_CNTRL, reg);
#elif defined(BCHP_SWITCH_REG_QPHY_CNTRL)
	/* Turn on QGPHY */
	reg = BDEV_RD(BCHP_SWITCH_REG_QPHY_CNTRL);

	if (phyad == 32) {
		/* Set reset bit */
		reg |= BCHP_SWITCH_REG_QPHY_CNTRL_phy_reset_MASK;
		/* Clear iddq_bias */
		reg &= ~BCHP_SWITCH_REG_QPHY_CNTRL_iddq_bias_MASK;
		BDEV_WR(BCHP_SWITCH_REG_QPHY_CNTRL, reg);
		bolt_usleep(20);

		/* Clear reset bit */
		reg = BDEV_RD(BCHP_SWITCH_REG_QPHY_CNTRL);
		reg &= ~BCHP_SWITCH_REG_QPHY_CNTRL_phy_reset_MASK;
		BDEV_WR(BCHP_SWITCH_REG_QPHY_CNTRL, reg);
	}

	if (phy_id < phyad) {
		phyad = phy_id;
		/* Set phy ID (only makes sense in qphy case */
		reg &= ~(BCHP_SWITCH_REG_QPHY_CNTRL_phy_phyad_MASK);
		reg |= (phy_id << BCHP_SWITCH_REG_QPHY_CNTRL_phy_phyad_SHIFT) &
			BCHP_SWITCH_REG_QPHY_CNTRL_phy_phyad_MASK;
	}

	phy_port = phy_id - phyad;
	if (phy_port <= 3) {
		/* Clear power down on this phy */
		reg &= ~(((1 << phy_port)
			<< BCHP_SWITCH_REG_QPHY_CNTRL_ext_pwr_down_SHIFT) &
			BCHP_SWITCH_REG_QPHY_CNTRL_ext_pwr_down_MASK);
	}

	BDEV_WR(BCHP_SWITCH_REG_QPHY_CNTRL, reg);
#else
	#error UNSUPPORTED SF2
#endif
}

static void sf2_led_ctrl_init(unsigned int port)
{
	unsigned long addr;
	uint32_t reg = 0;

	switch (port) {
	case 0:
		addr = BCHP_SWITCH_REG_LED_0_CNTRL;
		break;
	case 1:
		addr = BCHP_SWITCH_REG_LED_1_CNTRL;
		break;
	case 2:
		addr = BCHP_SWITCH_REG_LED_2_CNTRL;
		break;
#ifdef BCHP_SWITCH_REG_LED_3_CNTRL
	case 3:
		addr = BCHP_SWITCH_REG_LED_3_CNTRL;
		break;
#endif
	default:
		return;
	}

	/* gpio_000 -> link, gpio_001 -> activity */
	reg |= 1 << BCHP_SWITCH_REG_LED_0_CNTRL_sel_no_link_encode_SHIFT;
	reg |= 1 << BCHP_SWITCH_REG_LED_0_CNTRL_sel_10m_encode_SHIFT;
	reg |= 1 << BCHP_SWITCH_REG_LED_0_CNTRL_sel_100m_encode_SHIFT;
	reg |= 1 << BCHP_SWITCH_REG_LED_0_CNTRL_sel_1000m_encode_SHIFT;

	/* LED on -> link present, LED off -> no link */
	reg |= 3 << BCHP_SWITCH_REG_LED_0_CNTRL_no_link_encode_SHIFT;

	/* GPHY activity */
	reg |= 1 << BCHP_SWITCH_REG_LED_0_CNTRL_spdlnk_led0_act_sel_SHIFT;
	reg |= 1 << BCHP_SWITCH_REG_LED_0_CNTRL_spdlnk_led1_act_sel_SHIFT;
	reg |= BCHP_SWITCH_REG_LED_0_CNTRL_act_led_act_sel_SHIFT;

	BDEV_WR(addr, reg);

#if defined(BCHP_SWITCH_REG_QPHY_CNTRL)
	reg = BDEV_RD(BCHP_SWITCH_REG_LED_SERIAL_CNTRL);
	reg &= ~(BCHP_SWITCH_REG_LED_SERIAL_CNTRL_smode_MASK);
	reg |= (1 << BCHP_SWITCH_REG_LED_SERIAL_CNTRL_smode_SHIFT);
	reg |= (1 << port | 0x10) << BCHP_SWITCH_REG_LED_SERIAL_CNTRL_port_en_SHIFT;
	BDEV_WR(BCHP_SWITCH_REG_LED_SERIAL_CNTRL, reg);
#endif
}

static unsigned int sf2_rgmii_init(sf2_softc *softc)
{
	unsigned int pmode = 0x3;
	unsigned id_mode = 0;
	const enet_params *e;
	unsigned int i, j;
	unsigned int force_link = 0;
	unsigned int internal_phy_id = PHY_MAX_ADDR + 1;

	/* Configure the RGMII interface based on the board configuration
	 * make sure that we skip over Internal and MoCA PHYs as these
	 * do not require such a configuration.
	 */
	for (i = 0; i < board_num_enet(); i++) {
		e = board_enet(i);
		if (!e)
			continue;

		/* Isolate ports from each other */
		sf2_port_isolate_setup(e, 0);

		if (!strcmp(e->phy_type, "MOCA"))
			continue;

		/* This is the internal GPHY port, retain its MDIO addr */
		if (!strcmp(e->phy_type, "INT")) {
			for (j = 0; j < NUM_SWITCH_PHY; j++) {
				if (!softc->int_phy_params[j]) {
					softc->int_phy_params[j] = e;
					break;
				}
			}
			internal_phy_id = atoi(e->phy_id);
			sf2_sgphy_init(internal_phy_id);
			sf2_led_ctrl_init(e->switch_port);
			continue;
		}

		/* Get the port_mode and id_mode_dis values */
		if (!strcmp(e->phy_type, "GMII"))
			pmode = 0x1;
		if (!strcmp(e->phy_type, "MII"))
			pmode = 0x2;
		if (!strcmp(e->phy_type, "RGMII_NO_ID"))
			id_mode = 1;
		if (!strcmp(e->phy_type, "RVMII"))
			pmode = 0x4;

		/* This port has an external switch, retain the number */
		if (e->ethsw) {
			softc->ext_switch_port = e->switch_port;
			force_link = 1;
		}

		if (!e->mdio_mode || !strcmp(e->mdio_mode, "0"))
			force_link = 1;

		if (force_link)
			sf2_force_link(e);

		/* Enable the RGMII data-path */
		switch (e->switch_port) {
#if defined(BCHP_SWITCH_REG_RGMII_1_CNTRL)
		case 1:
			BDEV_WR_F(SWITCH_REG_RGMII_1_CNTRL, rgmii_mode_en, 1);
			BDEV_WR_F(SWITCH_REG_RGMII_1_CNTRL, port_mode, pmode);
			BDEV_WR_F(SWITCH_REG_RGMII_1_CNTRL, id_mode_dis,
					id_mode);
			break;
#endif
#if defined(BCHP_SWITCH_REG_RGMII_2_CNTRL)
		case 2:
			BDEV_WR_F(SWITCH_REG_RGMII_2_CNTRL, rgmii_mode_en, 1);
			BDEV_WR_F(SWITCH_REG_RGMII_2_CNTRL, port_mode, pmode);
			BDEV_WR_F(SWITCH_REG_RGMII_2_CNTRL, id_mode_dis,
					id_mode);
			break;
#endif
#if defined(BCHP_SWITCH_REG_RGMII_9_CNTRL)
		case 9:
			BDEV_WR_F(SWITCH_REG_RGMII_9_CNTRL, rgmii_mode_en, 1);
			BDEV_WR_F(SWITCH_REG_RGMII_9_CNTRL, port_mode, pmode);
			BDEV_WR_F(SWITCH_REG_RGMII_9_CNTRL, id_mode_dis,
				  id_mode);
			break;
#endif
#if defined(BCHP_SWITCH_REG_RGMII_10_CNTRL)
		case 10:
			BDEV_WR_F(SWITCH_REG_RGMII_10_CNTRL, rgmii_mode_en, 1);
			BDEV_WR_F(SWITCH_REG_RGMII_10_CNTRL, port_mode, pmode);
			BDEV_WR_F(SWITCH_REG_RGMII_10_CNTRL, id_mode_dis,
				  id_mode);
			break;
#endif
		default:
			break;
		}
	}

	return internal_phy_id;
}

static void sf2_mdio_probe(bolt_driver_t * drv, unsigned long probe_a,
				unsigned long probe_b, void *probe_ptr)
{
	sf2_softc *softc = probe_ptr;
	phy_speed_t phy_speed;
	const enet_params *e;
	char buf[255];
	int phy_addr[NUM_SWITCH_PHY] = {0};
	int i, cnt = 0;

	softc->base = BVIRTADDR(BCHP_SWITCH_MDIO_REG_START);
	xsprintf(buf, "%s at 0x%08x", drv->drv_description,
			BPHYSADDR(BCHP_SWITCH_MDIO_REG_START));
	bolt_attach(drv, softc, NULL, buf);

	softc->mdio = mdio_init("mdio0");
	if (!softc->mdio) {
		err_msg(PFX "init failed");
		return;
	}

	/* Apply GPHY workaround */
	for (i = 0; i < NUM_SWITCH_PHY; i++)
		if (softc->int_phy_params[i])
			phy_addr[cnt++] = atoi(softc->int_phy_params[i]->phy_id);

	bcm_gphy_workaround(softc->mdio, &phy_addr[0], cnt);
	for (i = 0; i < NUM_SWITCH_PHY; i++)
		if (softc->int_phy_params[i]) {
			phy_speed = atoi(softc->int_phy_params[i]->phy_speed);
			mdio_set_advert(softc->mdio, phy_addr[i], phy_speed);
		}

	/* If we could detect the external switch, configure it,
	 * the RGMII data path has been configured as part of sf2_rgmii_init()
	 */
	if (ethsw_is_present(softc->mdio)) {
		e = board_enet(softc->ext_switch_port);
		if (!e) {
			err_msg(PFX "missing external switch port");
			return;
		}

		ethsw_reset_ports(softc->mdio, 4, e->phy_type, atoi(e->phy_speed));
		ethsw_switch_unmanage_mode(softc->mdio);
		ethsw_config_learning(softc->mdio);
		ethsw_setup_flow_control(softc->mdio);
	}
}

static int sf2_mdio_open(bolt_devctx_t * ctx)
{
	return 0;
}

static int sf2_mdio_op(sf2_softc *softc, uint32_t opcode,
				uint32_t phy, uint32_t reg, uint16_t *data)
{
	unsigned int cnt = 10;
	uint32_t cmd_data;

	cmd_data = MDIO_START_BUSY | opcode |
			(phy << MDIO_PMD_SHIFT) |
			(reg << MDIO_REG_SHIFT) | *data;
	BDEV_WR(softc->base + MDIO_CMD, cmd_data);

	bolt_msleep(5);

	do {
		cmd_data = BDEV_RD(softc->base + MDIO_CMD);
		/* Broadcom BCM53125 external switches have a hardware bug that
		 * make them fail to release the MDIO lines during turn-around
		 * time, which gets flagged by the MDIO controller as a read
		 * failure, since this is only a problem for pseudo-PHY
		 * addresses, just ignore that error. If we need to read from
		 * the pseudo-PHY, this is because we have been able to detect
		 * an external switch in the first place.
		 */
		if (cmd_data & MDIO_READ_FAIL && phy != 30)
			return -1;

		if (!(cmd_data & MDIO_START_BUSY))
			break;

		bolt_usleep(1000);
	} while (cnt--);

	if (!cnt)
		 return -1;

	*data = cmd_data & 0xffff;

	return 0;
}

#ifdef CONFIG_BCM7445D0
static void sf2_mdio_indir_rw(int op, int addr, uint16_t regnum, uint16_t *val)
{
	uint32_t base = BCHP_SWITCH_CORE_REG_START;
	uint32_t reg;

	BDEV_WR_F(SWITCH_REG_SWITCH_CNTRL, mdio_master_sel, 1);

	/* Page << 8 | offset */
	reg = 0x70;
	reg <<= 2;
	BDEV_WR(base + reg, addr);

	/* Page << 8 | offset */
	reg = 0x80 << 8 | regnum << 1;
	reg <<= 2;

	if (op)
		*val = BDEV_RD(base + reg);
	else
		BDEV_WR(base + reg, *val);

	BDEV_WR_F(SWITCH_REG_SWITCH_CNTRL, mdio_master_sel, 0);
}
#endif /* CONFIG_BCM7445D0 */

static int sf2_mdio_read(bolt_devctx_t *ctx, iocb_buffer_t * buffer)
{
	sf2_softc *softc = (sf2_softc *)ctx->dev_softc;
	mdio_xfer_t *xfer = (mdio_xfer_t *)buffer->buf_ptr;
	int ret = 0;

	switch (xfer->addr) {
#ifdef CONFIG_BCM7445D0
	case 0:
	case 30:
		sf2_mdio_indir_rw(1, xfer->addr, xfer->regnum, &xfer->data);
		break;
#endif
	default:
		ret = sf2_mdio_op(softc, MDIO_RD, xfer->addr, xfer->regnum, &xfer->data);
		break;
	}
	if (softc->debug & DBG_MII_READ)
		xprintf("*** MII read: phy=%d reg=0x%04x data=0x%04x\n",
				xfer->addr, xfer->regnum, xfer->data);

	return ret;
}

static int sf2_mdio_write(bolt_devctx_t *ctx, iocb_buffer_t * buffer)
{
	sf2_softc *softc = (sf2_softc *)ctx->dev_softc;
	mdio_xfer_t *xfer = (mdio_xfer_t *)buffer->buf_ptr;

	if (softc->debug & DBG_MII_WRITE)
		xprintf("*** MII write: phy=%d reg=0x%04x data=0x%04x\n",
			xfer->addr, xfer->regnum, xfer->data);
	switch (xfer->addr) {
#ifdef CONFIG_BCM7445D0
	case 0:
	case 30:
		sf2_mdio_indir_rw(0, xfer->addr, xfer->regnum, &xfer->data);
		break;
#endif
	default:
		sf2_mdio_op(softc, MDIO_WR, xfer->addr, xfer->regnum, &xfer->data);
		break;
	}

	return 0;
}

static int sf2_mdio_close(bolt_devctx_t *ctx)
{
	return 0;
}

static int sf2_mdio_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	sf2_softc *softc = ctx->dev_softc;
	int retval = BOLT_OK;
	int phy_addr[NUM_SWITCH_PHY] = {0};
	int i, cnt = 0;
	unsigned int port, phyid;
	struct ether_phy_info *phy_info;

	buffer->buf_retlen = 0;

	switch ((int)buffer->buf_ioctlcmd) {
	case IOCTL_ETHER_SET_PHY_DEFCONFIG:
		for (i = 0; i < NUM_SWITCH_PHY; i++)
			if (softc->int_phy_params[i])
				phy_addr[cnt++] =
				atoi(softc->int_phy_params[i]->phy_id);
		bcm_gphy_workaround(softc->mdio, &phy_addr[0], cnt);
		break;

	case IOCTL_ETHER_GET_PORT_PHYID:
		port = (unsigned int)buffer->buf_offset;
		if (port >= NUM_SWITCH_PHY) {
			retval = BOLT_ERR_INV_PARAM;
			break;
		}

		if (!softc->int_phy_params[port]) {
			retval = BOLT_ERR_DEVNOTFOUND;
			break;
		}

		phyid = atoi(softc->int_phy_params[port]->phy_id);
		*(int *)(buffer->buf_ptr) = phyid;
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
		phy_info->type = ETH_SF2;
		phy_info->phyaddr = (unsigned long *)softc->base;
		phy_info->version = -1; /* No version */
		buffer->buf_retlen = sizeof(struct ether_phy_info);
	break;

	default:
		err_msg("SF2: Invalid IOCTL");
		retval = BOLT_ERR_UNSUPPORTED;
		break;
	}
	return retval;
}

static const bolt_devdisp_t sf2_mdio_dispatch = {
	.dev_open = sf2_mdio_open,
	.dev_read = sf2_mdio_read,
	.dev_write = sf2_mdio_write,
	.dev_close = sf2_mdio_close,
	.dev_ioctl = sf2_mdio_ioctl,
};

static bolt_driver_t sf2_mdiodrv = {
	.drv_description = "SF2 MDIO",
	.drv_bootname = "mdio",
	.drv_class = BOLT_DEV_OTHER,
	.drv_dispatch = &sf2_mdio_dispatch,
	.drv_probe = sf2_mdio_probe,
};

static void bcm_sf2_mgmt_init(void)
{
	uint32_t reg;

	/* Enable IMP port (IMP0) */
	BDEV_WR(BCHP_SWITCH_CORE_GMNGCFG, 0x80);

	/* Enable managed mode, software forwarding */
	reg = BDEV_RD(BCHP_SWITCH_CORE_SWMODE);
	reg |= BCHP_SWITCH_CORE_SWMODE_SW_FWDG_MODE_MASK |
		BCHP_SWITCH_CORE_SWMODE_SW_FWDG_EN_MASK;
	BDEV_WR(BCHP_SWITCH_CORE_SWMODE, reg);

	/* Disable software learning on all ports */
	BDEV_WR(BCHP_SWITCH_CORE_SFT_LRN_CTL, 0);

	/* Enable learning on all ports */
	BDEV_WR(BCHP_SWITCH_CORE_DIS_LEARN, 0);

	/* Unicast forward on ARL miss */
	BDEV_WR_F(SWITCH_CORE_NEW_CTRL, UC_FWD_EN, 1);

	/* Forward unicast lookup failures to all ports */
	BDEV_WR(BCHP_SWITCH_CORE_ULF_DROP_MAP,
		BCHP_SWITCH_CORE_ULF_DROP_MAP_UNI_LOOKUP_FAIL_FWD_MAP_MASK);

	/* Drop multicast forward lookup failures */
	BDEV_WR(BCHP_SWITCH_CORE_MLF_DROP_MAP, 0);

	/* Define no ports in IP multicast forward */
	BDEV_WR(BCHP_SWITCH_CORE_MLF_IPMC_FWD_MAP, 0);

	/* Accept only broadcast and unicast for IMP port */
	reg = BDEV_RD(BCHP_SWITCH_CORE_IMP_CTL);
	reg |= BCHP_SWITCH_CORE_IMP_CTL_RX_UCST_EN_MASK |
		BCHP_SWITCH_CORE_IMP_CTL_RX_BCST_EN_MASK;
	BDEV_WR(BCHP_SWITCH_CORE_IMP_CTL, reg);
}

mdio_info_t *bcm_sf2_mdio_init(void)
{
	unsigned int internal_phy_id;

	/* Configure the SF2 RGMII blocks */
	internal_phy_id = sf2_rgmii_init(&sf2_mdio_softc);

	/* Register the SF2 MDIO block driver */
	bolt_add_device(&sf2_mdiodrv, 0, 0, &sf2_mdio_softc);

	sf2_mdio_softc.mdio->phy_id = internal_phy_id;
	return sf2_mdio_softc.mdio;
}

void bcm_sf2_init(void)
{
	uint32_t reg;

	/* Enable ports */
	BDEV_WR(BCHP_SWITCH_CORE_G_PCTL_Port_0, 0);
	BDEV_WR(BCHP_SWITCH_CORE_G_PCTL_Port_1, 0);
	BDEV_WR(BCHP_SWITCH_CORE_G_PCTL_Port_2, 0);
#ifdef BCHP_SWITCH_CORE_G_PCTL_Port_3
	BDEV_WR(BCHP_SWITCH_CORE_G_PCTL_Port_3, 0);
#endif
#ifdef BCHP_SWITCH_CORE_G_PCTL_Port_4
	BDEV_WR(BCHP_SWITCH_CORE_G_PCTL_Port_4, 0);
#endif
#ifdef BCHP_SWITCH_CORE_G_PCTL_Port_5
	BDEV_WR(BCHP_SWITCH_CORE_G_PCTL_Port_5, 0);
#endif
#ifdef BCHP_SWITCH_CORE_P7_CTL
	BDEV_WR(BCHP_SWITCH_CORE_P7_CTL, 0);
#endif

	BDEV_WR(BCHP_SWITCH_CORE_SWITCH_CTRL,
			BCHP_SWITCH_CORE_SWITCH_CTRL_MII_DUMB_FWDG_EN_MASK);

	/* Force IMP port mode: 1Gbits/sec, no pause, link up, full duplex */
	reg = BDEV_RD(BCHP_SWITCH_CORE_STS_OVERRIDE_IMP);
	reg |= BCHP_SWITCH_CORE_STS_OVERRIDE_IMP_GMII_SPEED_UP_2G_MASK;
	reg &= ~(BCHP_SWITCH_CORE_STS_OVERRIDE_IMP_TXFLOW_CNTL_MASK |
		BCHP_SWITCH_CORE_STS_OVERRIDE_IMP_RXFLOW_CNTL_MASK |
		BCHP_SWITCH_CORE_STS_OVERRIDE_IMP_SPEED_MASK);
	reg |= (BCHP_SWITCH_CORE_STS_OVERRIDE_IMP_MII_SW_OR_MASK |
		BCHP_SWITCH_CORE_STS_OVERRIDE_IMP_LINK_STS_MASK |
		2 << BCHP_SWITCH_CORE_STS_OVERRIDE_IMP_SPEED_SHIFT |
		BCHP_SWITCH_CORE_STS_OVERRIDE_IMP_DUPLX_MODE_MASK);
	BDEV_WR(BCHP_SWITCH_CORE_STS_OVERRIDE_IMP, reg);

	/* Disable broadcom tag for IMP port */
	BDEV_WR(BCHP_SWITCH_CORE_BRCM_HDR_CTRL, 0);

	bcm_sf2_mgmt_init();
}

void bcm_sf2_exit(void)
{
	unsigned int timeout = 1000;
	uint32_t reg;

	/* Software reset the switch */
	reg = BDEV_RD(BCHP_SWITCH_CORE_WATCH_DOG_CTRL);
	reg |= BCHP_SWITCH_CORE_WATCH_DOG_CTRL_SOFTWARE_RESET_MASK |
		BCHP_SWITCH_CORE_WATCH_DOG_CTRL_EN_CHIP_RST_MASK |
		BCHP_SWITCH_CORE_WATCH_DOG_CTRL_EN_SW_RESET_MASK;
	BDEV_WR(BCHP_SWITCH_CORE_WATCH_DOG_CTRL, reg);

	do {
		reg = BDEV_RD(BCHP_SWITCH_CORE_WATCH_DOG_CTRL);
		if (!(reg & BCHP_SWITCH_CORE_WATCH_DOG_CTRL_SOFTWARE_RESET_MASK))
			break;

		bolt_msleep(1);
	} while (timeout-- > 0);

	if (timeout == 0)
		err_msg("failed to software reset switch!\n");
}
void bcm_sf2_multicast_enable(void)
{
	uint32_t reg;
	/* Accept only broadcast, multicast  and unicast for IMP port */
	reg = BDEV_RD(BCHP_SWITCH_CORE_IMP_CTL);
	reg |= BCHP_SWITCH_CORE_IMP_CTL_RX_UCST_EN_MASK |
		BCHP_SWITCH_CORE_IMP_CTL_RX_MCST_EN_MASK |
		BCHP_SWITCH_CORE_IMP_CTL_RX_BCST_EN_MASK;
	BDEV_WR(BCHP_SWITCH_CORE_IMP_CTL, reg);
}
