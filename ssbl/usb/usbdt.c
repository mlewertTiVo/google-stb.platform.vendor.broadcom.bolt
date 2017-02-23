/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_printf.h"
#include "lib_string.h"
#include "env_subr.h"

#include "libfdt_env.h"
#include "fdt.h"
#include "error.h"
#include "common.h"
#include "devtree.h"
#include "usbd.h"

#include "usbdt.h"
#include "board.h"
#include "bchp_sun_top_ctrl.h"
#include "chipid.h"
#include "usb-brcm-common-init.h"
#if defined(CONFIG_BCM7439B0)
#include "bchp_jtag_otp.h"
/* temporary define until bchp_jtag_otp.h is updated */
#define BCHP_JTAG_OTP_GENERAL_STATUS_3_usb_select_all_MASK 0x40
#endif

/* Defines for USB CAPS registers */
#define CAP_MAX_REGISTERS	12
#define CAP_ACTIVE		0x01
#define CAP_LAST		0x02
#define CAP_TYPE		0x0c
#define CAP_TYPE_SHIFT		0x02
#define CAP_TYPE_EHCI		0x00
#define CAP_TYPE_OHCI		0x01
#define CAP_ADDR		0xffffff00

/* Bits in the USB_SELECT environment var */
#define USB_SELECT_MASK_OHCI 0x01
#define USB_SELECT_MASK_EHCI 0x02
#define USB_SELECT_MASK_XHCI 0x04
#define USB_SELECT_MASK_BDC  0x08

struct prop_to_def {
	char *prop;
	int val;
} usb_device_props[] = {
	{"off", USB_CTLR_DEVICE_OFF},
	{"on", USB_CTLR_DEVICE_ON},
	{"dual", USB_CTLR_DEVICE_DUAL},
	{"typec_pd", USB_CTLR_DEVICE_TYPEC_PD},
};

struct usb_controller_list usb_clist;

static void *current_dtb;
static char dt_rdb_root[] = DT_RDB_DEVNODE_BASE_PATH;

static char *type_to_string(int type)
{
	switch (type) {
	case USB_PORT_TYPE_OHCI:
		return "ohci";
	case USB_PORT_TYPE_EHCI:
		return "ehci";
	case USB_PORT_TYPE_XHCI:
		return "xhci";
	case USB_PORT_TYPE_BDC:
		return "bdc";
	default:
		return "Unknown";
	}
}

static void dump_port(struct usb_port *port)
{
	xprintf("  Port - type: %s, addr: 0x%x, %s\n",
		type_to_string(port->type),
		port->regs, port->disabled ? "disabled" : "enabled");
}

static void dump_controller_list(struct usb_controller_list *clist)
{
	int x;
	int y;

	for (x = 0; x < clist->cnt; x++) {
		if (clist->ctrls[x].disabled) {
			xprintf("USB Controller %d, Disabled\n", x);
			continue;
		}
		xprintf("USB Controller %d, caps:0x%x ctrl:0x%x ", x,
			clist->ctrls[x].caps_regs,
			clist->ctrls[x].ctrl_regs);
		xprintf("ipp:0x%x ioc:0x%x device mode:%d\n",
			clist->ctrls[x].ipp, clist->ctrls[x].ioc,
			clist->ctrls[x].device_mode);
		for (y = 0; y < clist->ctrls[x].port_cnt; y++)
			dump_port(&clist->ctrls[x].ports[y]);
	}
}

static void usb_find_dt_ports(void *fdt, int *offset, int *depth,
			      struct usb_controller *ctl)
{
	struct usb_port *port;
	const struct fdt_property *prop;
	int proplen;
	const char *name;
	int type;
	char *ohci_name = "ohci@";
	char *ehci_name = "ehci@";
	char *xhci_name = "xhci@";
	char *bdc_name = "bdc@";

