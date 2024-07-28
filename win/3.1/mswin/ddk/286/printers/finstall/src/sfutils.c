/**[f******************************************************************
* sfutils.c -
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

//*******************************   sfutils.c   ****************************
//
//  SFUtils:  Soft Font Dialog utilities
//
// Contains:
//  InitLBStrings()
//  FillListBox()
//  UpDateStatus()
//  UpDatePermTemp()
//  UpdateControls()
//  resetLB()
//  buildDescrStr()
//  NoDlgFn()
//
//  09 Sep 91   RLK(HP) Changed comparison of OpenFile in NoDLdlgFn
//  09 Sep 91   RLK(HP) Changed comparison of OpenFile in ExtractFaceOrient
//  21 aug 91   RLK(HP) Changed length of gPortNm from 32 to 128
//  20 aug 91   RLK(HP) Changed length of gPort[] and gLand[] from 16 to 
//  20 aug 91   RLK(HP) Changed length of gPort[] and gLand[] from 16 to 
//              SF_STATUS_WORDLEN. SF_STATUS_WORDLEN is set to 32 in sfutils.h
//              to give localizers some room. Changed InitLBstrings to also
//              initialize new globals: gCartridge, and gScalable.
//  14 aug 91   RLK(HP) Modified FillListBox to check if lpModNm is "HPPCL5"
//              not "HPPCL5A". Added external reference to lstrncpy and local
//              variable driver.
//  23 jun 91   RLK(HP) Modified FillListBox to display the drivername to
//              the correct list box. Was displaying driver name always to
//              left list box and then updating with driver and port name
//              in right list box when Copy Fonts to New Port.
//  09 oct 89   peterbe Refining cursor handling during init of listbox.
//          Adding Wait message, too.
//  28 sep 89   peterbe Modify FillListBox() so window isn't repainted 'till
//          all the files are scanned.
//  03 aug 89   peterbe Modify FillListBox() so only cartridge fonts are
//          displayed if (gNoSoftFonts != FALSE)
//  17 jul 89   peterbe Displays different listbox headings (SF_LDRIVERNM,
//          SF_LDESKJET) depending on printer class.
//  28 mar 89   peterbe Extra space between '*' etc. and string in
//          buildDescrString().
//  27 mar 89   peterbe Space indicates (temporary) soft font again.
//  25 mar 89   peterbe Created ShowEmpty() to display "_No soft fonts ...".
//  24 mar 89   peterbe Modify buildDescrString to change characters indicating
//          permanent soft fonts, temp. soft fonts, and cartridge
//          fonts.
//
//   1-26-89    jimmat  Adjustments do to resource file changes.
//   1-27-89    jimmat  Moved some common strings to lclstr.c
//   2-20-89    jimmat  Font Installer/Driver use same WIN.INI section (again)!
//
//**************************************************************************
  
//#define DEBUG
  
#include "nocrap.h"
#undef NOMEMMGR
#undef NOCTLMGR
#undef NOWINMESSAGES
#undef NOOPENFILE
#undef NOGDI
#undef NOMB
#undef NOSCROLL
#include "windows.h"
#include "fntutils.h"
#include "neededh.h"
#include "resource.h"
#include "dlgutils.h"
#include "sfinstal.h"
#include "strings.h"
#include "sfdir.h"
#include "sflb.h"
#include "sfutils.h"
#include "pfm.h"
#include "lclstr.h"
#include "deskjet.h"
  
  
/* Added from sfi */
  
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
    #define DBGextractfo(msg)          /*DBMSG(msg)*/
    #define DBGfillbox(msg)            /*DBMSG(msg)*/
    #define DBGcart(msg)            /*DBMSG(msg)*/
    #define DBGinsertfile(msg)         /*DBMSG(msg)*/
    #define DBGbuildDS(msg)            /*DBMSG(msg)*/
#else
    #define DBGextractfo(msg)          /*nil*/
    #define DBGfillbox(msg)            /*nil*/
    #define DBGcart(msg)            /*nil*/
    #define DBGinsertfile(msg)         /*nil*/
    #define DBGbuildDS(msg)            /*nil*/
#endif
  
  
  
#define LPDESC_IDLEN    3
#define LOCAL  static
  
  
/*  Temporary structure used by FillListBox() to collect and
*  modify information.
*/
typedef struct {
    char   appName[64];     /* Application name for win.ini */
    char   drv[64];     /* String for "Driver on " */
    char   point[32];       /* String for point size */
    char   bold[32];        /* String for bold */
    char   italic[32];      /* String for italic */
    char   ind[2];          /* Space holder for SFDIRSTRNG entry */
    char   file[256];       /* PFM and DL file names */
    SFDIRFILE SFfile;       /* SFDIRFILE struct */
    char   s[256];          /* Buffer for strings at end of SFDIRFILE */
    char   buf[256];        /* General work buffer */
} FILL_LB_TEMP;
typedef FILL_LB_TEMP FAR *LPFILL_LB_TEMP;
  
extern int gPrintClass;     // passed to InstallSoftFont()
extern BOOL gNoSoftFonts;   // Indicates only cartridge fonts
extern HANDLE hLibInst;
  
  
// added for config bullet
  
  
char gIndexFile[MAX_FILENAME_SIZE];
char gSupportFileDirectory[MAX_FILENAME_SIZE];
char gTypeDirectory[MAX_FILENAME_SIZE];
char gSourcePath[MAX_FILENAME_SIZE];
//char gPortNm[32];
char gPortNm[128];
char gModNm[32];
char gAppName[sizeof(gModNm)+sizeof(gPortNm)];
BOOL gBowinstalled;
  
WORD gNumInstalledFonts;
  
DIRECTORY gSourceLibrary;
DIRECTORY gInstalledLibrary;
DIRECTORY gScreenLibrary;
  
/* Global Proc */  
LPSTR FAR PASCAL lstrncpy(LPSTR, LPSTR, int);
  
  
// Forward declarations
BOOL FAR PASCAL NoDLdlgFn(HWND, unsigned, WORD, LONG);
LOCAL BOOL extractFaceOrient(HANDLE, LPSTR, LPSFDIRFILE,FILL_LB_TEMP FAR *,int);
LOCAL void insertFile(LPSTR, int FAR *, WORD FAR *, LPSTR, int);
LOCAL void ShowEmpty(HWND, HANDLE, WORD, LPSTR, int);
LOCAL void showWaitMsg(HWND);
LOCAL void appendDot(HWND);
  
// local data
LOCAL HANDLE gHMd = 0;
LOCAL char gBuf[128];
LOCAL char gIDstr[12];
//LOCAL char gPort[16];
//LOCAL char gLand[16];
LOCAL char gPort[SF_STATUS_WORDLEN];  /* to give localizers a chance */
LOCAL char gLand[SF_STATUS_WORDLEN];
LOCAL char gCartridge[SF_STATUS_WORDLEN];
LOCAL char gScalable[SF_STATUS_WORDLEN];
  
/**************************************************************************/
/****************************   Global Procs   ****************************/
  
