/************************************************************

   PROGRAM: PENAPP.C

   PURPOSE: A generic template for a pen-aware app.

   COMMENTS:

      This code is a skeleton program containing all necessary
      functionality to recognize pen input using three separate
      recognizers: a custom, a shape, and the system recognizer.

      PENAPP accepts pen input through the Input Window.
      Depending on the menu option, PENAPP sends the input
      to either the system default recognizer, the sample custom
      recognizer (SREC), or the shape recognizer (SHAPEREC).

      The output from the recognizers are displayed through the
      Raw Data and Info windows.  The Raw Data window displays
      the actual ink the user wrote or drew.  The Info window
      displays one of three things:

         1) For the system recognizer -- the Info window outputs
            the recognized ANSI text.

         2) For the sample custom recognizer -- the Info window
            outputs an arrow indicating the compass direction of
            the input stroke (up, down, left, or right).

         3) For the shape recognizer -- the Info window displays
            a "clean" image of the shape.  Recognized shapes may
            be rectangles, ellipses, or lines.

************************************************************/

#define NOCOMM
#include <windows.h>
#include <string.h>
#include <penwin.h>
#include <penwoem.h>
#include "main.h"
#include "penapp.h"
#include "penres.h"


/******** Module Variables *********/

char     szResult[cchMax]  = {0};   /* Recognized string */
SYV      syvGlobal   = SYV_NULL;    /* Recognized symbol value */
WORD     wLineStyle;                /* Line style of the shape */
RECT     shapeRect;                 /* Bounding rectangle of the shape */
int      miRecMode   = miSample;    /* Menu ID of current recognizer */
BOOL     fSaveData   = TRUE;        /* Save data flag    */
HREC     hrecCur     = (HREC)RC_WDEFAULT; /* Current recognizer handle */
HPENDATA hpendata;                  /* Most recent raw data saved */
HWND     hwndMain;                  /* Window handles */
HWND     hwndInput;
HWND     hwndRaw;
HWND     hwndInfo;



/******** Code *********/


/*----------------------------------------------------------
Purpose: Main Windows function
Returns: Exit code
*/
int PASCAL WinMain(
   HANDLE hInstance,       /* Instance handle  */ 
   HANDLE hPrevInstance,   /* Previous instance handle */ 
   LPSTR lpszCommandLine,  /* Command line string */ 
   int cmdShow)            /* ShowWindow flag */ 
   {
   MSG   msg;

   Unused(lpszCommandLine);
   
   if (!hPrevInstance)
      {
      if (!FInitApp(hInstance))
         {
         return 1;
         }
      }
                
   if (FInitInstance(hInstance, hPrevInstance, cmdShow))
      {
      while (GetMessage((LPMSG)&msg,NULL,0,0) )
         {
         TranslateMessage((LPMSG)&msg);
         DispatchMessage((LPMSG)&msg);
         }
      }
   else
      msg.wParam = 0;

   return msg.wParam;
   }


