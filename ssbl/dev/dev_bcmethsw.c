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
#include "dev_bcmethsw.h"
#include "lib_types.h"
#include "lib_string.h"
#include "lib_printf.h"
#include "timer.h"
#include "net_mdio.h"

/* PSEUDO PHY register access through MDC/MDIO */

/*
 * When reading or writing PSEUDO PHY registers, use the exact starting address
 * and exact length for each register as defined in the data sheet.
 * In other words, for example, dividing a 32-bit register read into two 16-bit
 * reads will produce wrong result.  Neither can we start read/write from the
 * middle of a register.  Yet another bad example is trying
 * to read a 32-bit register as a 48-bit one.  This is very important!!
 */

static void ethsw_mdio_rreg(mdio_info_t *mdio, unsigned int phy_id,
			    int page, int reg, unsigned char *data, int len)
{
	int v;
	int i;

	v = (page << REG_PPM_REG16_SWITCH_PAGE_NUMBER_SHIFT) |
	    REG_PPM_REG16_MDIO_ENABLE;
	mdio_write(mdio, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG16, v);

	v = (reg << REG_PPM_REG17_REG_NUMBER_SHIFT) | REG_PPM_REG17_OP_READ;
	mdio_write(mdio, phy_id, REG_PSEUDO_PHY_MII_REG17, v);

	for (i = 0; i < 5; i++) {
		v = mdio_read(mdio, phy_id, REG_PSEUDO_PHY_MII_REG17);

		if ((v & (REG_PPM_REG17_OP_WRITE | REG_PPM_REG17_OP_READ)) ==
		    REG_PPM_REG17_OP_DONE)
			break;

		bolt_usleep(10);
	}

	if (i == 5) {
		xprintf("ethsw_mdio_rreg: timeout!\n");
		v = page << REG_PPM_REG16_SWITCH_PAGE_NUMBER_SHIFT;
		mdio_write(mdio, phy_id, REG_PSEUDO_PHY_MII_REG16, v);
		return;
	}

	switch (len) {
	case 1:
		v = mdio_read(mdio, phy_id, REG_PSEUDO_PHY_MII_REG24);
		data[0] = (unsigned char)v;
		break;
	case 2:
		v = mdio_read(mdio, phy_id, REG_PSEUDO_PHY_MII_REG24);
		((unsigned short *)data)[0] = (unsigned short)v;
		break;
	case 4:
		v = mdio_read(mdio, phy_id, REG_PSEUDO_PHY_MII_REG24);
		((unsigned short *)data)[0] = (unsigned short)v;
		v = mdio_read(mdio, phy_id, REG_PSEUDO_PHY_MII_REG25);
		((unsigned short *)data)[1] = (unsigned short)v;
		break;
	case 6:
		v = mdio_read(mdio, phy_id, REG_PSEUDO_PHY_MII_REG24);
		((unsigned short *)data)[0] = (unsigned short)v;
		v = mdio_read(mdio, phy_id, REG_PSEUDO_PHY_MII_REG25);
		((unsigned short *)data)[1] = (unsigned short)v;
		v = mdio_read(mdio, phy_id, REG_PSEUDO_PHY_MII_REG26);
		((unsigned short *)data)[2] = (unsigned short)v;
		break;
	case 8:
		v = mdio_read(mdio, phy_id, REG_PSEUDO_PHY_MII_REG24);
		((unsigned short *)data)[0] = (unsigned short)v;
		v = mdio_read(mdio, phy_id, REG_PSEUDO_PHY_MII_REG25);
		((unsigned short *)data)[1] = (unsigned short)v;
		v = mdio_read(mdio, phy_id, REG_PSEUDO_PHY_MII_REG26);
		((unsigned short *)data)[2] = (unsigned short)v;
		v = mdio_read(mdio, phy_id, REG_PSEUDO_PHY_MII_REG27);
		((unsigned short *)data)[3] = (unsigned short)v;
		break;
	}
	v = page << REG_PPM_REG16_SWITCH_PAGE_NUMBER_SHIFT;
	mdio_write(mdio, phy_id, REG_PSEUDO_PHY_MII_REG16, v);
}

