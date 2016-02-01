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

#ifndef _BCMSW_H_
#define _BCMSW_H_

#include "net_mdio.h"

/* MII_PHYSID1 & MII_PHYSID2 */
#define BCM53125_PHY_OUI				0x03625f20
#define BCM53125_PHY_MASK				0x1ffffff0

static inline unsigned int ethsw_is_bcm53125(uint32_t phy_id)
{
	return ((phy_id & BCM53125_PHY_MASK) ==
		(BCM53125_PHY_OUI & BCM53125_PHY_MASK));
}

#define BCM53101_PHY_OUI				0x03625ed0
#define BCM53101_PHY_MASK				0x1ffffff0

static inline unsigned int ethsw_is_bcm53101(uint32_t phy_id)
{
	return ((phy_id & BCM53101_PHY_MASK) ==
		(BCM53101_PHY_OUI & BCM53101_PHY_MASK));
}

#define PSEUDO_PHY_ADDR					0x1e

/* ARL Entry port id for unicast */
#define SWITCH_PORTID_0					0x0000
#define SWITCH_PORTID_1					0x0001
#define SWITCH_PORTID_2					0x0002
#define SWITCH_PORTID_3					0x0003
#define SWITCH_PORTID_4					0x0004
#define SWITCH_PORTID_5					0x0005
#define SWITCH_PORTID_IMP				0x0008

/* ARL Entry multicast map */
#define SWITCH_PORT_MAP_0				0x0001
#define SWITCH_PORT_MAP_1				0x0002
#define SWITCH_PORT_MAP_2				0x0004
#define SWITCH_PORT_MAP_3				0x0008
#define SWITCH_PORT_MAP_4				0x0010
#define SWITCH_PORT_MAP_5				0x0020
#define SWITCH_PORT_MAP_IMP				0x0100

/****************************************************************************
    External switch pseudo PHY: Page (0x00)
****************************************************************************/

#define PAGE_CONTROL					0x00

#define REG_PORT_CTRL					0x00

#define REG_PORT_NO_SPANNING_TREE			0x00
#define REG_PORT_STP_STATE_DISABLED			0x20
#define REG_PORT_STP_STATE_BLOCKING			0x40
#define REG_PORT_STP_STATE_LISTENING			0x60
#define REG_PORT_STP_STATE_LEARNING			0x80
#define REG_PORT_STP_STATE_FORWARDING			0xA0
#define REG_PORT_TX_DISABLE				0x02
#define REG_PORT_RX_DISABLE				0x01
#define REG_PORT_CTRL_DISABLE				0x03

#if defined(CONFIG_BCM96362)
#define EPHY_PORTS      6
#endif
#if defined(CONFIG_BCM96368)
#define EPHY_PORTS      6
#define USB_PORT_ID     6
#define SAR_PORT_ID     7
#endif
#if defined(CONFIG_BCM96816)
#define EPHY_PORTS      4
#define SERDES_PORT_ID  4
#define MOCA_PORT_ID    5
#define USB_PORT_ID     6
#define GPON_PORT_ID    7
#endif
#define MIPS_PORT_ID    8

#define REG_IMP_PORT_CONTROL				0x08

#define REG_IMP_PORT_CONTROL_RX_UCST_EN			0x10
#define REG_IMP_PORT_CONTROL_RX_MCST_EN			0x08
#define REG_IMP_PORT_CONTROL_RX_BCST_EN			0x04

#define REG_SWITCH_MODE					0x0b

#define REG_SWITCH_MODE_FRAME_MANAGE_MODE		0x01
#define REG_SWITCH_MODE_SW_FWDG_EN			0x02

#define REG_CONTROL_IMP_PORT_STATE_OVERRIDE		0x0e

#define REG_CONTROL_MPSO_MII_SW_OVERRIDE		0x80
#define REG_CONTROL_MPSO_TX_FLOW_CONTROL		0x20
#define REG_CONTROL_MPSO_RX_FLOW_CONTROL		0x10
#define REG_CONTROL_MPSO_SPEED1000			0x08
#define REG_CONTROL_MPSO_SPEED100			0x04
#define REG_CONTROL_MPSO_FDX				0x02
#define REG_CONTROL_MPSO_LINKPASS			0x01

#define REG_SWITCH_CTRL					0x20
#define REG_MII_DUMB_FWD_EN				0x01	/* 53101 only */

#define REG_PORT_FORWARD				0x21	/* 5397 only */

#define REG_PORT_FORWARD_MCST				0x80
#define REG_PORT_FORWARD_UCST				0x40
#define REG_PORT_FORWARD_IPMC				0x01

#define REG_PROTECTED_PORT_MAP				0x24	/* 5397 only */

