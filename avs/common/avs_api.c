/***************************************************************************
 *     Copyright (c) 2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 *
***************************************************************************/

#include "../fsbl/fsbl.h"	/* for uart i/o */
#include "avs.h"

/* This process MUST happen early in the boot cycle */
void avs_init(void)
{
	char *s = "AVS init";
	__puts(s);
	puts("...");
#if defined(ENABLE_AVS_INIT)
	avs_class_init();	/* initialize the AVS hardware */
	__puts(s);
	puts(" OK");
#else
	__puts(s);
	puts(" NO");
#endif
}


static void __maybe_unused avs_perror(int rc)
{
#if defined(ENABLE_AVS_FIRMWARE)
	switch (rc) {
	case AVS_TIMEOUT:
		puts("TIMEOUT");
		break;
	case AVS_SUCCESS:
		puts("OK");
		break;
	case AVS_FAILURE:
		puts("FAIL (board may need an ECO)");
		break;
	case AVS_DISABLED:
		puts("OFF");
		break;
	case AVS_LOADED:
		puts("LOADED");
		break;
	case AVS_VERIFY_FAILED:
		puts("!VERIFY");
		break;
	case AVS_NOT_FOUND:
		puts("!FOUND");
		break;
	case CHIP_MISMATCH:
		puts("MISMATCH");
		break;
	default:
		puts("?");
		break;
	}
	if ((rc == AVS_SUCCESS) || (rc == AVS_DISABLED) || (rc == AVS_LOADED))
		sec_avs_set_status(1); /* ok */
	else
		sec_avs_set_status(0); /* bad */

#else
	puts("");
#endif
}


int avs_load(void)
{
	int rc = AVS_DISABLED;
	__puts("AVS load:");
#if defined(ENABLE_AVS_FIRMWARE)
	rc = avs_common_load();
	avs_perror(rc);
#else
	puts("!EN");
#endif
	return rc;
}

/* Note: this process can be moved to later point in the boot cycle */
int avs_start(int en)
{
	int rc = AVS_DISABLED;
	__puts("AVS start:");
#if defined(ENABLE_AVS_FIRMWARE)
	if (en) {
		rc = avs_common_start();
		avs_perror(rc);
	} else {
		puts("NO");
	}
#else
	puts("!EN");
#endif
	return rc;
}
