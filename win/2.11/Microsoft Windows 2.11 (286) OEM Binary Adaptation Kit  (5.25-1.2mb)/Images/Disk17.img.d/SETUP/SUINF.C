/* SUINF.C -- process SETUP.INF file for Windows Setup 2.00
**
** Errors in this module exit with return code 10
*/

/* History
** 1.10 Windows ===========================================================
**
** 09 jun 88	peterbe	Added code/data for [compatibility] section of
**			SETUP.INF
**
** 1.03 Windows ===========================================================
** 11 may 87	plb	Built skeleton
** 12 may 87	plb	Coding GetWinUniversal(), etc.
** 13 may 87	plb	Speeding character input (big buffer).
** 18 may 87	plb	Disk buffer is allocated temporarily.
**			Rewrote ReadDiskAndFile(), etc.
** 19 may 87	plb	Debugging..
**			Removed 'Select*()' routines.
**			Filenames are store UPPERCASE to ease comparison.
** 20 may 87	plb	Added SUINF.H, began code to read driver info.
**			GetWinUniversal() changed to GetWinInfo().
**			Started GetDriverList().
** 21 may 87	plb	Debugging and improving GetDriverList().
**			Added GetQString().
** 22 may 87	plb	Added processing of [oemlogos], [logos], [grabber],
**			[harddisk], [keyboard], [display], [sysfonts],
**			[oemfonts], [io.device].
**			Reorganized processing of [windows] section.
**			Changed GetDriverList() parameters to just kind index.
**			[machine] processing remains to be done.
** 27 may 87	plb	Fixed record sizing for printer font record.
**			Began handling of [machine] section.
** 27 may 87	plb	Finishing [machine] section input.
**  4 jun 87	plb	Fixed comment at end of GetFontRes().
**  9 jun 87	plb	Removed table entries for [pif] section.
**			[fonts] section SW_DFPRINTFON now has descr.
** 11 jun 87	plb	Factored ConvertRes() out of GetFontRes().
** 12 jun 87	plb	ConvertRes() returns string length.
**			InfLine() returns line number in SETUP.INF.
**			Added handling of iMaxDisk, iMinDisk.
** 13 jun 87	plb	InfLine() removed, since access to iLine is local.
**			Using PutNumber(), PutDestName(), PutSecName() for
**			fatal exit now.
** 14 jun 87	plb	Combining .H files. Removed DiskIsIncluded().
**			Added OutOfMemory() fatal exit when malloc() fails.
** 15 jun 87	plb	Added fString and code for it in RewindInfo() and
**			GetChar() so that semicolons can be in strings.
** 16 jun 87	plb	changed character variables to 'unsigned'.
**                      moved extern statements for sKernel, etc.
** 21 jul 87    mp      [data] entry added.
** 31 jul 87    mp      [diskette] entry added.
**  3 aug 87    mp      aliasing for 386 files
*/

/* This module contains functions for processing the SETUP.INF file,
** and turning it into lists of strings and other data structures, and
** for making menu selections of systems, drivers, and fonts from the
** files on the standard diskette set.
*/

#define LINT_ARGS

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <io.h>

#include "setup.h"


/* 'forward declarations of functions in SUINF which are not generally
** called from other modules */

BOOL FindSection(unsigned char *);
BOOL IsLParens();
BOOL IsRParens();
BOOL IsChar(unsigned int);
void GetDriverList(int);
int GetFileName();
unsigned int GetChar();
int GetQString(unsigned char [], int);
unsigned int SkipDelims();


int fhInfo; 		/* file handle for SETUP.INF */
static BOOL fComment;		/* flags comment for input */
static BOOL fString;		/* flags string in " " for input */
unsigned char * buffer; 	/* buffer for read() */
static int cBuff;		/* no. of chars in buffer */
static unsigned int chThis;
static BOOL fReread;
static int pBuff;
static int iLine;		/* line number in SETUP.INF */

/* variables for determining range of diskette numbers to search for. */
int iMinDisk;           /* (public) minimum diskette number */
int iMaxDisk;		/* (public) maximum diskette number */


/* size table for variants of the 'struct dfile' records */

