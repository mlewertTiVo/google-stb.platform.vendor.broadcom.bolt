/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

#ifndef __BOARD_H__
#define __BOARD_H__

#include <arch_ops.h>
#include <lib_string.h>
#include <lib_types.h>
#include <ssbl-common.h>

#include <stdbool.h>

#define get_cpu_freq_mhz() ((uint32_t)(arch_get_cpu_freq_hz() / 1000000))
#define get_timer_freq_mhz() (arch_get_timer_freq_hz() / 1000000)
#define get_clocks_per_usec() get_timer_freq_mhz()

/* board
*/
void                board_printinfo(void);
void                board_print_ddrspeed(void);
unsigned int        board_bootmode(void);
bool                board_does_strap_disable_pcie(void);
bool                board_does_strap_disable_sata(void);
const char         *board_name(void);
const enet_params  *board_enet(int instance);
unsigned int        board_num_active_memc(void);
unsigned int        board_num_enet(void);
const moca_params  *board_moca(void);
const ext_moca_params *board_ext_moca(int instance);
void                board_pinmux(void);
struct board_type  *board_thisboard(void);
uint32_t            board_totaldram( void ); /* in Mb */
struct ddr_info    *board_find_ddr(struct board_type *b, uint32_t idx);
dt_ops_s           *board_dt_ops(void);
const struct dvfs_params *board_dvfs(void);
const sdio_params  *board_sdio(int sdio);
const gpio_key_params *board_gpio_keys(void);
const gpio_led_params *board_gpio_leds(void);
const bt_rfkill_params *board_bt_rfkill_gpios(void);
#ifdef DVFS_SUPPORT
unsigned int board_pmap_index(unsigned int pmap);
int is_pmap_valid(unsigned int pmap);
unsigned int        board_pmap(void);
#endif

/* generic access */
const struct ssbl_board_params *board_current_params(void);

struct partition_profile *board_flash_partition_table(void);
const struct nand_feature *board_flash_nand_feature(void);

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