/*----------------------------------------------------------
Purpose: Window procedure for main sample window.
Returns: Varies
*/
LRESULT CALLBACK MainWndProc(
   HWND hwnd,        /* Window handle */ 
   UINT message,     /* Message */ 
   WPARAM wParam,    /* Varies */ 
   LPARAM lParam)    /* Varies */ 
   {
   LONG  lRet  = 0L;
   RC    rc;
   BOOL (FAR PASCAL *lpfnConfig) (WORD, WORD, LONG);

   switch (message)
      {
      case WM_COMMAND:
         switch (wParam)
            {
            case miExit:
               DestroyWindow(hwndMain);
               break;

            case miSample:
            case miShape:
            case miSystem:
               SetGraphWindow(wParam);
               break;

            case miSaveData:
               {
               HMENU hmenu = GetMenu(hwnd);

               CheckMenuItem(hmenu, miSaveData,
                  (fSaveData = !fSaveData) ? MF_CHECKED : MF_UNCHECKED);
               break;
               }

            default:
               break;
            }
         break;

      case WM_SIZE:
         {
         int x;
         int y;
         int dx;
         int dy;

         x = XInputWnd(LOWORD(lParam));
         y = YInputWnd(0);
         dx = DxInputWnd(LOWORD(lParam));
         dy = DyInputWnd(HIWORD(lParam));
         MoveWindow(hwndInput, x, y, dx, dy, TRUE);

         x = XRawWnd(0);
         y = YRawWnd(HIWORD(lParam));
         dx = DxRawWnd(LOWORD(lParam));
         dy = DyRawWnd(HIWORD(lParam));
         MoveWindow(hwndRaw, x, y, dx, dy, TRUE);

         x = XInfoWnd(0);
         y = YInfoWnd(0);
         dx = DxInfoWnd(LOWORD(lParam));
         dy = DyInfoWnd(HIWORD(lParam));
         MoveWindow(hwndInfo, x, y, dx, dy, TRUE);
         break;
         }

      case WM_DESTROY:
         if (hpendata)
            DestroyPenData(hpendata);

         /* Unload current recognizer if not System recognizer
         */
         if (hrecCur != RC_WDEFAULT)      
            UninstallRecognizer(hrecCur);

         PostQuitMessage(0);
         break;

      case WM_GLOBALRCCHANGE:
         /* There is really no reason to pass the WM_GLOBALRCCHANGE on to
         ** the Sample Recognizer since it is a private recognizer and
         ** this application knows that the sample recognizer will not
         ** process the message.  But we'll pass it anyway.
         */
         GetGlobalRC ((LPRC)&rc, (LPSTR)NULL, (LPSTR)NULL, NULL);
         if (hrecCur != RC_WDEFAULT &&
	    ((FARPROC)lpfnConfig = GetProcAddress(hrecCur, "ConfigRecognizer")) != NULL)
            {
            lRet = (*lpfnConfig) (WCR_RCCHANGE, 0, (LONG) (LPRC) &rc);
            }
         break;

      default:
         lRet = DefWindowProc(hwnd, message, wParam, lParam);
         break;
      }

   return lRet;
   }



