/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

#ifndef __BOARD_INIT_H__
#define __BOARD_INIT_H__

#include <bolt.h>
#include <common.h>
#include <console.h>
#include <dev_emmcflash.h>
#include <dev_norflash.h>
#include <devfuncs.h>
#include <device.h>
#include <env_subr.h>
#include <flash-partitions.h>
#include <iocb.h>
#include <ioctl.h>
#include <lib_printf.h>
#include <lib_string.h>
#include <lib_types.h>
#include <macaddr.h>
#include <ssbl-common.h>
#include <timer.h>
#include <ui_command.h>
#include <usbdt.h>

#include <stdint.h>

/* driver handles
*/
#if CFG_GENET
extern bolt_driver_t genetdrv;
extern char *eth0_phyintf;
#endif
#if CFG_SYSTEMPORT
extern bolt_driver_t sysportdrv;
#endif
#if CFG_SATA
extern bolt_driver_t satadrv;
#endif
#if CFG_NAND_FLASH
extern bolt_driver_t nandflashdrv;
#endif
extern bolt_driver_t spiflashdrv;
extern bolt_driver_t emmcflashdrv;
extern bolt_driver_t emmcflashdrv_secondary;
extern bolt_driver_t norflashdrv;
extern bolt_driver_t bcm97XXX_uart;
extern bolt_driver_t dev_mem;


/* bsp init functions
*/
struct usb_controller_list *usb_dt_init(void);
int usb_board_init(struct usb_controller_list *clist);
int usb_init(int usbmode);
int usb_exit(void);

void board_init_avs(void);
void board_init_rts(void);
int board_init_rts_current_boxmode(void);
int board_init_rts_update(int rts_id);

void board_init_sata(void);
void sata_exit(void);

void board_init_flash(void);
void board_init_hdmi(void);
void board_init_reset_history(void);
uint32_t board_init_reset_history_value(void);
char *board_init_reset_history_string(void);

void board_init_enet(void);
void enet_exit(void);

#endif /* __BOARD_INIT_H__ */

