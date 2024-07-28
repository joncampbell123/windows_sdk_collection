/**[f******************************************************************
 * profile.c - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 **f]*****************************************************************/

/*********************************************************************
 * PROFILE.C
 *
 * 14Aug87 sjp    Moved MapProfile(), GetPaperType() and ReadProfile()
 *        from segment RESET.
 *
 * 88Jan13 chrisg    established MakeEnv and SaveEnv as means to
 *            change load/save envs and got rid of global
 *            devmode usage.
 *
 * 91Mar26 msd        Added support for Duplex.
 *
 * 91Jun12 peterwo  added new entry point DevInstall()
 *
 *********************************************************************/

#include "pscript.h"
#include <winexp.h>
#include "utils.h"
#include "debug.h"
#include "resource.h"
#include "defaults.h"
#include "psdata.h"
#include "profile.h"
#include "pserrors.h"
#include "getdata.h"
#include "dmrc.h"
#include "psver.h"
#include "control2.h"
#include "enum.h"
#include "etm.h"
#include "truetype.h"
             
/* temporary until windows.h gets real */
int  PASCAL GetInt( LPSTR, LPSTR, int, LPSTR );
int  PASCAL GetString( LPSTR, LPSTR, LPSTR, LPSTR, int, LPSTR );
BOOL PASCAL WriteString( LPSTR, LPSTR, LPSTR, LPSTR );
int FAR PASCAL fnSubFilterProc(LPLOGFONT lplf, LPTEXTMETRIC lptm, short nType,
                               DWORD dwData);


/* these should go in win.h */
int     FAR PASCAL GetPrivateProfileInt( LPSTR, LPSTR, int, LPSTR );
int     FAR PASCAL GetPrivateProfileString( LPSTR, LPSTR, LPSTR, LPSTR, int, LPSTR );
BOOL    FAR PASCAL WritePrivateProfileString( LPSTR, LPSTR, LPSTR, LPSTR );


/*--------------------------- global data ------------------------*/

#define MAXSUBS 128     /* maximum number of font substitutions */

extern HWND ghwndDlg;

/* GetPrinter cache.  Remember last printer requested in a local discardable
** memory block.
*/
extern HANDLE ghLastPrinter;
extern int giLastPrinter;

void    FAR PASCAL InitDefaults(HWND hwnd, PPRINTER pPrinter);


/*--------------------------- local functions ------------------------*/

int    PASCAL GetPaperType(LPSTR, int, LPSTR);
void    PASCAL WriteProfileInt(LPSTR, LPSTR, int, LPSTR);
void    PASCAL WriteProfile(LPSTR, LPPSDEVMODE, LPPSDEVMODE, LPSTR);
void    PASCAL ReadProfile(LPSTR, LPPSDEVMODE, LPSTR, LPSTR);

int NEAR PASCAL MatchPrinterFromWPD(LPSTR lpName);

int FAR PASCAL SubTabProc(LPLOGFONT lplf, LPTEXTMETRIC lptm, short nType,
                           LPSTR dwData);

/* use these until private profile gets fixed */

int PASCAL WriteString(LPSTR sec, LPSTR key, LPSTR buf, LPSTR pro )
{
    if (pro)
        return WritePrivateProfileString(sec, key, buf, pro);
    else
        return WriteProfileString(sec, key, buf);
}

int PASCAL GetString(LPSTR sec, LPSTR key, LPSTR def, LPSTR buf, int size, LPSTR pro )
{
    if (pro)
        return GetPrivateProfileString(sec, key, def, buf, size, pro);
    else
        return GetProfileString(sec, key, def, buf, size);
}


int PASCAL GetInt(LPSTR sec, LPSTR key, int def, LPSTR pro )
{
    if (pro)
        return GetPrivateProfileInt(sec, key, def, pro);
    else
        return GetProfileInt(sec, key, def);
}



int FAR PASCAL MatchPrinter(LPSTR lpName)
{
    PPRINTER pPrinter;
    char buf[40];
    int i;
    int num_ext;
    char szModel[32];

    // (10/21/91 ZhanW) support custom printer names
    if (GetProfileString("PrinterAliases", lpName, "", szModel, sizeof(szModel)))
    lpName = (LPSTR)szModel;

        /* Check the GetPrinter cache first before searching ... */
        if (giLastPrinter >= 0) {
            pPrinter = (PPRINTER)LocalLock(ghLastPrinter);
            if (pPrinter)
                i = lstrcmpi(pPrinter->Name, lpName);
            LocalUnlock(ghLastPrinter);

            if (!i)
                return giLastPrinter;
        }

    LoadString (ghInst, IDS_EXTPRINTERS, buf, sizeof(buf));
    num_ext = GetProfileInt(szModule, buf, 0);

    for (i = INT_PRINTER_MIN; i <= (INT_PRINTER_MAX + num_ext); i++) {

        if (pPrinter = GetPrinter(i)) {
        
            if (!lstrcmpi(pPrinter->Name, lpName)) {
                FreePrinter(pPrinter);
                return i;
            }

            FreePrinter(pPrinter);
        }
    }

        return MatchPrinterFromWPD(lpName);
}

LONG FAR PASCAL QueryDeviceNames(HANDLE hDriver, LPSTR lpaDeviceName)
{
    PPRINTER pPrinter;
    char buf[40];
    int i;
    int num_ext, num_printers;

    LoadString (ghInst, IDS_EXTPRINTERS, buf, sizeof(buf));
    num_ext = GetProfileInt(szModule, buf, 0);

    num_printers = 0;
    for (i = INT_PRINTER_MIN; i <= (INT_PRINTER_MAX + num_ext); i++) {

        if (pPrinter = GetPrinter(i)) {
    
                    ++num_printers;

                    if (lpaDeviceName) {
                        lstrcpy(lpaDeviceName, pPrinter->Name);
                        lpaDeviceName += 64;
                    }

            FreePrinter(pPrinter);
        }
    }

    return num_printers;
}

int FAR PASCAL OutputDebugString(LPSTR);

