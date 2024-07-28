/*/   FILE.C for TTY
 +-----------------------------------------+
 | Fralc Consultores (c), Mexico 1986-1989 |
 +-----------------------------------------+
/*/

// ------------------------------------------------------------------------
// Copyright (C) 1989-1990, Microsoft Corporation. All rights reserved.
// ------------------------------------------------------------------------
// Microsoft history
//	18 dec 89	peterbe		Added #ifdefs for messagebox code.
//	15 dec 89	peterbe		Use GetWindowsDirectory() to get the
//					(writable!) path for TTY.DAT, which is
//					now in the Windows directory.
//	14 dec 89	peterbe		A bit of cleanup and debugging in last
//					few days.
//	05 dec 89	peterbe		Took out 1 debug mod.
//	04 dec 89	peterbe		Changed data file back to to TTY.DAT.
//					Added debug calls.
//	01 dec 89	peterbe		Changed data file to TTY.BIN.
//					Checking on file handles is INCORRECT:
//					-1 is error value.
//	20 nov 89	peterbe		If NO font escapes are defined, we
//					make default 10cpi. Then apps won't
//					try to write too many chars per line.
//					(If 10 and 12cpi are defined, code
//					elsewhere will make default 12cpi)
//	13 nov 89	peterbe		Import bHasFont[], make stringlen()
//					and stringcat() near, initialize
//					bHasFont[] on basis of first char in
//					each escape string.  If we find no
//					font escapes, default to 12 cpi.
//	20 oct 89	peterbe		Checked in.
//					Removed #include's of c-runtime
//					(need to define SEEK_SET)
// ------------------------------------------------------------------------

#include "generic.h"

#ifdef DEBUG
#include "debug.h"
#define DBGfile(msg) DBMSG(msg)
#define DecFile	DBMSG((".._lclose(): iOpen =%d\n", --iOpen))
#else
#define DBGfile(msg)	/* zip */
#define DecFile		/* zilch */
#endif

#define NFONTS 6

#include "ttyres.h"

#define SEEK_SET	0

#define MB_OK		0x0000

		/*** Declaraciones Externas ***/

extern	BOOL bHasFont[];		// TRUE if font [i] is supported.

extern HANDLE	    hInstancia;
extern unsigned char defaultchars[];

#ifdef DEBUG
int iOpen;
#endif

PrinterInfo Printer		    = {0};
char	    PrinterFileName[65]    = {0};
int	    PrinterNumber	    =  0,
	    numprinters,
	    actprinter;
OFSTRUCT    os;

int far	 PASCAL GetPrinterFileName(void);
int far GetPrinter(int);
int far SetPrinter(int);
int far GetCurPrinter(void);
int far SetCurPrinter(int act);
int far GetNumPrinters(void);
int far SetNumPrinters(int num);
unsigned char near CodeCopy(LPESCAPEPAIR, LPSTR);
int far LoadThePrinter(LPESCAPECODE);
void far pathcopy(char *a,char *b);
int far PASCAL DeletePrinter(int number);

BOOL isstring(char *a);

int	fh = -1;

// routine for displaying handle, for debug purposes.
#ifdef DEBUGSHOWHANDLE

void near PASCAL ShowHandle(capt, h)
LPSTR capt;
int h;
{
char cDecimal[3];
char c[80];

// quick-and-dirty 2-digit decimal
cDecimal[0] = '0' + h / 10;
cDecimal[1] = '0' + h % 10;
cDecimal[2] = '\0';

lstrcpy((LPSTR) c, (LPSTR) "Handle: ");
lstrcat((LPSTR) c, (LPSTR) cDecimal);

MessageBox(NULL, (LPSTR) c, (LPSTR) capt, 0);
}
#endif


