/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
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
#include "console.h"
#include "error.h"
#include "devfuncs.h"
#include "timer.h"

#include "bolt.h"
#include "loader.h"
#include "fileops.h"
#include "elf.h"


/*  *********************************************************************
    *  Externs
    ********************************************************************* */

static int bolt_elfload(bolt_loadargs_t *la);

const bolt_loader_t elfloader = {
	"elf",
	bolt_elfload,
	0
};

/*  *********************************************************************
    *  readprogsegment(fsctx,ref,addr,size)
    *
    *  Read a program segment, generally corresponding to one
    *  section of the file.
    *
    *  Input parameters:
    *      fsctx - file I/O dispatch
    *      ref - reference data for open file handle
    *      addr - target virtual address
    *      size - size of region to read
    *
    *  Return value:
    *      Number of bytes copied or <0 if error occured
    ********************************************************************* */

static int readprogsegment(fileio_ctx_t *fsctx, void *ref,
			   void *addr, int size, int flags)
{
	int res;

	if (flags & LOADFLG_NOISY)
		xprintf("%p/%d ", addr, size);

	res = fs_read(fsctx, ref, addr, size);

	if (res < 0)
		return BOLT_ERR_IOERR;

	if (res != size)
		return BOLT_ERR_BADELFFMT;

	return size;
}

/*  *********************************************************************
    *  readclearbss(addr,size)
    *
    *  Process a BSS section, zeroing memory corresponding to
    *  the BSS.
    *
    *  Input parameters:
    *      addr - address to zero
    *      size - length of area to zero
    *
    *  Return value:
    *      number of zeroed bytes or <0 if error occured
    ********************************************************************* */

static int readclearbss(void *addr, int size, int flags)
{

	if (flags & LOADFLG_NOISY)
		xprintf("%p/%d ", addr, size);

	if (size > 0)
		memset(addr, 0, size);

	return size;
}

/*  *********************************************************************
    *  elfgetshdr(ops,ref,ep)
    *
    *  Get a section header from the ELF file
    *
    *  Input parameters:
    *      ops - file I/O dispatch
    *      ref - reference data for open file
    *      ep - extended header info
    *
    *  Return value:
    *      copy of section header (malloc'd) or NULL if no memory
    ********************************************************************* */

static Elf32_Shdr *elfgetshdr(fileio_ctx_t *fsctx, void *ref, Elf32_Ehdr *ep)
{
	Elf32_Shdr *shtab;
	unsigned int size = ep->e_shnum * sizeof(Elf32_Shdr);

	shtab = (Elf32_Shdr *) KMALLOC(size, 0);

	if (!shtab)
		return NULL;

	if (fs_seek(fsctx, ref, ep->e_shoff, FILE_SEEK_BEGINNING) !=
		(int)ep->e_shoff
	    || fs_read(fsctx, ref, (uint8_t *) shtab, size) != (int)size) {
		KFREE(shtab);
		return NULL;
	}

	return shtab;
}

/*  *********************************************************************
    *  elfload_internal(ops,ref,entrypt,flags)
    *
    *  Read an ELF file (main routine)
    *
    *  Input parameters:
    *      ops - file I/O dispatch
    *      ref - open file handle
    *      entrypt - filled in with entry vector
    *      flags - generic boot flags
    *
    *  Return value:
    *      0 if ok
    *      else error code
    ********************************************************************* */

