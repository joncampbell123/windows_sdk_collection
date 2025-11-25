
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
* FUNCTION: demoCursor(HANDLE hConOut)                               *
*                                                                    *
* PURPOSE: demonstrate GetConsoleCursorInfo, SetConsoleCursorInfo,   *
*          and SetConsoleCursorPosition. Show the current cursor     *
*          information, then have the cursor follow the mouse around *
*          the screen. Shrink the cursor size when the user clicks   *
*          one button, and grow the cursor when he clicks the other. *
*                                                                    *
* INPUT: the console output handle to write to and manipulate the    *
*        cursor for.                                                 *
*********************************************************************/

void demoCursor(HANDLE hConOut)
{
  BOOL bSuccess;
  /* to set initial size and visibility of cursor */
  CONSOLE_CURSOR_INFO cci;
  INPUT_RECORD inputBuffer;
  DWORD dwInputEvents;
  HANDLE hStdIn;
  CHAR szTemp[128];

  setConTitle(__FILE__);
  myPuts(hConOut, "GetConsoleCursorInfo で報告された\n"
                  "現在のカーソルの情報:\n");
  bSuccess = GetConsoleCursorInfo(hConOut, &cci);
  PERR(bSuccess, "GetConsoleCursorInfo");
  sprintf(szTemp, "カーソルのサイズ （最大値に対するパーセンテージ）: %d", cci.dwSize);
  myPuts(hConOut, szTemp);
  sprintf(szTemp, "カーソルの可視性: %s", cci.bVisible ? "可視" : "不可視");
  myPuts(hConOut, szTemp);
  myPuts(hConOut, "\nSetConsoleCursorPosition を使って、スクリーン内でマウス\n"
                  "ポインタについて行くコンソール カーソルにしてみましょう。\n"
                  "左のマウス ボタンがクリックされると SetConsoleCursorInfo \n"
                  "を使ってカーソルのサイズを１０％大きくします。右のマウス \n"
                  "ボタンがクリックされるとカーソルのサイズを１０％小さくし\n"
                  "ます。戻るには ESC を押してください。");
  hStdIn = GetStdHandle(STD_INPUT_HANDLE);
  for(;;)
    {
      /* get an input event */
    bSuccess = ReadConsoleInput(hStdIn, &inputBuffer, 1, &dwInputEvents);
    PERR(bSuccess, "ReadConsoleInput");
    switch (inputBuffer.EventType)
      {
      case KEY_EVENT:
        /* is it a key-down event? Is it an ESC char? If so return */
        if (inputBuffer.Event.KeyEvent.bKeyDown &&
            inputBuffer.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
          return;
        break;
      case MOUSE_EVENT:
        /* if the mouse moved draw the cursor at the mouse position */
        if (inputBuffer.Event.MouseEvent.dwEventFlags == MOUSE_MOVED)
          {
          bSuccess = SetConsoleCursorPosition(hConOut,
              inputBuffer.Event.MouseEvent.dwMousePosition);
          PERR(bSuccess, "SetConsoleCursorPosition");
          }
        /* if the mouse is clicked, increase/decrease cursor size. */
        /* Consider a double click a single click for this sample */
        if (!inputBuffer.Event.MouseEvent.dwEventFlags || /* a click */
            inputBuffer.Event.MouseEvent.dwEventFlags == DOUBLE_CLICK)
          {
          /* is the leftmost mouse button is down? If so, increase cursor */
          if (inputBuffer.Event.MouseEvent.dwButtonState ==
              FROM_LEFT_1ST_BUTTON_PRESSED)
            {
            /* if cursor size grows > 100, wrap around to small size */
            cci.dwSize = (cci.dwSize + 10) % 100 + 1;
            bSuccess = SetConsoleCursorInfo(hConOut, &cci);
            PERR(bSuccess, "SetConsoleCursorInfo");
            }
          /* is the rightmost button is down? */
          if (inputBuffer.Event.MouseEvent.dwButtonState ==
              RIGHTMOST_BUTTON_PRESSED)
            {
            /* if cursor size < 0, wrap around to large size */
            cci.dwSize -= 10;
            if ((int) cci.dwSize < 1)
              cci.dwSize = 100;
            bSuccess = SetConsoleCursorInfo(hConOut, &cci);
            PERR(bSuccess, "SetConsoleCursorInfo");
            }
          /* other buttons will be ignored */
          }
        break;
      } /* switch */
    }  /* while */
  return;
}
