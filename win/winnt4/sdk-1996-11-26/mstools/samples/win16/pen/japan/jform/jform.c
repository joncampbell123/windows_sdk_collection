/* PROGRAM: JFORM.C version 2.0

   PURPOSE:

      A program demonstrating the handwriting (HEdit and BEdit)
      and ink edit (IEdit) controls.

   COMMENTS:

      JFORM has several edit fields, typical of a generic
      form application (name, address, city, etc.).  The
      application registers itself as a pen aware application.

      This is for compatability with RC compilers that mark files 
      as 3.X. In 4.0 versions (i.e. Win95) this call is not 
      necessary, since the system automatically converts EDIT
      controls to HEDIT (but it does no harm).

      The lowermost field illustrates the ink edit control.
      In an earlier version of this sample, an HEDIT variant
      occupied that space.

      THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
      KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
      IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
      PURPOSE.

      Copyright (c) 1992-1995  Microsoft Corporation.  All Rights Reserved.
*/

/******************* Includes ***********************************************/
#include <windows.h>
#include <penwin.h>
#include "jform.h"   // incl string defines

/******************* Defines ************************************************/
// field aliases:
#define NAME            0
#define ADR1            1
#define ADR2            2
#define ZIP             3
#define NOTES           4
#define TEL             5
#define MAP             6
#define CFIELDS         7

// limits:
#define CBBUFMAX        1024  // for file transfer
#define CBSZNAMEMAX     32
#define CBSZDLGMAX      80
#define CBSZTMAX        1024

// control type flags:
#define FHEDIT          0x0001   // underline
#define FBEDIT          0x0002   // fixed-size BEDIT
#define FIEDIT          0x0004   // ink edit

// control attribute flags:
#define FSIZABLE        0x0010   // variable-size BEDIT
#define FPB             0x0020   // pushbutton
#define FPBOD           0x0040   // ownerdraw pushbutton

// sizes:
#define CXAPP           180   // In dialog units
#define CYAPP           206   // In dialog units

#define CXEESP          0     // external EDIT spacing 2 pix L, 2 pix R
#define CYEESP          5     // 3 pix T, 2 pix B
#define CXIBESP         2     // internal BEDIT spacing
#define CYIBESP         2

#define CXPB            11    // pushbutton size, dialog units
#define CYPB            9
#define CYEDIT          10    // height of normal edit controls, dialog units

// misc:
#define WM_SHOWDIW      WM_USER  // message to invoke Data Input Window
#define NEXTFIELD       1
#define PREVFIELD       2

#define CTLFIRST        1
#define PBFIRST         100

#define UCHZIP          0x81a7   // Postal Code sign (§)

/******************* Macros *************************************************/
#define mUnused(x)    x     // prevent warning message

// typedef stubs
#define mTypedefInst(RET, Name, args) \
   typedef RET (WINAPI *f##Name)##args

// invoker:
// (*(TYPE)(vrgFn[uId]))
#define mFnPW(Name)   (*(f##Name)(vrgFn[u##Name]))

// message cracker
#ifdef WIN32
#define mNotifyCOMMAND(wParam, lParam) HIWORD(wParam)
#define mHwndCOMMAND(wParam, lParam)   (HWND)(lParam)
#else
#define mNotifyCOMMAND(wParam, lParam) HIWORD(lParam)
#define mHwndCOMMAND(wParam, lParam)   (HWND)LOWORD(lParam)
#endif //WIN32

#define mPatUL(hdc, x, y, cx, cy, rop) do {\
   PatBlt(hdc, (x), (y), 1, (cy)-1, rop);   /* left */\
   PatBlt(hdc, (x), (y), (cx)-1, 1, rop);   /* top */\
   } while (0)

#define mPatLR(hdc, x, y, cx, cy, rop) do {\
   PatBlt(hdc, (x)+(cx)-1, (y), 1, (cy), rop);   /* right */\
   PatBlt(hdc, (x), (y)+(cy)-1, (cx), 1, rop);   /* bottom */\
   } while (0)

#define mPatRect(hdc, x, y, cx, cy, rop) do {\
   mPatUL(hdc, (x), (y), (cx), (cy), rop);\
   mPatLR(hdc, (x), (y), (cx), (cy), rop);\
   } while (0)

/******************* Typedefs ***********************************************/
typedef struct
   {
   // initialized:
   char  szName[CBSZNAMEMAX];    // static text to left of field
   UINT  flags;                  // FHEDIT, FBEDIT, FUSER, FIEDIT, FPB[OD]
   int   xName;                  // location of static text
   int   yName;
   int   xCtl;                   // location of control
   int   yCtl;
   int   cxCtl;                  // size of control
   int   cyCtl;
   DWORD dwStyle;                // style
   ALC   alc;                    // Alphabet Code
   ALC   alcPriority;            // Alphabet Priority
   BOOL  fEnableGestures;        // TRUE to enable gestures
   UINT  uLimit;                 // limit text (0 for none)
   char  szCaption[CBSZNAMEMAX]; // DIW caption or nothing
   UINT  uchPB;                  // char to display on pushbutton

   // calculated:
   HWND  hwndPB;                 // control's DIW pushbutton if any
   HWND  hwnd;                   // control's window handle
   RECT  r;                      // original window rect of control in pixels
   }
   FIELD, *PFIELD, FAR *LPFIELD;

mTypedefInst(int,  DestroyHRC,(HRC));
mTypedefInst(int,  DoDefaultPenInput,(HWND, UINT));
mTypedefInst(int,  EnableGestureSetHRC,(HRC, SYV, BOOL));
mTypedefInst(BOOL, IsPenEvent,(UINT, LONG));
mTypedefInst(int,  SetAlphabetHRC,(HRC, ALC, LPBYTE));
mTypedefInst(int,  SetAlphabetPriorityHRC,(HRC, ALC, LPBYTE));
mTypedefInst(int,  SetWordlistCoercionHRC,(HRC, UINT));
mTypedefInst(LONG, GetPenMiscInfo,(WPARAM, LPARAM));
mTypedefInst(int,  GetGuideHRC,(HRC, LPGUIDE, UINT FAR*));
mTypedefInst(int,  SetGuideHRC,(HRC, LPGUIDE, UINT));
mTypedefInst(int,  SetPenAppFlags,(UINT, UINT));
mTypedefInst(int,  CorrectWritingEx,(HWND, LPSTR, UINT, LPCWX));
mTypedefInst(HANDLE,  GetPenResource,(WPARAM));

/******************* Public Variables ***************************************/

char *vszJformClass        = SZAPP"SampleClass";
char *vszIeditROClass      = "ieditROClass";

#ifdef WIN32
char *vszJformWnd          = SZWND32;
char *vszAppName           = SZAPP"32";
char *vszPenWinDll         = "penwin32.dll";
#else
char *vszJformWnd          = SZWND;
char *vszAppName           = SZAPP;
#endif //WIN32

