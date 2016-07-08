/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <stdbool.h>

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "error.h"
#include "env_subr.h"
#include "devtree.h"
#include "board.h"
#include "arch_ops.h"
#include "chipid.h"
#include "bchp_common.h"

#include "tz.h"
#include "tz_priv.h"


#define NWOS_USE_RESERVED_MEM 1


static int tz_devtree_cpprop(void *fdt, int node,
	void *fdt_src, int node_src, const char *prop_name)
{
	const struct fdt_property *prop;
	int prop_len;

	prop = fdt_get_property(fdt_src, node_src, prop_name, &prop_len);
	if (!prop) {
		if (prop_len == -FDT_ERR_NOTFOUND)
			return 0;
		else
			return BOLT_ERR;
	}

	return bolt_devtree_at_node_addprop(fdt, node, prop_name,
		prop->data, prop_len);
}

static int tz_populate_rdb(void *fdt)
{
	int rc;
	struct tz_info *t;
	struct tz_reg_group *reg_group;
	int rdb_node;

	bolt_devtree_params_t p;
	void *fdt_nwos;
	int rdb_nwos;

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	/* Get NWOS device tree */
	bolt_devtree_getenvs(&p);

	fdt_nwos = p.dt_address;
	if (!fdt_nwos)
		return BOLT_ERR_BADADDR;

	/* Get NWOS rdb node */
	rdb_nwos = fdt_subnode_offset(fdt_nwos, 0, "rdb");
	if (rdb_nwos < 0)
		return BOLT_ERR;

	/* Add rdb node */
	rc = bolt_devtree_addnode_at(fdt, "rdb", 0, &rdb_node);
	if (rc)
		return rc;

	rc = bolt_dt_addprop_u32(fdt, rdb_node, "#size-cells", 1);
	if (rc)
		return rc;

	rc = bolt_dt_addprop_u32(fdt, rdb_node, "#address-cells", 1);
	if (rc)
		return rc;

	rc = tz_devtree_cpprop(fdt, rdb_node,
		fdt_nwos, rdb_nwos, "ranges");
	if (rc)
		return rc;

	rc = tz_devtree_cpprop(fdt, rdb_node,
		fdt_nwos, rdb_nwos, "compatible");
	if (rc)
		return rc;

	reg_group = t->reg_groups;
	if (!reg_group)
		return BOLT_ERR;

	while (reg_group->compatible) {
		uint32_t addr;
		uint32_t size;
		int node;
		char pname[128];
		char compat[128];
		int compat_len;
		uint32_t regs[2];

		addr = BCHP_PHYSICAL_OFFSET + reg_group->start;
		size = reg_group->end - reg_group->start + 4;

		xsprintf(pname, "syscon@%08x", addr);

		/* Add syscon node */
		rc = bolt_devtree_addnode_at(fdt, pname, rdb_node, &node);
		if (rc)
			return rc;

		regs[0] = cpu_to_fdt32(addr);
		regs[1] = cpu_to_fdt32(size);

		rc = bolt_devtree_at_node_addprop(fdt, node,
			"reg", regs, sizeof(regs));
		if (rc)
			return rc;

		compat_len = strlen(reg_group->compatible) + 1;
		memcpy(compat, reg_group->compatible, compat_len);

		xsprintf(compat + compat_len, "syscon");
		compat_len += 7;

		rc = bolt_devtree_at_node_addprop(fdt, node,
			"compatible", compat, compat_len);
		if (rc)
			return rc;

		reg_group++;
	}

	return 0;
}

