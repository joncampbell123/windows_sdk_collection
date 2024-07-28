/**[f******************************************************************
* sffile.c -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */

/*******************************   sffile.c   ******************************/
//
//  SFfile:  Utility for reading the sfinstal.dir file
//
//  Contains the function LoadSFdirFile().
//
// History.
//
// 02 dec 91    rk(HP)      Changed TMPBUF_STRSZ from 72 to 90 so WriteErr would
//                          not truncate SF_TKUNKDRV, SF_TKDUPDRV, and SF_TKEXPLSTR.              
// 04 sep 91    rk(HP)      Changed OpenFile (lpTstate->hErrFile) comparison in WriteErr      
// 29 jul 91    rk(HP)      Changed ch from char to BYTE so international
//              characters above 127 would not be considered as a negative
//              signed char and thus fail the >32 test.
// 07 aug 89    peterbe     Changed all lstrcmp() to lstrcmpi().
// 01 aug 89    peterbe     Adding calls to LZEXPAND.MOD (lzRead).
//
//  Fri 13-Jan-1989  10:20:26    -by-   Jim Mathews   [jimmat]
//      Reduced # of redundant strings by adding lclstr.h
//
/***************************************************************************/
  
  
//#define DEBUG
  
#include "nocrap.h"
#undef NOOPENFILE
#undef NOMEMMGR
#undef NOCTLMGR
#undef NOWINMESSAGES
#undef NOMB
#undef NOSHOWWINDOW
#undef NOSCROLL
#undef NOMSG
#undef NOHDC
#undef NOGDI
#include "windows.h"
#include "neededh.h"
#include "sffile.h"
#include "sfdir.h"
#include "sflb.h"
#define NOBLDDESCSTR
#include "sfutils.h"
#include "strings.h"
#include "sfadd.h"
#include "sfinstal.h"
#include "dlgutils.h"
#include "lclstr.h"
#include "expand.h"
  
  
/****************************************************************************\
* Debug Definitions
\****************************************************************************/
  
#ifdef DEBUG
    #define DBGx(msg)              /*DBMSG(msg)*/
    #define DBGwriteerr(msg)       /*DBMSG(msg)*/
    #define DBGgettoken(msg)       /*DBMSG(msg)*/
    #define DBGgetutil(msg)        /*DBMSG(msg)*/
    #define DBGscrn(msg)           /*DBMSG(msg)*/
    #define DBGdumppp
#else
    #define DBGx(msg)              /*null*/
    #define DBGwriteerr(msg)       /*null*/
    #define DBGgettoken(msg)       /*null*/
    #define DBGgetutil(msg)        /*null*/
    #define DBGscrn(msg)           /*null*/
#endif
  
  
#ifdef DEBUG
#undef FORCE_OUTOFMEM
#endif
  
  
  
  
  
  
#define LOCAL static
  
#define ABS(x)      (((x) >= 0) ? (x) : (-(x)))
#define isAlpha(ch) ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
  
  
#define EOT '\004'
#define TMPBUF_BUFSZ    512
//#define TMPBUF_STRSZ    72
#define TMPBUF_STRSZ    90
#define NUM_TOKENS  6
#define TK_NAMELEN  32
#define FILEREAD_BUFSZ  2048
  
  
LOCAL BYTE GetFontDef(LPSFDIRTKSTATE);
LOCAL BYTE GetCartridgeName(LPSFDIRTKSTATE);
LOCAL BYTE GetScrnFont(LPSFDIRTKSTATE);
LOCAL BYTE GetPackage(LPSFDIRTKSTATE);
LOCAL BYTE EndPackage(LPSFDIRTKSTATE);
LOCAL BYTE GetFamily(LPSFDIRTKSTATE);
LOCAL BYTE EndFamily(LPSFDIRTKSTATE);
LOCAL BYTE GetCartridge(LPSFDIRTKSTATE);
LOCAL BYTE GetLogDrive(LPSFDIRTKSTATE);
LOCAL BYTE DoNuthing(LPSFDIRTKSTATE);
  
LOCAL BYTE GetString(int, LPSTR, int, BYTE, BYTE);
LOCAL BYTE GetDriveID(int, LPSTR, int, BYTE);
LOCAL BYTE GetFileName(int, LPSTR, int, BYTE);
LOCAL BYTE GetToken(int, LPSTR, int, BYTE);
LOCAL int _readFileCH(int, LPSTR);
LOCAL void _closeFileRead(HANDLE);
LOCAL HANDLE _initFileRead(int);
  
LOCAL void WriteErr(LPSFDIRTKSTATE, int);
LOCAL BOOL isNumeric(LPSTR);
LOCAL WORD getScrnType(int, int);
LOCAL BYTE DumpLine(int, LPSTR, int, BYTE);
LOCAL BOOL PushTstate(LPSFDIRTKSTATE, LPSTR);
LOCAL BOOL PopTstate(LPSFDIRTKSTATE, BYTE);
LOCAL void initScrnData(LPSFDIRTKSTATE, LPSTR, int);
LOCAL int lpi(int, int);
LOCAL BOOL AddLogDrive(LPSFDIRTKSTATE, LPSFDIRLOGDRV, int);
LOCAL BOOL AddFile(LPSFDIRTKSTATE, LPSFDIRFILE, int FAR *, LPSTR, int);
LOCAL void ReportOutOfMem(LPSFDIRTKSTATE);
  
  
typedef BYTE (*GETPROC)();  /* ptr to a "C" Get<token> function. */
  
typedef struct {
    BYTE name[TK_NAMELEN];
    GETPROC proc;
} TOKENREC;
  
typedef struct {
    BYTE buf[TMPBUF_BUFSZ];     /* MUST be first */
    SFDIRTKSTATE tstate;
    BYTE errbuf[TMPBUF_STRSZ];      /* Note: WriteErr() depends upon */
    BYTE linenum[TMPBUF_STRSZ];     /* these strings following the   */
    BYTE aroundch[TMPBUF_STRSZ];    /* tstate struct.                */
    BYTE aroundeol[TMPBUF_STRSZ];
    TOKENREC tokens[NUM_TOKENS];
} TMPBUF;
  
typedef TMPBUF FAR *LPTMPBUF;
typedef TOKENREC FAR *LPTOKENREC;
  
  
LOCAL int gLineCount = 0;
LOCAL int gCharPos = 0;
LOCAL LPSTR gLPfileRead = 0L;
LOCAL int gSZfileRead = 0;
LOCAL int gPOSfileRead = 0;
LOCAL WORD gCountLOGdrv = 1;
  
