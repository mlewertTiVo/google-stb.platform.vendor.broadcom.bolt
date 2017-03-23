/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __XPT_DMA_H__
#define __XPT_DMA_H__

#include <bchp_common.h>
#if CFG_MEMDMA_MCPB
#include <bchp_xpt_memdma_mcpb_ch0.h>
#include <bchp_xpt_memdma_mcpb.h>
#else
#include <bchp_xpt_mcpb_ch0.h>
#include <bchp_xpt_mcpb.h>
#endif

#include <compiler.h>
#include <config.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if CFG_MEMDMA_MCPB
#ifndef BCHP_XPT_MEMDMA_MCPB_CH0_DMA_DATA_CONTROL
#define XPT_DMA_DESC_IS_64BIT /* DMA descriptor is 64-bit register */
#endif
#else
#ifndef BCHP_XPT_MCPB_CH0_DMA_DATA_CONTROL
#define XPT_DMA_DESC_IS_64BIT /* DMA descriptor is 64-bit register */
#endif
#endif

#if CFG_MEMDMA_MCPB
#define PID_CHANNEL_A	1022
#define PID_CHANNEL_B	1023
#else
#define PID_CHANNEL_A	510
#define PID_CHANNEL_B	511
#endif

/* always assume 64 bit physical address */
typedef uint64_t dma_addr_t;

struct dma_region {
	dma_addr_t addr;
	size_t len;
};

struct mcpb_dma_desc {
#ifdef XPT_DMA_DESC_IS_64BIT
	uint32_t buf_hi;
	uint32_t buf_lo;
	uint32_t size;
	uint32_t opts1;
	uint32_t opts2;
	uint32_t pid_channel;
	uint32_t next_hi;
	uint32_t next_offs; /* [0] == 1 for "last descriptor" */
#else
	uint32_t buf_hi;
	uint32_t buf_lo;
	uint32_t next_offs; /* [0] == 1 for "last descriptor" */
	uint32_t size;
	uint32_t opts1;
	uint32_t opts2;
	uint32_t pid_channel;
	uint32_t reserved;
#endif
};

struct wdma_desc {
	uint32_t buf_hi;
	uint32_t buf_lo;
	uint32_t size;
	uint32_t next_offs; /* options are in bits[3:0] (?) */
};

struct memdma_initparams {
	void (*sys_die)(const uint16_t die_code, const char *die_string);
	void (*udelay)(uint32_t time);
	void *(*memset)(void *s, int c, size_t n);
	int (*wait_for_int)(void);
};


/* IMPORTANT: This must always be called before other memdma API calls */
void memdma_init(const struct memdma_initparams *e);

int memdma_prepare_descs(struct mcpb_dma_desc *descs, dma_addr_t descs_pa,
		struct dma_region *regions, int numregions, bool channel_A);

int mcpb_init_desc(struct mcpb_dma_desc *mcpb, struct dma_region *region);

void wdma_init_desc(struct wdma_desc *desc, struct dma_region *region,
		bool int_enable);

int memdma_run(dma_addr_t desc1, dma_addr_t desc2, bool dual_channel);

int mem2mem_dma_run(dma_addr_t mcpb_desc, dma_addr_t wdma_desc,
		int pid_channel, bool int_enable);

void get_hash(uint32_t *hash, bool dual_channel);

size_t mcpb_get_dma_chain_len(struct mcpb_dma_desc *head, int max_descs);


void xpt_dma_sha(dma_addr_t addr, uint32_t size, int sha_num,
		 struct mcpb_dma_desc *desc, uint32_t *sha);

#endif /* __XPT_DMA_H__ */
