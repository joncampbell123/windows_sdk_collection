/**[f******************************************************************
* options.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.
* Company confidential.
*
**f]*****************************************************************/

/*
* $Header:
*/

/* 
 * $Log:	options.c,v $
 * Revision 3.890  92/02/06  16:12:34  16:12:34  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.872  91/12/18  16:05:57  16:05:57  daniels (Susan Daniels)
 * Fix Bug #771: Add code to OptionsDlg to save and restore TTRaster if
 * user cancels out.
 * 
 * Revision 3.871  91/11/12  14:00:03  14:00:03  daniels (Susan Daniels)
 * Fix Bug #734:  title on Output Bin box should not be grayed out.
 * 
 * Revision 3.870  91/11/08  11:44:19  11:44:19  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.866  91/11/08  11:31:07  11:31:07  dtk (Doug Kaltenecker)
 * Added 300dpi dependancy to TTRASTER.
 * 
 * Revision 3.865  91/11/01  13:52:17  13:52:17  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:47:37  13:47:37  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:49:05  09:49:05  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  15:00:05  15:00:05  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:50:19  16:50:19  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:17:35  14:17:35  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:33:53  16:33:53  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:34:39  10:34:39  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:12:32  14:12:32  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.814  91/08/29  10:33:06  10:33:06  daniels (Susan Daniels)
 * Fix Bug 610: About button not working in Options dlg.
 * 
 * Revision 3.813  91/08/26  15:07:26  15:07:26  daniels (Susan Daniels)
 * Fix Bug 3607: x in HorizDuplex cut in half.  Use Long Edge and Short Edge
 * for all printers;  build into control def in resource file.
 * 
 * Revision 3.812  91/08/22  14:32:28  14:32:28  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.809  91/08/22  13:39:44  13:39:44  daniels (Susan Daniels)
 * Fix BUG #581: Orientation / binding icons no longer work due to change
 * in Win 31.
 * 
 * Revision 3.808  91/08/13  15:43:32  15:43:32  daniels (Susan Daniels)
 * BUG #558: Putting name of printer in About 
 */


/***************************************************************************/
/******************************   options.c   ******************************/
//
//  Procs for handling options dialog.
//
//  18 dec 91  SD   Bug #771:  Added code to restore old value of TTRaster if user
//                  cancels out.
//  12 nov 91  SD   Bug #734:  Title for Output Bin box should not be grayed out.
//  29 aug 91  SD   Bug #610:  Introduced when fixed Bug #558.  Need to use
//                  hLibInst for AboutDlg call.
//  26 aug 91  SD   Bug #607:  Remove conditional strings for duplex since all
//                  Series III printers use Long Edge / Short Edge and it is 
//                  now hardcoded in the .rc file.
//  21 aug 91  SD   Bug #581:  WIN31 Change code to put up icons due to MS change.  
//  13 aug 91  SD   BUG #558: Use AboutDlg for About dialog box.
//  24 jun 91  SD   Added AdvancedSetUpDialog() for standard print dialogs.
//  24 jun 91  SD   Added code to save old values for brighten and grayscale
//                  in case user cancels out.
//  24 jun 91  SD   Added Help button to Options Dialog box for WIN31.
//  22 jun 91  SD   Moved Gray Scale to Options dialog for Win 31 common 
//                  dialogs.  Added About button.
//  11 feb 91  SD       Added ELI support.
//
//  01 dec 89   peterbe Declarations in ifdef also.
//
//  30 nov 89   peterbe Visual edge calls in ifdef now.
//
//  07 jun 89   peterbe Changed duplex groupbox to have only one
//          icon showing at a time.
//
//  31 mar 89   peterbe Added code for determining whether landscape port.
//          icons are to appear.  Implemented icon
//          enable/disable code -- works now.
//
//  30 mar 89   peterbe Disable laser port control if there's no hardware..
//
//   2-22-89    jimmat  Device Mode Dialog box changes for Windows 3.0.
//
  
#include "build.h"
#include "nocrap.h"
#undef NOCTLMGR
#undef NOWINMESSAGES
#undef NOSHOWWINDOW
#include "windows.h"
#include "hppcl.h"
#include "resource.h"
#include "strings.h"
#include "dlgutils.h"
#include "debug.h"
  
// laser port enable function.  Determine if it exists.
  