// do findfirst/findnext to get all the WPD files from the system
// directory.  If wpd file already in external printer list skip it.
// Otherwise load it in and see if the name matches.  If so, add it
// to the external printer list, update the internal data structures,
// and return its printer number.  If a matching driver isn't found
// return 0.
int NEAR PASCAL MatchPrinterFromWPD(LPSTR lpName)
{
    FCB fcb;
    char szSpec[160], buf[20];
    LPSTR lpTmp, lpszFilename;
    int rc, fh, cb;
    OFSTRUCT of;
    PPRINTER pPrinter;
    PS_RES_HEADER header;
    int nExtPrinters, nNewPrinter;
    BOOL  bFirstPass = TRUE;

    // build the search string
SECOND_PASS:
    if(bFirstPass)
        GetSystemDirectory(szSpec, 160);
    else 
        GetWindowsDirectory(szSpec, 160);

    lpszFilename = szSpec + lstrlen(szSpec);
    lpszFilename = AnsiPrev(szSpec, lpszFilename);
    if (*lpszFilename != '\\') {
        lpszFilename = AnsiNext(lpszFilename);
        *lpszFilename = '\\';
    }
    lpszFilename = AnsiNext(lpszFilename);
    *lpszFilename = '\0';
    lstrcat(szSpec, "*");
    lstrcat(szSpec, szRes);

    // assume we can't find printer by using error code
    nNewPrinter = -1;

    // process all of the WPD files in the SYSTEM directory...
    for (rc = DosFindFirst(&fcb, szSpec, ATTR_ARCHIVE); !rc;
         rc = DosFindNext(&fcb)) 
    {

        // open the WPD file
        lstrcpy(lpszFilename, fcb.szName);
        fh = OpenFile(szSpec, &of, OF_READ);
        if (fh == -1)
            continue;

        if (_lread(fh, (LPSTR)&header, sizeof(header)) != sizeof(header))
                goto DONE_CLOSE;

        cb = header.cap_len;         // get the cap size
        _llseek(fh, header.cap_loc, 0);    // position the file pointer

        if (!(pPrinter = (PPRINTER)LocalAlloc(LPTR, cb)))
            goto DONE_CLOSE;
    
        cb = _lread(fh, (LPSTR)pPrinter, cb);

        if (!lstrcmpi(pPrinter->Name, lpName)) 
        {
            LoadString(ghInst, IDS_EXTPRINTERS, szSpec, sizeof(szSpec));

            nExtPrinters = GetProfileInt(szModule, szSpec, 0);

            nExtPrinters++;    /* adding a new printer */

            nNewPrinter = nExtPrinters + EXT_PRINTER_MIN - 1;    

            wsprintf(buf, "%d", nExtPrinters);

            WriteProfileString(szModule, szSpec, buf);

            LoadString(ghInst, IDS_PRINTER, szSpec, sizeof(szSpec));

            wsprintf(buf, szSpec, nExtPrinters);

        // whack off extension
            for (lpTmp = fcb.szName; *lpTmp && *lpTmp != '.';
                lpTmp = AnsiNext(lpTmp))
                ;
            *lpTmp = '\0';

            WriteProfileString(szModule, buf, fcb.szName);

#ifdef APPLE_TALK
            if (ghwndDlg)
                InitDefaults(ghwndDlg, pPrinter);
#endif

            LocalFree((HANDLE)pPrinter);
            _lclose(fh);

            return nNewPrinter;
        }

        LocalFree((HANDLE)pPrinter);

DONE_CLOSE:
        _lclose(fh);

    }

    if(bFirstPass)
    {
        bFirstPass = FALSE;
        goto  SECOND_PASS ;
    }

    return nNewPrinter;
}


/****************************************************************************
 * void FAR PASCAL MakeEnvironment(lszDevType, lszFile, lpdm, lpProfile)
 *
 * fill the PSDEVMODE struct with a copy of the enviornment for *lszFile
 * and *lszDevType.  Then init this struct with the values from win.ini
 * 
 * in:
 *    hinst        driver instance handle
 *    lszDevType    device name "PostScript Printer" or
 *            "IBM Personal PP II", etc.
 *    lszFile        file or port name ("COM1:")
 *    lpProfile    file to read to (win.ini if NULL)
 *
 * out:
 *    lpdm    filled with initialized devmode struct
 *
 * returns:
 *    TRUE        if new env read from win ini
 *    FALSE        PSCRIPT env already existed
 *
 ****************************************************************************/

#define STR_EQ(s1, s2) (lstrcmpi((s1),(s2)) == 0)

BOOL FAR PASCAL MakeEnvironment(lszDevType, lszFile, lpdm, lpProfile)
LPSTR    lszDevType;
LPSTR    lszFile;
LPPSDEVMODE lpdm;
LPSTR    lpProfile;
{
    BOOL  result;
    BYTE  buffer[80];

    if (STR_EQ("PostScript Printer", lszDevType))
    {
        LPSTR  lpDrvname;
        int   i;
        BOOL  bTwoCommas = FALSE;

        lpDrvname = buffer;  // initialize before we get in trouble
        buffer[0] = '\0';

	GetProfileString("windows", "device", "", (LPSTR)buffer,
            80);

        //  this for loop expects that buffer[] contains three strings
        //  delimited by two commas.  It replaces both commas with NULLs
        //  and sets lpDrvname to point to the second string.

        for(i = 0 ; buffer[i] ; i++)
        {
            if(buffer[i] == ',')
            {
                buffer[i] = '\0';
                lpDrvname = buffer + ++i;
                for(  ; buffer[i] ; i++)
                {
                    if(buffer[i] == ',')
                    {
                        buffer[i] = '\0';
                        bTwoCommas = TRUE;
                        break;
                    }
                }
                break;
            }
        }
        if(bTwoCommas  &&  lpDrvname != buffer  &&  
              STR_EQ(lpDrvname, "pscript")  &&  
              !STR_EQ((LPSTR)buffer, lszDevType) )
            lszDevType = buffer;
        else
            lszDevType = "Apple LaserWriter Plus";

    }

    DBMSG(("\n>MakeEnv(%ls %ls %lp %ls)\n",
        lszDevType, lszFile, lpdm, lpProfile));

    /* copy the module name into the first part of the env incase
     * lszFile is the null port.  in this case GetEnv uses the first
     * string (our module name) for the env search */

    lstrcpy(lpdm->dm.dmDeviceName, szModule);

    /* if (env doesn't exists || does exists and doesn't belong to us) */

    if ((GetEnvironment(lszFile, (LPSTR)lpdm, sizeof(PSDEVMODE)) == 0) ||
        (lstrcmpi(lszDevType, lpdm->dm.dmDeviceName) != 0))
    {
             /* read env from win.ini */
        DBMSG(("\n MakeEnv read win.ini\n"));

        ReadProfile(lszFile, lpdm, lszDevType, lpProfile);

        result = TRUE;        /* was created */

    } else

        result = FALSE;        /* already existed */

    lpdm->dm.dmSize = sizeof(DEVMODE);
    lpdm->dm.dmSpecVersion = GDI_VERSION;
    lpdm->dm.dmDriverVersion = DRIVER_VERSION;
    lpdm->dm.dmDriverExtra = sizeof(PSDEVMODE) - sizeof(DEVMODE);

    lpdm->dm.dmFields = DM_ORIENTATION    |
                DM_PAPERSIZE    |
                DM_PAPERLENGTH    |
                DM_PAPERWIDTH    |
                DM_SCALE        |
                DM_COPIES        |
                DM_DEFAULTSOURCE    |
                DM_COLOR            |
                            DM_DUPLEX;


    if(lpdm->bSubFonts == TRUE)
        lpdm->dm.dmTTOption = DMTT_SUBDEV;
    else
        lpdm->dm.dmTTOption = DMTT_DOWNLOAD;

    lpdm->dm.dmFields |= DM_TTOPTION;

    DBMSG(("\n<MakeEnv() lszFile:%ls\n", lszFile));
    return result;
}


/****************************************************************************
 * void FAR PASCAL SaveEnvironment(lszDevType, lszFile, lpdm,
 *                   lpOrigDM, lpProfilefWriteProfile, fSendMsg)
 *
 * save the enviornemnt for lszDevType and lszFile defined by lpdm.
 * the new enviornemnt is saved for the current session by GDI with
 * SetEnviornment and optionally saved to win.ini. 
 * 
 * in:
 *    hinst        driver instance handle
 *    lszDevType    device type ("PostScript Printer")
 *    lszFile        file or port name ("COM1:")
 *    lpdm        enviornment to save
 *    lpProfile    profile to write to
 *    lpOrigDM    original devmode used to minimize writing to win.ini
 *            this may be NULL if fWriteProfile is FALSE
 *    fWriteProfile    write the new env to win.ini or not
 *    fSendMsg    set a message to all apps indicating devmode change
 *
 ****************************************************************************/

void FAR PASCAL SaveEnvironment(lszDevType, lszFile, lpdm, lpOrigDM,
                lpProfile, fWriteProfile, fSendMsg)
