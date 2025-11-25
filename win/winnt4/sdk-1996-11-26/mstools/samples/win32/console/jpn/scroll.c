
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

/* The following is the sample text that we will place on the lower
half of the screen to scroll: */

static PCHAR szSampTxt =
  "BOOL ScrollConsoleScreenBuffer(hConsoleOutput, lpScrollRectangle,\n"
  "                               lpClipRectangle, dwDestinationOrigin,\n"
  "                               lpFill)\n"
  "HANDLE hConsoleOutput;\n"
  "PSMALL_RECT lpScrollRectangle;\n"
  "PSMALL_RECT lpClipRectangle;\n"
  "COORD dwDestinationOrigin;\n"
  "PCHAR_INFO lpFill;\n"
  "\n"
  "このファンクションはスクリーン バッファのデータをスクロールするために使われ\n"
  "ます。\n"
  "\n"
  "パラメータ           説明\n"
  "----------------------------------------------------------------------------\n"
  "hConsoleOutput       コンソール出力へのオープンされたハンドルです。\n"
  "lpScrollRectangle    スクリーン バッファ内の、移動する領域へのポインタです。\n"
  "lpClipRectangle      スクリーン バッファ内の、スクロール移動することによって\n"
  "                     影響を受ける領域へのポインタです。このポインタは NULL\n"
  "                     かも知れません。\n"
  "dwDestinationOrigin  ScrollRectangle の左上角の新しい位置です。\n"
  "lpFill               ScrollRectangle の領域の新しい内容の構造体へのポインタ\n"
  "                     です。\n"
  "\n"
  "戻り値\n"
  "ファンクションが成功すれは戻り値は TRUE です。そうでない場合 FALSE を返し、\n"
  "この場合は、拡張エラー情報を GetLastError を呼ぶことによって得ることができ\n"
  "ます。\n"
  "\n"
  "コメント\n"
  "このファンクションはスクリーン バッファの矩形領域（スクロール領域）の内容を\n"
  "他のスクリーン バッファの領域（ターゲット領域）にコピーします。ターゲット領\n"
  "域はスクロール領域と同じ形の矩形として定義され、左上角で示されます。スクロー\n"
  "ル領域内のそれぞれのセルは FiLL の内容で満たされます。スクロール領域とターゲ\n"
  "ット領域の重なった部分については、なにも満たされません。スクロール矩形とター\n"
  "ゲット矩形のどちらの変更にもクリップ矩形が適用されます。もしクリップ矩形がス\n"
  "クロール矩形を含んでいないとき、スクロール矩形は更新されません。";


/*******************************************************************
* FUNCTION: demoScrollCon(HANDLE hConOut)                          *
*                                                                  *
* PURPOSE: demonstrate ScrollConsoleScreenBuffer. Scroll the lower *
*          half of the console with each mouse click               *
*                                                                  *
* INPUT: the console output handle to scroll                       *
********************************************************************/

void demoScrollCon(HANDLE hConOut)
{
  BOOL bSuccess;
  INPUT_RECORD inputBuffer;
  DWORD dwStdInMode;
  HANDLE hStdIn;
  DWORD dwInputEvents;
  COORD coordDest; /* destination of scroll movement */
  BOOL bDragMode = FALSE;
  CHAR_INFO chiFill; /* char and attributes to fill empty space with */
  CONSOLE_SCREEN_BUFFER_INFO csbi; /* used to get current attribute */
  SMALL_RECT srctScrollRect; /* area of the screen to scroll */

  setConTitle(__FILE__);
  myPuts(hConOut, "ScrollConsoleScreenBuffer を使って、マウスのクリックでスクリーン");
  myPuts(hConOut, "の下半分を上スクロールしましょう。スクリーンのサンプルのテキスト");
  myPuts(hConOut, "でその効果を見ることができます。続けるには Return を押してく");
  myPuts(hConOut, "ださい。ESC を押すといつでも戻ります。\n");
  myGetchar();
  myPuts(hConOut, szSampTxt);
  hStdIn = GetStdHandle(STD_INPUT_HANDLE);
  PERR(hStdIn != INVALID_HANDLE_VALUE, "GetStdHandle");
  bSuccess = GetConsoleMode(hStdIn, &dwStdInMode);
  PERR(bSuccess, "GetConsoleMode");
  bSuccess = SetConsoleMode(hStdIn, dwStdInMode | ENABLE_MOUSE_INPUT);
  PERR(bSuccess, "SetConsoleMode");
  /* define region we want to move */
  srctScrollRect.Top = getConY(hConOut) / 2;
  srctScrollRect.Bottom = getConY(hConOut) - 1;
  srctScrollRect.Left = 0;
  srctScrollRect.Right = getConX(hConOut) - 1;
  /* define origin where we want to move the scrolled region */
  coordDest.X = 0;
  coordDest.Y = (getConY(hConOut) / 2) - 1;
  /* get current attributes and fill out CHAR_INFO structure for fill char */
  bSuccess = GetConsoleScreenBufferInfo(hConOut, &csbi);
  PERR(bSuccess, "GetConsoleScreenBufferInfo");
  chiFill.Char.AsciiChar = ' ';
  chiFill.Attributes = csbi.wAttributes;
  for(;;)
    {
    bSuccess = ReadConsoleInput(hStdIn, &inputBuffer, 1, &dwInputEvents);
    PERR(bSuccess, "ReadConsoleInput");
    switch (inputBuffer.EventType)
      {
      case KEY_EVENT:
        if (inputBuffer.Event.KeyEvent.bKeyDown &&
            inputBuffer.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
          {
          /* set input mode back to what it was originally */
          bSuccess = SetConsoleMode(hStdIn, dwStdInMode);
          PERR(bSuccess, "SetConsoleMode");
          return;
          }
        break;
      case MOUSE_EVENT:
        /* was this was a click or double click event? Is any button down? */
        if (inputBuffer.Event.MouseEvent.dwEventFlags != MOUSE_MOVED &&
            inputBuffer.Event.MouseEvent.dwButtonState)
          {
          bSuccess = ScrollConsoleScreenBuffer(hConOut,
              &srctScrollRect,
              NULL, /* no clipping rectangle */
              coordDest, /* coordinates of destination */
              &chiFill); /* attribute to fill empty space with */
          PERR(bSuccess, "ScrollConsoleScreenBuffer");
          }
        break;
      } /* switch */
    } /* while */
}
