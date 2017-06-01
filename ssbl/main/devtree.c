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

#include "common.h"
#include "bolt.h"
#include "devtree.h"

#include "lib_printf.h"
#include "lib_string.h"
#include "env_subr.h"
#include "zimage.h"
/* below two required for arch-partions.h
*/
#include "iocb.h"
#include "dev_emmcflash.h"
#include "flash-partitions.h"
#include "bsp_config.h"

static int _dt_noise = 0;

int libfdt_error(int error);
#define _DTB_VAR(x, y) _binary_ ## x ## _config_dtb_ ## y
#define DTB_VAR(x, y) _DTB_VAR(x, y)

extern unsigned char DTB_VAR(ODIR, start);
extern unsigned char DTB_VAR(ODIR, end);
extern unsigned char DTB_VAR(ODIR, size);


/* ------------------------------------------------------------------------- */
/*                               API                                         */
/* ------------------------------------------------------------------------- */
int bolt_devtree_setnoise(int noise)
{
	int old = _dt_noise;
	_dt_noise = noise;
	return old;
}

void bolt_devtree_prep_builtin_dtb(void)
{
	uint8_t *d = NULL;
#ifdef CFG_DEVTREE_ADDRESS
	d = (uint8_t *)CFG_DEVTREE_ADDRESS;
#else
	d = KMALLOC(DT_MAX_SIZE, 4096); /* align to a 4K 'page' */
	if (!d)
		return;
#endif
	memset(d, 0, DT_MAX_SIZE);

	memcpy(d, (uint8_t *)&DTB_VAR(ODIR, start),
	       (uint32_t)&DTB_VAR(ODIR, size));

	(void)bolt_devtree_setenv((unsigned int)d,
				  (unsigned int)&DTB_VAR(ODIR, size));
}


int bolt_devtree_setenv(unsigned int address, unsigned int size)
{
	int error = DT_OK;
	char buffer[40];

	xsprintf(buffer, "%x", address);
	if (env_setenv("DT_ADDRESS", buffer, ENV_FLG_BUILTIN)) {
		xprintf("setenv DT_ADDRESS failed! %x\n", address);
		error = DT_ERR_SETENV;
	}

	xsprintf(buffer, "%x", size);
	if (env_setenv("DT_SIZE", buffer, ENV_FLG_BUILTIN)) {
		xprintf("DT: setenv DT_SIZE failed! %x\n", size);
		error = DT_ERR_SETENV;
	}

	return error;
}


int _bolt_devtree_close(void *fdt)
{
	int error = DT_OK;
	char buffer[40];
	uint32_t size;

	fdt_pack(fdt);

	size = fdt_totalsize(fdt);

	xsprintf(buffer, "%x",  size);
	if (env_setenv("DT_SIZE", buffer, ENV_FLG_BUILTIN)) {
		xprintf("DT: setenv_size DT_SIZE failed! %x\n", size);
		error = DT_ERR_SETENV;
	}

	return error;
}


/* ------------------------------------------------------------------------- */

void bolt_devtree_getenvs(bolt_devtree_params_t *params)
{
	char *dt_str;

	memset(params, 0, sizeof(*params));

	dt_str = env_getenv("DT_ADDRESS");
	if (dt_str)
		params->dt_address = xtop(dt_str);

	dt_str = env_getenv("DT_SIZE");
	if (dt_str)
		params->dt_size = xtoi(dt_str);

	dt_str = bolt_zimage_getenv_end();
	if (dt_str)
		params->dt_zimage_addr = xtop(dt_str);
}

/* ------------------------------------------------------------------------- */