/*  InitLBstrings
*/
void FAR PASCAL InitLBstrings(hMd, lpIDstr, lpPort, lpLand)
HANDLE hMd;
LPSTR lpIDstr;
LPSTR lpPort;
LPSTR lpLand;
{
    if (lpIDstr)
    {
        lmemcpy(gIDstr, lpIDstr, sizeof(gIDstr));
        gIDstr[sizeof(gIDstr)-1] = '\0';
    }
    else
        LoadString(hMd, SF_IDSTR, gIDstr, sizeof(gIDstr));
  
    if (lpPort)
    {
        lmemcpy(gPort, lpPort, sizeof(gPort));
        gPort[sizeof(gPort)-1] = '\0';
    }
    else
        LoadString(hMd, SF_PORT, gPort, sizeof(gPort));
  
    if (lpLand)
    {
        lmemcpy(gLand, lpLand, sizeof(gLand));
        gLand[sizeof(gLand)-1] = '\0';
    }
    else
        LoadString(hMd, SF_LAND, gLand, sizeof(gLand));

    /* Added for localization */
    LoadString(hMd, SF_SCALABLETF, gScalable, sizeof(gScalable));
    LoadString(hMd, SF_CARTRIDGEF, gCartridge, sizeof(gCartridge));

}
  
/****************************************************************************
  
===========================  FillListBox  ==============================
  
****************************************************************************/
  
  
HANDLE FAR PASCAL FillListBox(hDB, hMd, idLB, lpModNm, lpPortNm)
HWND hDB;
HANDLE hMd;
WORD idLB;
LPSTR lpModNm;
LPSTR lpPortNm;
{
    LPSFDIRFILE lpSFfile;
    LPFILL_LB_TEMP lpTemp = 0L;
    HANDLE hTemp = 0;
    HANDLE hWinSF = 0;
    HANDLE hSFlb = 0;
    LPSTR lpAppNm;
    LPSTR lpPFM;
    LPSTR lpDL;
    LPSTR top;
    LPSTR s;
    WORD state;
    int len, blen, slen;
    int id = 0;
    int ind = -1;
    int nTitle;
    int nSoftFonts;
    char buff[80];
    char driver[7];
  
    DBGfillbox(("FillListBox(%d,%d,%lp): %ls\n",
    hDB, hMd, lpPortNm, lpPortNm));
  
    // select string index for listbox title
    switch (gPrintClass)
    {
        case CLASS_DESKJET:
            nTitle = (idLB == SF_LB_LEFT) ? SF_LDESKJET : SF_RDESKJET;
            break;

        case CLASS_DESKJET_PLUS:
            nTitle = (idLB == SF_LB_LEFT) ? SF_LDESKJETPLUS : SF_RDESKJETPLUS;
            break;

        case CLASS_PAINTJET:
             nTitle = (idLB == SF_LB_LEFT) ? SF_LPAINTJET : SF_RPAINTJET;
            break;

        case CLASS_LSRJETIII:
             nTitle = (idLB == SF_LB_LEFT) ? SF_LLSRJETIII : SF_RLSRJETIII;
            break;

        case CLASS_LASERJET:
        default:
             nTitle = (idLB == SF_LB_LEFT) ? SF_LLASERJET : SF_RLASERJET;
            break;
    }
  

    // was always updating left list box title
//    RLK & DTK - TOOK OUT FOR NOW, SEEMS TO HAVE NO PURPOSE ??? - 9-6-91
//    if (idLB == SF_LB_LEFT)
//        {
//             LoadString (hMd, SF_LDRIVERNM, (LPSTR) buff, sizeof(buff));
//             SetDlgItemText(hDB, SF_PRINTER_LEFT, (LPSTR) buff);
//        }
//        else
//        {
//             LoadString (hMd, SF_RDRIVERNM, (LPSTR) buff, sizeof(buff));
//             SetDlgItemText(hDB, SF_PRINTER_RIGHT, (LPSTR) buff);
//        }

  
    if (lpPortNm &&
        (hTemp = GlobalAlloc(GMEM_FIXED, (DWORD)sizeof(FILL_LB_TEMP))) &&
        (lpTemp = (FILL_LB_TEMP FAR *)GlobalLock(hTemp)) &&
        LoadString(hMd, nTitle, lpTemp->drv, sizeof(lpTemp->drv)) &&
        LoadString(hMd, SF_POINT, lpTemp->point, sizeof(lpTemp->point)) &&
        LoadString(hMd, SF_BOLD, lpTemp->bold, sizeof(lpTemp->bold)) &&
        LoadString(hMd, SF_ITALIC, lpTemp->italic, sizeof(lpTemp->italic)))
    {
        lpAppNm = lpTemp->appName;
        lpPFM = lpTemp->file;
        lpSFfile = &lpTemp->SFfile;
        slen = sizeof(lpTemp->s);
        lpDL = 0L;
  
        /* Build "[<driver>,<port>]" for accessing win.ini file.*/
  
        MakeAppName(lpModNm,lpPortNm,lpAppNm,sizeof(lpTemp->appName));
  
        /*    Set "Printer on port" string at top of listbox. */
  
        if (lstrlen(lpTemp->drv) + lstrlen(lpPortNm) < sizeof(lpTemp->drv))
            lstrcat(lpTemp->drv, lpPortNm);
  
        if (idLB == SF_LB_LEFT)
            SetDlgItemText(hDB, SF_PRINTER_LEFT, trimLBcaption(lpTemp->drv));
        else
            SetDlgItemText(hDB, SF_PRINTER_RIGHT, trimLBcaption(lpTemp->drv));
  
  
  
        // =================== Search for IFW fonts ========================
  
        /* Configure IFW environment
        */
        lstrncpy((LPSTR)driver, lpModNm, 6);  
        driver[6] = '\0';

        if(lstrcmpi(driver, (LPSTR)"HPPCL5") == 0)
        {
            if (ConfigBullet(hDB, hLibInst,
                (LPSTR) gAppName,
                (LPSTR) gIndexFile,
                (LPSTR) gTypeDirectory,
                (LPSTR) gSourcePath,
                (LPSTR) gSupportFileDirectory))
            {
                gBowinstalled = TRUE;
  
                if (LoadString (hLibInst, SF_INITWAIT, (LPSTR) buff, sizeof(buff)))
                    SetDlgItemText(hDB, SF_STATUS, (LPSTR) buff);
  
                hSFlb = (ReadInstalledPrinterFonts(hDB, idLB,
                (LPSTR)lpTemp, hSFlb, (WORD FAR *)&gNumInstalledFonts));
  
            }
            else
                gBowinstalled = FALSE;
        }
        else
            gBowinstalled = FALSE;
  
  
        // =================== End IFW Stuff ==============================
  
  
  
        // ===== search for soft fonts, Traversing win.ini entries. ======
  
        //  [ Don't look for soft fonts if (gNoSoftFonts) ]
  
        if(!gNoSoftFonts)
            if (hWinSF = InitWinSF(lpAppNm))
            {
                //  Change cursor to hour glass and disable redrawing of listbox.
                //
                SetCursor(LoadCursor(NULL,IDC_WAIT));
                SendMessage(GetDlgItem(hDB,idLB),WM_SETREDRAW,FALSE,0L);
  
                // Display message in status line.
                showWaitMsg(hDB);
                nSoftFonts = 0;
  
                //  For each soft font entry.
                while ((id = NextWinSF(hWinSF,lpPFM,sizeof(lpTemp->file))) > -1)
                {
                    /*  Pick up the download file name.
                    */
                    for (lpDL=lpPFM; *lpDL && *lpDL != ','; ++lpDL)
                        ;
                    if (*lpDL ==',')
                        *(lpDL++) = '\0';
  
                    if (!(*lpDL))
                    {
                        /*  No download file, the entry is a permanently
                        *  downloaded font.  Check to see if there is a
                        *  PFMfile=DLfile entry in the win.ini file.
                        */
                        *(++lpDL) = '\0';
                        GetProfileString(lpAppNm, lpPFM, lpDL, lpDL,
                        sizeof(lpTemp->file) - lstrlen(lpPFM) - 2);
                        state = SFLB_PERM;
                    }
                    else
                        state = 0;
  
                    /*  Make sure file names are all upper case.
                    */
                    AnsiUpper(lpPFM);
                    AnsiUpper(lpDL);
  
                    DBGfillbox(("SoftFont%d=%ls,%ls\n", id, lpPFM, lpDL));
  
                    /*  Fill in the SFDIRFILE file structure...
                    */
                    lmemset((LPSTR)lpSFfile, 0, sizeof(SFDIRFILE)+slen);
  
                    if (!extractFaceOrient(hMd, lpPFM, lpSFfile, lpTemp, slen+1))
                        continue;
  
                    insertFile(lpDL, &lpSFfile->indDLpath,
                    &lpSFfile->offsDLname, lpSFfile->s, slen+1);
                    insertFile(lpPFM, &lpSFfile->indPFMpath,
                    &lpSFfile->offsPFMname, lpSFfile->s, slen+1);
  
                    lpSFfile->indLOGdrv = -1;
                    lpSFfile->indScrnFnt = -1;
  
                    /*  Figure out the structure's length by stepping backward
                    *  through the buffer at the end of the struct and stopping
                    *  at the first non-null character.
                    */
                    for (top = &lpSFfile->s[0], s = &lpSFfile->s[slen];
                        (s > top) && !(*s); --s)
                        ;
                    s += 2;
                    len = s - (LPSTR)lpSFfile;
  
                    DBGcart(("lpSFfile = %ls\n",(LPSTR)lpSFfile));
  
  
                    /*  Add the structure to the soft font directory list.
                    */
                    if ((ind=addSFdirEntry(0L,(LPSTR)lpSFfile,SF_FILE,len)) > -1)
                    {
                        hSFlb = addSFlistbox(hDB, hSFlb, idLB, id, ind,
                        state, gBuf, sizeof(gBuf), 0L);
                    }
  
                    // Append a period every so many fonts to status line.
                    if (!(nSoftFonts++ % 10))
                        appendDot(hDB);
  
                } //  end .. For each soft font entry.
  
                // done searching for soft fonts
                EndWinSF(hWinSF);
  
                SetDlgItemText(hDB, SF_STATUS, (LPSTR)"");
  
            }   // end of search for soft fonts
  
        // ========== Search for cartridges ==================
  
        if (hWinSF = InitWinSF(lpAppNm))
        {
            DBGcart(("   ...Got hWinSF for carts\n"));
  
            //  Change cursor to hour glass and disable redrawing of listbox.
            //
            if (gNoSoftFonts)
            {
                SetCursor(LoadCursor(NULL,IDC_WAIT));
                SendMessage(GetDlgItem(hDB,idLB),WM_SETREDRAW,FALSE,0L);
            }
  
            while ((id = NextWinCart(hWinSF,lpPFM,sizeof(lpTemp->file)))>-1)
            {
                AnsiUpper(lpPFM);
                DBGcart(("   ...Cartridge%d=%ls\n",id,lpPFM));
  
                lmemset((LPSTR)lpSFfile, 0, sizeof(SFDIRFILE)+slen);
  
                /* extract the cartridge title */
                if (!GetCartName(lpPFM,&lpSFfile->s[0],slen))
                    continue;
  
                DBGcart(("   ...got valid cartname\n"));
  
                insertFile(lpPFM,&lpSFfile->indPFMpath, &lpSFfile->offsPFMname,
                lpSFfile->s,slen+1);
  
                lpSFfile->indLOGdrv = -1;
                lpSFfile->indScrnFnt = -1;
                lpSFfile->indDLpath = -1;
                lpSFfile->offsDLname = 0;
                lpSFfile->orient = 0;
                lpSFfile->fIsPCM = 1;
  
                /* find the structure's length (same stunt as above) */
                for (top =&lpSFfile->s[0], s = &lpSFfile->s[slen];
                    (s>top) && !(*s); --s)
                    ;
  
                s += 2;
  
                len = s - (LPSTR)lpSFfile;
  
                /* add the structure to the SF dir list */
                if ((ind=addSFdirEntry(0L,(LPSTR)lpSFfile,SF_FILE|SF_CART,
                    len)) > -1)
                {
                    hSFlb = addSFlistbox(hDB, hSFlb, idLB, -id, ind,
                    SFLB_PERM|SFLB_CART, gBuf, sizeof(gBuf), 0L);
  
            #ifdef DEBUG
                    if (!hSFlb)
                        DBGcart(("   ...couldn't addSFlistbox() for cart!\n"));
            #endif
                }
        #ifdef DEBUG
                else
                    DBGcart(("   ...couldn't addSFdirEntry() for cart!\n"));
        #endif
            }
  
            EndWinSF(hWinSF);
  
        }   // End of search for cartridges
  
        #ifdef DEBUG
        else
            DBGcart(("   ...Didn't get hWinSF for carts\n"));
        #endif
  
        if (hSFlb)
        {
            SendMessage(GetDlgItem(hDB,idLB), WM_VSCROLL, SB_TOP, 0L);
            EnableWindow(GetDlgItem(hDB,idLB), TRUE);
        }
        else
        {
            // disable window
            EnableWindow(GetDlgItem(hDB,idLB), FALSE);
  
            // display 'no fonts' string..
            ShowEmpty(hDB, hMd, idLB, lpTemp->buf,  sizeof(lpTemp->buf));
  
            //      if (LoadString(hMd, SF_NOFNT, lpTemp->buf, sizeof(lpTemp->buf)))
            //      {
            //      SendDlgItemMessage(hDB, idLB, LB_INSERTSTRING,
            //          (WORD)(-1), (long)(LPSTR)lpTemp->buf);
            //      }
        }
  
        // make stuff in listbox visible again, repaint listbox.
        SendMessage(GetDlgItem(hDB,idLB),WM_SETREDRAW,TRUE,0L);
        InvalidateRect(GetDlgItem(hDB,idLB),(LPRECT)0L,FALSE);
  
        /*  Restore cursor to pointer.
        */
        SetCursor(LoadCursor(NULL,IDC_ARROW));
  
    #ifdef DEBUG
        /*  if (hSFlb)
        {
        DBGdumpSFbuf(0L);
        }
        */
    #endif
  
    }   /* end of outmost if */
  
    if (lpTemp)
    {
        GlobalUnlock(hTemp);
        lpTemp = 0L;
    }
  
    if (hTemp)
    {
        GlobalFree(hTemp);
        hTemp = 0;
    }
  
    return (hSFlb);
  
}   // FillListBox()
  
  
/*************************************************************************/
/*  UpdateStatusLine
*
*  Update the status line at the bottom of the installer dialog.
*/
WORD FAR PASCAL UpdateStatusLine(hDB, idLB, dlgState, hSFlb, selectAll)
HWND hDB;
WORD idLB;
WORD dlgState;
HANDLE hSFlb;
BOOL selectAll;
{
    LPSFDIRFILE lpSFfile = 0L;
    LPSFLBENTRY sflb = 0L;
    LPSFLB lpSFlb = 0L;
    int selected;
    int numsel;
    int sfind;
    int ind;
  
    /*  Lock down the list of soft font entries for this listbox.
    */
    if (hSFlb && (lpSFlb = (LPSFLB)GlobalLock(hSFlb)))
    {
        /*  Highlight everything in the listbox if everything should
        *  be selected.
        */
        if (selectAll)
        {
            SendDlgItemMessage(hDB, idLB, LB_SETSEL, TRUE, (long)(-1));
        }
  
        /*  First kill the opposite side of the dialog (i.e., unselect
        *  anything selected.
        */
        if (idLB == SF_LB_RIGHT)
        {
            if (dlgState & (SFDLG_SELECTED << 4))
            {
                SendDlgItemMessage(hDB, SF_LB_LEFT, LB_SETSEL, FALSE,
                (long)(-1));
                CheckRadioButton(hDB, SF_PERM_LEFT, SF_TEMP_LEFT, 0);
                EnableWindow(GetDlgItem(hDB, SF_PERM_LEFT), FALSE);
                EnableWindow(GetDlgItem(hDB, SF_TEMP_LEFT), FALSE);
                dlgState &= ~(SFDLG_SELECTED << 4);
            }
            dlgState |= SFDLG_SELECTED;
        }
        else
        {
            if (dlgState & SFDLG_SELECTED)
            {
                SendDlgItemMessage(hDB, SF_LB_RIGHT, LB_SETSEL, FALSE,
                (long)(-1));
  
                if (dlgState & SFDLG_RESFONTS)
                {
                    CheckRadioButton(hDB, SF_PERM_RIGHT, SF_TEMP_RIGHT, 0);
                    EnableWindow(GetDlgItem(hDB, SF_PERM_RIGHT), FALSE);
                    EnableWindow(GetDlgItem(hDB, SF_TEMP_RIGHT), FALSE);
                }
                dlgState &= ~(SFDLG_SELECTED);
            }
            dlgState |= (SFDLG_SELECTED << 4);
        }
  
        dlgState &= ~(SFDLG_CARTRIDGE);
        dlgState &= ~(SFDLG_FAIS);
  
        /*  Traverse the list modifying our record of which items
        *  are selected, sfind records the newly selected item.
        */
        for (numsel=ind=0, selected=sfind=-1, sflb=&lpSFlb->sflb[0];
            ind < lpSFlb->free; ++ind, ++sflb)
        {
            if (SendDlgItemMessage(hDB, idLB, LB_GETSEL, ind, 0L) > 0)
            {
                if (!(sflb->state & SFLB_SEL))
                    sfind = ind;
  
                selected = ind;
                ++numsel;
  
                sflb->state |= SFLB_SEL;
  
                /* remember if there are any cartridges in this mess */
                if (sflb->state & SFLB_CART)
                    dlgState |= SFDLG_CARTRIDGE;
  
                /* ditto for scalable typefaces */
                if (sflb->state & SFLB_FAIS)
                    dlgState |= SFDLG_FAIS;
            }
            else
            {
                sflb->state &= ~(SFLB_SEL);
  
                if (lpSFlb->prevsel == ind)
                    lpSFlb->prevsel = -1;
            }
        }
  
        /*  All selected, don't show anything in status line unless if
        *  there is only one font (the next check takes care of that).
        */
        if (selectAll)
            sfind = -1;
  
        /*  If only one item was selected, highlight it even if
        *  it was previously selected.
        */
        if (sfind < 0 && numsel == 1)
            sfind = selected;
  
        /*  If something was selected, update the status line at the
        *  bottom of the dialog and correctly set the perm/temp
        *  radio buttons.
        */
        if ((sfind > -1) && (sflb = &lpSFlb->sflb[sfind]) &&
            (lpSFfile = (LPSFDIRFILE)lockSFdirEntry(0L,sflb->indSFfile)))
        {
            BOOL fAllowTempPerm = (((dlgState & SFDLG_FAIS) ||
            (dlgState & SFDLG_CARTRIDGE)) == 0);
  
            lpSFlb->prevsel = sfind;
  
            /*  Set status line.
            */
            buildDescStr(gBuf, sizeof(gBuf), FALSE, sflb, lpSFfile);
            SetDlgItemText(hDB, SF_STATUS, gBuf);
  
            /*  Change perm/temp radio buttons.
            */
            if (idLB == SF_LB_LEFT)
            {
                EnableWindow(GetDlgItem(hDB, SF_PERM_LEFT), fAllowTempPerm);
                EnableWindow(GetDlgItem(hDB, SF_TEMP_LEFT), fAllowTempPerm);
  
                CheckRadioButton(hDB, SF_PERM_LEFT, SF_TEMP_LEFT,
                (sflb->state & SFLB_PERM) ? SF_PERM_LEFT : SF_TEMP_LEFT);
            }
            else if (dlgState & SFDLG_RESFONTS)
            {
                EnableWindow(GetDlgItem(hDB, SF_PERM_RIGHT), fAllowTempPerm);
                EnableWindow(GetDlgItem(hDB, SF_TEMP_RIGHT), fAllowTempPerm);
  
                CheckRadioButton(hDB, SF_PERM_RIGHT, SF_TEMP_RIGHT,
                (sflb->state & SFLB_PERM) ? SF_PERM_RIGHT : SF_TEMP_RIGHT);
            }
  
            unlockSFdirEntry(sflb->indSFfile);
            lpSFfile = 0L;
        }
        else
        {
            /*  Nothing was selected, go to a null state.
            */
            lpSFlb->prevsel = -1;
  
            /*  Zap the selected bit in the dlg state if nothing
            *  is selected.
            */
            if (numsel == 0)
            {
                if (idLB == SF_LB_LEFT)
                    dlgState &= ~(SFDLG_SELECTED << 4);
                else
                    dlgState &= ~(SFDLG_SELECTED);
            }
  
            SetDlgItemText(hDB, SF_STATUS, (LPSTR)"");
  
            if (idLB == SF_LB_LEFT)
            {
                CheckRadioButton(hDB, SF_PERM_LEFT, SF_TEMP_LEFT, 0);
                EnableWindow(GetDlgItem(hDB, SF_PERM_LEFT), FALSE);
                EnableWindow(GetDlgItem(hDB, SF_TEMP_LEFT), FALSE);
            }
            else if (dlgState & SFDLG_RESFONTS)
            {
                CheckRadioButton(hDB, SF_PERM_RIGHT, SF_TEMP_RIGHT, 0);
                EnableWindow(GetDlgItem(hDB, SF_PERM_RIGHT), FALSE);
                EnableWindow(GetDlgItem(hDB, SF_TEMP_RIGHT), FALSE);
            }
        }
  
        GlobalUnlock(hSFlb);
        lpSFlb = 0L;
    }
  
    return (dlgState);
}   // UpdateStatusLine()
  
