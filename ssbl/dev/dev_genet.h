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

/* uniMAC register definitions.*/

#ifndef __DEV_GENET_H
#define __DEV_GENET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dev_genet_defs.h"

#define PHY_ID_AUTO	0x100
#define PHY_ID_NONE	0x101

#ifndef __ASSEMBLY__


#define TOTAL_DESC			256
#define DMA_RING_DESC_INDEX		16

/* Register block offset */
#define UMAC_GR_BRIDGE_REG_OFFSET	0x0040
#define UMAC_EXT_REG_OFFSET		0x0080
#define UMAC_INTRL2_0_REG_OFFSET	0x0200
#define UMAC_INTRL2_1_REG_OFFSET	0x0240
#define UMAC_RBUF_REG_OFFSET		0x0300
#define UMAC_UMAC_REG_OFFSET		0x0800
#if CONFIG_BRCM_GENET_VERSION == 1
	#define UMAC_HFB_OFF                0x1000
	#define UMAC_RDMA_REG_OFFSET        0x2000
	#define UMAC_TDMA_REG_OFFSET        0x3000
#elif CONFIG_BRCM_GENET_VERSION == 2
	#define UMAC_TBUF_REG_OFFSET        0x0600
	#define UMAC_HFB_OFF                0x1000
	#define UMAC_HFB_REG_OFFSET         0x2000
	#define UMAC_RDMA_REG_OFFSET        0x3000
	#define UMAC_TDMA_REG_OFFSET        0x4000
#elif CONFIG_BRCM_GENET_VERSION == 3
	#define UMAC_TBUF_REG_OFFSET        0x0600
	#define UMAC_HFB_OFF                0x8000
	#define UMAC_HFB_REG_OFFSET         0xfc00
	#define UMAC_RDMA_REG_OFFSET        0x10000
	#define UMAC_TDMA_REG_OFFSET        0x11000
#elif CONFIG_BRCM_GENET_VERSION == 4
	#define UMAC_TBUF_REG_OFFSET        0x0600
	#define UMAC_HFB_OFF                0x8000
	#define UMAC_HFB_REG_OFFSET         0xfc00
	#define UMAC_RDMA_REG_OFFSET        0x2000
	#define UMAC_TDMA_REG_OFFSET        0x4000
#endif

/*****************************************************************************/
/* 64B Rx Status Block */
typedef struct RxStatus
{
	unsigned long length_status;	/* length and peripheral status */
	unsigned long ext_status;	/* Extended status*/
	unsigned long csum;		/* raw checksum */
#if CONFIG_BRCM_GENET_VERSION < 3
	unsigned long filter_index;	/* Filter index */
	unsigned long extracted_bytes[4];/* Extracted byte 0 - 16 */
	unsigned long reserved[4];
#else /* GENET_V3+ */
	unsigned long filter_index[2];	/* Filter index */
	unsigned long extracted_bytes[4];/* Extracted byte 0 - 16 */
	unsigned long reserved[3];
#endif
	unsigned long tx_csum_info;	/* Tx checksum info. */
	unsigned long unused[3];	/* unused */
}RSB;

#define RSB_PS_MASK		0x0FFF
#define RSB_SOP			0x2000
#define RSB_EOP			0x4000
#define RSB_LENGTH_MASK		0xFFF
#define RSB_LENGTH_SHIFT	16
#define RSB_EXT_STATUS_MASK	0x1FFFFF
#define RSB_CSUM_MASK		0xFFFF
#define RSB_FILTER_IDX_MASK	0xFFFF

/* 64B Tx status Block */
typedef struct TxStatus
{
	unsigned long length_status;
	unsigned long unused[15];
}TSB;
#define TSB_CSUM_MASK		0x0FFF
#define TSB_ULP_MASK		0x0FFF
#define TSB_ULP_SHIFT		16
#define TSB_LV			0x80000000

#if CONFIG_BRCM_GENET_VERSION > 3
#define NUM_DESC_PER_BD		3
#else
#define NUM_DESC_PER_BD		2
#endif
#define NUM_DMA_DESC		(TOTAL_DESC * NUM_DESC_PER_BD)