/*----------------------------------------------------------
Purpose: Window procedure for the input child window
Returns: Varies
Comment: This is a template of a typical pen-aware window procedure.

         The general interaction with recognition API is:

            1) Install specific recognizer using InstallRecognizer
               (this is done in FLoadRec in this sample)
            2) Initialize the RC data structure
            3) At pen input (WM_LBUTTONDOWN), call the recognizer
            3) Retrieve recognized data on WM_RCRESULT message
            4) Unload recognizer using UninstallRecognizer
*/
LRESULT CALLBACK InputWndProc(
   HWND hwnd,        /* Window handle */ 
   UINT message,     /* Message */ 
   WPARAM wParam,    /* Varies */ 
   LPARAM lParam)    /* Varies */ 
   {
   LONG  lRet  = 0L;
   RC    rc;

   switch (message)
      {
      case WM_LBUTTONDOWN:

         /* Two possibilities exist: user is using mouse or the pen.  
         ** If it is the pen, the user is starting to write.
         ** Initialize recognition-context for recognizer
         */
         if (IsPenEvent(message, GetMessageExtraInfo()))
            {
            InitRC(hwndInput, &rc);

            rc.rglpdf[0] = NULL; /* No dictionary search */

            rc.lRcOptions |= RCO_NOPOINTEREVENT;   /* Ignore Tap-n-Hold feature */
            rc.lRcOptions |= fSaveData ? RCO_SAVEALLDATA : 0;

            rc.hrec = hrecCur;   /* Set hrec since it might not be the system recognizer */
            if(miRecMode == miSample)
               {
               rc.lPcm |= PCM_PENUP;   /* Set this flag for single strokes */
               }
            else
               rc.lPcm |= PCM_TIMEOUT;

            if (Recognize(&rc) == REC_BUSY)
               MessageBox(hwndMain, "Recognizer is busy", szPenAppWnd, MB_OK|MB_ICONEXCLAMATION);
            }
         break;

      case WM_RCRESULT:
         {
         LPRECT   lprect;
         LPRCRESULT  lprcresult = (LPRCRESULT)lParam;

         /* The recognizer has recognized input and piped it through
         ** lParam (as an LPRCRESULT).
         **
         ** The sample recognizer returns a symbol graph containing codes
         ** indicating the general direction the input stroke is written,
         ** according to the four compass directions.
         **
         ** The shape recognizer returns a symbol graph indicating
         ** the geometric shape of the input (either line, rectangle,
         ** or ellipse).
         **
         ** The standard recognizer returns the recognized string.
         */
         lprect = &lprcresult->rectBoundInk;

         if ((int)wParam < 0)       /* Did an error occur? */
            {
            syvGlobal = SYV_NULL;   /* Reset */
            *szResult = NULL;

            /* We set the lprect to NULL to invalidate entire input window,
            ** since the error could be overflow, in which case ink could
            ** be outside the given lprect.
            */

            lprect = NULL;    
            }
         else if (!(lprcresult->wResultsType & rcrtNoResults))
            {
            switch (miRecMode)
               {
               case miSample:
                  syvGlobal = *(lprcresult->lpsyv);   /* Copy symbol value */
                  break;
               case miShape:
                  syvGlobal = *(lprcresult->lpsyv);   /* Copy symbol value */
                  shapeRect = *(LPRECT)(lprcresult->syg.rgpntHotSpots);
                  wLineStyle = (WORD)(lprcresult->syg.lRecogVal);
                  break;
               default:
                  *szResult = NULL;

                  /* Set syvGlobal simply to pass test condition in InfoWndProc
                  */
                  syvGlobal = *(lprcresult->lpsyv);   
                  SymbolToCharacter(lprcresult->lpsyv, cchMax, szResult, NULL);
                  break;
               }
            CopyRawData(lprcresult);
            }
         else  /* Nothing Recognized */
            {
            syvGlobal = SYV_NULL;
            *szResult = NULL;
            CopyRawData(lprcresult);
            }

         InvalidateRect(hwndInfo, NULL, TRUE);
         InvalidateRect(hwndInput, lprect, TRUE);
         InvalidateRect(hwndRaw, NULL, TRUE);

         /* Invalidate any topmost windows overlapping our inking rect.
         ** Note we wouldn't have to do this if we used ProcessWriting
         ** (it handles it for us).
         */
         if (lprect != NULL)     /* Recopy if necessary */
            lprect = &lprcresult->rectBoundInk;
         MapWindowPoints(hwnd, HWND_DESKTOP, (LPPOINT)lprect, 2);
         RedrawWindow(GetDesktopWindow(), lprect, NULL,
            RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
         break;
         }

      case WM_SYSCOMMAND:
         switch (wParam & 0xFFF0)
            {
            case SC_MOVE:     /* Don't allow window to be moved */
               break;

            default:
               DefWindowProc(hwnd, message, wParam, lParam);
               break;
            }
         break;

      case WM_PAINT:
         {
         HDC         hdc;
         PAINTSTRUCT ps;

         hdc = BeginPaint(hwnd, &ps);
         EndPaint(hwnd, &ps);
         break;
         }

      default:
         lRet = DefWindowProc(hwnd, message, wParam, lParam);
         break;
      }

   return lRet;
   }



/*----------------------------------------------------------
Purpose: Window procedure for the info child window
Returns: Varies
*/
LRESULT CALLBACK InfoWndProc(
   HWND hwnd,        /* Window handle */ 
   UINT message,     /* Message */ 
   WPARAM wParam,    /* Varies */ 
   LPARAM lParam)    /* Varies */ 
   {
   LONG        lRet  = 0L;

   switch (message)
      {
      case WM_PAINT:
         {
         HPEN     hpen;
         HPEN     hpenSav;
         HBRUSH   hbrushSav;
         HDC      hdc;
         PAINTSTRUCT ps;
         int      iSavBkMode;

         hdc = BeginPaint(hwnd, &ps);
         if(syvGlobal != SYV_NULL)
            {
            switch(miRecMode)
               {
               case miSample:
                  hpen = CreatePen(PS_SOLID, 2, GetSysColor(COLOR_WINDOWTEXT));
                  hpenSav = SelectObject(hdc, hpen);

                  DrawArrow(hwnd, hdc);

                  SelectObject(hdc, hpenSav);
                  DeleteObject(hpen);
                  break;

               case miShape:
                  hpen = CreatePen(wLineStyle, 1, GetSysColor(COLOR_WINDOWTEXT));
                  hpenSav = SelectObject(hdc, hpen);
                  
                  hbrushSav = SelectObject(hdc, GetStockObject(NULL_BRUSH));
                  iSavBkMode = SetBkMode(hdc, TRANSPARENT);

                  DrawShape(hwnd, hdc);

                  SetBkMode(hdc, iSavBkMode);
                  SelectObject(hdc, hbrushSav);
                  SelectObject(hdc, hpenSav);
                  DeleteObject(hpen);
                  break;

               default:
                  SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
                  SetBkMode(hdc, TRANSPARENT);

                  TextOut(hdc, 1, 1, szResult, strlen(szResult));
                  break;
               }
            }
         EndPaint(hwnd, &ps);
         break;
         }

      case WM_SYSCOMMAND:
         switch (wParam & 0xFFF0)
            {
            case SC_MOVE:        /* Don't allow window to be moved */
               break;

            default:
               DefWindowProc(hwnd, message, wParam, lParam);
               break;
            }
         break;

      default:
         lRet = DefWindowProc(hwnd, message, wParam, lParam);
         break;
      }

   return lRet;
   }


/*----------------------------------------------------------
Purpose: Window procedure for the raw child window
Returns: Varies
*/
LRESULT CALLBACK RawWndProc(
   HWND hwnd,        /* Window handle */ 
   UINT message,     /* Message */ 
   WPARAM wParam,    /* Varies */ 
   LPARAM lParam)    /* Varies */ 
   {
   LONG        lRet  = 0L;

   switch (message)
      {
      case WM_PAINT:
         {
         HDC         hdc;
         PAINTSTRUCT ps;

         hdc = BeginPaint(hwnd, &ps);
         DrawRawData(hdc);
         EndPaint(hwnd, &ps);
         break;
         }

      case WM_SYSCOMMAND:
         switch (wParam & 0xFFF0)
            {
            case SC_MOVE:        /* Don't allow window to be moved */
               break;

            default:
               DefWindowProc(hwnd, message, wParam, lParam);
               break;
            }
         break;

      default:
         lRet = DefWindowProc(hwnd, message, wParam, lParam);
         break;
      }

   return lRet;
   }


/*----------------------------------------------------------
Purpose: Initialize application data and register window classes
Returns: TRUE if all successful
Comment: There are four window classes: the main application window,
         and three child windows for: input, information display, and
         raw data display.

         The input window is the area in which handwriting is received.
         The raw data window outputs the raw input data.  The info window
         outputs the data of the recognized input.
*/
BOOL FInitApp(HANDLE hInstance)     /* Instance handle */ 
   {
   WNDCLASS wc;
   HCURSOR  hcursor;

   hcursor = LoadCursor(NULL, IDC_ARROW);

   /* Register Pen App window class
   */
   wc.hCursor = hcursor;
   wc.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(iconPenApp));
   wc.lpszMenuName = MAKEINTRESOURCE(menuPenApp);
   wc.lpszClassName = (LPSTR)szPenAppClass ;
   wc.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE+1;
   wc.hInstance = hInstance;
   wc.style = CS_VREDRAW | CS_HREDRAW ;
   wc.lpfnWndProc = MainWndProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;

   if (!RegisterClass((LPWNDCLASS) &wc))
      return FALSE;


   /* Register Pen App child window classes
   */
   wc.hCursor = LoadCursor(NULL, IDC_PEN);
   wc.hIcon = NULL;
   wc.lpszMenuName = NULL;
   wc.lpszClassName = (LPSTR)szPenAppInputClass;
   wc.hbrBackground = (HBRUSH)COLOR_WINDOW+1;
   wc.style = CS_VREDRAW | CS_HREDRAW | CS_SAVEBITS;
   wc.lpfnWndProc = InputWndProc;
   if (!RegisterClass((LPWNDCLASS) &wc))
      return FALSE;

   wc.hCursor = hcursor;
   wc.lpszClassName = (LPSTR)szPenAppInfoClass;
   wc.lpfnWndProc = InfoWndProc;
   if (!RegisterClass((LPWNDCLASS) &wc))
      return FALSE;

   wc.lpszClassName = (LPSTR)szPenAppRawClass;
   wc.lpfnWndProc = RawWndProc;
   wc.hbrBackground = (HBRUSH)COLOR_WINDOW+1;
   if (!RegisterClass((LPWNDCLASS) &wc))
      return FALSE;
   }


