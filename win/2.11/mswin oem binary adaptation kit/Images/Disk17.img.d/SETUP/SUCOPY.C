/* SUCOPY.C -- general copy of groups of files */

/*  History
**
**   8 jul 87   mp      start history
**  18 jul 87   mp      Display dependent files for Windows/386 added
**  28 jul 87   mp      eliminate flashing status message while writing bin/ovl
**                      copy PIF files
**  31 jul 87   mp      problem fixed: second copying from default drive
**  23 aug 87   mp      no installing of Terminal font for runtime
**  24 aug 87   mp      copying 386 files to second instead first diskette
**  31 jan 89   wch     removed references to IsTwoDiskette, Is360K and
**                      IsDiskette which were always FALSE
**   1 mar 89   wch     added static array iModuleWalk to alter order in which
**			modules are read in CopyDisk() so as to save memory
*/

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <string.h>
#include "setup.h"


/* copy bits: */
#define CB_OEM	      1	 /* file can be on extra OEM disk */
#define CB_RUNTIME    2	 /* file is important for runtime startup disk */
#define CB_STARTUP    4  /* file is important for nonruntime startup disk */

#define CB_DISK       8  /* appears on system diskette */
#define CB_RDISK      16 /* .. even if it is a runtime version */
#define CB_HARD       32 /* appears on if it is harddisk setup */
#define CB_RHARD      64 /* .. even if it is a runtime version */

#define CB_LOAD       128/* file is loaded into memory for BIN/OVL file */
#define CB_RENAME     256/* file is renamed while copying */

#define MAX_MODULE 13	/* maximal number of moduls in BIN/OVL */

/* Externals from SUBINOVL.C */
extern CHAR BinFile[];
extern CHAR OvlFile[];
extern int fhBO;

/* Externals from SUDISK.C */
extern void CopyPifDirectory();

/* External from sudest.c */
extern BOOL bCopyRamDrive;
extern char RamDrivePath[];

/* from susel.c */
extern union dfile * pselect[];


static int iCpyBits[] = {		/* copy bits: CB_.. for each filetype*/
					/* (Index is S_..) */
	   CB_RUNTIME|CB_STARTUP|  CB_LOAD,		  /* S_KERNEL */
	   CB_RUNTIME|CB_STARTUP|  CB_LOAD,		  /* S_GDI */
	   CB_RUNTIME|CB_STARTUP|  CB_LOAD,		  /* S_USER */
		      CB_STARTUP|  CB_LOAD,		  /* S_MSDOS */
			    CB_RHARD|CB_HARD,            /* S_RAMDRIVE */
	   CB_RUNTIME|CB_STARTUP|  CB_LOAD,		  /* S_SYSTEM */
    CB_OEM|CB_RUNTIME|CB_STARTUP|  CB_LOAD,		  /* S_KEYBOARD */
    CB_OEM|CB_RUNTIME|CB_STARTUP|  CB_LOAD,		  /* S_MOUSE */
    CB_OEM|CB_RUNTIME|CB_STARTUP|  CB_LOAD,		  /* S_DISPLAY */
	   CB_RUNTIME|CB_STARTUP|  CB_RENAME,		  /* S_OEMLOGO */
	   CB_RUNTIME|CB_STARTUP|  CB_LOAD,		  /* S_SOUND */
	   CB_RUNTIME|CB_STARTUP|  CB_LOAD,		  /* S_COMM */
    CB_OEM|CB_RUNTIME|CB_STARTUP|  CB_LOAD,		  /* S_SYSFONT */
    CB_OEM|           CB_STARTUP|  CB_LOAD,               /* S_OEMFONT */
		      CB_STARTUP|  CB_LOAD,		  /* S_MSDOSD */
				     CB_HARD|CB_RENAME,   /* S_WINOLDAP */
			    CB_RHARD|CB_HARD|CB_RENAME,   /* S_SPOOLER */
    CB_OEM|CB_RUNTIME|CB_STARTUP|  CB_RENAME,             /* S_LOGO */
    CB_OEM|         CB_DISK|         CB_HARD|CB_RENAME,   /* S_GRABBER */
    CB_OEM|CB_RDISK|CB_DISK|CB_RHARD|CB_HARD|CB_RENAME,   /* S_386 */
    CB_OEM|CB_RDISK|CB_DISK|CB_RHARD|CB_HARD|CB_RENAME,   /* S_3EX */
    CB_OEM|CB_RDISK|CB_DISK|CB_RHARD|CB_HARD,             /* S_PRINTER */
	   CB_RDISK|CB_DISK|CB_RHARD|CB_HARD,             /* S_FONT */
			    CB_RHARD|CB_HARD};            /* S_APPS */

