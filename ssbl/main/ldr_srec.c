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
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "iocb.h"
#include "device.h"
#include "error.h"
#include "devfuncs.h"

#include "bolt.h"
#include "fileops.h"

#include "boot.h"
#include "bootblock.h"

#include "loader.h"

/*  *********************************************************************
    *  Externs
    ********************************************************************* */

static int bolt_srecload(bolt_loadargs_t *la);

const bolt_loader_t srecloader = {
	"srec",
	bolt_srecload,
	0
};

typedef struct linebuf_s {
	char linebuf[256];
	int curidx;
	int buflen;
	int eof;
	void *ref;
	fileio_ctx_t *fsctx;
} linebuf_t;

#define initlinebuf(b, r, o)\
	do {\
		(b)->curidx = 0;\
		(b)->buflen = 0;\
		(b)->eof = 0 ;\
		(b)->ref = r;\
		(b)->fsctx = o;\
	} while (0)

/*  *********************************************************************
    *  readchar(file)
    *
    *  Read a character from a file.  It's kind of like getchar
    *  on "C" FILE devices, but not so fancy.
    *
    *  Input parameters:
    *      file - pointer to a linebuf_t containing reader state
    *
    *  Return value:
    *      character, or -1 if at EOF
    ********************************************************************* */

static int readchar(linebuf_t *file)
{
	int ch;
	int res;

	if (file->eof)
		return -1;

	if (file->curidx == file->buflen) {
		for (;;) {
			res =
			    fs_read(file->fsctx, file->ref,
				    (uint8_t *) file->linebuf,
				    sizeof(file->linebuf));
			if (res < 0) {
				file->eof = -1;
				return -1;
			}
			if (res == 0)
				continue;
			file->buflen = res;
			file->curidx = 0;
			break;
		}
	}

	ch = file->linebuf[file->curidx];
	file->curidx++;
	return ch;
}

/*  *********************************************************************
    *  readline(file,buffer,maxlen)
    *
    *  Read a line of text from a file using our crude file stream
    *  mechanism.
    *
    *  Input parameters:
    *      file - pointer to a linebuf_t containing reader state
    *      buffer - will receive line of text
    *      maxlen - number of bytes that will fit in buffer
    *
    *  Return value:
    *      0 if ok, else <0 if at EOF
    ********************************************************************* */

static int readline(linebuf_t *file, char *buffer, int maxlen)
{
	int ch;
	char *ptr;

	ptr = buffer;
	maxlen--;		/* account for terminating null */

	while (maxlen) {
		ch = readchar(file);
		if (ch == -1)
			return -1;
		if (ch == 27)
			return -1;	/* ESC */
		if ((ch == '\n') || (ch == '\r'))
			break;
		*ptr++ = (char)ch;
		maxlen--;
	}

	*ptr = '\0';

	return 0;
}

/*  *********************************************************************
    *  getxdigit(c)
    *
    *  Convert a hex digit into its numeric equivalent
    *
    *  Input parameters:
    *      c - character
    *
    *  Return value:
    *      value
    ********************************************************************* */

static int getxdigit(char c)
{
	if ((c >= '0') && (c <= '9'))
		return c - '0';
	if ((c >= 'A') && (c <= 'F'))
		return c - 'A' + 10;
	if ((c >= 'a') && (c <= 'f'))
		return c - 'a' + 10;
	return -1;
}

/*  *********************************************************************
    *  getbyte(line)
    *
    *  Process two hex digits and return the value
    *
    *  Input parameters:
    *      line - pointer to pointer to characters (updated on exit)
    *
    *  Return value:
    *      byte value, or <0 if bad hex digits
    ********************************************************************* */

static int getbyte(char **line)
{
	int res;
	int c1, c2;

	c1 = getxdigit(*(*(line) + 0));
	if (c1 < 0)
		return -1;

	c2 = getxdigit(*(*(line) + 1));
	if (c2 < 0)
		return -1;

	res = (c1 * 16 + c2);
	(*line) += 2;

	return res;
}