int far PASCAL GetPrinterFileName()
{
    int	    i;
    int	    fh;
    char    ModuleName[64];
    char    WinDir[64];
    LPSTR   lpModName;

    // Get fully qualified module name.
    GetModuleFileName(hInst, ModuleName, 64);

    // from the right, strip off extension.  We'll have a terminating
    // '.'
    for (i = lstrlen((LPSTR)ModuleName);
	(i > 0) && ModuleName[i] != '.'; i--)
		ModuleName[i] = '\0';

    // Then, keep backing down until we reach a ':'  or '\'.
    while ((i > 0) && (ModuleName[i-1] != ':') && (ModuleName[i-1] != '\\' ))
	i--;

    lpModName = (LPSTR) ModuleName + i;

    // Get the path of the Windows directory.
    GetWindowsDirectory((LPSTR)PrinterFileName, 64);

    //   Append '\', if the Windows dir. isn't the root.

    if ('\\' != PrinterFileName[lstrlen((LPSTR)PrinterFileName) - 1])
	lstrcat((LPSTR)PrinterFileName, (LPSTR) "\\");

    //   Append the module name and "DAT".
    lstrcat((LPSTR)PrinterFileName, lpModName);
    lstrcat((LPSTR)PrinterFileName, (LPSTR) "DAT");

    DBGfile(("GetPrinterFileName: Check for <%ls>\n", (LPSTR) PrinterFileName));

    // Note: OpenFile() and _lcreat() return -1 if they fail.

    if((fh = OpenFile(PrinterFileName, &os, OF_EXIST)) != -1)
	{
	short iPrinter;

	DBGfile((".. exists: fh = %d, return TRUE\n", fh));
	if ((iPrinter = GetCurPrinter()) != PRINTERFILEERROR)
	    {
	    iPrinter = GetPrinter(iPrinter);

	    if (iPrinter == PRINTEROK)
		return TRUE;
	    }

	if (iPrinter == PRINTERFILEERROR)
	    {
	    char rgch[120];
	    char rgch2[40];

	    LoadString(hInst, IDS_MSG_NOSETTINGS, (LPSTR)rgch, 120);
	    LoadString(hInst, IDS_MSG_CAPTION, (LPSTR)rgch2, 40);
	    if (MessageBox(0, (LPSTR)rgch, (LPSTR)rgch2, MB_OKCANCEL) != IDOK)
		return 0;

	    OpenFile(PrinterFileName, &os, OF_DELETE);
	    }
	}

    DBGfile((".. didn't exist: fh = %d..", fh));

    if( (fh = _lcreat(PrinterFileName, 0)) == -1)
	{

	#ifdef DEBUG_FAILCREATE
	MessageBox(NULL, (LPSTR) "_lcreat() failed",
		(LPSTR) "TTY: GetPrinterFileName()", 0);
	#endif

	DBGfile(("_lcreat() failed, return FALSE\n"));
	return FALSE;
	}
    DBGfile(("_lcreat() succeeded, fh = %d\n", fh));

    // We've created a new TTY.DAT file.  We create a default table
    // for one printer.

    Printer.piPrinterData.pcPageWidth	= 8;
    Printer.piPrinterData.pcPageHeight	= 66;
    Printer.piPrinterData.pcReset[0]	= '\0';
    Printer.piPrinterData.pc10cpi[0]	= '\0';
    Printer.piPrinterData.pc12cpi[0]	= '\0';
    Printer.piPrinterData.pc16cpi[0]	= '\0';
    Printer.piPrinterData.pcDoubleOn[0]  = '\0';
    Printer.piPrinterData.pcDoubleOff[0] = '\0';
    for(i = 0; i<=ANSIEND-ANSISTART; i++){
	Printer.piPrinterTable[i][0] = defaultchars[i];
	Printer.piPrinterTable[i][1] = '\0';
    }

    LoadString(hInst, STR_NEWID, Printer.piPrinterName, PRINTERLEN);

    numprinters = actprinter = 1;

    // write a little header consisting of the number of printers and
    // the number of the current ('actual') printer.
    _lwrite(fh,(LPSTR)&numprinters,sizeof(int));
    _lwrite(fh,(LPSTR)&actprinter,sizeof(int));

    if(_lwrite(fh,(LPSTR)&Printer,sizeof(Printer)) < sizeof(Printer))
	{
	_lclose(fh);
	DecFile;
	DBGfile((""));
	DBGfile(("GetPrinterFileName: error creating new TTY.DAT\n"));
	return FALSE;
	}

    _lclose(fh);
    DecFile;
    DBGfile(("GetPrinterFileName: created new TTY.DAT\n"));
    return TRUE;

}	// GetPrinterFileName()

