
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1992-1996 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

/******************************************************************************\
*
*  PROGRAM:     readdata.C
*  Part of:     BPointer sample application
*
\******************************************************************************/
#include <windows.h>
#include <stdio.h>
#include "shrstuff.h"


int main(void)
{   HANDLE hFileMapping;
    DWORD dwVersion;
    char cInput;

// check if running on Win32s, if so, display notice and terminate
    
    dwVersion = GetVersion();
    if( !(dwVersion < 0x80000000 ) && (LOBYTE(LOWORD(dwVersion)) < 4) )
    {
        MessageBox(NULL,
            "This sample application cannot be run on Windows 3.1.\n"
            "This application will now terminate.",
            "ReadData",
            MB_OK | MB_ICONSTOP | MB_SETFOREGROUND );
        return( 1 );
    }

// Set up the shared memory. 
// In this process, the mapped file must already exist.

    hFileMapping = OpenFileMapping( FILE_MAP_READ, FALSE, "FM_DemoMapObject");

    if (!hFileMapping)
    {   printf("Error %d: OpenFileMapping failed.  ", GetLastError());
        printf("Make sure ChgData is running first.\n");
        return(0);
    }

// MapViewOfFile is used here to get the file mapping mapped into
// this processe's address space.  There is no guarantee about which address
// will be chosen for this process.  The address used by another
// process could be different.  That is why we need based pointers.
// The pointers in this process are based on whatever address is used 
// by this process. For another process, the pointers will be based on some 
// other address.

    clBasePt = (CHAINLIST *)MapViewOfFile(hFileMapping,
                                          FILE_MAP_READ,
                                          0,0,
                                          0);
                                         
    
    if (clBasePt == NULL)
    {   printf("Error in MapViewOfFile: %d",GetLastError());
        CloseHandle(hFileMapping);
        return(0);
    }

// This loop continuously traverses the linked list, dereferencing the
// link pointers through the base implicitly.

    printf("Press q to quit, or <enter> to display list: ");
    scanf("%c",&cInput);
    while (cInput != 'q')
    {
        clRunning = 0;
        printf("Address of file mapping in this process: 0x%08x\n",(int)clBasePt);
        while (clRunning->clNext != 0)
        {
            printf ("Next Element in chain: %d\n",clRunning->iElement);
            clRunning = clRunning->clNext;
        }

        printf("Press q to quit, anything else to continue: ");
        scanf("%c",&cInput);
    }

// user didn't want any more dumps, so clean up.

    UnmapViewOfFile(clBasePt);
    CloseHandle(hFileMapping);
    return(0);
}