/*  *********************************************************************
    *  procsrec(line,loadaddr,blklen,data)
    *
    *  Process an S-record, reading the data into a local buffer
    *  and returning the block's address.
    *
    *  Input parameters:
    *      line - line of text (s-record line)
    *      loadaddr - will be filled with address where data should go
    *      blklen - will be filled in with size of record
    *      data - points to buffer to receive data
    *
    *  Return value:
    *      <0 if error occured (not an s-record)
    ********************************************************************* */

static int procsrec(char *line,
		    unsigned int *loadaddr,
		    unsigned int *blklen, unsigned char *data)
{
	char rectype;
	unsigned char b;
	unsigned int len;
	unsigned int minlen;
	unsigned int linelen;
	unsigned int addr;
	unsigned int chksum;

	unsigned int idx;
	int ret = 0;

	addr = 0;

	if (*line++ != 'S')
		return -1;	/* not an S record */

	rectype = *line++;

	minlen = 3;		/* type 1 record */
	switch (rectype) {
	case '0':
		break;

		/*
		 * data bytes
		 */
	case '3':
		minlen++;
		/* fall through */
	case '2':
		minlen++;
		/* fall through */
	case '1':
		chksum = 0;
		linelen = getbyte(&line);
		if (linelen < minlen) {
			xprintf("srec: line too short\n");
			return -1;
		}
		chksum += (unsigned int)linelen;

		/*
		 * There are two address bytes in a type 1 record, and three
		 * in a type 2 record.  The high-order byte is first, then
		 * one or two lower-order bytes.  Build up the adddress.
		 */
		b = getbyte(&line);
		chksum += (unsigned int)b;
		addr = b;
		b = getbyte(&line);
		chksum += (unsigned int)b;
		addr <<= 8;
		addr += b;
		if (rectype == '2') {
			b = getbyte(&line);
			chksum += (unsigned int)b;
			addr <<= 8;
			addr += b;
		}
		if (rectype == '3') {
			b = getbyte(&line);
			chksum += (unsigned int)b;
			addr <<= 8;
			addr += b;
			b = getbyte(&line);
			chksum += (unsigned int)b;
			addr <<= 8;
			addr += b;
		}
#if defined(VERBOSE)
		printf("Addr: %08X Len: %3u(%#x)\n", addr, linelen - minlen,
		       linelen - minlen);
#endif

		*loadaddr = addr;
		len = linelen - minlen;
		*blklen = len;

		for (idx = 0; idx < len; idx++) {
			b = getbyte(&line);
			chksum += (unsigned int)b;
			data[idx] = (unsigned char)b;
		}

		b = getbyte(&line);
		chksum = (~chksum) & 0x000000FF;
		if (chksum != b) {
			xprintf("Checksum error in s-record file\n");
			return -1;
		}
		ret = 1;
		break;

	case '9':
		linelen = getbyte(&line);
		b = getbyte(&line);
		addr = b;
		b = getbyte(&line);
		addr <<= 8;
		addr += b;
		*loadaddr = addr;
		ret = -2;
		break;

	case '8':
		linelen = getbyte(&line);
		b = getbyte(&line);
		addr = b;
		b = getbyte(&line);
		addr <<= 8;
		addr += b;
		b = getbyte(&line);
		addr <<= 8;
		addr += b;
		*loadaddr = addr;
		ret = -2;
		break;

	case '7':
		linelen = getbyte(&line);
		b = getbyte(&line);
		addr = b;
		b = getbyte(&line);
		addr <<= 8;
		addr += b;
		b = getbyte(&line);
		addr <<= 8;
		addr += b;
		b = getbyte(&line);
		addr <<= 8;
		addr += b;
		*loadaddr = addr;
		ret = -2;
		break;

	default:
		xprintf("Unknown S-record type: %c\n", rectype);
		return -1;
		break;
	}

	return ret;
}

