/* SUBINOVL.C -- reads, combines and writes modules for BIN/OVL and GRB file */
/* functions return FALSE if unable to open source files */

/*  History
**
**   8 jul 87   mp      start history
**  28 jul 87   mp      eliminate flashing status message while writing bin/ovl
**  10 aug 87   mp      change memcpy() to memmove() for C 5.00
**  21 aug 87   mp      add removing of WIN100.*
**  21 sep 87   mp      add removing of WIN200.BIN
*/

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <string.h>
#include <dos.h>
#include <newexe.h>
#include <sys\types.h>
#include <sys\stat.h>
#include "setup.h"
				    /* offset from beg. of file to new EXE h.*/
#define HEADB 0x0400		    /* # of header bytes not in memory */
#define HEADP (HEADB >> 4)	    /* # of header paragraphs not in mem */

#define RB_TO_P(x) (((x) + 15) >> 4)/* convert bytes to paragraphs (rounded) */
#define B_TO_P(x) ((x) >> 4)	    /* convert bytes to paragraphs */
#define P_TO_B(x) ((x) << 4)	    /* convert paragraphs to bytes */

#define PREST(x) (15 - ((x) - 1) % 16)	/* # of bytes to fill up a paragraph */

#define WBUFSIZE 128		    /* size of buffer for writing nulls */

struct modinfo {		    /* informations about modules for BIN/OVL*/
    CHAR     *mo_pFName;	    /* filename */
    struct bl_info far *mo_fpBuf;   /* pointer to file image in memory */
};

static struct modinfo moArr[] = {{"KERNEL.EXE"},{"SYSTEM.DRV"},{"GERMANY.DRV"},
		   {"MOUSE.DRV"}, {"EGAHIRES.DRV"}, {"SOUND.DRV"},
		   {"COMM.DRV"}, {"HIFONTS.FON"}, {"GDI.EXE"},
		   {"USER.EXE"}, {"MSDOSD.EXE"}, {"MSDOS.EXE"}, {NULL}};

int fhBO;		    /* file handle for WIN200.BIN/OVL and GRB*/
static int fhSrc;		    /* file handle for source file */
static unsigned uBinCnt = 0x40;	    /* counter of paragraphs already */
static unsigned uOvlCnt = 0;	    /* in BIN and OVL file */
static struct bl_info far *fpbGrb;  /* pointer to GRB file in memory */
CHAR BinFile[] = "WIN200.BIN";
CHAR OvlFile[] = "WIN200.OVL";
CHAR GrbFile[] = "WINOLDAP.GRB";

static CHAR winstub[] = "\
\x0e\x1f\xbe\x30\x01\x81\xc6\xff\x01\x81\xe6\x00\xfe\x81\x3c\x4e\
\x45\x75\x6e\x8c\xd8\xfa\x8e\xd0\x8b\xe6\xfb\x8b\x5c\x0e\x4b\x7c\
\x25\xd1\xe3\xd1\xe3\xd1\xe3\x03\x5c\x22\x8B\x00\x83\x7C\x32\x00\
\x75\x05\xC7\x44\x32\x09\x00\x8B\x4C\x32\x83\xE9\x04\xD3\xE0\x8C\
\xCF\x83\xEF\x20\x03\xF8\x8B\x5C\x16\x4B\x7C\x35\xD1\xE3\xD1\xE3\
\xD1\xE3\x03\x5C\x22\x8B\x00\x8B\x4C\x32\x83\xE9\x04\xD3\xE0\x8C\
\xCA\x83\xEA\x20\x03\xD0\x52\xFF\x74\x14\x8E\xDF\x8B\xCE\x81\xC1\
\x00\x02\xEB\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\xCB\xE8\x1E\x00KERNSTUB: Error during boot\x0D\
\x0A\x24\x5A\x0E\x1F\xB4\x09\xCD\x21\xB8\x01\x4C\xCD\x21\x4A";

/* external declaration from SUDEST.C
*/
extern void CheckOldVersion();

/* forward declarations lokal procedures
*/
static struct bl_info far *InitBlock(unsigned, unsigned);
static void ReadSegments(struct bl_info far **, struct new_exe far *);
static void ReadResources(struct bl_info far *, struct rsrc_typeinfo far *);
static void PrepSegments(struct bl_info far **, struct new_exe far *);
static void PrepResources(struct bl_info far *, struct rsrc_typeinfo far *);


