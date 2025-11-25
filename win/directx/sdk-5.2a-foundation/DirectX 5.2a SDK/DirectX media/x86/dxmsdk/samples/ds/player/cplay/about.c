//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;

// Simply handles the Help..About dialog box

#include "stdwin.h"
#include "about.h"
#include "resource.h"


//
// AboutDlgProc
//
BOOL FAR PASCAL AboutDlgProc( HWND hwnd, UINT message, UINT wParam, LONG lParam )
{
    switch( message )
    {
        case WM_INITDIALOG:
           return (LRESULT) 1;

        case WM_COMMAND:
            if( wParam==IDOK || wParam==IDCANCEL ){
                EndDialog( hwnd, 0);
                return (LRESULT) 1;
            }
    }
    return (LRESULT) 0;

} // AboutDlgProc


//
// DoAboutDialog
//
void DoAboutDialog( HINSTANCE hInstance, HANDLE hwnd )
{
    DialogBox( hInstance, MAKEINTRESOURCE( IDD_ABOUTBOX ), hwnd, AboutDlgProc );

} // DoAboutDialog