/*  UpdatePermTemp
*
*  Update the permanent/temporary status buttons.
*/
WORD FAR PASCAL UpdatePermTemp(hDB, hMd, idButton, dlgState, hSFlb)
HWND hDB;
HANDLE hMd;
WORD idButton;
WORD dlgState;
HANDLE hSFlb;
{
    LPSFDIRFILE lpSFfile = 0L;
    LPSFLBENTRY sflb = 0L;
    LPSFLB lpSFlb = 0L;
    WORD idLB = (idButton == SF_PERM_LEFT || idButton == SF_TEMP_LEFT) ?
    SF_LB_LEFT : SF_LB_RIGHT;
  
    /*  Lock down the list of soft font entries for this listbox.
    */
    if (hSFlb && (lpSFlb = (LPSFLB)GlobalLock(hSFlb)))
    {
        /*  If a listbox item is selected, then rebuild the status line
        *  and update the perm/temp radio buttons.
        */
        if ((lpSFlb->prevsel > -1) &&
            (sflb = &lpSFlb->sflb[lpSFlb->prevsel]) &&
            (lpSFfile = (LPSFDIRFILE)lockSFdirEntry(0L,sflb->indSFfile)))
        {
            /*  Whenever we pull up the NOSFDLFILE dialog for the first
            *  time, we get two extraneous messages to the main sfinstal
            *  dialog.  If the user cancels out of the NOSFDLFILE dialog,
            *  it ends up immediately popping back up again.  To work
            *  around this, send a special message to the sfinstal main
            *  dialog telling it to ignore any messages to the perm/temp
            *  button messages.  At the end of this proc, post another
            *  message to re-enable processing perm/temp button messages.
            */
            PostMessage(hDB, WM_COMMAND, SF_IGNORMESSAGES, (long)TRUE);
  
            /*  If this is the first time the user changed a font
            *  to permanent status, then put up a alert explaining
            *  the consequences of making fonts permanent.
            */
            if ((idButton == SF_PERM_LEFT || idButton == SF_PERM_RIGHT) &&
                !(gSF_FLAGS & SF_PERMALRT))
            {
                PermFontAlert(hDB, hMd);
                gSF_FLAGS |= SF_PERMALRT;
            }
  
            /*  Can't change a cartridge!!!!!!!
            */
            if (lpSFfile->fIsPCM)
                return dlgState;
  
            /*  Check to see if we are changing status from permanent
            *  to temporary on a font for which we do not know the
            *  name of the download file.  If this is the case, then
            *  we prompt the user for the name of the download file
            *  and add it to the SF dir entry.
            */
            if ((idButton == SF_TEMP_LEFT || idButton == SF_TEMP_RIGHT) &&
                !lpSFfile->offsDLname)
            {
                FARPROC lpDlgFunc;
                int response, k;
  
                gHMd = hMd;
  
                /*  Fold both the description of the font and the name
                *  of the PFM file into gBuf (so we don't have to make
                *  another damn global).
                */
                lmemset(gBuf, 0, sizeof(gBuf));
                buildDescStr(gBuf, sizeof(gBuf)-1, TRUE, sflb, lpSFfile);
                lstrcpy(gBuf, &gBuf[1]);
                k = lstrlen(gBuf) + 1;
  
                if (k < sizeof(gBuf) - 10)
                {
                    makeSFdirFileNm(0L, sflb->indSFfile, TRUE, &gBuf[k],
                    sizeof(gBuf)-k);
                }
  
                /*  Unlock directory entry just in case if it moves
                *  when we add the file.
                */
                unlockSFdirEntry(sflb->indSFfile);
  
                response = MyDialogBox(hMd,NOSFDLFILE,hDB,NoDLdlgFn);
  
                if ((response != IDOK) ||
                    !addSFdirFile(0L,sflb->indSFfile,FALSE,gBuf,sizeof(gBuf)))
                {
                    /*  Change status back to permanent download if the
                    *  user clicked on cancel or we could not add the file
                    *  name to the sf directory struct.
                    */
                    idButton = (idButton == SF_TEMP_LEFT) ?
                    SF_PERM_LEFT : SF_PERM_RIGHT;
                }
  
                /*  Lock down the directory entry again.
                */
                if (!(lpSFfile=(LPSFDIRFILE)lockSFdirEntry(0L,sflb->indSFfile)))
                {
                    GlobalUnlock(hSFlb);
                    PostMessage(hDB, WM_COMMAND, SF_IGNORMESSAGES, (long)FALSE);
                    return(dlgState);
                }
            }
  
            /*  Update our record of the status.
            */
            if (idButton == SF_PERM_LEFT || idButton == SF_PERM_RIGHT)
                sflb->state |= SFLB_PERM;
            else
                sflb->state &= ~(SFLB_PERM);
  
            /*  Change line inside of listbox.
            */
            buildDescStr(gBuf, sizeof(gBuf), TRUE, sflb, lpSFfile);
            SendMessage(GetDlgItem(hDB, idLB), WM_SETREDRAW, FALSE, 0L);
            SendDlgItemMessage(hDB, idLB, LB_DELETESTRING,
            (WORD)lpSFlb->prevsel, 0L);
  
            // Insert string in dialog box
            SendDlgItemMessage(hDB, idLB, LB_INSERTSTRING,
            (WORD)lpSFlb->prevsel, (long)(LPSTR)gBuf);
  
            SendDlgItemMessage(hDB, idLB, LB_SETSEL, TRUE,
            (long)lpSFlb->prevsel);
            SendMessage(GetDlgItem(hDB, idLB), WM_SETREDRAW, TRUE, 0L);
            InvalidateRect(GetDlgItem(hDB, idLB), (LPRECT)0L, FALSE);
  
            /*  Set status line.
            */
            buildDescStr(gBuf, sizeof(gBuf), FALSE, sflb, lpSFfile);
            SetDlgItemText(hDB, SF_STATUS, gBuf);
  
            /*  Change perm/temp radio buttons.
            */
            if (idButton == SF_PERM_LEFT || idButton == SF_TEMP_LEFT)
                CheckRadioButton(hDB, SF_PERM_LEFT, SF_TEMP_LEFT, idButton);
            else
                CheckRadioButton(hDB, SF_PERM_RIGHT, SF_TEMP_RIGHT, idButton);
  
            /*  Reenable processing of the perm/temp buttons.
            */
            PostMessage(hDB, WM_COMMAND, SF_IGNORMESSAGES, (long)FALSE);
  
            unlockSFdirEntry(sflb->indSFfile);
            lpSFfile = 0L;
        }
  
        GlobalUnlock(hSFlb);
        lpSFlb = 0L;
    }
  
    return (dlgState);
  
}   // UpdatePermTemp()
  
