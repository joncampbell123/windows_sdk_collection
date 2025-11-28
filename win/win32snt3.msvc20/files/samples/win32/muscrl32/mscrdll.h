/*
 * MUSCRDLL.H
 *
 * DLL Specific include file.
 *
 * Contains all definitions and and prototypes pertinent ONLY
 * to the DLL.  API related information is contained in MUSCROLL.H.
 * API in any way.
 *
 * Version 1.1, October 1991, Kraig Brockschmidt
 *
 * Win32 & Win32 control format port, April 1994, Tarn Faulkner
 *
 */

#include <windows.h>
#include <custcntl.h>   //Standard Windows header file for custom controls.
#include "muscrl32.h"   //Get interface stuff for the control.
#include "muscrres.h"   // Resource ID's

#define MUSCRL32CLASS           "MicroScroll32"
#define MUSCRL32DESCRIPTION     "A Spin control"
#define MUSCRL32DEFAULTTEXT     "TextForRange"

/*
 * In window extra bytes we simply store a local handle to
 * a MUSCROLL data structure.  The local memory is allocated
 * from the control's local heap (either application or DLL)
 * instead of from USER's heap, thereby saving system resources.
 *
 * Note that the window styles that are stored in the regular
 * windwow structure are copied here.  This is to optimize access
 * to these bits, avoiding extra calls to GetWindowLong.
 */

typedef struct tagMUSCROLL
    {
    HWND        hWndAssociate;  //Associate window handle
    DWORD       dwStyle;        //Copy of GetWindowLong(hWnd, GWL_STYLE)
    INT         nMin;           //Minimum position
    INT         nMax;           //Maximum position
    INT         nPos;           //Current position
    WORD        wState;         //State flags
    COLORREF    rgCr[CCOLORS];  //Configurable colors.
    } MUSCROLL;

typedef MUSCROLL FAR *LPMUSCROLL;
typedef MUSCROLL FAR *PMUSCROLL;

#define CBMUSCROLL      sizeof(MUSCROLL)

//Offsets to use with GetWindowLong
#define GWL_MUSCRL32DATA        0   // offset of control's instance data

//Extra bytes for the window if the size of a local handle.
#define MUSCRL32_EXTRA      sizeof(HANDLE)   // number of extra bytes for muscrl32 class

//Control state flags.
#define MUSTATE_GRAYED      0x0001
#define MUSTATE_HIDDEN      0x0002
#define MUSTATE_MOUSEOUT    0x0004
#define MUSTATE_UPCLICK     0x0008
#define MUSTATE_DOWNCLICK   0x0010
#define MUSTATE_LEFTCLICK   0x0008  //Repeated since MSS_VERTICAL and
#define MUSTATE_RIGHTCLICK  0x0010  //MSS_HORIZONTAL are exclusive.

//Combination of click states.
#define MUSTATE_CLICKED     (MUSTATE_LEFTCLICK | MUSTATE_RIGHTCLICK)

//Combination of state flags.
#define MUSTATE_ALL         0x001F

/*
 * Macros to change the control state given a PMUSCROLL
 * and state flag(s)
 */
#define StateSet(p, wFlags)    (p->wState |=  (wFlags))
#define StateClear(p, wFlags)  (p->wState &= ~(wFlags))
#define StateTest(p, wFlags)   (p->wState &   (wFlags))


//Private functions specific to the control.

//INIT.C
BOOL       PASCAL FRegisterControl(HANDLE);
LONG       PASCAL LMicroScrollCreate(HWND, UINT, PMUSCROLL, LPCREATESTRUCT);
BOOL       PASCAL FTextParse(LPSTR, LPINT, LPINT, LPINT);
INT        PASCAL NTranslateUpToChar(LPSTR FAR *, char);

//MSAPI.C
LONG       PASCAL LMicroScrollAPI(HWND, UINT, WPARAM, LONG, PMUSCROLL);


//MUSCROLL.C
void        PASCAL      PositionChange(HWND, PMUSCROLL);
void        PASCAL      ClickedRectCalc(HWND, PMUSCROLL, LPRECT);
INT         CALLBACK    MuScrl32SizeToText(DWORD, DWORD, HFONT,  LPSTR);
LRESULT     CALLBACK    MuScrl32WndProc(HWND,  UINT,  WPARAM, LPARAM);
// Dialog functions
BOOL        CALLBACK    MuScrl32Style(HWND,  LPCCSTYLE);
LRESULT     CALLBACK    MuScrl32DlgProc(HWND,  UINT,  WPARAM, LPARAM);
DWORD       PASCAL      DWFormStyleFlags(HWND hDlg, DWORD dwStyle );
BOOL        PASCAL      FRangePositionCheck(HWND hDlg);


//PAINT.C
LONG       PASCAL LMicroScrollPaint(HWND, PMUSCROLL);
void       PASCAL Draw3DButtonRect(HDC, HPEN, HPEN, INT, INT,\
                                   INT, INT, BOOL);

//Timer identifiers.
#define IDT_FIRSTCLICK      500
#define IDT_HOLDCLICK       501

#define CTICKS_FIRSTCLICK   400
#define CTICKS_HOLDCLICK    50


//Default range and position constants.
#define IDEFAULTMIN         0
#define IDEFAULTMAX         9
#define IDEFAULTPOS         5