/*
** DMA Descriptor
*/
typedef struct DmaDesc {
	unsigned long length_status;
	unsigned long address;
#if CONFIG_BRCM_GENET_VERSION > 3
	unsigned long address_hi;
#endif
} DmaDesc;

/*
** UniMAC TSV or RSV (Transmit Status Vector or Receive Status Vector
*/
/* Rx/Tx common counter group.*/
typedef struct PktCounterSize {
	unsigned long cnt_64;			/* RO */
	unsigned long cnt_127;			/* RO */
	unsigned long cnt_255;			/* RO */
	unsigned long cnt_511;			/* RO */
	unsigned long cnt_1023;			/* RO */
	unsigned long cnt_1518;			/* RO */
	unsigned long cnt_mgv;			/* RO */
	unsigned long cnt_2047;			/* RO */
	unsigned long cnt_4095;			/* RO */
	unsigned long cnt_9216;			/* RO */
}PktCounterSize;
/* RSV, Receive Status Vector */
typedef struct UniMacRSV {
	PktCounterSize stat_sz;			/* (0x400 - 0x424) */
	unsigned long rx_pkt;			/* RO (0x428) */
	unsigned long rx_bytes;			/* RO */
	unsigned long rx_mca;			/* RO */
	unsigned long rx_bca;			/* RO */
	unsigned long rx_fcs;			/* RO */
	unsigned long rx_cf;			/* RO */
	unsigned long rx_pf;			/* RO */
	unsigned long rx_uo;			/* RO */
	unsigned long rx_aln;			/* RO */
	unsigned long rx_flr;			/* RO */
	unsigned long rx_cde;			/* RO */
	unsigned long rx_fcr;			/* RO */
	unsigned long rx_ovr;			/* RO */
	unsigned long rx_jbr;			/* RO */
	unsigned long rx_mtue;			/* RO */
	unsigned long rx_pok;			/* RO */
	unsigned long rx_uc;			/* RO */
	unsigned long rx_ppp;			/* RO */
	unsigned long rcrc;			/* RO (0x470) */
}UniMacRSV;

/* TSV, Transmit Status Vector */
typedef struct UniMacTSV {
	PktCounterSize stat_sz;			/* (0x480 - 0x0x4a4) */
	unsigned long tx_pkt;			/* RO (0x4a8) */
	unsigned long tx_mca;			/* RO */
	unsigned long tx_bca;			/* RO */
	unsigned long tx_pf;			/* RO */
	unsigned long tx_cf;			/* RO */
	unsigned long tx_fcs;			/* RO */
	unsigned long tx_ovr;			/* RO */
	unsigned long tx_drf;			/* RO */
	unsigned long tx_edf;			/* RO */
	unsigned long tx_scl;			/* RO */
	unsigned long tx_mcl;			/* RO */
	unsigned long tx_lcl;			/* RO */
	unsigned long tx_ecl;			/* RO */
	unsigned long tx_frg;			/* RO */
	unsigned long tx_ncl;			/* RO */
	unsigned long tx_jbr;			/* RO */
	unsigned long tx_bytes;			/* RO */
	unsigned long tx_pok;			/* RO */
	unsigned long tx_uc;			/* RO */
}UniMacTSV;