int dfsize[] = {
	DFSIZE,         /* 0 type SW_DFND (may be variable) */
	DFSIZE,		/* 1 type SW_DF (may be variable) */
	DFKBSIZE,	/* 2 type SW_DFKB */
	DFFONSIZE,	/* 3 type SW_DFFON */
	DFOEMSIZE,	/* 4 type SW_DFOEMFON */
	DFLOGGRBSIZE,	/* 5 type SW_DFLOGGRB */
	DFIODEVSIZE,	/* 6 type SW_DFIODEV */
	DFCOUNSIZE,	/* 7 type SW_DFCOUN */
	DFMACHINESIZE,	/* 8 type SW_DFMACHINE */
	DFSIZE,		/* 9 type SW_DFHD (+ string length)  */
	DFFONSIZE,      /* 10 type SW_DFPRINTFON  */
	DFSIZE,          /* 11 type SW_DFDATA (+ string length) */
	DFDISPLAYSIZE			/* 12 type SW_DFDISPLAY */
	};

/* section name table. The order of this table is fixed.
** SETUP.INF will be scanned in this order, so the sections should be
** in this order to minimize file processing time. */

char * secname[] = {
	"data",                 /* N_DATA */
	"diskette",             /* N_DISKETTE */
	"windows",		/* N_WINDOWS */
	"system",		/* N_SYSTEM */
	"display",		/* N_DISPLAY */
	"logos",		/* N_LOGOS */
	"oemlogos",		/* N_OEMLOGOS */
	"grabber",              /* N_GRABBER */
	"386EXE",               /* N_386EXE */
	"386",                  /* N_386 */
	"keyboard",		/* N_KEYBOARD */
	"pointing.device",	/* N_POINTING_DEVICE */
	"sysfonts",		/* N_SYSFONTS */
	"oemfonts",		/* N_OEMFONTS */
	"country",		/* N_COUNTRY */
	"io.device",            /* N_IO_DEVICE */
	"io.dependent",         /* N_IO_DEPENDENT */
	"fonts",		/* N_PRINTFONTS */
	"apps",			/* N_APPS */
	"machine",              /* N_MACHINE */
	"harddisk",             /* N_HARDDISK */
	"compatibility"         /* N_COMPAT */
	};

/* This array maps indices to the dfile.machine.f[] array to the
** corresponding indices to section numbers. Note that several different 
** files are expected to be in the [system] section. */

int mn2sec[] = {
	N_SYSTEM,		/* M_SYSTEM */
	N_KEYBOARD,		/* M_KEYBOARD */
	N_POINTING_DEVICE,	/* M_MOUSE */
	N_DISPLAY,		/* M_DISPLAY */
	N_OEMLOGOS,		/* M_LOGO -- data file for logo */
	N_SYSTEM,		/* M_SOUND */
	N_SYSTEM,		/* M_COMM */
	N_SYSFONTS,		/* M_FONTS */
	N_OEMFONTS,		/* M_OEMFONTS */
	N_SYSTEM,		/* M_MSDOSD */
	N_SYSTEM,		/* M_WINOLDAP */
	N_SYSTEM		/* M_SPOOLER */
	};

int dfkind[] = {
	SW_DFDATA,              /* [data] */
	SW_DFDATA,              /* [diskette] */
	SW_DFND,		/* [windows] */
	SW_DFND,		/* [system] */
	SW_DFDISPLAY,		/* [display] */
	SW_DFLOGGRB,            /* [logos] */
	SW_DFND,		/* [oemlogos] */
	SW_DFLOGGRB,            /* [logos] */
	SW_DFLOGGRB,            /* [386EXE] */
	SW_DFLOGGRB,            /* [386] */
	SW_DFKB,		/* [keyboard] */
	SW_DF,                  /* [pointing.device] */
	SW_DFFON,		/* [sysfonts] */
	SW_DFOEMFON,		/* [oemfonts] */
	SW_DFCOUN,		/* [country] */
	SW_DFIODEV,             /* [io.device] */
	SW_DFLOGGRB,            /* [io.dependent] */
	SW_DFPRINTFON,		/* [fonts] */
	SW_DFND,		/* [apps] */
	SW_DFMACHINE,           /* [machine] */
	SW_DFHD,                /* [harddisk] */
	SW_DFDATA               /* [compatibility] */
	};

#define NSECTIONS (sizeof(dfkind)/sizeof (int))

/* head pointers to lists of entries from SETUP.INF */

union dfile * pheads[NSECTIONS];


/* Open_Info() -- open Setup information file SETUP.INF.
**
** Input:	The path for the information file is passed.  This module
**		assumes that this string does not move during Setup.
*/

