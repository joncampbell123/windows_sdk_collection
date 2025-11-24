//***********************************************************************
//
// dialogs.c
//
// Contains all the dialog procedures for WinCap Windows Screen Capture
// Program.
//
// Dialog Functions:
//
// AboutDlgProc()         // About Box
// PrintDlgProc()         // Print Options Dialog
// SavingDlgProc()        // Dialog which displays "Saving to file..."
//
// Written by Microsoft Product Support Services, Developer Support.
// Copyright (C) 1991-1995 Microsoft Corporation. All rights reserved.
//***********************************************************************

#define     STRICT      // enable strict type checking

#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include "WINCAP.h"
#include "DIALOGS.h"
#include "commdlg.h"
#include "resource.h"

// Global variables which are set in main program 

extern char         szAppName[20];  /// Name of app
extern HINSTANCE    ghInst;         /// Handle to instance
extern HWND         ghWndMain;      /// Handle to main window
extern BOOL         gbNowCapturing;

//**********************************************************************
//
//"About" Dialog Box Window Procedure
//
// Notable features:  This dialog box draws the application's icon
// in the dialog box itself. The icon is actually stretched larger to
// fit in the specified area, which must be done manually.
// See WM_PAINT case.
//
//
//***********************************************************************

BOOL APIENTRY AboutDlgProc(HWND hWndDlg, UINT Message, WPARAM wParam, LONG
        lParam)
{
    static HBITMAP  ghAboutBmp = NULL;

    switch (Message)
    {
        // Set focus on the OK button.  Since we set focus, return FALSE

        case WM_INITDIALOG:
            SetFocus(GetDlgItem(hWndDlg, IDOK));
            ghAboutBmp = LoadBitmap(ghInst, "ABOUTBMP");
            return FALSE;

        //Closing the Dialog behaves the same as Cancel

        case WM_CLOSE:
            PostMessage(hWndDlg, WM_COMMAND, IDCANCEL, 0L);
            break;

        case WM_COMMAND:
            switch (wParam)
            {
                case IDOK:

                case IDCANCEL:
                    EndDialog(hWndDlg, FALSE);
                    if (ghAboutBmp)
                        DeleteObject(ghAboutBmp);

                    break;
            }
            break;    //End of WM_COMMAND

        // Paint our bitmap in the about box in the WM_PAINT case

        case WM_PAINT:
        {
            HBITMAP     hbm;
            HDC         hdcMem;
            PAINTSTRUCT ps;   
            RECT        r1;        // Coordinates of TEXT1 control
            POINT       p1;        // Used to convert screen to client coords
            BITMAP      bm;        // For getting info about "About" bitmap

            BeginPaint(hWndDlg, &ps);

            // Place screen coords of TEXT1 in r1

            GetWindowRect(GetDlgItem(hWndDlg, IDC_TEXT1), &r1);
            p1.x = r1.left;
            p1.y = r1.top;

            ScreenToClient(hWndDlg, &p1);

            // p1 now describes the location of upper-left corner
            // for the top text in the dialog.  Let's use this
            // to make sure we don't draw our bitmap over the text

            r1.left = 12;
            r1.top = p1.y;
            r1.right = p1.x - 25;
            r1.bottom = r1.top + (r1.right - r1.left); // Make it square

            if (hdcMem = CreateCompatibleDC(ps.hdc))
            {
                GetObject(ghAboutBmp, sizeof(BITMAP), (LPVOID)&bm);
                hbm = SelectObject(hdcMem, ghAboutBmp);
                StretchBlt(ps.hdc, r1.left, r1.top, r1.right, r1.bottom, hdcMem,
                        0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
                SelectObject(hdcMem, hbm);
                DeleteDC(hdcMem);
            }

            EndPaint(hWndDlg, &ps);
            break;
        }
    
        default:
            return FALSE;
    }

    return TRUE;
} // End of DIALOGSMsgProc


//***********************************************************************
//
// "Print" Dialog Box Window Procedure
//
// This procedure takes care of the processing for the "Print" dialog
// box, which contains the printing options.
//
// This Dialog Box Procedure is called using DialogBoxParam.  This
// allows us to pass a parameter into the dialog box procedure -- the
// parameter that gets passed in here is a LPSTR to a structure holding
// all the options that the user specified in this dialog box.  This
// allows passing the options back to the main program WITHOUT using
// global variables.
//
//***********************************************************************

BOOL APIENTRY PrintDlgProc(HWND hWndDlg, UINT Message, WPARAM wParam,
        LPARAM lParam)
{
    static LPOPTIONSTRUCT   lpOS;

    switch (Message)
    {
        case WM_INITDIALOG:
        {

            // Because DialogBoxParam() was used to invoke this dialog box,
            // lParam contains a pointer to the OPTIONSTRUCT.
            // Place user input in this structure before returning.

            lpOS = (LPOPTIONSTRUCT)lParam;

            // Check the default button -- "BEST FIT"

            CheckRadioButton(hWndDlg, IDC_BESTFIT, IDC_STRETCHTOPAGE, IDC_BESTFIT);

            // Gray out the stuff under "SCALE"

            EnableWindow(GetDlgItem(hWndDlg, IDC_XAXIS), FALSE);
            EnableWindow(GetDlgItem(hWndDlg, IDC_YAXIS), FALSE);
            EnableWindow(GetDlgItem(hWndDlg, IDC_XTEXT), FALSE);
            EnableWindow(GetDlgItem(hWndDlg, IDC_YTEXT), FALSE);
            break;
        }

        // Closing the Dialog should behave the same as Cancel

        case WM_CLOSE:        
            PostMessage(hWndDlg, WM_COMMAND, IDCANCEL, 0L);
            break;

        case WM_COMMAND:
            switch (wParam)
            {
                case IDC_BESTFIT:

                case IDC_STRETCHTOPAGE:                

                case IDC_SCALE:

                    // Check the correct button

                    CheckRadioButton(hWndDlg, IDC_BESTFIT, IDC_SCALE, wParam);

                    // And enable or disable the options under "Scale",
                    // depending on whether or not the IDC_SCALE button
                    // is checked

                    EnableWindow(GetDlgItem(hWndDlg, IDC_XAXIS),
                            (BOOL)(wParam == IDC_SCALE));
                    EnableWindow(GetDlgItem(hWndDlg, IDC_YAXIS),
                            (BOOL)(wParam == IDC_SCALE));
                    EnableWindow(GetDlgItem(hWndDlg, IDC_XTEXT),
                            (BOOL)(wParam == IDC_SCALE));
                    EnableWindow(GetDlgItem(hWndDlg, IDC_YTEXT),
                            (BOOL)(wParam == IDC_SCALE));
                    break;

                case IDOK:
                {
                    char    szTmp[100];

                    // Save the user's selection into the OPTIONSTRUCT

                    if (!lpOS)
                    {
                        EndDialog(hWndDlg, FALSE);
                        break;
                    }

                    if (IsDlgButtonChecked(hWndDlg, IDC_BESTFIT))
                        lpOS->iOption = IDC_BESTFIT;

                    if (IsDlgButtonChecked(hWndDlg, IDC_STRETCHTOPAGE))
                        lpOS->iOption = IDC_STRETCHTOPAGE;

                    if (IsDlgButtonChecked(hWndDlg, IDC_SCALE))
                        lpOS->iOption = IDC_SCALE;

                    if (GetDlgItemText(hWndDlg, IDC_XAXIS, (LPSTR)szTmp, 100))
                        lpOS->iXScale = atoi(szTmp);

                    if (GetDlgItemText(hWndDlg, IDC_YAXIS, (LPSTR)szTmp, 100))
                        lpOS->iYScale = atoi(szTmp);

                    EndDialog(hWndDlg, TRUE);
                    break;
                }
         

                case IDCANCEL:
                    EndDialog(hWndDlg, FALSE);
                    break;
            } // End of WM_COMMAND

            break;
                
        default:
            return FALSE;
    }

    return TRUE;
}


//***********************************************************************
//
// "Saving file to..." Dialog Box Window Procedure
//
// This is a modeless dialog box which is called when we save the bitmap
// to a file (so the user dosen't think his machine has hung).
//
//***********************************************************************

BOOL APIENTRY SavingDlgProc(HWND hDlg, UINT message, WPARAM wParam,
        LPARAM lParam)
{
    switch (message)
    {
        case WM_SETFOCUS:
            MessageBeep(0);
            break;

        case WM_INITDIALOG:

            // Set the text of the filename in the dialog box.  This dialog
            // should be called with DialogBoxParam, and the parameter should
            // be a pointer to the filename.  It shows up as the lParam here.

            SetDlgItemText(hDlg, IDC_FILETEXT, (LPSTR)lParam);
            return TRUE;
            break;

        case WM_DESTROY:
            return TRUE;
            break;

        default:
            return FALSE;
    }
}


//***********************************************************************
//
// Selection Help Dialog Box Window Procedure
//
// This is a modeless dialog box which displays how to select a window to
// be captured
//
//***********************************************************************
BOOL APIENTRY SelectDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM
        lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
        {
            HGLOBAL     hMem;
            char        *szText;

            hMem = GlobalAlloc(GHND, 200);
            szText = (char *)GlobalLock(hMem);
            if(szText == NULL)
                return TRUE;

            LoadString(ghInst, IDS_SELECT2, szText, 200);
            SetDlgItemText(hDlg, IDC_STATIC_SELECT2, szText);
            LoadString(ghInst, IDS_SELECT3, szText, 200);
            SetDlgItemText(hDlg, IDC_STATIC_SELECT3, szText);

            GlobalUnlock(hMem);
            GlobalFree(hMem);

            return TRUE;
        }

        case WM_COMMAND:
            if ((wParam) != IDCANCEL)
                return FALSE;
            else
            {
                ShowWindow(hDlg, SW_HIDE);
                gbNowCapturing = FALSE;
                return TRUE;
            }

        case WM_SYSCOMMAND:
            if(wParam == SC_CLOSE)
            {
                ShowWindow(hDlg, SW_HIDE);
                gbNowCapturing = FALSE;
                return TRUE;
            }
            else
                return FALSE;

        case WM_DESTROY:
            return TRUE;

        default:
            return FALSE;
    }
}


