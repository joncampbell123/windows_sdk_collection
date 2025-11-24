//THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
//ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
//THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//
// Copyright (c) 1992-1995  Microsoft Corporation.  All Rights Reserved.
//                 
//	MODULE: 	console.c
//
//	PURPOSE:	Demostates use of Console API support.
//
//	PLATFORMS:	Windows 95
//
//	FUNCTIONS:	
//      myGetchar   -   Get char from console
//      perr        -   report API errors. Allocate a new console buffer, 
//                      display error number and error text, restore 
//                      previous console buffer
//      myPuts      -   write a string to the given console buffer, 
//                      appending a cr/lf
//      getConX     -   to get the current width of the console 
//                      output buffer
//      getConY     -   to get the current height of the console 
//                      output buffer
//      showConAPIs -   to display a list of console APIs on the 
//                      given console buffer
//      putStatusLine-  display a string on the top line of the 
//                      console output buffer
//      demoAPI     -   call the correct demo function based 
//                      on the input index
//      cls         -   clear the screen by filling it with 
//                      blanks, then home cursor
//      setConTitle -   simply set the current console title. 
//                      Called by each demo function to 
//                      specify the name of the source 
//                      file that contains the demo function.
//      main        -   main input loop. Set up console 
//                      input and output attributes, then read the 
//                      input queue and process input events. If 
//                      ESC is hit, end the process. If the 
//                      mouse is clicked, read the line from 
//                      the output console where the mouse was clicked. 
//                      If an index number can be found at the 
//                      beginning of the line, call the appropriate 
//                      demo function to demonstrate that API. 
//                      All input event information is displayed 
//                      on the status line on the top line.
//
//	SPECIAL INSTRUCTIONS: N/A
//

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "console.h"
          

/* used to set our initial console screen buffer size */
#ifdef DBCS
int CONX=80;
int CONY=43;
#else
#define CONX 80
#define CONY 43
#endif

/* prototypes of externally defined demo functions */
extern void demoAllocFree(HANDLE hConOld, HANDLE *hConsole);
extern void demoCreate(HANDLE hConOld);
extern void demoFillAtt(HANDLE hConOut);
extern void demoFillChar(HANDLE hConOut);
extern void demoFlush(HANDLE hConOut);
extern void demoCursor(HANDLE hConOut);
extern void demoConMode(HANDLE hConOut);
extern void demoConInfo(HANDLE hConOut);
extern void demoGetTitle(HANDLE hConOut);
extern void demoGetLargest(HANDLE hConOut);
extern void demoGetNumEvents(HANDLE hConOut);
extern void demoGetNumBut(HANDLE hConOut);
extern void demoReadConOut(HANDLE hConOut);
extern void demoReadConChar(HANDLE hConOut);
extern void demoScrollCon(HANDLE hConOut);
extern void demoSizeInfo(HANDLE hConOut);
extern void demoSetCtrlHandler(HANDLE hConOut);
extern void demoWriteIn(HANDLE hConOut);
extern void demoCodePage(HANDLE hConOut);

