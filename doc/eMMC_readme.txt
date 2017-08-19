:toc2:
:toclevels: 4
:max-width: 100%
eMMC support in BOLT
====================
Broadcom Ltd <support@broadcom.com>


Introduction
------------

This discusion is limited to ARM based systems using BOLT as the boot loader.
MIPS based system using the CFE bootloader use a slightly different approach
for the boot functionality.

Both eMMC hardware based partitions, which all start at offset 0, and software
based partitions, which use offsets within the hardware partition, are used to
divide the eMMC storage into sections. Three of the standard eMMC hardware
partitions, the two boot partitions and the user data partition are used.

The user data partition can further contain a software based GPT partition table.
This partition table is understood by both BOLT and the standard Linux kernel and
divides the user data hardware partition into software partitions common to both.

The hardware partitions will show up in BOLT "show devices" as flash0, flash1
and flash2 where flash0 is the data partition, flash1 is the first boot partition
and flash2 is the second boot partition.

The software partitions will show up in BOLT as flash0.GPT_partition_name,
example: flash0.nvram, flash0.macadr, flash0.kernel etc.

NOTE: When BOLT with eMMC support is booted from non-eMMC flash device, "emmc"
will be prepended to the device names (emmcflash0, emmcflash1, emmcflash2,
emmcflash0.macadr) so they don't collide with the non-eMMC flash device names.
Following is the BOLT output from "show devices" on a system booting from eMMC:

----
Device Name          Description
-------------------  ---------------------------------------------------------
              uart0  16550 DUART at 0xf040b400 channel 0
               mem0  Memory
             flash0  EMMC flash Data : 0x000000000-0x370000000 (14080MB)
      flash0.macadr  EMMC flash Data : 0x000004400-0x000004800 (1024B)
       flash0.nvram  EMMC flash Data : 0x000004800-0x000014800 (64KB)
        flash0.wlan  EMMC flash Data : 0x000014800-0x000024800 (64KB)
      flash0.kernel  EMMC flash Data : 0x000024800-0x000824800 (8MB)
      flash0.splash  EMMC flash Data : 0x000824800-0x0008A4800 (512KB)
     flash0.devtree  EMMC flash Data : 0x0008A4800-0x0008B4800 (64KB)
      flash0.rootfs  EMMC flash Data : 0x0008B4800-0x36FFFBE00 (14072MB)
             flash1  EMMC flash Boot1: 0x000000000-0x000400000 (4MB)
             flash2  EMMC flash Boot2: 0x000000000-0x000400000 (4MB)
             flash3  EMMC flash RPMB : 0x000000000-0x000020000 (128KB)
               eth0  GENET Internal Ethernet at 0xf0b60000
----


Startup sequence
----------------

Following is a description of how the partitions are used from the point where
the system is powered up through kernel boot:

1. At power up, hardware automatically starts loading a cache from one of the
two eMMC boot partitions and the CPU starts executing from this cache.
Note: The boot partition selection is an eMMC persistant parameter that
was configured when the partition was programmed.

2. The bootloader (BOLT) copies itself from the cache into main memory and
executes from there.

3. BOLT switches the eMMC device out of boot mode, now all eMMC access is done through
normal eMMC commands.

4. BOLT goes through eMMC device initialization, setting things like bus speed and width.

5. BOLT checks for a GPT partition table on the data partition.

6. If the GPT table is found, BOLT creates BOLT device partitions based on
the GPT partitions<<1>>.

7. If the GPT partition table is not found, BOLT creates the 2 BOLT device partitions
needed by BOLT to boot the kernel across the network, "macadr" and "nvram". These
partitions are at hardcoded offsets on the data partition.
NOTE: System startup will stop here in BOLT at the BOLT command prompt.

8. BOLT will read the "NVRAM" partition and get the kernel boot line and execute it.
This command line should specify booting from the BOLT "kernel" partition and should
pass the "/dev/mmcblkxxx" or GUID based partition name for the kernels rootfs. Example:

---- 
     boot flash0.kernel: "root=PARTUUID=86F01144-4B53-42AE-95D9-C0A47F5CEC59 rootwait rw debug"
---- 

or

---- 
     boot flash0.kernel: "root=/dev/mmcblk0p7 rootwait rw debug"
---- 

[1][[1]] The BOLT partition name is taken from the GPT partition name.


How to configure for eMMC
-------------------------