/**************************************************************************/
/****************************   Global Procs   ****************************/
  
  
/*  LoadSFdirFile
*/
HANDLE FAR PASCAL LoadSFdirFile(hDB, hMd, idLB, hSrcFile, lpFileNm, lpErrNm,
reportErr, lpCount)
HWND hDB;
HANDLE hMd;
WORD idLB;
int hSrcFile;
LPSTR lpFileNm;
LPSTR lpErrNm;
BOOL reportErr;
WORD FAR *lpCount;
{
    MSG msg;
    LPSFDIRTKSTATE lpTstate = 0L;
    LPTOKENREC lpTokens = 0L;
    LPTOKENREC t = 0L;
    HANDLE hBuf = 0;
    HANDLE hSFlb = 0;
    LPSTR lpBuf = 0L;
    int hFileRead = 0;
    int ind = 0;
//    char ch = '\0';
    BYTE ch = '\0';
  
    *lpCount = 0;
  
    if ((hBuf = GlobalAlloc(GMEM_FIXED,(DWORD)sizeof(TMPBUF))) &&
        (lpBuf = GlobalLock(hBuf)) &&
        (hFileRead = _initFileRead(hSrcFile)) &&
        LoadString(hMd,SF_TKLINENUM,((LPTMPBUF)lpBuf)->linenum,TMPBUF_STRSZ) &&
        LoadString(hMd,SF_TKAROUNDCH,((LPTMPBUF)lpBuf)->aroundch,
        TMPBUF_STRSZ) &&
        LoadString(hMd,SF_TKAROUNDEOL,((LPTMPBUF)lpBuf)->aroundeol,
        TMPBUF_STRSZ) &&
        (lpTokens = ((LPTMPBUF)lpBuf)->tokens) &&
        LoadString(hMd, SF_TKPACKAGE, lpTokens[0].name, TK_NAMELEN) &&
        LoadString(hMd, SF_TKFAMILY, lpTokens[1].name, TK_NAMELEN) &&
        LoadString(hMd, SF_TKLDRIVE, lpTokens[2].name, TK_NAMELEN) &&
        LoadString(hMd, SF_TKPORT, lpTokens[3].name, TK_NAMELEN) &&
        LoadString(hMd, SF_TKLAND, lpTokens[4].name, TK_NAMELEN) &&
        LoadString(hMd, SF_TKCARTRIDGE, lpTokens[5].name, TK_NAMELEN))
    {
        lpTstate = &((LPTMPBUF)lpBuf)->tstate;
        lmemset((LPSTR)lpTstate, 0, sizeof(SFDIRTKSTATE));
  
        AnsiUpper(lpTokens[0].name);
        AnsiUpper(lpTokens[1].name);
        AnsiUpper(lpTokens[2].name);
        AnsiUpper(lpTokens[3].name);
        AnsiUpper(lpTokens[4].name);
        AnsiUpper(lpTokens[5].name);
  
        lpTokens[0].proc = (GETPROC)GetPackage;
        lpTokens[1].proc = (GETPROC)GetFamily;
        lpTokens[2].proc = (GETPROC)GetLogDrive;
        lpTokens[3].proc = (GETPROC)DoNuthing;
        lpTokens[4].proc = (GETPROC)DoNuthing;
        lpTokens[5].proc = (GETPROC)GetCartridge;
  
        lpTstate->state = tk_null;
        lpTstate->prevind = -1;
        lpTstate->hFile = hSrcFile;
        lpTstate->lpBuf = lpBuf;
        lpTstate->bufsz = TMPBUF_BUFSZ;
        lpTstate->hMd = hMd;
        lpTstate->hSFlb = 0;
        lpTstate->hDB = hDB;
        lpTstate->idLB = idLB;
        lpTstate->count = 0;
        lpTstate->sline[0] = '\0';
        lpTstate->numLOGdrv = 0;
        lpTstate->hErrFile = -1;
        if ((lpTstate->reportErr=reportErr) && lpErrNm)
        {
            lmemcpy(lpTstate->errfile, lpErrNm, sizeof(lpTstate->errfile));
            lpTstate->errfile[sizeof(lpTstate->errfile)-1] = '\0';
        }
        else
            lpTstate->errfile[0] = '\0';
  
        initScrnData(lpTstate,lpBuf,TMPBUF_BUFSZ);
  
        SetDlgItemText(hDB, SF_STATUS, (LPSTR)NullStr);
        EnableWindow(GetDlgItem(hDB, SF_MOVE), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_COPY), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_ERASE), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_EDIT), FALSE);
        CheckRadioButton(hDB, SF_PERM_LEFT, SF_TEMP_LEFT, 0);
        EnableWindow(GetDlgItem(hDB, SF_PERM_LEFT), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_TEMP_LEFT), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_ADD_RIGHT), FALSE);
  
        /*  Change exit button to Cancel.
        */
        if (LoadString(hMd, SF_CNCLSTR, lpBuf, TMPBUF_BUFSZ))
        {
            SetDlgItemText(hDB, SF_EXIT, lpBuf);
            gSF_FLAGS |= SF_NOABORT;
        }
  
        PushTstate(lpTstate, lpFileNm);
  
        gLineCount = 1;
        gCharPos = 0;
  
    #ifdef FORCE_OUTOFMEM
        ReportOutOfMem(lpTstate);
    #endif
  
        /*  Parse the file.
        */
        while ((gSF_FLAGS & SF_NOABORT) && lpTstate->state != tk_fatalerr &&
            (ch=GetToken(hSrcFile,lpBuf,TMPBUF_BUFSZ,ch)) != EOT)
        {
            lpTstate->ch = ch;
  
            switch (ch)
            {
                case '/':
                    /*  Assume a comment.
                    */
                    break;
  
                case '"':
                    /*  We got a string, assume it begins a
                    *  font definition.
                    */
                    if (*lpBuf)
                        if (lpTstate->state==tk_cartridge)
                            ch = GetCartridgeName(lpTstate);
                        else
                            ch = GetFontDef(lpTstate);
                        else
                        {
                            if (reportErr)
                                WriteErr(lpTstate, SF_TKBADCLOSEQ);
                            ch = DumpLine(hSrcFile,lpBuf,TMPBUF_BUFSZ,ch);
                        }
                    break;
  
                case ':':
                    /*  We got a colon, assume it constitutes a
                    *  screen font definition.
                    */
                    if (isNumeric(lpBuf))
                        ch = GetScrnFont(lpTstate);
                    else
                    {
                        if (reportErr)
                            WriteErr(lpTstate, SF_TKINVALTK);
                        ch = DumpLine(hSrcFile,lpBuf,TMPBUF_BUFSZ,ch);
                    }
                    break;
  
                case '}':
                    /*  If we've encountered a closing brace, then call
                    *  associated processing procedure.  If the brace
                    *  is unexpected, then report an err.
                    */
                switch (lpTstate->state)
                {
                    case tk_package:
                        EndPackage(lpTstate);
                        break;
  
                    case tk_cartridge:
                    case tk_family:
                        EndFamily(lpTstate);
                        break;
  
                    default:
                        if (reportErr)
                            WriteErr(lpTstate, SF_TKBADBRACE);
                        break;
                }
                    break;
  
                case ' ':
                case '\t':
                case '\r':
                case '\n':
                    if (!(*lpBuf))
                        break;
  
                    AnsiUpper(lpBuf);
  
                    /*  Look for a valid token name.  If found, then
                    *  call its associated processing proc.
                    */
                    for (ind=0, t=lpTokens; ind < NUM_TOKENS; ++ind, ++t)
                    {
                        if (lstrcmpi(lpBuf, t->name) == 0)
                        {
                            ch = (*(t->proc))(lpTstate);
                            break;
                        }
                    }
  
                    /*  If not a valid token name, then report
                    *  an err.
                    */
                    if (ind == NUM_TOKENS)
                default:
                {
                    if (reportErr)
                        WriteErr(lpTstate, SF_TKINVALTK);
                    ch = DumpLine(hSrcFile,lpBuf,TMPBUF_BUFSZ,ch);
                }
            }
  
            /*  Process any messages to the installer's dialog box
            *  so we can detect the cancel button.
            */
            while (PeekMessage(&msg, hDB, NULL, NULL, TRUE) &&
                IsDialogMessage(hDB, &msg))
                ;
        }
  
        PopTstate(lpTstate, ch);
  
        /*  Restore exit button.
        */
        if (LoadString(hMd, SF_EXITSTR, lpBuf, TMPBUF_BUFSZ))
        {
            SetDlgItemText(hDB, SF_EXIT, lpBuf);
            gSF_FLAGS &= ~(SF_NOABORT);
        }
  
        SetDlgItemText(hDB, SF_STATUS, (LPSTR)NullStr);
        EnableWindow(GetDlgItem(hDB, SF_ADD_RIGHT), TRUE);
    }
  
    if (lpTstate)
    {
        if (lpTstate->state == tk_fatalerr)
        {
            if (lpTstate->hSFlb)
            {
                GlobalFree(lpTstate->hSFlb);
                lpTstate->hSFlb = 0;
                SendDlgItemMessage(hDB, idLB, LB_RESETCONTENT, 0, 0L);
            }
        }
  
        hSFlb = lpTstate->hSFlb;
        *lpCount = lpTstate->count;
  
    #ifdef DEBUG
        if (lpTstate->hSFlb)
        {
            DBGdumpSFbuf(0L);
        }
    #endif
  
        while (lpTstate->prevind > -1)
        {
            if (lpTstate->reportErr && hSFlb)
                WriteErr(lpTstate, SF_TKNOCLOSEBRC);
            PopTstate(lpTstate, ch);
        }
  
        /*  If an err file was opened, close it and report that
        *  errs were found.
        */
        if (lpTstate->hErrFile > 0)
        {
            _lclose(lpTstate->hErrFile);
            lpTstate->hErrFile = -1;
  
            LoadString(hMd, SF_TKERRSFOUND, lpBuf, TMPBUF_BUFSZ);
            if (lstrlen(lpErrNm) < TMPBUF_BUFSZ - lstrlen(lpBuf))
                lstrcat(lpBuf, lpErrNm);
            MessageBox(hDB, lpBuf, lpFileNm, MB_OK);
        }
  
        lpTstate = 0L;
    }
  
    if (!hSFlb &&
        LoadString(hMd, SF_NOFNTFOUND, lpBuf, TMPBUF_BUFSZ))
    {
        SetDlgItemText(hDB, SF_STATUS, lpBuf);
    }
  
    if (hFileRead)
    {
        _closeFileRead(hFileRead);
        hFileRead = 0;
    }
  
    if (lpBuf)
    {
        GlobalUnlock(hBuf);
        lpBuf = 0L;
    }
  
    if (hBuf)
    {
        GlobalFree(hBuf);
        hBuf = 0;
    }
  
    return (hSFlb);
}
  
