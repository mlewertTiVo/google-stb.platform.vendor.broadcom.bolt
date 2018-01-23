/*
 * Copyright 2014-current Broadcom Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _GPT_H
#define _GPT_H

#include "android_types.h"
#include "part_efi.h"

#define EFI_ENTRIES	128
#define SIZEOF_GPT	(2 + EFI_ENTRIES / 4) /* in num of LBA */

/* Linux sector addresses represent 512 byte blocks by convention. */
#define EFI_SECTORSIZE 512

#define EFI_GPT_HEADER_SIZE                     92     /* crc'd hdr bytes of 512 byte sector */
#define EFI_GPT_ENTRY_SIZE                     128     /* size of each partition entry */
#define EFI_GPT_NAMELEN_BYTES                   72     /* buffer size for name in partition record */

/* The number of partition entries per sector */
#define EFI_ENTRIES_PER_SECTOR			(EFI_SECTORSIZE / EFI_GPT_ENTRY_SIZE)

/* 
 * The spec says that
 * "If the size of the GUID Partition Entry Array is not an even multiple of the 
 * logical block size, then any space left over in the last logical block is Reserved 
 * and not covered by the Partition Entry Array CRC32 field."
 *
 * So this means that if the number of partition entries is not a multiple of 4, there
 * will be padding at the end of the disk, and the partition entry array must therefore
 * start at the beginning of a sector.  So we need to back up to a sector boundary
 * when trying to find the start of the array. The macro below helps us do that.
 * 
 * Convert number of partitions to a whole number of sectors
 * We know that the partition is 128 bytes and the sector is 512 
 * so there are 4 partitions in a sector. Round up to a multiple of 4. 
 */
#define EFI_PART2SECT(numEntries)       \
((((numEntries + (EFI_ENTRIES_PER_SECTOR-1)) / EFI_ENTRIES_PER_SECTOR) * EFI_ENTRIES_PER_SECTOR) * sizeof(efi_gpt_entry_t))

/* 
 * These structures are from linux/fs/partitions/efi.h. The similar byte array structures in u-boot/disk/efi_part.h
 * are very stale and hard to work with without the correct types. A future work item would be to change the
 * efi_part.h to use the proper structures below but that can be done a bit later.
 */
typedef struct {
        __le64 signature;
        __le32 revision;
        __le32 header_size;
        __le32 header_crc32;
        __le32 reserved1;
        __le64 my_lba;
        __le64 alternate_lba;
        __le64 first_usable_lba;
        __le64 last_usable_lba;
        efi_guid_t disk_guid;
        __le64 partition_entry_lba;
        __le32 num_partition_entries;
        __le32 sizeof_partition_entry;
        __le32 partition_entry_array_crc32;

        /* The rest of the logical block is reserved by UEFI and must be zero.
         * EFI standard handles this by: */
         uint8_t reserved2[ EFI_SECTORSIZE - EFI_GPT_HEADER_SIZE ];
}
#ifdef __GNUC__
__attribute__ ((packed))
#endif
efi_gpt_header_t;

typedef struct {
        efi_guid_t partition_type_guid;
        efi_guid_t unique_partition_guid;
        __le64 starting_lba;
        __le64 ending_lba;
        __le64 attributes;
        efi_char16_t partition_name[EFI_GPT_NAMELEN_BYTES / sizeof (efi_char16_t)];
}
#ifdef __GNUC__
__attribute__ ((packed))
#endif
efi_gpt_entry_t;

/* 
 * Define an efi partition table that uses the sensible types. This saves a lot
 * of complicated casting to get character arrays converted to u32 and u64 values.
 * The gpt_mmc.c file might be cleaner if it used this format as well to match linux.
 */

typedef union _efi_ptable {
        struct {
                legacy_mbr mbr;
                efi_gpt_header_t header;
                efi_gpt_entry_t entry[EFI_ENTRIES];
        } primary_ptable;
        struct {
                legacy_mbr mbr; /* Not Used */
                efi_gpt_entry_t entry[EFI_ENTRIES];
                efi_gpt_header_t header;
        } secondary_ptable;
} efi_ptable;

#endif /* _GPT_H */