/***************************************************************************
UpdateControls
****************************************************************************/
  
void FAR PASCAL UpdateControls(hDB, dlgState)
HWND hDB;
WORD dlgState;
{
    BOOL fAllowSFOps = (((dlgState & SFDLG_FAIS) ||
    (dlgState & SFDLG_CARTRIDGE)) == 0);
  
    /*  Update the pointer.
    */
    if ((dlgState & (SFDLG_SELECTED << 4)) &&
        (dlgState & SFDLG_RESFONTS))
    {
        /*  Left side selected, in copy fonts across ports mode.
        */
        SetDlgItemText(hDB, SF_POINTER, (LPSTR)"===>");
    }
    else if (dlgState & SFDLG_SELECTED)
    {
        /*  Right side selected, any mode.
        */
        SetDlgItemText(hDB, SF_POINTER, (LPSTR)"<===");
    }
    else
    {
        /*  Nothing valid selected.
        */
        SetDlgItemText(hDB, SF_POINTER, (LPSTR)"");
    }
  
    /*  Update the controls down the center of the dialog.
    */
    if (dlgState & (SFDLG_SELECTED << 4))
    {
        /*  Left side selected.
        */
        if (dlgState & (SFDLG_RESFONTS << 4))
        {
            /*  Left side is showing resident fonts (the only valid
            *  state for the left side).
            */
            if (dlgState & SFDLG_RESFONTS)
            {
                /*  The right side is also showing resident fonts,
                *  allow copy and move.
                */
                EnableWindow(GetDlgItem(hDB, SF_MOVE), TRUE);
                EnableWindow(GetDlgItem(hDB, SF_COPY), TRUE);
            }
            else
            {
                /*  Right side not showing resident fonts.
                */
                EnableWindow(GetDlgItem(hDB, SF_MOVE), FALSE);
                EnableWindow(GetDlgItem(hDB, SF_COPY), FALSE);
            }
  
            /*  Always allow erase.
            */
            EnableWindow(GetDlgItem(hDB, SF_ERASE), TRUE);
  
            /*  Allow edit if that mode is enabled.
            */
            if (dlgState & (SFDLG_ALLOWEDIT << 4))
                EnableWindow(GetDlgItem(hDB, SF_EDIT), fAllowSFOps);
            else
                EnableWindow(GetDlgItem(hDB, SF_EDIT), FALSE);
        }
        else if (dlgState & (SFDLG_DSKFONTS << 4))
        {
            /*  This condition should never exist.
            */
            EnableWindow(GetDlgItem(hDB, SF_MOVE), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_COPY), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_ERASE), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_EDIT), FALSE);
        }
        else
        {
            /*  This should not happen, but we'll just disable
            *  all controls.
            */
            EnableWindow(GetDlgItem(hDB, SF_MOVE), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_COPY), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_ERASE), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_EDIT), FALSE);
        }
    }
    else if (dlgState & SFDLG_SELECTED)
    {
        /*  Right side selected.
        */
        if (dlgState & SFDLG_RESFONTS)
        {
            /*  Showing resident fonts.
            */
            EnableWindow(GetDlgItem(hDB, SF_MOVE), TRUE);
            EnableWindow(GetDlgItem(hDB, SF_COPY), TRUE);
            EnableWindow(GetDlgItem(hDB, SF_ERASE), TRUE);
  
            /*  Allow edit if that mode is enabled.
            */
            if (dlgState & SFDLG_ALLOWEDIT)
                EnableWindow(GetDlgItem(hDB, SF_EDIT), fAllowSFOps);
            else
                EnableWindow(GetDlgItem(hDB, SF_EDIT), FALSE);
        }
        else if (dlgState & SFDLG_DSKFONTS)
        {
            /*  Showing disk fonts, allow only move (which should
            *  have been changed to display "add").
            */
            EnableWindow(GetDlgItem(hDB, SF_MOVE), TRUE);
            EnableWindow(GetDlgItem(hDB, SF_COPY), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_ERASE), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_EDIT), FALSE);
        }
        else
        {
            /*  This should not happen, but we'll just disable
            *  all controls.
            */
            EnableWindow(GetDlgItem(hDB, SF_MOVE), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_COPY), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_ERASE), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_EDIT), FALSE);
        }
    }
    else
    {
        /*  Neither side is selected, gray all the controls.
        */
        EnableWindow(GetDlgItem(hDB, SF_MOVE), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_COPY), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_ERASE), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_EDIT), FALSE);
    }
}   // UpdateControls()
  