#define REG_PAUSE_CAPBILITY				0x28

#define REG_PAUSE_CAPBILITY_OVERRIDE			(1<<23)
#define REG_PAUSE_CAPBILITY_MIPS_RX			(1<<17)
#define REG_PAUSE_CAPBILITY_EPHY5_RX			(1<<14)
#define REG_PAUSE_CAPBILITY_EPHY4_RX			(1<<13)
#define REG_PAUSE_CAPBILITY_EPHY3_RX			(1<<12)
#define REG_PAUSE_CAPBILITY_EPHY2_RX			(1<<11)
#define REG_PAUSE_CAPBILITY_EPHY1_RX			(1<<10)
#define REG_PAUSE_CAPBILITY_EPHY0_RX			(1<<9)
#define REG_PAUSE_CAPBILITY_MIPS_TX			(1<<8)
#define REG_PAUSE_CAPBILITY_EPHY5_TX			(1<<5)
#define REG_PAUSE_CAPBILITY_EPHY4_TX			(1<<4)
#define REG_PAUSE_CAPBILITY_EPHY3_TX			(1<<3)
#define REG_PAUSE_CAPBILITY_EPHY2_TX			(1<<2)
#define REG_PAUSE_CAPBILITY_EPHY1_TX			(1<<1)
#define REG_PAUSE_CAPBILITY_EPHY0_TX			(1<<0)

#define REG_UCST_LOOKUP_FAIL				0x32

#define REG_MCST_LOOKUP_FAIL				0x34

#define REG_IPMC_LOOKUP_FAIL				0x36

#define REG_DISABLE_LEARNING				0x3c

#define REG_PORT_STATE					0x58

#define REG_PORT_STATE_OVERRIDE				0x40
#define REG_PORT_STATE_FLOWCTL				0x30
#define REG_PORT_STATE_100				0x04
#define REG_PORT_STATE_1000				0x08
#define REG_PORT_STATE_FDX				0x02
#define REG_PORT_STATE_LNK				0x01

#define REG_RGMII_CTRL_IMP				0x60
#define REG_RGMII_CTRL_P4				0x64
#define REG_RGMII_CTRL_P5				0x65

#define REG_RGMII_CTRL_ENABLE_GMII			0x80
#define REG_RGMII_CTRL_TIMING_SEL			0x04
#define REG_RGMII_CTRL_DLL_RXC				0x02
#define REG_RGMII_CTRL_DLL_TXC				0x01

#define REG_RGMII_TIMING_P4				0x6c
#define REG_RGMII_TIMING_P5				0x6d

#define REG_RGMII_TIMING_DELAY_DEFAULT			0xF9

#define REG_SW_RESET					0x79
#define REG_SW_RST					0x80
#define REG_EN_RESET_REG				0x02
#define REG_EN_RESET_SW_CORE				0x01

#define REG_FAST_AGING_CTRL				0x88
#define REG_FAST_AGING_PORT				0x89

#define REG_IUDMA_CTRL					0xa8

#define REG_IUDMA_CTRL_USE_QUEUE_PRIO			0x0400
#define REG_IUDMA_CTRL_USE_DESC_PRIO			0x0200
#define REG_IUDMA_CTRL_TX_DMA_EN			0x0100
#define REG_IUDMA_CTRL_RX_DMA_EN			0x0080
#define REG_IUDMA_CTRL_PRIO_SEL				0x0040

#define REG_IUDMA_QUEUE_CTRL				0xac
#define REG_IUDMA_Q_CTRL_RXQ_SEL_S			16
#define REG_IUDMA_Q_CTRL_RXQ_SEL_M			0xFFFF
#define REG_IUDMA_Q_CTRL_PRIO_TO_CH_M			0x3
#define REG_IUDMA_Q_CTRL_TXQ_SEL_S			0
#define REG_IUDMA_Q_CTRL_TXQ_SEL_M			0xFFF
#define REG_IUDMA_Q_CTRL_CH_TO_PRIO_M			0x7

#define REG_MDIO_CTRL_ADDR				0xb0
#define REG_MDIO_CTRL_WRITE				(1 << 31)
#define REG_MDIO_CTRL_READ				(1 << 30)
#define REG_MDIO_CTRL_EXT				(1 << 16)
#define REG_MDIO_CTRL_EXT_BIT(id) (is_ext_phy_id(id) ? REG_MDIO_CTRL_EXT : 0)
#define REG_MDIO_CTRL_ID_SHIFT				25
#define REG_MDIO_CTRL_ID_MASK		(0x1f << REG_MDIO_CTRL_ID_SHIFT)
#define REG_MDIO_CTRL_ADDR_SHIFT			20
#define REG_MDIO_CTRL_ADDR_MASK		(0x1f << REG_MDIO_CTRL_ADDR_SHIFT)

