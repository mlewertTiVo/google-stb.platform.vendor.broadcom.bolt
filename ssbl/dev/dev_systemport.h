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

#ifndef __DEV_SYSTEMPORT_H
#define __DEV_SYSTEMPORT_H

#include "net_ebuf.h"

/* Receive/transmit descriptor format */
#define DESC_ADDR_HI_STATUS_LEN	0x00
#define  DESC_ADDR_HI_SHIFT	0
#define  DESC_ADDR_HI_MASK	0xff
#define  DESC_STATUS_SHIFT	8
#define  DESC_STATUS_MASK	0x3ff
#define  DESC_LEN_SHIFT		18
#define  DESC_LEN_MASK		0x3fff
#define DESC_ADDR_LO		0x04

/* HW supports 40-bit addressing hence the */
#define DESC_SIZE		(WORDS_PER_DESC * sizeof(uint32_t))

/* Default RX buffer allocation size */
#define RX_BUF_LENGTH		2048

/* Body(1500) + EH_SIZE(14) + VLANTAG(4) + BRCMTAG(6) + FCS(4) = 1528.
 * 1536 is multiple of 256 bytes
 */
#define ENET_BRCM_TAG_LEN	6
#define ENET_PAD		8
#define UMAC_MAX_MTU_SIZE	(ENET_MAX_PKT + ENET_VLAN_HLEN + \
				 ENET_BRCM_TAG_LEN + ENET_CRC_SIZE + ENET_PAD)

struct rsb {
	uint32_t rx_status_len;
	uint32_t brcm_egress_tag;
};

/* Common Receive/Transmit status bits */
#define DESC_L4_CSUM		(1 << 7)
#define DESC_SOP		(1 << 8)
#define DESC_EOP		(1 << 9)

/* Receive Status bits */
#define RX_STATUS_UCAST			0
#define RX_STATUS_BCAST			0x04
#define RX_STATUS_MCAST			0x08
#define RX_STATUS_L2_MCAST		0x0c
#define RX_STATUS_ERR			(1 << 4)
#define RX_STATUS_OVFLOW		(1 << 5)
#define RX_STATUS_PARSE_FAIL		(1 << 6)

/* Transmit Status bits */
#define TX_STATUS_VLAN_NO_ACT		0x00
#define TX_STATUS_VLAN_PCP_TSB		0x01
#define TX_STATUS_VLAN_QUEUE		0x02
#define TX_STATUS_VLAN_VID_TSB		0x03
#define TX_STATUS_OWR_CRC		(1 << 2)
#define TX_STATUS_APP_CRC		(1 << 3)
#define TX_STATUS_BRCM_TAG_NO_ACT	0
#define TX_STATUS_BRCM_TAG_ZERO		0x10
#define TX_STATUS_BRCM_TAG_ONE_QUEUE	0x20
#define TX_STATUS_BRCM_TAG_ONE_TSB	0x30
#define TX_STATUS_SKIP_BYTES		(1 << 6)

/* Specific register definitions */
#define SYS_PORT_TOPCTRL_OFFSET		0
#define REV_CNTL			0x00
#define  REV_MASK			0xffff

#define RX_FLUSH_CNTL			0x04
#define  RX_FLUSH			(1 << 0)

#define TX_FLUSH_CNTL			0x08
#define  TX_FLUSH			(1 << 0)

/* Receive buffer offset and defines */
#define SYS_PORT_RBUF_OFFSET		0x400

#define RBUF_CONTROL			0x00
#define  RBUF_RSB_EN			(1 << 0)
#define  RBUF_4B_ALGN			(1 << 1)
#define  RBUF_BRCM_TAG_STRIP		(1 << 2)
#define  RBUF_BAD_PKT_DISC		(1 << 3)
#define  RBUF_RESUME_THRESH_SHIFT	4
#define  RBUF_RESUME_THRESH_MASK	0xff
#define  RBUF_OK_TO_SEND_SHIFT		12
#define  RBUF_OK_TO_SEND_MASK		0xff
#define  RBUF_CRC_REPLACE		(1 << 20)
#define  RBUF_OK_TO_SEND_MODE		(1 << 21)
#define  RBUF_RSB_SWAP			(1 << 22)
#define  RBUF_ACPI_EN			(1 << 23)

/* UniMAC offset and defines */
#define SYS_PORT_UMAC_OFFSET		0x800

