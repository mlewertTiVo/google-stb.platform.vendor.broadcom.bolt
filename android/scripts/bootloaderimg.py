#! /usr/bin/env python
# this file is used to create the bootloader.img file that can be flashed to
# the Broadcom STB platforms via "fastboot flash bootloader" command
import os, sys, struct
from subprocess import STDOUT

verbose=1

def bootloader_img_usage():
    print 'usage: bootloaderimg.py <bolt-bin> <android-bsu-bin> <bootloader-img>'
    print '  <bolt-bin> - input BOLT binary file to create bootloader-img'
    print '  <android-bsu-bin> - input BSU binary file to create bootloader-img'
    print '  <bootloader-img> - ouput image file which combines BOLT and BSU binary files'
    print ''
    print ' e.g. bootloaderimg.py bolt-bb.bin android_bsu.elf bootloader.img'
    sys.exit(0)

input = len(sys.argv)
if input <= 3:
    bootloader_img_usage()

bolt_bin_filename = sys.argv[1]
bsu_bin_filename = sys.argv[2]
bl_img_filename = sys.argv[3]

if verbose:
    print 'input bolt-bin: %s' % bolt_bin_filename
    print 'input android-bsu-bin: %s' % bsu_bin_filename
    print 'output bootloader-img: %s' % bl_img_filename

# construct the bootloader img header with the following format:
#
#  struct bootloader_img_hdr
#  {
#        __le32 magic;
#        __le32 version;
#        __le32 bolt_img_offset;
#        __le32 bolt_img_size;
#        __le32 bsu_img_offset;
#        __le32 bsu_img_size;
#  };

# image offset is aligned to 4Kbytes boundard
page_size = 0x1000  # in bytes

magic = 0x214c4225  # ASCII text = %BL!
version = 0x1
bolt_img_offset = page_size  # offset in number of bytes
bolt_img_size = os.path.getsize(bolt_bin_filename);

bsu_img_offset = int(bolt_img_size / page_size) + (bolt_img_size % page_size > 0) + 1
bsu_img_offset = bsu_img_offset * page_size  # offset in number of bytes
bsu_img_size = os.path.getsize(bsu_bin_filename);

if verbose:
    print 'bolt_img_offset: %u' % bolt_img_offset
    print 'bolt_img_size: %u' % bolt_img_size
    print 'bsu_img_offset: %u' % bsu_img_offset
    print 'bsu_img_size: %u' % bsu_img_size

bl_img_hdr = struct.pack("<6I", magic, version, bolt_img_offset, bolt_img_size, bsu_img_offset, bsu_img_size)

# open input bolt binary file and read in the entire content
in_fd = open(bolt_bin_filename, 'rb')
bolt_img_content = in_fd.read()
in_fd.close()

# open input bsu binary file and read in the entire content
in_fd = open(bsu_bin_filename, 'rb')
bsu_img_content = in_fd.read()
in_fd.close()

# open the output bootloader-img file and write out all contents
with open(bl_img_filename, "wb") as f:
    f.seek(0)
    f.write(bl_img_hdr)
    f.seek(bolt_img_offset)
    f.write(bolt_img_content)
    f.seek(bsu_img_offset)
    f.write(bsu_img_content)
    f.close
