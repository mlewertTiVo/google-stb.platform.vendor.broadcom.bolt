/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __PSCI_H__
#define __PSCI_H__

/* For interfacing to System Management Mode (PSCI etc.) */

/* "All errors are considered to be 32-bit signed integers.
 * Therefore, when using the SMC64 calling convention, the
 * upper word will be zero." - POWER STATE COORDINATION
 * INTERFACE (PSCI), ARM DEN 0022C
 *
 *  Note: u64 parameters are used when called in aarch64 mode,
 * u32 parameters when called in A32/T32 mode, unless stated
 * otherwise. Return values are always cast as i32 (signed.)
 */

#define PSCI_SUCCESS			0x00000000
#define PSCI_ERR_NOT_SUPPORTED		0xffffffff /* -1 */
#define PSCI_ERR_INVALID_PARAMETERS	0xfffffffe
#define PSCI_ERR_DENIED			0xfffffffd
#define PSCI_ERR_ALREADY_ON		0xfffffffc
#define PSCI_ERR_ON_PENDING		0xfffffffb
#define PSCI_ERR_INTERNAL_FAILURE	0xfffffffa
#define PSCI_ERR_NOT_PRESENT		0xfffffff9
#define PSCI_ERR_DISABLED		0xfffffff8
#define PSCI_ERR_INVALID_ADDRESS	0xfffffff7 /* -9 */


/* ------------------------------------------
 * Broadcom specific (OEM Service Calls)
 * ------------------------------------------ */

/* OEM_PSCI_INIT
 *
 * SMC call to initalize PSCI. Pass in the
 * maximum number of cpus and cpu clusters
 * PSCI is expected to handle.
 *
 *  params  w0/x0: SMM_FUNC_INIT
 *          w1/x1: Number of cpus per cluster, max number of clusters
 *          w2/x2: unused
 *          w3/x3: unused
 */
#define OEM_PSCI_INIT			0x83000000

#define OEM_PSCI_INIT_NR_CPU_MASK	0x000000ff
#define OEM_PSCI_INIT_NR_CPU_SHIFT	0
#define OEM_PSCI_INIT_NR_CLUST_MASK	0x0000ff00
#define OEM_PSCI_INIT_NR_CLUST_SHIFT	8
#define OEM_PSCI_INIT_DEBUG_MASK	0x01000000
#define	OEM_PSCI_INIT_LOCK_MASK		0x02000000
#define	OEM_PSCI_INIT_IS_RAC_MASTER	0x04000000


/* OEM_FUNC_EXEC64
 *
 * SMC call to switch to aarch64 EL2, then boot Linux etc.
 * Note: Upper 32 bits of the parameters are masked off
 * if called from aarch32. Format is for smc32 calling
 * convention only.
 *
 *  params  w0/x0: SMM_FUNC_EXEC64
 *          w1/x1: aarch64 Executable (Linux) address
 *          w2/x2: Optional DTB address
 */
#define OEM_FUNC_EXEC64			0x83000001


/* OEM_FUNC_EXEC32
 *
 * SMC call to switch to SVC A32, then boot Linux etc.
 * Note: Upper 32 bits of the parameters are masked off
 * if called from aarch32. Format is for smc32 calling
 * convention only.
 *
 *  params  w0/x0: SMM_FUNC_EXEC32
 *          w1/x1: aarch32 Executable (Linux) address
 *          w2/x2: Optional DTB address
 */
#define OEM_FUNC_EXEC32			0x83000002


/* OEM_PSCI_LOCK
 *
 * SMC call to acquire PSCI lock. Only active if
 * debug was enabled in OEM_PSCI_INIT.
 *
 *  params  w0/x0: OEM_PSCI_LOCK
 *          w1/x1: unused
 *          w2/x2: unused
 *          w3/x3: unused
 */
#define OEM_PSCI_LOCK			0x83000003

/* OEM_PSCI_UNLOCK
 *
 * SMC call to free PSCI lock. Only active if
 * debug was enabled in OEM_PSCI_INIT.
 *
 *  params  w0/x0: OEM_PSCI_UNLOCK
 *          w1/x1: unused
 *          w2/x2: unused
 *          w3/x3: unused
 */
#define OEM_PSCI_UNLOCK			0x83000004


/* OEM_PSCI_RAC_DISABLE
 *
 * SMC call to disable and also flush the RAC.
 *
 * Disables the RAC, flushes it and returns the
 * original RAC setting. Returns zero if the
 * RAC was already disabled, and no flush will
 * be done in this case.
 *
 *  params  w0/x0: OEM_PSCI_RAC_DISABLE_FLUSH
 *          w1/x1: unused
 *          w2/x2: unused
 *          w3/x3: unused
 */
#define OEM_PSCI_RAC_DISABLE		0x83000005

/* OEM_PSCI_RAC_ENABLE
 *
 * SMC call to enable RAC.
 * To change a confg call OEM_PSCI_RAC_DISABLE_FLUSH
 * before OEM_PSCI_RAC_ENABLE.
 * Returns PSCI_SUCCESS.
 *
 *  params  w0/x0: OEM_PSCI_UNLOCK
 *          w1/x1: RAC config
 *          w2/x2: unused
 *          w3/x3: unused
 */