static void ethsw_mdio_wreg(mdio_info_t *mdio, unsigned int phy_id,
			    int page, int reg, unsigned char *data, int len)
{
	int v;
	int i;

	v = (page << REG_PPM_REG16_SWITCH_PAGE_NUMBER_SHIFT) |
	    REG_PPM_REG16_MDIO_ENABLE;
	mdio_write(mdio, phy_id, REG_PSEUDO_PHY_MII_REG16, v);

	switch (len) {
	case 1:
		v = data[0];
		mdio_write(mdio, phy_id, REG_PSEUDO_PHY_MII_REG24, v);
		break;
	case 2:
		v = ((unsigned short *)data)[0];
		mdio_write(mdio, phy_id, REG_PSEUDO_PHY_MII_REG24, v);
		break;
	case 4:
		v = ((unsigned short *)data)[0];
		mdio_write(mdio, phy_id, REG_PSEUDO_PHY_MII_REG24, v);
		v = ((unsigned short *)data)[1];
		mdio_write(mdio, phy_id, REG_PSEUDO_PHY_MII_REG25, v);
		break;
	case 6:
		v = ((unsigned short *)data)[0];
		mdio_write(mdio, phy_id, REG_PSEUDO_PHY_MII_REG24, v);
		v = ((unsigned short *)data)[1];
		mdio_write(mdio, phy_id, REG_PSEUDO_PHY_MII_REG25, v);
		v = ((unsigned short *)data)[2];
		mdio_write(mdio, phy_id, REG_PSEUDO_PHY_MII_REG26, v);
		break;
	case 8:
		v = ((unsigned short *)data)[0];
		mdio_write(mdio, phy_id, REG_PSEUDO_PHY_MII_REG24, v);
		v = ((unsigned short *)data)[1];
		mdio_write(mdio, phy_id, REG_PSEUDO_PHY_MII_REG25, v);
		v = ((unsigned short *)data)[2];
		mdio_write(mdio, phy_id, REG_PSEUDO_PHY_MII_REG26, v);
		v = ((unsigned short *)data)[3];
		mdio_write(mdio, phy_id, REG_PSEUDO_PHY_MII_REG27, v);
		break;
	}

	v = (reg << REG_PPM_REG17_REG_NUMBER_SHIFT) | REG_PPM_REG17_OP_WRITE;
	mdio_write(mdio, phy_id, REG_PSEUDO_PHY_MII_REG17, v);

	for (i = 0; i < 5; i++) {
		v = mdio_read(mdio, phy_id, REG_PSEUDO_PHY_MII_REG17);

		if ((v & (REG_PPM_REG17_OP_WRITE | REG_PPM_REG17_OP_READ)) ==
		    REG_PPM_REG17_OP_DONE)
			break;

		bolt_usleep(10);
	}
	if (i == 5)
		xprintf("ethsw_mdio_wreg: timeout!\n");

	v = page << REG_PPM_REG16_SWITCH_PAGE_NUMBER_SHIFT;
	mdio_write(mdio, phy_id, REG_PSEUDO_PHY_MII_REG16, v);
}

static void ethsw_rreg(mdio_info_t *mdio, unsigned int phy_id,
		       int page, int reg, unsigned char *data, int len)
{
	if (((len != 1) && (len % 2) != 0) || len > 8)
		xprintf("ethsw_rreg: wrong length!\n");

	ethsw_mdio_rreg(mdio, phy_id, page, reg, data, len);
}

static void ethsw_wreg(mdio_info_t *mdio, unsigned int phy_id,
		       int page, int reg, unsigned char *data, int len)
{
	if (((len != 1) && (len % 2) != 0) || len > 8)
		xprintf("ethsw_wreg: wrong length!\n");

	ethsw_mdio_wreg(mdio, phy_id, page, reg, data, len);
}

