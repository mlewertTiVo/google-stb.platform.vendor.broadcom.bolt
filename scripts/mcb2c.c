/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

static const char *COPYRIGHT_HEADER =
"/***************************************************************************\n"
" * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.\n"
" *\n"
" *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE\n"
" *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR\n"
" *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.\n"
" *\n"
" ***************************************************************************/\n\n"
"#include <fsbl-common.h>\n\n";

/* indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <libgen.h>

/* 7445Dx_933MHz_32b_dev4Gx8_DDR3-1866M_le.mcb
 *  => "7445dx", "933mhz", "32b", "dev4gx8", "ddr3-1866m", "le"
 */
static const int NUM_PARAMS_FROM_MCB_FILENAME = 6; /* 6 parameters */

int g_debug = 0;
int g_swap = 0;
int g_compress = 0;
int g_aliastemplate = 0;
int g_fixed_count = 0;
int g_fixed_main_table_top = 0;

#define DEBUG if(g_debug)
#define WSWAP(x) endian_swap32(x)

#define TEMPLATE_NAME "memsys_config_template"
#define MAX_FIXED_MCBS 1024 /* assume that 1024 is a bug number */
#define BUFSIZE_CHIP_ID 8 /* enough even for 5 digit product ID with '\0' */
#define FAMILY_CHIP_ID_COMMON "common"

struct mcb_properties {
	uint32_t ddr_clock; /* in MHz */
	uint32_t ddr_size; /* in Mbits */
	uint32_t ddr_width;
	uint32_t phy_width;
	char tag[PATH_MAX];
};

static int filter_reg(const struct dirent *de);
static int filter_dir(const struct dirent *de);
static char **extract_fnames(int ndentries, struct dirent **dentries,
	const char *chip_id);
static void free_fnames(int nentries, char **fnames);
static void free_dentries(int ndentries, struct dirent **dentries);
char *strdup_toupper(char *str);
static bool is_mcb_duplicate(struct mcb_properties *mcb,
	struct mcb_properties *mcbtable, unsigned int nentries);
static int parse_mcbfname(char *fname, struct mcb_properties *mcb_prop);
static int print_base_mcb(FILE *fp, int mcb_nentries, uint32_t *mcb,
	char *fname);
static int process_mcbfile(FILE *fp, char *fname,
	uint32_t *base_mcb, int mcb_fsize);
static int print_mcb(FILE *fp, int mcb_nentries,
	uint32_t *mcb, uint32_t *base_mcb, char *fname);
static int print_mcbtable(FILE *fp, int nfiles, char **mcbfiles);
static int print_mcbsubtable(FILE *fp, int nfiles, char **mcbfiles);
static char *find_matching_mcbfile(const char *pattern_mcb, const char *dir_mcb,
	const char *base_dir);
/* ------------------------------------------------------------------------- */

uint32_t endian_swap32(uint32_t x)
{
	if (!g_swap)
		return x;

	return ((x & 0xff000000) >> 24) |
	    ((x & 0x00ff0000) >> 8) |
	    ((x & 0x0000ff00) << 8) | ((x & 0x000000ff) << 24);
}

off_t filesize(char *fname)
{
	struct stat file_status;
	int rc;
	rc = stat(fname, &file_status);
	if (rc != 0) {
		int saved_errno = errno;
		fprintf(stderr, " could not stat %s\n", fname);
		exit(saved_errno);
	}
	return file_status.st_size;
}

char *remove_ext(char *sin, char cext, char csep)
{
	char *sout, *ext, *pathsep;
	int len;

	if (sin == NULL) {
		fprintf(stderr, "null string!\n");
		exit(-EINVAL);
	}

	len = strlen(sin) + 1;	/* \0 char */

	sout = malloc(len);
	if (!sout) {
		fprintf(stderr, "malloc fail!\n");
		exit(-ENOMEM);
	}

	strcpy(sout, sin);

	ext = strrchr(sout, cext);

	if (!csep)
		pathsep = NULL;
	else
		pathsep = strrchr(sout, csep);

	if (!ext)
		return sout;

	if (pathsep) {
		*ext = '\0';
		return sout;
	}

	if (pathsep < ext)
		*ext = '\0';

	return sout;
}

/* ------------------------------------------------------------------------- */

static int read_array(int f, uint32_t * parray, int size)
{
	int i, rc, reportage = 8;
	uint32_t tmp;

	for (i = 0; i < size; i++) {

		rc = read(f, &tmp, sizeof(tmp));

		parray[i] = WSWAP(tmp);

		if (rc != sizeof(uint32_t))
			return -EIO;

		DEBUG {
			if (i < reportage)
				fprintf(stderr, "0x%08x ", parray[i]);
			fflush(stderr);
		}
	}

	return 0;
}

/* ------------------------------------------------------------------------- */

static void cleanup_on_error(FILE *fp, const char *out_fname)
{
	if (fp != stdout) {
		fclose(fp);
		unlink(out_fname);
	}
}

void usage(char *s) {
	printf("Usage: %s [-a] [-c] [-d] "
		"[-{f|u} fixed-memsys [-{f|u} fixed-memsys ...]] "
		"{-m mcb_dir} "
		"{-i chip_id} "
		"[-o mcb.c] [-s]\n", s);
	puts("  -a\talias template name, and use real mcb as array name");
	puts("  -c\tcompress mcb (diff of the first mcb in files to the rest)");
	puts("  -d\tprint debug progress");
	puts("  -f mcb_main\tMCB pattern to be added at fixed location");
	puts("  -i chip_id\tfamily chip ID");
	puts("  -m mcb_dir\tdirectory where MCB files are");
	puts("  -o mcb.c\tbase file where output is saved to");
	puts("  -s\twordswap mcb data (ONLY)");
	puts("  -u mcb_sub\tcustom MCB pattern to be added at fixed location");
	putchar('\n');
}