/*----------------------------------------------------------
Purpose: Initialize data structures; create windows; load recognizer.
Returns: TRUE if all successful
*/
BOOL FInitInstance(
   HANDLE hInstance,       /* Instance handle */ 
   HANDLE hPrevInstance,   /* Previous instance handle */ 
   int cmdShow)            /* ShowWindow flag */ 
   {
   int      cxScreen = GetSystemMetrics(SM_CXSCREEN);
   int      cyScreen = GetSystemMetrics(SM_CYSCREEN);
   RECT  rect;

   Unused(hPrevInstance);

   /* Create Main window
   */
   hwndMain = CreateWindow((LPSTR)szPenAppClass, 
         (LPSTR)szPenAppWnd,    
         WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW,
         0, 0, cxScreen, cyScreen,
         (HWND)NULL,
         (HWND)NULL,
         (HANDLE)hInstance,
         (LPSTR)NULL
         );

   if (!hwndMain)
      {
      return FALSE;
      }


   /* Create Input window
   */
   GetClientRect(hwndMain, &rect);

   hwndInput = CreateWindow((LPSTR)szPenAppInputClass,
         (LPSTR)szInputWnd,     
         WS_CHILD | WS_BORDER | WS_CAPTION | WS_VISIBLE | WS_CLIPSIBLINGS,
         XInputWnd(rect.right), YInputWnd(0), DxInputWnd(rect.right), DyInputWnd(rect.bottom),
         hwndMain,
         NULL,
         (HANDLE)hInstance,
         (LPSTR)NULL
         );
   
   if (!hwndInput)
      {
      return FALSE;
      }

   
   /* Create Raw Data window
   */
   hwndRaw = CreateWindow((LPSTR)szPenAppRawClass,
         (LPSTR)szRawWnd,    
         WS_CHILD | WS_BORDER | WS_CAPTION | WS_VISIBLE | WS_CLIPSIBLINGS,
         XRawWnd(0), YRawWnd(rect.bottom),
         DxRawWnd(rect.right), DyRawWnd(rect.bottom),
         hwndMain,
         NULL,
         (HANDLE)hInstance,
         (LPSTR)NULL
         );
   
   if (!hwndRaw)
      {
      return FALSE;
      }


   /* Create Info window
   */
   hwndInfo = CreateWindow((LPSTR)szPenAppInfoClass,
         (LPSTR)szInfoWnd,   
         WS_CHILD | WS_BORDER | WS_CAPTION | WS_VISIBLE | WS_CLIPSIBLINGS,
         XInfoWnd(0), YInfoWnd(0),
         DxInfoWnd(rect.right), DyInfoWnd(rect.bottom),
         hwndMain,
         NULL,
         (HANDLE)hInstance,
         (LPSTR)NULL
         );
   
   if (!hwndInfo)
      {
      return FALSE;
      }

   ShowWindow(hwndMain, cmdShow);
   UpdateWindow(hwndMain);

   return FLoadRec();      /* Load the recognizer  */
   }


