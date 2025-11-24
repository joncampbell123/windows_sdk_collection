/****************************************************************************/
/*                                                                          */
/*        DISPDIB.H - Include file for DisplayDib() function.               */
/*                                                                          */
/*        Note: You must include WINDOWS.H before including this file.      */
/*                                                                          */
/*        Copyright (c) 1990-1995, Microsoft Corp.  All rights reserved.    */
/*                                                                          */
/****************************************************************************/

// DisplayDib() error return codes
#define DISPLAYDIB_NOERROR          0x0000  // success
#define DISPLAYDIB_NOTSUPPORTED     0x0001  // function not supported
#define DISPLAYDIB_INVALIDDIB       0x0002  // null or invalid DIB header
#define DISPLAYDIB_INVALIDFORMAT    0x0003  // invalid DIB format
#define DISPLAYDIB_INVALIDTASK      0x0004  // not called from current task
#define DISPLAYDIB_STOP             0x0005  // stop requested
#define DISPLAYDIB_NOTACTIVE	    0x0006  // DisplayDibWindow not foreground
#define DISPLAYDIB_BADSIZE          0x0007  //

// flags for <wFlags> parameter of DisplayDib()
#define DISPLAYDIB_NOPALETTE        0x0010  // don't set palette
#define DISPLAYDIB_NOCENTER         0x0020  // don't center image
#define DISPLAYDIB_NOWAIT           0x0040  // don't wait before returning
#define DISPLAYDIB_NOIMAGE          0x0080  // don't draw image
#define DISPLAYDIB_ZOOM2            0x0100  // stretch by 2
#define DISPLAYDIB_DONTLOCKTASK     0x0200  // don't lock current task
#define DISPLAYDIB_TEST             0x0400  // testing the command
#define DISPLAYDIB_NOFLIP           0x0800  // dont page flip
#define DISPLAYDIB_BEGIN            0x8000  // start of multiple calls
#define DISPLAYDIB_END              0x4000  // end of multiple calls

#define DISPLAYDIB_MODE             0x000F  // mask for display mode
#define DISPLAYDIB_MODE_DEFAULT     0x0000  // default display mode
#define DISPLAYDIB_MODE_320x200x8   0x0001  // 320-by-200
#define DISPLAYDIB_MODE_320x240x8   0x0005  // 320-by-240

// function prototypes
UINT FAR PASCAL DisplayDib(LPBITMAPINFOHEADER lpbi, LPSTR lpBits, WORD wFlags);
UINT FAR PASCAL DisplayDibEx(LPBITMAPINFOHEADER lpbi, int x, int y, LPSTR lpBits, WORD wFlags);

#define DisplayDibBegin() DisplayDib(NULL, NULL, DISPLAYDIB_BEGIN|DISPDIB_NOWAIT)
#define DisplayDibEnd()   DisplayDib(NULL, NULL, DISPLAYDIB_END|DISPDIB_NOWAIT)

//
//  DisplayDibWindow class.
//
//  simple interface to DISPDIB as a window class.
//  draw images and create a fullscreen window in one easy step.
//
//  advantages over calling the APIs directly.
//
//      while in fullscreen mode, window will be sized to
//      cover entire display preventing other apps from getting
//      clicked on.
//
//      if window looses activation, fullscreen mode will be disabled
//
//      forwards all mouse and keyboard events to owner, easy way
//      to take over entire screen.
//
//      alows interop with a Win32 application (via WM_COPYDATA)
//
//  class name:     "DisplayDibWindow"
//                  class is registered when DISPDIB.DLL is loaded.
//                  as a global class.
//
//  messages:
//
//      DDM_SETFMT  set new DIB format or program a new palette
//
//		    fullscreen mode, will use best mode
//                  for displaying the passed DIB format.
//		    defaul is 320x240x8 tripple buffered
//
//                  the palette will be programed with the color
//                  table of the passed BITMAPINFOHEADER.
//
//	    wParam = 0
//          lParam = LPBITMAPINFOHEADER
//
//          returns 0 if success else DISPLAYDIB_* error code.
//
//      DDM_DRAW    draws DIB data to fullscreen
//                  format is assumed the same as format passed to
//                  DDM_BEGIN or DDM_FMT
//
//          wParam = flags
//          lParam = bits pointer.
//
//          returns 0 if success else DISPLAYDIB_* error code.
//
//      DDM_CLOSE   destroy window *and* free the DLL
//
//      WM_COPYDATA allows a Win32 app to send a DDM_ message
//
//          wParam = hwnd of sender
//          lParam = PCOPYDATASTRUCT
//                  dwData      - LOWORD: DDM_* message value.
//                  dwData      - HIWORD: wParam for message
//                  lpData      - lParam (pointer to a BITMAPINFOHEADER or bits)
//                  cbData      - size of data
//
//          returns   0 if success else DISPLAYDIB_* error code.
//

#define DISPLAYDIB_WINDOW_CLASS     "DisplayDibWindow"
#define DISPLAYDIB_DLL		    "DISPDIB.DLL"

#define DDM_SETFMT      WM_USER+0
#define DDM_DRAW        WM_USER+1
#define DDM_CLOSE       WM_USER+2

//
// inline function to send a message to a DisplayDibWindow
//
__inline UINT DisplayDibWindowMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, DWORD cbSize)
{
#if defined(_WIN32) || defined(WIN32)
        COPYDATASTRUCT cds;
        cds.dwData = MAKELONG(msg, wParam);
        cds.cbData = lParam ? cbSize : 0;
        cds.lpData = (LPVOID)lParam;
        return (UINT)SendMessage(hwnd, WM_COPYDATA, (WPARAM)(HWND)NULL, (LPARAM)(LPVOID)&cds);
#else
        return (UINT)SendMessage(hwnd, msg, wParam, lParam);
#endif
}

//
// inline function to create a DisplayDibWindow
//
__inline HWND DisplayDibWindowCreate(HWND hwndParent, HINSTANCE hInstance)
{
#if defined(_WIN32) || defined(WIN32)
    DWORD show = 2;
    DWORD zero = 0;
    LPVOID params[4] = {NULL, &zero, &show, 0};

    if ((UINT)LoadModule(DISPLAYDIB_DLL, &params) < (UINT)HINSTANCE_ERROR)
        return NULL;    // loading DISPDIB did not work
#else
    if ((UINT)LoadLibrary(DISPLAYDIB_DLL) < (UINT)HINSTANCE_ERROR)
        return NULL;    // loading DISPDIB did not work
#endif

    return CreateWindow(DISPLAYDIB_WINDOW_CLASS,"",
            WS_POPUP,
            0, 0, GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),
            hwndParent, NULL,
            (hInstance ? hInstance : GetWindowInstance(hwndParent)), NULL);
}

__inline void DisplayDibWindowClose(HWND hwnd)
{
    SendMessage(hwnd, DDM_CLOSE, 0, 0);
}

//
//  helper macros for a DisplayDibWindow
//
#define DisplayDibWindowBegin(hwnd)                     ShowWindow(hwnd, SW_SHOWNORMAL)
#define DisplayDibWindowEnd(hwnd)                       ShowWindow(hwnd, SW_HIDE)
#define DisplayDibWindowSetFmt(hwnd, lpbi)              DisplayDibWindowMessage(hwnd, DDM_SETFMT, 0, (LPARAM)(LPVOID)(lpbi), sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD))
#define DisplayDibWindowDraw(hwnd, flags, bits, size)   DisplayDibWindowMessage(hwnd, DDM_DRAW, (WPARAM)(UINT)(flags), (LPARAM)(LPVOID)(bits), (DWORD)(size))