#define OEM_PSCI_RAC_ENABLE		0x83000006


/* ------------------------------------------
 *     PSCI v0.2 (from Issue 'C' v1.0 docs)
 * ------------------------------------------ */

/* u32 Function ID
 *  Bits [31:16] Major Version
 *  Bits [15:0] Minor Version
 */
#define PSCI_VERSION			0x84000000

#define PSCI_VERSION_MAX(x)		((x & 0xff00) >> 8)
#define PSCI_VERSION_MIN(x)		(x & 0xff)


/*     u32 Function ID
 *     u32 power_state
 * u32/u64 entry_point_address
 * u32/u64 context_id
 */
#define PSCI_CPU_SUSPEND		0xc4000001
#define PSCI_CPU_SUSPEND_32		0x84000001

/* v0.2 power_state field format */

/* State ID is implementation defined. */
#define PSCI_POWER_STATE_ID_MASK	0x0000ffff
#define PSCI_POWER_STATE_ID_SHIFT	0

#define PSCI_POWER_STATE_TYPE_MASK	0x00010000
#define PSCI_POWER_STATE_TYPE_SHIFT	16

#define PSCI_POWER_STATE_TYPE_STANDBY	0
#define PSCI_POWER_STATE_TYPE_POWERDOWN	1

#define PSCI_POWER_STATE_LEVEL_MASK	0x03000000
#define PSCI_POWER_STATE_LEVEL_SHIFT	24

#define PSCI_POWER_STATE_LEVEL_CORE	0
#define PSCI_POWER_STATE_LEVEL_CLUSTER	1
#define PSCI_POWER_STATE_LEVEL_SYSTEM	2
#define PSCI_POWER_STATE_LEVEL_UNDEF	3


/*     u32 Function ID
 */
#define PSCI_CPU_OFF			0x84000002


/*     u32 Function ID
 * u32/u64 target_cpu
 * u32/u64 entry_point_address
 * u32/u64 context_id
 */
#define PSCI_CPU_ON			0xc4000003
#define PSCI_CPU_ON_32			0x84000003


/*     u32 Function ID
 * u32/u64 target_affinity
 *     u32 lowest_affinity_level
 */
#define PSCI_AFFINITY_INFO		0xc4000004
#define PSCI_AFFINITY_INFO_32		0x84000004


/*     u32 Function ID
 * u32/u64 target_cpu
 */
#define PSCI_MIGRATE			0xc4000005
#define PSCI_MIGRATE_32			0x84000005


/*     u32 Function ID
 *     i32 0 Uniprocessor migrate capable
 *         1 Uniprocessor not migrate capable Trusted OS.
 *         2 Trusted OS is n/a or does not require migration.
 */
#define PSCI_MIGRATE_INFO_TYPE		0x84000006


/*     u32 Function ID
 * returns: u32/u64 MPIDR, else UNDEFINED if
 *         MIGRATE_INFO_TYPE returns 2
 *         or NOT_SUPPORTED.
 */
#define PSCI_MIGRATE_INFO_UP_CPU	0xc4000007
#define PSCI_MIGRATE_INFO_UP_CPU_32	0x84000007


/*     u32 Function ID
 */
#define PSCI_SYSTEM_OFF			0x84000008


/*     u32 Function ID
 */
#define PSCI_SYSTEM_RESET		0x84000009


/*     u32 Function ID
 *     u32 psci_func_id
 */
#define PSCI_PSCI_FEATURES		0x8400000a


/*     u32 Function ID
 */
#define PSCI_CPU_FREEZE			0x8400000b


/*     u32 Function ID
 * u32/u64 entry_point_address
 * u32/u64 context_id
 */
#define PSCI_CPU_DEFAULT_SUSPEND	0xc400000c
#define PSCI_CPU_DEFAULT_SUSPEND_32	0x8400000c


/*     u32 Function ID
 * u32/u64 target_cpu
 *     u32 power_level
 */
#define PSCI_NODE_HW_STATE		0xc400000d
#define PSCI_NODE_HW_STATE_32		0x8400000d


/*     u32 Function ID
 * u32/u64 entry_point_address
 * u32/u64 context_id
 */
#define PSCI_SYSTEM_SUSPEND		0xc400000e
#define PSCI_SYSTEM_SUSPEND_32		0x8400000e


/*     u32 Function ID
 *     u32 mode
 */
#define PSCI_PSCI_SET_SUSPEND_MODE	0x8400000f


/*     u32 Function ID
 * u32/u64 target_cpu
 *     u32 power_state
 * u32/u64 residency
 */
#define PSCI_PSCI_STAT_RESIDENCY	0xc4000010
#define PSCI_PSCI_STAT_RESIDENCY_32	0x84000010


/*     u32 Function ID
 * u32/u64 target_cpu
 *     u32 power_state
 * u32/u64 count
 */
#define PSCI_PSCI_STAT_COUNT		0xc4000011
#define PSCI_PSCI_STAT_COUNT_32		0x84000011

#endif /* __PSCI_H__ */
