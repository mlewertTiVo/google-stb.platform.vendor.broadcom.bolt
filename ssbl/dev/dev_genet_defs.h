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

#ifndef UNIMAC_COMMON_H
#define UNIMAC_COMMON_H

#include "bsp_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMAC_SPEED_10			0
#define UMAC_SPEED_100			1
#define UMAC_SPEED_1000			2
#define UMAC_SPEED_2500			3

#define PORT_MODE_INT_EPHY		0
#define PORT_MODE_INT_GPHY		1
#define PORT_MODE_EXT_EPHY		2
#define PORT_MODE_EXT_GPHY		3
#define PORT_MODE_EXT_RVMII_25		(4 | (1 << 4))
#define PORT_MODE_EXT_RVMII_50		4

/* umac register group start */
/* reg: umac->hdBkpCtrl */
#define	HD_FC_EN			(1 << 0 )
#define HD_FC_BKOFF_OK			(1 << 1 )
#define IPG_CONFIG_RX_SHIFT		2
#define IPG_CONFIG_RX_MASK		0x1F
/* reg: umac->cmd */
#define CMD_TX_EN			(1 << 0 )
#define CMD_RX_EN			(1 << 1 )
#define CMD_SPEED_SHIFT			2
#define CMD_SPEED_MASK			3
#define CMD_PROMISC			(1 << 4 )
#define CMD_PAD_EN			(1 << 5 )
#define CMD_CRC_FWD			(1 << 6 )
#define CMD_PAUSE_FWD			(1 << 7 )
#define CMD_RX_PAUSE_IGNORE		(1 << 8 )
#define CMD_TX_ADDR_INS			(1 << 9 )
#define CMD_HD_EN			(1 << 10)
#define CMD_SW_RESET			(1 << 13)
#define CMD_LCL_LOOP_EN			(1 << 15)
#define CMD_AUTO_CONFIG			(1 << 22)
#define CMD_CNTL_FRM_EN			(1 << 23)
#define CMD_NO_LEN_CHK			(1 << 24)
#define CMD_RMT_LOOP_EN			(1 << 25)
#define CMD_PRBL_EN			(1 << 27)
#define CMD_TX_PAUSE_IGNORE		(1 << 28)
#define CMD_TX_RX_EN			(1 << 29)
#define CMD_RUNT_FILTER_DIS		(1 << 30)
/* reg: umac->mode */
#define MODE_HD				(1 << 0 )
#define MODE_RX_PAUSE_EN		(1 << 3 )
#define MODE_TX_PAUSE_EN		(1 << 4 )
/* reg: umac->macsec_ctrl */
#define TX_LANUCH_EN			(1 << 0 )
#define TX_CRC_CORRUPT_EN		(1 << 4 )
#define TX_CRC_PROGRAM			(1 << 5 )
#define PAUSE_CTRL_EN			(1 << 17)
/* reg: umac->ts_status */
#define TX_TS_FIFO_FULL			(1 << 0 )
#define TX_TS_FIFO_EMPTY		(1 << 1 )
/* reg: umac->txfifo_status */
#define TX_FIFO_OVERRUN			(1 << 1 )
#define TX_FIFO_UNDERRUN		(1 << 0 )
/* reg: umac->ppp_ctrl */
#define PPP_EN_TX			(1 << 0 )
#define PPP_EN_RX			(1 << 1 )
#define PPP_FORCE_XON			(1 << 2 )
/* reg: umac_ppp_refresh_ctrl */
#define PPP_REFRESH_EN			(1 << 0 )
/* reg: umac->mib_ctrl */
#define MIB_RESET_RX			(1 << 0 )
#define MIB_RESET_RUNT			(1 << 1 )
#define MIB_RESET_TX			(1 << 2 )
/* reg: umac->mpd_ctrl */
#define MPD_EN				(1 << 0 )
#define MPD_PW_EN			(1 << 27)
#define MPD_MSEQ_LEN_SHIFT		16
#define MPD_MSEQ_LEN_MASK		0xFF
/* reg: umac->mdio_cmd, non-proc export */
#define MDIO_START_BUSY			(1 << 29)
#define MDIO_READ_FAIL			(1 << 28)
#define MDIO_RD				(2 << 26)
#define MDIO_WR				(1 << 26)
#define MDIO_PMD_SHIFT			21
#define MDIO_PMD_MASK			0x1F
#define MDIO_REG_SHIFT			16
#define MDIO_REG_MASK			0x1F
/* umac register group end */