#ifdef VISUALEDGE
int FAR PASCAL lp_enbl(void);
#endif
  
  
/*  DEBUG switches
*/
#define DBGdlgfn(msg)   /*    DBMSG(msg)   */
#define DBGTerry(msg)   /*    DBMSG(msg)   */

#define LOCAL static
  
LOCAL LPPCLDEVMODE glpDevmode;      /* we are not reentrant, so this is okay */

extern short FAR PASCAL AboutDlg(HANDLE, HWND, LPPCLDEVMODE);

/*  Forward references
*/
BOOL  FAR PASCAL OPdlgFn(HWND, unsigned, WORD, LONG);
short FAR PASCAL OptionsDlg(HANDLE, HWND, LPPCLDEVMODE);
LOCAL void   UpdateDuplex(HWND, LPPCLDEVMODE);
void NEAR PASCAL SetDuplexIcon(HWND, LPPCLDEVMODE);

#if defined(WIN31)  
extern HANDLE hLibInst;     /* Driver library Instance handle */
static BOOL HelpWasCalled = FALSE;
LPSTR		far PASCAL lmemcpy( LPSTR, LPSTR, WORD );
LOCAL BOOL  UpdateGrayScale (HWND, short, short);
#endif


/*  OptionsDlg
*/
short FAR PASCAL
OptionsDlg(HANDLE hMd, HWND hWndParent, LPPCLDEVMODE lpDevmode) {
  
    FARPROC lpDlgFunc;
    short response, OldOptions, OldDuplex;
    BYTE OldReartray;        /* ELI */
    BYTE OldOffset;          /* ELI */

#if defined(WIN31)
    BYTE OldBrighten;
    BYTE OldGrayscale;
    BYTE OldTTRaster;
#endif


#ifdef DEBUG_FUNCT
    DB(("Entering OptionsDlg\n"));
#endif
    DBGdlgfn(("OptionsDlg(%d,%d,%lp)\n",(WORD)hMd,(WORD)hWndParent,lpDevmode));
  
    glpDevmode = lpDevmode;
    OldOptions = lpDevmode->options;
    OldDuplex  = lpDevmode->dm.dmDuplex;
    OldReartray = lpDevmode->reartray;                 /* ELI */
    OldOffset = lpDevmode->offset;                 /* ELI */

#if defined(WIN31)
    OldBrighten = lpDevmode->brighten;
    OldGrayscale = lpDevmode->grayscale;
    OldTTRaster = lpDevmode->TTRaster;
#endif
    lpDlgFunc = MakeProcInstance(OPdlgFn, hMd);
    response = DialogBox(hMd, MAKEINTRESOURCE(OPTIONS), hWndParent, lpDlgFunc);
    FreeProcInstance(lpDlgFunc);
  
    /* restore old options if user canceled out */
  
    if (response == IDCANCEL) {
        lpDevmode->options = OldOptions;
        lpDevmode->dm.dmDuplex = OldDuplex;
        lpDevmode->reartray = OldReartray;                    /* ELI */
        lpDevmode->offset = OldOffset;                    /* ELI */

#if defined(WIN31)
        lpDevmode->brighten = OldBrighten;
        lpDevmode->grayscale = OldGrayscale;
        lpDevmode->TTRaster = OldTTRaster;
#endif
    }
  
    DBMSG(("...end, response=%d %ls\n", response, (response == IDOK) ?
    (LPSTR)"IDOK" : (LPSTR)"IDCANCEL"));
  
#ifdef DEBUG_FUNCT
    DB(("Exiting OptionsDlg\n"));
#endif
    return(response);
}
  