LPSTR        lszDevType;
LPSTR        lszFile;
LPPSDEVMODE    lpdm;
LPPSDEVMODE    lpOrigDM;
LPSTR        lpProfile;
BOOL        fWriteProfile;    /* TRUE write profile to win.ini */
BOOL        fSendMsg;    /* TRUE send message to all windows */
{
    BYTE  buffer[80];

    if (STR_EQ("PostScript Printer", lszDevType))
    {
        LPSTR  lpDrvname;
        int   i;
        BOOL  bTwoCommas = FALSE;

        lpDrvname = buffer;  // initialize before we get in trouble
        buffer[0] = '\0';

	GetProfileString("windows", "device", "", (LPSTR)buffer,
            80);

        //  this for loop expects that buffer[] contains three strings
        //  delimited by two commas.  It replaces both commas with NULLs
        //  and sets lpDrvname to point to the second string.

        for(i = 0 ; buffer[i] ; i++)
        {
            if(buffer[i] == ',')
            {
                buffer[i] = '\0';
                lpDrvname = buffer + ++i;
                for(  ; buffer[i] ; i++)
                {
                    if(buffer[i] == ',')
                    {
                        buffer[i] = '\0';
                        bTwoCommas = TRUE;
                        break;
                    }
                }
                break;
            }
        }
        if(bTwoCommas  &&  lpDrvname != buffer  &&  
              STR_EQ(lpDrvname, "pscript")  &&  
              !STR_EQ((LPSTR)buffer, lszDevType) )
            lszDevType = buffer;
        else
            lszDevType = "Apple LaserWriter Plus";
    }


    SetEnvironment(lszFile, (LPSTR)lpdm, sizeof(PSDEVMODE));

    if (fWriteProfile)
        WriteProfile(lszFile, lpdm, lpOrigDM, lpProfile);

    if (fSendMsg)
        SendMessage(-1, WM_PSDEVMODECHANGE, 0, (LONG)(LPSTR)lszDevType);
}



/**********************************************************************
 * Name: ReadProfile()
 *
 * Action:
 *    Read the device mode parameters from win.ini and fill lpdm
 *    structure with the results.  the win.ini section used is 
 *    "Post Script," concatenated with lszFile.  lszFile is a port or an
 *    output file name.  This info gets saved in win.ini when the user
 *    makes changes as well. the first time this is called we try
 *    to match the printer name (lszDevType) to those we know
 *    to auto configure to a certain printer.
 * 
 *    things not read from win.ini are set to resonable defaults.
 * 
 * note:
 *    some error checking code is to ensure that user changes to
 *    win.ini are not bogus.
 *
 **********************************************************************/

void PASCAL ReadProfile(lszFile, lpdm, lszDevType, lpProfile)
LPSTR        lszFile;    /* port used to form win.ini key */
LPPSDEVMODE    lpdm;        /* Far ptr to the device mode info */
LPSTR        lszDevType;
LPSTR        lpProfile;
{
    int    i;
    char    idsBuf[40];
    char    szKey[60];
    PPRINTER pPrinter;
    PPAPER pPaper, pP;
    BOOL rc = TRUE;

    SetKey((LPSTR)szKey, lszDevType, lszFile);

    DBMSG((">ReadProfile(): file=%ls\n", (LPSTR)lszFile));


    /* get the printer number from the "device=printer name"
     * section in win.ini */

    /* "device" is a resource string */
    LoadString (ghInst, IDS_DEVICE, idsBuf, sizeof(idsBuf));

    lpdm->iPrinter = MatchPrinter(lszDevType);

    pPrinter = NULL;

    if ((lpdm->iPrinter < INT_PRINTER_MIN) || 
        !(pPrinter = GetPrinter(lpdm->iPrinter))) {
        lpdm->iPrinter = DEFAULT_PRINTER;

        if (!(pPrinter = GetPrinter(lpdm->iPrinter))) 
            return;

        lstrcpy(lpdm->dm.dmDeviceName, pPrinter->Name);
    } else {
        lstrcpy(lpdm->dm.dmDeviceName, lszDevType);
    }

    LoadString(ghInst, IDS_PRINTERVM, idsBuf, sizeof(idsBuf));
    lpdm->iMaxVM = GetInt(szKey, idsBuf, pPrinter->iMaxVM, lpProfile);

    LoadString(ghInst, IDS_FAVORTT, idsBuf, sizeof(idsBuf));
    if(GetInt(szKey, idsBuf, 0, lpProfile))
        lpdm->bFavorTT = TRUE;
    else
        lpdm->bFavorTT = FALSE;

    LoadString(ghInst, IDS_LANDSCAPEORIENT, idsBuf, sizeof(idsBuf));
    lpdm->LandscapeOrient = GetInt(szKey, idsBuf, 90, lpProfile);
    if(lpdm->LandscapeOrient != 270)
        lpdm->LandscapeOrient = 90;

    LoadString(ghInst, IDS_RES, idsBuf, sizeof(idsBuf));
    lpdm->iRes = GetInt(szKey, idsBuf, pPrinter->defRes, lpProfile);

    LoadString(ghInst, IDS_SCREENFREQUENCY, idsBuf, sizeof(idsBuf));
    lpdm->ScreenFrequency = GetInt(szKey, idsBuf, pPrinter->ScreenFreq, lpProfile);

    LoadString(ghInst, IDS_SCREENANGLE, idsBuf, sizeof(idsBuf));
    lpdm->ScreenAngle = GetInt(szKey, idsBuf, pPrinter->ScreenAngle, lpProfile);

    LoadString(ghInst, IDS_CUSTOMWIDTH, idsBuf, sizeof(idsBuf));
    lpdm->iCustomWidth = GetInt(szKey, idsBuf, 0, lpProfile);

    LoadString(ghInst, IDS_CUSTOMHEIGHT, idsBuf, sizeof(idsBuf));
    lpdm->iCustomHeight = GetInt(szKey, idsBuf, 0, lpProfile);

    LoadString(ghInst, IDS_CUSTOMUNIT, idsBuf, sizeof(idsBuf));
    lpdm->dmSizeUnit = GetInt(szKey, idsBuf, 1, lpProfile);

    LoadString(ghInst, IDS_MINOUTLINEPPEM, idsBuf, sizeof(idsBuf));
    lpdm->iMinOutlinePPEM = GetInt(szKey, idsBuf, MINOUTLINEPPEM, lpProfile);

    lpdm->dm.dmCopies = GetInt(szKey, "Copies", 1, lpProfile);

    LoadString(ghInst, IDS_ADVFLAGS, idsBuf, sizeof(idsBuf));
    i = GetInt(szKey, idsBuf, 
                   ACCEPTS_TRUETYPE(pPrinter) ? ADVF_TIDEFAULTS : ADVF_DEFAULTS, 
                   lpProfile);

        lpdm->bNegImage = i & ADVF_NEGIMAGE;
        lpdm->bPerPage = i & ADVF_PERPAGE;
        lpdm->bDSC = i & ADVF_DSC;

        if(i & ADVF_TYPE3)
            lpdm->iDLFontFmt = DLFMT_TYPE3 ;
        else if(i & ADVF_TRUETYPE)
            lpdm->iDLFontFmt = DLFMT_TRUETYPE ;
#if 0
        else if(i & ADVF_NODOWNLOAD)
            lpdm->iDLFontFmt = DLFMT_NODOWNLOAD ;
#endif
        else
            lpdm->iDLFontFmt = DLFMT_TYPE1 ;

    lpdm->bNoDownLoad = i & ADVF_NODOWNLOAD;

        lpdm->bSubFonts = (i & ADVF_SUBFONTS) ? TRUE : FALSE;
        lpdm->bMirror = (i & ADVF_MIRROR) ? TRUE : FALSE;
        lpdm->bColorToBlack = (i & ADVF_COLORTOBLACK) ? TRUE : FALSE;
        lpdm->bCompress = (i & ADVF_COMPRESS) ? TRUE : FALSE;
        lpdm->bErrHandler = (i & ADVF_ERRHANDLER) ? TRUE : FALSE;

    LoadString(ghInst, IDS_PAPERSOURCE, idsBuf, sizeof(idsBuf));

    lpdm->dm.dmDefaultSource = GetInt(szKey, idsBuf, pPrinter->defFeed, lpProfile);

    if (lpdm->dm.dmDefaultSource < DMBIN_FIRST || 
        lpdm->dm.dmDefaultSource > DMBIN_LAST)
        lpdm->dm.dmDefaultSource = pPrinter->defFeed;

    lpdm->dm.dmPaperSize = GetPaperType(szKey, lpdm->dm.dmDefaultSource, lpProfile);

    /* in case user messed up WIN.INI value */
    if (!PaperSupported(pPrinter, lpdm, lpdm->dm.dmPaperSize))
        lpdm->dm.dmPaperSize = GetDefaultPaper();


    /* search for the paper metrics for the current printer and
     * set the devinit fields accordingly */

    pPaper = GetPaperMetrics(pPrinter, lpdm);

    if (pPaper) {

        for (pP = pPaper; pP->iPaper; pP++)
            if (pP->iPaper == lpdm->dm.dmPaperSize)
                break;

        if (pP->iPaper) {
            lpdm->dm.dmPaperWidth  = Scale(pP->cxPaper, 254, 100);
            lpdm->dm.dmPaperLength = Scale(pP->cyPaper, 254, 100);

            DBMSG(("Paper size (in mm) %d %d\n", 
                lpdm->dm.dmPaperWidth, lpdm->dm.dmPaperLength)); 
        }

        LocalFree((HANDLE)pPaper);
    } else {
        DBMSG(("GetPaperMetrics() failed!!\n"));
    }

    LoadString (ghInst, IDS_JOBTIMEOUT, idsBuf, sizeof(idsBuf));
    lpdm->iJobTimeout = GetInt(szKey, idsBuf, DEFAULTJOBTIMEOUT, lpProfile);

    /* keep positive */
    if (lpdm->iJobTimeout < 0)
        lpdm->iJobTimeout = DEFAULTJOBTIMEOUT;

    LoadString (ghInst, IDS_ORIENTATION, idsBuf, sizeof(idsBuf));
    lpdm->dm.dmOrientation = GetInt(szKey, idsBuf, DEFAULTORIENTATION, lpProfile);

    if (IS_DUPLEX(pPrinter)) {
        LoadString (ghInst, IDS_DUPLEX, idsBuf, sizeof(idsBuf));
        lpdm->dm.dmDuplex = GetInt(szKey, idsBuf, DEFAULTDUPLEX, lpProfile);
    } else {
        lpdm->dm.dmDuplex = 0;
    }

#if 0
    // binary stuff not supported anymore.  we now compress bitmaps
    // if we want to do this we should redefine readhexstring and readstring
    // in the header.  and then do all binary output in bitblt and strchblt

    LoadString (ghInst, IDS_BINARYIMAGE, idsBuf, sizeof(idsBuf));
    lpdm->fBinary = GetInt(szKey, idsBuf, FALSE, lpProfile);
#endif

    if (pPrinter->fColor) {
        LoadString (ghInst, IDS_COLOR, idsBuf, sizeof(idsBuf));
        lpdm->dm.dmColor = GetInt(szKey, idsBuf, DEFAULT_COLOR, lpProfile);
    } else
        lpdm->dm.dmColor = DMCOLOR_MONOCHROME;


    for (i = DMBIN_FIRST; i <= DMBIN_LAST; i++) {

        lpdm->rgiPaper[i] = GetPaperType(szKey, i, lpProfile);

        DBMSG((" ReadProfile(): [%d]%d\n", i, lpdm->rgiPaper[i]));
    }

    /* get the header flag */

    LoadString (ghInst, IDS_HEADER, idsBuf, sizeof(idsBuf));
    lpdm->fHeader = GetInt(szKey, idsBuf, TRUE, lpProfile);

    LoadString (ghInst, IDS_MARGINS, idsBuf, sizeof(idsBuf));
    lpdm->marginState = GetInt(szKey, idsBuf, DEFAULT_MARGINS, lpProfile);

    if (lpdm->marginState < DEFAULT_MARGINS || lpdm->marginState > ZERO_MARGINS)
        lpdm->marginState = DEFAULT_MARGINS;

    // 2/27/92 - #15648 - Read EPS setting from profile
    GetString(szKey, "EpsFile", NULL, lpdm->EpsFile, CHEPSFILE, lpProfile);
    lpdm->fDoEps = GetInt(szKey, "EPS", FALSE, lpProfile);

    lpdm->dm.dmScale = GetInt(szKey, "scale", 100, lpProfile);

    DBMSG((" ReadProfile(): iPrinter=%d dmDefaultSource=%d dmPaperSize=%d iRes=%d iJT=%d dmOrient=%d\n",
        lpdm->iPrinter, lpdm->dm.dmDefaultSource, lpdm->dm.dmPaperSize, lpdm->iRes,
        lpdm->iJobTimeout, lpdm->dm.dmOrientation));

    FreePrinter(pPrinter);

   /* initialize substitution handle to 0 */
   lpdm->hSubTable = 0;

    DBMSG(("<ReadProfile() \n"));
}