/* ------------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
	int c, i;
	int errflg = 0;
	int aflg = 0, cflg = 0, dflg = 0, fflg = 0, iflg = 0, mflg = 0,
		oflg = 0, sflg = 0, uflg = 0;
	char *fixed_mcbmain[MAX_FIXED_MCBS];
	char *fixed_mcbsub[MAX_FIXED_MCBS];
	char *fname_fixed_mcbmain[MAX_FIXED_MCBS];
	char *fname_fixed_mcbsub[MAX_FIXED_MCBS];
	char *dir_mcb;
	char *chip_id;
	char base_dir[PATH_MAX];
	char *out_mcb = NULL;
	FILE *fp;
	int ndentries; /* # directory entries */
	struct dirent **dentries;
	char **mcbfiles;
	char *base_mcb_fname;
	int base_mcb_fd;
	int mcb_fsize;
	int mcb_nentries;
	uint32_t *base_mcb;
	uint32_t *mcb;
	int saved_errno;

	i = 0;
	while ((c = getopt(argc, argv, "acdf:i:m:o:su:")) != -1) {
		switch (c) {
		case 'a':
			g_aliastemplate = 1;
			if (aflg)
				++errflg;
			++aflg;
			break;
		case 'c':
			g_compress = 1;
			if (cflg)
				++errflg;
			++cflg;
			break;
		case 'd':
			g_debug = 1;
			if (dflg)
				++errflg;
			++dflg;
			break;
		case 'f':
			if ((fflg+uflg) >= MAX_FIXED_MCBS) {
				++errflg;
			} else {
				fixed_mcbmain[fflg] = optarg;
				++fflg;
			}
			break;
		case 'i':
			chip_id = optarg;
			if (iflg)
				++errflg;
			++iflg;
			break;
		case 'm':
			dir_mcb = optarg;
			if (mflg)
				++errflg;
			++mflg;
			break;
		case 'o':
			out_mcb = optarg;
			if (oflg)
				++errflg;
			++oflg;
			break;
		case 's':
			g_swap = 1;
			if (sflg)
				++errflg;
			++sflg;
			break;
		case 'u':
			if ((fflg+uflg) >= MAX_FIXED_MCBS) {
				++errflg;
			} else {
				fixed_mcbsub[uflg] = optarg;
				++uflg;
			}
			break;
		case '?':
		default:
			++errflg;
		}
	}

	if (errflg || mflg == 0 || iflg == 0) {
		usage(argv[0]);
		return EINVAL;
	}

	if (NULL == getcwd(base_dir, sizeof(base_dir))) {
		saved_errno = errno;
		fprintf(stderr, "Cannot get the current working dir\n");
		return saved_errno;
	}

	if (out_mcb == NULL) {
		fp = stdout;
	} else {
		fp = fopen(out_mcb, "w+");
		if (fp == NULL) {
			saved_errno = errno;
			fprintf(stderr, "Cannot open %s\n", out_mcb);
			usage(argv[0]);
			return saved_errno;
		}
	}

	/* find file names that match MCB patterns for 'fixed'
	 *   fixed_mcbmain : 7445d0_933mhz_32b_dev4gx16
	 *   fixed_mcbsub : 7445d0/custom_933mhz_32b_dev4gx16
	 */
	memset(fname_fixed_mcbmain, 0, sizeof(fname_fixed_mcbmain));
	memset(fname_fixed_mcbsub, 0, sizeof(fname_fixed_mcbsub));
	for (i=0; i<fflg; ++i) {
		fname_fixed_mcbmain[i] = find_matching_mcbfile(
			fixed_mcbmain[i], dir_mcb, base_dir);
	}
	for (i=0; i<uflg; ++i) {
		fname_fixed_mcbsub[i] = find_matching_mcbfile(
			fixed_mcbsub[i], dir_mcb, base_dir);
	}
	if (fflg || uflg) {
		/* the first MCB is the base if compressed */
		if (fflg) {
			base_mcb_fname = fname_fixed_mcbmain[0];
		} else {
			base_mcb_fname = fname_fixed_mcbsub[0];
		}

		if (base_mcb_fname == NULL) {
			cleanup_on_error(fp, out_mcb);
			fprintf(stderr, "No valid MCB file for %s\n",
				fflg ? fixed_mcbmain[0] : fixed_mcbsub[0]);
			usage(argv[0]);
			return ENOENT;
		}
	} else {
		/* change directory to where MCB files are,
		 * and read the directory lexically
		 */
		if (chdir(dir_mcb) < 0) {
			saved_errno = errno;
			cleanup_on_error(fp, out_mcb);
			fprintf(stderr, "Cannot change directory to %s\n",
				dir_mcb);
			usage(argv[0]);
			return saved_errno;
		}

		ndentries = scandir(".", &dentries, filter_reg, alphasort);
		if (ndentries <= 0) {
			cleanup_on_error(fp, out_mcb);
			fprintf(stderr, "No valid MCB file in %s\n",
				dir_mcb);
			usage(argv[0]);
			return ENOENT;
		}

		mcbfiles = extract_fnames(ndentries, dentries, chip_id);
		if (mcbfiles == NULL) {
			cleanup_on_error(fp, out_mcb);
			fprintf(stderr, " not enough memory\n");
			return ENOMEM;
		}
		free_dentries(ndentries, dentries);

		/* the first MCB is the base if compressed */
		base_mcb_fname = mcbfiles[0];
		if (base_mcb_fname == NULL) {
			cleanup_on_error(fp, out_mcb);
			fprintf(stderr, "no valid MCB files\n");
			return EINVAL;
		}
	}

	base_mcb_fd = open(base_mcb_fname, O_RDONLY);
	if (base_mcb_fd < 0) {
		saved_errno = errno;
		cleanup_on_error(fp, out_mcb);
		fprintf(stderr, " fail to open %s\n", base_mcb_fname);
		return saved_errno;
	}

	mcb_fsize = filesize(base_mcb_fname);
	if ((mcb_fsize == 0) || (mcb_fsize % sizeof(uint32_t))) {
		cleanup_on_error(fp, out_mcb);
		fprintf(stderr, " bad size! %s - %d\n",
			base_mcb_fname, mcb_fsize);
		return EINVAL;
	}

	mcb_nentries = mcb_fsize / sizeof(uint32_t);

	base_mcb = malloc(mcb_fsize);
	if (base_mcb == NULL) {
		cleanup_on_error(fp, out_mcb);
		fprintf(stderr, " not enough memory for %d bytes\n",
			mcb_fsize);
		return ENOMEM;
	}

	read_array(base_mcb_fd, base_mcb, mcb_nentries);
	close(base_mcb_fd);
	/* 'base_mcb_fname' contains directory if 'fixed' */
	print_base_mcb(fp, mcb_nentries, base_mcb, basename(base_mcb_fname));

	if (g_compress) {
		mcb = malloc(mcb_fsize);
		if (mcb == NULL) {
			cleanup_on_error(fp, out_mcb);
			fprintf(stderr,
				" not enough memory for %d bytes\n", mcb_fsize);
			return ENOMEM;
		}
	}

	/* For "compressed" mode, it is an MCB versus the base MCB.
	 * An MCB should be expressed in the form of the difference from
	 * the base MCB and listed in the MCB table. For "non-compressed"
	 * mode, every MCB including the base MCB is treated equal.
	 * It can also be viewed as if there is no base MCB.
	 */
	if (g_compress)
		i = 0; /* base versus MCBs, it need be repeated */
	else
		i = 1; /* base becomes one of MCBs, no need for repetition */

	if (fflg == 0 && uflg == 0) {
		for ( ; i<ndentries; ++i) {
			if (mcbfiles[i] == NULL)
				break; /* no more 'chip_id' matching MCB */

			process_mcbfile(fp, mcbfiles[i], base_mcb, mcb_fsize);
		}
		print_mcbtable(fp, ndentries, mcbfiles);
		free_fnames(ndentries, mcbfiles);
	} else {
		if (fflg == 0) {
			/* fixed and custom only */
			for ( ; i<uflg; ++i)
				process_mcbfile(fp, fname_fixed_mcbsub[i],
					base_mcb, mcb_fsize);
		} else {
			/* fixed only, and might be fixed and custom */
			for ( ; i<fflg; ++i)
				process_mcbfile(fp, fname_fixed_mcbmain[i],
					base_mcb, mcb_fsize);
			for (i=0; i<uflg; ++i)
				process_mcbfile(fp, fname_fixed_mcbsub[i],
					base_mcb, mcb_fsize);
		}
		print_mcbtable(fp, fflg, fname_fixed_mcbmain);
		print_mcbsubtable(fp, uflg, fname_fixed_mcbsub);
	}


	if (fflg || uflg) {
		if (fp != stdout)
			fclose(fp);
		for (i=0; i<MAX_FIXED_MCBS; ++i) {
			if (fname_fixed_mcbmain[i] != NULL)
				free(fname_fixed_mcbmain[i]);
			if (fname_fixed_mcbsub[i] != NULL)
				free(fname_fixed_mcbsub[i]);
		}
		return 0;
	}

	/* scan directories to see if there is any custom MCB */
	ndentries = scandir(".", &dentries, filter_dir, alphasort);
	if (ndentries <= 0) {
		if (fp != stdout)
			fclose(fp);
		return 0;
	}
	mcbfiles = extract_fnames(ndentries, dentries, chip_id);
	if (mcbfiles == NULL) {
		cleanup_on_error(fp, out_mcb);
		fprintf(stderr, " not enough memory\n");
		return ENOMEM;
	}
	free_dentries(ndentries, dentries);

	for (i=0; i<ndentries; ++i) {
		int nsubentries;
		struct dirent **subentries;
		char **submcbfiles;
		int j;
		char *p, out_submcb[PATH_MAX];

		if (mcbfiles[i] == NULL)
			break;

		DEBUG fprintf(stderr, "scanning %s/...\n", mcbfiles[i]);

		nsubentries = scandir(mcbfiles[i], &subentries,
			filter_reg, alphasort);
		if (nsubentries <= 0)
			continue;

		submcbfiles = extract_fnames(nsubentries, subentries, NULL);
		if (submcbfiles == NULL) {
			fprintf(stderr, " not enough memory\n");
			return ENOMEM;
		}
		free_dentries(nsubentries, subentries);

		if (out_mcb != NULL) {
			p = remove_ext(out_mcb, '.', '/');
			if (p == NULL) {
				fprintf(stderr,
					"cannot remove extension from %s\n",
					out_mcb);
				return EINVAL;
			}
			sprintf(out_submcb, "%s/%s-%s.c",
				base_dir, p, mcbfiles[i]);
		}
		fprintf(fp, "\n\n/* under %s/... */\n\n", mcbfiles[i]);

		chdir(mcbfiles[i]);
		for (j=0; j<nsubentries; ++j)
			process_mcbfile(fp, submcbfiles[j],
				base_mcb, mcb_fsize);

		print_mcbsubtable(fp, nsubentries, submcbfiles);
		chdir("..");
		free_fnames(nsubentries, submcbfiles);
	}
	if (fp != stdout)
		fclose(fp);

	if (chdir(base_dir) < 0) {
		saved_errno = errno;
		fprintf(stderr, "Cannot change directory to %s\n",
			base_dir);
		return saved_errno;
	}

	return 0;
}

