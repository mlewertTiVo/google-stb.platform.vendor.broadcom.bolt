/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

/*  *********************************************************************
    *  USB-Ethernet driver - BOLT Network Layer Interfaces
    *  NOTE: Some of the device setup for the Admtek & Realtek devices
    *  was derived from reverse engineering! So these interfaces
    *        assume proper device operation.
    ********************************************************************* */

#include "lib_types.h"
#include "lib_malloc.h"
#include "lib_string.h"
#include "lib_printf.h"
#include "lib_hexdump.h"

#include "iocb.h"
#include "ioctl.h"
#include "timer.h"
#include "device.h"
#include "devfuncs.h"
#include "error.h"

#include "env_subr.h"

#include "usbd.h"
#include "usbeth.h"

#if CFG_NETWORK
#include "net_ebuf.h"
#include "net_api.h"
#endif

#define FAIL -1


#define USBETH_TRACE(x, ...)	{\
					if (usbeth_trace) {\
						xprintf("%s: ", __func__);\
						xprintf(x "\n", __VA_ARGS__);\
					} \
				}

static int Dev_cnt = 0;
static int usbeth_trace;

/******************************************************************************
  Debug functions
******************************************************************************/

/* To enable hexdump() of incoming packets */
/* #define DATA_DUMP */

/*  *********************************************************************
    * Interface functions for USB-Ethernet adapters
    ********************************************************************* */

enum { PEGASUS, PEGASUS_II, NETMATE, REALTEK, ASIX, ASIX772A, ASIX772B, ASIX178,
	USB_CDC,
	MAX_CHIP_ID
};
enum { VEN_NONE, _3_COM, LINKSYS, LINKSYS_10, LINKSYS_100,
	CATC_NM, BELKIN_CATC, BELKIN, LINKSYS_100M, SIEMENS,
	HAWKING, D_LINK, NETGEAR, GEN_PEG, GEN_ASIX, CDC,
	MAX_VEN_ID
};
static char *VENDOR_NAMES[] = {
	"?", "3-COM", "LinkSys", "LinkSys-10TX", "LinkSys-100TX",
	"CATC-Netmate", "Belkin/CATC", "Belkin", "Linksys-100M", "Siemens",
	"Hawking", "D-Link", "Netgear", "Pegasus-Based", "ASIX-Based",
	"USB CDC",
	"Yikes!"
};

typedef struct usbeth_softc_s usbeth_softc_t;

typedef struct usbeth_devif_s {
	void (*init)(usbeth_softc_t *softc);
	void (*open)(usbeth_softc_t *softc);
	void (*close)(usbeth_softc_t *softc);
	int (*rx)(usbeth_softc_t *softc, uint8_t *buf);
} usbeth_devif_t;

#define RX_PACKET_SIZE 2048 /* arbitrary but enough for ethernet packet */

struct usbeth_softc_s {
	usbdev_t *dev;
	usbeth_devif_t *intf;
	int bulk_inpipe;
	int bulk_outpipe;
	int dev_id;
	int ven_code;
	int embed_tx_len;
	uint8_t mac_addr[6];
	usbreq_t *rx_ur;
	uint8_t *rxbuf;
	char *fullname;
};

static void usbeth_queue_rx(usbeth_softc_t *softc);

/* **************************************
   *  CATC I/F Functions
   ************************************** */

#if 0
static int catc_get_reg(usbdev_t *dev, int16_t reg, uint8_t *val)
{
	return usb_std_request(dev, (USBREQ_TYPE_VENDOR | USBREQ_DIR_IN),
			       CATC_GET_REG, 0, reg, val, 1);
}
#endif

static int catc_set_reg(usbdev_t *dev, int16_t reg, int16_t val)
{
	return usb_std_request(dev, (USBREQ_TYPE_VENDOR | USBREQ_DIR_OUT),
			       CATC_SET_REG, val, reg, NULL, 0);
}

static int catc_set_mem(usbdev_t *dev, int16_t addr,
			uint8_t *data, int16_t len)
{
	return usb_std_request(dev, (USBREQ_TYPE_VENDOR | USBREQ_DIR_OUT),
			       CATC_SET_MEM, 0, addr, data, len);
}

static int catc_get_mac_addr(usbdev_t *dev, uint8_t *mac_addr)
{
	int status;

	status = usb_std_request(dev, (USBREQ_TYPE_VENDOR | USBREQ_DIR_IN),
				 CATC_GET_MAC_ADDR, 0, 0, mac_addr, 6);
	return status;
}

static void catc_init_device(usbeth_softc_t *softc)
{
	usbdev_t *dev = softc->dev;
	unsigned char *mcast_tbl;

	softc->embed_tx_len = 2;

	usb_std_request(dev, (USBREQ_TYPE_STD | USBREQ_REC_INTERFACE),
		USB_REQUEST_SET_INTERFACE, 1, /* alt setting 1 */ 0, NULL, 0);

	catc_set_reg(dev, CATC_TX_BUF_CNT_REG, 0x04);
	catc_set_reg(dev, CATC_RX_BUF_CNT_REG, 0x10);
	catc_set_reg(dev, CATC_ADV_OP_MODES_REG, 0x01);
	catc_set_reg(dev, CATC_LED_CTRL_REG, 0x08);

	/* Enable broadcast rx via bit in mutlicast table */
	mcast_tbl = KMALLOC(64, DMA_BUF_ALIGN);
	memset(mcast_tbl, 0, 64);
	mcast_tbl[31] = 0x80;	/* i.e. broadcast bit */
	catc_set_mem(dev, CATC_MCAST_TBL_ADDR, mcast_tbl, 64);
	KFREE(mcast_tbl);

	/* Read the adapter's MAC addr */
	catc_get_mac_addr(dev, softc->mac_addr);
}

static void catc_close_device(usbeth_softc_t *softc)
{
	/* Now disable adapter from receiving packets */
	catc_set_reg(softc->dev, CATC_ETH_CTRL_REG, 0);
}

static void catc_open_device(usbeth_softc_t *softc)
{
	int i;

	for (i = 0; i < 6; ++i)
		catc_set_reg(softc->dev, (CATC_ETH_ADDR_0_REG - i),
			     softc->mac_addr[i]);

	/* Now enable adapter to receive packets */
	catc_set_reg(softc->dev, CATC_ETH_CTRL_REG, 0x09);
}

static const usbeth_devif_t catc_if = {
	catc_init_device,
	catc_open_device,
	catc_close_device,
	NULL
};

/* **************************************
   *  PEGASUS I/F Functions
   ************************************** */

static int peg_get_reg(usbdev_t *dev, int16_t reg, uint8_t *val, int16_t len)
{
	return usb_std_request(dev, (USBREQ_TYPE_VENDOR | USBREQ_DIR_IN),
			       PEG_GET_REG, 0, reg, val, len);
}