char *vszFile              = SZAPP".DAT";

HANDLE vhAccel;            // Menu Accelerator Table

HWND vhwndJF;              // Parent window to all fields
HWND vhwndFocus = NULL;    // window with focus

HINSTANCE vhinstJF = NULL; // Jform App
HINSTANCE vhinstPW = NULL; // Pen Windows

HBITMAP vhbitmapLens = NULL;  // CorrectWritingEx invoker button bitmap

int vxDU, vyDU;            // dialog units converters

#define XM       4         // static text left margin
#define XC       XM+26     // control left (most controls)
#define YM       4         // static text top margin

FIELD vrgfield[CFIELDS] =
   {
   {SZFNAME,
      FHEDIT, XM, YM, XC, YM, 142, CYEDIT,
      ES_AUTOHSCROLL,
      ALC_DEFAULT,
      ALC_GLOBALPRIORITY,
      1, 0, "", 0},

   {SZFADR1,
      FHEDIT, XM, YM+16, XC, YM+16, 142, CYEDIT,
      ES_AUTOHSCROLL,
      ALC_DEFAULT,
      ALC_GLOBALPRIORITY,
      1, 0, "", 0},

   {SZFADR2,
      FHEDIT, XM, YM+32, XC, YM+32,  84, CYEDIT,
      ES_AUTOHSCROLL,
      ALC_DEFAULT,
      ALC_GLOBALPRIORITY,
      1, 0, "", 0},

   {SZFZIP,
      FBEDIT|FPB, XM+116, YM+32, XM+127, YM+28, BXD_CELLWIDTH*3+5, BXD_CELLHEIGHT-1,
      WS_BORDER,
      ALC_NUMERIC,
      ALC_NOPRIORITY,   // discourage DBCS input
      1, 3, SZCAPZIP, UCHZIP},   // note limit=3 for SBCS; make 6 for DBCS

   {SZFNOTES,
      FBEDIT|FSIZABLE, XM,  YM+48, XC, YM+48, 142, 51,
      ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | WS_BORDER,
      ALC_DEFAULT,
      ALC_GLOBALPRIORITY,
      1, 0, "", 0},

   {SZFTEL,
      FBEDIT|FPBOD, XM, YM+104, XC+14, YM+100, BXD_CELLWIDTH*10+5, BXD_CELLHEIGHT-1,
      WS_BORDER,
      ALC_NUMERIC,
      ALC_NOPRIORITY,   // discourage DBCS input
      1, 10, SZCAPTEL, 0},   // note limit=10 for SBCS; make 20 for DBCS

   {SZFMAP,
      FIEDIT, XM, YM+122, XC, YM+120, 142, 60,
      WS_BORDER,
      ALC_DEFAULT,
      ALC_NOPRIORITY,
      1, 0, "", 0},
   };

NPSTR vrgszFn[] =
   {
   #define  uDestroyHRC                0
            "DestroyHRC",

   #define  uDoDefaultPenInput         1
            "DoDefaultPenInput",

   #define  uEnableGestureSetHRC       2
            "EnableGestureSetHRC",

   #define  uIsPenEvent                3
            "IsPenEvent",

   #define  uSetAlphabetHRC            4
            "SetAlphabetHRC",

   #define  uSetAlphabetPriorityHRC    5
            "SetAlphabetPriorityHRC",

   #define  uSetWordlistCoercionHRC    6
            "SetWordlistCoercionHRC",

   #define  uGetPenMiscInfo            7
            "GetPenMiscInfo",

   #define  uGetGuideHRC               8
            "GetGuideHRC",

   #define  uSetGuideHRC               9
            "SetGuideHRC",
            
   #define  uSetPenAppFlags           10
            "SetPenAppFlags",
            
   #define  uCorrectWritingEx         11
            "CorrectWritingEx",
            
   #define  uGetPenResource           12
            "GetPenResource",
            
   };
#define cFnNames (sizeof(vrgszFn) / sizeof(NPSTR))

// function table ptr:
FARPROC FAR* vrgFn = NULL;

/******************* Local prototypes ***************************************/
int PASCAL  WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCommandLine, int cmdShow);

LONG     WINAPI   JformWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) ;
LONG     WINAPI   IeditROWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL     WINAPI   SampleDlgProc(HWND hdlg, UINT message, UINT wParam, LONG lParam);

VOID     NEAR  PASCAL   DrawBtnBase(HDC, BOOL, int, int, int, int);
VOID     NEAR  PASCAL   DrawLensBtn(HWND, HDC, BOOL, HBITMAP);
VOID     NEAR  PASCAL   ErrBox(HINSTANCE hinst, LPCSTR lpsz, BOOL fError);
BOOL     NEAR  PASCAL   FCreateForm(HWND hwndParent);
BOOL     NEAR  PASCAL   FGetHpndt(HWND hwnd, HPENDATA NEAR *phpndt);
BOOL     NEAR  PASCAL   FInitApp(HANDLE hInstance, BOOL fFirstInstance);
BOOL     NEAR  PASCAL   FInitInstance(HANDLE hInstance, HANDLE hPrevInstance, int cmdShow);
BOOL     NEAR  PASCAL   FInitPWStubs(HINSTANCE hinstPW, BOOL fInit);
BOOL     NEAR  PASCAL   FLoadSave(BOOL fLoad);
BOOL     NEAR  PASCAL   FLoadSaveText(HFILE hfile, BOOL fLoad, HWND hwnd);
BOOL     NEAR  PASCAL   FPenOrStubPresent(VOID);
BOOL     NEAR  PASCAL   FSetHpndt(HWND hwnd, HPENDATA hpndt);
VOID     NEAR  PASCAL   FTermApp(VOID);
int      NEAR  PASCAL   IFromHwnd(HWND hwnd);
HPENDATA NEAR  PASCAL   ReadPenData(HFILE hfile);
BOOL     NEAR  PASCAL   ProcessFieldChange(UINT wParam);
VOID     NEAR  PASCAL   SampleDialog(HWND hinstance);
BOOL     NEAR  PASCAL   WritePenData(HFILE hfile, HPENDATA hpndt);
VOID     NEAR  PASCAL   ResizeBEDIT(int i);

/******************* End of Header Section **********************************/

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                  LPSTR lpszCommandLine, int cmdShow)
/***************************************************************

FUNCTION:   WinMain(hInstance, hPrevInstance, lpszCommandLine, cmdShow)

PURPOSE:

   Main Windows application function.

***************************************************************/
   {
   MSG   msg;

   mUnused(lpszCommandLine);

   if (!FInitApp(hInstance, hPrevInstance == NULL))
      return 1;

   if (FInitInstance(hInstance, hPrevInstance, cmdShow))
      {
      while (GetMessage((LPMSG)&msg,NULL,0,0) )
         {
         // Check for menu accelerator message
         if (!TranslateAccelerator(vhwndJF, vhAccel, &msg))
            {
            TranslateMessage((LPMSG)&msg);
            DispatchMessage((LPMSG)&msg);
            }
         }
      }
   else
      msg.wParam = 0;

   FTermApp();
   return msg.wParam;
   }