BOOL Open_Info (pathname)
unsigned char* pathname;
{
	/* allocate buffer and initialize pointers, etc. */
	buffer = (unsigned char *) malloc(INFBUFSIZE);
	if (buffer == NULL)
		OutOfMemory();
	RewindInfo();

	/* init. flags that indicate which diskettes are used. */
	iMinDisk = 1000;
	iMaxDisk = 1;
	/* lDisksIncluded = 0L; */

	return ((fhInfo = open(pathname, O_RDONLY|O_BINARY, NULL)) != -1);

} /* OpenInfo() */

/* StoreInsertTexts() -- store info from [data] section in inserttext[]
*/
void StoreInsertTexts()
{
	int i;
	union dfile * pd = pheads[N_DATA];

	for (i = 0; i < DSIZE; i++) {
		if (pd == NULL) {
			PutSecName(secname[N_DATA]);
			PutNumber(i + 1);
			FatalError(smInfErr, 10);
		}
		inserttext[i] = pd->n.fname;

		pd = pd->n.p;
	}
}


/* RewindInfo() -- reset pointers, etc. to buffer
*/
RewindInfo ()
{
	fComment = FALSE;
	fString = FALSE;
	pBuff = INFBUFSIZE;
	cBuff = 0;
	fReread = FALSE;
	chThis = ' ';
	iLine = 1;
}

/* BILLHU
Close_Info()
{
	free(buffer);
	close(fhInfo);

	StoreInsertTexts();

}
*/
 /*Close_Info() */


/* GetWinInfo() -- read driver information from WINDOWS.INF
**
** This loads the list of drivers from the [windows] section of
** SETUP.INF (the files which do not change in name from system to
** system).
** The locations of these files are saved as well.
*/

void GetWinInfo()
{
    int secno;

    for (secno = 0; secno < NSECTIONS; secno++)
    	{
		GetDriverList(secno);
	}
} /* GetWinInfo() */


/* GetDriverList() -- get information for a driver or other file
** type from SETUP.INF.  A list is built of records of type
** 'union dfile'.  Description strings are also read and stored,
** where needed.
**
** Input	secno		index in secname[], dfkind[], etc.
**
*/

static void GetDriverList(secno)
int secno;