/*----------------------------------------------------------
Purpose: Install a recognizer, based upon the value of miRecMode
Returns: TRUE if successful
*/
BOOL FLoadRec(VOID)
   {
   LPSTR lpRecogName;
   HCURSOR hsave;

   /* Note that hrecCur == RC_WDEFAULT at start of program
   */
   if (hrecCur != RC_WDEFAULT)
      {
      UninstallRecognizer(hrecCur); /* Unload any current recognizer */
      }

   /* Install appropriate recognizer DLL
   */
   switch(miRecMode)
      {
      case miSample:       /* Sample recognizer */
         lpRecogName = (LPSTR)szSampleRec;
         break;
      case miShape:        /* Shape recognizer */
         lpRecogName = (LPSTR)szShapeRec;
         break;
      default:             /* System recognizer */
         /* The system recognizer is always loaded, so we just set
         ** our internal variables and return.
         */
         lpRecogName = NULL;
         hrecCur = RC_WDEFAULT;
         return TRUE;      /* Don't need to load the System recognizer */
      }
   hsave = SetCursor(LoadCursor(NULL, IDC_WAIT));
   hrecCur = InstallRecognizer(lpRecogName);
   SetCursor(hsave);

   if (!hrecCur)
      {
      MessageBox(hwndMain, "Could not install recognizer", szPenAppWnd, MB_OK);
      return FALSE;
      }

   return TRUE;
   }


