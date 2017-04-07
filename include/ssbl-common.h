/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

#ifndef __SSBL_COMMON_H__
#define __SSBL_COMMON_H__

#include "flash-partitions.h"
#include "fsbl-common.h"
#include "avs_dvfs.h"

typedef struct enet_params {
	int genet;
	int switch_port;
	uint32_t base;
	uint32_t umac_base;
	char *phy_type;
	char *mdio_mode;
	char *phy_speed;
	char *phy_id;
	char *ethsw;
} enet_params;

typedef struct {
	uint32_t base;
	char *enet_node;
	char *moca_band;
} moca_params;

typedef struct {
	const char *name;  /* Name given to gpio button */
	const char *gpio;  /* Name of the connected GPIO controller i.e. upg_gio or upg_gio_aon */
	unsigned int pin;  /* Pin Value of the connected gpio */
	unsigned int code; /* Key Code generated from the gpio button */
} gpio_key_params;

typedef struct bt_rfkill_params
{
	const char *name; /* Name of the gpio */
	const char *gpio; /* Name of the connected GPIO controller i.e. upg_gio or upg_gio_aon */
	unsigned int pin; /* Pin value of the connected gpio */
	unsigned int pol; /* Polarity of the connected gpio */
	unsigned int pdelay; /* Delay required post change of GPIO state */
}
bt_rfkill_params;

typedef void(fpinmux_t)(void);


typedef enum {
	DT_OP_NONE = 0,
	DT_OP_CULL,
	DT_OP_SPROP,
	DT_OP_IPROP,
	DT_OP_VPROP,
	DT_OP_MAC,
	DT_OP_BPROP,
	DT_OP_COMPILE_PROP,
	DT_OP_ADD_NODE,
} dt_ops_e;

typedef struct {
	const dt_ops_e op;
	const char *path;
	const char *node;
	const char *prop;
	const char *svalue;
	uint32_t ivalue;
	const uint32_t *vvalue;
} dt_ops_s;

typedef enum {
	SDIO_TYPE_NODEVICE = -1,
	SDIO_TYPE_SD = 0,
	SDIO_TYPE_EMMC,
	SDIO_TYPE_EMMC_ON_SDPINS,
} sdio_type_e;

typedef enum {
	SDIO_DRIVER_STRENGTH_UNDEFINED = 0,
	SDIO_DRIVER_STRENGTH_A = 1,
	SDIO_DRIVER_STRENGTH_B = 2,
	SDIO_DRIVER_STRENGTH_C = 3,
	SDIO_DRIVER_STRENGTH_D = 4,
} sdio_driver_strength_e;

typedef struct {
	const int index;
	const sdio_type_e type;
	const int uhs;
	const sdio_driver_strength_e host_driver_strength;
	const sdio_driver_strength_e host_hs_driver_strength;
	const sdio_driver_strength_e card_driver_strength;
} sdio_params;

struct reg_update {
	uint32_t reg;
	uint32_t mask;
	uint32_t val;
};

struct dvfs_params {
	enum avs_mode mode;
	uint32_t pmap;
	uint32_t pstate;
} dvfs_params;

typedef struct ssbl_board_params
{
	uint32_t     rtsdefault;
	enet_params  enet[NUM_ENET+1];
	moca_params  moca[NUM_MOCA+1];
	sdio_params  sdio[NUM_SDIO+1];
	struct reg_update sdio_pinsel;
	gpio_key_params gpio_key[MAX_GPIO_KEY+1];
	bt_rfkill_params bt_rfkill_gpio[MAX_BT_RFKILL_GPIO+1];
	fpinmux_t   *pinmuxfn;
	const dt_ops_s *dt_ops;
	struct partition_profile *mapselect;
	struct  dvfs_params  dvfs;
} ssbl_board_params;


struct rts {
	char     *name;
	uint32_t  number; /* of lists of values */
	uint32_t  rtsdefault; /* user defined id  */
	uint32_t *values[];
};

struct aon_history {
	uint32_t  mask;
	char     *name;
};


struct otp_status {
	uint32_t  reg;
	uint32_t  mask;
	char     *name;
};

/* FSBL provided array of boards. */
static inline struct board_type *get_board_type(struct fsbl_info *info)
{
       return (struct board_type *)&(info->board_types[info->board_idx]);
}

#endif