/* filter_reg -- filters regular files, and follows symbolic links
 */
static int filter_reg(const struct dirent *de)
{
	if (de->d_type == DT_REG || de->d_type == DT_LNK)
		return 1;

	return 0;
}

/* filter_dir -- filters only directory files except . and ..
 */
static int filter_dir(const struct dirent *de)
{
	if (de->d_type != DT_DIR)
		return 0;

	if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
		return 0;

	return 1;
}

/* does_chip_id_match_filename -- checks if chip ID matches file name
 *
 *  The check is done only up to the chip ID portion of the file name.
 *
 * Parameter:
 *  fname   [in] file name to be compared against chip_id
 *  chip_id [in] chip ID to compare
 * Returns:
 *  0 if does not match
 *  1 if chip_id is NULL
 *  the number of characters matching in file name otherwise
 */
static int does_chip_id_match_filename(const char *fname, const char *chip_id)
{
	/* 7445d0 == 7445dx
	 * 7445d1 == 7445dx
	 * 7445d0 == 7445d0
	 * 7445d1 != 7445d0
	 * 7271A0 == 7271A_
	 */
	size_t len;
	char last_char;

	if (chip_id == NULL)
		return 1;

	len = strlen(chip_id);
	if (strlen(fname) < len)
		return 0;
	if (strncasecmp(chip_id, fname, len-1))
		return 0;

	last_char = toupper(fname[len-1]);
	if (last_char == '_')
		return len - 1;
	if (last_char == 'X')
		return len - 1;

	if (last_char != chip_id[len-1])
		return 0;

	return len;
}