static int elfload_internal(fileio_ctx_t *fsctx, void *ref,
			    unsigned long *entrypt, int flags)
{
	Elf32_Ehdr *ep;
	Elf32_Phdr *phtab = 0;
	Elf32_Shdr *shtab = 0;
	unsigned int nbytes;
	int i;
	int res;
	Elf32_Ehdr ehdr;

	ep = &ehdr;
	if (fs_read(fsctx, ref, (uint8_t *) ep, sizeof(*ep)) != sizeof(*ep))
		return BOLT_ERR_IOERR;

	/* check header validity */
	if (ep->e_ident[EI_MAG0] != ELFMAG0 ||
	    ep->e_ident[EI_MAG1] != ELFMAG1 ||
	    ep->e_ident[EI_MAG2] != ELFMAG2 ||
	    ep->e_ident[EI_MAG3] != ELFMAG3) {
		return BOLT_ERR_BADEXE;
	}

	if (ep->e_ident[EI_CLASS] != ELFCLASS32)
		return BOLT_ERR_NOT32BIT;

#ifdef __BIG_ENDIAN
	if (ep->e_ident[EI_DATA] != ELFDATA2MSB)
		return BOLT_ERR_WRONGENDIAN;	/* big endian */
#endif
#ifdef __LITTLE_ENDIAN
	if (ep->e_ident[EI_DATA] != ELFDATA2LSB)
		return BOLT_ERR_WRONGENDIAN;	/* little endian */
#endif

	if (ep->e_ident[EI_VERSION] != EV_CURRENT)
		return BOLT_ERR_BADELFVERS;

#if   defined(CFG_ARCH_ARM)
	if (ep->e_machine != EM_ARM)
#elif defined(CFG_ARCH_MIPS)
	if (ep->e_machine != EM_MIPS)
#else
 #error Missing machine architecture!
#endif
		return BOLT_ERR_NOTMACHINE;

	/* Is there a program header? */
	if (ep->e_phoff == 0 || ep->e_phnum == 0 ||
	    ep->e_phentsize != sizeof(Elf32_Phdr))
		return BOLT_ERR_BADELFFMT;

	/* Load program header */
	nbytes = ep->e_phnum * sizeof(Elf32_Phdr);
	phtab = (Elf32_Phdr *) KMALLOC(nbytes, 0);

	if (!phtab)
		return BOLT_ERR_NOMEM;

	if (fs_seek(fsctx, ref, ep->e_phoff, FILE_SEEK_BEGINNING) !=
	    (int)ep->e_phoff
	    || fs_read(fsctx, ref, (uint8_t *) phtab, nbytes) != (int)nbytes) {
		KFREE(phtab);
		return BOLT_ERR_IOERR;
	}

	/*
	 * From now on we've got no guarantee about the file order,
	 * even where the section header is.  Hopefully most linkers
	 * will put the section header after the program header, when
	 * they know that the executable is not demand paged.  We assume
	 * that the symbol and string tables always follow the program
	 * segments.
	 */

	/* read section table (if before first program segment) */
	if (ep->e_shoff < phtab[0].p_offset)
		shtab = elfgetshdr(fsctx, ref, ep);

	/* load program segments */
	/* We cope with a badly sorted program header, as produced by
	 * older versions of the GNU linker, by loading the segments
	 * in file offset order, not in program header order. */

	while (1) {
		Elf32_Off lowest_offset = ~0;
		Elf32_Phdr *ph = 0;

		/* find nearest loadable segment */
		for (i = 0; i < ep->e_phnum; i++)
			if ((phtab[i].p_type == PT_LOAD)
			    && (phtab[i].p_offset < lowest_offset)) {
				ph = &phtab[i];
				lowest_offset = ph->p_offset;
			}

		if (!ph)
			break;	/* none found, finished */

		/* load the segment */
		if (ph->p_filesz) {
			if (fs_seek
			    (fsctx, ref, ph->p_offset,
			     FILE_SEEK_BEGINNING) != (int)ph->p_offset) {
				if (shtab)
					KFREE(shtab);
				KFREE(phtab);
				return BOLT_ERR_BADELFFMT;
			}
			res = readprogsegment(fsctx, ref,
					      (void *)(intptr_t) (signed)
					      ph->p_vaddr, ph->p_filesz, flags);
			if (res != (int)ph->p_filesz) {
				if (shtab)
					KFREE(shtab);
				KFREE(phtab);
				return res;
			}
		}

		if (ph->p_filesz < ph->p_memsz) {
			res =
			    readclearbss((void *)(intptr_t) (signed)ph->p_vaddr
					 + ph->p_filesz,
					 ph->p_memsz - ph->p_filesz, flags);
			if (res < 0) {
				if (shtab)
					KFREE(shtab);
				KFREE(phtab);
				return res;
			}
		}

		ph->p_type = PT_NULL;	/* remove from consideration */
	}

	KFREE(phtab);

	*entrypt = (intptr_t) (signed)ep->e_entry;	/* return entry point */
	return 0;
}

/*  *********************************************************************
    *  bolt_elfload(ops,file,flags)
    *
    *  Read an ELF file (main entry point)
    *
    *  Input parameters:
    *      ops - fileio dispatch
    *      file - name of file to read
    *      ept - where to put entry point
    *      flags - load flags
    *
    *  Return value:
    *      0 if ok, else error code
    ********************************************************************* */

static int bolt_elfload(bolt_loadargs_t *la)
{
	fileio_ctx_t *fsctx;
	void *ref;
	int res;

	if (la->la_offs) {
		xprintf("Error: ELF loader does not support seek to offset\n");
		return BOLT_ERR_UNSUPPORTED;
	}

	/*
	 * Look up the file system type and get a context
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
	 * Open the remote file
	 */

	res = fs_open(fsctx, &ref, la->la_filename, FILE_MODE_READ);
	if (res != 0) {
		fs_uninit(fsctx);
		return BOLT_ERR_FILENOTFOUND;
	}

	/*
	 * Load the image.
	 */

	la->la_entrypt = 0;
	res =
	    elfload_internal(fsctx, ref, (long unsigned int *)&(la->la_entrypt),
			     la->la_flags);

	/*
	 * All done, release resources
	 */

	fs_close(fsctx, ref);
	fs_uninit(fsctx);

	return res;
}
