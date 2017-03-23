/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

#ifndef __IOCTL_H__
#define __IOCTL_H__

#include "lib_types.h"
#include "boardcfg.h"

/*  *********************************************************************
    *  NVRAM and FLASH stuff
    ********************************************************************* */

#define IOCTL_NVRAM_GETINFO	1	/* return nvram_info_t */
#define IOCTL_NVRAM_ERASE	2	/* erase sector containing nvram_info_t area */
#define IOCTL_FLASH_ERASE_SECTOR 3	/* erase an arbitrary sector */
#define IOCTL_FLASH_ERASE_ALL   4	/* Erase the entire flash */
#define IOCTL_FLASH_GETINFO	6	/* get flash device info */
#define IOCTL_FLASH_GETSECTORS	7	/* get sector information */
#define IOCTL_FLASH_ERASE_RANGE 8	/* erase range of bytes */
#define IOCTL_NVRAM_UNLOCK	9	/* allow r/w beyond logical end of device */
#define IOCTL_FLASH_PROTECT_RANGE 10	/* Protect a group of sectors */
#define IOCTL_FLASH_UNPROTECT_RANGE 11	/* unprotect a group of sectors */
#define IOCTL_FLASH_DATA_WIDTH_MODE	12	/* switch flash and gen bus to support 8 or 16-bit mode I/Os */
#define IOCTL_FLASH_BURST_MODE	13	/* configure gen bus for burst mode */
#define IOCTL_FLASH_GETPARTINFO	14	/* get flash partition info */
#define IOCTL_FLASH_ERASE_BLOCK	15	/* erase an arbitrary block */
#define IOCTL_FLASH_HANDLE_PARTITION_READ_DISTURB 16	/* read disturbance */
/* RPMB (eMMC Replay Protected Memory Block) device stuff */
#define IOCTL_FLASH_RPMB_USE_KEY	17	/* Specify the key to be used */
#define IOCTL_FLASH_RPMB_PROGRAM_KEY	18	/* Program authentication key */
#define IOCTL_FLASH_RPMB_GET_WRITE_COUNTER 19	/* get RPMB Write Counter */
/* USB BDC driver (Broadcom Device Controller) */
#define IOCTL_USBBDC_GET_BULKOUT_MINSIZE	20
#define IOCTL_USBBDC_GET_BULKOUT_MAXSIZE	21
#define IOCTL_USBBDC_QUEUE_BULKOUT		22

typedef struct flash_range_s {
	uint64_t range_base;
	uint64_t range_length;
} flash_range_t;

struct flash_info {
	uint64_t flash_base;	/* flash physical base address */
	uint64_t flash_size;	/* available device size in bytes */
	unsigned int type;	/* the flash type */
	unsigned int flags;	/* Various flags (FLASH_FLAG_xxx) */
	unsigned int page_size; /* size of a page */
};

typedef struct flash_sector_s {
	unsigned int flash_sector_idx;
	int flash_sector_status;
	unsigned int flash_sector_offset;
	unsigned int flash_sector_size;
} flash_sector_t;

#define FLASH_SECTOR_OK		0
#define FLASH_SECTOR_INVALID	-1

/* Alias the BOOT_FROM_* macros as a generic FLASH_TYPE_* */
enum flash_type {
	FLASH_TYPE_UNKNOWN	= BOOT_DEV_ERROR, /* unknown: not a flash? */
	FLASH_TYPE_NOR		= BOOT_FROM_NOR,  /* parallel NOR flash */
	FLASH_TYPE_SPI		= BOOT_FROM_SPI,  /* SPI NOR flash */
	FLASH_TYPE_NAND		= BOOT_FROM_NAND, /* NAND flash */
	FLASH_TYPE_EMMC		= BOOT_FROM_EMMC, /* eMMC flash */
};

#define FLASH_FLAG_NOERASE	1	/* Byte-range writes supported,
					   Erasing is not necessary */

typedef struct nvram_info_s {
	unsigned long nvram_offset;	/* offset of environment area */
	unsigned long nvram_size;	/* size of environment area */
	unsigned long nvram_offs_part;	/* offset into partition */
	int nvram_eraseflg;	/* true if we need to erase first */
} nvram_info_t;

enum ether_phy_type {
	ETH_PHY_NA,
	ETH_EPHY,
	ETH_GPHY,
	ETH_SF2,
};

struct ether_phy_info {
	enum ether_phy_type type;
	unsigned long *phyaddr;
	int version;
};

