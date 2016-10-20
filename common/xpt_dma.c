/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <common.h>

#include <bchp_common.h>
#include <bchp_sun_top_ctrl.h>
#include <bchp_xpt_bus_if.h>
#include <bchp_xpt_fe.h>
#if CFG_MEMDMA_MCPB
#include <bchp_xpt_memdma_mcpb_ch0.h>
#include <bchp_xpt_memdma_mcpb.h>
#else
#include <bchp_xpt_mcpb_ch0.h>
#include <bchp_xpt_mcpb.h>
#endif
#include <bchp_xpt_wdma_regs.h>

#define INT_SUPPORTED 1

#if INT_SUPPORTED
#include <bchp_xpt_wdma_desc_done_intr_l2.h>
#include <bchp_xpt_wdma_cpu_intr_aggregator.h>
#endif

#ifdef BCHP_XPT_WDMA_CH0_REG_START
#include <bchp_xpt_wdma_ch0.h>
#define BCHP_XPT_WDMA_RAMS_COMPLETED_DESC_ADDRESS \
        BCHP_XPT_WDMA_CH0_COMPLETED_DESC_ADDRESS
#define BCHP_XPT_WDMA_RAMS_FIRST_DESC_ADDR \
        BCHP_XPT_WDMA_CH0_FIRST_DESC_ADDR
#else
#include <bchp_xpt_wdma_rams.h>
#endif

#include <bchp_xpt_security_ns.h>
#include <bchp_xpt_security_ns_intr2_0.h>
#include <bchp_xpt_pmu.h>

#include <lib_types.h>

#include "xpt_dma.h"
#include "cache_ops.h"

/* descriptor flags and shifts */
#define MCPB_DW2_LAST_DESC		(1 << 0)

#define MCPB_DW4_INT_EN			(1 << 31)
#define MCPB_DW4_PUSH_PARTIAL_PKT	(1 << 28)

#define MCPB_DW5_ENDIAN_STRAP_INV  (1 << 21)
#define MCPB_DW5_PID_CHANNEL_VALID (1 << 19)
#define MCPB_DW5_SCRAM_END         (1 << 18)
#define MCPB_DW5_SCRAM_START       (1 << 17)
#define MCPB_DW5_SCRAM_INIT        (1 << 16)

#define WDMA_DW3_LAST                  (1 << 0)
#define WDMA_DW3_INTR_EN               (1 << 1)
#define WDMA_DW3_END_INVERT            (1 << 2) /* SW7445-96: swap byte */

#define MEMDMA_DRAM_REQ_SIZE	256

#define XPT_MAC_OFFSET		0x14

#if CFG_MEMDMA_MCPB
#define MCPB_CHx_SPACING(channel) \
	((BCHP_XPT_MEMDMA_MCPB_CH1_REG_START - \
	 BCHP_XPT_MEMDMA_MCPB_CH0_REG_START) * (channel))
#else
#define MCPB_CHx_SPACING(channel) \
	((BCHP_XPT_MCPB_CH1_REG_START - \
	 BCHP_XPT_MCPB_CH0_REG_START) * (channel))
#endif

#define XPT_CHANNEL_A	0
#define XPT_CHANNEL_B	1

#define XPT_MAC_A	0
#define XPT_MAC_B	1

#define XPT_SHA20 0
#define XPT_SHA21 1

#define MAX_HASH_WAIT_US	(15 * 1000 * 1000) /* 15 seconds */
#define MAX_SHA_WAIT_US	(5 * 1000 * 1000) /* 5 seconds */

enum xpt_dma_mode {
	XPT_DMA_HASH = 0,       /* calculate hash */
	XPT_DMA_SCRAMBLE,       /* scramble payload of 130-byte DSS packets */
	XPT_DMA_SHA
};

static const struct memdma_initparams *efn;

void memdma_init(const struct memdma_initparams *e)
{
	efn = e;
}

static void check_memdma_initparams(void)
{
	if (efn == NULL) {
		puts("XPTDMA: !INIT");
		while (1)
			;
	}
}

