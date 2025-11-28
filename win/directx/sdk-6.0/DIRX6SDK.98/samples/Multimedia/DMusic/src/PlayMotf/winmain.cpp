//-----------------------------------------------------------------------------
// File: WinMain.cpp
//
// Desc: Plays a Primary Segment and a Motif using DirectMusic
//
//
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT
#include <windows.h>
#include <stdlib.h>
#include "resource.h"
#include "playmotf.h"

//-----------------------------------------------------------------------------
// extern global variables 
//-----------------------------------------------------------------------------
extern WCHAR g_awszMotifName[9][MAX_PATH];




//-----------------------------------------------------------------------------
// Function: MainDialogProc
//
// Description: 
//      Handles dialog messages
//
//-----------------------------------------------------------------------------
LRESULT CALLBACK MainDialogProc( HWND hDlg, UINT message, WPARAM wParam, 
                                 LPARAM lParam )
{
    int   nIndex;
    HWND  hWnd;
    char  szButton[255];
    char  szMotif[255];
    DWORD dwIndex;

    switch (message) 
    {
    case WM_INITDIALOG:
        // update buttons with names of the motifs
        for( dwIndex = 0; dwIndex < 9; dwIndex++ )
        {
            hWnd = GetDlgItem( hDlg, IDC_MOTIF_1 + dwIndex );
            GetWindowText( hWnd, szButton, 255 );
            wcstombs( szMotif, g_awszMotifName[ dwIndex ], 255 );
            strcat( szButton, ": " );
            strcat( szButton, szMotif );
            SetWindowText( hWnd, szButton );
        }
        break;

    case WM_COMMAND:
        switch ( LOWORD(wParam) )
        {
        case IDC_MOTIF_1:
        case IDC_MOTIF_2:
        case IDC_MOTIF_3:
        case IDC_MOTIF_4:
        case IDC_MOTIF_5:
        case IDC_MOTIF_6:
        case IDC_MOTIF_7:
        case IDC_MOTIF_8:
        case IDC_MOTIF_9:
            nIndex = LOWORD(wParam) - IDC_MOTIF_1;
            PlayMotif( g_awszMotifName[ nIndex ] );  // play selected motif
            break;

        case IDC_CLOSE:
            PostQuitMessage( 0 );
            break;
        }
        break;

    case WM_CLOSE:
        EndDialog(hDlg, TRUE);
        return (TRUE);
    }

    return FALSE;
}




//-----------------------------------------------------------------------------
// Function: WinMain(HANDLE, HANDLE, LPSTR, int)
//
// Description: 
//     Entry point for the application.  Since we use a simple dialog for 
//     user interaction we don't need to pump messages.
//
//-----------------------------------------------------------------------------
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                      LPSTR lpCmdLine, int nCmdShow )
{
    HRESULT hr;

    hr = InitDirectMusic( lpCmdLine );
    if ( FAILED(hr) )
    {
        MessageBox( NULL, 
            "Error Initializing DirectMusic", 
            "EchoTool", 
            MB_ICONERROR | MB_OK );
        return FALSE;
    }

    // Display the main dialog box.
    DialogBox( hInstance, 
        MAKEINTRESOURCE(IDD_PLAY_MOTIF), 
        NULL, 
        (DLGPROC)MainDialogProc );

    FreeDirectMusic();

    return TRUE;
}