static int tz_populate_serial(void *fdt)
{
	int rc;
	struct tz_info *t;
	int rdb_node;

	bolt_devtree_params_t p;
	void *fdt_nwos;
	int rdb_nwos;

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	/* Get NWOS device tree */
	bolt_devtree_getenvs(&p);

	fdt_nwos = p.dt_address;
	if (!fdt_nwos)
		return BOLT_ERR_BADADDR;

	/* Get NWOS rdb node */
	rdb_nwos = fdt_subnode_offset(fdt_nwos, 0, "rdb");
	if (rdb_nwos < 0)
		return BOLT_ERR;

    /* Get rdb node */
	rdb_node = fdt_subnode_offset(fdt, 0, "rdb");
	if (rdb_node < 0)
		return BOLT_ERR;

	if (t->uart_base) {
		int serial_node;
		int serial_nwos;
		char pname[128];

		xsprintf(pname, "serial@%08x", t->uart_base);

		/* Get NWOS serial node */
		serial_nwos = fdt_subnode_offset(fdt_nwos, rdb_nwos, pname);
		if (serial_nwos < 0)
			return BOLT_ERR;

		/* Add serial node */
		rc = bolt_devtree_addnode_at(fdt, pname,
				rdb_node, &serial_node);
		if (rc)
			return rc;

		rc = tz_devtree_cpprop(fdt, serial_node,
			fdt_nwos, serial_nwos, "clock-frequency");
		if (rc)
			return rc;

		rc = tz_devtree_cpprop(fdt, serial_node,
			fdt_nwos, serial_nwos, "interrupt-names");
		if (rc)
			return rc;

		rc = tz_devtree_cpprop(fdt, serial_node,
			fdt_nwos, serial_nwos, "interrupts");
		if (rc)
			return rc;

		rc = tz_devtree_cpprop(fdt, serial_node,
			fdt_nwos, serial_nwos, "reg-io-width");
		if (rc)
			return rc;

		rc = tz_devtree_cpprop(fdt, serial_node,
			fdt_nwos, serial_nwos, "reg-shift");
		if (rc)
			return rc;

		rc = tz_devtree_cpprop(fdt, serial_node,
			fdt_nwos, serial_nwos, "reg");
		if (rc)
			return rc;

		rc = tz_devtree_cpprop(fdt, serial_node,
			fdt_nwos, serial_nwos, "compatible");
		if (rc)
			return rc;
	}

	return 0;
}