static inline void xpt_set_power(int on)
{
#ifdef BCHP_XPT_PMU_FE_SP_PD_MEM_PWR_DN_CTRL
	uint32_t val = on ? 0 : ~0;

	/* Hard reset XPT. Whatever happens to XPT
	 * this should recover it from any abuse.
	 */
	BDEV_WR_F(SUN_TOP_CTRL_SW_INIT_0_SET, xpt_sw_init, 1);
	BARRIER();
	BDEV_WR_F(SUN_TOP_CTRL_SW_INIT_0_CLEAR, xpt_sw_init, 1);
	BARRIER();

	/* Power on/off everything */
	BDEV_WR(BCHP_XPT_PMU_FE_SP_PD_MEM_PWR_DN_CTRL, val);
	BDEV_WR(BCHP_XPT_PMU_MCPB_SP_PD_MEM_PWR_DN_CTRL, val);
	BDEV_WR(BCHP_XPT_PMU_MEMDMA_SP_PD_MEM_PWR_DN_CTRL, val);
#endif
}

static void mcpb_run(int enable, unsigned int channel)
{
#if CFG_MEMDMA_MCPB
	BDEV_WR_RB(BCHP_XPT_MEMDMA_MCPB_RUN_SET_CLEAR,
			(!!enable << 8) | channel);
#else
	BDEV_WR(BCHP_XPT_MCPB_RUN_SET_CLEAR, ((!!enable << 8) | channel));

#endif
}

static void wdma_run(int enable, unsigned int channel)
{
	BDEV_WR_RB(BCHP_XPT_WDMA_REGS_RUN_SET_CLEAR,
			(!!enable << 8) | channel);
}

static int mcpb_soft_init(void)
{
	int timeout = 1000 * 1000; /* 1 second timeout */

	check_memdma_initparams();
#if CFG_MEMDMA_MCPB 
	BDEV_WR(BCHP_XPT_BUS_IF_SUB_MODULE_SOFT_INIT_DO_MEM_INIT,
			1 << 1); /* MEMDMA */
	BDEV_WR(BCHP_XPT_BUS_IF_SUB_MODULE_SOFT_INIT_SET,
			1 << 1); /* MEMDMA */

#else
	
	BDEV_WR(BCHP_XPT_BUS_IF_SUB_MODULE_SOFT_INIT_DO_MEM_INIT,
		1); /* MCPB */
	BDEV_WR(BCHP_XPT_BUS_IF_SUB_MODULE_SOFT_INIT_SET,
		1); /* MCPB */
#endif
	for (;;) {
#if CFG_MEMDMA_MCPB 		
		if (!BDEV_RD_F(XPT_BUS_IF_SUB_MODULE_SOFT_INIT_STATUS,
					MEMDMA_MCPB_SOFT_INIT_STATUS))
#else
		if (!BDEV_RD_F(XPT_BUS_IF_SUB_MODULE_SOFT_INIT_STATUS,
					XPT_MCPB_SOFT_INIT_STATUS))
#endif
			break;
		if (timeout <= 0)
			return -1;

		timeout -= 10;
		efn->udelay(10);
	}

#if CFG_MEMDMA_MCPB
	BDEV_WR(BCHP_XPT_BUS_IF_SUB_MODULE_SOFT_INIT_CLEAR, 1 << 1); /* MEMDMA */
#else
	BDEV_WR(BCHP_XPT_BUS_IF_SUB_MODULE_SOFT_INIT_CLEAR, 1); /* MCPB */
#endif
			
	return 0;
}