int bolt_devtree_sane(const void *fdt, unsigned int *sz)
{
	unsigned int size;
	int rc;

	rc = fdt_check_header(fdt);
	if (rc)	{
		xprintf("DT: error %s\n", fdt_strerror(rc));
		return DT_ERR_NOT_A_DEVTREE;
	}

	size = fdt_totalsize(fdt);
	if (!size || (size > DT_MAX_SIZE)) {
		xprintf("DT: bad tree size %x (max is %#x) @ %x\n", size,
			DT_MAX_SIZE, (unsigned int)fdt);

		return DT_ERR_SIZE;
	}

	if (fdt_off_dt_strings(fdt) > size) {
		xprintf("DT: strings: %x > %x\n",
			fdt_off_dt_strings(fdt), size);
		return DT_ERR_SIZE;
	}

	if (fdt_off_dt_struct(fdt) > size) {
		xprintf("DT: structs: %x > %x\n",
			fdt_off_dt_strings(fdt), size);
		return DT_ERR_SIZE;
	}

	if ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION) ||
		(fdt_version(fdt) > FDT_LAST_SUPPORTED_VERSION)) {
		xprintf("DT: version: %x (%x to %x)\n", fdt_version(fdt),
				FDT_FIRST_SUPPORTED_VERSION,
				FDT_LAST_SUPPORTED_VERSION);
		return DT_ERR_RANGE;
	}

	if (sz)
		*sz = size;

	return DT_OK;
}


/* ------------------------------------------------------------------------- */

int bolt_devtree_test(void *fdt, void *presults, int noise)
{
	int res = 0;

	res = bolt_devtree_sane(fdt, (unsigned int *)presults);
	if (res) {
		if (noise)
			xprintf("DT: test %s\n", fdt_strerror(res));
		res = BOLT_ERR_UNSUPPORTED;
	} else {
		if (noise && presults)
			xprintf("DT: size is %d\n", *(unsigned int *)presults);
	}

	return res;
}

/* ------------------------------------------------------------------------- */

int bolt_devtree_at_node_addprop(void *fdt, int node,
		const char *prop_name, const void *data, int datalen)
{
	int rc = fdt_open_into(fdt, fdt, DT_MAX_SIZE);

	if (!rc) {
		rc = fdt_setprop(fdt, node, prop_name, data, datalen);

		_bolt_devtree_close(fdt);
	}

	return libfdt_error(rc);
}

int bolt_devtree_at_node_appendprop(void *fdt, int node,
		const char *prop_name, const void *data, int datalen)
{
	int rc = fdt_open_into(fdt, fdt, DT_MAX_SIZE);

	if (!rc) {
		rc = fdt_appendprop(fdt, node, prop_name, data, datalen);

		_bolt_devtree_close(fdt);
	}

	return libfdt_error(rc);
}

int bolt_devtree_at_node_delprop(void *fdt, int node, const char *prop_name)
{
	int rc = fdt_open_into(fdt, fdt, DT_MAX_SIZE);

	if (!rc) {
		rc = fdt_delprop(fdt, node, prop_name);

		_bolt_devtree_close(fdt);
	}

	return libfdt_error(rc);
}



/* ------------------------------------------------------------------------- */

int bolt_devtree_addprop_path(void *fdt, const char *path,
		const char *prop_name, const char *data, int datalen)
{
	int rc, offset;

	rc = bolt_devtree_node_from_path(fdt, path);
	if (rc >= 0) {
		offset = rc;
		rc = fdt_open_into(fdt, fdt, DT_MAX_SIZE);
		if (!rc) {
			rc = fdt_setprop(fdt, offset, prop_name, data, datalen);
			_bolt_devtree_close(fdt);
		}
	}
	return libfdt_error(rc);
}


int bolt_devtree_delprop_path(void *fdt, const char *path,
					const char *prop_name)
{
	int rc, offset;

	rc = bolt_devtree_node_from_path(fdt, path);
	if (rc >= 0) {
		offset = rc;
		rc = fdt_open_into(fdt, fdt, DT_MAX_SIZE);
		if (!rc) {
			rc = fdt_delprop(fdt, offset, prop_name);
			_bolt_devtree_close(fdt);
		}
	}
	return libfdt_error(rc);
}


/* ------------------------------------------------------------------------- */

int bolt_devtree_addnode_at(void *fdt, const char *node_name, int parent,
	int *us)
{
	int rc = BOLT_ERR;

	rc = fdt_open_into(fdt, fdt, DT_MAX_SIZE);
	if (!rc) {
		rc = fdt_add_subnode(fdt, parent, node_name);
		if (rc >= 0) {
			if (us)
				*us = rc;
			rc = BOLT_OK;
		}

		_bolt_devtree_close(fdt);
	}

	return libfdt_error(rc);
}


