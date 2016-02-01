/***************************************************************************
 *     Copyright (c) 2012-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"
#include "lib_physio.h"

#include "ui_command.h"

#include "libfdt_env.h"
#include "fdt.h"

#include "bitops.h"
#include "bolt.h"
#include "common.h"
#include "error.h"
#include "devtree.h"
#include "env_subr.h"

static void *get_dtb(void)
{
	bolt_devtree_params_t params;
	void *dtb;

	bolt_devtree_getenvs(&params);

	dtb = params.dt_address;

	/* Every address except 0x0 is considered valid.
	*/
	if (!dtb)
		xprintf("No DTB specified\n");
	else
		xprintf("DTB @ %p\n", dtb);

	return dtb;
}

static int ui_cmd_dt_off(ui_cmdline_t *cmd, int argc, char *argv[])
{
	return env_setenv("DT_OFF", "1", ENV_FLG_BUILTIN);
}

static int ui_cmd_dt_on(ui_cmdline_t *cmd, int argc, char *argv[])
{
	/*
	 * Do not use delenv here; this is a temp override.  The user
	 * should use the unsetenv command if they wish to disable DT_OFF
	 * permanently.  This keeps behavior consistent for 'dt on' and
	 * 'dt off'
	 */
	return env_setenv("DT_OFF", "0", ENV_FLG_BUILTIN);
}

static int ui_cmd_dt_bolt(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int rc;
	const char *val;
	void *dtb = get_dtb();

	if (!dtb)
		return BOLT_ERR_BADADDR;

	val = env_getenv("DT_OFF");
	if (val && strcmp(val, "1") == 0) {
		xprintf("Command not done: dt modification is ");
		xprintf("off, use 'dt on' to re-enable\n");
		return BOLT_ERR;
	}

	rc = bolt_devtree_boltset(dtb);
	if (rc) {
		xprintf("ERROR: DT generation failed\n");
		return rc;
	}

	rc = env_setenv("DT_OFF", "1", ENV_FLG_BUILTIN);
	if (!rc) {
		xprintf("Note: dt modification is ");
		xprintf("now off, use 'dt on' to re-enable\n");
	}

	return rc;
}


static int ui_cmd_dt_decode(ui_cmdline_t *cmd, int argc, char *argv[])
{
	void *dtb = get_dtb();
	struct dtb_to_dts_params p;

	p.offset = 0;
	p.depth = 0;

	if (!dtb)
		return BOLT_ERR_BADADDR;

	if (argc >= 1)
		p.match = argv[0];
	else
		p.match = NULL;

	p.justnodes = cmd_sw_isset(cmd, "-nodes");
	p.fullpaths = cmd_sw_isset(cmd, "-paths");

	/* 1. 'justnodes' is not meant to do the full '{}' brace
	 * DT node/property, its just to give an overview of the
	 * hierarchy.
	 *
	 * 2. 'fullpaths' requires a match param as its not the
	 * full DT but a subset. 'dt show' with no params will
	 * print the whole thing anyway.
	 */
	if (p.fullpaths && (p.justnodes || !p.match))
		return BOLT_ERR_INV_PARAM;

	return bolt_devtree_dtb_to_dts(dtb, &p);
}


static int ui_cmd_dt_sane(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int rc;
	unsigned int size = 0;
	void *dtb = get_dtb();

	if (!dtb)
		return BOLT_ERR_BADADDR;

	rc = bolt_devtree_sane(dtb, &size);
	if (!rc) {
		xprintf("DTB reported size %#x @ %p\n", size, dtb);
		rc = bolt_devtree_setenv((unsigned int)dtb, size);
	}

	return rc;
}

static int ui_cmd_dt_addnode(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *path, *newnode;
	int parent;
	void *dtb = get_dtb();

	if (!dtb)
		return BOLT_ERR_BADADDR;

	path = cmd_getarg(cmd, 0);
	if (!path)
		return BOLT_ERR_INV_PARAM;

	newnode = cmd_getarg(cmd, 1);
	if (!newnode)
		return BOLT_ERR_INV_PARAM;

	parent = bolt_devtree_node_from_path(dtb, path);
	if (parent < 0)
		return BOLT_ERR_NOMORE;

	return bolt_devtree_addnode_at(dtb, newnode, parent, NULL);
}

static int ui_cmd_dt_delnode(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *path, *existingnode;
	int parent;
	void *dtb = get_dtb();

	if (!dtb)
		return BOLT_ERR_BADADDR;

	path = cmd_getarg(cmd, 0);
	if (!path)
		return BOLT_ERR_INV_PARAM;

	existingnode = cmd_getarg(cmd, 1);
	if (!existingnode)
		return BOLT_ERR_INV_PARAM;

	parent = bolt_devtree_node_from_path(dtb, path);
	if (parent < 0)
		return BOLT_ERR_NOMORE;

	return bolt_devtree_delnode_at(dtb, existingnode, parent);
}

static int ui_cmd_dt_delprop(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *path, *prop;
	void *dtb = get_dtb();

	if (argc < 2)
		return BOLT_ERR_INV_PARAM;

	if (!dtb)
		return BOLT_ERR_BADADDR;

	path = cmd_getarg(cmd, 0);
	if (!path)
		return BOLT_ERR_INV_PARAM;

	prop = cmd_getarg(cmd, 1);
	if (!prop)
		return BOLT_ERR_INV_PARAM;

	return bolt_devtree_delprop_path(dtb, path, prop);
}