typedef struct uniMacRegs {
	unsigned long unused;			/* (00) */
	unsigned long hdBkpCtrl;		/* (04) */
	unsigned long cmd;			/* (08) */
	unsigned long mac_0;			/* (0x0c) RW */
	unsigned long mac_1;			/* (0x10) RW */
	unsigned long max_frame_len;		/* (0x14) RW */
	unsigned long pause_quant;		/* (0x18) RW */
	unsigned long unused0[9];
	unsigned long sdf_offset;		/* (0x40) RW */
	unsigned long mode;			/* (0x44) RO */
	unsigned long frm_tag0;			/* (0x48) RW */
	unsigned long frm_tag1;			/* (0x4c) RW */
	unsigned long unused10[3];
	unsigned long tx_ipg_len;		/* (0x5c) RW */
	unsigned long unused1[172];
	unsigned long macsec_tx_crc;		/* (0x310) RW */
	unsigned long macsec_ctrl;		/* (0x314) RW */
	unsigned long ts_status;		/* (0x318) RO */
	unsigned long ts_data;			/* (0x31c) RO */
	unsigned long unused2[4];
	unsigned long pause_ctrl;		/* (0x330) RW */
	unsigned long tx_flush;			/* (0x334) RW */
	unsigned long rxfifo_status;		/* (0x338) RO */
	unsigned long txfifo_status;		/* (0x33c) RO */
	unsigned long ppp_ctrl;			/* (0x340) RW */
	unsigned long ppp_refresh_ctrl;		/* (0x344) RW */
	unsigned long unused11[4];		/* (0x348 - 0x354) RW */
	unsigned long unused12[4];		/* (0x358 - 0x364) RW */
	unsigned long unused13[38];
	UniMacRSV rsv;				/* (0x400 - 0x470) */
	unsigned long unused3[3];
	UniMacTSV tsv;				/* (0x480 - 0x4f0) */
	unsigned long unused4[7];
	unsigned long unused5[28];
	unsigned long mib_ctrl;			/* (0x580) RW */
	unsigned long unused6[31];
	unsigned long bkpu_ctrl;		/* (0x600) RW */
	unsigned long mac_rxerr_mask;		/* (0x604) RW */
	unsigned long max_pkt_size;		/* (0x608) RW */
	unsigned long unused7[2];
	unsigned long mdio_cmd;			/* (0x614) RO */
	unsigned long mdio_cfg;			/* (0x618) */
#if CONFIG_BRCM_GENET_VERSION > 1
	unsigned long unused9;
#else
	unsigned long rbuf_ovfl_pkt_cnt;	/* (0x61c) RO */
#endif
	unsigned long mpd_ctrl;			/* (0x620) RW */
	unsigned long mpd_pw_ms;		/* (0x624) RW */
	unsigned long mpd_pw_ls;		/* (0x628) RW */
	unsigned long unused8[9];
	unsigned long mdf_ctrl;			/* (0x650) RW */
	unsigned long mdf_addr[34];		/* (0x654 - 0x6d8) */
}uniMacRegs;

#if CONFIG_BRCM_GENET_VERSION < 3
#define HFB_NUM_FLTRS		16
#else
#define HFB_NUM_FLTRS		48
#endif

#if CONFIG_BRCM_GENET_VERSION > 1
typedef struct tbufRegs
{
	unsigned long tbuf_ctrl;		/* (00) */
	unsigned long unused0;
	unsigned long tbuf_endian_ctrl;		/* (08) */
	unsigned long tbuf_bp_mc;		/* (0c) */
	unsigned long tbuf_pkt_rdy_thld;	/* (10) */
	unsigned long tbuf_energy_ctrl;		/* (14) */
	unsigned long tbuf_ext_bp_stats;	/* (18) */
	unsigned long tbuf_tsv_mask0;
	unsigned long tbuf_tsv_mask1;
	unsigned long tbuf_tsv_status0;
	unsigned long tbuf_tsv_status1;
}tbufRegs;

