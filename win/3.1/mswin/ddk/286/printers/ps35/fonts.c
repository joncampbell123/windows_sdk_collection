/**[f******************************************************************
 * fonts.c (soft fonts)
 *
 *
 **f]*****************************************************************/

/*********************************************************************
 * 8Jan89	chrisg	created (moved from enable.c and sucked in
 *			fontdir.c)
 *
 *********************************************************************/

#include "pscript.h"
#include <winexp.h>
#include "psdata.h"
#include "debug.h"
#include "utils.h"
#include "resource.h"
#include "etm.h"
#include "fonts.h"
#include "profile.h"
#include "truetype.h"
#include "getdata.h"

typedef  short  huge  *HPSHORT;
typedef  BYTE   huge  *HPBYTE;

#define MAX_SOFT_FONTS 1024	/* limit for softfonts=# win.ini entry */

#define NUM_INITIAL_PRINTERS    (NUM_INT_PRINTERS + 5)
#define PRINTER_INCREMENT       5

#define SCALE_PRINTER(p)	((p) - INT_PRINTER_MIN)

typedef struct {
	int	LockCount;	/* lock count for printer font directory */
	HANDLE	FontsHandle;	/* handle for printer font data */
} PRINTER_FONTS;

extern BOOL bTTEnabled;

/*---------------------------- local data ---------------------------*/

PRINTER_FONTS *PrinterFonts = NULL;
int giNumPrinters;

/*--------------------------- local functions -------------------------*/


long PASCAL LoadSoftFonts(HPBYTE, int far *, LPSTR, LPSTR);
int PASCAL LoadDirEntry(LPSTR, HPBYTE );
BOOL NEAR PASCAL MakeRoomForPrinter(int iPrinter);



/*********************************************************************
 * Name: LoadFontDir()
 *
 * Action: Load the specified font directory into memory from the
 *	  resource and append the font metrics for any
 *	  external softfonts if they exist.
 *
 * return TRUE on success
 * return FALSE on failure
 *
 *********************************************************************/