/* opens WIN200.BIN and writes old EXE header and STUB */

void BinOpen()
{
    #define OBUFSIZE 512
    union uniBuf {		    /* write buffer */
	struct exe_hdr eh;	    /* old EXE header */
	CHAR p[OBUFSIZE];
    } Buf;

    remove(FileName(WindowsPath, "WIN100.BIN"));  /* try to remove old file */
    remove(FileName(WindowsPath, BinFile));       /* try to remove old file */
    CheckOldVersion();                  /* check old versions */

    PutFileName(BinFile);
    PutDestName(WindowsPath);
    if ((fhBO = open(FileName(WindowsPath, BinFile),
		O_CREAT|O_RDWR|O_TRUNC|O_BINARY, S_IREAD|S_IWRITE)) == -1)
	FatalError(smOpenErr, 4);

    memset(Buf.p, 0, OBUFSIZE);	     /* initialize old EXE header */
    E_MAGIC(Buf.eh) = EMAGIC;
    E_CPARHDR(Buf.eh) = 0x0020;
    E_SS(Buf.eh) = 0x000B;
    E_SP(Buf.eh) = 0x0080;
    E_LFARLC(Buf.eh) = 0x0040;
    E_LFANEW(Buf.eh) = 0x0400;
    if (write(fhBO, Buf.p, OBUFSIZE) < OBUFSIZE)
	FatalError(smWriteErr, 4);

    memset(Buf.p, 0, OBUFSIZE);	     /* initialize STUB program */
    memmove(Buf.p, winstub, 176);
    if (write(fhBO, Buf.p, OBUFSIZE) < OBUFSIZE)
	FatalError(smWriteErr, 4);
}


/* Reads in a module */

BOOL ReadModule(pFName, pfpb)
CHAR *pFName;			    /* File Name */
struct bl_info far **pfpb;	    /* pointer to root of bl_info chain */
{
    long nrestab;		    /* value of ne_nrestab */
    unsigned uTableB;		    /* length of table in bytes */
    struct bl_info far *fpb;	    /* pointer to block info */
    struct new_exe far *fpe;	    /* pointer to new_exe header */
    struct new_rsrc far *fpr;	    /* pointer to new_rsrc info */
    unsigned uOffsetP;		    /* file offset in paragraphs */

    PutFileName(pFName);
    if ((fhSrc = open(FileName(pSourcePath, pFName), O_RDONLY|O_BINARY))
									== -1)
	return FALSE;


    /* read header information */

    DisplayStatusLine(smLoading);

    lseek(fhSrc, 0x42Cl, SEEK_SET);	/* seek and read ne_nrestab */
    if (read(fhSrc, (CHAR *)&nrestab, sizeof(long)) < sizeof(long))
	FatalError(smLoadErr, 4);
    uTableB = nrestab - 0x400l;

    lseek(fhSrc, 0x400l, SEEK_SET);	/* read header */
    *pfpb = fpb = InitBlock(uTableB, BL_BIN);

    fpe = (struct new_exe far *)(fpb + 1);


    /* read non resident table */

    lseek(fhSrc, nrestab, SEEK_SET);
    fpb = fpb->b_next = InitBlock(fpe->ne_cbnrestab, BL_OVL);


    ReadSegments(&fpb, fpe);


    /* read resources */

    if (fpe->ne_rsrctab != fpe->ne_restab) {
	fpr = (struct new_rsrc far *)((CHAR far *)fpe + fpe->ne_rsrctab);
	if (fpr->rs_align != 4)
	    FatalError(smLoadErr, 103);		/* no paragraph alignment */

	uOffsetP = ((struct rsrc_nameinfo far *)
		    (&fpr->rs_typeinfo + 1))->rn_offset;
	lseek(fhSrc, (long)uOffsetP << 4, SEEK_SET); /* seek to first rsrc */

	ReadResources(fpb, &(fpr->rs_typeinfo));
    }

    close(fhSrc);
    return TRUE;
}

/* allocates memory and reads block (file pointer must be in right position */

static struct bl_info far *InitBlock(u, uBits)
unsigned u;				/* length of block in bytes */
unsigned uBits;				/* BIN and OVL bits */
{
    struct bl_info far *fpb;		/* pointer to bl_info */
					/* allocate memory */
    if (!(fpb = (struct bl_info far *)_fmalloc(sizeof(struct bl_info) + u)))
	FatalError(smNoMemory, 4);
    fpb->b_size = u;
    fpb->b_null = PREST(u);
    fpb->b_bits = uBits;
    fpb->b_next = 0;
					/* read */
    if (readf(fhSrc, (CHAR far *)(fpb + 1), u) < u)
	FatalError(smLoadErr, 4);
    return fpb;
}

