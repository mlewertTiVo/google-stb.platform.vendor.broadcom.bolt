/***************************************************************************
 *     Copyright (c) 2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>
#include <errno.h>
#include <endian.h>
#ifndef NO_ZLIB
#include <zlib.h>
#endif
#include <ctype.h>
#include "../splash/splash-media.h"

/* For RHEL and older glibc's */
#ifndef htole32
#include <byteswap.h>
#include <arpa/inet.h>
#define htole32(x) (bswap_32(htonl(x)))
#define le32toh(x) (ntohl(bswap_32(x)))
#endif

#ifndef MAX
#define MAX(a, b)  ((a > b) ? a : b)
#endif

#define IS_ZB_SIGANTURE(p) \
	((p[0] == 'G' && p[1] == 'Z' && p[2] == 'B' && p[3] == 'R'))

struct zbhdr {
	uint32_t signature;
	uint32_t ucsize;
	uint32_t csize;
	uint32_t uc_crc;
};

struct outbuf {
	uint8_t *data;
	int hpos;
	int dpos;
};


static void usage(void)
{
	printf("Usage: splash_create_flash_file [option] | [option] <file>...\n"
		"Options: [hz], [bpto] <file>\n"
		"  -h         Help (this text)\n"
#ifndef NO_ZLIB
		"  -z         Compress output file\n"
#endif
		"  -b <file>  BMP image to be displayed during bootup.\n"
		"  -p <file>  PCM audio to be played during bootup\n"
		"  -t <file>  BMP image to be displayed during overtemp.\n"
		"  -f <file>  BMP image to be displayed when in fastboot mode.\n"
		"  -r <file>  BMP image to be displayed when boot failed.\n"
		"  -o <file>  Output filename\n");
	exit(0);
}


static void __die_if_null(void *p, const char *func, const int line)
{
	if (NULL != p)
		return;

	fprintf(stderr, "%s():%d NULL pointer!\n", func, line);
	exit(-EFAULT); /* Bad address (POSIX.1) */
}
#define die_if_null(x) __die_if_null(x, __func__, __LINE__)


static uint32_t getFileSize(FILE *fp)
{
	uint32_t fileSize = 0;
	fpos_t pos;

	fgetpos(fp, &pos);
	fseek(fp, 0L, SEEK_END);
	fileSize = ftell(fp);
	fsetpos(fp, &pos);
	return fileSize;
}


static void badopt(int o)
{
	if (strchr("bpto", optopt) && (o == '?'))
		fprintf(stderr, "missing argument for option '%c'\n",
				optopt);
	else if (isprint(optopt))
		fprintf(stderr, "bad option: '%c'\n",
				optopt);
	else
		fprintf(stderr, "unprintable or missing bad option: %x\n",
				optopt);
}


static void sanity_check(void)
{
	if (SPLASH_MEDIA_INFO_BYTES != sizeof(uint32_t)) {
		fprintf(stderr, "splashFile creation for %d",
			SPLASH_MEDIA_INFO_BYTES);
		fprintf(stderr, " media info bytes is not supported.\n");
		exit(-ENOEXEC); /* kinda */
	}
}


static void add_data(struct outbuf *b, uint8_t *p, unsigned int len)
{
	if (!p || (len < 1))
		return;

	b->data = realloc(b->data,
			MAX(b->hpos, b->dpos) + len);

	die_if_null(b->data);

	memcpy(&(b->data[b->dpos]), p, len);

	b->dpos += len;
}


void header_init(struct outbuf *b)
{
	uint8_t sig[4] = {'B', 'R', 'C', 'M'};

	b->hpos = 0;
	b->dpos = SPLASH_FILE_HDR_SIZE;

	b->data = calloc(1, b->dpos);
	die_if_null(b->data);

	memcpy(b->data, sig, sizeof(sig));
	b->hpos += 4;

	*(uint32_t *)&(b->data[b->hpos]) = htole32(SPLASH_FILE_VER);
	b->hpos += 4;

	*(uint32_t *)&(b->data[b->hpos]) = htole32(SPLASH_FILE_HDR_SIZE);
	b->hpos += 4;
}