BOOL FAR PASCAL LoadFontDir(iPrinter, lszFile)
int	iPrinter;
LPSTR	lszFile;	    /* ptr to the output filename string */
{
	HANDLE	hres;	    /* The font directory resource */
	HANDLE	h;
	int	cSoftFonts;	    /* The number of soft fonts */
	int	cFonts; 	    /* The number of fonts */
	int	fh;		    /* The resource file handle */
	long	cb;		    /* Byte count */
	int	i;
    HPBYTE  lpbDir, lpbDst;
	PS_RES_HEADER header;
    LOGFONT lf;
    TEXTXFORM ft;
    BYTE  DevName[32];

    //  just obtain the DevName for this printer, needed by SetKey
#if 0
    //  this code is unnecessary as long as we expect softfont info
    //  in [PostScript,port]
    { /* begin block */
      PPRINTER  pprinter;
      pprinter = GetPrinter(iPrinter);
      if(pprinter == NULL)
         return(FALSE);
      lstrcpy((LPSTR)DevName, (LPSTR)pprinter->Name);
      FreePrinter(iPrinter);
   }
#endif
	DBMSG(("LoadFontDir(): iPrinter:%d\n", iPrinter));


        /* make sure that the PrinterFonts table is big enough to hold at
         * least iPrinter printers and expand it if not.
         */
        if (!MakeRoomForPrinter(iPrinter))
            return FALSE;

	/* If the font directory is already loaded, just return */

	if (lpbDir = LockFontDir(iPrinter)) {

		UnlockFontDir(iPrinter);

		PrinterFonts[SCALE_PRINTER(iPrinter)].LockCount++;

		return TRUE;
	}


	fh = -1;

	if (iPrinter >= EXT_PRINTER_MIN) {
	
		if ((fh = GetExternPrinter(iPrinter - EXT_PRINTER_MIN + 1)) == -1) {
			DBMSG(("external printer failed\n"));
			return FALSE;
		}

		if (_lread(fh, (LPSTR)&header, sizeof(header)) != sizeof(header)) {
			DBMSG(("external printer read failed\n"));
			return FALSE;
		}

		_llseek(fh, header.dir_loc, 0);	// position the file pointer

		DBMSG(("dir file:%ls fh:%d\n", (LPSTR)buf, fh));
	} else {

		/* Find the font directory */
		if (!(hres = FindResource(ghInst, 
			MAKEINTRESOURCE(iPrinter),
			MAKEINTRESOURCE(MYFONTDIR)))) {
			return FALSE;
		}
	}


	/* There will be two passes through the following code. */
	/* Once to compute the directory size, then again to load it */

AGAIN:

        /* start by computing amount of space Font Engine will need */
        if (bTTEnabled) {
            lf.lfFaceName[0] = 'A';     /* give it an arbitrary font */
            lf.lfFaceName[1] = '\0';
            if (!(cb = EngineRealizeFont((LPLOGFONT) &lf, (LPTEXTXFORM) &ft, 
                                        NULL))) {
                DBMSG(("LoadFontDir: Engine Size Inquiry Failed!\n"));

	        UnlockFontDir(iPrinter);
	        DeleteFontDir(iPrinter);
	        return FALSE;
            }
            cb *= 2;    // !!! another GDI hack
        } else {
            cb = 0;
        }

        /* if storage has been allocated put offset to actual directory
        * in the first word and move lpbDir past Engine data.
        */
        if (lpbDir) {
	    *((HPSHORT)lpbDir)++ = (short)cb + 3 * sizeof(WORD);
            lpbDir += cb + sizeof(WORD);
	    *((HPSHORT)lpbDir)++ = (short)cb + 2 * sizeof(WORD);
        }

        /* add in a length field before and after engine data so that we 
         * can skip to font directory or engine data.
         */
        cb += 3 * sizeof(WORD);     

	if (fh != -1) {
		cb += header.dir_len;		// get the dir size
	} else
		cb += SizeofResource(ghInst, hres);


	DBMSG(("size of font dir %d\n", cb));

	lpbDst = lpbDir;

	if (lpbDir) {
                if (fh == -1) {
                    /* reading directory from resource file */

                    HANDLE h;
                    LPSTR  lpStr;
        
                    h = LoadResource(ghInst, hres);
                    lpStr = LockResource(h);
                    if (lpStr) {
                        lmemcpy(lpbDir, lpStr, (short)cb - ((HPSHORT)lpbDir)[FDIR_TTDF] - 2 * sizeof(WORD));
                    }
                    UnlockResource(h);

                    /* if we couldn't read font directory give up */
                    if (!lpStr) {
			UnlockFontDir(iPrinter);
			DeleteFontDir(iPrinter);
			return FALSE;
                    }
                } else {
                    /* reading directory from external file */

		    if ((int)_lread(fh, lpbDir, header.dir_len) < 0) {
			    cb = 0;
			    DBMSG(("read failed\n"));
		    }
		    _lclose(fh);

		    if (cb <= 0) {
			    UnlockFontDir(iPrinter);
			    DeleteFontDir(iPrinter);
			    return FALSE;
		    }
                }

		/* Compute ptr to first "softfont" slot */
		cFonts = *((HPSHORT)lpbDst)++;

		DBMSG(("cFonts = %d\n", cFonts));

		for (i = 0; i < cFonts; ++i)
			lpbDst += *((HPSHORT)lpbDst);
	}

	/* Extend the font directory by adding any softfonts */
	cb += LoadSoftFonts(lpbDst, (int far *)&cSoftFonts, DevName, lszFile);

	/* Update the font count at the beginning of the directory */
	if (lpbDir) {
		*((HPSHORT)lpbDir) = cFonts + cSoftFonts;
		((HPSHORT)lpbDir)[FDIR_CSOFTFONTS] = cSoftFonts;
        }

	/* At this point we may have only computed the font directory size.
	 * If so, then allocate memory for it and go back to load it. */

	if (!lpbDir) {
		if (h = GlobalAlloc(GHND | GMEM_SHARE, (long) cb)) {

			PrinterFonts[SCALE_PRINTER(iPrinter)].FontsHandle = h;
			PrinterFonts[SCALE_PRINTER(iPrinter)].LockCount = 1;
			if (lpbDir = LockFontDir(iPrinter))
				goto AGAIN;
			DeleteFontDir(iPrinter);
		}
		return FALSE;
	} else {
		UnlockFontDir(iPrinter);
	}
	return TRUE;
}


/***********************************************************
* Name: LoadDirEntry()
*
* Action: Calculate the size of the softfont directory and
*	  load it (if storage has been allocated for it).
*
************************************************************/