static int peg_set_reg(usbdev_t *dev, int16_t reg, int16_t val)
{
	unsigned char data = (uint8_t) val & 0xff;

	return usb_std_request(dev, (USBREQ_TYPE_VENDOR | USBREQ_DIR_OUT),
			       PEG_SET_REG, val, reg, &data, 1);
}

static int peg_set_regs(usbdev_t *dev, int16_t reg, uint8_t *vals,
			int16_t len)
{
	return usb_std_request(dev, (USBREQ_TYPE_VENDOR | USBREQ_DIR_OUT),
			       PEG_SET_REG, 0, reg, vals, len);
}

static int peg_get_eep_word(usbdev_t *dev, int16_t ofs, uint8_t *val)
{
	int status = 0, tries = 20;
	uint8_t data[2];

	if (peg_set_reg(dev, PEG_EEPROM_CTL_REG, 0) == FAIL)
		return FAIL;
	if (peg_set_reg(dev, PEG_EEPROM_OFS_REG, ofs) == FAIL)
		return FAIL;
	if (peg_set_reg(dev, PEG_EEPROM_CTL_REG, 0x02) == FAIL)	/* read */
		return FAIL;

	while (--tries) {
		if (peg_get_reg(dev, PEG_EEPROM_CTL_REG, data, 1) == FAIL)
			return FAIL;
		if (data[0] & 0x04)
			break;	/* eeprom data ready */
	}
	if (!tries) {
		xprintf("Pegasus Eeprom read failed!\n");
		return FAIL;
	}
	if (peg_get_reg(dev, PEG_EEPROM_DATA_REG, data, 2) == FAIL)
		return FAIL;
	val[0] = data[0];
	val[1] = data[1];

	return status;
}

static int peg_get_mac_addr(usbdev_t *dev, uint8_t *mac_addr)
{
	int i, status;

	for (i = 0; i < 3; ++i)
		status = peg_get_eep_word(dev, i, &mac_addr[i * 2]);

	return status;
}

static void peg_init_phy(usbdev_t *dev)
{
	/* needed for earlier versions (before Rev B)
	of the USB-100TX adapters */
	static uint8_t phy_magic_wr[] = { 0, 4, 0, 0x1b };
	static uint8_t read_status[] = { 0, 0, 0, 1 };
	uint8_t data[4];

	/* reset the MAC ans set up GPIOs */
	peg_set_reg(dev, PEG_ETH_CTL1_REG, 0x08);
	peg_get_reg(dev, PEG_ETH_CTL1_REG, data, 1);

	/* do following steps to enable link activitiy LED */
	peg_set_reg(dev, PEG_GPIO1_REG, 0x26);
	peg_set_reg(dev, PEG_GPIO0_REG, 0x24);
	peg_set_reg(dev, PEG_GPIO0_REG, 0x26);

	/* do following set of steps to enable LINK LED */
	memcpy(data, phy_magic_wr, 4);

	/* set up for magic word */
	peg_set_regs(dev, PEG_PHY_ADDR_REG, data, 4);
	peg_set_reg(dev, PEG_PHY_CTRL_REG, (0x1b | PHY_WRITE));

	/* read status of write */
	peg_get_reg(dev, PEG_PHY_CTRL_REG, data, 1);
	memcpy(data, read_status, 4);

	/* set up for phy status reg */
	peg_set_regs(dev, PEG_PHY_ADDR_REG, data, 4);
	peg_set_reg(dev, PEG_PHY_CTRL_REG, (1 | PHY_READ));

	/* read status of read */
	peg_get_reg(dev, PEG_PHY_CTRL_REG, data, 1);

	/* read status regs */
	peg_get_reg(dev, PEG_PHY_DATA_REG, data, 2);
}

static void peg_init_device(usbeth_softc_t *softc)
{
	usbdev_t *dev = softc->dev;

	softc->embed_tx_len = 2;

	if (softc->dev_id == PEGASUS_II) /* enable internal PHY */
		peg_set_reg(dev, PEG_INT_PHY_REG, 0x02);
	else
		peg_init_phy(dev);

	/* Read the adapter's MAC addr */
	peg_get_mac_addr(dev, softc->mac_addr);
}

static void peg_close_device(usbeth_softc_t *softc)
{
	/* Now disable adapter from receiving or transmitting packets */
	peg_set_reg(softc->dev, PEG_ETH_CTL1_REG, 0);
}

static void peg_open_device(usbeth_softc_t *softc)
{
	usbdev_t *dev = softc->dev;

	/* Now setup adapter's receiver with MAC address */
	peg_set_regs(dev, PEG_MAC_ADDR_0_REG, softc->mac_addr, 6);

	/* Now enable adapter to receive and transmit packets */
	peg_set_reg(dev, PEG_ETH_CTL0_REG, 0xc1);
	peg_set_reg(dev, PEG_ETH_CTL1_REG, 0x30);
}

static const usbeth_devif_t peg_if = {
	peg_init_device,
	peg_open_device,
	peg_close_device,
	NULL
};

/* **************************************
   *  REALTEK I/F Functions
   ************************************** */

static int rtek_get_reg(usbdev_t *dev, int16_t reg, uint8_t *val, int16_t len)
{
	return usb_std_request(dev, (USBREQ_TYPE_VENDOR | USBREQ_DIR_IN),
			       RTEK_REG_ACCESS, reg, 0, val, len);
}

static int rtek_set_reg(usbdev_t *dev, int16_t reg, int16_t val)
{
	unsigned char data = (uint8_t) val & 0xff;

	return usb_std_request(dev, (USBREQ_TYPE_VENDOR | USBREQ_DIR_OUT),
			       RTEK_REG_ACCESS, reg, 0, &data, 1);
}

static int rtek_get_mac_addr(usbdev_t *dev, uint8_t *mac_addr)
{
	int status;

	status = rtek_get_reg(dev, RTEK_MAC_REG, mac_addr, 6);

	return status;
}

static void rtek_init_device(usbeth_softc_t *softc)
{
	int i;
	usbdev_t *dev = softc->dev;
	uint8_t val;

	softc->embed_tx_len = 0;

	/* Reset the adapter */
	rtek_set_reg(dev, RTEK_CMD_REG, RTEK_RESET);
	for (i = 0; i < 10; ++i) {
		rtek_get_reg(dev, RTEK_CMD_REG, &val, 1);
		if (!(val & RTEK_RESET))
			break;
		usb_delay_ms(NULL, 1);
	}

	/* autoload the internal registers */
	rtek_set_reg(dev, RTEK_CMD_REG, RTEK_AUTOLOAD);
	for (i = 0; i < 50; ++i) {
		rtek_get_reg(dev, RTEK_CMD_REG, &val, 1);
		if (!(val & RTEK_AUTOLOAD))
			break;
		usb_delay_ms(NULL, 1);
	}

	/* Read the adapter's MAC addr */
	rtek_get_mac_addr(dev, softc->mac_addr);
}