/***************************************************************************/
/**********************   SFINSTAL Token Utilities   ***********************/
  
  
/*  GetFontDef
*/
LOCAL BYTE GetFontDef(lpTstate)
LPSFDIRTKSTATE lpTstate;
{
    LPSFDIRFILE lpSFfile = (LPSFDIRFILE)lpTstate->lpBuf;
    LPSTR lpBuf = (LPSTR)lpSFfile->s;
    LPSTR j, k;
    int bufsz = lpTstate->bufsz - sizeof(SFDIRFILE) + 1;
    int ind;
    BYTE ch = lpTstate->ch;
  
    DBGgettoken(("GetFontDef(%lp)\n", lpTstate));
  
    if (lpTstate->state == tk_fatalerr)
        return(ch);
  
    if (lpTstate->state != tk_family)
    {
        if (lpTstate->reportErr);
        WriteErr(lpTstate, SF_TKINVALSTR);
        goto backout;
    }
  
    /*  Pick up the length of the string and abort if its too big.
    */
    if ((ind = lstrlen((LPSTR)lpSFfile)) > bufsz)
    {
        if (lpTstate->reportErr);
        WriteErr(lpTstate, SF_TKSTRTOOBIG);
        goto backout;
    }
  
    /*  Shift the string to the "s" field at the end of the
    *  SFDIRFILE structure.
    */
    for (j=(LPSTR)lpSFfile+ind, k=lpBuf+ind; k >= (LPSTR)lpSFfile->s;
        --k, --j)
    {
        *k = *j;
    }
  
    /*  Look for the equal sign (=).
    */
    while (ch != '=' && ch >= ' ' && lpBuf[ind] == '\0')
    {
        ch = GetToken(lpTstate->hFile, &lpBuf[ind], bufsz-ind, ch);
    }
  
    /*  If we did not find the equal sign (=) or we found something
    *  else, abort.
    */
    if (ch != '=' || lpBuf[ind] != '\0')
    {
        if (lpTstate->reportErr)
            WriteErr(lpTstate, SF_TKEXPEQUAL);
        goto backout;
    }
  
    /*  Preserve the NULL character.
    */
    ++ind;
  
    /*  Pick up a "P", "L", or "PL" to indicate orientation.
    */
    ch = GetString(lpTstate->hFile, &lpBuf[ind], bufsz-ind, ch, ',');
  
    /*  Translate the letter to the numeric value for orientation.
    */
    if (lpBuf[ind] == 'P' || lpBuf[ind] == 'p')
    {
        lpSFfile->orient = 1;
    }
    else if (lpBuf[ind] == 'L' || lpBuf[ind] == 'l')
    {
        lpSFfile->orient = 2;
    }
    else
    {
        if (lpTstate->reportErr)
            WriteErr(lpTstate, SF_TKINVALORIENT);
        goto backout;
    }
  
    /*  If there is a second character, then it must indicate that
    *  orientation may be either portrait or landscape.  Otherwise,
    *  it is an err.
    */
    if (lpBuf[ind+1] != '\0')
    {
        if ((lpBuf[ind+1] == 'P' || lpBuf[ind+1] == 'p') &&
            (lpBuf[ind] == 'L' || lpBuf[ind] == 'l'))
        {
            lpSFfile->orient = 0;
        }
        else if ((lpBuf[ind+1] == 'L' || lpBuf[ind+1] == 'l') &&
            (lpBuf[ind] == 'P' || lpBuf[ind] == 'p'))
        {
            lpSFfile->orient = 0;
        }
  
        if (lpSFfile->orient != 0 || lpBuf[ind+2] != '\0')
        {
            if (lpTstate->reportErr)
                WriteErr(lpTstate, SF_TKINVALORIENT);
            goto backout;
        }
    }
  
    /*  Abort if we did not stop at a comma in our last parse.
    */
    if (ch != ',')
    {
        if (lpTstate->reportErr)
            WriteErr(lpTstate, SF_TKNODLFILE);
        goto backout;
    }
  
    /*  Pre-initialize indices to SF dir items.
    */
    lpSFfile->indLOGdrv = -1;
    lpSFfile->indScrnFnt = -1;
    lpSFfile->indDLpath = -1;
    lpSFfile->indPFMpath = -1;
    lpSFfile->fIsPCM = 0;
  
    /*  Pick up the download file name.
    */
    ch = GetFileName(lpTstate->hFile, &lpBuf[ind], bufsz-ind, ch);
  
    if (lpBuf[ind] != '\0')
    {
        /*  Add the file path and logical label to the SF directory.
        */
        if (!AddFile(lpTstate,lpSFfile,&lpSFfile->indDLpath,&lpBuf[ind],
            bufsz-ind))
            goto backout;
  
        /*  If we don't have anything left, abort.
        */
        if (lpBuf[ind] == '\0')
        {
            if (lpTstate->reportErr)
                WriteErr(lpTstate, SF_TKNODLFILE);
            goto backout;
        }
  
        /*  Store the offset to the download name.
        */
        lpSFfile->offsDLname = ind;
  
        /*  Step past the download file name, preserve the NULL.
        */
        ind += lstrlen(&lpBuf[ind]);
    }
    else if (ch == ',')
    {
        /*  We did not read a download file but we did read
        *  two commas in a row, which means we should skip
        *  the download file.
        */
        lpSFfile->indDLpath = -1;
        lpSFfile->offsDLname = 0;
        --ind;
    }
    else
    {
        /*  Nothing valid.
        */
        if (lpTstate->reportErr)
            WriteErr(lpTstate, SF_TKNODLFILE);
        goto backout;
    }
  
    /*  Look for a comma (,).
    */
    while (ch != ',' && ch >= ' ' && lpBuf[ind] == '\0')
    {
        ch = GetToken(lpTstate->hFile, &lpBuf[ind], bufsz-ind, ch);
    }
  
    /*  If we read some text while searching for the comma (,)
    *  then abort.
    */
    if (lpBuf[ind] != '\0')
    {
        if (lpTstate->reportErr)
            WriteErr(lpTstate, SF_TKEXPLCMA);
        goto backout;
    }
  
    /*  If we encounterd a comma (,) then read the PFM file name.
    */
    if (ch == ',')
    {
        /*  Preserve the NULL character.
        */
        ++ind;
  
        ch = GetFileName(lpTstate->hFile, &lpBuf[ind], bufsz-ind, ch);
  
        /*  No file name, abort.
        */
        if (lpBuf[ind] == '\0')
        {
            if (lpTstate->reportErr)
                WriteErr(lpTstate, SF_TKNOPFMFILE);
            goto backout;
        }
  
        /*  Add the file path and logical label to the SF directory.
        */
        if (!AddFile(lpTstate,lpSFfile,&lpSFfile->indPFMpath,&lpBuf[ind],
            bufsz-ind))
            goto backout;
  
        /*  If we don't have anything left, abort.
        */
        if (lpBuf[ind] == '\0')
        {
            if (lpTstate->reportErr)
                WriteErr(lpTstate, SF_TKNOPFMFILE);
            goto backout;
        }
  
        /*  Store the offset to the PFM file name.
        */
        lpSFfile->offsPFMname = ind;
  
        ind += lstrlen(&lpBuf[ind]);
    }
    else if (lpSFfile->indDLpath > -1 && lpSFfile->offsDLname > 0)
    {
        /*  No PFM file, but there is a valid download file,
        *  so we'll go ahead and use the entry.
        */
        lpSFfile->indPFMpath = -1;
        lpSFfile->offsPFMname = 0;
    }
    else
    {
        /*  Nothing valid.
        */
        if (lpTstate->reportErr)
            WriteErr(lpTstate, SF_TKNODLFILE);
        goto backout;
    }
  
    /*  Calc total size of the SF_FILE struct (including
    *  terminating NULL).
    */
    ind += sizeof(SFDIRFILE);
  
    /*  Insert the screen font.
    */
    if (lpTstate->indScrnFnt > -1)
    {
        lpSFfile->indScrnFnt = lpTstate->indScrnFnt;
        addSFdirOwner(0L,lpSFfile->indScrnFnt);
    }
  
    /*  Add file to the SF directory.
    */
    if ((ind = addSFdirEntry(0L, (LPSTR)lpSFfile, SF_FILE, ind)) > -1)
    {
        /*  Build a listbox list.
        */
        lpTstate->hSFlb = addSFlistbox(lpTstate->hDB, lpTstate->hSFlb,
        lpTstate->idLB, -1, ind, 0, lpBuf, bufsz, 0L);
  
        ++lpTstate->count;
    }
    else
    {
        /*  If we failed to add a SF dir file entry, then report
        *  an "out of memory" fatal err.
        */
        ReportOutOfMem(lpTstate);
        lpTstate->state = tk_fatalerr;
        goto backout;
    }
  
    backout:
    ch = DumpLine(lpTstate->hFile, lpTstate->lpBuf, lpTstate->bufsz, ch);
  
    return (lpTstate->ch = ch);
}
  
