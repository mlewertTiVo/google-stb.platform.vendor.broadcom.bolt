/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "error.h"
#include "fileops.h"
#include "iocb.h"
#include "devfuncs.h"

#include "loader.h"
#include "bolt.h"
#include "env_subr.h"

#include "parttbl.h"

/*  *********************************************************************
    *  Constants
    ********************************************************************* */

#define SECTORSIZE	512
#define DIRENTRYSIZE	32
#define DIRPERSECTOR	(SECTORSIZE/DIRENTRYSIZE)

/*#define _FATFS_DEBUG_*/

/*
 * Bios Parameter Block sector offsets, and values.
 */

#define BPB_JMPINSTR		0x00
#define BPB_JMPINSTR_VALUE	0xEB
#define BPB_JMPINSTR_VALUE2	0xE9
#define BPB_SEAL		0x1FE
#define BPB_SEAL_VALUE		0xAA55

#define BPB_BYTESPERSECTOR	0x0B
#define BPB_SECTORSPERCLUSTER	0x0D
#define BPB_RESERVEDSECTORS	0x0E
#define BPB_NUMFATS		0x10
#define BPB_MAXROOTDIR		0x11
#define BPB_TOTALSECTORS	0x13
#define BPB_SECTORSPERFAT	0x16
#define BPB_SECTORSPERTRACK	0x18
#define BPB_NUMHEADS		0x1A
#define BPB_HIDDENSECTORS	0x1C
#define BPB_SYSTEMID		54
#define BPB_MEDIADESCRIPTOR	21
#define BPB_SIGNATURE		38
#define BPB_EXTENDED_SIGNATURE	66
#define BPB_SIGNATURE_VALUE1	0x28
#define BPB_SIGNATURE_VALUE2	0x29

#if CFG_FAT32FS
#define BPB_TOTALSECTOR32	0x20
#define BPB_FATSIZE16		0x16
#define BPB_FATSIZE32		0x24
#define BPB_EXTFLAGS		0x28
#define BPB_FSVER		0x2A
#define BPB_ROOTCLUSTER		0x2c
#define BPB_FSINFO		0x30
#define BPB_BKBOOTSECTOR	0x32
#define BPB_RESERVED		0x34
#endif

/*
 * Partition types
 */

#define PARTTYPE_EMPTY		0
#define PARTTYPE_FAT12		1
#define PARTTYPE_FAT16		4
#define PARTTYPE_FAT16BIG	6
#define PARTTYPE_FAT16W95_A	0x0E
#define PARTTYPE_FAT16W95_B	0x0F
#define PARTTYPE_FAT32		0x0B
#define PARTTYPE_FAT32_LBA	0x0C

/*
 * Partition table offsets
 */
#define PTABLE_STATUS		0
#define PTABLE_STARTHEAD	1
#define PTABLE_STARTSECCYL	2	/* 2 bytes */
#define PTABLE_TYPE			4
#define PTABLE_ENDHEAD		5
#define PTABLE_ENDSECCYL	6	/* 2 bytes */
#define PTABLE_BOOTSECTOR	8	/* 4 bytes */
#define PTABLE_NUMSECTORS	12	/* 4 bytes */

#define PTABLE_SIZE		16
#define PTABLE_COUNT		4
#define PTABLE_OFFSET		(512-2-(PTABLE_COUNT*PTABLE_SIZE))

#define PTABLE_STATUS_ACTIVE	0x80

/*
 * Directory attributes
 */

#define ATTRIB_NORMAL		0x00
#define ATTRIB_READONLY		0x01
#define ATTRIB_HIDDEN		0x02
#define ATTRIB_SYSTEM		0x04
#define ATTRIB_LABEL		0x08
#define ATTRIB_DIR			0x10
#define ATTRIB_ARCHIVE		0x20

#define ATTRIB_LFN			0x0F

/*
 * Macros to read fields in directory & BPB entries
 */

#define READWORD(buffer, x) (((unsigned int) (buffer)[(x)]) | \
	(((unsigned int) (buffer)[(x)+1]) << 8))

#define READWORD32(buffer, x) (READWORD(buffer, (x)) | \
	(READWORD(buffer, (x)+2) << 16))

#define READBYTE(buffer, x) ((unsigned int) (buffer)[(x)])

/*
 * Directory entry offsets and values
 */

#define DIR_CHECKSUM		13
#define DIR_FILELENGTH		28
#define DIR_STARTCLUSTER_LO	26
#define DIR_STARTCLUSTER_HI	20
#define DIR_ATTRIB		11
#define DIR_NAMEOFFSET		0
#define DIR_NAMELEN		8
#define DIR_EXTOFFSET		8
#define DIR_EXTLEN		3

#define DIRENTRY_CHECKSUM(e) READBYTE(e, DIR_CHECKSUM)
#define DIRENTRY_FILELENGTH(e) READWORD32(e, DIR_FILELENGTH)

#if CFG_FAT32FS
#define DIRENTRY_STARTCLUSTER(e) (READWORD(e, DIR_STARTCLUSTER_LO) +\
				(READWORD(e, DIR_STARTCLUSTER_HI) << 16))
#else
#define DIRENTRY_STARTCLUSTER(e) READWORD(e, DIR_STARTCLUSTER_LO)
#endif

#define DIRENTRY_ATTRIB(e) READBYTE(e, DIR_ATTRIB)

#define DIRENTRY_LAST		0
#define DIRENTRY_DELETED	0xE5
#define DIRENTRY_PARENTDIR	0x2E

#define DIRENTRY_LFNIDX(e) READBYTE(e, 0)
#define LFNIDX_MASK	0x1F
#define LFNIDX_END	0x40
#define LFNIDX_MAX	20

/*  *********************************************************************
    *  Types
    ********************************************************************* */

/*
 * Internalized BPB
 */

typedef struct bpb_s {
	unsigned int bpb_bytespersector;
	unsigned int bpb_sectorspercluster;
	unsigned int bpb_reservedsectors;
	unsigned int bpb_numfats;
	unsigned int bpb_maxrootdir;
	unsigned int bpb_totalsectors;
	unsigned int bpb_sectorsperfat;
	unsigned int bpb_sectorspertrack;
	unsigned int bpb_numheads;
	unsigned int bpb_hiddensectors;
	unsigned int bpb_signature;
	char bpb_sysid[8];
} bpb_t;

#if CFG_FAT32FS
typedef struct bs_s {
	unsigned char bs_ignored[3];	/* Bootstrap code */
	unsigned char bs_systemid[8];	/* Name of fs */
	unsigned int bs_totalsect; /* Number of sectors (if sectors == 0) */
	unsigned int bs_fatsize16;
	/* FAT32 only */
	unsigned int bs_fat32length;	/* Sectors/FAT */
	unsigned int bs_flags;	/* Bit 8: fat mirroring, low 4: active fat */
	unsigned int bs_version[2];	/* Filesystem version */
	unsigned int bs_rootcluster;	/* First cluster in root directory */
	unsigned int bs_infosector;	/* Filesystem info sector */
	unsigned int bs_backupboot;	/* Backup boot sector */
	unsigned int bs_reserved2[6];	/* Unused */
} bs_t;
#endif

/*
 * FAT Filesystem descriptor - contains working information
 * about an "open" file system
 */

typedef struct fatfs_s {
	int fat_fh;
	int fat_refcnt;
#if CFG_FAT32FS
	bs_t fat_bs;
	int fat_32bit;
#endif
	bpb_t fat_bpb;
	int fat_twelvebit;
	int fat_partstart;
	uint8_t fat_dirsector[SECTORSIZE];
	int fat_dirsecnum;
	uint8_t fat_fatsector[SECTORSIZE];
	int fat_fatsecnum;
} fatfs_t;

/*
 * FAT Chain - describes a series of FAT entries
 */

typedef struct fatchain_s {
	int fat_start;
#if CFG_FAT32FS
	uint32_t *fat_entries;
#else
	uint16_t *fat_entries;
#endif
	int fat_count;
} fatchain_t;

/*
 * FAT File descriptor - contains working information
 * about an open file (including the filesystem info)
 */

typedef struct fatfile_s {
	fatfs_t *ff_fat;
	int ff_filelength;
	fatchain_t ff_chain;
	int ff_curpos;
	int ff_cursector;
	uint8_t ff_sector[SECTORSIZE];
} fatfile_t;

/*  *********************************************************************
    *  Prototypes
    ********************************************************************* */

static int fatfs_fileop_xinit(void **fsctx, void *filename);
static int fatfs_fileop_pinit(void **fsctx, void *filename);
static int fatfs_fileop_open(void **ref, void *fsctx,
		const char *filename, int mode);