/* information to display on the screen for user to click on */
PCHAR conAPIs[] = {
  "AllocConsole                    Creates a console for the current process",
  "CreateConsoleScreenBuffer       Returns a handle to a new screen buffer",
  "FillConsoleOutputAttribute      Writes attributes to the screen buffer",
  "FillConsoleOutputCharacter      Writes characters to the screen buffer",
  "FlushConsoleInputBuffer         Clears the console input buffer",
  "FreeConsole                     Frees the current console",
  "GenerateConsoleCtrlEvent        Generates a console control event",
  "GetConsoleCP                    Get current console input codepage",
  "GetConsoleOutputCP              Get current console output codepage",
  "GetConsoleCursorInfo            Returns console size and visibility",
  "GetConsoleMode                  Returns console input or output mode",
  "GetConsoleScreenBufferInfo      Returns screen-buffer information",
  "GetConsoleTitle                 Returns console-window title",
  "GetLargestConsoleWindowSize     Returns largest possible window size",
  "GetNumberOfConsoleInputEvents   Retrieves number of console-queue events",
  "GetNumberOfConsoleMouseButtons  Retrieves number of mouse buttons",
  "PeekConsoleInput                Previews console input data",
  "ReadConsoleInput                Reads console input data",
  "ReadConsoleOutput               Reads screen-buffer data",
  "ReadConsoleOutputAttribute      Reads a console attribute string",
  "ReadConsoleOutputCharacter      Reads a screen-buffer string",
  "ScrollConsoleScreenBuffer       Scrolls data in the screen buffer",
  "SetConsoleActiveScreenBuffer    Changes displayed screen buffer",
  "SetConsoleCP                    Sets console input codepage",
  "SetConsoleOutputCP              Sets console output codepage",
  "SetConsoleCursorInfo            Sets cursor size and visibility",
  "SetConsoleCursorPosition        Sets cursor position",
  "SetConsoleMode                  Sets console input or output mode",
  "SetConsoleScreenBufferSize      Changes screen-buffer size",
  "SetConsoleTextAttribute         Sets attributes for screen text",
  "SetConsoleTitle                 Sets console-window title string",
  "SetConsoleWindowInfo            Sets console window size",
  "SetConsoleCtrlHandler           Sets console ctrl-c handler",
  "WriteConsoleInput               Writes to console input buffer",
  "WriteConsoleOutput              Writes to screen buffer",
  "WriteConsoleOutputAttribute     Writes an attribute string to console",
  "WriteConsoleOutputCharacter     Writes a character string to console"
};

/* this variable holds the number next to the API on the screen that */
/* the user clicks on */
enum cAPIs { ALLOC = 1, CREATE, FILLATT, FILLCHAR, FLUSH, FREE,
  GENCTRL, GETCP, GETOUTCP, GETCUR, GETMODE, GETCONINFO, GETTITLE,
  GETLARGEST, GETNUMEV, GETNUMBUT, PEEK, READCONIN, READCONOUT,
  READCONATT, READCONCHAR, SCROLL, SETACTIVE, SETCP, SETOUTCP,
  SETCURINF, SETCURPOS, SETMODE, SETSIZE, SETATT, SETTITLE, SETINFO,
  SETHAND, WRITEIN, WRITEOUT, WRITEATT, WRITECHAR };


/*****************************************************************
* FUNCTION: myGetchar(void)                                      *
*                                                                *
* PURPOSE: get a single character from the standard input handle *
*                                                                *
* INPUT: none                                                    *
*                                                                *
* RETURNS: the char received from the console                    *
*****************************************************************/

CHAR myGetchar(void)
{
  HANDLE hStdIn; /* standard input */
  DWORD dwInputMode; /* to save the input mode */
  BOOL bSuccess;
  CHAR chBuf; /* buffer to read into */
  DWORD dwRead;

  /* get the standard input handle to read from. There is only one */
  /* instance of standard input per process at any given time */
  hStdIn = GetStdHandle(STD_INPUT_HANDLE);
  PERR(hStdIn != INVALID_HANDLE_VALUE, "GetStdHandle");
  /* save the console mode */
  bSuccess = GetConsoleMode(hStdIn, &dwInputMode);
  PERR(bSuccess, "GetconsoleMode");
  /* disable line input. Echo input must be disabled when disabling */
  /* line input */
  bSuccess = SetConsoleMode(hStdIn, dwInputMode & ~ENABLE_LINE_INPUT &
      ~ENABLE_ECHO_INPUT);
  PERR(bSuccess, "SetConsoleMode");
  /* read a character from the console input */
  bSuccess = ReadFile(hStdIn, &chBuf, sizeof(chBuf), &dwRead, NULL);
  PERR(bSuccess, "ReadFile");
  /* restore the original console input mode */
  bSuccess = SetConsoleMode(hStdIn, dwInputMode);
  PERR(bSuccess, "SetConsoleMode");
  return(chBuf);
}

