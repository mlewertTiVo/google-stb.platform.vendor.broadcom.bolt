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

#ifndef __GISB_H__
#define __GISB_H__

extern void bolt_gisb_task(void *arg);

extern uint32_t bolt_gisb_read(void);
extern void	bolt_gisb_write(uint32_t value);

#endif /* __GISB_H__ */