VOID NEAR PASCAL DrawBtnBase(HDC hdc, BOOL fDown, int x, int y, int cx, int cy)
/***************************************************************

FUNCTION:   DrawBtnBase(hdc, fDown, x, y, cx, cy)

PURPOSE:

   Draws an ownerdraw button base, a face without makeup.

***************************************************************/
   {
   WORD i;

#define mMakeBrush(br, cr)\
   HBRUSH br = CreateSolidBrush(GetSysColor(cr))

   mMakeBrush(hbrBtnDkshadow, COLOR_BTNSHADOW);
   mMakeBrush(hbrBtnShadow, COLOR_BTNSHADOW);
   mMakeBrush(hbrBtnHighlight, COLOR_BTNHIGHLIGHT);
   mMakeBrush(hbrBtnFacedown, COLOR_BTNFACE);
   mMakeBrush(hbrBtnFace, COLOR_BTNFACE);

   HBRUSH hbrushOld = SelectObject(hdc, GetStockObject(BLACK_BRUSH));

   if (!fDown)
      {
      mPatLR(hdc, x, y, cx, cy, PATCOPY);
      cx--;
      cy--;
      }

   // clear face:
   SelectObject(hdc, fDown?hbrBtnFacedown: hbrBtnFace);
   PatBlt(hdc, x, y, cx, cy, PATCOPY);

   SelectObject(hdc, hbrBtnDkshadow);
   if (fDown)
      {
      mPatUL(hdc, x, y, cx, cy, PATCOPY);
      x++;
      y++;
      }
   else
      mPatLR(hdc, x, y, cx, cy, PATCOPY);

   cx--;
   cy--;

   SelectObject(hdc, hbrBtnShadow);
   if (fDown)
      mPatUL(hdc, x+i, y+i, cx-i-i, cy-i-i, PATCOPY);
   else 
      mPatLR(hdc, x+1+i, y+1+i, cx-1-i-i, cy-1-i-i, PATCOPY);

   SelectObject(hdc, hbrBtnHighlight);
   if (fDown)
      mPatLR(hdc, x, y, cx, cy, PATCOPY);
   else 
      mPatUL(hdc, x, y, cx+1, cy+1, PATCOPY);
   x++;
   y++;
   cx -= 2;
   cy -= 2;

   SelectObject(hdc, hbrushOld);

   DeleteObject(hbrBtnDkshadow);
   DeleteObject(hbrBtnShadow);
   DeleteObject(hbrBtnHighlight);
   DeleteObject(hbrBtnFacedown);
   DeleteObject(hbrBtnFace);
   }

VOID NEAR PASCAL DrawLensBtn(HWND hwnd, HDC hdc, BOOL fDown, HBITMAP hbitmap)
/***************************************************************

FUNCTION:   DrawLensBtn(hwnd, hdc, fDown, hbitmap)

PURPOSE:

   Draws an ownerdraw button with the system "lens" bitmap, which
   invokes the CorrectWritingEx dialog.

***************************************************************/
   {
   HDC hmemDC;
   RECT r;
   BITMAP bm;
   POINT ptSize, ptOrg ;
   int xStart, yStart;
   int cx, cy;
   COLORREF crBkOld = SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
   COLORREF crTextOld = SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
   
   hmemDC = CreateCompatibleDC(hdc);
   SelectObject(hmemDC, hbitmap);
   SetMapMode(hmemDC, GetMapMode(hdc));

   GetObject(hbitmap, sizeof(BITMAP), (LPSTR)&bm);
   ptSize.x = bm.bmWidth;
   ptSize.y = bm.bmHeight;
   DPtoLP(hdc, &ptSize, 1);

   ptOrg.x = 0;
   ptOrg.y = 0;
   DPtoLP(hmemDC, &ptOrg, 1);

   GetClientRect(hwnd, (LPRECT)&r);
   cx = r.right - r.left;
   cy = r.bottom - r.top;
   DrawBtnBase(hdc, fDown, r.left, r.top, cx, cy);

   xStart = r.left + fDown + ((cx - bm.bmWidth) >> 1);
   yStart = r.top + fDown + ((cy - bm.bmHeight) >> 1);
   BitBlt(hdc, xStart, yStart, ptSize.x, ptSize.y,
      hmemDC, ptOrg.x, ptOrg.y, SRCCOPY);

   DeleteDC(hmemDC);

   SetBkColor(hdc, crBkOld);
   SetTextColor(hdc, crTextOld);
   }

VOID NEAR PASCAL ErrBox(HINSTANCE hinst, LPCSTR lpsz, BOOL fError)
/***************************************************************

FUNCTION:   ErrBox(hinst, lpsz, fError)

PURPOSE:

   Put user error message based on resource id. If fError
   is TRUE, "Error" will appear as the caption.

***************************************************************/
   {
   MessageBox(NULL, lpsz, fError ? NULL : vszAppName, MB_OK);
   }

BOOL NEAR PASCAL FInitApp(HINSTANCE hInstance, BOOL fFirstInstance)
/***************************************************************

FUNCTION:   FInitApp(hInstance, fFirstInstance)

PURPOSE:

   Initialize application data and register window classes.

   Returns FALSE if function could not register window classes.
   TRUE if successful.

***************************************************************/
   {
   WNDCLASS wc;
   HCURSOR  hcursorArrow;

   hcursorArrow = LoadCursor(NULL, IDC_ARROW);

   /* Init Pen Windows function pointers if penwin library is present.

      In 32 bits, you need to load the library before trying to
      call functions from it. In 16 bits, it is OK to simply
      get the module handle and call functions from it:
   */

#ifdef WIN32
   vhinstPW = LoadLibrary(vszPenWinDll);
#else
   vhinstPW = (HINSTANCE)GetSystemMetrics(SM_PENWINDOWS);
#endif //WIN32

   if (vhinstPW)
      {
      if (!FInitPWStubs(vhinstPW, TRUE))
         return FALSE;

      // Register as pen app with DBCS by default:
      mFnPW(SetPenAppFlags)(RPA_DBCSPRIORITY, PENVER);

      // get lens bitmap:
      vhbitmapLens = mFnPW(GetPenResource)((WPARAM)GPR_BMLENSBTN);
      }

   else if (fFirstInstance)
      // no Pen Windows: use read-only (RO) mode
      {
      ErrBox(hInstance, SZSNOPW, FALSE);

      // register readonly IEDIT class if no penwindows

      wc.hCursor = hcursorArrow;
      wc.hIcon = NULL;
      wc.lpszMenuName = NULL;
      wc.lpszClassName = (LPSTR)vszIeditROClass;
      wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
      wc.hInstance = hInstance;
      wc.style = 0;
      wc.lpfnWndProc = IeditROWndProc;
      wc.cbClsExtra = 0;
      wc.cbWndExtra = sizeof(LONG);

      if (!RegisterClass((LPWNDCLASS) &wc))
         return FALSE;
      }

   // register main window class
   if (fFirstInstance)
      {
      wc.hCursor = vhinstPW ? LoadCursor(vhinstPW, IDC_PEN) : hcursorArrow;
      wc.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(ICONJF));
      wc.lpszMenuName = MAKEINTRESOURCE(IDM_MAIN);
      wc.lpszClassName = (LPSTR)vszJformClass;
      wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
      wc.hInstance = hInstance;
      wc.style = 0;
      wc.lpfnWndProc = JformWndProc;
      wc.cbClsExtra = 0;
      wc.cbWndExtra = 0;

      if (!RegisterClass((LPWNDCLASS) &wc))
         return FALSE;
      }

   return TRUE;
   }

