/**[f******************************************************************
* sfinstal.c -
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

/******************************   sfinstal.c   *****************************/
/*
*  SFInstall:  Main program module for soft font installer.
*
*  7 nov 91    rk(hp) In SFdlgFn (SF_ADD_RIGHT) and (SF_MOVE) only check for 
*              "HPPCL5" so San Diego Driver will work. Added external reference
*              to lstrncpy and local variable driver .
*  4 nov 91    rk(hp) In SFdlgFn (SF_COPYPORT) increased buf from 31 to 64
*              for SF_NOPORT (line 828) and SF_YESPORT (line 894)
* 25 oct 91    rk(hp) In SFdlgFn (SF_INITLB_LEFT) use LoadString SF_INSTALMSG
*              instead of hardcoding string so it can be localized. Local 
*              buf[81]. Bug 718.
* 15 jul 91    rk(hp) Added change to InstallSoftFont proposed by ZhanW
*              to fix sporatic bug caused by gLPortNm, gModNm and gPortNm, and
*              increased gLPortNm and gPortNm from 32 bytes to 128 bytes.
*
* 09 jul 91    rk(hp) Call FreeListBoxBitmaps() from InstallSoftFont to 
*              DeleteObject(s) hBMDisk and hBMCart. Add function declaration
*              for FreeListBoxBitmaps().
*
* 25 jun 91    rk(hp) Added gPortNm as an extern, and in InstallSoftFont
*              used lmemcpy to copy lpPortNm which is passed in.
*
* 18 jun 91    rk(hp) expanded both char buf in SF_COPYPORT from 24 to 31 to 
*              handle change to "End Copy Fonts to New &Port..."
*
* 01 aug 90    doug kaltenecker(hp) and ken oakeson(hp) modified many
*              files in the finstall code to add support for ifw, afs,
*              and numerous other font types.
*
* 02 oct 89    peterbe Help button calls WinHelp(..HELP_INDEX..) now.
*
* 27 sep 89    peterbe Added InitStatusLine() to indicate whether soft fonts
*              are handled.
*
* 13 sep 89    peterbe Moved WEP() to WEP.A
*
* 23 aug 89    peterbe Added HELP support (see HelpWasCalled, SF_HELP,etc.)
*
* 22 aug 89    peterbe Remember to unlock segment on error returns.
*
* 08 aug 89    peterbe Add neg. return codes for lack of or bad LZ module.
*
* 01 aug 89    peterbe Add lpOpenFile for LZOpenFile() call.
*              Add WEP().
*
* 28 jul 89    peterbe Load LZEXPAND.DLL and get addresses for the 4
*              main functions used here.
*
* 17 jul 89    peterbe Add global gPrintClass, set from InstallSoftFont()
*              parameter (for LaserJet/DeskJet/DJ Plus selection)
*
* 28 jun 89    peterbe (1.29 next). Renamed SoftFontInstall() to
*              InstallSoftFont() and added parameter to indicate
*              what driver's calling this, to identify printer class.
*
* 11 may 89    peterbe (1.23 build next).  Making Edit button always
*              enabled.
*
* 05 may 89    peterbe Hide SF_ADD_RIGHT before calling GetPort() to fix
*              redraw problem if port dialog moves.
*
* 04 may 89    peterbe If DrawItem() returns FALSE, call DefWindowProc(),
*              to redraw/clear the focus caret for a selection.
*
* 15 apr 89    peterbe About box evoked by button instead of sys menu now.
*
* 27 mar 89    peterbe Add call to GetListboxBitmaps().  Passes hDb now.
*
* 25 mar 89    peterbe DrawItem() returns BOOL value now.
*
* 24 mar 89    peterbe Moved routines created yesterday to SFOWNER.C,
*              new module.  Just have calls to them here.
* 23 mar 89    peterbe Adding code for WM_MEASUREITEM and WM_DRAWITEM,
*              for user-draw listbox items.
* 21 mar 89    peterbe Removed copyright string SF_COPYRIGHT from main
*              dialog box.
* 07 mar 89    peterbe Making SF_EDIT pushbutton permanently enabled.
*
* 02 mar 89    peterbe Changed tabs to 8 spaces.  Removed SF_YESEDIT and
*              SF_YESPORT sys. menu items, and related code.
*   1-25-89    jimmat  Changed SoftFontInstall() parameters to not pass
*              the module Instance handle--use hLibInst instead.
*   1-26-89    jimmat  Adjustments do to changes in the resource file.
*   2-20-89    jimmat  Font Installer/Driver use same WIN.INI section (again)!
*/
/***************************************************************************/
  
  
//#define DEBUG
  
//#include "nocrap.h"
#undef NOCTLMGR
#undef NOWINMESSAGES
#undef NOSHOWWINDOW
#undef NOMEMMGR
#undef NOVIRTUALKEYCODES
#undef NOMENUS
#undef NOSYSCOMMANDS
#undef NOMSG
#undef NOMB
//#undef NOTEXTMETRIC
#include "windows.h"
#include "sfinstal.h"
#include "dlgutils.h"
#include "neededh.h"
#include "resource.h"
#include "sfdir.h"
#define NOBLDDESCSTR
#include "sfutils.h"
#include "strings.h"
#include "sffile.h"
#include "sfadd.h"
#include "sferase.h"
#include "sfcopy.h"
#include "sfedit.h"
#include "sfdownld.h"
  
/* Added for IFW support */
#include "_cgifwin.h"
#include "_tmu.h"
#include "_sflib.h"
#include "_readlib.h"
#include "_resourc.h"
#include "_sfadd.h"
#include "_support.h"
  
/****************************************************************************\
* Debug Definitions
\****************************************************************************/
  