static int fatfs_fileop_read(void *ref, uint8_t *buf, int len);
static int fatfs_fileop_write(void *ref, uint8_t *buf, int len);
static int fatfs_fileop_seek(void *ref, int offset, int how);
static int fatfs_fileop_dir(void *fsctx, char *name);
static void fatfs_fileop_close(void *ref);
static void fatfs_fileop_uninit(void *fsctx);

static int fatfs_check_for_partition_table(fatfs_t *fatfs);

/*  *********************************************************************
    *  FAT fileio dispatch table
    ********************************************************************* */

/*
 * Raw FAT (no partition table) - used only on floppies
 */

const fileio_dispatch_t fatfs_fileops = {
	.method    = "rfat",
	.loadflags = LOADFLG_NOBB,
	.init      = fatfs_fileop_xinit,
	.open      = fatfs_fileop_open,
	.read      = fatfs_fileop_read,
	.write     = fatfs_fileop_write,
	.seek      = fatfs_fileop_seek,
	.dir       = fatfs_fileop_dir,
	.close     = fatfs_fileop_close,
	.uninit    = fatfs_fileop_uninit,
};

/*
 * Partitioned FAT - used on Zip disks, removable hard disks,
 * hard disks, flash cards, etc.
 */

const fileio_dispatch_t pfatfs_fileops = {
	.method    = "fat",
	.loadflags = LOADFLG_NOBB,
	.init      = fatfs_fileop_pinit,
	.open      = fatfs_fileop_open,
	.read      = fatfs_fileop_read,
	.write     = fatfs_fileop_write,
	.seek      = fatfs_fileop_seek,
	.dir       = fatfs_fileop_dir,
	.close     = fatfs_fileop_close,
	.uninit    = fatfs_fileop_uninit,
};

/*  *********************************************************************
    *  fat_readsector(fatfs,sector,numsec,buffer)
    *
    *  Read one or more sectors from the disk into memory
    *
    *  Input parameters:
    *      fatfs - fat filesystem descriptor
    *      sector - sector number
    *      numsec - number of sectors to read
    *      buffer - buffer to read sectors into
    *
    *  Return value:
    *      0 if ok
    *      else error code
    ********************************************************************* */

static int fat_readsector(fatfs_t *fatfs, int sector, int numsec,
			  uint8_t *buffer)
{
	int res;

	res = bolt_readblk(fatfs->fat_fh,
			  ((bolt_offset_t) (sector + fatfs->fat_partstart) *
			   (bolt_offset_t) SECTORSIZE), buffer,
			  numsec * SECTORSIZE);

	if (res != numsec * SECTORSIZE)
		return BOLT_ERR_IOERR;

	return 0;
}

static int fat_gpt(fatfs_t *fatfs, uint8_t *buffer, uint32_t buffer_sz,
	uint32_t *part_start)
{
	unsigned int	  i;
	int               res      = 0;
	int               found_it = 0;
	gpt_hdr_t         *gpt_hdr = NULL;
	guid_part_entry_t *entry;
	uint64_t          prev_lba = ~0;

	/*
	 * Detect a protective MBR, since the presence of one implies that the
	 * medium is GPT-formatted
	 */
	if (!gpt_is_protective_mbr(buffer, SECTORSIZE))
		return -1;

	/*
	 * A protective MBR exists on the medium. Locate the primary
	 * GPT and attempt to discover a FAT partition.
	 */

	/* The primary GPT is usually located immediately after the MBR */
	res = fat_readsector(fatfs, 1, 1, buffer);
	if (res < 0)
		goto cleanup;

	if (gpt_alloc_hdr(buffer, SECTORSIZE, &gpt_hdr) < 0)
		return -1;

	/* GPT header is legit. Hunt for a FAT partition */
	for (i = 0; i < gpt_hdr->part_entry_cnt; i++) {
		uint64_t part_tbl_lba;
		uint32_t entry_ofs;
		uint64_t rel_entry_lba;
		uint32_t rel_entry_ofs;

		res = gpt_get_part_entry_loc(gpt_hdr, i,
				&part_tbl_lba, &entry_ofs);
		if (res < 0)
			goto cleanup;

		/* GPT is sector-size agnostic,
		so we have to calculate the LBA */
		rel_entry_lba = part_tbl_lba + (entry_ofs / SECTORSIZE);
		rel_entry_ofs = entry_ofs % SECTORSIZE;

		/*
		 * Partition entries are sub-block length, so avoid repeated
		 * disk accesses.
		 */
		if (prev_lba != rel_entry_lba) {
			res = fat_readsector(fatfs,
					(uint32_t)rel_entry_lba, 1, buffer);
			if (res < 0)
				goto cleanup;

			prev_lba = rel_entry_lba;
		}

		entry = (guid_part_entry_t *)(buffer + rel_entry_ofs);
		if (gpt_is_part_type(entry, PART_TYPE_WIN)) {
			found_it = 1;
			break;
		}
	}

	if (found_it) {
		if (part_start)
			*part_start = (uint32_t)entry->first_lba;
	}

cleanup:
	if (gpt_hdr)
		gpt_free_hdr(gpt_hdr);
	return res;
}

/*  *********************************************************************
    *  fat_dumpbpb(bpb)
    *
    *  Debug function; display fields in a BPB
    *
    *  Input parameters:
    *      bpb - BIOS parameter block structure
    *
    *  Return value:
    *      nothing
    ********************************************************************* */

#ifdef _FATFS_DEBUG_
static void fat_dumpbpb(bpb_t *bpb)
{
	int i;

	xprintf("Bytes per sector    %d\n", bpb->bpb_bytespersector);
	xprintf("Sectors per cluster %d\n", bpb->bpb_sectorspercluster);
	xprintf("Reserved sectors    %d\n", bpb->bpb_reservedsectors);
	xprintf("Number of FATs      %d\n", bpb->bpb_numfats);
	xprintf("Root dir entries    %d\n", bpb->bpb_maxrootdir);
	xprintf("Total sectors       %d\n", bpb->bpb_totalsectors);
	xprintf("Sectors per FAT     %d\n", bpb->bpb_sectorsperfat);
	xprintf("Sectors per track   %d\n", bpb->bpb_sectorspertrack);
	xprintf("Number of heads     %d\n", bpb->bpb_numheads);
	xprintf("Hidden sectors      %d\n", bpb->bpb_hiddensectors);
	xprintf("Signature           %#02x\n", bpb->bpb_signature);
	xprintf("System ID           ");
	for (i = 0; i < 8; ++i)
		xprintf("%c", bpb->bpb_sysid[i]);
	xprintf("\n");
}
#endif

/*  *********************************************************************
    *  fat32_dumpbs(bs)
    *
    *  Debug function; display fields in a BS
    *
    *  Input parameters:
    *      fatfs - FAT filesystem descriptor
    *
    *  Return value:
    *      nothing
    ********************************************************************* */

#if CFG_FAT32FS && defined(_FATFS_DEBUG_)
static void fat_dumpbs(fatfs_t *fatfs)
{
	xprintf("Total sectors (bs)  %d\n", fatfs->fat_bs.bs_totalsect);
	xprintf("FAT16         (bs)  %d\n", fatfs->fat_bs.bs_fatsize16);
	xprintf("FAT32               %d\n", fatfs->fat_32bit);
	xprintf("FAT32 length  (bs)  %d\n", fatfs->fat_bs.bs_fat32length);
	xprintf("Flags         (bs)  %x\n", fatfs->fat_bs.bs_flags);
	xprintf("Root cluster  (bs)  %d\n", fatfs->fat_bs.bs_rootcluster);
	xprintf("Info sector   (bs)  %d\n", fatfs->fat_bs.bs_infosector);
	xprintf("Backup boot   (bs)  %d\n", fatfs->fat_bs.bs_backupboot);
}
#endif

/*  *********************************************************************
    *  fat_partition_entry_valid(part_id)
    *
    *  Test MBR partition entry's status and partition id for validity
    *  and recognition.
    *
    *  Input parameters:
    *      part_id - ID read from MBR
    *
    *  Return value:
    *      1 if valid, 0 otherwise
    ********************************************************************* */

static int fat_partition_entry_valid(uint8_t status, uint8_t part_id)
{
	return (((status == PTABLE_STATUS_ACTIVE) ||
		 (status == 0x00)) &&
		((part_id == PARTTYPE_FAT12) ||
		 (part_id == PARTTYPE_FAT16) ||
		 (part_id == PARTTYPE_FAT16W95_A) ||
		 (part_id == PARTTYPE_FAT16W95_B) ||
#if CFG_FAT32FS
		 (part_id == PARTTYPE_FAT32) ||
		 (part_id == PARTTYPE_FAT32_LBA) ||
#endif
		 (part_id == PARTTYPE_FAT16BIG)));
}