/* reads segments */

static void ReadSegments(pfpb, fpe)
struct bl_info far **pfpb;	    /* on entry: pointer to current bl_info */
				    /* on exit:	 pointer to last bl_info */
struct new_exe far *fpe;	    /* pointer to new new_exe header */
{
    unsigned uAlign;		    /* alignment for segments in paragraphs */
    struct bl_info far *fpb = *pfpb;/* pointer to block info */
    struct new_seg far *fps;	    /* pointer to entry of segment table */
    BOOL bBin, bOvl;		    /* does block appear in BIN/OVL? */
    unsigned uBits;		    /* BIN and OVL bits */
    unsigned nreloc;		    /* number of relocation items */
    unsigned uBytes;		    /* length of relocation info */
    int i;

    if ((uAlign = fpe->ne_align) < 4)
	FatalError(smLoadErr, 101);	/* alignment smaller than paragraph */
    fpe->ne_align = 4;

    fps = (struct new_seg far *)((CHAR far *)fpe + fpe->ne_segtab);
    for (i = fpe->ne_cseg; i > 0; i--) {
	bBin = (fps->ns_flags & NSPRELOAD);
	bOvl = (fps->ns_flags & NSDISCARD) || !(fps->ns_flags & NSPRELOAD);
	if ((fpe->ne_flags & NEINST) && (fps->ns_flags & NSDATA)) {
	    fps->ns_flags |= 0xF000;	/* mark data of instance as discard.*/
	    bOvl = TRUE;
	}
	uBits = (bBin ? BL_BIN : 0) | (bOvl ? BL_OVL : 0);

	lseek(fhSrc, (long)fps->ns_sector << uAlign, SEEK_SET);
	fpb = fpb->b_next = InitBlock(fps->ns_cbseg, uBits);

	if (fps->ns_flags & (NSITER|NSDEBUG))
	    FatalError(smLoadErr, 102);

	if (fps->ns_flags & NSRELOC) {
	    if (read(fhSrc, (CHAR *)&nreloc, sizeof(unsigned)) <
							    sizeof(unsigned))
		FatalError(smLoadErr, 4);
	    lseek(fhSrc, (long)-sizeof(unsigned), SEEK_CUR);	/* seek back */
	    uBytes = sizeof(unsigned) + nreloc * sizeof(struct new_rlc);
	    fpb = fpb->b_next = InitBlock(uBytes, uBits);
	}
	fps++;
    }
    *pfpb = fpb;
}

/* reads resources */

static void ReadResources(fpb, fprt)
struct bl_info far *fpb;	    /* pointer to current bl_info */
struct rsrc_typeinfo far *fprt;	    /* pointer to resource information block */
{
    struct rsrc_nameinfo far *fprn; /* pointer to resource entry */
    unsigned uBits;		    /* BIN and OVL bits */
    int i;			    /* resource counter */
    unsigned uLen;

    for (; fprt->rt_id; fprt = (struct rsrc_typeinfo far *) fprn) {
	fprn = (struct rsrc_nameinfo far *) (fprt + 1);
	for (i = fprt->rt_nres; i > 0; i--) {
	    uBits = BL_BIN *
		((fprn->rn_flags & RNPRELOAD) || !(fprn->rn_flags & RNMOVE));
	    if (fprn->rn_flags & (RNDISCARD | RNMOVE | RNPURE))
		uBits |= BL_OVL;

	    fpb = fpb->b_next = InitBlock(fprn->rn_length << 4, uBits);
	    fprn++;
	} /* all resources */
    } /* all types of resources */
}


/* prepares module (modifies offsets in headers to match BIN/OVL files) */

void PrepModule(fpb)
struct bl_info far *fpb;	    /* root of bl_info chain */
{
    unsigned nrestab;		    /* value of ne_nrestab */
    unsigned uTableB;		    /* length of table in bytes */
    struct new_exe far *fpe;	    /* pointer to new_exe header */
    struct new_rsrc far *fpr;	    /* pointer to new_rsrc info */
    unsigned uOffsetP;		    /* file offset in paragraphs */