typedef struct rbufRegs
{
	unsigned long rbuf_ctrl;		/* (00) */
	unsigned long unused0;
	unsigned long rbuf_pkt_rdy_thld;	/* (08) */
	unsigned long rbuf_status;		/* (0c) */
	unsigned long rbuf_endian_ctrl;		/* (10) */
	unsigned long rbuf_chk_ctrl;		/* (14) */
#if CONFIG_BRCM_GENET_VERSION == 2
	unsigned long rbuf_rxc_offset[8];	/* (18 - 34) */
	unsigned long unused1[18];
	unsigned long rbuf_ovfl_pkt_cnt;	/* (80) */
	unsigned long rbuf_err_cnt;		/* (84) */
	unsigned long rbuf_energy_ctrl;		/* (88) */

	unsigned long unused2[7];
	unsigned long rbuf_pd_sram_ctrl;	/* (a8) */
	unsigned long unused3[12];
	unsigned long rbuf_test_mux_ctrl;	/* (dc) */
#else /* GENET_V3+ */
	unsigned long unused1[7];		/* (18 - 34) */
	unsigned long rbuf_rxc_offset[24];	/* (34 - 90) */
	unsigned long rbuf_ovfl_pkt_cnt;	/* (94) */
	unsigned long rbuf_err_cnt;		/* (98) */
	unsigned long rbuf_energy_ctrl;		/* (9c) */
	unsigned long rbuf_pd_sram_ctrl;	/* (a0) */
	unsigned long rbuf_test_mux_ctrl;	/* (a4) */
	unsigned long rbuf_spare_reg0;		/* (a8) */
	unsigned long rbuf_spare_reg1;		/* (ac) */
	unsigned long rbuf_spare_reg2;		/* (b0) */
	unsigned long rbuf_tbuf_size_ctrl;	/* (b4) */
#endif
}rbufRegs;

typedef struct hfbRegs
{
	unsigned long hfb_ctrl;
#if CONFIG_BRCM_GENET_VERSION > 2
	unsigned long hfb_flt_enable[2];
	unsigned long unused[4];
#endif
	unsigned long hfb_fltr_len[HFB_NUM_FLTRS / 4];
}hfbRegs;

#else /* CONFIG_BRCM_GENET_VERSION > 1 */
typedef struct rbufRegs
{
	unsigned long rbuf_ctrl;		/* (00) */
	unsigned long rbuf_flush_ctrl;		/* (04) */
	unsigned long rbuf_pkt_rdy_thld;	/* (08) */
	unsigned long rbuf_status;		/* (0c) */
	unsigned long rbuf_endian_ctrl;		/* (10) */
	unsigned long rbuf_chk_ctrl;		/* (14) */
	unsigned long rbuf_rxc_offset[8];	/* (18 - 34) */
	unsigned long rbuf_hfb_ctrl;		/* (38) */
	unsigned long rbuf_fltr_len[HFB_NUM_FLTRS / 4];	/* (3c - 48) */
	unsigned long unused0[13];
	unsigned long tbuf_ctrl;		/* (80) */
	unsigned long tbuf_flush_ctrl;		/* (84) */
	unsigned long unused1[5];
	unsigned long tbuf_endian_ctrl;		/* (9c) */
	unsigned long tbuf_bp_mc;		/* (a0) */
	unsigned long tbuf_pkt_rdy_thld;	/* (a4) */
	unsigned long unused2[2];
	unsigned long rgmii_oob_ctrl;		/* (b0) */
	unsigned long rgmii_ib_status;		/* (b4) */
	unsigned long rgmii_led_ctrl;		/* (b8) */
	unsigned long unused3;
	unsigned long moca_status;		/* (c0) */
	unsigned long unused4[6];
	unsigned long test_mux_ctrl;		/* (dc) */
}rbufRegs;
#endif

/* uniMac intrl2 registers */
typedef struct intrl2Regs
{
	unsigned long cpu_stat;			/*(00) */
	unsigned long cpu_set;			/*(04) */
	unsigned long cpu_clear;		/*(08) */
	unsigned long cpu_mask_status;		/*(0c) */
	unsigned long cpu_mask_set;		/*(10) */
	unsigned long cpu_mask_clear;		/*(14) */
	unsigned long pci_stat;			/*(00) */
	unsigned long pci_set;			/*(04) */
	unsigned long pci_clear;		/*(08) */
	unsigned long pci_mask_status;		/*(0c) */
	unsigned long pci_mask_set;		/*(10) */
	unsigned long pci_mask_clear;		/*(14) */
}intrl2Regs;

typedef struct SysRegs
{
	unsigned long sys_rev_ctrl;
	unsigned long sys_port_ctrl;
#if CONFIG_BRCM_GENET_VERSION > 1
	unsigned long rbuf_flush_ctrl;
	unsigned long tbuf_flush_ctrl;
#endif
}SysRegs;