static int ui_cmd_dt_addprop(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *path, *prop, *ptype, *tmp;
	unsigned int *data;
	int len = 0, i = 0, rc;
	void *dtb = get_dtb();

	if (argc < 3)
		return BOLT_ERR_INV_PARAM;

	if (!dtb)
		return BOLT_ERR_BADADDR;

	path = cmd_getarg(cmd, 0);
	if (!path)
		return BOLT_ERR_INV_PARAM;

	prop = cmd_getarg(cmd, 1);
	if (!prop)
		return BOLT_ERR_INV_PARAM;

	ptype = cmd_getarg(cmd, 2);
	if (!ptype)
		return BOLT_ERR_INV_PARAM;

	if (ptype[0] == 's') {
		data = (void *)cmd_getarg(cmd, 3);
		if (!data)
			return BOLT_ERR_INV_PARAM;
		len = strlen((char *)data) + 1;
		rc = bolt_devtree_addprop_path(dtb, path, prop,
				(char *)data, len);
	} else if (ptype[0] == 'i') {
		data = KMALLOC(DT_MAX_SIZE/4, 4);
		if (!data)
			return BOLT_ERR_NOMEM;

		for (i = 3; i < argc; i++) {
			tmp = cmd_getarg(cmd, i);
			if (!tmp) {
				KFREE(data);
				return BOLT_ERR_INV_PARAM;
			}
			data[i-3] = cpu_to_fdt32((unsigned int)xtoi(tmp));
			len += 4;
		}
		rc = bolt_devtree_addprop_path(dtb, path, prop,
				(char *)data, len);
		KFREE(data);
	} else if (ptype[0] == 'b') {
		rc = bolt_devtree_addprop_path(dtb, path, prop, NULL, 0);
	} else {
		return BOLT_ERR_INV_PARAM;
	}

	return rc;
}

static int ui_cmd_dt_memreserve(ui_cmdline_t *cmd, int argc, char *argv[])
{
	uint64_t addr;
	uint64_t len;
	const char *s;
	void *dtb = get_dtb();

	if (!dtb)
		return BOLT_ERR_BADADDR;

	s = cmd_getarg(cmd, 0);
	if (!s)
		return BOLT_ERR_INV_PARAM;
	addr = xtoq(s);

	s = cmd_getarg(cmd, 1);
	if (!s)
		return BOLT_ERR_INV_PARAM;
	len = xtoq(s);

	return bolt_devtree_add_memreserve(dtb, addr, len);
}

static int ui_cmd_dt_phandle(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *path;
	int phandle;
	void *dtb = get_dtb();

	if (!dtb)
		return BOLT_ERR_BADADDR;

	path = cmd_getarg(cmd, 0);
	if (!path)
		return BOLT_ERR_INV_PARAM;

	phandle = bolt_devtree_phandle_from_path(dtb, path);
	if (phandle < 0)
		return libfdt_error(phandle);

	xprintf("%s ", path);
	if (phandle)
		xprintf("phandle = 0x%x (%d)\n", phandle, phandle);
	else
		xprintf("has no phandle\n");

	return BOLT_OK;
}


int ui_init_devtree(void)
{
	cmd_addcmd("dt show", ui_cmd_dt_decode, NULL,
		"Decode contents of a memory resident DTB file as a DTS.\n"
		" The DT_ADDRESS envar is used as the base address of the dtb.\n",
		"dt show [-nodes|-paths] [partial or full node name]",
		"-nodes;show only the nodes|"
		"-paths;show full paths if node name is specified");

	cmd_addcmd("dt sane", ui_cmd_dt_sane, NULL,
		"Check DTB header.\n"
		"The DT_ADDRESS envar is used as the base address of the dtb.\n",
		"dt sane", "");

	cmd_addcmd("dt add node", ui_cmd_dt_addnode, NULL,
		"Add a new node to the DTB.\n"
		"The DT_ADDRESS envar is used as the base address of the dtb.\n",
		"dt add node [path] [name]", "");

	cmd_addcmd("dt del node", ui_cmd_dt_delnode, NULL,
		"Delete a node from the DTB.\n"
		"The DT_ADDRESS envar is used as the base address of the dtb.\n",
		"dt del node [path] [name]", "");

	cmd_addcmd("dt del prop", ui_cmd_dt_delprop, NULL,
		"Delete a property from the DTB.\n"
		"The DT_ADDRESS envar is used as the base address of the dtb.\n",
		"dt del prop [path] [property name]", "");

	cmd_addcmd("dt add prop", ui_cmd_dt_addprop, NULL,
		"Add a new property to the DTB.\n"
		"The DT_ADDRESS envar is used as the base address of the dtb.\n",
		"dt add prop [path] [property name] [type: s<tring>|i<nt>|b<oolean>] [\"string\" | 0xNNNNnnnn 0x...]",
		"");

	cmd_addcmd("dt memreserve", ui_cmd_dt_memreserve, NULL,
		"Add an entry to the DTB memory reservation table.\n"
		"The address and length shall be specified in hex.\n"
		"The DT_ADDRESS envar is used as the base address of the dtb.\n",
		"dt memreserve <address> <length>", "");

	cmd_addcmd("dt bolt", ui_cmd_dt_bolt, NULL,
		"Modify an existing DTB with an autogenerated BOLT config.\n"
		"The DT_ADDRESS envar is used as the base address of the dtb.\n",
		"dt bolt", "");

	cmd_addcmd("dt off", ui_cmd_dt_off, NULL,
		"Disable the silent 'dt bolt' command "
		"that happens just before the 'boot' command executes.\n",
		"dt off", "");

	cmd_addcmd("dt on", ui_cmd_dt_on, NULL,
		"(Re-)enable the silent 'dt bolt' command "
		"that happens just before the 'boot' command executes.\n",
		"dt on", "");

	cmd_addcmd("dt phandle", ui_cmd_dt_phandle, NULL,
		"Get or set the phandle associated with an existing node",
		"dt phandle [full path]", "");

	return 0;
}
