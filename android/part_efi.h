/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ***   To edit the content of this header, modify the corresponding
 ***   source file (e.g. under external/kernel-headers/original/) then
 ***   run bionic/libc/kernel/tools/update_all.py
 ***
 ***   Any manual change here will be lost the next time this script will
 ***   be run. You've been warned!
 ***
 ****************************************************************************
 ****************************************************************************/
#ifndef _DISK_PART_EFI_H
#define _DISK_PART_EFI_H
#define MSDOS_MBR_SIGNATURE 0xAA55
#define EFI_PMBR_OSTYPE_EFI 0xEF
#define EFI_PMBR_OSTYPE_EFI_GPT 0xEE
#define GPT_BLOCK_SIZE 512
#define GPT_HEADER_SIGNATURE 0x5452415020494645ULL
#define GPT_HEADER_REVISION_V1 0x00010000
#define GPT_PRIMARY_PARTITION_TABLE_LBA 1ULL
#define GPT_ENTRY_NAME "gpt"
#define EFI_GUID(a,b,c,d0,d1,d2,d3,d4,d5,d6,d7) ((efi_guid_t) { { (a) & 0xff, ((a) >> 8) & 0xff, ((a) >> 16) & 0xff, ((a) >> 24) & 0xff, (b) & 0xff, ((b) >> 8) & 0xff, (c) & 0xff, ((c) >> 8) & 0xff, (d0), (d1), (d2), (d3), (d4), (d5), (d6), (d7) } })
#define PARTITION_SYSTEM_GUID EFI_GUID(0xC12A7328, 0xF81F, 0x11d2, 0xBA, 0x4B, 0x00, 0xA0, 0xC9, 0x3E, 0xC9, 0x3B)
#define LEGACY_MBR_PARTITION_GUID EFI_GUID(0x024DEE41, 0x33E7, 0x11d3, 0x9D, 0x69, 0x00, 0x08, 0xC7, 0x81, 0xF3, 0x9F)
#define PARTITION_MSFT_RESERVED_GUID EFI_GUID(0xE3C9E316, 0x0B5C, 0x4DB8, 0x81, 0x7D, 0xF9, 0x2D, 0xF0, 0x02, 0x15, 0xAE)
#define PARTITION_BASIC_DATA_GUID EFI_GUID(0xEBD0A0A2, 0xB9E5, 0x4433, 0x87, 0xC0, 0x68, 0xB6, 0xB7, 0x26, 0x99, 0xC7)
#define PARTITION_LINUX_RAID_GUID EFI_GUID(0xa19d880f, 0x05fc, 0x4d3b, 0xa0, 0x06, 0x74, 0x3f, 0x0f, 0x84, 0x91, 0x1e)
#define PARTITION_LINUX_SWAP_GUID EFI_GUID(0x0657fd6d, 0xa4ab, 0x43c4, 0x84, 0xe5, 0x09, 0x33, 0xc8, 0x4b, 0x4f, 0x4f)
#define PARTITION_LINUX_LVM_GUID EFI_GUID(0xe6d6d379, 0xf507, 0x44c2, 0xa2, 0x3c, 0x23, 0x8f, 0x2a, 0x3d, 0xf9, 0x28)
typedef unsigned short efi_char16_t;
typedef struct {
  unsigned char b[16];
} efi_guid_t;
struct partition {
  unsigned char boot_ind;
  unsigned char head;
  unsigned char sector;
  unsigned char cyl;
  unsigned char sys_ind;
  unsigned char end_head;
  unsigned char end_sector;
  unsigned char end_cyl;
  unsigned char start_sect[4];
  unsigned char nr_sects[4];
} __attribute__((packed));
typedef struct _gpt_header {
  unsigned char signature[8];
  unsigned char revision[4];
  unsigned char header_size[4];
  unsigned char header_crc32[4];
  unsigned char reserved1[4];
  unsigned char my_lba[8];
  unsigned char alternate_lba[8];
  unsigned char first_usable_lba[8];
  unsigned char last_usable_lba[8];
  efi_guid_t disk_guid;
  unsigned char partition_entry_lba[8];
  unsigned char num_partition_entries[4];
  unsigned char sizeof_partition_entry[4];
  unsigned char partition_entry_array_crc32[4];
  unsigned char reserved2[GPT_BLOCK_SIZE - 92];
} __attribute__((packed)) gpt_header;
typedef struct _gpt_entry_attributes {
  unsigned long long required_to_function : 1;
  unsigned long long reserved : 47;
  unsigned long long type_guid_specific : 16;
} __attribute__((packed)) gpt_entry_attributes;
typedef struct _gpt_entry {
  efi_guid_t partition_type_guid;
  efi_guid_t unique_partition_guid;
  unsigned char starting_lba[8];
  unsigned char ending_lba[8];
  gpt_entry_attributes attributes;
  efi_char16_t partition_name[72 / sizeof(efi_char16_t)];
} __attribute__((packed)) gpt_entry;
typedef struct _legacy_mbr {
  unsigned char boot_code[440];
  unsigned char unique_mbr_signature[4];
  unsigned char unknown[2];
  struct partition partition_record[4];
  unsigned char signature[2];
} __attribute__((packed)) legacy_mbr;
#endif