static int tz_populate_tzioc(void *fdt, bool tz_fdt)
{
	int rc;
	struct tz_info *t;
	struct tz_mem_layout *mem_layout;
	int tzioc_node;
	uint64_t tzioc_addr;
	uint64_t tzioc_size;
	uint64_t tzioc_regs[2];

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	/* Add tzioc node */
	rc = bolt_devtree_addnode_at(fdt, "tzioc", 0, &tzioc_node);
	if (rc)
		return rc;

	rc = bolt_dt_addprop_u32(fdt, tzioc_node, "#size-cells", 1);
	if (rc)
		return rc;

	rc = bolt_dt_addprop_u32(fdt, tzioc_node, "#address-cells", 1);
	if (rc)
		return rc;

	rc = bolt_dt_addprop_u32(fdt, tzioc_node, "irq", t->tzioc_irq);
	if (rc)
		return rc;

	mem_layout = t->mem_layout;
	if (!mem_layout)
		return BOLT_ERR;

	tzioc_addr = t->mem_addr + mem_layout->tzioc_offset;
	tzioc_size = mem_layout->tzioc_size;

	tzioc_regs[0] = cpu_to_fdt64(tzioc_addr);
	tzioc_regs[1] = cpu_to_fdt64(tzioc_size);

	rc = bolt_devtree_at_node_addprop(fdt, tzioc_node,
		"reg", tzioc_regs, sizeof(tzioc_regs));
	if (rc)
		return rc;

	rc = bolt_dt_addprop_str(fdt, tzioc_node, "compatible",
						"brcm,tzioc");
	if (rc)
		return rc;

	if (tz_fdt) {
		int node;
		uint32_t addr;
		uint32_t size;
		uint32_t regs[2];

		/* Add tz-heaps node */
		rc = bolt_devtree_addnode_at(fdt, "tz-heaps",
						tzioc_node, &node);
		if (rc)
			return rc;

		addr = mem_layout->tzheaps_offset;
		size = mem_layout->tzheaps_size;

		regs[0] = cpu_to_fdt32(addr);
		regs[1] = cpu_to_fdt32(size);

		rc = bolt_devtree_at_node_addprop(fdt, node, "reg",
						regs, sizeof(regs));
		if (rc)
			return rc;

		rc = bolt_dt_addprop_str(fdt, node, "compatible",
						"brcm,mem-heaps");
		if (rc)
			return rc;

		/* Add n2t-ring node */
		rc = bolt_devtree_addnode_at(fdt, "n2t-ring",
						tzioc_node, &node);
		if (rc)
			return rc;

		addr = mem_layout->n2t_offset;
		size = mem_layout->n2t_size;

		regs[0] = cpu_to_fdt32(addr);
		regs[1] = cpu_to_fdt32(size);

		rc = bolt_devtree_at_node_addprop(fdt, node, "reg",
						regs, sizeof(regs));
		if (rc)
			return rc;

		rc = bolt_dt_addprop_str(fdt, node, "compatible",
						"brcm,msg-ring");
		if (rc)
			return rc;

		/* Add t2n-ring node */
		rc = bolt_devtree_addnode_at(fdt, "t2n-ring",
						tzioc_node, &node);
		if (rc)
			return rc;

		addr = mem_layout->t2n_offset;
		size = mem_layout->t2n_size;

		regs[0] = cpu_to_fdt32(addr);
		regs[1] = cpu_to_fdt32(size);

		rc = bolt_devtree_at_node_addprop(fdt, node, "reg",
						regs, sizeof(regs));
		if (rc)
			return rc;

		rc = bolt_dt_addprop_str(fdt, node, "compatible",
						"brcm,msg-ring");
		if (rc)
			return rc;
	} else {
		int node;
		uint32_t addr;
		uint32_t size;
		uint32_t regs[2];

		/* Add nw-heaps node */
		rc = bolt_devtree_addnode_at(fdt, "nw-heaps",
						tzioc_node, &node);
		if (rc)
			return rc;

		addr = mem_layout->nwheaps_offset;
		size = mem_layout->nwheaps_size;

		regs[0] = cpu_to_fdt32(addr);
		regs[1] = cpu_to_fdt32(size);

		rc = bolt_devtree_at_node_addprop(fdt, node, "reg",
						regs, sizeof(regs));
		if (rc)
			return rc;

		rc = bolt_dt_addprop_str(fdt, node, "compatible",
						"brcm,mem-heaps");
		if (rc)
			return rc;
	}

	return 0;
}

static int tz_populate_nwos(void *fdt)
{
	int rc;
	int node;
	bolt_devtree_params_t p;

	/* Add nwos node */
	rc = bolt_devtree_addnode_at(fdt, "nwos", 0, &node);
	if (rc)
		return rc;

	rc = bolt_dt_addprop_u32(fdt, node, "kernel",
					BOOT_START_ADDRESS);
	if (rc)
		return rc;

	/* Get NWOS fdt */
	bolt_devtree_getenvs(&p);

	rc = bolt_dt_addprop_u32(fdt, node, "device-tree",
					(uint32_t)p.dt_address);
	if (rc)
		return rc;

	rc = bolt_dt_addprop_str(fdt, node, "compatible",
						"brcm,brcmstb");
	if (rc)
		return rc;

	return 0;
}

static int tz_populate_memory(void *fdt)
{
	int rc;
	struct tz_info *t;
	struct tz_mem_layout *mem_layout;
	int node;
	uint64_t addr;
	uint64_t size;
	uint64_t regs[2];

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	/* Add memory node */
	rc = bolt_devtree_addnode_at(fdt, "memory", 0, &node);
	if (rc)
		return rc;

	rc = bolt_dt_addprop_u32(fdt, node, "#size-cells", 1);
	if (rc)
		return rc;

	rc = bolt_dt_addprop_u32(fdt, node, "#address-cells", 1);
	if (rc)
		return rc;

	mem_layout = t->mem_layout;
	if (!mem_layout)
		return BOLT_ERR;

	addr = t->mem_addr + mem_layout->os_offset;
	size = mem_layout->os_size;

	regs[0] = cpu_to_fdt64(addr);
	regs[1] = cpu_to_fdt64(size);

	rc = bolt_devtree_at_node_addprop(fdt, node, "reg", regs, sizeof(regs));
	if (rc)
		return rc;

	rc = bolt_dt_addprop_str(fdt, node, "device_type", "memory");
	if (rc)
		return rc;

	return 0;
}