int far PASCAL GetPrinter(number)
int number; /* 1..num de impresoras */
{
    int		fd;
    long	offs;

    DBGfile(("GetPrinter():\n"));
    if(number <=0)
	{
	DBGfile((".. negative number, not found.\n"));
	return PRINTERNOTFOUND;
	}
    if( (fd = OpenFile(
	(LPSTR)PrinterFileName,(LPOFSTRUCT)&os,OF_READWRITE)) == -1)
	{
	DBGfile((".. can't open TTY.DAT\n"));
	return PRINTERFILEERROR;
	}

    #ifdef DEBUG
    iOpen++;
    DBGfile((".. handle = %d. iOpen =%d\n", fd, iOpen));
    #endif

    //ShowHandle((LPSTR)"GetPrinter()", fd);

    if(_llseek(fd,NUMPRINTERS,SEEK_SET) == -1L)
	{
	_lclose(fd);
	DecFile;
	DBGfile((".. can't seek to numprinters\n"));
	return PRINTERFILEERROR;
	}
    if(_lread(fd,(LPSTR)&numprinters,sizeof(int)) < sizeof(int))
	{
	_lclose(fd);
	DecFile;
	DBGfile((".. can't read numprinters\n"));
	return PRINTERFILEERROR;
	}
    if(number > numprinters)
	{
	_lclose(fd);
	DecFile;
	DBGfile((".. number > numprinters .. printer not found\n"));
	return PRINTERNOTFOUND;
	}
    number--;
    offs = (long)(number*sizeof(Printer) + 2*sizeof(int));
    if(_llseek(fd,offs,SEEK_SET) == -1L)
	{
	_lclose(fd);
	DecFile;
	DBGfile((".. can't seek to printer record.\n"));
	return PRINTERFILEERROR;
	}
    if(_lread(fd,(LPSTR)&Printer,sizeof(Printer)) < sizeof(Printer))
	{
	_lclose(fd);
	DecFile;
	DBGfile((".. can't read whole record for printer.\n"));
	return PRINTERFILEERROR;
	}
    if(_lclose(fd) == -1)
	{
	DecFile;
	DBGfile((".. error closing TTY.DAT.\n"));
	return PRINTERFILEERROR;
	}
    DecFile;
    DBGfile((".. close successful, got printer record.\n"));
    return PRINTEROK;

}	// GetPrinter()

int far PASCAL SetPrinter(number)
int number; /* 1..num de impresoras */
{
    int	    fd;
    long    offs;

    DBGfile(("SetPrinter(%d)\n", number));

    if((fd = OpenFile(
	(LPSTR)PrinterFileName,(LPOFSTRUCT)&os, OF_EXIST)) != -1)
	{
	DBGfile((".. TTY.DAT exists, handle = %d\n", fd));
	if( (fd = OpenFile(
		(LPSTR)PrinterFileName,(LPOFSTRUCT)&os,OF_READWRITE)) == -1)
	    {
	    DBGfile((".. can't open read/write.\n"));
	    return PRINTERFILEERROR;
	    }
	}
    else
	{
	DBGfile((".. create new TTY.DAT"));
	if( (fd = OpenFile((LPSTR)PrinterFileName,
			(LPOFSTRUCT)&os,OF_CREATE|OF_READWRITE)) == -1)
	    {
	    #ifdef DEBUG_FAILCREATE
	    MessageBox(NULL, (LPSTR) "OpenFile() failed to create TTY.DAT",
		(LPSTR) "TTY: SetPrinter()", 0);
	    #endif
	    DBGfile((".. can't create.\n"));
	    return PRINTERFILEERROR;
	    }
	}

    #ifdef DEBUG
    iOpen++;
    DBGfile((".. handle = %d. iOpen =%d\n", fd, iOpen));
    #endif

    if(_llseek(fd,NUMPRINTERS,SEEK_SET)== -1L)
	{
	_lclose(fd);
	DecFile;
	DBGfile((".. can't seek to numprinters.\n"));
	return PRINTERFILEERROR;
	}
    if(_lread(fd,(LPSTR)&numprinters,sizeof(int)) < sizeof(int))
	{
	_lclose(fd);
	DecFile;
	DBGfile((".. can't read numprinters.\n"));
	return PRINTERFILEERROR;
	}
    if(number > numprinters)
	{
	DBGfile((".. writing numprinters.\n"));
	_llseek(fd,NUMPRINTERS,SEEK_SET);
	numprinters = number; /*necesario si ss != ds */
	_lwrite(fd,(LPSTR)&numprinters,sizeof(int));
	}
    number--;
    offs = (long)(number*sizeof(Printer) + 2*sizeof(int));
    if(_llseek(fd,offs,SEEK_SET) == -1L)
	{
	_lclose(fd);
	DecFile;
	DBGfile((".. can't seek to record.\n"));
	return PRINTERFILEERROR;
	}
    if(_lwrite(fd,(LPSTR)&Printer,sizeof(Printer)) < sizeof(Printer))
	{
	_lclose(fd);
	DecFile;
	DBGfile((".. can't write new record.\n"));
	return PRINTERFILEERROR;
	}
    if(_lclose(fd) == -1)
	{
	DecFile;
	DBGfile((".. error closing.\n"));
	return PRINTERFILEERROR;
	}
	DecFile;
    return PRINTEROK;
}	// SetPrinter()