static int iIdx[] = {			/* Index in fpbHead[] or pRename[] */
					/* (Index is S_..) */
    0,					/* S_KERNEL */
    9,					/* S_GDI */
    10,					/* S_USER */
    12,					/* S_MSDOS */
	 0,               /* S_RAMDRIVE , never used, just a dummy to accomodate for
												 a proper index for others that follow. */
    1,					/* S_SYSTEM */
    2,					/* S_KEYBOARD */
    3,					/* S_MOUSE */
    4,					/* S_DISPLAY */
     0,					/* S_OEMLOGO */
    5,					/* S_SOUND */
    6,					/* S_COMM */
    7,					/* S_SYSFONT */
    8,					/* S_OEMFONT */
    11,					/* S_MSDOSD */
     1,					/* S_WINOLDAP */
     2,					/* S_SPOOLER */
     3,                                 /* S_LOGO */
    -1,                                 /* S_GRABBER */
     4,                                 /* S_386 */
     5,                                 /* S_3EX */
};

/* BILLHU new array that somewhat inverts the above array iIdx
**   this array is used in CopyDisk to walk the modules in 'write order'
**   to save memory */
/* should be SSIZE entries here */
static int iModuleWalk[] = {
S_KERNEL,
S_RAMDRIVE,
S_SYSTEM,
S_KEYBOARD,
S_MOUSE,
S_DISPLAY,
S_OEMLOGO,
S_SOUND,
S_COMM,
S_SYSFONT,
S_OEMFONT,
S_GDI,
S_USER,
S_MSDOSD,
S_MSDOS,
S_WINOLDAP,
S_SPOOLER,
S_LOGO,
S_GRABBER,
S_386,
S_3EX,
S_PRINTER,
S_FONT,
S_APPS
};


static struct bl_info far *fpbHead[MAX_MODULE + 1];
					/* root pointer for each module */
					/* to blocks in memory */
static CHAR *pRename[] = {		/* strings for rename while copying */
    "SETUP.LGD",			/* (Index is number in iIdx) */
    "WINOLDAP.MOD",
    "SPOOLER.EXE",
    "SETUP.LGO",
    "WIN386.386",
    "WIN386.EXE"};

/* forward declarations local procedures */
static void BinWrite();


/* copies all needed files to startup or system diskette, */
/* prompt for inserting new diskette */

void Copy(iType)
int iType;				/* types: CD_STARTUP or CD_SYSTEM */
{
    static CHAR *pSrcPath = "";
    int iNextDisk;			/* next disk to be inserted */
    int iDisk;				/* current disk number */

    do {
	iNextDisk = 0;
	for (iDisk = iMinDisk; iDisk <= iMaxDisk; iDisk++) {
	    if (!(pSourcePath = DiskToHardPath(iDisk)))
		pSourcePath = pSrcPath;

	    if (CopyDisk(iDisk, iType) == CDR_IMPORTANT && !iNextDisk)
		iNextDisk = iDisk;
	    pSourcePath = pSrcPath;
	}
	if (iNextDisk) {
	    InsertDisk(iNextDisk);
	    pSrcPath = pSourcePath;
	}
    } while (iNextDisk);
}


