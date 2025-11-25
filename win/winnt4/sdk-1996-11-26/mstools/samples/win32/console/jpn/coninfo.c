
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
#include <conio.h>
#include "console.h"

/*******************************************************************
* FUNCTION: demoConInfo(HANDLE hConOut)                            *
*                                                                  *
* PURPOSE: demonstrate GetConsoleScreenBufferInfo. Get the current *
*          console screen buffer information and display it on the *
*          console.                                                *
*                                                                  *
* INPUT: console handle to output to                               *
*******************************************************************/

void demoConInfo(HANDLE hConOut)
{
  BOOL bSuccess;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  CHAR szTemp[128];

  setConTitle(__FILE__);
  myPuts(hConOut, "コンソール バッファの情報を得るためには\n"
                  "GetConsoleScreenBufferInfo を呼ぶ必要があります。\n"
                  "スクリーン バッファ/コンソール ウィンドウを調整\n"
                  "して、何かキーを押してください...\n");
  myGetchar();
  bSuccess = GetConsoleScreenBufferInfo(hConOut, &csbi);
  PERR(bSuccess, "GetConsoleScreenBufferInfo");
  myPuts(hConOut, "\nCONSOLE_SCREEN_BUFFER_INFO構造体に返された\n"
                  "バッファの内容です:\n");
  sprintf(szTemp, "サイズ: X = %d, Y = %d", csbi.dwSize.X, csbi.dwSize.Y);
  myPuts(hConOut, szTemp);
  sprintf(szTemp, "カーソル位置: X = %d, Y = %d", csbi.dwCursorPosition.X,
      csbi.dwCursorPosition.Y);
  myPuts(hConOut, szTemp);
  sprintf(szTemp, "文字表示アトリビュート: 0x%04x", csbi.wAttributes);
  myPuts(hConOut, szTemp);
  sprintf(szTemp, "ウィンドウ位置: 左 = %d, 右 = %d, 上 = %d, "
      "下 = %d", csbi.srWindow.Left, csbi.srWindow.Top,
      csbi.srWindow.Right, csbi.srWindow.Bottom);
  myPuts(hConOut, szTemp);
  sprintf(szTemp, "最大ウィンドウサイズ: X = %d, Y = %d",
      csbi.dwMaximumWindowSize.X, csbi.dwMaximumWindowSize.Y);
  myPuts(hConOut, szTemp);
  myPuts(hConOut, "\n戻るには、何かキーを押してください...");
  myGetchar();
  return;
}
