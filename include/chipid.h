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

#ifndef __BCHP_CHIPID_H__
#define __BCHP_CHIPID_H__

/* supplement to bchp_sun_top_ctrl.h */

#define CHIPID_MAJOR_REV_MASK 0xF0
#define CHIPID_MINOR_REV_MASK 0x0F
#define CHIPID_REV_MASK       (CHIPID_MAJOR_REV_MASK | CHIPID_MINOR_REV_MASK)

#endif /* __BCHP_CHIPID_H__ */

