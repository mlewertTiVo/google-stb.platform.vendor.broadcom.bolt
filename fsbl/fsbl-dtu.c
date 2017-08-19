/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <aon_defs.h>
#include <bchp_common.h>
#include <boot_defines.h>
#include <common.h>
#include <fsbl.h>
#include <fsbl-common.h>
#include <fsbl-dtu.h>
#include <stdbool.h>

#ifdef BCHP_MEMC_DTU_CONFIG_0_REG_START
#include <bchp_memc_dtu_config_0.h>
#include <bchp_memc_dtu_map_state_0.h>
#include <bchp_memc_l2_0_1.h>
#endif
#ifdef BCHP_MEMC_DTU_CONFIG_1_REG_START
#include <bchp_memc_dtu_config_1.h>
#include <bchp_memc_dtu_map_state_1.h>
#include <bchp_memc_l2_0_2.h>
#endif

/* All the below macros are valid for the chips that have DTU.
 *   Below #ifdef is used to remove the compilation errors for non-dtu chips
 */
#ifdef BCHP_MEMC_DTU_CONFIG_0_REG_START

#define DTU_MAP_STATE_VALUE(idx) \
	((BCHP_MEMC_DTU_MAP_STATE_0_MAP_STATEi_COMMAND_CMD_MAP & \
		BCHP_MEMC_DTU_MAP_STATE_0_MAP_STATEi_COMMAND_MASK) | \
	(((idx) << BCHP_MEMC_DTU_MAP_STATE_0_MAP_STATEi_DEVICE_PAGE_SHIFT) & \
		BCHP_MEMC_DTU_MAP_STATE_0_MAP_STATEi_DEVICE_PAGE_MASK))

#define DTU_VALID_ENTRY(value) \
	((value) & BCHP_MEMC_DTU_MAP_STATE_0_MAP_STATEi_VALID_MASK)

#define DTU_SET_VALID_ENTRY_CMD(value) \
	((value) | BCHP_MEMC_DTU_MAP_STATE_0_MAP_STATEi_COMMAND_CMD_MAP)

#define DTU_OWNED_ENTRY(value) \
	((value) & BCHP_MEMC_DTU_MAP_STATE_0_MAP_STATEi_OWNED_MASK)

#define DTU_SET_OWNED_ENTRY_CMD(value) \
	((value) | BCHP_MEMC_DTU_MAP_STATE_0_MAP_STATEi_COMMAND_CMD_SETOWN)

#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

/* dtu_page_index.
 * computes the dtu(bus page) index for a given address
 *
 * NOTE: The caller MUST ensure that 'addr' is higher than 'base_addr_mb'.
 */
static unsigned int dtu_page_index(uintptr_t addr, unsigned int base_addr_mb)
{
	uintptr_t offset_mb;

	offset_mb = addr / _MB(1) - base_addr_mb;
	return offset_mb / DTU_PAGE_SIZE_MB;
}

/* dtu_memory_range_check
 * Verify whether memory area[addr, (addr+size-1)] resides in given
 * ddr[ddr_base_mb, (ddr_base_mb + ddr_size_mb-1)]. return true on succesful
 * range check, else false.
 */
static bool dtu_memory_range_check(uint32_t ddr_base_mb,
		uint32_t ddr_size_mb, uintptr_t addr, unsigned int size)
{
	if (ddr_base_mb > (addr / _MB(1)))
		return false;

	if ((ddr_base_mb + (ddr_size_mb - 1)) < ((addr + (size - 1)) / _MB(1)))
		return false;

	return true;
}

/*
 * dtu_memory_page_index
 * Finds the DTU bus page indices of a given memory range and returns true
 * if the range belongs to the given DDR. If not, false is returned.
 */