/*  GetScrnFont
*/
LOCAL BYTE GetScrnFont(lpTstate)
LPSFDIRTKSTATE lpTstate;
{
    LPSFDIRSCRFNT lpSFscrn = 0L;
    LPSTR lpBuf = lpTstate->lpBuf;
    int bufsz = lpTstate->bufsz;
    BYTE ch = lpTstate->ch;
    int width;
    int height;
    int bestwidth = 0;
    int bestheight = 0;
    int ind;
  
    DBGgettoken(("GetScrnFont(%lp)\n", lpTstate));
  
    if (lpTstate->state == tk_fatalerr)
        return(ch);
  
    do {
        if (!(*lpBuf) || !isNumeric(lpBuf))
        {
            if (lpTstate->reportErr)
                WriteErr(lpTstate, SF_TKBADWIDTH);
            goto backout;
        }
  
        width = atoi(lpBuf);
  
        ch = GetToken(lpTstate->hFile, lpBuf, bufsz, ch);
  
        if (!(*lpBuf) || !isNumeric(lpBuf))
        {
            if (lpTstate->reportErr)
                WriteErr(lpTstate, SF_TKBADHEIGHT);
            goto backout;
        }
  
        height = atoi(lpBuf);
  
        if (getScrnType(width,height) == lpTstate->scrnType)
        {
            if (!bestwidth || !bestheight)
            {
                bestwidth = width;
                bestheight = height;
            }
            else
            {
                if (ABS(lpTstate->scrnHeight - height) <
                    ABS(lpTstate->scrnHeight - bestheight))
                {
                    bestwidth = width;
                    bestheight = height;
                }
            }
        }
  
        *lpBuf = '\0';
  
        /*  Advance to the equal sign or the next
        *  aspect-ratio value.
        */
        while (ch != ':' && ch != '=' && ch >= ' ' && !(*lpBuf))
        {
            ch = GetToken(lpTstate->hFile, lpBuf, bufsz, ch);
        }
  
    } while (ch == ':');
  
  
    DBMSG(("bestwidth=%d, bestheight=%d, scrnHeight=%d, bestdif=%d\n",
    bestwidth, bestheight, lpTstate->scrnHeight, lpTstate->bestScrnDif));
  
    /*  If this is the best match for a screen font, then load it.
    */
    if (bestwidth && bestheight &&
        (ABS(lpTstate->scrnHeight - bestheight) <= lpTstate->bestScrnDif))
    {
        /*  Set up the screen font struct.
        */
        lpSFscrn = (LPSFDIRSCRFNT)lpTstate->lpBuf;
        lpBuf = (LPSTR)lpSFscrn->s;
        bufsz = lpTstate->bufsz - sizeof(SFDIRSCRFNT) + 1;
        *lpBuf = '\0';
  
        /*  Initialize.
        */
        lpSFscrn->unused1 = 0;
        lpSFscrn->scrnType = lpTstate->scrnType;
        lpSFscrn->width = bestwidth;
        lpSFscrn->height = bestheight;
        lpSFscrn->indLOGdrv = -1;
        lpSFscrn->indFNpath = -1;
        lpSFscrn->offsFN = 0;
  
        /*  Look for the equal sign (=).
        */
        while (ch != '=' && ch >= ' ' && *lpBuf == '\0')
        {
            ch = GetToken(lpTstate->hFile, lpBuf, bufsz, ch);
        }
  
        /*  If we did not find the equal sign (=) or we found something
        *  else, abort.
        */
        if (ch != '=' || (*lpBuf))
        {
            if (lpTstate->reportErr)
                WriteErr(lpTstate, SF_TKEXPEQUAL);
            goto backout;
        }
  
        /*  Read the description string.
        */
        ch = GetString(lpTstate->hFile, lpBuf, bufsz, ch, ',');
  
        /*  Error if we did not find the comma, or found the comma
        *  but did not find a string.
        */
        if (ch != ',' || !(*lpBuf))
        {
            if (lpTstate->reportErr)
                WriteErr(lpTstate, SF_TKEXPSCRFNM);
            goto backout;
        }
  
        /*  Preserve the description string.
        */
        ind = lstrlen(lpBuf) + 1;
  
        /*  Pick up the file name.
        */
        ch = GetFileName(lpTstate->hFile, &lpBuf[ind], bufsz-ind, ch);
  
        /*  No file name, abort.
        */
        if (lpBuf[ind] == '\0')
        {
            if (lpTstate->reportErr)
                WriteErr(lpTstate, SF_TKEXPSCRFNM);
            goto backout;
        }
  
        /*  Add the file path and logical label to the SF directory.
        */
        if (!AddFile(lpTstate,(LPSFDIRFILE)lpSFscrn,
            &lpSFscrn->indFNpath,&lpBuf[ind],bufsz-ind))
            goto backout;
  
        /*  Set offset to file name.
        */
        lpSFscrn->offsFN = ind;
  
        /*  Set total size.
        */
        ind += lstrlen(&lpBuf[ind]) + sizeof(SFDIRSCRFNT);
  
        if ((ind=addSFdirEntry(0L,(LPSTR)lpSFscrn,SF_SCRN,ind)) > -1)
        {
            /*  Replace screen font.
            */
            if (lpTstate->indScrnFnt > -1)
                delSFdirEntry(0L, lpTstate->indScrnFnt);
            lpTstate->indScrnFnt = ind;
  
            lpTstate->bestScrnDif = ABS(lpTstate->scrnHeight - bestheight);
        }
    }
  
    backout:
    ch = DumpLine(lpTstate->hFile, lpTstate->lpBuf, lpTstate->bufsz, ch);
  
    return (lpTstate->ch = ch);
}
  
/*  GetPackage
*/
LOCAL BYTE GetPackage(lpTstate)
LPSFDIRTKSTATE lpTstate;
{
    BYTE ch = lpTstate->ch;
  
    DBGgettoken(("GetPackage(%lp)\n", lpTstate));
  
    if (lpTstate->state == tk_fatalerr)
        return(ch);
  
    return (lpTstate->ch = ch);
}
  
  
/*  EndPackage
*/
LOCAL BYTE EndPackage(lpTstate)
LPSFDIRTKSTATE lpTstate;
{
    BYTE ch = lpTstate->ch;
  
    DBGgettoken(("EndPackage(%lp)\n", lpTstate));
  
    if (lpTstate->state == tk_fatalerr)
        return(ch);
  
    return (lpTstate->ch = ch);
}
  
/*  GetFamily
*/
LOCAL BYTE GetFamily(lpTstate)
LPSFDIRTKSTATE lpTstate;
{
    LPSTR lpBuf = lpTstate->lpBuf;
    int bufsz = lpTstate->bufsz;
    BYTE ch = lpTstate->ch;
    int ind;
  
    DBGgettoken(("GetFamily(%lp)\n", lpTstate));
  
    if (lpTstate->state == tk_fatalerr)
        return(ch);
  
    *lpBuf = '\0';
  
    /*  Look for the open brace ({).
    */
    while (ch != '{' && ch <= ' ' && !(*lpBuf))
    {
        ch = GetToken(lpTstate->hFile, lpBuf, bufsz, ch);
    }
  
    /*  Found a quote, assume it is the family name (an optional
    *  part of the syntax).
    */
    if (ch == '"')
    {
        /*  Save the family name.
        */
        ind = lstrlen(lpBuf) + 1;
  
        /*  Continue looking for the open brace.
        */
        do {
            ch = GetToken(lpTstate->hFile, &lpBuf[ind], bufsz-ind, ch);
        } while (ch != '{' && ch <= ' ' && lpBuf[ind] == '\0');
    }
  
    /*  Save the current state and change the state to FAMILY.
    */
    if (ch == '{' && lpBuf[ind] == '\0')
    {
        PushTstate(lpTstate, lpBuf);
        lpTstate->state = tk_family;
    }
    else
    {
        if (lpTstate->reportErr)
            WriteErr(lpTstate, SF_TKEXPFBRC);
        ch = DumpLine(lpTstate->hFile, lpBuf, bufsz, ch);
    }
  
    return (lpTstate->ch = ch);
}
  
/*  EndFamily
*/
LOCAL BYTE EndFamily(lpTstate)
LPSFDIRTKSTATE lpTstate;
{
    BYTE ch = lpTstate->ch;
  
    DBGgettoken(("EndFamily(%lp)\n", lpTstate));
  
    if (lpTstate->state == tk_fatalerr)
        return(ch);
  
    /*  Restore the previous state.
    */
    PopTstate(lpTstate, ch);
  
    return (lpTstate->ch = ch);
}
  
