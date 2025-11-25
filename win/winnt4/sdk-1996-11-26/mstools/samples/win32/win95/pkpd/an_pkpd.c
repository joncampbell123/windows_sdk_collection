/* AN_PKDK.C

   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
   PURPOSE.

   Copyright (c) 1993-1996 Microsoft Corporation.  All Rights Reserved.


   This sample program illustrates how to display, scale, and compress
   pen data on any Windows 95 computer, given pen data that has been
   collected on a computer with Windows Pen Services 2.0 and a pen tablet 
   installed. It uses and demonstrates several functions from the PKPD.DLL
   and PKPD32.DLL libraries, shipped with every Windows 95 computer.  
   
   This sample has not been written to be localizable: to do
   so, move strings to an_pkpd.rc and use the Windows LoadString API.

*/


/******************* Includes and Controlling Defines ***********************/
#include <windows.h>
#include <penwin.h>
#include <commdlg.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "anres.h"

/******************* Defines ************************************************/
#define cbSzTMax        256   // size of temp string buffers

#if (WINVER >= 0x0400)
#define cbSzFileMax        260   // max size of file names
#else
#define cbSzFileMax        128   // max size of file names
#endif

#define wExitError      1

#define CALLBACKNEVER   0
#define CALLBACKSHORT   1
#define CALLBACKLONG    2
#define CALLBACKSTROKE  3
#define SBMIN           0     // min scroll pos
#define SBMAX           10000 // ditto max
#define SBINC           10    // scroll increment
#define SBPGINC         100   // scroll page increment

#define cbPtBuf         32
#define cbBufMax        1024L // for file transfer

#define DPDEX           (IDM_DRAWEX - IDM_DRAWEX)
#define DPDPART         (IDM_DRAWPARTIAL - IDM_DRAWEX)

/******************* Typedefs ***********************************************/
typedef struct tagANDLG // dialog init [default values in brackets]
   {
   UINT uStrk0;         // first stroke to render [0]
   UINT uPt0;           // point offset in first stroke [0]
   UINT uStrk1;         // last stroke to render [-1] 
   UINT uPt1;           // point offset in last stroke [-1] 
   BOOL fSkipUp;        // FALSE to animate upstrokes, TRUE to skip them
   BOOL fAutoRepeat;    // FALSE to end after one rendering, TRUE to repeat to tap
   UINT uCBPeriodCode;  // callback Period code [CALLBACKNEVER]
   UINT uSpeedPct;      // speed of animation [100%]
   BOOL fRenderScale;   // TRUE to scale pen data to output window, FALSE to clip
   }
   ANDLG, FAR *LPANDLG;

/******************* Macros *************************************************/
#define MenuEnable(hmenu, id, f) EnableMenuItem((HMENU)(hmenu), (id), \
   ((f)? MF_ENABLED: MF_DISABLED | MF_GRAYED))

#define EnableRedraw(hdlg) do {\
   if (IsWindow(hdlg))\
      EnableWindow(GetDlgItem(hdlg, IDD_PBREDRAW),\
         vhpndt != NULL && IsWindow(vhwndOut) && IsWindowVisible(vhwndOut));\
   } while (0)

#define ErrBox(sz, szTitle){\
   MessageBox(NULL, (LPSTR)(sz), (LPSTR)(szTitle),\
      MB_TASKMODAL|MB_ICONSTOP|MB_OK);\
   }

#define InfoBox(sz, szTitle)\
   MessageBox(NULL, (LPSTR)(sz), (LPSTR)(szTitle),\
      MB_TASKMODAL|MB_ICONEXCLAMATION|MB_OK)

// make callback code into a duration in ms:
#define MakeMs(u)\
   ((u)==CALLBACKSTROKE? AI_CBSTROKE:\
   (u)==CALLBACKLONG? 1000:\
   (u)==CALLBACKSHORT? 250: 0)

// draw only part of pen data:
#define DrawPenDataPartial(hdc, lprect, hpndt, s0, s1, p0, p1)\
   DrawPenDataEx(hdc, lprect, hpndt, s0, s1, p0, p1, NULL, NULL, 0)

#define Redraw()\
   PostMessage(vhwndOut, WM_USER, 0, 0)


/******************* Variables **********************************************/
HWND vhwndAN = NULL;             // Main wnd
HWND vhdlg = NULL;               // modeless dialog
HWND vhwndOut = NULL;            // drawing window

HINSTANCE vhInstanceCur = NULL;
HINSTANCE vhPenWin = NULL;

ANIMATEPROC vlpfnAnimateProc = NULL;

HPENDATA vhpndt = NULL;             // input pen data
HPCM vhpcmInp = NULL;               // collection

PSTR vpszWndMain = "Output";         // main window title
PSTR vpszWndOut = "Output";         // draw window title
PSTR vpszClassMain = "ANclass";     // class name
PSTR vpszClassOut = "ANOutclass";   // class name of drawing window
PSTR vpszEmpty = "";                // empty string

DLGPROC vlpDlgProc = NULL;

char *vszIniFile = "animpd.ini";
ANDLG vandlg =
   {
   0,             // uStrk0         =from first stroke
   0,             // uPt0           =from first point
   (UINT)-1,      // uStrk1         =to last stroke
   (UINT)-1,      // uPt1           =to last point
   FALSE,         // fSkipUp        =wait during up periods too
   FALSE,         // fAutoRepeat    =cycle only once
   CALLBACKSHORT, // uCBPeriodCode  =callback every 250 ms
   100,           // uSpeedPct      =original speed
   TRUE          // fRenderScale   =render scaled to window
   };

int nDrawProc = DPDEX;
UINT vcCB = 0;                // for callback display count
BOOL vfCB = FALSE;            // enable callback display, initially off
BOOL vfDrawing = FALSE;       // TRUE when drawing
UINT vnPDTS; 				  // current scaling mode 
BOOL vfScaled;				  // TRUE after scaled first time

