/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 ***************************************************************************/
#ifndef MEMSYS_TOP_API_H__
#define MEMSYS_TOP_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MEMSYS_FW_VERSION_MAJOR_SHIFT	24
#define MEMSYS_FW_VERSION_MINOR_SHIFT	16
#define MEMSYS_FW_VERSION_PATCH_SHIFT	8
#define MEMSYS_FW_VERSION_ENG_SHIFT	0

#define MEMSYS_FW_VERSION_MAJOR	0
#define MEMSYS_FW_VERSION_MINOR	0
#define MEMSYS_FW_VERSION_PATCH	0
#define MEMSYS_FW_VERSION_ENG	0

#define MEMSYS_FW_VERSION (\
	(MEMSYS_FW_VERSION_MAJOR << MEMSYS_FW_VERSION_MAJOR_SHIFT) | \
	(MEMSYS_FW_VERSION_MINOR << MEMSYS_FW_VERSION_MINOR_SHIFT) | \
	(MEMSYS_FW_VERSION_PATCH << MEMSYS_FW_VERSION_PATCH_SHIFT) | \
	(MEMSYS_FW_VERSION_ENG   << MEMSYS_FW_VERSION_ENG_SHIFT))

/* memsys_version_t
 *   Memsys Library version data structure
 *
 *   version - Memsys Library version, same as MEMSYS_FW_VERSION.
 *   build_ver - A string showing internal build information such as source
 *       code revision. Note this is a pointer to a global address. This
 *       pointer may be NULL.
 *   hw_ver_str - A string showing the HW combo (PHY + MEMC) this library
 *       supports, for example, "hpf0_b1p3". Note this is a pointer to a
 *       global address. This pointer may be NULL.
 *   ver_str - Version string. This is a human readable string of the library
 *       version, e.g. "1.0.0.0". Note this is a pointer to a global address.
 *       This pointer may be NULL to prevent external output of version info.
 */
typedef struct memsys_version {
	uint32_t version;	/* MEMSYS_FW_VERSION */
	const char *build_ver;	/* internal */
	const char *hw_ver_str;	/* HW version this library supports */
	const char *ver_str;	/* library version */
} memsys_version_t;

typedef struct memsys_error {
	/* skeleton is good enough */
} memsys_error_t;

typedef struct memsys_top_params {
	/* skeleton is good enough */
} memsys_top_params_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MEMSYS_TOP_API_H__ */