/*  resetLB
*
*  Zap the status line and perm/temp controls and reset the listbox
*  if it does not contain any more entries.  Return TRUE if the
*  listbox is empty.
*/
BOOL FAR PASCAL resetLB(hDB, hMd, idLB, hSFlb, count, idStatus, showMsg)
HWND hDB;
HANDLE hMd;
WORD idLB;
HANDLE hSFlb;
WORD count;
int idStatus;
BOOL showMsg;
{
    LPSFLB lpSFlb;
    BOOL isEmpty = FALSE;
    int k;
  
    /*  Clear and disable the perm/temp buttons.
    */
    if (idLB == SF_LB_LEFT)
    {
        CheckRadioButton(hDB, SF_PERM_LEFT, SF_TEMP_LEFT, 0);
        EnableWindow(GetDlgItem(hDB, SF_PERM_LEFT), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_TEMP_LEFT), FALSE);
    }
    else
    {
        CheckRadioButton(hDB, SF_PERM_RIGHT, SF_TEMP_RIGHT, 0);
        EnableWindow(GetDlgItem(hDB, SF_PERM_RIGHT), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_TEMP_RIGHT), FALSE);
    }
  
    /*  Set or clear the status line.
    */
    if (idStatus && (k=itoa(count,gBuf)) &&
        LoadString(hMd, idStatus, &gBuf[k], sizeof(gBuf)-k))
    {
        SetDlgItemText(hDB, SF_STATUS, gBuf);
    }
    else
    {
        SetDlgItemText(hDB, SF_STATUS, (LPSTR)"");
    }
  
    /*  Lock down the listbox list.
    */
    if (hSFlb && (lpSFlb=(LPSFLB)GlobalLock(hSFlb)))
    {
        /*  Disable the listbox if no fonts listed.
        */
        if (lpSFlb->free == 0)
        {
            isEmpty = TRUE;
  
            EnableWindow(GetDlgItem(hDB, idLB), FALSE);
  
            if (showMsg)
                // && LoadString(hMd, SF_NOFNT, gBuf, sizeof(gBuf)))
            {
                // display 'no fonts' string.
                ShowEmpty(hDB, hMd, idLB, gBuf, sizeof(gBuf));
                //SendDlgItemMessage(hDB, idLB, LB_INSERTSTRING,
                //    (WORD)(-1), (long)(LPSTR)gBuf);
            }
        }
  
        if (lpSFlb)
        {
            GlobalUnlock(hSFlb);
            lpSFlb = 0L;
        }
  
    }
  
    return (isEmpty);
} // resetLB()
  