// user requests during callback:
BOOL vfReqCxl = FALSE;        // dialog redraw cancel
BOOL vfReqClear = FALSE;      // set to TRUE on request to clear pen data
BOOL vfReqExit = FALSE;       // set to TRUE on request to exit app
BOOL vfReqCloseOut = FALSE;   // set to TRUE on request to close output
int vnReqLoadSave = -1;       // set to 0 or 1 on request to do file i/o

RECT vrectOut;                // location of output window

char vszFile[cbSzFileMax];
char *vszSaveFileDef = "an.pdt";

/******************* Export Prototypes **************************************/
BOOL CALLBACK ANDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AnimateProc(HPENDATA, UINT, UINT, UINT FAR*, LPARAM);
LRESULT CALLBACK ANOutWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ANWndProc(HWND, UINT, WPARAM, LPARAM);

/******************* Local prototypes ***************************************/
VOID NEAR PASCAL CancelDrawing(VOID);
VOID NEAR PASCAL ClearAppQueue(VOID);
VOID NEAR PASCAL ClearOutputWindow(VOID);
BOOL NEAR PASCAL FGetFileName(HWND, BOOL, LPSTR);
BOOL NEAR PASCAL FInitAN(HINSTANCE, HINSTANCE, LPSTR);
BOOL NEAR PASCAL FInitInstance(HINSTANCE, HINSTANCE, int);
UINT NEAR PASCAL GetEditVal(HWND, int, UINT);
VOID NEAR PASCAL InitDlgItemInt(HWND, int, int);
VOID NEAR PASCAL LoadSave(BOOL);
BOOL NEAR PASCAL MakeOutputWindow(HWND, int, int, int, int, int);
HPENDATA NEAR PASCAL ReadPenData(HFILE hfile);
VOID NEAR PASCAL ShowCancel(BOOL);
VOID NEAR PASCAL TermAN(VOID);
BOOL NEAR PASCAL WritePenData(HFILE hfile, HPENDATA hpndt);

/******************* EXPORT FUNCTIONS ***************************************/

