/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __DEVTREE_H__
#define __DEVTREE_H__

#include "byteorder.h"
#include "lib_types.h"
#include "loader.h"
#include "error.h"
#include "libfdt.h"

#define DT_DEVTREE_SIGNATURE	cpu_to_be32(0xD00DFEEDU)

#define DT_OK					 0
#define DT_ERR_NOT_IMPLEMENTED  -1
#define DT_ERR_NOT_A_ZIMAGE 	-2
#define DT_ERR_RANGE			-3
#define DT_ERR_NOT_A_DEVTREE	-4
#define DT_ERR_SETENV			-5
#define DT_ERR_SIZE 			-6
#define DT_ERR_PARAM 			-7

#define DT_DEFAULT_ADDRESS (unsigned int)-1
#define DT_DEFAULT_SIZE    (unsigned int)0

#define DT_NO_SETENV 0
#define DT_SETENV    1


/* Hard limit for sanity checking
*/
#define DT_MAX_SIZE    (64*1024)

/* Header size, in bytes
*/
#define DT_MAX_HEADER FDT_V17_SIZE

/* Macros to help extract property data */
#define U8TOU32(b,offs) \
	((((uint32_t)b[0+offs]<<0) &0x000000ff) | \
	 (((uint32_t)b[1+offs]<<8) &0x0000ff00) | \
	 (((uint32_t)b[2+offs]<<16)&0x00ff0000) | \
	 (((uint32_t)b[3+offs]<<24)&0xff000000))

#define DT_PROP_DATA_TO_U32(b, offs) (cpu_to_fdt32(U8TOU32(b, offs)))

/* This is the parent path for all RDB-derived device nodes */
#define DT_RDB_DEVNODE_BASE_PATH "/rdb"

/* ------------------------------------------------------------------------- */

typedef struct bolt_devtree_params_s
{
	unsigned int dt_size;
	void *dt_address;
	void *dt_zimage_addr;
}
bolt_devtree_params_t;


struct dtb_to_dts_params {
	int offset;
	int depth;
	char *match;
	int justnodes;
	int fullpaths;
};


struct dt_boot_mods {
	int loader_32bit;
};

/* ------------------------------------------------------------------------- */

void bolt_devtree_prep_builtin_dtb(void);

int bolt_devtree_setenv(unsigned int address, unsigned int size);

void bolt_devtree_getenvs(bolt_devtree_params_t *params);


/*	set the blather level, returns the old value.
*/
int bolt_devtree_setnoise(int noise);

/*	Check DTB header
*/
int bolt_devtree_sane(const void *fdt, unsigned int *sz);


/*	Dump to console a DTS for a DTB in memory
*/
int bolt_devtree_dtb_to_dts(void *fdt, struct dtb_to_dts_params *p);


/*	Used when we load a DTB to get its size.
*/
int bolt_devtree_test(void *fdt, void *presults, int noise);


/* Apply BOLT-isms to a dt
*/
int bolt_devtree_boltset(void *fdt);

/* Apply specific modifications to a dt, even if "dt off" has been done.
* This api should be called just before a Linux boot. Note: Does _not_
* apply to a BSU app boot.
*/
int bolt_devtree_boltset_boot(void *fdt, struct dt_boot_mods *bm);


/* Support functions
*/
int libfdt_error(int error);


/* ------------------------------------------------------------------------- */
/*	node find
*/
int bolt_devtree_node_from_path(void *fdt, const char *path);

/**
 * Get the offset of a node's subnode
 *
 * @fdt: FDT
 * @snode: subnode name
 * @at: parent node offset within FDT
 *
 * Return negative on error
 * On success, return the offset of the subnode with the FDT
 */
int bolt_devtree_subnode(void *fdt, const char *snode, int at);


/*	node add/rm, +root node helper defines.
*/
int bolt_devtree_addnode_at(void *fdt, const char *node_name, int parent,
	int *us);

int bolt_devtree_delnode_at(void *fdt, const char *node_name, int parent);


/*	property add/rm via complete node path
*/
int bolt_devtree_addprop_path(void *fdt, const char *path,
					const char *prop_name, const char *data, int datalen);

int bolt_devtree_delprop_path(void *fdt, const char *path,
					const char *prop_name);


/*	node & property functions via phandle reference.
*/
int bolt_devtree_at_handle_addprop(void *fdt, int node_phandle,
		const char *prop_name, void *data, int datalen);

int bolt_devtree_at_handle_delprop(void *fdt, int node_phandle,
	const char *prop_name);

int bolt_devtree_at_handle_delnode(void *fdt, int node_phandle);

static inline int bolt_devtree_at_handle_addprop_int(void *fdt,
		int node_phandle, const char *prop_name, uint32_t data)
{
	data = cpu_to_fdt32(data);
	return bolt_devtree_at_handle_addprop(fdt, node_phandle, prop_name,
					      &data, sizeof(data));
}

/*	property functions via existing node reference.
*/
int bolt_devtree_at_node_addprop(void *fdt, int node,
		const char *prop_name, const void *data, int datalen);

int bolt_devtree_at_node_appendprop(void *fdt, int node,
		const char *prop_name, const void *data, int datalen);

int bolt_devtree_at_node_delprop(void *fdt, int node, const char *prop_name);


/*	reading a property
*/
int bolt_devtree_getprop_at(void *fdt, const char *node_name,
		const char *prop_name, void *data, int datalen, /* in bytes */
		int *rtnlen, int parent);

int bolt_devtree_at_handle_getprop(void *fdt, int node_phandle,
	const char *prop_name, void *data, int datalen, int *rtnlen);

#define bolt_devtree_getprop(fdt, node, prop, data, datalen, rtnlen) \
		bolt_devtree_getprop_at(fdt, node, prop, data, datalen, rtnlen, 0)


/*	memory reservation table
*/
int bolt_devtree_add_memreserve(void *fdt, uint64_t address, uint64_t size);

/*
 * 	phandle manipulation
 */
/**
 * Create phandle properties for a node
 *
 * @fdt: FDT
 * @at: offset of node
 *
 * Return negative, if error.
 * Return phandle number, if success.
 */
int bolt_devtree_set_phandle(void *fdt, int at);

int bolt_devtree_count_aliases(void *fdt, const char *pattern);
int bolt_devtree_phandle_from_alias(void *fdt, const char *alias);
int bolt_devtree_phandle_from_path(void *fdt, const char *path);

void bolt_board_specific_mods(void *fdt);

/*
 * Property creation helpers
 */
static inline int bolt_dt_addprop_str(void *fdt, int at, const char *prop_name,
				      const char *str)
{
	return bolt_devtree_at_node_addprop(fdt, at, prop_name, str,
					    strlen(str) + 1);
}

static inline int bolt_dt_addprop_u32(void *fdt, int at, const char *prop_name,
				      uint32_t val)
{
	val = cpu_to_fdt32(val);
	return bolt_devtree_at_node_addprop(fdt, at, prop_name, &val,
					    sizeof(uint32_t));
}

static inline int bolt_dt_addprop_bool(void *fdt, int at, const char *prop_name)
{
	return bolt_devtree_at_node_addprop(fdt, at, prop_name, NULL, 0);
}


int bolt_devtree_compile_prop_value(void *fdt, const char *in,
				    unsigned char *out,
				    int out_size);

#endif /* __DEVTREE_H__ */

