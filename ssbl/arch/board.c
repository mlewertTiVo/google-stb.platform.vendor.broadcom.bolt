/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <board.h>
#include <board_init.h>
#include <board_params.h>
#include <bchp_sun_top_ctrl.h>
#include <ddr.h>
#include <error.h>
#include <lib_physio.h>
#ifdef DVFS_SUPPORT
#include <pmap.h>
#endif

#include <stdbool.h>

/*
 * No floating point in BOLT so we have to scale. The
 * values are in MHz so plenty of room (0..21474).
 */
#define SIGNIFICANCE	1000
#define PERCENT_SIGNIFICANCE (100 * SIGNIFICANCE)

static void board_print_ddr_ssc(int which, uint32_t divisor)
{
	uint32_t ratio, dividend;

	if (memsys_ssbl_ddr_info(which, &dividend, NULL, NULL, NULL))
		return;

	/* Can't do rational ratios with infinities. */
	if ((0 == dividend) || (0 == divisor))
		return;

	ratio = (dividend * PERCENT_SIGNIFICANCE) / (divisor * 100);

	xprintf("DDR%d Actual frequency: %dMHz, ", which, dividend);

	/*
	 * NOTE: If you change SIGNIFICANCE then you *must* match it to the
	 * significant places i.e. "%03u" in the xprintf line below.
	 */
	xprintf("ratio %u.%03u\n",
		ratio / SIGNIFICANCE , ratio % SIGNIFICANCE);
}


static struct fsbl_info *get_inf(void)
{
	struct fsbl_info *inf = board_info();

	if (!inf || (inf->board_idx >= FSBLINFO_N_BOARDS(inf->n_boards)))
		return NULL;

	return inf;
}


/*  *********************************************************************
    *  board_config_info()
    *
    *  Display board configuration messages
    *
    *  Input parameters:
    *	nothing
    *
    *  Return value:
	*	nothing
    ********************************************************************* */
void board_printinfo(void)
{
	struct board_type *b;
	struct ddr_info *ddr;
	char name[20];
	char value[20];
	unsigned int i;
	const sdio_params *s;
	int x;

	xprintf("     CPU speed: %dMHz\n", get_cpu_freq_mhz());

	b = board_thisboard();
	if (b) {
		for (i = 0; i < b->nddr; i++) {
			ddr = &(b->ddr[i]);
			if (!ddr)
				continue; /* ok, so I'm paranoid. */

			xprintf("DDR%d Frequency: %dMHz, ",
				ddr->which, ddr->ddr_clock);
			if (ddr->ddr_size < 1024)
				xprintf("%dM", ddr->ddr_size);
			else {
				uint32_t ddr_size = ddr->ddr_size / 1024;

				if (ddr_size < 1024)
					xprintf("%dG", ddr_size);
				else {
					ddr_size /= 1024;
					xprintf("%dT", ddr_size);
				}
			}
			xprintf("x%d phy:%d\t", ddr->ddr_width, ddr->phy_width);
			xprintf("%08x @ %08x\n",
				_MB(ddr->size_mb), _MB(ddr->base_mb));

			xsprintf(name, "DRAM%d_SIZE", ddr->which);
			xsprintf(value, "%d", ddr->size_mb);
			env_setenv(name, value,
				ENV_FLG_BUILTIN | ENV_FLG_READONLY);
		}

		/*
		 * Spin round the DDRs again, this time reporting the
		 * actual frequency and a ratio between it and the
		 * nominal. We could do this in the previous loop
		 * but could be hitting line widths of > 100 chars.
		 */
		for (i = 0; i < b->nddr; i++) {
			ddr = &(b->ddr[i]);
			if (!ddr)
				continue;
			board_print_ddr_ssc(ddr->which, ddr->ddr_clock);
		}

	}
	xprintf("  Total memory: %dMB\n", board_totaldram());

	xprintf("   Boot Device: ");
	switch (board_bootmode()) {
	case BOOT_FROM_NAND:
		xprintf("NAND");
		break;
	case BOOT_FROM_SPI:
		xprintf("SPI");
		break;
	case BOOT_FROM_EMMC:
		xprintf("EMMC");
		break;
	default:
		xprintf("NOR");
		break;
	}
	xprintf("\n");

	xprintf("SDIO:\n");
	for (x = 0; x < NUM_SDIO; x++) {
		s = board_sdio(x);
		xprintf(" controller %d, TYPE: ", x);
		if (!s || (s->type == SDIO_TYPE_NODEVICE)) {
			xprintf("NODEVICE\n");
			continue;
		}
		switch (s->type) {
		case SDIO_TYPE_SD:
			xprintf("SD");
			break;
		case SDIO_TYPE_EMMC:
			xprintf("EMMC");
			break;
		case SDIO_TYPE_EMMC_ON_SDPINS:
			xprintf("EMMC on SD Pins");
			break;
		default:
			xprintf("???");
			break;
		}
		xprintf("\n");
	}
}