/*+-------------------------------------------------------------------------*/
BOOL CALLBACK           // ret TRUE if handled
ANDlgProc(              // main modal dialog
   HWND hdlg,           // will become vhdlg
   UINT message,
   WPARAM wParam,
   LPARAM lParam)
   {
   UINT wNotifyCode;
   UINT u;
   HWND hSBSpeed = GetDlgItem(hdlg, IDD_SBSPEED);
   static RECT rectDlg;

   switch (message)
      {
   case WM_INITDIALOG:  // in lieu of WM_CREATE
      InitDlgItemInt(hdlg, IDD_ETSTRK0, (int)vandlg.uStrk0);
      InitDlgItemInt(hdlg, IDD_ETPT0, (int)vandlg.uPt0);
      InitDlgItemInt(hdlg, IDD_ETSTRK1, (int)vandlg.uStrk1);
      InitDlgItemInt(hdlg, IDD_ETPT1, (int)vandlg.uPt1);

      CheckDlgButton(hdlg, IDD_CHSKIPUP, vandlg.fSkipUp);
      CheckDlgButton(hdlg, IDD_CHAUTOREPEAT, vandlg.fAutoRepeat);

      CheckRadioButton(hdlg, IDD_RBCBNEVER, IDD_RBCBSTROKE,
         IDD_RBCBNEVER + vandlg.uCBPeriodCode);

      InitDlgItemInt(hdlg, IDD_ETSPEED, (int)vandlg.uSpeedPct);

      SetScrollRange(hSBSpeed, SB_CTL, SBMIN, SBMAX, FALSE);
      SetScrollPos(hSBSpeed, SB_CTL, vandlg.uSpeedPct, TRUE);

      CheckRadioButton(hdlg, IDD_RBRENDERSCALE, IDD_RBRENDERCLIP,
         IDD_RBRENDERSCALE + !vandlg.fRenderScale);
      if (!IsRectEmpty(&rectDlg))
         SetWindowPos(hdlg, NULL, rectDlg.left, rectDlg.top, 0, 0,
            SWP_NOSIZE | SWP_NOZORDER);

      EnableRedraw(hdlg);
      EnableWindow(GetDlgItem(hdlg, IDD_PBCLEAR), vhpndt != NULL);
      return FALSE;     // let system set focus

   case WM_COMMAND:     // user actions
#ifdef WIN32
      wNotifyCode = HIWORD(wParam);
#else
      wNotifyCode = HIWORD(lParam);
#endif

      switch (LOWORD(wParam))
         {
      case IDD_ETSTRK0:
         if (wNotifyCode == EN_KILLFOCUS)
            vandlg.uStrk0 = GetEditVal(hdlg, LOWORD(wParam), vandlg.uStrk0);
         break;

      case IDD_ETPT0:
         if (wNotifyCode == EN_KILLFOCUS)
            vandlg.uPt0 = GetEditVal(hdlg, LOWORD(wParam), vandlg.uPt0);
         break;

      case IDD_ETSTRK1:
         if (wNotifyCode == EN_KILLFOCUS)
            vandlg.uStrk1 = GetEditVal(hdlg, LOWORD(wParam), vandlg.uStrk1);
         break;

      case IDD_ETPT1:
         if (wNotifyCode == EN_KILLFOCUS)
            vandlg.uPt1 = GetEditVal(hdlg, LOWORD(wParam), vandlg.uPt1);
         break;

      case IDD_CHSKIPUP:
         vandlg.fSkipUp = IsDlgButtonChecked(hdlg, IDD_CHSKIPUP)==1;
         break;

      case IDD_CHAUTOREPEAT:
         vandlg.fAutoRepeat = IsDlgButtonChecked(hdlg, IDD_CHAUTOREPEAT)==1;
         break;

      case IDD_RBCBNEVER:
      case IDD_RBCB250MS:
      case IDD_RBCB1SEC:
      case IDD_RBCBSTROKE:
         for (u = IDD_RBCBSTROKE - IDD_RBCBNEVER; u > 0; u--)
            if (IsDlgButtonChecked(hdlg, IDD_RBCBNEVER+u))
               break;
         vandlg.uCBPeriodCode = u;
         break;

      case IDD_ETSPEED:
         if (wNotifyCode == EN_KILLFOCUS)
            vandlg.uSpeedPct = GetEditVal(hdlg, LOWORD(wParam), vandlg.uSpeedPct);
         if ((UINT)GetScrollPos(hSBSpeed, SB_CTL) != vandlg.uSpeedPct)
#ifdef WIN32
            PostMessage(hdlg, WM_HSCROLL,
               MAKEWPARAM(SB_THUMBPOSITION, vandlg.uSpeedPct),
               (LPARAM)hSBSpeed);
#else
            PostMessage(hdlg, WM_HSCROLL, SB_THUMBPOSITION,
               MAKELPARAM(vandlg.uSpeedPct, hSBSpeed));
#endif // WIN32
         break;

      case IDD_SBSPEED:
         // see WM_HSCROLL
         break;


      case IDD_RBRENDERSCALE:
      case IDD_RBRENDERCLIP:
         vandlg.fRenderScale = IsDlgButtonChecked(hdlg, IDD_RBRENDERSCALE)==1;
         break;

      case IDD_PBCLEAR:
         if (vfDrawing)
            {
            CancelDrawing();
            vfReqClear = TRUE;   // after DrawPenDataEx ret
            }
         else
            ClearOutputWindow();
         break;

      case IDD_PBREDRAW:
         /*
            The Redraw() macro really invalidates, so pressing the button
            multiple times only invalidates once. It doesn't redraw
            the same number of times the button is pushed.
         */
         Redraw();
         break;

      case IDD_PBEXIT:
         PostMessage(hdlg, WM_CLOSE, 0, 0L);
         break;

      case IDCANCEL:
         CancelDrawing();
         break;

      default:
         return TRUE;
         }
      return FALSE;  // WM_COMMAND processed

   case WM_HSCROLL:
#ifdef WIN32
      if ((HWND)lParam == hSBSpeed)
#else
      if ((HWND)HIWORD(lParam) == hSBSpeed)
#endif // WIN32
         {
         int jT, j = (int)vandlg.uSpeedPct;

         switch (LOWORD(wParam))
            {
         case SB_LEFT:
            j = SBMIN;
            break;

         case SB_RIGHT:
            j = SBMAX;
            break;

         case SB_LINELEFT:
            j = j-SBINC > SBMIN? j-SBINC: SBMIN;
            break;

         case SB_LINERIGHT:
            j = j+SBINC < SBMAX? j+SBINC: SBMAX;
            break;

         case SB_PAGELEFT:
            j = j-SBPGINC > SBMIN? j-SBPGINC: SBMIN;
            break;

         case SB_PAGERIGHT:
            j = j+SBPGINC < SBMAX? j+SBPGINC: SBMAX;
            break;

         case SB_THUMBTRACK:
         case SB_THUMBPOSITION:
#ifdef WIN32
            j = (int)HIWORD(wParam);
#else
            j = (int)LOWORD(lParam);
#endif // WIN32

            // adjust to nearest:
            if (jT = (j-SBMIN) % SBINC)
               j += SBINC - jT < jT? SBINC - jT: -jT;
            break;

            }
         SetScrollPos(hSBSpeed, SB_CTL, vandlg.uSpeedPct = (UINT)j, TRUE);

         if (j != (int)GetEditVal(hdlg, IDD_ETSPEED, j))
            SetDlgItemInt(hdlg, IDD_ETSPEED, (int)vandlg.uSpeedPct, TRUE);
         }
      break;   // WM_HSCROLL

   case WM_CLOSE:
      PostMessage(vhwndAN, WM_COMMAND, IDM_DLG, 0L);  // toggle dlg off
      break;

   case WM_DESTROY:
      GetWindowRect(hdlg, &rectDlg);
      vhdlg = NULL;
      break;

   default:
      break;
      }

   return FALSE;
   }


/*+-------------------------------------------------------------------------*/
BOOL CALLBACK        // ret LRESULT; NB _export to ensure correct ds
AnimateProc(                  // animation callback proc
   HPENDATA hpndt,            // pen data
   UINT wStroke,              // current stroke
   UINT cPnts,                // number of points yet to draw
   UINT FAR *lpuSpeedPct,     // addr of speed pct
   LPARAM lParam)             // app value
   {
   BOOL fRet = !vfReqCxl;     // set in dialog and File menu
   hpndt, wStroke, cPnts, lParam;   // unused

   if (fRet)
      {
      char sz[cbSzTMax];

      if (!vcCB)
         ShowCancel(TRUE);

      ClearAppQueue();     // handle message backlog in app queue

      *lpuSpeedPct = vandlg.uSpeedPct;    // get latest speed setting

      wsprintf((LPSTR)sz, (LPSTR)"CB=%u", ++vcCB);
        if (vfCB) 
           SetWindowText(vhwndAN, (LPSTR)sz);

      // vfReqCxl may have gotten set in ANOutWndProc's WM_PAINT, if
      // the user changed the window size during a callback for example:
      fRet = !vfReqCxl;
      }

   return fRet;
   }