    fpe = (struct new_exe far *)(fpb + 1);
    fpe->ne_nrestab = uOvlCnt;	     /* beginning of this module in OVL */
    uBinCnt += RB_TO_P(fpb->b_size);	/* Add header size */

    fpb = fpb->b_next;			/* non res name tab is active */
    uOvlCnt += RB_TO_P(fpb->b_size);	/* Add size */


    fpb = fpb->b_next;			/* first segment is active */
    PrepSegments(&fpb, fpe);


    /* prepares resources */

    if (fpe->ne_rsrctab != fpe->ne_restab) {
	fpr = (struct new_rsrc far *)((CHAR far *)fpe + fpe->ne_rsrctab);

	PrepResources(fpb, &(fpr->rs_typeinfo));
    }

    fpe->ne_crc = uBinCnt;	    /* chain to next module in BIN */
}

/* prepares segments */

static void PrepSegments(pfpb, fpe)
struct bl_info far **pfpb;	    /* on entry: pointer to current bl_info */
				    /* on exit:	 pointer to last bl_info */
struct new_exe far *fpe;	    /* pointer to new new_exe header */
{
    struct bl_info far *fpb = *pfpb;/* pointer to block info */
    struct bl_info far *fpbR;	    /* pointer to block info of reloc info */
    struct new_seg far *fps;	    /* pointer to entry of segment table */
    unsigned uBits;		    /* BIN and OVL bits */
    unsigned uBytes;		    /* length of relocation info */
    BOOL bReloc;		    /* has segment relocation information? */
    int i;

    fps = (struct new_seg far *)((CHAR far *)fpe + fpe->ne_segtab);
    for (i = fpe->ne_cseg; i > 0; i--) {
	fpbR = fpb->b_next;
	uBits = fpb->b_bits;
	fpb->b_null = 0;
	uBytes = fpb->b_size +
		 ((bReloc = fps->ns_flags & NSRELOC) ? fpbR->b_size : 0);

	if (uBits & BL_BIN) {
	    if (uBits & BL_OVL) {
		uBytes += fpb->b_null =
			    (fps->ns_cbseg = fps->ns_minalloc) - fpb->b_size;
		fps->ns_minalloc = uOvlCnt;
		uOvlCnt += RB_TO_P(uBytes);
	    }
	    fps->ns_sector = uBinCnt;
	    uBinCnt += RB_TO_P(uBytes);
	} else {
	    if (uBits & BL_OVL) {
		fps->ns_sector = uOvlCnt;
		uOvlCnt += RB_TO_P(uBytes);
	    } else
		FatalError(smLoadErr, 104);   /* neither in BIN nor in OVL */
	}
	if (bReloc) {			    /* fill up paragraph */
	    fpbR->b_null = PREST(uBytes);
	    fpbR = fpbR->b_next;
	} else
	    fpb->b_null += PREST(uBytes);

	fpb = fpbR;
	fps++;
    }
    *pfpb = fpb;
}

/* prepares resource table */

static void PrepResources(fpb, fprt)
struct bl_info far *fpb;	    /* pointer to current bl_info */
struct rsrc_typeinfo far *fprt;	    /* pointer to resource information block */
{
    struct rsrc_nameinfo far *fprn;  /* pointer to resource entry */
    unsigned uBits;		    /* BIN and OVL bits */
    int i;			    /* resource counter */
    unsigned uLen;

    for (; fprt->rt_id; fprt = (struct rsrc_typeinfo far *) fprn) {
	fprn = (struct rsrc_nameinfo far *) (fprt + 1);
	for (i = fprt->rt_nres; i > 0; i--) {
	    uBits = fpb->b_bits;

	    uLen = fprn->rn_length;
	    if (uBits & BL_BIN) {
		if (uBits & BL_OVL) {
		    fprn->rn_handle = uOvlCnt;
		    uOvlCnt += uLen;
		}
		fprn->rn_offset = uBinCnt;
		uBinCnt += uLen;
	    } else {
		if (uBits & BL_OVL) {
		    fprn->rn_offset = uOvlCnt;
		    uOvlCnt += uLen;
		} else
		    FatalError(smLoadErr, 105);	 /* neither in BIN nor in OVL */
	    }
	    fpb = fpb->b_next;
	    fprn++;
	} /* all resources */
    } /* all types of resources */
}


/* writes module to file fhBO */

