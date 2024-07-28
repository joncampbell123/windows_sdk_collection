/************************************************************

   Header file for PENAPP.C


************************************************************/

#ifndef _INC_PENAPP
#define _INC_PENAPP

/******** Prototypes *********/

LRESULT CALLBACK MainWndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK InputWndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK InfoWndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK RawWndProc (HWND, UINT, WPARAM, LPARAM);

BOOL FLoadRec (VOID);
BOOL FInitInstance (HANDLE hInstance, HANDLE hPrevInstance, int cmdShow);
BOOL FInitApp (HANDLE hInstance);
VOID CopyRawData (LPRCRESULT lprcresult);
VOID DrawArrow (HWND hwnd, HDC hdc);
VOID DrawShape (HWND hwnd, HDC hdc);
VOID DrawRawData (HDC hdc);
int  NSetRawExtents (HDC hdc, LPPENDATAHEADER lppndt, LPRECT lprectWnd);
VOID EnumerateStrokes (HDC hdc, LPRECT lprectWnd, int nWidth);
VOID SetGraphWindow (int mi);

/******** Macros *********/

/* GetLPWidth(): Convert given ink width into logical coordinates.  For
** these purposes, logical coordinates are actually tablet coordinates.
*/
#define GetLPWidth(hdc, nInkWidth, pnLPWidth)      \
            {                                      \
            POINT ptT = {nInkWidth, nInkWidth};    \
            DPtoTP(&ptT, 1);                       \
            *pnLPWidth = ptT.x;                    \
            }

/* The following macros return window dimensions.
*/
#define  XRawWnd(dxParent)    (0)
#define  YRawWnd(dyParent)    (DyInfoWnd(dyParent))
#define  DxRawWnd(dxParent)      ((dxParent)/3)
#define  DyRawWnd(dyParent)      ((dyParent)-DyInfoWnd(dyParent))

#define  XInfoWnd(dxParent)      (0)
#define  YInfoWnd(dyParent)      (0)
#define  DxInfoWnd(dxParent)     ((dxParent)/3)
#define  DyInfoWnd(dyParent)     ((dyParent)/2)

#define  XInputWnd(dxParent)     (DxRawWnd(dxParent))
#define  YInputWnd(dyParent)     (0)
#define  DxInputWnd(dxParent)    (dxParent-XInputWnd(dxParent))
#define  DyInputWnd(dyParent)    (dyParent-YInputWnd(dyParent))


/******** Constants *********/

#define rcrtNoResults   (RCRT_NOSYMBOLMATCH | RCRT_ALREADYPROCESSED | RCRT_NORECOG)
#define rgbRed          RGB(255, 0, 0)

#define szPenAppWnd  "Sample Pen App"        /* Window titles */
#define szInputWnd   "Input"
#define szInfoWnd    "Info"
#define szRawWnd     "Raw Data"

#define szPenAppClass      "PASampleClass"   /* Window class names */
#define szPenAppInputClass "PAInputClass"
#define szPenAppInfoClass  "PAInfoClass"
#define szPenAppRawClass   "PARawClass"

#define szSampleRec  "SREC.DLL"        /* Recognizer DLL names */
#define szShapeRec   "SHAPEREC.DLL"

#define cchMax       256         /* Count of characters for generic buffer */

#define dwLength     80          /* Arrow and border dimensions */
#define dxArrow         10
#define dyArrow         5
#define cBorder         5

#endif /* !_INC_PENAPP */
