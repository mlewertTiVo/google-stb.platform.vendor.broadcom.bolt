/***************************************************************************
 *     Copyright (c) 2012-2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "bolt.h"

#include "lib_types.h"
#include "lib_string.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "env_subr.h"
#include "zimage.h"
#include "devtree.h"

#include "ioctl.h"
#include "devfuncs.h"
#include "device.h"
#include "dev_emmcflash.h"
#include "../dev/dev_genet.h"
#include "ssbl-common.h"
#include "board.h"
#include "boardcfg.h"
#include "macutils.h"
#include "bchp_ebi.h"
#include "bchp_nand.h"
#include "bchp_avs_top_ctrl.h"
#include "bchp_avs_cpu_ctrl.h"
#include "flash.h"
#include "board_init.h"

#include "mmap-dram.h"
#include "net_ebuf.h"
#include "net_ether.h"
#include "net_api.h"
#include "splash-api.h"
#include "otp_status.h"
#include "arch_ops.h"
#include "bsp_config.h"
#include "ssbl-sec.h"


/* ------------------------------------------------------------------------- */
/* Remove me after a grace period */
#define ENET_DEPRECATED_DT_BINDING	1

#if ENET_DEPRECATED_DT_BINDING
#define PHY_TYPE_INT			1
#define PHY_TYPE_EXT_MII		2
#define PHY_TYPE_EXT_RVMII		3
#define PHY_TYPE_EXT_RGMII		4
#define PHY_TYPE_EXT_RGMII_IBS		5
#define PHY_TYPE_EXT_RGMII_NO_ID	6
#define PHY_TYPE_MOCA			7
#endif /* ENET_DEPRECATED_DT_BINDING */


/* ------------------------------------------------------------------------- */
/*      Modify an existing DTB with bolt specifics, may get messy here.      */
/* ------------------------------------------------------------------------- */


#define BREG_PA(block)	(BPHYSADDR(BCHP_##block##_REG_START))
#define BREG_LEN(block)	(BCHP_##block##_REG_END + 4 - BCHP_##block##_REG_START)

/* ------------------------------------------------------------------------- */

/* Some items can only be done once. */
int dt_oneshot;


/* ------------------------------------------------------------------------- */

static void dt_ize_ints(uint32_t *_array, size_t _of_array)
{
	int i, count = _of_array/sizeof(uint32_t);
	for (i = 0; i < count; i++)
		_array[i] = cpu_to_fdt32(_array[i]);
}


/* ------------------------------------------------------------------------- */

static char *boot2str(int mode)
{
	switch (mode) {
	case BOOT_DEV_ERROR:
		return "error"; break;
	case BOOT_FROM_NAND:
		return "nand"; break;
	case BOOT_FROM_NOR:
		return "nor"; break;
	case BOOT_FROM_SPI:
		return "spi"; break;
	case BOOT_FROM_EMMC:
		return "emmc"; break;
	default:
		break;
	}
	return "unknown";
}

/* ------------------------------------------------------------------------- */

static int populate_partition(void *fdt, int sat, const char *partname,
		uint64_t offset, uint64_t len)
{
	uint64_t regs[2];
	char node[80];
	int rc;
	int partition;

	xsprintf(node, "%s@%llx", partname, offset);

	rc = bolt_devtree_addnode_at(fdt, node, sat, &partition);
	if (rc)
		return rc;

	regs[0] = cpu_to_fdt64(offset);
	regs[1] = cpu_to_fdt64(len);

	rc = bolt_devtree_at_node_addprop(fdt, partition, "reg", (char *)regs,
			sizeof(regs));
	return rc;
}

/**
 * Populate the partitions for a flash device
 *
 * @fdt The FDT blob
 * @sat The FDT offset for the node
 */
static int populate_partitions(void *fdt, struct flash_dev *flash, int sat)
{
	int rc, i;

	/* Add 64-bit address/size properties to parent, required for ofpart */
	rc = bolt_dt_addprop_u32(fdt, sat, "#address-cells", 2);
	if (rc)
		return rc;
	rc = bolt_dt_addprop_u32(fdt, sat, "#size-cells", 2);
	if (rc)
		return rc;

	/* Reverse order, since libfdt adds them in reverse order */
	for (i = flash->nparts - 1; i >= 0; i--) {
		struct flash_partition *part = &flash->parts[i];
		rc = populate_partition(fdt, sat, part->name, part->offset,
				part->size);
		if (rc)
			return rc;
	}

	return rc;
}


/* ------------------------------------------------------------------------- */

static int bolt_populate_nand(void *fdt, struct flash_dev *flash)
{
	int rc = 0;
#if CFG_NAND_FLASH /* for now */
	struct board_type *b;
	int sat, at;
	char node[80];

	b = board_thisboard();
	if (!b)
		return FDT_ERR_NOTFOUND;

	/* full rdb address */
	xsprintf(node, DT_RDB_DEVNODE_BASE_PATH"/nand@%08x",
			BREG_PA(NAND));
	rc = bolt_devtree_node_from_path(fdt, node);
	if (rc < 0)
		return rc;

	at = rc;

	/* remove the "status" property to indicate this controller
	 * is operational
	 */
	bolt_devtree_delprop_path(fdt, node, "status");

	/* delete then create subnode */
	xsprintf(node, "nandcs@%d", flash->cs);
	(void)bolt_devtree_delnode_at(fdt, node, at);

	rc = bolt_devtree_addnode_at(fdt, node, at, &sat);
	if (rc)
		return rc;

	rc = bolt_dt_addprop_bool(fdt, sat, "nand-on-flash-bbt");
	if (rc < 0)
		return rc;
	rc = bolt_dt_addprop_u32(fdt, sat, "reg", flash->cs);
	if (rc < 0)
		return rc;
	rc = bolt_dt_addprop_str(fdt, sat, "compatible", "brcm,nandcs");
	if (rc < 0)
		return rc;
	rc = bolt_dt_addprop_u32(fdt, sat, "nand-ecc-strength",
				 flash->ecc_strength);
	if (rc < 0)
		return rc;
	rc = bolt_dt_addprop_u32(fdt, sat, "nand-ecc-step-size",
				 flash->ecc_step);
	if (rc < 0)
		return rc;

	/* create partiton map sub-sub nodes + its prop.
	*/
	rc = populate_partitions(fdt, flash, sat);
#endif /* CFG_NAND_FLASH */
	return rc;
}


/* ------------------------------------------------------------------------- */
/* SPI, nor efi.
*/
static int bolt_populate_spi(void *fdt, struct flash_dev *flash)
{
	int rc = 0;
	int sat, at;
	char node[80];

	/* /params
	*/

	xsprintf(node, DT_RDB_DEVNODE_BASE_PATH"/spi@%x",
			BREG_PA(HIF_MSPI));
	at = bolt_devtree_node_from_path(fdt, node);
	if (at < 0)
		return at;

	/* remove the "status" property to indicate this controller
	 * is operational
	 */
	bolt_devtree_delprop_path(fdt, node, "status");

	/* delete then create subnode
	*/
	xsprintf(node, "m25p80@%d", flash->cs);
	(void)bolt_devtree_delnode_at(fdt, node, at);

	rc = bolt_devtree_addnode_at(fdt, node, at, &sat);
	if (rc)
		return rc;

	rc = bolt_dt_addprop_bool(fdt, sat, "m25p,fast-read");
	if (rc)
		return rc;
	rc = bolt_dt_addprop_bool(fdt, sat, "use-bspi");
	if (rc)
		return rc;
	rc = bolt_dt_addprop_bool(fdt, sat, "spi-cpha");
	if (rc)
		return rc;
	rc = bolt_dt_addprop_bool(fdt, sat, "spi-cpol");
	if (rc)
		return rc;
	rc = bolt_dt_addprop_u32(fdt, sat, "spi-max-frequency", 40000000);
	if (rc)
		return rc;
	rc = bolt_dt_addprop_u32(fdt, sat, "reg", flash->cs);
	if (rc)
		return rc;
	rc = bolt_dt_addprop_str(fdt, sat, "compatible", "m25p80");
	if (rc)
		return rc;

	/* create partiton map sub-sub nodes + its prop.
	*/
	return populate_partitions(fdt, flash, sat);
}


/* ------------------------------------------------------------------------- */
/* CFI NOR
*/
static int bolt_populate_nor(void *fdt, struct flash_dev *flash)
{
	int rc, at;
	char node[80];

	/* params, hard wired for now.
	*/
	uint32_t base;
	uint32_t addr, size_shift;
	/* size cells == address cells == 2 */
	uint32_t reg[4];

	/* /params
	*/

	base = BDEV_RD(BCHP_EBI_CS_BASE_0 + (flash->cs * 0xc));
	size_shift = (base & 0x0f) + 13;
	addr = ALIGN_TO(base, 1 << size_shift);

	/* size cells == address cells == 2 */
	reg[0] = 0;
	reg[1] = addr;
	reg[2] = 0;
	reg[3] = 1 << size_shift;

	xsprintf(node, "flash@%x", addr);

	(void)bolt_devtree_delnode_at(fdt, node, 0);
	/* Place node at root; we don't have an 'EBI' node */
	rc = bolt_devtree_addnode_at(fdt, node, 0, &at);
	if (rc)
		return rc;

	dt_ize_ints(reg, sizeof(reg));

	rc = bolt_dt_addprop_u32(fdt, at, "bank-width", 1);
	if (rc)
		return rc;
	rc = bolt_devtree_at_node_addprop(fdt, at, "reg", reg, sizeof(reg));
	if (rc)
		return rc;
	rc = bolt_dt_addprop_u32(fdt, at, "device-width", 2);
	if (rc)
		return rc;
	rc = bolt_dt_addprop_str(fdt, at, "compatible", "cfi-flash");
	if (rc)
		return rc;

	/* create partiton map sub-sub nodes + its prop.
	*/
	return populate_partitions(fdt, flash, at);
}