BOOL NEAR PASCAL FInitInstance(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                              int cmdShow)
/***************************************************************

FUNCTION:   FInitInstance(hInstance, hPrevInstance, cmdShow)

PURPOSE:

   Initialize all data structures for program instance and create
   the necessary windows.

   Returns TRUE if successsful, FALSE if failed.

***************************************************************/
   {
   int   i;
   LONG  lT     = GetDialogBaseUnits();

   mUnused(hPrevInstance);

   vxDU   = LOWORD(lT);
   vyDU   = HIWORD(lT);
   vhinstJF = hInstance;

   // Convert field coordinates to window coordinates
   for (i = 0; i < CFIELDS; i++)
      {
      vrgfield[i].xName = (vrgfield[i].xName * vxDU)/4;
      vrgfield[i].yName = (vrgfield[i].yName * vyDU)/8;
      vrgfield[i].xCtl = (vrgfield[i].xCtl * vxDU)/4;
      vrgfield[i].yCtl = (vrgfield[i].yCtl * vyDU)/8;
      vrgfield[i].cxCtl = (vrgfield[i].cxCtl * vxDU)/4;
      vrgfield[i].cyCtl = (vrgfield[i].cyCtl * vyDU)/8;
      }

   // Create Main window
   if (vhwndJF = CreateWindow((LPSTR)vszJformClass,
      (LPSTR)vszJformWnd,
      WS_CLIPCHILDREN | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
      4*vxDU/4, 4*vyDU/8,
      (CXAPP*vxDU)/4, (CYAPP*vyDU)/8,
      NULL,
      NULL,
      hInstance,
      NULL
      ))
      {
      vhAccel = LoadAccelerators(hInstance, IDJFORM);
      ShowWindow(vhwndJF, cmdShow);
      UpdateWindow(vhwndJF);
      }

   SetFocus(vhwndFocus = vrgfield[NAME].hwnd);
   return vhwndJF != NULL;
   }

VOID NEAR PASCAL FTermApp(VOID)
/***************************************************************

FUNCTION:   FTermApp()

PURPOSE:

   Terminate application and free memory.

***************************************************************/
   {
   // Unregister with Pen Windows:
   if (vhinstPW)
      {
      mFnPW(SetPenAppFlags)(0, 0);

      if (vhbitmapLens)
         {
         DeleteObject(vhbitmapLens);
         vhbitmapLens = NULL;
         }

      // drop function pointers:
      if (vrgFn)
         FInitPWStubs(vhinstPW, FALSE);

#ifdef WIN32
      FreeLibrary(vhinstPW);
#endif // WIN32
      vhinstPW = NULL;
      }
   }

LONG WINAPI IeditROWndProc(HWND hwnd, UINT message,
            WPARAM wParam, LPARAM lParam)
/***************************************************************

FUNCTION:   IeditROWndProc(hwnd, message, wParam, lParam)

PURPOSE:

   Windows procedure for readonly IEDIT window.

***************************************************************/
   {
   LONG     lRet  = 0L;

   switch (message)
      {
      case WM_PAINT:
         {
         PAINTSTRUCT ps;
         HDC         hdc = BeginPaint(hwnd, &ps);

         if (hdc)
            {
            HPENDATA hpndt = (HPENDATA)GetWindowLong(hwnd, 0);

            if (hpndt)
               DrawPenDataFmt(hdc, NULL, hpndt);
            EndPaint(hwnd, &ps);
            }
         break;
         }
      break;

   default:
      lRet = DefWindowProc(hwnd, message, wParam, lParam);
      break;
      }

   return lRet;
   }

LONG WINAPI JformWndProc(HWND hwnd, UINT message,
            WPARAM wParam, LPARAM lParam)
