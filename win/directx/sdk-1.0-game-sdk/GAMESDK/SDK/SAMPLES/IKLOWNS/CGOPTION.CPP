/*===========================================================================*\
|
|  File:        cgoption.cpp
|
|  Description: 
|   Routines to display lines of text (from resource strings) overlayed on 
|   a bitmap.  The text will be sized appropriately to fit within the 
|   rectangle specified in the profile.  The user may hilight a particular 
|   line and the index of the line will be returned to the caller.
|
|   Also, routines to scroll text across a bitmap image (useful for
|   story lines and credits).
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

/**************************************************************************

    (C) Copyright 1995 Microsoft Corp.  All rights reserved.

    You have a royalty-free right to use, modify, reproduce and 
    distribute the Sample Files (and/or any modified version) in 
    any way you find useful, provided that you agree that 
    Microsoft has no warranty obligations or liability for any 
    Sample Application Files which are modified. 

    we do not recomend you base your game on IKlowns, start with one of
    the other simpler sample apps in the GDK

 **************************************************************************/

#include <windows.h>
#include "cgglobl.h"
#include "cgdib.h"
#include "strrec.h"
#include "cginput.h"
#include "cgtext.h"
#include "cgmidi.h"
#include "cgimage.h"
#include "cgrsrce.h"
#include "cgoption.h"

#define MIN_JOY_MOVE    15
#define MIN_MOUSE_MOVE  50

#define SOURCEX SCREEN_WIDTH
#define SOURCEY SCREEN_HEIGHT
#define SCREENX GetSystemMetrics(SM_CXSCREEN)
#define SCREENY GetSystemMetrics(SM_CYSCREEN)
#define CenterX (GetSystemMetrics(SM_CXSCREEN) / 2)
#define CenterY (GetSystemMetrics(SM_CXSCREEN) / 2)

#define PIXEL_STEP  2

#define BASE_HWND   ghMainWnd
#define NUM_ENTRIES(x)  (sizeof(x)/sizeof(x[0]))

//** external functions **
extern void GetRectFromProfile(RECT &, LPSTR, LPSTR, LPSTR);
COLORREF GetColorFromProfile(LPSTR, LPSTR, LPSTR);

//** external data **
//** public data **

//** private data **
static HDC hdcIntro;
static CGameDIB *pDib;
static HPALETTE hPal;

static int iMouseMove;
static int iJoyMove;
static int iCycleRate;

//** public functions **
//** private functions **

// ----------------------------------------------------------
// LoadBitmap - load bitmap from a file into a DC and setup
//  palette.
// ----------------------------------------------------------
HDC LoadBitmapFile (
    LPSTR   pBitmapFile // Name of .BMP file
)
{
    HDC     hdcBase, hdcBitmap;

    // Loadup bitmap
    pDib = new CGameDIB(pBitmapFile);
    
    // Create a DC and copy bitmap to it
    hdcBase = GetDC(BASE_HWND);
    hdcBitmap = CreateCompatibleDC(hdcBase);
    SelectObject(hdcBitmap, pDib->GetHBitmap());

    // Set palette so image looks ok!
    hPal = pDib->CreatePalette();
    SelectPalette(hdcBase, hPal, FALSE);
    RealizePalette(hdcBase);

    ReleaseDC( BASE_HWND, hdcBase );

    return(hdcBitmap);
}

// ----------------------------------------------------------
// GetUpDownDone - poll user input devices looking to see if
//  there are any selection changes.
// ----------------------------------------------------------
void GetUpDownDone(CGameInput *Input, BOOL &up, BOOL &down, BOOL &done)
{
    JOYINFO joy;
    int x, y, buttons;

    up = down = done = FALSE;

    // Check for keyboard input
    down = (Input->GetKeyboard(VK_DOWN) !=0) ||
           (Input->GetKeyboard(VK_NUMPAD2) != 0);
    up = (Input->GetKeyboard(VK_UP) != 0) ||
           (Input->GetKeyboard(VK_NUMPAD8) != 0);
    done = ((Input->GetKeyboard(VK_RETURN)) || (Input->GetKeyboard(VK_SPACE)));

    // No keyboard input?
    if (!(up || down || done))
    {

        // Check for mouse input
        if (Input->GetMouse(x, y, buttons))
        {
            down = (y - CenterY > iMouseMove);
            up   = (y - CenterY < -iMouseMove);
            done = (buttons & 1);

            // Keep mouse in center of screen
            SetCursorPos(CenterX, CenterY);
        }
        
        // No mouse input?
        if (!(up || down || done))
        {
            // Check for joystick input
            Input->UpdateJoystick();
            if (Input->GetJoystick(1, &joy))
            {
                down = ((int)joy.wYpos >= iJoyMove);
                up = ((int)joy.wYpos <= -iJoyMove);
                done = (joy.wButtons & JOY_BUTTON1);
            }           
        }
    }
}   