/* rbuf register group start */
/* reg: rbuf->rbuf_ctrl */
#define RBUF_64B_EN			(1 << 0 )
#define RBUF_ALIGN_2B			(1 << 1 )
#define RBUF_BAD_DIS			(1 << 2 )
/* reg: rbuf->rbuf_status */
#define RBUF_STATUS_WOL			(1 << 0 )
#define RBUF_STATUS_MPD_INTR_ACTIVE	(1 << 1 )
#define RBUF_STATUS_ACPI_INTR_ACTIVE	(1 << 2 )
/* reg: rbuf->rbuf_endian_ctrl */
#define RBUF_ENDIAN_SWAP		(1 << 2 )
#define RBUF_ENDIAN_NOSWAP		(1 << 0 )
#define RBUF_ENDIAN_MODE		(1 << 1 )
/* reg: rbuf->rbuf_chk_ctrl */
#define RBUF_RXCHK_EN			(1 << 0 )
#define RBUF_SKIP_FCS			(1 << 4 )
/* reg: rbuf->rbuf_hfb_ctrl */
#define RBUF_HFB_FILTER_EN_SHIFT	16
#define RBUF_HFB_FILTER_EN_MASK	0xffff0000
#define RBUF_HFB_EN			(1 << 0 )
#define RBUF_HFB_256B			(1 << 1 )
#define RBUF_ACPI_EN			(1 << 2 )
/* reg: rbuf->rbuf_fltr_len */
#define RBUF_FLTR_LEN_MASK		0xFF
#define RBUF_FLTR_LEN_SHIFT		8

/* reg: rbuf->tbuf_bp_mc */
#define TBUF_CPU_BP			(1 << 8 )
/* rbuf register group end */

/* ext register group start */
/* reg: ext->ephy_pwr_mgmt */
#define EXT_PWR_DOWN_BIAS		(1 << 0 )
#define EXT_PWR_DOWN_DLL		(1 << 1 )
#define EXT_PWR_DOWN_PHY		(1 << 2 )
#define EXT_PWR_DN_EN_LD		(1 << 3 )
#define EXT_ENERGY_DET			(1 << 4 )
#define EXT_IDDQ_FROM_PHY		(1 << 5 )
#define EXT_PHY_RESET			(1 << 8 )
#define EXT_ENERGY_DET_MASK		(1 << 12 )

/* reg: ext->emcg_ctrl */
#define EXT_Ck25_EN			(1 << 0 )
#define EPHY_CLK_SEL			(1 << 4 )

/* reg: ext->rgmii_oob_ctrl */
#define RGMII_LINK			(1 << 4 )
#define OOB_DISABLE			(1 << 5 )
#define RGMII_MODE_EN			(1 << 6)
#define ID_MODE_DIS			(1 << 16)

/* reg: ext->ext_gphy_ctrl */
#define EXT_GPHY_CNTRL_CFG_IDDQ_BIAS       (1 <<  0)
#define EXT_GPHY_CNTRL_CFG_EXT_PWRDOWN     (1 <<  1)
#define EXT_GPHY_CNTRL_CFG_FORCE_DLL_EN    (1 <<  2)
#define EXT_GPHY_CNTRL_CFG_IDDQ_GLOBAL_PWR (1 <<  3)
#define EXT_GPHY_CNTRL_CK25_DIS            (1 <<  4)
#define EXT_GPHY_CNTRL_GPHY_RESET          (1 <<  5)
#define EXT_GPHY_CNTRL_GPHY_PHYAD_SHIFT		8
#define EXT_GPHY_CNTRL_GPHY_PHYAD_MASK		0x1F

/* intrl2_0 register group start */
/* reg: intrl2_0->cpu_mask_status, others are same */
#define UMAC_IRQ_SCB			(1 << 0 )
#define UMAC_IRQ_EPHY			(1 << 1 )
#define UMAC_IRQ_PHY_DET_R		(1 << 2 )
#define UMAC_IRQ_PHY_DET_F		(1 << 3 )
#define UMAC_IRQ_LINK_UP		(1 << 4 )
#define UMAC_IRQ_LINK_DOWN		(1 << 5 )
#define UMAC_IRQ_UMAC			(1 << 6 )
#define UMAC_IRQ_UMAC_TSV		(1 << 7 )
#define UMAC_IRQ_TBUF_UNDERRUN		(1 << 8 )
#define UMAC_IRQ_RBUF_OVERFLOW		(1 << 9 )
#define UMAC_IRQ_HFB_SM			(1 << 10 )
#define UMAC_IRQ_HFB_MM			(1 << 11 )
#define UMAC_IRQ_MPD_R			(1 << 12)
#define UMAC_IRQ_RXDMA_MBDONE		(1 << 13)
#define UMAC_IRQ_RXDMA_PDONE		(1 << 14)
#define UMAC_IRQ_RXDMA_BDONE		(1 << 15)
#define UMAC_IRQ_TXDMA_MBDONE		(1 << 16)
#define UMAC_IRQ_TXDMA_PDONE		(1 << 17)
#define UMAC_IRQ_TXDMA_BDONE		(1 << 18)

