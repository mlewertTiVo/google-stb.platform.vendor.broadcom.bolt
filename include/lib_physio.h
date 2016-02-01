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

#ifndef __LIB_PHYSIO_H__
#define __LIB_PHYSIO_H__

#define PHYSIO_CAST_U8	(volatile uint8_t *)
#define PHYSIO_CAST_U16 (volatile uint16_t *)
#define PHYSIO_CAST_U32 (volatile uint32_t *)
#define PHYSIO_CAST_U64 (volatile uint64_t *)

#define PHYSIO_MEM_U8(m)	(*(PHYSIO_CAST_U8(m)))
#define PHYSIO_MEM_U16(m)	(*(PHYSIO_CAST_U16(m)))
#define PHYSIO_MEM_U32(m)	(*(PHYSIO_CAST_U32(m)))
#define PHYSIO_MEM_U64(m)	(*(PHYSIO_CAST_U64(m)))

#define PHYSIO_REG_U8(r)	PHYSIO_MEM_U8(BPHYSADDR(r))
#define PHYSIO_REG_U16(r)	PHYSIO_MEM_U16(BPHYSADDR(r))
#define PHYSIO_REG_U32(r)	PHYSIO_MEM_U32(BPHYSADDR(r))
#define PHYSIO_REG_U64(r)	PHYSIO_MEM_U64(BPHYSADDR(r))

#ifndef __ASSEMBLER__
 #define BPHYSADDR(x) ((uint32_t)(x) | BCHP_PHYSICAL_OFFSET)
 #define REG_CAST	PHYSIO_CAST_U32
 #define REG(reg)	PHYSIO_REG_U32(reg)
 typedef uint32_t	physaddr_t;
#else
 #define BPHYSADDR(x) ((x) | BCHP_PHYSICAL_OFFSET)
 #define REG(reg)	BPHYSADDR(reg)
#endif


#define DEV_RD(x) PHYSIO_MEM_U32(x)
#define DEV_WR(x, y) do { PHYSIO_MEM_U32(x) = (y); } while (0)


#define DEV_UNSET(x, y) do { DEV_WR((x), DEV_RD(x) & ~(y)); } while (0)
#define DEV_SET(x, y) do { DEV_WR((x), DEV_RD(x) | (y)); } while (0)

#define DEV_WR_RB(x, y) do { DEV_WR((x), (y)); DEV_RD(x); } while (0)
#define DEV_SET_RB(x, y) do { DEV_SET((x), (y)); DEV_RD(x); } while (0)
#define DEV_UNSET_RB(x, y) do { DEV_UNSET((x), (y)); DEV_RD(x); } while (0)

#define BDEV_RD(x) (DEV_RD(BVIRTADDR(x)))
#define BDEV_WR(x, y) do { DEV_WR(BVIRTADDR(x), (y)); } while (0)
#define BDEV_UNSET(x, y) do { BDEV_WR((x), BDEV_RD(x) & ~(y)); } while (0)
#define BDEV_SET(x, y) do { BDEV_WR((x), BDEV_RD(x) | (y)); } while (0)

#define BDEV_SET_RB(x, y) do { BDEV_SET((x), (y)); BDEV_RD(x); } while (0)
#define BDEV_UNSET_RB(x, y) do { BDEV_UNSET((x), (y)); BDEV_RD(x); } while (0)
#define BDEV_WR_RB(x, y) do { BDEV_WR((x), (y)); BDEV_RD(x); } while (0)


#define BDEV_RD_F(reg, field) \
	((BDEV_RD(BCHP_##reg) & BCHP_##reg##_##field##_MASK) >> \
	 BCHP_##reg##_##field##_SHIFT)

#define BDEV_WR_F(reg, field, val) do { \
	BDEV_WR(BCHP_##reg, \
	(BDEV_RD(BCHP_##reg) & ~BCHP_##reg##_##field##_MASK) | \
	(((val) << BCHP_##reg##_##field##_SHIFT) & \
	 BCHP_##reg##_##field##_MASK)); \
	} while (0)

#define BDEV_WR_MASK(reg, field) do { \
	BDEV_WR(BCHP_##reg, BCHP_##reg##_##field##_MASK); \
	} while (0)

#define BDEV_WR_F_RB(reg, field, val) do { \
	BDEV_WR(BCHP_##reg, \
	(BDEV_RD(BCHP_##reg) & ~BCHP_##reg##_##field##_MASK) | \
	(((val) << BCHP_##reg##_##field##_SHIFT) & \
	 BCHP_##reg##_##field##_MASK)); \
	BDEV_RD(BCHP_##reg); \
	} while (0)


#endif /* __LIB_PHYSIO_H__ */