void PASCAL WriteProfileInt(lszApp, lszKey, iVal, lpProfile)
LPSTR    lszApp;
LPSTR    lszKey;
int    iVal;
LPSTR    lpProfile;
{
    char    sz[10];

    wsprintf(sz, "%d", iVal);

    WriteString(lszApp, lszKey, sz, lpProfile);
}


/*******************************************************************
 * Name: WriteProfile()
 *
 * Action: Write the device mode parameters out to win.ini.
 *
 *******************************************************************/

void PASCAL WriteProfile(lszFile, lpdm, lpOrigDM, lpProfile)
LPSTR        lszFile;    /* Ptr to the com port's file name */
LPPSDEVMODE    lpdm;        /* new PSDEVMODE to write */
LPPSDEVMODE    lpOrigDM;    /* old copy to save unnecessary writes */
LPSTR        lpProfile;
{
    char    szKey[64];
    char    sz[64];
    char    idsBuf[40];
    char    buf[10];    /* used for wsprintf */
    int    i,iFeed;
    PPRINTER pPrinter;
    PPRINTER pOrigPrinter;

    DBMSG((">WriteProfile()\n"));


    if (!(pPrinter = GetPrinter(lpdm->iPrinter)))
        return;

    if (!(pOrigPrinter = GetPrinter(lpOrigDM->iPrinter))) {
        FreePrinter(pPrinter);
        return;
    }
#if 0
    SetKey((LPSTR)szKey, (LPSTR)pPrinter->Name, lszFile);
#else
    // save under [<custom name>,<port>]
    SetKey((LPSTR)szKey, (LPSTR)lpdm->dm.dmDeviceName, lszFile);
#endif
        if (lpdm->iMaxVM != lpOrigDM->iMaxVM) {
        LoadString(ghInst, IDS_PRINTERVM, idsBuf, sizeof(idsBuf));
            WriteProfileInt(szKey, idsBuf, lpdm->iMaxVM, lpProfile);
        }

        if (lpdm->iRes != lpOrigDM->iRes) {
        LoadString(ghInst, IDS_RES, idsBuf, sizeof(idsBuf));
            WriteProfileInt(szKey, idsBuf, lpdm->iRes, lpProfile);
        }

        if (lpdm->ScreenFrequency != lpOrigDM->ScreenFrequency) {
        LoadString(ghInst, IDS_SCREENFREQUENCY, idsBuf, sizeof(idsBuf));
            WriteProfileInt(szKey, idsBuf, lpdm->ScreenFrequency, lpProfile);
        }

        if (lpdm->ScreenAngle != lpOrigDM->ScreenAngle) {
        LoadString(ghInst, IDS_SCREENANGLE, idsBuf, sizeof(idsBuf));
            WriteProfileInt(szKey, idsBuf, lpdm->ScreenAngle, lpProfile);
        }

        if (lpdm->iCustomWidth != lpOrigDM->iCustomWidth) {
            LoadString(ghInst, IDS_CUSTOMWIDTH, idsBuf, sizeof(idsBuf));
            WriteProfileInt(szKey, idsBuf, lpdm->iCustomWidth, lpProfile);
        }

        if (lpdm->iCustomHeight != lpOrigDM->iCustomHeight) {
            LoadString(ghInst, IDS_CUSTOMHEIGHT, idsBuf, sizeof(idsBuf));
            WriteProfileInt(szKey, idsBuf, lpdm->iCustomHeight, lpProfile);
        }

        if (lpdm->dmSizeUnit != lpOrigDM->dmSizeUnit) {
            LoadString(ghInst, IDS_CUSTOMUNIT, idsBuf, sizeof(idsBuf));
            WriteProfileInt(szKey, idsBuf, lpdm->dmSizeUnit, lpProfile);
        }

        if (lpdm->bNegImage != lpOrigDM->bNegImage
            || lpdm->bPerPage != lpOrigDM->bPerPage
            || lpdm->bDSC != lpOrigDM->bDSC
            || lpdm->iDLFontFmt != lpOrigDM->iDLFontFmt
            || lpdm->bSubFonts != lpOrigDM->bSubFonts
            || lpdm->bMirror != lpOrigDM->bMirror
            || lpdm->bColorToBlack != lpOrigDM->bColorToBlack
            || lpdm->bCompress != lpOrigDM->bCompress
            || lpdm->bErrHandler != lpOrigDM->bErrHandler
           ) {

        LoadString(ghInst, IDS_ADVFLAGS, idsBuf, sizeof(idsBuf));

            switch(lpdm->iDLFontFmt)
            {
                case  DLFMT_TYPE3:
                    i = ADVF_TYPE3;
                    break;
                case  DLFMT_TRUETYPE:
                    i = ADVF_TRUETYPE;
                    break;
                default:
                    i = ADVF_TYPE1;
                    break;
            }


            i |= (lpdm->bNegImage ? ADVF_NEGIMAGE : 0)
                | (lpdm->bPerPage ? ADVF_PERPAGE : 0)
                | (lpdm->bDSC ? ADVF_DSC : 0)
                | (lpdm->bSubFonts ? ADVF_SUBFONTS : 0)
                | (lpdm->bMirror ? ADVF_MIRROR : 0)
                | (lpdm->bColorToBlack ? ADVF_COLORTOBLACK : 0)
                | (lpdm->bCompress ? ADVF_COMPRESS : 0)
                | (lpdm->bErrHandler ? ADVF_ERRHANDLER : 0)
        | (lpdm->bNoDownLoad ? ADVF_NODOWNLOAD : 0)
                ;

            WriteProfileInt(szKey, idsBuf, i, lpProfile);
        }

    if (lpdm->dm.dmOrientation != lpOrigDM->dm.dmOrientation) {

        DBMSG_WP((" WriteProfile(): %ls %d\n",
            (LPSTR)idsBuf, lpdm->dm.dmOrientation == DMORIENT_LANDSCAPE));

        LoadString(ghInst, IDS_ORIENTATION, idsBuf, sizeof(idsBuf));
        WriteProfileInt(szKey, idsBuf, lpdm->dm.dmOrientation, lpProfile);
    }

    if (lpdm->dm.dmDuplex != lpOrigDM->dm.dmDuplex) {
        LoadString(ghInst, IDS_DUPLEX, idsBuf, sizeof(idsBuf));
        WriteProfileInt(szKey, idsBuf, lpdm->dm.dmDuplex, lpProfile);
    }

    if (lpdm->dm.dmColor != lpOrigDM->dm.dmColor) {
        LoadString(ghInst, IDS_COLOR, idsBuf, sizeof(idsBuf));
        WriteProfileInt(szKey, idsBuf, lpdm->dm.dmColor, lpProfile);
    }


    if (lpdm->iJobTimeout != lpOrigDM->iJobTimeout) {
        LoadString(ghInst, IDS_JOBTIMEOUT, idsBuf, sizeof(idsBuf));
        WriteProfileInt(szKey, idsBuf, lpdm->iJobTimeout, lpProfile);
    }

    /* save the paper source, i.e. upper, lower or ... */
    if (lpdm->dm.dmDefaultSource != lpOrigDM->dm.dmDefaultSource) {
        DBMSG_WP((" WriteProfile(): %ls %d\n", (LPSTR)idsBuf, lpdm->iFeed));

        LoadString(ghInst, IDS_PAPERSOURCE, idsBuf, sizeof(idsBuf));
        WriteProfileInt(szKey, idsBuf, lpdm->dm.dmDefaultSource, lpProfile);
    }

    if (lpdm->fHeader != lpOrigDM->fHeader) {

        LoadString(ghInst, IDS_HEADER, sz, sizeof(sz));
        WriteProfileInt(szKey, sz, lpdm->fHeader, lpProfile);

        DBMSG_WP((" WriteProfile(): %ls %ls\n", (LPSTR)idsBuf, (LPSTR)sz));
    }

    if (lpdm->marginState != lpOrigDM->marginState) {

        LoadString(ghInst, IDS_MARGINS, sz, sizeof(sz));
        WriteProfileInt(szKey, sz, lpdm->marginState, lpProfile);

        DBMSG_WP((" WriteProfile(): marginState %d\n", lpdm->marginState));
    }

    /* For each feed available to the printer save the paper
     * associated with it.
     */
    LoadString(ghInst, IDS_PAPERX, buf, sizeof(buf));

    for (iFeed = DMBIN_FIRST; iFeed <= DMBIN_LAST; iFeed++) {

        if (pPrinter->feed[iFeed - DMBIN_FIRST]) {
            wsprintf(idsBuf, buf, iFeed);
    
            DBMSG_WP((" WriteProfile(): [%d]%d %ls\n",
                iFeed, lpdm->rgiPaper[iFeed], (LPSTR)idsBuf));

            DBMSG_WP((" WriteProfile(): %ls %ls\n",
                (LPSTR)idsBuf, (LPSTR)sz));

            WriteProfileInt(szKey, idsBuf, lpdm->rgiPaper[iFeed], lpProfile);

        } else if (pOrigPrinter->feed[iFeed - DMBIN_FIRST] && !pPrinter->feed[iFeed - DMBIN_FIRST]) {

            /* null it out in case it isn't used by the new printer */

            WriteString(szKey, idsBuf, szNull, lpProfile);
        }
    }

    if (lpdm->dm.dmCopies != lpOrigDM->dm.dmCopies)
        WriteProfileInt(szKey, "Copies", lpdm->dm.dmCopies, lpProfile);

    if (lpdm->fDoEps != lpOrigDM->fDoEps)
	WriteProfileInt(szKey, "EPS", (lpdm->fDoEps? 1: 0), lpProfile);

    WriteProfileString(szKey, "EpsFile", (LPSTR)lpdm->EpsFile);

    if (lpdm->dm.dmScale != lpOrigDM->dm.dmScale)
	WriteProfileInt(szKey, "scale", lpdm->dm.dmScale, lpProfile);

    FreePrinter(pOrigPrinter);
    FreePrinter(pPrinter);

    DBMSG_WP(("<WriteProfile()\n"));
    return;
}