static bool dtu_memory_page_index(struct ddr_info *ddr, uintptr_t addr,
				unsigned int size, int *index_0, int *index_1)
{
	/* check whether address range [addr, (addr + size-1)]
	 * belongs to this ddr(memory controller)
	 */
	if (dtu_memory_range_check(ddr->base_mb, ddr->size_mb, addr, size)) {
		/* Bus page index for addr - Start address */
		*index_0 = dtu_page_index(addr, ddr->base_mb);
		/* Bus page index for addr - End address */
		*index_1 = dtu_page_index(addr + (size - 1), ddr->base_mb);
		return true;
	}
	return false;
}

/*
 * dtu_reload_done.
 * Gets the DTU Reload status
 */
static bool dtu_reload_done(unsigned int reg_addr, uint32_t reload_status)
{
	uint32_t done;

	done = DEV_RD(reg_addr) &
		BCHP_MEMC_DTU_CONFIG_0_RELOAD_STATUS_RELOADING_MASK;

	return done == reload_status;
}

/*
 * dtu_translate_done.
 * Gets the DTU Translation status
 */
static bool dtu_translate_done(unsigned int reg_addr, uint32_t translate_status)
{
	uint32_t done;

	done = DEV_RD(reg_addr) &
		BCHP_MEMC_DTU_CONFIG_0_TRANSLATE_STATUS_ENABLED_MASK;

	return done == translate_status;
}

/*
 * dtu_process_remap_entry
 * Verify whether this is a valid entry, then map and
 * if it is owned by someone, set the ownership
 */
static void dtu_process_remap_entry(unsigned int reg_addr, uint32_t value)
{
	/* Check for Valid entry */
	if (DTU_VALID_ENTRY(value)) {
		DEV_WR(reg_addr, DTU_SET_VALID_ENTRY_CMD(value));

		/* Check if someone owned this entry */
		if (DTU_OWNED_ENTRY(value))
			DEV_WR(reg_addr, DTU_SET_OWNED_ENTRY_CMD(value));
	}
}

/*
 * dtu_verify_entry
 * Verify whether this is a valid entry, check against
 * map value(this value was stored in S3 memory area during warmboot
 */
static bool dtu_verify_entry(unsigned int reg_addr, uint32_t value)
{
	bool verified = true;
	uint32_t reg_value;

	reg_value = DEV_RD(reg_addr);
	/* Check for Valid entry. When the valid bit is not set,
	 * dtu is not going to use that entry at all. There is no
	 * need to crosscheck any other fileds
	*/
	if (DTU_VALID_ENTRY(value) || DTU_VALID_ENTRY(reg_value)) {
		if (reg_value != value)
			verified  = false;
	}
	return verified;
}

/*
 * dtu_is_available.
 * DTU availabilty check
 */
static bool dtu_is_available(void)
{
	uint32_t major_rev_id, minor_rev_id;

	major_rev_id = BDEV_RD_F(MEMC_DTU_CONFIG_0_REV_ID, MAJOR_REV_ID);
	minor_rev_id = BDEV_RD_F(MEMC_DTU_CONFIG_0_REV_ID, MINOR_REV_ID);

	if (major_rev_id > 0 && minor_rev_id > 0)
		return true;

	return false;
}

/*
 * dtu_is_disabled.
 * checks whether DTU is disabled or improperly prepared
 */
static bool dtu_is_disabled(void)
{
	uint32_t regval;

	/* DTU cannot be used if has not been initialized */
	regval = BDEV_RD(BCHP_MEMC_DTU_CONFIG_0_INIT_STATUS);
	if (regval != BCHP_MEMC_DTU_CONFIG_0_INIT_STATUS_INITIALIZATION_DONE)
		return true;

	/* DTU cannot be used if reload has been disabled */
	regval = BDEV_RD(BCHP_MEMC_DTU_CONFIG_0_RELOAD_STATUS);
	if (regval == BCHP_MEMC_DTU_CONFIG_0_RELOAD_STATUS_RELOADING_DISABLED)
		return true;

	/* DTU cannot be used if translate has been disabled */
	regval = BDEV_RD(BCHP_MEMC_DTU_CONFIG_0_TRANSLATE);
	if (regval == BCHP_MEMC_DTU_CONFIG_0_TRANSLATE_ENABLE_DISABLE)
		return true;

	return false;
}