static void memdma_init_mcpb_channel(unsigned int channel, enum xpt_dma_mode mode)
{
	unsigned long packet_len_val;
	unsigned long dma_buf_ctrl_val;
	unsigned long offs = MCPB_CHx_SPACING(channel);
#if CFG_MEMDMA_MCPB	
	unsigned long parser_ctrl = BCHP_XPT_MEMDMA_MCPB_CH0_SP_PARSER_CTRL
					+ offs;
	unsigned long packet_len = BCHP_XPT_MEMDMA_MCPB_CH0_SP_PKT_LEN + offs;
	unsigned long dma_buf_ctrl = BCHP_XPT_MEMDMA_MCPB_CH0_DMA_BBUFF_CTRL
					+ offs;
	unsigned long dma_data_ctrl = BCHP_XPT_MEMDMA_MCPB_CH0_DMA_DATA_CONTROL
					+ offs;
#else
	unsigned long parser_ctrl = BCHP_XPT_MCPB_CH0_SP_PARSER_CTRL
					+ offs;
	unsigned long packet_len = BCHP_XPT_MCPB_CH0_SP_PKT_LEN + offs;
	unsigned long dma_buf_ctrl = BCHP_XPT_MCPB_CH0_DMA_BBUFF_CTRL
					+ offs;
	unsigned long dma_data_ctrl = BCHP_XPT_MCPB_CH0_DMA_DATA_CONTROL
					+ offs;

#endif


	/* setup for block mode */
	BDEV_WR(parser_ctrl,
		(1 << 0) |		/* parser enable */
		(6 << 1) |		/* block mode */
		(channel << 6) |	/* band ID */
#if !CFG_MEMDMA_MCPB
		(1 << 26) |  /*  MEM DMA PIPE Enable */
#endif
		(1 << 14));		/* select playback parser */

	if (mode == XPT_DMA_SHA) {
		/* 188=MPEG|TS 130=DTV 208=0xd0=Block
		    192=0xC0=SHA2* needs 32byte blocks */
		packet_len_val = 192;
		/* 192=MPEG|TS 134=DTV 224=0xe0=Block DMA read from Burst Buf.
		     208=0xd0 SHA2* needs this */
		dma_buf_ctrl_val = 208;
	} else {
		packet_len_val = 208; /* packet length */
		dma_buf_ctrl_val = 224; /* stream feed size */
	}

	BDEV_WR(packet_len, packet_len_val);
	BDEV_WR(dma_buf_ctrl, dma_buf_ctrl_val);
	BDEV_WR(dma_data_ctrl, (MEMDMA_DRAM_REQ_SIZE << 0) |
#if !CFG_MEMDMA_MCPB
		(1 << 10) |
#endif
		(0 << 11));	/* disable run version match */
}

static void memdma_init_wdma_channel(unsigned int channel)
{
	BDEV_WR_F(XPT_WDMA_REGS_RBUF_MODE_CONFIG, CHANNEL_NUM, channel);
	BDEV_WR_F(XPT_WDMA_REGS_RBUF_MODE_CONFIG, CONFIG, 0);

	BDEV_WR_F(XPT_WDMA_REGS_MATCH_RUN_VERSION_CONFIG, CHANNEL_NUM,
			channel);
	BDEV_WR_F(XPT_WDMA_REGS_MATCH_RUN_VERSION_CONFIG, CONFIG, 0);

#if !CFG_MEMDMA_MCPB
	BDEV_WR_F(XPT_WDMA_CH0_DATA_CONTROL, INV_STRAP_ENDIAN_CTRL, 1);  
#endif

}

static void xpt_init_ctx(unsigned int channel, unsigned int pid_channel)
{
	/* configure PID channel */
	BDEV_WR(BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + 4 * pid_channel,
			(1 << 14) |		/* enable PID channel */
			(channel << 16) |	/* input parser band */
			(1 << 23) |		/* playback parser */
			(1 << 28));		/* direct to XPT security */

	BDEV_WR(BCHP_XPT_FE_SPID_TABLE_i_ARRAY_BASE + 4 * pid_channel, 0);

	BDEV_WR(BCHP_XPT_FE_SPID_EXT_TABLE_i_ARRAY_BASE +
		     4 * pid_channel, 1); /* G pipe */
}

static void memdma_init_hw(unsigned int channel, int pid,
				enum xpt_dma_mode mode, bool int_enable)
{
	mcpb_run(0, channel);

	if (mode == XPT_DMA_SCRAMBLE)
		wdma_run(0, 0);

	memdma_init_mcpb_channel(channel, mode);

	if (mode == XPT_DMA_SCRAMBLE)
		memdma_init_wdma_channel(channel);