static void rtek_close_device(usbeth_softc_t *softc)
{
	/* Now disable adapter from receiving or transmitting packets */
	rtek_set_reg(softc->dev, RTEK_CMD_REG, 0);
}

static void rtek_open_device(usbeth_softc_t *softc)
{
	/* accept broadcast & own packets */
	rtek_set_reg(softc->dev, RTEK_RXCFG_REG, 0x0c);

	/* Now enable adapter to receive and transmit packets */
	rtek_set_reg(softc->dev, RTEK_CMD_REG, 0x0c);
}

static const usbeth_devif_t rtek_if = {
	rtek_init_device,
	rtek_open_device,
	rtek_close_device,
	NULL
};

/* **************************************
   *  ASIX 88172/88772/88178 I/F Functions
   ************************************** */

static int asix_get_reg(usbdev_t *dev, uint8_t cmd, int16_t val, int16_t index,
			uint8_t *buf, int16_t len)
{
	return usb_std_request(dev, (USBREQ_TYPE_VENDOR | USBREQ_DIR_IN),
			       cmd, val, index, buf, len);
}

static int asix_set_reg(usbdev_t *dev, uint8_t cmd, int16_t val, int16_t index,
			uint8_t *buf, int16_t len)
{
	return usb_std_request(dev, (USBREQ_TYPE_VENDOR | USBREQ_DIR_OUT),
			       cmd, val, index, buf, len);
}

static int asix_get_mac_addr(usbeth_softc_t *softc, uint8_t *mac_addr)
{
	int status;

	if (softc->dev_id == ASIX)
		status =
		    asix_get_reg(softc->dev, ASIX_MAC_ADDR_CMD, 0, 0, mac_addr,
				 6);
	else
		status =
		    asix_get_reg(softc->dev, ASIX2_MAC_ADDR_CMD, 0, 0, mac_addr,
				 6);

	return status;
}

static void asix_init_device(usbeth_softc_t *softc)
{
	if (softc->dev_id != ASIX)
		softc->embed_tx_len = 4;

	/* Read the adapter's MAC addr */
	asix_get_mac_addr(softc, softc->mac_addr);
}

static void asix_close_device(usbeth_softc_t *softc)
{
	/* Now disable adapter from receiving packets */
	asix_set_reg(softc->dev, ASIX_RXCTL_CMD, 0, 0, NULL, 0);
}

static void asix_open_device(usbeth_softc_t *softc)
{
	uint8_t data[2];
	int16_t phyid;

	asix_set_reg(softc->dev, ASIX_MII_SWOP_CMD, 0, 0, NULL, 0);
	asix_get_reg(softc->dev, ASIX_PHYID_CMD, 0, 0, data, 2);

	/* UF200 seems to need a GPIO settings */
	asix_set_reg(softc->dev, ASIX_GPIO_WRITE_CMD, 0x11, 0, NULL, 0);
	asix_set_reg(softc->dev, ASIX_GPIO_WRITE_CMD, 0x13, 0, NULL, 0);
	asix_set_reg(softc->dev, ASIX_GPIO_WRITE_CMD, 0x0c, 0, NULL, 0);

	phyid = data[1];
	data[0] = 0;
	data[1] = 0;
	asix_set_reg(softc->dev, ASIX_MII_WRITE_CMD, phyid, 0, data, 2);
	data[1] = 0x80;
	asix_set_reg(softc->dev, ASIX_MII_WRITE_CMD, phyid, 0, data, 2);
	asix_set_reg(softc->dev, ASIX_MED_WRITE_CMD, 6, 0, NULL, 0);
	asix_set_reg(softc->dev, ASIX_IPG1_CMD, 0x15, 0, NULL, 0);
	asix_set_reg(softc->dev, ASIX_IPG2_CMD, 0x0c, 0, NULL, 0);
	asix_set_reg(softc->dev, ASIX_IPG3_CMD, 0x12, 0, NULL, 0);
	data[0] = 0x01;
	data[1] = 0x05;
	asix_set_reg(softc->dev, ASIX_MII_WRITE_CMD, phyid, 4, data, 2);
	data[0] = 0;
	data[1] = 0x12;
	asix_set_reg(softc->dev, ASIX_MII_WRITE_CMD, phyid, 0, data, 2);
	asix_set_reg(softc->dev, ASIX_MII_HWOP_CMD, 0, 0, NULL, 0);
	asix_set_reg(softc->dev, ASIX_RXCTL_CMD, 0x81, 0, NULL, 0);
}

static void asix772_open_device(usbeth_softc_t *softc)
{
	uint8_t data[2];

	asix_get_reg(softc->dev, ASIX_PHYID_CMD, 0, 0, data, 2);
	asix_set_reg(softc->dev, ASIX_GPIO_WRITE_CMD, 0xb0, 0, NULL, 0);

	asix_set_reg(softc->dev, ASIX_RXCTL_CMD, 0, 0, NULL, 0);
	asix_set_reg(softc->dev, ASIX_MII_SWOP_CMD, 0, 0, NULL, 0);

	asix_set_reg(softc->dev, 0x20, 0x48, 0, NULL, 0);
	asix_set_reg(softc->dev, 0x20, 0x08, 0, NULL, 0);
	asix_set_reg(softc->dev, 0x20, 0x28, 0, NULL, 0);

	data[0] = 0;
	data[1] = 0x80;
	asix_set_reg(softc->dev, ASIX_MII_WRITE_CMD, 0x03, 0, data, 2);
	data[0] = 0xe1;
	data[1] = 0x05;
	asix_set_reg(softc->dev, ASIX_MII_WRITE_CMD, 0x03, 0x04, data, 2);
	data[0] = 0;
	data[1] = 0x33;
	asix_set_reg(softc->dev, ASIX_MII_WRITE_CMD, 0x03, 0, data, 2);

	asix_set_reg(softc->dev, ASIX_MED_WRITE_CMD, 0x336, 0, NULL, 0);
	asix_set_reg(softc->dev, ASIX_MII_HWOP_CMD, 0, 0, NULL, 0);

	/* change rx packet length length if burst size != 2048 */
	asix_set_reg(softc->dev, ASIX_RXCTL_CMD, 0x88, 0, NULL, 0);

	if (softc->dev_id == ASIX772A) {
		asix_set_reg(softc->dev, 0x22, 1, 0, NULL, 0);
		asix_set_reg(softc->dev, ASIX_IPG1_CMD, 0x1d, 0x12, NULL, 0);
	} else { /* external PHY */
		asix_set_reg(softc->dev, 0x22, 0, 0, NULL, 0);
		asix_set_reg(softc->dev, ASIX_IPG1_CMD, 0x0c15, 0x0e, NULL, 0);
	}
}