/*********************************************************************
* FUNCTION: perr(PCHAR szFileName, int line, PCHAR szApiName,        *
*                DWORD dwError)                                      *
*                                                                    *
* PURPOSE: report API errors. Allocate a new console buffer, display *
*          error number and error text, restore previous console     *
*          buffer                                                    *
*                                                                    *
* INPUT: current source file name, current line number, name of the  *
*        API that failed, and the error number                       *
*                                                                    *
* RETURNS: none                                                      *
*********************************************************************/

/* maximum size of the buffer to be returned from FormatMessage */
#define MAX_MSG_BUF_SIZE 512

void perr(PCHAR szFileName, int line, PCHAR szApiName, DWORD dwError)
{
  CHAR szTemp[1024];
  DWORD cMsgLen;
  CHAR *msgBuf; /* buffer for message text from system */
  int iButtonPressed; /* receives button pressed in the error box */

  /* format our error message */
  sprintf(szTemp, "%s: Error %d from %s on line %d:\n", szFileName,
      dwError, szApiName, line);
  /* get the text description for that error number from the system */
  cMsgLen = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 40, NULL, dwError,
      MAKELANGID(0, SUBLANG_ENGLISH_US), (LPTSTR) &msgBuf, MAX_MSG_BUF_SIZE,
      NULL);
  if (!cMsgLen)
    sprintf(szTemp + strlen(szTemp), "Unable to obtain error message text! \n"
        "%s: Error %d from %s on line %d", __FILE__,
        GetLastError(), "FormatMessage", __LINE__);
  else
    strcat(szTemp, msgBuf);
  strcat(szTemp, "\n\nContinue execution?");
  MessageBeep(MB_ICONEXCLAMATION);
  iButtonPressed = MessageBox(NULL, szTemp, "Console API Error",
      MB_ICONEXCLAMATION | MB_YESNO | MB_SETFOREGROUND);
  /* free the message buffer returned to us by the system */
  if (cMsgLen)
    LocalFree((HLOCAL) msgBuf);
  if (iButtonPressed == IDNO)
    exit(1);
  return;
}

/*************************************************************************
* FUNCTION: myPuts(HANDLE hConsole, PCHAR s)                             *
*                                                                        *
* PURPOSE: write a string to the given console buffer, appending a cr/lf *
*                                                                        *
* INPUT: the console to write to, and the string to write                *
*                                                                        *
* RETURNS: TRUE if success, FALSE if an error occured                    *
*************************************************************************/

BOOL myPuts(HANDLE hConsole, PCHAR s)
{
  BOOL bSuccess;
  DWORD cCharsWritten;
  const PCHAR crlf = "\n";
  BOOL retflag = TRUE;

  /* write the string to the console */
  bSuccess = WriteFile(hConsole, s, strlen(s), &cCharsWritten, NULL);
  PERR(bSuccess, "WriteFile");
  retflag = bSuccess;
  /* put a carriage return & line feed after the string */
  bSuccess = WriteFile(hConsole, crlf, strlen(crlf), &cCharsWritten, NULL);
  PERR(bSuccess, "WriteFile");
  if (!bSuccess)
    retflag = FALSE;
  return(retflag);
}


/********************************************************************
* FUNCTION: getConX(HANDLE hCon)                                    *
*                                                                   *
* PURPROSE: to get the current width of the console output buffer   *
*                                                                   *
* INPUT: the handle to get the information for                      *
*                                                                   *
* RETURNS: the width of the current console output buffer, in chars *
********************************************************************/

SHORT getConX(HANDLE hCon)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  BOOL bSuccess;

  bSuccess = GetConsoleScreenBufferInfo(hCon, &csbi);
  PERR(bSuccess, "GetConsoleScreenBufferInfo");
  return(csbi.dwSize.X);
}