typedef struct GrBridgeRegs
{
	unsigned long gr_bridge_rev;
	unsigned long gr_bridge_ctrl;
	unsigned long gr_bridge_sw_reset_0;
	unsigned long gr_bridge_sw_reset_1;
}GrBridgeRegs;

typedef struct ExtRegs
{
	unsigned long ext_pwr_mgmt;
	unsigned long ext_emcg_ctrl;
	unsigned long ext_test_ctrl;
#if CONFIG_BRCM_GENET_VERSION > 1
	unsigned long rgmii_oob_ctrl;
	unsigned long rgmii_ib_status;
	unsigned long rgmii_led_ctrl;
	unsigned long ext_genet_pwr_mgmt;
#if CONFIG_BRCM_GENET_VERSION > 3
	unsigned long ext_gphy_ctrl;
	unsigned long ext_gphy_status;
#endif
#else
	unsigned long ext_in_ctrl;
	unsigned long ext_fblp_ctrl;
	unsigned long ext_stat0;
	unsigned long ext_stat1;
	unsigned long ext_ch_ctrl[6];
#endif
}ExtRegs;

typedef struct rDmaRingRegs
{
	unsigned long rdma_write_pointer;
#if CONFIG_BRCM_GENET_VERSION > 3
	unsigned long rdma_write_pointer_hi;
#endif
	unsigned long rdma_producer_index;
	unsigned long rdma_consumer_index;
	unsigned long rdma_ring_buf_size;
	unsigned long rdma_start_addr;
#if CONFIG_BRCM_GENET_VERSION > 3
	unsigned long rdma_start_addr_hi;
#endif
	unsigned long rdma_end_addr;
#if CONFIG_BRCM_GENET_VERSION > 3
	unsigned long rdma_end_addr_hi;
#endif
	unsigned long rdma_mbuf_done_threshold;
	unsigned long rdma_xon_xoff_threshold;
	unsigned long rdma_read_pointer;
#if CONFIG_BRCM_GENET_VERSION > 3
	unsigned long rdma_read_pointe_hi;
	unsigned long unused[3];
#else
	unsigned long unused[7];
#endif
}rDmaRingRegs;

typedef struct tDmaRingRegs
{
	unsigned long tdma_read_pointer;
#if CONFIG_BRCM_GENET_VERSION > 3
	unsigned long tdma_read_pointer_hi;
#endif
	unsigned long tdma_consumer_index;
	unsigned long tdma_producer_index;
	unsigned long tdma_ring_buf_size;
	unsigned long tdma_start_addr;
#if CONFIG_BRCM_GENET_VERSION > 3
	unsigned long tdma_start_addr_hi;
#endif
	unsigned long tdma_end_addr;
#if CONFIG_BRCM_GENET_VERSION > 3
	unsigned long tdma_end_addr_hi;
#endif
	unsigned long tdma_mbuf_done_threshold;
	unsigned long tdma_flow_period;
	unsigned long tdma_write_pointer;
#if CONFIG_BRCM_GENET_VERSION > 3
	unsigned long tdma_write_pointer_hi;
	unsigned long unused[3];
#else
	unsigned long unused[7];
#endif
}tDmaRingRegs;

typedef struct rDmaRegs
{
	rDmaRingRegs rDmaRings[17];
#if CONFIG_BRCM_GENET_VERSION > 1
	unsigned long rdma_ring_cfg;
#endif
	unsigned long rdma_ctrl;
	unsigned long rdma_status;
#if CONFIG_BRCM_GENET_VERSION < 2
	unsigned long unused;
#endif
	unsigned long rdma_scb_burst_size;
	unsigned long rdma_activity;
	unsigned long rdma_mask;
	unsigned long rdma_map[3];
	unsigned long rdma_back_status;
	unsigned long rdma_override;
	unsigned long rdma_timeout[17];
#if CONFIG_BRCM_GENET_VERSION > 1
	unsigned long rdma_index2ring[8];
#endif
	unsigned long rdma_test;
	unsigned long rdma_debug;
}rDmaRegs;