/********************************************************************/

short    FAR PASCAL GetDefaultPaper()
{
    if (isUSA())
        return(DMPAPER_LETTER);    /* US LETTER */
    else
        return(DMPAPER_A4);    /* DIN A4 */
}



/***********************************************************************
* Name: GetPaperType()
*
* Action: Get the paper type from win.ini for feeder iFeed
*
* return the default
*
************************************************************************/

int PASCAL GetPaperType(szKey, iFeed, lpProfile)
LPSTR    szKey;
int    iFeed;      /* The tray from which the paper is being fed */
LPSTR    lpProfile;
{
    char    idsBuf[10];
    char    feed[10];
    int    val;

    LoadString (ghInst, IDS_PAPERX, feed, sizeof(feed));

    wsprintf(idsBuf, feed, iFeed);

    val = GetInt(szKey, idsBuf, GetDefaultPaper(), lpProfile);

    return val;
}



/*
 * int FAR PASCAL GetExternPrinter(int iExtPrinterNum)
 *
 * returns file handel for external printer (-1 if it doesn't exist)
 * 
 * uses the section in win.in that looks like this:
 *
 *    [PSCRIPT]
 *    External Printers=1
 *    printer1=FILENAME
 *
 * in:
 *    i    external printer index (1 - N)
 *
 * return:
 *    fh    of external printer (.WPD) file
 */