/* extract_fnames -- re-constructs list of files from "struct dirent *"
 *
 *  An array of string (char *) is allocated, and each file name of
 * the given "struct dirent *" is duplicated to the array.
 *
 * Parameter:
 *  ndentries  [in] # entries in struct dirent *
 *  dentries   [in] pointer to scanned directory
 *  chip_id    [in] pointer to chip ID string
 * Returns:
 *  pointer to pointers of file names on success, NULL otherwise
 */
static char **extract_fnames(int ndentries, struct dirent **dentries,
	const char *chip_id)
{
	int i;
	char **p, **fnames;

	fnames = malloc(ndentries*sizeof(char*));
	if (fnames == NULL)
		return NULL;
	memset(fnames, 0, ndentries*sizeof(char*));

	p = fnames;
	for (i=0; i<ndentries; ++i) {
		if (!does_chip_id_match_filename(dentries[i]->d_name, chip_id))
			continue;

		*p = strdup(dentries[i]->d_name);
		if (*p == NULL) {
			int j;
			for (j=0; j<i; ++j)
				free(fnames[j]);
			free(fnames);
			return NULL;
		}
		p++;
	}

	return fnames;
}

static void free_fnames(int nentries, char **fnames)
{
	int i;

	for (i=0; i<nentries; ++i) {
		if (fnames[i] == NULL)
			break; /* no more 'chip_id' matching MCB */
		free(fnames[i]);
	}
	free(fnames);
}

static void free_dentries(int ndentries, struct dirent **dentries)
{
	int i;

	for (i=0; i<ndentries; ++i)
		free(dentries[i]);
	free(dentries);
}

/* strdup_toupper -- clone a string and then upper-case it.
 *
 * The same length string is allocated, and the source string
 * is copied to the newly allocated string then all the contents
 * are converted into upper case.
 *
 * Parameter:
 *  str          [in] input string to be converted into upper case
 * Returns:
 *  pointer to a new uppercased string on success, NULL otherwise
 */
char *strdup_toupper(char *str)
{
	char *p, *uc;

	p = uc = strdup(str);
	if (NULL == p) {
		fprintf(stderr, "strdup alloc fail!\n");
		return NULL;
	}

	while ('\0' != *p) {
		*p = toupper(*p);
		p++;
	}

	return uc;
}

static char *cute_size(char *buf, uint32_t mega)
{
	if (buf == NULL)
		return NULL;

	if (mega < 1024) {
		sprintf(buf, "%dM", mega);
		return buf;
	}
	mega /= 1024; /* Giga */
	if (mega < 1024) {
		sprintf(buf, "%dG", mega);
		return buf;
	}
	mega /= 1024; /* Tera */
	if (mega < 1024) {
		sprintf(buf, "%dT", mega);
		return buf;
	}
	mega /= 1024; /* Peta */
	/* no more possibility due to 32bit precision */
	sprintf(buf, "%dP", mega);
	return buf;
}

static int normalize_fname(char *sname, char *fname)
{
	int nfields;
	struct mcb_properties mcb_prop;
	char buf[4];

	if (sname == NULL)
		return -1;

	nfields = parse_mcbfname(fname, &mcb_prop);
	if (nfields != NUM_PARAMS_FROM_MCB_FILENAME) {
		fprintf(stderr, "%s splits into %d pieces\n",
			fname, nfields);
		return -1;;
	}

	if (isdigit(fname[0]) || !strcmp(FAMILY_CHIP_ID_COMMON, mcb_prop.tag))
		sprintf(sname, "mcb_%dmhz_%db_dev%sx%d", 
			mcb_prop.ddr_clock, mcb_prop.phy_width,
			cute_size(buf, mcb_prop.ddr_size), mcb_prop.ddr_width);
	else
		sprintf(sname, "%s_%dmhz_%db_dev%sx%d", 
			mcb_prop.tag, mcb_prop.ddr_clock, mcb_prop.phy_width,
			cute_size(buf, mcb_prop.ddr_size), mcb_prop.ddr_width);

	return 0;
}

/* print_base_mcb -- prints out the base MCB in C format
 *
 * All the contents of the base MCB is printed out.
 * Only the name(identifier) for the number of entries changes depending
 * on whether "compress" is selected.
 *
 * Parameters:
 *  fp           [in] file pointer that the base MCB will be written to
 *  mcb_nentries [in] # entries in base MCB
 *  mcb          [in] base MCB
 *  fname        [in] file name from where MCB was extracted
 * Returns:
 *  0 on success, -1 otherwise
 */
static int print_base_mcb(FILE *fp, int mcb_nentries, uint32_t *mcb,
	char *fname)
{
	int i;
	char fname_base[PATH_MAX], *tname = TEMPLATE_NAME;
	char *fname_upper;

	if (fp == NULL || mcb_nentries == 0 || mcb == NULL || fname == NULL)
		return -1;

	fputs(COPYRIGHT_HEADER, fp);
	fprintf(fp, "#define SHMOO_ARRAY_SIZE\t0x%08x\n",
	      	(unsigned int) (mcb_nentries * sizeof(*mcb)));
	fprintf(fp, "#define SHMOO_ARRAY_ELEMENTS\t0x%08x\n\n", mcb_nentries);

	normalize_fname(fname_base, fname);

	if (!g_compress) {
		fname_upper = strdup_toupper(fname_base);
		if (fname_upper == NULL)
			return -1;
	}

	fprintf(fp, "/* %s\n*/\n", fname);

	if (g_aliastemplate) {
		tname = fname_base;
		fprintf(fp, "#define %s %s", TEMPLATE_NAME, tname);
		if (!g_compress)
			fprintf(fp, "_%d", g_fixed_count);
		fprintf(fp, "\n\n");
	}

	if (g_compress) {
		fprintf(fp, "static const uint32_t %s[SHMOO_ARRAY_ELEMENTS] "
			"__attribute__ ((section(\".mcbdata\"))) = {\n", tname);
	} else {
		fprintf(fp, "static const uint32_t %s_%d[] __attribute__ "
			"((section(\".mcbdata\"))) = {\n",
			fname_base, g_fixed_count);
		g_fixed_count++;
	}

	DEBUG fprintf(stderr, "reference MCB: %s\n", fname);

	for (i = 0; i < mcb_nentries; i++) {
		fprintf(fp, "\t0x%08x,\t/* %d */\n", mcb[i], i);
	}
	fprintf(fp, "};\n");

	if (g_compress) {
		putc('\n', fp);
	} else {
		fprintf(fp, "#define %s_ENTRIES %d\n\n",
			fname_upper, mcb_nentries);
		free(fname_upper);
	}

	return 0;
}