/*  buildDescStr
*/
void FAR PASCAL buildDescStr(lpDesc, desclen, forLB, sflb, lpSFfile)
LPSTR lpDesc;
int desclen;
BOOL forLB;
LPSFLBENTRY sflb;
LPSFDIRFILE lpSFfile;
{
    LPSTR lpStr;
    int i, k;
  
    DBGbuildDS(("ENTER buildDescStr\n"));
  
  
    //  Put in an asterix to indicate a permanent soft font,
    //  an 'S' to indicate a soft font, and
    //  a copyright sign to indicat a cartridge font.
    if (sflb->state & SFLB_PERM)
        if (sflb->state & SFLB_CART)
            *lpDesc = 0xA9;     /* (c) ==> cartridge */
        else
            *lpDesc = '*';      // '*' indicates permanent soft font
        else
            if (sflb->state & SFLB_FAIS)
                *lpDesc = 0xB7;          // small bullet indicates FAIS
            else
                *lpDesc = 0x20;          // SPACE indicates soft font
  
    lpDesc[1] = 0x20;           // space after (c) or asterisk
    lpDesc[2] = '\0';
  
    /*  Add font description string.
    */
  
    DBGbuildDS(("lpSFfile->s in buildDescStr = %ls\n", (LPSTR)lpSFfile->s));
  
    if (lstrlen(lpSFfile->s) < desclen - 2)
        lstrcat(lpDesc, lpSFfile->s);
  
    /*  The rest of the string depends upon if it is targeted to be a
    *  listbox entry or for the long description string at the bottom
    *  of the dialog.  If it is for the listbox, we omit the font id
    *  number and indicate landscape fonts by displaying "landscape"
    *  after the name (fonts are assumed to be portrait unless specified).
    */
    if (sflb->state & SFLB_CART)
    {
        if (!forLB)
//          lstrcat(lpDesc, " [cartridge]");    // this should be localizable!!
            lstrcat(lpDesc, gCartridge); 
    }
    else if (sflb->state & SFLB_FAIS)
    {
        if (!forLB)
//            lstrcat(lpDesc, " [scalable typeface]");    // ditto!
            lstrcat(lpDesc, gScalable); 
    }
    else if (forLB)
    {
        /*  Add "landscape" if it is a landscape font.
        */
        if ((lpSFfile->orient == 2) &&
            (lstrlen(lpDesc) < desclen - lstrlen(gLand) - 2))
        {
            lstrcat(lpDesc, (LPSTR)" ");
            lstrcat(lpDesc, gLand);
        }
    }
    else
    {
        /*  Add open bracket for long description string.
        */
        if (lstrlen(lpDesc) < desclen - 3)
            lstrcat(lpDesc, (LPSTR)" [");
  
        /*  Add font id.
        */
        if (sflb->id > -1)
        {
            if (lstrlen(lpDesc) < desclen - lstrlen(gIDstr) - 5)
            {
                lstrcat(lpDesc, gIDstr);
                itoa(sflb->id, &lpDesc[lstrlen(lpDesc)]);
            }
        }
  
        /*  Add orientation.
        */
        if (lpSFfile->orient == 1 || lpSFfile->orient == 2)
        {
            if (lpSFfile->orient == 2)
                lpStr = (LPSTR)gLand;
            else
                lpStr = (LPSTR)gPort;
  
            /*  Add ", portrait" or "landscape" to the string.
            */
            if (lstrlen(lpDesc) < desclen - lstrlen(lpStr) - 3)
            {
                if (sflb->id > -1)
                    lstrcat(lpDesc, (LPSTR)CommaStr);
                lstrcat(lpDesc, lpStr);
            }
        }
        else if (lpSFfile->orient == 0)
        {
            /*  Add ", portrait, landscape" to the string.
            */
            if (lstrlen(lpDesc) < desclen -lstrlen(gPort) -lstrlen(gLand) -6)
            {
                if (sflb->id > -1)
                    lstrcat(lpDesc, (LPSTR)CommaStr);
                lstrcat(lpDesc, gPort);
                lstrcat(lpDesc, (LPSTR)CommaStr);
                lstrcat(lpDesc, gLand);
            }
        }
  
        /*  Add the name of the download file.
        */
        if (lpSFfile->offsDLname)
        {
            LPSTR lpDL = &lpSFfile->s[lpSFfile->offsDLname];
  
            if (lstrlen(lpDesc) < desclen - lstrlen(lpDL) - 3)
            {
                lstrcat(lpDesc, (LPSTR)CommaStr);
                lstrcat(lpDesc, lpDL);
            }
        }
  
        /*  Add close bracket.
        */
        if (lstrlen(lpDesc) < desclen - 2)
            lstrcat(lpDesc, (LPSTR)"]");
    }
}   // buildDescStr()
  
