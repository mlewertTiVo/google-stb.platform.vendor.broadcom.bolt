/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __LOADER_H__
#define __LOADER_H__

#define LOADFLG_NOISY		0x0001	/* print out noisy info */
#define LOADFLG_EXECUTE		0x0002	/* execute loaded program */
#define LOADFLG_SPECADDR	0x0004	/* Use a specific size & addr */
#define LOADFLG_NOBB		0x0008	/* don't look for a boot block */
#define LOADFLG_NOCLOSE		0x0010	/* don't close network */
#define LOADFLG_COMPRESSED	0x0020	/* file is compressed */
#define LOADFLG_BATCH		0x0040	/* batch file */
#define LOADFLG_SKIPHEAD	0x0080	/* skip BRCM image header (8K) */
#define LOADFLG_BSU 		0x0100	/* BSU sidecar app */
#define LOADFLG_APP64		0x0200	/* 64 bit app */
#define LOADFLG_DIRECT_CALL	0x0400	/* Don't use PSCI etc. */
#define LOADFLG_EL3_EXEC	0x0800	/* Execute at EL3 level */
#define LOADFLG_SECURE		0x1000	/* Indicate the image is secure */

typedef struct bolt_loadargs_s {
	const char *la_filename; /* name of file on I/O device */
	const char *la_filesys;	/* file system name */
	char *la_device;	/* device name (ide0, etc.) */
	char *la_options;	/* args to pass to loaded prog */
	unsigned int la_flags;	/* various flags */
	unsigned long la_offs;	/* offset within the device/file */
	long la_address;	/* used by SPECADDR only */
	unsigned long la_maxsize;	/* used by SPECADDR only */
	long la_entrypt;	/* returned entry point */
} bolt_loadargs_t;

typedef struct bolt_loader_s {
	const char *name;			/* name of loader */
	int (*loader) (bolt_loadargs_t *);	/* access function */
	int flags;		/* flags */
} bolt_loader_t;

#define LDRLOAD(ldr,arg) (*((ldr)->loader))(arg)

int bolt_load_program(const char *name, bolt_loadargs_t * la);
const bolt_loader_t *bolt_findloader(const char *name);
void splitpath(char *path, char **devname, char **filename);
int bolt_go(bolt_loadargs_t * la);
int bolt_boot(const char *ldrname, bolt_loadargs_t * la);
int bolt_savedata(char *fsname, char *devname, char *filename, uint8_t * start,
		 uint8_t * end);
int bolt_addloader(const bolt_loader_t *loader);

#endif /* __LOADER_H__ */