#ifdef DEBUG
    #define DBGisf(msg)     /*DBMSG(msg)*/
    #define DBGsfdlg(msg)   /*DBMSG(msg)*/
#else
    #define DBGisf(msg)     /*null*/
    #define DBGsfdlg(msg)   /*null*/
#endif
  
  
#define LOCAL static
  
/*  This seems to prevent keyboard shortcuts from getting
*  confused after updating the status line.
*/
#define KERPLUNK(hDB) SetFocus(GetDlgItem(hDB, SF_STATUS))
  
/*  Forward references
*/
  
/* main dialog function */
BOOL FAR PASCAL SFdlgFn(HWND, unsigned, WORD, LONG);
  
/* Functions in SFOWNER.C for dialog function */
void FAR PASCAL FillMeasureItem(LPMEASUREITEMSTRUCT);
BOOL FAR PASCAL DrawItem(HWND, LPDRAWITEMSTRUCT);
void FAR PASCAL GetListboxBitmaps(HWND);
void FAR PASCAL FreeListboxBitmaps(void);                                                // rk(hp) 7/9/91
//VOID FAR PASCAL ResetLibrary(lpFontLibrary);
  
int gSF_FLAGS = 0;
int gFSvers = 0;
int gPrintClass;
  
extern HANDLE hLibInst;
LPSTR FAR PASCAL lstrncpy(LPSTR, LPSTR, int);                                            // rk(HP) 11/7/91
  
LOCAL WORD gDlgState = 0;
LOCAL HANDLE gHLBleft = 0;
LOCAL HANDLE gHLBright = 0;
LOCAL BOOL gIgnoreMessages = FALSE;
//LOCAL char gLPortNm[32];
LOCAL char gLPortNm[128];
//LOCAL char gRPortNm[32];
LOCAL char gRPortNm[128];
LOCAL BOOL bCopyPort = FALSE;
LOCAL BOOL HelpWasCalled = FALSE;
  
LPSTR glpUpCasePrintClass;
LPSTR glpLoCasePrintClass;
  
BOOL gNewPrinterFonts = FALSE;
BOOL gNewScreenFonts = FALSE;
BOOL GetKeyName(LPSTR, int);
  
#define FSD_NONE    0
#define FSD_SOURCE  1
#define FSD_INSTALLED   2
#define FSD_SCREEN  3
LOCAL BOOL FontSelDisplayed = FSD_NONE;
  
BOOL gNoSoftFonts = FALSE;  // if TRUE, printer has only cartridges.
  
// Handle and FAR function pointers for the LZEXPAND.DLL library.
HANDLE hLZ;
FARPROC lpOpenFile;
FARPROC lpInit;
FARPROC lpSeek;
FARPROC lpRead;
FARPROC lpClose;
  
// local function
  
void NEAR PASCAL InitStatusLine(HWND);
/* also one in the IFW section */
  
  
/**********************************************************************
*           variables added for ifw font installation
***********************************************************************/
  
extern char gModNm[32];
extern char gIndexFile[MAX_FILENAME_SIZE];
extern char gSupportFileDirectory[MAX_FILENAME_SIZE];
extern char gTypeDirectory[MAX_FILENAME_SIZE];
extern char gSourcePath[MAX_FILENAME_SIZE];
//extern char gPortNm[32];
extern char gPortNm[128];
extern char gAppName[sizeof(gModNm)+sizeof(gPortNm)];
extern WORD gNumInstalledFonts;
extern DIRECTORY gSourceLibrary;
extern DIRECTORY gInstalledLibrary;
extern DIRECTORY gScreenLibrary;
extern char gBulletIntellifont;
  
BOOL gUpdateFontSummary = FALSE;
BOOL SFI_HelpWasCalled = FALSE;
BOOL SFG_HelpWasCalled = FALSE;
  
int goption;
int gNumSourceFonts=0;
  
LOCAL BOOL SomeSourceFontSelected = FALSE;
LOCAL BOOL SomeScalableFontSelected = FALSE;
LOCAL BOOL SomeScreenFontSelected = FALSE;
  
/* Handles needed for call to EnumFonts */
  
  
void UpdateBow(HWND, HANDLE);
int FAR PASCAL EnumFunc(LPLOGFONT, LPTEXTMETRIC, WORD, LPSTR);
  
/**************************************************************************/
/*************************   Global Procs   *******************************/
  
/*  InstallSoftFont
*
*  Soft font installer startup procedure.
*/
  