static int bolt_populate_flash(void *fdt)
{
	struct flash_dev *flash;
	int ret, i;

	for_each_flash_dev(flash, i) {
		switch (flash->type) {
		case FLASH_TYPE_NAND:
			ret = bolt_populate_nand(fdt, flash);
			break;
		case FLASH_TYPE_SPI:
			ret = bolt_populate_spi(fdt, flash);
			break;
		case FLASH_TYPE_NOR:
			ret = bolt_populate_nor(fdt, flash);
			break;
		default:
			continue;
		}
		if (ret)
			return ret;
	}

	return 0;
}

/* ------------------------------------------------------------------------- */
/* ePAPR 1.1, Section 3.4, Table 3-3
 memory:reg - Consists of an arbitrary number of address and size pairs
 that specify the physical address and size of the memory ranges.
*/

/**
 * Maps the installed DDR from MEMC address space to CPU address space
 *
 * The installed DDR might not be contiguous from CPU address space point
 * of view depending on the memory map of the chip. How the memory map is
 * structured (for MEMC's) is described in dram_mapping_table[] of
 * mmap-dram.h that is generated via the 'mmap' configuration command.
 *
 * @regs buffer to which mapped results (off,len) are to be saved
 * @memc the index of MEMC whose DRAM is mapped to CPU address space
 * @size_mb the size of DRAM to be mapped
 *
 * @returns the number of mappings made
 *
 */
static unsigned int map_ddr_to_cpu(uint64_t *regs, const unsigned int memc,
	uint32_t size_mb)
{
	unsigned int i;
	unsigned int nsegs;
	uint64_t *p;

	if (regs == NULL || size_mb == 0)
		return 0;

	p = regs;
	nsegs = 0;
	for (i = 0; i < NUM_DRAM_MAPPING_ENTRIES; i++) {
		uint64_t offset_mb;
		uint64_t length_mb;

		if (memc != dram_mapping_table[i].which)
			continue;

		offset_mb = dram_mapping_table[i].to_mb;
		if (size_mb >= dram_mapping_table[i].size_mb) {
			length_mb = dram_mapping_table[i].size_mb;
			size_mb -= dram_mapping_table[i].size_mb;
		} else {
			length_mb = size_mb;
			size_mb = 0;
		}
		*p = _MB(offset_mb);
		p++;
		*p = _MB(length_mb);
		p++;
		nsegs++;

		if (size_mb == 0)
			break;
	}

	return nsegs;
}

/**
 * Returns the pointer to a mapped result with the lowest offset address
 *
 * @regs buffer that contains mapped results (off,len)
 * @navail number of valid mapped results in regs
 *
 * @returns pointer to a mapped result with the lowest offset address
 *
 */
static uint64_t *lowest_mapped_address(uint64_t *regs,
	const unsigned int navail)
{
	unsigned int i;
	uint64_t *p;
	uint64_t lowest;

	p = NULL;
	lowest = UINT64_MAX;
	for (i = 0; i < navail; i++) {
		if (regs[i*2+1] == 0)
			continue;

		if (lowest > regs[i*2]) {
			lowest = regs[i*2];
			p = &regs[i*2];
		}
	}

	return p;
}

/**
 * Updates the reg property of the top memory node based on the available DDR
 * of the board
 *
 * The name populate_memory_regions came from the compiler directive name,
 * POPULATE_MEMORY_REGIONS, but what it currently does is only updating
 * the reg property of the memory node at the top of device tree.
 *
 * @fdt The FDT blob
 *
 * @returns BOLT_ERR_DEVNOTFOUND if no board information, BOLT_ERR_INV_PARAM if
 *      the number of entries in the mapping table is smaller than the number
 *      of configured MEMC's, BOLT_ERR_NOMEM if memory allocation failes,
 *      otherwise the return value of bolt_devtree_addprop
 *
 */
static int bolt_populate_memory(void *fdt)
{
	struct board_type *b;
	struct ddr_info *ddr;
	unsigned int num_mapping_entries;
	uint64_t *regs;
	uint64_t *regs_sorted;
	unsigned int i, count, bytes;
	int node;
	int rc;

	b = board_thisboard();
	if (!b)
		return BOLT_ERR_DEVNOTFOUND;

	if (NUM_DRAM_MAPPING_ENTRIES >= b->nddr) {
		num_mapping_entries = NUM_DRAM_MAPPING_ENTRIES;
	} else if (NUM_DRAM_MAPPING_ENTRIES == 0) {
		/* no mapping table */
		num_mapping_entries = b->nddr;
	} else {
		/* mapping, but (NUM_DRAM_MAPPING_ENTRIES < b->nddr) */
		xprintf("%s: #mapping table entries %d, #MEMC %d\n",
			__func__, NUM_DRAM_MAPPING_ENTRIES, b->nddr);
		return BOLT_ERR_INV_PARAM;
	}

	/* 2 == (offset,size) */
	regs = (uint64_t *) KMALLOC(num_mapping_entries*2*sizeof(uint64_t), 0);
	if (regs == NULL)
		return BOLT_ERR_NOMEM;

	count = 0;
	for (i = 0; i < b->nddr; i++) {
		unsigned int nsegs;

		ddr = board_find_ddr(b, i);
		if (!ddr)
			continue;

		if (NUM_DRAM_MAPPING_ENTRIES > 0) {
			nsegs = map_ddr_to_cpu(&regs[count*2], i, ddr->size_mb);
			count += nsegs;
		} else {
			/* there is no mapping table, use ddr->* as is */
			regs[i*2] = _MB(ddr->base_mb);
			regs[i*2+1] = _MB(ddr->size_mb);
			count++;
		}
	}

	/* sort the mapped results in the ascending order of offset */
	regs_sorted = (uint64_t *) KMALLOC(count*2*sizeof(uint64_t), 0);
	if (regs_sorted == NULL) {
		KFREE(regs);
		return BOLT_ERR_NOMEM;
	}
	for (i = 0; i < num_mapping_entries; i++) {
		uint64_t *p = lowest_mapped_address(regs, count);

		if (p == NULL)
			break; /* no more to sort */

		regs_sorted[i*2] = cpu_to_fdt64(*p);
		regs_sorted[i*2+1] = cpu_to_fdt64(*(p+1));
		*(p+1) = 0; /* take out of the list */
	}
	KFREE(regs);
	regs = NULL;

	bytes = count * 2 * sizeof(uint64_t);

	/* /memory may already exist */
	node = bolt_devtree_node_from_path(fdt, "/memory");
	if (node < 0)
		(void)bolt_devtree_addnode_at(fdt, "memory", 0, &node);

	rc = bolt_devtree_at_node_addprop(fdt, node, "reg", regs_sorted, bytes);
	if (rc)
		goto out;

	/* if dt autogen memory is not used, fill in the blanks.
	*/
	rc = bolt_dt_addprop_str(fdt, node, "device_type", "memory");
	if (rc)
		goto out;
	rc = bolt_dt_addprop_u32(fdt, node, "#size-cells", 1);
	if (rc)
		goto out;
	rc = bolt_dt_addprop_u32(fdt, node, "#address-cells", 1);

out:
	KFREE(regs_sorted);
	return rc;
}

/*
 * Populate the memory controller nodes with information about the DDR
 * frequency. Needed by performance monitoring tools.
 */
static int bolt_populate_memory_ctls(void *fdt)
{
	int rc = 0;
#ifdef BCHP_MEMC_DDR_0_REG_START
	struct board_type *b;
	struct ddr_info *ddr;
	char tmpstr[512];
	unsigned int i, base_addr;
	int node;

	b = board_thisboard();
	if (!b)
		return BOLT_ERR_DEVNOTFOUND;

	for (i = 0; i < b->nddr; i++) {
		ddr = board_find_ddr(b, i);
		if (!ddr)
			continue;

		/* Get the spacing between the controllers, but only do
		 * this for multi-MEMC chips
		 */
		base_addr = BCHP_MEMC_DDR_0_REG_START
#if NUM_MEMC > 1
			    + (BCHP_MEMC_DDR_1_REG_START -
			       BCHP_MEMC_DDR_0_REG_START) * ddr->which
#endif
			    ;
		xsprintf(tmpstr,
			 DT_RDB_DEVNODE_BASE_PATH
			 "/memory_controllers/memc@%d/memc-ddr@%08x",
			 ddr->which, BPHYSADDR(base_addr));

		node = bolt_devtree_node_from_path(fdt, tmpstr);
		if (node < 0)
			continue;

		/* clock-frequency is always in Hertz whereas our local storage
		 * for ddr_clock is in Mhz, convert that
		 */
		rc = bolt_dt_addprop_u32(fdt, node, "clock-frequency",
					 ddr->ddr_clock * 1000 * 1000);
	}
#endif
	return rc;
}

/* ------------------------------------------------------------------------- */

#define OPP_TBL_MAX_SZ 16
/*
 * Generate an OPP table to hold all available frequencies that can be
 * generated using the CPU VCO rate + MDIV, without having to enter SAFE_MODE.
 * Listed as pairs of frequencies (KHz) and voltages (mV).
 */