/*----------------------------------------------------------
Purpose: Duplicate the pen data buffer
Returns: --
*/
VOID CopyRawData(LPRCRESULT lprcresult)   /* Ptr to RCRESULT struct */ 
   {
   if (hpendata != NULL)         /* Destroy old buffer */
      DestroyPenData(hpendata);

   /* Allocate new buffer for data points
   */
   if ((hpendata = DuplicatePenData(lprcresult->hpendata, 0)) == NULL)
      {
      MessageBox(hwndMain, "Insufficient memory", szPenAppWnd, MB_ICONEXCLAMATION | MB_OK);
      *szResult = NULL;
      }
   }


/*----------------------------------------------------------
Purpose: Draw an arrow in the direction of the endpoint line
Returns: --
Comment: Direction is specified in szResult, and are one of the four
         compass directions.
*/
VOID DrawArrow(
   HWND hwnd,     /* Window handle */ 
   HDC hdc)       /* DC handle */ 
   {
   RECT  rect;
   int      xEnd;
   int      yEnd;
   int      xArrow[2];
   int      yArrow[2];

   GetClientRect(hwnd, &rect);
   SetMapMode(hdc, MM_ISOTROPIC);

   SetWindowExt(hdc, 100, 100);
   SetViewportExt(hdc, rect.right/2, rect.bottom/2);
   SetViewportOrg(hdc, rect.right/2, rect.bottom/2);

   /* Draw arrow
   */
   switch ((int)syvGlobal)
      {
      case (int)syvEast:
         xEnd = dwLength;
         yEnd = 0;
         xArrow[0] = xEnd-dxArrow;
         yArrow[0] = -dyArrow;
         xArrow[1] = xEnd-dxArrow;
         yArrow[1] = dyArrow;
         break;
      case (int)syvSouth:
         xEnd = 0;
         yEnd = dwLength;
         xArrow[0] = dyArrow;
         yArrow[0] = yEnd-dxArrow;
         xArrow[1] = -dyArrow;
         yArrow[1] = yEnd-dxArrow;
         break;
      case (int)syvWest:
         xEnd = -dwLength;
         yEnd = 0;
         xArrow[0] = xEnd+dxArrow;
         yArrow[0] = -dyArrow;
         xArrow[1] = xEnd+dxArrow;
         yArrow[1] = dyArrow;
         break;
      case (int)syvNorth:
         xEnd = 0;
         yEnd = -dwLength;
         xArrow[0] = dyArrow;
         yArrow[0] = yEnd+dxArrow;
         xArrow[1] = -dyArrow;
         yArrow[1] = yEnd+dxArrow;
         break;
      case (int)syvDot:
         Ellipse(hdc, -dwLength/10, -dwLength/10, dwLength/10, dwLength/10);
         return;
      default:
         return;
      }
   MoveTo(hdc, 0, 0);
   LineTo(hdc, xEnd, yEnd);
   LineTo(hdc, xArrow[0], yArrow[0]);
   MoveTo(hdc, xEnd, yEnd);
   LineTo(hdc, xArrow[1], yArrow[1]);
   }