{
    char * section;	/* pointer to section name string */
    int kind;		/* type flag for record */
    int asize;		/* record size */
    int disk;
    int recsize;
    unsigned char filename[81]; /* 13 used for files, more for country info */
    int flen;
    unsigned char description[81];
    int descrlen;
    unsigned char * pdescr;
    BOOL hasdescr;
    unsigned char alias[13];	/* aliased filename for grabbers and logos */
    int alen;
    union dfile * phead;
    union dfile * ptail;
    union dfile * ptailold;
    union dfile * pdf;
    int oemset;
    BOOL hasoemset;
    BOOL hasres1;
    BOOL hasres2;
    int resl1[3];
    int resl2[3];
    int i;
    union dfile * mf[NMFILES];
    BOOL bDescr;
	 char IdString[10];
	 int DisplayId = 0, MachineId, n;

    /* initialize pointers to list and other little things */
    phead = ptail = ptailold = NULL;
    filename[0] = 0;
    description[0] = 0;

    /* compute various things from section number */
    section = secname[secno];
    kind = dfkind[secno];
    asize = dfsize[kind];

    /* find where the stuff is in SETUP.INF */
	if (!FindSection(section))	{
		PutSecName(section);
		FatalError(smNoSection, 10);
	}

    /* determine whether this type of record has a description string */
    switch(kind)
	{
	case SW_DF:
	case SW_DFKB:
	case SW_DFFON:
	case SW_DFDISPLAY:
	case SW_DFOEMFON:
	case SW_DFPRINTFON:
	case SW_DFIODEV:
	case SW_DFCOUN:		/* descr. string is country name */
	case SW_DFMACHINE:
			hasdescr = TRUE;
			break;

	case SW_DFND:		/* short record, no descr pointer */
	case SW_DFHD:		/* same for [harddisk] */
	case SW_DFLOGGRB:       /* [logos] [grabber] */
	case SW_DFDATA:         /* [data] [diskette] [compatibility] */
	default:
			hasdescr = FALSE;
			pdescr = NULL;
	}

    /* determine whether this driver or resource has font resolution info. */
    switch(kind)
    	{
	case SW_DFFON:
	case SW_DFDISPLAY:
	case SW_DFOEMFON:
	case SW_DFPRINTFON:
			hasres1 = TRUE;
			hasres2 = FALSE;
			break;
	case SW_DFIODEV:
			hasres1 = TRUE;
			hasres2 = TRUE;
			break;
	default:
			hasres1 = FALSE;
			hasres2 = FALSE;
	}

    /* determine whether this driver or resource has a character set no. */
    switch(kind)
	{
	case SW_DFOEMFON:
	case SW_DFKB:
			hasoemset = TRUE;
			oemset = 0;
			break;

	default:
			hasoemset = FALSE;
	}

    /* scan file, reading records */

    for (;;)
    	{
	    /* find next record. */
	    if (!IsLParens())
	    	break;

		/* get 'standard' info for usual record types. */
	    switch(kind)
	    	{
		case SW_DFCOUN:
			/* for country, description is country name */
			disk = 0;
		        descrlen = GetQString(description, 80);
			/* filename field contains country parameters */
			flen = GetQString(filename, 80);
			break;

		case SW_DFHD:
			/* Get (N = "path") record for [harddisk] */ 
			if (0 == (disk = GetNum()) )
			    break;
			SkipDelims();
			if ('=' != GetChar())
				{
	                        fReread = TRUE;
				break;
				}
			SkipDelims();
		case SW_DFDATA:
			flen = GetQString(filename, 80);
			break;

		case SW_DFMACHINE:
			disk = 0;  /* means nothing, but entry exists */
			descrlen = GetQString(description, 80);

			n = GetQString(IdString, 4);
			if (n == 0 )	{ 			/* no Machine Id present : Error */
				PutSecName(section);
				PutNumber(iLine);
				FatalError(smMissingID, 10);
			}
			else if (strlen(IdString) > 3) { /* Illegal Machine Id */
				PutSecName(section);
				PutNumber(iLine);
				FatalError(smIllegalID, 10);
			}
			
			/* Add a Machine Id to the list */
			n = atoi(IdString);
			if (strcmp(IdString, "0") == 0)
				MachineId = 0;
			else if (n == 0)	{	/* couldn't convert given string to number */
				PutSecName(section);
				PutNumber(iLine);
				FatalError(smIllegalID, 10);
			}
			else	
					MachineId = n;
			break;
		
		case SW_DFDISPLAY:
			/* Get disk-number, filename, and description information */
			if (0 == (disk = GetDiskNum()) )
			    break;
			flen = GetFileName(filename, 12);

		   if (hasdescr)
				descrlen = GetQString(description, 80);
			break;
			
		default:
			/* most types of records have disk-number,
			** filename, and description information */
			if (0 == (disk = GetDiskNum()) )
			    break;
			flen = GetFileName(filename, 12);

		        /* Get description string */
		   if (hasdescr)
				descrlen = GetQString(description, 80);
		}

			

	    /* more */
	    /* get font resolution information, if any. */
	    if (hasres1)	
	    	GetFontRes(resl1);

		 /* Get display adapter ID, occurs after the resolution. */
		 if (kind == SW_DFDISPLAY)	{
			 if (GetQString(IdString, 10) == 0)	{
					PutSecName(section);
					PutNumber(iLine);
					FatalError(smMissingID, 10);
			 }

			 DisplayId = atoi(IdString);
			 if (strcmp(IdString, "0") != 0 && DisplayId == 0)	{
					PutSecName(section);
					PutNumber(iLine);
					FatalError(smIllegalID, 10);
				}
		 }
		
		
	    /* get second font resolution record, if any. */
	    if (hasres2)	
	    	GetFontRes(resl2);

	    /* is this a [logos] or [grabber] record? If so, get
	    ** filename alias string, if any. */
	    if (kind == SW_DFLOGGRB)
	    	{
		SkipDelims();
		alen = GetFileName(alias, 12);
		if (alen == 0)
			{
			/* if no alias, duplicate filename */
			strcpy(alias, filename);
			alen = flen;
			}
		}

	    /* get any oem character set information */
	    if (hasoemset)	/* SW_DFKB, SW_OEMFON */
	    	{
		oemset = GetNum();
		}

	    /* Handle [machine] entry */
	    if (kind == SW_DFMACHINE)
	    	{
		for (i = 0; i < NMFILES; i++)
		    {
		    SkipDelims();
		    mf[i] = NULL;
		    bDescr = FALSE;
		    if (0 != GetFileName(filename, 12) ||
			(bDescr = (GetQString(filename, 80) > 0)))
			{
			/* search a file list for file name/description */
			for (pdf = pheads[mn2sec[i]]; pdf != NULL;
				pdf = pdf->machine.p)
			    {
			    if (0 == stricmp(filename,
				     bDescr ? pdf->n.descr : pdf->n.fname))
			    	{
				/* found! */
				mf[i] = pdf;
				break;
				}
			    }
			}
		    else
			{
			/* Fatal error, display section name */
			PutSecName(section);
			PutNumber(iLine);
			FatalError(smInfErr, 10);
			}
		    }
		}

	    /* check for ')' at end of record. */

	    if (!IsRParens())
		{
		/* Fatal error, display section name */
		PutSecName(section);
		PutNumber(iLine);
		FatalError(smInfErr, 10);
		}

	    /* if there is a description string, allocate space for it
	    ** and copy the description to it. */

	    if (hasdescr)
	    	{
		pdescr = malloc(strlen(description) + 1);
		if (NULL == pdescr)
			OutOfMemory();
		else
			strcpy(pdescr, description);
		}

	    /* allocate space for record, and link new record to list */

	    ptailold = ptail; 
	    switch(kind)	/* determine space required */
		{
		case SW_DFND:	/* variable-length string at end.. */
		case SW_DF:
		case SW_DFHD:
		case SW_DFCOUN:
		case SW_DFDATA:
				recsize = asize + flen;
				break;
		case SW_DFLOGGRB:
				recsize = asize + alen;
				break;
		default:	recsize = asize;
				break;
		}
	    ptail = (union dfile *) malloc(recsize);
	    if (NULL == ptail)
	    	OutOfMemory();
	    if (NULL == phead)
		/* point head pointer to first record */
	    	phead = ptail;
	    else
		{
		/* link old last record to new one */
		ptailold->n.p = ptail;
		}

	    ptail->n.p = NULL;


	    /* copy data to record common to all types */
	    ptail->n.descr =  (hasdescr) ? pdescr : NULL;
	    ptail->n.d = disk;

	    /* copy type-specific information */
	    switch(kind)
	    	{
		case SW_DFKB:
				/* copy character set */
				ptail->kb.charset = oemset;
				break;
		case SW_DFFON:
		case SW_DFOEMFON:
		case SW_DFPRINTFON:
		case SW_DFIODEV:
				/* copy font resolution */
				for (i = 0; i < 3; i++)
				    ptail->fon.res[i] = resl1[i];
				/* copy second font or char. set */
				switch(kind)
				    {
				    case SW_DFIODEV:
					/* copy second font resolution */
					for (i = 0; i < 3; i++)
					    ptail->iodev.res2[i] = resl2[i];
					break;
				    case SW_DFOEMFON:
					/* copy character set */
					ptail->oemfon.charset = oemset;
					break;
				    }
				break;
		case SW_DFDISPLAY:
				/* copy font resolution */
				for (i = 0; i < 3; i++)
				    ptail->display.res[i] = resl1[i];
				ptail->display.Id = DisplayId;
				break;
		
		case SW_DFLOGGRB:
				/* copy filename alias */
				strcpy(ptail->loggrb.aname, alias);
				break;
		case SW_DFMACHINE:
				/* copy pointers to filename records */
				for (i = 0; i < NMFILES; i++)
					ptail->machine.f[i] = mf[i];

				ptail->machine.Id = MachineId;
		}

	    /* copy filename */
		if (kind != SW_DFMACHINE)
		    strcpy(ptail->n.fname, filename);

	}	

    pheads[secno] = phead;

} /* GetDriverList() */