/* print_mcb -- dumps out the whole MCB or differences from the base only
 *
 * If "compress" is selected, only the differences from the base MCB
 * is printed. Otherwise, the whole content will be.
 *
 * Parameters:
 *  fp           [in] file pointer that the base MCB will be written to
 *  mcb_nentries [in] # entries in MCB
 *  mcb          [in] MCB to be printed out
 *  base_mcb     [in] base MCB
 *  fname        [in] file name from where MCB was extracted
 * Returns:
 *  0 on success, -1 otherwise
 */
static int print_mcb(FILE *fp, int mcb_nentries,
	uint32_t *mcb, uint32_t *base_mcb, char *fname)
{
	int i;
	char fname_base[PATH_MAX], *fname_upper;
	int differences;
	uint32_t checksum;

	if (fp == NULL || mcb_nentries == 0 || mcb == NULL || fname == NULL)
		return -1;

	normalize_fname(fname_base, fname);

	if (!g_compress) {
		fname_upper = strdup_toupper(fname_base);
		if (fname_upper == NULL)
			return -1;
	}

	DEBUG fprintf(stderr, "  delta MCB: %s\n", fname);

	fprintf(fp, "/* %s\n*/\n", fname);

	fprintf(fp, "static const uint32_t %s", fname_base);
	if (!g_compress)
		fprintf(fp, "_%d", g_fixed_count);
	fprintf(fp, "[] __attribute__ ((section(\".mcbdata\"))) = {\n");

	differences = 0;
	checksum = 0;

	if (g_compress) {
		for (i = 0; i < mcb_nentries; i++) {	/* go thru the current mcb and diff against the reference */
			if (base_mcb[i] != mcb[i]) {
				differences++;
				fprintf(fp, "\t0x%08x, ", i);
				fprintf(fp, " 0x%08x,\t/* %d */\n",
					      mcb[i], i);
			}
			checksum = (checksum + mcb[i]) & 0xffffffff;
		}

		fprintf(fp, "\t0x%08x, ", (uint32_t) -1);
		fprintf(fp, " 0x%08x,\t/* %d */\n", checksum, (uint32_t) -1);
		fprintf(fp, "}; /* %d difference%s */\n\n",
			      differences, (differences > 1) ? "s" : "");
	} else {
		for (i = 0; i < mcb_nentries; i++)	/* dump out the current mcb */
			fprintf(fp, "\t0x%08x,\t/* %d */\n", mcb[i], i);
		fprintf(fp, "};\n");
		fprintf(fp, "#define %s_ENTRIES %d\n\n",
			fname_upper, mcb_nentries);
		free(fname_upper);
		g_fixed_count++;
	}

	return 0;
}

/* process_mcbfile -- reads an MCB file and prints out its contents
 *
 * If "compress" is selected, only the differences from the base MCB
 * is printed. Otherwise, the whole content will be.
 *
 * Parameters:
 *  fp           [in] file pointer that results will be written to
 *  fname        [in] file name from where MCB will be extracted
 *  base_mcb     [in] base MCB
 *  mcb_fsize    [in] the size of the base MCB file
 * Returns:
 *  #entries processed on success, 0 (zero) otherwise
 */
static int process_mcbfile(FILE *fp, char *fname,
	uint32_t *base_mcb, int mcb_fsize)
{
	int fsize = filesize(fname);
	int mcb_nentries;
	int fd;
	uint32_t *mcb;

	if (fp == NULL || fname == NULL)
		return 0;

	if (g_compress) {
		if (fsize != mcb_fsize) {
			fprintf(stderr, " all mcb must be the "
				"same size! %s - %d != %d\n",
				fname, fsize, mcb_fsize);
			return 0;
		}
	} else {
		if ((fsize == 0) || (fsize % sizeof(uint32_t))) {
			fprintf(stderr, " bad size! %s - %d\n", fname, fsize);
			return 0;
		}

	}

	mcb_nentries = fsize / sizeof(uint32_t);
	mcb = malloc(fsize);
	if (mcb == NULL) {
		fprintf(stderr, " not enough memory for %d bytes\n", fsize);
		return 0;
	}

	fd = open(fname, O_RDONLY);
	if (fd < 0) {
		free(mcb);
		fprintf(stderr, " fail to open %s\n", fname);
		return 0;
	}
	read_array(fd, mcb, mcb_nentries);
	close(fd);
	/* dump or print out differences from the base MCB */
	print_mcb(fp, mcb_nentries, mcb, base_mcb,
		basename(fname)); /* 'fname' contains directory if 'fixed' */

	free(mcb);
	return mcb_nentries;
}

/* decompose_fname -- decomposes file name
 *
 *   '_' (underscore) is the separator of the input file name.
 *   Decomposed elements are converted into lower case and placed in
 *   an allocated buffer with '\0' separated. Decomposing stops once
 *   '.' (period) is found.
 *
 *   For example, "7445Cx_933MHz_32b_dev4Gx8_DDR3-1866M_le.mcb" becomes
 *   7445dx \0 933mhz \0 32b \0 dev4gx8 \0 ddr3-1866m \0 le \0\0
 *
 * Parameter:
 *  fname [in] file name to be decomposed
 * Returns:
 *  an allocated buffer containing decomposed elements, NULL on failure
 */
static char *decompose_fname(const char *fname)
{
	char *p, *tmp;
	size_t fname_size;

	if (fname == NULL) return NULL;

	/* copy 'fname' to an allocated buffer */
	fname_size = strlen(fname);
	tmp = malloc(fname_size+2); /* terminating null, and terminating null */
	if (tmp == NULL)
		return 0;
	memcpy(tmp, fname, fname_size);
	tmp[fname_size] = '\0';
	tmp[fname_size+1] = '\0'; /* terminating null for list of elements */

	p = tmp;
	while (*p != '\0') {
		switch (*p) {
		case '_':
			*p = '\0';
			break;
		case '.':
			*p = '\0';
			*(p+1) = '\0'; /* terminate decomposing */
			break;
		default:
			*p = tolower(*p);
			break;
		}
		++p;
	}
	/* "7445dx", "933mhz", "32b", "dev4gx8", "ddr3-1866m", "le", "" */
	return tmp;
}