/*  *********************************************************************
    *  fat_findpart(fatfs)
    *
    *  For partitioned disks, locate the active partition
    *  and set "fat_partstart" accordingly
    *
    *  Input parameters:
    *      fatfs - FAT filesystem descriptor
    *
    *  Return value:
    *      0 if we found a valid partition table; else error code
    ********************************************************************* */

static int fat_findpart(fatfs_t *fatfs)
{
	uint8_t buffer[SECTORSIZE];
	uint8_t *part;
	int res;
	int idx;
	int is_gpt = 0;

	fatfs->fat_partstart = 0; /* make sure we get real boot sector */
	res = fat_readsector(fatfs, 0, 1, buffer);
	if (res < 0)
		return res;

	/*
	 * Normally you're supposed to check for a JMP instruction.
	 * At least that's what many people do.  Flash MBRs don't start
	 * with JMP instructions, so just look for the seal.
	 *
	 *
	 *  if (READBYTE(buffer,BPB_JMPINSTR) != BPB_JMPINSTR_VALUE) {
	 *                 return BOLT_ERR_BADFILESYS;
	 *                 }
	 */

	res = fat_gpt(fatfs, buffer, SECTORSIZE,
			 (uint32_t *)&fatfs->fat_partstart);
	if (res == 0) {
		/*
		 * Found a valid primary FAT partition via GPT. Hop over to the
		 * appropriate boot sector
		 */
		xprintf("fatfs (gpt): Found a WIN partition @ lba %xh\n",
				fatfs->fat_partstart);

		is_gpt = 1;

		/* Reading BPB */
		res = fat_readsector(fatfs, 0, 1, buffer);
		if (res < 0)
			return res;

	} else {
		/* failover to regular MBR code */
		res = 0;
	}

	/*
	 * Check the seal at the end of th sector
	 */

	if (READWORD(buffer, BPB_SEAL) != BPB_SEAL_VALUE)
		return BOLT_ERR_BADFILESYS;

	/*
	 * The following check for GPT-managed disks since the BPB doesn't
	 * have primary/extended partitions. The partition start was already
	 * updated earlier.
	 */
	if (!is_gpt) {
		/*
		 * Look for an active FAT partition.  The partition we want must
		 * be the active one.   We do not deal with extended partitions
		 * here.  Hey, this is supposed to be boot code!
		 */
		part = &buffer[PTABLE_OFFSET];
		for (idx = 0; idx < PTABLE_COUNT; idx++) {
			if (fat_partition_entry_valid
				(part[PTABLE_STATUS], part[PTABLE_TYPE]))
				break;
			part += PTABLE_SIZE;
		}

		if (idx == PTABLE_COUNT) {
			xprintf("*** Could not find a valid partition!\n");
			return BOLT_ERR_BADFILESYS;
		}

		/*
		 * The info we want is really just the pointer to the
		 * boot (BPB) sector.  Get that and we'll use it for an
		 * offset into the disk later.
		 */

		fatfs->fat_partstart = READWORD32(part, PTABLE_BOOTSECTOR);
	}

	return 0;
}

/*  *********************************************************************
    *  fat_readbpb(fatfs)
    *
    *  Read and internalize the BIOS Parameter Block
    *
    *  Input parameters:
    *      fatfs - FAT filesystem descriptor
    *
    *  Return value:
    *      0 if ok
    *      else error code (usually, BPB is not valid)
    ********************************************************************* */

static int fat_readbpb(fatfs_t *fatfs)
{
	uint8_t buffer[SECTORSIZE];
	int res;

	res = fat_readsector(fatfs, 0, 1, buffer);
	if (res < 0)
		return res;

	if (READWORD(buffer, BPB_SEAL) != BPB_SEAL_VALUE) {
		xprintf("*** Invalid FAT partition seal...%#04x!\n",
			READWORD(buffer, BPB_SEAL));
		return BOLT_ERR_BADFILESYS;
	}
	if (READBYTE(buffer, BPB_JMPINSTR) != BPB_JMPINSTR_VALUE)
		return BOLT_ERR_BADFILESYS;

	fatfs->fat_bpb.bpb_bytespersector =
	    READWORD(buffer, BPB_BYTESPERSECTOR);
	fatfs->fat_bpb.bpb_sectorspercluster =
	    READBYTE(buffer, BPB_SECTORSPERCLUSTER);
	fatfs->fat_bpb.bpb_reservedsectors =
	    READWORD(buffer, BPB_RESERVEDSECTORS);
	fatfs->fat_bpb.bpb_numfats = READBYTE(buffer, BPB_NUMFATS);
	fatfs->fat_bpb.bpb_maxrootdir = READWORD(buffer, BPB_MAXROOTDIR);
	fatfs->fat_bpb.bpb_totalsectors = READWORD(buffer, BPB_TOTALSECTORS);
	fatfs->fat_bpb.bpb_sectorsperfat = READWORD(buffer, BPB_SECTORSPERFAT);
	fatfs->fat_bpb.bpb_sectorspertrack =
	    READWORD(buffer, BPB_SECTORSPERTRACK);
	fatfs->fat_bpb.bpb_numheads = READWORD(buffer, BPB_NUMHEADS);
	fatfs->fat_bpb.bpb_hiddensectors = READWORD(buffer, BPB_HIDDENSECTORS);
	fatfs->fat_bpb.bpb_signature = READBYTE(buffer, BPB_SIGNATURE);
	memcpy(fatfs->fat_bpb.bpb_sysid, &buffer[BPB_SYSTEMID], 8);

	fatfs->fat_twelvebit = 1;
	if (memcmp(&buffer[BPB_SYSTEMID], "FAT16   ", 8) == 0)
		fatfs->fat_twelvebit = 0;

	if (fatfs->fat_bpb.bpb_bytespersector != SECTORSIZE)
		return BOLT_ERR_BADFILESYS;
	if (fatfs->fat_bpb.bpb_numfats > 2)
		return BOLT_ERR_BADFILESYS;

	/*
	 * XXX sanity check other fields
	 */

#if CFG_FAT32FS
	fatfs->fat_bs.bs_totalsect = READWORD32(buffer, BPB_TOTALSECTOR32);
	fatfs->fat_bs.bs_fatsize16 = READWORD(buffer, BPB_FATSIZE16);
	fatfs->fat_32bit = 0;

	/* FAT32 entries? */
	if (fatfs->fat_bs.bs_fatsize16 == 0) {
		uint8_t es = READBYTE(buffer, BPB_EXTENDED_SIGNATURE);

		/* Check for FAT32 extended signatures. */
		if ((es != BPB_SIGNATURE_VALUE1) &&
				(es != BPB_SIGNATURE_VALUE2))
			goto not_fat32;

		fatfs->fat_bs.bs_fat32length =
		    READWORD32(buffer, BPB_FATSIZE32);
		fatfs->fat_bs.bs_flags = READWORD(buffer, BPB_EXTFLAGS);
		fatfs->fat_bs.bs_rootcluster =
		    READWORD32(buffer, BPB_ROOTCLUSTER);
		fatfs->fat_bs.bs_infosector = READWORD(buffer, BPB_FSINFO);
		fatfs->fat_bs.bs_backupboot =
		    READWORD(buffer, BPB_BKBOOTSECTOR);
		fatfs->fat_twelvebit = 0;
		fatfs->fat_32bit = 1;
		fatfs->fat_bpb.bpb_sectorsperfat = fatfs->fat_bs.bs_fat32length;
	}
not_fat32:
#endif

#ifdef _FATFS_DEBUG_
	fat_dumpbpb(&(fatfs->fat_bpb));
#if CFG_FAT32FS
	fat_dumpbs(fatfs);
#endif
#endif

	return 0;
}

/*  *********************************************************************
    *  fat_getentry(fatfs,entry)
    *
    *  Read a FAT entry.  This is more involved than you'd think,
    *  since we have to deal with 12 and 16 (and someday 32) bit FATs,
    *  and the nasty case where a 12-bit FAT entry crosses a sector
    *  boundary.
    *
    *  Input parameters:
    *      fatfs - FAT filesystem descriptor
    *      entry - index of FAT entry
    *
    *  Return value:
    *      FAT entry, or <0 if an error occured
    ********************************************************************* */

