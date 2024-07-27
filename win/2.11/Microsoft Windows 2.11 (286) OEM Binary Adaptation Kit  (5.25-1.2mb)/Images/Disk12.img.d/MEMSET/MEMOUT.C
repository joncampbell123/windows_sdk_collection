/** MEMOUT.C : ModifyConfig() ***********************************

Program: memset

Module:	 MemOut


*****************************************************************




	PLEASE PUT NOTES in HISTORY (below) WHEN YOU EDIT
	THIS FILE.




*****************************************************************
some history
 21 jun 88	peterbe		Rearranged logic of output messages.
				txRerunSavedSetup eliminated.
 17 jun 88	peterbe		cosmetic changes, textpos() on return
				removed.
 16 jun 88	peterbe		DEVICE= is now UpperCase
 14 jun 88	peterbe		DriveIsHarddisk() calls removed.
				Bad path causes reprompt for boot drive
				and path input. (Bugs 1473, 1486) 
*****************************************************************

This module contains the ModifyConfig() function, which

--	Prompts the user for the destination path for the EMM
	and/or SMARTDRV drivers,

--	Constructs CONFIG.SYS command lines for the Emm driver
	and/or SmartDrv driver from the path and the SmLine[] and
	SmLine[] strings.

--	Copies the drivers to the specified directory,

--	Backs up the CONFIG.SYS file to CONFIG.BAK, if this
	has not already been done,

--	Removes any reference to the VDISK driver in the old
	CONFIG.SYS, and any old SMARTDRV and EMM driver lines,

*****************************************************************/

#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <direct.h>
#include <stdlib.h>
#include <errno.h>
#include <sys\types.h>
#include <sys\stat.h>

#include "memdef.h"
#include "setup.h"

/* forward declarations of local functions */

static VOID GetPath(char *, char *, char *);
static VOID FileCopy(char *, char *);
static VOID ReadConfig(BOOL, char *);
static int buildarray(NPBYTE);
static VOID WriteConfig(char *, char **, int, BOOL);
static VOID WriteNew(char *, char *, char *);

VOID ExitError(BOOL, textline *);

/* Static disk error flag */
static BOOL DiskError = FALSE;

/* static data for time/date of backup file */
static unsigned date,time;
static int in_time, out_time, out_flush;

/* global strings filled in here for SmartDrv filename and command */

extern char SmName[13];		/* will be "Smartdrv.sys" */
extern char SmLine[81];		/* e.g "SMARTDRV.SYS 192" */

/* global strings filled in here for SmartDrv filename and command */

extern char EmmName[13];	/* will be some driver name */
static char EmmHelp[20];	/* will be some HelpFile name */
extern char EmmLine[81];	/* e.g "EMM.SYS at 208" */

/* imported screen images and strings. */
extern CHAR * InsertText[];

extern textline txBootDrive[];
/* extern textline txBootBad[]; */

extern textline txNotMade[];
extern textline txDriverPath[];

/* screens for FileCopy() */
extern textline txNeedsFile[];	/* "MEMSET needs to copy \013 ...... */
extern textline txCantFind[];	/* "MEMSET can not find \013 on ...... */
extern textline txInsertDisk[];	/* "Insert the Utilities 2 diskette... */
extern textline txUnable[];	/* "Can not create ... */

/* screens for CONFIG.SYS */
extern textline txTooLarge[];	/* CONFIG.SYS is too large */
extern textline txConfigSmart[]; /* Smartdrive information */
extern textline txConfigEmm[];	/* EMM driver information */
extern textline txRmVdisk[];	/* Remove Vdisk? */

extern char sNewDelete[];	/* Delete the following from CONFIG.SYS */
extern char sNewAdd[];	 	/* Add the following to CONFIG.SYS */

extern textline txChangeConfig[];	/* Screen to Change CONFIG.SYS */
extern textline txSaveChoices[];	/* Choices */

/* screens for Exit from MEMSET */

extern textline txCouldNotSave[];	/* Error in writing CONFIG.BAK */
extern textline txNotSaved[];		/* CONFIG.SYS not saved */
extern textline txSaved[];		/* CONFIG.SYS saved */

