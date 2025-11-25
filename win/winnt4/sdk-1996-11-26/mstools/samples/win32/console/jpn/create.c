
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
* FUNCTION: demoCreate(HANDLE hConOld)                               *
*                                                                    *
* PURPOSE: demonstrate CreateConsoleScreenBuffer,                    *
*          SetConsoleTextAttribute, and                              *
*          SetConsoleActiveScreenBuffer. Create a 'help' screen on a *
*          new buffer and quickly switch between the main buffer and *
*          the help buffer on command without redrawing the entire   *
*          screens. SetConsoleTextAttribute will be used to make the *
*          help screen a different color.                            *
*                                                                    *
* INPUT: the output handle to write to                               *
**********************************************************************/

void demoCreate(HANDLE hConOld)
{
  BOOL bSuccess;
  HANDLE hConHelp; /* console for the help screen */
  COORD dwWriteCoord = {0, 0}; /* where to write the screen attributes */
  DWORD cCharsWritten;
  HANDLE hStdIn; /* standard input handle */
  CHAR c = 0; /* virtual key code that we will read */
  HANDLE hConCurrent; /* keep track of the current visible console buffer */
  INPUT_RECORD inputBuf; /* console input event record */
  DWORD cInputEvents;

  setConTitle(__FILE__);
  myPuts(hConOld, "ヘルプ スクリーンを他のバッファに作成しましょう。\n"
                  "ユーザが F1 を押したとき、単にアクティブなバッファを変更\n"
                  "するだけで、簡単に現在のスクリーンとヘルプのスクリーンを入れ\n"
                  "換えることができます。戻すときも現在のスクリーンの文字を再構\n"
                  "築することなしに、単にアクティブなバッファを変更するだけです。\n"
                  "ヘルプのスクリーンを作成する前に SetConsoleTextAttribute で\n"
                  "デフォルトのテキスト アトリビュートを変更しますので、そのサ\n"
                  "ンプル テキストは違った色になります。F1 を押してヘルプを\n"
                  "表示し、ESC でここに戻ってください。このスクリーンで ESC を\n"
                  "押すと、API リストに戻ります。");
  /* create a separate console buffer for the help screen */
  hConHelp = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
      FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER,
      NULL);
  PERR(hConHelp != INVALID_HANDLE_VALUE, "CreateConsoleScreenBuffer");
  /* change the color of the help screen */
  bSuccess = FillConsoleOutputAttribute(hConHelp, BACKGROUND_BLUE,
      getConX(hConHelp) * getConY(hConHelp), dwWriteCoord, &cCharsWritten);
  PERR(bSuccess, "FillConsoleOutputAttribute");
  /* set the color for future text output */
  bSuccess = SetConsoleTextAttribute(hConHelp, FOREGROUND_YELLOW |
      FOREGROUND_INTENSITY | BACKGROUND_BLUE);
  PERR(bSuccess, "SetConsoleTextAttribute");
  myPuts(hConHelp, "                 超だまされやすい人の基本ヘルプ\n"
               "\n\n                  F1: 検索        F6: ウィンドウ\n"
                   "                  F2: 抽出        F7: 初期化\n"
                   "                  F3: ソート      F8: 終了\n"
                   "                  F4: 問い合わせ  F9: 保管\n"
                   "                  F5: 実行       F10: 爆発\n"
                   "                  ESC: ヘルプ終了");
  hStdIn = GetStdHandle(STD_INPUT_HANDLE);
  PERR(hStdIn != INVALID_HANDLE_VALUE, "GetStdHandle");
  /* keep track of the currently visible console */
  hConCurrent = hConOld;
  /* switch between the help and previous buffer when user hits F1 or ESC */
  for(;;)
    {
    do
      {
      /* throw away any non-keystroke events or any key-up events */
      bSuccess = ReadConsoleInput(hStdIn, &inputBuf, 1, &cInputEvents);
      PERR(bSuccess, "ReadConsoleInput");
      } while (inputBuf.EventType != KEY_EVENT ||
          !inputBuf.Event.KeyEvent.bKeyDown);
    /* get the virtual scan code of the key-down event */
    c = (char) inputBuf.Event.KeyEvent.wVirtualKeyCode;
    switch(c)
      {
      case VK_F1:
        /* if the current buffer is the original buffer, switch to the */
        /* help buffer */
        if (hConCurrent == hConOld)
          {
          bSuccess = SetConsoleActiveScreenBuffer(hConHelp);
          PERR(bSuccess, "SetConsoleActiveScreenBuffer");
          hConCurrent = hConHelp;
          }
        break;
      case VK_ESCAPE:
        /* if the current buffer is the help buffer, switch to the */
        /* original buffer. Otherwise, clean up and return */
        if (hConCurrent == hConHelp)
          {
          bSuccess = SetConsoleActiveScreenBuffer(hConOld);
          PERR(bSuccess, "SetConsoleActiveScreenBuffer");
          hConCurrent = hConOld;
          }
        else
          {
          CloseHandle(hConHelp);
          return;
          }
        break;
      case VK_F10:
        if (hConCurrent == hConHelp)
          myPuts(hConHelp, "ドッカーン!");
        break;
      default:
        break;
      }  /* switch */
    }  /* while */
  return;
}
