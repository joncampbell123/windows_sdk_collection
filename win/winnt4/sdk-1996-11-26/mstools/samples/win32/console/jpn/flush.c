
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
* FUNCTION: demoFlush(HANDLE hConOut)                                *
*                                                                    *
* PURPOSE: demonstrate FlushConsoleInputBuffer. Slowly read from the *
*          iput queue, allowing a backlog of input events to start   *
*          filling the queue. Flush the input queue after outputting *
*          every fifth character.                                    *
*                                                                    *
* INPUT: the output console handle to write to                       *
*********************************************************************/

void demoFlush(HANDLE hConOut)
{
  HANDLE hStdIn;
  INPUT_RECORD InputBuffer;
  DWORD dwInputEvents;
  int i = 0;
  BOOL bSuccess;
  DWORD dwBytesWritten;

  setConTitle(__FILE__);
  hStdIn = GetStdHandle(STD_INPUT_HANDLE);
  PERR(hStdIn != INVALID_HANDLE_VALUE, "GetStdHandle");
  myPuts(hConOut, "６文字を超える文字を入力すばやく入力してください。Sleep() を\n"
                  "使って文字をゆっくり読み込みます。６文字を超えて入力された\n"
                  "文字は、FlushConsoleInputBuffer で破棄し、また最初から始め\n"
                  "ます。破棄されたことによって入力された文字が失われ、読み込\n"
                  "まれないことに注目してください。\n"
                  "文字を入力してください。（戻るには ESC を押してください。）:");
  for(;;)
    {
    bSuccess = ReadConsoleInput(hStdIn, &InputBuffer, 1, &dwInputEvents);
    PERR(bSuccess, "ReadConsoleInput");
    /* is it a key down event? */
    if (InputBuffer.EventType == KEY_EVENT && 
        InputBuffer.Event.KeyEvent.bKeyDown)
      {
      if (InputBuffer.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
        return;
      /* write the ascii character out to the console */
      bSuccess = WriteFile(hConOut,
          &InputBuffer.Event.KeyEvent.uChar.AsciiChar,
          1, &dwBytesWritten, NULL);
      PERR(bSuccess, "WriteFile");
      Sleep(1000); /* pause for 1s */
      i++;
      if (i > 5)
        {
        /* flush the input buffer */
        bSuccess = FlushConsoleInputBuffer(hStdIn);
        PERR(bSuccess, "FlushConsoleInputBuffer");
        i = 0;
        }
      } /* if */
    } /* while */
  return;
}
