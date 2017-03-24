/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#define _LIB_NO_MACROS_
#include "lib_string.h"
#include "lib_malloc.h"
#include "lib_printf.h"

char *lib_strcpy(char *dest, const char *src)
{
	char *ptr = dest;

	while (*src)
		*ptr++ = *src++;
	*ptr = '\0';

	return dest;
}

char *lib_strncpy(char *dest, const char *src, size_t cnt)
{
	char *ptr = dest;

	while (*src && (cnt > 0)) {
		*ptr++ = *src++;
		cnt--;
	}
	if (cnt > 0)
		*ptr = '\0';

	return dest;
}

size_t lib_xstrncpy(char *dest, const char *src, size_t cnt)
{
	char *ptr = dest;
	size_t copied = 0;

	while (*src && (cnt > 1)) {
		*ptr++ = *src++;
		cnt--;
		copied++;
	}
	*ptr = '\0';

	return copied;
}

size_t lib_strlen(const char *str)
{
	size_t cnt = 0;

	while (*str) {
		str++;
		cnt++;
	}

	return cnt;
}

#ifndef LIBSTRING_ASM
int lib_strcmp(const char *dest, const char *src)
{
	while (*src && *dest) {
		if (*dest < *src)
			return -1;
		if (*dest > *src)
			return 1;
		dest++;
		src++;
	}

	if (*dest && !*src)
		return 1;
	if (!*dest && *src)
		return -1;
	return 0;
}
#endif

int lib_strncmp(const char *dest, const char *src, size_t cnt)
{
	size_t i = 0;
	if (!cnt) /* SWBOLT-161 */
		return 0;
	while ((*src && *dest) && (i < (cnt - 1))) {
		if (*dest < *src)
			return -1;
		if (*dest > *src)
			return 1;
		dest++;
		src++;
		i++;
	}

	if (*dest == *src)
		return 0;
	else if (*dest > *src)
		return 1;
	else
		return -1;
}

int lib_strncasecmp(const char *s1, const char *s2, size_t n)
{
	if (n == 0)
		return 0;

	while (n-- != 0 && lib_tolower(*s1) == lib_tolower(*s2)) {
		if (n == 0 || *s1 == '\0' || *s2 == '\0')
			break;
		s1++;
		s2++;
	}
	return (lib_tolower(*(unsigned char *) s1) -
		lib_tolower(*(unsigned char *) s2));
}
int lib_strcmpi(const char *dest, const char *src)
{
	char dc, sc;

	while (*src && *dest) {
		dc = lib_toupper(*dest);
		sc = lib_toupper(*src);
		if (dc < sc)
			return -1;
		if (dc > sc)
			return 1;
		dest++;
		src++;
	}

	if (*dest && !*src)
		return 1;
	if (!*dest && *src)
		return -1;
	return 0;
}
int lib_snprintf(char *outbuf, int n, const char *format, ...)
{
	va_list marker;
	int count;

	va_start(marker, format);
	count = xvsnprintf(outbuf, n, format, marker);
	va_end(marker);

	return count;
}

char *lib_strchr(const char *dest, int c)
{
	while (*dest) {
		if (*dest == c)
			return (char *)dest;
		dest++;
	}
	return NULL;
}

char *lib_strstr(const char *dest, const char *find)
{
	char c, sc;
	size_t len;
	char *s = (char *)dest;

	c = *find++;
	if (c != 0) {
		len = lib_strlen(find);
		do {
			do {
				sc = *s++;
				if (sc == 0)
					return NULL;
			} while (sc != c);
		} while (lib_strncmp(s, find, len) != 0);
		s--;
	}
	return s;
}

char *lib_strnchr(const char *dest, int c, size_t cnt)
{
	while (*dest && (cnt > 0)) {
		if (*dest == c)
			return (char *)dest;
		dest++;
		cnt--;
	}
	return NULL;
}

char *lib_strrchr(const char *dest, int c)
{
	char *ret = NULL;

	while (*dest) {
		if (*dest == c)
			ret = (char *)dest;
		dest++;
	}

	return ret;
}

int lib_memcmp(const void *dest, const void *src, size_t cnt)
{
	const unsigned char *d;
	const unsigned char *s;

	d = (const unsigned char *)dest;
	s = (const unsigned char *)src;

	while (cnt) {
		if (*d < *s)
			return -1;
		if (*d > *s)
			return 1;
		d++;
		s++;
		cnt--;
	}

	return 0;
}

#ifndef LIBSTRING_ASM_MEMCPY
void *lib_memcpy(void *dest, const void *src, size_t cnt)
{
	unsigned char *d;
	const unsigned char *s;

	d = (unsigned char *)dest;
	s = (const unsigned char *)src;

	while (cnt) {
		*d++ = *s++;
		cnt--;
	}

	return dest;
}
#endif