typedef struct tDmaRegs
{
	tDmaRingRegs tDmaRings[17];
#if CONFIG_BRCM_GENET_VERSION > 1
	unsigned long tdma_ring_cfg;
#endif
	unsigned long tdma_ctrl;
	unsigned long tdma_status;
#if CONFIG_BRCM_GENET_VERSION > 1
	unsigned long unused;
#endif
	unsigned long tdma_scb_burst_size;
	unsigned long tdma_activity;
	unsigned long tdma_mask;
#if CONFIG_BRCM_GENET_VERSION > 2
	unsigned long tdma_map[2];
#else
	unsigned long tdma_map[3];
#endif
	unsigned long tdma_back_status;
	unsigned long tdma_override;
	unsigned long tdma_rate_limit_ctrl;
	unsigned long tdma_arb_ctrl;
	unsigned long tdma_priority[3];
#if CONFIG_BRCM_GENET_VERSION > 1
	unsigned long tdma_rate_adj;
	unsigned long tdma_test;
	unsigned long tdma_debug;
#else
	unsigned long tdma_test;
	unsigned long tdma_debug;
	unsigned long tdma_rate_adj;
#endif
}tDmaRegs;

/* These macros are defined to deal with register map change
 * between GENET1.1 and GENET2. Only those currently being used
 * by driver are defined.
 */
#if CONFIG_BRCM_GENET_VERSION > 1

#define GENET_TBUF_CTRL(pdev)			(pdev->tbuf->tbuf_ctrl)
#define GENET_TBUF_BP_MC(pdev)			(pdev->tbuf->tbuf_bp_mc)
#define GENET_TBUF_ENDIAN_CTRL(pdev)		(pdev->tbuf->tbuf_endian_ctrl)
#define GENET_TBUF_FLUSH_CTRL(pdev)		(pdev->sys->tbuf_flush_ctrl)
#define GENET_RBUF_FLUSH_CTRL(pdev)		(pdev->sys->rbuf_flush_ctrl)
#define GENET_RGMII_OOB_CTRL(pdev)		(pdev->ext->rgmii_oob_ctrl)
#define GENET_RGMII_IB_STATUS(pdev)		(pdev->ext->rgmii_ib_status)
#define GENET_RGMII_LED_STATUS(pdev)		(pdev->ext->rgmii_led_ctrl)
#define GENET_HFB_CTRL(pdev)			(pdev->hfbReg->hfb_ctrl)
#define GENET_HFB_FLTR_LEN(pdev, i)		(pdev->hfbReg->hfb_fltr_len[i])

#else

#define GENET_TBUF_CTRL(pdev)			(pdev->rbuf->tbuf_ctrl)
#define GENET_TBUF_BP_MC(pdev)			(pdev->rbuf->tbuf_bp_mc)
#define GENET_TBUF_ENDIAN_CTRL(pdev)		(pdev->rbuf->tbuf_endian_ctrl)
#define GENET_TBUF_FLUSH_CTRL(pdev)		(pdev->rbuf->tbuf_flush_ctrl)
#define GENET_RBUF_FLUSH_CTRL(pdev)		(pdev->rbuf->rbuf_flush_ctrl)
#define GENET_RGMII_OOB_CTRL(pdev)		(pdev->rbuf->rgmii_oob_ctrl)
#define GENET_RGMII_IB_STATUS(pdev)		(pdev->rbuf->rgmii_ib_status)
#define GENET_RGMII_LED_STATUS(pdev)		(pdev->rbuf->rgmii_led_ctrl)
#define GENET_HFB_CTRL(pdev)			(pdev->rbuf->rbuf_hfb_ctrl)
#define GENET_HFB_FLTR_LEN(pdev, i)		(pdev->rbuf->rbuf_fltr_len[i])

#endif /* CONFIG_BRCM_GENET_VERSION > 1 */

#endif /* __ASSEMBLY__ */

extern uint32_t mii_get_phy_id(volatile uniMacRegs *umac, uint32_t phy);
extern char *genet_get_phyintf(uint32_t base);

#ifdef __cplusplus
}
#endif
#endif