/*+-------------------------------------------------------------------------*/
LRESULT CALLBACK        // ret LRESULT
ANOutWndProc(           // drawing window proc
   HWND hwnd,           // == vhwndOut
   UINT message,
   WPARAM wParam,
   LPARAM lParam)
   {
   LRESULT lRet = 1L;
   static char const szBoxTitle[] = "DrawPenDataEx";
   int nDisplayWidth, nDisplayHeight, nMapWidth, nMapHeight;
       
   switch (message)
      {
   case WM_USER:  // for Redraw() macro
      InvalidateRect(hwnd, NULL, TRUE);
      if (IsWindow(vhdlg))
         EnableWindow(GetDlgItem(vhdlg, IDD_PBCLEAR), vhpndt != NULL);
      SetFocus(vhwndOut);     // catch Esc
      break;

   case WM_CHAR:
      if (wParam == VK_ESCAPE)
         vfReqCxl = TRUE;
      break;

   case WM_PAINT:
      if (vfDrawing)
         {
         // since vfDrawing is TRUE, we are repainting during a
         // callback, so we have to cancel the current animation first:
         vfReqCxl = TRUE;  // this is picked up in AnimateProc()
         break;
         }

      EnableRedraw(vhdlg);

      if (vhpndt)
         {
         PAINTSTRUCT ps;
         HDC hdc = BeginPaint(hwnd, &ps);
         
         vfDrawing = TRUE;       // set semaphore

         if (hdc)
            {
            RECT r;      
            float AspectRatio;
            int iRet;
            ANIMATEINFO ai =
               {
               sizeof(ANIMATEINFO),                   // struct size
               vandlg.uSpeedPct,                      // speed as a percent
               MakeMs(vandlg.uCBPeriodCode),          // callback Period in ms
               vandlg.fSkipUp? AI_SKIPUPSTROKES: 0,   // options
               0L,                                    // lParam
               0L                                     // reserved
               };

       
            if (vandlg.fRenderScale)            // scale to window
               GetClientRect(hwnd, &r);
            else 
             if (vfScaled == FALSE)
			  {
                 // display in same aspect ratio as original ink and 
                 // clip if outside window

                 // scale the pen data to display coordinates and trim excess data
                 if(MetricScalePenData(vhpndt, PDTS_DISPLAY))
                 	TrimPenData(vhpndt, TPD_COLLINEAR |      // duplicate strokes
                 						TPD_EMPTYSTROKES | 	 // strokes w/o points
                 						TPD_USER | 			 // header info
                 						TPD_PENINFO | 		 // OEM data
                 						TPD_OEMDATA ,  		 // OEM data
                 						0);   
                 						
                 // use the following if not animating and not using up strokes
                 //TrimPenData(vhpndt, TPD_EVERYTHING, 0);
                 	
                 // set mapping mode to same as PDTS_DISPLAY 
                 SetMapMode(hdc, MM_TEXT); 
                      
                 // get bounding rectangle of pen data in PDTS_DISPLAY mapping mode 
                 GetPenDataAttributes(vhpndt, &r, GPA_RECTBOUND);
                 nMapWidth = r.right - r.left;
                 nMapHeight = r.bottom - r.top;

                 // get screen resolution
                 nDisplayWidth=GetDeviceCaps(hdc, HORZRES);
                 nDisplayHeight=GetDeviceCaps(hdc, VERTRES);

                 // if still to big resize data to fit on screen
                 if (r.right > nDisplayWidth || r.bottom > nDisplayHeight)
                 {
	                 // resize rectangle maintaining aspect ratio
	                  if(nMapWidth > nMapHeight)
	                  	AspectRatio = (float)nDisplayWidth/(float)nMapWidth;
	                  else
	                    AspectRatio = (float)nDisplayHeight/(float)nMapHeight;
	                  
	                  // reduce slightly for margins on screen 
	                  AspectRatio *= (float) 0.8;  
	                  
					  // set right and bottom of rectangle to new size
	                  r.right = (int) (AspectRatio * (float)r.right);
	                  r.bottom = (int) (AspectRatio * (float)r.bottom);
	                  
					  // resize the pen data to new rectangle
	                  ResizePenData(vhpndt, &r);
	             }
				 // do scaling only once
				 vfScaled = TRUE;
               }    
			 else  // vfScaled == TRUE
				   // get current rectangle for pen data
                 GetPenDataAttributes(vhpndt, &r, GPA_RECTBOUND);
               
         switch (nDrawProc)
               {
            case DPDEX:    // animation
               vcCB = 0;                        // animate callback counter
               vfReqCxl = FALSE;             // reset
               ShowCancel(vandlg.uCBPeriodCode != CALLBACKNEVER);

               iRet = DrawPenDataEx(
                  hdc,                          // handle to DC
                  &r,                           // rect for scaling/clipping
                  vhpndt,                       // pen data
                  vandlg.uStrk0,                // first stroke
                  vandlg.uStrk1,                // last stroke
                  vandlg.uPt0,                  // first point in first stroke
                  vandlg.uPt1,                  // last point in last stroke
                  vlpfnAnimateProc,             // AnimateProc callback function
                  &ai,
                  0);

               if (iRet < 0 && iRet >= -10)
                  {
                  static char *szErr[] =
                     {
                     "PDR_ERROR",         // -1  PDR_ERROR
                     "PDR_PNDTERR",       // -2  PDR_PNDTERR
                     "PDR_VERSIONERR",    // -3  PDR_VERSIONERR
                     "PDR_COMPRESSED",    // -4  PDR_COMPRESSED
                     "PDR_STRKINDEXERR",  // -5  PDR_STRKINDEXERR
                     "PDR_PNTINDEXERR",   // -6  PDR_PNTINDEXERR
                     "PDR_MEMERR",        // -7  PDR_MEMERR
                     "PDR_INKSETERR",     // -8  PDR_INKSETERR
                     "PDR_ABORT",         // -9  PDR_ABORT
                     "PDR_NA",            // -10 PDR_NA
                     };

                  ErrBox(szErr[-iRet-1], szBoxTitle);
                  CancelDrawing();
                  }

               if (!iRet && !vfReqCxl)
                  {
                  InfoBox("PDR_CANCEL: callback impasse", szBoxTitle);
                  CancelDrawing();
                  }

               if (!vandlg.fAutoRepeat)
                  ShowCancel(FALSE);            // hide cancel button again

               if (IsWindow(vhdlg)
                  && (iRet == PDR_STRKINDEXERR || iRet == PDR_PNTINDEXERR))
                  {
                  SetFocus(vhdlg);  // required
                  SetFocus(GetDlgItem(vhdlg,
                     iRet == PDR_STRKINDEXERR?
                        IDD_ETSTRK0:
                        IDD_ETPT0));
                  }

               vfReqCxl = FALSE;
               break;

            case DPDPART:  // partial drawing
            default:
               DrawPenDataPartial(hdc, // DC
                  &r,               // rect for scaling/clipping
                  vhpndt,           // pen data
                  vandlg.uStrk0,    // first stroke to draw
                  vandlg.uStrk1,    // last stroke to draw
                  vandlg.uPt0,      // first point in first stroke to draw
                  vandlg.uPt1);     // last point in last stroke to draw
               break;
               }

            ClearAppQueue();  // handle message backlog if any

            if (vandlg.fAutoRepeat)
               Redraw();
            }

         EndPaint(hwnd, &ps);
         vfDrawing = FALSE;
         }

      // user requested exit or file i/o during callback; now we can do it:
      if (vfReqExit)       // check this one first
         {
         PostMessage(vhwndAN, WM_CLOSE, 0, 0L);
         break;
         }

      if (vfReqCloseOut)
         {
         vfReqCloseOut = FALSE;
         PostMessage(vhwndOut, WM_CLOSE, 0, 0L);
         break;
         }

      if (vnReqLoadSave != -1)
         {
         BOOL fLoad = vnReqLoadSave > 0;

         vnReqLoadSave = -1;     // reset global first
         LoadSave(fLoad);
         }

      if (vfReqClear)
         {
         vfReqClear = FALSE;
         ClearOutputWindow();
         }

      break;


   case WM_CLOSE:
      if (vfDrawing)
         {
         vfReqCloseOut = TRUE;   // can't close it just yet
         return 0;
         }
      break;

   case WM_DESTROY:
      GetWindowRect(hwnd, &vrectOut);  // save position
      vhwndOut = NULL;
      EnableRedraw(vhdlg);
      break;

   default:
      break;
      }

   return DefWindowProc(hwnd, message, wParam, lParam);
   }