int far PASCAL GetCurPrinter()
{
    int	    fd;

    DBGfile(("GetCurPrinter()\n"));

    if( (fd = OpenFile((LPSTR)PrinterFileName,
		(LPOFSTRUCT)&os,OF_READWRITE)) == -1)
	{
	DBGfile((".. can't open TTY.DAT.\n"));
	return PRINTERFILEERROR;
	}

    #ifdef DEBUG
    iOpen++;
    DBGfile((".. handle = %d. iOpen =%d\n", fd, iOpen));
    #endif

    if(_llseek(fd,ACTPRINTER,SEEK_SET)== -1L)
	{
	_lclose(fd);
	DecFile;
	DBGfile((".. can't seek to current printer number.\n"));
	return PRINTERFILEERROR;
	}
    if(_lread(fd,(LPSTR)&actprinter,sizeof(int)) < sizeof(int))
	{
	_lclose(fd);
	DecFile;
	DBGfile((".. can't read current printer number.\n"));
	return PRINTERFILEERROR;
	}
    if(_lclose(fd) == -1)
	{
	DecFile;
	DBGfile((".. can't close TTY.DAT.\n"));
	return PRINTERFILEERROR;
	}
    DecFile;
    return actprinter;

}	// GetCurPrinter()

int far PASCAL SetCurPrinter(act)
int act;
{
    int	    fd;

    DBGfile(("SetCurPrinter(%d)\n", act));

    actprinter = act;	// necessary if ss != ds

    if((fd = OpenFile((LPSTR)PrinterFileName,(LPOFSTRUCT)&os, OF_EXIST)) != -1)
	{
	DBGfile((".. TTY.DAT exists, handle = %d.\n", fd));
	if( (fd = OpenFile((LPSTR)PrinterFileName,
			(LPOFSTRUCT)&os,OF_READWRITE)) == -1)
	    {
	    DBGfile((".. can't open readwrite.\n"));
	    return PRINTERFILEERROR;
	    }
	}
    else
	{
	DBGfile((".. TTY.DAT didn't exist.\n"));
	if( (fd = OpenFile((LPSTR)PrinterFileName,
			(LPOFSTRUCT)&os,OF_CREATE|OF_READWRITE)) == -1)
	    {
	    #ifdef DEBUG_FAILCREATE
	    MessageBox(NULL, (LPSTR) "OpenFile() failed to create TTY.DAT",
		(LPSTR) "TTY: SetCurPrinter()", 0);
	    #endif
	    DBGfile((".. can't create TTY.DAT.\n"));
	    return PRINTERFILEERROR;
	    }
	}

    #ifdef DEBUG
    iOpen++;
    DBGfile((".. handle = %d. iOpen =%d\n", fd, iOpen));
    #endif

    if(_llseek(fd,ACTPRINTER,SEEK_SET)== -1L)
	{
	_lclose(fd);
	DecFile;
	DBGfile((".. can't seek to current printer no.\n"));
	return PRINTERFILEERROR;
	}
    if(_lwrite(fd,(LPSTR)&actprinter,sizeof(int)) < sizeof(int))
	{
	_lclose(fd);
	DecFile;
	DBGfile((".. can't write current printer no.\n"));
	return PRINTERFILEERROR;
	}
    if(_lclose(fd) == -1)
	{
	DecFile;
	DBGfile((".. can't close TTY.DAT.\n"));
	return PRINTERFILEERROR;
	}
    DecFile;
    return PRINTEROK;

}	// SetCurPrinter()

