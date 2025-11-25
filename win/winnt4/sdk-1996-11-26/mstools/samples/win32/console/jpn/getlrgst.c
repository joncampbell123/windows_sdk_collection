
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
* FUNCTION: demoGetLargest(HANDLE hConOut)                           *
*                                                                    *
* PURPOSE: demonstrate GetLargestConsoleWindowSize. Reports the size *
*          of the largest possible console window, given the current *
*          font.                                                     *
*                                                                    *
* INPUT: console input handle to query the information from and to   *
*        output to                                                   *
*********************************************************************/

void demoGetLargest(HANDLE hConOut)
{
  COORD coordLargest; /* hold the largest window size */
  CHAR szTemp[128];

  setConTitle(__FILE__);
  coordLargest = GetLargestConsoleWindowSize(hConOut);
  PERR(coordLargest.X | coordLargest.Y, "GetLargestConsoleWindowSize");
  myPuts(hConOut, "GetLargestConsoleWindowSize より報告された\n"
                  "コンソール ウィンドウの最大値は:");
  sprintf(szTemp, "幅 %d  高さ %d", coordLargest.X, coordLargest.Y);
  myPuts(hConOut, szTemp);
  myPuts(hConOut, "\n戻るには Return を押してください...");
  myGetchar();
  return;
}
