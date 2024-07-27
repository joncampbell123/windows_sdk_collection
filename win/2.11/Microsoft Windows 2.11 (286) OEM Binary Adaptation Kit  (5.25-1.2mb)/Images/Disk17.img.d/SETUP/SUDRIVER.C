/* SUDRIVER.C -- get driver information from OEM diskette.
**
** These are routines for handling reading OEM driver diskettes for
** keyboard, mouse, display, and printer drivers, building menus for
** selection of these drivers, and selecting the drivers.
**
** This module is an extension to SUSEL.C, and shares the arrays for
** menu selection.
**
** History
**
** 11 jun 87	plb	Begun module. 
** 12 jun 87	plb	Added resolution scanning (call ConvertRes()
**			in SUINF), and set charset of OEM keyboard records
**			to 0 to force SUSEL to list all OEM fonts.
** 14 jun 87	plb	Combining .H files, moving extern declarations
**			to SETUP2.H. Use OutOfMemory() when malloc() fails.
** 15 jun 87	plb	Factored out SETUP0.H
** 16 jun 87	plb	Made char vars unsigned.
** 17 jun 87	plb	Wrote FileExists(), LogGrbExist().
** 18 jun 87    plb     Now explicitly include stdio.h, etc (no more setup0.h).
** 18 jul 87    mp      LogGrbExist() renamed to DispExist() and SetOemDriver()
**                      changed for reading of 386 display dependent files
**                      Eliminating MakeLogGrbNames()
** 19 jul 87    mp      Reading of System and Terminal fonts from OEM diskette
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dos.h>
#include <newexe.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>
#include <string.h>

#include "setup.h"

/* imported from SUDATA -- not in setup.h
*/
extern CHAR sInsertDisk[];
extern CHAR smNoSelection[];
extern textline CopyMsg[];

/* imported from SUSEL
*/
void GetFileRoot(CHAR *, CHAR *);
extern CHAR logext[];
extern CHAR grbext[];
extern CHAR d386ext[];
extern CHAR d3exext[];

/* imported from SUMAIN
*/
extern BOOL Is386;

/* statics
*/
static int sel[] = { S_LOGO, S_GRABBER, S_386, S_3EX};
static CHAR *ext[] = { logext, grbext, d386ext, d3exext};

/* external declarations from suinf.c
*/
int ConvertRes(CHAR *, int[]);

/* some entirely local variables...
*/
static int ndrivers;    /* number of driver records, this driver */
static int ndescrip;	/* actually >= no. of files for Printers and KEYBOARD */
			/* due to multiple description strings */
static int recordsize;	/* size of 'union dchar' record for this type file */

/* some forward declarations */

BOOL IsType(int, char *, char *);
char * skipcolon(char *);
char * skipspaceorchar(char *, char);
char * GetOemDesc(char *, char *);
BOOL DispExist(CHAR *, CHAR *);
BOOL FileExists(CHAR *, CHAR *);

/* GetOemDriverList() -- Scan OEM driver diskette or path, find all drivers of
** specified type, adds "no selection.." message at the top of the list.
**
** Inputs -- see below.
**
** Output -- returns count of description strings in menu[] array.
*/

int GetOemDriverList(iType, pPathInput)
int iType;		/* must be S_DISPLAY, S_MOUSE, S_KEYBOARD, S_PRINTER */
CHAR * pPathInput;     /* path to search */
{
    FINDTYPE buff;
    int i;
    int n;
    int ch;
    char pLib[30];
    char pDescr[DESCMAX];
    char pSearch[130];
    char pPath[80];

    /* Display a message */
    ShowReading(sInsertDisk);

    switch (iType)
	{
	case S_DISPLAY: recordsize = DFDISPLAYSIZE;
			break;
	case S_MOUSE:   recordsize = DFSIZE;    /* plus filename len. */
			break;
	case S_KEYBOARD:recordsize = DFKBSIZE;
			break;
	case S_PRINTER: recordsize = DFIODEVSIZE;
			break;
	case S_SYSFONT: recordsize = DFFONSIZE;
			break;
	case S_OEMFONT: recordsize = DFOEMSIZE;
			break;
	}

    ndrivers = 0;
    ndescrip = 0;
    menu[0] = NULL;

    strcpy (pPath, pPathInput);

    /* pPath must end in colon or backslash */

    if (pPath[0] != 0)
	{
	ch = pPath[strlen(pPath) - 1];
	if ((ch != '\\') && (ch != ':'))
		strcat(pPath, "\\");
	}

    strcat(strcpy(pSearch, pPath), iType == S_SYSFONT || iType == S_OEMFONT ?
							"*.fon" : "*.drv");

    for( n = 0;
	 0 == (i = (n == 0) ? ffirst(pSearch, 0, &buff) : fnext( & buff) );
	 n++)
	{
	if (IsNewExe(pPath, buff.name, pLib, pDescr))
	    if (IsType(iType, pLib, pDescr) )
		{
		if ((iType != S_DISPLAY) ||
		    ((iType == S_DISPLAY) && DispExist(buff.name, pPath)))
		    /* Ok, create the record for the driver */
		    GetOemDriver(iType, buff.name, pLib, pDescr);
		}
	}

    menu[ndescrip] = NULL;	/* NULL terminate menu array. */
    return(ndescrip);

} /* GetOemDriverList() */


