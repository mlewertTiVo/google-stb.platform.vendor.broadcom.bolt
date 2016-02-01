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
#include "lib_malloc.h"
#define _LIB_NO_MACROS_
#include "lib_string.h"

/*  *********************************************************************
    *  lib_parseipaddr(ipaddr,dest)
    *
    *  Parse an IP address.
    *
    *  Input parameters:
    *		ipaddr - string of IP address
    *		dest - pointer to 4 bytes to receive binary IP address
    *
    *  Return value:
    *		0 if ok
    *		-1 if ip address is invalid
    ********************************************************************* */

int lib_parseipaddr(const char *ipaddr, uint8_t *dest)
{
	int a, b, c, d;
	char *x;

	/* make sure it's all digits and dots. */
	x = (char *)ipaddr;
	while (*x) {
		if ((*x == '.') || ((*x >= '0') && (*x <= '9'))) {
			x++;
			continue;
		}
		return -1;
	}

	x = (char *)ipaddr;
	a = lib_atoi(ipaddr);
	x = lib_strchr(x, '.');
	if (!x)
		return -1;
	b = lib_atoi(x + 1);
	x = lib_strchr(x + 1, '.');
	if (!x)
		return -1;
	c = lib_atoi(x + 1);
	x = lib_strchr(x + 1, '.');
	if (!x)
		return -1;
	d = lib_atoi(x + 1);

	if ((a < 0) || (a > 255))
		return -1;
	if ((b < 0) || (b > 255))
		return -1;
	if ((c < 0) || (c > 255))
		return -1;
	if ((d < 0) || (d > 255))
		return -1;

	dest[0] = (uint8_t) a;
	dest[1] = (uint8_t) b;
	dest[2] = (uint8_t) c;
	dest[3] = (uint8_t) d;

	return 0;
}

/*  *********************************************************************
    *  lib_lookup(list,str)
    *
    *  Look up an element on a {string,value} list.
    *
    *  Input parameters:
    *		list - list to search
    *		str - string to find on the list
    *
    *  Return value:
    *		0 if string was not found
    *		else number associated with this string
    ********************************************************************* */

int lib_lookup(const cons_t *list, char *str)
{
	while (list->str) {
		if (lib_strcmp(list->str, str) == 0)
			return list->num;
		list++;
	}

	return 0;

}

/*  *********************************************************************
    *  lib_findinlist(list,str)
    *
    *  Like lib_lookup but returns cons structure instead of value
    *
    *  Input parameters:
    *		list - list of associations
    *		str - what to find
    *
    *  Return value:
    *		cons_t or null if not found
    ********************************************************************* */

static const cons_t *lib_findinlist(const cons_t *list, char *str)
{
	while (list->str) {
		if (lib_strcmp(list->str, str) == 0)
			return list;
		list++;
	}
	return NULL;
}

/*  *********************************************************************
    *  lib_setoptions(list,str,flags)
    *
    *  Set or reset one or more bits in a flags variable based
    *  on the list of valid bits and a string containing what
    *  to change.  flags starts off as a default value.
    *
    *  The input string is a comma-separated list of options,
    *  optionally prefixed by "no_" or "no" to invert the
    *  sense of the option.  negative values in the table
    *  remove options, positive add options (you can't use
    *  bit 31 as an option for this reason).
    *
    *  Input parameters:
    *		list - list of valid options
    *		str - options to parse
    *		flags - pointer to variable to be modified
    *
    *  Return value:
    *		number of options we did not understand, 0=ok
    ********************************************************************* */

int lib_setoptions(const cons_t *list, char *str, unsigned int *flags)
{
	char *dupstr;
	char *x;
	char *ptr;
	const cons_t *val;
	int newbits;
	int errors = 0;

	if (!list || !str || !flags)
		return 0;

	dupstr = lib_strdup(str);
	if (!dupstr)
		return 0;

	ptr = dupstr;

	while (*ptr) {
		x = lib_strchr(ptr, ',');
		if (x)
			*x = '\0';

		val = lib_findinlist(list, ptr);
		newbits = 0;
		if (!val) {

			if (lib_memcmp(ptr, "no_", 3) == 0)
				val = lib_findinlist(list, ptr + 3);
			else if (lib_memcmp(ptr, "no", 2) == 0)
				val = lib_findinlist(list, ptr + 2);

			if (val)
				newbits = ~((unsigned int)(val->num));
			else
				errors++;
		} else {
			newbits = (val->num);
		}

		/* if new bits are negative, it's an AND mask
		   otherwise it's an OR mask */

		if (newbits < 0)
			*flags &= (unsigned int)newbits;
		else
			*flags |= (unsigned int)newbits;

		if (x)
			ptr = x + 1;
		else
			break;
	}

	KFREE(dupstr);

	return errors;
}