/*********************************************************************
* FUNCTION: getConY(HANDLE hCon)                                     *
*                                                                    *
* PURPROSE: to get the current height of the console output buffer   *
*                                                                    *
* INPUT: the handle to get the information for                       *
*                                                                    *
* RETURNS: the height of the current console output buffer, in chars *
*********************************************************************/


SHORT getConY(HANDLE hCon)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  BOOL bSuccess;

  bSuccess = GetConsoleScreenBufferInfo(hCon, &csbi);
  PERR(bSuccess, "GetConsoleScreenBufferInfo");
  return(csbi.dwSize.Y);
}


/*************************************************************************
* FUNCTION: showConAPIs(HANDLE hConsole)                                 *
*                                                                        *
* PURPOSE: to display a list of console APIs on the given console buffer *
*                                                                        *
* INPUT: the console to display the APIs on                              *
*                                                                        *
* RETURNS: none                                                          *
*************************************************************************/

void showConAPIs(HANDLE hConsole)
{
  COORD coordScreen = {CONX, CONY}; /* used to set the console buffer size */
  /* used to set the console window size */
  SMALL_RECT srWindowRect = {0, 0, CONX - 1, CONY - 1};
  int i;
  #ifdef DBCS
  int SHORTLIST=1;
  #endif
  BOOL bSuccess;
  DWORD cCharsWritten;
  CHAR szTemps[128];
  WORD wConBufSize; /* used to hold current console buffer size */
  /* used to get maximum size of the current buffer so that we don't */
  /* make a window too large for the current screen */
  CONSOLE_SCREEN_BUFFER_INFO csbi;

  /* get the current console buffer size, in chars */
  wConBufSize = getConX(hConsole) * getConY(hConsole);
  /* force a CONX x CONY screen */
  /* if the current buffer is larger than what we want, resize the */
  /* console window first, then the buffer */
  if (wConBufSize > CONX * CONY)
    {
    bSuccess = SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect);
    PERR(bSuccess, "SetConsoleWindowInfo");
    bSuccess = SetConsoleScreenBufferSize(hConsole, coordScreen);
    PERR(bSuccess, "SetConsoleScreenBufferSize");
    }
  /* if the current buffer is smaller than what we want, resize the */
  /* buffer first, then the console window */
  if (wConBufSize < CONX * CONY)
    {
    bSuccess = SetConsoleScreenBufferSize(hConsole, coordScreen);

    #ifndef DBCS
    PERR(bSuccess, "SetConsoleScreenBufferSize");
    #else
    if (!bSuccess) {
    // Try 25-line FE full screen mode.
    CONY=25;
    coordScreen.Y=CONY; /* Try the FE 25 lines full screen support */
    bSuccess = SetConsoleScreenBufferSize(hConsole, coordScreen);
    PERR(bSuccess, "SetConsoleScreenBufferSize");
    }
    #endif
    /* get maximum size of the window given the current buffer */
    bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
    PERR(bSuccess, "GetConsoleScreenBufferInfo");
    /* if the size we want is larger than the maximum, make the size */
    /* that we want the same as the maximum */
    if (srWindowRect.Right + 1 > csbi.dwMaximumWindowSize.X)
      srWindowRect.Right = csbi.dwMaximumWindowSize.X - 1;
    if (srWindowRect.Bottom + 1 > csbi.dwMaximumWindowSize.Y)
      srWindowRect.Bottom = csbi.dwMaximumWindowSize.Y - 1;
    bSuccess = SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect);
    PERR(bSuccess, "SetConsoleWindowInfo");
    }
  coordScreen.X = 0;
  coordScreen.Y = 0;
  /* set the screen */
  cls(hConsole);
  bSuccess = FillConsoleOutputAttribute(hConsole, BACKGROUND_BLUE,
      CONX * CONY, coordScreen, &cCharsWritten);
  PERR(bSuccess, "FillConsoleOutputAttribute");
  /* set attributes for new writes to the console */
  bSuccess = SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE |
      FOREGROUND_WHITE);
  PERR(bSuccess, "SetConsoleTextAttribute");
  /* fill screen with API list */
  #ifndef DBCS
  for (i = 0; i < sizeof(conAPIs) / sizeof(conAPIs[0]); i++)
  #else
  if (CONY==25) SHORTLIST=2;
  for (i = 0; i < (sizeof(conAPIs) / sizeof(conAPIs[0]))/SHORTLIST; i++)
  #endif
    {
    coordScreen.X = 0;
    coordScreen.Y = (SHORT) (i + 2);  /* start on third line (zero based) */
    /* position the cursor to start the API list */
    bSuccess = SetConsoleCursorPosition(hConsole, coordScreen);
    PERR(bSuccess, "SetConsoleCursorPosition");
    /* format a line and write it to the screen */
    sprintf(szTemps, "%-3d%s", i + 1, conAPIs[i]);
    myPuts(hConsole, szTemps);
    /* color the number yellow on blue */
    bSuccess = FillConsoleOutputAttribute(hConsole, FOREGROUND_YELLOW |
        FOREGROUND_INTENSITY | BACKGROUND_BLUE, 3, coordScreen,
        &cCharsWritten);
    PERR(bSuccess, "FillConsoleOutputAttribute");
    coordScreen.X = 3;
    /* color text light cyan on blue */
    bSuccess = FillConsoleOutputAttribute(hConsole, FOREGROUND_CYAN |
        FOREGROUND_INTENSITY | BACKGROUND_BLUE, strlen(conAPIs[i]),
        coordScreen, &cCharsWritten);
    PERR(bSuccess, "FillConsoleOutputAttribute");
    }
  myPuts(hConsole, "\nClick on an API to see a demonstration of that API.\n"
                   "Hit ESC to exit the program.");
  return;
}


