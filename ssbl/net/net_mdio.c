/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "mii.h"
#include "net_ether.h"
#include "net_mdio.h"

#include "devfuncs.h"
#include "env_subr.h"
#include "error.h"
#include "iocb.h"
#include "ioctl.h"
#include "timer.h"

#include "lib_malloc.h"
#include "lib_printf.h"
#include "lib_string.h"
#include "lib_types.h"

static mdio_info_t *mdio_busses[NUM_ENET + 1];
static unsigned int mdio_bus_cnt;

static mdio_info_t *mdio_find(char *devname)
{
	unsigned int i;

	for (i = 0; i < mdio_bus_cnt; i++)
		if (!strcmp(mdio_busses[i]->devname, devname))
			return mdio_busses[i];

	return NULL;
}

mdio_info_t *mdio_init(char *devname)
{
	mdio_info_t *info = NULL;
	int devhandle;

	devhandle = bolt_open(devname);
	if (devhandle < 0) {
		if (devhandle == BOLT_ERR_DEVOPEN)
			info = mdio_find(devname);
		return info;
	}

	/* Don't bother if no enets exist to init (Coverity fix) */
	if (NUM_ENET == 0)
		return NULL;

	/* Don't overflow the array */
	if (mdio_bus_cnt == NUM_ENET)
		return NULL;

	info = KMALLOC(sizeof(*info), 0);
	if (!info) {
		bolt_close(devhandle);
		return NULL;
	}

	memset(info, 0, sizeof(*info));
	info->devhandle = devhandle;
	strncpy(info->devname, devname, sizeof(info->devname)-1);

	/* Add this bus to the list */
	mdio_busses[mdio_bus_cnt] = info;
	mdio_bus_cnt++;

	return info;
}

void mdio_uninit(mdio_info_t *mdio)
{
	bolt_close(mdio->devhandle);
	KFREE(mdio);
	if (mdio_bus_cnt > 0)
		mdio_bus_cnt--;
}

int mdio_read(mdio_info_t *mdio, int addr, uint16_t regnum)
{
	mdio_xfer_t xfer;
	int ret;

	if (addr >= PHY_MAX_ADDR)
		return BOLT_ERR_INV_PARAM;

	if (regnum > 31)
		return BOLT_ERR_INV_PARAM;

	xfer.addr = addr;
	xfer.regnum = regnum;

	ret = bolt_read(mdio->devhandle, (unsigned char *)&xfer, sizeof(xfer));
	if (ret < 0)
		return ret;

	return xfer.data & 0xffff;
}

int mdio_write(mdio_info_t *mdio, int addr, uint16_t regnum, uint16_t data)
{
	mdio_xfer_t xfer;

	if (addr >= PHY_MAX_ADDR)
		return BOLT_ERR_INV_PARAM;

	if (regnum > 31)
		return BOLT_ERR_INV_PARAM;

	xfer.addr = addr;
	xfer.regnum = regnum;
	xfer.data = data;

	return bolt_write(mdio->devhandle,
		(unsigned char *)&xfer, sizeof(xfer));
}

/* mdio_get_linkstatus -- checks whether the link of the specified PHY is up
 *
 * The link status bit of BMSR is checked to determine whether it is up.
 *
 * Parameter:
 *  mdio [in] pointer to MDIO device context
 *
 * Returns:
 *  1 if link status bit is set
 *  0 if link status bit is not set
 *  -1 if link status is not known
 */
int mdio_get_linkstatus(mdio_info_t *mdio, int addr)
{
	uint16_t bmsr;

	if (mdio == NULL || addr >= PHY_MAX_ADDR)
		return -1;

	/* dummy read to get latched value so that the current value
	 * can be obtained at the next read
	 * LINKSTAT is one of them, from 22.2.4.2.12 of IEEE 802.3-2012
	 */
	bmsr = mdio_read(mdio, addr, MII_BMSR);

	bmsr = mdio_read(mdio, addr, MII_BMSR);

	if (bmsr & BMSR_LINKSTAT)
		return 1;

	/* if auto-negotiation has ever been done, LINKSTAT should be correct */
	if (bmsr & BMSR_ANCOMPLETE)
		return 0;

	/* timer has already expired, or no timer has ever been set */
	if (!mdio->is_timer_aneg)
		return 0;

	if (TIMER_EXPIRED(mdio->timer_aneg)) {
		mdio->is_timer_aneg = false;
		return 0;
	}

	/* timer is valid and still going on, too early to say no link */
	return -1;
}

uint32_t mdio_get_phy_id(mdio_info_t *mdio, int addr)
{
	int res;
	uint32_t phy_id;

	if (addr >= PHY_MAX_ADDR)
		return BOLT_ERR_INV_PARAM;

	res = mdio_read(mdio, addr, MII_PHYIDR1);
	if (res < 0)
		return 0;

	phy_id = res << 16;

	res = mdio_read(mdio, addr, MII_PHYIDR2);
	if (res < 0)
		return 0;

	phy_id |= res;

	return phy_id;
}

/* mdio_get_timeout_autoneg -- timeout on auto-negotiation in seconds
 *
 * Returns:
 *  ETH_ANEG_TIMEOUT if the same name environment variable is not defined
 *  integer value of ETH_ANEG_TIMEOUT (environment variable), otherwise
 */
unsigned int mdio_get_timeout_autoneg(void)
{
	char *env_aneg_timeout = env_getenv("ETH_ANEG_TIMEOUT");
	unsigned int aneg_timeout = ETH_ANEG_TIMEOUT;

	if (env_aneg_timeout != NULL)
		aneg_timeout = lib_atoi(env_aneg_timeout);

	if (aneg_timeout != ETH_ANEG_TIMEOUT) {
		xprintf("MDIO: auto-negotiation timeout %d, "
			"default %d seconds\n",
			aneg_timeout, ETH_ANEG_TIMEOUT);
	}

	return aneg_timeout;
}

