/***************************************************************************
 *     Copyright (c) 2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <boot_defines.h>
#include <common.h>

#define RETVAL 0

int __weak get_bootparam(unsigned long *param)
{
	return RETVAL;
}

int __weak verify_avs(image_info *info)
{
	return RETVAL;
}

int __weak verify_memsys(image_info *info)
{
	return RETVAL;
}

int __weak reverify_region(int magic, int *secure_otp)
{
	return RETVAL;
}

int __weak memsys_region_disable(void)
{
	return RETVAL;
}

int __weak disable_region(int magic)
{
	return RETVAL;
}

int __weak wait_for_disable_region(int magic)
{
	return RETVAL;
}

int __weak verify_ssbl(void)
{
	return RETVAL;
}

int __weak lock_memc(int memc)
{
	return RETVAL;
}

uint32_t __weak memsys_ready(void)
{
	return RETVAL;
}

uint32_t __weak read_otp_bit(uint32_t bit, uint32_t *value)
{
	return RETVAL;
}

uint32_t __weak disable_MICH(void)
{
	return RETVAL;
}

uint32_t __weak bfw_load(image_info *info, uint32_t pagelist)
{
	return RETVAL;
}

uint32_t __weak scramble_sdram(void)
{
	return RETVAL;
}