#define MAX_NUM_CPUS 4

static int tz_populate_cpus(void *fdt)
{
	int rc;
	int cpus, cpu;
	char cpu_pname[128];
	int cpus_node, cpu_node;

	bolt_devtree_params_t p;
	void *fdt_nwos;
	int cpus_nwos, cpu_nwos;

	/* Get NWOS device tree */
	bolt_devtree_getenvs(&p);

	fdt_nwos = p.dt_address;
	if (!fdt_nwos)
		return BOLT_ERR_BADADDR;

	/* Get NWOS cpus node */
	cpus_nwos = fdt_subnode_offset(fdt_nwos, 0, "cpus");
	if (cpus_nwos < 0)
		return BOLT_ERR;

	/* Add cpus node */
	rc = bolt_devtree_addnode_at(fdt, "cpus", 0, &cpus_node);
	if (rc)
		return rc;

	rc = bolt_dt_addprop_u32(fdt, cpus_node, "#size-cells", 0);
	if (rc)
		return rc;

	rc = bolt_dt_addprop_u32(fdt, cpus_node, "#address-cells", 1);
	if (rc)
		return rc;

	cpus = arch_get_num_processors();

	for (cpu = cpus - 1; cpu >= 0; cpu--) {
		xsprintf(cpu_pname, "cpu@%d", cpu);

		/* Get NWOS cpu node */
		cpu_nwos = fdt_subnode_offset(fdt_nwos,
						cpus_nwos, cpu_pname);
		if (cpu_nwos < 0)
			return BOLT_ERR;

		/* Add cpu node */
		rc = bolt_devtree_addnode_at(fdt, cpu_pname,
						cpus_node, &cpu_node);
		if (rc)
			return rc;

		rc = bolt_dt_addprop_u32(fdt, cpu_node, "reg", cpu);
		if (rc)
			return rc;

		rc = tz_devtree_cpprop(fdt, cpu_node,
			fdt_nwos, cpu_nwos, "clock-frequency");
		if (rc)
			return rc;

		rc = tz_devtree_cpprop(fdt, cpu_node,
			fdt_nwos, cpu_nwos, "enable-method");
		if (rc)
			return rc;

		rc = tz_devtree_cpprop(fdt, cpu_node,
			fdt_nwos, cpu_nwos, "compatible");
		if (rc)
			return rc;

		rc = bolt_dt_addprop_str(fdt, cpu_node, "device_type", "cpu");
		if (rc)
			return rc;
	}

	return 0;
}

static int tz_populate_chosen(void *fdt)
{
	int rc;
	struct tz_info *t;
	int node;
	char serial[128];

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	/* Add chosen node */
	rc = bolt_devtree_addnode_at(fdt, "chosen", 0, &node);
	if (rc)
		return rc;

	xsprintf(serial, "/rdb/serial@%08x:115200", t->uart_base);

	rc = bolt_dt_addprop_str(fdt, node, "astra,stdout-path", serial);
	if (rc)
		return rc;

	return 0;
}