#define REG_MDIO_DATA_ADDR				0xb4

/****************************************************************************
    External switch pseudo PHY: Page (0x01)
****************************************************************************/

#define PAGE_STATUS					0x01

#define REG_LINK_STATUS					0x00

/****************************************************************************
    Global Management Configuration: Page (0x02)
****************************************************************************/

#define PAGE_MANAGEMENT					0x02

#define REG_GLOBAL_CONFIG				0x00

#define ENABLE_IMP_PORT					0x80
#define ENABLE_PORT5					0x40
#define LINK_STATUS_CHANGE_INTR_EM			0x10
#define ENABLE_RX_BPDU					0x02
#define RESET_MIB					0x01

#define REG_BRCM_HDR_CNTL				0x03

#define BRCM_HDR_EN					0x01

#define REG_AGING_TIME_CNTL				0x06

#define REG_DEV_ID					0x30

/****************************************************************************
    ARL/VLAN Table Access: Page (0x05)
****************************************************************************/

#define PAGE_AVTBL_ACCESS				0x05

#define REG_ARL_TBL_CTRL				0x00
#define ARL_TBL_CTRL_START_DONE				0x80
#define REG_ARL_MAC_INDX_LO				0x02
#define REG_ARL_MAC_INDX_HI				0x04
#define REG_ARL_VLAN_INDX				0x08
#define REG_ARL_MAC_LO_ENTRY				0x10
#define REG_ARL_VID_MAC_HI_ENTRY			0x14
#define REG_ARL_DATA_ENTRY				0x18
#define REG_ARL_SRCH_CTRL				0x30
#define ARL_SRCH_CTRL_START_DONE			0x80
#define ARL_SRCH_CTRL_SR_VALID				0x01
#define REG_ARL_SRCH_ADDR				0x32
#define REG_ARL_SRCH_VID_MAC_HI_ENTRY			0x34
#define REG_ARL_SRCH_MAC_LO_ENTRY			0x38
#define REG_ARL_SRCH_DATA_ENTRY				0x3C
#define REG_VLAN_TBL_CTRL				0x80
#define REG_VLAN_TBL_INDX				0x81
#define REG_VLAN_TBL_ENTRY				0x83

/****************************************************************************
   Flow Control: Page (0x0A)
****************************************************************************/

#define PAGE_FLOW_CTRL					0x0A

#define REG_FC_DIAG_PORT_SEL				0x00
#define REG_FC_CTRL					0x0E
#define REG_FC_PRIQ_HYST				0x10
#define REG_FC_PRIQ_PAUSE				0x20
#define REG_FC_PRIQ_DROP				0x30
#define REG_FC_PRIQ_TOTAL_HYST				0x40
#define REG_FC_PRIQ_TOTAL_PAUSE				0x50
#define REG_FC_PRIQ_TOTAL_DROP				0x60
#define REG_FC_PRIQ_LO_DROP				0x70
#define REG_FC_Q_MON_CNT				0x90

/****************************************************************************
   Flow Control: Page (0x0A)
****************************************************************************/

#define PAGE_MIB_P0					0x20

/****************************************************************************
    Registers for pseudo PHY access
****************************************************************************/

#define PSEUDO_PHY_ADDR					0x1e

#define REG_PSEUDO_PHY_MII_REG16			0x10

#define REG_PPM_REG16_SWITCH_PAGE_NUMBER_SHIFT		8
#define REG_PPM_REG16_MDIO_ENABLE			0x01

#define REG_PSEUDO_PHY_MII_REG17			0x11

#define REG_PPM_REG17_REG_NUMBER_SHIFT			8
#define REG_PPM_REG17_OP_DONE				0x00
#define REG_PPM_REG17_OP_WRITE				0x01
#define REG_PPM_REG17_OP_READ				0x02

#define REG_PSEUDO_PHY_MII_REG24			0x18
#define REG_PSEUDO_PHY_MII_REG25			0x19
#define REG_PSEUDO_PHY_MII_REG26			0x1a
#define REG_PSEUDO_PHY_MII_REG27			0x1b

/****************************************************************************
   QOS : Page (0x30)
****************************************************************************/

#define PAGE_QOS					0x30

#define REG_QOS_GLOBAL_CTRL				0x00
#define REG_QOS_8021P_EN				0x04
#define REG_QOS_8021P_PRIO_MAP				0x10
#define REG_QOS_PORT_PRIO_MAP_P0			0x50
#if defined(CONFIG_BCM96816)
#define REG_QOS_PRIO_TO_QID_SEL_BITS			3
#define REG_QOS_PRIO_TO_QID_SEL_M			0x7
#else
#define REG_QOS_PRIO_TO_QID_SEL_BITS			2
#define REG_QOS_PRIO_TO_QID_SEL_M			0x3
#endif