int bolt_devtree_delnode_at(void *fdt, const char *node_name, int parent)
{
	int rc = BOLT_ERR;

	rc = fdt_open_into(fdt, fdt, DT_MAX_SIZE);
	if (!rc) {
		rc = fdt_subnode_offset(fdt, parent, node_name);
		if (rc >= 0)
			rc = fdt_del_node(fdt, rc);

		_bolt_devtree_close(fdt);
	}

	return libfdt_error(rc);
}


/* ------------------------------------------------------------------------- */

int bolt_devtree_getprop_at(void *fdt, const char *node_name,
	const char *prop_name, void *data, int datalen, int *rtnlen, int parent)
{
	int lout = 0, rc = 0, bytes;
	const struct fdt_property *f;

	rc = fdt_subnode_offset(fdt, parent, node_name);
	if (rc < 0)
		return libfdt_error(rc);

	f = fdt_get_property(fdt, rc, prop_name, &lout);
	if (lout < 0)
		return libfdt_error(lout);

	bytes = min(datalen, lout);

	if (rtnlen)
		*rtnlen = bytes;

	memcpy(data, f->data, bytes);

	return libfdt_error(0);
}

int bolt_devtree_at_handle_getprop(void *fdt, int node_phandle,
	const char *prop_name, void *data, int datalen, int *rtnlen)
{
	int node_offs, lout = 0, bytes;
	const struct fdt_property *f;

	node_offs = fdt_node_offset_by_phandle(fdt, node_phandle);
	if (node_offs < 0)
		return libfdt_error(node_offs);

	f = fdt_get_property(fdt, node_offs, prop_name, &lout);
	if (lout < 0)
		return libfdt_error(lout);

	bytes = min(datalen, lout);

	if (rtnlen)
		*rtnlen = bytes;

	memcpy(data, f->data, bytes);

	return libfdt_error(0);
}

/* ------------------------------------------------------------------------- */

int bolt_devtree_at_handle_addprop(void *fdt, int node_phandle,
	const char *prop_name, void *data, int datalen)
{
	int rc = 0, node_offs;

	node_offs = fdt_node_offset_by_phandle(fdt, node_phandle);
	if (node_offs < 0)
		return libfdt_error(node_offs);

	rc = fdt_open_into(fdt, fdt, DT_MAX_SIZE);
	if (!rc) {
		rc = fdt_setprop(fdt, node_offs, prop_name, data, datalen);

		_bolt_devtree_close(fdt);
	}

	return libfdt_error(rc);
}


int bolt_devtree_at_handle_delprop(void *fdt, int node_phandle,
	const char *prop_name)
{
	int rc = 0, node_offs;

	node_offs = fdt_node_offset_by_phandle(fdt, node_phandle);
	if (node_offs < 0)
		return libfdt_error(node_offs);

	rc = fdt_open_into(fdt, fdt, DT_MAX_SIZE);
	if (!rc) {
		rc = fdt_delprop(fdt, node_offs, prop_name);

		_bolt_devtree_close(fdt);
	}

	return libfdt_error(rc);
}

/* ------------------------------------------------------------------------- */

int bolt_devtree_at_handle_delnode(void *fdt, int node_phandle)
{
	int rc = 0, node_offs;

	node_offs = fdt_node_offset_by_phandle(fdt, node_phandle);
	if (node_offs < 0)
		return libfdt_error(node_offs);

	rc = fdt_open_into(fdt, fdt, DT_MAX_SIZE);
	if (!rc) {
		rc = fdt_del_node(fdt, node_offs);

		_bolt_devtree_close(fdt);
	}

	return libfdt_error(rc);
}


/* ------------------------------------------------------------------------- */

static char *findstart(char *c, int *pos, int top)
{
	int n = *pos;
	while (*c == '\0') {
		if (n >= top)
			return NULL;
		c++; n++;
	 }
	*pos = n;
	 return c;
}


static char *nextstr(char *c, int *pos, int top)
{
	int n = *pos;
	while (*c != '\0') {
		if (n >= top)
			return NULL;
		c++; n++;
	 }
	*pos = n;
	 return findstart(c, pos, top);
}


