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

/*  *********************************************************************
    *  USB-Ethernet adapter driver includes
    ********************************************************************* */

#ifndef __usbeth_h__
#define __usbeth_h__

/* **************************************
   *  CATC Netmate adapter
   ************************************** */

#define CATC_MCAST_TBL_ADDR         0xFA80	//in Netmate's SRAM
#define CATC_GET_MAC_ADDR			0xF2
#define CATC_SET_REG		        0xFA
#define CATC_GET_REG		        0xFB
#define CATC_SET_MEM		        0xFC

#define CATC_TX_BUF_CNT_REG         0x20
#define CATC_RX_BUF_CNT_REG         0x21
#define CATC_ADV_OP_MODES_REG       0x22
#define CATC_RX_FRAME_CNT_REG       0x24

#define CATC_ETH_CTRL_REG           0x60
#define CATC_ENET_STATUS_REG        0x61
#define CATC_ETH_ADDR_0_REG         0x67	// Byte #0 (leftmost)
#define CATC_LED_CTRL_REG           0x81

/* **************************************
   *  Admtek (PEGASUS II) adapter
   ************************************** */

#define PEG_SET_REG		           0xF1
#define PEG_GET_REG		           0xF0

#define PEG_MCAST_TBL_REG          0x08
#define PEG_MAC_ADDR_0_REG         0x10
#define PEG_EEPROM_OFS_REG         0x20
#define PEG_EEPROM_DATA_REG        0x21
#define PEG_EEPROM_CTL_REG         0x23
#define PEG_PHY_ADDR_REG	       0x25
#define PEG_PHY_DATA_REG	       0x26	//& 27 for 2 bytes
#define PEG_PHY_CTRL_REG	       0x28
#define PEG_ETH_CTL0_REG           0x00
#define PEG_ETH_CTL1_REG           0x01
#define PEG_ETH_CTL2_REG           0x02
#define PEG_GPIO0_REG              0x7e
#define PEG_GPIO1_REG              0x7f
#define PEG_INT_PHY_REG			   0x7b

#define PHY_WRITE				   0x20
#define PHY_READ				   0x40

/* **************************************
   *  Realtek adapter
   ************************************** */

#define RTEK_REG_ACCESS		0x05
#define RTEK_MAC_REG		0x0120
#define RTEK_CMD_REG		0x012E
#define RTEK_RXCFG_REG		0x0130
#define RTEK_RESET			0x10
#define RTEK_AUTOLOAD		0x01

/* **************************************
   *  ASIX AX88172/AX88178 adapters
   ************************************** */

#define ASIX_MCAST_FILTER_LEN	8

#define ASIX_MII_SWOP_CMD	0x06
#define ASIX_MII_READ_CMD	0x07
#define ASIX_MII_WRITE_CMD	0x08
#define ASIX_MII_HWOP_CMD	0x0a
#define ASIX_RXCTL_CMD		0x10
#define ASIX_IPG1_CMD		0x12
#define ASIX_IPG2_CMD		0x13
#define ASIX_IPG3_CMD		0x14
#define ASIX_SET_MCAST_FILTER	0x16
#define ASIX_MAC_ADDR_CMD	0x17
#define ASIX_PHYID_CMD		0x19
#define ASIX_MED_WRITE_CMD	0x1b
#define ASIX_GPIO_WRITE_CMD	0x1f

#define ASIX2_MAC_ADDR_CMD	0x13

#endif //__usbeth_h_