extern textline txRerunMemset[];	/* exit, for Exp mem manager*/
extern textline txRerunSaved[];		/* .. did update CONFIG.SYS */
extern textline txRerunNotSaved[];	/* .. did NOT update CONFIG.SYS */


/* constants */
#define BUFSIZE 2 * 1024	/* buffer size for file copy **
				** Must be big enough for ALL of
				** config.sys */
#define MAXTABLE 50		/* max lines handled in config.sys */

/* file copy buffer, and pointer array to it. */

static char CopyBuf[BUFSIZE];

static char *npchTable[MAXTABLE];	/* arrays of pointers into CopyBuf[] */
static char *npchTableOld[MAXTABLE];

/* static variables */

static BOOL fDiskA = FALSE;

static char DEVICEEQ[] = "DEVICE=";
static char SETUP[] = "Setup";
static char MEMSET[] = "Memset";

static char sVdisk[81];	/* command line for VDISK */

/* local string for complete command line */
static char sCom[81];

#ifdef DEBUG1

/* messages for checking bFromSetup */

textline isMemset[] = {
{1,1, "MEMSET: DEBUG1 only message:"},
{3,1, "ModifyConfig() reports program is run standalone"},
{0,0,NULL}
};

textline isSetup[] = {
{1,1, "MEMSET: DEBUG1 only message:"},
{3,1, "ModifyConfig() reports program is run from Setup"},
{0,0,NULL}
};

#endif


/* ********************************************************************

FUNCTION
	ModifyConfig()

INPUTS
	the global SmName	Name of SmartDrive,
				null if not to be installed.

	the global SmLine	Command line for SmartDrive,
				starting with filename.

	the global EmmName	Name of Emm driver,
				null if not to be installed.

	the global EmmLine	Command line for Emm driver,
				starting with filename.

	parameter sBootDrive	String containing drive letter
				of boot drive (null if not
				supplied by Setup).
	
	parameter bBackedUp	If FALSE, ModifyConfig will back up
				CONFIG.SYS to CONFIG.BAK

	parameter bFromSetup	Determines (here) insertion string
				(program name) in text.

OUTPUT
	No output values returned.
******************************************************************** */

ModifyConfig(sBootDrive, bBackedUp, bFromSetup,
	nkConv, nkExt, nkExp)
