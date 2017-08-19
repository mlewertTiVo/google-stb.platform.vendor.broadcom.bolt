/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef _BCMSF2_H_
#define _BCMSF2_H_

#include <bchp_switch_core.h>
#include <net_mdio.h>

/* Translation between the old and new RDB defines */
#ifndef BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0

#define BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0 \
	BCHP_SWITCH_CORE_STS_OVERRIDE_GMII_P0
#define BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_1 \
	BCHP_SWITCH_CORE_STS_OVERRIDE_GMII_P1
#define BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_2 \
	BCHP_SWITCH_CORE_STS_OVERRIDE_GMII_P2
#define BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0_SW_OVERRIDE_MASK \
	BCHP_SWITCH_CORE_STS_OVERRIDE_GMII_P0_SW_OVERRIDE_MASK
#define BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0_LINK_STS_MASK \
	BCHP_SWITCH_CORE_STS_OVERRIDE_GMII_P0_LINK_STS_MASK
#define BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0_DUPLX_MODE_MASK \
	BCHP_SWITCH_CORE_STS_OVERRIDE_GMII_P0_DUPLX_MODE_MASK
#define BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0_SPEED_MASK \
	BCHP_SWITCH_CORE_STS_OVERRIDE_GMII_P0_SPEED_MASK
#define BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0_RXFLOW_CNTL_MASK \
	BCHP_SWITCH_CORE_STS_OVERRIDE_GMII_P0_RXFLOW_CNTL_MASK
#define BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0_TXFLOW_CNTL_MASK \
	BCHP_SWITCH_CORE_STS_OVERRIDE_GMII_P0_TXFLOW_CNTL_MASK
#define BCHP_SWITCH_CORE_STS_OVERRIDE_GMIIP_Port_0_SPEED_SHIFT \
	BCHP_SWITCH_CORE_STS_OVERRIDE_GMII_P0_SPEED_SHIFT
#define BCHP_SWITCH_CORE_PORT_VLAN_CTL_Port_0 \
	BCHP_SWITCH_CORE_VLAN_CTL_P0
#define BCHP_SWITCH_CORE_PORT_VLAN_CTL_Port_1 \
	BCHP_SWITCH_CORE_VLAN_CTL_P1
#define BCHP_SWITCH_CORE_PORT_VLAN_CTL_Port_2 \
	BCHP_SWITCH_CORE_VLAN_CTL_P2
#define BCHP_SWITCH_CORE_PORT_VLAN_CTL_P7 \
	BCHP_SWITCH_CORE_VLAN_CTL_P7
#define BCHP_SWITCH_CORE_PORT_VLAN_CTL_Port_0_PORT_EGRESS_EN_MASK \
	BCHP_SWITCH_CORE_VLAN_CTL_P0_PORT_EGRESS_EN_MASK
#define BCHP_SWITCH_CORE_G_PCTL_Port_0 \
	BCHP_SWITCH_CORE_G_PCTL_P0
#define BCHP_SWITCH_CORE_G_PCTL_Port_1 \
	BCHP_SWITCH_CORE_G_PCTL_P1
#define BCHP_SWITCH_CORE_G_PCTL_Port_2 \
	BCHP_SWITCH_CORE_G_PCTL_P2
#define BCHP_SWITCH_CORE_IMP_CTL \
	BCHP_SWITCH_CORE_CTL_IMP
#define BCHP_SWITCH_CORE_IMP_CTL_RX_UCST_EN_MASK \
	BCHP_SWITCH_CORE_CTL_IMP_RX_UCST_EN_MASK
#define BCHP_SWITCH_CORE_IMP_CTL_RX_BCST_EN_MASK \
	BCHP_SWITCH_CORE_CTL_IMP_RX_BCST_EN_MASK
#define BCHP_SWITCH_CORE_IMP_CTL_RX_MCST_EN_MASK \
	BCHP_SWITCH_CORE_CTL_IMP_RX_MCST_EN_MASK
#endif

#ifndef BCHP_SWITCH_REG_LED_0_CNTRL_sel_no_link_encode_SHIFT
#define BCHP_SWITCH_REG_LED_0_CNTRL_sel_no_link_encode_SHIFT \
	BCHP_SWITCH_REG_LED_0_LINK_AND_SPEED_ENCODING_SEL_sel_no_link_encode_SHIFT
#define BCHP_SWITCH_REG_LED_0_CNTRL_sel_10m_encode_SHIFT \
	BCHP_SWITCH_REG_LED_0_LINK_AND_SPEED_ENCODING_SEL_sel_10m_encode_SHIFT
#define BCHP_SWITCH_REG_LED_0_CNTRL_sel_100m_encode_SHIFT \
	BCHP_SWITCH_REG_LED_0_LINK_AND_SPEED_ENCODING_SEL_sel_100m_encode_SHIFT
#define BCHP_SWITCH_REG_LED_0_CNTRL_sel_1000m_encode_SHIFT \
	BCHP_SWITCH_REG_LED_0_LINK_AND_SPEED_ENCODING_SEL_sel_1000m_encode_SHIFT
#define BCHP_SWITCH_REG_LED_0_CNTRL_no_link_encode_SHIFT \
	BCHP_SWITCH_REG_LED_0_LINK_AND_SPEED_ENCODING_no_link_encode_SHIFT
#endif


void bcm_sf2_init(void);
mdio_info_t *bcm_sf2_mdio_init(void);
void bcm_sf2_exit(void);
void bcm_sf2_multicast_enable(unsigned int *filter_cnt,
		bool enable);
#endif /* _BCMSF2_H */
