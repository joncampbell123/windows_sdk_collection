
/*****************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1992-1996 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\*****************************************************************************/

/****************************************************************************
*
*    PROGRAM: WALK.C
*
*    PURPOSE: Uses recursion to walk through all subdirectories in the
*             desired directory
*
*    FUNCTIONS:
*
*       FindFirstDirectory() - finds the first directory in the current
*                 working directory
*       Walk() - finds the subdirectories in the current working directory,
*                 changes the current working directory to this subdirectory,
*                 recusively calls itself until there are no more
*                 subdirectories
*
*    COMMENTS:
*
*       This program does not report all subdirectories like a
*       directory listing would yield.  The reason for this is
*       because some subdirectories may be secured.  If so, they
*       are ignored by this program.  The concept of this 'walk' is
*       that items actually stepped into are reported.
*
****************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include "walk.h"

/****************************************************************************
*
*    FUNCTION: FindFirstDirectory(LPTSTR, LPWIN32_FIND_DATA)
*
*    PURPOSE: finds the first directory in the current working directory
*
*    COMMENTS:
*
*       This function is called by Walk() each time a new subdirectory is
*       entered.  Since only directory entries are of interest, this call
*       provides a simple means to bypass non-directory filenames.
*
*    INPUT: lpszSearchFile -> "*"
*           lpffd - pointer to the file find data structure of type
*                   WIN32_FIND_DATA
*
*    OUTPUT: Returns a file handle if a directory is found
*            Returns INVALID_HANDLE_VALUE if no directory is found        
*
****************************************************************************/

HANDLE FindFirstDirectory(LPTSTR lpszSearchFile, LPWIN32_FIND_DATA lpffd)
{
    BOOL     fContinue;
    HANDLE   hSearch;
    DWORD    dwAtt;     // file attributes
    int      iDiff;     // string comparison result

    hSearch = FindFirstFile(lpszSearchFile, lpffd);
    if (hSearch == INVALID_HANDLE_VALUE)
        return hSearch;
    
    for(;;)
    {
        dwAtt = GetFileAttributes(lpffd->cFileName);
        if (dwAtt != 0xFFFFFFFF)
        {
            if (dwAtt & FILE_ATTRIBUTE_DIRECTORY)
            {
                iDiff = strcmp(lpffd->cFileName, ".");
                if (iDiff)
                {
                    iDiff = strcmp(lpffd->cFileName, "..");
                    if (iDiff)
                        return hSearch;
                }
            }
        }
    
        fContinue = FindNextFile(hSearch, lpffd);
        if (fContinue == FALSE)
        {
            FindClose(hSearch);
            return INVALID_HANDLE_VALUE;
        }
    }
}

/****************************************************************************
*
*    FUNCTION: FindNextDirectory(LPTSTR, LPWIN32_FIND_DATA)
*
*    PURPOSE: finds the next directory in the current working directory
*
*    COMMENTS:
*
*       This function is called by Walk() each time a new subdirectory is
*       entered.  Since only directory entries are of interest, this call
*       provides a simple means to bypass non-directory filenames.
*
*    INPUT: lpszSearchFile -> "*"
*           lpffd - pointer to the file find data structure of type
*                   WIN32_FIND_DATA
*
*    OUTPUT: Returns a file handle if a directory is found
*            Returns a -1 if no directory is found        
*
****************************************************************************/

BOOL FindNextDirectory(HANDLE hSearch, LPWIN32_FIND_DATA lpffd)
{
    BOOL     bContinue;
    DWORD    dwAtt;

    for(;;)
    {
        bContinue = FindNextFile(hSearch, lpffd);
        if (bContinue == FALSE)
            return FALSE;

        dwAtt = GetFileAttributes(lpffd->cFileName);

        // If an error occurs while getting the file attributes, it is
        //   ignored, then the next file is tried.
        //   GetLastError could be called to determine the cause of the error.
        //   More than likely an invalid filename, or entry is secured.
        if (dwAtt == 0xFFFFFFFF)    
            continue;

        if (dwAtt & FILE_ATTRIBUTE_DIRECTORY)
            return TRUE;
    }
}

/****************************************************************************
*
*    FUNCTION: Walk(WORD)
*
*    PURPOSE: finds a subdirectory in the current working directory,
*             changes the current working directory to this subdirectory,
*             and recusively calls itself until there are no more
*             subdirectories
*
*    COMMENTS:When a new directory is entered from above, a handle for
*             the new directory is obtained using FindFirstDirectory.  Once
*             the first directory is found, the current working directory
*             is changed to this first directory and Walk() is recursively
*             called again.  At this point, the next available directory
*             is searched for using FindNextFile, entered and a recursive
*             call is made to Walk().  When each directory has been searched,
*             until no more directories exist, the current working directory
*             is changed to the parent directory (..).  This continues until
*             the current working directory is equal to the original
*             directory.
*
*    INPUT: wLevel - bookmark, when wLevel is greater than 0, then the
*             current working directory is a subdirectory of the original
*             directory.  If wLevel is equal to 0, then the directory is the
*             original directory and the recursive calls stop
*
*    OUTPUT: 
*
****************************************************************************/

VOID Walk(WORD wLevel)
{
    BOOL              bContinue = TRUE;
    DWORD             dwSize;
    HANDLE            hSearch;
    WIN32_FIND_DATA   w32FindBuf;

    dwSize = GetCurrentDirectory(BUFSIZE, chPathName);
    if (dwSize == 0)
    {
        printf("Error getting current directory name.  Exiting...\n");
        exit(1);
    }
    
    if (dwSize >= (BUFSIZE-1))
    {
        printf("\nDirectory name too large.  Skipping over...\n");
        return;
    }

    printf("%s\n", chPathName);

    hSearch = FindFirstDirectory("*", &w32FindBuf);
    if (hSearch == INVALID_HANDLE_VALUE)
    {
        if (wLevel)
            SetCurrentDirectory("..");
        return;
    }

    for (;;)
    {
        if (SetCurrentDirectory(w32FindBuf.cFileName))
            Walk(++wLevel);
        else
            printf("\nSkipping subdirectory: %s\n", w32FindBuf.cFileName);
        
        bContinue = FindNextDirectory(hSearch, &w32FindBuf);
        if (bContinue == FALSE)
        {
            SetCurrentDirectory("..");
            FindClose(hSearch);
            return;
        }
    }
}

VOID main(int argc, char * argv[])
{
    // check if Win32s, if so, display notice and terminate
    if( GetVersion() & 0x80000000 && (GetVersion() & 0xFF) == 3)
    {
        MessageBox( NULL,
            "This application cannot run on Windows 3.1.\n"
            "This application will now terminate.",
            "Walk",
            MB_OK | MB_ICONSTOP | MB_SETFOREGROUND );
        return;
    }

    // if a directory is supplied on the command line
    //  start there
    if (argc == 2)
    {
        if (!SetCurrentDirectory(argv[1]))
        {
            printf("Can't walk '%s'\n", argv[1]);
            return;
        }
    }

    Walk(0);

    return;
}