int PASCAL LoadDirEntry(lszFile, lpbDst)
LPSTR lszFile;		/* The PFM file name */
HPBYTE  lpbDst;		/* Ptr to place to load the entry */
{
	int	fh;
	int	cbEntry;	/* The size of the directory entry */
	int	cbdf;		/* The size of the device font header */
	int	cbDevice;	/* The size of the device name */
	int	cbFace; 	/* The size of the face name */
	int	cbFont; 	/* Size of the font name */
	LPSTR	lpbSrc;
	int	i;
	PFM	pfm;
	char	szDevType[32];
	char	szFace[32];
	LPSTR	ptr;
	char	old_char;
	LONG	lStart;

	/* traverse the softfont entry, and chop off the download file name
	 * if necessary. "softfontX=d:\path\file.pfm[,d:\path\file.pfb]" */

	ptr = lszFile;
	while (*ptr && *ptr != ',')
            ptr = AnsiNext(ptr);

	if (old_char = *ptr)
		*ptr = 0;	/* chop off extra stuff */


	fh = _lopen(lszFile, READ);

	/* look for softfont as a resource here */
	if (fh < 0) {

		HANDLE hData;

		DBMSG(("try softfont as resorce %ls\n", lszFile));

		hData = FindResource(ghInst, lszFile, MAKEINTRESOURCE(MYFONT));

		if (hData) {
			DBMSG(("softfont using resource!\n"));
			fh = AccessResource(ghInst, hData);
		}
	}

	if (fh < 0)
		return 0;

	/* get start file position incase we are reading as a resource */

	lStart = _llseek(fh, 0L, SEEK_CUR);

	DBMSG(("lStart %ld\n", lStart));

	if (_lread(fh, (LPSTR)&pfm, sizeof(PFM)) != sizeof(PFM))
		goto READERR;

	_llseek(fh, lStart, 0);		/* return to start */

	_llseek(fh, (long)pfm.df.dfFace, SEEK_CUR);
	if (_lread(fh, szFace, sizeof(szFace)) <= 0)
		goto READERR;

	_llseek(fh, lStart, 0);		/* return to start */

	_llseek(fh, (long)pfm.df.dfDevice, SEEK_CUR);
	if (_lread(fh, szDevType, sizeof(szDevType)) <= 0)
		goto READERR;

	_lclose(fh);


	*ptr = old_char;	/* resore ',' if necessary */

	cbdf     = ((LPSTR) & pfm.df.dfDriverInfo) - (LPSTR) & pfm;
	cbFace   = lstrlen(szFace) + 1;
	cbDevice = lstrlen(szDevType) + 1;
	cbFont   = lstrlen(lszFile) + 1;
	cbEntry  = cbdf + cbFace + cbDevice + cbFont + 5;  // the '$'

	if (lpbDst) {
		*((HPSHORT)lpbDst)++ = cbEntry;
		*((HPSHORT)lpbDst)++ = cbdf + cbFace + cbDevice;


		lpbSrc = (LPSTR) & pfm.df;
		pfm.df.dfFace = cbdf;
		pfm.df.dfDevice = cbdf + cbFace;
		for (i = 0; i < cbdf; ++i)
			*lpbDst++ = *lpbSrc++;

		/* Copy the face name into the font directory */
		lpbSrc = szFace;
		for (i = 0; i < cbFace; ++i)
			*lpbDst++ = *lpbSrc++;

		/* Copy the device name into the font directory */
		lpbSrc = szDevType;
		for (i = 0; i < cbDevice; ++i)
			*lpbDst++ = *lpbSrc++;

		/* Copy the PFM file name into the font directory */
		*lpbDst++ = '$';	 /* Mark this as a softfont */
		lpbSrc = lszFile;
		for (i = 0; i < cbFont ; ++i)
			*lpbDst++ = *lpbSrc++;
	}
	return(cbEntry);

READERR:
	_lclose(fh);
	return(0);
}



/****************************************************************
* Name: LoadSoftFonts()
*
* Action: Either load the soft fonts into memory or get information
*	  about them depending on the value of the destination pointer.
*	  If the destination pointer is NULL, then this routine just
*	  computes the size and number of softfonts specified in
*	  win.ini.  If the destination pointer is not NULL, then
*	  the soft fonts are actually loaded.
* in:
*	lpbDir		font directory, if NULL just compute size
*   szDevName   Printer name, used to read win.ini
*	lszFile		port name, used to read win.ini
*
* out:
*	*lpcFonts	gets the number of softfonts loaded
*	
* returns:
*	the size of the softfonts in bytes
*
*********************************************************************/