/*  *********************************************************************
    *  Ethernet stuff
    ********************************************************************* */

#define IOCTL_ETHER_GETHWADDR	1	/* Get hardware address (6bytes) */
#define IOCTL_ETHER_SETHWADDR   2	/* Set hardware address (6bytes) */
#define IOCTL_ETHER_GETSPEED    3	/* Get Speed and Media (int) */
#define IOCTL_ETHER_SETSPEED    4	/* Set Speed and Media (int) */
#define IOCTL_ETHER_GETLINK	5	/* get link status (int) */
#define IOCTL_ETHER_GETLOOPBACK	7	/* get loopback state */
#define IOCTL_ETHER_SETLOOPBACK	8	/* set loopback state */
#define IOCTL_ETHER_SETPACKETFIFO 9	/* set packet fifo mode (int) */
#define IOCTL_ETHER_SETSTROBESIG 10	/* set strobe signal (int) */

/* Added for ephytest */
#define IOCTL_ETHER_GET_PHY_REGBASE	11 /* Depricated */
#define IOCTL_ETHER_GET_PORT_PHYID	12 /* Get the PHY ID for a given port */
#define IOCTL_ETHER_GET_MDIO_PHYID	13 /* Get the PHY ID for the mdio */
#define IOCTL_ETHER_SET_PHY_DEFCONFIG	14 /* Apply default phy workarounds */
#define IOCTL_ETHER_GET_PHY_INFO	15
#define IOCTL_ETHER_SETMULTICAST_HWADDR	16
#define ETHER_LOOPBACK_OFF	0	/* no loopback */
#define ETHER_LOOPBACK_MAC_INT	1	/* MAC Internal loopback */
#define ETHER_LOOPBACK_MAC_EXT	2	/* MAC External loopback */
#define ETHER_LOOPBACK_PHY_INT	3	/* PHY Internal loopback */
#define ETHER_LOOPBACK_PHY_EXT	4	/* External loopback (through PHY) */

#define ETHER_SPEED_AUTO	0	/* Auto detect */
#define ETHER_SPEED_UNKNOWN	0	/* Speed not known (on link status) */
#define ETHER_SPEED_10HDX	1	/* 10MB hdx and fdx */
#define ETHER_SPEED_10FDX	2
#define ETHER_SPEED_100HDX	3	/* 100MB hdx and fdx */
#define ETHER_SPEED_100FDX	4
#define ETHER_SPEED_1000HDX	5	/* 1000MB hdx and fdx */
#define ETHER_SPEED_1000FDX	6

#define ETHER_FIFO_8		0	/* 8-bit packet fifo mode */
#define ETHER_FIFO_16		1	/* 16-bit packet fifo mode */
#define ETHER_ETHER		2	/* Standard ethernet mode */

#define ETHER_STROBE_GMII	0	/* GMII style strobe signal */
#define ETHER_STROBE_ENCODED	1	/* Encoded */
#define ETHER_STROBE_SOP	2	/* SOP flagged. Only in 8-bit mode */
#define ETHER_STROBE_EOP	3	/* EOP flagged. Only in 8-bit mode */

/*  *********************************************************************
    *  Serial Ports
    ********************************************************************* */

#define IOCTL_SERIAL_SETSPEED	1	/* get baud rate (int) */
#define IOCTL_SERIAL_GETSPEED	2	/* set baud rate (int) */
#define IOCTL_SERIAL_SETFLOW	3	/* Set Flow Control */
#define IOCTL_SERIAL_GETFLOW	4	/* Get Flow Control */

#define SERIAL_FLOW_NONE	0	/* no flow control */
#define SERIAL_FLOW_SOFTWARE	1	/* software flow control (not impl) */
#define SERIAL_FLOW_HARDWARE	2	/* hardware flow control */

/*  *********************************************************************
    *  Block device stuff
    ********************************************************************* */

#define IOCTL_BLOCK_GETBLOCKSIZE 1	/* get block size (int) */
#define IOCTL_BLOCK_GETTOTALBLOCKS 2	/* get total bocks (long long) */
#define IOCTL_BLOCK_GETDEVTYPE 3	/* get device type (struct) */

typedef struct blockdev_info_s {
	unsigned long long blkdev_totalblocks;
	unsigned int blkdev_blocksize;
	unsigned int blkdev_devtype;
} blockdev_info_t;

#define BLOCK_DEVTYPE_DISK	0
#define BLOCK_DEVTYPE_CDROM	1

#endif /* __IOCTL_H__ */
