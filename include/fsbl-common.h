/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __FSBL_COMMON_H__
#define __FSBL_COMMON_H__

#include <common.h>
#include <lib_types.h>


#define NAMELEN 24
#define DDR_TAGLEN 9 /* 8 chars */

#define NAND_CHIP_NONE	-1
#define NAND_CHIP_ONFI	-2


struct ddr_info {
	uint32_t	which;
	uint32_t	size_mb;
	uint32_t	base_mb; /* don't presume contiguous memc's */

	/* match against 'struct memsys_info'
	*/
	uint16_t	ddr_clock;
	uint16_t	ddr_size; /* in Mbits, outside header and template */
	uint16_t	ddr_width;
	uint16_t	phy_width;

	char		*tag; /* identifier for custom MCB, < DDR_TAGLEN */
};


#define MCB_MAGIC1 0x4d33 /* 'M' '3' */
#define MCB_MAGIC2 ((BOLT_VER_MAJOR<<8)|BOLT_VER_MINOR) /* BOLT_VERSION */
#define MCB_MAGIC3 0xA152

struct memsys_info {
	const uint32_t	*values;	/* Header      Template */

	/* match against 'struct ddr_info'
	*/
	const uint16_t	ddr_clock;	/* MCB_MAGIC1  MCB_MAGIC1 */
	const uint16_t	ddr_size;	/* MCB_MAGIC2  SHMOO_ARRAY_SIZE */
	const uint16_t	ddr_width;	/* MCB_MAGIC3  SHMOO_ARRAY_ELEMENTS */
	const uint16_t	phy_width;	/* -1          -1 */

	/* NB: < DDR_TAGLEN */
	const char	*tag;		/* NULL         NULL */
};

struct memsys_params {
	unsigned long memc_reg_base;
	unsigned long phy_reg_base;
	unsigned long shim_reg_base;
	unsigned long edis_reg_base;
};

#define AVS_ENABLE(avs) ((avs) & 0x1)
#define AVS_DOMAINS(avs) (((avs) >> 1) & 0x3)
#define AVS_PMAP_ID(avs) (((avs) >> 3) & 0x1F)

/*
name:	Unambiguous name that maps directly to
	one specific board hardware name.
	revisions then it SHOULD incorporate
	these, if it affects compatability.

ddr:	info on the DDR (ram) for a specific board
	If a board can be equipped with different ddr
	then it requires another board name and a
	warning in 'comment' about this, if possible.

memsys: Select which memsysinitlib.a (e.g. for DDR3
	or DDR4) to use for SHMOO. Only applies to
	standard builds and not '-fixed -' ones where
	the MCB data is in MEMSYS rather than the SHMOO
	section of BOLT. 0 = select first (standard)
	or 1 = select 2nd etc.
*/
struct board_type {
	const char name[NAMELEN];
	uint32_t prid; /* Product ID from CFG file */
	uint8_t bid; /* I2C board id */
	uint8_t avs; /* Packed value. Refer to AVS_ENABLE and AVS_DOMAINS */
	uint8_t memsys;
	uint8_t nddr;
	struct	ddr_info ddr[MAX_DDR];
};

struct boards_nvm_list {
	uint32_t magic;
	uint32_t n_boards;
	const struct board_type *board_types;
	uint32_t n_pmaps;
};

struct nand_chip {
	const char		name[NAMELEN];
	uint32_t		id_mask[2];
	uint32_t		id_val[2];
	uint32_t		page_size;
	uint32_t		block_size;
	uint64_t		size;
	uint32_t		oob_sector;
	uint16_t		min_ecc_bits; /* Correctable bits per sector */
	uint16_t		ecc_codeword; /* Recommended ECC sector size */
	uint32_t		flags;
};

struct boot_shape_mask {
	uint8_t			mask;
	uint8_t			val;
};


#define DDR_OVERRIDE_NO 0xFFFFFFFF

/* Allocated:			0xFFE000FF */
#define FSBL_HARDFLAG_AVS_MASK	0x00000003
#define FSBL_HARDFLAG_AVS_SHIFT 0
#define FSBL_HARDFLAG_AVS_BOARD	0x00000000
#define FSBL_HARDFLAG_AVS_ON	0x00000001
#define FSBL_HARDFLAG_AVS_OFF	0x00000002
#define FSBL_HARDFLAG_AVS_RSVD	0x00000003

#define FSBL_HARDFLAG_PMAP_MASK	0x0000001F
#define FSBL_HARDFLAG_PMAP_SHIFT 3
#define FSBL_HARDFLAG_PMAP_BOARD FSBL_HARDFLAG_PMAP_MASK
#define FSBL_HARDFLAG_PMAP_ID(f) \
	(((f) >> FSBL_HARDFLAG_PMAP_SHIFT) & FSBL_HARDFLAG_PMAP_MASK)

#define FSBL_HARDFLAG_RTS_MASK	0x000003FF
#define FSBL_HARDFLAG_RTS_SHIFT 22
#define FSBL_HARDFLAG_RTS_BOARD FSBL_HARDFLAG_RTS_MASK

#define FSBL_HARDFLAG_RTS_V2		BIT(21)

#define FSBL_HARDFLAG_OTPARK_DISABLE	0x00000001
#define FSBL_HARDFLAG_OTPARK_SHIFT	2
#define FSBL_HARDFLAG_OTPARK_MASK	(FSBL_HARDFLAG_OTPARK_DISABLE \
					 << FSBL_HARDFLAG_OTPARK_SHIFT)