/* count_fields -- counts #fields(elements) of the given list of strings
 *
 *   A file name is decomposed into a list of strings terminating with
 * an empty string. Each string corresponds to a field/element for
 * further interpretatino or parsing. Counts the number of strings
 * from the given list.
 *
 *   For example, "7445Cx_933MHz_32b_dev4Gx8_DDR3-1866M_le.mcb" becomes
 *   7445dx \0 933mhz \0 32b \0 dev4gx8 \0 ddr3-1866m \0 le \0\0
 *   and then, counts as 6 (six).
 *
 * Parameter:
 *  list_fields [in] list of strings terminating with empty string
 * Returns:
 *  #fields/elements/strings in the list
 */
static int count_fields(const char *list_fields)
{
	int n;
	const char *p;

	if (list_fields == NULL) return 0;

	n = 0;
	p = list_fields;
	while (*p != '\0') {
		++n;
		p += strlen(p) + 1;
	}

	return n;
}

/* parse_dramparts -- parses string description on DRAM part dimension
 *
 *   [dev]{size}Gx{width}
 *     "dev" is optional
 *     "size" is decimal
 *     "width" is decimal
 *
 * Parameters:
 *  p        [in]  file name to be decomposed
 *  mcb_prop [out] filled with DDR size and width
 * Returns:
 *  0 on success, -1 otherwise
 */
static int parse_dramparts(char *p, struct mcb_properties *mcb_prop)
{
	char buf[PATH_MAX];

	/* p should be pointing to "dev4gx8" */
	if (p == NULL || mcb_prop == NULL) return -1;

	if (!strncmp(p, "dev", 3))
		p += 3;

	mcb_prop->ddr_size = atoi(p);
	if (mcb_prop->ddr_size == 0)
		return -1;

	sprintf(buf, "%d", mcb_prop->ddr_size);
	p += strlen(buf);

	/* tera (tx), giga (gx) or mega (mx) bits */
	if (!strncmp(p, "tx", 2))
		mcb_prop->ddr_size *= 1024*1024; /* from Tbits to Mbits */
	else if (!strncmp(p, "gx", 2)) {
		if (mcb_prop->ddr_size >= 1024)
			return -1; /* use "tx", not "gx" */
		mcb_prop->ddr_size *= 1024; /* from Gbits to Mbits */
	} else if (!strncmp(p, "mx", 2)) {
		if (mcb_prop->ddr_size >= 1024)
			return -1; /* use "gx", not "mx" */
	} else
		return -1;
	p += 2; /* skip "tx", "gx" or "mx" */

	mcb_prop->ddr_width = atoi(p);
	if (mcb_prop->ddr_width == 0)
		return -1;

	return 0;
}

/* is_mcb_duplicate -- checks if the identical property of an MCB has
 *   already been processed
 *
 *  Historically, an MCB file name contained the minor revision number
 * of a chip family. It is now omitted by default. To support MCB files
 * both with and without a minor revision number, the minor revision
 * number is ignored in terms of converting MCB's into C data structure.
 * Supporting both conventions dramatically increases the chance of
 * having multiple MCB with the same properties as such multiplicity
 * is not detected/prevented by a file system.
 *
 *  Whenever a new MCB is processed, its PHY width, device size, device
 * width and speed will be checked against already processed MCB's. If
 * a match is found, it should be treated as an error condition.
 *
 *  Please note that checking duplicate is not required for custom MCB's
 * because their tags are considered unique.
 *
 * Parameters:
 *  mcb_prop [in]  pointer to an MCB property
 *  mcbtable [in]  pointer to a table of MCB properties
 *  nentries [in]  number of valid entries in mcbtable
 * Returns:
 *  true if the identical property of mcb_prop are found in mcbtable
 *  false otherwise
 */
static bool is_mcb_duplicate(struct mcb_properties *mcb_prop,
	struct mcb_properties *mcbtable, unsigned int nentries)
{
	unsigned int i;

	if (mcb_prop == NULL || mcbtable == NULL || nentries == 0)
		return false;

	for (i=0; i<nentries; ++i) {
		if (mcb_prop->ddr_clock != mcbtable[i].ddr_clock)
			continue;
		if (mcb_prop->ddr_size != mcbtable[i].ddr_size)
			continue;
		if (mcb_prop->ddr_width != mcbtable[i].ddr_width)
			continue;
		if (mcb_prop->phy_width != mcbtable[i].phy_width)
			continue;
		return true;
	}

	return false;
}

/* parse_mcbfname -- parses an MCB filename into DDR speed, PHY width, device
 *   size/width and endianness
 *
 *  An MCB file name contains the identification of the MCB, DDR speed,
 * PHY width, device size, device width, endianness and tag name.
 * A file name gets tokenized mainly by '_' (underscore) before they get
 * further parsed. For example, "7445Dx_933MHz_32b_dev4Gx8_DDR3-1866M_le.mcb"
 * gets parsed into:
 *   "7445dx", 933MHz, 32bit PHY, 4Gbit device with 8bit, DDR3-1866M and
 *   Little Endian
 * DDR3-1866M and Little Endian are ignored, and "7445dx" (position-wise)
 * is used as a tag when a custom MCB is specified.
 *
 * Parameters:
 *  fname    [in]  file name to be parsed
 *  mcb_prop [out] pointer to an MCB property filled with parsed results
 * Returns:
 *  NUM_PARAMS_FROM_MCB_FILENAME on success, -(#fields parsed) otherwise
 */