/* tries to copy or load all files of disk number iDisk and returns whether
** the function succeeds or not (with the distinction there are important
** files or not).
** The function is used for 3 different purposes:
** 1. to copy and load OEM driver from the OEM diskette
** 2. to copy and load modules for the startup disk
** 3. to copy files on to the system disk
** Depending on the different cases the function copies only specific files
*/
int CopyDisk(iDisk, iType)
int iDisk;				/* disk number */
int iType;				/* CD_OEMDRIVER,CD_STARTUP,CD_SYSTEM */
{
union dfile *pd;			/* current file information */
int sno;				/* index to list of sel. files */
int iResult;			/* result of this function */
BOOL b;				/* reading ok? */

int iTestMask;			/* mask for testing bits whether file */
			/* is relevant */
int iImpMask = 0;			/* .. whether file is important */
char SavedPath[PATHMAX];
int i;

		
	if (iType == CD_OEMDRIVER)
		iTestMask = CB_OEM;
	else {
		if (iType == CD_STARTUP) {
			iImpMask = iTestMask = IsRuntime ? CB_RUNTIME : CB_STARTUP;
			if (!IsQuick)
				iTestMask |= IsRuntime ? CB_RHARD : CB_HARD;
		}
		else {
			iImpMask = iTestMask = (IsRuntime ? CB_RHARD : CB_HARD);
		}
	}
	PutDestName(WindowsPath);
	PutNumber(iDisk);

	iResult = DiskToHardPath(iDisk) ? CDR_OK : -1;

/* BILLHU space saving change of order
	for (sno = S_KERNEL; sno < SSIZE; sno++) {
*/
	for (i = 0, sno = iModuleWalk[i]; i < SSIZE; i++, sno = iModuleWalk[i]) {
		if (iCpyBits[sno] & iTestMask) {
/* BILLHU
			for (pd = RecNPtr(sno); pd; pd = pd->n.p) {
*/
    		for (pd = (sno < SSIZE) ? pselect[sno] : NULL; pd; pd = pd->n.p) {
				if (pd->n.d == iDisk) {
				    if (iResult < CDR_IMPORTANT) {
						if (iCpyBits[sno] & CB_LOAD) {
						    if (b = ReadModule(pd->n.fname,
										&fpbHead[iIdx[sno]]))
								BinWrite();
						} 
						else if (sno == S_GRABBER) {
						    b = ReadGrabber(pd->n.fname);
						}
						else if (sno == S_RAMDRIVE)	{
							if (bCopyRamDrive)	{
								strcpy(SavedPath, WindowsPath);
								strcpy(WindowsPath, RamDrivePath);
								b = CopyFile(pd->n.fname, pd->n.fname);
								strcpy(WindowsPath, SavedPath);
							}
							else
								continue;
						}
						else
						    b = CopyFile(pd->n.fname,
									 iCpyBits[sno] & CB_RENAME ?
										 pRename[iIdx[sno]] : pd->n.fname);
						if (b) {
						    iResult = CDR_OK;
						    pd->n.d = 0;   /* disk number = 0  ==>  ready */
						} 
						else {
					    	if (iResult == CDR_OK)
								FatalError(smNoFile, 6);
						    iResult = CDR_WRONGDISK;
						}
				    }
				    if (iResult == CDR_WRONGDISK && (iCpyBits[sno] & iImpMask))
					 /* ret */           return CDR_IMPORTANT;
				} /* right disk? */
				if (iCpyBits[sno] & (CB_LOAD | CB_RENAME))
				    break;        /* list of files not allowed */
			} /* all files of same type (like fonts, printers, apps) */
		} /* is type relevant? */
	} /* all types of files */

	if (iResult == -1)
		iResult = CDR_WRONGDISK;

	if (iType == CD_OEMDRIVER && iResult == CDR_WRONGDISK)
		FatalError(smNoFile, 6);

	if (iResult == CDR_OK && iType != CD_OEMDRIVER && !IsQuick) {
		CopyPifDirectory();
	}
	ClearStatusLine();

	return iResult;
}

/* tests whether modules can be written to the BIN file and does this */

static void BinWrite()
{
    static struct bl_info far **pfpb = fpbHead;
					/* pointer to module, */
					/* which has to be written next */
    PutFileName(BinFile);
    DisplayStatusLine(smWriting);

    while (*pfpb) {
	PrepModule(*pfpb);
	WriteModule(pfpb, BL_BIN);
	pfpb++;
	if (IsRuntime && (pfpb - fpbHead == iIdx[S_OEMFONT]))
	    pfpb++;                     /* skip OEMFONT */
    }
}


/* opens, writes and closes OVL file */

void OvlWrite()
{
    struct bl_info far **pfpb = fpbHead;  /* pointer to module infos */

    PutFileName(OvlFile);
    PutDestName(WindowsPath);
    DisplayStatusLine(smWriting);

    OvlOpen();

    while (*pfpb) {
	WriteModule(pfpb, BL_OVL);
	pfpb++;
	if (IsRuntime && (pfpb - fpbHead == iIdx[S_OEMFONT]))
	    pfpb++;                     /* skip OEMFONT */
    }

/* BILLHU
    OvlClose();
*/
    close(fhBO);

    ClearStatusLine();
}