/***************************************************************

FUNCTION:   JformWndProc(hwnd, message, wParam, lParam)

PURPOSE:

   Windows procedure for application's parent window.

***************************************************************/
   {
   int      i;
   LONG     lRet  = 0L;

   static HBRUSH hbrBackgnd = NULL;

   switch (message)
      {
      case WM_CREATE:
         // Create fields

         if (!FCreateForm(hwnd))
            {
            lRet = 1L;  // Failed
            break;
            }

         hbrBackgnd = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
         break;

      case WM_LBUTTONDOWN:
         if (vhinstPW)
            {
            LONG lExtraInfo = GetMessageExtraInfo();

            if (mFnPW(IsPenEvent)(message, lExtraInfo)
               && (mFnPW(DoDefaultPenInput)(hwnd, LOWORD(lExtraInfo))
                  == PCMR_OK))
                  return 1L;
            }
         break;

      case WM_DRAWITEM:
         if (vhbitmapLens)
            {
            LPDRAWITEMSTRUCT lpdi = (LPDRAWITEMSTRUCT)lParam;

            if (lpdi->CtlType == ODT_BUTTON
               && lpdi->CtlID == PBFIRST+TEL
               && (lpdi->itemAction == ODA_SELECT
               || lpdi->itemAction == ODA_DRAWENTIRE))
               {
               DrawLensBtn(lpdi->hwndItem, lpdi->hDC,
                  (lpdi->itemState & ODS_SELECTED) != 0, vhbitmapLens);
               }
            }
         break;

      case WM_COMMAND:
         {
         // Edit control commands
         UINT wId = LOWORD(wParam);
         UINT wNotify = mNotifyCOMMAND(wParam, lParam);
         HWND hwndCtl = mHwndCOMMAND(wParam, lParam);

         if (wNotify == EN_SETFOCUS || wNotify == IN_SETFOCUS)
            {
            // Field focus is being set
            vhwndFocus = hwndCtl;
            break;
            }

         // disallow gray preconversion mode in BEDITs:
         if (wNotify == EN_UPDATE                        // about to change
            && vhinstPW                                  // penwin active
            && (vrgfield[wId-CTLFIRST].flags & FBEDIT)      // BEDIT control..
            && !(vrgfield[wId-CTLFIRST].flags & FSIZABLE))  // ..fixed
            {
            SendMessage(hwndCtl, WM_PENCTL,
               HE_SETCONVERTRANGE, MAKELONG(-1, 0));
            }

         // buttons:
         if (vhinstPW && (wId-PBFIRST == ZIP || wId-PBFIRST == TEL))
            {
            wId -= PBFIRST;
            PostMessage(hwnd, WM_SHOWDIW, wId, (LPARAM)vrgfield[wId].hwnd);
            SetFocus(vhwndFocus = vrgfield[wId].hwnd);
            return 1L;  // abort original request
            }

         // hook DIW calls:
         if (vhinstPW
            && wNotify == HN_BEGINDIALOG
            && (wId-CTLFIRST == ZIP || wId-CTLFIRST == TEL))
            {
            wId -= CTLFIRST;
            PostMessage(hwnd, WM_SHOWDIW, wId, (LPARAM)hwndCtl);
            SetFocus(vhwndFocus = hwndCtl);
            return 1L;  // abort original request
            }

         // break if non-menu
         if (wNotify && hwndCtl)
            break;

         // Menu commands

         switch (wId)
            {
            case IDM_EXIT:
               DestroyWindow(hwnd);
               break;

            case IDM_CLEAR:
               for (i = 0; i < CFIELDS; i++)
                  {
                  if (vrgfield[i].hwnd != NULL)
                     {
                     // Clear field
                     if (vrgfield[i].flags & FIEDIT)
                        FSetHpndt(vrgfield[i].hwnd, NULL);
                     else
                        SendMessage(vrgfield[i].hwnd, WM_SETTEXT,
                           0, (LONG)(LPSTR)"");
                     }
                  }
               SetFocus(vrgfield[NAME].hwnd);
               break;

            case IDM_SAMPLEDLG:
               SampleDialog(hwnd);
               break;

            case IDM_LOAD:
            case IDM_SAVE:
               FLoadSave(wId == IDM_LOAD);
               break;

            case IDM_NEXT:
               // Focus on the next field
               ProcessFieldChange((UINT)NEXTFIELD);
               break;

            case IDM_PREV:
               // Set Focus on the preceeding field
               ProcessFieldChange((UINT)PREVFIELD);
               break;

            default:
               break;
            }
         break;
         }

      case WM_SHOWDIW:  // private message opens specific Data Input Window
         // wParam: zero-base field id
         // lParam: window handle of input control
         if (vhinstPW)
            {
            CWX cwx =
               {sizeof(CWX),        // Apply flags...
               CWXA_KBD             // ...specify kbd
               | CWXA_STATE         // ...specify state
               | CWXA_NOUPDATEMRU,  // ...do not update registry when done
               NULL, NULL, {0}, 0L, 0L};
            HWND hwndCtl = (HWND)lParam;
            int i;

            cwx.dwSel = SendMessage(hwndCtl, EM_GETSEL, 0, 0);
            cwx.hwndText = hwndCtl;
            cwx.dwEditStyle = GetWindowLong(hwndCtl, GWL_STYLE);
            lstrcpy((LPSTR)cwx.szCaption, (LPSTR)vrgfield[wParam].szCaption);

            // setup required state, default states for other kbds;
            // in this sample app, we only show Num Hankaku....

            cwx.ixkb = CWXK_NUM;
            for (i = 0; i < CKBCWX; i++)
               cwx.rgState[i] = CWXKS_DEFAULT;               // existing
            cwx.rgState[CWXK_NUM - CWXK_FIRST] = CWXKS_HAN;  // hankaku numbers

            // call the API
            if (mFnPW(CorrectWritingEx)(hwnd,
               NULL,                      // get text with WM_GETTEXT
               vrgfield[wParam].uLimit+1, // LIMITTEXT size plus one
               (LPCWX)&cwx) == CWXR_MODIFIED)
               {
#ifdef WIN32
               SendMessage(hwndCtl, EM_SETSEL, LOWORD(cwx.dwSel), HIWORD(cwx.dwSel));
#else
               SendMessage(hwndCtl, EM_SETSEL, 1, cwx.dwSel);
#endif //WIN32
               SendMessage(hwndCtl, EM_SETMODIFY, 1, 0);
               }
            }
         break;

#ifdef WIN32
      case WM_CTLCOLOREDIT:
         {
         #define cbClass   8
         char szClass[cbClass];

         // use grey background for single line edit controls if penwin
         if (vhinstPW && hbrBackgnd &&
             !(GetWindowLong((HWND)LOWORD(lParam), GWL_STYLE) & ES_MULTILINE) &&
             GetClassName((HWND)LOWORD(lParam), szClass, cbClass) &&
             (lstrcmpi(szClass, "BEDIT") != 0))
            {
            SetBkColor((HDC)wParam, GetSysColor(COLOR_BTNFACE));
            return (LRESULT)hbrBackgnd;
            }

         lRet = DefWindowProc(hwnd, message, wParam, lParam);
         break;
         }
#else
      case WM_CTLCOLOR:
         // use grey background for single line edit controls if penwin
         if (vhinstPW && hbrBackgnd)
            {
            #define cbClass   8
            char szClass[cbClass];

            switch (HIWORD(lParam))
               {
            case CTLCOLOR_EDIT:
               // if multiline or bedit, break
               if (GetWindowLong((HWND)LOWORD(lParam), GWL_STYLE) & ES_MULTILINE)
                  break;
               if (GetClassName((HWND)LOWORD(lParam), szClass, cbClass) &&
                   (lstrcmpi(szClass, "BEDIT") == 0))
                  break;
               SetBkColor((HDC)wParam, GetSysColor(COLOR_BTNFACE));

               // fall through
            case CTLCOLOR_MSGBOX:
               return (LRESULT)hbrBackgnd;
               }
            }

         lRet = DefWindowProc(hwnd, message, wParam, lParam);
         break;
#endif // !WIN32

      case WM_PAINT:
         {
         PAINTSTRUCT ps;
         HDC         hdc;

         hdc = BeginPaint(hwnd, &ps);
         SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
         SetBkMode(hdc, TRANSPARENT);

         for (i = 0; i < CFIELDS; i++)
            {
            PFIELD pfield = vrgfield + i;
            int cbTitle = lstrlen(pfield->szName);

            if (cbTitle)
               {
               char sz[CBSZNAMEMAX];

               lstrcpy((LPSTR)sz, (LPSTR)pfield->szName);
               TextOut(hdc, pfield->xName, pfield->yName,
                  (LPSTR)sz, cbTitle);
               }
            }

         EndPaint(hwnd, &ps);
         break;
         }
      break;

      case WM_CTLINIT:
         if (wParam == CTLINIT_BEDIT)
            {
            LPCTLINITBEDIT lpcib = (LPCTLINITBEDIT)lParam;

            if (vrgfield[lpcib->id-1].flags & FSIZABLE)
               lpcib->wSizeCategory = BESC_USERDEFINED;
            }
         break;

      case WM_SETFOCUS:
         SetFocus(vhwndFocus);
         break;

      case WM_DESTROY:
         if (hbrBackgnd)
            DeleteObject(hbrBackgnd);
         PostQuitMessage(0);
         break;

      case WM_PENMISC:
         switch (LOWORD(wParam))
            {
         case PMSC_LOADPW:
            if (lParam == PMSCL_UNLOADING)
               {
               FTermApp();
               PostQuitMessage(0);
               }
            break;

         case PMSC_BEDITCHANGE:
            {
            int i;

            for (i = 0; i < CFIELDS; i++)
               if (vrgfield[i].flags & FSIZABLE)
                  {
                  SendMessage(vrgfield[i].hwnd, message, wParam, lParam);
                  ResizeBEDIT(i);
                  }
            }
            break;

         default:
            break;
            }
         break;

      default:
         lRet = DefWindowProc(hwnd, message, wParam, lParam);
         break;
      }

   return lRet;
   }

