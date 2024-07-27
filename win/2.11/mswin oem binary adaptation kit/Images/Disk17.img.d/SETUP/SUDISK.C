/* SUDISK.C -- disk i/o functions */

/*
**  History
**
**  15 jul 87   mp      start history
**  21 jul 87   mp      renaming or removing of WIN.COM
**  28 jul 87   mp      CopyPifDirectory() added
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <string.h>
#include <dos.h>
#include <newexe.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <direct.h>
#include "setup.h"


extern textline Incorrect386File[];

/* external from SUMAIN.C */
extern BOOL Is386;


CHAR WinCom[] = "WIN.COM";
CHAR SetupLgo[] = "SETUP.LGO";
CHAR SetupLgd[] = "SETUP.LGD";
static CHAR MsDos[] = "MSDOS.EXE";
CHAR Win386Ver[] = "MICROSOFT WIN386 VERSION 2.11";


/* concatenates a pathname and a filename */

CHAR *FileName(pPath, pFName)
CHAR *pPath;			    /* pointer to path */
CHAR *pFName;			    /* pointer to filename */
{
    static CHAR pPathBuf[PATHMAX];

    return(strcat(strcpy(pPathBuf, pPath), pFName));
}


/*
 * ffirst() and fnext() are looking for files in a subdirectory
 * see also WINDOWS: CONFIG.ASM
 */

int ffirst(name, attr, buf)
CHAR *name;			   /* search specs like *.drv */
unsigned attr;			   /* attribute of searched files */
FINDTYPE *buf;			   /* data transfer area */
{
    union REGS inregs, outregs;

    bdos(0x1a, (unsigned)buf, 0);  /* Set Disk Transfer Address */

    inregs.x.cx = attr;
    inregs.x.dx = (short)name;
    inregs.h.ah = 0x4e;		   /* Find Next File */
    intdos(&inregs, &outregs);

    return (outregs.x.cflag ? _doserrno : 0);
}

int fnext(buf)
FINDTYPE *buf;			   /* data transfer area */
{
    union REGS inregs, outregs;

    bdos(0x1a, (unsigned)buf, 0);  /* Set Disk Transfer Address */

    inregs.h.ah = 0x4f;		   /* Find Next File */
    intdos(&inregs, &outregs);

    return (outregs.x.cflag ? _doserrno : 0);
}

/* the following are borrowed from CONTROL, which borrowed it */
/* from old SETUP with several appropriate changes. */
/* This function extracts the description string and library name */
/* from new_exe header */

BOOL IsNewExe(pPath, pFName, pLib, pDescr)
CHAR *pPath;				/* pointer to path */
CHAR *pFName;				/* pointer to file name */
CHAR *pLib;				/* pointer to space for library name */
CHAR *pDescr;				/* pointer to space for description */
{
    int result = FALSE;
    int fh;
    struct exe_hdr oeHeader;
    struct new_exe neHeader;
    unsigned int offrestab;
    CHAR cBytes;
    CHAR buf[20];
    long lNewHeader;
    extern long lseek( int, long, int );

    if ((fh = open(FileName(pPath, pFName), O_RDONLY|O_BINARY, NULL)) > -1) {
	read(fh, (CHAR *)&oeHeader, sizeof(oeHeader));
	if (oeHeader.e_magic == EMAGIC && oeHeader.e_lfanew)
	    lNewHeader = oeHeader.e_lfanew;
	else
	    lNewHeader = 0L;
	lseek(fh, lNewHeader, 0);
	read(fh, (CHAR *)&neHeader, sizeof(neHeader));
	if (neHeader.ne_magic == NEMAGIC) {
	    lseek(fh, neHeader.ne_restab+lNewHeader, 0);
	    read(fh, &cBytes, 1);
	    read(fh, pLib, cBytes);
	    *((pLib)+cBytes) = 0;
	    /* seek to the string (location+size of header) */
	    lseek(fh, neHeader.ne_nrestab, 0);
	    /* read in the description */
	    read(fh, &cBytes, 1);
	    cBytes = cBytes < DESCMAX-1 ? cBytes : DESCMAX-1;
	    pDescr[read(fh, pDescr, (int)cBytes)] = 0;
	    result = TRUE;
	}
	close(fh);
    }
    return(result);
}


/* ValidateWIN386: Opens, validates and closes the passed in file
/* returns TRUE if valid WIN386 version, else returns FALSE */

BOOL ValidateWIN386(fhSrc)
int fhSrc;				    /* file handle for src file */
{
    BOOL result;
    CHAR far *fpBuf;			    /* buffer for file load */
    unsigned uSize;			    /* size of allocated memory */

 	 
    result = FALSE;

    uSize = 0x2000;			    /* mem allocation */

    while ((fpBuf = _fmalloc(uSize)) == NULL)
		if ((uSize >>= 1) < 0x400)
	   	FatalError(smNoMemory, 2);

    if ((uSize = readf(fhSrc, fpBuf, uSize)) <= 0)  /* read source file */
		FatalError(smLoadErr, 2);

    result = SearchStrng(fpBuf, uSize);

	if (lseek(fhSrc, 0L, SEEK_SET) == -1L)
		FatalError(smLoadErr, 2);

    _ffree(fpBuf);

    return(result);
}