/* rts:0..14, 15:unset */
#define FSBL_HARDFLAG_RTS_V1_MASK	0x0000000F
#define FSBL_HARDFLAG_RTS_V1_SHIFT	28

/* rts:0..1022, 1023:unset */
#define FSBL_HARDFLAG_DEFAULT (\
	(FSBL_HARDFLAG_RTS_BOARD << FSBL_HARDFLAG_RTS_SHIFT) | \
	(FSBL_HARDFLAG_RTS_V2) | \
	(FSBL_HARDFLAG_PMAP_BOARD << FSBL_HARDFLAG_PMAP_SHIFT) | \
	(FSBL_HARDFLAG_AVS_BOARD << FSBL_HARDFLAG_AVS_SHIFT))

/* This interface between FSBL & SSBL is
 used to pass along the current board number
 stored in FSBL. Its used by SSBL to
 re-flash 'board_idx' along with 'product_id'
 if the board selection changed either by FSBL or
 SSBL. If 'product_id' does not match its source
 register, something wild must have happened,
 for example swapping chips on a board.
*/
struct board_nvm_info {
	uint32_t	magic;
	uint32_t	board_idx;
	uint32_t	product_id; /* SUN_TOP_CTRL_PRODUCT_ID */
	uint32_t	romstart;
	uint32_t	romoffset;
	uint32_t	ddr_override;
	uint32_t	hardflags;
	struct memsys_info *shmoo_data;
};

/* Convert a board index to an ASCII char */
static inline char board_idx_to_char(unsigned int board_idx)
{
	if (board_idx < 26)
		return board_idx + 'a';
	else
		return board_idx - 26 + 'A';
}

/* Convert an ASCII char to a board index */
static inline unsigned int board_char_to_idx(char board)
{
	if (board >= 'a')
		return (unsigned int) board - 'a';
	else
		return (unsigned int) board - 'A' + 26;
}

/* Is a valid board selection character? */
static inline unsigned int is_board_char(int board)
{
	return ((board >= 'a') && (board <= 'z')) ||
		((board >= 'A') && (board <= 'Z'));
}

/* Allocated: 0x0000000f
	bit [0]: Indicate if the current board has been runtime
		detected, only applicable for a CFG_BOARD_ID build.
	bit [1..3]: MHL boot/power status
	bit [4]: Booted in A64 mode.
*/
enum {
	FSBL_RUNFLAG_BID_MATCH	= BIT(0),
	FSBL_RUNFLAG_A64_BOOT	= BIT(4),
};


/* PLIM    Power avail  MHL versions
 {0,0,0}   500mA        1.x, 2.x, 3.0
 {0,0,1}   900mA        2.x, 3.0
 {0,1,0}   1.5A         2.x, 3.0
 {0,1,1}   Dongle       2.x, 3.0
 {1,0,0}   2A           3.0 [1]
 Note: enums are +1 WRT PLIM values
*/
enum {
	FSBL_RUNFLAG_MHL_BOOT_Nope = 0,
	FSBL_RUNFLAG_MHL_BOOT_500mA = 1,
	FSBL_RUNFLAG_MHL_BOOT_900mA = 2,
	FSBL_RUNFLAG_MHL_BOOT_1500mA = 3,
	FSBL_RUNFLAG_MHL_BOOT_Ivbus_dongle2src = 4,
	FSBL_RUNFLAG_MHL_BOOT_2000mA = 5, /* MHL v3 [1] */
	FSBL_RUNFLAG_MHL_BOOT_Res1 = 6, /* reserved */
	FSBL_RUNFLAG_MHL_BOOT_Res2 = 7, /* reserved */
};

#define FSBL_RUNFLAG_MHL_MASK  0x7
#define FSBL_RUNFLAG_MHL_SHIFT 1

/*
 * As of BOLT v1.22 'struct fsbl_info' will
 * be versioned. The version will share the
 * n_boards element.
 *
 * [31:16] x = unused
 * [15:08] v = version
 * [07:00] b = number of boards
 *
 * xxxxxxxx.xxxxxxxx.vvvvvvvv.bbbbbbbb
 */
#define	FSBLINFO_N_BOARDS_MASK		(0x00000000ff)
#define	FSBLINFO_N_BOARDS_SHIFT		0
#define	FSBLINFO_N_BOARDS(x)		(((x) & FSBLINFO_N_BOARDS_MASK)\
						>> FSBLINFO_N_BOARDS_SHIFT)

#define	FSBLINFO_VERSION_MASK		(0x000000ff00)
#define	FSBLINFO_VERSION_SHIFT		8
#define	FSBLINFO_VERSION(x)		(((x) & FSBLINFO_VERSION_MASK)\
						>> FSBLINFO_VERSION_SHIFT)

#define FSBLINFO_CURRENT_VERSION	3

/* This is the primary interface between FSBL & SSBL
*/
struct fsbl_info {
	uint32_t uart_base;

	uint32_t board_idx;
	struct board_nvm_info saved_board;

	uint32_t n_boards;
	struct board_type *board_types;

	int nand_chip_idx;
	uint32_t pte;
	int avs_err;
#if CFG_BOARD_ID
	uint8_t bid;
#endif
	uint32_t runflags;
#ifdef STUB64_START
	uint32_t psci_base;
#endif
	uint32_t shmoo_ver; /* VERSION 2 */
	uint16_t srr_size_mb; /* VERSION 3 */
	uint16_t srr_offset_mb; /* VERSION 3 */
};

#endif /* __FSBL_COMMON_H__ */
