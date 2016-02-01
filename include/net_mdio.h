/***************************************************************************
 *     Copyright (c) 2014-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __NET_MDIO_H__
#define __NET_MDIO_H__

#include "lib_types.h"

#define PHY_MAX_ADDR		32
#define PHY_BCAST_ADDR		0

/* Marshalling structure for MDIO abstraction library */
typedef struct mdio_xfer_s {
	int addr;
	uint16_t regnum;
	uint16_t data;
} mdio_xfer_t;

typedef struct mdio_info_s {
	int devhandle;
	uint32_t phy_id;
	char devname[8]; /* 8 (eight) for "mdio0", "mdio1", and so on */
} mdio_info_t;

typedef enum phy_speed {
	SPEED_10	= 10,
	SPEED_100	= 100,
	SPEED_1000	= 1000,
	SPEED_2000	= 2000,
} phy_speed_t;

/*
 * mdio_init - initialize a given MDIO bus driver
 *
 * @devname: mdio bus name (usually mdioN)
 *
 * Return value: a pointer to the mdio context associated with that
 * driver instsance
 */
mdio_info_t *mdio_init(char *devname);

/* mdio_uninit - uninitialize a given MDIO bus driver
 *
 * @mdio: a pointer to the mdio context previously initialized with a call
 * to mdio_init.
 *
 * Return value: none
 */
void mdio_uninit(mdio_info_t * mdio);

/*
 * mdio_read - Perform a MDIO read at the given PHY address and register
 * location
 *
 * @mdio_info_t: pointer to the mdio_info_t structure kept by the driver
 * @addr: PHY address on the MDIO bus to access
 * @regnum: register to read from
 *
 * Note: return value must be placed in xfer->data
 *
 * Return value: 0 on success, < 0 otherwise
 */
int mdio_read(mdio_info_t * mdio, int addr, uint16_t regnum);

/*
 * mdio_write - Performa MDIO write at the given PHY adress, register location
 * with the specified value
 *
 * @mdio_info_t: pointer to the mdio_info_t structure kept by the driver
 * @addr: PHY address on the MDIO bus to access
 * @regnum: register to write to
 * @data: value to write at the PHY address and the register location
 *
 * Return value: 0 on success, < 0 otherwise
 */
int mdio_write(mdio_info_t * mdio, int addr, uint16_t regnum, uint16_t data);

/* Library functions used by Ethernet/MDIO bus drivers */
uint32_t mdio_get_phy_id(mdio_info_t * mdio, int addr);
int mdio_phy_find_first(mdio_info_t * mdio, int skip_addr);
void mdio_set_advert(mdio_info_t * mdio, int addr, phy_speed_t speed);
int mdio_phy_reset(mdio_info_t * mdio, int addr);
int mdio_set_clr_bits(mdio_info_t * mdio, int phy, int reg, int set, int clr);

typedef enum {
	MDIO_10MBIT	= 0x0001,
	MDIO_100MBIT	= 0x0002,
	MDIO_1000MBIT	= 0x0004,
	MDIO_FULLDUPLEX	= 0x0008,
	MDIO_AUTONEG	= 0x0010,
} mdio_config;

mdio_config mdio_get_config(mdio_info_t * mdio, int addr);
mdio_config mdio_auto_configure(mdio_info_t * mdio, int addr);

#endif /* __NET_MDIO_H__ */