static int parse_mcbfname(char *fname, struct mcb_properties *mcb_prop)
{
	char *p, *tmp;
	int nfields;
	int nparams;

	if (fname == NULL || mcb_prop == NULL)
		return 0;

	tmp = decompose_fname(fname);
	if (tmp == NULL)
		return 0;
	/* "7445dx", "933mhz", "32b", "dev4gx8", "ddr3-1866m", "le", "" */

	nfields = count_fields(tmp);
	if (nfields != NUM_PARAMS_FROM_MCB_FILENAME)
		goto error_return;
 
	nparams = 0; /* let the game begin */
	p = tmp;

	/* copy the chip ID (7445dx) or tag */
	strncpy(mcb_prop->tag, p, sizeof(mcb_prop->tag));
	++nparams;
	p += strlen(p) + 1;

	/* p is pointing to "933mhz" */
	mcb_prop->ddr_clock = atoi(p);
	if (mcb_prop->ddr_clock == 0)
		goto error_return;
	if (strstr(p, "ghz"))
		mcb_prop->ddr_clock *= 1000;
	++nparams;
	p += strlen(p) + 1;

	/* p is pointing to "32b" */
	mcb_prop->phy_width = atoi(p);
	if (mcb_prop->phy_width == 0)
		goto error_return;
	++nparams;
	p += strlen(p) + 1;

	/* p is pointing to "dev4gx8" */
	if (0 != parse_dramparts(p, mcb_prop))
		goto error_return;
	nparams += 2; /* 2 == DRAM size and DRAM width */
	p += strlen(p) + 1;

	/* p is pointing to "ddr3-1866m" */

	free(tmp);
	return NUM_PARAMS_FROM_MCB_FILENAME;

error_return:
	if (tmp != NULL)
		free(tmp);
	return -nparams;
}

/* print_mcbtable -- prints out the main MCB table
 *
 *  The main MCB table is printed out in "struct memsys_info []" format.
 * The "tag" field is ignored and hardwired to NULL.
 *
 * Parameters:
 *  fp        [in] file pointer that the base MCB will be written to
 *  nfiles    [in] #entries in mcbfiles
 *  mcbfiles  [in] pointer to entries of common MCB files
 * Returns:
 *  0 on success, -1 otherwise
 */
static int print_mcbtable(FILE *fp, int nfiles, char **mcbfiles)
{
	int i;
	struct mcb_properties *mcbtable;

	if (fp == NULL || nfiles == 0 || mcbfiles == NULL)
		return -1;

	mcbtable = (struct mcb_properties *)
		malloc(nfiles * sizeof(struct mcb_properties));
	/*
		const struct memsys_info __maybe_unused shmoo_data[] __attribute__ ((section(\".mcbtable\"))) = {
			{ NULL, MCB_MAGIC1, MCB_MAGIC2, MCB_MAGIC3, -1, NULL },
			{ memsys_config_template, MCB_MAGIC1, SHMOO_ARRAY_SIZE, SHMOO_ARRAY_ELEMENTS, -1, NULL },
	*/
	fprintf(fp, "const struct memsys_info __maybe_unused shmoo_data[] "
		"__attribute__ ((section(\".mcbtable\"))) = {\n");
	fprintf(fp, "\t{ NULL, MCB_MAGIC1, MCB_MAGIC2, MCB_MAGIC3, "
		"-1, NULL },\n");
	fprintf(fp, "\t{ memsys_config_template,\tMCB_MAGIC1,"
		"\tSHMOO_ARRAY_SIZE,\tSHMOO_ARRAY_ELEMENTS, "
		"-1, NULL },\n");
	for (i=0; i<nfiles; ++i) {
		int nfields;
		struct mcb_properties mcb_prop;
		char fname_base[PATH_MAX];
		char *fname = /* mcbfiles[i] contains directory if 'fixed' */
			basename(mcbfiles[i]);

		if (mcbfiles[i] == NULL)
			break; /* no more 'chip_id' matching MCB */

		normalize_fname(fname_base, fname);

		nfields = parse_mcbfname(fname, &mcb_prop);
		if (nfields != NUM_PARAMS_FROM_MCB_FILENAME) {
			fprintf(stderr, "%s splits into %d pieces\n",
				fname, nfields);
			return -1;;
		}

		/* Check duplication only when compressing. For fixed MCB's
		 * (non-compressed), duplication should be allowed.
		 */
		if (g_compress)
			if (is_mcb_duplicate(&mcb_prop, mcbtable, i)) {
				fprintf(stderr, "%s is duplicate\n",
					mcbfiles[i]);
				return -1;;
			}

		memcpy(&mcbtable[i], &mcb_prop, sizeof(mcb_prop));

		fprintf(fp, "\t{ %s", fname_base);

		if (!g_compress) {
			fprintf(fp, "_%d", g_fixed_main_table_top);
			g_fixed_main_table_top++;
		}

		fprintf(fp, ",\t%d,\t%d,\t%d, %d, NULL },\n",
			mcb_prop.ddr_clock,
			mcb_prop.ddr_size, mcb_prop.ddr_width,
			mcb_prop.phy_width);

	}
	fprintf(fp, "\t{ NULL, 0, 0, 0, 0, NULL },\n");
	fprintf(fp, "};\n");

	return 0;
}

/* print_mcbsubtable -- prints out platform specific custom MCB table
 *
 *  A platform specific custom MCB table is printed out in
 * "struct memsys_info []" format. The "tag" field is significant unlike
 * the main MCB table. "tag"s are defined before the table, so that
 * the table can refer to "tag"s.
 *
 * Parameters:
 *  fp        [in] file pointer that the base MCB will be written to
 *  nfiles    [in] #entries of common MCB files
 *  mcbfiles  [in] pointer to entries of common MCB files
 * Returns:
 *  0 on success, -1 otherwise
 */