void board_print_ddrspeed(void)
{
	unsigned int i;
	struct board_type *b;

	b = board_thisboard();
	if (!b)
		return;

	for (i = 0; i < b->nddr; i++) {
		struct ddr_info *ddr = &(b->ddr[i]);

		if (i != 0)
			xprintf(", ");

		xprintf("DDR%d ", ddr->which);
		if (ddr->ddr_size == 0)
			xprintf("N/A");
		else
			xprintf("@ %dMHz", ddr->ddr_clock);
	}
	xprintf("\n");
}

unsigned int board_bootmode(void)
{
	uint32_t boot_strap_val;

	boot_strap_val = REG(BCHP_SUN_TOP_CTRL_STRAP_VALUE_0);

	boot_strap_val =
	    (boot_strap_val &
	     BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_MASK) >>
	    BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_SHIFT;

	if (boot_strap_val <=
	    SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_NAND_VALUE)
		return BOOT_FROM_NAND;
	else if (boot_strap_val <=
		 SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_SPI_VALUE)
		return BOOT_FROM_SPI;
	else if (boot_strap_val <=
		 SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_PNOR_VALUE)
		return BOOT_FROM_NOR;
	else if (boot_strap_val ==
		 SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_eMMC_VALUE)
		return BOOT_FROM_EMMC;
	else if (boot_strap_val ==
		 SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_SPI_4B_VALUE)
		return BOOT_FROM_SPI;
	else
		return BOOT_DEV_ERROR;
}

bool board_does_strap_disable_pcie(void)
{
#ifdef BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_pcie_sata_MASK
	/* strap_pcie_sata == 0 means SATA over PCIe */
	return 0 == (BDEV_RD(BCHP_SUN_TOP_CTRL_STRAP_VALUE_0) &
		BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_pcie_sata_MASK);
#endif

	return false; /* no way to disable PCIe */
}

bool board_does_strap_disable_sata(void)
{
#ifdef BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_pcie_sata_MASK
	/* strap_pcie_sata != 0 means PCIe over SATA */
	return 0 != (BDEV_RD(BCHP_SUN_TOP_CTRL_STRAP_VALUE_0) &
		BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_pcie_sata_MASK);
#endif

	return false; /* no way to disable SATA */
}

struct board_type *board_thisboard(void)
{
	struct fsbl_info *inf = get_inf();

	if (!inf)
		return NULL;

	return get_board_type(inf);
}


struct partition_profile *board_flash_partition_table(void)
{
	struct fsbl_info *inf = get_inf();

	if (!inf)
		return NULL;

	return board_params[inf->board_idx].mapselect;
}

const struct nand_feature *board_flash_nand_feature(void)
{
	struct fsbl_info *inf = get_inf();

	if (!inf)
		return NULL;

	return board_params[inf->board_idx].nand_feature;
}

const struct dvfs_params *board_dvfs(void)
{
	struct fsbl_info *inf = get_inf();

	if (!inf)
		return NULL;

	return &(board_params[inf->board_idx].dvfs);
}

#ifdef DVFS_SUPPORT
unsigned int board_pmap_index(unsigned int pmap)
{
	unsigned int i = 0, pmaps_count = 0;

	pmaps_count = sizeof(pmapTable) / sizeof(pmapTable[0]);
	for (i = 0; i < pmaps_count; i++) {
		if (pmap == pmapTable[i].pmapId)
			break;
	}

	return i;
}

int is_pmap_valid(unsigned int pmap)
{
	int index = board_pmap_index(pmap);

	if ((pmapTable[index].pmapId == PMap_eMax) ||
		(pmapTable[index].pmapId != pmap)) {
		err_msg("PMap %d is invalid.", pmap);
		return BOLT_ERR;
	}

	return BOLT_OK;
}

unsigned int board_pmap(void)
{
	struct fsbl_info *inf = board_info();
	unsigned int pmap_id;

	if (!inf || (inf->board_idx >= inf->n_boards))
		return PMAP_MAX;

	pmap_id = FSBL_HARDFLAG_PMAP_ID(inf->saved_board.hardflags);
	if (pmap_id == FSBL_HARDFLAG_PMAP_BOARD) {
		/* has not been overridden */
		pmap_id = AVS_PMAP_ID(inf->board_types[inf->board_idx].avs);
	}

	return pmap_id;
}
#endif

const char *board_name(void)
{
	struct board_type *b;
	const char *c;

	do {
		b = board_thisboard();
		if (!b)
			break;

		c = b->name;
		if (!c)
			break;
		return c;

	} while (0);

	return NULL;
}


uint32_t board_totaldram(void)
{
	struct board_type *b;
	unsigned int i;
	static uint32_t acc = 0U;

	/* do once only
	*/
	if (acc)
		return acc;

	b = board_thisboard();
	if (!b)
		return 0;

	for (i = 0; i < b->nddr; i++)
		acc += b->ddr[i].size_mb;

	return acc;
}


const enet_params *board_enet(int instance)
{
	struct fsbl_info *inf = get_inf();
	const enet_params *e;

	if (!inf)
		return NULL;

	e = board_params[inf->board_idx].enet;

	while (e->genet != -1 || e->switch_port != -1) {
		/* GENET_<N> or matching switch port */
		if (e->genet == instance || e->switch_port == instance)
			return e;
		e++;
	}

	return NULL;
}

