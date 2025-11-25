// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993 - 1996  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE: printdlg.c
//
//  PURPOSE:
//    To show the use of the "Print" and "Print Setup" common dialog
//    boxes.
//
//  FUNCTIONS:
//    CmdFilePrint   - Present the print dialog to the user and process 
//                       the results.
//    CmdFilePrintSU -Present the print setup common dialog to the user.
//
//  COMMENTS:
//
//
//
//  SPECIAL INSTRUCTIONS: N/A
//

#include <windows.h>            // required for all Windows applications
#include "globals.h"            // prototypes specific to this application

static HANDLE hDevMode = NULL;
static HANDLE hDevNames = NULL;

//
//  FUNCTION: CmdFilePrint(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Present the print dialog to the user and process the results.
//
//  PARAMETERS:
//    hwnd     - The window handle.
//    wCommand - IDM_PRINT (Unused)
//    wNotify  - Notification number (unused)
//    hwndCtrl - NULL (Unused)
//
//  RETURN VALUE:
//    Always returns 0 - Command handled.
//
//  COMMENTS:
//
//

LRESULT CmdFilePrint(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    PRINTDLG pd = {0};

    static DWORD Flags     = PD_ALLPAGES | PD_SHOWHELP | PD_RETURNDC;
    static WORD  nFromPage = 0xFFFF;
    static WORD  nToPage   = 0xFFFF;
    static WORD  nCopies   = 1;

    pd.lStructSize = sizeof(pd);
    pd.hwndOwner   = hwnd;
    pd.hDevMode    = hDevMode;
    pd.hDevNames   = hDevNames;
    pd.Flags       = Flags;
    pd.nFromPage   = nFromPage;
    pd.nToPage     = nToPage;
    pd.nMinPage    = 0;
    pd.nMaxPage    = 0xFFFF;
    pd.nCopies     = nCopies;

    if (PrintDlg(&pd))
    {
        Print(hwnd,
              pd.hDC,
              MAKEBOOL(pd.Flags & PD_PAGENUMS),
              MAKEBOOL(pd.Flags & PD_SELECTION),
              MAKEBOOL(pd.Flags & PD_COLLATE),
              MAKEBOOL(pd.Flags & PD_PRINTTOFILE),
              pd.nFromPage,
              pd.nToPage,
              pd.nCopies,
              pd.hDevNames);
        hDevMode   = pd.hDevMode;
        hDevNames  = pd.hDevNames;
        Flags      = pd.Flags;
        nFromPage  = pd.nFromPage;
        nToPage    = pd.nToPage;
        nCopies    = pd.nCopies;
    }
//    else
//    {
//        CommDlgExtendedError();
//    }

    return 0;
}


//
//  FUNCTION: CmdPrintSU(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Present the print setup common dialog to the user.
//
//  PARAMETERS:
//    hwnd     - The window handle.
//    wCommand - IDM_PRINTSU (Unused)
//    wNotify   - Notification number (unused)
//    hwndCtrl - NULL (Unused)
//
//  RETURN VALUE:
//    Always returns 0 - Command handled.
//
//  COMMENTS:
//
//

LRESULT CmdFilePrintSU(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    PRINTDLG pd = {0};

    pd.lStructSize = sizeof(pd);
    pd.hwndOwner   = hwnd;
    pd.hDevMode    = hDevMode;
    pd.hDevNames   = hDevNames;
    pd.Flags = PD_SHOWHELP | PD_PRINTSETUP;

    if (PrintDlg(&pd))
    {
        hDevMode = pd.hDevMode;
        hDevNames = pd.hDevNames;
    }

    return 0;
}


//
//  FUNCTION: GetCurrentPrinterDC(void)
//
//  PURPOSE: Retrieve a DC for the current printer or default
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    Printer DC or NULL on error
//
//  COMMENTS:
//    If no printer has been selected yet, a DC for the default
//    printer is retrieved using PrintDlg.  Otherwise, a DC is
//    created from the existing hDevNames and hDevMode.
//

HDC GetCurrentPrinterDC(void)
{
    HDC hdc = NULL;

    if (hDevNames)
    {
        LPDEVNAMES lpdn = (LPDEVNAMES)GlobalLock(hDevNames);
        LPDEVMODE lpdm = NULL;

        if (hDevMode)
            lpdm = (LPDEVMODE)GlobalLock(hDevMode);

        hdc = CreateDC((LPSTR)lpdn + lpdn->wDriverOffset,
                       (LPSTR)lpdn + lpdn->wDeviceOffset,
                       (LPSTR)lpdn + lpdn->wOutputOffset,
                       lpdm);
    }
    else
    {
        PRINTDLG pd = {0};

        if (hDevMode)
            GlobalFree(hDevMode);

        pd.lStructSize = sizeof(pd);
        pd.hwndOwner   = NULL;
        pd.Flags = PD_RETURNDEFAULT | PD_RETURNDC;

        if (PrintDlg(&pd))
        {
            hDevMode = pd.hDevMode;
            hDevNames = pd.hDevNames;
            hdc = pd.hDC;
        }
    }

    return hdc;
}


//
//  FUNCTION: CmdPrintPreview(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Do WYSIWYG print preview for the current document
//
//  PARAMETERS:
//    hwnd     - The window handle.
//    wCommand - IDM_PRINTPREVIEW (Unused)
//    wNotify   - Notification number (unused)
//    hwndCtrl - NULL (Unused)
//
//  RETURN VALUE:
//    Always returns 0 - Command handled.
//
//  COMMENTS:
//
//

LRESULT CmdFilePrintPreview(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    HDC hdc;
    HWND hwndEdit = GetEditWindow(NULL);

    if (!hwndEdit)
        return 0;

    hdc = GetCurrentPrinterDC();

    if (hdc)
    {
        PrintPreview(hwnd, hwndEdit, hdc);
        DeleteDC(hdc);
    }
    else
        MessageBox(hwnd, "Unable to create printer DC.", NULL, MB_OK);

    return 0;
}
