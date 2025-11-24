
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993 Microsoft Corporation.
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
{     HANDLE hFileMapping;
      char cInput;

// Set up the shared memory. By using MapViewOfFileEx(), we force the
// memory onto a specific location. This is just to show how based
// pointers work for memory that is mapped differenctly in two processes.
// In this process, the mapped file must already exist unless in chgdata
// where it must not exist yet.


      hFileMapping = CreateFileMapping((HANDLE)0xffffffff,NULL,PAGE_READONLY,0,4096,
                                       "DemoMapObject");
      if (GetLastError() != ERROR_ALREADY_EXISTS)
         { printf("Please run chgdata before readdata!");
           return(0);
         };
      if (!hFileMapping)
         { printf ("Failed on CreateFileMapping...");
           return(0);
         };
      clBasePt = (CHAINLIST *)MapViewOfFileEx(hFileMapping,FILE_MAP_READ,0,0,0,
                                (PVOID)0x50000000);
      if (clBasePt == 0)
       { printf("Error in MapViewOfFile: %d",GetLastError());
         scanf("%c",&cInput);
         return(0);
       };

// This loop continuously traverses the linkes list, dereferencing the
// link pointers through the base implicitly.

      printf("Press q to quit, anything else to display list: ");
      scanf("%c",&cInput);
      while (cInput != 'q')
      {
      clRunning = 0;
      printf("Base address of file mapping in this process: 0x%x\n",(int)clBasePt);
      while (clRunning->clNext != 0)
       { printf ("Next Element in chain: %d\n",clRunning->iElement);
         clRunning = clRunning->clNext;
       };
      printf("Press q to quit, anything else to continue: ");
      scanf("%c",&cInput);
      };

// user didn't want any more dumps, so clean up.

      UnmapViewOfFile(clBasePt);
      CloseHandle(hFileMapping);
 return(0);
}