static int tz_populate_model_and_compatible(void *fdt)
{
	int rc;
	bolt_devtree_params_t p;
	void *fdt_nwos;

	/* Get NWOS device tree */
	bolt_devtree_getenvs(&p);

	fdt_nwos = p.dt_address;
	if (!fdt_nwos)
		return BOLT_ERR_BADADDR;

	rc = bolt_dt_addprop_u32(fdt, 0, "#size-cells", 2);
	if (rc)
		return rc;

	rc = bolt_dt_addprop_u32(fdt, 0, "#address-cells", 2);
	if (rc)
		return rc;

	rc = tz_devtree_cpprop(fdt, 0, fdt_nwos, 0, "compatible");
	if (rc)
		return rc;

	rc = tz_devtree_cpprop(fdt, 0, fdt_nwos, 0, "model");
	if (rc)
		return rc;

	return 0;
}


#if NWOS_USE_RESERVED_MEM
static int tz_reserve_mem_nwos(void *fdt)
{
	int rc;
	struct tz_info *t;
	int rmem_node;
	int node;
	char pname[128];
	uint64_t addr;
	uint64_t size;
	uint64_t regs[2];

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	/* Get reserved-memory node */
	rmem_node = fdt_subnode_offset(fdt, 0, "reserved-memory");
	if (rmem_node < 0) {
		if (rmem_node != -FDT_ERR_NOTFOUND)
			return BOLT_ERR;

		/* Add reserved-memory node if not found */
		rc = bolt_devtree_addnode_at(fdt, "reserved-memory", 0,
								&rmem_node);
		if (rc)
			return rc;
	}

	addr = t->mem_addr;
	size = t->mem_size;

	xsprintf(pname, "reserved-tzos@%08x",
			(unsigned int)(addr & 0xffffffff));

	/* Add reserved-tzos node */
	rc = bolt_devtree_addnode_at(fdt, pname, rmem_node, &node);
	if (rc)
		return rc;

	regs[0] = cpu_to_fdt64(addr);
	regs[1] = cpu_to_fdt64(size);

	rc = bolt_devtree_at_node_addprop(fdt, node, "reg",
						regs, sizeof(regs));
	if (rc)
		return rc;

	return 0;
}

#else

static int tz_patch_memory_nwos(void *fdt)
{
	int rc;
	struct tz_info *t;
	int node;
	const struct fdt_property *prop;
	int prop_len;
	uint64_t *regs;
	int count;
	int i;

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	node = fdt_subnode_offset(fdt, 0, "memory");
	if (node < 0)
		return BOLT_ERR;

	prop = fdt_get_property(fdt, node, "reg", &prop_len);
	if (!prop)
		return BOLT_ERR;

	regs = KMALLOC(prop_len, 0);
	if (!regs)
		return BOLT_ERR_NOMEM;

	memcpy((void *)regs, prop->data, prop_len);
	count = prop_len / (sizeof(uint64_t)*2);

	for (i = 0; i < count; i++) {
		uint64_t addr;
		uint64_t size;

		addr = fdt64_to_cpu(regs[i*2]);
		size = fdt64_to_cpu(regs[i*2+1]);

		if ((addr + size) == (t->mem_addr + t->mem_size)) {
			size -= t->mem_size;
			regs[i*2+1] = cpu_to_fdt64(size);
			break;
		}
	}

	if (i == count) {
		rc = BOLT_ERR;
		goto out;
	}

	rc = bolt_devtree_at_node_addprop(fdt, node, "reg", regs, prop_len);

	/* fall through */

out:
	KFREE(regs);

	return rc;
}

#endif /* NWOS_USE_RESERVED_MEM */