/*
*  GetCartridgeName
*
*  Finds the name of the cartridge definition
*/
LOCAL BYTE GetCartridgeName(LPSFDIRTKSTATE lpTstate)
{
    LPSFDIRFILE lpSFfile = (LPSFDIRFILE)lpTstate->lpBuf;
    LPSTR lpBuf = (LPSTR)lpSFfile->s;
    LPSTR j, k;
    int bufsz = lpTstate->bufsz - sizeof(SFDIRFILE) + 1;
    int ind;
    BYTE ch = lpTstate->ch;
  
    DBGgettoken(("GetCartridgeName(%lp)\n", lpTstate));
  
    /* these error checks should never be executed since this function
    * is only called when state is cartridge, but I leave them in for
    * future changes...
    */
    if (lpTstate->state == tk_fatalerr)
        return(ch);
  
    if (lpTstate->state != tk_cartridge)
    {
        if (lpTstate->reportErr);
        WriteErr(lpTstate, SF_TKINVALSTR);
        goto backout;
    }
  
    /*  Pick up the length of the string and abort if its too big.
    */
    if ((ind = lstrlen((LPSTR)lpSFfile)) > bufsz)
    {
        if (lpTstate->reportErr);
        WriteErr(lpTstate, SF_TKSTRTOOBIG);
        goto backout;
    }
  
    /*  Shift the string to the "s" field at the end of the
    *  SFDIRFILE structure.
    */
    for (j=(LPSTR)lpSFfile+ind, k=lpBuf+ind; k >= (LPSTR)lpSFfile->s;
        --k, --j)
    {
        *k = *j;
    }
  
    /*  Look for the equal sign (=).
    */
    while (ch != '=' && ch >= ' ' && lpBuf[ind] == '\0')
    {
        ch = GetToken(lpTstate->hFile, &lpBuf[ind], bufsz-ind, ch);
    }
  
    /*  If we did not find the equal sign (=) or we found something
    *  else, abort.
    */
    if (ch != '=' || lpBuf[ind] != '\0')
    {
        if (lpTstate->reportErr)
            WriteErr(lpTstate, SF_TKEXPEQUAL);
        goto backout;
    }
  
    /*  Preserve the NULL character.
    */
    ++ind;
  
    /*  Pre-initialize indices to SF dir items.
    */
    lpSFfile->indLOGdrv = -1;
    lpSFfile->indScrnFnt = -1;
    lpSFfile->indDLpath = -1;
    lpSFfile->indPFMpath = -1;
    lpSFfile->offsDLname = 0;
    lpSFfile->fIsPCM = 1;
    lpSFfile->orient = 0;
  
    /* read the PCM file name */
    ch = GetFileName(lpTstate->hFile, &lpBuf[ind], bufsz-ind, ch);
  
    /*  No file name, abort.
    */
    if (lpBuf[ind] == '\0')
    {
        if (lpTstate->reportErr)
            WriteErr(lpTstate, SF_TKNOPCMFILE);
        goto backout;
    }
  
    /*  Add the file path and logical label to the SF directory.
    */
    if (!AddFile(lpTstate,lpSFfile,&lpSFfile->indPFMpath,&lpBuf[ind],bufsz-ind))
        goto backout;
  
    /*  If we don't have anything left, abort.
    */
    if (lpBuf[ind] == '\0')
    {
        if (lpTstate->reportErr)
            WriteErr(lpTstate, SF_TKNOPCMFILE);
        goto backout;
    }
  
    /*  Store the offset to the PFM file name.
    */
    lpSFfile->offsPFMname = ind;
  
    ind += lstrlen(&lpBuf[ind]);
  
    /*  Calc total size of the SF_FILE struct (including
    *  terminating NULL).
    */
    ind += sizeof(SFDIRFILE);
  
    /*  Insert the screen font.
    */
    if (lpTstate->indScrnFnt > -1)
    {
        lpSFfile->indScrnFnt = lpTstate->indScrnFnt;
        addSFdirOwner(0L,lpSFfile->indScrnFnt);
    }
  
    /*  Add file to the SF directory.
    */
    if ((ind = addSFdirEntry(0L, (LPSTR)lpSFfile, SF_FILE|SF_CART, ind)) > -1)
    {
        /*  Build a listbox list.
        */
        lpTstate->hSFlb = addSFlistbox(lpTstate->hDB, lpTstate->hSFlb,
        lpTstate->idLB, -1, ind, SFLB_PERM|SFLB_CART, lpBuf, bufsz, 0L);
  
        ++lpTstate->count;
    }
    else
    {
        /*  If we failed to add a SF dir file entry, then report
        *  an "out of memory" fatal err.
        */
        ReportOutOfMem(lpTstate);
        lpTstate->state = tk_fatalerr;
        goto backout;
    }
  
    backout:
    ch = DumpLine(lpTstate->hFile, lpTstate->lpBuf, lpTstate->bufsz, ch);
  
    return (lpTstate->ch = ch);
}
  
/*
*  GetCartridge
*
*  reads a cartridge definition
*/
LOCAL BYTE GetCartridge(LPSFDIRTKSTATE lpTstate)
{
    LPSTR lpBuf = lpTstate->lpBuf;
    int bufsz = lpTstate->bufsz;
    BYTE ch = lpTstate->ch;
    int ind;
  
    DBGgettoken(("GetCartridge(%lp)\n",lpTstate));
  
    if (lpTstate->state == tk_fatalerr)
        return ch;
  
    *lpBuf=0;
  
    /* look for an open brace */
    while (ch != '{' && ch <= ' ' && !*lpBuf)
    {
        ch = GetToken(lpTstate->hFile, lpBuf, bufsz, ch);
    }
  
    if (ch == '{' && !*lpBuf)
    {
        PushTstate(lpTstate,lpBuf);
        lpTstate->state = tk_cartridge;
    }
    else
    {
        if (lpTstate->reportErr)
            WriteErr(lpTstate, SF_TKEXPFBRC);
        ch = DumpLine(lpTstate->hFile, lpBuf, bufsz, ch);
    }
  
    return lpTstate->ch=ch;
}
  
/*  GetLogDrive
*
*  Format for a logical drive spec is:
*
*      DRIVE logdrv: = llabel.LBL, "log description"
*
*  The logical description is optional, but if the comma is there
*  then the logical description should be there.
*
*  The proc is entered with the file pointer just after the reserved
*  word "DRIVE."
*/
LOCAL BYTE GetLogDrive(lpTstate)
LPSFDIRTKSTATE lpTstate;
{
    LPSFDIRLOGDRV lpSFdrv = (LPSFDIRLOGDRV)lpTstate->lpBuf;
    LPSTR lpBuf = (LPSTR)lpSFdrv->s;
    int bufsz = lpTstate->bufsz - sizeof(SFDIRLOGDRV) + 1;
    int ind = 0;
    BYTE ch = lpTstate->ch;
  
    DBGgettoken(("GetLogDrive(%lp)\n", lpTstate));
  
    if (lpTstate->state == tk_fatalerr)
        return(ch);
  
    /*  Pick up the logical drive ID.
    */
    ch = GetDriveID(lpTstate->hFile, lpBuf, bufsz, ch);
  
    /*  If nothing was received, then abort.
    */
    if ((ind = lstrlen(lpBuf)) == 0)
    {
        if (lpTstate->reportErr)
            WriteErr(lpTstate, SF_TKINVALDRV);
        goto backout;
    }
  
    /*  Look for the equal sign (=).
    */
    while (ch != '=' && ch >= ' ' && lpBuf[ind] == '\0')
    {
        ch = GetToken(lpTstate->hFile, &lpBuf[ind], bufsz-ind, ch);
    }
  
    /*  If we did not find the equal sign (=) or we found something
    *  else, abort.
    */
    if (ch != '=' || lpBuf[ind] != '\0')
    {
        if (lpTstate->reportErr)
            WriteErr(lpTstate, SF_TKEXPEQUAL);
        goto backout;
    }
  
    /*  Preserve the NULL character.
    */
    ++ind;
  
    /*  Pick up the label file name.
    */
    ch = GetFileName(lpTstate->hFile, &lpBuf[ind], bufsz-ind, ch);
  
    /*  If we failed to get a valid file name, then abort.
    */
    if (lpBuf[ind] == '\0')
    {
        if (lpTstate->reportErr)
            WriteErr(lpTstate, SF_TKINVALLBL);
        goto backout;
    }
  
    /*  Valid file name received, save its offset.
    */
    lpSFdrv->offsLabel = ind;
  
    ind += lstrlen(&lpBuf[ind]);
  
    /*  Look for a comma (,).
    */
    while (ch != ',' && ch >= ' ' && lpBuf[ind] == '\0')
    {
        ch = GetToken(lpTstate->hFile, &lpBuf[ind], bufsz-ind, ch);
    }
  
    /*  If we read some text while searching for the comma (,)
    *  then abort.
    */
    if (lpBuf[ind] != '\0')
    {
        if (lpTstate->reportErr)
            WriteErr(lpTstate, SF_TKEXPLCMA);
        goto backout;
    }
  
    /*  If we encounterd a comma (,) then read the description string.
    */
    if (ch == ',')
    {
        /*  Preserve the NULL character.
        */
        ++ind;
  
        /*  Read all text to the end of the line.
        */
        ch = GetString(lpTstate->hFile, &lpBuf[ind], bufsz-ind, ch, EOT);
  
        /*  No string, abort.
        */
        if (lpBuf[ind] == '\0')
        {
            if (lpTstate->reportErr)
                WriteErr(lpTstate, SF_TKEXPLSTR);
            goto backout;
        }
  
        /*  A string was read, save its offset.
        */
        lpSFdrv->offsDesc = ind;
  
        ind += lstrlen(&lpBuf[ind]);
    }
    else
        lpSFdrv->offsDesc = 0;
  
    /*  Set the logical drive priority so when fonts are loaded,
    *  they'll be loaded based upon the order the logical drives
    *  appear in the sfinstal.dir file.
    */
    lpSFdrv->priority = gCountLOGdrv++;
  
    /*  Calc total size of the SF_LDRV struct (including
    *  terminating NULL).
    */
    ind += sizeof(SFDIRLOGDRV);
  
    /*  Add the logical drive to the SF dir struct.
    */
    AddLogDrive(lpTstate, lpSFdrv, ind);
  
    backout:
    ch = DumpLine(lpTstate->hFile, lpTstate->lpBuf, lpTstate->bufsz, ch);
  
    return (lpTstate->ch = ch);
}
  
/*  DoNuthing
*
*  Generic proc for handling a token we don't want to support.
*/
LOCAL BYTE DoNuthing(lpTstate)
LPSFDIRTKSTATE lpTstate;
{
    BYTE ch = lpTstate->ch;
  
    DBGgettoken(("DoNuthing(%lp)\n", lpTstate));
  
    if (lpTstate->state == tk_fatalerr)
        return(ch);
  
    ch = DumpLine(lpTstate->hFile, lpTstate->lpBuf, lpTstate->bufsz, ch);
  
    return (lpTstate->ch = ch);
}
  