static void asix178_open_device(usbeth_softc_t *softc)
{
	uint8_t data[2];

	asix_set_reg(softc->dev, 0x0d, 0, 0, NULL, 0);
	asix_set_reg(softc->dev, 0x0e, 0, 0, NULL, 0);
	asix_set_reg(softc->dev, ASIX_GPIO_WRITE_CMD, 0x8c, 0, NULL, 0);
	asix_set_reg(softc->dev, ASIX_GPIO_WRITE_CMD, 0x3c, 0, NULL, 0);
	asix_set_reg(softc->dev, ASIX_GPIO_WRITE_CMD, 0x1c, 0, NULL, 0);
	asix_set_reg(softc->dev, ASIX_GPIO_WRITE_CMD, 0x3c, 0, NULL, 0);
	asix_set_reg(softc->dev, ASIX_RXCTL_CMD, 0, 0, NULL, 0);
	asix_set_reg(softc->dev, ASIX_MII_SWOP_CMD, 0, 0, NULL, 0);

	/* s/w reset register */
	asix_set_reg(softc->dev, 0x20, 0x48, 0, NULL, 0);
	data[0] = 0x82;
	data[1] = 0x00;
	asix_set_reg(softc->dev, ASIX_MII_WRITE_CMD, 0x18, 0x14, data, 2);
	data[0] = 0xe1;
	data[1] = 0x05;
	asix_set_reg(softc->dev, ASIX_MII_WRITE_CMD, 0x18, 0x04, data, 2);
	data[0] = 0x00;
	data[1] = 0x02;
	asix_set_reg(softc->dev, ASIX_MII_WRITE_CMD, 0x18, 0x09, data, 2);
	data[0] = 0x40;
	data[1] = 0x12;
	asix_set_reg(softc->dev, ASIX_MII_WRITE_CMD, 0x18, 0x00, data, 2);

	/* change rx packet length if burst size != 2048 */
	/* SO, AM, AB, MFB=2048 */
	asix_set_reg(softc->dev, ASIX_RXCTL_CMD, 0x98, 0, NULL, 0);
	asix_set_reg(softc->dev, ASIX_MED_WRITE_CMD, 0x37e, 0, NULL, 0);
}

/*	 For 88772 and 88178. Handles multiple ethernet frames per USB packet
	and fragments across USB packets
*/
static int asix2_get_eth_frame(usbeth_softc_t *softc, unsigned char *buf)
{
	static uint8_t *rbuf = NULL;
	int len = 0, lenb, actual_len, re_queue_rx = 0;
	static uint8_t *frag = NULL;
	static int flen1 = 0, flen2 = 0;

	if (rbuf == NULL)
		rbuf = softc->rxbuf;

	/* check if final data from previous rx  */
	if (flen1 && frag) {

		memcpy(buf, frag, flen1);
		memcpy((buf + flen1), rbuf, flen2);
		len = flen1 + flen2;

		KFREE(frag);
		frag = NULL;

		softc->rx_ur->ur_xferred -= flen2;

		/* if no more eth frames then re-queue bulk rx
		*/
		if (softc->rx_ur->ur_xferred <= 0) {

			if (softc->rx_ur->ur_xferred < 0) {
				xprintf("ERROR: asix2_get_eth_fram...");
				xprintf("follow up buf length is negative!\n");
			}

			re_queue_rx = 1;
		} else
			rbuf += flen2;

		flen1 = flen2 = 0;

	} else if (softc->rx_ur->ur_xferred) {
		len = (((int)rbuf[1] << 8) + rbuf[0]) & 0x7ff;
		lenb = ~(((int)rbuf[3] << 8) + rbuf[2]) & 0x7ff;

		/* Only use a valid length field */
		if (len == lenb) {

			/* frame length adjusted for even byte boudary */
			actual_len = (len + 4 + 1) & 0xfffe;
			softc->rx_ur->ur_xferred -= actual_len;

			/* last frame or fragment */
			if (softc->rx_ur->ur_xferred <= 0) {

				/* last frame...send upward &
				adjust for length fields */
				if (softc->rx_ur->ur_xferred == 0)
					memcpy(buf, (rbuf + 4), len);
				else {	/* fragment */
					if (frag) {
						xprintf
						    ("asix2_get_eth_frame...fragment buffer exists!\n");
						KFREE(frag);
						flen1 = flen2 = 0;
					} else {
						/* fragment to be received next */
						flen2 = -softc->rx_ur->ur_xferred;

						/* fragment left over now w/o length fields */
						flen1 = actual_len - flen2 - 4;

						if (flen1 < 0) {
							xprintf
							    ("asix2_get_eth_frame...negative fragment length!"
							     "  (al %d; ux %d)\n",
							     actual_len,
							     softc->
							     rx_ur->ur_xferred);
						} else {
							frag =
							    KMALLOC(flen1, 0);
							memcpy(frag, (rbuf + 4),
							       flen1);
						}
					}
				}
				re_queue_rx = 1; /* re-queue bulk rx */
			} else	{ /* multi-frame...send next sub-frame upward */

				/* adjust for length fields */
				memcpy(buf, (rbuf + 4), len);

				/* point holding buffer to next frame */
				rbuf += actual_len;
			}
		} else {
			xprintf
			    ("asix2_get_eth_frame...length field error rx=%d; [%04X %04X]!\n",
			     softc->rx_ur->ur_xferred, len, lenb);
			len = 0;
			re_queue_rx = 1;
		}
	} else			/* probably got a zero length packet */
		re_queue_rx = 1;

	if (re_queue_rx) {
		rbuf = NULL;
		usb_free_request(softc->rx_ur);
		usbeth_queue_rx(softc);
	}

	return len;
}

static const usbeth_devif_t asix_if = {
	asix_init_device,
	asix_open_device,
	asix_close_device,
	NULL
};

static const usbeth_devif_t asix772_if = {
	asix_init_device,
	asix772_open_device,
	asix_close_device,
	asix2_get_eth_frame
};

static const usbeth_devif_t asix178_if = {
	asix_init_device,
	asix178_open_device,
	asix_close_device,
	asix2_get_eth_frame
};

/* **************************************
   *  CDC I/F Functions
   ************************************** */

uint8_t asc2hex(char ch)
{
	uint8_t x = ch - '0';

	if (x > 9)
		x -= 7;

	return x;
}

static int cdc_get_mac_addr(usbeth_softc_t *softc, uint8_t *mac_addr)
{
	int ii, status;
	char buf[30], *p;

	/* 26 = len of unicoded macaddr */
	status = usb_get_string(softc->dev, 3, buf, 26);
	if (status > 0) {
		p = buf;
		for (ii = 0; ii < 6; ++ii) {
			mac_addr[ii] = (asc2hex(*p) << 4) + asc2hex(*(p + 1));
			p += 2;
		}
	}
	return status;
}