//***********************************************************************
//
// Selection Help Dialog Box Window Procedure
//
// This is a modeless dialog box which displays how to select a window to
// be captured
//
//***********************************************************************
BOOL APIENTRY RectangleDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM
        lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
        {
            HGLOBAL     hMem;
            char        *szText;

            hMem = GlobalAlloc(GHND, 200);
            szText = (char *)GlobalLock(hMem);
            if(szText == NULL)
                return TRUE;

            LoadString(ghInst, IDS_RECT2, szText, 200);
            SetDlgItemText(hDlg, IDC_STATIC_RECT2, szText);
            LoadString(ghInst, IDS_RECT3, szText, 200);
            SetDlgItemText(hDlg, IDC_STATIC_RECT3, szText);

            GlobalUnlock(hMem);
            GlobalFree(hMem);

            return TRUE;
        }

        case WM_COMMAND:
            if ((wParam) != IDCANCEL)
                return FALSE;
            else
            {
                ShowWindow(hDlg, SW_HIDE);
                gbNowCapturing = FALSE;
                return TRUE;
            }

        case WM_SYSCOMMAND:
            if(wParam == SC_CLOSE)
            {
                ShowWindow(hDlg, SW_HIDE);
                gbNowCapturing = FALSE;
                return TRUE;
            }
            else
                return FALSE;

        case WM_DESTROY:
            return TRUE;

        default:
            return FALSE;
    }
}