/* FindSection() -- find section in SETUP.INF
**
** reads SETUP.INF until character after specified section heading.
** Will restart from beginning of file if EOF is reached the first time.
** 
** Input:	'title' is string to be found.  In the file, it is inside
**		        square brackets '[', ']'.
*/

BOOL FindSection(title)
unsigned char title[];
{
	int tries;
	BOOL found;
	unsigned int ich;
	unsigned int sch;
	int i;
	unsigned char sSearch[32];

	strcpy(sSearch,title);
	strcat(sSearch,"]");

	for (tries = 0, found = FALSE; (tries < 2) && !found; )
		{
		sch = -1;
		while(!found && ((ich = GetChar()) != EOF))
		  if (ich == '[')
		      {
		      for (i = 0;
			   ((sch = sSearch[i]) != 0) &&
			   ((ich = GetChar() ) != EOF) &&
			   (sch == ich) ;
			   i++) ;
		      if (sch == 0)
			   found = TRUE;
		      }
		if (!found)
			{
			tries++;
			lseek(fhInfo, 0L, SEEK_SET);
			RewindInfo();
			}
		}

	return(found);

} /* FindSection() */


/* GetDiskNum() -- read decimal integer followed by ':' from file
*
* Numbers are expected to be nonzero diskette numbers.
* If the character following the integer is not ':', the routine
* returns 0 also.
*/