/*************************************************************************
* FUNCTION: putStatusLine(HANDLE hOut, PCHAR buf)                        *
*                                                                        *
* PURPOSE: display a string on the top line of the console output buffer *
*                                                                        *
* INPUT: the output handle, the string to display                        *
*                                                                        *
* RETURNS: none                                                          *
*************************************************************************/

void putStatusLine(HANDLE hOut, PCHAR buf)
{
  BOOL bSuccess;
  /* position the status line at (0, 0) */
  const COORD dwWriteCoord = {0, 0};
  DWORD cCharsWritten;
  int len; /* the length of the input string parameter */
  CHAR szTemp[256];
  SHORT sWidth; /* console width */

  sWidth = getConX(hOut);
  strcpy(szTemp, buf);
  len = strlen(szTemp);
  memset(szTemp + len, ' ', sWidth - len);  /* blank out rest of line */
  /* write the string to the console at the correct position */
  bSuccess = WriteConsoleOutputCharacter(hOut, szTemp, sWidth,
      dwWriteCoord, &cCharsWritten);
  PERR(bSuccess, "WriteConsoleOutput");
  /* color the status line so it stands out */
  bSuccess = FillConsoleOutputAttribute(hOut, FOREGROUND_RED |
      BACKGROUND_WHITE, sWidth, dwWriteCoord, &cCharsWritten);
  PERR(bSuccess, "FillConsoleOutputAttribute");
  return;
}


/********************************************************************
* FUNCTION: demoAPI(HANDLE *hConsole, enum cAPIs apiNumber)         *
*                                                                   *
* PURPOSE: call the correct demo function based on the input index  *
*                                                                   *
* INPUT: current console output buffer, and screen index of the API *
*        to demonstrate                                             *
*                                                                   *
* RETURNS: none                                                     *
********************************************************************/