static int gen_operating_points(void *fdt, int cpu_node)
{
	uint32_t cpu_vco_khz = arch_get_cpu_vco_hz() / 1000;
	uint32_t cpu_max_khz = arch_get_cpu_pll_hz() / 1000;
	uint32_t cpu_min_khz = arch_get_cpu_min_safe_hz() / 1000;
	int mdiv;
	uint32_t opp_be[OPP_TBL_MAX_SZ];
	unsigned int i;

	/*
	 * If we're below the SAFE_CLK_MODE threshold, allow for a single fixed
	 * OPP
	 */
	if (cpu_min_khz > cpu_max_khz)
		cpu_min_khz = cpu_max_khz;

	i = 0;
	/*
	 * Fill out OPPs for all MDIV where
	 *  min <= VCO / MDIV <= max
	 */
	for (mdiv = 1; mdiv * cpu_min_khz <= cpu_vco_khz; mdiv++) {
		uint32_t cpu_khz = cpu_vco_khz / mdiv;

		if (cpu_khz > cpu_max_khz)
			continue;

		/* CPU frequency (KHz) */
		opp_be[i++] = cpu_to_fdt32(cpu_khz);
		/* No dynamic voltage control */
		/* coverity[overrun-local] */
		opp_be[i++] = 0;
		if (i >= OPP_TBL_MAX_SZ)
			break;
	}

	if (i == 0) {
		xprintf("WARNING: no OPPs generated\n");
		return 0;
	}

	return bolt_devtree_at_node_addprop(fdt, cpu_node, "operating-points",
					  opp_be, i * sizeof(uint32_t));
}

#define MAX_NUM_CPUS			4
static int bolt_populate_cpus(void *fdt)
{
	struct board_type *b;
	int rc;
	int cpus, cpu;
	char cpu_pname[80];
	int cpus_node, cpu_node;

	cpus = arch_get_num_processors();

	b = board_thisboard();
	if (!b)
		return FDT_ERR_NOTFOUND;

	cpus_node = bolt_devtree_node_from_path(fdt, "/cpus");
	if (cpus_node < 0)
		return cpus_node;

	for (cpu = 0; cpu < MAX_NUM_CPUS; cpu++) {
		xsprintf(cpu_pname, "cpu@%d", cpu);
		cpu_node = bolt_devtree_subnode(fdt, cpu_pname, cpus_node);

		/* Disabled CPU? */
		if (cpu >= cpus) {
			if (cpu_node < 0)
				/* CPU not in device tree */
				break;
			/* remove CPU node */
			rc = bolt_devtree_delnode_at(fdt, cpu_pname, cpus_node);
			if (rc < 0) {
				xprintf("%s: couldn't delete CPU %d\n", __func__, cpu);
				return rc;
			}
			continue;
		} else if (cpu_node < 0) {
			xprintf("%s: couldn't find CPU %d\n", __func__, cpu);
			return cpu_node;
		}

		rc = bolt_dt_addprop_u32(fdt, cpu_node, "clock-frequency",
				arch_get_cpu_freq_hz());
		if (rc < 0) {
			xprintf("%s: couldn't set clock frequency\n", __func__);
			return rc;
		}

		if (cpu == 0) {
			rc = gen_operating_points(fdt, cpu_node);
			if (rc) {
				xprintf("%s: unable to add OPPs\n", __func__);
				return rc;
			}
		}
	}

	return 0;
}

/*
 * We need to fixup the clk-div table for cpu-clk-div if our active CPU clock
 * rate is less than the safe mode threshold. This is currently only the case
 * for chips like BCM73649 (CPU 601MHz); all other chips leave SAFE_CLK_MODE=0.
 */
static int bolt_fixup_cpu_clk_div(void *fdt)
{
	int phandle;
	/* HACK (?): Assume a single-divisor table */
	uint32_t safe_mode_table[] = {
		cpu_to_fdt32(0x10), /* SAFE_CLK_MODE=1, RATIO=divide-by-1 */
		cpu_to_fdt32(1), /* divisor */
	};

	/* CPU freq is above safe-mode threshold? Then nothing to fixup */
	if (arch_get_cpu_pll_hz() >= arch_get_cpu_min_safe_hz())
		return 0;

	phandle = bolt_devtree_phandle_from_alias(fdt, "cpuclkdiv0");
	/* No divider to fix up? */
	if (phandle < 0)
		return 0;

	return bolt_devtree_at_handle_addprop(fdt, phandle, "div-table",
			safe_mode_table, sizeof(safe_mode_table));
}

/* ------------------------------------------------------------------------- */

static void bolt_add_macaddr_prop(void *fdt, int phandle,
				  int atnode, uint8_t *macaddr)
{
	const char *property = "local-mac-address";

	if (phandle)
		bolt_devtree_at_handle_addprop(fdt, phandle,
				property, macaddr, 6);
	else if (atnode)
		(void)bolt_devtree_at_node_addprop(fdt, atnode,
				property, macaddr, 6);
	else
		return;

	macaddr_increment(macaddr, MACADDR_INCREMENT);
}

#if ENET_DEPRECATED_DT_BINDING
static uint32_t phystr2int(const char *phy_type)
{
	if (!strcmp(phy_type, "INT"))
			return PHY_TYPE_INT;
	else if (!strcmp(phy_type, "MII"))
			return PHY_TYPE_EXT_MII;
	else if (!strcmp(phy_type, "RGMII_NO_ID"))
			return PHY_TYPE_EXT_RGMII_NO_ID;
	else if (!strcmp(phy_type, "RGMII_IBS"))
			return PHY_TYPE_EXT_RGMII_IBS;
	else if (!strcmp(phy_type, "RGMII"))
			return PHY_TYPE_EXT_RGMII;
	else if (!strcmp(phy_type, "RVMII"))
			return PHY_TYPE_EXT_RVMII;
	else if (!strcmp(phy_type, "MOCA"))
			return PHY_TYPE_MOCA;

	return 0;
}
#endif /* ENET_DEPRECATED_DT_BINDING */

static char *phystr2std(const char *phy_type)
{
	if (!strcmp(phy_type, "INT"))
		return "internal";
	else if (!strcmp(phy_type, "MII"))
		return "mii";
	else if (!strcmp(phy_type, "RGMII_NO_ID"))
		return "rgmii";
	else if (!strcmp(phy_type, "RGMII_IBS"))
		return "rgmii-ibs";
	else if (!strcmp(phy_type, "RGMII"))
		return "rgmii-txid";
	else if (!strcmp(phy_type, "RVMII"))
		return "rev-mii";
	else if (!strcmp(phy_type, "MOCA"))
		return "moca";

	warn_msg("unhandled phy_type: %s\n", phy_type);
	return NULL;
}


/* For 'probe' or 'noprobe' we rely on the genet eth interface
  bolt uses being up and running.
*/
static int get_phy_id(int instance)
{
	char *s;
	uint32_t id = PHY_ID_NONE;
	char envvar[255];

	xsprintf(envvar, "ETH%d_PHYPROBE_ID", instance);
	s = env_getenv(envvar);
	if (s) {
		id = atoi(s);
		/* 'probe' failed */
		if ((id > PHY_ID_NONE) || (id == 0) || (id == 32))
			id = PHY_ID_NONE;
	}
	return id;
}

static uint32_t bolt_enet_get_phy_id(const char *phy_id, int instance)
{
	uint32_t data;

	if (!strcmp(phy_id, "noprobe") || !strcmp(phy_id, "probe"))
		data = get_phy_id(instance);
	else
		data = atoi(phy_id);

	return data;
}

#if ENET_DEPRECATED_DT_BINDING
static int bolt_genet_write_dt_props(void *fdt, int phandle,
		const enet_params *e,
		const char *phy_type, const char *mdio_mode,
		const char *phy_id, const char *phy_speed)
{
	uint32_t data;

	/* Depending on how we are switching between configurations
	 * we might need to wipe out all the properties we just
	 * added, let's do this to get us in a clean state
	 */
	bolt_devtree_at_handle_delprop(fdt, phandle, "phy-type");
	bolt_devtree_at_handle_delprop(fdt, phandle, "phy-id");
	bolt_devtree_at_handle_delprop(fdt, phandle, "phy-speed");
	bolt_devtree_at_handle_delprop(fdt, phandle, "ethsw-type");

	if (phy_type) {
		data = phystr2int(phy_type);

		bolt_devtree_at_handle_addprop_int(fdt,
			phandle, "phy-type", data);
	} else {
		/*   If no phy type ('-' in script) then mark the node as
		 *   "disabled" so Linux won't see & init it.
		 */
		bolt_devtree_at_handle_addprop(fdt,
			phandle, "status", "disabled", strlen("disabled") + 1);

		return 1;
	}

	/* ----------------- */

	if (phy_id) {
		data = bolt_enet_get_phy_id(phy_id, e->genet);
		bolt_devtree_at_handle_addprop_int(fdt,
			phandle, "phy-id", data);
	}

	/* ----------------- */


	if (phy_speed) {
		data = atoi(phy_speed);

		bolt_devtree_at_handle_addprop_int(fdt,
			phandle, "phy-speed", data);
	}

	if (e->ethsw) {
		data = xtoi(e->ethsw); /* note: HEX number */
		bolt_devtree_at_handle_addprop_int(fdt,
			phandle, "ethsw-type", data);
	}

	return 0;
}
#endif /* ENET_DEPRECATED_DT_BINDING */