	xpt_init_ctx(channel, pid);

#if INT_SUPPORTED
	if (int_enable) {
		BDEV_WR(BCHP_XPT_WDMA_DESC_DONE_INTR_L2_CPU_MASK_CLEAR,
			(1 << channel));

		BDEV_WR_F(XPT_WDMA_CPU_INTR_AGGREGATOR_INTR_W0_MASK_CLEAR,
			DESC_DONE_INTR, 1);
	}
#endif
}

static int __mcpb_init_desc(struct mcpb_dma_desc *desc, dma_addr_t next,
		dma_addr_t buf, size_t len, int first, int last,
		unsigned int pid_channel, bool hashmode)
{
	check_memdma_initparams();
	efn->memset(desc, 0, sizeof(*desc));

	/* 5 LSBs must be 0; can only handle 32-bit addresses */
	if ((next & 0x1f) || upper_32_bits(next))
		return -1;

	desc->buf_hi = upper_32_bits(buf);
	desc->buf_lo = lower_32_bits(buf); /* BUFF_ST_RD_ADDR [31:0] */
	desc->next_offs = lower_32_bits(next); /* NEXT_DESC_ADDR [31:5] */
	desc->size = len; /* BUFF_SIZE [31:0] */
	desc->opts2 = MCPB_DW5_PID_CHANNEL_VALID | MCPB_DW5_ENDIAN_STRAP_INV;
	desc->pid_channel = pid_channel;

	if (first)
		desc->opts2 |= (MCPB_DW5_SCRAM_INIT | MCPB_DW5_SCRAM_START);

	if (last) {
		desc->next_offs = MCPB_DW2_LAST_DESC;
		desc->opts1 |= MCPB_DW4_PUSH_PARTIAL_PKT;
		if (hashmode)
			desc->opts2 |= MCPB_DW5_SCRAM_END;
	}

	return 0;
}

/* Setup single MCPB descriptor, for use with WDMA (e.g., decryption) */
int mcpb_init_desc(struct mcpb_dma_desc *mcpb, struct dma_region *region)
{
	return __mcpb_init_desc(mcpb, 0, region->addr, region->len, true, true,
				PID_CHANNEL_B, false);
}

/* Setup single WDMA descriptor */
void wdma_init_desc(struct wdma_desc *desc, struct dma_region *region,
		bool int_enable)
{
	desc->buf_hi = upper_32_bits(region->addr);
	desc->buf_lo = lower_32_bits(region->addr);
	desc->size = region->len;
	desc->next_offs = WDMA_DW3_END_INVERT;
	desc->next_offs |= WDMA_DW3_LAST;

	if (INT_SUPPORTED && int_enable)
		desc->next_offs |= WDMA_DW3_INTR_EN;
}

/*
 * memdma_prepare_descs - prepare a MEMDMA descriptor chain
 *
 * @descs: array of descriptors
 * @descs_pa: physical address of @descs
 * @regions: the address ranges to set up for MEMDMA
 * @numregions: number of regions (in @descs and @regions)
 * @channel_A: if true, use the first MAC channel (a.k.a. "channel A"); if
 *     false, use the second MAC channel (a.k.a. "channel B")
 */
int memdma_prepare_descs(struct mcpb_dma_desc *descs, dma_addr_t descs_pa,
		struct dma_region *regions, int numregions, bool channel_A)
{
	int i;
	int pid = channel_A ? PID_CHANNEL_A : PID_CHANNEL_B;
	dma_addr_t next_pa = descs_pa;
	int ret;

	for (i = 0; i < numregions; i++) {
		int first = (i == 0);
		int last = (i == (numregions - 1));

		if (last)
			next_pa = 0;
		else
			next_pa += sizeof(*descs);

		ret = __mcpb_init_desc(&descs[i], next_pa, regions[i].addr,
				     regions[i].len, first, last, pid, true);
		if (ret)
			return ret;
	}

	return 0;
}

