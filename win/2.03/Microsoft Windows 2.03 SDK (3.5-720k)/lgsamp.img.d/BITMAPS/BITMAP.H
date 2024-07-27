#define ID_ABOUT                100

/* bitmap menu items */

#define IDM_BITMAP1             200
#define IDM_BITMAP2             201
#define IDM_BITMAP3             202

/* pattern menu items */

#define IDM_PATTERN1            300
#define IDM_PATTERN2            301
#define IDM_PATTERN3            302
#define IDM_PATTERN4            303

/* mode menu items */

#define IDM_BLACKONWHITE        400
#define IDM_WHITEONBLACK        401
#define IDM_COLORONCOLOR        402

#define PATORDEST       0x00FA0089L

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL BitmapInit(HANDLE);
long FAR PASCAL BitmapWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
HBITMAP MakeColorBitmap(HWND);
