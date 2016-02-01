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

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

/*  *********************************************************************
    *  Constants
    ********************************************************************* */

#define VKEY(x)		(0x100|(x))
#define VKEY_UP		VKEY(1)
#define VKEY_DOWN	VKEY(2)
#define VKEY_LEFT	VKEY(3)
#define VKEY_RIGHT	VKEY(4)
#define VKEY_PGUP	VKEY(5)
#define VKEY_PGDN	VKEY(6)
#define VKEY_HOME	VKEY(7)
#define VKEY_END	VKEY(8)
#define VKEY_F1		VKEY(0x10)
#define VKEY_F2		VKEY(0x11)
#define VKEY_F3		VKEY(0x12)
#define VKEY_F4		VKEY(0x13)
#define VKEY_F5		VKEY(0x14)
#define VKEY_F6		VKEY(0x15)
#define VKEY_F7		VKEY(0x16)
#define VKEY_F8		VKEY(0x17)
#define VKEY_F9		VKEY(0x18)
#define VKEY_F10	VKEY(0x19)
#define VKEY_F11	VKEY(0x1A)
#define VKEY_F12	VKEY(0x1B)
#define VKEY_ESC	27

/*  *********************************************************************
    *  Prototypes
    ********************************************************************* */

int console_open(char *name);
int console_close(void);
int console_read(unsigned char *buffer, int length);
int console_write( /* arm unsigned */ char *buffer, int length);
int console_status(void);
int console_readkey(void);
int console_readline(char *prompt, char *str, int len);
int console_readline_noedit(char *prompt, char *str, int len);
int console_readline(char *prompt, char *str, int len);
extern char *console_name;
extern int console_handle;
void console_log(const char *tmplt, ...);

#endif /* __CONSOLE_H__ */