/*----------------------------------------------------------
Purpose: Draw a "clean" version of a shape 
Returns: --
Comment: Shape is specified in syvGlobal
*/
VOID DrawShape(
   HWND hwnd,     /* Window handle */ 
   HDC hdc)       /* DC handle */ 
   {
   RECT rect;
   RECT rectWnd;
   RECT rectT;

   rectT = shapeRect;

   /* Determine mapping region
   */
   TPtoDP((LPPOINT)&rectT, 2);
   MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rectT, 2);
   GetClientRect(hwnd, &rectWnd);

   /* Set Mapping to fit shape into window
   */
   rect = rectT;
   if (rect.top > rect.bottom) 
      {
      rect.top = rectT.bottom;
      rect.bottom = rectT.top;
      }

   if (rect.right-rect.left > rectWnd.right-2*cBorder ||
       rect.bottom-rect.top > rectWnd.bottom-2*cBorder) 
      {
      SetMapMode(hdc, MM_ISOTROPIC);
      SetWindowExt(hdc, rect.right-rect.left, rect.bottom-rect.top); 
      SetViewportExt(hdc, rectWnd.right-2*cBorder, rectWnd.bottom-2*cBorder);
      SetWindowOrg(hdc, rect.left, rect.top);
      SetViewportOrg(hdc, cBorder, cBorder);
      }
   else
      OffsetRect(&rectT
               , -(rectT.left+rectT.right)/2+(rectWnd.right - rectWnd.left)/2
               , -(rectT.top+rectT.bottom)/2+(rectWnd.bottom - rectWnd.top)/2);
      
   /* Draw "clean" shape
   */
   switch((int)syvGlobal)
      {
      case (int)SYV_SHAPELINE:
         MoveTo(hdc, rectT.left, rectT.top);
         LineTo(hdc, rectT.right, rectT.bottom);
         break;
      case (int)SYV_SHAPERECT:
         Rectangle(hdc  , rectT.left
                        , rectT.top
                        , rectT.right
                        , rectT.bottom);
         break;
      case (int)SYV_SHAPEELLIPSE:
         Ellipse(hdc , rectT.left
                     , rectT.top
                     , rectT.right
                     , rectT.bottom);
         break;
      }
   }


/*----------------------------------------------------------
Purpose: Draw the actual ink taken by the recognizer
Returns: 
Comment: We demonstrate two techniques.  One uses the DrawPenData
         API and the other enumerates through the penstrokes to
         draw the image.

         We use DrawPenData for straight-forward writing/drawing
         (when the Save All Data menuitem is not checked).

         We enumerate so we can also ink the upstrokes (pen
         movements when pen is NOT in contact with the tablet).
         We do this when the Save All Data menuitem is checked.
*/
VOID DrawRawData(HDC hdc)     /* DC handle */ 
   {
   PENDATAHEADER pendataheader;
   RECT     rectWnd;
   int      nWidth;
   
   GetClientRect(hwndRaw, &rectWnd);
   if (hpendata == NULL || rectWnd.right <= 3*cBorder ||
      rectWnd.bottom <= 3*cBorder)
      {
      return;     /* Bad handle or window is too small */
      }
   if ( !GetPenDataInfo(hpendata, &pendataheader, NULL, 0) )
      {
      return;
      }
   nWidth = NSetRawExtents(hdc, &pendataheader, &rectWnd);

   /* Draw the pen strokes
   */
   if (fSaveData)
      {
      EnumerateStrokes(hdc, &rectWnd, nWidth);
      }
   else
      {
      HPEN  hpenOld;

      hpenOld = SelectObject(hdc, CreatePen(PS_SOLID, nWidth, pendataheader.rgbInk));

      DrawPenData(hdc, NULL, hpendata);

      DeleteObject(SelectObject(hdc, hpenOld));
      }
   }



/*----------------------------------------------------------
Purpose: Set the window extents for the given DC handle
Returns: Width of ink to use when redrawing
*/
int NSetRawExtents(
   HDC hdc,                /* DC handle */ 
   LPPENDATAHEADER lppndt, /* Ptr to PENDATAHEADER struct */ 
   LPRECT lprectWnd)       /* Ptr to rectangle of window */ 
   {
   RECT     rectDP;
   LPRECT   lprect = &lppndt->rectBound;     /* In tablet coordinates */
   int      nWidth;

   /* Set mapping to fit raw data into window
   */
   SetMapMode(hdc, MM_ISOTROPIC);

   /* Window extents are the tablet-coord dimensions of drawing
   */
   SetWindowExt(hdc, lprect->right-lprect->left, lprect->bottom-lprect->top);

   rectDP = *lprect;
   TPtoDP((LPPOINT)&rectDP,2);

   /* Now set viewport extents.  Check for special case when rectDP
   ** is empty (otherwise GDI won't display anything)
   */
   if (IsRectEmpty(&rectDP))
      {
      SetViewportExt(hdc, lprect->right-lprect->left, lprect->bottom-lprect->top); 
      nWidth = lppndt->nInkWidth;
      }
   else
      {
      if (rectDP.right-rectDP.left > lprectWnd->right-2*cBorder ||
         rectDP.bottom-rectDP.top > lprectWnd->bottom-2*cBorder) 
         {
         SetViewportExt(hdc, lprectWnd->right-2*cBorder, lprectWnd->bottom - 2*cBorder);
         }
      else     /* Drawing is smaller than raw window */
         {
         SetViewportExt(hdc, rectDP.right-rectDP.left, rectDP.bottom-rectDP.top); 
         }

      /* Convert ink width to logical coordinates (tablet coordinates)
      */
      GetLPWidth(hdc, lppndt->nInkWidth, &nWidth);
      }

   SetWindowOrg(hdc, lprect->left, lprect->top);
   SetViewportOrg(hdc, cBorder, cBorder);
   return nWidth;
   }