	while (1) {
		*offset = fdt_next_node(fdt, *offset, depth);
		if ((*offset < 0) || (*depth < 3))
			break;

		name = fdt_get_name(fdt, *offset, NULL);
		if (strncmp(name, ohci_name, strlen(ohci_name)) == 0)
			type = USB_PORT_TYPE_OHCI;
		else if (strncmp(name, ehci_name, strlen(ehci_name)) == 0)
			type = USB_PORT_TYPE_EHCI;
		else if (strncmp(name, xhci_name, strlen(xhci_name)) == 0)
			type = USB_PORT_TYPE_XHCI;
		else if (strncmp(name, bdc_name, strlen(bdc_name)) == 0)
			type = USB_PORT_TYPE_BDC;
		else
			continue;

		prop = fdt_get_property(fdt, *offset, "reg", &proplen);
		if (proplen < (int)((sizeof(uint32_t) * 2))) {
			xprintf("USB: error, %s node didn't have reg "
				"attribute\n", name);
			continue;
		}

		if (type == USB_PORT_TYPE_XHCI) {
			ctl->has_xhci = 1;
			/* XHCI EC does not exist on every chip */
			if (proplen >= (int)((sizeof(uint32_t) * 4)))
				ctl->xhci_ec_regs =
					DT_PROP_DATA_TO_U32(prop->data,
						2*sizeof(uint32_t));
		}
		port = &ctl->ports[ctl->port_cnt++];
		port->type = type;
		port->regs = DT_PROP_DATA_TO_U32(prop->data, 0);
		prop = fdt_get_property(current_dtb, *offset,
					"status", &proplen);
		if (prop && (strcmp(prop->data, "disabled") == 0))
			port->disabled = 1;
	}
}

static int is_in_caps(uint32_t reg, uint32_t caps_regs)
{
	int x;
	uint32_t cap;
	uint32_t *cregs = (unsigned int *)caps_regs;

	for (x = 0; x < CAP_MAX_REGISTERS; x++, cregs++) {
		cap = BDEV_RD((uint32_t)cregs);
		if ((cap & CAP_ACTIVE) && ((cap & CAP_ADDR) == reg))
			return 1;
	}
	return 0;
}

static void usb_validate_with_caps(struct usb_controller_list *clist)
{
	int x;
	int y;

	for (x = 0; x < clist->cnt; x++) {
		struct usb_controller *ctl = &clist->ctrls[x];

		if (ctl->disabled)
			continue;
		for (y = 0; y < ctl->port_cnt; y++) {
			struct usb_port *port = &ctl->ports[y];

			if (!is_in_caps(port->regs, ctl->caps_regs))
				port->disabled = 1;
		}
	}
}

static void usb_validate_with_select(struct usb_controller_list *clist)
{
	int x;
	int y;
	char *s;
	int val;

	s = env_getenv("USB_SELECT");
	if (s) {
		val = lib_atoi(s);
	} else {
#if !defined(CONFIG_BCM7439B0)
		return;
#else
		/* B0 - Just XHCI */
		if ((BDEV_RD(BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID)
				& CHIPID_REV_MASK) == 0x10)
			val = USB_SELECT_MASK_XHCI;
		/* All */
		else if (BDEV_RD(BCHP_JTAG_OTP_GENERAL_STATUS_3) &
			BCHP_JTAG_OTP_GENERAL_STATUS_3_usb_select_all_MASK)
			val = (USB_SELECT_MASK_OHCI | USB_SELECT_MASK_EHCI |
				USB_SELECT_MASK_XHCI | USB_SELECT_MASK_BDC);
		/* EHCI Only */
		else
			val = USB_SELECT_MASK_EHCI | USB_SELECT_MASK_BDC;
		xprintf("USB otp=%08x\n",
			BDEV_RD(BCHP_JTAG_OTP_GENERAL_STATUS_3) &
			BCHP_JTAG_OTP_GENERAL_STATUS_3_usb_select_all_MASK);

#endif
	}
	for (x = 0; x < clist->cnt; x++) {
		struct usb_controller *ctl = &clist->ctrls[x];

		if (ctl->disabled)
			continue;
		for (y = 0; y < ctl->port_cnt; y++) {
			struct usb_port *port = &ctl->ports[y];

			switch (port->type) {
			case USB_PORT_TYPE_OHCI:
				if ((val & USB_SELECT_MASK_OHCI) == 0)
					port->disabled = 1;
				break;
			case USB_PORT_TYPE_EHCI:
				if ((val & USB_SELECT_MASK_EHCI) == 0)
					port->disabled = 1;
				break;
			case USB_PORT_TYPE_XHCI:
				if ((val & USB_SELECT_MASK_XHCI) == 0)
					port->disabled = 1;
				break;
			case USB_PORT_TYPE_BDC:
				if ((val & USB_SELECT_MASK_BDC) == 0)
					port->disabled = 1;
				break;
			}
		}
	}
}

