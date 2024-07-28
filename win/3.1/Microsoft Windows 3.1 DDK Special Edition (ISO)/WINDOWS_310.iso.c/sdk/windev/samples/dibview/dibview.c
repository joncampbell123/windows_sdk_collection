/*************************************************************************

      File:  DIBVIEW.C

   Purpose:  Contains WinMain/the main message loop.

 Functions:  WinMain

  Comments:  

   History:   Date     Reason

             6/1/91    Created

*************************************************************************/

#include <windows.h>
#include "init.h"
#include "file.h"
#include "paint.h"
#include "dibview.h"



HANDLE hInst         = NULL;                    // Handle to this instance
HWND   hWndMDIClient;                           // MDI Client's window handle.




//---------------------------------------------------------------------
//
// Function:   WinMain
//
// Purpose:    What Windows calls when our application is started up.
//             Here, we do all necessary initialization, then enter
//             our message loop.  The command line is also parsed,
//             and if it lists any DIBs to open, they're opened up.
//
//             This is a pretty standard WinMain.
//
//             Since we're an MDI app, we call 
//             TranslateMDISysAccel (hWndMDIClient, &msg) during
//             message loop processing.
//
// Parms:      hInstance     == Instance of this task.
//             hPrevInstance == Instance of previous DIBView task (NULL if none).
//             lpCmdLine     == Command line.
//             nCmdShow      == How DIBView should come up (i.e. normally,
//                              minimized, maximized, etc.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

int PASCAL WinMain(HANDLE hInstance,            // This instance
                   HANDLE hPrevInstance,        // Last instance
                   LPSTR  lpCmdLine,            // Command Line
                   int    nCmdShow)             // Minimized or Normal?
{
   MSG msg;

   if (!hPrevInstance)
      if (!InitMyDIB (hInstance))
         return (FALSE);

   if (!InitInstance(hInstance, nCmdShow))
      return (FALSE);

   // Parses Command line for DIB's
   ParseCommandLine (lpCmdLine);

   while (GetMessage(&msg,             // Put Message Here
                     NULL,             // Handle of win receiving msg
                     NULL,             // lowest message to examine
                     NULL))            // highest message to examine
   {
   if (!TranslateMDISysAccel (hWndMDIClient, &msg))
      {
      TranslateMessage(&msg);          // Translates virtual key codes
      DispatchMessage(&msg);           // Dispatches message to window
      }
   }

   return (msg.wParam);                // Returns the value from PostQuitMessage
}




