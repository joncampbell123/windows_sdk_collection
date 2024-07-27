/* SUWININI.C  --  provides routines for accessing WIN.INI
**                 similar to those in the Windows library
**
** Errors in this module exit with return code 11
*/
/*  History
**
**   8 jul 87   mp      start history
**  10 aug 87   mp      change memcpy() to memmove() for C 5.00
**                      fixing some potential problems
**  31 jan 89   wch     removed references to Is360K - always FALSE
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <string.h>
#include <sys\types.h>
#include <errno.h>
#include <sys\stat.h>
#include "setup.h"

#define BUFLENGTH 5120
#define EOB '\003'
#define CR '\x0D'
#define LF '\x0A'


static CHAR *buffer;
static CHAR WinIniFile[] = "WIN.INI";
static CHAR IntlEntry[] = "intl";
static CHAR FontEntry[] = "fonts";

/* global variables
*/
BOOL IsReinstall;

/* externals
*/
extern CHAR WinIniBakFile[];
extern CHAR smWinIniBackUpFail[];
extern textline WinIniMsg[];

extern union dfile * pselect[];


/******************************************************************************/
/******************************************************************************/

static int GetLengthToEnd(pPosition)
/****************************/

    CHAR *pPosition;

{
    CHAR *p;

    p = memchr(pPosition, EOB, buffer + BUFLENGTH - pPosition);

    return (p - pPosition);
}


static int GetLineLength(pLine)
    CHAR *pLine;
{
    int length = 0;

    while (*(pLine++) != CR) length++;
    return length;
}


static CHAR *GetNextLine(pLine)
/**************************/

    CHAR *pLine;

{

    int lengthLine = 80;

    do {
	while (*(++pLine) != LF)
	    pLine = memchr(pLine, CR, lengthLine);
    } while ((*(++pLine) == ';') || (*pLine == CR)) ;  /* Test for */
    return pLine;			       /* comments or empty lines */
}

static int GetAppLength(pApp)
    CHAR *pApp;
{
    int length = 0;

    while (*(pApp++) != ']') length++;
    return length;
}



static	CHAR *GetApp(pApplicationName)
      /*********************************/

    CHAR *pApplicationName;

{
    int result = 1;
    CHAR *pApp = buffer;
    int length;

    while (result != 0) {
	if ((pApp = memchr(pApp, '[', GetLengthToEnd(pApp)))==NULL) {
	    /* if no application then return a pointer 
		to the end of the buffer */
	    result = 0;
	    pApp = (buffer + GetLengthToEnd(buffer));
	    }
	else {
	    length = GetAppLength(++pApp);
	    if (length == strlen(pApplicationName))
		result = memicmp(pApp, pApplicationName, length);
	    }
	}
    return pApp;
}


static int GetKeyLength(pKey)
    CHAR *pKey;

{
    int length = 0;

    while (*(pKey++) != '=') length++;
    return length;
}


static BOOL GetKey(pKeyName, ppKey)
    CHAR *pKeyName;
    CHAR **ppKey;

{
    int length, result = 1;


    while (result != 0) {
	*ppKey = GetNextLine(*ppKey);
	if ((**ppKey == '[') || (**ppKey == EOB))
	    break;
	length = GetKeyLength(*ppKey);
	if (length == strlen(pKeyName))
	    result = memicmp(*ppKey, pKeyName, length);
	}
    return result;
}

static	void AppendApp(pApplicationName, pApp)
      /***************************************/

    CHAR *pApplicationName;
    CHAR *pApp;

{
#define PRE 3
#define SUFF 4
    int length;

    length = strlen(pApplicationName);

    if (pApp+length+PRE+SUFF >= buffer+BUFLENGTH)
	exit (11);

    memmove(pApp, "\015\012[", PRE);   /* write CR LF [ */
    pApp += PRE;
    memmove(pApp, pApplicationName, length);   /* write Application Name */
    pApp += length;
    memmove(pApp, "]\015\012\003", SUFF);     /* write ] CR LF EOBuffer */
}


static	void WriteKey(pKeyName, pString, pKey)
      /***************************************/

    CHAR *pKeyName;
    CHAR *pString;
    CHAR *pKey;
{
    int Nmove, total, Lkey, Lstring, suff = 2;

    Lkey = strlen(pKeyName);
    Lstring = strlen(pString);
    total = Lkey + 1 + Lstring + suff;	 /*  KeyName=String CR LF */
    Nmove = GetLengthToEnd(pKey) + 1;

    if (pKey+total+Nmove >= buffer+BUFLENGTH)
	exit (11);

    memmove(pKey+total, pKey, Nmove); /* free space for 'total' characters */
    memmove(pKey, pKeyName, Lkey);    /* write Key Name */
    pKey += Lkey;
    *(pKey++) = '=';		/* write = */
    memmove(pKey, pString, Lstring);  /* write String */
    pKey += Lstring;
    memmove(pKey, "\015\012", suff);   /* write CR LF */
}


static	void WriteStringKey(pString, pKey)
      /************************************/

    CHAR *pString;
    CHAR *pKey;

{
    int Lstring, Nmove, LoldString;

    Lstring = strlen(pString);
    LoldString = GetLineLength(pKey);
    Nmove = GetLengthToEnd(pKey+LoldString) + 1;

    if (pKey+Lstring+Nmove >= buffer+BUFLENGTH)
	exit (11);

    memmove(pKey+Lstring, pKey+LoldString, Nmove); /* free Lstring-LoldString
			    caracters*/
    memmove(pKey, pString, Lstring);  /* write new string */
}

static	CHAR *GetStringKey(pKey)
    CHAR *pKey;
{
    while (*(++pKey) != '=') ;
    return ++pKey;
}

