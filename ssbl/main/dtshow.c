/***************************************************************************
 *     Copyright (c) 2015, Broadcom Corporation
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


/* ------------------------------------------------------------------------- */
/*                    support functions                                      */
/* ------------------------------------------------------------------------- */

static int decode_as_bytes(unsigned char *data, int len)
{
	int i;

	xprintf("[ ");
	for (i = 0; i < len; i++)
		xprintf("%02x ", data[i]);
	xprintf("]");
	return len;
}

static int decode_bytes(unsigned char *data, int len)
{
	uint32_t d = 0;
	int count = 0;
	int i;

	/* If its binary data & not aligned u32 (u64) then
	  represent it as a string of bytes.
	*/
	if (len % 4)
		return decode_as_bytes(data, len);

	xprintf("<");
	for (i = 0; i < len; i++) {
		d = (d << 8U) | (uint32_t)data[i];

		count++;

		if (count == 4)	{
			xprintf("%#x", d);
			count = 0;
			d = 0;
			if (i < (len-1))
				xprintf(" ");
		}
	}
	xprintf(">");
	return len;
}

static void puttabs(int i)
{
	while (i--)
		xprintf("\t");
}

/* Allow NULLs in c string data (stringlist.)
 DT spec lacks type metadata, which is a PITA.
 NB: See ePAR 1.1, 2.2.4.2 Property Values
 Test for SPACE to one below DEL, or NULL
*/
static int is_char(unsigned char c)
{
	return (((c >= 32) && (c <= 126)) || (c == '\0'));
}

static int check_is_cstr(unsigned char *data, int len)
{
	int i;

	len--;

	for (i = 0; i < len; i++)	{
		/* check for 2 nulls in a row
		*/
		if (((i+1) <= len) &&
			(data[i] == '\0') && (data[i+1] == '\0'))
			return 0;

		if (!is_char(data[i]))
			return 0;
	}

	if (data[len] != '\0')
		return 0;

	return 1;
}

static void text_out(unsigned char *data, int len)
{
	int i;

	for (i = 0; i < len; i++) {
		if (data[i] == '\0')
			if ((i+1) == len)
				return;
			else
				xprintf("\", \"");
		else
			xprintf("%c", data[i]);
	}
}

static void decode_prop(unsigned char *data, int len)
{
	if (len) {
		xprintf(" = ");
		if (check_is_cstr(data, len)) {
			xprintf("\"");
			text_out(data, len);
			xprintf("\"");
		} else {
			decode_bytes(data, len);
		}
	}
	xprintf(";\n");
}

static int dump_prop(const void *fdt, unsigned int offset)
{
	struct fdt_property *prop;
	char *s;

	prop = (struct fdt_property *)fdt_offset_ptr(fdt,
					offset, sizeof(*prop));

	if (!prop)
		return 0;

	s = (char *)fdt_string(fdt, fdt32_to_cpu(prop->nameoff));
	if (s)
		xprintf("%s", s);
	else
		xprintf("<NONAME>");

	decode_prop((unsigned char *)prop->data, fdt32_to_cpu(prop->len));

	return 1;
}

static int decode_tags(void *fdt, struct dtb_to_dts_params *p)
{
	uint32_t tag;
	int tags = 0;
	int test;
	int nextoffset = -1;
	int orginal_depth;
	char *name;
	int first = 1;
	int prnode = 0;

	orginal_depth = p->depth;

	do {
		tag = fdt_next_tag(fdt, p->offset, &nextoffset);

		switch (tag) {
		case FDT_BEGIN_NODE:
			name = (char *)fdt_get_name(fdt, p->offset, NULL);
			if (((p->match && name) && !strncmp(p->match, name,
							 strlen(p->match)))
			    || prnode)
				prnode++;

			if (!p->match || prnode) {
				if (first)
					first = 0;
				else
					xprintf("\n");
				puttabs(p->depth);
				xprintf("%s", name);
				if (!p->justnodes)
					xprintf(" {\n");
			}
			p->depth += 1;
			break;

		case FDT_END_NODE:
			if ((!p->match || prnode) && !p->justnodes) {
				puttabs(p->depth - 1);
				xprintf("};\n");
			}
			if (prnode > 0)
				prnode--;
			p->depth -= 1;
			break;

		case FDT_PROP:
			if (p->justnodes)
				break;
			if (!p->match || prnode) {
				puttabs(p->depth);
				dump_prop(fdt, p->offset);
			}
			break;

		case FDT_NOP:
			break;

		case FDT_END:
			p->depth -= 1;
			break;
		}

		p->offset = nextoffset;
		tags++;
		test = (p->depth >= orginal_depth) && (tag != FDT_END);
	} while (test);

	if (p->justnodes)
		xprintf("\n");

	return 0;
}


/* Support for decode_paths()
 * --------------------------
 * As we iterate up and down the nodes, record where all
 * the previous parent nodes reside (i.e. their offset.)
 * We'll then play them back once we hit the first
 * 'match' node name.
 */
struct node_stack {
	queue_t queue;
	int offset;
};


