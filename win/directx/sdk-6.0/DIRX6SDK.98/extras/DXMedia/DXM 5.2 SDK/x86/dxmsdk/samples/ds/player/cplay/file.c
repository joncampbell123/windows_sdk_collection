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

// This handles the file..open dialog box

#include "stdwin.h"
#include "file.h"

static OPENFILENAME ofn;

//
// InitFileOpenDialog
//
BOOL InitFileOpenDialog( HWND hwnd )
{
    ofn.lStructSize         = sizeof( OPENFILENAME );
    ofn.hwndOwner           = hwnd;
    ofn.hInstance           = NULL;
    ofn.lpstrFilter         = "Media files\0*.MPG;*.AVI;*.MOV;*.WAV\0"
                              "MPEG files\0*.MPG\0"
                              "AVI files\0*.AVI\0"
                              "Quick Time files\0*.MOV\0"
                              "Wave audio files\0*.WAV\0"
                              "Filter graph files\0*.GRF\0"
                              "All Files\0*.*\0\0";
    ofn.lpstrCustomFilter   = NULL;
    ofn.nMaxCustFilter      = 0;
    ofn.nFilterIndex        = 0;
    ofn.lpstrFile           = NULL;
    ofn.nMaxFile            = _MAX_PATH;
    ofn.lpstrFileTitle      = NULL;
    ofn.nMaxFileTitle       = _MAX_FNAME + _MAX_EXT;
    ofn.lpstrInitialDir     = NULL;
    ofn.lpstrTitle          = NULL;
    ofn.Flags               = OFN_FILEMUSTEXIST;
    ofn.nFileOffset         = 0;
    ofn.nFileExtension      = 0;
    ofn.lpstrDefExt         = "mpg";
    ofn.lCustData           = 0;
    ofn.lpfnHook            = NULL;
    ofn.lpTemplateName      = NULL;

    return TRUE;

} // InitFileOpenDialog


//
// DoFileOpenDialog
//
BOOL DoFileOpenDialog( HWND hwnd, LPSTR lpstrFileName, LPSTR lpstrTitleName )
{
    // Called to display the open file dialog
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = lpstrFileName;
    ofn.lpstrFileTitle = lpstrTitleName;

    return GetOpenFileName( &ofn );

} // DoFileOpenDialog

