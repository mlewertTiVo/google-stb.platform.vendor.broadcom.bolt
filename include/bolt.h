/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __BOLT_H__
#define __BOLT_H__

/*  *********************************************************************
    *  Version number
    ********************************************************************* */

#define BOLT_VERSION ((BOLT_VER_MAJOR<<16)|BOLT_VER_MINOR)

/*  *********************************************************************
    *  Some runtime startup parameters
    ********************************************************************* */

extern unsigned bolt_startflags;

extern const int buildyear;
extern const char builddate[];
extern const char builduser[];
extern const char buildtag[];

#define BOLT_INIT_SAFE	0x00010000	/* "Safe mode" */
#define BOLT_INIT_USER	0x0000FFFF	/* these are BSP-specific flags */
#define BOLT_LDT_SLAVE   0x00040000	/* Select LDT slave mode */

/*  *********************************************************************
    *  Other constants
    ********************************************************************* */

#define BOLT_BUFFER_CONSOLE "buffer"
#define BOLT_MAX_HANDLE 64	/* max file handles */

/*  *********************************************************************
    *  prototypes
    ********************************************************************* */

/* Please keep these in alphabetical order */
const char *bolt_errortext(int err);
void board_device_init(void);
void board_device_reset(void);
void board_final_exit(unsigned long *start_address);
void board_final_init(void);
void board_init_post_splash(void);
void bolt_command_loop(void);
void bolt_config_info(int detail);
int  bolt_docommands(const char *buf);
void bolt_free_envdevname(void);
void bolt_leds(unsigned int val);
void bolt_main(int, int);
void bolt_restart(void);
int  bolt_set_console(char *);
int  bolt_set_envdevice(char *);
void bolt_set_envdevname(char *name);
int  bolt_set_macdevice(char *);
void bolt_warmstart(unsigned long long);
void console_init(void);
void bolt_psci_init(void);

/*  _start() are 'c' wrappers for assembly code _launch()
 * functions.
 */
void bolt_start(unsigned long ept, unsigned long param1,
		unsigned long param2, unsigned long param3);

void bolt_launch(unsigned long ept, unsigned long param1,
		unsigned long param2, unsigned long param3);

/* PSCI */
void bolt_start64(unsigned long ept, unsigned long param1,
		unsigned long param2, unsigned long param3);

void bolt_start32(unsigned long ept, unsigned long param1,
		unsigned long param2, unsigned long param3);

void bolt_launch64(unsigned long ept, unsigned long param1,
		unsigned long param2, unsigned long param3);

void bolt_launch32(unsigned long ept, unsigned long param1,
		unsigned long param2, unsigned long param3);

unsigned long psci(unsigned long r0, unsigned long r1,
		unsigned long r2, unsigned long r3);

#endif /* __BOLT_H__ */
