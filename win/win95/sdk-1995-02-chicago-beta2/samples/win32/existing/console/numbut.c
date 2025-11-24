#include <windows.h>
#include <stdio.h>
#include "console.h"

/* Microsoft Developer Support
   Copyright (c) 1992-1995 Microsoft Corporation. All Rights Reserved */

/*************************************************************
* FUNCTION: demoGetNumBut(HANDLE hConOut)                    *
*                                                            *
* PURPOSE: simply report the number of buttons on your mouse *
*                                                            *
* INPUT: the console output handle to write to               *
*************************************************************/

void demoGetNumBut(HANDLE hConOut)
{
  DWORD dwNumMouseButtons;
  BOOL bSuccess;
  CHAR szTemp[256];

  setConTitle(__FILE__);
  bSuccess=GetNumberOfConsoleMouseButtons(&dwNumMouseButtons);
  PERR(bSuccess, "GetNumberOfConsoleMouseButtons");
  myPuts(hConOut, "Using GetNumberOfConsoleMouseButtons to obtain the\n"
                  "number of buttons on your mouse...");
  sprintf(szTemp, "Your mouse has %d buttons.", dwNumMouseButtons);
  myPuts(hConOut, szTemp);
  myPuts(hConOut, "\nHit enter to return...");
  myGetchar();
  return;
}