/*+-------------------------------------------------------------------------*/
LRESULT CALLBACK        // ret LRESULT
ANWndProc(              // main window proc
   HWND hwnd,           // == vhwndAN
   UINT message,
   WPARAM wParam,
   LPARAM lParam)
   {
   LRESULT lRet = 1L;

   switch (message)
      {

 
   case WM_SIZE:
      if (IsWindow(vhwndOut))
         MoveWindow(vhwndOut, 0, 0, LOWORD(lParam), HIWORD(lParam), FALSE);
      break;

   case WM_INITMENU:
      MenuEnable((HMENU)wParam, IDM_OUTPUT, vhwndOut == NULL);
      CheckMenuItem((HMENU)wParam, IDM_CBDISP, vfCB? MF_CHECKED: MF_UNCHECKED);
      break;

   case WM_COMMAND:
      {
      HMENU hmenu = GetMenu(vhwndAN);

      switch (LOWORD(wParam))
         {
      case IDM_DRAWEX:        // animate with DrawPenDataEx
      case IDM_DRAWPARTIAL:   // DrawPenDataEx
         CheckMenuItem(hmenu, IDM_DRAWEX+nDrawProc, MF_UNCHECKED);
         nDrawProc = LOWORD(wParam) - IDM_DRAWEX;
         CheckMenuItem(hmenu, LOWORD(wParam), MF_CHECKED);
         Redraw();
         return 0;

      case IDM_DLG:
         {
         BOOL fCheck = GetMenuState(hmenu, LOWORD(wParam), MF_BYCOMMAND) == MF_CHECKED;

         if (fCheck)
            DestroyWindow(vhdlg);   // sets vhdlg to NULL
         else
            {
            vhdlg = CreateDialog(vhInstanceCur,
               MAKEINTRESOURCE(IDD_ANIMATE), hwnd, vlpDlgProc);
            }

         CheckMenuItem(hmenu, LOWORD(wParam), fCheck? MF_UNCHECKED: MF_CHECKED);
         }
         return 0;


      case IDM_CBDISP:        // set callback display
         vfCB = !vfCB;
         if (!vfCB)           // turn off: restore title
            SetWindowText(vhwndAN, (LPSTR)vpszWndOut);
         return 0;

      case IDM_OPEN:
      case IDM_SAVE:
         if (vfDrawing)       // defer file i/o until ret from drawing
            {
            CancelDrawing();
            vnReqLoadSave = (LOWORD(wParam) == IDM_OPEN);   // 1 to load, 0 to save
            }
         else
            LoadSave(LOWORD(wParam) == IDM_OPEN);
         return 0;

      case IDM_EXIT:
         PostMessage(hwnd, WM_CLOSE, 0, 0L);
         return 0;

      default:
         break;
         }
      }
      break;

   case WM_ERASEBKGND:
      return 1L;  // skip irritating flash

   case WM_PENMISC:
      if (wParam == PMSC_LOADPW && lParam == PMSCL_UNLOADING)
         PostQuitMessage(0);
      break;

   case WM_CLOSE:
      if (vfDrawing)
         {
         CancelDrawing();
         vfReqExit = TRUE; // defer exit until ret from drawing
         return 0;         // abort close
         }
      break;               // go to termination

   case WM_DESTROY:
      TermAN();
      vhwndAN = NULL;
      PostQuitMessage(0);
      break;

   default:
      break;
      }

   return DefWindowProc(hwnd, message, wParam, lParam);
   }