BOOL NEAR PASCAL FCreateForm(HWND hwndParent)
/***************************************************************

FUNCTION:   FCreateForm(hwndParent)

PURPOSE:

   This function creates the fields.

   Returns TRUE is successful, FALSE if field hedit window could
   not be created.

***************************************************************/
   {
   int      i;
   HRC      hrc;

   for (i = 0; i < CFIELDS; i++)
      {
      PFIELD pfield = vrgfield + i;
      DWORD dwStyle = WS_VISIBLE | WS_CHILD | pfield->dwStyle;
      LPCSTR lpszClass;
      BOOL fPB = vhinstPW != NULL && ((pfield->flags & (FPB|FPBOD)) != 0);

      if (vhinstPW)
         {
         lpszClass = (pfield->flags & FHEDIT)? (LPCSTR)"hedit":
            (pfield->flags & FBEDIT)? (LPCSTR)"bedit":
            (pfield->flags & FIEDIT)? (LPCSTR)"iedit": NULL;
         }
      else 
         {
         lpszClass = (pfield->flags & FHEDIT)
            || (pfield->flags & FBEDIT)? (LPCSTR)"edit":
            (pfield->flags & FIEDIT)? (LPCSTR)"ieditROClass": NULL;

         dwStyle |= WS_BORDER;   // no underline available; subst border

         // SL BEDITs for Zip & Tel:
         if ((pfield->flags & FBEDIT)
            && !(pfield->dwStyle & ES_MULTILINE))
            {
            pfield->yCtl += (pfield->cyCtl / 2) - (CYEDIT/2 * vyDU/8);
            pfield->cyCtl = CYEDIT * vyDU/8;
            dwStyle |= ES_AUTOHSCROLL;   // allow std lens
            }
         }

      if (!lpszClass)
         return FALSE;  // should never happen

      SetRect(&pfield->r,
         pfield->xCtl,
         pfield->yCtl,
         pfield->xCtl + pfield->cxCtl,
         pfield->yCtl + pfield->cyCtl);

      pfield->hwnd = CreateWindow(
         lpszClass,
         (LPCSTR)NULL,
         dwStyle,
         pfield->r.left,
         pfield->r.top,
         pfield->r.right - pfield->r.left,
         pfield->r.bottom - pfield->r.top,
         hwndParent,
         (HMENU)(i+CTLFIRST), // unique non-zero child ID
         vhinstJF,
         NULL);

      if (!pfield->hwnd)
         {
         continue;
         }

      // Recalc variable BEDIT size:
        if (pfield->flags & FSIZABLE)
           ResizeBEDIT(i);

      // Set alphabet preferences for this edit control
      if (vhinstPW)
         {
         if (hrc = (HRC)SendMessage(pfield->hwnd, WM_PENMISC, PMSC_GETHRC, 0L))
            {
            mFnPW(SetAlphabetHRC)(hrc, pfield->alc, NULL);
            mFnPW(SetAlphabetPriorityHRC)(hrc, pfield->alcPriority, NULL);
            mFnPW(SetWordlistCoercionHRC)(hrc, SCH_ADVISE);
            if (!pfield->fEnableGestures)    // default: all enabled
               mFnPW(EnableGestureSetHRC)(hrc, GST_ALL, FALSE);
            SendMessage(pfield->hwnd, WM_PENMISC, PMSC_SETHRC, (LPARAM)hrc);
            mFnPW(DestroyHRC)(hrc);
            }

         // If no border, put underline in unless just kernel
         if (!(pfield->dwStyle & ES_MULTILINE)
            && (pfield->flags & FHEDIT))
            {
            SendMessage(pfield->hwnd, WM_PENCTL, HE_SETUNDERLINE, 1L);
            }
         }

      // set limit text if any:
      if (pfield->uLimit > 0)
         SendMessage(pfield->hwnd, EM_LIMITTEXT, pfield->uLimit, 0L);

      // add Data Input Window Pushbuttons:
      if (fPB)
         {
         int x = pfield->r.left - (CXPB+2)*vxDU/4;
         int y = ((pfield->r.top + pfield->r.bottom) - CYPB*vyDU/8 + 1) / 2;
         char szPB[3] = {pfield->uchPB >> 8, pfield->uchPB & 0xff, 0};
         DWORD dwStyle = WS_VISIBLE | WS_CHILD;

         pfield->hwndPB = CreateWindow(
            (LPCSTR)"button",
            (LPCSTR)szPB,
            dwStyle | (pfield->flags & FPBOD? BS_OWNERDRAW: BS_PUSHBUTTON),
            x, y,
            CXPB*vxDU/4,
            CYPB*vyDU/8,
            hwndParent,
            (HMENU)(i+PBFIRST), // unique non-zero child ID
            vhinstJF,
            NULL);
         }

      // one last thing - hide static zip sign if button visible:
      if (i == ZIP && fPB)
         pfield->szName[0] = '\0';
      }

   return TRUE;
   }

VOID NEAR PASCAL SampleDialog(HWND hwnd)
/***************************************************************

FUNCTION:   SampleDialog()

PURPOSE:

   Brings up a sample dialog containing an EDIT (not HEDIT) control.
   If we're running under Pen Windows system, EDIT will act like an HEDIT.

***************************************************************/
   {
   DLGPROC lpSampleDlg;

   lpSampleDlg = MakeProcInstance((DLGPROC) SampleDlgProc, vhinstJF);
   DialogBox(vhinstJF, "SampleH", hwnd, lpSampleDlg);
   FreeProcInstance(lpSampleDlg);
   }