// ----------------------------------------------------------
// NewStringResource -  Load string from resource into memory
// ----------------------------------------------------------
LPSTR NewStringResource(
    HINSTANCE   hInst,
    int         idString
)
{
    char    TempBuf[256];
    DWORD   nBytes=0;
    LPSTR   pStr=NULL;

    nBytes = LoadString(hInst, idString, TempBuf, sizeof(TempBuf));
    if (nBytes > 0)
    {
        pStr = new char [nBytes+1];
        lstrcpy(pStr, TempBuf);
    }
    return(pStr);
}

// ----------------------------------------------------------
// COptionScreen constructor - Options screen object for 
//  displaying option screen while allowing repaints to occur
// These routines do not take advantage of DirectDraw, unfortunately.
// ----------------------------------------------------------
COptionScreen::COptionScreen()
{
    mHdcScreen = NULL;
    mHdcIntro = NULL;
    mInput = NULL;
    pText = NULL;
    pDib = NULL;
    mnChoices = 0;
}

// ----------------------------------------------------------
// Init - initialises context for option screen
//  NOTE: resource strings are assumed to be sequential!
// ----------------------------------------------------------
BOOL COptionScreen::Init(
    LPSTR       pBitmapName,    // name of .BMP file or NULL to use default
    LPSTR       ProfileName,    // filename of game config file
    CGameInput  *Input,     // ptr to input object or NULL if no input
    int     timeout     // maximum time to wait or -1 forever
)
{
    return( TRUE );
}

// ----------------------------------------------------------
// Init - initialises context for option screen
//  NOTE: resource strings are assumed to be sequential!
// ----------------------------------------------------------
BOOL COptionScreen::Init(
    LPSTR       pBitmapName,    // name of .BMP file or NULL to use default
    int     idStringBase,   // first resource id of text lines
    int     nChoices,   // number of text lines to display
    LPSTR       ProfileName,    // filename of game config file
    CGameInput  *Input,     // ptr to input object or NULL if no input
    int     defSelect,  // line to hilight first or -1 if none
    int     timeout,        // maximum time to wait or -1 forever
    int     spacing,
    int     lines
)
{
    return( TRUE );
}

// ----------------------------------------------------------
// AddText - 
// ----------------------------------------------------------
int COptionScreen::AddText(
    LPSTR       pNewText
)
{
    if (pText == NULL)
    {
        return(-1);
    }

    mnChoices++;
    pText->AddLine(pNewText, colorDefault, colorDefaultShadow);
    return(mnChoices);
}

// ----------------------------------------------------------
// SelectText - 
// ----------------------------------------------------------
BOOL COptionScreen::SelectText(
    int     defSelect   // line to hilight first or -1 if none
)
{
    if (pText == NULL)
    {
        return(FALSE);
    }

    CurSel = defSelect;

    pText->TextBlt();
    pText->ChangeColor(CurSel+1, colorSelected, colorSelectedShadow);
    return( TRUE );
}

// ----------------------------------------------------------
// SetSpacing - 
// ----------------------------------------------------------
void COptionScreen::SetSpacing(int spacing)
{
    if (pText != NULL)
    {
        pText->SetSpacing(spacing);
    }

}   

// ----------------------------------------------------------
// SetMaxLines - 
// ----------------------------------------------------------
void COptionScreen::SetMaxLines(int lines)
{
    if (pText != NULL)
    {
        pText->SetMaxLines(lines);
    }

}   