int far PASCAL GetNumPrinters()
{
    int	    fd;

    DBGfile(("GetNumPrinters().\n"));

    if( (fd = OpenFile((LPSTR)PrinterFileName,
			(LPOFSTRUCT)&os,OF_READWRITE)) == -1)
	{
	DBGfile((".. can't open TTY.DAT.\n"));
	return PRINTERFILEERROR;
	}

    #ifdef DEBUG
    iOpen++;
    DBGfile((".. handle = %d. iOpen =%d\n", fd, iOpen));
    #endif

    if(_llseek(fd,NUMPRINTERS,SEEK_SET)== -1L)
	{
	_lclose(fd);
	DecFile;
	DBGfile((".. can't seek to numprinters.\n"));
	return PRINTERFILEERROR;
	}
    if(_lread(fd,(LPSTR)&actprinter,sizeof(int)) < sizeof(int))
	{
	_lclose(fd);
	DecFile;
	DBGfile((".. can't read numprinters.\n"));
	return PRINTERFILEERROR;
	}
    if(_lclose(fd) == -1)
	{
	DecFile;
	DBGfile((".. can't close TTY.DAT.\n"));
	return PRINTERFILEERROR;
	}
    DBGfile((".. numprinters = %d\n", numprinters));
    DecFile;
    return actprinter;

}	// GetNumPrinters()

int far PASCAL DeletePrinter(int number)
{
    HANDLE	clp;
    int		fd;
    long	offs;
    WORD	readed;
    LPSTR	lpBuf;
    static DWORD TamBloque = 0;

    DBGfile(("DeletePrinter(%d)\n", number));

    if(number <=0)
	return PRINTERNOTFOUND;

    if( (fd = OpenFile((LPSTR)PrinterFileName,
		(LPOFSTRUCT)&os,OF_READWRITE)) == -1)
	{ /* MessageBeep(MB_OK); */
	return PRINTERFILEERROR;
	}

    #ifdef DEBUG
    iOpen++;
    DBGfile((".. handle = %d. iOpen =%d\n", fd, iOpen));
    #endif

    if(_llseek(fd,NUMPRINTERS,SEEK_SET)== -1L)
	{
	_lclose(fd);
	DecFile;
	return PRINTERFILEERROR;
	}

    if(_lread(fd,(LPSTR)&numprinters,sizeof(int)) < sizeof(int))
	{
	_lclose(fd);
	DecFile;
	return PRINTERFILEERROR;
	}

    if(number > numprinters)
	{
	_lclose(fd);
	DecFile;
	return PRINTERNOTFOUND;
	}

    if(number < numprinters)
	{
	offs = (long)( number*sizeof(Printer) + 2*sizeof(int));

	TamBloque = (numprinters - number) * sizeof(Printer);

	for(clp = NULL; TamBloque >= sizeof(Printer); TamBloque /= 2 )
	    if(clp = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, TamBloque))
		break;

	if( !clp )
	    {
	    _lclose(fd);
	    DecFile;
	    return PRINTERFILEERROR;
	    }

	lpBuf = GlobalLock(clp);

	do
	    {
	    if(_llseek(fd, offs, SEEK_SET) == -1L)
		{
		_lclose(fd);
		DecFile;
		GlobalUnlock(clp);
		GlobalFree(clp);
		return PRINTERFILEERROR;
		}

	    if( (readed = _lread(fd, lpBuf, (unsigned)TamBloque)) <= 0)
		break;

	    if( _llseek(fd, offs-sizeof(Printer), SEEK_SET) == -1L)
		{
		_lclose(fd);
		DecFile;
		GlobalUnlock(clp);
		GlobalFree(clp);
		return PRINTERFILEERROR;
		}

	    if( _lwrite(fd, lpBuf, readed) < readed )
		{
		_lclose(fd);
		DecFile;
		GlobalUnlock(clp);
		GlobalFree(clp);
		return PRINTERFILEERROR;
		}

	    offs += TamBloque;

	    }while(1);

	GlobalUnlock(clp);
	GlobalFree(clp);
	}

    if(_llseek(fd,NUMPRINTERS,SEEK_SET)== -1L){
	_lclose(fd);
	DecFile;
	return PRINTERFILEERROR;
    }

    numprinters--;

    if(_lwrite(fd, (LPSTR)&numprinters, sizeof(int)) < sizeof(int))
	{
	_lclose(fd);
	DecFile;
	return PRINTERFILEERROR;
	}

    if(_lclose(fd) == -1)
	{
	DecFile;
	return PRINTERFILEERROR;
	}

    DecFile;
    return PRINTEROK;

}	// DeletePrinter()