/*  NoDLdlgFn
*/
BOOL FAR PASCAL NoDLdlgFn(hDB, wMsg, wParam, lParam)
HWND hDB;
unsigned wMsg;
WORD wParam;
LONG lParam;
{
    switch (wMsg)
    {
        case WM_INITDIALOG:
        {
            /*  The font name and the PFM file name come in gBuf.
            */
            LPSTR s, t;
  
            CenterDlg(hDB);
  
            SetDlgItemText(hDB, SFNODL_FONT, gBuf);
  
            for (s=(LPSTR)gBuf; *s; ++s)
                ;
            if (*(++s))
            {
                SetDlgItemText(hDB, SFNODL_PFM, s);
  
                for (t=s; *t && *t != '.'; ++t)
                    ;
                if (*t == '.')
                {
                    t[1] = 'U';
                    t[2] = 'S';
                    t[3] = t[-1];
                    t[-1] = 'N';
                    SetDlgItemText(hDB, SFNODL_DL, s);
                }
            }
        }
            break;
  
        case WM_COMMAND:
        switch (wParam)
        {
            case SFNODL_FONT:
            case SFNODL_PFM:
            case SFNODL_DL:
                break;
  
            case IDOK:
            case IDCANCEL:
                GetDlgItemText(hDB, SFNODL_DL, gBuf, sizeof(gBuf));
  
                if (wParam == IDOK)
                {
                    OFSTRUCT ofstruct;
                    int hFile;
  
                    AnsiUpper(gBuf);
  
//                    if ((hFile=OpenFile(gBuf,&ofstruct,OF_EXIST)) > 0)
                    if ((hFile=OpenFile(gBuf,&ofstruct,OF_EXIST)) >= 0)                  //RK 09/04/91
                    {
                        _lclose(hFile);
                        EndDialog(hDB, wParam);
                    }
                    else
                    {
                        int k = lstrlen(gBuf);
  
                        if (!k)
                        {
                            LoadString(gHMd,SFNODL_FILE,gBuf,sizeof(gBuf));
                            k = lstrlen(gBuf);
                        }
  
                        if (LoadString(gHMd,SFNODL_BADFILE,&gBuf[k],
                            sizeof(gBuf)-k))
                        {
                            MessageBox(hDB, gBuf, (LPSTR)"",
                            MB_OK | MB_ICONEXCLAMATION);
                        }
                    }
                }
                else
                    EndDialog(hDB, wParam);
                break;
        }
            break;
  
        default:
            return FALSE;
    }
  
    return TRUE;
} //NoDLdlgFn()
  
/**************************************************************************/
/*****************************   Local Procs   ****************************/
  
// ShowEmpty()
// displays SF_NOFNT string "No soft fonts installed"
// with underscore prefix.
  
LOCAL void ShowEmpty(hDB, hMd, idLB, lpBuf, nBuf)
HWND hDB;               // dialog handle
HANDLE hMd;             // instance handle
WORD idLB;              // listbox control ID
LPSTR lpBuf;            // buffer for string
int nBuf;               // size of buffer
{
    // load string into buffer, starting at second byte:
    if (LoadString(hMd, SF_NOFNT, lpBuf+1, nBuf - 1))
    {
        // prefix an underscore
        lpBuf[0] = '_';
        // display the string
        SendDlgItemMessage(hDB, idLB, LB_INSERTSTRING,
        (WORD)(-1), (long)lpBuf);
    }
} // ShowEmpty()
  