char * sBootDrive;	/* volume letter is all that's used */
BOOL bBackedUp;		/* CONFIG.SYS is backed up */
BOOL bFromSetup;	/* Run from Setup */
int	nkConv;		/* Size of conventional memory in kBytes */
int	nkExt;		/* Size of extended memory in kBytes */
int	nkExp;		/* Size of expanded in kBytes */
{
    int i, j;
    char sTemp[100];

    /* Boot drive (location of CONFIG.SYS), and path of drivers. */
    char sBoot[4];
    char sPath[81];

    /* Full pathname for CONFIG.SYS and CONFIG.BAK and CONFIG.NEW */
    char sConfig[14];			/* C:\CONFIG.SYS */
    char sConfigOld[14];		/* C:\CONFIG.BAK */
    char sConfigNew[14];		/* C:\CONFIG.NEW */

    /* local strings or pointers for filenames, commands, and file paths */
    char * pName;
    char * pLine;

    char sSrc[20];
    char sDst[81];

    /* variables for CONFIG.SYS */
    int iLines;
    int iLinesOld;
    char sNKConv[7];
    char sNKExt[7];
    char sNKExp[7];

    BOOL fSmart;		/* TRUE if SmartDrive, FALSE if EMM */

    /* *** code *** */

#ifdef DEBUG1

ScrClear();
if (bFromSetup)
    ScrDisplay(isSetup);
else
    ScrDisplay(isMemset);

ContinueOrQuit();

#endif

    if (nkConv < 0)
	nkConv = 0;
    if (nkExt < 0)
	nkExt = 0;
    if (nkExp < 0)
	nkExp = 0;

    /* Determine whether this is a Smartdrive or EMM driver by examining
    ** the SmName string, and set flags and copy names accordingly.
    */

    if (*SmName)
	{
	fSmart = TRUE;
	pName = SmName;
	pLine = SmLine;
	}
    else
	{
	fSmart = FALSE;
	pName = EmmName;
	pLine = EmmLine;
	}


    /* Get the boot drive and the destination path for the drivers,
    ** and create the drivers directory if necessary.
    ** When done, path will terminate in a backslash.
    */
    strcpy(sBoot, "0:\\");
    *sBoot = *sBootDrive;
    strupr(sBoot);
    /* if drive letter supplied was '0' or otherwise bad, set it to 'C' */
    if ((*sBoot < 'A') || (*sBoot > 'Z'))
	* sBoot = 'C';
    GetPath(sBoot, pName, sPath);

    /* Create the complete CONFIG.SYS command lines:
    **
    **   "DEVICE=" + path + filename + parameters.
    **
    ** and create source and destination paths for the drivers.
    */

    /* create command line */
    strcpy(sCom, DEVICEEQ);		/* Device= */
    strcat(sCom, sPath);		/* path */
    strcat(sCom, pLine);		/* filename and parameters */

    /* create full path strings */
    strcpy(sSrc, "A:");			/* drive A */
    strcat(sSrc, pName);		/* filename */

    strcpy(sDst,sPath);			/* dest. path */
    strcat(sDst,pName);			/* filename */

    /* Copy driver to destination drive from A: */
    FileCopy(sSrc, sDst);


    /* Create full pathnames for CONFIG.SYS, CONFIG.BAK, CONFIG.NEW. */
    /* setting the drive letter of the boot drive. */
    strcpy(sConfig, "C:\\CONFIG.SYS");
    strcpy(sConfigOld, "C:\\CONFIG.BAK");
    strcpy(sConfigNew, "C:\\CONFIG.NEW");

    /* Put in the drive letter of the boot drive. */
    sConfig[0] = sConfigOld[0] = sConfigNew[0] = * sBoot;

    /* Read the present contents of CONFIG.SYS into CopyBuf[] */
    ReadConfig(bFromSetup, sConfig);

    /* Set up buffer for editing. EOL's become NUL's, pointers
    ** to the lines go into npchTable, and a count is returned.
    ** The count is saved in 2 places, for outputting the backup.
    */
    iLinesOld = iLines = buildarray(CopyBuf);

    /* Check for VDISK in CONFIG.SYS and display a screen if it's there.
    ** Also, remove duplicate drivers. */

    sVdisk[0] = 0;

    for (i = 0; i <= iLines; i++) {
        strcpy(sTemp, npchTable[i]);               /* copy the string    */
        strlwr(sTemp);                             /* make it lower case */

        /* VDISK is not compatable with Windows. Give the user a choice as
         * to whether to delete it from CONFIG.SYS.
         */

        if (strstr(sTemp, "vdisk.sys")) {
	    InsertText[006-1] = npchTable[i];	/* display command */
	    strcpy(sVdisk, npchTable[i]);	/* save for later */
            if (YesNo(txRmVdisk)) {
		if (i == (iLines - 1))
		    iLines--;
                else {
		    for (j = i + 1; j <= iLines; j++)
			npchTable[j - 1] = npchTable[j];
		    iLines--;
		    i--;
		}
            }
        }

        /* Remove drivers which duplicate ones this program installs. */

        else	if ( ((!fSmart) &&		/* One of the following ... */
                      ( strstr(sTemp, "ps2emm.sys") ||
			strstr(sTemp,    "emm.sys") ||
			strstr(sTemp,   "remm.sys")
		     )) ||
		     (fSmart && strstr(sTemp, "smartdrv.sys")) ) {
		    if (i == (iLines - 1))
			iLines--;
		    else {
			for (j = i + 1; j < iLines; j++)
			    npchTable[j - 1] = npchTable[j];
			iLines--;
			i--;
		    }
		}
    }

    /* Append new command line to CONFIG.SYS line list */

    npchTable[iLines++] = sCom;

    /* Display memory sizes and command lines for CONFIG.SYS */
    ScrClear();

    if (fSmart)	/* SMARTDrive command line info. */
	{
	/* setup to display screen with memory sizes and command line. */
	InsertText[014-1] = itoa(nkConv, sNKConv, 10);
	InsertText[016-1] = itoa(nkExt, sNKExt, 10);
	InsertText[017-1] = itoa(nkExp, sNKExp, 10);
	InsertText[006-1] = sCom;
	ScrDisplay(txConfigSmart);
	}

    else	/* Emm command line info. */
	{
	/* setup to display screen with command line and help file name. */
	InsertText[006-1] = sCom;	/* command line */
	InsertText[007-1] = pName;	/* file name */
	strcpy(EmmHelp, EmmName);
	for (i = 0; (i < 9) ; i++)
	    if (EmmHelp[i] == '.')
		{
		EmmHelp[i] = 0;
		break;
		}
	strcat(EmmHelp, ".TXT");
	InsertText[010-1] = EmmHelp;	/* help file */
	ScrDisplay(txConfigEmm);
	}

    /* Now, add to the screen displayed above ---
    **
    ** Chose between saving CONFIG.SYS and just writing changes in
    ** CONFIG.NEW
    */

    ScrDisplay(txChangeConfig);

    if (ScrSelectChar(txSaveChoices, 0) == 0)
	{
	/* Write backup file "CONFIG.BAK" */
	if ((!bFromSetup) || (!bBackedUp))
	    WriteConfig(sConfigOld, npchTableOld, iLinesOld, TRUE);
	    if (DiskError)
		{
		ScrClear();
		ExitError(bFromSetup, txNotSaved);
		}

	/* Write updated "CONFIG.SYS" */
	if (!DiskError)
	    WriteConfig(sConfig, npchTable, iLines, FALSE);
	if (DiskError)
	    {
	    ScrClear();
	    ScrDisplay(txCouldNotSave);
	    ExitError(bFromSetup, txNotSaved);
	    }

	ScrClear();
	if (fSmart)
	    /* Smartdrive was installed */
	    {
	    ScrDisplay(txSaved);
	    }
	else
	    /* message for rebooting if EMM driver is installed */
	    {
	    InsertText[013-1] = pName;
	    ScrDisplay(txRerunSaved);
	    ScrDisplay(txRerunMemset);
	    }
	}
    else
	{
	/* Save CONFIG.NEW */
	WriteNew(sConfigNew, pName, sCom);

	ScrClear();
	if (fSmart)
	    ScrDisplay(txNotSaved);
	else
	    {
	    /* message for rebooting if EMM driver is installed */
	    InsertText[013-1] = pName;
	    ScrDisplay(txRerunNotSaved);
	    ScrDisplay(txRerunMemset);
	    }
	}

    /* Return to main() and exit.. */

	/* textpos((fSmart) ? 21 : 23, 1); */ /* handle in main() */

} /* end ModifyConfig() */

