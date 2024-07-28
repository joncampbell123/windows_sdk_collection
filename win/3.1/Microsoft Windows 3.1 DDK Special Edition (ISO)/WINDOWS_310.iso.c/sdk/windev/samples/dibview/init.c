/*************************************************************************

      File:  INIT.C

   Purpose:  Routines called when the application first runs to do
             all the necessary initialization.

 Functions:  InitMyDIB
             InitInstance

  Comments:  

   History:   Date      Reason
             6/ 1/91    Created

*************************************************************************/


#include <windows.h>
#include <direct.h>
#include "frame.h"
#include "file.h"
#include "options.h"
#include "child.h"
#include "init.h"
#include "palette.h"
#include "dibview.h"


char szFrameClass[] = "MyDIBWClass";   // Class name of frame window.
char szPalClass[]   = "MyDIBPalClass"; // Class name of palette windows.
char szFrameMenu[]  = "TestMenu";      // Main menu (in .RC file).
char szPalMenu[]    = "PalMenu";       // Palette windows' menu (in .RC file).
char szMyIcon[]     = "MyIcon";        // Icon name (in .RC file).



//---------------------------------------------------------------------
//
// Function:   InitMyDIB
//
// Purpose:    Does initialization for DIBView.  Registers all the
//             classes we want, etc.  Called by first running instance
//             of DIBView, only (in DIBVIEW.C).
//
// Parms:      hInst == Handle to *this* instance of the app.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

BOOL InitMyDIB (HANDLE hInst)
{
   WNDCLASS  wc;

   wc.style          = CS_HREDRAW |       // Class style(s).
		       CS_VREDRAW;
   wc.lpfnWndProc    = FrameWndProc;      // Function to retrieve messages for
                                          //    windows of this class.
   wc.cbClsExtra     = 0;                 // No per-class extra data.
   wc.cbWndExtra     = 0;                 // No per-window extra data.
   wc.hInstance      = hInst;             // Application that owns the class.
   wc.hIcon          = LoadIcon(hInst, szMyIcon);
   wc.hCursor        = LoadCursor (NULL, IDC_ARROW);
   wc.hbrBackground  = COLOR_APPWORKSPACE + 1;  // Use system color for window bgrnd 
   wc.lpszMenuName   = szFrameMenu;        // Name of menu resource in .RC file.
   wc.lpszClassName  = szFrameClass;       // Name used in call to CreateWindow.


      // Register the window class and return success/failure code.

   if (!RegisterClass(&wc))
      return FALSE;


      // Register the MDI child class

   wc.style         = 0;
   wc.lpfnWndProc   = ChildWndProc;
   wc.lpszMenuName  = NULL;
   wc.cbWndExtra    = CBWNDEXTRA;
   wc.hbrBackground = COLOR_WINDOW + 1;
   wc.lpszClassName = szMDIChild;
   wc.hIcon         = NULL;            // Icon -- draws part of DIB


   if (!RegisterClass(&wc))
      {
      UnregisterClass (szFrameClass, hInst);
	   return FALSE;
      }


      // Register the Palette window class.

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = PaletteWndProc;
   wc.lpszMenuName  = szPalMenu;
   wc.cbWndExtra    = PAL_CBWNDEXTRA;
   wc.hbrBackground = COLOR_WINDOW + 1;
   wc.lpszClassName = szPalClass;
   wc.hIcon         = LoadIcon (NULL, IDI_APPLICATION);


   if (!RegisterClass(&wc))
      {
      UnregisterClass (szFrameClass, hInst);
      UnregisterClass (szMDIChild, hInst);
	   return FALSE;
      }


   return TRUE;
}



//---------------------------------------------------------------------
//
// Function:   InitInstance
//
// Purpose:    Do necessary initialization for this instance of the
//             app.  Creates the main, overlapped window, sets the
//             global hInst, sets our working directory for FILE.C
//             routines.  Called from DIBView.C.
//
// Parms:      hInstance == Handle to this instance (passed to WinMain()).
//             nCmdShow  == How window should come up (passed to WinMain()).
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

BOOL InitInstance(HANDLE hInstance, 
                  int    nCmdShow)
{
   HWND hWnd;

    // Save the instance handle in static variable, which will be used in
    // many subsequence calls from this application to Windows.          

   hInst = hInstance;

   // Create a main window for this application instance.

   hWnd = CreateWindow(szFrameClass,      // See RegisterClass() call.
      "DibView",                          // Text for window title bar.
      WS_OVERLAPPEDWINDOW |               // Window style.
      WS_CLIPCHILDREN,
      CW_USEDEFAULT,                      // Default horizontal position.
      CW_USEDEFAULT,                      // Default vertical position.
      CW_USEDEFAULT,                      // Default width.
      CW_USEDEFAULT,                      // Default height.
      NULL,                               // Overlapped windows have no parent.
      NULL,                               // Use the window class menu.        
      hInstance,                          // This instance owns this window.   
      NULL);                              // Pointer not needed.               

   // If window could not be created, return "failure"

   if (!hWnd)
      return (FALSE);

   // Make the window visible; update its client area; and return "success"

   ShowWindow(hWnd, nCmdShow);         // Show the window                        
   UpdateWindow(hWnd);                 // Sends WM_PAINT message

   // Gets the default system directory for file open/save
   getcwd (szDirName, sizeof (szDirName));

   return (TRUE);      
}