/*----------------------------------------------------------
Purpose: Enumerate and draw each stroke
Returns: --
Comment: We enumerate and draw separate strokes so we can ink
         the upstrokes (the pen movements when pen is NOT in contact
         with the tablet).  We do this when the Save All Data
         menu-item is checked.
*/
VOID EnumerateStrokes(
   HDC hdc,             /* DC handle */ 
   LPRECT lprectWnd,    /* Ptr to rectangle of window */ 
   int nWidth)          /* Ink width */ 
   {
   LPPENDATA   lppndt;
   LPPOINT  lppoint;
   WORD     iStroke;
   STROKEINFO  si;
   HPEN     hpenDown;
   HPEN     hpenUp;
   HPEN     hpenSav;

   /* If the RCO_SAVEALLDATA bit is set in the lRcOptions field in the
   ** RC structure the recognizer saves all the pen data. If the bit is
   ** masked out, only data used by the recognizer is saved; this could
   ** include upstrokes.  Upstrokes are strokes made while the pen is
   ** not in contact with the tablet.
   **
   ** We ink the downstrokes in the ink color and the upstrokes in red.
   ** (Note that a red window background will make these invisible).
   */

   if ((lppndt = BeginEnumStrokes(hpendata)) == NULL)
      return;

   hpenUp = CreatePen(PS_SOLID, nWidth, rgbRed);
   hpenDown = CreatePen(PS_SOLID, nWidth, lppndt->rgbInk);
   hpenSav = SelectObject(hdc, hpenDown);

   for (iStroke = 0; iStroke < (WORD)lppndt->cStrokes; iStroke++)
      {
      GetPenDataStroke(lppndt, iStroke, &lppoint, NULL, &si);

      SelectObject(hdc, si.wPdk & PDK_DOWN ? hpenDown : hpenUp);

      /* Polyline requires 2 or more points. If the stroke consists
      ** of only one point use MoveTo and LineTo to draw the point.
      */
      if (si.cPnt < 2)
         {
         MoveTo(hdc, (*lppoint).x, (*lppoint).y);
         LineTo(hdc, (*lppoint).x+1, (*lppoint).y+1);
         }
      else 
         Polyline(hdc, lppoint, si.cPnt);
      }

   SelectObject(hdc, hpenSav);
   DeleteObject(hpenUp);
   DeleteObject(hpenDown);

   EndEnumStrokes(hpendata);
   }


/*----------------------------------------------------------
Purpose: Set the graph window and (re)install appropriate
         recognizer
Returns: --
*/
VOID SetGraphWindow(int mi)      /* Menu ID of recognizer to switch to */
   {
   HMENU hmenu;

   if (mi == miRecMode)    /* Stay the same? */
      return;

   hmenu = GetMenu(hwndMain);

   CheckMenuItem(hmenu, miRecMode, MF_UNCHECKED);
   miRecMode = mi;
   CheckMenuItem(hmenu, mi, MF_CHECKED);
   
   FLoadRec();
   syvGlobal = SYV_NULL;   /* Reset window globals */
   if (hpendata)
      DestroyPenData(hpendata);
   hpendata = NULL;

   InvalidateRect(hwndInput, NULL, TRUE);
   InvalidateRect(hwndRaw, NULL, TRUE);
   InvalidateRect(hwndInfo, NULL, TRUE);
   }