BOOL WINAPI SampleDlgProc(HWND hdlg, UINT message, UINT wParam, LONG lParam)
/***************************************************************

FUNCTION:   SampleDlgProc(hdlg, message, wParam, lParam)

PURPOSE:

   Dialog window procedure. Does nothing much, just shows a dialog.

***************************************************************/
   {
   mUnused(lParam);

   switch (message)
      {
      case WM_INITDIALOG:
         SetDlgItemText(hdlg, IDC_EDIT, "Sample Name");

         // Set focus to edit field and select all text
         SetFocus(GetDlgItem(hdlg, IDC_EDIT));
         SendDlgItemMessage(hdlg, IDC_EDIT, EM_SETSEL, 0, MAKELONG(0,-1));
         break;

      case WM_COMMAND:
         switch (LOWORD(wParam))
            {
            case IDOK:
            case IDCANCEL:
               EndDialog(hdlg, LOWORD(wParam) == IDOK);
               return TRUE;

            default:
               break;
            }
         break;

      default:
         break;
      }

   return FALSE;
   }

BOOL NEAR PASCAL ProcessFieldChange (UINT wParam)
/***************************************************************

FUNCTION:   ProcessFieldChange (wParam)

PURPOSE:

   Set the focus on either the next or previous field based on
   the value of wParam. wParam is can be set to NEXTFIELD or
   PREVFIELD.  The global window handle vhwndFocus is assigned
   the value of the newly focused field.

   Returns TRUE if successful, FALSE otherwise.

***************************************************************/
   {
   if ((wParam == NEXTFIELD) || (wParam == PREVFIELD))
      {
      int inc = wParam == PREVFIELD ? CFIELDS-1 : 1;
      int i = (IFromHwnd(vhwndFocus)+inc) %(CFIELDS);

      SetFocus(vhwndFocus = vrgfield[i].hwnd);
      return TRUE;
      }

   return FALSE;
   }

int NEAR PASCAL IFromHwnd(HWND hwnd)
/***************************************************************

FUNCTION:   IFromHwnd(hwnd)

PURPOSE:

   Returns the index into the vrgfield which corresponds to
   the entry containing parameter hwnd.

   Returns 0 if a match is not found.

***************************************************************/
   {
   register int   i;
   LPFIELD        lpfield;

   for (lpfield = vrgfield, i = 0; i < CFIELDS; i++, lpfield++)
      if (lpfield->hwnd == hwnd)
         return i;

   return 0;    // default on err
   }

BOOL NEAR PASCAL FLoadSaveText(HFILE hfile, BOOL fLoad, HWND hwnd)
/***************************************************************

FUNCTION:   FLoadSaveText(hfile, fLoad, hwnd)

PURPOSE:

   Loads (fLoad TRUE) or saves (fLoad FALSE) the contents of
   a text field. The file format is 2 bytes for length followed
   by that many bytes of text (no null term)

   Returns TRUE if successful.

***************************************************************/
   {
   char  sz[CBSZTMAX];
   WORD  cb;
   BOOL  fRet;

   if (fLoad)
      {
      if (fRet = _lread(hfile, &cb, (UINT)sizeof(WORD)) != HFILE_ERROR
         && _lread(hfile, sz, cb) != HFILE_ERROR)
         {
         sz[cb] = '\0'; // terminate
         SetWindowText(hwnd, (LPSTR)sz);
         }
      }
   else  // save text to file
      {
      cb = GetWindowText(hwnd, (LPSTR)sz, sizeof(sz));
      fRet = _lwrite(hfile, (LPCSTR)&cb, sizeof(WORD)) != HFILE_ERROR
         && _lwrite(hfile, (LPCSTR)sz, (UINT)cb) != HFILE_ERROR;
      }

   return fRet;
   }

BOOL NEAR PASCAL FLoadSave(BOOL fLoad)
/***************************************************************

FUNCTION:   FLoadSave(fLoad)

PURPOSE:

   Loads (fLoad TRUE) or saves (fLoad FALSE) the contents of
   the form, in file JFORM.DAT.

   Returns TRUE if successful.

***************************************************************/
   {
   HCURSOR  hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
   HFILE    hfile;
   OFSTRUCT of;
   int      i;
   BOOL     fRet = TRUE;

   if ((hfile = OpenFile((LPSTR)vszFile, &of,
      fLoad? OF_READ: OF_CREATE)) != HFILE_ERROR)
      {
      // first do the text fields:
      for (i = 0; fRet && i < CFIELDS-1; i++)
         fRet = FLoadSaveText(hfile, fLoad, vrgfield[i].hwnd);

      // now do the IEDIT (assumed to be last):
      if (fRet)
         {
         HPENDATA hpndt;

         if (fLoad)
            {
            fRet = ((hpndt = ReadPenData(hfile)) != NULL)
               && FSetHpndt(vrgfield[MAP].hwnd, hpndt);
            }
         else  // save
            {
            if (FGetHpndt(vrgfield[MAP].hwnd, &hpndt) // retrieve
               && hpndt)   // may be empty though
               {
               fRet = WritePenData(hfile, hpndt);
               DestroyPenData(hpndt);  // we took it so we drop it too
               }
            else
               fRet = FALSE;
            }
         }
      _lclose(hfile);
      }
   else
      fRet = FALSE;

   SetCursor(hCursor);
   return fRet;
   }

HPENDATA NEAR PASCAL ReadPenData(HFILE hfile)
/***************************************************************

FUNCTION:   ReadPenData(hfile)

PURPOSE:

   Reads pen data from a file. The file format at this point is
   a UINT value representing the size of the pendata, followed
   by that many bytes of pendata.

   Returns a handle to a pendata if successful.

   Before calling this function, the caller should have already
   opened the file specified by hfile and ensured that the
   file pointer is offset to the beginning of the pen data.
   When the function returns, the file pointer will be offset
   to the end of the pen data in the file.

***************************************************************/
   {
   HPENDATA hpndt = NULL;
   WORD     cb, cbRead, cbHpndt;
   BYTE     lpbBuf[CBBUFMAX];    // buffer
   DWORD    dwState = 0L;        // required init
   BOOL     fError = FALSE;

   if (!hfile
      || (cb = _lread(hfile, &cbHpndt, (UINT)sizeof(WORD))) == HFILE_ERROR
      || cb != sizeof(WORD))
         return NULL;

   while (cbHpndt > 0)
      {
      if ((cbRead = _lread(hfile, lpbBuf, (UINT)min(cbHpndt, CBBUFMAX)))
         == HFILE_ERROR
         || PenDataFromBuffer(&hpndt, 0, lpbBuf, CBBUFMAX, &dwState) < 0)
         {
         if (hpndt)
            DestroyPenData(hpndt);
         return NULL;
         }

      if (cbRead > 0)
         cbHpndt -= cbRead;
      else
         break;
      }

   return hpndt;
   }