Following is a description of how to setup a system for eMMC boot, starting with a
blank eMMC device (assumes starting with SPI as the boot device):

--
   1. Modify the BOLT .cfg file by adding "EMMC_FLASH on" and change
      'dt autogen -node sdio -choose 0' to 'dt autogen -node sdio' as
      eMMC is the *second* interface, sdio is the *first*.

   2. Rebuild BOLT with new .cfg file.

   3. Flash BOLT to SPI using BBS or RDB Live.

   4. reboot system.

   5. Flash BOLT to the eMMC device. Example:
+
----
      ifconfig -auto eth0
      flash <TFTP IP ADDR>/bolt-7445d0.bin emmcflash1
----

   6. Set board boot shape to boot from eMMC and reboot.

   7. run 'setsn' or 'macprog' to setup the Ethernet interface.

   8. reboot so the MAC address change takes effect.

   9. Boot an initrd kernel across the network. Example:
+
----
      ifconfig eth0 -auto
      boot <TFTP IP ADDR> vmlinuz-initrd-7445d0 'ip=dhcp debug'
----

  10. Run 'sgdisk' to create the software partitions:
+
[source,shell]
----
      sgdisk -o /dev/mmcblk0
      sgdisk -a 1 -n 1:34:35 -c 1:"macadr" /dev/mmcblk0
      sgdisk -a 1 -n 2:36:163 -c 2:"nvram" /dev/mmcblk0
      sgdisk -a 1 -n 3:164:291 -c 3:"wlan" /dev/mmcblk0
      sgdisk -a 1 -n 4:292:16675 -c 4:"kernel" /dev/mmcblk0
      sgdisk -a 1 -n 5:16676:17699 -c 5:"splash" /dev/mmcblk0
      sgdisk -a 1 -n 6:17700:17827 -c 6:"devtree" /dev/mmcblk0
      sgdisk -n 7:17828:`sgdisk -E /dev/mmcblk0` -c 7:"rootfs" /dev/mmcblk0
----

  11. Copy a kernel and rootfs to eMMC. The following example will copy a
      kernel and rootfs to the correct eMMC flash partitions for a 7445d0 system:
+
[source,shell]
----
      mkfs.ext4 /dev/mmcblk0p7
      mount /dev/mmcblk0p7 /mnt/hd
      cd /mnt/hd
      tftp <TFTP IP ADDR> -g -r vmlinuz-7445d0
      dd if=vmlinuz-7445d0 of=/dev/mmcblk0p4
      rm vmlinuz-7445d0
      tftp <TFTP IP ADDR> -g -r nfsroot-7445d0.tar.bz2
      tar -xjf nfsroot-7445d0.tar.bz2
      mv romfs/* .
      rmdir romfs
      rm nfsroot-7445d0.tar.bz2
----

  12. Reboot the system.

  13. Rerun 'setsn' (or 'macprog') to setup the network.

  14. Boot the kernel. Example:
+
----
      boot flash0.kernel: "root=/dev/mmcblk0p7 rootwait rw debug"
----
--

 Miscellaneous
-------------- 
Unlike SPI or NAND devices, eMMC has an intelligent front
end controller rather than just being a raw storage device.
It has its own interface connections to the STB chip and
does not use 'chip select' lines (CS0, CS1 etc.) like
a SPI or NAND device.

The BOLT 'erase' command is not for use (won't work) with eMMC.
Use Linux to format partitions.

BOLT defaults to booting from the BOOT1 partition. It does
not consider BOOT2.

 Bringup Checklist
~~~~~~~~~~~~~~~~~~
* The SDIO/eMMC pinmuxing is correct for your board.
* The 'sdio' script command has been configured correctly.

* Check if you need to pull-down the STB SDIO 'wprot' and 
pull-up the 'pres' pin, do not allow them to float. This is
also important if you are using microSD as its slot does
not have the write-protection feature.

* You board boot shape (strap) pins are configured for eMMC boot,
if required.
* Some eMMC devices have pins marked N/C (not connected) and for
other devices the same pin is connected to ground or a pulldown.
Don't be tempted to route (connect) eMMC pins to the STB
chip via any other supposedly unused eMMC pins. That way lies
potential bootloader failure and a restriction on second sourcing
other eMMC devices.

Appendix: Copyright Info
------------------------

Copyright (C) 2017, Broadcom Ltd.
All Rights Reserved.
Confidential Property of Broadcom Ltd.