/*
 * dtu_config_S3.
 * Enable DTU bit for S3 parameters
 */
static inline void dtu_config_s3(void)
{
		AON_REG(AON_REG_MAGIC_FLAGS) |= S3_FLAG_DTU;
}

/*
 * dtu_config_status.
 * Read config status for boot parameters
 * return true if config parameter is set otherwise false
 */
static bool dtu_config_status(void)
{
	bool configured = false;

#if (CFG_ZEUS4_2_1 || CFG_ZEUS5_0)
	configured = (*(uint32_t *)(PARAM_DTU_ENABLE + SRAM_ADDR) != 0);
#endif
	return configured;
}

/*
 * DTU Reload
 * This is one shot register. Once enable/disabled, we cannot enable/disable
 * DTU reload must be disabled to load BFW.
 * returns true on sucessful enable/disable - false on timeout.
 */
static bool dtu_reload(unsigned int memc_index, bool enable)
{
	bool status = true;
	unsigned int timeout = 0;
	uint32_t value;
	uint32_t reload_status;
	unsigned int sreg;
	unsigned int ireg;

	if (memc_index == 0) {
		sreg = BPHYSADDR(BCHP_MEMC_DTU_CONFIG_0_RELOAD_STATUS);
		ireg = BPHYSADDR(BCHP_MEMC_DTU_CONFIG_0_RELOAD);
	}
#ifdef BCHP_MEMC_DTU_CONFIG_1_REG_START
	else {
		sreg = BPHYSADDR(BCHP_MEMC_DTU_CONFIG_1_RELOAD_STATUS);
		ireg = BPHYSADDR(BCHP_MEMC_DTU_CONFIG_1_RELOAD);
	}
#endif

	/* Enable/Disable value that needs to be initialized */
	value = enable ?
		BCHP_MEMC_DTU_CONFIG_0_RELOAD_RELOAD_ENABLE :
		BCHP_MEMC_DTU_CONFIG_0_RELOAD_RELOAD_DISABLE;

	DEV_WR(ireg, value);

	/* Status value that is expected after above setting */
	reload_status = enable ?
		BCHP_MEMC_DTU_CONFIG_0_RELOAD_STATUS_RELOADING_ENABLED :
		BCHP_MEMC_DTU_CONFIG_0_RELOAD_STATUS_RELOADING_DISABLED;

	do {
		if (dtu_reload_done(sreg, reload_status))
			break;
		udelay(DTU_DELAY);
	} while (timeout++ < DTU_TIMEOUT);

	if (timeout >= DTU_TIMEOUT) {
		puts("dtu_reload - timeout");
		status = false;
	}

	return status;
}

/*
 * DTU Transalte
 * Enable DTU translation - Need to map the state map registers before
 *                                     enable the translation.
 * return true on successful enable or false on timeout
 */
bool dtu_translate(unsigned int memc_index, bool enable)
{
	bool status = true;
	unsigned int timeout = 0;
	uint32_t value;
	uint32_t translate_status;
	unsigned int sreg;
	unsigned int ireg;

	if (memc_index == 0) {
		sreg = BPHYSADDR(BCHP_MEMC_DTU_CONFIG_0_TRANSLATE_STATUS);
		ireg = BPHYSADDR(BCHP_MEMC_DTU_CONFIG_0_TRANSLATE);
	}
#ifdef BCHP_MEMC_DTU_CONFIG_1_REG_START
	else {
		sreg = BPHYSADDR(BCHP_MEMC_DTU_CONFIG_1_TRANSLATE_STATUS);
		ireg = BPHYSADDR(BCHP_MEMC_DTU_CONFIG_1_TRANSLATE);
	}
#endif

	/* Enable/Disable value that needs to be initialized */
	value = enable ?
		BCHP_MEMC_DTU_CONFIG_0_TRANSLATE_ENABLE_ENABLE :
		BCHP_MEMC_DTU_CONFIG_0_TRANSLATE_ENABLE_DISABLE;

	DEV_WR(ireg, value);

	/* Status value that is expected after above setting */
	translate_status = enable ?
		BCHP_MEMC_DTU_CONFIG_0_TRANSLATE_STATUS_ENABLED_ENABLED :
		BCHP_MEMC_DTU_CONFIG_0_TRANSLATE_STATUS_ENABLED_DISABLED;

	do {
		if (dtu_translate_done(sreg, translate_status))
			break;
		udelay(DTU_DELAY);
	} while (timeout++ < DTU_TIMEOUT);

	if (timeout >= DTU_TIMEOUT) {
		puts("dtu_translate - timeout");
		status = false;
	}

	return status;
}