static inline unsigned int enet_needs_fixed_link(const char *phy_type,
						  const char *mdio_mode)
{
	/* These type of configurations will imply using a fixed-link property:
	 * MOCA -> no real PHY
	 * MDIO_MODE = "0" -> MDIO bus voluntarily disabled
	 * MDIO_MODE == NULL and phy_type != NULL
	 */
	return ((phy_type && !strcmp(phy_type, "MOCA")) ||
		(mdio_mode && !strcmp(mdio_mode, "0")) ||
		((mdio_mode == NULL) && phy_type != NULL));
}

static void bolt_devtree_remove_phy_nodes(void *fdt, int mdio_node)
{
	unsigned int i;
	char tmpstr[255];

	for (i = 0; i < 32; i++) {
		xsprintf(tmpstr, "phy%d: ethernet-phy@%x", i, i);
		bolt_devtree_delnode_at(fdt, tmpstr, mdio_node);
	}
}

static void bolt_devtree_add_fixed_link(void *fdt, int phandle,
		int instance,
		char *phy_type,
		const char *phy_speed)
{
	int i = -1;
	int fixed_link_property[5];

	/* emulated PHY id */
	fixed_link_property[++i] = cpu_to_fdt32(instance);
	/* duplex settings */
	fixed_link_property[++i] = cpu_to_fdt32(1);
	/* link speed */
	fixed_link_property[++i] = cpu_to_fdt32(atoi(phy_speed));
	/* No pause or asym pause supported */
	fixed_link_property[++i] = 0;
	fixed_link_property[++i] = 0;

	bolt_devtree_at_handle_addprop(fdt, phandle, "fixed-link",
			fixed_link_property, 5 * sizeof(int));
}

static void bolt_devtree_add_phy(void *fdt, int phandle,
		const enet_params *e,
		uint32_t data,
		int mdio_node,
		const char *phy_type,
		const char *phy_speed,
		const char *phy_id)
{
	int rc;
	char tmpstr[255];
	char *default_phystr = "ethernet-phy-ieee802.3-c22";
	char phystr[255];
	size_t l1, l2;
	char *compat;
	int us, phy_phandle;
	unsigned int needs_gphy_clk = 0;
	unsigned int broken_ta = 0;
	int gphy_clk_phandle;

	xsprintf(tmpstr, "phy%d: ethernet-phy@%x", atoi(phy_id), atoi(phy_id));

	rc = bolt_devtree_addnode_at(fdt, tmpstr, mdio_node, &us);
	if (rc) {
		err_msg("failed to create ethernet-phy subnode: %d\n", rc);
		return;
	}

	/* Fetch the "sw_gphy" clock alias, and allocate a phandle for the
	 * aliased node if needed
	 */
	gphy_clk_phandle = bolt_devtree_phandle_from_alias(fdt, "sw_gphy");

	if (!strcmp(phy_type, "INT")) {
		needs_gphy_clk = 1;
		xsprintf(phystr, "%s", "brcm,28nm-gphy");
	} else if (e->ethsw) {
		/* Skip the trailing 0x */
		broken_ta = 11;
		xsprintf(phystr, "brcm,bcm%s", (strchr(e->ethsw, 'x') + 1));
	} else {
		/* Skip the first compatible property string */
		phystr[0] = '\0';
	}

	l1 = strlen(phystr);
	if (l1)
		l1++;
	l2 = strlen(default_phystr) + 1;

	compat = KMALLOC(l1 + l2, 0);
	if (!compat) {
		rc = BOLT_ERR_NOMEM;
		goto out;
	}

	if (l1)
		memcpy(compat, phystr, l1);
	memcpy(&compat[l1], default_phystr, l2);

	rc = bolt_devtree_at_node_addprop(fdt, us, "compatible", compat, l1 + l2);
	if (rc)
		goto out;
	rc = bolt_dt_addprop_u32(fdt, us, "reg", data);
	if (rc)
		goto out;
	rc = bolt_dt_addprop_u32(fdt, us, "max-speed", atoi(phy_speed));
	if (rc)
		goto out;
	rc = bolt_dt_addprop_str(fdt, us, "device_type", "ethernet-phy");
	if (rc)
		goto out;
	if (needs_gphy_clk && gphy_clk_phandle >= 0) {
		rc = bolt_dt_addprop_u32(fdt, us, "clocks", gphy_clk_phandle);
		if (rc)
			goto out;
		rc = bolt_dt_addprop_str(fdt, us, "clock-names", "sw_gphy");
		if (rc)
			goto out;
	}
	if (broken_ta) {
		rc = bolt_dt_addprop_bool(fdt, us, "broken-turn-around");
		if (rc)
			goto out;
	}

	/* Create the phandle for the node to be valid */
	phy_phandle = bolt_devtree_set_phandle(fdt, us);
	bolt_devtree_at_handle_addprop_int(fdt, phandle, "phy-handle",
					   phy_phandle);

out:
	KFREE(compat);
}

static int bolt_populate_moca(void *fdt, uint8_t *macaddr)
{
#ifdef BCHP_DATA_MEM_REG_START /* MoCA_DATA_RAM->DATA_MEM */
	char tmpstr[255];
	int moca_node;
	const moca_params *m;
	int phandle;
	uint32_t data, data1, avs_disabled;

	m = board_moca();
	if (!m)
		return BOLT_ERR_INV_PARAM;

	if (!m->base && !m->moca_band && !m->enet_node)
		return BOLT_ERR_INV_PARAM;

	xsprintf(tmpstr,
		DT_RDB_DEVNODE_BASE_PATH"/bmoca@%08x", BPHYSADDR(m->base));

	/* Only do if MoCA node exists.
	*/
	moca_node = bolt_devtree_node_from_path(fdt, tmpstr);
	if (moca_node < 0)
		return moca_node;

	if (m->moca_band)
		(void)bolt_dt_addprop_str(fdt, moca_node, "rf-band",
					  m->moca_band);

	phandle = bolt_devtree_phandle_from_alias(fdt, m->enet_node);
	if (phandle < 0)
		return phandle;

	bolt_devtree_at_node_delprop(fdt, moca_node, "enet-id");
	bolt_dt_addprop_u32(fdt, moca_node, "enet-id", phandle);

	/* A network instance is tied to MoCA so its DT config
	 * code should have set up its MAC address before directly
	 * calling us here for us to match or not (MAC address
	 * tracking post increments) as the case may be.
	 */
	if (BSP_CFG_MOCA_MAC_EQ_ETH_MAC)
		macaddr_decrement(macaddr, MACADDR_INCREMENT);

	bolt_add_macaddr_prop(fdt, 0, moca_node, macaddr);

	data = BDEV_RD(BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID) + 0xa0;
	bolt_dt_addprop_u32(fdt, moca_node, "chip-id", data);

	/* Set the MoCA AVS RMON properties by reading back values
          calibrated by fsbl */
#ifdef BCHP_AVS_TOP_CTRL_OTP_STATUS_STB
	avs_disabled = BDEV_RD(BCHP_AVS_TOP_CTRL_OTP_STATUS_STB) &
		BCHP_AVS_TOP_CTRL_OTP_STATUS_STB_OTP_AVS_DISABLE_MASK;
#else
	avs_disabled = BDEV_RD(BCHP_AVS_TOP_CTRL_OTP_STATUS) &
		BCHP_AVS_TOP_CTRL_OTP_STATUS_OTP_AVS_DISABLE_MASK;
#endif

	if (!avs_disabled) {
#if defined(BCHP_AVS_TOP_CTRL_STB_RMON_HZ)
		data = BDEV_RD(BCHP_AVS_TOP_CTRL_STB_RMON_HZ);
		data1 = BDEV_RD(BCHP_AVS_TOP_CTRL_STB_RMON_VT);
#elif defined(BCHP_AVS_TOP_CTRL_RMON_HZ)
		data = BDEV_RD(BCHP_AVS_TOP_CTRL_RMON_HZ);
		data1 = BDEV_RD(BCHP_AVS_TOP_CTRL_RMON_VT);
#else
		data = BDEV_RD(BCHP_AVS_CPU_CTRL_SW_SPARE0);
		data1 = BDEV_RD(BCHP_AVS_CPU_CTRL_SW_SPARE1);
#endif
		bolt_dt_addprop_u32(fdt, moca_node, "brcm,avs-rmon-hz", data);
		bolt_dt_addprop_u32(fdt, moca_node, "brcm,avs-rmon-vt", data1);
	}
#endif /* MoCA_DATA_RAM */
	return 0;
}