static int fat_getfatentry(fatfs_t *fatfs, int entry)
{
	int fatsect;
	int byteoffset;
	int fatstart;
	int fatoffset;
	uint8_t b1, b2, b3;
	int res;

	fatstart = fatfs->fat_bpb.bpb_reservedsectors;

	if (fatfs->fat_twelvebit) {
		int odd;
		odd = entry & 1;
		byteoffset = ((entry & ~1) * 3) / 2;
		fatsect = byteoffset / SECTORSIZE;
		fatoffset = byteoffset % SECTORSIZE;

		if (fatfs->fat_fatsecnum != fatsect) {
			res =
			    fat_readsector(fatfs, fatsect + fatstart, 1,
					   fatfs->fat_fatsector);
			if (res < 0)
				return res;

			fatfs->fat_fatsecnum = fatsect;
		}

		b1 = fatfs->fat_fatsector[fatoffset];

		if ((fatoffset + 1) >= SECTORSIZE) {
			res =
			    fat_readsector(fatfs, fatsect + 1 + fatstart, 1,
					   fatfs->fat_fatsector);
			if (res < 0)
				return res;

			fatfs->fat_fatsecnum = fatsect + 1;
			fatoffset -= SECTORSIZE;
		}

		b2 = fatfs->fat_fatsector[fatoffset + 1];

		if ((fatoffset + 2) >= SECTORSIZE) {
			res =
			    fat_readsector(fatfs, fatsect + 1 + fatstart, 1,
					   fatfs->fat_fatsector);
			if (res < 0)
				return res;

			fatfs->fat_fatsecnum = fatsect + 1;
			fatoffset -= SECTORSIZE;
		}

		b3 = fatfs->fat_fatsector[fatoffset + 2];

		if (odd) {
			return ((unsigned int)b3 << 4) +
			    ((unsigned int)(b2 & 0xF0) >> 4);
		} else {
			return ((unsigned int)(b2 & 0x0F) << 8) +
			    ((unsigned int)b1);
		}

	} else {
#if CFG_FAT32FS
		if (fatfs->fat_32bit) {
			uint8_t b4;
			unsigned int ret_entry = 0;

			byteoffset = entry * 4;
			fatsect = byteoffset / SECTORSIZE;
			fatoffset = byteoffset % SECTORSIZE;

			/* check b1..b4 is within the span of a sector */
			if (fatoffset > (SECTORSIZE - 4))
				return BOLT_ERR_BADFILESYS;

			if (fatfs->fat_fatsecnum != fatsect) {
				res =
				    fat_readsector(fatfs, fatsect + fatstart, 1,
						   fatfs->fat_fatsector);
				if (res < 0)
					return res;

				fatfs->fat_fatsecnum = fatsect;
			}

			b1 = fatfs->fat_fatsector[fatoffset];
			b2 = fatfs->fat_fatsector[fatoffset + 1];
			b3 = fatfs->fat_fatsector[fatoffset + 2];
			b4 = fatfs->fat_fatsector[fatoffset + 3];
			ret_entry += b1;
			ret_entry += (((unsigned int)b2) << 8);
			ret_entry += (((unsigned int)b3) << 16);
			/* top nibble should be 0 as per FAT spec */
			ret_entry += (((unsigned int)(b4 & 0x0f)) << 24);
			return ret_entry;
		}
#endif
		byteoffset = entry * 2;
		fatsect = byteoffset / SECTORSIZE;
		fatoffset = byteoffset % SECTORSIZE;

		if (fatfs->fat_fatsecnum != fatsect) {
			res =
			    fat_readsector(fatfs, fatsect + fatstart, 1,
					   fatfs->fat_fatsector);
			if (res < 0)
				return res;

			fatfs->fat_fatsecnum = fatsect;
		}

		/* check b1..b2 is within the span of a sector */
		if (fatoffset > (SECTORSIZE - 2))
			return BOLT_ERR_BADFILESYS;

		b1 = fatfs->fat_fatsector[fatoffset];
		b2 = fatfs->fat_fatsector[fatoffset + 1];
		return ((unsigned int)b1) + (((unsigned int)b2) << 8);
	}
}

/*  *********************************************************************
    *  fat_getrootdirentry(fatfs,entryidx,entry)
    *
    *  Read a root directory entry.  The FAT12/16 root directory
    *  is a contiguous group of sectors, whose size is specified in
    *  the BPB.  This routine just digs out an entry from there
    *
    *  Input parameters:
    *      fatfs - FAT filesystem descriptor
    *      entryidx - 0-based entry index to read
    *      entry - pointer to directory entry (32 bytes)
    *
    *  Return value:
    *      0 if ok
    *      <0 if error occured
    ********************************************************************* */

static int fat_getrootdirentry(fatfs_t *fatfs, unsigned int entryidx,
				uint8_t *entry)
{
	int rootdirstart;
	int dirsecnum;
	int res;

	if (entryidx >= fatfs->fat_bpb.bpb_maxrootdir) {
		memset(entry, 0, DIRENTRYSIZE);
		return BOLT_ERR_INV_PARAM;
	}

	rootdirstart = fatfs->fat_bpb.bpb_reservedsectors +
	    fatfs->fat_bpb.bpb_numfats * fatfs->fat_bpb.bpb_sectorsperfat;

	dirsecnum = rootdirstart + entryidx / DIRPERSECTOR;

	if (fatfs->fat_dirsecnum != dirsecnum) {
		res = fat_readsector(fatfs, dirsecnum, 1, fatfs->fat_dirsector);
		if (res < 0)
			return res;

		fatfs->fat_dirsecnum = dirsecnum;
	}

	memcpy(entry,
	       &(fatfs->fat_dirsector
		 [(entryidx % DIRPERSECTOR) * DIRENTRYSIZE]), DIRENTRYSIZE);

	return 0;
}

#ifndef DISABLE_LONG_NAMES
/*  *********************************************************************
    *  fat_checksumname(name)
    *
    *  Calculate the "long filename" checksum for a given short name.
    *  All LFN directory entries associated with the short name are
    *  given the same checksum byte, to help keep the long name
    *  consistent.
    *
    *  Input parameters:
    *      name - pointer to 32-byte directory entry
    *
    *  Return value:
    *      checksum value
    ********************************************************************* */

static uint8_t fat_checksumname(uint8_t *name)
{
	uint8_t sum = 0;
	uint8_t newbit;
	int idx;

	for (idx = 0; idx < 11; idx++) {
		newbit = (sum & 1) ? 0x80 : 0x00;
		sum >>= 1;
		sum |= newbit;
		sum += name[idx];
	}

	return sum;
}
#endif

#ifdef _FATFS_DEBUG_
void fat_dumpdirentry(uint8_t *entry)
{
	uint8_t name[32];
	int idx;

	if (entry[11] != ATTRIB_LFN) {
		memcpy(name, entry, 11);
		name[11] = 0;
		xprintf("%s   %02X %04X %d\n",
			name, DIRENTRY_ATTRIB(entry),
			DIRENTRY_STARTCLUSTER(entry),
			DIRENTRY_FILELENGTH(entry));
	} else {
		for (idx = 0; idx < 5; idx++)
			name[idx] = entry[(idx * 2) + 1];

		for (idx = 0; idx < 6; idx++)
			name[idx + 5] = entry[(idx * 2) + 14];

		for (idx = 0; idx < 2; idx++)
			name[idx + 11] = entry[(idx * 2) + 28];

		name[13] = '\0';
		xprintf("%02X: %s   %04X  cksum %02X\n", entry[0],
			name, READWORD(entry, 0x1A), entry[13]);
	}
}
#endif

/*  *********************************************************************
    *  fat_walkfatchain(fat,start,arg,func)
    *
    *  Walk a FAT chain, calling a callback routine for each entry
    *  we find along the way.
    *
    *  Input parameters:
    *      fat - FAT filesystem descriptor
    *      start - starting FAT entry (from the directory, usually)
    *      arg - argument to pass to callback routine
    *      func - function to call for each FAT entry
    *
    *  Return value:
    *      0 if all elements processed
    *      <0 if error occured
    *      >0 if callback routine returned a nonzero value
    ********************************************************************* */

static int fat_walkfatchain(fatfs_t *fat, int start,
			    void *arg,
			    int (*func) (fatfs_t *fat,
					 int e, int prev_e, void *arg))
{
	int prev_e = 0;
	int ending_e;
	int e;
	int res = 0;

	e = start;

	/* Note: ending FAT entry can be 0x(F)FF8..0x(F)FFF. We assume that the
	   'getfatentry' call won't return values above that. */
	if (fat->fat_twelvebit) {
		ending_e = 0xFF8;
	} else {
		ending_e = 0xFFF8;
#if CFG_FAT32FS
		if (fat->fat_32bit)
			ending_e = 0x0FFFFFF8;
#endif
	}

	while (e < ending_e) {
		res = (*func) (fat, e, prev_e, arg);
		if (res)
			break;
		prev_e = e;
		e = fat_getfatentry(fat, e);
		if (e < 0)
			return e;
	}

	return res;
}

/*  *********************************************************************
    *  fat_getwalkfunc(fat,e,prev_e,arg)
    *
    *  Callback routien to collect all of the FAT entries into
    *  a FAT chain descriptor
    *
    *  Input parameters:
    *      fat - FAT filesystem descriptor
    *      e - current entry
    *      prev_e - previous entry (0 if first entry)
    *      arg - argument passed to fat_walkfatchain
    *
    *  Return value:
    *      0 to keep walking
    *      else value to return from fat_walkfatchain
    ********************************************************************* */