int ethsw_config_learning(mdio_info_t *mdio)
{
	unsigned char v8;
	unsigned short v16;
	int phy_id = PSEUDO_PHY_ADDR;

	/* Forward lookup failure to CPU */
	v8 = (REG_PORT_FORWARD_MCST | REG_PORT_FORWARD_UCST |
	      REG_PORT_FORWARD_IPMC);
	ethsw_wreg(mdio, phy_id, PAGE_CONTROL, REG_PORT_FORWARD,
		   (unsigned char *)&v8, sizeof(v8));

	/* Forward unlearned unicast frames to the MIPS */
	v16 = SWITCH_PORT_MAP_IMP;
	ethsw_wreg(mdio, phy_id, PAGE_CONTROL, REG_UCST_LOOKUP_FAIL,
		   (unsigned char *)&v16, sizeof(v16));

	/* Forward unlearned multicast frames to all LAN ports */
	v16 = SWITCH_PORT_MAP_1 |
	      SWITCH_PORT_MAP_2 |
	      SWITCH_PORT_MAP_3 |
	      SWITCH_PORT_MAP_4 | SWITCH_PORT_MAP_5 | SWITCH_PORT_MAP_IMP;
	ethsw_wreg(mdio, phy_id, PAGE_CONTROL, REG_MCST_LOOKUP_FAIL,
		   (unsigned char *)&v16, sizeof(v16));

	/* Forward unlearned IP multicast frames to all LAN ports */
	v16 = SWITCH_PORT_MAP_1 |
	      SWITCH_PORT_MAP_2 |
	      SWITCH_PORT_MAP_3 |
	      SWITCH_PORT_MAP_4 | SWITCH_PORT_MAP_5 | SWITCH_PORT_MAP_IMP;
	ethsw_wreg(mdio, phy_id, PAGE_CONTROL, REG_IPMC_LOOKUP_FAIL,
		   (unsigned char *)&v16, sizeof(v16));

	/* Disable learning on port[8], which is the CPU. */
	v16 = SWITCH_PORT_MAP_IMP;
	ethsw_wreg(mdio, phy_id, PAGE_CONTROL, REG_DISABLE_LEARNING,
		   (unsigned char *)&v16, sizeof(v16));

	return 0;
}

void ethsw_setup_flow_control(mdio_info_t *mdio)
{
	int phy_id = PSEUDO_PHY_ADDR;
	/*
	 * Enable flow control so switch can backpressure MIPS -> switch
	 * won't run out of internal buffers -> good for downstream throughput
	 */
	unsigned int val;
	val = REG_PAUSE_CAPBILITY_OVERRIDE |
	      REG_PAUSE_CAPBILITY_MIPS_RX |
	      REG_PAUSE_CAPBILITY_EPHY5_RX |
	      REG_PAUSE_CAPBILITY_EPHY4_RX |
	      REG_PAUSE_CAPBILITY_EPHY3_RX |
	      REG_PAUSE_CAPBILITY_EPHY4_RX |
	      REG_PAUSE_CAPBILITY_EPHY1_RX |
	      REG_PAUSE_CAPBILITY_EPHY0_RX |
	      REG_PAUSE_CAPBILITY_MIPS_TX |
	      REG_PAUSE_CAPBILITY_EPHY5_TX |
	      REG_PAUSE_CAPBILITY_EPHY4_TX |
	      REG_PAUSE_CAPBILITY_EPHY3_TX |
	      REG_PAUSE_CAPBILITY_EPHY2_TX |
	      REG_PAUSE_CAPBILITY_EPHY1_TX | REG_PAUSE_CAPBILITY_EPHY0_TX;

	ethsw_wreg(mdio, phy_id, PAGE_CONTROL, REG_PAUSE_CAPBILITY,
		   (unsigned char *)&val, sizeof(val));
}

