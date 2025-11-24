/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

#include <windows.h>
#include <winspool.h>
#include <winsplp.h>
#include <regstr.h>

#include "spltypes.h"
#include "local.h"
#include "localmon.h"
#include "dialogs.h"


// Local functions:
BOOL
PortNameInitDialog(
    HWND hwnd,
    LPTSTR FAR *ppPortName
);
BOOL
PortNameCommandOK(
    HWND hwnd
);
BOOL
PortNameCommandCancel(
    HWND hwnd
);
BOOL
PortIsValid(
    LPTSTR pPortName
);

BOOL
PrintToFileInitDialog(
    HWND  hwnd,
    LPTSTR pFileName
);

BOOL
PrintToFileCommandOK(
    HWND hwnd
);

BOOL
PrintToFileCommandCancel(
    HWND hwnd
);


// -----------------------------------------------------------------------
//
// Add Port UI
//
// -----------------------------------------------------------------------
BOOL APIENTRY
PortNameDlg(
   HWND   hwnd,
   WORD   msg,
   WPARAM wparam,
   LPARAM lparam
)
{
    switch(msg)
    {
    case WM_INITDIALOG:
        return PortNameInitDialog(hwnd, (LPTSTR FAR *)lparam);

    case WM_COMMAND:
        switch (LOWORD(wparam))
        {
        case IDOK:
            return PortNameCommandOK(hwnd);

        case IDCANCEL:
            return PortNameCommandCancel(hwnd);
        }
        break;
    }

    return FALSE;
}


BOOL
PortNameInitDialog(
    HWND hwnd,
    LPTSTR FAR *ppPortName
)
{
    SetForegroundWindow(hwnd);

    SetWindowLong (hwnd, GWL_USERDATA, (LONG)ppPortName);
    SendDlgItemMessage (hwnd, IDD_PN_EF_PORTNAME, EM_LIMITTEXT, MAX_PATH, 0);

    return TRUE;
}


BOOL
PortNameCommandOK(
    HWND hwnd
)
{
    LPTSTR FAR *ppPortName;
    TCHAR string[MAX_PATH];

    ppPortName = (LPTSTR FAR *)GetWindowLong( hwnd, GWL_USERDATA );

    GetDlgItemText( hwnd, IDD_PN_EF_PORTNAME, string, sizeof string );

    if( PortIsValid( string ) )
    {
       EnterMonSem( );
        *ppPortName = AllocSplStr( string );
       LeaveMonSem( );
        EndDialog( hwnd, TRUE );
    }
    else
        Message( hwnd, MSG_ERROR, IDS_WIN32_SPOOLER, IDS_INVALIDPORTNAME_S, (LPTSTR)string );

    return TRUE;
}


BOOL
PortNameCommandCancel(
    HWND hwnd
)
{
    EndDialog( hwnd, FALSE );
    return TRUE;
}


// PortIsValid
//
// Validate the port by attempting to create/open it.
//
BOOL
PortIsValid(
    LPTSTR pPortName
)
{
    HANDLE hFile;
    BOOL   Valid;
    char pDOSName[MAX_PATH];

    // remove trailing colon
    // remove trailing spaces ???
    if (pPortName)
        lstrcpy(pDOSName, pPortName);
    else
        return FALSE;

    if (pDOSName[lstrlen(pDOSName) - 1] == ':')
    {
        pDOSName[lstrlen(pDOSName) - 1] = 0;
    }

    hFile = CreateFile( pDOSName, GENERIC_WRITE, 0 /*FILE_SHARE_READ*/, NULL,
                        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

    if (hFile == INVALID_HANDLE_VALUE)
        Valid = FALSE;
    else
    {
        BOOL Existed = (GetLastError() == ERROR_ALREADY_EXISTS);

        CloseHandle( hFile );
        Valid = TRUE;

        // delete the file if we just created it
        if (!Existed)
            DeleteFile(pDOSName);
    }

    return Valid;
}


// -----------------------------------------------------------------------
//
// Print to File UI
//
// -----------------------------------------------------------------------
BOOL APIENTRY
PrintToFileDlg(
   HWND   hwnd,
   WORD   msg,
   WPARAM wparam,
   LPARAM lparam
)
{
    switch(msg)
    {
    case WM_INITDIALOG:
        return PrintToFileInitDialog(hwnd, (LPTSTR)lparam);

    case WM_COMMAND:
        switch (LOWORD(wparam))
        {
        case IDOK:
            return PrintToFileCommandOK(hwnd);

        case IDCANCEL:
            return PrintToFileCommandCancel(hwnd);
        }
        break;
    }

    return FALSE;
}


BOOL
PrintToFileInitDialog(
    HWND  hwnd,
    LPTSTR pFileName
)
{
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

    SetFocus(hwnd);

    SetWindowLong( hwnd, GWL_USERDATA, (LONG)pFileName );

    SendDlgItemMessage( hwnd, IDD_PF_EF_OUTPUTFILENAME, EM_LIMITTEXT, MAX_PATH, 0);

    return TRUE;
}


BOOL
PrintToFileCommandOK(
    HWND hwnd
)
{
    LPTSTR pFileName;
    OFSTRUCT ofs;
    CHAR pBuf1[128];
    CHAR pBuf2[16];

    pFileName = (LPTSTR)GetWindowLong( hwnd, GWL_USERDATA );

    GetDlgItemText( hwnd, IDD_PF_EF_OUTPUTFILENAME,
                    pFileName, MAX_PATH );

    if (OpenFile(pFileName, &ofs, OF_EXIST) != HFILE_ERROR)
    {
        LoadString(hInst, IDS_OVERWRITE_FILE, pBuf1, sizeof(pBuf1));
        LoadString(hInst, IDS_FILE_CAPTION, pBuf2, sizeof(pBuf2));

        if (MessageBox(hwnd, pBuf1, pBuf2, MB_ICONINFORMATION | MB_OKCANCEL) == IDCANCEL)
        {
            return FALSE;
        }
    }

    EndDialog( hwnd, (BOOL)*pFileName );

    return TRUE;
}


BOOL
PrintToFileCommandCancel(
    HWND hwnd
)
{
    EndDialog(hwnd, FALSE);
    return TRUE;
}