/* SWBOLT-277 */
struct rmprids {
	uint32_t product_id;
	uint32_t instance;
} rmprids[] = {
#if defined(CONFIG_BCM7445D0)
	{ 0x72520000, 1 },
	{ 0x74490000, 1 },
	{ 0x07252000, 1 },
	{ 0x07449000, 1 },
#endif
#if defined(CONFIG_BCM74371A0)
	/* no need for 7437a0 workaround, older version is not supported */
	{ 0x07437100, 1 },	/* 74371a0 */
#endif
};


static void usb_validate_with_product_id(struct usb_controller_list *clist)
{
	struct board_type *b;
	int x;

	b = board_thisboard();
	if (NULL == b)
		return;

	for (x = 0; x < (int)ARRAY_SIZE(rmprids); x++) {
		uint32_t prod_id = rmprids[x].product_id;
		uint32_t id = rmprids[x].instance;

		if (prod_id == (b->prid & ~CHIPID_REV_MASK))
			clist->ctrls[id].disabled = 1;
	}
}

static int add_status_disabled(const char *path)
{
	return bolt_devtree_addprop_path(current_dtb, path, "status",
					"disabled", 9);
}

static int dis_ctlr_node(uint32_t ctrl_addr)
{
	char node_name[80];
	int rc;

	xsprintf(node_name, "%s/usb@%8x", dt_rdb_root, ctrl_addr);
	rc = add_status_disabled(node_name);
	if (rc)
		return rc;
	xsprintf(node_name, "%s/usb-phy@%8x", dt_rdb_root, ctrl_addr);
	return add_status_disabled(node_name);
}

static int dis_port_node(uint32_t ctrl_addr, char *port_name,
			uint32_t port_addr)
{
	char node_name[80];
	int rc;

	xsprintf(node_name, "%s/usb@%x/%s@%x", dt_rdb_root, ctrl_addr,
		port_name, port_addr);
	rc = add_status_disabled(node_name);
	if (rc)
		return rc;
	xsprintf(node_name, "%s/%s_v2@%x", dt_rdb_root, port_name, port_addr);
	return add_status_disabled(node_name);
}

static int disable_xhci_phy(uint32_t ctrl_addr)
{
	char node_name[80];
	int node;

	xsprintf(node_name, "%s/usb-phy@%8x", dt_rdb_root,
		ctrl_addr);
	node = bolt_devtree_node_from_path(current_dtb, node_name);
	if (node < 0)
		return node;
	return bolt_devtree_at_node_delprop(current_dtb, node, "has_xhci");
}

/*
 * Remove both old and new style Device Tree nodes for any USB port
 * or controller that is disabled. This will prevent use by the kernel.
 */
void usb_remove_disabled_from_dt(struct usb_controller_list *clist)
{
	int x;
	int y;
	char err_dis_xhci_phy[] = "USB: Error disabling XHCI in PHY\n";

	for (x = 0; x < clist->cnt; x++) {
		struct usb_controller *ctl = &clist->ctrls[x];

		for (y = 0; y < ctl->port_cnt; y++) {
			struct usb_port *port = &ctl->ports[y];

			if (ctl->disabled || port->disabled) {
				/*
				 * If we're disabling XHCI, also disable
				 * it in the phy
				 */
				if (port->type == USB_PORT_TYPE_XHCI)
					if (disable_xhci_phy(ctl->ctrl_regs))
						xprintf(err_dis_xhci_phy);
				if (dis_port_node(
						ctl->ctrl_regs,
						type_to_string(port->type),
						port->regs))
					xprintf("USB: Error disabling"
						" node in Device Tree\n");
			}
		}
		if (ctl->disabled) {
			if (dis_ctlr_node(ctl->ctrl_regs))
				xprintf("USB: Error disabling node in USB%d\n",
					x);
		}
	}
}

static void set_single_or_override(void *fdt, int offset, const char *strnode,
				uint32_t *target, char *envar, char *strprop)
{
	const struct fdt_property *prop;
	int p, node, plen = 0;
	char *s;
	uint32_t data;

	p = bolt_devtree_node_from_path(fdt, dt_rdb_root);
	if (p < 0) {
		xprintf("USB init: no such parent node!\n");
		return;
	}

	s = env_getenv(envar);
	if (s) {
		*target = lib_atoi(s);
		data = cpu_to_fdt32(*target);
		node = bolt_devtree_subnode(fdt, strnode, p);
		if (node < 0) {
			xprintf("USB init: no such node\n");
			return;
		}
		if (bolt_devtree_at_node_addprop(fdt, node, strprop, &data,
					sizeof(data)))
			xprintf("USB init: devicetree write error %s for %s:%s\n",
					fdt_strerror(plen), strnode, strprop);
		else /* make some cluebat noise */
			xprintf("USB init: envar %s override %s: %s = %d\n",
					envar, strnode, strprop, *target);
	} else {
		prop = fdt_get_property(fdt, offset, strprop, &plen);
		if (prop && (plen >= (int)(sizeof(uint32_t))))
			*target = DT_PROP_DATA_TO_U32(prop->data, 0);
		else
			xprintf("USB init: devicetree read error %s for %s:%s\n",
					fdt_strerror(plen), strnode, strprop);
	}
}