void ethsw_switch_unmanage_mode(mdio_info_t *mdio)
{
	int phy_id = PSEUDO_PHY_ADDR;
	unsigned char v8;
	unsigned short v16;

	ethsw_rreg(mdio, phy_id, PAGE_CONTROL, REG_SWITCH_MODE, &v8,
		   sizeof(v8));
	v8 &= ~REG_SWITCH_MODE_FRAME_MANAGE_MODE;
	v8 |= REG_SWITCH_MODE_SW_FWDG_EN;
	ethsw_wreg(mdio, phy_id, PAGE_CONTROL, REG_SWITCH_MODE, &v8,
		   sizeof(v8));

	v8 = 0;
	ethsw_wreg(mdio, phy_id, PAGE_MANAGEMENT, REG_GLOBAL_CONFIG, &v8,
		   sizeof(v8));

	if (ethsw_is_bcm53101(mdio->phy_id)) {
		v8 = REG_MII_DUMB_FWD_EN;
		ethsw_wreg(mdio, phy_id, PAGE_CONTROL, REG_SWITCH_CTRL, &v8,
			   sizeof(v8));
	}

	/* Delete port-based VLAN */
	v16 = 0x01ff;
	ethsw_wreg(mdio, phy_id, PAGE_PORT_BASED_VLAN, REG_VLAN_CTRL_P0,
		   (unsigned char *)&v16, sizeof(v16));
	ethsw_wreg(mdio, phy_id, PAGE_PORT_BASED_VLAN, REG_VLAN_CTRL_P1,
		   (unsigned char *)&v16, sizeof(v16));
	ethsw_wreg(mdio, phy_id, PAGE_PORT_BASED_VLAN, REG_VLAN_CTRL_P2,
		   (unsigned char *)&v16, sizeof(v16));
	ethsw_wreg(mdio, phy_id, PAGE_PORT_BASED_VLAN, REG_VLAN_CTRL_P3,
		   (unsigned char *)&v16, sizeof(v16));
	ethsw_wreg(mdio, phy_id, PAGE_PORT_BASED_VLAN, REG_VLAN_CTRL_P4,
		   (unsigned char *)&v16, sizeof(v16));
	ethsw_wreg(mdio, phy_id, PAGE_PORT_BASED_VLAN, REG_VLAN_CTRL_P5,
		   (unsigned char *)&v16, sizeof(v16));
	ethsw_wreg(mdio, phy_id, PAGE_PORT_BASED_VLAN, REG_VLAN_CTRL_P8,
		   (unsigned char *)&v16, sizeof(v16));
}

int ethsw_config_wanport(mdio_info_t *mdio, int enable)
{
	int phy_id = PSEUDO_PHY_ADDR;
	unsigned char v8;

	if (enable)
		v8 = 0x00; /* WAN port - No spanning tree and tx/rx enable */
	else
		v8 = 0x03; /* WAN port - No spanning tree and tx/rx disable */
	ethsw_wreg(mdio, phy_id, PAGE_CONTROL, REG_PORT_CTRL + 0, &v8, 1);

	return 0;
}