static int fat_getwalkfunc(fatfs_t *fat, int e, int prev_e, void *arg)
{
	fatchain_t *chain = arg;

	if (chain->fat_entries) {
#if CFG_FAT32FS
		chain->fat_entries[chain->fat_count] = (uint32_t) e;
#else
		chain->fat_entries[chain->fat_count] = (uint16_t) e;
#endif
	}

	chain->fat_count++;

	return 0;
}

/*  *********************************************************************
    *  fat_getchain(fat,start,chain)
    *
    *  Walk an entire FAT chain and remember the chain in a
    *  FAT chain descriptor
    *
    *  Input parameters:
    *      fat - FAT filesystem descriptor
    *      start - starting FAT entry
    *      chain - chain descriptor
    *
    *  Return value:
    *      0 if ok
    *      else error code
    ********************************************************************* */

static int fat_getchain(fatfs_t *fat, int start, fatchain_t *chain)
{
	int res;

	chain->fat_entries = NULL;
	chain->fat_count = 0;
	chain->fat_start = start;

	/*
	 * walk once to count the entries.
	 *
	 * For regular files, you probably don't have to do this
	 * since you can predict exactly how many FAT entries
	 * there are given the file length.
	 */

	res = fat_walkfatchain(fat, start, chain, fat_getwalkfunc);
	if (res < 0)
		return res;

	/*
	 * allocate space for the entries. Include one extra
	 * slot for the first entry, since the first entry
	 * does not actually appear in the FAT (the fat is
	 * only the 'next' pointers).
	 */

	if (chain->fat_count == 0)
		return 0;
#if CFG_FAT32FS
	chain->fat_entries =
	    KMALLOC((chain->fat_count + 1) * sizeof(uint32_t), 0);
#else
	chain->fat_entries =
	    KMALLOC((chain->fat_count + 1) * sizeof(uint16_t), 0);
#endif
	chain->fat_count = 0;

	/*
	 * walk again to collect entries
	 */
	res = fat_walkfatchain(fat, start, chain, fat_getwalkfunc);
	if (res < 0)
		return res;

	return chain->fat_count;
}

/*  *********************************************************************
    *  fat_freechain(chain)
    *
    *  Free memory associated with a FAT chain
    *
    *  Input parameters:
    *      chain - chain descriptor
    *
    *  Return value:
    *      nothing
    ********************************************************************* */

static void fat_freechain(fatchain_t *chain)
{
	if (chain->fat_entries) {
		KFREE(chain->fat_entries);
		chain->fat_entries = NULL;
	}
	chain->fat_count = 0;
}

/*  *********************************************************************
    *  fat_clusteridx(fat,chain,idx)
    *
    *  Index into a FAT chain and return the nth cluster number
    *  from the chain
    *
    *  Input parameters:
    *      fat - fat filesystem descriptor
    *      chain - chain descriptor
    *      idx - index into FAT chain
    *
    *  Return value:
    *      FAT entry at the nth index, or
    *      <0 if an error occured
    ********************************************************************* */
static int fat_clusteridx(fatfs_t *fat, fatchain_t *chain, int idx)
{
	if (idx >= chain->fat_count)
		return BOLT_ERR_INV_PARAM;	/* error! */
	if (!chain->fat_entries)
		return BOLT_ERR_INV_PARAM;	/* error! */
	return (int)(unsigned int)chain->fat_entries[idx];
}

/*  *********************************************************************
    *  fat_sectoridx(fat,chain,idx)
    *
    *  Return the sector nunber of the nth sector in a given
    *  FAT chain.  This routine knows how to translate cluster
    *  numbers into sector numbers.
    *
    *  Input parameters:
    *      fat - FAT filesystem descriptor
    *      chain - FAT chain
    *      idx - index of which sector to find
    *
    *  Return value:
    *      sector number
    *      <0 if an error occured
    ********************************************************************* */
static int fat_sectoridx(fatfs_t *fat, fatchain_t *chain, int idx)
{
	int clusteridx;
	int sectoridx;
	int sector;
	int fatentry;

	clusteridx = idx / fat->fat_bpb.bpb_sectorspercluster;
	sectoridx = idx % fat->fat_bpb.bpb_sectorspercluster;

	fatentry = fat_clusteridx(fat, chain, clusteridx);

	if (fatentry < 0)
		xprintf("ran off end of fat chain!\n");
	if (fatentry < 2)
		xprintf("fat entries should be >= 2\n");

	sector = fat->fat_bpb.bpb_reservedsectors +
	    (fat->fat_bpb.bpb_maxrootdir * DIRENTRYSIZE) / SECTORSIZE +
	    (fat->fat_bpb.bpb_numfats * fat->fat_bpb.bpb_sectorsperfat) +
	    (fatentry - 2) * fat->fat_bpb.bpb_sectorspercluster + sectoridx;

	return sector;
}

/*  *********************************************************************
    *  fat_getsubdirentry(fat,chain,idx,direntry)
    *
    *  This routine is similar to fat_getrootdirentry except it
    *  works on a subdirectory.  FAT subdirectories are like files
    *  containing directory entries, so we use the "get nth sector
    *  in chain" routines to walk the chains of sectors reading directory
    *  entries.
    *
    *  Input parameters:
    *      fat - FAT filesystem descriptor
    *      chain - FAT chain
    *      idx - index of entry to read
    *      direntry - place to put directory entry we read
    *
    *  Return value:
    *      0 if ok
    *      else error code
    ********************************************************************* */

static int fat_getsubdirentry(fatfs_t *fat, fatchain_t *chain,
			      int idx, uint8_t *direntry)
{
	int sector;
	int res;

	sector = fat_sectoridx(fat, chain, idx / DIRPERSECTOR);
	if (sector < 0)
		return sector;

	if (fat->fat_dirsecnum != sector) {
		res = fat_readsector(fat, sector, 1, fat->fat_dirsector);
		if (res < 0)
			return res;

		fat->fat_dirsecnum = sector;
	}

	memcpy(direntry,
	       &(fat->fat_dirsector[(idx % DIRPERSECTOR) * DIRENTRYSIZE]),
	       DIRENTRYSIZE);

	return 0;
}

/*  *********************************************************************
    *  fat_getshortname(direntry,name)
    *
    *  Read the short filename from a directory entry, converting
    *  it into its classic 8.3 form
    *
    *  Input parameters:
    *      direntry - directory entry
    *      name - place to put 8.3 name
    *
    *  Return value:
    *      nothing
    ********************************************************************* */

static void fat_getshortname(uint8_t *direntry, char *name)
{
	int idx;

	/*
	 * Collect the base file name
	 */

	for (idx = DIR_NAMEOFFSET;
		idx < (DIR_NAMEOFFSET + DIR_NAMELEN); idx++) {
		if (direntry[idx] == ' ')
			break;
		*name++ = direntry[idx];
	}

	/*
	 * Put in the dot for the extension only if there
	 * is an extension.
	 */

	if (direntry[DIR_EXTOFFSET] != ' ')
		*name++ = '.';

	/*
	 * Collect the extension
	 */

	for (idx = DIR_EXTOFFSET; idx < (DIR_EXTOFFSET + DIR_EXTLEN); idx++) {
		if (direntry[idx] == ' ')
			break;
		*name++ = direntry[idx];
	}

	*name = '\0';
}

/*  *********************************************************************
    *  fat_getlongname(fat,chain,diridx,shortentry,longname)
    *
    *  Look backwards in the directory to locate the long file name
    *  that corresponds to the short file name passed in 'shortentry'
    *
    *  Input parameters:
    *      fat - FAT filesystem descriptor
    *      chain - chain describing current directory, or NULL
    *              if the current directory is the root directory
    *      diridx - index of the short file name
    *      shortentry - points to the short directory entry
    *      longname - buffer to receive long file name (up to 261 chars)
    *
    *  Return value:
    *      0 if ok (even if LFN is invalid or not found)
    *      else error code
    ********************************************************************* */

