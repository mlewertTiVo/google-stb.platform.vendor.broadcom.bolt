/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <addr-mapping.h>
#include <arch_ops.h>
#include <bchp_aon_ctrl.h>
#include <bchp_avs_cpu_ctrl.h>
#include <bchp_avs_top_ctrl.h>
#include <bchp_ebi.h>
#include <bchp_nand.h>
#include <board.h>
#include <board_init.h>
#include <boardcfg.h>
#include <bolt.h>
#include <bsp_config.h>
#include <dev_emmcflash.h>
#include "../dev/dev_genet.h"
#include <devfuncs.h>
#include <device.h>
#include <devtree.h>
#include <dt_ops.h>
#include <env_subr.h>
#include <flash.h>
#include <ioctl.h>
#include <lib_malloc.h>
#include <lib_printf.h>
#include <lib_queue.h>
#include <lib_string.h>
#include <lib_types.h>
#include <macutils.h>
#include <mmap-dram.h>
#include <net_api.h>
#include <net_ebuf.h>
#include <net_ether.h>
#include <otp_status.h>
#include <ssbl-common.h>
#include <ssbl-sec.h>
#include <zimage.h>
#ifdef DVFS_SUPPORT
#include <pmap.h>
#endif

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


#if CONFIG_BRCM_GENET_VERSION == 5
#define ENET_COMPAT_INT_PHY_STR "brcm,28nm-ephy"
#else
#define ENET_COMPAT_INT_PHY_STR "brcm,28nm-gphy"
#endif

#define MEMC_NUM_MAPS_MAX 2 /* max number of mappings from DRAM to CPU */
	/* From a memory controller's point of view, the maximum
	 * (or populated) amount of memory ranges from 0 (zero) to
	 * the maximum (or populated) amount of memory. But, the range
	 * has to be mapped to the system memory map, which breaks up
	 * the range of MEMC into multiple chunks except v7-64.
	 * The maximum number of chunks is 2 (two), one under the 4GB
	 * boundary and one above the 4GB boundary.
	 */

#define BREG_PA(block)	(BPHYSADDR(BCHP_##block##_REG_START))
#define BREG_LEN(block)	(BCHP_##block##_REG_END + 4 - BCHP_##block##_REG_START)

/* ------------------------------------------------------------------------- */

/* Some items can only be done once. */
int dt_oneshot;

/* ------------------------------------------------------------------------- */
/*
 * get_attributes() evaluates a set of attributes (eg V7_64).  Each
 * attribute is represented by a single bit.  The function returns
 * a uint16_t value which may be used to evalate conditions when
 * a dt_op should be performed.
 */
uint16_t get_attributes()
{
	static int use_cached_val = false;
	static uint16_t attr;
	int i;

	if (use_cached_val)
		return attr;

	for (i = 0; i < DT_OPS_ATTR_MAX; i++)
		switch(i) {
		case DT_OPS_ATTR_V7_64:
			if (is_mmap_v7_64())
				attr |= 1 << i;
			break;
		default:
			xprintf("WARNING: Bolt compile out of sync with runtime!\n");
			break;
		}
	use_cached_val = true;
	return attr;
}

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
	xsprintf(node, DT_RDB_DEVNODE_BASE_PATH"/nand@%x",
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

	/* For NAND flashes at CS0, we inherit the OOB sector size, and do not
	 * use ONFI detection, there may be a disagreement here (ONFI will
	 * round up to the nearest even number), so indicate that here that we
	 * want the strapped OOB size so BOLT and the OS agree on what to use.
	 */
	if (flash->cs == 0 && flash->oob_sector_size) {
		rc = bolt_dt_addprop_u32(fdt, sat, "brcm,nand-oob-sector-size",
					 flash->oob_sector_size);
		if (rc < 0)
			return rc;
	}

	/* create partiton map sub-sub nodes + its prop.
	*/
	rc = populate_partitions(fdt, flash, sat);
#endif /* CFG_NAND_FLASH */
	return rc;
}