static int GetDiskNum()
{
    int n;
    unsigned int ich;

    n = GetNum();
    if(':' != GetChar() )
	{
	fReread = TRUE;
    	return(0);
	}
    else
	{
	/* ok -- handle variables to indicate range of diskettes. */
	if (n > iMaxDisk)
		iMaxDisk = n;
	if (n < iMinDisk)
		iMinDisk = n;
	/* if ((n > 0) && (n <= 32)) */
	/* 	lDisksIncluded |= (1L << (n - 1)); */
    	return(n);
	}

} /* GetDiskNum() */



/* GetNum() -- read decimal integer from file */

static int GetNum()
{
    int n;
    unsigned int ich;

    SkipDelims();
    n = 0;
    while ( ( (ich = GetChar()) >= '0') && (ich <= '9') )
	n = 10 * n + ich - '0';

    fReread = TRUE;
    return(n);

} /* GetNum() */



/* GetFileName() -- read filename string.
** 
** Exits when number of characters exceeds max.
** Only legal filename characters are allowed.
** Drive or path separators (':', '\') are NOT allowed.
** SkipDelims() must be called previously, if the file is not expected to
** be positioned at the beginning of the filename.
**
** returns length of filename.
*/

static int GetFileName(pfile, max)
unsigned char pfile[];
int max;
{
    int i;
    unsigned int ich;
    BOOL ok;

    for (i = 0, ok = TRUE; ok; )
    	{
	ich = GetChar();
	switch(ich)
	    {
	    case '(': case ')':		/* we don't allow "()" */
	    case EOF:			/* MSDOS doesn't allow the rest. */
	    case '"': case '/': case '\\':
	    case '[': case ']':
	    case ':': case '<': case '>':
	    case '+': case '=': case ';':
	    case ',': case ' ':
	    	ok = FALSE;
		break;
	    default:
		if (ich < 32)
		    ok = FALSE;
	    	break;
	    }
	if ((ok) && (i < max))
	    pfile[i++] = toupper(ich);
	}

    fReread = TRUE;
    pfile[i] = 0;
    return(i);

} /* GetFileName */



/* GetFontRes() -- read font resolution information in double quotes
** If the information is numeric, it is converted to integers and
** the 3 values are returned in the array resvec[].
** "CONTINUOUSSCALING" causes (0,1,0) to be returned.
** "DEVICESPECIFIC" causes (0,2,0) to be returned.
*/

GetFontRes(resvec)
int resvec[];
{
    unsigned char s[31];
    int i;
    int len;

    len = GetQString(s, 30);
    if (0 == len)				/* length 0 string */
    	{
	for (i = 0; i < 3; i++)
	    resvec[i] = 0;
	}
    else if (0 == strcmp(s, "CONTINUOUSSCALING"))	/* special value */
    	{
	resvec[0] = resvec[2] = 0;
	resvec[1] = 1;
	}
    else if (0 == strcmp(s, "DEVICESPECIFIC"))	/* special value */
    	{
	resvec[0] = resvec[2] = 0;
	resvec[1] = 2;
	}
    else
    	{
	/* extract 3 integers from the string */
	ConvertRes(s, resvec);
	}

}  /* GetFontRes() */