/***************************************************************************/
/***********************   Generic Token Utilities   ***********************/
  
  
/*  GetString
*
*  Read until the desired termination character is encountered.
*/
LOCAL BYTE GetString(hFile, lpBuf, size, ch, endswith)
int hFile;
LPSTR lpBuf;
int size;
BYTE ch;
BYTE endswith;
{
    int ind = 0;
  
    for (; (ch=GetToken(hFile,&lpBuf[ind],size-ind,ch)) != EOT; )
    {
        if (ch < ' ' || ch == endswith)
            break;
  
        ind = lstrlen(lpBuf);
  
        /*  Put character in the buffer if its not an end-quote.
        */
        if (ch != '"' && ind < size - 1)
        {
            lpBuf[ind++] = ch;
            lpBuf[ind] = '\0';
        }
    }
  
    DBGgetutil(("GetString(%d,%lp,%d,%d,%c): /%ls/\n",
    hFile, lpBuf, size, (int)ch, endswith, lpBuf));
  
    return (ch);
}
  
/*  GetDriveID
*
*  Read the next token and assume it is a drive name (like "A:" or "B:").
*  Add the colon if there is room.
*/
LOCAL BYTE GetDriveID(hFile, lpBuf, size, ch)
int hFile;
LPSTR lpBuf;
int size;
BYTE ch;
{
    int ind;
  
    do {
        ch = GetToken(hFile, lpBuf, size, ch);
    } while (!(*lpBuf) && ch < ' ' && ch != EOT);
  
    if ((ind = lstrlen(lpBuf)) > 0 && ind < size - 1)
    {
        lpBuf[ind++] = ':';
        lpBuf[ind] = '\0';
    }
  
    DBGgetutil(("GetDriveID(%d,%lp,%d,%d): /%ls/\n",
    hFile, lpBuf, size, (int)ch, lpBuf));
  
    return (ch);
}
  
/*  GetFileName
*
*  Read a valid filename.
*/
LOCAL BYTE GetFileName(hFile, lpBuf, size, ch)
int hFile;
LPSTR lpBuf;
int size;
BYTE ch;
{
    BOOL allowColon = TRUE;
    BOOL allowSlash = TRUE;
    BOOL allowPeriod = TRUE;
    int ind = 0;
  
    for (; (ch=GetToken(hFile,&lpBuf[ind],size-ind,ch)) != EOT; )
    {
        if (ch == ':' && *lpBuf && allowColon)
        {
            allowColon = FALSE;
        }
        else if (ch == '\\' && allowSlash)
        {
            allowColon = FALSE;
        }
        else if (ch == '.' && *lpBuf && allowPeriod)
        {
            allowColon = FALSE;
            allowSlash = FALSE;
            allowPeriod = FALSE;
        }
        else if (ch >= ' ' || *lpBuf)
            break;
  
        if ((ind = lstrlen(lpBuf)) < size - 1)
        {
            lpBuf[ind++] = ch;
            lpBuf[ind] = '\0';
        }
    }
  
    DBGgetutil(("GetFileName(%d,%lp,%d,%d): /%ls/\n",
    hFile, lpBuf, size, (int)ch, lpBuf));
  
    return (ch);
}
  
/*  GetToken
*
*  Read a string from the file which is free of comments.  If the string
*  is preceded by a quote mark, then read until a concluding quote is
*  encountered.  Otherwise, read in a string until white space or an
*  invalid character is encountered.  Invalid characters are those defined
*  in IBM's spec for a file name.
*
*  GetToken() returns the character which terminated the token.  The caller
*  may use this character to determine what kind of a token was returned.
*  For example, a quote (") indicates a the token is a quote string.  The
*  caller should pass back the character in the prevch field on the next
*  call to GetToken() to ensure that a continuous stream of text is
*  processed.
*/
LOCAL BYTE GetToken(hsrcfile, lpBuf, size, prevch)
int hsrcfile;
LPSTR lpBuf;
int size;
BYTE prevch;
{
    BOOL readingComment = FALSE;
    BOOL readingQuote = FALSE;
    int ind, r;
    BYTE ch;
  
    lpBuf[0] = '\0';
  
    DBGgetutil(("GetToken(%d,%lp,%d,%d)\n",
    hsrcfile, lpBuf, size, (int)prevch));
  
    if (size <= 0 || prevch == EOT)
    {
        DBGgetutil(("...GetToken(): size <= 0 || prevch == EOT\n"));
        return (EOT);
    }
  
    /*  For each character.
    */
    for (ind=0; (r=_readFileCH(hsrcfile,&lpBuf[ind])) > 0; prevch=ch)
    {
        ch = lpBuf[ind];
  
        /*  Update counts for err reporting.
        */
        ++gCharPos;
        if (ch == '\r')
        {
            ++gLineCount;
            gCharPos = 0;
        }
  
        /*  If already reading a comment, loop until we find the
        *  end of the comment.
        */
        if (readingComment)
        {
            if (ch == '/' && prevch == '*')
                readingComment = FALSE;
            continue;
        }
  
        /*  If already reading a quote, loop until we find the
        *  end of the quote.  Any characters below a space
        *  terminate the quote.
        */
        if (readingQuote)
        {
            if (ch == '"' || ch < ' ')
                break;
            else if (ind < size - 1)
                ++ind;
            continue;
        }
  
        /*  If we've not read anything yet, then blow through
        *  any white space, and look for the beginning of a
        *  comment or quote.
        */
        if (ind == 0)
        {
            if (prevch < ' ' && ch < ' ')
                continue;
  
            if (ch == ' ' || ch == '\t')
                continue;
  
            if (ch == '*' && prevch == '/')
            {
                readingComment = TRUE;
                continue;
            }
  
            if (ch == '"')
            {
                readingQuote = TRUE;
                continue;
            }
        }
  
        /*  If we got this far, then we're looking for an invalid
        *  character per IBM's spec for a file name.  One exception:
        *  open and close squiggly braces are valid characters in
        *  the IBM spec, they're invalid here.
        */
        if ((ch <= ' ') ||
            (ch == '.') ||
            (ch == '"') ||
            (ch == '/') ||
            (ch == '\\') ||
            (ch == '[') ||
            (ch == ']') ||
            (ch == ':') ||
            (ch == '|') ||
            (ch == '<') ||
            (ch == '>') ||
            (ch == '+') ||
            (ch == '=') ||
            (ch == ';') ||
            (ch == ',') ||
            (ch == '{') ||
            (ch == '}'))
        {
            break;
        }
  
        /*  The character is valid, put it in the buffer
        *  if there is room.
        */
        if (ind < size - 1)
            ++ind;
    }
  
    /*  Turn tabs into spaces.
    */
    if (ch == '\t')
        ch == ' ';
  
    /*  Check for end of file.
    */
    if (r <= 0)
        ch = EOT;
  
    lpBuf[ind] = '\0';
  
    DBGgetutil(("...GetToken(%d%c): /%ls/\n",
    (int)ch, ((ch < ' ') ? (BYTE)' ' : (BYTE)ch), lpBuf));
  
    return (ch);
}
  
/***************************************************************************/
/*************************   File-read Utilities   *************************/
  
  
//  _initFileRead()
//
// set up to read into a buffer.  Allocates the buffer and returns its handle.
//
LOCAL HANDLE _initFileRead(hsrcFile)
int hsrcFile;
{
    HANDLE hFileRead = 0;
  
    gLPfileRead = 0L;
    gSZfileRead = 0;
    gPOSfileRead = 0;
  
    if ((hFileRead=GlobalAlloc(GMEM_FIXED,(DWORD)FILEREAD_BUFSZ)) &&
        (gLPfileRead=GlobalLock(hFileRead)))
    {
        DBGx(("_initFileRead(): calling lzRead(%d..)", hsrcFile));
        gSZfileRead = lzRead(hsrcFile, gLPfileRead, FILEREAD_BUFSZ);
        DBGx(("...Ok.\n"));
        return (hFileRead);
    }
  
    if (hFileRead)
        GlobalFree(hFileRead);
  
    return (0);
}
  
  
//  _closeFileRead()
//
//  Frees the buffer allocated by _initFileRead().
//
LOCAL void _closeFileRead(hFileRead)
HANDLE hFileRead;
{
    if (hFileRead)
    {
        if (gLPfileRead)
        {
            GlobalUnlock(hFileRead);
            gLPfileRead = 0L;
        }
  
        GlobalFree(hFileRead);
        hFileRead = 0;
    }
  
    gSZfileRead = 0;
    gPOSfileRead = 0;
}
  
//  _readFileCH()
//
//  Reads a character from the buffer.
//  If the buffer is empty, reads some more from the file.
//
LOCAL int _readFileCH(hsrcFile, lpBuf)
int hsrcFile;
LPSTR lpBuf;
{
    if (gPOSfileRead < gSZfileRead)
    {
        *lpBuf = gLPfileRead[gPOSfileRead++];
        return (1);
    }
  
    if (gSZfileRead < FILEREAD_BUFSZ)
    {
        return (0);
    }
  
    DBGx(("_readFile(): lzRead(%d)", hsrcFile));
    gSZfileRead = lzRead(hsrcFile, gLPfileRead, FILEREAD_BUFSZ);
    DBGx(("..Ok.\n"));
    gPOSfileRead = 0;
  
    if (gPOSfileRead < gSZfileRead)
    {
        *lpBuf = gLPfileRead[gPOSfileRead++];
        return (1);
    }
  
    return (0);
}
  