/****************************************************************************

FUNCTION GetPath

    First, prompt the user with the drive path passed in sBoot, and
    allow the user to change this.

    Then, get the destination path for the drivers, check it, and
    create the directory if it isn't the root and doesn't exist.
    Repeat or abort if there is an error creating the directory.
    When done, path should terminate in a backslash.

****************************************************************************/

static VOID GetPath(sBoot,pName, sPath)
char * sBoot;		/* this is BOTH input and output */
char * pName;		/* name of file for screen */
char * sPath;		/* output: path of drivers. */
{
    char c;
    char sTemp[20];
    BOOL dirnotmade;
    struct stat statbuf;		/* defined in stat.h */
    int iLen;

    /* Get any change to the boot drive from the user. Accept any
    ** designation A-Z -- user terminates with ':' or ':\'.  This
    ** routine will copy the first character back to sBoot[]. */

    do {	/* .. while (dirnotmade) */

	/* allow the user to change the boot drive.
	** Don't save it yet, until the path is checked.
	*/
	ScrClear();
	ScrDisplay(txBootDrive);
	ScrInputPath(sBoot, sTemp, 3);

	/* get the path for the drivers and check it. */
	ScrClear();
	InsertText[013 - 1] = pName;	/* put filename in screen text */
	ScrDisplay(txDriverPath);

	/* sPath contains the directory path for the drivers */
	ScrInputPath(sTemp, sPath, 40);

	/* If sPath() included a trailing backslash, then delete it. */
	iLen = strlen(sPath);
	if (sPath[iLen - 1] == '\\')
	    sPath[--iLen] = 0;

	/* If the path is a subdirectory, attempt to create
	** the directory. If it already exists, mkdir()
	** will return a nonzero value, in which case the user must
	** try again or quit.
	**/

	dirnotmade = FALSE;

	if ((iLen = strlen(sPath)) > 3) { /* is the path a subdirectory? */
	    if (dirnotmade = mkdir(sPath)) {
		/* Existing file, directory or device? */
		if (EACCES == errno) {
		    /* Yes, is it a directory? */
		    stat(sPath, & statbuf);
		    /* if so, it's OK! */
		    if(S_IFDIR & statbuf.st_mode)
			dirnotmade = FALSE;
		}
	    }
	}

	/* if there was an error, display a message and try again */
	if (dirnotmade) {
	    ScrClear();
	    ScrDisplay(txNotMade);
	    ContinueOrQuit();
	}

    } while (dirnotmade);

    /* if the directory was OK, set the boot drive */
    *sBoot = *sTemp;

    /* add terminating backslash if necessary */
    if (sPath[iLen - 1] != '\\')
	strcat(sPath, "\\");

} /* end GetPath() */