static int bolt_populate_genet(void *fdt, int enet_instance,
		int phandle, char *m, uint8_t *macaddr)
{
	uint32_t data;
	const enet_params *e;
	char *env;
	char envvar[255];
	int mdio_node;
	char *phy_type, *mdio_mode, *phy_speed, *phy_id;
	char tmpstr[255];

	/* Skip over disabled instances and mark them as such in DT */
	e = board_enet(enet_instance);
	if (!e) {
		bolt_devtree_at_handle_addprop(fdt, phandle,
			"status", "disabled", strlen("disabled") + 1);
		return 1;
	}

	/* Assign MAC address to the this adapter */
	if (m)
		bolt_add_macaddr_prop(fdt, phandle, 0, macaddr);

	/* Fetch the environment (updated) or board default configuration
	 * once and for all
	 */
	phy_type = e->phy_type;

	xsprintf(envvar, "ETH%d_PHY", e->genet);
	env = env_getenv(envvar);
	if (env)
		phy_type = env;

#ifdef OTP_OPTION_GPHY_DISABLE
	/* Do this check here instead of in bolt_otp_unpopulate() since we have
	 * the full-context of the Ethernet interface settings
	 */
	if (OTP_OPTION_GPHY_DISABLE() && !strcmp(phy_type, "INT")) {
		bolt_devtree_at_handle_addprop(fdt, phandle,
			"status", "disabled", strlen("disabled") + 1);
		return 1;
	}
#endif

	phy_id = e->phy_id;

	xsprintf(envvar, "ETH%d_PHYADDR", e->genet);
	env = env_getenv(envvar);
	if (env)
		phy_id = env;

	phy_speed = e->phy_speed;

	xsprintf(envvar, "ETH%d_SPEED", e->genet);
	env = env_getenv(envvar);
	if (env)
		phy_speed = env;

	mdio_mode = e->mdio_mode;
	xsprintf(envvar, "ETH%d_MDIO_MODE", e->genet);
	env = env_getenv(envvar);
	if (env)
		mdio_mode = env;

#if ENET_DEPRECATED_DT_BINDING
	if (bolt_genet_write_dt_props(fdt, phandle, e, phy_type,
				mdio_mode, phy_id, phy_speed))
		return 1;
#endif
	/* ----------------- */

	/* Depending on how we are switching between configurations
	 * we might need to wipe out all the properties we just
	 * added, let's do this to get us in a clean state
	 */
	bolt_devtree_at_handle_delprop(fdt, phandle, "fixed-link");
	bolt_devtree_at_handle_delprop(fdt, phandle, "phy-handle");
	bolt_devtree_at_handle_delprop(fdt, phandle, "phy-mode");
	bolt_devtree_at_handle_delprop(fdt, phandle, "status");

	xsprintf(tmpstr, DT_RDB_DEVNODE_BASE_PATH"/ethernet@%x/mdio@%x",
		BPHYSADDR(e->base), e->umac_base - e->base + 0x614);
	mdio_node = bolt_devtree_node_from_path(fdt, tmpstr);
	if (mdio_node < 0) {
		warn_msg("GENE%d: cannot find MDIO node", e->genet);
		return 1;
	}

	bolt_devtree_remove_phy_nodes(fdt, mdio_node);

	/* Get the PHY ID */
	data = bolt_enet_get_phy_id(phy_id, e->genet);

	if (enet_needs_fixed_link(phy_type, mdio_mode)) {
		bolt_devtree_add_fixed_link(fdt, phandle,
			e->genet, phy_type, phy_speed);
	} else if (phy_type != NULL && data != PHY_ID_NONE) {
		/* This is the switch pseudo-PHY address which won't respond
		 * properly on the MDIO bus for the MII_ID[1] & MII_ID[2]
		 * registers use address 0 which is the correct one instead
		 */
		if (e->ethsw && atoi(phy_id) == 0x1e) {
			phy_id = "0";
			data = 0;
		}
		bolt_devtree_add_phy(fdt, phandle, e,
			data, mdio_node, phy_type, phy_speed, phy_id);
	} else {
		/* If no phy type ('-' in script) then explicitely mark the
		 * node as disabled
		 */
		bolt_devtree_at_handle_addprop(fdt, phandle,
			"status", "disabled", strlen("disabled") + 1);
		return 1;
	}

	phy_type = phystr2std(phy_type);

	bolt_devtree_at_handle_addprop(fdt, phandle,
		"phy-mode", phy_type, strlen(phy_type) + 1);

	if (!strcmp(phy_type, "moca")) {
		if (bolt_populate_moca(fdt, macaddr))
			return 1;
	}

	return 0;
}

static int bolt_populate_systemport(void *fdt, int enet_instance,
		int phandle, char *m, uint8_t *macaddr)
{
	/* Assign MAC address to the this adapter */
	if (m)
		bolt_add_macaddr_prop(fdt, phandle, 0, macaddr);

	return 0;
}

static int bolt_populate_eth_switch(void *fdt, uint8_t *macaddr)
{
#if defined(BCHP_SWITCH_CORE_REG_START) && (NUM_SWITCH_PORTS > 0)
	uint32_t data;
	const enet_params *e;
	char tmpstr[255];
	int phandle;
	char *phy_type, *phy_id;
	int mdio_node;
	unsigned int port;

	xsprintf(tmpstr, DT_RDB_DEVNODE_BASE_PATH"/switch_top@%x/mdio@%x",
		BPHYSADDR(BCHP_SWITCH_CORE_REG_START),
		BCHP_SWITCH_MDIO_REG_START - BCHP_SWITCH_CORE_REG_START);

	mdio_node = bolt_devtree_node_from_path(fdt, tmpstr);
	if (mdio_node < 0) {
		xprintf("cannot find %s\n", tmpstr);
		return mdio_node;
	}
	bolt_devtree_remove_phy_nodes(fdt, mdio_node);

	for (port = 0; port < NUM_SWITCH_PORTS; port++) {
		xsprintf(tmpstr, "switch_port%d", port);
		phandle = bolt_devtree_phandle_from_alias(fdt, tmpstr);
		if (phandle < 0)
			continue;

		e = board_enet(port);
		if (!e) {
			/* Last port is the CPU port, never mark it as disabled */
			if (port != NUM_SWITCH_PORTS - 1)
				bolt_devtree_at_handle_addprop(fdt, phandle,
					"status", "disabled", strlen("disabled") + 1);
			continue;
		}

		bolt_devtree_at_handle_delprop(fdt, phandle, "fixed-link");
		bolt_devtree_at_handle_delprop(fdt, phandle, "phy-handle");
		bolt_devtree_at_handle_delprop(fdt, phandle, "phy-mode");

		/* The MDIO node offset will change as we start patching, so we
		 * need to re-fetch its offset before each switch port
		 modification */

		xsprintf(tmpstr,
			DT_RDB_DEVNODE_BASE_PATH"/switch_top@%x/mdio@%x",
			BPHYSADDR(BCHP_SWITCH_CORE_REG_START),
			BCHP_SWITCH_MDIO_REG_START -
			BCHP_SWITCH_CORE_REG_START);

		mdio_node = bolt_devtree_node_from_path(fdt, tmpstr);
		if (mdio_node < 0) {
			xprintf("cannot find %s\n", tmpstr);
			return mdio_node;
		}

		phy_id = e->phy_id;
		if (!phy_id)
			phy_id = "0";

		data = bolt_enet_get_phy_id(phy_id, e->switch_port);
		if (enet_needs_fixed_link(e->phy_type, e->mdio_mode))
			bolt_devtree_add_fixed_link(fdt,
					phandle, e->switch_port,
					e->phy_type, e->phy_speed);

		else if (e->phy_type && data != PHY_ID_NONE) {
			if (e->ethsw && atoi(phy_id) == 0x1e) {
				phy_id = "0";
				data = 0;
			}

			bolt_devtree_add_phy(fdt, phandle, e, data, mdio_node,
					e->phy_type, e->phy_speed, phy_id);
		}

		phy_type = phystr2std(e->phy_type);
		bolt_devtree_at_handle_addprop(fdt, phandle,
				"phy-mode", phy_type, strlen(phy_type) + 1);

		if (!strcmp(phy_type, "moca")) {
			if (bolt_populate_moca(fdt, macaddr))
				return 1;
		}
	}
#endif
	return 0;
}


#if defined BCHP_RF4CE_CPU_PROG0_MEM_REG_START /* Start of rf4ce */
static void bolt_add_eui64_prop(void *fdt, int atnode, uint8_t *macaddr)
{
	char eui64[8];
	const char *propname = "local-extended-address";

	/* RFC2373: "Links or Nodes with IEEE 802 48 bit MAC's"
	 * e.g. MAC-48: 00 10 18       b5 c3 78 to...
	 *      EUI-64: 02 10 18 ff fe b5 c3 78
	 */
	eui64[0] = macaddr[0];
	eui64[1] = macaddr[1];
	eui64[2] = macaddr[2];
	eui64[3] = 0xff; /* 0xfffe reserved for encapsulation of EUI/MAC-48 */
	eui64[4] = 0xfe;
	eui64[5] = macaddr[3];
	eui64[6] = macaddr[4];
	eui64[7] = macaddr[5];

	eui64[0] ^= 2; /* Invert universal/local bit */

	(void)bolt_devtree_at_node_addprop(fdt, atnode, propname, eui64, 8);
}

static void bolt_populate_rf4ce(void *fdt, uint8_t *macaddr)
{
	char nodestr[255];
	int rc = BOLT_OK, sub = 0, node;

	xsprintf(nodestr, DT_RDB_DEVNODE_BASE_PATH"/rf4ce@%08x",
			BPHYSADDR(BCHP_RF4CE_CPU_PROG0_MEM_REG_START));

	node = bolt_devtree_node_from_path(fdt, nodestr);
	if (node < 0)
		return;

	/* It exists, so enable it. */
	(void)bolt_dt_addprop_str(fdt, node, "status", "okay");

	/* NB: 'sub' is a subnode so should never be == 0 (base node),
	 * and bolt_add_macaddr_prop() post-increments 'macaddr' so
	 * the EUI-64 will encapsulate the correct MAC-48 value.
	 */
	rc = bolt_devtree_addnode_at(fdt, "mac-rf4ce", node, &sub);
	if (!rc && (sub > 0)) {
		bolt_add_eui64_prop(fdt, sub, macaddr);
		bolt_add_macaddr_prop(fdt, 0, sub, macaddr);
	}

	rc = bolt_devtree_addnode_at(fdt, "mac-zbpro", node, &sub);
	if (!rc && (sub > 0)) {
		bolt_add_eui64_prop(fdt, sub, macaddr);
		bolt_add_macaddr_prop(fdt, 0, sub, macaddr);
	}
}
#else /* We have no rf4ce peripheral */
static void bolt_populate_rf4ce(void *fdt, uint8_t *macaddr)
{
}
#endif /* End of rf4ce */