static bool hash_is_ready(int mac)
{
	if (mac)
		return BDEV_RD_F(XPT_SECURITY_NS_INTR2_0_CPU_STATUS, MAC1_READY);
	else
		return BDEV_RD_F(XPT_SECURITY_NS_INTR2_0_CPU_STATUS, MAC0_READY);
}

static void clear_hash_interrupt(int mac)
{
	if (mac)
		BDEV_WR_F(XPT_SECURITY_NS_INTR2_0_CPU_CLEAR, MAC1_READY, 1);
	else
		BDEV_WR_F(XPT_SECURITY_NS_INTR2_0_CPU_CLEAR, MAC0_READY, 1);
}

static int memdma_wait_for_hash(int mac)
{
	int timeout = MAX_HASH_WAIT_US;

	check_memdma_initparams();
	for (;;) {
		if (hash_is_ready(mac))
			break;
		if (timeout <= 0) {
			puts("XPTDMA: HASH !RDY");
			return -1;
		}

		timeout -= 10;
		efn->udelay(10);
	}

	/* Clear status */
	clear_hash_interrupt(mac);

	return 0;
}

static void memdma_start(dma_addr_t desc, unsigned int channel)
{
	unsigned long reg = 
#if CFG_MEMDMA_MCPB		
		BCHP_XPT_MEMDMA_MCPB_CH0_DMA_DESC_CONTROL 
#else
		BCHP_XPT_MCPB_CH0_DMA_DESC_CONTROL 
#endif
		+
		MCPB_CHx_SPACING(channel);

	BDEV_WR(reg, (uint32_t)desc);
	mcpb_run(1, channel);
}

/*
 * memdma_run - Run the MEMDMA MAC on up to 2 DMA descriptor chains
 *
 * @desc1: the physical address of the first descriptor chain
 * @desc2: the physical address of the second descriptor chain (optional)
 * @dual_channel: if true, then use desc2 with a second DMA channel; otherwise,
 *     ignore desc2
 */
int memdma_run(dma_addr_t desc1, dma_addr_t desc2, bool dual_channel)
{
	int ret, ret2 = 0;
	enum xpt_dma_mode mode = XPT_DMA_HASH;

	xpt_set_power(1);

	ret = mcpb_soft_init();
	if (ret)
		goto out;

	memdma_init_hw(0, PID_CHANNEL_A, mode, false);
	BARRIER();
	memdma_start(desc1, XPT_CHANNEL_A);

	if (dual_channel) {
		memdma_init_hw(1, PID_CHANNEL_B, mode, false);
		BARRIER();
		memdma_start(desc2, XPT_CHANNEL_B);
	}

	ret = memdma_wait_for_hash(XPT_MAC_A);
	if (dual_channel)
		ret2 = memdma_wait_for_hash(XPT_MAC_B);

	/* Report the 1st non-zero return code */
	if (!ret)
		ret = ret2;

out:
	xpt_set_power(0);

	return ret;
}

static int mem2mem_dma_init(unsigned int channel, int pid_channel,
		bool int_enable)
{
	int ret;

	xpt_set_power(1);

	ret = mcpb_soft_init();
	if (ret)
		return ret;

	memdma_init_hw(channel, pid_channel, XPT_DMA_SCRAMBLE, int_enable);

	return 0;
}

static int __mem2mem_dma_run(dma_addr_t mcpb, dma_addr_t wdma,
		bool int_enable)
{
	dma_addr_t reg;
	int timeout = 15 * 1000 * 1000; /* 15 seconds */;

	check_memdma_initparams();
	wdma_run(0, 0);

	/* CDA must be cleared, since we're waiting */
	BDEV_WR(BCHP_XPT_WDMA_RAMS_COMPLETED_DESC_ADDRESS, 0);

	/* always start WDMA before MCPB */
	BDEV_WR(BCHP_XPT_WDMA_RAMS_FIRST_DESC_ADDR, wdma);

	wdma_run(1, 0);

	/* never WAKE. always RUN */
	mcpb_run(0, 0);

	memdma_start(mcpb, 0);

	if (INT_SUPPORTED && int_enable && (efn->wait_for_int != NULL)) {
		if (0 == efn->wait_for_int())
			goto out;
		/* else drop down and die() */
	} else {
		do {
			reg = BDEV_RD(
				BCHP_XPT_WDMA_RAMS_COMPLETED_DESC_ADDRESS);
			if (reg == wdma)
				goto out;
			efn->udelay(10);
		} while ((timeout -= 10) > 0);
	}

	efn->die("XPTDMA: DMA !DONE");

out:
	xpt_set_power(0);

	return 0;
}