#define DMA_RW_POINTER_MASK		0x1FF
/* reg: rDmaRingRegs->rdma_producer_index */
#define DMA_PRODUCER_INDEX_DISCARD_CNT_MASK	0xFFFF	/* For RDMA only */
#define DMA_PRODUCER_INDEX_DISCARD_CNT_SHIFT	16	/* For RDMA only */
#define DMA_BUFFER_DONE_CNT_MASK	0xFFFF		/* For TDMA only */
#define DMA_BUFFER_DONE_CNT_SHIFT	16		/* For TDMA only */
#define DMA_PRODUCER_INDEX_MASK		0xFFFF
#define DMA_CONSUMER_INDEX_MASK		0xFFFF
/* reg: rDmaRingRegs->rdma_ring_buf_size */
#define DMA_RING_SIZE_MASK		0xFFFF
#define DMA_RING_SIZE_SHIFT		16
#define DMA_RING_BUFFER_SIZE_MASK	0xFFFF
/* reg: rDmaRingRegs->rdma_mbuf_done_threshold */
#define DMA_INTR_THRESHOLD_MASK		0x00FF
/* reg: rDmaRingRegs->rdma_xon_xoff_threshold */
#define DMA_XON_THREHOLD_MASK		0xFFFF		/* For RDMA only */
#define DMA_XOFF_THRESHOLD_MASK		0xFFFF		/* For RDMA only */
#define DMA_XOFF_THRESHOLD_SHIFT	16		/* For RDMA only */
/* reg: dDmaRingRegs->tdma_flow_period */
#define DMA_FLOW_PERIOD_MASK		0xFFFF		/* For TDMA only */
#define DMA_MAX_PKT_SIZE_MASK		0xFFFF		/* For TDMA only */
#define DMA_MAX_PKT_SIZE_SHIFT		16		/* For TDMA only */

/* rdma register group start */
/* reg: rDmaRegs->rdma_ctrl */
#define DMA_EN				( 1 << 0)
#define DMA_RING_BUF_EN_SHIFT		0x01
#define DMA_RING_BUF_EN_MASK		0xFFFF
#define DMA_TSB_SWAP_EN			( 1 << 20)	/* For TDMA only */
/* reg : rDmaRegs->rdma_status */
#define DMA_DISABLED			( 1 << 0)
#define DMA_DESC_RAM_INIT_BUSY		( 1 << 1)
/* reg: rDmaRegs->rdma_scb_burst_size */
#define DMA_SCB_BURST_SIZE_MASK		0x1F
/* reg: rDmaRegs->rdma_activity */
#define DMA_ACTIVITY_VECTOR_MASK	0x1FFFF
/* reg: rDmaRegs->rdma_mask */
#define DMA_BACKPRESSURE_MASK		0x1FFFF
#define DMA_PFC_ENABLE			( 1 << 31)
/* reg: rDmaRegs->rdma_map non-proc export*/
#define DMA_MAP_MASK			0x0F
#define DMA_MAP_SHIFT			4
/* reg: rDmaRegs->rdma_back_status */
#define DMA_BACKPRESSURE_STATUS_MASK	0x1FFFF
/* reg: rDmaRegs->rdma_override */
#define DMA_LITTLE_ENDIAN_MODE		( 1 << 0)
#define DMA_REGISTER_MODE		( 1 << 1)
/* reg: rDmaRegs->rdma_timeout non-proc export */
#define DMA_TIMEOUT_MASK		0xFFFF		/* For RDMA only */

#define DMA_RATE_LIMIT_EN_MASK		0xFFFF		/* For TDMA only */
#define DMA_ARBITER_MODE_MASK		0x03		/* For TDMA only */
#define DMA_RING_BUF_PRIORITY_MASK	0x1F		/* For TDMA only */
#define DMA_RING_BUF_PRIORITY_SHIFT	5		/* For TDMA only */
#define DMA_RATE_ADJ_MASK		0xFF		/* For TDMA only */

/*-------------------Dma Descriptor defines ------------------------------------*/
/* reg : none , non-proc export (not for registers ) */
/* Tx/Rx Dma Descriptor common bits*/
#define DMA_BUFLENGTH_MASK		0x0fff
#define DMA_BUFLENGTH_SHIFT		16
#define DMA_OWN				0x8000
#define DMA_EOP				0x4000
#define DMA_SOP				0x2000
#define DMA_WRAP			0x1000
/* Tx specific Dma descriptor bits */
#define DMA_TX_UNDERRUN			0x0200
#define DMA_TX_APPEND_CRC		0x0040
#define DMA_TX_OW_CRC			0x0020
#define DMA_TX_DO_CSUM			0x0010
#if CONFIG_BRCM_GENET_VERSION < 3
#define DMA_TX_QTAG_MASK		0x001F
#else /* GENET_V3+: 6-bit QTAG */
#define DMA_TX_QTAG_MASK		0x003F
#endif
#define DMA_TX_QTAG_SHIFT		7

/* Rx Specific Dma descriptor bits */
#define DMA_RX_BRDCAST			0x0040
#define DMA_RX_MULT			0x0020
#define DMA_RX_LG			0x0010
#define DMA_RX_NO			0x0008
#define DMA_RX_RXER			0x0004
#define DMA_RX_CRC_ERROR		0x0002
#define DMA_RX_OV			0x0001
#define DMA_RX_FI_MASK			0x001F
#define DMA_RX_FI_SHIFT			0x0007
#define DMA_DESC_ALLOC_MASK		0x00FF

#define DMA_ARBITER_RR			0x00
#define DMA_ARBITER_WRR			0x01
#define DMA_ARBITER_SP			0x02

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* UNIMAC_COMMON_H */