/* IsType() -- determine if driver is of specified type
*/
BOOL IsType(iType, pLib, pDescr)
int iType;      /* must be S_DISPLAY, S_MOUSE, S_KEYBOARD,
		**         S_PRINTER, S_SYSFONT, S_OEMFONT */
char * pLib;
char * pDescr;
{
    char sName[11];
    char ch;
    int i;

    switch(iType)
	{
	case S_KEYBOARD:
		return(0 == strcmp("KEYBOARD", pLib));

	case S_DISPLAY:
		return(0 == strcmp("DISPLAY", pLib));

	case S_MOUSE:
		return(0 == strcmp("MOUSE", pLib));

	case S_PRINTER:
		/* get 0-terminated string from beginning of pDescr */
		for (i = 0; (i < 10) && (' ' != (ch = pDescr[i])); )
		    sName[i++] = ch;
		sName[i] = 0;
		/* Now compare */
		return(0 == strcmp("DDRV", sName));

	case S_SYSFONT:
		return(0 == strcmp("FONTS", pLib));

	case S_OEMFONT:
		return(0 == strcmp("OEMFONTS", pLib));

	default:
		return(FALSE);
	}

} /* IsType() */


/* GetOemDriver() -- create a record for an OEM driver file (on a diskette).
**
** The description string in the new EXE header for the driver is parsed,
** and a 'union dfile' record is created for the driver.
** Description string(s) are created for the driver (possibly multiple
** ones if it is a printer or keyboard driver, and linked to the 'menu[]'
** array. Pointers to the 'union dfile' records are placed in the 
** menuptr[] array, and mapping from indices to menu[] to indices to
** menuptr[] is placed in menumap[].
** The description pointers in the union dfile records are NULL, and
** so are the links.
*/

GetOemDriver(iType, pName, pLib, pDescr)
int iType;      /* must be S_DISPLAY, S_MOUSE, S_KEYBOARD,
		**         S_PRINTER, S_SYSFONT, S_OEMFONT */