int mem2mem_dma_run(dma_addr_t mcpb_desc, dma_addr_t wdma_desc, int pid_channel,
		bool int_enable)
{
	int ret;

	/* setup and run dma */
	ret = mem2mem_dma_init(0, pid_channel, int_enable);
	if (ret)
		return ret;

	return __mem2mem_dma_run(mcpb_desc, wdma_desc, int_enable);
}

static uint32_t get_hash_idx(int mac_num, int word)
{
	int len = 128 / 32;
	unsigned long reg_base;

	if (word >= len)
		return 0;

	reg_base = BCHP_XPT_SECURITY_NS_MAC0_0 + mac_num * XPT_MAC_OFFSET;
	return BDEV_RD(reg_base + word * 4);
}

void get_hash(uint32_t *hash, bool dual_channel)
{
	/* 128-bit AES CMAC hash */
	int i, len = 128 / 8;

	/* MAC0 */
	for (i = 0; i < len / (int)sizeof(*hash); i++)
		hash[i] = get_hash_idx(0, i);

	if (dual_channel)
		/* MAC1 */
		for (i = 0; i < len / (int)sizeof(*hash); i++)
			hash[i] ^= get_hash_idx(1, i);
}

/**
 * Return the total length of an array of descriptors, terminated by
 * MCPB_DW2_LAST_DESC or by a maximum array length
 */
size_t mcpb_get_dma_chain_len(struct mcpb_dma_desc *head, int max_descs)
{
	struct mcpb_dma_desc *curr = head;
	size_t len = 0;
	int count = 0;

	if (!curr || max_descs == 0)
		return 0;

	for (count = 0; max_descs < 0 || count < max_descs; count++, curr++) {
		len += curr->size;
		if (curr->next_offs == MCPB_DW2_LAST_DESC)
			break;
	}

	return len;
}

#if CFG_HARDWARE_SHA
static void set_up_sha_control(int sha_num)
{
	if (sha_num == XPT_SHA20) {
		BDEV_RD(BCHP_XPT_SECURITY_NS_SHA20_SEMAPHORE);
		BDEV_WR(BCHP_XPT_SECURITY_NS_SHA20_SEMAPHORE, 0x55555555);
		BDEV_RD(BCHP_XPT_SECURITY_NS_SHA20_SEMAPHORE);
		BDEV_WR(BCHP_XPT_SECURITY_NS_SHA20_CTRL,
			/*IB=0 PB/DMA=1*/
			(1 << BCHP_XPT_SECURITY_NS_SHA20_CTRL_PB_BAND_SHIFT)
			/*sha1=0 sha224=1 sha256=2*/
			| ((2 & 0x3) <<
			BCHP_XPT_SECURITY_NS_SHA20_CTRL_SHA2_SEL_SHIFT)
			/*DMA|PB|IB band#*/
			| (0 << BCHP_XPT_SECURITY_NS_SHA20_CTRL_BAND_NUM_SHIFT)
			/*enabled*/
			| BCHP_XPT_SECURITY_NS_SHA20_CTRL_SHA2_EN_MASK);
	} else {
		BDEV_RD(BCHP_XPT_SECURITY_NS_SHA21_SEMAPHORE);
		BDEV_WR(BCHP_XPT_SECURITY_NS_SHA21_SEMAPHORE, 0xAAAAAAAA);
		BDEV_RD(BCHP_XPT_SECURITY_NS_SHA21_SEMAPHORE);
		BDEV_WR(BCHP_XPT_SECURITY_NS_SHA21_CTRL,
			/*IB=0 PB/DMA=1*/
			(1 << BCHP_XPT_SECURITY_NS_SHA21_CTRL_PB_BAND_SHIFT)
			/*sha1=0 sha224=1 sha256=2*/
			| ((2 & 0x3) <<
			BCHP_XPT_SECURITY_NS_SHA21_CTRL_SHA2_SEL_SHIFT)
			/*DMA|PB|IB band#*/
			| (0 << BCHP_XPT_SECURITY_NS_SHA21_CTRL_BAND_NUM_SHIFT)
			/*enabled*/
			| BCHP_XPT_SECURITY_NS_SHA21_CTRL_SHA2_EN_MASK);
	}
}