int mdio_phy_find_first(mdio_info_t *mdio, int skip_addr)
{
	int i;
	uint32_t res;

	for (i = PHY_MAX_ADDR - 1; i >= 0; i--) {
		if (i == skip_addr)
			continue;

		res = mdio_get_phy_id(mdio, i);
		if (res != 0 && res != 0xffffffff)
			return i;
	}

	return BOLT_ERR_DEVNOTFOUND;
}

void mdio_set_advert(mdio_info_t *mdio, int addr, phy_speed_t speed)
{
	uint16_t base = 0, giga = 0;

	switch (speed) {
	case SPEED_2000:
	case SPEED_1000:
		giga |= K1TCR_1000BT_FDX | K1TCR_1000BT_HDX;
		/* fallthrough */
	case SPEED_100:
		base |= ANAR_TXFD | ANAR_TXHD;
		/* fallthrough */
	case SPEED_10:
		base |= ANAR_10HD | ANAR_10FD | PSB_802_3;
		break;
	}

	mdio_write(mdio, addr, MII_ANAR, base);
	mdio_write(mdio, addr, MII_K1CTL, giga);
}

/* mii register read/modify/write helper function */
int mdio_set_clr_bits(mdio_info_t *mdio, int addr,
			int reg, int set, int clr)
{
	int v;

	v = mdio_read(mdio, addr, reg);
	v &= ~clr;
	v |= set;
	mdio_write(mdio, addr, reg, v);

	return v;
}

int mdio_phy_reset(mdio_info_t *mdio, int addr)
{
	unsigned int timeout = 1000;
	uint16_t data;

	mdio_write(mdio, addr, MII_BMCR, BMCR_RESET);
	bolt_usleep(10);

	do {
		data = mdio_read(mdio, addr, MII_BMCR);
		if (!(data & BMCR_RESET))
			return 0;

		bolt_usleep(10);
	} while (timeout-- > 0);

	return BOLT_ERR_TIMEOUT;
}

/* mii_get_config: Return the current MII configuration
 */
mdio_config mdio_get_config(mdio_info_t *mdio, int addr)
{
	uint16_t bmcr, ctrl1000 = 0, stat1000 = 0;
	uint32_t advertising = 0, lp_advertising = 0;
	mdio_config config = 0;

	bmcr = mdio_read(mdio, addr, MII_BMCR);
	ctrl1000 = mdio_read(mdio, addr, MII_K1CTL);
	stat1000 = mdio_read(mdio, addr, MII_K1STSR);
	if (bmcr & BMCR_ANENABLE) {
		config |= MDIO_AUTONEG;
		advertising = mdio_read(mdio, addr, MII_ANAR);
		lp_advertising = mdio_read(mdio, addr, MII_ANLPAR);
		if ((ctrl1000 & (K1TCR_1000BT_FDX | K1TCR_1000BT_HDX))
			&& (stat1000 & (K1STSR_LP1KFD | K1STSR_LP1KHD))) {
			config |= MDIO_1000MBIT;
			if ((ctrl1000 & K1TCR_1000BT_FDX)
				&& (stat1000 & K1STSR_LP1KFD))
				config |= MDIO_FULLDUPLEX;
		}
		if ((config & MDIO_1000MBIT) == 0) {
			if (advertising & lp_advertising &
						(ANAR_TXFD|ANAR_TXHD)) {
				config |= MDIO_100MBIT;
				if (advertising & lp_advertising & ANAR_TXFD)
					config |= MDIO_FULLDUPLEX;
			} else {
				config |= MDIO_10MBIT;
				if (advertising & lp_advertising & ANAR_10FD)
					config |= MDIO_FULLDUPLEX;
			}
		}
	} else {
		if ((bmcr & BMCR_SPEED1000) && ((bmcr & BMCR_SPEED100) == 0)) {
			config |= MDIO_1000MBIT;
		} else {
			if (bmcr & BMCR_SPEED100)
				config |= MDIO_100MBIT;
			else
				config |= MDIO_10MBIT;
		}
		if (bmcr & BMCR_DUPLEX)
			config |= MDIO_FULLDUPLEX;
	}

	return config;
}

/* mii_auto_configure: Auto-Configure this MII interface
 */
mdio_config mdio_auto_configure(mdio_info_t *mdio, int addr)
{
	uint16_t data;
	unsigned int aneg_timeout;

	aneg_timeout = mdio_get_timeout_autoneg();

	/* enable and restart autonegotiation */
	data = mdio_read(mdio, addr, MII_BMCR);
	data |= (BMCR_RESTARTAN | BMCR_ANENABLE);
	mdio_write(mdio, addr, MII_BMCR, data);
	if (aneg_timeout != 0)
		TIMER_SET(mdio->timer_aneg, aneg_timeout*BOLT_HZ);

	mdio->is_timer_aneg = true; /* auto-negotiation is in progress */
	/* wait for it to finish */
	while (1) {
		data = mdio_read(mdio, addr, MII_BMSR);
		if (data & BMSR_ANCOMPLETE)
			break;

		if (aneg_timeout != 0) {
			if (TIMER_EXPIRED(mdio->timer_aneg)) {
				mdio->is_timer_aneg = false;
				return 0;
			}
		}
		POLL();
	}
	mdio->is_timer_aneg = false; /* auto-negotiation has completed */

	return mdio_get_config(mdio, addr);
}