static int bolt_populate_gpio_key(void *fdt)
{
	int rc = 0;
	int property[3];
	int gpio_key, phandle, gpio_key_btn, root_node;
	const char *keys_node = "gpio_keys_polled";
	char buffer[20];

	const gpio_key_params *m = board_gpio_keys();

	if (!m || !m->name || !m->gpio)
		return rc;

	root_node = bolt_devtree_node_from_path(fdt, DT_RDB_DEVNODE_BASE_PATH);
	if (root_node < 0)
		return root_node;

	(void)bolt_devtree_delnode_at(fdt, keys_node, root_node);

	rc = bolt_devtree_addnode_at(fdt, keys_node, root_node,
				&gpio_key);
	if (rc < 0)
		return rc;

	rc = bolt_dt_addprop_str(fdt, gpio_key,
				"compatible", "gpio-keys-polled");
	if (rc)
		return rc;

	rc = bolt_dt_addprop_u32(fdt, gpio_key, "poll-interval", 100);
	if (rc)
		return rc;

	rc = bolt_dt_addprop_u32(fdt, gpio_key, "#address-cells", 2);
	if (rc)
		return rc;

	rc = bolt_dt_addprop_u32(fdt, gpio_key, "#size-cells", 0);
	if (rc)
		return rc;

	/* generate sub node for each button */

	while (m->name && m->gpio) {
		rc = bolt_devtree_addnode_at(fdt, m->name, gpio_key,
					&gpio_key_btn);
		if (rc < 0)
			return rc;

		rc = bolt_dt_addprop_u32(fdt, gpio_key_btn,
					"linux,code", m->code);
		if (rc)
			return rc;

		phandle = bolt_devtree_phandle_from_alias(fdt, m->gpio);

		property[0] = cpu_to_fdt32(phandle);
		property[1] = cpu_to_fdt32(m->pin);
		/* configure the pin to active low */
		property[2] = cpu_to_fdt32(1);

		rc = bolt_devtree_at_node_addprop(fdt, gpio_key_btn,
					"gpios", property, 3 * sizeof(int));
		if (rc)
			return rc;

		xsprintf(buffer, "%s %d", m->gpio, m->pin);
		buffer[19] = '\0';
		env_setenv(m->name, buffer, ENV_FLG_BUILTIN | ENV_FLG_READONLY);

		m++;
	}

	return 0;
}

static int bolt_populate_enet(void *fdt)
{
	int rc = 0;
	dt_ops_s *ops = board_dt_ops();
	char *m = env_getenv(MACADDR_ENVSTR);
	uint8_t macaddr[6];
	int enet_instance, phandle, c, count = 0;
	char tmpstr[255];
	unsigned int is_systemport = 0;
	int compat_len;

	if (m && MAC_str2bin(m, macaddr)) {
		xprintf("warning: %s contains illegal MAC address '%s'\n",
			MACADDR_ENVSTR, m);
		m = NULL;
	}
	count = bolt_devtree_count_aliases(fdt, "eth");

	for (c = 0; c < count; c++) {
		enet_instance = c;
		xsprintf(tmpstr, "eth%d", enet_instance);
		phandle = bolt_devtree_phandle_from_alias(fdt, tmpstr);
		if (phandle < 0)
			continue;

		rc = bolt_devtree_at_handle_getprop(fdt, phandle, "compatible",
						    tmpstr, sizeof(tmpstr), &compat_len);
		if (rc < 0) {
			err_msg("failed to get \"compatible\" property for eth%d\n",
				enet_instance);
			return rc;
		}

		if (strstr(tmpstr, "genet")) {
			rc = bolt_populate_genet(fdt, enet_instance,
				phandle, m, macaddr);
		} else if (strstr(tmpstr, "systemport")) {
			is_systemport = 1;
			rc = bolt_populate_systemport(fdt, enet_instance,
				phandle, m, macaddr);
		} else {
			warn_msg("unsupported Ethernet adapter: %s\n", tmpstr);
			return 1;
		}

		if (rc)
			continue;
	}

	if (is_systemport)
		bolt_populate_eth_switch(fdt, macaddr);

	bolt_populate_rf4ce(fdt, macaddr);

	/* board specific custom MAC populate. Note that the other
	 * DT_OP_XXX are handled in a general manner in the
	 * function bolt_board_specific_mods().
	 */
	for (; ops && ops->path; ops++) {
		int node;
		if (ops->op != DT_OP_MAC)
			continue;
		sprintf(tmpstr, "%s/%s", ops->path, ops->node);
		/* If node doesn't exist, create it */
		node = bolt_devtree_node_from_path(fdt, tmpstr);
		if (node < 0) {
			int parent = bolt_devtree_node_from_path(fdt,
								 ops->path);
			if (parent >= 0)
				rc = bolt_devtree_addnode_at(
					fdt, ops->node, parent, &node);
			else
				rc = 1;
			if (rc)
				xprintf("warning: did not add macaddr to %s\n",
					tmpstr);
		}
		if (node >= 0)
			bolt_add_macaddr_prop(fdt, 0, node, macaddr);
	}

	return 0;
}


/* ------------------------------------------------------------------------- */

#if CFG_SPLASH
static int bolt_modify_cma(void *fdt, uint32_t mtop, uint32_t mlow)
{
	int rc = 0;
	int proplen, subnode, memory, rlen, nlen;
	const char *name, *pname, *region = "region";
	const struct fdt_property *prop;
	uint32_t cma_top, cma_low, cma_size;
	int dep, depth;
	uint32_t data[2];

	rc = fdt_subnode_offset(fdt, 0 /* root */, "memory");
	if (rc < 0)
		return libfdt_error(rc);

	memory = rc;

	dep = fdt_node_depth(fdt, memory);
	if (dep < 0)
		return libfdt_error(dep);

	rlen = strlen(region);

	rc = fdt_subnode_offset_namelen(fdt, memory, region, rlen);
	do { /* find the right /memory/region@ */

		if (rc < 0)
			break;

		subnode = rc;
		rc = 0; /* reset */

		name = fdt_get_name(fdt, subnode, NULL);

		/* '/memory' node might have other subnodes
		 so we check the subnode name.
		*/
		do {
			if (!name)
				break;

			nlen = strlen(region);
			if (nlen < rlen)
				break;

			if (memcmp(name, region, rlen))
				break;

			prop = fdt_get_property(fdt, subnode, "reg", &proplen);
			if (proplen < (int)(sizeof(uint32_t) * 2))
				break;

			cma_low  = DT_PROP_DATA_TO_U32(prop->data, 0);
			cma_size = DT_PROP_DATA_TO_U32(prop->data,
						       sizeof(uint32_t));

			cma_top = cma_low + cma_size;

			/* Splash memory is from the top of each ddr & grows
			downward. At the moment we presume the CMA (NEXUS)
			region has the same properties & we can carve out
			the top from it.
			Splitting regions might lead to unexpected NEXUS
			behaviour. NB: mtop == cma_top in most cases &
			it all has to fall within the span of a single
			CMA region.
			*/
			if ((cma_top >= mlow) && (cma_top <= mtop)) {
				uint32_t delta;

				pname = fdt_string(fdt,
						fdt32_to_cpu(prop->nameoff));

				delta = ALIGN_UP_TO(cma_top - mlow,
						CFG_CMA_DEF_ALIGN);

				if (delta >= cma_size) {
					xprintf("%s: region %s, "
						"is not sufficiently "
						"sized for splash memory "
						"adjustment.\n",
						__func__, name);

					rc = BOLT_ERR_INV_PARAM;
					break;
				}

				cma_size -= delta;

				data[0] = cpu_to_fdt32(cma_low); /* unchanged */
				data[1] = cpu_to_fdt32(cma_size); /* lower */

				rc = bolt_devtree_at_node_addprop(fdt, subnode, pname,
						data, sizeof(data));
			}
		} while (0);

		/* check possible addprop failure */
		if (rc < 0)
			break;

		rc = fdt_next_node(fdt, subnode, NULL);
		if (rc < 0)
			break;

		depth = fdt_node_depth(fdt, subnode);

	} while (dep < depth);

	return (rc) ? libfdt_error(rc) : rc;
}
#endif /* CFG_SPLASH */


static int bolt_populate_splash(void __maybe_unused *fdt)
{
	int rc = 0;
#if CFG_SPLASH
	int i;
	uint32_t memtop, memlow;
	uint64_t mtop, mlow;

	/* Its either a oneshot, or go looping thru the values and
	checking if we already have an entry. Might possibly need
	this more bloaty solution later on if the user has
	overlapping ranges with us.
	*/
	if (dt_oneshot)
		return 0;

	for (i = 0; i < MAX_DDR; i++) {
		rc = splash_glue_getmem(i, &memtop, &memlow);
		if (!rc && (memtop != memlow)) {
			mtop = (uint64_t)memtop; /* should be top of a ddr */
			/* Since the splash heap grows down, we align
			  to the next lowest 4k page for Linux.
			*/
			mlow = ALIGN_TO((uint64_t)memlow, CFG_CMA_DEF_ALIGN);

			(void)bolt_devtree_add_memreserve(fdt,
						mlow, mtop - mlow);
			(void)bolt_modify_cma(fdt, mtop, mlow);
		}
	}
#endif /* CFG_SPLASH */
	return rc;
}


