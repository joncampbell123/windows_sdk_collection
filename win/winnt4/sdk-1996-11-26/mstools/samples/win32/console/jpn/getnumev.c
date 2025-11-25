
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
#include <string.h>
#include "console.h"

/* maximum number of input queue events to peek at */
#define INPUT_RECS 256

/* array of records to store peeked events from the input queue */
INPUT_RECORD aInputBuffer[INPUT_RECS];

/*********************************************************************
* FUNCTION: demoGetNumEvents(HANDLE hConOut)                         *
*                                                                    *
* PURPOSE: demonstrate GetNumberOfConsoleInputEvents,                *
*          PeekConsoleInput, and ReadConsoleInput. Delay the         *
*          processing of console input to start filling the input    *
*          queue. The number of console events in the input queue    *
*          will be updated in the status line at the top of the      *
*          console. Peek the unread characters for an ESC and return *
*          when one is found                                         *
*                                                                    *
* INPUT: console output handle to write to                           *
*********************************************************************/

void demoGetNumEvents(HANDLE hConOut)
{
  BOOL bSuccess;
  DWORD dwNumEvents; /* number of events in the input queue */
  DWORD dwStdInMode; /* save the input mode here */
  HANDLE hStdIn;
  DWORD dwInputEvents; /* number of events read from the queue */
  CHAR bOutBuf[256], szTemp[256];
  /* indexes to latest unread event checked for ESC char */
  DWORD iEvent, iPrevEvent;
  DWORD dwEventsPeeked; /* number of events peeked at */
  unsigned i;
  DWORD dwCharsWritten;

  setConTitle(__FILE__);
  hStdIn = GetStdHandle(STD_INPUT_HANDLE);
  PERR(hStdIn != INVALID_HANDLE_VALUE, "GetStdHandle");
  myPuts(hConOut, "\nGetNumberOfConsoleInputEvents API を使ってコンソール\n"
                  "入力キューの中で待っているイベントの数を表示しましょ\n"
                  "う。このコンソールではマウス入力を可能にします。マウ\n"
                  "スとキーボードのイベントをたくさん発生させてみてくだ\n"
                  "さい。ReadConsoleInput を使って０．５秒ごとにそれらの\n"
                  "イベントを入力キューから読み込みます。イベント数とイ\n"
                  "ベント情報は上の行に表示されます。\n\n"
                  "ESC を押せば、いつでも戻れます。読み込まれていない入\n"
                  "力キューの中に ESC があるかどうかを見るために\n"
                  "PeekConsoleInput を使っています。ESC が見つかったら、\n"
                  "入力キューの中味を破棄して、直ちに戻ります。デモを簡\n"
                  "単にするため、ESC の検索は、読み込まれていないイベン\n"
                  "トのうち、最初の２５６個の中で行なわれることに注意し\n"
                  "てください。\n\n");

  bSuccess = GetConsoleMode(hStdIn, &dwStdInMode);
  PERR(bSuccess, "GetConsoleMode");
  /* when turning off ENABLE_LINE_INPUT, you MUST also turn off */
  /* ENABLE_ECHO_INPUT. */
  bSuccess = SetConsoleMode(hStdIn, (dwStdInMode & ~(ENABLE_LINE_INPUT |
      ENABLE_ECHO_INPUT)) | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
  PERR(bSuccess, "SetConsoleMode");
  iEvent = 0; /* index to last event peeked in the input queue */
  for(;;)
    {
    Sleep(500);
    bSuccess = GetNumberOfConsoleInputEvents(hStdIn, &dwNumEvents);
    PERR(bSuccess, "GetNumberOfConsoleInputEvents");
    sprintf(bOutBuf, "入力キューのイベント: %d", dwNumEvents);
    if (!dwNumEvents)
      /* put a status line on the first line */
      putStatusLine(hConOut, bOutBuf);
    else
      {
      /* save the previous index we've peeked at */
      iPrevEvent = iEvent;
      /* peek at the console input queue. Don't peek more than what will */
      /* fit in the buffer */
      bSuccess = PeekConsoleInput(hStdIn, aInputBuffer, min(dwNumEvents,
          INPUT_RECS), &dwEventsPeeked);
      PERR(bSuccess, "PeekConsoleInput");
      /* set current index to the highest number event peeked at */
      iEvent = dwEventsPeeked;
      /* scan unread events for an ESC key */
      for (i = iPrevEvent; i < iEvent; i++)
        {
        if (aInputBuffer[i].EventType == KEY_EVENT &&
            aInputBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
          {
          /* set input mode back to what it was originally */
          bSuccess = SetConsoleMode(hStdIn, dwStdInMode);
          PERR(bSuccess, "SetConsoleMode");
          /* flush the input buffer and return */
          bSuccess = FlushConsoleInputBuffer(hStdIn);
          PERR(bSuccess, "FlushConsoleInputBuffer");
          return;
          }
        } /* for */
      } /* else */
    bSuccess = ReadConsoleInput(hStdIn, &aInputBuffer[0], 1, &dwInputEvents);
    PERR(bSuccess, "ReadConsoleInput");
    /* decrement "last peeked at" index by number of records we just read */
    iEvent -= dwInputEvents;
    switch (aInputBuffer[0].EventType)
      {
      case KEY_EVENT:
        if (aInputBuffer[0].Event.KeyEvent.bKeyDown)
          {
          if (aInputBuffer[0].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
            {
            /* set input mode back to what it was originally */
            bSuccess = SetConsoleMode(hStdIn, dwStdInMode);
            PERR(bSuccess, "SetConsoleMode");
            return;
            }
          /* put the keystroke information on the status line */
          sprintf(szTemp, " キー ストローク: %c",
              aInputBuffer[0].Event.KeyEvent.uChar.AsciiChar);
          strcat(bOutBuf, szTemp);
          /* put the status line on the screen */
          putStatusLine(hConOut, bOutBuf);
          /* output the character read from the input queue */
          bSuccess = WriteFile(hConOut,
              &aInputBuffer[0].Event.KeyEvent.uChar.AsciiChar, 1,
              &dwCharsWritten, NULL);
          PERR(bSuccess, "WriteFile");
          }
        break;
      case MOUSE_EVENT:
        sprintf(szTemp, " マウス: %d, %d で %s",
            aInputBuffer[0].Event.MouseEvent.dwMousePosition.X,
            aInputBuffer[0].Event.MouseEvent.dwMousePosition.Y,
            (aInputBuffer[0].Event.MouseEvent.dwEventFlags == MOUSE_MOVED) ?
            "移動" : "クリック");
        strcat(bOutBuf, szTemp);
        putStatusLine(hConOut, bOutBuf);
        break;
      case WINDOW_BUFFER_SIZE_EVENT:
        sprintf(szTemp, " ウィンドウ: %d, %d",
            aInputBuffer[0].Event.WindowBufferSizeEvent.dwSize.X,
            aInputBuffer[0].Event.WindowBufferSizeEvent.dwSize.Y);
        strcat(bOutBuf, szTemp);
        putStatusLine(hConOut, bOutBuf);
        break;
      } /* switch */
    } /* while */
}