/***************************************************************************/
/**************************   General Utilities   **************************/
  
  
/*  WriteErr
*/
LOCAL void WriteErr(lpTstate, err)
LPSFDIRTKSTATE lpTstate;
int err;
{
    LPSTR lpErrBuf = (LPSTR)lpTstate + sizeof(SFDIRTKSTATE);
    LPSTR lpLineNum = lpErrBuf + TMPBUF_STRSZ;
    LPSTR lpAroundCH = lpLineNum + TMPBUF_STRSZ;
    LPSTR lpAroundEOL = lpAroundCH + TMPBUF_STRSZ;
  
    DBGwriteerr(("***WriteErr(%d): ", err));
  
    if (!lpTstate->reportErr)
    {
        DBGwriteerr(("not reported.\n"));
        return;
    }
  
    if ((lpTstate->hErrFile <= 0) && (lpTstate->errfile[0] != '\0'))
    {
        OFSTRUCT ofstruct;
  
        lpTstate->hErrFile =
        OpenFile(lpTstate->errfile, &ofstruct, OF_WRITE | OF_CREATE);
    }
  
//  if (lpTstate->hErrFile > 0)
    if (lpTstate->hErrFile >= 0)                                                         // RK 09/04/91
    {
        _lwrite(lpTstate->hErrFile, lpLineNum, lstrlen(lpLineNum));
  
        if (gCharPos)
        {
            _lwrite(lpTstate->hErrFile,lpErrBuf,itoa(gLineCount,lpErrBuf));
            _lwrite(lpTstate->hErrFile,lpAroundCH,lstrlen(lpAroundCH));
            _lwrite(lpTstate->hErrFile,lpErrBuf,itoa(gCharPos,lpErrBuf));
        }
        else
        {
            _lwrite(lpTstate->hErrFile,lpErrBuf,itoa(gLineCount-1,lpErrBuf));
            _lwrite(lpTstate->hErrFile,lpAroundEOL,lstrlen(lpAroundEOL));
        }
  
        _lwrite(lpTstate->hErrFile, (LPSTR)": ", 2);
        LoadString(lpTstate->hMd, err, lpErrBuf, TMPBUF_STRSZ);
        _lwrite(lpTstate->hErrFile, lpErrBuf, lstrlen(lpErrBuf));
        _lwrite(lpTstate->hErrFile, (LPSTR)CrLf, 2);
        DBGwriteerr(("%ls\n", lpErrBuf));
    }
    #ifdef DEBUG
    else
    {
        DBGwriteerr(("could not create err file\n"));
    }
    #endif
}
  
/*  isNumeric
*
*  Return TRUE if the string contains only numbers.  Return FALSE if
*  it is NULL or contains something other than numbers.
*/
LOCAL BOOL isNumeric(lpNum)
LPSTR lpNum;
{
    if (!(*lpNum))
        return FALSE;
  
    for (; *lpNum; ++lpNum)
    {
        if (*lpNum < '0' || *lpNum > '9')
            return FALSE;
    }
  
    return TRUE;
}
  
/*  getScrnType
*
*  Determine what type of screen this is, either EGA, CGA, or 1:1.
*  We do this by catagorizing the ratio of height to width.
*/
LOCAL WORD getScrnType(width, height)
int width;
int height;
{
    WORD scrnType;
    long ratio;
  
    if (width > 0 && height > 0)
    {
        ratio = ldiv(lmul((long)height,(long)10000),(long)width);
  
        DBGscrn(("getScrnType(%d,%d): ratio %ld is ", width, height, ratio));
  
        if (ratio < 6250)
        {
            scrnType = SCRN_CGA;
            DBGscrn(("CGA\n"));
        }
        else if (ratio < 9375)
        {
            scrnType = SCRN_EGA;
            DBGscrn(("EGA\n"));
        }
        else
        {
            scrnType = SCRN_1TO1;
            DBGscrn(("1:1\n"));
        }
    }
    else
    {
        scrnType = SCRN_UNDEF;
        DBGscrn(("getScrnType(%d,%d): ratio 0 is *undefined*\n",
        width, height));
    }
  
    return (scrnType);
}
  
/*  DumpLine
*
*  Read until an end-of-line character (or anything whose ASCII value
*  is less than a space) is encountered.
*/
LOCAL BYTE DumpLine(hFile, lpBuf, size, ch)
int hFile;
LPSTR lpBuf;
int size;
BYTE ch;
{
    DBGgetutil(("DumpLine(%d,%lp,%d,%d): /%ls/\n",
    hFile, lpBuf, size, (int)ch, lpBuf));
  
    while (ch >= ' ' && ch != EOT)
    {
        ch = GetToken(hFile, lpBuf, size, ch);
    }
  
    return (ch);
}
  
/*  PushTstate
*
*  Copy the SFDIRTKSTATE struct into the SF directory.
*/
LOCAL BOOL PushTstate(lpTstate, lpSLine)
LPSFDIRTKSTATE lpTstate;
LPSTR lpSLine;
{
    int FAR *lpIND;
    int ind;
  
    /*  Save this copy of the token state struct in the SF directory.
    */
    ind = addSFdirEntry(0L, (LPSTR)lpTstate, SF_TKST, sizeof(SFDIRTKSTATE));
  
    /*  Pick up a pointer to the saved state.
    */
    if ((lpTstate->prevind = ind) < 0)
    {
        ReportOutOfMem(lpTstate);
        lpTstate->state = tk_fatalerr;
        return FALSE;
    }
  
    /*  Add another owner to all the logical drives -- the current
    *  state owns the drives and the one just pushed into the
    *  SF directory owns the drives.
    */
    for (ind=0, lpIND=&lpTstate->indLOGdrv[0];
        ind < lpTstate->numLOGdrv; ++ind, ++lpIND)
    {
        addSFdirOwner(0L, *lpIND);
    }
  
    /*  Add another owner to this screen font, but set the difference
    *  to be very large, so if we encounter a screen font that just
    *  matches the right screen type, it will replace this screen font.
    */
    if (lpTstate->indScrnFnt > -1)
        addSFdirOwner(0L, lpTstate->indScrnFnt);
    lpTstate->bestScrnDif = (WORD)(-1);
  
    /*  Change status line.
    */
    LoadString(lpTstate->hMd, SFADD_SCAN, lpTstate->sline,
    sizeof(lpTstate->sline));
    ind = lstrlen(lpTstate->sline);
    lmemcpy(&lpTstate->sline[ind], lpSLine, sizeof(lpTstate->sline)-ind);
    lpTstate->sline[sizeof(lpTstate->sline)-1] = '\0';
    SetDlgItemText(lpTstate->hDB, SF_STATUS, lpTstate->sline);
  
    #ifdef DBGdumppp
    DBGdumpSFbuf(0L);
    #endif
}
  
/*  PopTstate
*
*  Restore the previous SFDIRTKSTATE saved in the SF directory.
*/
LOCAL BOOL PopTstate(lpTstate, ch)
LPSFDIRTKSTATE lpTstate;
BYTE ch;
{
    LPSFDIRTKSTATE t;
    int ind;
  
    if (lpTstate->prevind < 0)
        return FALSE;
  
    /*  Copy the prevous state from the SF directory.
    */
    if (t = (LPSFDIRTKSTATE)lockSFdirEntry(0L, ind=lpTstate->prevind))
    {
        /*  Copy any variables which should not change
        *  across states.
        */
        t->hSFlb = lpTstate->hSFlb;
        t->hErrFile = lpTstate->hErrFile;
        t->count = lpTstate->count;
  
        /*  Copy back previous state.
        */
        lmemcpy((LPSTR)lpTstate, (LPSTR)t, sizeof(SFDIRTKSTATE));
  
        /*  Update some variables.
        */
        lpTstate->ch = ch;
        *lpTstate->lpBuf = '\0';
  
        unlockSFdirEntry(ind);
        delSFdirEntry(0L, ind);
  
        SetDlgItemText(lpTstate->hDB, SF_STATUS, lpTstate->sline);
  
    #ifdef DBGdumppp
        DBGdumpSFbuf(0L);
    #endif
  
        return TRUE;
    }
  
    return FALSE;
}
  