static void cdc_init_device(usbeth_softc_t *softc)
{
	softc->embed_tx_len = 0;

	usb_std_request(softc->dev, (USBREQ_TYPE_STD | USBREQ_REC_INTERFACE),
		USB_REQUEST_SET_INTERFACE, 1, /* alt setting 1 */ 1,
			 /* interface 1 */ NULL, 0);

	/* Read the adapter's MAC addr */
	cdc_get_mac_addr(softc, softc->mac_addr);
}

static void cdc_close_device(usbeth_softc_t *softc)
{
}

static void cdc_open_device(usbeth_softc_t *softc)
{
}

static const usbeth_devif_t cdc_if = {
	cdc_init_device,
	cdc_open_device,
	cdc_close_device,
	NULL
};

/* *********************** USB-ETH I/F Functions **************************** */

static const int ID_TBL[] = {
	/* first entry must be for CDC */
	0x0000, 0x0000, USB_CDC, CDC, (int)&cdc_if,
	0x0506, 0x4601, PEGASUS_II, _3_COM, (int)&peg_if, /* 3-Com */
	0x066b, 0x2202, PEGASUS_II, LINKSYS_10, (int)&peg_if, /* LinkSys */
	0x066b, 0x2203, PEGASUS, LINKSYS_100, (int)&peg_if,
	0x066b, 0x2204, PEGASUS, LINKSYS_100, (int)&peg_if,
	0x066b, 0x2206, PEGASUS, LINKSYS, (int)&peg_if,
	0x066b, 0x400b, PEGASUS_II, LINKSYS_10, (int)&peg_if,
	0x066b, 0x200c, PEGASUS_II, LINKSYS_10, (int)&peg_if,
	0x0bda, 0x8150, REALTEK, LINKSYS_100M, (int)&rtek_if,
	0x0423, 0x000a, NETMATE, CATC_NM, (int)&catc_if, /* CATC (Netmate I) */
	/*Belkin & CATC (Netmate II) */
	0x0423, 0x000c, NETMATE, BELKIN_CATC, (int)&catc_if,
	0x067c, 0x1001, PEGASUS_II, SIEMENS, (int)&peg_if, /* Siemens */
	0x07b8, 0x110c, PEGASUS_II, GEN_PEG, (int)&peg_if, /* ABOCOM */
	0x07b8, 0x4104, PEGASUS, GEN_PEG, (int)&peg_if,	/* ABOCOM */
	0x07b8, 0x4004, PEGASUS, GEN_PEG, (int)&peg_if,	/* ABOCOM */
	0x07b8, 0x4007, PEGASUS, GEN_PEG, (int)&peg_if,	/* ABOCOM */
	0x07b8, 0x4102, PEGASUS_II, GEN_PEG, (int)&peg_if, /* ABOCOM */
	0x07b8, 0x4002, PEGASUS, GEN_PEG, (int)&peg_if,	/* ABOCOM */
	0x07b8, 0x400b, PEGASUS_II, GEN_PEG, (int)&peg_if, /* ABOCOM */
	0x07b8, 0x400c, PEGASUS_II, GEN_PEG, (int)&peg_if, /* ABOCOM */
	0x07b8, 0xabc1, PEGASUS, GEN_PEG, (int)&peg_if,	/* ABOCOM */
	0x07b8, 0x200c, PEGASUS_II, GEN_PEG, (int)&peg_if, /* ABOCOM */
	0x083a, 0x1046, PEGASUS, GEN_PEG, (int)&peg_if,	/* Accton */
	0x083a, 0x5046, PEGASUS_II, SIEMENS, (int)&peg_if, /* SpeedStream */
	/* ADMtek, Trendnet */
	0x07a6, 0x8511, PEGASUS_II, GEN_PEG, (int)&peg_if,
	0x07a6, 0x8513, PEGASUS_II, GEN_PEG, (int)&peg_if, /* ADMtek */
	0x07a6, 0x1986, PEGASUS_II, GEN_PEG, (int)&peg_if, /* ADMtek */
	0x3334, 0x1701, PEGASUS_II, GEN_PEG, (int)&peg_if, /* AEI */
	0x07c9, 0xb100, PEGASUS_II, GEN_PEG, (int)&peg_if, /* Allied */
	0x050d, 0x0121, PEGASUS_II, BELKIN, (int)&peg_if, /* Belkin */
	0x08dd, 0x0986, PEGASUS, GEN_PEG, (int)&peg_if,	/* Billionton */
	0x08dd, 0x0987, PEGASUS, GEN_PEG, (int)&peg_if,	/* Billionton */
	0x08dd, 0x0988, PEGASUS, GEN_PEG, (int)&peg_if,	/* Billionton */
	0x08dd, 0x8511, PEGASUS_II, GEN_PEG, (int)&peg_if, /* Billionton */
	0x07aa, 0x0004, PEGASUS, GEN_PEG, (int)&peg_if,	/* Corega */
	0x07aa, 0x000d, PEGASUS_II, GEN_PEG, (int)&peg_if, /* Corega */
	0x2001, 0x4001, PEGASUS, D_LINK, (int)&peg_if, /* D-Link */
	0x2001, 0x4002, PEGASUS, D_LINK, (int)&peg_if, /* D-Link */
	0x2001, 0x4102, PEGASUS_II, D_LINK, (int)&peg_if, /* D-Link */
	0x2001, 0x400b, PEGASUS_II, D_LINK, (int)&peg_if, /* D-Link */
	0x2001, 0x200c, PEGASUS_II, D_LINK, (int)&peg_if, /* D-Link */
	0x2001, 0xabc1, PEGASUS, D_LINK, (int)&peg_if, /* D-Link */
	0x0db7, 0x0002, PEGASUS_II, GEN_PEG, (int)&peg_if, /* GOLDPFEIL */
	0x1342, 0x0304, PEGASUS, GEN_PEG, (int)&peg_if,	/* EasiDock */
	0x05cc, 0x3000, PEGASUS, GEN_PEG, (int)&peg_if,	/* Elsa */
	0x0e66, 0x400c, PEGASUS_II, HAWKING, (int)&peg_if, /* Hawking */
	0x03f0, 0x811c, PEGASUS_II, GEN_PEG, (int)&peg_if, /* HP */
	0x04bb, 0x0904, PEGASUS, GEN_PEG, (int)&peg_if,	/* IO Data */
	0x04bb, 0x0913, PEGASUS_II, GEN_PEG, (int)&peg_if,	/* IO Data */
	0x0951, 0x000a, PEGASUS, GEN_PEG, (int)&peg_if,	/*Kingston */
	0x056e, 0x4002, PEGASUS, GEN_PEG, (int)&peg_if,	/* LANEED */
	0x056e, 0x400b, PEGASUS_II, GEN_PEG, (int)&peg_if, /* LANEED */
	0x056e, 0xabc1, PEGASUS, GEN_PEG, (int)&peg_if,	/* LANEED */
	0x056e, 0x200c, PEGASUS_II, GEN_PEG, (int)&peg_if, /* LANEED */
	0x0411, 0x0001, PEGASUS, GEN_PEG, (int)&peg_if,	/* MELCO */
	0x0411, 0x0005, PEGASUS, GEN_PEG, (int)&peg_if,	/* MELCO */
	0x0411, 0x0009, PEGASUS_II, GEN_PEG, (int)&peg_if, /* MELCO */
	0x0846, 0x1020, PEGASUS_II, NETGEAR, (int)&peg_if, /* NETGEAR */
	0x08d1, 0x0003, PEGASUS_II, GEN_PEG, (int)&peg_if, /* smartNIC */
	0x0707, 0x0200, PEGASUS, GEN_PEG, (int)&peg_if,	/* SMC 202 */
	0x0707, 0x0201, PEGASUS_II, GEN_PEG, (int)&peg_if, /* SMC 2206 */
	0x15e8, 0x9100, PEGASUS, GEN_PEG, (int)&peg_if,	/* SOHOware */
	0x15e8, 0x9110, PEGASUS_II, GEN_PEG, (int)&peg_if, /* SOHOware */
	0x067c, 0x1001, PEGASUS_II, SIEMENS, (int)&peg_if, /* SpeedStream */
	0x0846, 0x1040, ASIX, NETGEAR, (int)&asix_if, /* Netgear FA120 */
	0x07b8, 0x420a, ASIX, HAWKING, (int)&asix_if, /* Hawking UF200 */
	0x077b, 0x2226, ASIX, LINKSYS, (int)&asix_if, /* Linksys USB200M */
	0x0b95, 0x1720, ASIX, GEN_ASIX, (int)&asix_if, /* Intellinet ST Lab */
	0x2001, 0x1a00, ASIX, D_LINK, (int)&asix_if, /* D-Link DUB-E100 */
	/* D-Link DUB-E10...H/W Version C1 */
	0x2001, 0x1a02, ASIX772B, D_LINK, (int)&asix772_if,
	/* D-Link DUB-E100...H/W Version B1 */
	0x2001, 0x3C05, ASIX772B, D_LINK, (int)&asix772_if,
	/* TrendNet TU2-ET100 or Airlink-101 */
	0x0b95, 0x7720, ASIX772A, GEN_ASIX, (int)&asix772_if,
	/* Linksys USB200M...ver 2 */
	0x13b1, 0x0018, ASIX772A, LINKSYS, (int)&asix772_if,
	/* Linksys USB1000...gigabit */
	0x1737, 0x0039, ASIX178, LINKSYS, (int)&asix178_if,
	0x07a6, 0x8515, PEGASUS_II, GEN_PEG, (int)&peg_if, /* ADMtek USB 2.0 */
	0x07b8, 0x401a, REALTEK, HAWKING, (int)&rtek_if, /* Hawking HUF1 */
	/* ASIX gigabit, Hawking HGU1, etc */
	0x0b95, 0x1780, ASIX178, GEN_ASIX, (int)&asix178_if,
	/* AX88772A 10/100 */
	0x0b95, 0x772a, ASIX772A, GEN_ASIX, (int)&asix772_if,
	/* Apple USB Ethernet adapter */
	0x05ac, 0x1402, ASIX772A, GEN_ASIX, (int)&asix772_if,
	/* AX88772B 10/100 */
	0x0b95, 0x772b, ASIX772B, GEN_ASIX, (int)&asix772_if,
	/* AX88772B 10/100 */
	0x0b95, 0x7e2b, ASIX772B, GEN_ASIX, (int)&asix772_if,
	-1, 0, 0, 0, 0
};