/* ------------------------------------------------------------------------- */

#define rm_nodes(f, s, p) \
	do { \
		int __rc; \
		do { \
			__rc = bolt_devtree_delnode_at(f, s, p); \
		} while (!__rc); /* err = no more nodes to kill */ \
	} while (0)

static void bolt_otp_unpopulate(void *fdt)
{
	int p_rdb = 0, p_root = 0;
	char __maybe_unused text[128];

	p_rdb = bolt_devtree_node_from_path(fdt, DT_RDB_DEVNODE_BASE_PATH);
	if (p_rdb < 0) {
		xprintf("OTP unpopulate: no parent node found!\n");
		return;
	}

	p_root = bolt_devtree_node_from_path(fdt, "/");
	if (p_root < 0) {
		xprintf("OTP unpopulate: no root node found!\n");
		return;
	}

#ifdef OTP_OPTION_MOCA_DISABLE
		if (OTP_OPTION_MOCA_DISABLE())
			rm_nodes(fdt, "bmoca", p_rdb);
#endif

#if	defined(OTP_OPTION_SATA0_DISABLE) ||\
	defined(OTP_OPTION_SATA1_DISABLE) ||\
	defined(OTP_OPTION_SATA_DISABLE)
#if defined(OTP_OPTION_SATA0_DISABLE) && defined(OTP_OPTION_SATA1_DISABLE)
		if (OTP_OPTION_SATA0_DISABLE() && OTP_OPTION_SATA1_DISABLE())
#elif defined(OTP_OPTION_SATA_DISABLE)
		if (OTP_OPTION_SATA_DISABLE())
#else /* Catch any deviation from the current RDB naming conventions. */
#error OTP_OPTION_SATA[0|1]_DISABLE without OTP_OPTION_SATA[0|1]_DISABLE
#endif
		{
			rm_nodes(fdt, "sata", p_rdb);
			rm_nodes(fdt, "sata_phy", p_rdb);
		}
#endif /* OTP_OPTION_SATA */

#ifdef OTP_OPTION_PCIE_DISABLE
		if (OTP_OPTION_PCIE_DISABLE())
			rm_nodes(fdt, "pcie", p_root);
#endif

#if defined(BCHP_PCIE_0_RC_CFG_TYPE1_REG_START) && \
	defined(OTP_OPTION_PCIE0_DISABLE)
		if (OTP_OPTION_PCIE0_DISABLE()) {
			xsprintf(text, "pcie@%08x", BCHP_PHYSICAL_OFFSET +
				BCHP_PCIE_0_RC_CFG_TYPE1_REG_START);
			rm_nodes(fdt, text, p_root);
		}
#endif

#if defined(BCHP_PCIE_1_RC_CFG_TYPE1_REG_START) && \
	defined(OTP_OPTION_PCIE1_DISABLE)
		if (OTP_OPTION_PCIE1_DISABLE()) {
			xsprintf(text, "pcie@%08x", BCHP_PHYSICAL_OFFSET +
				BCHP_PCIE_1_RC_CFG_TYPE1_REG_START);
			rm_nodes(fdt, text, p_root);
		}
#endif

	/*	For usb/devtree work we need to consider:

			OTP_OPTION_USB[0..1]_P[0..1]_DISABLE (7366a0)
			OTP_OPTION_USB_P[0..1]_DISABLE (7145a0)
	*/
}

/* ------------------------------------------------------------------------- */

/* Build time defined board or common chip specific devicetree items
  to manipulate for the current board configuration
*/
void bolt_board_specific_mods(void *fdt)
{
	dt_ops_s *ops = board_dt_ops();
	dt_ops_s *cull = ops;
	power_det_e powerdet = board_powermode();
	int __maybe_unused err = 0, parent, child;

	if (!ops) {
		if (CFG_CMD_LEVEL >= 5)
			xprintf("no hard wired devicetree mods for this board\n");
		return;
	}

	for ( ; ops->path != NULL; ops++) { /* path is REQUIRED */
		switch (ops->op) {
		case DT_OP_MAC:
			/* handled in bolt_populate_enet() */
		case DT_OP_CULL:
			/* handled last; skip for now */
		case DT_OP_NONE:
			continue;
		default:
			/* Continue, to process all other ops */
			break;
		}

		parent = bolt_devtree_node_from_path(fdt, ops->path);
		if (parent < 0) {
			xprintf("info: %s() path not found! %s\n",
					__func__, ops->path);
			err = parent;
			break;
		}
		if (!ops->node) {
			xprintf("%s: op is missing node name: %s\n",
					__func__, ops->path);
			continue;
		}
		child = bolt_devtree_subnode(fdt, ops->node, parent);
		if (child < 0 && ops->op != DT_OP_ADD_NODE) {
			xprintf("%s: could not find node %s\n",
					__func__, ops->node);
			err = child;
			break;
		}

		switch (ops->op) {
		case DT_OP_COMPILE_PROP:
		{
			unsigned char buf[128];
			int n = bolt_devtree_compile_prop_value(
				fdt, ops->svalue, buf, sizeof(buf));
			if (n < 0) {
				err = n;
				break;
			}
			/* The above information may have
			 * altered the positioning of the device tree,
			 * so recalculate the parent and child
			 * offset */
			parent = bolt_devtree_node_from_path(fdt, ops->path);
			if (parent < 0) {
				xprintf("%s: parent node not found!\n", __func__);
				err = parent;
				break;
			}
			child = bolt_devtree_subnode(fdt, ops->node, parent);
			if (child < 0) {
				xprintf("%s: child node not found!\n", __func__);
				err = child;
				break;
			}
			err = bolt_devtree_at_node_addprop(
				fdt, child, ops->prop, buf, n);
			if (err) {
				xprintf("%s: addprop failed: node=%s prop=%s!\n",
					__func__, ops->node, ops->prop);
				break;
			}
		} break;

		case DT_OP_ADD_NODE:
		{
			int at = bolt_devtree_node_from_path(fdt, ops->path);
			int node;

			if (at < 0)
				err = at;
			else
				err = bolt_devtree_addnode_at(fdt, ops->node,
							      at, &node);
			if (err)
				xprintf("%s: failed to add node '%s'! (err=%d)\n",
					__func__, ops->node, err);
		} break;

		case DT_OP_SPROP:
			err = bolt_dt_addprop_str(fdt, child, ops->prop,
					ops->svalue);
			if (CFG_CMD_LEVEL >= 5)
				xprintf("%s() add/mod %s %s %s, err=%d\n",
					__func__, ops->path, ops->node, ops->prop, err);
			break;


		case DT_OP_VPROP:
			err = bolt_devtree_at_node_addprop(fdt, child, ops->prop,
				   ops->vvalue, ops->ivalue * sizeof(uint32_t));
			break;

		case DT_OP_IPROP:
			err = bolt_dt_addprop_u32(fdt, child, ops->prop,
						  ops->ivalue);
			if (CFG_CMD_LEVEL >= 5)
				xprintf("%s() add/mod %s %s %s, err=%d\n",
					__FUNCTION__, ops->path, ops->node,
					ops->prop, err);
			break;

		case DT_OP_BPROP:
			if (ops->ivalue)
				err = bolt_dt_addprop_bool(fdt, child,
							   ops->prop);
			else
				err = bolt_devtree_at_node_delprop(fdt, child,
						ops->prop);
			if (CFG_CMD_LEVEL >= 5)
				xprintf("%s() add/mod/del %s %s %s, err=%d\n",
				__func__, ops->path, ops->node, ops->prop, err);
			break;

		default:
			xprintf("%s: unhandled DT_OP: %d\n", __func__, ops->op);
			break;
		}
	}
	/* the last thing we do is rm nodes as any previous mods might leave
	a hanging property in a node that should have been 100% removed.
	*/
	while (cull->path != NULL) { /* path is REQUIRED */
		if ((cull->op == DT_OP_CULL) &&
				((cull->powerdet == POWER_DET_NA) ||
				(cull->powerdet == powerdet))) {

			parent = bolt_devtree_node_from_path(fdt, cull->path);
			if (parent >= 0) {
				err = bolt_devtree_delnode_at(fdt,
						cull->node, parent);
				if (CFG_CMD_LEVEL >= 5)
					xprintf("%s() removed %s %s, err=%d\n",
						__func__, cull->path, cull->node, err);
			} else {
				/* we could have repeated runs of 'dt bolt' or
				duplicated inherited things to nuke, so just
				continue on */
				if (CFG_CMD_LEVEL >= 3)
					xprintf("info: %s() path not found! %s\n",
						__func__, cull->path);
			}
		}
		cull++;
	}
}

static void bolt_populate_model_and_compatible(void *fdt)
{
	const struct fdt_property *prop;
	const char *name;
	char *compat;
	int name_len, compat_len;

	name = board_name();
	if (!name)
		return;

	name_len = strlen(name) + 1;

	(void)bolt_dt_addprop_str(fdt, 0, "model", name);

	/* Now update the root "compatible" property with our board name
	 * at the very top
	 */
	prop = fdt_get_property(fdt, 0, "compatible", &compat_len);
	if (compat_len < 0)
		return;

	compat = KMALLOC(compat_len + name_len, 0);
	if (!compat)
		return;

	memcpy(compat, name, name_len);
	memcpy(&compat[name_len], prop->data, compat_len);

	(void)bolt_devtree_at_node_addprop(fdt, 0, "compatible",
			compat, name_len + compat_len);

	KFREE(compat);
}