static void demoAPI(HANDLE *phConsole, enum cAPIs apiNumber)
{
  const COORD coordScreen = {0, 0};
  DWORD cCharsWritten;
  BOOL bSuccess;
  HANDLE hConTemp;
  CHAR szConsoleTitle[128]; /* to store the console title */
  DWORD dwCharsRead;

  /* create a temp screen buffer to write to */
  hConTemp = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
      FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER,
      NULL);
  PERR(hConTemp != INVALID_HANDLE_VALUE,
      "CreateConsoleScreenBuffer");
  /* make it the active buffer */
  bSuccess = SetConsoleActiveScreenBuffer(hConTemp);
  PERR(bSuccess, "SetConsoleActiveScreenBuffer");
  /* clear screen & save the console title */
  cls(hConTemp);
  bSuccess = FillConsoleOutputAttribute(hConTemp, BACKGROUND_CYAN, 
      getConX(hConTemp) * getConY(hConTemp), coordScreen, &cCharsWritten);
  PERR(bSuccess, "FillConsoleOutputAttribute");
  bSuccess = SetConsoleTextAttribute(hConTemp, BACKGROUND_CYAN);
  PERR(bSuccess, "SetConsoleTextAttribute");
  dwCharsRead = GetConsoleTitle(szConsoleTitle, sizeof(szConsoleTitle));
  PERR(dwCharsRead, "GetConsoleTitle");

  /* flush the input buffer and call the correct demo function */
  bSuccess = FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
  PERR(bSuccess, "FlushConsoleInputBuffer");
  switch(apiNumber)
    {
    case ALLOC:
    case FREE: 
      demoAllocFree(hConTemp, phConsole);
      return;
      break;  
    case CREATE:
    case SETACTIVE:
    case SETATT:
      demoCreate(hConTemp);
      break;
    case FILLATT:
    case WRITECHAR:
      demoFillAtt(hConTemp);
      break;
    case FILLCHAR:
      demoFillChar(hConTemp);
      break;
    case FLUSH:
      demoFlush(hConTemp);
      break;
    case GETCUR:
    case SETCURINF:
    case SETCURPOS:
      demoCursor(hConTemp);
      break;
    case GETMODE:
    case SETMODE:
      demoConMode(hConTemp);
      break;
    case GETCONINFO:
      demoConInfo(hConTemp);
      break;
    case GETTITLE:
    case SETTITLE:
      demoGetTitle(hConTemp);
      break;
    case GETLARGEST:
      demoGetLargest(hConTemp);
      break;
    case GETNUMEV:
    case PEEK:
    case READCONIN:
      demoGetNumEvents(hConTemp);
      break;
    case GETNUMBUT:
      demoGetNumBut(hConTemp);
      break;
    case READCONOUT:
    case WRITEOUT:
    case READCONATT:
    case WRITEATT:
      demoReadConOut(hConTemp);
      break;
    case READCONCHAR:
      demoReadConChar(hConTemp);
      break;
    case SCROLL:
      demoScrollCon(hConTemp);
      break;
    case SETSIZE:
    case SETINFO:
      demoSizeInfo(hConTemp);
      break;
    case SETHAND:
    case GENCTRL:
      demoSetCtrlHandler(hConTemp);
      break;
    case WRITEIN:
      demoWriteIn(hConTemp);
      break;
    case GETCP:
    case GETOUTCP:
    case SETCP:
    case SETOUTCP:
      demoCodePage(hConTemp);
      break;
    default:
      break;
    }
  CloseHandle(hConTemp);  /* free temporary console buffer */
  /* reset active buffer to original buffer. If the temp buffer has been */
  /* resized, the console window will automatically be sized correctly */
  /* when the current console is set back to the old one */
  bSuccess = SetConsoleActiveScreenBuffer(*phConsole);
  PERR(bSuccess, "SetConsoleActiveScreenBuffer");
  /* restore the original console title */
  bSuccess = SetConsoleTitle(szConsoleTitle);
  PERR(bSuccess, "SetConsoleTitle");
  /* flush the input buffer to remove any 'leftover' clicks or key events */
  bSuccess = FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
  PERR(bSuccess, "FlushConsoleInputBuffer");
  return;
}