void header_add_entry(struct outbuf *b, uint32_t datsize,
		uint32_t total, const uint8_t *sig)
{
	uint32_t *p = (uint32_t *)b->data;

	/* add an entry:
	 * signature
	 * size of data
	 * offset (start) of data into this buffer
	*/
	memcpy(&(b->data[b->hpos]), sig, 4);
	b->hpos += 4;

	*(uint32_t *)&(b->data[b->hpos]) = htole32(datsize);
	b->hpos += 4;

	*(uint32_t *)&(b->data[b->hpos]) = htole32(b->dpos);
	b->hpos += 4;

	/* update the total size
	*/
	total += le32toh(p[2]);
	p[2] = htole32(total);
}


void read_media(struct outbuf *b, char *fname, const uint8_t sig[4])
{
	FILE *fi;
	uint32_t fsize, padding, total;
	uint8_t *fbuf;
	size_t nread;

	if (NULL == fname)
		return;

	fi = fopen(fname, "rb");
	if (!fi) {
		fprintf(stderr, "can't open %s for reading\n", fname);
		exit(-EBADF);
	}

	fsize = getFileSize(fi);

	padding = (fsize % SPLASH_ALIGNMENT_WIDTH);
	if (padding)
		padding = SPLASH_ALIGNMENT_WIDTH - padding;

	total = fsize + padding;

	fbuf = calloc(1, total);
	if (!fbuf) {
		fprintf(stderr, "can't alloc memory for %s!\n", fname);
		fclose(fi);
		exit(-ENOMEM);
	}

	fseek(fi, 0, SEEK_SET);

	nread = fread(fbuf, sizeof(uint8_t), fsize, fi);
	fclose(fi);

	if (fsize != nread) {
		fprintf(stderr, "file size %d bytes != %zu bytes read\n",
			fsize, nread);
		free(fbuf);
		exit(-EIO);
	}

	header_add_entry(b, fsize, total, sig);

	add_data(b, fbuf, total);

	free(fbuf);
}


#ifndef NO_ZLIB
void check_zip(struct outbuf *b, char *fname)
{
	FILE *fi = NULL;
	uint8_t *fbuf = NULL, *ucbuff = NULL, *saved = NULL;
	uint32_t fsize;
	size_t nread;
	struct zbhdr *z;
	int rc = 0;
	size_t toread;
	uLongf ucsize;

	if (NULL == fname)
		return;

	fi = fopen(fname, "rb");
	if (!fi) {
		fprintf(stderr, "check zip: can't open file\n");
		rc = -EBADF;
		goto out;
	}

	fsize = getFileSize(fi);

	fbuf = calloc(1, fsize);
	if (!fbuf) {
		fprintf(stderr, "check zip: can't alloc file buffer\n");
		rc = -ENOMEM;
		goto out;
	}

	saved = fbuf;
	fseek(fi, 0, SEEK_SET);
	nread = fread(fbuf, sizeof(uint8_t), fsize, fi);

	if (fsize != nread) {
		fprintf(stderr, "check zip: %u bytes != %zu bytes read\n",
			fsize, nread);
		rc = -EIO;
		goto out;
	}

	if (!IS_ZB_SIGANTURE(fbuf)) {
		fprintf(stderr, "check zip: bad signature\n");
		rc = -EILSEQ;
		goto out;
	}

	toread = MAX(b->hpos, b->dpos);

	z = (struct zbhdr *)fbuf;
	if (z->ucsize != toread) {
		fprintf(stderr, "check zip: bad uc size\n");
		rc = -EILSEQ;
		goto out;
	}

	/* step over header to the raw compessed data */
	fbuf += sizeof(struct zbhdr);

	ucbuff = calloc(1, z->ucsize);
	if (!ucbuff) {
		fprintf(stderr, "check zip: can't alloc uc buffer\n");
		rc = -ENOMEM;
		goto out;
	}

	ucsize = (uLong)z->ucsize;
	rc = uncompress(ucbuff, &ucsize, fbuf, z->csize);
	if (rc != Z_OK) {
		fprintf(stderr, "uncompress failed\n");
		goto out;
	}

	if (memcmp(ucbuff, b->data, toread)) {
		fprintf(stderr, "check zip: bad uncompress\n");
		rc = -EILSEQ;
	}

out:
	if (ucbuff)
		free(ucbuff);
	if (saved)
		free(saved);
	if (fi)
		fclose(fi);
	if (rc)
		exit(rc);
}
#endif