/****************************************************************************
    FUNCTION: FileCopy(sSrc, sDest)

    Copies file with path sSrc to file with path sDest, prompting
    the user to insert a diskette in drive A:, if it is not already
    there.

****************************************************************************/

static VOID FileCopy(sSrc, sDest)
char * sSrc;		/* source pathname including "A:" */
char * sDest;		/* destination pathname */
{
    FILE *fileIn, *fileOut;
    INT Count;

    /* Copy file sSrc from drive a: to specified path */

	InsertText[013 - 1] = sSrc + 2;
	strupr(sSrc + 2);

	/* prompt for putting diskette in drive A: if it wasn't
	** inserted for a previous file copy. */
	if (!fDiskA) 
	    {
	    ScrClear();
	    ScrDisplay(txNeedsFile);
	    ScrDisplay(txInsertDisk);
	    ContinueOrQuit();
	    }

	while ((fileIn = fopen(sSrc, "rb")) == NULL) {
	    /* if error, prompt (again) with slightly different message */
	    ScrClear();
	    ScrDisplay(txCantFind);
	    ScrDisplay(txInsertDisk);
	    ContinueOrQuit();
	}
	fDiskA = TRUE;         /* The disk is now in drive A: */

	fileOut = fopen(sDest, "wb");

	/* Exit if can't open output file */
	if (fileOut == NULL) {
	    InsertText[013 - 1] = sDest;
	    ScrDisplay(txUnable);
	    fclose(fileIn);
	    ContinueOrQuit();
	    ExitPgm();
	}

	/* file read/write (copy) loop */

	/* try to fill buffer from fileIn */
	while ((Count = fread(CopyBuf, 1, BUFSIZE, fileIn)) != 0) {
	    /* try to write 'Count' bytes from buffer to fileOut */
	    if (fwrite(CopyBuf, 1, Count, fileOut) != Count) {
		/* can't write what we just read: */
		InsertText[013 - 1] = sDest;
		ScrDisplay(txUnable);
		ContinueOrQuit();
		fclose(fileIn);
		fclose(fileOut);
		ExitPgm();
	    }
	}

	fclose(fileIn);
	fclose(fileOut);

} /* end FileCopy () */


/****************************************************************

FUNCTION: ReadConfig()

    This function reads all of CONFIG.SYS into CopyBuf[], and terminates
    it with a NULL byte.

    If the file is too big to fit, the program will terminate.

    If the file does not exist or is empty, the output will be a
    null string.

*****************************************************************/

static VOID ReadConfig(bFromSetup, sConfig)
BOOL bFromSetup;
char * sConfig;
{
    FILE *fileIn;
    INT Count;

    Count = 0;

    if ((fileIn = fopen(sConfig, "r")) != NULL) {

        /* Read in the file, abort if it is too large. */

        if ((Count = fread(CopyBuf, 1, BUFSIZE, fileIn)) == BUFSIZE) {
	    InsertText[013 - 1] = sConfig;
	    ScrClear();
	    ExitError(bFromSetup, txTooLarge);
        }

	/* Get date and time from file */
	in_time = _dos_getftime( fileno(fileIn), &date, &time);
        fclose(fileIn);
    }

    CopyBuf[Count] = 0;          /* make certain it's null terminated */

} /* end ReadConfig() */