/************************************************************************
* FUNCTION: cls(HANDLE hConsole)                                        *
*                                                                       *
* PURPOSE: clear the screen by filling it with blanks, then home cursor *
*                                                                       *
* INPUT: the console buffer to clear                                    *
*                                                                       *
* RETURNS: none                                                         *
*************************************************************************/

void cls(HANDLE hConsole)
{
  COORD coordScreen = { 0, 0 }; /* here's where we'll home the cursor */
  BOOL bSuccess;
  DWORD cCharsWritten;

  /* fill the entire screen with blanks */
  bSuccess = FillConsoleOutputCharacter(hConsole, (TCHAR) ' ',
      getConX(hConsole) * getConY(hConsole), coordScreen, &cCharsWritten);
  PERR(bSuccess, "FillConsoleOutputCharacter");
  /* put the cursor at (0, 0) */
  bSuccess = SetConsoleCursorPosition(hConsole, coordScreen);
  PERR(bSuccess, "SetConsoleCursorPosition");
  return;
}


/*********************************************************************
* FUNCTION: setConTitle                                              *
*                                                                    *
* PURPOSE: simply set the current console title. Called by each demo *
*          function to specify the name of the source file that      *
*          contains the demo function.                               *
*                                                                    *
* INPUT: null terminated string                                      *
*********************************************************************/

void setConTitle(PCHAR szTitle)
{
  BOOL bSuccess;

  /* set the console title to the input string parameter */
  bSuccess = SetConsoleTitle(szTitle);
  PERR(bSuccess, "SetConsoleTitle");
  return;
}

/*********************************************************************
* FUNCTION: main                                                     *
*                                                                    *
* PURPOSE: main input loop. Set up console input and output          *
*          attributes, then read the input queue and process input   *
*          events. If ESC is hit, end the process. If the mouse is   *
*          clicked, read the line from the output console where the  *
*          mouse was clicked. If an index number can be found at the *
*          beginning of the line, call the appropriate demo function *
*          to demonstrate that API. All input event information is   *
*          displayed on the status line on the top line.             *
*                                                                    *
* INPUT: none                                                        *
*                                                                    *
* RETURNS: 1 if can't free console, 2 if can't allocate a console, 0 *
*          for a normal exit                                         *
*********************************************************************/