/* Search for string in buffer */

BOOL SearchStrng(fpBuf, bufSize)
CHAR far *fpBuf;			/* buffer for copying */
unsigned bufSize;			/* size of allocated memory */

{
CHAR *sstrng;
CHAR far *tfpBuf;

   while(bufSize) {
		sstrng = Win386Ver;
		--bufSize;
		if (*sstrng++ == *fpBuf++) {
		   tfpBuf = fpBuf;
		   while(*sstrng == *tfpBuf) {
				if(*sstrng == '\0')
			   	return TRUE;
				else {
		    		sstrng++;
		  			tfpBuf++;
				}
	     	}
         if (*tfpBuf == '0'&& *sstrng == '1')  /* Quick and dirty hack to make 2.10    */
            return TRUE;                       /* loaders (.3EX files) pass this test. */
		}
   }
   return FALSE;
}


/* CopyFile: Opens, copies and closes files and corrects creation time */
/* returns whether it was successful to open the file or not */

BOOL CopyFile(pSrcFName, pDstFName)
CHAR *pSrcFName;			    /* Source FName */
CHAR *pDstFName;			    /* Destination FName */
{
    int fhSrc, fhDst;			    /* file handle for src/dst file */
    union REGS regs;
    BOOL result;

   PutFileName(pSrcFName);

   if ((fhSrc = open(FileName(pSourcePath, pSrcFName),
	       O_RDONLY|O_BINARY)) == -1)
		return FALSE;

	/* KLUDGE! FOr 2.1 SETUP, in case of WIN386, make sure the
	   file that gets copied as WIN386.EXE (source file name - *.3EX)
	   is indeed 2.1 stuff. This is necessary while OTHER option is chosen
	   by the user. The verification is being done in all cases though. */

	if (Is386 && stricmp(pDstFName, "WIN386.EXE") == 0
				 && !ValidateWIN386(fhSrc))	{
		ScrClear();
		ScrDisplay(Incorrect386File);
		ContinueOrQuit();
	}

   if ((fhDst = open(FileName(WindowsPath, pDstFName),
	       O_CREAT|O_RDWR|O_TRUNC|O_BINARY,
	       S_IREAD|S_IWRITE)) == -1)
		FatalError(smOpenErr, 2);

    DisplayStatusLine(smCopying);

    CopyFileContents(fhSrc, fhDst);

    regs.x.bx = fhSrc;
    regs.x.ax = 0x5700;		   /* Get Date/Time of (source) File */
    intdos(&regs, &regs);	   /* regs.x.dx/cx contains date/time */
    regs.x.bx = fhDst;
    regs.x.ax = 0x5701;		   /* Get Date/Time of (destination) File */
    intdos(&regs, &regs);

    close(fhSrc);
    close(fhDst);

    return TRUE;
}

/* Copies contents of an opened file to another opened file */

void CopyFileContents(fhSrc, fhDst)
int fhSrc, fhDst;			    /* file handle for src/dst file */
{
    BOOL result;
    CHAR far *fpBuf;			    /* buffer for copying */
    unsigned uSize;			    /* size of allocated memory */

    uSize = 0x8000;			    /* mem allocation */
    while ((fpBuf = _fmalloc(uSize)) == NULL)
	if ((uSize >>= 1) < 16)
	    FatalError(smNoMemory, 2);

    while (!eof(fhSrc)) {
	if ((uSize = readf(fhSrc, fpBuf, uSize)) <= 0)	/* read source file */
	    FatalError(smLoadErr, 2);

	if (writef(fhDst, fpBuf, uSize) != uSize)	/* write dest. file */
	    FatalError(smWriteErr, 2);
    }

    _ffree(fpBuf);
}

/* combines WIN.COM, SETUP.LGO and SETUP.LGD to create WIN.COM */

