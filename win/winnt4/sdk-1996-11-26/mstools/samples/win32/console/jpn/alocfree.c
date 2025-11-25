
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993-1995 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

#include <windows.h>
#include <stdio.h>
#include "console.h"

/*********************************************************************
* FUNCTION: demoAllocFree(HANDLE hConOld, HANDLE *hConOut)           *
*                                                                    *
* PURPOSE: demonstrate FreeConsole & AllocConsole. Free the console  *
*          and allocate a new one                                    *
*                                                                    *
* INPUT: the current console output handle and a temporary 'scratch' *
*        console handle                                              *
*********************************************************************/

void demoAllocFree(HANDLE hConOld, HANDLE *hConOut)
{
  BOOL bSuccess;

  setConTitle(__FILE__);
  myPuts(hConOld, "現在のコンソールを FreeConsole で解放しましょう。\n"
                  "次に、３秒間待ってから AllocConsole で新しいものを\n"
                  "作成します。\n"
                  "続けるには Enter を押してください...\n");
  myGetchar();
  bSuccess = FreeConsole();
  PERR(bSuccess, "FreeConsole");
  Sleep(3000);
  bSuccess = AllocConsole();
  PERR(bSuccess, "AllocConsole");
  *hConOut = GetStdHandle(STD_OUTPUT_HANDLE);
  /* set our console text attribute */
  bSuccess = SetConsoleTextAttribute(*hConOut, FOREGROUND_CYAN);
  PERR(bSuccess, "SetConsoleTextAttribute");
  myPuts(*hConOut, "これは AllocConsole で作成した新しいコンソールです。");
  Sleep(2000);
  /* must refresh the screen since we've replaced the console */
  showConAPIs(*hConOut);
  return;
}
