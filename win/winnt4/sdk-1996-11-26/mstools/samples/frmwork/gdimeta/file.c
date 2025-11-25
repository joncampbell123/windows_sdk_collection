// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993 - 1996  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE: file.c
//
//  PURPOSE: File manipulation functions.
//
//  FUNCTIONS:
//    CmdFileNew       - Create a new text document.
//    CmdFileSave      - Save the current document.
//    Open             - Open a file based on a file name.
//    QuerySaveFile    - Prompt and save the current document.
//    SaveFile         - Save the metafile to the current file.
//    SaveAs           - Save the metafile to the specified file.
//    MapFileReadOnly  - Opens a file for reading and map it into memory.
//    MapFileReadWrite - Opens a file for read/write and map it into memory.
//    CloseFileMapping - Unmaps and closes a file mapping.
//
//  COMMENTS:
//


#include <windows.h>            // required for all Windows applications
#include <windowsx.h>
#include <commctrl.h>
#include "globals.h"            // prototypes specific to this application
#include "resource.h"
#include <string.h>             // for _strnicmp
#include "palette.h"            // palette support functions
#include "metafile.h"           // metafile support functions


// Local function prototypes

BOOL SaveFile(HWND);


// Global variables

char szFName[256];
char szUntitled[] = SZAPPNAME" - (untitled)";
BOOL bModify = FALSE;
HENHMETAFILE hEMF = NULL;


//
//  FUNCTION: CmdFileNew(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Create a new text document.
//
//  PARAMETERS:
//    hwnd     - The window.
//    wCommand - IDM_FILENEW (unused)
//    wNotify  - Notification number (unused)
//    hwndCtrl - NULL (unused)
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//
//

LRESULT CmdFileNew(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    // If current file has been modified, query user about saving it.
    if (!QuerySaveFile(hwnd))
        return 0;

    if (NULL != hEMF)
    {
        DeleteEnhMetaFile(hEMF);
        hEMF = NULL;
    }
    bModify = FALSE;
    GetFName()[0] = 0;
    SetWindowText(hwnd, szUntitled);

    // Tell client window...
    ClientNewDrawing();

    return 0;
}

//
//  FUNCTION: CmdFileSave(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Save the current document.
//
//  PARAMETERS:
//    hwnd     - The window.
//    wCommand - IDM_FILESAVE (unused)
//    wNotify  - Notification number (unused)
//    hwndCtrl - NULL (unused)
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//
//

LRESULT CmdFileSave(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    // If there is no filename, use the saveas command to get one.
    //   Otherwise, save the file using the current filename.
    if (!GetFName()[0])
    {
        CmdFileSaveAs(hwnd, wCommand, 0, hwndCtrl);
    }
    else
    {
        // Only save the document if it has been modified.
        if (bModify && SaveFile(hwnd))
            bModify = FALSE;
    }

    return 0;
}

//
//  FUNCTION: Open(LPSTR, BOOL, HWND)
//
//  PURPOSE: Open a file based on a file name and readonly attribute.
//
//  PARAMETERS:
//    lsz - The full path name of the file to be opened.
//    fReadOnly - Read only attribute of the file. (ignored in GDIMeta)
//    hwnd - The window into which the file is being opened.
//
//  RETURN VALUE:
//    NONE
//
//  COMMENTS:
//
//

VOID Open(LPSTR lsz, BOOL fReadOnly, HWND hwnd)
{
    char rgch[256];

    if (NULL != hEMF)                       // Delete previous metafile
        DeleteEnhMetaFile(hEMF);

    hEMF = LoadMetaFile(lsz);               // Load new metafile

    if (NULL == hEMF)                       // Success?
    {
        MessageBox(hwnd,
                   "Not a valid MetaFile.",
                   NULL,
                   MB_OK | MB_ICONINFORMATION);
        return;
    }

    // Set new window title
    wsprintf(rgch, "%s - %s", szAppName, lsz);
    SetWindowText(hwnd, rgch);

    bModify = FALSE;                        // Just opened ==> not modified
    lstrcpy(GetFName(), lsz);               // Save file name

    // Tell client window that we've opened a metafile
    ClientNewDrawing();
}