int sdio_handle_driver_strength(void *fdt, int node,
				sdio_driver_strength_e strength, char *prop)
{
	char *val;

	switch (strength) {
	case SDIO_DRIVER_STRENGTH_A:
		val = "A";
		break;
	case SDIO_DRIVER_STRENGTH_B:
		val = "B";
		break;
	case SDIO_DRIVER_STRENGTH_C:
		val = "C";
		break;
	case SDIO_DRIVER_STRENGTH_D:
		val = "D";
		break;
	default:
		return 0;
	}
	return bolt_dt_addprop_str(fdt, node, prop, val);
}


static int bolt_populate_sdhci(void __maybe_unused *fdt)
{
	int rc = 0, width, non_removable;
	struct board_type *b;
	uint32_t regs[] = {
#ifdef BCHP_SDIO_0_CFG_REG_START
		BREG_PA(SDIO_0_CFG),
#endif
#ifdef BCHP_SDIO_1_CFG_REG_START
		BREG_PA(SDIO_1_CFG),
#endif
	};
	const sdio_params *params;
	char node[80];
	unsigned int x;

	b = board_thisboard();
	if (!b)
		return FDT_ERR_NOTFOUND;

	for (x = 0; x < ARRAY_SIZE(regs); x++) {
		int sdhci;
		/*
		 * If the DT entry for this controller doesn't exist
		 * just skip it.
		 */
		xsprintf(node, DT_RDB_DEVNODE_BASE_PATH"/sdhci@%08x", regs[x]);
		sdhci = bolt_devtree_node_from_path(fdt, node);
		if (sdhci < 0)
			continue;

		rc = 0;
		params = board_sdio(x);
		if (!params || (params->type == SDIO_TYPE_NODEVICE)) {
			rc = bolt_dt_addprop_str(fdt, sdhci, "status",
						 "disabled");
			if (rc)
				break;
			continue;
		}

		switch (params->type) {
		case SDIO_TYPE_EMMC:
			width = 8;
			non_removable = 1;
			break;
		case SDIO_TYPE_EMMC_ON_SDPINS:
			width = 4;
			non_removable = 1;
			break;
		default:
			width = 0;
			non_removable = 0;
			break;
		}

		if (width) {
			rc = bolt_dt_addprop_u32(fdt, sdhci, "bus-width", width);
			if (rc)
				break;
		}
		if (non_removable) {
			rc = bolt_dt_addprop_bool(fdt, sdhci, "non-removable");
			if (rc)
				break;
		}
		if (!params->uhs) {
			rc = bolt_dt_addprop_bool(fdt, sdhci, "no-1-8-v");
			if (rc)
				break;
		}
		rc = sdio_handle_driver_strength(fdt, sdhci,
						params->host_driver_strength,
						"host-driver-strength");
		if (rc)
			break;
		rc = sdio_handle_driver_strength(fdt, sdhci,
						params->host_hs_driver_strength,
						"host-hs-driver-strength");
		if (rc)
			break;
		rc = sdio_handle_driver_strength(fdt, sdhci,
						params->card_driver_strength,
						"card-driver-strength");
		if (rc)
			break;
	}
	if (rc)
		err_msg("EMMC: Error doing DT fixup for SDIO%d", x);
	return rc;
}


/* Move to lib if we get traction on this. */
static char *strdupcat(char *s1, char *s2)
{
	char *s = KMALLOC(strlen(s1) + strlen(s2) + 1, 0);

	if (s) {
		strcpy(s, s1);
		strcat(s, s2);
	}
	return s;
}


/* Move to devtree if we start to use this a lot. */
static int add_compat(void *fdt, int parent, char *s, int prepend)
{
	const struct fdt_property *prop;
	char *c;
	const char *pn = "compatible";
	int rc, sl, fl, cl = 0;

	prop = fdt_get_property(fdt, parent, pn, &cl);
	if (!prop || (cl < 1))
		return BOLT_ERR;

	sl = strlen(s) + 1; /* +nul char */
	fl = cl + sl; /* total length of final compat property */

	c = KMALLOC(fl, 0);
	if (!c)
		return BOLT_ERR_NOMEM;

	if (prepend) {
		memcpy(c, s, sl);
		memcpy(&c[sl], prop->data, cl);
	} else {
		memcpy(c, prop->data, cl);
		memcpy(&c[cl], s, sl);
	}

	rc = bolt_devtree_at_node_addprop(fdt, parent, pn, c, fl);

	KFREE(c);
	return rc;
}


static int bolt_populate_bsp(void *fdt)
{
	int node, rc;
	char *ver, *vs;
	char strnode[20];

	/* only prepend once. Its either this
	or we do a compat str\0str... search function. */
	if (dt_oneshot)
		return 0;

	ver = (char *)sec_get_bsp_version();
	if (!ver)
		return BOLT_OK;

	/* get node from full rdb address */
	xsprintf(strnode, DT_RDB_DEVNODE_BASE_PATH"/bsp@%08x",
		BREG_PA(BSP_CMDBUF));

	/* NB: return OK in case 'dt autogen bsp' was not
	included in the build config. */
	node = bolt_devtree_node_from_path(fdt, strnode);
	if (node < 0)
		return BOLT_OK;

	vs = strdupcat("brcm,bsp-v", ver);
	if (!vs)
		return BOLT_ERR_NOMEM;

	rc = add_compat(fdt, node, vs, 1/*prepend*/);

	KFREE(vs);
	return rc;
}

/* ------------------------------------------------------------------------- */

int bolt_devtree_boltset(void *fdt)
{
	int rc = 0;
	struct board_type *b = board_thisboard();
	char *r = board_init_current_rts();
	int bm = board_init_current_rts_boxmode();
	char *s, bs[16]; /* 10 + sign + \0 + guard */
	int bolt;

	if (!b)
		return BOLT_ERR;

	rc = bolt_populate_memory(fdt);
	if (rc)
		goto out;

	rc = bolt_populate_memory_ctls(fdt);
	if (rc)
		goto out;

	rc = bolt_populate_flash(fdt);
	if (rc)
		goto out;

	rc = bolt_populate_cpus(fdt);
	if (rc)
		goto out;

	rc = bolt_fixup_cpu_clk_div(fdt);
	if (rc)
		goto out;

	rc = bolt_populate_enet(fdt);
	if (rc)
		goto out;

	rc = bolt_populate_gpio_key(fdt);
	if (rc)
		goto out;

	rc = bolt_populate_splash(fdt);
	if (rc)
		goto out;

	rc = bolt_populate_sdhci(fdt);
	if (rc)
		goto out;

	rc = bolt_populate_bsp(fdt);
	if (rc)
		goto out;

	/* BOLT node. err if already exists so allow props
	   to fail instead. */
	(void)bolt_devtree_addnode_at(fdt, "bolt", 0, NULL);

	bolt = bolt_devtree_node_from_path(fdt, "/bolt");
	if (bolt < 0) {
		rc = bolt;
		goto out;
	}

	/* SWBOLT-
	   Watchdog & Programmable Timers Registers
	   */
	rc = bolt_dt_addprop_u32(fdt, bolt, "timer-wdog", BREG_PA(TIMER));
	if (rc)
		goto out;

	/* SWBOLT-99
	*/
	rc = bolt_dt_addprop_u32(fdt, bolt, "reset-history",
				 get_aon_reset_history());

	s = aon_reset_as_string();
	if (s) {
		rc = bolt_dt_addprop_str(fdt, bolt, "reset-list", s);
		if (rc)
			goto out;
	}

	/*
	 * SWBOLT-13: Arbitrary devicetree compatibility version number each
	 * Linux requires from each BOLT release.
	 */
	rc = bolt_dt_addprop_u32(fdt, bolt, "dt-compat", BOLT_VER_DT_COMPAT);
	if (rc)
		goto out;

	rc = bolt_dt_addprop_str(fdt, bolt, "bootdev",
				 boot2str(board_bootmode()));
	if (rc)
		goto out;

	rc = bolt_dt_addprop_str(fdt, bolt, "rts", (r) ? r : "");
	if (rc)
		goto out;
	xsprintf(bs, "%d", bm);
	rc = bolt_dt_addprop_str(fdt, bolt, "box", bs);
	if (rc)
		goto out;

	rc = bolt_dt_addprop_u32(fdt, bolt, "board-id",   b->bid);
	if (rc)
		goto out;
	rc = bolt_dt_addprop_u32(fdt, bolt, "product-id", b->prid);
	if (rc)
		goto out;
	rc = bolt_dt_addprop_u32(fdt, bolt, "family-id",
				 BDEV_RD(BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID));
	if (rc)
		goto out;

	rc = bolt_dt_addprop_str(fdt, bolt, "board", board_name());
	if (rc)
		goto out;
	rc = bolt_dt_addprop_str(fdt, bolt, "tag",   buildtag);
	if (rc)
		goto out;
	rc = bolt_dt_addprop_str(fdt, bolt, "user",  builduser);
	if (rc)
		goto out;
	rc = bolt_dt_addprop_str(fdt, bolt, "date",  builddate);
	if (rc)
		goto out;
	rc = bolt_dt_addprop_u32(fdt, bolt, "version", BOLT_VERSION);
	if (rc)
		goto out;

out:
	bolt_populate_model_and_compatible(fdt);

	bolt_otp_unpopulate(fdt);

	dt_oneshot = 1;

	return libfdt_error(rc);
}