/* No comparisons, braindead.
*/
void *lib_memmove(void *dest, const void *src, size_t n)
{
	void *tmp;
	tmp = KMALLOC(n, 1);
	if (!tmp)
		return NULL;
	lib_memcpy(tmp, src, n);
	lib_memcpy(dest, tmp, n);
	KFREE(tmp);
	return dest;
}


void *lib_memchr(const void *s, int c, size_t n)
{
	unsigned char *p = (unsigned char *)s;
	while (n) {
		if (*p == c)
			return p;
		p++; n--;
	}
	return NULL;
}



#ifndef LIBSTRING_ASM
void *lib_memset(void *dest, int c, size_t cnt)
{
	unsigned char *d;

	d = dest;

	while (cnt) {
		*d++ = (unsigned char)c;
		cnt--;
	}

	return dest;
}
#endif

char lib_toupper(char c)
{
	if ((c >= 'a') && (c <= 'z'))
		c -= 32;
	return c;
}

char lib_tolower(char c)
{

	if ((c >= 'A') && (c <= 'Z'))
		c += 32;
	return c;
}

void lib_strupr(char *str)
{
	while (*str) {
		*str = lib_toupper(*str);
		str++;
	}
}

char *lib_strcat(char *dest, const char *src)
{
	char *ptr = dest;

	while (*ptr)
		ptr++;
	while (*src)
		*ptr++ = *src++;
	*ptr = '\0';

	return dest;
}

#define isspace(x) (((x) == ' ') || ((x) == '\t'))

char *lib_gettoken(char **ptr)
{
	char *p = *ptr;
	char *ret;

	/* skip white space */

	while (*p && isspace(*p))
		p++;
	ret = p;

	/* check for end of string */

	if (!*p) {
		*ptr = p;
		return NULL;
	}

	/* skip non-whitespace */

	while (*p) {
		if (isspace(*p))
			break;

		/* do quoted strings */

		if (*p == '"') {
			p++;
			ret = p;
			while (*p && (*p != '"'))
				p++;
			if (*p == '"')
				*p = '\0';
		}

		p++;

	}

	if (*p)
		*p++ = '\0';

	*ptr = p;

	return ret;
}

int lib_atoi(const char *dest)
{
	int x = 0;
	int digit;

	if ((*dest == '0') && (*(dest + 1) == 'x'))
		return lib_xtoi(dest + 2);

	while (*dest) {
		if ((*dest >= '0') && (*dest <= '9'))
			digit = *dest - '0';
		else
			break;

		x *= 10;
		x += digit;
		dest++;
	}

	return x;
}

uint64_t lib_atoq(const char *s)
{
	uint64_t x = 0;
	int digit;

	if ((*s == '0') && (*(s + 1) == 'x'))
		return lib_xtoq(s);

	while (*s) {
		if ((*s >= '0') && (*s <= '9'))
			digit = *s - '0';
		else
			break;

		x *= 10;
		x += digit;
		s++;
	}

	return x;
}

uint64_t lib_xtoq(const char *dest)
{
	uint64_t x = 0;
	unsigned int digit;

	if ((*dest == '0') && (*(dest + 1) == 'x'))
		dest += 2;

	while (*dest) {
		if ((*dest >= '0') && (*dest <= '9'))
			digit = *dest - '0';
		else if ((*dest >= 'A') && (*dest <= 'F'))
			digit = 10 + *dest - 'A';
		else if ((*dest >= 'a') && (*dest <= 'f'))
			digit = 10 + *dest - 'a';
		else
			break;

		x *= 16;
		x += digit;
		dest++;
	}

	return x;
}

int lib_xtoi(const char *dest)
{
	int x = 0;
	int digit;

	if ((*dest == '0') && (*(dest + 1) == 'x'))
		dest += 2;

	while (*dest) {
		if ((*dest >= '0') && (*dest <= '9'))
			digit = *dest - '0';
		else if ((*dest >= 'A') && (*dest <= 'F'))
			digit = 10 + *dest - 'A';
		else if ((*dest >= 'a') && (*dest <= 'f'))
			digit = 10 + *dest - 'a';
		else
			break;

		x *= 16;
		x += digit;
		dest++;
	}

	return x;
}

/**********************************************************************
 *  lib_crc32(buf, len)
 *
 *  Yes, this is an Ethernet CRC. I'm lazy.
 *
 *  Input parameters:
 *	buf - buffer to CRC
 *	len - length of data
 *
 *  Return value:
 *	CRC-32
 **********************************************************************/
#define CRC32_POLY	0xEDB88320UL /* CRC-32 Poly */
unsigned int lib_crc32(const unsigned char *buf, unsigned int len)
{
	unsigned int idx, bit, data, crc = 0xFFFFFFFFUL;

	for (idx = 0; idx < len; idx++) {
		for (data = *buf++, bit = 0; bit < 8; bit++, data >>= 1) {
			crc =
			    (crc >> 1) ^ (((crc ^ data) & 1) ? CRC32_POLY : 0);
		}
	}

	return crc;
}