static int fat_getlongname(fatfs_t *fat, fatchain_t *chain, int diridx,
			   uint8_t *shortentry, char *longname)
{
#ifdef DISABLE_LONG_NAMES
	*longname = '\0';	/* no long name support */
#else
	unsigned int lfnidx = 1;
	int res;
	uint8_t checksum;
	uint8_t direntry[DIRENTRYSIZE];
	int idx;
	char *lfnptr;
	int badlfn = 0;

	*longname = '\0';

	/*
	 * idx is the entry # of the short name
	 */

	checksum = fat_checksumname(shortentry);

	/*
	 * Start working backwards from current entry
	 * and collect pieces of the lfn
	 */

	lfnptr = longname;
	diridx--;

	while (diridx >= 0) {

		/*
		 * Read previous entry
		 */

		if (chain)
			res = fat_getsubdirentry(fat, chain, diridx, direntry);
		else
			res = fat_getrootdirentry(fat, diridx, direntry);

		if (res < 0)
			return res;

		/*
		 * Checksum must match, it must have the right entry index,
		 * and it must have the LFN attribute
		 */

		if (DIRENTRY_CHECKSUM(direntry) != checksum) {
			badlfn = 1;
			break;
		}
		if ((DIRENTRY_LFNIDX(direntry) & LFNIDX_MASK) != lfnidx) {
			badlfn = 1;
			break;
		}

		if (DIRENTRY_ATTRIB(direntry) != ATTRIB_LFN) {
			badlfn = 1;
			break;
		}

		/*
		 * Collect the chars from the filename.  Note we
		 * don't deal well with real unicode chars here.
		 */

		for (idx = 0; idx < 5; idx++)
			*lfnptr++ = direntry[(idx * 2) + 1];

		for (idx = 0; idx < 6; idx++)
			*lfnptr++ = direntry[(idx * 2) + 14];

		for (idx = 0; idx < 2; idx++)
			*lfnptr++ = direntry[(idx * 2) + 28];


		/*
		 * Don't go too far
		 */

		if (DIRENTRY_LFNIDX(direntry) & LFNIDX_END)
			break;
		lfnidx++;
		if (lfnidx > LFNIDX_MAX) {
			badlfn = 1;
			break;
		}

		diridx--;
	}

	/*
	 * Null terminate the lfn
	 */

	*lfnptr = 0;

	if (badlfn)
		longname[0] = 0;
#endif

	return 0;
}

/*  *********************************************************************
    *  fat_scandir(fat,chain,name,direntry)
    *
    *  Scan a single directory looking for a file name
    *
    *  Input parameters:
    *      fat - FAT filesystem descriptor
    *      chain - FAT chain for directory or NULL for root directory
    *      name - name of file to look for (short or long name)
    *      direntry - place to put directory entry if we find one
    *
    *  Return value:
    *      1 if name was found
    *      0 if name was not found
    *      else <0 is error code
    ********************************************************************* */

static int fat_scandir(fatfs_t *fat, fatchain_t *chain,
		       char *name, uint8_t *direntry)
{
	int idx, res;
	int count;
	char shortname[16];
	char longname[280];

	/*
	 * Get directory size
	 */

	if (chain) {
		count =
		    (chain->fat_count * fat->fat_bpb.bpb_sectorspercluster) *
		    DIRPERSECTOR;
	} else {
		count = (int)fat->fat_bpb.bpb_maxrootdir;
	}

	/*
	 * Scan whole directory
	 */

	for (idx = 0; idx < count; idx++) {

		/*
		 * Get entry by root or chain depending...
		 */

		if (chain)
			res = fat_getsubdirentry(fat, chain, idx, direntry);
		else
			res = fat_getrootdirentry(fat, idx, direntry);

		if (res < 0)
			return res;

		/*
		 * Ignore stuff we don't want to see
		 */

		if (direntry[0] == DIRENTRY_LAST)
			break;	/* stop if at end of dir */
		if (direntry[0] == DIRENTRY_DELETED)
			continue;	/* skip deleted entries */
		if (direntry[0] == DIRENTRY_PARENTDIR)
			continue;	/* skip ./.. entries */

		if (DIRENTRY_ATTRIB(direntry) == ATTRIB_LFN)
			continue;	/* skip LFNs */
		if (DIRENTRY_ATTRIB(direntry) & ATTRIB_LABEL)
			continue;	/* skip volume labels */

		/*
		 * Get actual file names from directory
		 */

		fat_getshortname(direntry, shortname);
		res = fat_getlongname(fat, chain, idx, direntry, longname);
		if (res < 0)
			return res;

		if (name) {
			if (strcmpi(name, shortname) == 0)
				return 1;
			if (longname[0] && (strcmpi(name, longname) == 0))
				return 1;
		} else {
#ifdef DIR_DISPLAY_VER_0
			xprintf("%-50s", longname[0] ? longname : shortname);
			if (DIRENTRY_ATTRIB(direntry) == ATTRIB_DIR)
				xprintf("    <DIR>");
			else
				xprintf("    %d",
					DIRENTRY_FILELENGTH(direntry));
#else
			if (DIRENTRY_ATTRIB(direntry) == ATTRIB_DIR)
				xprintf("      <DIR> ");
			else {
				char size[50], *p;
				xsprintf(size, "%12d",
					 DIRENTRY_FILELENGTH(direntry));
				p = size;
				while (*p && *(p + 1) && (*p == '0'))
					*p++ = ' ';
				xprintf("%s", size);
			}
			xprintf("    %s", longname[0] ? longname : shortname);
#endif
			xprintf("\n");
		}
	}

	return 0;
}

/*  *********************************************************************
    *  fat_findfile(fat,name,direntry)
    *
    *  Locate a directory entry given a complete path name
    *
    *  Input parameters:
    *      fat - FAT filesystem descriptor
    *      name - name of file to locate (forward or reverse slashses ok)
    *      direntry - place to put directory entry we find
    *
    *  Return value:
    *      0 if file not found
    *      1 if file was found
    *      <0 if error occurs
    ********************************************************************* */

static int fat_findfile(fatfs_t *fat, const char *name, uint8_t *direntry)
{
	char *namecopy;
	char *namepart;
	char *ptr;
	fatchain_t chain;
	int res;
	int e;

	/*
	 * Copy the name, we're going to hack it up
	 */

	namecopy = strdup(name);

	/*
	 * Chew off the first piece up to the first slash.  Remove
	 * a leading slash if it is there.
	 */

	ptr = namecopy;

	if ((*ptr == '/') || (*ptr == '\\'))
		ptr++;

	namepart = ptr;
	while (*ptr && (*ptr != '/') && (*ptr != '\\'))
		ptr++;
	if (*ptr)
		*ptr++ = '\0';

	/*
	 * Scan the root directory looking for the first piece
	 */

#if CFG_FAT32FS
	if (fat->fat_32bit) {
		memset(&chain, 0, sizeof(chain));
		fat_getchain(fat, fat->fat_bs.bs_rootcluster, &chain);
		res = fat_scandir(fat, &chain, namepart, direntry);
		fat_freechain(&chain);
	} else
		res = fat_scandir(fat, NULL, namepart, direntry);
#else
	res = fat_scandir(fat, NULL, namepart, direntry);
#endif
	if (res == 0) {
		KFREE(namecopy);
		return 0;	/* file not found */
	}

	/*
	 * Start scanning subdirectories until we run out
	 * of directory components.
	 */

	namepart = ptr;
	while (*ptr && (*ptr != '/') && (*ptr != '\\'))
		ptr++;
	if (*ptr)
		*ptr++ = '\0';
	if (!*namepart)
		namepart = NULL;

	while (namepart) {

		/*
		 * Scan the subdirectory
		 */

		e = DIRENTRY_STARTCLUSTER(direntry);
		memset(&chain, 0, sizeof(chain));
		fat_getchain(fat, e, &chain);
		res = fat_scandir(fat, &chain, namepart, direntry);

		if (res == 0)
			break;

		fat_freechain(&chain);

		/*
		 * Advance to the next piece
		 */

		namepart = ptr;
		while (*ptr && (*ptr != '/') && (*ptr != '\\'))
			ptr++;
		if (*ptr)
			*ptr++ = '\0';
		if (!*namepart)
			namepart = NULL;

		/*
		 * If there's more to go and we hit something that
		 * is not a directory, stop here.
		 */

		if (namepart && !(DIRENTRY_ATTRIB(direntry) & ATTRIB_DIR))
			res = 0;
	}

	KFREE(namecopy);

	/*
	 * The last piece we enumerate has to be a file.
	 */

	if ((res > 0) && (DIRENTRY_ATTRIB(direntry) & ATTRIB_DIR))
		return 0;

	return res;
}

/*  *********************************************************************
    *  fat_init(fat,name)
    *
    *  Create the filesystem descriptor and attach to the hardware
    *  device.
    *
    *  Input parameters:
    *      fat - filesystem descriptor
    *      name - hardware device name
    *	   part - true to look for partition tables
    *
    *  Return value:
    *      0 if ok
    *      else error code
    ********************************************************************* */