/*
 * DTU Configuration of state map registeres
 * FSBL always maps same bus page number(based on index)
 * to device page number(Identity mapping)
 * map_data will be used only in case of warm boot(which is remap flag)
 */
static void dtu_map_config(unsigned int memc_index,
				struct ddr_info *ddr,
				bool remap)
{
	int num_dev_pages;
	int index, num_dtu_pages;
	unsigned int ireg;
	/* Bus page index that maps S3 parameters */
	int index_0 = -1, index_1 = -1;
	struct brcmstb_bootloader_dtu_table *remap_data = NULL;
	uintptr_t addr;
	uint32_t *map_data = NULL;
	bool valid_dtu_bus_index = false;

	/* Build will fail if S3 params size is greater than one DTU bus page */
	BUILD_BUG_ON(sizeof(struct brcmstb_s3_params) > _MB(DTU_PAGE_SIZE_MB));

	num_dev_pages = BDEV_RD_F(MEMC_DTU_CONFIG_0_SIZE_DEVICE_PAGES,
		SIZE_DEVICE_PAGES);

	if (memc_index == 0)
		ireg = BPHYSADDR(
			BCHP_MEMC_DTU_MAP_STATE_0_MAP_STATEi_ARRAY_BASE);
#ifdef BCHP_MEMC_DTU_CONFIG_1_REG_START
	else
		ireg = BPHYSADDR(
			BCHP_MEMC_DTU_MAP_STATE_1_MAP_STATEi_ARRAY_BASE);
#endif

	num_dtu_pages = ddr->size_mb/DTU_PAGE_SIZE_MB;

	if (remap) {
		/* S3 parameters address was saved  by Linux before going to
		 * deep sleep in AON registers
		 * (BCHP_AON_CTRL_SYSTEM_DATA_RAMi_ARRAY_BASE) at an offset of
		 * AON_REG_CONTROL_LOW and AON_REG_CONTROL_HIGH
		 */
		addr = AON_REG(AON_REG_CONTROL_LOW);
		addr |= shift_left_32((uintptr_t)AON_REG(AON_REG_CONTROL_HIGH));
		remap_data = ((struct brcmstb_s3_params *)addr)->dtu;
		map_data = remap_data[memc_index].dtu_state_map;
		valid_dtu_bus_index = dtu_memory_page_index(ddr, addr,
				sizeof(struct brcmstb_s3_params),
				&index_0, &index_1);
		num_dtu_pages = num_dev_pages;
	}

	if (num_dev_pages >= num_dtu_pages) {

		/* In warmboot, we need to make sure that memory area
		 * that saves dtu info must be identity map.
		*/
		if (valid_dtu_bus_index) {
			DEV_WR(ireg + (index_0 << 2),
				DTU_MAP_STATE_VALUE(index_0));
			/* Only when end address falls on next bus page idx.
			 * It is possible that end address bus page index is
			 * not same as start address bus page index when S3
			 * parameters address is around the upper bounday of
			 * a bus page
			 */
			if (index_1 != index_0)
				DEV_WR(ireg + (index_1 << 2),
					DTU_MAP_STATE_VALUE(index_1));
		}

		for (index = 0; index < num_dtu_pages; index++) {
			if ((index != index_0) && (index != index_1)) {
				/* value of state map register
				 * Bit field 31: Valid = 1
				 * (Read only - after mapped )
				 * Bit Fiedld 30:16 Device page
				 * Bit 2:0 Command (Map command to map)
				 */
				if (remap)
					dtu_process_remap_entry(
						ireg + (index << 2),
						map_data[index]);
				else
					DEV_WR(ireg + (index << 2),
						DTU_MAP_STATE_VALUE(index));
			}
		}
	}
}