void WriteModule(pfpb, uBits)
struct bl_info far **pfpb;	    /* pointer to root of bl_info chain */
unsigned uBits;			    /* writing to BIN or OVL file */
{
    struct bl_info far *fpb = *pfpb;/* pointer to current block */
    struct bl_info far *fpbLast = (struct bl_info *)pfpb;
				    /* pointer to last block */
    CHAR pBuf[WBUFSIZE];	    /* buffer for nulls */
    unsigned u, uLen;
    unsigned uBitsBl;		    /* bits of block */

    memset(pBuf, 0, WBUFSIZE);

    while (fpb) {
	if ((uBitsBl = fpb->b_bits) & uBits) {
	    u = fpb->b_size;
	    if (writef(fhBO, (CHAR far *)(fpb + 1), u) < u) /* write data */
		FatalError(smWriteErr, 4);

	    for (uLen = fpb->b_null; uLen; uLen -= u)	    /* write nulls */
		if ((u = write(fhBO, pBuf, MIN(uLen, WBUFSIZE))) < 1)
		    FatalError(smWriteErr, 4);
	}
	if ((uBits & BL_OVL) || !(uBitsBl & BL_OVL)) {
	    fpbLast->b_next = fpb->b_next;
	    _ffree((CHAR far *)fpb);
	    fpb = fpbLast->b_next;
	} else {
	    fpb = (fpbLast = fpb)->b_next;
	}
    }
}


/* write null paragraph, */
/* fill BIN file to page boundary (32 paragraphs = 512 Bytes) and close file */

void BinClose()
{
    CHAR pBuf[528];
    struct exe_hdr eh;
    unsigned uPages = uBinCnt >> 5;	    /* # of pages */
    unsigned uLastPageP = uBinCnt - (uPages << 5); /* # of para occupied */

    memset(pBuf, 0, 528);
    if (IsRuntime) {			    /* append application name */
	*((unsigned *)pBuf) = strlen(inserttext[D_EXEFILE]);
	strcpy(pBuf + sizeof(unsigned), inserttext[D_EXEFILE]);
    }
    if (write(fhBO, pBuf, 528) < 528)
	FatalError(smWriteErr, 4);

    lseek(fhBO, 0l, SEEK_SET);		/* update EXE header */
    read(fhBO, (CHAR *)&eh, sizeof(struct exe_hdr));
    eh.e_cblp = (uLastPageP + 1) << 4;
    eh.e_cp = uPages + 2;
    lseek(fhBO, 0l, SEEK_SET);
    if (write(fhBO, (CHAR *)&eh, sizeof(struct exe_hdr)) < 1)
	FatalError(smWriteErr, 4);

    close(fhBO);
}


/* opens WIN200.OVL */

void OvlOpen()
{
    remove(FileName(WindowsPath, "WIN100.OVL"));  /* try to remove old file */

    if ((fhBO = open(FileName(WindowsPath, OvlFile),
		O_CREAT|O_RDWR|O_TRUNC|O_BINARY, S_IREAD|S_IWRITE)) == -1)
	FatalError(smOpenErr, 4);

    uOvlCnt = 0;
}

/* closes OVL file */

/* BILLHU
void OvlClose()
{
    close(fhBO);
}
*/



/* reads grabber file into the memory */

BOOL ReadGrabber(pFName)
CHAR *pFName;			    /* File Name */
{
    int fh;			    /* file handle for grabber file */
    int fhSt = fhSrc;		    /* file handle to store BIN file handle */

    PutFileName(pFName);

    if ((fh = open(FileName(pSourcePath, pFName), O_RDONLY|O_BINARY)) == -1)
	return FALSE;

    DisplayStatusLine(smLoading);

    fhSrc = fh;
    fpbGrb = InitBlock((unsigned)filelength(fh), BL_BIN);
    fhSrc = fhSt;

    fpbGrb->b_null = 0;

    close(fh);

    return TRUE;
}


/* writes grabber file from memory on the disk */
/* don't invoke this function when BIN or OVL file is open! */

void WriteGrabber()
{
    if ((fhBO = open(FileName(WindowsPath, GrbFile),
		O_CREAT|O_RDWR|O_TRUNC|O_BINARY, S_IREAD|S_IWRITE)) == -1)
	FatalError(smOpenErr, 4);

    PutFileName(GrbFile);
    PutDestName(WindowsPath);

    WriteModule(&fpbGrb, BL_BIN);

    close(fhBO);
}