//
//  FUNCTION: QuerySaveFile(HWND)
//
//  PURPOSE: Save the current document if necessary & the user agrees.
//
//  PARAMETERS:
//    hwnd - The document window.
//
//  RETURN VALUE:
//    TRUE  - The file has been saved, or any changes can be thrown out.
//    FALSE - The use has changed his/her mind, don't continue the
//      operation that would throw away this document.
//
//  COMMENTS:
//
//

BOOL QuerySaveFile(HWND hwnd)
{
    int  idResponse;
    char rgch[256];

    if (bModify)  // Are there any changes to save?
    {
        wsprintf(rgch, "Save current changes? %s", GetFName());
        idResponse = MessageBox(hwnd,
                                rgch,
                                "EditFile",
                                MB_YESNOCANCEL | MB_ICONEXCLAMATION);

        if (idResponse == IDYES)            // Save changes
        {
            // Make sure there is a filename to save to
            if (!GetFName()[0])
                return (BOOL)CmdFileSaveAs(hwnd, IDM_FILESAVEAS, 0, NULL);
            else
                SaveFile(hwnd);
        }
        else if (idResponse == IDCANCEL)    // Cancel entire operation
            return FALSE;
    }

    return TRUE;                            // Proceed
}

//
//  FUNCTION: SaveFile(HWND)
//
//  PURPOSE: Save the MetaFile to the current file.
//
//  PARAMETERS:
//    hwnd - The main application window.
//
//  RETURN VALUE:
//    TRUE - Sucessfully saved the file.
//    FALSE - Unable to save the file.
//
//  COMMENTS:
//

BOOL SaveFile(HWND hwnd)
{
    HENHMETAFILE hemfTemp = GetCurrentEMF();

    // First make sure we have a metafile and a file name

    if (!GetFName()[0] || NULL == hemfTemp)
        return FALSE;

    // Delete old metafile and save new metafile handle in hEMF

    if (hEMF)
        DeleteEnhMetaFile(hEMF);

    hEMF = hemfTemp;


    // Check file extension for file type
    // "WMF" ==> placeable format, otherwise default to enhanced format

    if (0 == _strnicmp(GetFName() + lstrlen(GetFName()) - 4, ".wmf", 4))
    {
        // Save as a placeable metafile.
        if (!SavePlaceableMetaFile(hEMF, GetFName()))
            return FALSE;
    }
    else
    {
        // Save in enhanced metafile format... let the system
        // do all the work of saving the metafile.

        hemfTemp = CopyEnhMetaFile(hEMF, GetFName());
        if (NULL == hemfTemp)
            return FALSE;

        DeleteEnhMetaFile(hemfTemp);
    }

    // Success!
    bModify = FALSE;
    return TRUE;
}


//
//  FUNCTION: SaveAs(LPSTR, HWND)
//
//  PURPOSE: Save the edit text to the specified file.
//
//  PARAMETERS:
//    lsz  - The fully qualified path of the file to save.
//    hwnd - The main application window.
//
//  RETURN VALUE:
//    NONE
//
//  COMMENTS:
//
//

VOID SaveAs(LPSTR lsz, HWND hwnd)
{
    char rgch[256];

    lstrcpy(rgch, GetFName());
    lstrcpy(GetFName(), lsz);
    if (SaveFile(hwnd))
    {
        wsprintf(rgch, "%s - %s", szAppName, GetFName());
        SetWindowText(hwnd, rgch);
    }
    else
        lstrcpy(GetFName(), rgch);
}


//
//  FUNCTION: MapFileReadOnly(LPCSTR, LPCSTR, PFILEMAP)
//
//  PURPOSE: Open a file for reading and map it into memory.
//
//  PARAMETERS:
//    szFile    - The fully qualified path of the file to open.
//    szMapName - The name of the file mapping object to create.
//    pfm       - Points to a FILEMAP structure that receives the file
//                handle, the file mapping object's handle, and the
//                pointer to the view of the file in memory.
//
//  RETURN VALUE:
//    TRUE for success, FALSE otherwise.
//
//  COMMENTS:
//
//