int FAR PASCAL
InstallSoftFont(hWndParent, lpModNm, lpPortNm, smartMode, fsvers, nPrintClass)
HWND hWndParent;
LPSTR lpModNm;
LPSTR lpPortNm;
BOOL smartMode;
int fsvers;
int nPrintClass;        // 0 for PCL, 1 for DeskJet, etc.
{
    FARPROC lpDlgFunc;
    HANDLE hWinSF = 0;
    LPSTR dlFile = 0L;
  
    /* Added for ifw */
    HWND hWnd;
    HANDLE hInst;
  
    hWnd = hWndParent;
    hInst = hLibInst;
  
    LockSegment(-1);    /* lock the soft font installer's data seg */
  
    // DBGisf(("InstallSoftFont(%d,%ls,%ls,%d,%d): %d\n",(WORD)hWndParent,
    // lpModNm, lpPortNm, (WORD)smartMode, fsvers, nPrintClass));
  
    /*  Pick up the global information.
    */
    gDlgState = SFDLG_INACTIVE;
    gSF_FLAGS = 0;
    gFSvers = fsvers;
    gHLBleft = 0;
    gHLBright = 0;
    gIgnoreMessages = FALSE;
    {
    int tmp;

    if ((tmp = lstrlen(lpModNm)) >= sizeof (gModNm))
       // truncate the string passsed in
       tmp = sizeof(gModNm) -1;
    lmemcpy((LPSTR)gModNm, lpModNm, tmp);
    gModNm[tmp] = '\0';

    if ((tmp = lstrlen(lpPortNm)) >= sizeof (gLPortNm))
       // truncate the string passsed in
       tmp = sizeof(gLPortNm) -1;
    lmemcpy((LPSTR)gLPortNm, lpPortNm, tmp);
    gLPortNm[tmp] = '\0';
    lmemcpy((LPSTR)gPortNm, lpPortNm, tmp);
    gPortNm[tmp] = '\0';
    }
    /* Updated global to be used in AddDlgFn in sfadd.c */

    gRPortNm[0] = '\0';
    gPrintClass = nPrintClass;
  
    gInstalledLibrary.hFiles = NULL;
    gSourceLibrary.hFiles = NULL;
  
  
    // handle gPrintClass bit for 'no soft fonts'
    if (256 & gPrintClass)
    {
        gPrintClass &= !(WORD)256;
        gNoSoftFonts= TRUE;
    }
  
  
    lstrcpy((LPSTR)gAppName,(LPSTR)gModNm);
    lstrcat((LPSTR)gAppName,(LPSTR)",");
    lstrcat((LPSTR)gAppName,(LPSTR)gLPortNm);
    if(gAppName[lstrlen((LPSTR)gAppName)-1]==':')
        gAppName[lstrlen((LPSTR)gAppName)-1]='\0';
  
    DBGisf(("gAppName = %ls\n",(LPSTR)gAppName));
  
  
    // Load the LZ decompression library and get the addresses
    // of its functions.
    if ( 32 > (WORD) (hLZ = LoadLibrary((LPSTR) "lzexpand.dll")))
    {
        UnlockSegment(-1);  // unlock segment
        return(-1);
    }
  
    if (    // get proc. addresses and check them
        (NULL == (lpOpenFile = GetProcAddress(hLZ, (LPSTR) "LZOpenFile"))) ||
        (NULL == (lpInit = GetProcAddress(hLZ, (LPSTR) "LZInit"))) ||
        (NULL == (lpRead = GetProcAddress(hLZ, (LPSTR) "LZRead"))) ||
        (NULL == (lpSeek = GetProcAddress(hLZ, (LPSTR) "LZSeek"))) ||
        (NULL == (lpClose = GetProcAddress(hLZ, (LPSTR) "LZClose"))) )
    {   // oops, something wasn't there..
        //MessageBox(hWndParent,
        //  (LPSTR)"A function is missing in LZEXPAND.DLL",
        //  (LPSTR)"Font Installer",
        //  MB_OK | MB_ICONEXCLAMATION );
        FreeLibrary(hLZ);
        UnlockSegment(-1);  // unlock segment
        return(-2);
    }
  
    // this isn't enough, somehow .. see WM_INITDIALOG code
    if (smartMode)
        gDlgState |= SFDLG_ALLOWEDIT | (SFDLG_ALLOWEDIT << 4);
  
  
    GetListboxBitmaps(hWndParent);  // for user-draw listboxes
  
    MyDialogBox(hLibInst,SFINSTALL, hWndParent, SFdlgFn);
  
    /* Delete bitmaps from memory */
    FreeListboxBitmaps();                                                                // rk(hp) 7/9/91


    FreeLibrary(hLZ);
  
    /* Added to Update ifw font list if fonts were added/deleted */
  
    if (gUpdateFontSummary)
    {
        DBGisf(("inside if UndateFontSummary\n"));
        UpdateBow(hWnd, hInst);
    }
  
    UnlockSegment(-1);  /* about to exit, unlock soft font installers seg */
  
    return ((gSF_FLAGS & SF_CHANGES) ? ((gFSvers > 0) ? gFSvers : 1) : 0);
  
} // InstallSoftFont()
  
  
/****************************************************************************
*
*     SFdlgFn: Main Dialog function for font installer. (export)
*
*****************************************************************************/
  