/******************* LOCAL FUNCTIONS ****************************************/

/*+-------------------------------------------------------------------------*/
VOID NEAR PASCAL  // no ret
CancelDrawing(    // sets cancel-drawing flag and updates dialog box
   VOID)          // no params
   {
   vfReqCxl = TRUE;
   if (vandlg.fAutoRepeat)
      {
      vandlg.fAutoRepeat = FALSE;
      if (IsWindow(vhdlg) && IsWindowVisible(vhdlg))
         CheckDlgButton(vhdlg, IDD_CHAUTOREPEAT, vandlg.fAutoRepeat);
      }
   ShowCancel(FALSE);   // update button face
   }

/*+-------------------------------------------------------------------------*/
VOID NEAR PASCAL        // no ret
ClearAppQueue(          // cycles through pending messages to yield.
   VOID)                // no params
   {
   MSG msg;

   while (PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE))
      {
      if (!vhdlg || !IsDialogMessage(vhdlg, &msg))
         {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
         }
      }
   }


/*+-------------------------------------------------------------------------*/
VOID NEAR PASCAL        // no ret
ClearOutputWindow(      // clears vhwndOut
   VOID)                // no params
   {
   if (vhpndt)
      {
      DestroyPenData(vhpndt);
      vhpndt = NULL;
      Redraw();
      }
   }


/*+-------------------------------------------------------------------------*/
BOOL NEAR PASCAL        // ret T if user selected a file
FGetFileName(           // get save/load filename
   HWND hwnd,           // owner
   BOOL fRead,          // T read F write
   LPSTR lpszFile)      // addr of filename
/*------------------------
:  see <commdlg.h> and MS C7 Prog Ref/2 p.414
------------------------*/
   {
   static OPENFILENAME ofn;
   char szPath[cbSzTMax];
   char szFile[cbSzTMax];
   char szDlgTitle[cbSzTMax];
   char szFilter[cbSzTMax];
   char chReplace;
   UINT i, cb;
   BOOL fRet = FALSE;                
   
   if (!LoadString(vhInstanceCur, RS_OFNWRITETITLE,
      (LPSTR)szDlgTitle, cbSzTMax))
         return FALSE;
   if (!(cb = LoadString(vhInstanceCur, RS_FILTER, (LPSTR)szFilter, cbSzTMax)))
      return FALSE;
   chReplace = szFilter[cb-1];   // retrieve wild character
   for (i = 0; szFilter[i]; i++)
      if (szFilter[i] == chReplace)
         szFilter[i] = '\0';

   if (!ofn.lpstrInitialDir)
      GetSystemDirectory(szPath, cbSzTMax);

   GetFileTitle(lpszFile, (LPSTR)szFile, cbSzTMax);   // strip path

   // setup info for common dialog:
   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hwnd;
   ofn.hInstance = vhInstanceCur;
   ofn.lpstrFilter = (LPSTR)szFilter;     // file types
   ofn.lpstrCustomFilter = NULL;
   ofn.nFilterIndex = 0;                  // index of first one
   ofn.nMaxCustFilter = 0;
   ofn.lpstrFile = (LPSTR)szFile;         // initial file suggested
   ofn.nMaxFile = cbSzTMax;
   ofn.lpstrTitle = (LPSTR)szDlgTitle;    // dlg title
   ofn.lpstrInitialDir = (LPSTR)szPath;   // initial path
   ofn.Flags = OFN_SHOWHELP | OFN_HIDEREADONLY
      | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_SHAREAWARE;
   ofn.lpstrDefExt = "*";                 // default extension
   ofn.lpfnHook = NULL;

   fRet = fRead? GetOpenFileName((LPOPENFILENAME)&ofn):
      GetSaveFileName((LPOPENFILENAME)&ofn);

   if (fRet)
      lstrcpy(lpszFile, (LPSTR)ofn.lpstrFile);

   return fRet;
   }


/*+-------------------------------------------------------------------------*/
BOOL NEAR PASCAL           // ret T if successful init
FInitAN(                   // init class[es], icon
   HINSTANCE hInstance,    // this instance
   HINSTANCE hPrevInstance,// prev instance if any
   LPSTR lpszCommandLine)  // ptr to command line
   {
   WNDCLASS wndClass;
   char szIcon[] = "iconAN";
 //  WORD wSysFlags;

   if (!hPrevInstance)
      {
      vhInstanceCur = hInstance;

      // Main Window
      wndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
      wndClass.hIcon          = LoadIcon(hInstance, szIcon);
      wndClass.lpszMenuName   = NULL;
      wndClass.lpszClassName  = vpszClassMain;
      wndClass.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
      wndClass.hInstance      = hInstance;
      wndClass.style          = CS_VREDRAW | CS_HREDRAW;
      wndClass.lpfnWndProc    = ANWndProc;
      wndClass.cbClsExtra     = 0;
      wndClass.cbWndExtra     = 0;

      if (!RegisterClass(&wndClass))
         return FALSE;


      // Output Window
      wndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
      wndClass.hIcon          = NULL;
      wndClass.lpszMenuName   = NULL;
      wndClass.lpszClassName  = vpszClassOut;
      wndClass.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
      wndClass.hInstance      = hInstance;
      wndClass.style          = CS_VREDRAW | CS_HREDRAW;
      wndClass.lpfnWndProc    = ANOutWndProc;
      wndClass.cbClsExtra     = 0;
      wndClass.cbWndExtra     = 0;

      if (!RegisterClass(&wndClass))
         return FALSE;
      }

   return TRUE;
   }