BOOL NEAR PASCAL WritePenData(HFILE hfile, HPENDATA hpndt)
/***************************************************************

FUNCTION:   WritePenData(hfile, hpndt)

PURPOSE:

   Writes pen data into a file, preceded by a UINT consisting of
   the size of the pen data in bytes.

   Returns TRUE if successful.

   Before calling this function, the caller should have
   already opened the file specified by hfile and ensured that
   the file pointer is correctly placed.  When the function
   returns, the file pointer will be offset to the end of the
   pen data in the file. The function fails if the pen data is
   larger than 64K.

***************************************************************/
   {
   BYTE lpbBuf[CBBUFMAX];
   DWORD dwState = 0L;  // required init
   LONG cb;
   DWORD dwSize;
   WORD cbSize;

   if (!hfile || !hpndt)
      return FALSE;

   if (GetPenDataAttributes(hpndt, (LPVOID)&dwSize, GPA_SIZE) < 0)
      return FALSE;

   cbSize = LOWORD(dwSize);

   if (_lwrite(hfile, (LPCSTR)&cbSize, (UINT)sizeof(WORD)) == HFILE_ERROR)
      return FALSE;

   while (cb = (LONG)PenDataToBuffer(hpndt, lpbBuf, CBBUFMAX, &dwState))
      if (_lwrite(hfile, lpbBuf, (UINT)cb) == HFILE_ERROR)
         return FALSE;

   return cb >= 0L;
   }

BOOL NEAR PASCAL FInitPWStubs(HINSTANCE hinstPW, BOOL fInit)
/***************************************************************

FUNCTION:   FInitPWStubs(hinstPW, fInit)

PURPOSE:

   Initializes or releases a table of functions required from penwin.dll.

   Returns TRUE if successful.

***************************************************************/
   {
   BOOL fRet = TRUE;
   static HGLOBAL hFnTable = NULL;

   if (fInit)  // alloc table on heap and fill it
      {
      if (hFnTable = GlobalAlloc(GHND | GMEM_DDESHARE,
         cFnNames * sizeof(FARPROC)))
         {
         int i;

         if (!((LPVOID)vrgFn = GlobalLock(hFnTable)))
            {
            fRet = FALSE;
            goto endFIPFT1;
            }

         for (i = 0; i < cFnNames; i++)
            {
            if (!(vrgFn[i] = GetProcAddress(vhinstPW, (LPCSTR)vrgszFn[i])))
               {
               fRet = FALSE;
               goto endFIPFT2;
               }
            }
         }
      else
         fRet = FALSE;  // mem err
      }

   else if (hFnTable)   // free table
      {
endFIPFT2:
      fRet = !GlobalUnlock(hFnTable) && fRet;
      vrgFn = NULL;

endFIPFT1:
      fRet = !GlobalFree(hFnTable) && fRet;
      hFnTable = NULL;
      }

   return fRet;
   }

BOOL NEAR PASCAL FGetHpndt(HWND hwnd, HPENDATA NEAR *phpndt)
/***************************************************************

FUNCTION:   FGetHpndt(hwnd, phpndt)

PURPOSE:

   Gets a copy of a pendata from a window into parameter.

   Returns TRUE if successful.

***************************************************************/
   {
   if (vhinstPW)
      {
      HPENDATA hpndt = (HPENDATA)SendMessage(hwnd, IE_GETINK, IEGI_ALL, 0);

      if ((int)hpndt < 0)
         return FALSE;
      *phpndt = hpndt;
      return TRUE;
      }

   // else it's our own readonly "control":
   *phpndt = DuplicatePenData((HPENDATA)GetWindowWord(hwnd, 0), 0);
   return TRUE;
   }

BOOL NEAR PASCAL FSetHpndt(HWND hwnd, HPENDATA hpndt)
/***************************************************************

FUNCTION:   FSetHpndt(hwnd, hpndt)

PURPOSE:

   Sets a pendata into a window and destoys any pendata that may
   already be in there.

   Returns TRUE if successful.

***************************************************************/
   {
   BOOL fRet = TRUE;

   if (vhinstPW)
      {
      fRet = SendMessage(hwnd, IE_SETINK, IESI_REPLACE, (LPARAM)hpndt) == IER_OK;
      }
   else
      {
      if (hpndt = (HPENDATA)SetWindowLong(hwnd, 0, (LONG)hpndt))  // old pendata?
         DestroyPenData(hpndt);                             // destroy it
      }
   InvalidateRect(hwnd, NULL, TRUE);

   return fRet;
   }

VOID NEAR PASCAL ResizeBEDIT(int i)
/***************************************************************

FUNCTION:   ResizeBEDIT(int i)

PURPOSE:

   Resizes i'th variable BEDIT control after a control panel change.

***************************************************************/
   {
   if (vhinstPW)
      {
      HWND hwnd = vrgfield[i].hwnd;
      HRC hrc = (HRC)SendMessage(hwnd, WM_PENMISC, PMSC_GETHRC, 0L);
      GUIDE guide;

      if (hrc && (mFnPW(GetGuideHRC)(hrc, (LPGUIDE)&guide, NULL) == HRCR_OK))
         {
         int cxScroll = GetSystemMetrics(SM_CXVSCROLL) + 1;
         BOXEDITINFO bei;
         int cx = vrgfield[i].r.right - vrgfield[i].r.left - CXEESP;   
         int cy = vrgfield[i].r.bottom - vrgfield[i].r.top - CYEESP;

         mFnPW(GetPenMiscInfo)(PMI_BEDIT, (LPARAM)(LPBOXEDITINFO)&bei);

         // disallow BEDIT boxes larger than entire control:
         if (cy < bei.cyBox)
            return;

         guide.xOrigin = 0;
         guide.yOrigin = 0;
         guide.cyMid   = 0;
         guide.cxBox   = bei.cxBox;
         guide.cyBox   = bei.cyBox;
         guide.cxBase  = bei.cxBase;
         guide.cyBase  = bei.cyBase;

         SendMessage(hwnd, WM_PENCTL, HE_SETBOXLAYOUT,
            (LPARAM)(LPBOXLAYOUT)&bei.boxlayout);

         mFnPW(SetGuideHRC)(hrc, &guide, 0);
         SendMessage(hwnd, WM_PENMISC, PMSC_SETHRC, (LPARAM)hrc);

         cx = (((cx - cxScroll) / bei.cxBox) * bei.cxBox)
            + cxScroll + CXIBESP;
         cy = ((cy / bei.cyBox) * bei.cyBox) + CYIBESP;

         SetWindowPos(hwnd, HWND_TOP,
            vrgfield[i].r.left,
            vrgfield[i].r.top,
            cx, cy, 0);
         }
      }
   }

