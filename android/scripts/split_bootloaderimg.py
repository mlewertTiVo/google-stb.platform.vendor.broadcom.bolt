#! /usr/bin/env python
# this file is used to extract binaries from the bootloader.img file
import os, sys, struct
from subprocess import STDOUT

verbose=1

def split_bootloader_img_usage():
    print 'usage: split_bootloaderimg.py <bootloader-img> [bolt-bin] [android-bsu-bin] [bl31-bin]'
    print '  <bootloader-img> - input image file which combines BOLT and BSU binary files'
    print '  <bolt-bin> - (optional) output BOLT binary file extracted from bootloader-img (default "bolt.bin")'
    print '  <android-bsu-bin> - (optional) output BSU binary file extracted from bootloader-img (default "android_bsu.elf")'
    print '  <bl31-bin> - (optional) output BL31 binary file extracted from bootloader-img (default "bl31.bin")'
    print ''
    print ' e.g. split_bootloaderimg.py bootloader.img'
    sys.exit(0)

input = len(sys.argv)
if input < 1 or input > 4:
    split_bootloader_img_usage()
else:
    bl_img_filename = sys.argv[1]

if input < 2:
    bolt_bin_filename = sys.argv[2]
else:
    bolt_bin_filename = "bolt.bin"

if input < 3:
    bsu_bin_filename = "android_bsu.elf"
else:
    bsu_bin_filename = sys.argv[3]

if input < 4:
    bl31_bin_filename = "bl31.bin"
else:
    bl31_bin_filename = sys.argv[4]

if verbose:
    print 'input bootloader-img: %s' % bl_img_filename
    print 'output bolt-bin: %s' % bolt_bin_filename
    print 'output android-bsu-bin: %s' % bsu_bin_filename
    print 'output bl31-bin: %s' % bl31_bin_filename

# open input bootloader-img file and read in the header
#
#  struct bootloader_img_hdr
#  {
#        __le32 magic;
#        __le32 version;
#        __le32 bolt_img_offset;
#        __le32 bolt_img_size;
#        __le32 bsu_img_offset;
#        __le32 bsu_img_size;
#        __le32 bl31_img_offset;
#        __le32 bl31_img_size;
#  };
in_fd = open(bl_img_filename, 'rb')
bl_img_hdr = in_fd.read(8*4)
(magic, version, bolt_img_offset, bolt_img_size, bsu_img_offset, bsu_img_size, bl31_img_offset, bl31_img_size) = struct.unpack("<8I", bl_img_hdr)

if magic != 0x214c4225:  # ASCII text = %BL!
    print 'Sorry, wrong magic: 0x%x' % magic
    in_fd.close()
    sys.exit(0)

if verbose:
    print 'version: %u' % version
    print 'bolt_img_offset: %u' % bolt_img_offset
    print 'bolt_img_size: %u' % bolt_img_size
    print 'bsu_img_offset: %u' % bsu_img_offset
    print 'bsu_img_size: %u' % bsu_img_size
    print 'bl31_img_offset: %u' % bl31_img_offset
    print 'bl31_img_size: %u' % bl31_img_size

# read bolt binary content
in_fd.seek(bolt_img_offset)
bolt_img_content = in_fd.read(bolt_img_size)
# read bsu binary content
in_fd.seek(bsu_img_offset)
bsu_img_content = in_fd.read(bsu_img_size)
# read bl31 binary content
if version >= 0x2 and bl31_img_size != 0:
    in_fd.seek(bl31_img_offset)
    bl31_img_content = in_fd.read(bl31_img_size)
in_fd.close()

# open output bolt binary file and write out the entire content
with open(bolt_bin_filename, "wb") as f:
    f.write(bolt_img_content)
    f.close

# open output bsu binary file and write out the entire content
with open(bsu_bin_filename, "wb") as f:
    f.write(bsu_img_content)
    f.close

# open output bl31 binary file and write out the entire content
if version >= 0x2 and bl31_img_size != 0:
    with open(bl31_bin_filename, "wb") as f:
        f.write(bl31_img_content)
        f.close