/*+-------------------------------------------------------------------------*/
BOOL NEAR PASCAL              // ret T if init successful
FInitInstance(                // create windows and init
   HINSTANCE hInstance,       // this instance
   HINSTANCE hPrevInstance,   // prev instance
   int cmdShow)               // show command
   {
   DWORD dwStyle = WS_OVERLAPPEDWINDOW;
   DWORD dwExStyle = 0; // could be topmost
   HMENU hMenu = NULL;
   int xWnd = 0;
   int yWnd = 0;
   int cxWnd = GetSystemMetrics(SM_CXSCREEN) / 2;
   int cyWnd = GetSystemMetrics(SM_CYSCREEN) / 3;


   hPrevInstance; // noref

   hMenu = LoadMenu(hInstance, "ANMenu");
   vlpDlgProc = (DLGPROC)MakeProcInstance((FARPROC)ANDlgProc, vhInstanceCur);

   vlpfnAnimateProc = (ANIMATEPROC)MakeProcInstance(
      (FARPROC)AnimateProc, vhInstanceCur);



   // Create main window:
   if (!(vhwndAN = CreateWindowEx(
      dwExStyle,        // extended style
      vpszClassMain,    // class name
      vpszWndMain,      // window title
	  dwStyle,          // main style
      xWnd, yWnd,       // pos
      cxWnd, cyWnd,     // size
      NULL,             // no parent
      hMenu,            // menu if any
      hInstance,        // instance handle
      NULL              // no params
      )))
      return FALSE;



   // Create CHILD output window:
   if (!(vhwndOut = CreateWindow(
      vpszClassOut,      // class name
      NULL,             // window title
      WS_CHILD,         // style
      0, 0,
      0, 0,
      vhwndAN,
      NULL,             // no menu
      hInstance,        // instance handle
      NULL              // no params
      )))
      return FALSE;   
    


   // start with dialog box open:
   PostMessage(vhwndAN, WM_COMMAND, IDM_DLG, 0L);

   ShowWindow(vhwndAN, cmdShow);
   ShowWindow(vhwndOut, cmdShow);
   UpdateWindow(vhwndAN);
   UpdateWindow(vhwndOut);
   return TRUE; 
   } 

/*+-------------------------------------------------------------------------*/
UINT NEAR PASCAL
GetEditVal(
   HWND hdlg,
   int id,
   UINT u)
   {
   char sz[cbSzTMax];
   LPSTR lpsz = (LPSTR)sz;

   GetDlgItemText(hdlg, id, lpsz, cbSzTMax);

   if (!isdigit(*lpsz) && *lpsz != '-')
      goto errGEV;      // first char not digit or minus sign

   for (++lpsz; *lpsz; lpsz++)
      if (!isdigit(*lpsz))
         goto errGEV;   // other char not digit

   return (UINT)atoi(sz);  // good value

errGEV:
   // error in input: restore old value:
   SetDlgItemInt(hdlg, id, u, TRUE);
   MessageBeep(0);
   return u;
   }

/*+-------------------------------------------------------------------------*/
VOID NEAR PASCAL                               
InitDlgItemInt(
   HWND hdlg,
   int id,
   int i)
   {
   HWND hwndEdit = GetDlgItem(hdlg, id);
   HRC hrc = (HRC)SendMessage(hwndEdit, WM_PENMISC, PMSC_GETHRC, 0L);

   SetDlgItemInt(hdlg, id, i, TRUE);

   }

/*+-------------------------------------------------------------------------*/
VOID NEAR PASCAL
LoadSave(
   BOOL fOpen)
   {
   HCURSOR hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
   HFILE hfile;
   static char const szOpenTitle[] = "Open File";
   static char const szSaveTitle[] = "Save File";

   if (!*vszFile)
      lstrcpy((LPSTR)vszFile, vszSaveFileDef);

   if (FGetFileName(vhwndAN, fOpen, vszFile))
      {
      OFSTRUCT of;
      if (!vhpndt)
        // create a pen data object to read pen data into
      	vhpndt = CreatePenDataEx((LPPENINFO)NULL, PDTS_STANDARDSCALE, 0, 0);

      if ((hfile = OpenFile((LPSTR)vszFile, &of,
         fOpen? OF_READ: OF_CREATE)) != HFILE_ERROR)
        {
        if (fOpen)
         {
            if (vhpndt)  DestroyPenData(vhpndt);
            vhpndt = ReadPenData(hfile);
			vfScaled = FALSE;
            // determine whether pen data is compressed
            vnPDTS = GetPenDataAttributes(vhpndt, NULL, GPA_PDTS); 
            // if so, decompress it 
            if ((vnPDTS & PDTS_COMPRESSED) == PDTS_COMPRESSED) 
            	CompressPenData(vhpndt, CMPD_DECOMPRESS, 0);
    
            Redraw();
         }
         else{
            // compress pen data before saving it 
            CompressPenData(vhpndt, CMPD_COMPRESS, 0);
            WritePenData(hfile, vhpndt);
            // decompress pen data for further displaying
            CompressPenData(vhpndt, CMPD_DECOMPRESS, 0); 
         }
         _lclose(hfile);
         }
      else ErrBox("error opening file", szOpenTitle);
      }
   else if (fOpen)
      InfoBox("did not get file", szOpenTitle);
   else 
      InfoBox("did not save file", szOpenTitle);
   SetCursor(hCursor);
   
 }




/*+-------------------------------------------------------------------------*/
HPENDATA NEAR PASCAL ReadPenData(   // ret handle to pen data
   HFILE hfile)   // Handle of open file