static int fat_init(fatfs_t *fat, char *name, int part)
{
	int res;

	memset(fat, 0, sizeof(fatfs_t));
	fat->fat_dirsecnum = -1;
	fat->fat_fatsecnum = -1;

	fat->fat_fh = bolt_open(name);

	if (fat->fat_fh < 0)
		return fat->fat_fh;

	res = fatfs_check_for_partition_table(fat);
	/* If we were able to figure it out, use that as the default */
	if (res >= 0)
		part = res;
	else {
		bolt_close(fat->fat_fh);
		fat->fat_fh = -1;
		return res;
	}

	if (part) {
		res = fat_findpart(fat);
		if (res < 0) {
			bolt_close(fat->fat_fh);
			fat->fat_fh = -1;
			return res;
		}
	}

	res = fat_readbpb(fat);
	if (res != 0) {
		bolt_close(fat->fat_fh);
		fat->fat_fh = -1;
		return res;
	}

	return 0;
}

/*  *********************************************************************
    *  fat_uninit(fat)
    *
    *  Uninit the filesystem descriptor and release any resources
    *  we allocated.
    *
    *  Input parameters:
    *      fat - filesystem descriptor
    *
    *  Return value:
    *      nothing
    ********************************************************************* */

static void fat_uninit(fatfs_t *fat)
{
	if (fat->fat_fh >= 0)
		bolt_close(fat->fat_fh);
	fat->fat_fh = -1;
}

/*  *********************************************************************
    *  fatfs_fileop_dir(fsctx,name)
    *
    *  Scan and print a directory
    *
    *  Input parameters:
    *      fsctx - file system context (return pointer)
    *      name - name of directory
    *
    *  Return value:
    *      1 if dir or file found, 0 otherwise, < 0 error
    ********************************************************************* */

static int fatfs_fileop_dir(void *fsctx, char *name)
{
	fatfs_t *fatfs = fsctx;
	uint8_t direntry[32];
	int res = 1, e;
	fatchain_t chain;

	if (name) {		/* sub-directories case */
		res = fat_findfile(fatfs, name, direntry);
		if (res < 0)
			return BOLT_ERR_FILENOTFOUND;	/* not found */

		if (DIRENTRY_ATTRIB(direntry) & ATTRIB_DIR) {
			e = DIRENTRY_STARTCLUSTER(direntry);
			memset(&chain, 0, sizeof(chain));

			if (fat_getchain(fatfs, e, &chain) > 0)
				(void)fat_scandir(fatfs, &chain,
					NULL, direntry);

			/* This checks chain->fat_entries first
			so we are safe if fat_getchain failed.
			 It has to always be done as the 2nd
			fat_walkfatchain could fail after we
			have already malloc'd fat_entries. */
			fat_freechain(&chain);
			res = 1;
		} else
			xprintf("*** %s: File %sfound\n", name,
				((res == 1) ? "" : "not "));
	}
#if CFG_FAT32FS
	else if (fatfs->fat_32bit) {	/* FAT32 root-directory case */
		memset(&chain, 0, sizeof(chain));
		fat_getchain(fatfs, fatfs->fat_bs.bs_rootcluster, &chain);
		res = fat_scandir(fatfs, &chain, NULL, direntry);
		fat_freechain(&chain);
		if (res >= 0)
			res = 1;
	}
#endif
	else {			/* FAT16 root-directory case */
		res = fat_scandir(fatfs, NULL, NULL, direntry);
		if (res >= 0)
			res = 1;
	}

	return res;
}

/*  *********************************************************************
    *  fatfs_fileop_init(fsctx,devname,part)
    *
    *  Create a FAT filesystem context and open the associated
    *  block device.
    *
    *  Input parameters:
    *      fsctx - file system context (return pointer)
    *      devname - device name to open
    *	   part - true to look for a partition table
    *
    *  Return value:
    *      0 if ok, else error
    ********************************************************************* */

static int fatfs_fileop_init(void **fsctx, char *devname, int part)
{
	int res;
	fatfs_t *fatfs;

	/*
	 * Allocate a file system context
	 */

	fatfs = (fatfs_t *) KMALLOC(sizeof(fatfs_t), 0);
	if (!fatfs)
		return BOLT_ERR_NOMEM;

	/*
	 * Open a handle to the underlying device
	 */

	res = fat_init(fatfs, devname, part);
	if (res != 0) {
		KFREE(fatfs);
		return res;
	}

	*fsctx = fatfs;

	return 0;
}

/*  *********************************************************************
    *  fatfs_check_for_partition_table(fatfs)
    *
    *  This routine attempts to determine if the disk contains a
    *  partition table or if it contains a standard MS-DOS boot recod.
    *  We try to find both, and return what we find but with priority
    *  going to a partition table, or an error if it is still unclear.
    *
    *  Input parameters:
    *      fatfs - fat filesystem context
    *
    *  Return value:
    *      0 if no partition table
    *      1 if partition table
    *      <0 = error occured, could not tell or I/O error
    ********************************************************************* */

static int fatfs_check_for_partition_table(fatfs_t *fatfs)
{
	int res;
	uint8_t buffer[SECTORSIZE];
	uint8_t *part;
	int idx;
	int foundit = 0;
	uint32_t part_sts = 0, part_ids = 0;

	/*
	 * Read sector 0
	 */

	fatfs->fat_partstart = 0;
	res = fat_readsector(fatfs, 0, 1, buffer);
	if (res < 0)
		return res;

	/*
	 * Check the seal at the end of th sector.  Both
	 * boot sector and MBR should contain this seal.
	 */
	if (READWORD(buffer, BPB_SEAL) != BPB_SEAL_VALUE) {
		xprintf("*** Invalid MBR seal...%#04x!\n",
			READWORD(buffer, BPB_SEAL));
		res = BOLT_ERR_BADFILESYS;
		return res;
	}

	/*
	 * See Microsoft Knowledgebase article # Q140418, it contains
	 * a good description of the boot sector format.
	 *
	 * If the extended information is present, and SystemID is "FAT"
	 * and the "bytes per sector" is 512, assume it's a regular boot block
	 */

	if (((buffer[BPB_SIGNATURE] == BPB_SIGNATURE_VALUE1) ||
	     (buffer[BPB_SIGNATURE] == BPB_SIGNATURE_VALUE2)) &&
	    (memcmp(&buffer[BPB_SYSTEMID], "FAT", 3) == 0) &&
	    (READWORD(buffer, BPB_BYTESPERSECTOR) == 512)) {
		/* Not partitioned */
		foundit = 2;
	}

	/* If no extended information is present,
	check a few other key values. */

	if ((READWORD(buffer, BPB_BYTESPERSECTOR) == 512) &&
	    (READWORD(buffer, BPB_RESERVEDSECTORS) >= 1) &&
	    ((READWORD(buffer, BPB_MEDIADESCRIPTOR) & 0xF0) == 0xF0)) {
		foundit = 3;
	}

	res = fat_gpt(fatfs, buffer, SECTORSIZE, NULL);
	if (res == 0) {
		xprintf("fatfs (gpt): Media is partitioned with GPT!\n");
		return 1;
	} else {
		/* fallthrough */
		res = 0;
	}

	/*
	 * If we're still confused, look for a partition table with a valid FAT
	 * partition on it.  We might not detect a partition table that has
	 * only non-FAT partitions on it, like a disk with all Linux partitions,
	 * but that is fine here in the FATFS module, since we only want to
	 * find FAT partitions anyway.
	 */
	part = &buffer[PTABLE_OFFSET];
	for (idx = 0; idx < PTABLE_COUNT; idx++) {
		if (fat_partition_entry_valid
		    (part[PTABLE_STATUS], part[PTABLE_TYPE])) {
			foundit = 1;
			res = 1;	/* Partition table present */
			break;
		}
		part_sts = (part_sts << 8) + part[PTABLE_STATUS];
		part_ids = (part_ids << 8) + part[PTABLE_TYPE];
		part += PTABLE_SIZE;
	}

	/*
	 * If at this point we did not find what we were looking for,
	 * return an error.
	 */
	if (foundit == 1) {
		res = 1;	/*Partition table is present. */
	} else if (foundit) {
		res = 0;
		if (env_getenv("FAT_DBG"))
			xprintf("*** Found no partition..."
			"assume regular boot block (%d)\n", foundit);
	} else {
		/*Error!  We can't decide if partition table exists or not */
		xprintf("*** Could not find a valid partition..."
			"status=%08x, ids=%08x!\n", part_sts, part_ids);
		res = BOLT_ERR_BADFILESYS;
	}

	return res;
}

static int fatfs_fileop_xinit(void **fsctx, void *dev)
{
	char *devname = (char *)dev;

	return fatfs_fileop_init(fsctx, devname, 0);
}

static int fatfs_fileop_pinit(void **fsctx, void *dev)
{
	char *devname = (char *)dev;

	return fatfs_fileop_init(fsctx, devname, 1);
}

/*  *********************************************************************
    *  fatfs_fileop_open(ref,name)
    *
    *  Open a file on the FAT device.
    *
    *  Input parameters:
    *      ref - place to store pointer to fileinfo
    *      fsctx - filesystem context
    *      name - name of file to open
    *
    *  Return value:
    *      0 if ok
    *      else error code
    ********************************************************************* */

