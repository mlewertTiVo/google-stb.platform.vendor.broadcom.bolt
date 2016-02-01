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

#include "lib_types.h"
#include "macutils.h"

int MAC_str2bin(char *srcptr, uint8_t *dstptr)
{
	char c;
	int i;
	int m;
	int retval = 0;

	for (i = 0; i < 6; i++)
		dstptr[i] = 0;

	for (i = 0; i < 17; i++) {
		c = *srcptr++;

		m = (i + 1) % 3;

		/* Skip every 3rd char.  Just a separator. */
		if (!m)
			continue;

		if (c < '0' || c > 'f') {
			/* Invalid character */
			retval = 1;
			break;
		} else if (c < ':') {
			/* It's a digit */
			*dstptr |= c - '0';
		} else if (c >= 'a') {
			/* It's a lowercase hex digit */
			*dstptr |= c - 'a' + 10;
		} else if (c < 'A' || c > 'F') {
			/* Invalid character */
			retval = 1;
			break;
		} else {
			/* It's an uppercase hex digit */
			*dstptr |= c - 'A' + 10;
		}

		if (m == 1)
			*dstptr <<= 4;
		else
			dstptr++;
	}

	return retval;
}
