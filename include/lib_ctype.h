/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __LIB_CTYPE_H__
#define __LIB_CTYPE_H__

static inline int lib_isdigit(int c)
{
	return ((c >= '0') && (c <= '9'));
}

/* lib_isspace -- checks for white-space characters.
 *
 * In the "C" and "POSIX" locales, these are: space, form-feed ('\f'),
 * newline ('\n'), carriage return ('\r'), horizontal tab ('\t'), and
 * vertical tab ('\v').
 */
static inline int lib_isspace(int c)
{
	/* in the order of most common white space characters,
	 * to take advangate of short-circuit
	 */
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r' ||
		c == '\f' || c == '\v');
}

int lib_isxdigit(int c);
int lib_tolower(int c);
int lib_toupper(int c);

#ifndef _LIB_NO_MACROS_
#define isdigit(c) lib_isdigit(c)
#define isspace(c) lib_isspace(c)
#define isxdigit(c) lib_isxdigit(c)
#define tolower(c) lib_tolower(c)
#define toupper(c) lib_toupper(c)
#endif

#endif /* __LIB_CTYPE_H__ */