// ----------------------------------------------------------
// Shutdown - closes down context
// ----------------------------------------------------------
void COptionScreen::Shutdown( void )
{
    // Cleanup
    if ( mHdcIntro ) {
        DeleteDC(mHdcIntro);
        mHdcIntro = NULL;
    }
    if ( pText ) {
        delete pText;   // destructor will delete our strings for us!
    }
    if ( pDib ) {
        delete pDib;
    }
    if ( mHdcScreen ) {
        ReleaseDC(BASE_HWND, mHdcScreen);
        mHdcScreen = NULL;
    }
}

// ----------------------------------------------------------
// DlgProcOptions - main game options dialog
// ----------------------------------------------------------
BOOL CALLBACK DlgProcOptions(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static LONG iIndex = -1;
    static HWND hWndCtl;
    // char   chBuffer[128];

    switch (msg)
    {
    case WM_INITDIALOG:

        iIndex = -1;

        hWndCtl = GetDlgItem(hDlg, IDC_LISTOPTION);
        if (hWndCtl == NULL)
        {
            EndDialog(hDlg, -1);
            return(TRUE);
        }
        SendMessage(hWndCtl, LB_ADDSTRING, 0, (LPARAM) "Play Solo");
        SendMessage(hWndCtl, LB_ADDSTRING, 0, (LPARAM) "Two Player Game");
        SendMessage(hWndCtl, LB_ADDSTRING, 0, (LPARAM) "Play Remote Opponents");
        SendMessage(hWndCtl, LB_ADDSTRING, 0, (LPARAM) "Quit Game");
        SendMessage(hWndCtl, LB_SETCURSEL, 0, 0);
        SetFocus(hWndCtl);
        iIndex = 0;

        return(FALSE);

    case WM_COMMAND:

        switch( HIWORD(wParam))
        {
        case LBN_SELCHANGE:
            iIndex = SendMessage((HWND) lParam, LB_GETCURSEL, 0, 0);
            hWndCtl = (HWND) lParam;
            //wsprintf( chBuffer, "Index set to %d, %8x %8x %8x\r\n",
            //     iIndex, msg, wParam, lParam);
            // OutputDebugString(chBuffer);
            return(FALSE);

        case LBN_DBLCLK:
            iIndex = SendMessage((HWND) lParam, LB_GETCURSEL, 0, 0);
            //wsprintf( chBuffer, "Index set to %d, %8x %8x %8x\r\n",
            //     iIndex, msg, wParam, lParam);
            // OutputDebugString(chBuffer);
            EndDialog(hDlg, iIndex);
            return(TRUE);

        case 0:
            //wsprintf( chBuffer, "Index returned %d, %8x %8x %8x\r\n",
            //     iIndex, msg, wParam, lParam);
            // OutputDebugString(chBuffer);

            if (LOWORD(wParam) == IDOK)
            {
                EndDialog(hDlg, iIndex);
                return(TRUE);
            }
            else if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, -1);
                return(TRUE);
            }
            break;

        }
    }
    //wsprintf( chBuffer, "Ret False Index set to %d, %8x %8x %8x\r\n",
    //     iIndex, msg, wParam, lParam);
    // OutputDebugString(chBuffer);
    return (FALSE);
}

// ----------------------------------------------------------
// DoOptionScreen - display bitmap & text and allow user to 
//  make a selection.
// ----------------------------------------------------------
int COptionScreen::DoOptionScreen( void )
{
    // Allow user to change options
    BOOL    done = FALSE;
    BOOL    switchedAway = FALSE;
    DWORD   startTime = timeGetTime();
    DWORD   dwRet;

    pDib = new CGameDIB("instruct.bmp");

    InvalidateRect(NULL, NULL, TRUE);

    dwRet = DialogBox (ghInst, (LPCTSTR) IDD_OPTIONS, ghMainWnd, (DLGPROC) DlgProcOptions);
    return(dwRet);
}


// ----------------------------------------------------------
// Paint - 
// ----------------------------------------------------------
void COptionScreen::Paint( void )
{
    HDC     hdcScreen = GetDC(BASE_HWND);
    HDC     hdcBitmap;

    hdcBitmap = CreateCompatibleDC(hdcScreen);
    SelectObject( hdcBitmap, pDib->GetHBitmap() );

    StretchBlt(hdcScreen, 0, 0, SCREENX, SCREENY, hdcBitmap
    , 0, 0, SOURCEX, SOURCEY, SRCCOPY);

    ReleaseDC(BASE_HWND, hdcScreen);
    DeleteObject(hdcBitmap);
}