BOOL MapFileReadOnly(LPCSTR szFile, LPCSTR szMapName, PFILEMAP pfm)
{
    //
    // Open the file
    //
    pfm->hFile = CreateFile(szFile,
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_READONLY,
                            NULL);

    if (pfm->hFile == (HANDLE)-1)
        return FALSE;

    //
    // Create a file mapping of the opened file
    //
    pfm->hFileMap = CreateFileMapping(pfm->hFile,
                                      NULL,
                                      PAGE_READONLY,
                                      0, 0,
                                      szMapName);
    if (pfm->hFileMap == NULL)
    {
        CloseHandle(pfm->hFile);
        return FALSE;
    }

    //
    // Map a view of the whole file
    //
    pfm->pFileMap = MapViewOfFile(pfm->hFileMap, FILE_MAP_READ, 0, 0, 0);
    if (pfm->pFileMap == NULL)
    {
        CloseHandle(pfm->hFileMap);
        CloseHandle(pfm->hFile);
        return FALSE;
    }

    return TRUE;
}


//
//  FUNCTION: MapFileReadWrite(LPCSTR, LPCSTR, PFILEMAP, DWORD, DWORD)
//
//  PURPOSE: Open a file for reading and writing and map it into memory.
//
//  PARAMETERS:
//    szFile    - The fully qualified path of the file to open.
//    szMapName - The name of the file mapping object to create.
//    pfm       - Points to a FILEMAP structure that receives the file
//                handle, the file mapping object's handle, and the
//                pointer to the view of the file in memory.
//    dwCreateFlags - File creation flags passed to CreateFile as fdwCreate
//    dwMaxSize - The size of the file mapping object.
//
//  RETURN VALUE:
//    TRUE for success, FALSE otherwise.
//
//  COMMENTS:
//
//

BOOL MapFileReadWrite(LPCSTR   szFile,
                      LPCSTR   szMapName,
                      PFILEMAP pfm,
                      DWORD    dwCreateFlags, // e.g. OPEN_EXISTING
                      DWORD    dwMaxSize)
{
    //
    // Open the file
    //
    pfm->hFile = CreateFile(szFile,
                            GENERIC_READ | GENERIC_WRITE,
                            0,
                            NULL,
                            dwCreateFlags,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);

    if (pfm->hFile == (HANDLE)-1)
        return FALSE;

    //
    // Create a file mapping of the opened file
    //
    pfm->hFileMap = CreateFileMapping(pfm->hFile,
                                      NULL,
                                      PAGE_READWRITE,
                                      0, dwMaxSize,
                                      szMapName);
    if (pfm->hFileMap == NULL)
    {
        CloseHandle(pfm->hFile);
        return FALSE;
    }

    //
    // Map a view of the whole file
    //
    pfm->pFileMap = MapViewOfFile(pfm->hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, dwMaxSize);
    if (pfm->pFileMap == NULL)
    {
        CloseHandle(pfm->hFileMap);
        CloseHandle(pfm->hFile);
        return FALSE;
    }

    return TRUE;
}


//
//  FUNCTION: CloseFileMapping(PFILEMAP)
//
//  PURPOSE: Unmaps and closes a file previously opened with MapFileReadOnly
//    or MapFileReadWrite.
//
//  PARAMETERS:
//    pfm       - Points to a FILEMAP structure containing a file
//                handle, a file mapping handle, and the address of
//                the view of the file in memory.
//
//  RETURN VALUE:
//    Always returns TRUE.
//
//  COMMENTS:
//
//

BOOL CloseFileMapping(PFILEMAP pfm)
{
    UnmapViewOfFile(pfm->pFileMap);
    CloseHandle(pfm->hFileMap);
    CloseHandle(pfm->hFile);

    return TRUE;
}