BOOL FAR PASCAL SFdlgFn(hDB, wMsg, wParam, lParam)
HWND hDB;
unsigned wMsg;
WORD wParam;
LONG lParam;
{
  
    FARPROC lpDlgFunc;
    BOOL NewSelection;
    char ListBoxEntry[64];
    char StatusLine[128];
    char cValue[16];
    WORD NumInstalled;
    WORD NumRemoved;
    int  option;
  
    switch (wMsg)
    {
        case WM_INITDIALOG:
            DBGsfdlg(("SFdlgFn(%d,%d,%d,%ld): WM_INITDIALOG\n",
            hDB, wMsg, wParam, lParam));
  
            CenterDlg(hDB);
  
            InitLBstrings(hLibInst, 0L, 0L, 0L);
  
            // if this is a LaserJet (no soft fonts) tell the user.
            InitStatusLine(hDB);
  
            EnableWindow(GetDlgItem(hDB, SF_PERM_LEFT), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_TEMP_LEFT), FALSE);
  
            EnableWindow(GetDlgItem(hDB, SF_LB_LEFT), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_LB_RIGHT), FALSE);
  
            ShowWindow(GetDlgItem(hDB, SF_PERM_RIGHT), HIDE_WINDOW);
            ShowWindow(GetDlgItem(hDB, SF_TEMP_RIGHT), HIDE_WINDOW);
  
            /*  Kill the two active controls because the message
            *  SF_INITLB_LEFT enables them after filling in the
            *  left listbox.
            */
            EnableWindow(GetDlgItem(hDB, SF_ADD_RIGHT), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_EXIT), FALSE);
  
            //  Make edit button always enabled now.... (11 may 89)
            gDlgState |= (SFDLG_ALLOWEDIT) | (SFDLG_ALLOWEDIT << 4);
  
            gIgnoreMessages = FALSE;
  
            /*  Show the dialog now and send a message to fill
            *  in the left listbox after the dialog is up.
            */
            ShowWindow(hDB, SHOW_OPENWINDOW);
            UpdateWindow(hDB);
            PostMessage(hDB, WM_COMMAND, SF_INITLB_LEFT, 0L);
  
  
            break;
  
        case WM_COMMAND:
        switch (wParam)
        {
  
            case SF_INITLB_LEFT:
                DBGsfdlg(("SFdlgFn(%d,%d,%d,%ld): SF_INITLB_LEFT\n",
                hDB, wMsg, wParam, lParam));
  
                if (gHLBleft)
                {
                    GlobalFree(gHLBleft);
                    gHLBleft = 0;
                    SendDlgItemMessage(hDB,SF_LB_LEFT,LB_RESETCONTENT,0,0L);
                }
  
                if (gHLBleft=FillListBox(
                    hDB,hLibInst,SF_LB_LEFT,gModNm,gLPortNm));
                gDlgState |= (SFDLG_RESFONTS <<  4);
  
                EnableWindow(GetDlgItem(hDB, SF_ADD_RIGHT), TRUE);
                EnableWindow(GetDlgItem(hDB,SF_EXIT),TRUE);
                SetFocus(GetDlgItem(hDB, SF_ADD_RIGHT));             /* BUG #47 */
                UpdateControls(hDB, gDlgState);

//                SetDlgItemText(hDB, SF_STATUS, (LPSTR)"To install new fonts, click on the [Add fonts] button.");
                {
                  char buf[81];

                  if (LoadString(hLibInst, SF_INSTALMSG, buf, sizeof(buf)))
                      SetDlgItemText(hDB, SF_STATUS, (LPSTR) buf);
                  else
                      SetDlgItemText(hDB, SF_STATUS, (LPSTR)"");
                }
                break;
  
            case SF_IGNORMESSAGES:
                DBGsfdlg(("SFdlgFn(%d,%d,%d,%ld): SF_IGNORMESSAGES\n",
                hDB, wMsg, wParam, lParam));
  
                /*  Enable/disable processing of messages to the perm/temp
                *  buttons (see UpdatePermTemp in sfutils.c).
                */
                if (lParam)
                    gIgnoreMessages = TRUE;
                else
                    gIgnoreMessages = FALSE;
                break;
  
  
            case SF_LB_LEFT:
            case SF_LB_RIGHT:
                DBGsfdlg(("SFdlgFn(%d,%d,%d,%ld): SF_LB\n",
                hDB, wMsg, wParam, lParam));
  
                if (HIWORD(lParam) == LBN_ERRSPACE)
                    EndDialog(hDB,-1);
  
                gDlgState = UpdateStatusLine(hDB, wParam, gDlgState,
                (wParam == SF_LB_LEFT) ? gHLBleft : gHLBright,
                (GetKeyState(VK_SHIFT) < 0 &&
                GetKeyState(VK_CONTROL) < 0));
  
                UpdateControls(hDB, gDlgState);
                break;
  
  
  
            case SF_PERM_LEFT:
            case SF_PERM_RIGHT:
            case SF_TEMP_LEFT:
            case SF_TEMP_RIGHT:
                DBGsfdlg(("SFdlgFn(%d,%d,%d,%ld): SF_PERM\TEMP\n",
                hDB, wMsg, wParam, lParam));
  
                /*  Ignore this message if it comes while we're already
                *  doing something in UpdatePermTemp.
                */
                if (gIgnoreMessages)
                {
                    DBGsfdlg(("...ignoring\n"));
                    break;
                }
  
                gDlgState = UpdatePermTemp(hDB, hLibInst, wParam, gDlgState,
                (wParam == SF_PERM_LEFT || wParam == SF_TEMP_LEFT) ?
                gHLBleft : gHLBright);
  
                UpdateControls(hDB, gDlgState);
                break;
  
  
            case SF_ADD_RIGHT:
                DBGsfdlg(("SFdlgFn(%d,%d,%d,%ld): SF_ADD\n",
                hDB, wMsg, wParam, lParam));
                if (gHLBright)
                {
                    gHLBright = EndAddFontsMode(
                    hDB, hLibInst, gHLBright, SF_LB_RIGHT);
                    gDlgState &= ~(SFDLG_DSKFONTS);
  
                    //          GlobalFree(gSourceLibrary.hFiles);
                    gSourceLibrary.NextEntry = 0;
                    gSourceLibrary.NumFiles = 0;
  
                    if (gDlgState & SFDLG_SELECTED)
                    {
                        gDlgState &= ~(SFDLG_SELECTED);
                        //SetDlgItemText(hDB, SF_STATUS, (LPSTR)"");
                        InitStatusLine(hDB);
                        UpdateControls(hDB, gDlgState);
                    }
                    else if (!(gDlgState & (SFDLG_SELECTED << 4)))
                    {
                        //SetDlgItemText(hDB, SF_STATUS, (LPSTR)"");
                        InitStatusLine(hDB);
                    }
                }
                else
                {
                    WORD n;
                    BOOL PCLV;  /* TRUE if prn can handle scalables */
                    char driver[7];                                                      // rk(hp) 11/7/91
  
//                  PCLV = (lstrcmpi((LPSTR)gModNm,(LPSTR)"HPPCL5A") == 0);
                    lstrncpy((LPSTR)driver, gModNm, 6);  
                    driver[6] = '\0';
                    PCLV = (lstrcmpi((LPSTR)driver, (LPSTR)"HPPCL5") == 0);
  
                    InitStatusLine(hDB);
                    gHLBright = AddFontsMode(hDB, hLibInst, SF_LB_RIGHT, &n,
                    (GetKeyState(VK_SHIFT) < 0 &&
                    GetKeyState(VK_CONTROL) < 0),
                    PCLV);
  
                    if (gHLBright)
                    {
                        gDlgState |= SFDLG_DSKFONTS;
                        resetLB(hDB,hLibInst,SF_LB_RIGHT,gHLBright,n,
                        SF_ADDREADY,FALSE);
                    }
                }
                KERPLUNK(hDB);
                break;
  
  
  
  
            case SF_MOVE:
                DBGsfdlg(("SFdlgFn(%d,%d,%d,%ld): SF_MOVE_OLD\n",
                hDB, wMsg, wParam, lParam));
  
                if (gDlgState & (SFDLG_SELECTED << 4))
                {
                    DBGsfdlg(("inside first if for left\n"));
  
                    if (gDlgState &  SFDLG_RESFONTS)
                    {
                        WORD n;
  
                        gHLBright =    CopyFonts(hDB,hLibInst,SF_LB_RIGHT,
                        gHLBright,gRPortNm,SF_LB_LEFT,gHLBleft,
                        gLPortNm,TRUE,&n,gModNm);
                        resetLB(hDB,hLibInst,SF_LB_LEFT,gHLBleft,n,
                        SF_MOVSUCCESS,TRUE);
                        gDlgState &= ~(SFDLG_SELECTED << 4);
                        UpdateControls(hDB,    gDlgState);
                    }
                }
                else if (gDlgState & SFDLG_SELECTED)
                {
                    if (gDlgState & SFDLG_DSKFONTS)
                    {
                        WORD n;
                        BOOL DoTheWildThang = FALSE;        /* T = Call WriteSel */
                        BOOL PCLV;       /* TRUE if prn can handle scalables */
                        char driver[7];                                                  // rk(hp) 11/7/91
  
//                      PCLV = (lstrcmpi((LPSTR)gModNm,(LPSTR)"HPPCL5A") == 0);
                        lstrncpy((LPSTR)driver, gModNm, 6);  
                        driver[6] = '\0';
                        PCLV = (lstrcmpi((LPSTR)driver, (LPSTR)"HPPCL5") == 0);

                        if (gHLBleft=AddFonts(hDB, hLibInst, SF_LB_LEFT,
                            gHLBleft,SF_LB_RIGHT,gHLBright,&DoTheWildThang,
                            gModNm,gLPortNm,&n, (LPSTR)&gSourceLibrary,
                            PCLV))
                        {
                            if (DoTheWildThang)
                            {
                                /* WriteSelections assumes that the source listbox
                                * is SF_LB_RIGHT and the destination listbox is
                                * SF_LB_LEFT
                                */
  
                                gHLBleft = WriteSelections(hDB,
                                (LPSTR) gTypeDirectory,
                                (LPSTR) gSupportFileDirectory,
                                (WORD FAR *) &NumInstalled,
                                gHLBleft, gHLBright);
                                if (NumInstalled)
                                {
                                    DBGsfdlg(("inside if NumInstalled if\n"));
                                    n+=NumInstalled;
                                    //                             gSourceLibrary.NumFiles -= NumInstalled;
                                    gUpdateFontSummary = TRUE;
                                }
                            }
  
                            gDlgState |= (SFDLG_RESFONTS << 4);
                            gDlgState &= ~(SFDLG_SELECTED);
  
                            if (resetLB(hDB,hLibInst,SF_LB_RIGHT,gHLBright,
                                n, SF_ADDSUCCESS,FALSE))
                            {
                                /*  Listbox is empty, end add fonts mode.
                                */
                                gHLBright = EndAddFontsMode(hDB, hLibInst,
                                gHLBright, SF_LB_RIGHT);
                                gDlgState &= ~(SFDLG_DSKFONTS);
                            }
                        }
                        else
                            gDlgState &= ~(SFDLG_SELECTED);
  
                        UpdateControls(hDB, gDlgState);
                    }
  
                    else if (gDlgState & SFDLG_RESFONTS)
                    {
                        WORD n;
  
                        gHLBleft = CopyFonts(hDB,hLibInst,SF_LB_LEFT,
                        gHLBleft,gLPortNm,SF_LB_RIGHT,gHLBright,
                        gRPortNm,TRUE,&n,gModNm);
                        resetLB(hDB,hLibInst,SF_LB_RIGHT,gHLBright,n,
                        SF_MOVSUCCESS,TRUE);
                        gDlgState &= ~(SFDLG_SELECTED);
                    }
                }
                KERPLUNK(hDB);
                break;
  
  
  
            case SF_COPY:
                DBGsfdlg(("SFdlgFn(%d,%d,%d,%ld): SF_COPY\n",
                hDB, wMsg, wParam, lParam));
  
                if (gDlgState & SFDLG_RESFONTS)
                {
                    if (gDlgState & (SFDLG_SELECTED << 4))
                    {
                        WORD n;
  
                        gHLBright = CopyFonts(hDB,hLibInst,SF_LB_RIGHT,
                        gHLBright,gRPortNm,SF_LB_LEFT,gHLBleft,
                        gLPortNm,FALSE,&n,gModNm);
                        resetLB(hDB,hLibInst,SF_LB_LEFT,gHLBleft,n,
                        SF_CPYSUCCESS,TRUE);
                        gDlgState &= ~(SFDLG_SELECTED << 4);
                        UpdateControls(hDB, gDlgState);
                    }
                    else if (gDlgState & SFDLG_SELECTED)
                    {
                        WORD n;
  
                        gHLBleft = CopyFonts(hDB,hLibInst,SF_LB_LEFT,
                        gHLBleft,gLPortNm,SF_LB_RIGHT,gHLBright,
                        gRPortNm,FALSE,&n,gModNm);
                        resetLB(hDB,hLibInst,SF_LB_RIGHT,gHLBright,n,
                        SF_CPYSUCCESS,TRUE);
                        gDlgState &= ~(SFDLG_SELECTED);
                        UpdateControls(hDB, gDlgState);
  
                        if (n > 0)
                            gDlgState |= (SFDLG_RESFONTS << 4);
                    }
                }
                KERPLUNK(hDB);
                break;
  
  
  
            case SF_ERASE:
                DBGsfdlg(("SFdlgFn(%d,%d,%d,%ld): SF_ERASE\n",
                hDB, wMsg, wParam, lParam));
  
                if (gDlgState & (SFDLG_SELECTED << 4))
                {
                    WORD n;
  
                    if (RemoveFonts(hDB, hLibInst, SF_LB_LEFT,
                        gHLBleft, gModNm, gLPortNm, &n,
                        (LPSTR)gSupportFileDirectory, (LPSTR)&gInstalledLibrary))
                    {
                        gDlgState &= ~(SFDLG_SELECTED << 4);
                        resetLB(hDB, hLibInst, SF_LB_LEFT, gHLBleft,
                        n, SF_RMVSUCCESS, TRUE);
                        UpdateControls(hDB, gDlgState);
                        gUpdateFontSummary = TRUE;
                    }
                }
                else if (gDlgState & SFDLG_SELECTED)
                {
                    WORD n;
  
                    if (RemoveFonts(hDB, hLibInst, SF_LB_RIGHT,
                        gHLBright, gModNm, gRPortNm, &n,
                        (LPSTR)gSupportFileDirectory, (LPSTR)&gInstalledLibrary))
                    {
                        gDlgState &= ~(SFDLG_SELECTED);
                        resetLB(hDB,hLibInst,SF_LB_RIGHT,gHLBright,
                        n,SF_RMVSUCCESS,TRUE);
                        UpdateControls(hDB, gDlgState);
                        gUpdateFontSummary = TRUE;
                    }
                }
                KERPLUNK(hDB);
                break;
  
  
  
            case SF_EDIT:
                DBGsfdlg(("SFdlgFn(%d,%d,%d,%ld): SF_EDIT\n",
                hDB, wMsg, wParam, lParam));
                if (gDlgState & (SFDLG_SELECTED << 4))
                {
                    WORD n;
  
                    if (EditFonts(hDB, hLibInst, SF_LB_LEFT, gHLBleft,
                        ((gDlgState & SFDLG_RESFONTS) ? gHLBright : 0),
                        gModNm, gLPortNm, &n))
                    {
                        resetLB(hDB, hLibInst, SF_LB_LEFT, gHLBleft, n,
                        SF_EDSUCCESS, FALSE);
                    }
                    gDlgState &= ~(SFDLG_SELECTED << 4);
                    UpdateControls(hDB, gDlgState);
                }
                else if ((gDlgState & SFDLG_SELECTED) &&
                    (gDlgState & SFDLG_RESFONTS))
                {
                    WORD n;
  
                    if (EditFonts(hDB, hLibInst, SF_LB_RIGHT, gHLBright,
                        gHLBleft, gModNm, gRPortNm, &n))
                    {
                        resetLB(hDB, hLibInst, SF_LB_RIGHT, gHLBright, n,
                        SF_EDSUCCESS, FALSE);
                    }
                    gDlgState &= ~(SFDLG_SELECTED);
                    UpdateControls(hDB, gDlgState);
                }
                KERPLUNK(hDB);
                break;
  
            case SF_COPYPORT:
                /* turn on/off port copy */
  
                if(!bCopyPort)
                {
                    DBGsfdlg((
                    "SFdlgFn(%d,%d,%d,%ld): SF_COPYPORT, (bCopyPort = FALSE\n",
                    hDB, wMsg, wParam, lParam));
  
                    // hide Add Fonts pushbutton before displaying Ports
                    // dialog, since it may move, causing strange repainting
                    // of the buttons (because it will be replaced by
                    // SF_PERM_RIGHT and SF_TEMP_RIGHT if a port is selected).
  
                    ShowWindow(GetDlgItem(hDB, SF_ADD_RIGHT), HIDE_WINDOW);
  
                    if (GetPort(hDB,hLibInst,gLPortNm,gRPortNm,
                        sizeof(gRPortNm)))
                    {
                        /* for SF_YESPORT */
                        char buf[64];
  
                        bCopyPort = TRUE;
  
                        /*  Clear add fonts mode if the right listbox
                        *  is currently active.
                        */
                        if (gHLBright)
                        {
                            gHLBright = EndAddFontsMode(hDB,
                            hLibInst, gHLBright, SF_LB_RIGHT);
                            gDlgState &= ~(SFDLG_DSKFONTS);
  
                            if (gDlgState & SFDLG_SELECTED)
                            {
                                gDlgState &= ~(SFDLG_SELECTED);
                                SetDlgItemText(hDB, SF_STATUS, (LPSTR)"");
                                UpdateControls(hDB, gDlgState);
                            }
                            else if (!(gDlgState & (SFDLG_SELECTED << 4)))
                            {
                                SetDlgItemText(hDB, SF_STATUS, (LPSTR)"");
                            }
                        }
  
                        /*  Fill right listbox.
                        */
                        gHLBright = FillListBox(hDB, hLibInst,
                        SF_LB_RIGHT, gModNm, gRPortNm);
  
                        /*  Invert text in pushbutton.
                        */
                        if (LoadString(hLibInst, SF_NOPORT, buf, sizeof(buf)))
                            SetDlgItemText(hDB, SF_COPYPORT, (LPSTR) buf);
  
                        gDlgState |= SFDLG_RESFONTS;
  
                        /*  Change the dialog contols.
                        */
                        ShowWindow(GetDlgItem(hDB, SF_PERM_RIGHT),
                        SHOW_OPENWINDOW);
                        ShowWindow(GetDlgItem(hDB, SF_TEMP_RIGHT),
                        SHOW_OPENWINDOW);
                        EnableWindow(GetDlgItem(hDB, SF_PERM_RIGHT), FALSE);
                        EnableWindow(GetDlgItem(hDB, SF_TEMP_RIGHT), FALSE);
                        UpdateControls(hDB, gDlgState);
                    } /* if (GetPort(..)) */
                    else
                    {
                        // Didn't select a port, so restore the
                        // ADD FONTS pushbutton
                        ShowWindow(GetDlgItem(hDB, SF_ADD_RIGHT),
                        SHOW_OPENWINDOW);
                    }
  
  
                    KERPLUNK(hDB);
                }
  
                else
                {
                    DBGsfdlg((
                    "SFdlgFn(%d,%d,%d,%ld): SF_COPYPORT, bCopyPort = TRUE\n",
                    hDB, wMsg, wParam, lParam));
                    {
                        /* for SF_YESPORT */
                        char buf[64];
  
                        bCopyPort = FALSE;
  
                        if (gHLBright && (gDlgState & SFDLG_RESFONTS))
                        {
                            EndPort(hDB,hLibInst,gHLBright,gModNm,gRPortNm,FALSE);
                            gRPortNm[0] = '\0';
                            GlobalFree(gHLBright);
                            gHLBright = 0;
                        }
  
                        /*  Invert text in pushbutton.
                        */
                        if (LoadString(hLibInst, SF_YESPORT, buf, sizeof(buf)))
                            SetDlgItemText(hDB, SF_COPYPORT, (LPSTR) buf);
  
                        gDlgState &= ~(SFDLG_RESFONTS);
                        gDlgState &= ~(SFDLG_SELECTED);
  
                        /*  Change the dialog contols.
                        */
                        SetDlgItemText(hDB, SF_PRINTER_RIGHT, (LPSTR)"");
                        SendDlgItemMessage(hDB,SF_LB_RIGHT,LB_RESETCONTENT,0,0L);
                        CheckRadioButton(hDB, SF_PERM_RIGHT, SF_TEMP_RIGHT, 0);
                        EnableWindow(GetDlgItem(hDB, SF_LB_RIGHT), FALSE);
                        EnableWindow(GetDlgItem(hDB, SF_PERM_RIGHT), FALSE);
                        EnableWindow(GetDlgItem(hDB, SF_TEMP_RIGHT), FALSE);
                        ShowWindow(GetDlgItem(hDB, SF_PERM_RIGHT), HIDE_WINDOW);
                        ShowWindow(GetDlgItem(hDB, SF_TEMP_RIGHT), HIDE_WINDOW);
                        ShowWindow(GetDlgItem(hDB,SF_ADD_RIGHT),SHOW_OPENWINDOW);
                        UpdateControls(hDB, gDlgState);
                    }
                    KERPLUNK(hDB);
                }
                break;
  
  
  
            case SF_ABOUT:  // ABOUT pushbutton -- display About box.
  
  
                MyDialogBox(hLibInst,SFABOUT,hDB, GenericWndProc);
                KERPLUNK(hDB);
                break;
  
  
  
  
  
            case SF_HELP:   // HELP pushbutton -- run help.
                HelpWasCalled = WinHelp(hDB, (LPSTR) "finstall.hlp",
                (WORD) HELP_INDEX,
                (DWORD) 0L);
                break;
  
  
  
  
            case SF_EXIT:
                sysclose:
                DBGsfdlg(("SFdlgFn(%d,%d,%d,%ld): SF_EXIT\n",
                hDB, wMsg, wParam, lParam));
  
                // End Help if it was called.
                if (HelpWasCalled)
                    WinHelp(hDB, (LPSTR) "finstall.hlp",
                    (WORD) HELP_QUIT, (DWORD) NULL);
  
                if (gSF_FLAGS & SF_NOABORT)
                {
                    /*  The exit button has been changed to a cancel
                    *  button -- indicate it has been clicked and
                    *  continue without actually exiting the dialog.
                    *  Flush extra exit messages from the queue so
                    *  we won't exit right away, this also prevents
                    *  the right LB struct from getting freed in the
                    *  middle of AddFonts, where it is still locked.
                    */
                    MSG msg;
  
                    while (PeekMessage(&msg, hDB, NULL, NULL, TRUE))
                    {
                        /*  We have to process paint messages.
                        */
                        if (msg.message == WM_PAINT)
                            IsDialogMessage(hDB, &msg);
                    }
  
                    gSF_FLAGS &= ~(SF_NOABORT);
                    break;
                }
  
                /*  Place the installer dialog in a "null" state by
                *  unselecting any selected fonts and sending messages
                *  to their processing procs to clean up.  This seems
                *  to be the proper thing to do plus it fixes an obscure
                *  bug where the perm/temp buttons get some extraneous
                *  messages when we don't want them.
                */
                if (gDlgState & (SFDLG_SELECTED << 4))
                {
                    SendDlgItemMessage(hDB, SF_LB_LEFT, LB_SETSEL,
                    FALSE, (long)(-1));
                    gDlgState = UpdateStatusLine(hDB, wParam, gDlgState,
                    gHLBleft, FALSE);
                    UpdateControls(hDB, gDlgState);
                }
                else if (gDlgState & SFDLG_SELECTED)
                {
                    SendDlgItemMessage(hDB, SF_LB_RIGHT, LB_SETSEL,
                    FALSE, (long)(-1));
                    gDlgState = UpdateStatusLine(hDB, wParam, gDlgState,
                    gHLBright, FALSE);
                    UpdateControls(hDB, gDlgState);
                }
  
                if (gHLBleft && !EndPort(hDB,hLibInst,gHLBleft,gModNm,
                    gLPortNm,
                    (GetKeyState(VK_SHIFT) < 0 && GetKeyState(VK_CONTROL))))
                {
                    break;
                }
  
                if (gHLBright && (gDlgState & SFDLG_RESFONTS) &&
                    !EndPort(hDB,hLibInst,gHLBright,gModNm,gRPortNm,FALSE))
                {
                    break;
                }
  
                EndDialog(hDB, wParam);
  
                endSFdir(0L);
  
                if (gHLBleft)
                {
                    GlobalFree(gHLBleft);
                    gHLBleft = 0;
                }
                if (gHLBright)
                {
                    GlobalFree(gHLBright);
                    gHLBright = 0;
                }
                if (gSourceLibrary.hFiles)
                {
                    GlobalFree(gSourceLibrary.hFiles);
                    gSourceLibrary.hFiles = 0;
                }
                if (gInstalledLibrary.hFiles)
                {
                    GlobalFree(gInstalledLibrary.hFiles);
                    gInstalledLibrary.hFiles = 0;
                }
                break;
  
  
        }// casw WM_COMMAND
            break;


  
        case WM_SYSCOMMAND:
            DBGsfdlg(("SFdlgFn(%d,%d,%d,%ld): WM_SYSCOMMAND\n",
            hDB, wMsg, wParam, lParam));
  
        switch (wParam)
        {
            case SC_CLOSE:
                DBGsfdlg(("SFdlgFn(%d,%d,%d,%ld): SC_CLOSE\n",
                hDB, wMsg, wParam, lParam));
                goto sysclose;
                break;
  
            default:
                return FALSE;
  
        }
            break;  /* end of case WM_SYSCOMMAND */
  
        case WM_MEASUREITEM:
            // An owner-draw control is being created, and this message
            // is sent to obtain its dimensions.
            // So far, only the main dialog listboxes fit in this category.
            // lParam points to a MEASUREITEMSTRUCT containing this info.
  
            FillMeasureItem( (LPMEASUREITEMSTRUCT) lParam);
  
            break;
  
        case WM_DRAWITEM:
            // This message causes an owner-draw control to be drawn
            // or updated.
            // In this program, the control will be an item in one of
            // the two main listboxes.
            // lParam points to a DRAWITEMSTRUCT indicating what's to
            // be done, and what it's to be done to or with.
  
            if (!DrawItem(hDB,  (LPDRAWITEMSTRUCT) lParam))
                DefWindowProc(hDB, wMsg, wParam, lParam) ;
            break;
  
        default:
            return FALSE;
  
    }   /* switch (wMsg) */
  
    return TRUE;
}   // SFdlgFn()
  