/*
 * dtu Config verification of state map registeres after S3 memory verify.
 */
static void dtu_verify_map(unsigned int memc_index)
{
	int index;
	unsigned int ireg;
	int num_dtu_pages;
	struct brcmstb_bootloader_dtu_table *remap_data;
	uintptr_t addr;
	uint32_t *map_data;

	num_dtu_pages = BDEV_RD_F(MEMC_DTU_CONFIG_0_SIZE_DEVICE_PAGES,
		SIZE_DEVICE_PAGES);

	if (memc_index == 0)
		ireg = BPHYSADDR(
			BCHP_MEMC_DTU_MAP_STATE_0_MAP_STATEi_ARRAY_BASE);
#ifdef BCHP_MEMC_DTU_CONFIG_1_REG_START
	else
		ireg = BPHYSADDR(
			BCHP_MEMC_DTU_MAP_STATE_1_MAP_STATEi_ARRAY_BASE);
#endif
	/* S3 parameters address was saved  by Linux before going to deep sleep
	 * in AON registers(BCHP_AON_CTRL_SYSTEM_DATA_RAMi_ARRAY_BASE) at an
	 * offset of AON_REG_CONTROL_LOW and AON_REG_CONTROL_HIGH
	 */
	addr = AON_REG(AON_REG_CONTROL_LOW);
	addr |= shift_left_32((uintptr_t) AON_REG(AON_REG_CONTROL_HIGH));
	remap_data = ((struct brcmstb_s3_params *)addr)->dtu;
	map_data = remap_data[memc_index].dtu_state_map;

	/* To verify that translate table not modified during
	 * S3 memory area validation.
	 */
	for (index = 0; index < num_dtu_pages; index++) {
		/* value of state map register
		 * Bit field 31: Valid = 1
		 * (Read only - after mapped )
		 * Bit Fiedld 30:16 Device page
		 * Bit 2:0 Command (Map command to map)
		 */
		if (!dtu_verify_entry(
			ireg + (index << 2), map_data[index]))
			handle_boot_err(ERR_S3_DTU_REMAP_FAILED);
	}
}

/*
 * dtu_load
 * Loading state map from S3 in warm boot and Identity map on cold boot
 * Set up is complete in here for cold boot
 * (for warm boot translation will enabled later)
 */
void dtu_load(struct board_type *b, bool warm_boot)
{
	unsigned int index;
	bool enable;

	if (!dtu_is_available() || dtu_is_disabled())
		return;

	dtu_config_s3();

	enable = dtu_config_status();

	for (index = 0; index < b->nddr; index++) {
		if (enable)
			dtu_map_config(index, &b->ddr[index], warm_boot);

		dtu_reload(index, false);

		dtu_translate(index, enable);
	}
}

/*
 * dtu_verify
 * verify the translation table from S3 memory area
 * This method is called from PM when a warm boot occured
 */
void dtu_verify(unsigned int num_memc)
{
	unsigned int index;
	bool enabled;

	/* Do not check whether DTU is disabled because DTU reload
	 * has already been disabled. This function is called after
	 * disabling dtu reload in S3 warm boot.
	 */
	if (!dtu_is_available())
		return;

	enabled = dtu_config_status();

	if (enabled) {
		for (index = 0; index < num_memc; index++)
			dtu_verify_map(index);
	}
}

#endif /*BCHP_MEMC_DTU_CONFIG_0_REG_START*/
