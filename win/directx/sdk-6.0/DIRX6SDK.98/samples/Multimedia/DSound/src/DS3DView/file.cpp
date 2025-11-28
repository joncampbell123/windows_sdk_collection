/*==========================================================================
 *
 *  Copyright (C) 1995-1997 Microsoft Corporation. All Rights Reserved.
 *
 *  File: file.cpp
 *
 *  DESCRIPTION: Functionality to let users pick files and return their
 *                           names to the object viewer (one for XOF, one for WAV)
 *
 ***************************************************************************/

#include "d3drmwin.h"
#include "resource.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <commdlg.h>

/*
** Lets the user select a new geometry file to load and returns the full path and filename
*/

char* OpenNewFile( HWND hwnd )
{
    static char file[256];
    static char fileTitle[256];
    static char filter[] = "Geometry files (*.x)\0*.x\0" \
                           "All Files (*.*)\0*.*\0";
    OPENFILENAME ofn;

	memset( &ofn, 0, sizeof( ofn ) );
    strcpy( file, "");
    strcpy( fileTitle, "");

	// Set up the OPENFILENAME structure
    ofn.lStructSize       = sizeof( ofn );
    ofn.hwndOwner         = hwnd;
#ifdef WIN32
    ofn.hInstance         = (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE);
#else
    ofn.hInstance         = (HINSTANCE) GetWindowWord(hwnd, GWW_HINSTANCE);
#endif
    ofn.lpstrFilter       = filter;
    ofn.lpstrCustomFilter = (LPSTR) NULL;
    ofn.nMaxCustFilter    = 0L;
    ofn.nFilterIndex      = 1L;
    ofn.lpstrFile         = file;
    ofn.nMaxFile          = sizeof(file);
    ofn.lpstrFileTitle    = fileTitle;
    ofn.nMaxFileTitle     = sizeof(fileTitle);
    ofn.lpstrInitialDir   = NULL;
    ofn.lpstrTitle        = "Open a File";
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = "*.x";
    ofn.lCustData         = 0;

    ofn.Flags = OFN_SHOWHELP | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Call windows functionality to make it happen!
    if (GetOpenFileName(&ofn))
    	return (char*)ofn.lpstrFile;
    else 
	    return NULL;
}

/*
** Lets the user select a new .WAV file to load and returns it's full path and filename
*/

char* OpenNewSoundFile( HWND hwnd )
{
    static char file[256];
    static char fileTitle[256];
    static char filter[] = "Sound files (*.wav)\0*.wav\0"
			   "All Files (*.*)\0*.*\0";
    OPENFILENAME ofn;

    strcpy( file, "");
    strcpy( fileTitle, "");

    ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = hwnd;
#ifdef WIN32
    ofn.hInstance         = (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE);
#else
    ofn.hInstance         = (HINSTANCE) GetWindowWord(hwnd, GWW_HINSTANCE);
#endif
    ofn.lpstrFilter       = filter;
    ofn.lpstrCustomFilter = (LPSTR) NULL;
    ofn.nMaxCustFilter    = 0L;
    ofn.nFilterIndex      = 1L;
    ofn.lpstrFile         = file;
    ofn.nMaxFile          = sizeof(file);
    ofn.lpstrFileTitle    = fileTitle;
    ofn.nMaxFileTitle     = sizeof(fileTitle);
    ofn.lpstrInitialDir   = NULL;
    ofn.lpstrTitle        = "Open a File";
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = "*.wav";
    ofn.lCustData         = 0;

    ofn.Flags = OFN_SHOWHELP | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn))
	return (char*)ofn.lpstrFile;
    else
	return NULL;
}
