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

#ifndef __LIB_PRINTF_H__
#define __LIB_PRINTF_H__

#include <stdarg.h>

#define __printf(a, b)          __attribute__((format(printf, a, b)))

#define ANSI_RED	"\e[31;1m"
#define ANSI_YELLOW	"\e[33;1m"
#define ANSI_CYAN	"\e[36;1m"
#define ANSI_BLINK	"\e[5m"
#define ANSI_NOBLINK	"\e[25m"
#define ANSI_RESET	"\e[0m"

__printf(2, 0)
int xvsprintf(char *outbuf, const char *templat, va_list marker);

__printf(2, 3)
int xsprintf(char *buf, const char *templat, ...);

__printf(1, 2)
int xprintf(const char *templat, ...);

extern int (*xprinthook) (const char *);

__printf(1, 0)
int xvprintf(const char *template, va_list marker);

__printf(3, 0)
int xvsnprintf(char *outbuf, int n, const char *templat, va_list marker);

#define err_msg(fmt, ...) xprintf(ANSI_RED fmt ANSI_RESET "\n", ##__VA_ARGS__)
#define warn_msg(fmt, ...) xprintf(ANSI_YELLOW fmt ANSI_RESET "\n", ##__VA_ARGS__)
#define info_msg(fmt, ...) xprintf(ANSI_CYAN fmt ANSI_RESET "\n", ##__VA_ARGS__)

/*
 * compatibility macros
 */

#define printf xprintf
#define sprintf xsprintf
#define vsprintf xvsprintf
#define vsnprintf xvsnprintf
#define vprintf xvprintf

#endif /* __LIB_PRINTF_H__ */