const void *usbeth_driver_list[] = { &catc_if, &peg_if, &asix_if, &asix772_if,
	&asix178_if, &rtek_if, &cdc_if
};

static int *usbeth_find_device_info(uint16_t ven_id, uint16_t dev_id)
{
	int i;
	char *s;
	static int newdev_info[5];
	const int *ptr = ID_TBL;

	while (*ptr != -1) {

		if ((ven_id == ptr[0]) && (dev_id == ptr[1]))
			return ((int *)ptr);

		ptr += 5;
	}

	/* Check if device info is available through environment variable.
	 * Parameters must be comma separated and have same content as
	 * ID_TBL above.
	 */
	s = env_getenv("USBETH");
	if (s) {
		for (i = 0; i < 5; ++i) {
			newdev_info[i] = lib_atoi(s);
			s = lib_strchr(s, ',');
			if (!s)
				break;
			++s;
		}
		if ((newdev_info[2] < MAX_CHIP_ID)
		    && (newdev_info[3] < MAX_VEN_ID) && (newdev_info[4] < 5)) {
			newdev_info[4] =
			    (int)usbeth_driver_list[newdev_info[4]];
			return newdev_info;
		}
	}

	return NULL;
}

static int usbeth_init_device(usbeth_softc_t *softc)
{
	int i;
	uint16_t vendor_id, device_id;
	uint8_t dev_class;
	int *ptr;
	char *st1;
	usb_device_descr_t *dev_desc;

	/* find out which device is connected */
	dev_desc = &(softc->dev->ud_devdescr);
	vendor_id = (uint16_t) GETUSBFIELD(dev_desc, idVendor);
	device_id = (uint16_t) GETUSBFIELD(dev_desc, idProduct);
	dev_class = dev_desc->bDeviceClass;

	/* Look for device in list since some
	ethernet adapters identify as CDC! */
	ptr = usbeth_find_device_info(vendor_id, device_id);
	st1 = "USB-Ethernet Adapter";
	if (ptr == NULL) {
		if (dev_class == USB_DEVICE_CLASS_COMMUNICATIONS) {
			ptr = (int *)ID_TBL; /* first entry is for CDC */
			st1 = "Device";
		} else {
			xprintf("Unrecognized USB-Ethernet device\n");
			return -1;
		}
	}
	softc->dev_id = ptr[2];
	softc->ven_code = ptr[3];
	softc->intf = (usbeth_devif_t *) ptr[4];

	/* init the adapter */
	softc->intf->init(softc);

	/* display adapter info */
	xprintf("usb: identified %s %s (", VENDOR_NAMES[softc->ven_code], st1);
	for (i = 0; i < 6; ++i)
		xprintf("%02x%s", softc->mac_addr[i], (i == 5) ? ")\n" : ":");

	return 0;
}

