
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
*  PROGRAM:     chgdata.C
*  Part of:     BPointer sample application
*
\******************************************************************************/

#include <windows.h>
#include <stdio.h>
#include "shrstuff.h"


void InsertElement(int iValue)
{
  clRunning->iElement = iValue;
  clRunning->clNext = clRunning + 1;
  clRunning += 1;
}


int main(void)
{   HANDLE hFileMapping;
    DWORD dwVersion;
    int iCounter;
    int iNextElement;
    int iInternalCounter = 0;

// check if running on Win32s, if so, display notice and terminate
    
    dwVersion = GetVersion();
    if( !(dwVersion < 0x80000000 ) && (LOBYTE(LOWORD(dwVersion)) < 4) )
    {
        MessageBox( NULL,
            "This sample application will not run on Windows 3.1.\n"
            "This application will now terminate.",
            "ChgData",
            MB_OK | MB_ICONSTOP | MB_SETFOREGROUND );
        return( 1 );
    }

// Set up the shared memory. 
// In this process, the file mapping must not yet exist.

    hFileMapping = CreateFileMapping( (HANDLE)0xffffffff,
                                      NULL,
                                      PAGE_READWRITE,
                                      0,4096,
                                      "FM_DemoMapObject");

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {   printf("Please close ReadData before running this application!");
        CloseHandle(hFileMapping);
        return 0;
    }

    if (!hFileMapping)
    {   printf ("Error %d: CreateFileMapping failed.", GetLastError());
        return 0;
    }

// MapViewOfFile is used here to get the file mapping mapped into
// this processe's address space.  There is no guarantee about which address
// will be chosen for this process.  The address used by another
// process could be different.  That is why we need based pointers.
// The pointers in this process are based on whatever address is used 
// by this process. For another process, the pointers will be based on some 
// other address.

    clBasePt = (CHAINLIST *)MapViewOfFile( hFileMapping,
                                           FILE_MAP_ALL_ACCESS,
                                           0,0,
                                           0);

    if (clBasePt == NULL)
    {   printf("Error %d: MapViewOfFileEx failed.\n", GetLastError());
        CloseHandle(hFileMapping);
        return 0;
    }

// Now the memory is set up. The following loops and prompts the user for new
// elements until a 0 is hit. The memory is displayed so that we see that
// the offsets are entered into the link fields, ignoring the based
// pointers.

    printf("Address of file mapping in this process: 0x%08x\n", (int)clBasePt);
    printf("Enter element to insert; 0 terminates: ");
    scanf("%d",&iNextElement);
    while (iNextElement != 0)
    { 
        InsertElement(iNextElement);
        iInternalCounter += (sizeof(CHAINLIST)/sizeof(DWORD));
        printf ("Now displaying raw data at address: 0x%x\n", (int)clBasePt);
        printf ("You might want to run readdata.exe now.\n");
        for (iCounter = 0; iCounter <= iInternalCounter; iCounter += 2)
            printf("%p: %08x %08x\n", (LPVOID)((DWORD *)clBasePt+iCounter), 
                                     ((DWORD *)clBasePt)[iCounter], 
                                     ((DWORD *)clBasePt)[iCounter+1]);
        printf("\nEnter next element to insert; 0 terminates: ");
        scanf("%d", &iNextElement);
    }

// user has entered 0 -- now clean up.

    UnmapViewOfFile(clBasePt);
    CloseHandle(hFileMapping);

    return(0);
}
