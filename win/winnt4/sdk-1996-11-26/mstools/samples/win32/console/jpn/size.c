
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
#include "console.h"

/*********************************************************************
* FUNCTION: resizeConBufAndWindow(HANDLE hConsole, SHORT xSize,      *
*                                 SHORT ySize)                       *
*                                                                    *
* PURPOSE: resize both the console output buffer and the console     *
*          window to the given x and y size parameters               *
*                                                                    *
* INPUT: the console output handle to resize, and the required x and *
*        y size to resize the buffer and window to.                  *
*                                                                    *
* COMMENTS: Note that care must be taken to resize the correct item  *
*           first; you cannot have a console buffer that is smaller  *
*           than the console window.                                 *
*********************************************************************/

void resizeConBufAndWindow(HANDLE hConsole, SHORT xSize, SHORT ySize)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi; /* hold current console buffer info */
  BOOL bSuccess;
  SMALL_RECT srWindowRect; /* hold the new console size */
  COORD coordScreen;

  bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
  PERR(bSuccess, "GetConsoleScreenBufferInfo");
  /* get the largest size we can size the console window to */
  coordScreen = GetLargestConsoleWindowSize(hConsole);
  PERR(coordScreen.X | coordScreen.Y, "GetLargestConsoleWindowSize");
  /* define the new console window size and scroll position */
  srWindowRect.Right = (SHORT) (min(xSize, coordScreen.X) - 1);
  srWindowRect.Bottom = (SHORT) (min(ySize, coordScreen.Y) - 1);
  srWindowRect.Left = srWindowRect.Top = (SHORT) 0;
  /* define the new console buffer size */
  coordScreen.X = xSize;
  coordScreen.Y = ySize;
  /* if the current buffer is larger than what we want, resize the */
  /* console window first, then the buffer */
  if ((DWORD) csbi.dwSize.X * csbi.dwSize.Y > (DWORD) xSize * ySize)
    {
    bSuccess = SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect);
    PERR(bSuccess, "SetConsoleWindowInfo");
    bSuccess = SetConsoleScreenBufferSize(hConsole, coordScreen);
    PERR(bSuccess, "SetConsoleScreenBufferSize");
    }
  /* if the current buffer is smaller than what we want, resize the */
  /* buffer first, then the console window */
  if ((DWORD) csbi.dwSize.X * csbi.dwSize.Y < (DWORD) xSize * ySize)
    {
    bSuccess = SetConsoleScreenBufferSize(hConsole, coordScreen);
    PERR(bSuccess, "SetConsoleScreenBufferSize");
    bSuccess = SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect);
    PERR(bSuccess, "SetConsoleWindowInfo");
    }
  /* if the current buffer *is* the size we want, don't do anything! */
  return;
}


/*********************************************************************
* FUNCTION: demoSizeInfo(HANDLE hConOut)                             *
*                                                                    *
* PURPOSE: demonstrate SetConsoleWindowInfo and                      *
*          SetConsoleScreenBufferSize. Resize the console buffer and *
*          window                                                    *
*                                                                    *
* INPUT: console output handle to set the information for            *
*********************************************************************/

void demoSizeInfo(HANDLE hConOut)
{
  SHORT sConX, sConY; /* save the current console dimensions */

  setConTitle(__FILE__);
  myPuts(hConOut, "コンソール バッファとウィンドウのサイズを SetConsoleScreenBufferSize と\n"
                  "SetConsoleWindowInfo API を使って 40×25 の大きさに変えてみましょう。\n"
                  "続けるには Enter を押してください...\n");
  myGetchar();
  sConX = getConX(hConOut);
  sConY = getConY(hConOut);
  resizeConBufAndWindow(hConOut, (SHORT) 40, (SHORT) 25);
  myPuts(hConOut, "それでは、200×200 の大きいサイズに変更してみましょう。\n"
                  "... コンソール ウィンドウのサイズは物理的なスクリーンのサイズより大きく\n"
                  "ならないことがわかります。続けるには Enter を押してください...\n");
  myGetchar();
  resizeConBufAndWindow(hConOut, (SHORT) 200, (SHORT) 200);
  myPuts(hConOut, "ここでスクリーンを元のサイズに戻しましょう。\n"
                  "続けるには Enter を押してください...\n");
  myGetchar();
  resizeConBufAndWindow(hConOut, sConX, sConY);
  myPuts(hConOut, "元の大きさに戻りました。戻るには Enter を押してください...");
  myGetchar();
  return;
}