long PASCAL LoadSoftFonts(lpbDir, lpcFonts, lszDevName, lszFile)
HPBYTE lpbDir;		/* ptr to font directory */
int far *lpcFonts;	/* output # of soft fonts found */
LPSTR  lszDevName;    //  printer name for reading win.ini
LPSTR lszFile;		/* port name for reading win.ini */
{
	char	szField[20];
	char	szFile[90];	/* must hold stuff after "softfontX=" */
	int	cFonts;
	int	i;
	long	cbDir;
	int	cbEntry;
	char	key[50];

	/* I'll bet this isn't the right way to do it...the name of the driver
	 * should come from somewhere central... */

//	SetKey(key, lszDevName, lszFile);
//  no... for backword compatibility, Third Party Font installers
//  write their info into win.ini [PostScript,port]

{
	static char szDevType[] = "PostScript,";	/* used to make win.ini keys */
	LPSTR ptr;

	lstrcpy(key, szDevType);
	lstrcat(key, lszFile);

	ptr = key;
	while (*ptr && *ptr != ':')	/* search for ":" */
		ptr++;

	*ptr = 0;			/* and toast it if found */
}


	DBMSG((">LoadSoftFonts(): key=%ls, ", (LPSTR)key));

	cbDir = 0;
	cFonts = GetProfileInt(key, "softfonts", 0);

	DBMSG(("# softfonts %d\n", cFonts));

	if (cFonts > MAX_SOFT_FONTS)
		cFonts = MAX_SOFT_FONTS;

	*lpcFonts = 0;		// start with no softfonts

	for (i = 1; i <= cFonts; ++i) {

		wsprintf(szField, "softfont%d", i);

		GetProfileString(key, szField, szNull, szFile, sizeof(szFile));

		DBMSG(("softfont%d=%ls\n", i, (LPSTR)szFile));

		cbEntry = LoadDirEntry(szFile, lpbDir);

		if (cbEntry) {			// only if successful

			(*lpcFonts)++;		// inc count of softfonts

			if (lpbDir)
				lpbDir += cbEntry;
			cbDir += cbEntry;
		}
	}

	DBMSG(("<LoadSoftFonts()\n"));
	return cbDir;
}



/********************************************************
 * Name: LockFontDir()
 *
 * iPrinter	printer who's font dir should be locked
 *
 * Action: Lock the specified font directory and return a pointer to it.
 *
 * Returns: A pointer to the locked font directory.
 *
 **********************************************************/

LPSTR FAR PASCAL LockFontDir(iPrinter)
int iPrinter;
{
    LPSTR lpbDir;

    iPrinter = SCALE_PRINTER(iPrinter);

    lpbDir = NULL;

    if (iPrinter < giNumPrinters && PrinterFonts[iPrinter].FontsHandle) {
        lpbDir = GlobalLock(PrinterFonts[iPrinter].FontsHandle);
        if (lpbDir)
            lpbDir += *((LPSHORT) lpbDir);
    }

    return lpbDir;
}


/**************************************************************
 * Name: UnlockFontDir()
 *
 * iPrinter	printer who's font dirctory should be unlocked
 *
 * Action: This routine unlocks the specified font directory.
 *
 ***************************************************************/

void FAR PASCAL UnlockFontDir(iPrinter)
int iPrinter;
{
	iPrinter = SCALE_PRINTER(iPrinter);

	if (iPrinter < giNumPrinters && PrinterFonts[iPrinter].FontsHandle)
		GlobalUnlock(PrinterFonts[iPrinter].FontsHandle);
}




/*******************************************************************
 * Name: DeleteFontDir()
 *
 * Action: Delete a font directory and free its memory if this is
 *	  the last reference to it.
 *
 *******************************************************************/

void FAR PASCAL DeleteFontDir(iPrinter)
int iPrinter;
{
	iPrinter = SCALE_PRINTER(iPrinter);

	if (iPrinter < giNumPrinters && 
	    PrinterFonts[iPrinter].FontsHandle) {
		if (--PrinterFonts[iPrinter].LockCount <= 0) {
			GlobalFree(PrinterFonts[iPrinter].FontsHandle);
	    		PrinterFonts[iPrinter].LockCount = 0;
		    	PrinterFonts[iPrinter].FontsHandle = NULL;
		}
	}
}


BOOL NEAR PASCAL MakeRoomForPrinter(int iPrinter)
{
    if (!PrinterFonts) {
        giNumPrinters = max(NUM_INITIAL_PRINTERS, iPrinter+1);
        PrinterFonts = (PRINTER_FONTS *)LocalAlloc(LPTR, giNumPrinters * sizeof(PRINTER_FONTS));
        if (!PrinterFonts)
            return FALSE;
    }

    if (iPrinter >= giNumPrinters) {
        HANDLE h;
        int iNewNumPrinters;

        iNewNumPrinters = giNumPrinters + PRINTER_INCREMENT;
        h = LocalReAlloc((HANDLE)PrinterFonts, 
                         iNewNumPrinters * sizeof(PRINTER_FONTS),
                         LMEM_ZEROINIT);
        if (h) {
            giNumPrinters = iNewNumPrinters;
            PrinterFonts = (PRINTER_FONTS *)h;
        } else
            return FALSE;
    }

    return TRUE;
}