char * pName;	/* pointer to file name */
char * pLib;	/* pointer to library name */
char * pDescr;	/* pointer to description string */
{
    char * p;           /* pointer to description string */
    char * pRes = NULL; /* pointer to resolution string */
    char sDesc[80];	/* description substring */
    union dfile * pd;
    int rsize;
    int i;

    /* display status */
    ShowReading(pName);

    /* allocate space for record */

    rsize = recordsize;
    if (iType == S_MOUSE)
	rsize += strlen(pName);
    if (NULL == (pd = (union dfile *) malloc(rsize)))
	OutOfMemory();

    /* Put filename in record, and put NULL in pointer fields,
    ** and set disk number to -1. */

    strcpy(pd->n.fname, pName);
    pd->n.p = NULL;
    pd->n.descr = NULL;
    pd->n.d = -1;

    /* If menu is empty, place "no selection.." entry at the top of the list
    */
    if (ndrivers == 0)
	{
	menuptr[0] = NULL;
	menu[0] = smNoSelection;
	menumap[0] = -1;
	ndescrip++;
	}

    /* put pointer to record in menu pointer array. This is how the
    ** menu selection routine will find the driver information. */

    menuptr[ndrivers] = pd;

    /* Handle description string(s), init. keyboard char. set. */

    switch(iType)
	{
	case S_DISPLAY:
		/* for display, description is rest of pDescr[]
		** string, after second colon */
		pRes = skipcolon(pDescr);
		p = skipcolon(pRes);
		PutOemDesc(p);
		break;

	case S_OEMFONT:
		/* clear char. set */
		pd->oemfon.charset = 0;
		/* no break!! */

	case S_SYSFONT:
		/* for fonts, description is rest of pDescr[]
		** string, after colon */
		p = pDescr + 7;         /* skip "FONTRES" */
		pRes = skipspaceorchar(p, ':');
		p = skipcolon(p);
		PutOemDesc(p);
		break;

	case S_MOUSE:
		/* for mouse, use whole description string */
		PutOemDesc(pDescr);
		break;

	case S_KEYBOARD:
		/* incidentally, clear keyboard char. set. */
		pd->kb.charset = 0;
		p = pDescr + 8;         /* skip "KEYBOARD" */
		/* no break!! */

	case S_PRINTER:
		/* keyboard and printer:
		** These have (possibly) multiple descriptions, separated
		** by commas.  Make an entry for each one in the
		** menu[] and menumap[] arrays.
		*/
		if (iType == S_PRINTER) {
		    p = pDescr + 4;     /* skip "DDRV" */
		    pRes = skipcolon(p);
		}
		p = skipspaceorchar(p, ':');
		while (NULL != (p = GetOemDesc(p, sDesc)))
			PutOemDesc(sDesc);
		break;
	}

    /* handle resolution values for fonts, display and printer */

    if (pRes)
	{
	/* use ConvertRes on the resolution string.. but first,
	** if first char is not a digit, force 0-length string.
	*/
	if (('0' > *p) || ('9' < *p))
		*p = 0;
	if (iType == S_PRINTER)
	    {
	    pRes += ConvertRes(pRes, pd->iodev.res1);
	    if (0 == pd->iodev.res1[0])
		pd->iodev.res1[1] = 1;
	    ConvertRes(pRes, pd->iodev.res2);
	    }
	else
	    {
	    ConvertRes(pRes, pd->fon.res);
	    }
	}

    ndrivers++;

} /* GetOemDriver() */


/* GetOemDesc() -- read next description substring.
**
** input:	p points to or 'just before' next descr. substring.
**		sDesc is array to copy substring into.
** output:	return value is next char. in desc. string to examine.
**		it is NULL if a colon or 0 character is read.
*/
char * GetOemDesc(p, sDesc)
char * p;
char * sDesc;
{
    int i;
    char * pp;
    char ch;

    i = 0;

    /* move past any comma or leading blank */
    pp = skipspaceorchar(p, ',');

    /* if this is not end of descr. list, read another */
    if (((ch = *pp) == ':') || (ch == 0))
	pp = NULL;
    else
	{
	while ((':' != (ch = *pp)) && (',' != ch)  && (0 != ch))
		{
		sDesc[i++] = ch;
		pp++;
		}
	}

    /* remove trailing blanks */
    while ((i > 0) && (' ' == sDesc[i - 1]))
	i--;

    sDesc[i] = 0;	/* 0-terminate */
    return(pp);

} /* GetOemDesc() */



/* skipcolon() -- skip past colon in string, and any spaces following it,
** but not past end of string.
**
** returns adjusted pointer to string.
*/
char * skipcolon(p)
char *p;
{
    char ch;

    /* skip to colon */
    while( (':' != (ch = *p)) && (0 != ch))
	p++;

    /* skip past colon */
    if (ch == ':')
	p++;

    /* skip past following spaces */
    while( (' ' == (ch = *p)) && (0 != ch))
	p++;

    return(p);

} /* skipcolon() */

/* skipspaceorchar() -- skip past any spaces or specified char.
*/
char * skipspaceorchar(p, chSkip)
char *p;
char chSkip;
{
    char ch;

    while( ((' ' == (ch = *p)) || (chSkip == ch)) && (0 != ch))
	p++;

    return(p);

} /* skipspaceorchar() */



/* PutOemDesc() -- save description string for OEM driver.
Allocate space for a description string, and copy to it.
*/

