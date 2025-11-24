#include <windows.h>
#include "console.h"

/* Microsoft Developer Support
   Copyright (c) 1992-1995 Microsoft Corporation. All Rights Reserved */

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
  myPuts(hConOut, "Type a number of characters quickly. I will read 5\n"
                  "characters from the input buffer with a Sleep() delay\n"
                  "which will allow it to fill with characters. After 5\n"
                  "characters I will flush the input buffer with\n"
                  "FlushConsoleInputBuffer and restart the sequence. Note\n"
                  "that any characters you've typed that haven't been read\n"
                  "yet are lost due to the flush.\n"
                  "Enter characters (hit ESC to return):");
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