static void *fdt_iter;

static void node_push(struct node_stack *root, int offset)
{
	struct node_stack *s;

	s = KMALLOC(sizeof(*s), 0);
	if (!s) {
		err_msg("node_push: queue malloc fail");
		return;
	}

	s->offset = offset;
	q_enqueue((queue_t *)root, (queue_t *)s);
}

static void node_pop_discard(struct node_stack *root)
{
	struct node_stack *s;

	if (q_count((queue_t *)root) < 1) {
		err_msg("node_pop_discard: empty");
		return;
	}

	s = (struct node_stack *)q_deqprev((queue_t *)root);
	KFREE(s);
}

/* Support for decode_paths()
 * --------------------------
 * Print out only the properties within
 * the parent node (offset) specified.
 */
static void print_all_node_properties(void *fdt, int node, int depth)
{
	int prop;

	for (prop = fdt_first_property_offset(fdt, node);
		prop >= 0;
		prop = fdt_next_property_offset(fdt, prop)) {

		puttabs(depth);
		dump_prop(fdt, prop);
	}
}

static int node_print_iter(queue_t *q, unsigned int a, unsigned int b)
{
	static int depth;
	struct node_stack *s = (struct node_stack *)q;
	char *name;

	if (!q) {
		depth = 0;
		return 0;
	}

	puttabs(depth);
	depth++;

	name = (char *)fdt_get_name(fdt_iter, s->offset, NULL);
	if (name && strlen(name))
		xprintf("%s ", name);

	xprintf("{\n");

	print_all_node_properties(fdt_iter, s->offset, depth);

	return 0;
}


/* Support for decode_paths()
 * --------------------------
 * Core prolog and epiliog code that wraps around
 * a 'matched' node or set of nodes.
 */
static void print_parent_nodes(struct node_stack *root)
{
	node_print_iter(NULL, 0, 0); /* reset depth */
	(void)q_map((queue_t *)root, node_print_iter, 0, 0);
	xprintf("\n");
}

static void print_parent_nodes_end(int depth)
{
	for (depth -= 1; depth > 0; depth--) {
		puttabs(depth);
		xprintf("};\n");
	}
}


/* ------------------------------------------------------------------------- */

static int decode_paths(void *fdt, struct dtb_to_dts_params *p)
{
	uint32_t tag;
	int test;
	int nextoffset = -1;
	int orginal_depth;
	char *name;
	int first = 1;
	int found_at_depth = 0;
	int prnode = 0;
	struct node_stack nstack;

	q_init((queue_t *)&nstack);

	orginal_depth = p->depth;

	fdt_iter = fdt;

	do {
		tag = fdt_next_tag(fdt, p->offset, &nextoffset);
		switch (tag) {
		case FDT_BEGIN_NODE:
			name = (char *)fdt_get_name(fdt, p->offset, NULL);
			if ((name && !strncmp(p->match, name, strlen(p->match)))
					|| prnode)
				prnode++;

			if (prnode) {
				if (first) {
					first = 0;
					found_at_depth = p->depth;
					print_parent_nodes(&nstack);
				} else
					xprintf("\n");

				puttabs(p->depth);
				xprintf("%s {\n", name);
			}

			node_push(&nstack, p->offset);
			p->depth += 1;
			break;

		case FDT_END_NODE:
			node_pop_discard(&nstack);
			if (prnode) {
				puttabs(p->depth - 1);
				xprintf("};\n");
			}

			if (prnode > 0)
				prnode--;
			p->depth -= 1;
			break;

		case FDT_PROP:
			if (!prnode)
				break;
			puttabs(p->depth);
			dump_prop(fdt, p->offset);
			break;

		case FDT_NOP:
			break;

		case FDT_END:
			p->depth -= 1;
			break;
		}

		p->offset = nextoffset;

		test = (p->depth >= orginal_depth) && (tag != FDT_END);
	} while (test);

	if (!first && found_at_depth)
		print_parent_nodes_end(found_at_depth);

	xprintf("};\n");

	return 0;
}


/* ------------------------------------------------------------------------- */
/*                                  API                                      */
/* ------------------------------------------------------------------------- */

int bolt_devtree_dtb_to_dts(void *fdt, struct dtb_to_dts_params *p)
{
	int num, n, rc;
	uint64_t address, size;

	if (bolt_devtree_sane(fdt, NULL))
			return BOLT_ERR;

	xprintf("/dts-v%d/;\n\n", 1 /* dtc can barf on 0x11 fdt_version(fdt)*/);

	num = fdt_num_mem_rsv(fdt);
	if (num > 0) {
		for (n = 0; n < num; n++) {
			rc = fdt_get_mem_rsv(fdt, n, &address, &size);
			if (!rc)
				xprintf("/memreserve/ %#llx %#llx;\n",
					address, size);
		}
		xprintf("\n");
	}

	xprintf("/"); /* root node */

	if (p->fullpaths)
		decode_paths(fdt, p);
	else
		decode_tags(fdt, p);

	return DT_OK;
}