#define UMAC_CMD			0x008
#define  CMD_TX_EN			(1 << 0)
#define  CMD_RX_EN			(1 << 1)
#define  CMD_SPEED_SHIFT		2
#define  CMD_SPEED_10			0
#define  CMD_SPEED_100			1
#define  CMD_SPEED_1000			2
#define  CMD_SPEED_2500			3
#define  CMD_SPEED_MASK			3
#define  CMD_PROMISC			(1 << 4)
#define  CMD_PAD_EN			(1 << 5)
#define  CMD_CRC_FWD			(1 << 6)
#define  CMD_PAUSE_FWD			(1 << 7)
#define  CMD_RX_PAUSE_IGNORE		(1 << 8)
#define  CMD_TX_ADDR_INS		(1 << 9)
#define  CMD_HD_EN			(1 << 10)
#define  CMD_SW_RESET			(1 << 13)
#define  CMD_LCL_LOOP_EN		(1 << 15)
#define  CMD_AUTO_CONFIG		(1 << 22)
#define  CMD_CNTL_FRM_EN		(1 << 23)
#define  CMD_NO_LEN_CHK			(1 << 24)
#define  CMD_RMT_LOOP_EN		(1 << 25)
#define  CMD_PRBL_EN			(1 << 27)
#define  CMD_TX_PAUSE_IGNORE		(1 << 28)
#define  CMD_TX_RX_EN			(1 << 29)
#define  CMD_RUNT_FILTER_DIS		(1 << 30)

#define UMAC_MAC0			0x00c
#define UMAC_MAC1			0x010
#define UMAC_MAX_FRAME_LEN		0x014

#define UMAC_TX_FLUSH			0x334

#define UMAC_MIB_START			0x400

/* There is a 0xC gap between the end of RX and beginning of TX stats and then
 * between the end of TX stats and the beginning of the RX RUNT
 */
#define UMAC_MIB_STAT_OFFSET		0xc

#define UMAC_MIB_CTRL			0x580
#define  MIB_RX_CNT_RST			(1 << 0)
#define  MIB_RUNT_CNT_RST		(1 << 1)
#define  MIB_TX_CNT_RST			(1 << 2)
#define UMAC_MDF_CTRL			0x650
#define UMAC_MDF_ADDR			0x654

/* Present on SYSTEMPORT Lite only */
#define SYS_PORT_GIB_OFFSET		0x1000

#define GIB_CONTROL			0x00
#define GIB_MAC1			0x08
#define GIB_MAC0			0x0c

/* Receive DMA offset and defines */
#define SYS_PORT_RDMA_OFFSET		0x2000

#define RDMA_CONTROL			0x1000
#define  RDMA_EN			(1 << 0)
#define  RDMA_RING_CFG			(1 << 1)
#define  RDMA_DISC_EN			(1 << 2)
#define  RDMA_BUF_DATA_OFFSET_SHIFT	4
#define  RDMA_BUF_DATA_OFFSET_MASK	0x3ff

#define RDMA_STATUS			0x1004
#define  RDMA_DISABLED			(1 << 0)
#define  RDMA_DESC_RAM_INIT_BUSY	(1 << 1)
#define  RDMA_BP_STATUS			(1 << 2)

#define RDMA_SCB_BURST_SIZE		0x1008

#define RDMA_RING_BUF_SIZE		0x100c
#define  RDMA_RING_SIZE_SHIFT		16

#define RDMA_WRITE_PTR_HI		0x1010
#define RDMA_WRITE_PTR_LO		0x1014
#define RDMA_PROD_INDEX			0x1018
#define  RDMA_PROD_INDEX_MASK		0xffff

#define RDMA_CONS_INDEX			0x101c
#define  RDMA_CONS_INDEX_MASK		0xffff

#define RDMA_START_ADDR_HI		0x1020
#define RDMA_START_ADDR_LO		0x1024
#define RDMA_END_ADDR_HI		0x1028
#define RDMA_END_ADDR_LO		0x102c

#define RDMA_MBDONE_INTR		0x1030
#define  RDMA_INTR_THRESH_MASK		0xff
#define  RDMA_TIMEOUT_SHIFT		16
#define  RDMA_TIMEOUT_MASK		0xffff

#define RDMA_XON_XOFF_THRESH		0x1034
#define  RDMA_XON_XOFF_THRESH_MASK	0xffff
#define  RDMA_XOFF_THRESH_SHIFT		16

#define RDMA_READ_PTR_HI		0x1038
#define RDMA_READ_PTR_LO		0x103c

#define RDMA_OVERRIDE			0x1040
#define  RDMA_LE_MODE			(1 << 0)
#define  RDMA_REG_MODE			(1 << 1)