PutOemDesc(pDescr)
char * pDescr;	/* pointer into whole desc. string */
{
    char * p;

    if (ndescrip < MENUSIZE - 1)
	{
	p = malloc(strlen(pDescr) + 1);
	if (p == NULL)
	    OutOfMemory();
	strcpy(p, pDescr);
	menu[ndescrip] = p;
	menumap[ndescrip++] = ndrivers;
	}

} /* PutOemDesc() */

/* SetOemDriver()
**
** This points the selection array to the record pDriver().
** If iType is S_DISPLAY, then records are created for the corresponding
** display dependent files.
** All these records have the disk number entry set to -1 .. CopyDisk()
** will set the values back to 0 for files it copies.
** All memory allocated for the menu data structures, except the selected
** records, is disposed of.
** 
*/
SetOemDriver(iType, pDriver)
int iType;
union dfile * pDriver;
{
    CHAR Name[9];               /* file name without extension */
    union dfile * pd;
    int i;

    switch(iType)
    {
    case S_DISPLAY:
	/* need to create records display dependent files corresp. to
	** this display (we already know the file exists, since
	** DispExist() was called).
	*/
	GetFileRoot(pDriver->n.fname, Name);
	for (i = 0; i < (Is386 ? 4 : 2); i++) {
	    if ((pd = (union dfile *) malloc(DFLOGGRBSIZE)) == NULL)
		OutOfMemory();
	    pd->loggrb.d = -1;
	    pd->loggrb.p = NULL;
	    strcat(strcpy(pd->loggrb.fname, Name), ext[i]);
	    StoreStandardItem(sel[i], pd);
	}
	/* Don't break -- fall through to main cases: */

    case S_KEYBOARD:
    case S_MOUSE:
    case S_OEMFONT:
    case S_SYSFONT:
	StoreStandardItem(iType, pDriver);

    }

    /* dispose of menu structures, no longer needed */
    DeleteOemRecords(pDriver);

    /* finally, get CopyDisk() to load the file(s) */
    ScrClear();
    ScrDisplay(CopyMsg);
    CopyDisk(-1, CD_OEMDRIVER);
}

/* This deletes all records pointed to in the menu[], menumap[], menuptr[]
** arrays, except the one pointed to by pDriver.  Entries in menu[] and
** menuptr[] pointing to deleted (free'd) records or strings are set to NULL.
** All description strings are also deleted.
** (this may be called with pDriver == NULL if ALL the records are to be
** deleted).
**/
DeleteOemRecords(pDriver)
union dfile * pDriver;
{
    int i;
    int j;
    CHAR * p;
    union dfile * pd;

    for (i = 0; NULL != (p = menu[i]); i++)
	{
	if ((j = menumap[i]) >= 0) {
	    free(p);
	    menu[i] = NULL;
	    pd = menuptr[j];
	    if ((pd != NULL) && (pd != pDriver))
		{
		free((char *) pd);
		menuptr[j] = NULL;
		}
	    }
	}

} /* DeleteOemRecords() */


/* DispExist() -- determine display dependent files exist for a display
** driver.
**
**/
BOOL DispExist(pDisplayName, pPathInput)
CHAR * pDisplayName;
CHAR * pPathInput;     /* path to find file in */
{
    char Name[9];
    char NameBuf[13];
    int i;

    GetFileRoot(pDisplayName, Name);
    for (i = 0; i < (Is386 ? 4 : 2); i++) {
	if (!FileExists(strcat(strcpy(NameBuf, Name), ext[i]), pPathInput))
	    return(FALSE);
    }
    return(TRUE);
} /* DispExist() */

/* FileExists() -- determines if file exists.
**
** Input:	sName -- a filename.
**		sPathInput -- a path name, terminated by colon or '\',
**		or 0-length.
** Returns:	TRUE iff the file exists.
*/

BOOL FileExists(sName, pPathInput)
CHAR * sName;
CHAR * pPathInput;     /* path to find file in */
{
    int fh;
    BOOL ok;
    CHAR pPath[PATHMAX];

    strcat(strcpy(pPath, pPathInput), sName);
    if (ok = (-1 < (fh = open(pPath, O_RDONLY, NULL))))
	{
	close(fh);
	/* display logo filename */
	ShowReading(sName);
	}
    return(ok);

} /* FileExists() */


ShowReading(ps)		/* status message */
CHAR * ps;
{
    if (!IsQuick)
	{
	PutFileName(ps);
	DisplayStatusLine(smReading);
	}

} /* ShowReading(ps) */