int FAR PASCAL GetExternPrinter(int i)
{
    OFSTRUCT os;
    char    temp[66];

    DBMSG(("GetExternPrinter() %d\n", i));

        GetExternPrinterFilename(i, temp);
     return OpenFile(temp, &os, OF_READ);
}


void FAR PASCAL GetExternPrinterFilename(int i, LPSTR lpFilename)
{
    OFSTRUCT os;
    short   fh;
    char    temp[40];
    char    idsBuf[12];
    char    szPrinter[20];
    LPSTR   lpszFilename;

    DBMSG(("GetExternPrinterFilename() %d\n", i));

    LoadString (ghInst, IDS_PRINTER, idsBuf, sizeof(idsBuf));
    wsprintf(temp, idsBuf, i);

    GetProfileString(szModule, temp, szNull, szPrinter, sizeof(szPrinter));

    DBMSG(("GetExternPrinter %d %ls\n", i, (LPSTR)szPrinter));

    GetSystemDirectory(lpFilename, 160);
    lpszFilename = lpFilename + lstrlen(lpFilename);
    lpszFilename = AnsiPrev(lpFilename, lpszFilename);
    if (*lpszFilename != '\\') 
    {
        lpszFilename = AnsiNext(lpszFilename);
        *lpszFilename = '\\';
    }
    lpszFilename = AnsiNext(lpszFilename);
    *lpszFilename = '\0';
    lstrcat(lpFilename, szPrinter);
     lstrcat(lpFilename, szRes);

     if((fh = OpenFile(lpFilename, &os, OF_READ)) > -1)
    {
        _lclose(fh);
        return;
    }
    //  if there is no WPD file in the system directory
    //  try looking in the windows directory.

    GetWindowsDirectory(lpFilename, 160);
    lpszFilename = lpFilename + lstrlen(lpFilename);
    lpszFilename = AnsiPrev(lpFilename, lpszFilename);
    if (*lpszFilename != '\\') 
    {
        lpszFilename = AnsiNext(lpszFilename);
        *lpszFilename = '\\';
    }
    lpszFilename = AnsiNext(lpszFilename);
    *lpszFilename = '\0';
    lstrcat(lpFilename, szPrinter);
     lstrcat(lpFilename, szRes);
}

int FAR PASCAL SubTabProc(LPLOGFONT lplf, LPTEXTMETRIC lptm, short nType,
                           LPSTR dwData)
{
    // this procedue is called so there must be a device font.
    return 0;
}

int FAR PASCAL SubTabTTProc(LPLOGFONT lplf, LPTEXTMETRIC lptm, short nType,
               LPSTR lpstr)
{
    LPSUBTAB lpTable;
    short   i, nSub;

    lpTable = (LPSUBTAB) lpstr;
    nSub = lpTable->nSub;
    for (i = 0; i < nSub; i++)
    {
        // we are happy, the tt font is already in the table
        if (!lstrcmpi(lplf->lfFaceName, lpTable->SubEnt[i].rgTTFont))
            return 1;
    }

    if (nSub >= lpTable->MaxSub)
        return 0;
    lstrcpy(lpTable->SubEnt[nSub].rgTTFont, lplf->lfFaceName);
    lpTable->SubEnt[nSub].rgDevFont[0] = '\0';
    lpTable->nSub++;
    return 1;
}

LPSUBTAB FAR PASCAL LockSubTable(lpdv, bUser)
LPDV    lpdv;
int bUser;
{
    LPSUBTAB lpTable;
    LPSUBENTRY lpCurEntry;
    char szBuf[66];
    LPSTR lpsz;
    int i;

    if (lpdv->DevMode.hSubTable)
        lpTable = (LPSUBTAB)GlobalLock(lpdv->DevMode.hSubTable);
    else
        lpTable = NULL;

    if (lpTable && lpTable->bUser == bUser)
    goto done;

    if (!lpTable || bUser == 0)
    // build default lpTable
    {
        if (!lpTable)
        {
            lpdv->DevMode.hSubTable = GlobalAlloc(GDLLHND, sizeof(SUBTAB) + IDS_NUMDEFSUBS * 2 * sizeof(SUBENTRY));
            if (lpTable = (LPSUBTAB) GlobalLock(lpdv->DevMode.hSubTable))
            {
                lpTable->MaxSub = IDS_NUMDEFSUBS * 2;
                // LHND make it zero-init
                // lpTable->bUser = 0;
                // lpTable->nSub = 0;
            }
        }

        lpCurEntry = lpTable->SubEnt;

        // Initialize with Default table
        for (i = 0; ; i++, lpCurEntry++)
        {
            if (!LoadString(ghInst, IDS_DEFSUBBASE + i, szBuf, sizeof(szBuf)))
                break;

            /* find the comma & change it to NULL */
            for (lpsz = szBuf; *lpsz != ','; lpsz = AnsiNext(lpsz))
                ;
            *lpsz++ = '\0';

            /* our two strings are now pointed to by szBuf & lpsz, resp. */
            lstrcpy(lpCurEntry->rgTTFont,szBuf);
            lstrcpy(lpCurEntry->rgDevFont,lpsz);

            // make sure that this device font is available on the device
            //  EnumFonts returns 1 to indicate failure.

            if (!EnumFonts(lpdv, lpsz, SubTabProc, (LPSTR)lpCurEntry,
                ENUM_INTERNAL | ENUM_SOFTFONTS))
                continue;

            // need to try the TrueType font name itself.
            if (!EnumFonts(lpdv, (LPSTR)szBuf, SubTabProc, (LPSTR)lpCurEntry,
                ENUM_INTERNAL | ENUM_SOFTFONTS))
            {
                lstrcpy(lpCurEntry->rgDevFont, szBuf);
                continue;
            }

            // oops nothing is available, no substitution
        }

        lpTable->nSub = i;

        // add TT font which are not in the default substitution table
        while (!EnumFonts(lpdv, 0, SubTabTTProc, (LPSTR)lpTable, ENUM_TRUETYPE))
        {
            HANDLE hNew;
            int     xSub;

            xSub = lpTable->MaxSub + IDS_NUMDEFSUBS;

            GlobalUnlock(lpdv->DevMode.hSubTable);

            hNew = GlobalReAlloc(lpdv->DevMode.hSubTable, sizeof(SUBTAB) + xSub * sizeof(SUBENTRY),
                                LMEM_MOVEABLE);
            if (hNew)
            {
                lpdv->DevMode.hSubTable = hNew;
                lpTable = (LPSUBTAB) GlobalLock(hNew);
                lpTable->MaxSub = xSub;
            }
            else
                // cut off additional tt fonts due to low memory
                break;
        }
    }

    // Apply user entries from win.ini
    if (bUser)
    {
        lpCurEntry = lpTable->SubEnt;
        for (i = 0; i < lpTable->nSub; i++, lpCurEntry++)
        {
            // subtable entry >= IDS_NUMDEFSUBS, no entry in win.ini means
            //    to download
            // subtable entry < IDS_NUMDEFSUBS, no entry in win.ini means
            //    to use default table
            if (GetProfileString(szModule, lpCurEntry->rgTTFont, "", szBuf, sizeof(szBuf)))
            {
                // This device font is available
                if (!EnumFonts(lpdv, szBuf, SubTabProc, (LPSTR)lpCurEntry,
                        ENUM_INTERNAL | ENUM_SOFTFONTS))
                    lstrcpy(lpCurEntry->rgDevFont, szBuf);
                else            // key word for downloading
                    if (szBuf[0] == '0')
                        lpCurEntry->rgDevFont[0] = '\0';
                lpTable->bUser = bUser;
            }
            // found no entry in win.ini, no need to do anything.
            // use the default table or download if not found in default table.
        }
    }

done:
    return lpTable;
}