/*  initScrnData
*/
LOCAL void initScrnData(lpTstate, lpBuf, bufsz)
LPSFDIRTKSTATE lpTstate;
LPSTR lpBuf;
int bufsz;
{
    HDC hDC;
    int height = 0;
    int width = 0;
  
    if (hDC = GetWindowDC(lpTstate->hDB))
    {
        /*  We picked up the Display Context of the installer dialog,
        *  and will use its dimensions to determine the lines/inch
        *  in the horizontal and vertical dimensions of the screen.
        */
        width = lpi(GetDeviceCaps(hDC,HORZRES),GetDeviceCaps(hDC,HORZSIZE));
        height = lpi(GetDeviceCaps(hDC,VERTRES),GetDeviceCaps(hDC,VERTSIZE));
        ReleaseDC(lpTstate->hDB,hDC);
    }
  
    lpTstate->scrnType = getScrnType(width,height);
    lpTstate->scrnHeight = height;
    lpTstate->scrnWidth = width;
    lpTstate->bestScrnDif = (WORD)(-1);
    lpTstate->indScrnFnt = -1;
  
    if (lpTstate->reportErr)
    {
        int j, k;
  
        LoadString(lpTstate->hMd,SF_TKSCRNCAP,lpBuf,bufsz);
        j = k = lstrlen(lpBuf) + 1;
  
        LoadString(lpTstate->hMd,SF_TKSCRNMSG1,&lpBuf[k],bufsz-k);
        k += lstrlen(&lpBuf[k]);
        k += itoa(lpTstate->scrnWidth, &lpBuf[k]);
        lpBuf[k++] = ':';
        k += itoa(lpTstate->scrnHeight, &lpBuf[k]);
  
        LoadString(lpTstate->hMd,SF_TKSCRNMSG2,&lpBuf[k],bufsz-k);
        k += lstrlen(&lpBuf[k]);
  
        switch (lpTstate->scrnType)
        {
            case SCRN_CGA:
                LoadString(lpTstate->hMd,SF_TKSCRNCGA,&lpBuf[k],bufsz-k);
                break;
            case SCRN_EGA:
                LoadString(lpTstate->hMd,SF_TKSCRNEGA,&lpBuf[k],bufsz-k);
                break;
            case SCRN_1TO1:
                LoadString(lpTstate->hMd,SF_TKSCRN1TO1,&lpBuf[k],bufsz-k);
                break;
            case SCRN_UNDEF:
            default:
                LoadString(lpTstate->hMd,SF_TKSCRNUNDF,&lpBuf[k],bufsz-k);
                break;
        }
  
        MessageBox(lpTstate->hDB,&lpBuf[j],lpBuf,MB_OK);
    }
}
  
/*  lpi
*
*  Take a dimension represented in screen lines and millimeters and
*  return the ratio in lines/inch.
*/
LOCAL int lpi(lines, mm)
int lines;
int mm;
{
    int n;
  
    if (lines > 0 && mm > 0)
    {
        n = (int)ldiv(lmul((long)lines,(long)25),(long)mm);
  
        DBGscrn(("lpi(%d,%d)=%d\n", lines, mm, n));
        return (n);
    }
    else
    {
        DBGscrn(("lpi(%d,%d)=0\n", lines, mm));
        return (0);
    }
}
  
/*  AddLogDrive
*
*  Add the logical drive to the SF directory struct.  First traverse
*  the existing list of logical drives and if a matching drive is
*  found, then replace it.
*/
LOCAL BOOL AddLogDrive(lpTstate, lpSFdrv, size)
LPSFDIRTKSTATE lpTstate;
LPSFDIRLOGDRV lpSFdrv;
int size;
{
    LPSFDIRLOGDRV lpSFcmp;
    int FAR *lpIND;
    int ind;
  
    /*  Look for a matching logical drive.
    */
    for (ind=0, lpIND=&lpTstate->indLOGdrv[0];
        ind < lpTstate->numLOGdrv; ++ind, ++lpIND)
    {
        if (lpSFcmp=(LPSFDIRLOGDRV)lockSFdirEntry(0L,*lpIND))
        {
            if (lstrcmpi(lpSFdrv->s, lpSFcmp->s) == 0)
            {
                /*  Match found, break out of the search.
                */
                unlockSFdirEntry(*lpIND);
                break;
            }
            unlockSFdirEntry(*lpIND);
        }
        else
        {
            /*  If we failed to lock a SF dir entry, then report
            *  an "out of memory" fatal err.
            */
            ReportOutOfMem(lpTstate);
            lpTstate->state = tk_fatalerr;
            return FALSE;
        }
    }
  
    /*  If a match was found, delete it.  Otherwise, make sure we
    *  have room for a new entry. If not, then report a fatal err.
    */
    if (ind < lpTstate->numLOGdrv)
    {
        delSFdirEntry(0L, *lpIND);
    }
    else if (lpTstate->numLOGdrv >= SFMAXDRV)
    {
        if (lpTstate->reportErr)
            WriteErr(lpTstate, SF_TKMAXDRV);
        lpTstate->state = tk_fatalerr;
        return FALSE;
    }
    else
        ++lpTstate->numLOGdrv;
  
    /*  Add the new entry.  If we fail, then report an "out of memory"
    *  fatal err.
    */
    if ((*lpIND=addSFdirEntry(0L,(LPSTR)lpSFdrv,SF_LDRV,size)) < 0)
    {
        ReportOutOfMem(lpTstate);
        lpTstate->state = tk_fatalerr;
        return FALSE;
    }
  
    return TRUE;
}
  
/*  AddFile
*
*  Parse the file name for a logical path.  If one is found, strip it
*  off and add it as a logical drive.  If a path is included in the
*  file name, strip it off and add it as a SF_PATH directory entry.
*/
LOCAL BOOL AddFile(lpTstate, lpSFfile, lpPath, lpFile, fbufsz)
LPSFDIRTKSTATE lpTstate;
LPSFDIRFILE lpSFfile;
int FAR *lpPath;
LPSTR lpFile;
int fbufsz;
{
    LPSTR s, t;
    BYTE savech;
  
    /*  Step forward through the file name stopping at the end
    *  of the drive name.
    */
    for (s=lpFile; *s && *s != ':'; ++s)
        ;
  
    if (*s == ':')
    {
        LPSFDIRLOGDRV lpSFcmp;
        int FAR *lpIND;
        int ind;
  
        /*  Temporarily terminate the drive name with a NULL.
        */
        savech = *(++s);
        *s = '\0';
  
        /*  Look for a matching logical drive.
        */
        for (ind=0, lpIND=&lpTstate->indLOGdrv[0];
            ind < lpTstate->numLOGdrv; ++ind, ++lpIND)
        {
            if (lpSFcmp=(LPSFDIRLOGDRV)lockSFdirEntry(0L,*lpIND))
            {
                if (lstrcmpi(lpFile, lpSFcmp->s) == 0)
                {
                    /*  Match found, break out of the search.
                    */
                    unlockSFdirEntry(*lpIND);
                    break;
                }
                unlockSFdirEntry(*lpIND);
            }
            else
            {
                /*  If we failed to lock a SF dir entry, then report
                *  an "out of memory" fatal err.
                */
                ReportOutOfMem(lpTstate);
                lpTstate->state = tk_fatalerr;
                return FALSE;
            }
        }
  
        /*  Matching logical drive found.
        */
        if (ind < lpTstate->numLOGdrv)
        {
            if (lpSFfile->indLOGdrv < 0)
            {
                /*  Add another owner to this SF directory entry.
                */
                addSFdirOwner(0L, *lpIND);
                lpSFfile->indLOGdrv = *lpIND;
            }
            else if (lpSFfile->indLOGdrv != *lpIND)
            {
                /*  A logical drive already exists for this file
                *  entry and it does not match the one we just
                *  found, so abort.
                */
                if (lpTstate->reportErr)
                    WriteErr(lpTstate, SF_TKDUPDRV);
                return FALSE;
            }
        }
        else if (lstrlen(lpFile) > 2 || !isAlpha(lpFile[0]))
        {
            /*  Warn that this may be a bogus logical drive.
            */
            if (lpTstate->reportErr)
                WriteErr(lpTstate, SF_TKUNKDRV);
        }
  
        /*  Restore the character.
        */
        *s = savech;
    }
  
    /*  Replace the drive with the path from which we are reading.
    */
    MergePath(0L, lpFile, fbufsz, FALSE);
  
    /*  Step backward through the file name stopping at the end
    *  of the path.
    */
    for (s=lpFile+lstrlen(lpFile);
        (s > lpFile) && (s[-1] != ':') && (s[-1] != '\\'); --s)
        ;
  
    if (s > lpFile)
    {
        /*  Temporarily terminate the path with a NULL.
        */
        savech = *s;
        *s = '\0';
  
        /*  Insert string path name, allow two bytes before the
        *  string for use by the SF directory utilities and
        *  one byte at the end for the NULL-terminator.
        */
        *lpPath = addSFdirEntry(0L, lpFile-2, SF_PATH, lstrlen(lpFile)+3);
  
        /*  Restore the character.
        */
        *s = savech;
  
        /*  Shift the file name back over the path.
        */
        for (t=lpFile; *s; ++t, ++s)
            *t = *s;
        *t = '\0';
    }
    else
        *lpPath = -1;
  
    return TRUE;
}
  
/*  ReportOutOfMem
*/
LOCAL void ReportOutOfMem(lpTstate)
LPSFDIRTKSTATE lpTstate;
{
    LPSTR lpBuf = lpTstate->lpBuf;
    int bufsz = lpTstate->bufsz;
    int ind;
  
    if (LoadString(lpTstate->hMd, SF_TKOOMCAP, lpBuf, bufsz) &&
        (ind=lstrlen(lpBuf)+1) &&
        LoadString(lpTstate->hMd, SF_TKOOMMSG, &lpBuf[ind], bufsz-ind))
    {
        MessageBox(lpTstate->hDB, &lpBuf[ind], lpBuf, MB_OK | MB_ICONHAND);
    }
}