static int print_mcbsubtable(FILE *fp, int nfiles, char **mcbfiles)
{
	int i, f = g_fixed_main_table_top;

	if (fp == NULL || nfiles == 0 || mcbfiles == NULL)
		return -1;

	/*
	 * static const char mcb_nocs0[] __attribute__ ((section(".mcbdata"))) = "nocs0";
	 */
	for (i=0; i<nfiles; ++i) {
		int nfields;
		struct mcb_properties mcb_prop;
		char fname_base[PATH_MAX];
		char *fname = /* mcbfiles[i] contains directory if 'fixed' */
			basename(mcbfiles[i]);

		normalize_fname(fname_base, fname);

		nfields = parse_mcbfname(fname, &mcb_prop);
		if (nfields != NUM_PARAMS_FROM_MCB_FILENAME) {
			fprintf(stderr, "%s splits into %d pieces\n",
				fname, nfields);
			return -1;
		}

		fprintf(fp, "static const char tag_%s",	fname_base);

		if (!g_compress) {
			fprintf(fp, "_%d", f);
			f++;
		}

		fprintf(fp, "[] __attribute__ ((section(\".mcbdata\")))"
			" = \"%s\";\n", mcb_prop.tag);
	}

	/* reset */
	f = g_fixed_main_table_top;

	/*
	 * const struct memsys_info __maybe_unused customshmoo_data[] __attribute__ ((section(\".mcbsubtable\"))) = {
	 *     { NULL, MCB_MAGIC1, MCB_MAGIC2, MCB_MAGIC3, -1, NULL },
	 */
	fprintf(fp, "const struct memsys_info __maybe_unused customshmoo_data[]"
		" __attribute__ ((section(\".mcbsubtable\"))) = {\n");
	fprintf(fp, "\t{ NULL, MCB_MAGIC1, MCB_MAGIC2, MCB_MAGIC3, "
		"-1, NULL },\n");

	for (i=0; i<nfiles; ++i) {
		int nfields;
		struct mcb_properties mcb_prop;
		char fname_base[PATH_MAX];
		char *fname = /* mcbfiles[i] contains directory if 'fixed' */
			basename(mcbfiles[i]);

		normalize_fname(fname_base, fname);

		memset(&mcb_prop, 0, sizeof(mcb_prop));

		/* redo in case the compiler get aggressive.
		*/
		nfields = parse_mcbfname(fname, &mcb_prop);
		if (nfields != NUM_PARAMS_FROM_MCB_FILENAME) {
			fprintf(stderr, "%s splits into %d pieces\n",
				fname, nfields);
			return -1;
		}

		fprintf(fp, "\t{ %s", fname_base);

		if (!g_compress)
			fprintf(fp, "_%d", f);

		fprintf(fp, ",\t%d,\t%d,\t%d, %d,\ttag_%s",
			mcb_prop.ddr_clock, mcb_prop.ddr_size,
			mcb_prop.ddr_width, mcb_prop.phy_width,
			fname_base);

		if (!g_compress) {
			fprintf(fp, "_%d", f);
			f++;
		}

		fprintf(fp, " },\n");
	}
	fprintf(fp, "\t{ NULL, 0, 0, 0, 0, NULL },\n");
	fprintf(fp, "};\n");

	return 0;
}

/* find_matching_mcbfile -- finds an MCB filename that matches the given
 *  MCB pattern in the given directory
 *
 * Find file names that match MCB patterns for 'fixed', for example:
 *   custom : 7445d0/custom_933mhz_32b_dev4gx16
 *   non-custom : 7445d0_933mhz_32b_dev4gx16
 *
 * Parameters:
 *  pattern_mcb [in] MCB pattern to be matched
 *  dir_mcb     [in] where MCB files are stored
 *  base_dir    [in] the starting directory to where return after this function
 * Returns:
 *  NULL on failure, otherwise a buffer is returned after allocated and filled
 *  with a matching MCB filename
 */
static char *find_matching_mcbfile(const char *pattern_mcb, const char *dir_mcb,
	const char *base_dir)
{
	int i;
	char *mcbfilename;
	const char *pattern;
	char *tmp, buf[PATH_MAX];
	char dir_sub[BUFSIZE_CHIP_ID], chip_id[BUFSIZE_CHIP_ID];
	size_t len_chip_id = 0;
	int ndentries; /* # directory entries */
	struct dirent **dentries;

	/* change directory to where MCB files are */
	if (chdir(dir_mcb) < 0) {
		fprintf(stderr, "Cannot change directory to %s\n",
			dir_mcb);
		return NULL;
	}

	strcpy(buf, pattern_mcb);
	tmp = dirname(buf);
	if (strcmp(tmp, ".")) {
		/* 'custom' MCB, change to the sub directory */
		size_t len_subdir = strlen(tmp);

		if (len_subdir >= sizeof(dir_sub)) {
			fprintf(stderr, "Too long sub_dir %s\n", tmp);
			return NULL;
		}
		strcpy(dir_sub, tmp);
		pattern = pattern_mcb + strlen(dir_sub) + 1; /* "dir_sub" + "/" */
		if (chdir(dir_sub) < 0) {
			fprintf(stderr, "Cannot change directory to %s\n",
				dir_sub);
			return NULL;
		}
	} else {
		/* non 'custom' MCB, a matching MCB should be in 'dir_mcb' */
		char *items;

		pattern = pattern_mcb;
		items = decompose_fname(buf);
		if (items == NULL) {
			fprintf(stderr, "Cannot decompose %s\n", buf);
			return NULL;
		}
		len_chip_id = strlen(items);
		if (len_chip_id >= sizeof(chip_id)) {
			free(items);
			fprintf(stderr, "Too long chip_id %s\n", items);
			return NULL;
		}
		strcpy(chip_id, items);
		free(items);
	}

	ndentries = scandir(".", &dentries, filter_reg, alphasort);
	if (ndentries <= 0) {
		fprintf(stderr, "No valid MCB file in %s\n", dir_sub);
		return NULL;
	}

	mcbfilename = NULL;
	for (i=0; i<ndentries; ++i) {
		char *fname = dentries[i]->d_name;

		if (len_chip_id == 0) {
			/* custom MCB, must be an identical match */
			size_t len = strlen(pattern);

			if (strlen(fname) < len)
				continue;
			if (strncasecmp(pattern, fname, len))
				continue;
		} else {
			char *f_mcb; /* excluding chip ID */
			const char *p_mcb; /* excluding chip ID */
			size_t len_chip_id_fname;

			/* 7445d0 first, and then 7445dx for 7445d0 */
			len_chip_id_fname =
				does_chip_id_match_filename(fname, chip_id);
			if (!len_chip_id_fname)
				continue;
			f_mcb = fname + len_chip_id_fname;
			p_mcb = pattern + len_chip_id;
			if (strncasecmp(f_mcb, p_mcb, strlen(p_mcb)))
				continue;
		}

		if (len_chip_id == 0) {
			sprintf(buf, "%s/%s/%s", dir_mcb, dir_sub, fname);
		} else {
			sprintf(buf, "%s/%s", dir_mcb, fname);
		}

		mcbfilename = strdup(buf);
		break;
	}
	free_dentries(ndentries, dentries);

	if (chdir(base_dir) < 0) {
		free(mcbfilename);
		fprintf(stderr, "Cannot change directory to %s\n",
			base_dir);
		return NULL;
	}

	return mcbfilename;
}