static bool sha_is_ready(int sha_num)
{
	if (sha_num == XPT_SHA21)
		return BDEV_RD_F(XPT_SECURITY_NS_INTR2_0_CPU_STATUS,
				 SHA21_READY);
	else
		return BDEV_RD_F(XPT_SECURITY_NS_INTR2_0_CPU_STATUS,
				 SHA20_READY);
}

static void clear_sha_interrupt_and_control(int sha_num)
{
	if (sha_num == XPT_SHA21) {
		BDEV_WR_F(XPT_SECURITY_NS_INTR2_0_CPU_CLEAR, SHA21_READY, 1);
		BDEV_WR(BCHP_XPT_SECURITY_NS_SHA21_CTRL, 0);
	} else {
		BDEV_WR_F(XPT_SECURITY_NS_INTR2_0_CPU_CLEAR, SHA20_READY, 1);
		BDEV_WR(BCHP_XPT_SECURITY_NS_SHA20_CTRL, 0);
	}
}


static int memdma_wait_for_sha(int sha_num)
{
	int timeout = MAX_SHA_WAIT_US;

	check_memdma_initparams();
	for (;;) {
		if (sha_is_ready(sha_num))
			break;
		if (timeout <= 0) {
			puts("XPTDMA: SHA !RDY");
			return -1;
		}
		timeout -= 10;
		efn->udelay(10);
	}

	/* Clear status and control registers*/
	clear_sha_interrupt_and_control(sha_num);

	return 0;
}

static int memdma_sha(dma_addr_t desc1, int sha_num)
{
	int ret = 0;

	xpt_set_power(1);

	ret = mcpb_soft_init();
	if (ret == 0) {
		set_up_sha_control(sha_num);

		memdma_init_hw(0, PID_CHANNEL_A, XPT_DMA_SHA, false);
		BARRIER();
		memdma_start(desc1, XPT_CHANNEL_A);

		ret = memdma_wait_for_sha(sha_num);
	}

	xpt_set_power(0);

	return ret;
}

static void get_sha_result(int sha_num, uint32_t *sha)
{
	unsigned long reg_base;
	int i;

	if (sha_num == XPT_SHA20)
		reg_base = BCHP_XPT_SECURITY_NS_SHA20_0;
	else
		reg_base = BCHP_XPT_SECURITY_NS_SHA21_0;

	for (i = 0; i < 8; ++i)
		*(sha  + i) = BDEV_RD(reg_base + i * 4);
}

/*
 * xpt_dma_sha - Run hardware accelerated SHA256 and return results in a buffer
 *
 * @addr: the beginning physical address of the memory chunk
 * @size: the size of the physical chunk
 * @sha_num: if 0, XPT_SHA20; otherwise, XPT_SHA21
 * @desc: a mcpb_dma_desc pointer
 * @sha: an uint32_t buffer[8]
 */
void xpt_dma_sha(dma_addr_t addr, uint32_t size, int sha_num,
		 struct mcpb_dma_desc *desc, uint32_t *sha)
{
	struct dma_region region;

	region.addr = addr;
	region.len = size;

	memdma_prepare_descs(desc,
			     (dma_addr_t)(uintptr_t)desc,
			     &region, 1, true);
	clear_d_cache((void *)(uint32_t)desc, sizeof(*desc));
	clear_d_cache((void *)(uint32_t)addr, size);

	if (memdma_sha((dma_addr_t)(uintptr_t)desc, sha_num))
		puts("XPTDMA: SHA ERR!");

	get_sha_result(sha_num, sha);
}
#endif