static int tz_patch_chosen_nwos(void *fdt)
{
	int rc;
	struct tz_info *t;
	int node;
	const struct fdt_property *prop;
	int prop_len;
	char serial[128];

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	/* Get chosen node */
	node = fdt_subnode_offset(fdt, 0, "chosen");
	if (node < 0) {
		if (node != -FDT_ERR_NOTFOUND)
			return BOLT_ERR;

		/* Add chosen node if not found */
		rc = bolt_devtree_addnode_at(fdt, "chosen", 0, &node);
		if (rc)
			return rc;
	} else {
		/* Bolt devtree delprop considers -FDT_ERR_NOTFOUND an error */
		prop = fdt_get_property(fdt, node, "stdout-path", &prop_len);
		if (!prop) {
			if (prop_len != -FDT_ERR_NOTFOUND)
				return BOLT_ERR;

			/* Look for alternate name */
			prop = fdt_get_property(fdt, node, "linux,stdout-path",
								&prop_len);
			if (!prop) {
				if (prop_len != -FDT_ERR_NOTFOUND)
					return BOLT_ERR;
			} else {
				rc = bolt_devtree_at_node_delprop(fdt, node,
							"linux,stdout-path");
				if (rc)
					return rc;
			}
		} else {
			rc = bolt_devtree_at_node_delprop(fdt, node,
							"stdout-path");
			if (rc)
				return rc;
		}
	}

	xsprintf(serial, "/rdb/serial@%08x:115200", t->uart_nwos);

	rc = bolt_dt_addprop_str(fdt, node, "linux,stdout-path", serial);
	if (rc)
		return rc;

	return 0;
}

int tz_devtree_init(void)
{
	int rc;
	struct tz_info *t;
	void *fdt;

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	fdt = KMALLOC(DT_MAX_SIZE, 4096);
	if (!fdt)
		return BOLT_ERR_NOMEM;

	rc = fdt_create_empty_tree(fdt, DT_MAX_SIZE);
	if (rc)
		goto out;

	rc = tz_populate_rdb(fdt);
	if (rc)
		goto out;

	rc = tz_populate_tzioc(fdt, true);
	if (rc)
		goto out;

	rc = tz_populate_nwos(fdt);
	if (rc)
		goto out;

	rc = tz_populate_memory(fdt);
	if (rc)
		goto out;

	rc = tz_populate_cpus(fdt);
	if (rc)
		goto out;

	rc = tz_populate_model_and_compatible(fdt);
	if (rc)
		goto out;

	t->dt_addr = fdt;

	return 0;

out:
	KFREE(fdt);

	return rc;
}


int tz_devtree_init_nwos(void)
{
	int rc;
	struct tz_info *t;
	void *fdt;
	bolt_devtree_params_t p;
	char *dt_off;

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	bolt_devtree_getenvs(&p);

	fdt = p.dt_address;
	if (!fdt)
		return BOLT_ERR_BADADDR;

	dt_off = env_getenv("DT_OFF");
	if (dt_off && strcmp(dt_off, "1") == 0) {
		/* Bypass run-time DT patching */
		xprintf("DT_OFF: bypass device tree modification\n");
	} else {
		/* Run-time DT patching */
		rc = bolt_devtree_boltset(fdt);
		if (rc) {
			xprintf("ERROR: DT generation failed\n");
			return rc;
		}

		xprintf("DT_ON: DT modification is done\n");

		/* Turn off run-time DT patching */
		rc = env_setenv("DT_OFF", "1", ENV_FLG_BUILTIN);
		if (!rc)
			xprintf("DT_OFF: DT modification is now off\n");
	}

#if NWOS_USE_RESERVED_MEM
	rc = tz_reserve_mem_nwos(fdt);
#else
	rc = tz_patch_memory_nwos(fdt);
#endif
	if (rc)
		goto out;

	rc = tz_populate_tzioc(fdt, false);
	if (rc)
		goto out;

	if (t->uart_nwos) {
		rc = tz_patch_chosen_nwos(fdt);
		if (rc)
			goto out;
	}

	return 0;

out:
	return rc;
}


int tz_devtree_set(void)
{
	int rc;
	struct tz_info *t;
	void *fdt;

	t = tz_info();
	if (!t)
		return BOLT_ERR;

	/* Final update in case somethings have changed */
	fdt = t->dt_addr;

	if (t->uart_base) {
		rc = tz_populate_chosen(fdt);
		if (rc)
			goto out;

		rc = tz_populate_serial(fdt);
		if (rc)
			goto out;
	}

	return 0;

out:
	return rc;
}


int tz_devtree_set_nwos(void)
{
	/* Final update in case somethings have changed */
	return 0;
}

/* EOF */