static int usbeth_get_dev_addr(usbeth_softc_t *softc, uint8_t *mac_addr)
{
	memcpy(mac_addr, softc->mac_addr, 6);
	return 0;
}

static void usbeth_queue_rx(usbeth_softc_t *softc)
{
	softc->rx_ur = usb_make_request(softc->dev, softc->bulk_inpipe,
					softc->rxbuf, RX_PACKET_SIZE,
					(UR_FLAG_IN | UR_FLAG_SHORTOK));
	usb_queue_request(softc->rx_ur);
}

static void usbeth_close_device(usbeth_softc_t *softc)
{
	softc->intf->close(softc);
}

static void usbeth_open_device(usbeth_softc_t *softc)
{
	softc->intf->open(softc);

	/* kick start the receive */
	usbeth_queue_rx(softc);
}

static int usbeth_data_rx(usbeth_softc_t *softc)
{
	usb_poll(softc->dev->ud_bus);
	return (!softc->rx_ur->ur_inprogress);
}

static int usbeth_get_eth_frame(usbeth_softc_t *softc, unsigned char *buf)
{
	int len = 0;

	if (!softc->rx_ur->ur_inprogress) {
		if (softc->intf->rx) /* need special receive process? */
			len = softc->intf->rx(softc, buf);
		else {
			len = softc->rx_ur->ur_xferred;
			memcpy(buf, softc->rxbuf, len);
			usb_free_request(softc->rx_ur);
			usbeth_queue_rx(softc);
		}
	} else
		xprintf("Bulk data is not available yet!\n");

	return len;
}

static int usbeth_send_eth_frame(usbeth_softc_t *softc, unsigned char *buf,
				 int len)
{
	usbreq_t *ur;
	int txlen = len;
	unsigned char *txbuf;

	if (softc->embed_tx_len) {
		txbuf = KMALLOC((len + softc->embed_tx_len), DMA_BUF_ALIGN);
		txbuf[0] = txlen & 0xff; /* 1st two bytes...little endian */
		txbuf[1] = (txlen >> 8) & 0xff;
		if (softc->embed_tx_len == 4) {
			/* 2nd two bytes...1's complement */
			txbuf[2] = txbuf[0] ^ 0xff;
			txbuf[3] = txbuf[1] ^ 0xff;
		}
		memcpy(&txbuf[softc->embed_tx_len], buf, txlen);
		txlen += softc->embed_tx_len;
	} else {
		if (softc->dev_id == REALTEK) {
			/* Now for some Realtek chip workarounds */
			if (txlen < 60)	/* some strange limitation */
				txlen = 60;
		}
		txbuf = KMALLOC(txlen, DMA_BUF_ALIGN);
		memcpy(txbuf, buf, txlen);
	}

	/* Modify tx length to handle modulo 64 packets...
	adapter will discard extra byte */
	if (!(txlen % 64))
		++txlen;

	ur = usb_make_request(softc->dev, softc->bulk_outpipe,
			      txbuf, txlen, UR_FLAG_OUT);
	usb_sync_request(ur);
	usb_free_request(ur);
	KFREE(txbuf);

	return len;
}

/*  *********************************************************************
    * BOLT-USB interfaces
    ********************************************************************* */

/*  *********************************************************************
    *  usbeth_probe(drv,probe_a,probe_b,probe_ptr)
    *
    *  Our probe routine.  Identify if device is USB-ethernet adapter.
    *
    *  Input parameters:
    *	drv - not used
    *	probe_a - vendor ID
    *	probe_b - device ID
    *	probe_ptr - local device structure
    *
    *  Return value:
    *	nothing
    ********************************************************************* */

static int usbeth_probe(unsigned int probe_a, unsigned int probe_b)
{
	return ((int)usbeth_find_device_info(probe_a, probe_b));
}

/*  *********************************************************************
    *  usbeth_attach(dev,drv)
    *
    *  This routine is called when the bus scan stuff finds a usb-ethernet
    *  device.  We finish up the initialization by configuring the
    *  device and allocating our softc here.
    *
    *  Input parameters:
    *      dev - usb device, in the "addressed" state.
    *      drv - the driver table entry that matched
    *
    *  Return value:
    *      0
    ********************************************************************* */

const bolt_driver_t usbethdrv;	/* forward declaration */


static void usbeth_free(usbdev_t *dev)
{
	usbeth_softc_t *softc = (usbeth_softc_t *)dev->ud_private;

	if (softc) {
		if (softc->bulk_outpipe)
			usb_destroy_pipe(dev, softc->bulk_outpipe);

		if (softc->bulk_inpipe)
			usb_destroy_pipe(dev, softc->bulk_inpipe);

		if (softc->rxbuf)
			KFREE(softc->rxbuf);

		KFREE(softc);
	}
}


static int usbeth_attach(usbdev_t *dev, usb_driver_t *drv)
{
	usb_config_descr_t *cfgdscr = dev->ud_cfgdescr;
	usb_endpoint_descr_t *epdscr;
	usb_endpoint_descr_t *indscr = NULL;
	usb_endpoint_descr_t *outdscr = NULL;
	usb_interface_descr_t *ifdscr;
	usbeth_softc_t *softc;
	int idx;
	char *desc;

	dev->ud_drv = drv;

	usbeth_trace = (env_getenv("USBETH_TRACE")) ? 1 : 0;

	softc = (usbeth_softc_t *) KMALLOC(sizeof(usbeth_softc_t), 0);
	if (softc == NULL) {
		xprintf("Failed to allocate softc memory.\n");
		return BOLT_ERR;
	}
	memset(softc, 0, sizeof(usbeth_softc_t));
	softc->rxbuf = (uint8_t *) KMALLOC(RX_PACKET_SIZE, DMA_BUF_ALIGN);
	if (softc->rxbuf == NULL) {
		xprintf("Failed to allocate RX buffer.\n");
		KFREE(softc);
		return BOLT_ERR;
	}
	dev->ud_private = softc;
	softc->dev = dev;

	ifdscr = usb_find_cfg_descr(dev, USB_INTERFACE_DESCRIPTOR_TYPE, 0);
	if (ifdscr == NULL) {
		xprintf("USBETH: ERROR...no interface descriptor\n");
		return BOLT_ERR;
	}

	for (idx = 0; idx < 3; idx++) {
		epdscr =
		    usb_find_cfg_descr(dev, USB_ENDPOINT_DESCRIPTOR_TYPE, idx);
		if ((epdscr->bmAttributes & USB_ENDPOINT_TYPE_MASK) ==
		    USB_ENDPOINT_TYPE_BULK) {
			if (USB_ENDPOINT_DIR_OUT(epdscr->bEndpointAddress))
				outdscr = epdscr;
			else
				indscr = epdscr;
		}
	}

	if (!indscr || !outdscr) {
		/*
		 * Could not get descriptors, something is very wrong.
		 * Leave device addressed but not configured.
		 */
		xprintf
		    ("USBETH: ERROR...bulk endpoint descriptor(s) missing\n");
		return BOLT_ERR;
	}

	/*
	 * Choose the standard configuration.
	 */

	usb_set_configuration(dev, cfgdscr->bConfigurationValue);

	/* Quit if not able to initialize the device */
	if (usbeth_init_device(softc) < 0)
		return BOLT_ERR;

	/*
	 * Open the pipes.
	 */

	softc->bulk_inpipe = usb_open_pipe(dev, indscr);
	softc->bulk_outpipe = usb_open_pipe(dev, outdscr);

	desc = (softc->dev_id == USB_CDC) ?
		"USB-CDC device" : usbethdrv.drv_description;

	/* Now attach this device as a BOLT Ethernet device */
	softc->fullname = (char *)bolt_attach((bolt_driver_t *) &usbethdrv,
				softc, NULL, desc);
	if (softc->fullname) {
		xprintf("usb: device '%s' is now available\n", softc->fullname);
		++Dev_cnt;
		return BOLT_OK;
	}

	err_msg("usb (eth): we ran out of device slots?");

	usbeth_free(dev);

	return BOLT_ERR_NOHANDLES;
}