unsigned char near PASCAL CodeCopy(lpEscapePair, lpCode)
LPESCAPEPAIR	lpEscapePair;
LPSTR	    lpCode;
{
    int l;

    lpEscapePair->code = lpCode;

    for(l = 0; *lpCode; l++, lpCode++)
	if (*lpCode == (unsigned char) '\xff')
	    *lpCode = 0;

    lpEscapePair->length = l;
}


int far PASCAL LoadThePrinter(lpEscapecode)
LPESCAPECODE  lpEscapecode;
{
    int actual;
    int i;
    BOOL bAnyFonts;

    if(!GetPrinterFileName())
	{
	return PRINTERFILEERROR;
	}
    if( (actual = GetCurPrinter()) == PRINTERFILEERROR)
	return actual;
    if( actual = GetPrinter(actual))
	return actual;
    CodeCopy(&lpEscapecode->compress_on, Printer.piPrinterData.pc16cpi);
    CodeCopy(&lpEscapecode->elite_on, Printer.piPrinterData.pc12cpi);
    CodeCopy(&lpEscapecode->pica_on, Printer.piPrinterData.pc10cpi);
    CodeCopy(&lpEscapecode->expand_on, Printer.piPrinterData.pcDoubleOn);
    CodeCopy(&lpEscapecode->expand_off, Printer.piPrinterData.pcDoubleOff);
    CodeCopy(&lpEscapecode->reset, Printer.piPrinterData.pcReset);

    // Initialize the bHasFont[] array.  Fonts are supported if they
    // have escapes.

    bHasFont[0] = isstring(Printer.piPrinterData.pc16cpi);  // 17 cpi
    bHasFont[1] = isstring(Printer.piPrinterData.pc12cpi);  // 12 cpi
    bHasFont[2] = isstring(Printer.piPrinterData.pc10cpi);  // 10 cpi

    // Do the double-width escapes exist?
    if (isstring(Printer.piPrinterData.pcDoubleOn) &&
	isstring(Printer.piPrinterData.pcDoubleOff) )
	{
	bHasFont[3] = bHasFont[0];	// 8 cpi
	bHasFont[4] = bHasFont[1];	// 6 cpi
	bHasFont[5] = bHasFont[2];	// 5 cpi
	}
    else
	bHasFont[5] = bHasFont[4] = bHasFont[3] = FALSE;

    // if NO font escapes exist, we default to 10 cpi.  The printer will
    // be sent no escapes for font selection, and we hope telling app's we
    // are at 10 cpi will not put too many chars per line.
    for (i = 0, bAnyFonts = FALSE; i < NFONTS; i++)
	if (bHasFont[i])
	    bAnyFonts = TRUE;

    if (!bAnyFonts)			// if NO font escapes defined,
	bHasFont[2] = TRUE;		// set 10 cpi

    return PRINTEROK;
}

BOOL isstring(a)
char *a;
{
    if (*a)
	if (*a != 0xff)
	    return TRUE;

    return FALSE;
}

//int stringlen(a)
//char *a;
//{
//    int l;
//
//    for(l=0; *a; l++, a++);
//    return l;
//}

//void stringcat(a,b)
//char *a,*b;
//{
//    for( ; *a; a++);
//    for( ; *a = *b; a++, b++);
//}