#define REG_QOS_TXQ_CTRL				0x80
#if defined(CONFIG_BCM96816)
#define TXQ_CTRL_TXQ_MODE_M				0x7
#define TXQ_CTRL_TXQ_MODE_S				2
#define TXQ_CTRL_HQ_PREEMPT_M				0x1
#define TXQ_CTRL_HQ_PREEMPT_S				5
#else
#define TXQ_CTRL_TXQ_MODE_M				0x3
#define TXQ_CTRL_TXQ_MODE_S				2
#define TXQ_CTRL_HQ_PREEMPT_M				0x1
#define TXQ_CTRL_HQ_PREEMPT_S				4
#endif
#define REG_QOS_TXQ_WEIGHT_Q0				0x81
#define REG_QOS_VID_REMAP				0xB0
#define REG_QOS_VID_REMAP_CTRL				0xD0

/****************************************************************************
    Port-Based VLAN Control: Page (0x31)
****************************************************************************/

#define PAGE_PORT_BASED_VLAN				0x31

#define REG_VLAN_CTRL_P0				0x00
#define REG_VLAN_CTRL_P1				0x02
#define REG_VLAN_CTRL_P2				0x04
#define REG_VLAN_CTRL_P3				0x06
#define REG_VLAN_CTRL_P4				0x08
#define REG_VLAN_CTRL_P5				0x0a
#define REG_VLAN_CTRL_P6				0x0c
#define REG_VLAN_CTRL_P7				0x0e
#define REG_VLAN_CTRL_P8				0x10

/****************************************************************************
   802.1Q VLAN Control: Page (0x34)
****************************************************************************/

#define PAGE_8021Q_VLAN					0x34

#define REG_VLAN_GLOBAL_8021Q				0x00
#define REG_VLAN_CTRL0_ENABLE_1Q			(1 << 7)
#define REG_VLAN_CTRL0_SVLM				(0 << 5)
#define REG_VLAN_CTRL0_IVLM				(3 << 5)
#define REG_VLAN_GLOBAL_CTRL1				0x01
#define REG_VLAN_GLOBAL_CTRL2				0x02
#define REG_VLAN_GLOBAL_CTRL3				0x04
#define REG_VLAN_GLOBAL_CTRL4				0x06
#define REG_VLAN_GLOBAL_CTRL5				0x07
#define REG_VLAN_CTRL5_DROP_VTAB_MISS			(1 << 3)
#define REG_VLAN_CTRL5_ENBL_MANAGE_RX_BYPASS		(1 << 0)
#define REG_VLAN_8021P_REMAP				0x0c
#define REG_VLAN_DEFAULT_TAG				0x10
#define REG_VLAN_DOUBLE_TAG_TPID			0x30
#define REG_VLAN_ISP_PORT_SEL				0x32

/****************************************************************************
   Broadcast Storm Suprression Registers
****************************************************************************/

#define PAGE_BSS					0x41

#define REG_BSS_IRC_CONFIG				0x00
#define IRC_CFG_XLENEN					16
#define IRC_CFG_PKT_MSK0_EXT_S				17

#define REG_BSS_RX_RATE_CTRL_P0				0x10
#define IRC_BKT0_RATE_CNT_M				0xFF
#define IRC_BKT0_RATE_CNT_S				0
#define IRC_BKT0_SIZE_M					0x7
#define IRC_BKT0_SIZE_S					8
#define IRC_BKT0_EN_M					0xFF
#define IRC_BKT0_EN_S					11

#define REG_BSS_PKT_DROP_CNT_P0				0x50

#define REG_BSS_TX_RATE_CTRL_P0				0x80
#define ERC_RFSH_CNT_M					0xFF
#define ERC_RFSH_CNT_S					0
#define ERC_BKT_SIZE_M					0x7
#define ERC_BKT_SIZE_S					8
#define ERC_ERC_EN_M					0xFF
#define ERC_ERC_EN_S					11

/****************************************************************************
    Prototypes
****************************************************************************/
extern int ethsw_reset_ports(mdio_info_t *info, int num_port, char *phyintf,
			     phy_speed_t physpeed);
extern void ethsw_switch_unmanage_mode(mdio_info_t *info);
extern int ethsw_config_learning(mdio_info_t *info);
extern void ethsw_setup_flow_control(mdio_info_t *info);
extern int ethsw_config_wanport(mdio_info_t *info, int enable);

extern unsigned int ethsw_is_present(mdio_info_t *mdio);

#endif /* _BCMSW_H_ */