/*  *********************************************************************
    *  bolt_srecload(la)
    *
    *  Read an s-record file
    *
    *  Input parameters:
    *      la - loader args
    *
    *  Return value:
    *      0 if ok, else error code
    ********************************************************************* */
static int bolt_srecload(bolt_loadargs_t *la)
{
	int res;
	fileio_ctx_t *fsctx;
	void *ref;
	unsigned int loadaddr;
	unsigned int blklen;
	linebuf_t lb;
	char line[256];
	uint8_t data[256];
	int cnt;
	unsigned int specaddr; /* current addr - if loading to a special addr */
	int specflg; /* true if in "special address" mode */
	int firstrec = 1; /* true if we have not seen the first record */

	if (la->la_offs) {
		xprintf("Error: S-rec loader does not support seek to offset\n");
		return BOLT_ERR_UNSUPPORTED;
	}

	/* FIX error: 'loadaddr' may be used uninitialized in this function */
	loadaddr = (unsigned int)-1;

	/* FIX error: 'blklen' may be used uninitialized in this function */
	blklen = (unsigned int)-1;

	/*
	 * Figure out if we're loading to a "special address".  This lets
	 * us load S-records into a temporary buffer, ignoring the
	 * addresses in the records (but using them so we'll know where
	 * they go relative to each other
	 */

	specflg = (la->la_flags & LOADFLG_SPECADDR) ? 1 : 0;
	specaddr = 0;

	/*
	 * Create a file system context
	 */

	res = fs_init(la->la_filesys, &fsctx, la->la_device);
	if (res != 0)
		return res;

	/*
	 * Turn on compression if we're doing that.
	 */

	if (la->la_flags & LOADFLG_COMPRESSED) {
		res = fs_hook(fsctx, "z");
		if (res != 0)
			return res;
	}

	/*
	 * Open the boot device
	 */

	res = fs_open(fsctx, &ref, la->la_filename, FILE_MODE_READ);
	if (res != 0) {
		fs_uninit(fsctx);
		return res;
	}

	/*
	 * Okay, go load the boot file.  Process S-records until
	 * we get an entry point record (usually at the end).
	 *
	 * XXX what if it's *not* at the end?
	 */

	initlinebuf(&lb, ref, fsctx);

	cnt = 0;
	for (;;) {
		/*
		 * Read a line of text
		 */
		res = readline(&lb, line, sizeof(line));
		if (res < 0)
			break;	/* reached EOF */

		/*
		 * Process the S-record.  If at EOF, procsrec returns 0.
		 * Invalid s-records returns -1.
		 */

		if (line[0] == 0)
			continue;

		res = procsrec(line, &loadaddr, &blklen, data);

#if 0
		if (res == -2 || res >= 0)
			xprintf(".");
#endif
		if (res < 0)
			break;

		/*
		 * Handle "special address" mode.   All S-records will be
		 * loaded into a buffer passed by the caller to the loader.
		 * We use the addresses in the S-records to determine
		 * relative placement of the data, keying on the first
		 * S-record in the file.
		 */

		if ((res == 1) && (specflg)) {
			if (firstrec) {
				/* First S-record seen sets the base
				for all that follow */
				specaddr = loadaddr;
				firstrec = 0;
			}
			loadaddr =
			    la->la_address + (intptr_t) (loadaddr - specaddr);
		}

		cnt++;

		if (res == 1) {
			memcpy((uint8_t *) (intptr_t) (signed)loadaddr, data,
			       blklen);
		}
	}

	/*
	 * We're done with the file.
	 */

	fs_close(fsctx, ref);
	fs_uninit(fsctx);

	/*
	 * Show final stats.
	 * Not to be done for every s-record, since that can take a Long Time.
	 */
	xprintf("%04d lines\n", cnt);

	if (res == -2) {
		la->la_entrypt = (intptr_t) (signed)loadaddr;
		res = 0;
	}

	return res;
}