/*  extractFaceOrient
*/
LOCAL BOOL extractFaceOrient(hMd, lpPFM, lpSFfile, lpTemp, slen)
HANDLE hMd;
LPSTR lpPFM;
LPSFDIRFILE lpSFfile;
FILL_LB_TEMP FAR *lpTemp;
int slen;
{
    LPPFMHEADER lpPFMhead = (LPPFMHEADER)lpTemp->buf;
    LPPFMEXTENSION lpPFMext =
    (LPPFMEXTENSION)((LPSTR)lpPFMhead+sizeof(PFMHEADER));
    LPEXTTEXTMETRIC lpExtMetric =
    (LPEXTTEXTMETRIC)((LPSTR)lpPFMext+sizeof(PFMEXTENSION));
    LPSTR s, end;
    BOOL success = FALSE;
    long seek;
    int hFile = -1;
    int err;
  
    DBGextractfo(("extractFaceOrient(%lp,%lp): %ls\n", lpPFM, lpSFfile, lpPFM));
  
    if ((hFile = _lopenp(lpPFM, OF_READ)) > 0)
    {
        err = _lread(hFile, (LPSTR)lpPFMhead, sizeof(PFMHEADER));
  
        if (err != sizeof(PFMHEADER))
        {
            DBGextractfo(("extractFaceOrient(): failed to read PFM header\n"));
            goto backout;
        }
  
        /*  Move file pointer to after width table in file.
        *  (for fixed pitch fonts the table does not exist)
        */
        seek = sizeof(PFMHEADER) - 2;
        if (lpPFMhead->dfPitchAndFamily & 0x1)
            seek += (lpPFMhead->dfLastChar - lpPFMhead->dfFirstChar + 2) * 2;
        _llseek(hFile, seek, 0);
        err = _lread(hFile, (LPSTR)lpPFMext, sizeof(PFMEXTENSION));
  
        /*  Pick up PFM extension -- a list of pointers.
        */
        if (err != sizeof(PFMEXTENSION))
        {
            DBGextractfo(
            ("extractFaceOrient(): failed to read PFM extension\n"));
            goto backout;
        }
  
        /*  Read the extended text metrics structure to get the orientation.
        */
        if (lpPFMext->dfExtMetricsOffset)
        {
            _llseek(hFile, lpPFMext->dfExtMetricsOffset, 0);
            err = _lread(hFile, (LPSTR)lpExtMetric, sizeof(EXTTEXTMETRIC));
  
            if (err != sizeof(EXTTEXTMETRIC))
            {
                DBGextractfo(
                ("extractFaceOrient(): failed to read ext text metrics\n"));
                goto backout;
            }
  
            lpSFfile->orient = (BYTE) lpExtMetric->emOrientation;
        }
        else
            lpSFfile->orient = 0;
  
        lpSFfile->s[0] = '\0';
  
        /*  Read the face name from the file.
        */
        if (lpPFMhead->dfFace)
        {
            _llseek(hFile, lpPFMhead->dfFace, 0);
  
            /*  Read one byte at a time into our own struct, stop
            *  when we encounter a NULL character.
            */
            for (s=&lpSFfile->s[0], end=&lpSFfile->s[slen-1];
                s < end && _lread(hFile, s, 1) == 1 && *s; ++s)
                ;
            *s = '\0';
        }
  
        if (lpSFfile->s[0] == '\0')
        {
            /*  No face name, use a generic "no name" string.
            */
            LoadString(hMd, SF_NODESCSTR, lpSFfile->s, slen);
            s = (LPSTR)lpSFfile->s + lstrlen(lpSFfile->s);
        }
  
        makeDesc((LPSTR)lpPFMhead, s, (int)(end - s),
        lpTemp->point, lpTemp->bold, lpTemp->italic);
  
        DBGextractfo(("extractFaceOrient(): orient=%d, face=%ls\n",
        lpSFfile->orient, (LPSTR)lpSFfile->s));
  
        success = TRUE;
  
    }
  
    backout:
    if (hFile > 0)
        _lclose(hFile);
  
    return (success);
}
  
/*  insertFile
*/
LOCAL void insertFile(lpFile, lpInd, lpOffs, lpBuf, slen)
LPSTR lpFile;
int FAR *lpInd;
WORD FAR *lpOffs;
LPSTR lpBuf;
int slen;
{
    LPSTR end;
    LPSTR s, t;
    char c;
    int len = lstrlen(lpFile);
  
    DBGinsertfile(("insertFile(%lp,%lp,%lp,%lp,%d): %ls\n",
    lpFile, lpInd, lpOffs, lpBuf, slen, lpFile));
  
    /*  Step backward through the file name stopping at the end
    *  of the path.
    */
    for (s = lpFile + len;
        (s > lpFile) && (s[-1] != ':') && (s[-1] != '\\'); --s)
        ;
  
    /*  If there is a path, insert it.
    */
    if (s > lpFile)
    {
        /*  Turn the end character into a null.
        */
        c = *s;
        *s = '\0';
        len = lstrlen(lpFile);
  
        DBGinsertfile(("insertFile(): path is %ls\n", lpFile));
  
        /*  Insert string path name, allow two bytes before the
        *  string for use by the SF directory utilities and
        *  one byte at the end for the null-terminator.
        */
        *lpInd = addSFdirEntry(0L, lpFile-2, SF_PATH, len+3);
  
        *s = c;
    }
    else
        *lpInd = -1;
  
    /*  Add the rest of the file name to the buffer at the end
    *  of the SFDIRFILE struct.
    */
    end = &lpBuf[slen-1];
    len = lstrlen(s);
  
    /*  Step backward to the end of the last inserted string.
    */
    for (t = end; (t > lpBuf) && !(*t); --t)
        ;
    if (t > lpBuf)
        t += 2;
  
    /*  Insert the string if there is room.
    */
    if (len > 0 && len < end - t)
    {
        lstrcpy(t, s);
        *lpOffs = t - lpBuf;
  
        DBGinsertfile(
        ("insertFile(): insert file %ls at %d\n", s, (int)*lpOffs));
    }
    else
        *lpOffs = 0;
}   // insertfile()
  
// show wait message while listbox is being filled with soft fonts
LOCAL void showWaitMsg(hDB)
HWND hDB;
{
    char buff[80];
  
    if (LoadString (hLibInst, SF_INITWAIT, (LPSTR) buff, sizeof(buff)))
        SetDlgItemText(hDB, SF_STATUS, (LPSTR) buff);
  
}   //showWaitMsg()
  
// Append a dot to the status line.
LOCAL void appendDot(hDB)
HWND hDB;
{
    char buff[80];
    int len;
  
    if (len = GetDlgItemText(hDB, SF_STATUS, (LPSTR)buff, sizeof(buff)))
        if (len < sizeof(buff) - 2);
        {
            lstrcat((LPSTR)buff, (LPSTR)".");
            SetDlgItemText(hDB, SF_STATUS, (LPSTR) buff);
        }
}