int main(void)
{
  BOOL bSuccess;
  HANDLE hStdIn, hStdOut; /* standard input, output handles */
  DWORD dwMode;
  /* array of console input event records */
  INPUT_RECORD inputBuffer;
  DWORD dwInputEvents; /* number of events actually read */
  CHAR bOutBuf[256]; /* buffer to format event information into */
  /* used to get the new console screen buffer size if it is changed */
  COORD coordScreen; /* used when reading the index number from the screen */
  CHAR *szLineBuf; /* buffer to read the index number into */
  DWORD cCharsRead;
  enum cAPIs apiNumber; /* the index number of the API read from the screen */
  /* used when turning off the cursor */
  CONSOLE_CURSOR_INFO cciHidden = {100, FALSE};


  /* free the console and immediately allocate a new one. This is done so */
  /* that when debugging under ntsd, the application output will not be */
  /* intermingled with the debugger output. This also makes cleanup */
  /* much easier; we won't need to restore the cursor, buffer/console */
  /* sizes, screen colors, etc. since we'll have our own console window */
  /* that will go away when we terminate. */

#if defined ( FULLCONSOLE )

  bSuccess = FreeConsole();
  /* C run-time output will not work from now on! */
  PERR(bSuccess, "FreeConsole");
  bSuccess = AllocConsole();
  PERR(bSuccess, "AllocConsole");

#endif

  /* let's put up a meaningful console title */
  bSuccess = SetConsoleTitle("Win32 Console API Demo");
  PERR(bSuccess, "SetConsoleTitle");
  /* get the standard handles */
  hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  PERR(hStdOut != INVALID_HANDLE_VALUE, "GetStdHandle");
  hStdIn = GetStdHandle(STD_INPUT_HANDLE);
  PERR(hStdIn != INVALID_HANDLE_VALUE, "GetStdHandle");
  /* set up mouse and window input */
  bSuccess = GetConsoleMode(hStdIn, &dwMode);
  PERR(bSuccess, "GetConsoleMode");
  /* when turning off ENABLE_LINE_INPUT, you MUST also turn off */
  /* ENABLE_ECHO_INPUT. */
  bSuccess = SetConsoleMode(hStdIn, (dwMode & ~(ENABLE_LINE_INPUT |
      ENABLE_ECHO_INPUT)) | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
  PERR(bSuccess, "SetConsoleMode");
  /* save then hide the cursor */
  bSuccess = SetConsoleCursorInfo(hStdOut, &cciHidden);
  PERR(bSuccess, "SetConsoleCursorInfo");
  /* resize console to CONX * CONY and put list of console APIs up */
  showConAPIs(hStdOut);
  /* This is the main input loop. Read from the input queue and process */
  /* the events read */
  szLineBuf = (char *) malloc(getConX(hStdOut));
  PERR(szLineBuf, "malloc");

  do
    {
    /* read an input events from the input event queue */
    bSuccess = ReadConsoleInput(hStdIn, &inputBuffer, 1, &dwInputEvents);
    PERR(bSuccess, "ReadConsoleInput");
    switch (inputBuffer.EventType)
      {
      case KEY_EVENT:
        if (inputBuffer.Event.KeyEvent.bKeyDown)
          {
          /* display the key event info on the status line */
          sprintf(bOutBuf, "key: virtual=%d ascii=%c",
              inputBuffer.Event.KeyEvent.wVirtualKeyCode,
              inputBuffer.Event.KeyEvent.uChar.AsciiChar);
          putStatusLine(hStdOut, bOutBuf);
          }
        break;
      case MOUSE_EVENT:
        sprintf(bOutBuf, "mouse: %s at %d, %d",
            (inputBuffer.Event.MouseEvent.dwEventFlags == MOUSE_MOVED ?
            "moved" : "clicked"), inputBuffer.Event.MouseEvent.dwMousePosition.X,
            inputBuffer.Event.MouseEvent.dwMousePosition.Y);
        putStatusLine(hStdOut, bOutBuf);
        /* was this a mouse click? If so dwEventFlags contains 0 */
        /* are any mouse buttons down? If so dwButtonState is not 0 */
        if (inputBuffer.Event.MouseEvent.dwEventFlags == 0 &&
            inputBuffer.Event.MouseEvent.dwButtonState)
          {
          coordScreen.X = 0;
          coordScreen.Y = inputBuffer.Event.MouseEvent.dwMousePosition.Y;
          /* read the line where the mouse clicked */
          bSuccess = ReadConsoleOutputCharacter(hStdOut, szLineBuf,
              sizeof(szLineBuf), coordScreen, &cCharsRead);
          PERR(bSuccess, "ReadConsoleOutputCharacter");
          /* scan for an index number at the beginning of that line. */
          /* if there was one read, call the correct demo function */
          if (sscanf(szLineBuf, "%d", &apiNumber))
            demoAPI(&hStdOut, apiNumber);
          }
        break;
      case WINDOW_BUFFER_SIZE_EVENT:
        sprintf(bOutBuf, "window: %d, %d",
            inputBuffer.Event.WindowBufferSizeEvent.dwSize.X,
            inputBuffer.Event.WindowBufferSizeEvent.dwSize.Y);
        putStatusLine(hStdOut, bOutBuf);
        Sleep(1000);
        break;
      } /* switch */
    /* when we receive an esc, drop out of do loop */
    } while (inputBuffer.EventType != KEY_EVENT ||
            inputBuffer.Event.KeyEvent.wVirtualKeyCode != VK_ESCAPE);
  free(szLineBuf); /* free allocated line buffer */
  return(0);
}
