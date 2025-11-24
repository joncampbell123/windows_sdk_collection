#include <windows.h>
#include "console.h"

/* Microsoft Developer Support
   Copyright (c) 1992-1995 Microsoft Corporation. All Rights Reserved */

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

static void resizeConBufAndWindow(HANDLE hConsole, SHORT xSize, SHORT ySize)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi; /* hold current console buffer info */
  BOOL bSuccess;
  SMALL_RECT srWindowRect; /* hold the new console size */
  COORD coordScreen;

  setConTitle(__FILE__);
  bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
  PERR(bSuccess, "GetConsoleScreenBufferInfo");
  /* define the new console size */
  srWindowRect.Left = srWindowRect.Top = (SHORT) 0;
  srWindowRect.Right = (SHORT) (xSize - 1);
  srWindowRect.Bottom = (SHORT) (ySize - 1);
  /* define the new console buffer size */
  coordScreen.X = xSize;
  coordScreen.Y = ySize;
  /* if the current buffer is larger than what we want, resize the */
  /* console window first, then the buffer */
  if (csbi.dwSize.X * csbi.dwSize.Y > xSize * ySize)
    {
    bSuccess = SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect);
    PERR(bSuccess, "SetConsoleWindowInfo");
    bSuccess = SetConsoleScreenBufferSize(hConsole, coordScreen);
    PERR(bSuccess, "SetConsoleScreenBufferSize");
    }
  /* if the current buffer is smaller than what we want, resize the */
  /* buffer first, then the console window */
  if (csbi.dwSize.X * csbi.dwSize.Y < xSize * ySize)
    {
    bSuccess = SetConsoleScreenBufferSize(hConsole, coordScreen);
    PERR(bSuccess, "SetConsoleScreenBufferSize");
    bSuccess = SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect);
    PERR(bSuccess, "SetConsoleWindowInfo");
    }
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

  myPuts(hConOut, "Let's resize the console buffer and window to a 40 x 25");
  myPuts(hConOut, "size screen by using the SetConsoleScreenBufferSize and");
  myPuts(hConOut, "SetConsoleWindowInfo APIs. Hit enter to continue...");
  myGetchar();
  sConX = getConX(hConOut);
  sConY = getConY(hConOut);
  resizeConBufAndWindow(hConOut, (SHORT) 40, (SHORT) 25);
  myPuts(hConOut, "Now let's resize back to our original");
  myPuts(hConOut, "size screen.");
  myPuts(hConOut, "Hit enter to continue...");
  myGetchar();
  resizeConBufAndWindow(hConOut, sConX, sConY);
  myPuts(hConOut, "Now we're back to our original size. Hit enter to return...");
  myGetchar();
  return;
}
