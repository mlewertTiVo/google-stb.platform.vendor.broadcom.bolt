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

/* ARM *SSBL* start (see:ssbl/arch/arm-start.S) */

#ifndef __ARM_START_H__
#define __ARM_START_H__
#ifndef __ASSEMBLER__

/* arm-start */
extern struct fsbl_info *_fsbl_info;
extern void __setstack(unsigned int sp);
extern unsigned int __getstack(void);

/* ssbl_main */
int puts(const char *s);
void __puts(const char *s);
void writehex(uint32_t val);

#endif /* __ASSEMBLER__ */
#endif /* __ARM_START_H__ */
