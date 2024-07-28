#include "options.h"

   // Externally used variables.

extern BOOL  bUseDDBs;
extern HWND  hWndAnimate;
extern HWND  hWndClip;
extern POINT ptClipSize;
extern int   nDIBsOpen;


   // Defines

#ifndef MAX_FILENAME
#define MAX_FILENAME 129               // Max # of bytes in a filename.
#endif


   // Class extra bytes for MDI Children.  Room for DIBINFO structure handle.

#define CBWNDEXTRA (sizeof (WORD))


   // MDI Child Window Words.

#define WW_DIB_HINFO 0                 // Handle to DIBINFO structure





   // Structure whose handle is stored in the MDI Children's window
   //  extra bytes.  Has all the info needed to be stored on a per
   //  bitmap basis.

typedef struct
   {
   HANDLE      hDIB;                // Handle to the DIB
   HPALETTE    hPal;                // Handle to the bitmap's palette.
   HBITMAP     hBitmap;             // Handle to the DDB.

   WORD        wDIBType;            // DIB's type - RGB, RLE4, RLE8, PM
   WORD        wDIBBits;            // Bits per pixel
   WORD        wDIBWidth;           // Print width of the DIB
   WORD        wDIBHeight;          // Print height of the DIB

   char        szFileName[MAX_FILENAME];  // DIB's filename
   RECT        rcClip;              // Clipboard cut rectangle.

   OPTIONSINFO Options;             // Option info from Options dialog box.

   } DIBINFO, FAR *LPDIBINFO;





   // A handle to memory containing this structure is passed into the
   //  WM_CREATE case of the DIB window.  It's used to initialize the
   //  DIBINFO structure stored in the DIB window's window words.

typedef struct
   {     
   HANDLE hDIB;                  // Handle to the DIB.
   char   szFileName [128];      // Its filename.
   } DIBCREATEINFO, FAR * LPDIBCREATEINFO;




   // User defined message for MDI child windows.

#define MYWM_ANIMATE         WM_USER     // Start palette animation
#define MYWM_RESTOREPALETTE  WM_USER+1   // Stop animation and restore palette
#define MYWM_QUERYNEWPALETTE WM_USER+2   // Frame got a WM_QUERYNEWPALETTE, 
                                         //   realize child's palette as 
                                         //   foreground palette.



long FAR PASCAL ChildWndProc(HWND hWnd, 
			     UINT message,
			     WPARAM wParam,
			     LPARAM lParam);


HPALETTE CurrentDIBPalette        (void);
HWND     GetCurrentMDIWnd         (void);
BOOL     GetCurrentDIBStretchFlag (void);
void     SetCurrentDIBStretchFlag (BOOL bFlag);

void     SendMessageToAllChildren (HWND hWnd, 
                               unsigned message, 
                                   WORD wParam, 
                                   LONG lParam);

void     CloseAllDIBWindows (void);

RECT     GetCurrentClipRect (HWND hWnd);
POINT    GetCurrentDIBSize  (HWND hWnd);