/****************************************************************
FUNCTION: buildarray()
     The buildarray() function will convert all \n's to zeros, and build
     up a pointer array in npchTable[]. Config.sys can then be processed by
     reading/and writing a line at a time.

     This returns the number of lines in the buffer.

*****************************************************************/
static int buildarray(npBuf)
NPBYTE npBuf;
{
    int i = 0;

    npchTableOld[i] = npBuf;	/* backup of ptr to first line */
    npchTable[i++] = npBuf;	/* pointer to first line */

    /* When an EOL is found, convert it to 0 so it can be treated as a
    ** string, increase the pointer and place the address of the new string
    ** into the table of string addresses.
    ** 2 copies of the array are created; one will be edited, the other
    ** is for outputting the backup copy of CONFIG.SYS.
    **/

    while (*npBuf != 0) {
        if (*npBuf == '\n') {
            *npBuf++ = 0;
            npchTableOld[i] = npBuf;	/* save for backup */
            npchTable[i++] = npBuf;	/* save for editing */
	    if (i > MAXTABLE)
		{
		i--;
		break;
		}
        }
        else
            npBuf++;
    }
    return (i - 1);                         /* last pointer is not a string */

} /* end buildarray() */


/****************************************************************
FUNCTION: WriteConfig()

    This writes all iLineCount lines of either the original
    or the backup CONFIG.SYS file
    from the lines pointed to in npchT[], to the output
    file sOutput.

    WriteConfig(sConfig, npchTable, iLines, FALSE) writes edited CONFIG.SYS,
	with current time/date

    WriteConfig(sConfigOld, npchTableOld, iLinesOld, TRUE) writes
	backup CONFIG.BAK, with time & date of original file.

*****************************************************************/

static VOID WriteConfig(sOutput, npchT, iLineCount, SameTime)
char * sOutput;		/* Pathname of file */
char ** npchT;		/* pointer to pointer table */
int iLineCount;		/* number of lines */
BOOL SameTime;		/* set time/date to that of input file? */
{
    FILE *fileOut;
    int i;

    if ((fileOut = fopen(sOutput, "w")) != NULL)
	{
	for (i = 0; i < iLineCount; i++)
		{
		fprintf(fileOut, "%s\n", npchT[i]);
		}
	if (SameTime != FALSE)
	    {
	    /* set date/time of original CONFIG.SYS.  flush file to make
	    ** sure a write does not occur after the time is set.
	    */
	    out_flush = fflush(fileOut);
	    out_time = _dos_setftime( fileno(fileOut), date, time);
	    }
	fclose(fileOut);
	}
    else
	DiskError = TRUE;

} /* end WriteConfig() */

/****************************************************************
FUNCTION: WriteNew()

	This writes the file CONFIG.NEW, containing something
	like:

    =================================================
	Delete the following line from Config.sys:

	    DEVICE=C:\VDISK.SYS ...

	Add the following line to Config.sys:

	    DEVICE=SMARTDRV.SYS ...

    =================================================

*****************************************************************/

static VOID WriteNew(sNew, pName, pCom)
char * sNew;	/* prompt string in file */
char * pName;	/* filename (used as flag) */
char * pCom;	/* command line */
{
    FILE *fileOut;
    if ((fileOut = fopen(sNew, "w")) != NULL)
	{
	if (*sVdisk)
	    {
	    fprintf(fileOut,"%s\n\n", sNewDelete);
	    fprintf(fileOut,"    %s\n\n", sVdisk);
	    }

	if (*pName)
	    {
	    fprintf(fileOut,"%s\n\n", sNewAdd);
	    fprintf(fileOut,"    %s\n\n", pCom);
	    }

	fclose(fileOut);
	}

} /* end WriteNew() */

/* ExitError()
** Displays message
** if run from Setup, pauses for user input.
** returns with error code.
*/
VOID ExitError(bFromSetup, msg)
BOOL bFromSetup;
textline * msg;
{
ScrDisplay(msg);
ExitPgm();	/* positions cursor, exits with status 1 */

}	/* end ExitError() */
