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

#ifndef __CACHE_OPS_H__
#define __CACHE_OPS_H__

#if CFG_UNCACHED
#define CACHE_INVAL_RANGE(s,l)	do {} while(0)
#define CACHE_FLUSH_RANGE(s,l)	do {} while(0)
#define CACHE_INVAL_ALL()	do {} while(0)
#define CACHE_FLUSH_ALL()	do {} while(0)
#define CACHE_INVAL_INS()	do {} while(0)

#else	/* !CFG_UNCACHED */

#define CACHE_INVAL_RANGE(s,l)  invalidate_d_cache(s,l)
#define CACHE_FLUSH_RANGE(s,l)  clear_d_cache(s,l)

#define CACHE_INVAL_ALL() { clear_all_d_cache(); invalidate_all_i_cache(); }
#define CACHE_FLUSH_ALL() clear_all_d_cache()
#define CACHE_INVAL_INS() invalidate_all_i_cache()

#endif	/* CFG_UNCACHED */


#ifndef __ASSEMBLER__
void clear_all_d_cache(void);
void invalidate_d_cache(void *addr, unsigned int len);
void clear_d_cache(void *addr, unsigned int len);
void invalidate_all_i_cache(void);

void init_tlb(void);

void enable_caches(void);
void disable_caches(void);
#endif /* __ASSEMBLER__ */

#endif	/* __CACHE_OPS_H__ */