int ethsw_reset_ports(mdio_info_t *mdio, int num_port, char *phyintf,
		      phy_speed_t phy_speed)
{
	int phy_id = PSEUDO_PHY_ADDR;
	int i;
	unsigned char v8;

	v8 = 0; /* No spanning tree and tx/rx enable */
	for (i = 0; i < num_port; i++) {
		ethsw_wreg(mdio, phy_id, PAGE_CONTROL, REG_PORT_CTRL + i, &v8,
			   1);
	}
	/*
	 * Config IMP port RGMII clock delay by DLL disabled and
	 * tx_clk aligned timing (restoring to reset defaults).
	 */
	ethsw_rreg(mdio, phy_id, PAGE_CONTROL, REG_RGMII_CTRL_IMP, &v8,
		   sizeof(v8));
	v8 &= ~(REG_RGMII_CTRL_DLL_RXC | REG_RGMII_CTRL_DLL_TXC);
	v8 &= ~REG_RGMII_CTRL_TIMING_SEL;

	/* "RGMII" in BOLT means RGMII with TX internal delay, make sure that we
	 * enable the IMP Port TX clock internal delay to account for this
	 * internal delay that is inserted, otherwise the switch won't be able
	 * to receive correctly.
	 *
	 * "RGMII_NO_ID" means that we are not introducing any delay neither on
	 * transmission nor reception, so the BCM53125 must also be configured
	 * accordingly to account for the lack of delay and introduce
	 *
	 * The BCM53125 switch has its RX clock and TX clock control swapped, hence
	 * the reason why we modify the TX clock path in the "RGMII" case
	 */
	if (strcmp(phyintf, "RGMII") == 0)
		v8 |= REG_RGMII_CTRL_DLL_TXC;
	if (strcmp(phyintf, "RGMII_NO_ID") == 0)
		v8 |= (REG_RGMII_CTRL_DLL_TXC | REG_RGMII_CTRL_DLL_RXC);
	v8 |= REG_RGMII_CTRL_TIMING_SEL;
	ethsw_wreg(mdio, phy_id, PAGE_CONTROL, REG_RGMII_CTRL_IMP, &v8,
		   sizeof(v8));

	/*
	 * Config port 5 RGMII clock delay by DLL disabled and
	 * tx_clk aligned timing (restoring to reset defaults)
	 */
	ethsw_rreg(mdio, phy_id, PAGE_CONTROL, REG_RGMII_CTRL_P5, &v8,
		   sizeof(v8));
	v8 &= ~(REG_RGMII_CTRL_DLL_RXC | REG_RGMII_CTRL_DLL_TXC);
	v8 &= ~REG_RGMII_CTRL_TIMING_SEL;

	/* Configure Port 5 (usually Cable Modem) to account for the BCM338x
	 * TX clock delay introduced on transmit, since the RX clock and TX
	 * clock control are swapped, we need to modify the TX clock delay
	 * programming.
	 */
	v8 |= (REG_RGMII_CTRL_DLL_TXC | REG_RGMII_CTRL_TIMING_SEL);
	ethsw_wreg(mdio, phy_id, PAGE_CONTROL, REG_RGMII_CTRL_P5, &v8,
		   sizeof(v8));

	ethsw_rreg(mdio, phy_id, PAGE_CONTROL, REG_PORT_STATE + 5, &v8,
		   sizeof(v8));
	v8 &=
		~(REG_PORT_STATE_OVERRIDE | REG_PORT_STATE_1000 |
		  REG_PORT_STATE_FDX | REG_PORT_STATE_LNK);
	v8 |=
		(REG_PORT_STATE_OVERRIDE | REG_PORT_STATE_1000 |
		 REG_PORT_STATE_FDX | REG_PORT_STATE_LNK);
	ethsw_wreg(mdio, phy_id, PAGE_CONTROL, REG_PORT_STATE + 5, &v8,
		   sizeof(v8));

	/* Set IMP port RMII mode */
	v8 = 0;
	v8 |= REG_IMP_PORT_CONTROL_RX_UCST_EN;
	v8 |= REG_IMP_PORT_CONTROL_RX_MCST_EN;
	v8 |= REG_IMP_PORT_CONTROL_RX_BCST_EN;
	ethsw_wreg(mdio, phy_id, PAGE_CONTROL, REG_IMP_PORT_CONTROL, &v8, 1);

	v8 = 0;
	v8 |= (REG_CONTROL_MPSO_MII_SW_OVERRIDE | REG_CONTROL_MPSO_LINKPASS);
	v8 |= (REG_CONTROL_MPSO_TX_FLOW_CONTROL |
	       REG_CONTROL_MPSO_RX_FLOW_CONTROL | REG_CONTROL_MPSO_FDX);

	switch (phy_speed) {
	case SPEED_2000:
	case SPEED_1000:
		v8 |= REG_CONTROL_MPSO_SPEED1000;
		break;
	case SPEED_100:
		v8 |= REG_CONTROL_MPSO_SPEED100;
		break;
	case SPEED_10:
		v8 &= ~(REG_CONTROL_MPSO_SPEED1000 | REG_CONTROL_MPSO_SPEED100);
		break;
	}
	ethsw_wreg(mdio, phy_id, PAGE_CONTROL,
		   REG_CONTROL_IMP_PORT_STATE_OVERRIDE, &v8, sizeof(v8));

	/* checking Switch functional */
	v8 = 0;
	ethsw_rreg(mdio, phy_id, PAGE_CONTROL,
		   REG_CONTROL_IMP_PORT_STATE_OVERRIDE, &v8, sizeof(v8));
	if ((v8 &
	     (REG_CONTROL_MPSO_MII_SW_OVERRIDE | REG_CONTROL_MPSO_LINKPASS)) !=
	    (REG_CONTROL_MPSO_MII_SW_OVERRIDE | REG_CONTROL_MPSO_LINKPASS)
	    || (v8 == 0xff)) {
		xprintf("error on Ethernet Switch setup\n");
		return -1;
	}

	return 0;
}

unsigned int ethsw_is_present(mdio_info_t *mdio)
{
	uint32_t phy_id;

	/* Switch will respond for MII_PHYSID[12] at address 0 */
	phy_id = mdio_get_phy_id(mdio, 0);

	mdio->phy_id = phy_id;

	return ethsw_is_bcm53125(phy_id) || ethsw_is_bcm53101(phy_id);
}