BOOL   FAR PASCAL UpdateSubTable(lpdv, hwndTT, hwndDV)
LPDV  lpdv;
HWND  hwndTT, hwndDV;
{
    int  i, index, iDevFont;
    LPSUBTAB   lpTable;
    LPSUBENTRY lpCurEntry;

    // start with default table
    lpTable = LockSubTable(lpdv, 0);

    lpCurEntry = lpTable->SubEnt;

    // Update with mapping from ListBox
    for (i = 0; i < lpTable->nSub; ++i, lpCurEntry++)
    {
    index = (int)SendMessage(hwndTT, LB_FINDSTRING, -1, (DWORD)((LPSTR)lpCurEntry->rgTTFont));

    if (index < 0)
        continue;

    iDevFont = (int)SendMessage(hwndTT, LB_GETITEMDATA, index, 0L);

    // user wants to download
        if (!iDevFont)
        lpCurEntry->rgDevFont[0] = '\0';
    else
        SendMessage(hwndDV, LB_GETTEXT, iDevFont, (DWORD)((LPSTR)lpCurEntry->rgDevFont));
    }

    lpTable->bUser = 1;

    UnlockSubTable(lpdv);
    return(TRUE);
}


void FAR PASCAL UnlockSubTable(lpdv)
LPDV    lpdv;
{
    if (lpdv->DevMode.hSubTable)
    GlobalUnlock(lpdv->DevMode.hSubTable);
}


void FAR PASCAL FreeSubTable(lpdv)
LPDV    lpdv;
{
    if (lpdv->DevMode.hSubTable)
    {
    GlobalFree(lpdv->DevMode.hSubTable);
    lpdv->DevMode.hSubTable = NULL;
    }
}

void FAR PASCAL WriteSubTable(lpdv)
LPDV    lpdv;
{
    LPSUBTAB lpTable = NULL;
    LPSUBENTRY lpCurEntry;
    int i;

    lpTable = (LPSUBTAB) GlobalLock(lpdv->DevMode.hSubTable);

    if (!lpTable)
    return;

    lpCurEntry = lpTable->SubEnt;

    for (i = 0; i < lpTable->nSub; ++i, lpCurEntry++)
    {
            /* write it */
        if (lpCurEntry->rgDevFont[0])
            WriteProfileString(szModule, lpCurEntry->rgTTFont, lpCurEntry->rgDevFont);
        // over-write default substitution table with download
        else  //  this TT font is mapped to "download as softfont"
        {
            if (i < IDS_NUMDEFSUBS)
                WriteProfileString(szModule, lpCurEntry->rgTTFont, "0");
            else
                WriteProfileString(szModule, lpCurEntry->rgTTFont, NULL);
        }
    }

    GlobalUnlock(lpdv->DevMode.hSubTable);
}


BOOL FAR PASCAL FindSubFont(lpdv, bUser, lpszTTFont, lpszDevFont)
LPDV lpdv;
BOOL    bUser;
LPSTR lpszTTFont;
LPSTR lpszDevFont;
{
    LPSUBTAB     lpTable;
    LPSUBENTRY     lpCurEnt;
    int     i, nSub;
    BOOL    bSub = FALSE;

    lpTable = (LPSUBTAB) LockSubTable(lpdv, bUser);

    if (!lpTable)
        return NULL;

    nSub = lpTable->nSub;

    lpCurEnt = lpTable->SubEnt;

    for (i = 0; i < nSub; ++i, lpCurEnt++)
    if (!lstrcmpi(lpszTTFont, lpCurEnt->rgTTFont))
        {
        if (lpCurEnt->rgDevFont)
        {
        lstrcpy(lpszDevFont, lpCurEnt->rgDevFont);
        bSub = TRUE;
        }
        break;
        }

    UnlockSubTable(lpdv);

    return bSub;
}

#if 0
int FAR PASCAL fnSubFilterProc(LPLOGFONT lplf, LPTEXTMETRIC lptm, short nType,
                               DWORD dwData)
{
   struct subfilter FAR *lpSub = (struct subfilter FAR *)dwData;

   /* if the TT font is resident in printer don't substitute */
   if ((nType & TYPE_TRUETYPE) && !lstrcmpi(lpSub->lpszTTFont, lplf->lfFaceName)) {
      return 0;
   }

   /* if the Device font is resident in printer substitute */
   if (!lstrcmpi(lpSub->lpszDevFont, lplf->lfFaceName)) {
      lpSub->bDoSubstitution = TRUE;
      return 0;
   }
   return 1;
}
#endif


LPSTR   NEAR  PASCAL  SetOldKey(LPSTR  key, LPSTR  lszPort);
void   NEAR  PASCAL  StripColon(LPSTR  lszPort);
BOOL  NEAR  PASCAL  DupProfileSection(LPSTR  lpOldSec, 
                            LPSTR  lpNewSec);
LPSTR  NEAR  PASCAL   Transfer(LPSTR  buffer1, LPSTR  lszPort);