// Local function
// If this is a basic laserJet, indicate that only cartridges can be installed,
// Otherwise, clear status line.
  
void NEAR PASCAL InitStatusLine(hDB)
HWND hDB;
{
    char buf[80];
  
    // init to blank status line
    buf[0] = '\0';
  
    // if this is a LaserJet (no soft fonts) tell the user.
    if (gNoSoftFonts)
        LoadString(hLibInst, SF_NOSOFT, buf, sizeof(buf));
  
    // display blank or warning message
    SetDlgItemText(hDB, SF_STATUS, (LPSTR) buf);
  
} // InitStatusLine()
  
/***************************************************************************
*  Added for IFW support:
*                         Function used to make a call to enumerate fonts
*                         after a library file is installed with the dirver.
****************************************************************************/
  
void UpdateBow(hWnd, hInst)
HWND     hWnd;
HANDLE   hInst;
{
    HDC      hDC;
    int      dbgval;
    FARPROC  lpEnumFunc;
  
    if((GetProfileInt((LPSTR)"Intellifont",(LPSTR)"FontSumUpdt",-1)) != 1)
    {
        DBGsfdlg(("inside if !getprofstr if\n"));
        WriteProfileString((LPSTR)"Intellifont",(LPSTR)"FontSumUpdt","1");
    }
  
    hDC = GetDC(hWnd);
    DBGsfdlg(("Value of the device context handle = %ld\n", hDC));
  
    DBGsfdlg(("Before call to EnumFunc\n"));
  
    lpEnumFunc = MakeProcInstance(EnumFunc,hInst);
    DBGsfdlg(("Value returned by call to MakeProcInstance.\n"));
    DBGsfdlg(( "   should be a pointer to EnumFunc = %ld\n", lpEnumFunc));
  
    dbgval = (EnumFonts(hDC, (LPSTR) NULL, lpEnumFunc, (LPSTR) NULL));
    DBGsfdlg(("Value returned by call to EnumFonts = %d\n", dbgval));
  
    FreeProcInstance(lpEnumFunc);
    ReleaseDC(hWnd, hDC);
    DBGsfdlg(("After call to EnumFunc\n"));
  
}
  
  
  
  
/****************************************************************************/
  
int FAR PASCAL EnumFunc(lplf, lptm, FontType, lpData)
LPLOGFONT      lplf;
LPTEXTMETRIC   lptm;
WORD           FontType;
LPSTR          lpData;
{
    DBGsfdlg(("EnumFunc: LOGFONT... = %ls\n", lplf->lfFaceName));
    return(1);
}
  
  