/******************************************************************************/
/******************************************************************************/

/* opens, reads and closes WIN.INI in pSourcePath */

   void WinIniOpen()
/********************/


{
    int fh, length;

    PutFileName(WinIniFile);
    PutNumber(1);
    if (!(buffer = malloc(BUFLENGTH)))
	FatalError(smNoMemory, 5);
    if ((fh = open(FileName(pSourcePath, WinIniFile),
		    O_RDONLY|O_BINARY)) == -1)
	FatalError(smNoFile, 5);
    if (((length = read(fh, buffer, BUFLENGTH)) <= 0) ||
	(length == BUFLENGTH))
	FatalError(smLoading, 5);

    *(buffer + length) = EOB;

    close(fh);

    return;
}

/* searches in WIN.INI for an application entry and returns all keys */
/* founded in the ReturnedString, which must be big enough. Each key */
/* is seperated by 0, the end is marked with two 0. */

 void GetProfileString(pApplicationName, pReturnedString)
/********************************************************/

    CHAR *pApplicationName;
    CHAR *pReturnedString;

{
    CHAR *pointer;

    pointer = GetApp(pApplicationName);
    if (*pointer != EOB) {
	    pointer = GetNextLine(pointer);
	while ((*pointer != '[') && (*pointer != EOB)) {
	/* test for "end of list" or "end of file" */
	    while (*pointer != '=')  /* test for the = sign */
		*pReturnedString++ = *pointer++;
	    *pReturnedString++ = '\0';
	    pointer = GetNextLine(pointer);
	    }
	}
    else
	*pReturnedString++ = '\0';
    *pReturnedString = '\0';

}


/* writes a string after the equal sign of key in the application section */
/* if the key is not there it is added */

  void WriteProfileString(pApplicationName, pKeyName, pString)
/*************************************************************/

    CHAR *pApplicationName;
    CHAR *pKeyName;
    CHAR *pString;

{
    CHAR *pApp = buffer;
    CHAR *pKey;
    BOOL end = TRUE;
    int length = 0;
    int result = 1;

    pApp = GetApp(pApplicationName);
    if (*pApp == EOB) {               /* new application */
        AppendApp(pApplicationName, pApp);
        pKey = GetNextLine(++pApp);
        WriteKey(pKeyName, pString, pKey);
    } else {
        pKey = pApp;
        if (GetKey(pKeyName, &pKey) != 0) {
            pKey = GetNextLine(pApp);
            WriteKey(pKeyName, pString, pKey);
        } else {
            pKey = GetStringKey(pKey);
            WriteStringKey(pString, pKey);
        }
    }
}

/* opens, writes and closes WIN.INI in WindowsPath */

  void WinIniClose()
/*******************/

{
    BOOL result = FALSE;
    int fh, length;

    if (IsQuick)
	return;

    PutFileName(WinIniFile);
    PutDestName(WindowsPath);
    if ((fh = open(FileName(WindowsPath, WinIniFile),
		    O_CREAT|O_TRUNC|O_WRONLY|O_BINARY, S_IWRITE)) <= 0)
	FatalError(smWriting, 5);
    length = GetLengthToEnd(buffer);
    if (write(fh, buffer, length) < length)
	FatalError(smWriting, 5);

    free(buffer);
    close(fh);
}


/* checks whether there is an old WIN.INI, if not reads WIN.INI from source */
/* and checks whether there is an [intl] entry and sets appropriate flags */
void WinIniPrepare()
{
CHAR path[PATHMAX];
CHAR WinIni[PATHMAX];
CHAR WinOld[PATHMAX];

	strcpy(WinIni, FileName(WindowsPath, "WIN.INI"));
	strcpy(WinOld, FileName(WindowsPath, WinIniBakFile));
	
	if (access(WinIni, 0) == 0 && access(WinOld, 0) == 0)	{
		/* both win.ini and win.old exists. */
		ScrClear();
		ScrDisplay(WinIniMsg);
		ScrInputPath(WinOld, path, PATHMAX-1);
		/* remove \ at the end of the path which ScrInputPath appends. */
		path[strlen(path)-1] = '\0';
		strcpy(WinIniBakFile, path);
		strcpy(WinOld, path);
	}
		
/* now, WinOld has WIN.OLD or the file name given by the user. */


	if (access(WinIni, 0 ) == 0)	{		/* win.ini exists */
		if (remove(WinOld) == -1)	{
			if (errno == EACCES)  {			/* win.old is read only */
				PutFileName(WinOld);
				FatalError(smWinIniBackUpFail, 5);
			}
			else	/* no win.old exists */
				;
		}
		
		/* now, win.ini exists and WIN.OLD if any is removed */
		if (rename(WinIni, WinOld) != 0)	{
			PutFileName(WinOld);
			FatalError(smWinIniBackUpFail, 5);
		}				 
		/* at this stage a back up of WIN.INI has been successfully created. */
		IsReinstall = TRUE;
	}

		
	WinIniOpen();

/* BILLHU
	WriteProfileString("windows", "spooler", Is360K ? "no" : "yes");
*/
	WriteProfileString("windows", "spooler", "yes");
}


/* write font information to WIN.INI */

void WinIniFonts()
{
    union dfile *pd;			/* pointer to font info */
    CHAR buf[14];

/* BILLHU
    for (pd = RecNPtr(S_FONT); pd; pd = pd->fon.p) {
*/
    for (pd = (S_FONT < SSIZE) ? pselect[S_FONT] : NULL; pd; pd = pd->fon.p) {
	*strchr(strcat(strcpy(buf, pd->fon.fname), "."), '.') = '\0';

	WriteProfileString(FontEntry, pd->fon.descr, buf);
    }
}