/*  *********************************************************************
    *  usbeth_detach(dev)
    *
    *  This routine is called when the bus scanner notices that
    *  this device has been removed from the system.  We should
    *  do any cleanup that is required.  The pending requests
    *  will be cancelled automagically.
    *
    *  Input parameters:
    *      dev - usb device
    *
    *  Return value:
    *      0
    ********************************************************************* */

static int usbeth_detach(usbdev_t *dev)
{
	usbeth_softc_t *softc = (usbeth_softc_t *) dev->ud_private;
	int rc = BOLT_ERR;

	if (softc) {
		if (!softc->fullname)
			return rc;
#if CFG_NETWORK
		/* If we have a network context	then uninit it
		as well, or else the net stack will be left with
		stale references to us. */
		if (!net_unregister(softc->fullname))
			xprintf("usb: unregistered %s from network stack\n",
				softc->fullname);
#endif
		if (Dev_cnt)
			--Dev_cnt;

		xprintf("usb: detaching [%s]\n", softc->fullname);
		rc = bolt_detach(softc->fullname);

		usbeth_free(dev);
	}

	return rc;
}

/* BOLT USB device interface structure */
usb_driver_t usbeth_driver = {
	"Ethernet Device",
	usbeth_attach,
	usbeth_detach,
	usbeth_probe
};

/*  *********************************************************************
    * BOLT-Ethernet device interfaces
    ********************************************************************* */

static int usbeth_ether_open(bolt_devctx_t *ctx)
{
	USBETH_TRACE("called. Dev_cnt: %d", Dev_cnt);

	if (!Dev_cnt)
		return BOLT_ERR_NOTREADY;

	usbeth_open_device((usbeth_softc_t *) ctx->dev_softc);

	return 0;
}

static int usbeth_ether_read(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	if (!Dev_cnt)
		return BOLT_ERR_NOTREADY;

	buffer->buf_retlen =
	    usbeth_get_eth_frame((usbeth_softc_t *) ctx->dev_softc,
				 buffer->buf_ptr);

#ifdef DATA_DUMP
	xprintf("Incoming packet:\n");
	hexdump(buffer->buf_ptr, buffer->buf_retlen);
#endif

	return 0;
}

static int usbeth_ether_inpstat(bolt_devctx_t *ctx, iocb_inpstat_t *inpstat)
{
	if (!Dev_cnt)
		return BOLT_ERR_NOTREADY;

	inpstat->inp_status = usbeth_data_rx((usbeth_softc_t *) ctx->dev_softc);

	return 0;
}

static int usbeth_ether_write(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	if (!Dev_cnt)
		return BOLT_ERR_NOTREADY;

	/* Block until hw notifies you data is sent. */
	usbeth_send_eth_frame((usbeth_softc_t *) ctx->dev_softc,
			      buffer->buf_ptr, buffer->buf_length);

	return 0;
}

static int usbeth_ether_ioctl(bolt_devctx_t *ctx, iocb_buffer_t *buffer)
{
	int retval = 0;
	char *unimplemented = NULL;

	if (!Dev_cnt)
		return BOLT_ERR_NOTREADY;

	switch ((int)buffer->buf_ioctlcmd) {
	case IOCTL_ETHER_GETHWADDR:
		USBETH_TRACE("IOCTL_ETHER_GETHWADDR called. Dev_cnt: %d",
			Dev_cnt);
		usbeth_get_dev_addr((usbeth_softc_t *) ctx->dev_softc,
				    buffer->buf_ptr);
		break;

	case IOCTL_ETHER_SETHWADDR:
		unimplemented = "SETHWADDR";
		break;
	case IOCTL_ETHER_GETSPEED:
		unimplemented = "GETSPEED";
		break;
	case IOCTL_ETHER_SETSPEED:
		unimplemented = "SETSPEED";
		break;
	case IOCTL_ETHER_GETLINK:
		unimplemented = "GETLINK";
		break;
	case IOCTL_ETHER_GETLOOPBACK:
		unimplemented = "GETLOOPBACK";
		break;
	case IOCTL_ETHER_SETLOOPBACK:
		unimplemented = "SETLOOPBACK";
		break;

	default:
		err_msg("%s: Invalid IOCTL %d",
			__func__, (int)buffer->buf_ioctlcmd);
		retval = -1;
		break;
	}

	/*
	 *  Recognised ioctls that have not been implemented
	 * for USB Ethernet drivers, and no expectation yet
	 * that they will be.
	 */
	if (unimplemented) {
		USBETH_TRACE("Note that IOCTL_ETHER_%s is not implemented",
			unimplemented);
		retval = -1;
	}

	return retval;
}

static int usbeth_ether_close(bolt_devctx_t *ctx)
{
	USBETH_TRACE("called. Dev_cnt: %d", Dev_cnt);

	if (!Dev_cnt)
		return BOLT_ERR_NOTREADY;

	usbeth_close_device((usbeth_softc_t *) ctx->dev_softc);

	return 0;
}

/* BOLT ethernet device interface structures */
static const bolt_devdisp_t usbeth_ether_dispatch = {
	usbeth_ether_open,
	usbeth_ether_read,
	usbeth_ether_inpstat,
	usbeth_ether_write,
	usbeth_ether_ioctl,
	usbeth_ether_close,
	NULL,
	NULL
};

const bolt_driver_t usbethdrv = {
	"USB-Ethernet Device",
	"eth",
	BOLT_DEV_NETWORK,
	&usbeth_ether_dispatch,
	NULL,			/* probe...not needed */
};