const sdio_params *board_sdio(int sdio)
{
	struct fsbl_info *inf = get_inf();
	const sdio_params *s;
	int i;

	if (!inf || (sdio >= NUM_SDIO) || (sdio < 0))
		return NULL;

	s = board_params[inf->board_idx].sdio;

	for (i = 0; i < NUM_SDIO; i++) {
		if (s[i].index == -1)
			return NULL;
		if (s[i].index == sdio)
			return &(s[i]);
	}

	return NULL;
}

unsigned int board_num_enet(void)
{
	struct fsbl_info *inf = get_inf();

	if (!inf)
		return 0;

	return NUM_ENET;
}

const moca_params *board_moca(void)
{
	struct fsbl_info *inf = get_inf();

	if (!inf)
		return NULL;

	return board_params[inf->board_idx].moca;
}

const ext_moca_params *board_ext_moca(int instance)
{
	struct fsbl_info *inf = get_inf();
	const ext_moca_params *m;

	if (!inf)
		return NULL;

	m = board_params[inf->board_idx].external_moca;

	while (m->rgmii != -1) {
		if (m->rgmii == instance)
			return m;
		m++;
	}

	return NULL;
}

const gpio_key_params *board_gpio_keys(void)
{
	struct fsbl_info *inf = get_inf();

	if (!inf)
		return NULL;

	return board_params[inf->board_idx].gpio_key;
}

const gpio_led_params *board_gpio_leds(void)
{
	struct fsbl_info *inf = get_inf();

	if (!inf)
		return NULL;

	return board_params[inf->board_idx].gpio_led;
}

const bt_rfkill_params *board_bt_rfkill_gpios(void)
{
	struct fsbl_info *inf = board_info();

	if (!inf || (inf->board_idx >= inf->n_boards))
		return NULL;

	return board_params[inf->board_idx].bt_rfkill_gpio;
}

const struct ssbl_board_params *board_current_params(void)
{
	struct fsbl_info *inf = get_inf();

	if (!inf)
		return NULL;

	return &board_params[inf->board_idx];
}


dt_ops_s *board_dt_ops(void)
{
	struct fsbl_info *inf = get_inf();
	dt_ops_s *c;

	if (!inf)
		return NULL;

	c = (dt_ops_s *)board_params[inf->board_idx].dt_ops;
	if (c && (c[0].path != NULL))
		return c;

	return NULL;
}

static void board_control_pad_rgmii(const struct pad_params pad_rgmii[])
{
#ifdef BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1_multi_D_pad_modehv_override_MASK
	const struct pad_params *p;

	for (p = pad_rgmii; p != NULL; ++p) {
		unsigned int modehv;

		if (p->id == -1) /* end of list */
			return;

		if (p->ctrl == PAD_CTRL_NONE) /* hardware default */
			continue;

		if (p->ctrl == PAD_CTRL_HIGH) {
			modehv = 1;
		} else {
			/* PAD_CTRL_LOW */
			modehv = 0;
		}

		switch (p->id) {
		case 0:
			BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_1,
				multi_D_pad_modehv_override, 1);
			BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_0,
				rgmii_0_pad_modehv, modehv);
			break;
		case 1:
			BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_1,
				multi_E_pad_modehv_override, 1);
			BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_0,
				rgmii_1_pad_modehv, modehv);
			break;
		default:
			return;
		}
	}
#endif
}

void board_pinmux(void)
{
	struct fsbl_info *inf = get_inf();
	const struct reg_update *extra_mux;

	if (!inf)
		return;

	board_control_pad_rgmii(board_params[inf->board_idx].pad_rgmii);

	if (board_params[inf->board_idx].pinmuxfn)
		board_params[inf->board_idx].pinmuxfn();

	/* certain chips require extra pinmux depending on selected SDIO pins */
	extra_mux = &board_params[inf->board_idx].sdio_pinsel;
	if (extra_mux->reg != 0) {
		unsigned long regval = BDEV_RD(extra_mux->reg);

		regval = (regval & ~extra_mux->mask) | extra_mux->val;
		BDEV_WR(extra_mux->reg, regval);
	}
}


struct ddr_info *board_find_ddr(struct board_type *b, uint32_t idx)
{
	struct ddr_info *ddr;
	unsigned int i;

	if (!b)
		return NULL;

	for (i = 0; i < b->nddr; i++) {
		ddr = &(b->ddr[i]);
		if (!ddr_is_populated(ddr))
			continue;
		if (ddr->which == idx)
			return ddr;
	}
	return NULL;
}

/* board_num_active_memc -- number of active memory controllers
 *
 * returns the number of active memory controllers on board
 */
unsigned int board_num_active_memc(void)
{
	int i;
	unsigned int num_active_memc;
	struct board_type *b = board_thisboard();

	/* at least one memory controller, otherwise how do you find
	 * yourself at this location?
	 */
	if (!b)
		return 1;

	num_active_memc = 0;
	for (i = 0; i < b->nddr; i++) {
		struct ddr_info *ddr = &(b->ddr[i]);

		if (ddr->ddr_size != 0)
			num_active_memc++;
	}

	return num_active_memc;
}

