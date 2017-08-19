/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef _TZ_PRIV_H_
#define _TZ_PRIV_H_

#include <loader.h>
#include <stdint.h>

typedef enum { e_tz_spd_image, e_tz_tz_image, e_tz_nw_image } tz_payload_type;

struct tz_reg_group {
	const char	*compatible;
	uint32_t	start;
	uint32_t	end;
};

struct tz_mem_layout {
	/* offset from mem_addr */
	uint64_t	os_offset;
	uint64_t	os_size;

	uint64_t	tzioc_offset;
	uint64_t	tzioc_size;

	/* offset from tzioc_offset */
	uint32_t	t2n_offset;
	uint32_t	t2n_size;
	uint32_t	n2t_offset;
	uint32_t	n2t_size;

	uint32_t	tzheaps_offset;
	uint32_t	tzheaps_size;
	uint32_t	nwheaps_offset;
	uint32_t	nwheaps_size;
};

struct tz_info {
	uint32_t	uart_base;
	uint32_t	uart_nwos;

	uint32_t	which;
	uint64_t	mem_addr;
	uint64_t	mem_size;

	uint64_t	bl31_addr;
	uint64_t	bl31_size;

	struct tz_mem_layout	*mem_layout;
	struct tz_reg_group	*reg_groups;

	uint8_t		tzioc_irq;

	void		*dt_addr;
};

struct tz_info *tz_info(void);

/* tz_config */
extern struct tz_reg_group s_tz_reg_groups[];
extern struct tz_mem_layout s_tz_mem_layout_16MB;
extern struct tz_mem_layout s_tz_mem_layout_32MB;

int tz_config_init(void);
int tz_config_uart(int uart);

void tz_smm_set_params(bolt_loadargs_t *la);

#endif /* _TZ_PRIV_H_ */
