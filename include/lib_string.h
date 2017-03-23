/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

#ifndef __LIB_STRING_H__
#define __LIB_STRING_H__

#include "lib_types.h"
#include <stddef.h>
#include <stdarg.h>

char *lib_strcpy(char *dest, const char *src);
char *lib_strncpy(char *dest, const char *src, size_t cnt);
size_t lib_xstrncpy(char *dest, const char *src, size_t cnt);
size_t lib_strlen(const char *str);

int lib_strcmp(const char *dest, const char *src);
int lib_strcmpi(const char *dest, const char *src);
int lib_strncasecmp(const char *s1, const char *s2, size_t n);
char *lib_strchr(const char *dest, int c);
char *lib_strrchr(const char *dest, int c);
int lib_memcmp(const void *dest, const void *src, size_t cnt);
void *lib_memcpy(void *dest, const void *src, size_t cnt);
void *lib_memmove(void *dest, const void *src, size_t n);
void *lib_memchr(const void *s, int c, size_t n);
void *lib_memset(void *dest, int c, size_t cnt);
char *lib_strdup(const char *str);
void lib_strupr(char *s);
char lib_toupper(char c);
char lib_tolower(char c);
char *lib_strcat(char *dest, const char *src);
char *lib_gettoken(char **str);
char *lib_strnchr(const char *dest, int c, size_t cnt);
int lib_parseipaddr(const char *ipaddr, uint8_t * dest);
int lib_atoi(const char *dest);
uint64_t lib_atoq(const char *s);
int lib_lookup(const cons_t * list, char *str);
int lib_setoptions(const cons_t * list, char *str, unsigned int *flags);
int lib_xtoi(const char *dest);
uint64_t lib_xtoq(const char *dest);
char *lib_strstr(const char *dest, const char *find);
int lib_strncmp(const char *dest, const char *src, size_t cnt);
int lib_snprintf(char *outbuf, int n, const char *format, ...);

static inline void *lib_xtop(const char *s)
{
	/*
	 * The extra cast to uintptr_t is ugly, but without it the compiler
	 * will warn about the possibility of unintentional truncation if
	 * pointer size is less than 64 bits.
	 */
	return (void *)((uintptr_t)lib_xtoq(s));
}

#ifndef _LIB_NO_MACROS_
#define strcpy(d,s) lib_strcpy(d,s)
#define strncpy(d,s,c) lib_strncpy(d,s,c)
#define xstrncpy(d,s,c) lib_xstrncpy(d,s,c)
#define strlen(s) lib_strlen(s)
#define strchr(s,c) lib_strchr(s,c)
#define strrchr(s,c) lib_strrchr(s,c)
#define strdup(s) lib_strdup(s)
#define strcmp(d,s) lib_strcmp(d,s)
#define strcmpi(d,s) lib_strcmpi(d,s)
#define memcmp(d,s,c) lib_memcmp(d,s,c)
#define memset(d,s,c) lib_memset(d,s,c)
#define memcpy(d,s,c) lib_memcpy(d,s,c)
#define memmove(d,s,c) lib_memmove(d,s,c)
#define memchr(s,c,n) lib_memchr(s,c,n)
#define bcopy(s, d, c) lib_memcpy(d, s, c)
#define bzero(d, c) lib_memset(d, 0, c)
#define strupr(s) lib_strupr(s)
#define toupper(c) lib_toupper(c)
#define tolower(c) lib_tolower(c)
#define strcat(d, s) lib_strcat(d, s)
#define gettoken(s) lib_gettoken(s)
#define strnchr(d,ch,cnt) lib_strnchr(d,ch,cnt)
#define atoi(d) lib_atoi(d)
#define atoq(d) lib_atoq(d)
#define xtoi(d) lib_xtoi(d)
#define xtoq(d) lib_xtoq(d)
#define xtop(d) lib_xtop(d)
#define parseipaddr(i,d) lib_parseipaddr(i,d)
#define lookup(x,y) lib_lookup(x,y)
#define setoptions(x,y,z) lib_setoptions(x,y,z)
#define strstr(d,s) lib_strstr(d,s)
#define strncmp(d,s,c) lib_strncmp(d,s,c)
#define strncasecmp(d, s, n) lib_strncasecmp(d, s, n)

#endif

void
qsort(void *bot, size_t nmemb, size_t size,
      int (*compar) (const void *, const void *));

unsigned int lib_crc32(const unsigned char *buf, unsigned int len);

#endif /* __LIB_STRING_H__ */