/* ConvertRes() -- convert resolution digits.
**
** Used by GetFontRes() and by GetDriver() in SUDRIVER.C to convert an
** ASCII string containing 3 decimal integers into an array of 3 integers.
**
** This now returns the number of characters scanned.
*/
int ConvertRes(s, resvec)
unsigned char * s;
int resvec[];
{
    int i;
    int j;
    unsigned int ich;
    j = 0;
    for (i = 0; i < 3; i++)
	{
	resvec[i] = 0;
	/* skip non-digits to end */
	while ( (0 != (ich = s[j])) && ((ich < '0') || (ich > '9')) )
		j++;
	/* read digits from string and convert to integer */
	while ( (0 != (ich = s[j])) && (ich >= '0') && (ich <= '9') )
		{
		resvec[i] = 10 * resvec[i] + ich - '0';
		j++;
		}
	}

   return(j);
 
} /* ConvertRes */


/* GetQString() -- read string delimited by double quotes
*
* returns length of string
*/

int GetQString(s, max)
unsigned char s[];
int max;
{
    int i;
    unsigned int ich;
    BOOL ok;

    ok = TRUE;
    i = 0;

    SkipDelims();
    if ('"' != GetChar() )
    	{
	ok = FALSE;
	fReread = TRUE;
	}

    while (ok)
    	{
	ich = GetChar();
	switch(ich)
	    {
	    case EOF:	i = 0;
	    case '"':
	    		ok = FALSE;
			break;
	    default:	if (ich < 32)
			    {
			    i = 0;
	    		    ok = FALSE;
			    }
	    }
	if(ok && (i < max))
	    s[i++] = ich;
    	}

    s[i] = 0;
    return(i);

} /* GetQString() */


/* IsLParens(), IsRParens() -- check for left/right parenthesis in file
*
*	returns	TRUE if next char in file is parens , and it reads the char.
*	else returns FALSE, and GetChar will re-read the char next time.
*/

BOOL IsLParens()
{
    SkipDelims();
    return(IsChar('('));

} /* IsLParens() */

BOOL IsRParens()
{
    SkipDelims();
    return(IsChar(')'));

} /* IsRParens() */

BOOL IsChar(tch)
unsigned int tch;
{
    if (tch == GetChar())
	return(TRUE);
    else
	{
	fReread = TRUE;
	return(FALSE);
	}

} /* IsChar() */



/* SkipDelims() -- read past delimiter characters
**
** Returns a comma (if 1 or more commas were passed),
** or EOF if the end of the file is reached,
** otherwise a space character.
*/

unsigned int SkipDelims()
{
    unsigned int ich;
    unsigned int rch;

	rch = ' ';

    do 
	{
	ich = GetChar();
	switch(ich)
	    {
	    case EOF:
		rch = ich;
		break;
	    case ',':
		rch = ich;
	    case ' ':
	    case '\t':
	    case 10:
	    case 12:
	    case 13:
	    	ich = ' ';
	    }
	}
    while (ich == ' ');

	fReread = TRUE;
	return(rch);

} /* SkipDelims() */


/* GetChar() -- read char from info file.
**
** Characters inside comments, including initial semicolon, are ignored.
*/

unsigned int GetChar()
{
    if (chThis == EOF)
    	fReread = TRUE;

    if (fReread)
	fReread = FALSE;
    else do	/* keep reading 'till after a comment */
    	{
	if (pBuff < cBuff)
		chThis = buffer[pBuff++];
	else
		{
		if ( (cBuff = read(fhInfo, buffer, INFBUFSIZE)) < 1)
			chThis = EOF;
		else
			{
			/* xxx chThis = 0xff & buffer[0]; */
			chThis = buffer[0];
			pBuff = 1;
			}
		}
	/* handle line number iLine */
	if (chThis == '\n')
		iLine++;
	if (fComment)
		{ /* in comment */
		/* if ((chThis == 13) || (chThis == 10) || (chThis == EOF)) */
		if ((chThis == '\n') || (chThis == EOF))
			fComment = FALSE;
		}
	else
		{ /* not in comment -- handle fString and fComment */
		if ((chThis == ';') && (!fString))
			fComment = TRUE;
		if (fString)
		    {
		    if((chThis == '\n') || (chThis == '"') || (chThis == EOF))
			fString = FALSE;
		    }
		else
		    {
		    if (chThis == '"')
			fString = TRUE;
		    }
		}

	} while (fComment);

	return(chThis);

} /* GetChar() */


/* OutOfMemory() -- fatal error, no more memory
*/
OutOfMemory()
{
    FatalError(smNoMemory, 10);

} /* OutOfMemory() */

