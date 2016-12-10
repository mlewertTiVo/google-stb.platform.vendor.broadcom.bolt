/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

#ifndef __BOARD_H__
#define __BOARD_H__

#include "lib_types.h"
#include "ssbl-common.h"
#include "lib_string.h"
#include "arch_ops.h"
#ifdef DVFS_SUPPPORT
#include "pmap.h"
#endif

#define get_cpu_freq_mhz() ((uint32_t)(arch_get_cpu_freq_hz() / 1000000))
#define get_timer_freq_mhz() (arch_get_timer_freq_hz() / 1000000)
#define get_clocks_per_usec() get_timer_freq_mhz()


/* SWBOLT-99 (bcm_init_history)
*/
void         aon_reset_history(void);
uint32_t get_aon_reset_history(void);
char         *aon_reset_as_string(void);

/* board
*/
void                board_printinfo(void);
void                board_print_ddrspeed(void);
unsigned int        board_bootmode(void);
const char         *board_name(void);
const enet_params  *board_enet(int instance);
unsigned int        board_num_active_memc(void);
unsigned int        board_num_enet(void);
const moca_params  *board_moca(void);
void                board_pinmux(void);
struct board_type  *board_thisboard(void);
uint32_t            board_totaldram( void ); /* in Mb */
struct ddr_info    *board_find_ddr(struct board_type *b, uint32_t idx);
dt_ops_s           *board_dt_ops(void);
const struct dvfs_params *board_dvfs(void);
char               *board_init_current_rts(void);
int                 board_init_current_rts_boxmode(void);
const sdio_params  *board_sdio(int sdio);
const gpio_key_params *board_gpio_keys(void);
const bt_rfkill_params *board_bt_rfkill_gpios(void);
#ifdef DVFS_SUPPPORT
unsigned int        board_pmap(void);
#endif

/* generic access */
const struct ssbl_board_params *board_current_params(void);

struct partition_profile *board_flash_partition_table(void);

/* ssbl_main
*/
struct fsbl_info *board_info(void);

/* board_update
*/
int board_check(int force);

/* enet helpers */
static inline int enet_params_cmp(const enet_params *e,
				  char *phy_type,
				  char *mdio_mode,
				  char *phy_speed,
				  char *phy_id)
{
	return (strcmp(e->phy_type, phy_type) ||
		(e->mdio_mode && strcmp(e->mdio_mode, mdio_mode)) ||
		strcmp(e->phy_speed, phy_speed) ||
		strcmp(e->phy_id, phy_id));
}


#endif