/*------------------------
:  Reads pen data from a file. The file format at this point is
:  a UINT value representing the size of the pen data, followed
:  by that many bytes of pen data.
:
:  Before calling this function, the caller should have already
:  opened the file specified by hfile and ensured that the
:  file pointer is offset to the beginning of the pen data.
:  When the function returns, the file pointer will be offset
:  to the end of the pen data in the file.
------------------------*/
   {
   HPENDATA hpndt = NULL;
   LONG     cb, cbRead, cbHpndt;
   BYTE     lpbBuf[cbBufMax];    // buffer
   DWORD    dwState = 0L;        // required init
   BOOL     fError = FALSE;

   if (!hfile
      || (cb = _lread(hfile, &cbHpndt, sizeof(LONG))) == HFILE_ERROR
      || cb != sizeof(LONG))
         return NULL;

   while (cbHpndt > 0)
      {
      if ((cbRead = _lread(hfile, lpbBuf, (UINT)min(cbHpndt, cbBufMax)))
         == HFILE_ERROR
         || PenDataFromBuffer(&hpndt, 0, lpbBuf, cbBufMax, &dwState) < 0)
         {
         if (hpndt)
            DestroyPenData(hpndt);
         return NULL;
         }
      cbHpndt -= cbRead;
      }

   return hpndt;
   }


/*+-------------------------------------------------------------------------*/
VOID NEAR PASCAL  // no ret
ShowCancel(       // hides the redraw button and shows the cancel button in the options dialog
   BOOL fShow)    // TRUE show cancel button, FALSE show redraw
/*------------------------
:  both buttons occupy the same place. Escape is a keyboard accelerator
:     for Cancel.
------------------------*/
   {
   if (IsWindow(vhdlg))
      {
      HWND hwndShow = GetDlgItem(vhdlg, fShow? IDCANCEL: IDD_PBREDRAW);
      HWND hwndHide = GetDlgItem(vhdlg, fShow? IDD_PBREDRAW: IDCANCEL);

      if (fShow != IsWindowVisible(GetDlgItem(vhdlg, IDCANCEL)))
         {
         ShowWindow(hwndShow, SW_SHOW);
         ShowWindow(hwndHide, SW_HIDE);
         InvalidateRect(hwndShow, NULL, FALSE);
         UpdateWindow(hwndShow);
         }
      }
   }

/*+-------------------------------------------------------------------------*/
VOID NEAR PASCAL        // no ret
TermAN(                 // terminate app
   VOID)                // no params
   {
   static BOOL fTerminated = FALSE;

   if (!fTerminated)
      {
      FreeProcInstance((FARPROC)vlpDlgProc);
      vlpDlgProc = NULL;
      fTerminated = TRUE;
      }
   }



/*+-------------------------------------------------------------------------*/
BOOL NEAR PASCAL WritePenData(   // ret TRUE if successful
   HFILE hfile,         // Handle to open file
   HPENDATA hpndt)      // pen data to write
/*------------------------
:  Writes pen data into a file, preceded by a UINT consisting of
:  the size of the pen data in bytes.
:
:  Before calling this function, the caller should have
:  already opened the file specified by hfile and ensured that
:  the file pointer is correctly placed.  When the function
:  returns, the file pointer will be offset to the end of the
:  pen data in the file. The function fails if the pen data is
:  larger than 64K.
------------------------*/
   {
   BYTE lpbBuf[cbBufMax];
   DWORD dwState = 0L;  // required init
   LONG cb;
   LONG cbSize;

   if (!hfile || !hpndt)
      return FALSE;

   if (GetPenDataAttributes(hpndt, (LPVOID)&cbSize, GPA_SIZE) < 0)
      return FALSE;

   if (_lwrite(hfile, (LPCSTR)&cbSize, sizeof(LONG)) == HFILE_ERROR)
      return FALSE;

   while ((cb = PenDataToBuffer(hpndt, lpbBuf, cbBufMax, &dwState)) > 0L)
      if (_lwrite(hfile, lpbBuf, (UINT)cb) == HFILE_ERROR)
         return FALSE;

   return cb >= 0;
   }


/******************* WINMAIN ************************************************/

/*+-------------------------------------------------------------------------*/
int PASCAL                 // ret errorlevel
WinMain(                   // Windows entry point; proto in <windows.h>
   HINSTANCE hInstance,    // current instance
   HINSTANCE hPrevInstance,// prev instance if any, or NULL
   LPSTR lpszCommandLine,  // ptr to commandline args
   int cmdShow)            // WM_SHOW param
   {
   MSG msg;

   /* Even though this application links to penwin.lib, we still
      have to check that Pen Windows was properly initialized (i.e.
      loaded as an installable driver). GetSystemMetrics() returns
      a valid module handle in this case:
   */
   if (!(vhPenWin = (HINSTANCE)GetSystemMetrics(SM_PENWINDOWS)))
      {
      MessageBox(NULL,
         (LPSTR)"This application requires Pen Windows to be loaded as an installable driver.",
         (LPSTR)"Pen Data Drawing Sample", MB_TASKMODAL|MB_ICONSTOP|MB_OK);
      return wExitError;
      }

   if (!FInitAN(hInstance, hPrevInstance, lpszCommandLine))
      exit(wExitError);

   if (FInitInstance(hInstance, hPrevInstance, cmdShow)) // show main window
      {
      while (GetMessage((LPMSG)&msg, NULL, 0, 0) )
         {
         if (!vhdlg || !IsDialogMessage(vhdlg, &msg))
            {
            TranslateMessage((LPMSG)&msg);
            DispatchMessage((LPMSG)&msg);
            }
         }
      }
   else
      msg.wParam = wExitError;   // error

   TermAN();   // insurance
   return msg.wParam;
   }