static int device_prop_to_val(const char *name, int length)
{
	unsigned int x;

	for (x = 0;
	     x < (sizeof(usb_device_props) / sizeof(struct prop_to_def)); x++) {
		if (!strncmp(usb_device_props[x].prop, name, length))
			return usb_device_props[x].val;
	}
	return -1;
}

/*
 * Return the list of USB controllers and each controllers ports.
 *
 * The list is generated based on the USB entries in Device Tree.
 * Each port is further checked for a valid CAPS entry and, if not found
 * will be flagged as invalid. Only the valid entries should be used by
 * the caller.
 */
int usb_find_ports(struct usb_controller_list *clist)
{
	bolt_devtree_params_t params;
	const struct fdt_property *prop;
	char *top_node_name = "usb@";
	const char *name;
	int offset = 0, depth, x, len, proplen;
	char name_v2[80];

	memset(clist, 0, sizeof(*clist));
	bolt_devtree_getenvs(&params);
	current_dtb = params.dt_address;
	if (current_dtb == 0) {
		xprintf("No DTB specified\n");
		return 1;
	}

	depth = 0;
	offset = 0;
	x = 0;
	len = strlen(top_node_name);

	offset = fdt_next_node(current_dtb, offset, &depth);
	while (x < USB_MAX_CONTROLLERS) {
		if ((offset < 0) || (depth < 1))
			break;
		name = fdt_get_name(current_dtb, offset, NULL);
		if (strncmp(name, top_node_name, len) == 0) {
			prop = fdt_get_property(current_dtb, offset,
						"reg", &proplen);
			if (proplen < (int)((sizeof(uint32_t) * 2)))
				return 1;

			clist->ctrls[x].ctrl_regs =
				DT_PROP_DATA_TO_U32(prop->data, 0);
			clist->ctrls[x].caps_regs =
				clist->ctrls[x].ctrl_regs - 0x200;
			/* change the old style node */
			set_single_or_override(current_dtb, offset, name,
				&(clist->ctrls[x].ipp), "USBIPP", "ipp");
			set_single_or_override(current_dtb, offset, name,
				&(clist->ctrls[x].ioc), "USBIOC", "ioc");
			/* change the new style node */
			sprintf(name_v2, "usb-phy@%x",
				clist->ctrls[x].ctrl_regs);
			set_single_or_override(current_dtb, offset, name_v2,
				&(clist->ctrls[x].ipp), "USBIPP", "ipp");
			set_single_or_override(current_dtb, offset, name_v2,
				&(clist->ctrls[x].ioc), "USBIOC", "ioc");

			prop = fdt_get_property(current_dtb, offset,
						"device", &proplen);
			if (proplen > 0) {
				int val;

				val = device_prop_to_val(prop->data, proplen);
				if (val >= 0)
					clist->ctrls[x].device_mode = val;
				else
					xprintf("USB: bad \"device\" prop %s\n",
						prop->data);
			}
			usb_find_dt_ports(current_dtb, &offset, &depth,
					  &clist->ctrls[x]);
			x++;
		} else
			offset = fdt_next_node(current_dtb, offset, &depth);
	}
	clist->cnt = x;
	usb_validate_with_product_id(clist);
	usb_validate_with_caps(clist);
	usb_validate_with_select(clist);
	if (env_getenv("USBDT"))
		dump_controller_list(clist);
	return 0;
}


/*  *********************************************************************
    *  usb_dt_init()
    *
    *  Run the Devicetree init only
    *
    *  Input parameters:
    *	nothing
    *
    *  Return value:
    *	nothing
    *
    ********************************************************************* */
struct usb_controller_list *usb_dt_init(void)
{

	/* Find all USB ports from Device Tree */
	usb_find_ports(&usb_clist);

	/* Remove ports not listed as valid in USB CAPS */
	usb_remove_disabled_from_dt(&usb_clist);
	return &usb_clist;
}