/*  path /<node>/<subnode>/... Can use PARTIAL subnode, but which one
  it will pick is undefined.
*/
int bolt_devtree_node_from_path(void *fdt, const char *path)
{
	int offset = 0, pos = 0, bytes, i;
	char *node, *saved;

	if (!path)
		return -FDT_ERR_NOTFOUND;

	bytes = strlen(path);
	if (!bytes)
		return -FDT_ERR_NOTFOUND;

	if ((bytes == 1) && (path[0] == '/'))
		return 0; /* root node */

	node = strdup(path);
	if (!node)
		return -FDT_ERR_NOSPACE; /* KMALLOC() fail */

	saved = node; /* we advance node */

	for (i = 0; i < bytes; i++) {
		if (node[i] == '/')
			node[i] = '\0';
	}

	node = findstart(node, &pos, bytes);
	if (!node) {
		KFREE(saved);
		return -FDT_ERR_NOTFOUND;
	}

	do {
		offset = fdt_subnode_offset_namelen(fdt,
				offset, node, strlen(node));
		if (offset < 0)
			goto out;

		node = nextstr(node, &pos, bytes);
	} while (NULL != node);

out:
	KFREE(saved);
	return offset;
}

int bolt_devtree_subnode(void *fdt, const char *snode, int at)
{
	int offs;

	if (!snode)
		return BOLT_ERR;

	offs = fdt_subnode_offset(fdt, at, snode);
	if (offs < 0)
		return BOLT_ERR;

	return offs;
}


/* ------------------------------------------------------------------------- */

int bolt_devtree_add_memreserve(void *fdt, uint64_t address, uint64_t size)
{
	int rc = 0;

	rc = fdt_open_into(fdt, fdt, DT_MAX_SIZE);
	if (!rc) {
		rc = fdt_add_mem_rsv(fdt, address, size);

		_bolt_devtree_close(fdt);
	}

	return libfdt_error(rc);
}

int bolt_devtree_set_phandle(void *fdt, int at)
{
	unsigned int phandle = 0;
	unsigned int data;
	int offset;
	int rc;

	for (offset = fdt_next_node(fdt, -1, NULL); offset >= 0;
		offset = fdt_next_node(fdt, offset, NULL)) {
		phandle = max(phandle, fdt_get_phandle(fdt, offset));
	}

	phandle++;
	data = cpu_to_fdt32(phandle);

	rc = bolt_devtree_at_node_addprop(fdt, at, "phandle", &data,
					  sizeof(data));
	if (rc < 0)
		return rc;

	rc = bolt_devtree_at_node_addprop(fdt, at, "linux,phandle", &data,
					  sizeof(data));
	if (rc < 0)
		return rc;

	return phandle;
}

int bolt_devtree_count_aliases(void *fdt, const char *pattern)
{
	int count = 0;
	int offset = 0, depth = 0;
	const char *name;

	offset = fdt_next_node(fdt, offset, &depth);
	while (1) {
		if ((offset < 0) || (depth < 1))
			break;

		name = fdt_get_name(fdt, offset, NULL);
		if (!strncmp(name, pattern, strlen(pattern)))
			count++;

		offset = fdt_next_node(fdt, offset, &depth);
	}

	return count;
}

int bolt_devtree_phandle_from_path(void *fdt, const char *path)
{
	int node_offs;
	int rval;

	node_offs = bolt_devtree_node_from_path(fdt, path);
	if (node_offs < 0)
		return node_offs;

	rval = fdt_get_phandle(fdt, node_offs);
	if (rval <= 0)
		rval = bolt_devtree_set_phandle(fdt, node_offs);

	return rval;
}

int bolt_devtree_phandle_from_alias(void *fdt, const char *alias)
{
	char node[255];
	int rc, node_offs;
	unsigned int phandle;

	rc = bolt_devtree_getprop(fdt, "aliases", alias,
				node, sizeof(node), NULL);
	if (rc)
		return rc;

	node_offs = bolt_devtree_node_from_path(fdt, node);
	if (node_offs < 0)
		return node_offs;

	phandle = fdt_get_phandle(fdt, node_offs);
	if (phandle > 0)
		return phandle;

	/* Allocate a new phandle */
	rc = bolt_devtree_set_phandle(fdt, node_offs);
	if (rc)
		return rc;

	return phandle;
}

/* ------------------------------------------------------------------------- */
/*                    support functions                                      */
/* ------------------------------------------------------------------------- */

int libfdt_error(int error)
{
	if (_dt_noise && error)
		xprintf("DT: libftd error %s\n", fdt_strerror(error));
	return (error) ? BOLT_ERR : BOLT_OK;
}