static int fatfs_fileop_open(void **ref, void *fsctx,
			const char *name, int mode)
{
	int res;
	uint8_t direntry[DIRENTRYSIZE];
	fatfile_t *ff;
	fatfs_t *fatfs;

	if (mode != FILE_MODE_READ)
		return BOLT_ERR_UNSUPPORTED;

	if (!name)
		return BOLT_ERR_INV_PARAM;

	fatfs = (fatfs_t *) fsctx;

	ff = (fatfile_t *) KMALLOC(sizeof(fatfile_t), 0);
	if (ff == NULL)
		return BOLT_ERR_NOMEM;

	memset(ff, 0, sizeof(fatfile_t));

	ff->ff_fat = fatfs;

	res = fat_findfile(ff->ff_fat, name, direntry);
	if (res <= 0) {
		res = BOLT_ERR_FILENOTFOUND;	/* not found */
		goto err;
	}

	/*
	 * Okay, the file was found.  Enumerate the FAT chain
	 * associated with this file.
	 */

	ff->ff_filelength = DIRENTRY_FILELENGTH(direntry);

	ff->ff_curpos = 0;
	ff->ff_cursector = -1;

	res = fat_getchain(ff->ff_fat,
			   DIRENTRY_STARTCLUSTER(direntry), &(ff->ff_chain));
	if (res < 0)
		goto err;

	/*
	 * Return the file handle
	 */

	fatfs->fat_refcnt++;
	*ref = (void *)ff;
	return 0;

err:
	KFREE(ff);
	return res;
}

/*  *********************************************************************
    *  fatfs_fileop_close(ref)
    *
    *  Close the file.
    *
    *  Input parameters:
    *      ref - pointer to open file information
    *
    *  Return value:
    *      nothing
    ********************************************************************* */

static void fatfs_fileop_close(void *ref)
{
	fatfile_t *file = (fatfile_t *) ref;
	fatfs_t *fatctx = file->ff_fat;

	fatctx->fat_refcnt--;

	fat_freechain(&(file->ff_chain));
	KFREE(file);
}

/*  *********************************************************************
    *  fatfs_fileop_uninit(ref)
    *
    *  Uninitialize the file system.
    *
    *  Input parameters:
    *      fsctx - filesystem context
    *
    *  Return value:
    *      nothing
    ********************************************************************* */
static void fatfs_fileop_uninit(void *fsctx)
{
	fatfs_t *fatctx = (fatfs_t *) fsctx;

	if (fatctx->fat_refcnt)
		xprintf("fatfs_fileop_unint: warning: refcnt should be 0\n");

	fat_uninit(fatctx);

	KFREE(fatctx);
}

/*  *********************************************************************
    *  fatfs_fileop_seek(ref,offset,how)
    *
    *  Move the file pointer within the file
    *
    *  Input parameters:
    *      ref - pointer to open file information
    *      offset - new file location or distance to move
    *      how - method for moving
    *
    *  Return value:
    *      new file offset
    *      <0 if error occured
    ********************************************************************* */

static int fatfs_fileop_seek(void *ref, int offset, int how)
{
	fatfile_t *file = (fatfile_t *) ref;

	switch (how) {
	case FILE_SEEK_BEGINNING:
		file->ff_curpos = offset;
		break;
	case FILE_SEEK_CURRENT:
		file->ff_curpos += offset;
		break;
	default:
		break;
	}

	if (file->ff_curpos >= file->ff_filelength)
		file->ff_curpos = file->ff_filelength;

	return file->ff_curpos;
}

/*  *********************************************************************
    *  fatfs_fileop_read(ref,buf,len)
    *
    *  Read data from the file.
    *
    *  Input parameters:
    *      ref - pointer to open file information
    *      buf - buffer to read data into
    *      len - number of bytes to read
    *
    *  Return value:
    *      number of bytes read
    *      <0 if error occured
    *      0 means eof
    ********************************************************************* */

#define MAX_SECTORS_PER_LOAD	8	/* i.e. 4k */
static int fatfs_fileop_read(void *ref, uint8_t *buf, int len)
{
	fatfile_t *file = (fatfile_t *) ref;
	int amtcopy;
	int ttlcopy = 0;
	int offset;
	int sector;
	unsigned int secidx;
	int origpos;
	int res;
	/* coverity[stack_use_local_overflow] */
	uint8_t temp_buf[SECTORSIZE * MAX_SECTORS_PER_LOAD];
	int sectors_per_load, bytes_to_load;
	unsigned int sectors_per_clus, clusidx;

	/*
	 * Remember orig position in case we have an error
	 */

	origpos = file->ff_curpos;

	/*
	 * bounds check the length based on the file length
	 */

	if ((file->ff_curpos + len) > file->ff_filelength)
		len = file->ff_filelength - file->ff_curpos;

	/*
	 *  If there is an offset during a normal (MAX_SECTORS_PER_LOAD) read
	 * request, then adjust the len down to try and avoid a recurring
	 * offset.
	 *  This adjustment will be to read to end of this sector. So, from the
	 * next read on, the file pointer will be at sector boundary and the
	 * block reads will be more efficient.
	 * NOTE: This may cause problems if the caller terminates when the
	 *       returned length is less than its request.
	 */

	if ((file->ff_curpos % SECTORSIZE)
	    && (len == (SECTORSIZE * MAX_SECTORS_PER_LOAD)))
		len = SECTORSIZE - (file->ff_curpos % SECTORSIZE);

	res = 0;

	/*
	 * while ther is still data to be transferred
	 */

	while (len) {

		/*
		 * Calculate the sector offset and index in the sector
		 */

		offset = file->ff_curpos % SECTORSIZE;
		secidx = file->ff_curpos / SECTORSIZE;

		/* Calculate max transfer size and adjust the load size
		 to avoid running off the end of the cluster. */

		sectors_per_clus = file->ff_fat->fat_bpb.bpb_sectorspercluster;
		if (sectors_per_clus > MAX_SECTORS_PER_LOAD)
			sectors_per_load = MAX_SECTORS_PER_LOAD;
		else
			sectors_per_load = sectors_per_clus;

		/* id of sector within a cluster */
		clusidx = secidx % sectors_per_clus;

		/* adjust load size */
		if ((sectors_per_load + clusidx) > sectors_per_clus)
			sectors_per_load = sectors_per_clus - clusidx;

		bytes_to_load = sectors_per_load * SECTORSIZE;

		sector = fat_sectoridx(file->ff_fat, &(file->ff_chain), secidx);

		if (sector < 0) {
			xprintf("should not happen, sector = -1!\n");
			return sector;
		}

		/*
		 * first transfer up to the sector boundary
		 */

		if (offset && (len > (SECTORSIZE - offset)))
			amtcopy = (SECTORSIZE - offset); /* partial sector */
		else if (len >= bytes_to_load)
			amtcopy = bytes_to_load; /* full (burst) load */
		else {
			amtcopy = len; /* last lot */
			sectors_per_load = len / SECTORSIZE;
			if (len % SECTORSIZE)
				++sectors_per_load;
		}

		/*
		 * If transferring modulo sectors, on a sector
		 * boundary, read the data directly into the user buffer
		 *
		 * Otherwise: read into the sector or temp buffer and
		 * transfer the data to user memory.
		 */

		if (offset == 0) {
			if (amtcopy == bytes_to_load) {
				res =
				    fat_readsector(file->ff_fat, sector,
						   sectors_per_load, buf);
				if (res < 0) {
					xprintf("I/O error!\n");
					break;
				}
			} else {
				res =
				    fat_readsector(file->ff_fat, sector,
						   sectors_per_load, temp_buf);
				if (res < 0) {
					xprintf("I/O error!\n");
					break;
				}
				memcpy(buf, temp_buf, amtcopy);
			}
		} else {
			if (file->ff_cursector != sector) {
				res =
				    fat_readsector(file->ff_fat, sector, 1,
						   file->ff_sector);
				if (res < 0)
					break;

				file->ff_cursector = sector;
			}
			memcpy(buf, &(file->ff_sector[offset]), amtcopy);
		}

		/*
		 * Adjust/update all our pointers.
		 */

		buf += amtcopy;
		file->ff_curpos += amtcopy;
		ttlcopy += amtcopy;
		len -= amtcopy;

		/*
		 * see if we ran off the end of the file.  Should not
		 * be necessary.
		 */

		if (file->ff_curpos >= file->ff_filelength) {
			/* should not be necessary */
			break;
		}
	}

	/*
	 * If an error occured, get out now.
	 */

	if (res < 0) {
		file->ff_curpos = origpos;
		return res;
	}

	return ttlcopy;

}

static int fatfs_fileop_write(void *ref, uint8_t *buf, int len)
{
	return BOLT_ERR_UNSUPPORTED;
}
