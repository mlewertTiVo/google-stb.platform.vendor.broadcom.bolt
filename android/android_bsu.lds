/*****************************************************************************
*
* Copyright 2014 Broadcom Corporation.  All rights reserved.
*
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2, available at
* http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
*
* Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a
* license other than the GPL, without Broadcom's express prior written
* consent.
*
*****************************************************************************/

ENTRY(android_bsu_init)

MEMORY {
	android_bsu	: ORIGIN = 0x01800000,	LENGTH = 0x00100000
}

SECTIONS {
        .android_bsu.text :
        {
                * (.init);
                * (.text*);
                . = ALIGN(8);
        } >android_bsu

        .android_bsu.data BLOCK(4096) :
        {
                * (.rodata*);
                . = ALIGN(8);
                * (.data*);
                . = ALIGN(8);
        }

        .android_bsu.bss BLOCK(4096) :
        {
                * (.bss*);
                . = ALIGN(8);
                * (.bss.*);
                . = ALIGN(8);
                * (COMMON);
                . = ALIGN(8);
        }
};

