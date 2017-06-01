/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#define _LIB_NO_MACROS_
#include <lib_ctype.h>

int lib_isxdigit(int c)
{
	if (lib_isdigit(c))
		return 1;
	if ((c >= 'A') && (c <= 'F'))
		return 1;
	if ((c >= 'a') && (c <= 'f'))
		return 1;

	return 0;
}

int lib_tolower(int c)
{
	/* The coded value of 'a' must be bigger than 'A', which is
	 * true in ASCII. But, it might not be in other coding systems.
	 */
	if ((c >= 'A') && (c <= 'Z'))
		c += 'a' - 'A';
	return c;
}

int lib_toupper(int c)
{
	/* The coded value of 'a' must be bigger than 'A', which is
	 * true in ASCII. But, it might not be in other coding systems.
	 */
	if ((c >= 'a') && (c <= 'z'))
		c -= 'a' - 'A';
	return c;
}