/* ------------------------------------------------------------------------- */
/* SPI, nor efi.
*/
static int bolt_populate_spi(void *fdt, struct flash_dev *flash, char *node_name)
{
	int rc = 0;
	int sat, at;
	char node[80];

	/* /params
	*/
	if (!node_name)
		xsprintf(node, DT_RDB_DEVNODE_BASE_PATH"/spi@%x",
			 BREG_PA(HIF_MSPI));

	xsprintf(node, DT_RDB_DEVNODE_BASE_PATH"/%s@%x",
		 node_name, BREG_PA(HIF_MSPI));

	/*
	 * If the (dt autogen) node does not exist then
	 * instead of failing 'dt bolt' we return with
	 * no error. After this checkpoint we must return
	 * an error for any further failures.
	 */
	at = bolt_devtree_node_from_path(fdt, node);
	if (at < 0)
		return 0; /* No such node */

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

#if CFG_SPI_QUAD_MODE
	rc = bolt_dt_addprop_u32(fdt, sat, "spi-rx-bus-width", 4);
	if (rc)
		return rc;
#endif
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
			ret = bolt_populate_spi(fdt, flash, "qspi");
			if (ret)
				break;
			ret = bolt_populate_spi(fdt, flash, "spi");
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
 * @size_mb the size of DRAM to be mapped, more than the installed amount
 *      of memory can be passed to figure out the system memory map
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
	const struct addr_mapping_entry *am_entry = dram_mapping_table;
	unsigned int num_entries = NUM_DRAM_MAPPING_ENTRIES;

	if (regs == NULL || size_mb == 0)
		return 0;

#ifdef BCHP_AON_CTRL_GLOBAL_ADDRESS_MAP_VARIANT_map_variant_MASK
	if (is_mmap_v7_64()) {
		num_entries = sizeof(dram_mapping_table_v7_64) /
			sizeof*(dram_mapping_table_v7_64);
		am_entry = dram_mapping_table_v7_64;
	}
#endif

	p = regs;
	nsegs = 0;
	for (i = 0; i < num_entries; i++) {
		uint64_t offset_mb;
		uint64_t length_mb;

		if (memc != am_entry[i].which)
			continue;

		offset_mb = am_entry[i].to_mb;
		if (size_mb >= am_entry[i].size_mb) {
			length_mb = am_entry[i].size_mb;
			size_mb -= am_entry[i].size_mb;
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
	unsigned int num_entries = NUM_DRAM_MAPPING_ENTRIES;

	b = board_thisboard();
	if (!b)
		return BOLT_ERR_DEVNOTFOUND;

#ifdef BCHP_AON_CTRL_GLOBAL_ADDRESS_MAP_VARIANT_map_variant_MASK
	if (is_mmap_v7_64())
		num_entries = sizeof(dram_mapping_table_v7_64) /
			sizeof*(dram_mapping_table_v7_64);
#endif

	if (num_entries >= b->nddr) {
		num_mapping_entries = num_entries;
	} else if (num_entries == 0) {
		/* no mapping table */
		num_mapping_entries = b->nddr;
	} else {
		/* mapping, but (num_entries < b->nddr) */
		xprintf("%s: #mapping table entries %d, #MEMC %d\n",
			__func__, num_entries, b->nddr);
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

		if (num_entries > 0) {
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

/**
 * Populates a child node of /reserved-memory for a reserved memory area
 *
 * @fdt The FDT blob
 * @node FDT offset to /reserved-memory
 * @m pointer to reserved memory area
 *
 * @returns BOLT_OK on success, otherwise BOLT error code
 */
static int bolt_populate_reserved_memory_subnode(void *fdt, int node,
	struct memory_area *m)
{
	int rc, subnode;
	char subnode_name[30]; /* strlen("noname@%llx") + margin */
	uint64_t regs[2]; /* #address-cells and #size-cells are 2 */

	/* no sanity check on fdt, node, m and
	 * m->options & BOLT_RESERVE_MEMORY_OPTION_DT_NEW
	 */

	if (m->tag[0] == '\0')
		sprintf(subnode_name, "noname@%llx", m->offset);
	else
		sprintf(subnode_name, "%s@%llx", m->tag, m->offset);

	/* re-create after removing if already exists */
	bolt_devtree_delnode_at(fdt, subnode_name, node);

	rc = bolt_devtree_addnode_at(fdt, subnode_name, node, &subnode);
	if (rc)
		goto out;

	regs[0] = cpu_to_fdt64(m->offset);
	regs[1] = cpu_to_fdt64(m->size);
	rc = bolt_devtree_at_node_addprop(fdt, subnode, "reg",
		regs, sizeof(regs));
	if (rc)
		goto out;

	if (m->options & BOLT_RESERVE_MEMORY_OPTION_DT_NOMAP) {
		rc = bolt_dt_addprop_bool(fdt, subnode, "no-map");
		if (rc)
			goto out;
	}

	if (m->options & BOLT_RESERVE_MEMORY_OPTION_DT_REUSABLE) {
		rc = bolt_dt_addprop_bool(fdt, subnode, "reusable");
		if (rc)
			goto out;
	}

	if (m->tag[0] != '\0') {
		rc = bolt_dt_addprop_str(fdt, subnode,
			"reserved-names", m->tag);
		if (rc)
			goto out;
	}

out:
	KFREE(subnode_name);
	return rc;
}

/**
 * Populates /reserved-memory or /memreserve nodes for reserved memory areas
 *
 * Memory areas are marked reserved so that Linux does not use them.
 * The list of reserved memory areas is retrieved, and added under
 * /reserved-memory node (or as /memreserve for backward compatibility).
 *
 * @fdt The FDT blob
 *
 * @returns the number of processed devive tree nodes on success,
 *      BOLT error code othrewise
 */
static int bolt_populate_reserved_memory(void *fdt)
{
	struct memory_area list;
	int rc, actual_count, list_count, node;

	list_count = bolt_reserve_memory_getlist(&list);
	if (list_count <= 0)
		return list_count;

	/* /reserved-memory must exist */
	node = bolt_devtree_node_from_path(fdt, "/reserved-memory");
	if (node < 0) {
		err_msg("/reserved-memory is missing\n");
		return node;
	}

	actual_count = 0;
	while (!q_isempty((queue_t *)&list)) {
		struct memory_area *m =
			(struct memory_area *) q_getfirst((queue_t *)&list);

		/* A reserved memory area should not show up on both
		 * the legacy and new DT nodes. But, it should be
		 * filtered out or checked when a reservation is made.
		 * Here, DT nodes are created based on any request.
		 * It is also okay for a reservation to have no DT entry.
		 */
		if (m->options & BOLT_RESERVE_MEMORY_OPTION_DT_LEGACY) {
			rc = bolt_devtree_add_memreserve(fdt,
				m->offset, m->size);
			if (rc)
				goto out;
		}
		if (m->options & BOLT_RESERVE_MEMORY_OPTION_DT_NEW) {
			rc = bolt_populate_reserved_memory_subnode(fdt,
				node, m);
			if (rc)
				goto out;
		}

		if (m->options & BOLT_RESERVE_MEMORY_OPTION_DT_LEGACY &&
		    m->options & BOLT_RESERVE_MEMORY_OPTION_DT_NEW)
			warn_msg("dual DT entries of [%llx..%llx)\n",
				m->offset, m->offset + m->size);

		++actual_count;
		q_dequeue((queue_t *)m);
		KFREE(m);
	}
	rc = actual_count;

out:
	if (actual_count != list_count)
		warn_msg("%d entries of reserved memory areas was reported, "
			"but %d was processed from the list\n",
			list_count, actual_count);

	/* release remaining nodes if any */
	while (!q_isempty((queue_t *)&list)) {
		queue_t *q = q_getfirst((queue_t *)&list);

		q_dequeue(q);
		KFREE(q);
	}

	return rc;
}

/**
 * Adjusts prepopulated mappings between MEMC and CPU address spaces
 *
 * The scripting engine populates a DT property, 'brcm,map-to-cpu', under
 * each memory controller DT node, '/rdb/memory_controllers/memc@n' based
 * on the mmap BOLT configuration commands. But, v7-64 is expressed via
 * mmap64. Also, whether v7-64 or v7-32 is determined at run-time by
 * a strap setting. The pre-populated memory map needs be adjusted if
 * v7-64 is selected.
 *
 * @fdt The FDT blob
 * @memc the index of MEMC whose DRAM is mapped to CPU address space
 *
 * @returns BOLT_OK if no need for adjustment or successfully adjusted,
 *      BOLT_ERR_NOMEM if mapping requires more than MEMC_NUM_MAPS_MAX
 *      segments (chunks), otherwise the return value of
 *      bolt_devtree_node_from_path() or bolt_devtree_at_node_addprop()
 */
static int bolt_adjust_memory_map(void *fdt, unsigned int memc)
{
	char path[128];
	int node;
	uint64_t simple_regs[2*MEMC_NUM_MAPS_MAX]; /* {cpu_off, size} */
	uint64_t regs[3*MEMC_NUM_MAPS_MAX]; /* {dram_off, cpu_off, size} */
	uint64_t dram_offset;
	unsigned int nsegs;
	unsigned int i;
	int rc;

	/* adjustment is required if and only if v7-64 */
	if (!is_mmap_v7_64())
		return BOLT_OK;

	xsprintf(path, DT_RDB_DEVNODE_BASE_PATH "/memory_controllers/memc@%d",
		memc);
	node = bolt_devtree_node_from_path(fdt, path);
	if (node < 0) {
		err_msg("DT node for MEMC%d does NOT exist", memc);
		return node;
	}

	nsegs = map_ddr_to_cpu(simple_regs, memc, UINT32_MAX);
	if (nsegs > MEMC_NUM_MAPS_MAX) {
		err_msg("too many %d mappings for MEMC%d", nsegs, memc);
		return BOLT_ERR_NOMEM;
	}

	dram_offset = 0;
	for (i = 0; i < nsegs; ++i) {
		regs[i*3] = cpu_to_fdt64(dram_offset);
		regs[i*3 + 1] = cpu_to_fdt64(simple_regs[i*2]);
		regs[i*3 + 2] = cpu_to_fdt64(simple_regs[i*2 + 1]);
		dram_offset += simple_regs[i*2 + 1];
	}

	rc = bolt_devtree_at_node_addprop(fdt, node, "brcm,map-to-cpu",
		regs, sizeof(uint64_t) * 3 * nsegs);

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
	char tmpstr[128];
	unsigned int i, base_addr;
	int node;

	b = board_thisboard();
	if (!b)
		return BOLT_ERR_DEVNOTFOUND;

	for (i = 0; i < b->nddr; i++) {
		/* the mapping between DRAM and CPU should exist regardless
		 * whether a corresponding MEMC is actually faciliated.
		 */
		rc = bolt_adjust_memory_map(fdt, i);
		if (rc < 0) {
			err_msg("failed adjusting mmap for MEMC%d %d", i, rc);
			return rc;
		}

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
			 "/memory_controllers/memc@%d/memc-ddr@%x",
			 ddr->which, BPHYSADDR(base_addr));

		node = bolt_devtree_node_from_path(fdt, tmpstr);
		if (node < 0)
			continue;

		/*
		 * Mark unused memory controllers as disabled. This helps Linux
		 * in determining how many active memory controllers there are.
		 * We use this in the CPUfreq driver, for instance.
		 */
		if (ddr->ddr_size == 0)
			rc = bolt_dt_addprop_str(fdt, node, "status",
				"disabled");
		if (rc)
			return rc;

		/* clock-frequency is always in Hertz whereas our local storage
		 * for ddr_clock is in Mhz, convert that
		 */
		rc = bolt_dt_addprop_u32(fdt, node, "clock-frequency",
					 ddr->ddr_clock * 1000 * 1000);
	}
#endif
	return rc;
}

static int bolt_populate_scb_freq(void *fdt)
{
	int brcmstb_clk_node, sw_scb_node, rc;

	brcmstb_clk_node = bolt_devtree_node_from_path(fdt,
					DT_RDB_DEVNODE_BASE_PATH
					"/brcmstb-clks");
	if (brcmstb_clk_node < 0) /* no clock tree (PLX file) */
		return BOLT_OK;

	sw_scb_node = bolt_devtree_node_from_path(fdt,
					DT_RDB_DEVNODE_BASE_PATH
					"/brcmstb-clks/sw_scb");
	/* Backward compatibility: if the node doesn't exist, create it. */
	if (sw_scb_node < 0) {
		rc = bolt_devtree_addnode_at(fdt, "sw_scb", brcmstb_clk_node,
					&sw_scb_node);
		if (rc)
			return rc;

		rc = bolt_dt_addprop_str(fdt, sw_scb_node, "compatible",
					"fixed-clock");
		if (rc)
			return rc;

		rc = bolt_dt_addprop_u32(fdt, sw_scb_node, "#clock-cells", 0);
		if (rc)
			return rc;
	}

	/* We re-use the existing node and add the clock frequency. */
	rc = bolt_dt_addprop_u32(fdt, sw_scb_node, "clock-frequency",
				 arch_get_scb_freq_hz());

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

#ifdef STUB64_START
		rc = bolt_dt_addprop_str(fdt, cpu_node,
			"enable-method", "psci");
		if (rc < 0) {
			xprintf("%s: couldn't set enable method\n",
				__func__);
			return rc;
		}
#endif
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
	else if (!strcmp(phy_type, "GMII"))
		return "gmii";

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

static void bolt_devtree_add_fixed_link(void *fdt, int phandle, int offset,
					int instance, char *phy_type,
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

	if (phandle >= 0)
		bolt_devtree_at_handle_addprop(fdt, phandle, "fixed-link",
					       fixed_link_property,
					       5 * sizeof(int));
	else if (offset >= 0)
		bolt_devtree_at_node_addprop(fdt, offset, "fixed-link",
					     fixed_link_property,
					     5 * sizeof(int));
	else
		err_msg("invalid phandle or offset specified\n");
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
	char *compat = NULL;
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
		xsprintf(phystr, "%s", ENET_COMPAT_INT_PHY_STR);
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

static int bolt_populate_ext_moca(void *fdt, uint8_t *macaddr)
{
	unsigned int i, port_mask = 0;
	const ext_moca_params *m;
	char tmpstr[255];
	char *phy_type;
	int phandle;
	int rc, port;
	int eth_ports;

	phandle = bolt_devtree_phandle_from_alias(fdt, "ext_moca");
	if (phandle < 0)
		return phandle;

	rc = fdt_node_offset_by_phandle(fdt, phandle);
	if (rc < 0)
		return rc;

	eth_ports = bolt_devtree_subnode(fdt, "ethernet-ports", rc);
	if (eth_ports < 0)
		return eth_ports;

	/* These properties apply to the 'ethernet-ports' sub-nodes */
	for (i = 0; i < NUM_EXT_MOCA; i++) {
		m = board_ext_moca(i);
		if (!m)
			continue;

		xsprintf(tmpstr, "rgmii%d", m->rgmii);
		port = bolt_devtree_subnode(fdt, tmpstr, eth_ports);
		if (port < 0)
			continue;

		if (enet_needs_fixed_link(m->phy_type, m->mdio_mode))
			bolt_devtree_add_fixed_link(fdt, -1, port,
						    m->rgmii,
						    m->phy_type,
						    m->phy_speed);

		phy_type = phystr2std(m->phy_type);
		bolt_dt_addprop_str(fdt, port, "phy-mode", phy_type);

		/* A network instance is tied to MoCA so its DT config code
		 * should have set up its MAC address before directly calling
		 * us here for us to match or not (MAC address tracking post
		 * increments) as the case may be.
		 */
		 if (atoi(m->phy_id) == 257 && atoi(m->mdio_mode) == 0) {
			if (BSP_CFG_MOCA_MAC_EQ_ETH_MAC)
				macaddr_decrement(macaddr, MACADDR_INCREMENT);

			bolt_add_macaddr_prop(fdt, 0, port, macaddr);
		}

		port_mask |= 1 << i;

		rc = bolt_devtree_phandle_from_alias(fdt, m->enet_node);
		if (rc < 0)
			continue;

		bolt_devtree_at_node_delprop(fdt, port, "enet-id");
		bolt_dt_addprop_u32(fdt, port, "enet-id", rc);
	}

	/* Re-calculate the offset */
	rc = fdt_node_offset_by_phandle(fdt, phandle);
	if (rc < 0)
		return rc;

	if (port_mask) {
		bolt_devtree_at_node_delprop(fdt, rc, "status");
		bolt_devtree_at_node_addprop(fdt, rc, "status",
					     "okay", strlen("okay") + 1);
	}

	return 0;
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
		DT_RDB_DEVNODE_BASE_PATH"/bmoca@%x", BPHYSADDR(m->base));

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
		bolt_devtree_add_fixed_link(fdt, phandle, -1,
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

	if (bolt_populate_ext_moca(fdt, macaddr))
		return 1;

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

#if defined(BCHP_SWITCH_CORE_REG_START) && (NUM_SWITCH_PORTS > 0)
static int bolt_populate_eth_switch_port(void *fdt, uint8_t *macaddr,
					 unsigned int port,
					 const char *alias_stem,
					 bool first_time)
{
	uint32_t data;
	const enet_params *e;
	char tmpstr[255];
	int phandle;
	char *phy_type, *phy_id;
	int mdio_node;
	int rc;

	xsprintf(tmpstr, "%s%d", alias_stem, port);
	phandle = bolt_devtree_phandle_from_alias(fdt, tmpstr);
	if (phandle < 0)
		return 1;

	e = board_enet(port);
	if (!e) {
		/* Last port is the CPU port, never mark it as disabled */
		if (port != NUM_SWITCH_PORTS - 1)
			bolt_devtree_at_handle_addprop(fdt, phandle,
				"status", "disabled", strlen("disabled") + 1);
		return 1;
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
				phandle, -1, e->switch_port,
				e->phy_type, e->phy_speed);

	else if (e->phy_type && data != PHY_ID_NONE) {
		if (e->ethsw && atoi(phy_id) == 0x1e) {
			phy_id = "0";
			data = 0;
		}

		/* Create the PHY the first time, or re-use its existing
		 * phandle number otherwise
		 */
		if (first_time) {
			bolt_devtree_add_phy(fdt, phandle, e, data, mdio_node,
					     e->phy_type, e->phy_speed, phy_id);
		} else {
			xsprintf(tmpstr,
				 DT_RDB_DEVNODE_BASE_PATH
				 "/switch_top@%x/mdio@%x/phy%x: ethernet-phy@%x",
				 BPHYSADDR(BCHP_SWITCH_CORE_REG_START),
				 BCHP_SWITCH_MDIO_REG_START -
				 BCHP_SWITCH_CORE_REG_START,
				 atoi(phy_id), atoi(phy_id));

			rc = bolt_devtree_phandle_from_path(fdt, tmpstr);
			if (rc < 0)
				return rc;
			bolt_devtree_at_handle_addprop_int(fdt, phandle,
							   "phy-handle", rc);
		}
	}

	phy_type = phystr2std(e->phy_type);
	bolt_devtree_at_handle_addprop(fdt, phandle,
			"phy-mode", phy_type, strlen(phy_type) + 1);

	if (first_time) {
		if (!strcmp(phy_type, "moca"))
			if (bolt_populate_moca(fdt, macaddr))
				return 1;

		if (bolt_populate_ext_moca(fdt, macaddr))
			return 1;
	}

	return 0;
}
#endif

static int bolt_populate_eth_switch(void *fdt, uint8_t *macaddr)
{
#if defined(BCHP_SWITCH_CORE_REG_START) && (NUM_SWITCH_PORTS > 0)
	const char *aliases[] = { "switch_port", "sw_port" };
	unsigned int port;
	char tmpstr[255];
	int mdio_node;
	int rc;

	xsprintf(tmpstr, DT_RDB_DEVNODE_BASE_PATH"/switch_top@%x/mdio@%x",
		BPHYSADDR(BCHP_SWITCH_CORE_REG_START),
		BCHP_SWITCH_MDIO_REG_START - BCHP_SWITCH_CORE_REG_START);

	mdio_node = bolt_devtree_node_from_path(fdt, tmpstr);
	if (mdio_node < 0) {
		xprintf("cannot find %s\n", tmpstr);
		return mdio_node;
	}
	bolt_devtree_remove_phy_nodes(fdt, mdio_node);

	/* Populate for the two aliases we need to support */
	for (port = 0; port < NUM_SWITCH_PORTS; port++) {
		rc = bolt_populate_eth_switch_port(fdt, macaddr, port,
						   aliases[0], true);
		rc |= bolt_populate_eth_switch_port(fdt, macaddr, port,
						    aliases[1], false);
		if (rc == 1)
			continue;
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

	xsprintf(nodestr, DT_RDB_DEVNODE_BASE_PATH"/rf4ce@%x",
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

	root_node = bolt_devtree_node_from_path(fdt, "/");
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

static int bolt_populate_gpio_led(void *fdt)
{
	int rc = 0;
	int property[3];
	int gpio_leds, phandle, gpio_led, root_node;
	const char *leds_node = "leds";
	char buffer[20];

	const gpio_led_params *m = board_gpio_leds();

	if (!m || !m->name || !m->gpio)
		return rc;

	/* Skip re-creating if node already exists */
	gpio_leds = bolt_devtree_node_from_path(fdt, "/leds");
	if (gpio_leds >= 0)
		return 0;

	root_node = bolt_devtree_node_from_path(fdt, "/");
	if (root_node < 0)
		return root_node;

	rc = bolt_devtree_addnode_at(fdt, leds_node, root_node,
				&gpio_leds);
	if (rc < 0)
		return rc;

	rc = bolt_dt_addprop_str(fdt, gpio_leds,
				"compatible", "gpio-leds");
	if (rc)
		return rc;

	/* generate sub node for each led */

	while (m->name && m->gpio) {
		rc = bolt_devtree_addnode_at(fdt, m->name, gpio_leds,
					&gpio_led);
		if (rc < 0)
			return rc;

		phandle = bolt_devtree_phandle_from_alias(fdt, m->gpio);

		property[0] = cpu_to_fdt32(phandle);
		property[1] = cpu_to_fdt32(m->pin);
		property[2] = cpu_to_fdt32(m->pol);

		rc = bolt_devtree_at_node_addprop(fdt, gpio_led,
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

static int bolt_populate_bt_rfkill(void *fdt)
{
	int rc = 0;
	int property[3];
	int bt_rfkill, phandle, root_node;
	unsigned int post_delay = 0;
	const char *rfkill_node = "bcmbt_rfkill";

	const bt_rfkill_params *m = board_bt_rfkill_gpios();

	if (!m || !m->name || !m->gpio)
		return rc;

	root_node = bolt_devtree_node_from_path(fdt, "/");
	if (root_node < 0)
		return root_node;

	(void)bolt_devtree_delnode_at(fdt, rfkill_node, root_node);

	rc = bolt_devtree_addnode_at(fdt, rfkill_node, root_node,
				&bt_rfkill);
	if (rc < 0)
		return rc;

	rc = bolt_dt_addprop_str(fdt, bt_rfkill,
				"compatible", "brcm,bcmbt-rfkill");
	if (rc)
		return rc;

	/* Generate GPIO properties */
	while (m->name && m->gpio) {
		phandle = bolt_devtree_phandle_from_alias(fdt, m->gpio);

		property[0] = cpu_to_fdt32(phandle);
		property[1] = cpu_to_fdt32(m->pin);
		property[2] = cpu_to_fdt32(m->pol);
		post_delay = max(post_delay, m->pdelay);

		rc = bolt_devtree_at_node_addprop(fdt, bt_rfkill,
					m->name, property, 3 * sizeof(int));
		if (rc)
			return rc;

		m++;
	}

	return bolt_dt_addprop_u32(fdt, bt_rfkill,
				   "brcm,post-delay", post_delay);
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
	const uint16_t attr = get_attributes();

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
		if (ops->op != DT_OP_MAC ||
			((ops->attr_mask & attr) != ops->attr_val))
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
#elif defined(OTP_OPTION_SATA0_DISABLE)
		if (OTP_OPTION_SATA0_DISABLE())
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
			xsprintf(text, "pcie@%x", BCHP_PHYSICAL_OFFSET +
				BCHP_PCIE_0_RC_CFG_TYPE1_REG_START);
			rm_nodes(fdt, text, p_root);
		}
#endif

#if defined(BCHP_PCIE_1_RC_CFG_TYPE1_REG_START) && \
	defined(OTP_OPTION_PCIE1_DISABLE)
		if (OTP_OPTION_PCIE1_DISABLE()) {
			xsprintf(text, "pcie@%x", BCHP_PHYSICAL_OFFSET +
				BCHP_PCIE_1_RC_CFG_TYPE1_REG_START);
			rm_nodes(fdt, text, p_root);
		}
#endif

	/*	For usb/devtree work we need to consider:

			OTP_OPTION_USB[0..1]_P[0..1]_DISABLE (7366a0)
			OTP_OPTION_USB_P[0..1]_DISABLE
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
	int __maybe_unused err = 0, parent, child;
	const uint16_t attr = get_attributes();

	if (!ops) {
		if (CFG_CMD_LEVEL >= 5)
			xprintf("no hard wired devicetree mods for this board\n");
		return;
	}

	for ( ; ops->path != NULL; ops++) { /* path is REQUIRED */
		/* See if this op is conditional */
		if ((ops->attr_mask & attr) != ops->attr_val)
			continue;

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
		if (cull->op == DT_OP_CULL &&
			((ops->attr_mask & attr) == ops->attr_val))
		{
			parent = bolt_devtree_node_from_path(fdt, cull->path);
			if (parent >= 0) {
				if (cull->prop) {
					/* CULL the property, not the node */
					child = bolt_devtree_subnode(fdt,
						     cull->node, parent);
					if (child >= 0)
						err = bolt_devtree_at_node_delprop(fdt,
							child, cull->prop);
					else
						err = child;
					if (err)
						xprintf("failure deleting prop=%s"
							" (err=%d)\n",
							cull->prop, err);
				} else {
					/* CULL the node */
					err = bolt_devtree_delnode_at(fdt,
						      cull->node, parent);
					if (CFG_CMD_LEVEL >= 5)
						xprintf("%s() removed %s %s, "
							"err=%d\n",
							__func__, cull->path,
							cull->node, err);
				}
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

/**
 * Populate properties that directly belong to / (root)
 *
 * Populated (or updated) properties are:
 * - model: populated with board name
 * - compatible: refined with the board name
 * - serial-number: populated with the BOARD_SERIAL environment variable
 *
 * @fdt The FDT blob
 */
static void bolt_populate_root_properties(void *fdt)
{
	const struct fdt_property *prop;
	const char *name;
	char *compat;
	int name_len, compat_len;
	char *serial;

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

	serial = env_getenv(ENVSTR_BOARD_SERIAL);
	if (serial)
		(void)bolt_dt_addprop_str(fdt, 0, "serial-number", serial);
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

/*
 * Set the environment variable 'EMMC_DEVNAME' that represents
 * the name of the SDHCI device-tree node used for EMMC.
 */
static int bolt_set_emmc_devname(void *fdt, int sdhci_node)
{
	const char *sdhci_node_name;
	const char *unit;
	const struct fdt_property *prop;
	int proplen, namelen;
	uint32_t reg;
	char emmc_devname[100];
	char name[20];

	sdhci_node_name = fdt_get_name(fdt, sdhci_node, NULL);
	if (!sdhci_node_name)
		return BOLT_ERR;
	unit = strchr(sdhci_node_name, '@');
	if (unit)
		namelen = unit - sdhci_node_name;
	else
		namelen = strlen(sdhci_node_name);
	if (namelen > (int)(sizeof(name)-1))
		return BOLT_ERR;
	strncpy(name, sdhci_node_name, namelen);
	name[namelen] = '\0';

	prop = fdt_get_property(fdt, sdhci_node, "reg", &proplen);
	if (proplen < (int)(sizeof(uint32_t) * 4))
		return BOLT_ERR;

	reg = DT_PROP_DATA_TO_U32(prop->data, 0);

	xsprintf(emmc_devname, "%x.%s", reg, name);
	env_setenv("EMMC_DEVNAME", emmc_devname, 0);

	return BOLT_OK;
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
		int sdhci, use_cmd_12 = 0;
		/*
		 * If the DT entry for this controller doesn't exist
		 * just skip it.
		 */
		xsprintf(node, DT_RDB_DEVNODE_BASE_PATH"/sdhci@%x", regs[x]);
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
			use_cmd_12 = 1; /* not eMMC */
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
			rc = bolt_set_emmc_devname(fdt, sdhci);
			if (rc)
				break;
		}
		if (use_cmd_12) {
			rc = bolt_dt_addprop_bool(fdt, sdhci,
				"sdhci,auto-cmd12");
			if (rc)
				break;
		}
		if (!params->uhs) {
			rc = bolt_dt_addprop_bool(fdt, sdhci, "no-1-8-v");
			if (rc)
				break;
		} else {
			/*
			 * Later kernels depend on these props to enable UHS
			 * For eMMC, enable HS200
			 * For SD, enable DDR50 and SDR50
			 */
			if (non_removable) {
				rc = bolt_dt_addprop_bool(fdt, sdhci,
							"mmc-hs200-1_8v");
				if (rc)
					break;
			} else {
				rc = bolt_dt_addprop_bool(fdt, sdhci,
							"sd-uhs-ddr50");
				if (rc)
					break;
				rc = bolt_dt_addprop_bool(fdt, sdhci,
							"sd-uhs-sdr50");
				if (rc)
					break;
			}
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
	xsprintf(strnode, DT_RDB_DEVNODE_BASE_PATH"/bsp@%x",
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


int bolt_devtree_boltset_boot(void *fdt, struct dt_boot_mods *bm)
{
	if (!bm) /* Nothing (else) requiring external bm options. */
		return BOLT_OK;

#ifdef STUB64_START
	if (bm->loader_32bit)
		(void)bolt_devtree_delnode_at(fdt, "psci", 0);
#endif
	return BOLT_OK;
}

static int bolt_populate_pmap(void *fdt)
{
#ifndef PMAP_MAX_MUXES
	return BOLT_OK; /* do nothing */
#else
	int rc = BOLT_OK;
	char node[80];
	unsigned int data, i;
	unsigned int pmap_number = board_pmap();
	unsigned int pmap_index = board_pmap_index(pmap_number);
	int brcmstb_clk_node = 0, offset = 0, pmap_node;

	brcmstb_clk_node = bolt_devtree_node_from_path(fdt,
		DT_RDB_DEVNODE_BASE_PATH"/brcmstb-clks");
	if (brcmstb_clk_node == 0) {
		warn_msg("cannot find a clock tree\n");
		return BOLT_OK; /* not a critical failure condition */
	}

	for (i = 0; i < PMAP_MAX_MUXES; i++) {
		xsprintf(node, "pmap-mux%d:pmap-mux%d@%x",
			i, i, BPHYSADDR(pmapMuxes[i].reg));

		offset = bolt_devtree_subnode(fdt, node, brcmstb_clk_node);
		if (offset >= 0)
			continue; /* skip re-creating */

		rc = bolt_devtree_addnode_at(fdt, node, brcmstb_clk_node,
				&pmap_node);
		if (rc)
			goto out;

		rc = bolt_dt_addprop_str(fdt, pmap_node, "compatible",
			"brcm,pmap-mux");
		if (rc)
			goto out;

		data = cpu_to_fdt32(BPHYSADDR(pmapMuxes[i].reg));
		rc = bolt_devtree_at_node_appendprop(fdt, pmap_node, "reg",
				&data, sizeof(data));
		if (rc)
			goto out;

		data = cpu_to_fdt32(sizeof(pmapMuxes[i].reg));
		rc = bolt_devtree_at_node_appendprop(fdt, pmap_node, "reg",
			&data, sizeof(data));
		if (rc)
			goto out;

		rc = bolt_dt_addprop_u32(fdt, pmap_node, "bit-mask",
			pmapMuxes[i].mask);
		if (rc)
			goto out;

		rc = bolt_dt_addprop_u32(fdt, pmap_node, "bit-shift",
			pmapMuxes[i].shift);
		if (rc)
			goto out;

		rc = bolt_dt_addprop_u32(fdt, pmap_node, "brcm,value",
			pmapMuxValues[pmap_index][i]);
		if (rc)
			goto out;
	}

	for (i = 0; i < PMAP_MAX_DIVIDERS; i++) {
		xsprintf(node, "pmap-divider%d:pmap-divider%d@%x",
			i, i, BPHYSADDR(pmapDividers[i].reg));

		offset = bolt_devtree_subnode(fdt, node, brcmstb_clk_node);
		if (offset >= 0)
			continue; /* skip re-creating */

		rc = bolt_devtree_addnode_at(fdt, node, brcmstb_clk_node,
			&pmap_node);
		if (rc)
			goto out;

		rc = bolt_dt_addprop_str(fdt, pmap_node, "compatible",
			"brcm,pmap-divider");
		if (rc)
			goto out;

		data = cpu_to_fdt32(BPHYSADDR(pmapDividers[i].reg));
		rc = bolt_devtree_at_node_appendprop(fdt, pmap_node, "reg",
			&data, sizeof(data));
		if (rc)
			return rc;

		data = cpu_to_fdt32(sizeof(pmapDividers[i].reg));
		rc = bolt_devtree_at_node_appendprop(fdt, pmap_node, "reg",
			&data, sizeof(data));
		if (rc)
			goto out;

		rc = bolt_dt_addprop_u32(fdt, pmap_node, "bit-mask",
			pmapDividers[i].mask);
		if (rc)
			goto out;

		rc = bolt_dt_addprop_u32(fdt, pmap_node, "bit-shift",
			pmapDividers[i].shift);
		if (rc)
			goto out;

		rc = bolt_dt_addprop_u32(fdt, pmap_node, "brcm,value",
			pmapDividerValues[pmap_index][i]);
		if (rc)
			goto out;
	}

#ifdef PMAP_MAX_MULTIPLIERS
	for (i = 0; i < PMAP_MAX_MULTIPLIERS; i++) {
		xsprintf(node, "pmap-multiplier%d:pmap-multiplier%d@%x",
			i, i, BPHYSADDR(pmapMultipliers[i].reg));

		offset = bolt_devtree_subnode(fdt, node, brcmstb_clk_node);
		if (offset >= 0)
			continue; /* skip re-creating */

		rc = bolt_devtree_addnode_at(fdt, node, brcmstb_clk_node,
			&pmap_node);
		if (rc)
			goto out;

		rc = bolt_dt_addprop_str(fdt, pmap_node, "compatible",
			"brcm,pmap-multiplier");
		if (rc)
			goto out;

		data = cpu_to_fdt32(BPHYSADDR(pmapMultipliers[i].reg));
		rc = bolt_devtree_at_node_appendprop(fdt, pmap_node, "reg",
			&data, sizeof(data));
		if (rc)
			return rc;

		data = cpu_to_fdt32(sizeof(pmapMultipliers[i].reg));
		rc = bolt_devtree_at_node_appendprop(fdt, pmap_node, "reg",
			&data, sizeof(data));
		if (rc)
			goto out;

		rc = bolt_dt_addprop_u32(fdt, pmap_node, "bit-mask",
			pmapMultipliers[i].mask);
		if (rc)
			goto out;

		rc = bolt_dt_addprop_u32(fdt, pmap_node, "bit-shift",
			pmapMultipliers[i].shift);
		if (rc)
			goto out;

		rc = bolt_dt_addprop_u32(fdt, pmap_node, "brcm,value",
			pmapMultiplierValues[pmap_index][i]);
		if (rc)
			goto out;
	}
#endif /* PMAP_MAX_MULTIPLIERS */

out:
	return rc;
#endif /* PMAP_MAX_MUXES */
}

int bolt_devtree_boltset(void *fdt)
{
	int rc = 0;
	struct board_type *b = board_thisboard();
	int boxmode = board_init_rts_current_boxmode();
	char *s, bs[16]; /* 10 + sign + \0 + guard */
	int bolt;

	if (!b)
		return BOLT_ERR;

	rc = bolt_populate_memory(fdt);
	if (rc)
		goto out;

	rc = bolt_populate_reserved_memory(fdt);
	if (rc < 0)
		goto out;

	rc = bolt_populate_memory_ctls(fdt);
	if (rc)
		goto out;

	rc = bolt_populate_scb_freq(fdt);
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

	rc = bolt_populate_gpio_led(fdt);
	if (rc)
		goto out;

	rc = bolt_populate_bt_rfkill(fdt);
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

	/* The property of 'rts' has been deprecated. So has its string
	 * format. It was something like "CCYYMMDDHHMMSS_shortdesc_box%d".
	 * But, Nexus looked only at "_box%d" even when 'rts' was used.
	 */
	xsprintf(bs, "deprecated_format_box%d", boxmode);
	rc = bolt_dt_addprop_str(fdt, bolt, "rts", bs);
	if (rc)
		goto out;
	xsprintf(bs, "%d", boxmode);
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

#ifdef DVFS_SUPPORT
	rc = bolt_dt_addprop_u32(fdt, bolt, "pmap", board_pmap());
	if (rc)
		goto out;
#endif

	rc = bolt_populate_pmap(fdt);
	if (rc)
		goto out;

out:
	bolt_populate_root_properties(fdt);

	bolt_otp_unpopulate(fdt);

	dt_oneshot = 1;

	return libfdt_error(rc);
}