void write_all(struct outbuf *b, char *fname, int zip)
{
	FILE *fo;
	size_t written, towrite;
#ifndef NO_ZLIB
	uLongf compsize;
	uLong uc_crc;
	uint8_t zbsig[4] = {'G', 'Z', 'B', 'R'};
	uint32_t zhdr[4];
	uint8_t *cb = NULL;
	int rc = 0;
#endif

	fo = fopen(fname, "wb");
	if (!fo) {
		fprintf(stderr, "can't open %s for write\n", fname);
		exit(-EBADF);
	}

	fseek(fo, 0, SEEK_SET);

	towrite = MAX(b->hpos, b->dpos);

	if (zip) {
#ifdef NO_ZLIB
		fprintf(stderr, "zlib is not supported!\n");
		exit(-ENOTSUP);
#else
		compsize = compressBound(towrite);
		cb = calloc(1, compsize);
		die_if_null(cb);

		rc = compress(cb, &compsize, b->data, towrite);
		if (rc != Z_OK) {
			fprintf(stderr, "compress failed! rc=%d\n", rc);
			exit(rc);
		}

		uc_crc = crc32(0L, Z_NULL, 0);
		uc_crc = crc32(uc_crc, b->data, towrite);

		/* make a simple compressed file header. */
		memcpy(&zhdr, zbsig, 4);
		zhdr[1] = htole32(towrite);  /* uncompressed size */
		zhdr[2] = htole32(compsize); /* compressed size */
		zhdr[3] = htole32(uc_crc); /* uncompressed crc */

		written = fwrite(zhdr, sizeof(zhdr), 1, fo);
		if (written != 1) {
			fprintf(stderr, "write zheader failed! rc=%d\n", rc);
			rc = -EIO;
		}

		written = fwrite(cb, 1, compsize, fo);
		if (written != compsize) {
			printf("write %lu bytes != %zu bytes written\n",
				compsize, written);
			rc = -EIO;
		}

		fclose(fo);
		free(cb);

		if (rc)
			exit(rc);

		check_zip(b, fname);

#endif
	} else {
		written = fwrite(b->data, 1, towrite, fo);
		fclose(fo);

		if (towrite != written) {
			printf("write %zd bytes != %zd bytes written\n",
				towrite, written);
			exit(-EIO);
		}
	}
}

int main(int argc, char *argv[])
{
	const uint8_t bsig[] = {'b', 'm', 'p', '0'};
	const uint8_t tsig[] = {'b', 'm', 'p', '1'};
	const uint8_t fsig[] = {'b', 'm', 'p', '2'};
	const uint8_t rsig[] = {'b', 'm', 'p', '3'};
	const uint8_t psig[] = {'p', 'c', 'm', '0'};
	char *ofile = NULL, *bfile = NULL, *pfile = NULL, *tfile = NULL, *ffile = NULL, *rfile = NULL;
	struct outbuf b;
	int c, infiles = 0, zipit = 0;

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	sanity_check();

	if (argc < 2)
		usage();

	opterr = 0;

#ifndef NO_ZLIB
	while ((c = getopt(argc, argv, "zhb:p:t:o:f:r:")) != -1)
#else
	while ((c = getopt(argc, argv, "hb:p:t:o:f:r:")) != -1)
#endif
		switch (c) {
		case 'b':
			bfile = optarg;
			infiles++;
			break;
		case 'p':
			pfile = optarg;
			infiles++;
			break;
		case 't':
			tfile = optarg;
			infiles++;
			break;
		case 'o':
			ofile = optarg;
			break;
#ifndef NO_ZLIB
		case 'z':
			zipit = 1;
			break;
#endif
		case 'h':
			usage();
			break;
		case 'f':
			ffile = optarg;
			infiles++;
			break;
		case 'r':
			rfile = optarg;
			infiles++;
			break;
		case '?':
		default:
			badopt(c);
			return -1;
		}

	if (!infiles)
		fprintf(stderr, "warning: no input files!\n");

	if (!ofile) {
		fprintf(stderr, "error: no output file!\n");
		exit(-ENOENT); /* no such file */
	}

	header_init(&b);

	read_media(&b, bfile, bsig);
	read_media(&b, pfile, psig);
	read_media(&b, tfile, tsig);
	read_media(&b, ffile, fsig);
	read_media(&b, rfile, rsig);

	write_all(&b, ofile, zipit);

	if (b.data)
		free(b.data);

	return 0;
}