void CreateWinCom()
{
    int fhSrc, fhDst;			    /* file handle for src/dst file */
    BOOL result;
    CHAR *s;				    /* string */
    CHAR **ps;				    /* pointer to string */
    CHAR Path[PATHMAX + 13];
    int iLen;
    extern CHAR *LogoText[];

    PutFileName(inserttext[D_COMFILE]);
    DisplayStatusLine(smWriting);

    if ((fhDst = open(FileName(WindowsPath, WinCom), O_WRONLY|O_BINARY))
								    == -1)
	FatalError(smOpenErr, 3);
    lseek(fhDst, 0l, SEEK_END);		    /* append to file */

    if ((fhSrc = open(s = FileName(WindowsPath, SetupLgo),
		      O_RDONLY|O_BINARY)) == -1)
	FatalError(smLoadErr, 3);

    CopyFileContents(fhSrc, fhDst);

    if (close(fhSrc) || remove(s))
	FatalError(smWriteErr, 3);

    if ((fhSrc = open(s = FileName(WindowsPath, SetupLgd),
		      O_RDONLY|O_BINARY)) == -1)
	FatalError(smLoadErr, 3);

    CopyFileContents(fhSrc, fhDst);

    if (close(fhSrc) || close(fhDst) || remove(s))
	FatalError(smWriteErr, 3);

    if (strcmp(WinCom, inserttext[D_COMFILE])) {    /* rename to com name */
	if (strlen(inserttext[D_COMFILE])) {
	    strcat(strcpy(Path, WindowsPath), inserttext[D_COMFILE]);
	    remove(Path);                           /* remove if already there */
	    if (rename(FileName(WindowsPath, WinCom), Path))
		FatalError(smWriteErr, 3);
	} else                                      /* no WIN.COM ! */
	    remove(FileName(WindowsPath, WinCom));
    }
    ClearStatusLine();
}


/* read with far pointer */

unsigned readf(fh, fp, uLen)
int fh;					/* file handle referring to open file*/
CHAR far *fp;				/* storage location for data */
unsigned uLen;				/* maximum number of bytes */
{
    union REGS inregs, outregs;
    struct SREGS segregs;

    inregs.x.bx = fh;			/* file handle */
    inregs.x.cx = uLen;			/* bytes to read */
    inregs.x.dx = FP_OFF(fp);		/* offset of buffer */
    segregs.ds	= FP_SEG(fp);		/* segment of buffer */
    inregs.h.ah = 0x3F;			/* Read Handle */
    intdosx(&inregs, &outregs, &segregs);

    return outregs.x.cflag ? 0 : outregs.x.ax;
}


/* write with far pointer */

unsigned writef(fh, fp, uLen)
int fh;					/* file handle referring to open file*/
CHAR far *fp;				/* storage location for data */
unsigned uLen;				/* maximum number of bytes */
{
    union REGS inregs, outregs;
    struct SREGS segregs;

    inregs.x.bx = fh;			/* file handle */
    inregs.x.cx = uLen;			/* bytes to write */
    inregs.x.dx = FP_OFF(fp);		/* offset of buffer */
    segregs.ds	= FP_SEG(fp);		/* segment of buffer */
    inregs.h.ah = 0x40;			/* Write Handle */
    intdosx(&inregs, &outregs, &segregs);

    return outregs.x.cflag ? 0 : outregs.x.ax;
}

/* CopyPifDirectory() -- if pif files are not already copied, it looks for
**                       a \pif directory, if there is one, it copies the dir
*/

void CopyPifDirectory()
{
    static BOOL CopyPifDone = FALSE;
    FINDTYPE findbuf;
    CHAR *pOldSPath = pSourcePath;
    CHAR SPath[PATHMAX + 17];
    CHAR DPath[PATHMAX + 17];

    if (CopyPifDone || (IsReinstall && !Is386))
	return;                         /* correct PIFs already there */

    if (ffirst(strcat(strcpy(SPath, pSourcePath), "pif\\*.*"), 0, &findbuf))
	return;                         /* no pif directory found */

    CopyPifDone = TRUE;
    pSourcePath = SPath;                /* set new source path */
    strcpy(DPath, WindowsPath);         /* store current destination path */
    strcat(strcpy(pSourcePath, pOldSPath), "PIF\\");
    strcat(WindowsPath, "PIF\\");
    PutDestName(WindowsPath);

    do {                                /* copy all files */
	CopyFile(findbuf.name, findbuf.name);
    } while (!fnext(&findbuf));

    pSourcePath = pOldSPath;            /* restore source path */
    strcpy(WindowsPath, DPath);         /* restore destination path */
    PutDestName(WindowsPath);
}

/* writes MSDOS.EXE if it is no runtime version */

void WriteMsDos()
{
    int fh;

    PutFileName(MsDos);

    if (!IsRuntime) {
	if (((fh = open(FileName(WindowsPath, MsDos),
		       O_CREAT|O_RDWR|O_TRUNC|O_BINARY,
		       S_IREAD|S_IWRITE)) == -1) ||
	    (write(fh, "\xCD", 1) < 1))
	    FatalError(smWriteErr, 2);
	close(fh);
    }
}

/* copy WIN.CNF from current directory to WIN.COM in WindowsPath */

void CopyWinCnf()
{
    pSourcePath = "";

    PutNumber(1);
    PutDestName(WindowsPath);
    if (!CopyFile("WIN.CNF", "WIN.COM"))
	FatalError(smNoFile, 5);

    ClearStatusLine();
}