#define RDMA_TEST			0x1044
#define  RDMA_TP_OUT_SEL		(1 << 0)
#define  RDMA_MEM_SEL			(1 << 1)

#define RDMA_DEBUG			0x1048

/* Transmit DMA offset and defines */
#define TDMA_NUM_PORTS			32	/* ports = queues */
#define TDMA_PORT_SIZE			DESC_SIZE /* two 32-bits words */

#define SYS_PORT_TDMA_OFFSET		0x4000
#define TDMA_WRITE_PORT_OFFSET		0x0000
#define TDMA_WRITE_PORT_HI(i)		(TDMA_WRITE_PORT_OFFSET + \
					(i) * TDMA_PORT_SIZE)
#define TDMA_WRITE_PORT_LO(i)		(TDMA_WRITE_PORT_OFFSET + \
					sizeof(uint32_t) + (i) * TDMA_PORT_SIZE)

#define TDMA_READ_PORT_OFFSET		(TDMA_WRITE_PORT_OFFSET + \
					(TDMA_NUM_PORTS * TDMA_PORT_SIZE))
#define TDMA_READ_PORT_HI(i)		(TDMA_READ_PORT_OFFSET + \
					(i) * TDMA_PORT_SIZE)
#define TDMA_READ_PORT_LO(i)		(TDMA_READ_PORT_OFFSET + \
					sizeof(uint32_t) + (i) * TDMA_PORT_SIZE)

#define TDMA_READ_PORT_CMD_OFFSET	(TDMA_READ_PORT_OFFSET + \
					(TDMA_NUM_PORTS * TDMA_PORT_SIZE))
#define TDMA_READ_PORT_CMD(i)		(TDMA_READ_PORT_CMD_OFFSET + \
					(i) * sizeof(uint32_t))

#define TDMA_DESC_RING_00_BASE		(TDMA_READ_PORT_CMD_OFFSET + \
					(TDMA_NUM_PORTS * sizeof(uint32_t)))

/* Register offsets and defines relatives to a specific ring number */
#define RING_HEAD_TAIL_PTR		0x00
#define  RING_HEAD_MASK			0x7ff
#define  RING_TAIL_SHIFT		11
#define  RING_TAIL_MASK			0x7ff
#define  RING_FLUSH			(1 << 24)
#define  RING_EN			(1 << 25)

#define RING_COUNT			0x04
#define  RING_COUNT_MASK		0x7ff
#define  RING_BUFF_DONE_SHIFT		11
#define  RING_BUFF_DONE_MASK		0x7ff

#define RING_MAX_HYST			0x08
#define  RING_MAX_THRESH_MASK		0x7ff
#define  RING_HYST_THRESH_SHIFT		11
#define  RING_HYST_THRESH_MASK		0x7ff

#define RING_INTR_CONTROL		0x0c
#define  RING_INTR_THRESH_MASK		0x7ff
#define  RING_EMPTY_INTR_EN		(1 << 15)
#define  RING_TIMEOUT_SHIFT		16
#define  RING_TIMEOUT_MASK		0xffff

#define RING_PROD_CONS_INDEX		0x10
#define  RING_PROD_INDEX_MASK		0xffff
#define  RING_CONS_INDEX_SHIFT		16
#define  RING_CONS_INDEX_MASK		0xffff

#define RING_MAPPING			0x14
#define  RING_QID_MASK			0x3
#define  RING_PORT_ID_SHIFT		3
#define  RING_PORT_ID_MASK		0x7
#define  RING_IGNORE_STATUS		(1 << 6)
#define  RING_FAILOVER_EN		(1 << 7)
#define  RING_CREDIT_SHIFT		8
#define  RING_CREDIT_MASK		0xffff

#define RING_PCP_DEI_VID		0x18
#define  RING_VID_MASK			0x7ff
#define  RING_DEI			(1 << 12)
#define  RING_PCP_SHIFT			13
#define  RING_PCP_MASK			0x7
#define  RING_PKT_SIZE_ADJ_SHIFT	16
#define  RING_PKT_SIZE_ADJ_MASK		0xf

#define TDMA_DESC_RING_SIZE		28

/* Defininition for a given TX ring base address */
#define TDMA_DESC_RING_BASE(i)		(TDMA_DESC_RING_00_BASE + \
					((i) * TDMA_DESC_RING_SIZE))

/* Ring indexed register addreses */
#define TDMA_DESC_RING_HEAD_TAIL_PTR(i)	(TDMA_DESC_RING_BASE(i) + \
					RING_HEAD_TAIL_PTR)
#define TDMA_DESC_RING_COUNT(i)		(TDMA_DESC_RING_BASE(i) + \
					RING_COUNT)
