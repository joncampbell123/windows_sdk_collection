
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
{ HANDLE hFileMapping;
  int iCounter;
  int iNextElement;
  int iInternalCounter = 0;

// Set up the shared memory. By using MapViewOfFileEx(), we force the
// memory onto a specific location. This is just to show how based
// pointers work for memory that is mapped differenctly in two processes.

      hFileMapping = CreateFileMapping((HANDLE)0xffffffff,NULL,PAGE_READWRITE,0,4096,
                                       "DemoMapObject");
      if (GetLastError() == ERROR_ALREADY_EXISTS)
         { printf("Please close readdata before running this application!");
           return(0);
         };
      if (!hFileMapping)
         { printf ("Failed on CreateFileMapping...");
           return(0);
         };
      clBasePt = (CHAINLIST *)MapViewOfFileEx(hFileMapping,FILE_MAP_ALL_ACCESS,0,0,0,
                                (PVOID)0x40000000);

// Now the memory is set up. The following loops prompts the user for new
// elements until a 0 is hit. The memory is displayed so that we see that
// the offsets are entered into the link fields, ignoring the based
// pointers.

      printf("Base address of file mapping in this process: 0x%x\n",(int)clBasePt);
      printf("Enter element to insert; 0 terminates: ");
      scanf("%d",&iNextElement);
      while (iNextElement != 0)
       { InsertElement(iNextElement);
         iInternalCounter+= sizeof(CHAINLIST);
         printf ("Now displaying raw data at address: 0x%x\n",(int)clBasePt);
         printf ("You might want to run readdata.exe now.\n");
         for (iCounter = 0; iCounter<=iInternalCounter;iCounter++)
           printf("%2x ",((char *)clBasePt)[iCounter]);
         printf("\n");
         printf("Enter next element to insert; 0 terminates: ");
         scanf("%d",&iNextElement);
       };

// user has entered 0 -- now clean up.

      UnmapViewOfFile(clBasePt);
      CloseHandle(hFileMapping);
  return(0);
}