BOOL  FAR  PASCAL  DevInstall(hWnd, lszModelName, lszOldPort, lszNewPort)
HWND   hWnd;
LPSTR  lszModelName, lszOldPort, lszNewPort;
{
    BYTE  buffer1[80], buffer2[80];

    lszOldPort = Transfer(buffer1, lszOldPort);
    lszNewPort = Transfer(buffer2, lszNewPort);

    StripColon(lszOldPort);
    StripColon(lszNewPort);

    if(lszNewPort)   // ATM specific things
    {
        BYTE  atmPath[156];
        WORD  len;
        OFSTRUCT  oaf;


        // fix for bug num 13051

        GetWindowsDirectory(atmPath, 144);
        len = lstrlen(atmPath);
        if(atmPath[len - 1] != '\\')
            lstrcat(atmPath, "\\");
        lstrcat(atmPath, "atm.ini");
        if(OpenFile(atmPath, (LPOFSTRUCT)(&oaf), OF_EXIST) > -1)
        {
            WritePrivateProfileString("Aliases", "Times Roman", "Times",
                atmPath);
            WritePrivateProfileString("Synonyms", "Times Roman", "Times",
                atmPath);
        }

    }

    if(lszOldPort && lszNewPort)
    {   // rename new style printer in win.ini
        BYTE  NewKey[40], OldKey[40];

        SetOldKey(OldKey, lszNewPort);
        WriteProfileString(OldKey, (LPSTR)"ATM", (LPSTR)"placeholder");

        SetKey(OldKey, lszModelName, lszOldPort);
        SetKey(NewKey, lszModelName, lszNewPort);

        DupProfileSection(OldKey, NewKey);

        return(WriteProfileString(OldKey, NULL, NULL));
        // delete  entire section in Win.ini 
    }
    else if(lszOldPort && !lszNewPort)
    {   //  remove new style printer from win.ini
        BYTE  OldKey[40];

        SetKey(OldKey, lszModelName, lszOldPort);

        return(WriteProfileString(OldKey, NULL, NULL));
        // delete  entire section in Win.ini 
    }
    else if(!lszOldPort && lszNewPort)
    {   //  install new printer
        //  if first time install, (Distinguished by absence of
        //  ATM=placeholder , consider this to be an upgrade
        //  else its a plain install.

        BYTE  NewKey[40], OldKey[40];
        WORD   iPaper, iFeed;

        SetOldKey(OldKey, lszNewPort);

        //  call GetProfileString just to see if section exists
        if(!GetProfileString(OldKey, NULL, "", NewKey, 40))
        {
            WriteProfileString(OldKey, (LPSTR)"ATM", (LPSTR)"placeholder");
            //  create this section - which is needed to install ATM
            return(FALSE);    // Oldkey section  is missing
        }

        GetProfileString(OldKey, "ATM", "Upgrade", NewKey, 40);
        //  NewKey contains either Upgrade or placeholder

        if(lstrcmp("Upgrade", NewKey))
            return(FALSE);

#if  0

    /* Why bother with seeing what printer model name Win 3.1 has
       associated with this win 3.0 iPrinter number?  they may be totally
       different for all we know.  Its irrelavent!
       Setup figured this out for us and gave us the proper name to use.
       So don't render his efforts in vain, Stop Questioning Authority!
    */


        BYTE  DevName[32];
        PPRINTER  pPrinter;
        WORD  iPrinter;

        iPrinter = GetProfileInt(OldKey, "device", DEFAULT_PRINTER);

        //  what is the printer Model associated with this iPrinter value?
        pPrinter = GetPrinter(iPrinter);
        if(pPrinter == NULL)
            return(FALSE);  // can't identify this iPrinter number
        lstrcpy((LPSTR)DevName, (LPSTR)pPrinter->Name);
        FreePrinter(pPrinter);
        
        if(lstrcmp(DevName, lszModelName))
            return(FALSE); //  This entry in win.ini is not for 
                           //  the printer I want to install.

#endif

        //  now we get down to business of modifying win.ini

        SetKey(NewKey, lszModelName, lszNewPort);

        WriteProfileString(OldKey, "device", NULL);
        // delete  entry  device=...

        DupProfileSection(OldKey, NewKey);

        //-------convert 3.0 binnumbers to 3.1--------
        //               bug 10047
        //  first delete obsolete paper feeds

        WriteProfileString(NewKey, "feed2", NULL);
        WriteProfileString(NewKey, "feed3", NULL);
        WriteProfileString(NewKey, "feed4", NULL);
        WriteProfileString(NewKey, "feed12", NULL);
        WriteProfileString(NewKey, "feed13", NULL);
        WriteProfileString(NewKey, "feed15", NULL);

        //  now create the new paper feeds 

        if(iPaper = GetProfileInt(OldKey, "feed2", 0) )
            WriteProfileInt(NewKey, "feed1", iPaper, NULL);
        if(iPaper = GetProfileInt(OldKey, "feed3", 0) )
            WriteProfileInt(NewKey, "feed2", iPaper, NULL);
        if(iPaper = GetProfileInt(OldKey, "feed4", 0) )
            WriteProfileInt(NewKey, "feed3", iPaper, NULL);
        if(iPaper = GetProfileInt(OldKey, "feed12", 0) )
            WriteProfileInt(NewKey, "feed9", iPaper, NULL);
        if(iPaper = GetProfileInt(OldKey, "feed13", 0) )
            WriteProfileInt(NewKey, "feed10", iPaper, NULL);
        if(iPaper = GetProfileInt(OldKey, "feed15", 0) )
            WriteProfileInt(NewKey, "feed4", iPaper, NULL);

        //  may need to alter arg to 'source'

        iFeed = GetProfileInt(OldKey, "source", 0);
        switch (iFeed)
        {
            case 2:
              iFeed = 1;
              break;
            case 3:
              iFeed = 2;
              break;
            case 4:
              iFeed = 3;
              break;
            case 12:
              iFeed = 9;
              break;
            case 13:
              iFeed = 10;
              break;
            case 15:
              iFeed = 4;
              break;
        }
        WriteProfileInt(NewKey, "source", iFeed, NULL);



        // ensure that the lines
        // [PostScript,Port]
        // dummy=placeholder
        // exists in win.ini   required for Adobe ATM to install.
        // also marks this Old section as being already upgraded.

        WriteProfileString(OldKey, (LPSTR)"ATM", (LPSTR)"placeholder");
        return(TRUE);  // Finally - we upgraded!
    }
    return(FALSE);  // undefined operation.
}



//-------- SetOldKey ----------------------
//  creates old style format Section Keys
//  for postscript printers
//  expects that terminating : have been 
//  removed.
//-----------------------------------------

LPSTR   NEAR  PASCAL  SetOldKey(key, lszPort)
LPSTR  key, lszPort;
{
    lstrcpy(key, "PostScript,");
    lstrcat(key, lszPort);
    return(key);
}

void   NEAR  PASCAL  StripColon(lszPort)
LPSTR  lszPort;
{
    //  strip terminating :  from port names (if not NULL).
    if(lszPort)
    {
        WORD   len;
        len = lstrlen(lszPort);
        if(len  &&  lszPort[len-1] == ':')
            lszPort[len-1] = '\0';
    }
}


#define KEY_BUF_SIZE_INC    256
#define MAX_STRING_LENGTH   128


BOOL  NEAR  PASCAL  DupProfileSection(lpOldSec, lpNewSec)
LPSTR   lpOldSec, lpNewSec;
{
    HANDLE  hMem;
    LPSTR   lpKeyBuf;
    int     nSize;
    char    szTmp[MAX_STRING_LENGTH];

    // this code taken from Unidrv.dll
    // read in all the key names under the old section
    nSize = KEY_BUF_SIZE_INC;
    if (!(hMem = GlobalAlloc(GHND, (long)nSize)))
        return FALSE;
    lpKeyBuf = GlobalLock(hMem);

    while (GetProfileString(lpOldSec, (LPSTR)NULL, "", lpKeyBuf, nSize)
           == nSize - 2)
    {
        // not enough buffer space for all the key names.
        GlobalUnlock(hMem);
        nSize += KEY_BUF_SIZE_INC;
        if (!(hMem = GlobalReAlloc(hMem, (long)nSize, GHND)))
            return FALSE;
        lpKeyBuf = GlobalLock(hMem);
    }
    // loop through each key and copy the value from the old to the new.
    while ((nSize = lstrlen(lpKeyBuf)) > 0)
    {
        //  filter out all keys starting with "softfont"

        lstrcpy((LPSTR)szTmp, lpKeyBuf);
        szTmp[8] = '\0';   //  Truncate after 8 chars

        if(lstrcmpi((LPSTR)"softfont", (LPSTR)szTmp))
        {
            GetProfileString(lpOldSec, lpKeyBuf, "", (LPSTR)szTmp, sizeof(szTmp));
            if(!WriteProfileString(lpNewSec, lpKeyBuf, (LPSTR)szTmp))
                return(FALSE);
        }
        lpKeyBuf += (nSize + 1);
    }
    GlobalUnlock(hMem);
    GlobalFree(hMem);
    return(TRUE);
}

LPSTR  NEAR  PASCAL   Transfer(buffer1, lszPort)
LPSTR  buffer1, lszPort;
{
    //  this function copies the string from lszPort to buffer1
    //  and returns  a pointer to buffer1.  If lszPort is null
    //  nothing happens and lszPort  is returned.

    if(!lszPort)
        return(lszPort);
    lstrcpy(buffer1, lszPort);
    return(buffer1);
}