#define TDMA_DESC_RING_MAX_HYST(i)	(TDMA_DESC_RING_BASE(i) + \
					RING_MAX_HYST)
#define TDMA_DESC_RING_INTR_CONTROL(i)	(TDMA_DESC_RING_BASE(i) + \
					RING_INTR_CONTROL)
#define TDMA_DESC_RING_PROD_CONS_INDEX(i) \
					(TDMA_DESC_RING_BASE(i) + \
					RING_PROD_CONS_INDEX)
#define TDMA_DESC_RING_MAPPING(i)	(TDMA_DESC_RING_BASE(i) + \
					RING_MAPPING)
#define TDMA_DESC_RING_PCP_DEI_VID(i)	(TDMA_DESC_RING_BASE(i) + \
					RING_PCP_DEI_VID)

#define TDMA_CONTROL			0x600
#define  TDMA_EN			(1 << 0)
#define  TSB_EN				(1 << 1)
#define  TSB_SWAP			(1 << 2)
#define  ACB_ALGO			(1 << 3)
#define  BUF_DATA_OFFSET_SHIFT		4
#define  BUF_DATA_OFFSET_MASK		0x3ff
#define  VLAN_EN			(1 << 14)
#define  SW_BRCM_TAG			(1 << 15)
#define  WNC_KPT_SIZE_UPDATE		(1 << 16)
#define  SYNC_PKT_SIZE			(1 << 17)
#define  ACH_TXDONE_DELAY_SHIFT		18
#define  ACH_TXDONE_DELAY_MASK		0xff

#define TDMA_STATUS			0x604
#define  TDMA_DISABLED			(1 << 0)
#define  TDMA_LL_RAM_INIT_BUSY		(1 << 1)

#define TDMA_SCB_BURST_SIZE		0x608
#define TDMA_OVER_MAX_THRESH_STATUS	0x60c
#define TDMA_OVER_HYST_THRESH_STATUS	0x610
#define TDMA_TPID			0x614

#define TDMA_FREE_LIST_HEAD_TAIL_PTR	0x618
#define  TDMA_FREE_HEAD_MASK		0x7ff
#define  TDMA_FREE_TAIL_SHIFT		11
#define  TDMA_FREE_TAIL_MASK		0x7ff

#define TDMA_FREE_LIST_COUNT		0x61c
#define  TDMA_FREE_LIST_COUNT_MASK	0x7ff

#define TDMA_TIER2_ARB_CTRL		0x620
#define  TDMA_ARB_MODE_RR		0
#define  TDMA_ARB_MODE_WEIGHT_RR	0x1
#define  TDMA_ARB_MODE_STRICT		0x2
#define  TDMA_ARB_MODE_DEFICIT_RR	0x3
#define  TDMA_CREDIT_SHIFT		4
#define  TDMA_CREDIT_MASK		0xffff

#define TDMA_TIER1_ARB_0_CTRL		0x624
#define  TDMA_ARB_EN			(1 << 0)

#define TDMA_TIER1_ARB_0_QUEUE_EN	0x628
#define TDMA_TIER1_ARB_1_CTRL		0x62c
#define TDMA_TIER1_ARB_1_QUEUE_EN	0x630
#define TDMA_TIER1_ARB_2_CTRL		0x634
#define TDMA_TIER1_ARB_2_QUEUE_EN	0x638
#define TDMA_TIER1_ARB_3_CTRL		0x63c
#define TDMA_TIER1_ARB_3_QUEUE_EN	0x640

#define TDMA_SCB_ENDIAN_OVERRIDE	0x644
#define  TDMA_LE_MODE			(1 << 0)
#define  TDMA_REG_MODE			(1 << 1)

#define TDMA_TEST			0x648
#define  TDMA_TP_OUT_SEL		(1 << 0)
#define  TDMA_MEM_TM			(1 << 1)

#define TDMA_DEBUG			0x64c

/* Transmit/Receive descriptor */
struct dma_desc {
	uint32_t	addr_status_len;
	uint32_t	addr_lo;
};

/* Number of Receive hardware descriptor words */
#define NUM_HW_RX_DESC_WORDS		1024
/* Real number of usable descriptors */
#define NUM_RX_DESC			(NUM_HW_RX_DESC_WORDS / WORDS_PER_DESC)

/* Internal linked-list RAM has up to 1536 entries */
#define NUM_TX_DESC			1536

#define WORDS_PER_DESC			(sizeof(struct dma_desc) / sizeof(uint32_t))

#endif /* __DEV_SYSTEMPORT_H */