/*  OPdlgFn
*/
BOOL FAR PASCAL
OPdlgFn(HWND hDB, unsigned wMsg, WORD wParam, LONG lParam) {
  
#ifdef DEBUG_FUNCT
    DB(("Entering OPdlgFn\n"));
#endif
    switch (wMsg) {
  
        case WM_INITDIALOG:
  
            DBGdlgfn(("OPdlgFn(%d,%d,%d,%ld): WM_INITDIALOG\n",
            hDB, wMsg, wParam, lParam));
  
#ifdef VISUALEDGE
            // if there's a LaserPort installed (or Intel Visual Edge),
            if (lp_enbl())
                // display LaserPort button checked,
                CheckDlgButton(hDB, OPTN_IDDPTEK,
                (glpDevmode->options) & OPTIONS_DPTEKCARD);
            else
                // otherwise disable this control:
                EnableWindow(GetDlgItem(hDB, OPTN_IDDPTEK), FALSE);
#endif
  
            /* BEGIN ELI */
            DBGdlgfn(("glpDevmode->prtCaps2 = %d\n",glpDevmode->prtCaps2));
            DBGdlgfn(("LOWERBIN = %d;  LOWERBIN & prtCaps2 = %d\n",LOWERBIN,
            glpDevmode->prtCaps2 & LOWERBIN));
            if (glpDevmode->prtCaps2 & LOWERBIN)
            {
                DBGdlgfn(("Displaying active buttons\n"));
                /* display LOWERBINBOX and JOBSEPBOX.  If rear tray output
                is on, then the offset box is inactive.
                */
                CheckRadioButton(hDB, UPPERBINBOX, LOWERBINBOX,
                glpDevmode->reartray ? LOWERBINBOX : UPPERBINBOX);
                if (glpDevmode->reartray)
                    EnableWindow(GetDlgItem(hDB, JOBSEPBOX), FALSE);
                else
                    CheckDlgButton(hDB, JOBSEPBOX, glpDevmode->offset);
            }
            else
            {
                // otherwise disable these controls:
                DBGdlgfn(("Displaying inactive buttons\n"));
/*              EnableWindow(GetDlgItem(hDB, BINGROUPBOX), FALSE);  */ /*Bug #734 */
                EnableWindow(GetDlgItem(hDB, UPPERBINBOX), FALSE);
                EnableWindow(GetDlgItem(hDB, LOWERBINBOX), FALSE);
                CheckDlgButton(hDB, JOBSEPBOX, FALSE);
                EnableWindow(GetDlgItem(hDB, JOBSEPBOX), FALSE);
            }
            /* END ELI */
  
            UpdateDuplex(hDB,glpDevmode);

#if defined(WIN31)

            /* Put the gray scale options into the gray scale box. */
            UpdateGrayScale(hDB, glpDevmode->brighten, glpDevmode->grayscale);

            /* Check the state of the box for TT as graphics
             * added for TT as graphics 6-19 dtk
             */
            if (glpDevmode->prtResFac == SF300)
	            CheckDlgButton(hDB, TTRASTER, (WORD)glpDevmode->TTRaster);
            else
            {
                EnableWindow(GetDlgItem(hDB, TTRASTER), FALSE);
                glpDevmode->TTRaster = 0;
            }
#endif  
            CenterDlg(hDB);
            break;
  
  
        case WM_COMMAND:
  
        switch (wParam) {
  
#if defined(WIN31)
        case GRAYSCBOX:
        {
            short temp;

                /* Pick up selected gray scale method from combobox */
  
                temp = (short)SendDlgItemMessage(hDB, GRAYSCBOX,
                CB_GETCURSEL, 0, 0L);
  
                DBGTerry (("Reading dlgbox, you choose message # %d", temp));
  
                switch ((int)temp)
                {
                    case ((int)0x00):
                        glpDevmode->brighten = (BYTE) FALSE;
                        glpDevmode->grayscale = (BYTE) TRUE;
                        break;
  
                    case ((int)0x01):
                        glpDevmode->brighten = (BYTE) FALSE;
                        glpDevmode->grayscale = (BYTE) FALSE;
                        break;
  
                    case ((int)0x02):
                        glpDevmode->brighten = (BYTE) TRUE;
                        glpDevmode->grayscale = (BYTE) TRUE;
                        break;
  
                    default:
                        SendDlgItemMessage(hDB,GRAYSCBOX,CB_SETCURSEL,0,0L);
                        glpDevmode->brighten = (BYTE) FALSE;
                        glpDevmode->grayscale = (BYTE) TRUE;
                        break;
  
            }
            DBGTerry(("Your choice is updated as brighten = %d, grayscale = %d",
                        glpDevmode->brighten, glpDevmode->grayscale));
            break;
        } /* end case GRAYSCBOX */
#endif /*if defined(WIN31) */

            case NODUPLEX:
            case VDUPLEX:
            case HDUPLEX:
  
                CheckRadioButton(hDB, NODUPLEX, HDUPLEX, wParam);
                glpDevmode->dm.dmDuplex = (wParam == NODUPLEX) ?
                DMDUP_SIMPLEX : ((wParam == VDUPLEX) ?
                DMDUP_VERTICAL : DMDUP_HORIZONTAL);
                SetDuplexIcon(hDB, glpDevmode);
                break;
                /* BEGIN ELI */
            case LOWERBINBOX:
            {
                CheckRadioButton(hDB, UPPERBINBOX, LOWERBINBOX, LOWERBINBOX);
                glpDevmode->reartray = TRUE;
                CheckDlgButton(hDB, JOBSEPBOX, FALSE);
                EnableWindow(GetDlgItem(hDB, JOBSEPBOX), FALSE);
                DBGdlgfn(("RearTray is now %d\n",glpDevmode->reartray));
                break;
            }
            case UPPERBINBOX:
            {
                CheckRadioButton(hDB, UPPERBINBOX, LOWERBINBOX, UPPERBINBOX);
                glpDevmode->reartray = FALSE;
                CheckDlgButton(hDB, JOBSEPBOX, glpDevmode->offset);
                EnableWindow(GetDlgItem(hDB, JOBSEPBOX), TRUE);
                DBGdlgfn(("RearTray is now %d\n",glpDevmode->reartray));
                break;
            }
  
  
#ifdef WIN31

            /* added for TT as graphics 6-19 dtk
             */
            case TTRASTER:

                glpDevmode-> TTRaster = ! glpDevmode->TTRaster;
                break;

#endif

            case JOBSEPBOX:
            {
                glpDevmode->offset = (BYTE) IsDlgButtonChecked(hDB, JOBSEPBOX);
                DBGdlgfn(("Offset is now %d\n",glpDevmode->offset));
                break;
            }
                /* END ELI */
  
#ifdef VISUALEDGE
            case OPTN_IDDPTEK:
            {
                short flag = wParam - OPTN_DLG_BASE;
  
                DBGdlgfn(("OPdlgFn(%d,%d,%d,%ld): OPTION %d\n",
                hDB, wMsg, wParam, lParam, flag));
  
                glpDevmode->options ^= flag;
  
                CheckDlgButton(hDB, wParam, (glpDevmode->options & flag));
            }
                break;
#endif

#if defined(WIN31)
            case IDABOUT:
            {
                AboutDlg(hLibInst, hDB, glpDevmode);  /* BUG #558 */
            }
            /* BUG #516:  Change focus from PRTBOX to ID_ABOUT 
             * This change is OK for both WIN31 and WIN30.
             */
                SetFocus(GetDlgItem(hDB, IDABOUT));
                break;

            case IDHELP:
                // We must call WinHelp(.... HELP_QUIT...) when
                // we exit from the dialog now..
                HelpWasCalled = WinHelp(hDB, (LPSTR) "hppcl5op.hlp",
                (WORD) HELP_INDEX,
                (DWORD) 0L);
                break;
#endif
  
            case IDOK:
            case IDCANCEL:
#if defined(WIN31)
                if (HelpWasCalled)
                    WinHelp(hDB, (LPSTR) "hppcl5a.hlp",
                    (WORD) HELP_QUIT,
                    (DWORD) NULL);
#endif
                EndDialog(hDB, wParam);
                break;

 
            default:
                return FALSE;
        }
            break;
  
        default:
            return FALSE;
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting OPdlgFn\n"));
#endif
    return TRUE;
}
  
/*  UpdateDuplex
*/
LOCAL void
UpdateDuplex(HWND hDB, LPPCLDEVMODE lpDevmode) {
  
    extern HANDLE hLibInst;
  
#ifdef DEBUG_FUNCT
    DB(("Entering UpdateDuplex\n"));
#endif
    if (!(lpDevmode->prtCaps & ANYDUPLEX)) {
  
        // This case will occur if there's a laserport installed
        // the printer doesn't handle duplex.
  
        CheckRadioButton(hDB,NODUPLEX,HDUPLEX,0);
        EnableWindow(GetDlgItem(hDB,NODUPLEX),FALSE);
        EnableWindow(GetDlgItem(hDB,VDUPLEX),FALSE);
        EnableWindow(GetDlgItem(hDB,HDUPLEX),FALSE);
  
        lpDevmode->dm.dmDuplex = 0;
  
    } else {
  
        // Show the icons.. which depend on whether we're in
        // landscape or portrait mode, and what the duplex mode is.
  
        SetDuplexIcon(hDB, lpDevmode);
  
        // check the proper radio button.
  
        CheckRadioButton(hDB,NODUPLEX,HDUPLEX,
        (lpDevmode->dm.dmDuplex == DMDUP_SIMPLEX) ?
        NODUPLEX : ((lpDevmode->dm.dmDuplex == DMDUP_VERTICAL) ?
        VDUPLEX : HDUPLEX));
    }
#ifdef DEBUG_FUNCT
    DB(("Exiting UpdateDuplex\n"));
#endif
}    // UpdateDuplex()
  
void NEAR PASCAL SetDuplexIcon(HWND hDB, LPPCLDEVMODE lpDevmode)
{
    LPSTR lpIconName;
    extern HANDLE hLibInst;
#ifdef DEBUG_FUNCT
    DB(("Entering SetDuplexIcon\n"));
#endif
    // first, get the name of the icon
    switch(lpDevmode->dm.dmDuplex)
    {
        case DMDUP_SIMPLEX:
  
            lpIconName =
            (lpDevmode->dm.dmOrientation == DMORIENT_PORTRAIT) ?
            (LPSTR) "ICO_NONEPORT" : (LPSTR) "ICO_NONELAND";
            break;
  
        case DMDUP_HORIZONTAL:
  
            lpIconName =
            (lpDevmode->dm.dmOrientation == DMORIENT_PORTRAIT) ?
            (LPSTR) "ICO_HORPORT" : (LPSTR) "ICO_HORLAND";
            break;
  
        case DMDUP_VERTICAL:
        default:
  
            lpIconName =
            (lpDevmode->dm.dmOrientation == DMORIENT_PORTRAIT) ?
            (LPSTR) "ICO_VERTPORT" : (LPSTR) "ICO_VERTLAND";
            break;
    }
  
    // Now, load the icon and display it
    /*  Bug #581:  Win31 needs to use SendDlgItemMessage() to change icons. */
#if defined (WIN31)
    {
        WORD wVers;
        wVers = GetVersion();
        if (LOBYTE(wVers) > 0x03 || LOBYTE(wVers) == 0x03 && HIBYTE(wVers) > 0x00)
            SendDlgItemMessage(hDB, OPT_ICON, STM_SETICON,
                               (WORD)LoadIcon(hLibInst, lpIconName),
                               0L);
        else
            SetDlgItemText(hDB, OPT_ICON,
                           MAKEINTRESOURCE(LoadIcon(hLibInst, lpIconName)));
    }
#else
    SetDlgItemText(hDB, OPT_ICON,
                   MAKEINTRESOURCE(LoadIcon(hLibInst, lpIconName)));
#endif

#ifdef DEBUG_FUNCT
    DB(("Exiting SetDuplexIcon\n"));
#endif
}   // SetDuplexIcon()

#if defined(WIN31)  
/***********************************************************************
U P D A T E  G R A Y  S C A L E
***********************************************************************/
  
/*  Update the Gray Scale combobox.
*/
  
LOCAL BOOL
UpdateGrayScale(HWND hDB, short brighten, short grayscale) {
  
    short i, strid, selectInd;
    char buf[25];
#ifdef DEBUG_FUNCT
    DB(("Entering UpdateGrayScale\n"));
#endif
  
    /* Erase contents of combobox. */
  
    SendDlgItemMessage(hDB, GRAYSCBOX, CB_RESETCONTENT, 0, 0L);
  
  
    for (i = FSUM_MSGLAST; i <= IDS_LAST_ENTRY; i++) {
  
        switch (i) {
            case IDS_SMOOTH:
                strid = IDS_SMOOTH;
                break;
  
            case IDS_DETAIL:
                strid = IDS_DETAIL;
                break;
  
            case IDS_SCANNED:
                strid = IDS_SCANNED;
                break;
  
            default:
                strid = 0;
                break;
  
  
        } // ends switch statement
  
        if (strid) {
            LoadString(hLibInst,strid,buf,sizeof(buf));
            SendDlgItemMessage(hDB, GRAYSCBOX, CB_INSERTSTRING,(WORD)-1,
            (LONG)(LPSTR)buf);
        }
  
    } // ends message for loop
  
  
    if (brighten)
        selectInd = (short)0x02;        //IDS_SCANNED
    else
        if (grayscale)
            selectInd = (short)0x00;     //IDS_SMOOTH
        else
            selectInd = (short)0x01;     //IDS_DETAIL
  
    SendDlgItemMessage(hDB, GRAYSCBOX, CB_SETCURSEL, selectInd, 0L);
  
  
  
  
#ifdef DEBUG_FUNCT
    DB(("Exiting UpdateGrayScale\n"));
#endif
    return TRUE;
} // ends function UpdateGrayScale
#endif  
  
  
