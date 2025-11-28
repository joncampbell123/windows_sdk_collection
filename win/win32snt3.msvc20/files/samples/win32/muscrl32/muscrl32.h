/*
 * MUSCRL32.H
 *
 * Public definitions for applications that use the MicroScroll32,
 * a Win32 custom control DLL implementing a spin control.
 *
 *  - Messages
 *  - Prototypes for Message API Functions
 *  - Notification codes.
 *  - Control Styles.
 *
 * Version 1.1, October 1991, Kraig Brockschmidt
 *
 * Win32 & Win32 control format port, April 1994, Tarn Faulkner
 *
 */

//Control-specific messages
// *** Note that ranges and positions can be set larger than positions can 
// *** be returned for in WM_VSCROLL messages.  See SDK documentation
// *** for WM_VSCROLL for details.
#define MSM_HWNDASSOCIATESET  (WM_USER+1)
#define MSM_HWNDASSOCIATEGET  (WM_USER+2)
#define MSM_DWRANGESET        (WM_USER+3)
#define MSM_DWRANGEGET        (WM_USER+4)
#define MSM_WCURRENTPOSSET    (WM_USER+5)
#define MSM_WCURRENTPOSGET    (WM_USER+6)
#define MSM_FNOPEGSCROLLSET   (WM_USER+7)
#define MSM_FNOPEGSCROLLGET   (WM_USER+8)
#define MSM_FINVERTRANGESET   (WM_USER+9)
#define MSM_FINVERTRANGEGET   (WM_USER+10)
#define MSM_CRCOLORSET        (WM_USER+11)
#define MSM_CRCOLORGET        (WM_USER+12)


//Message API Functions
// *** Note that ranges and positions can be set larger than positions can 
// *** be returned for in WM_VSCROLL messages.  See SDK documentation
// *** for WM_VSCROLL for details.
HWND     CALLBACK MSHAssociateSet(HWND hWnd, HWND hWndAssociate);
HWND     CALLBACK MSHAssociateGet(HWND hWnd);
INT      CALLBACK MSDwRangeSet(HWND hWnd, INT nMin, INT nMax);
void     CALLBACK MSDwRangeGet(HWND hWnd, INT * pNMin, INT * pNMax);
INT      CALLBACK MSWCurrentPosSet(HWND hWnd, INT nPos);
INT      CALLBACK MSWCurrentPosGet(HWND hWnd);
BOOL     CALLBACK MSFNoPegScrollSet(HWND hWnd, BOOL fNoPegScroll);
BOOL     CALLBACK MSFNoPegScrollGet(HWND hWnd);
BOOL     CALLBACK MSFInvertRangeSet(HWND hWnd, BOOL fInvertRange);
BOOL     CALLBACK MSFInvertRangeGet(HWND hWnd);
COLORREF CALLBACK MSCrColorSet(HWND hWnd, UINT nColor, COLORREF cr);
COLORREF CALLBACK MSCrColorGet(HWND hWnd, UINT nColor);

//Notification codes sent via WM_COMMAND from the control.
#define MSN_ASSOCIATEGAIN   1
#define MSN_ASSOCIATELOSS   2
#define MSN_RANGECHANGE     3

//Color indices for MSM_COLORSET/GET and MSCrColorSet/Get
#define MSCOLOR_FACE        0
#define MSCOLOR_ARROW       1
#define MSCOLOR_SHADOW      2
#define MSCOLOR_HIGHLIGHT   3
#define MSCOLOR_FRAME       4

#define CCOLORS             5

//Control specific styles.

#define NUM_MUSCRL32_STYLES     5

#define MSS_VERTICAL        0x0001
#define MSS_HORIZONTAL      0x0002
#define MSS_NOPEGSCROLL     0x0004
#define MSS_TEXTHASRANGE    0x0008
#define MSS_INVERTRANGE     0x0010
