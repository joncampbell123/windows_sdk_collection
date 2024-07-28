/*----------------------------------------------------------------------------*\
|   Routines for dealing with Device independent bitmaps                       |
|                                                                              |
|                                                                              |
\*----------------------------------------------------------------------------*/

HANDLE      OpenDIB(HFILE fh);
BOOL        WriteDIB(HFILE fh,HANDLE hdib);
WORD        PaletteSize(VOID FAR * pv);
WORD        DibNumColors(VOID FAR * pv);
HPALETTE    CreateDibPalette(HANDLE hdib);
HPALETTE    CreateBIPalette(LPBITMAPINFOHEADER lpbi);
HANDLE      DibFromBitmap(HBITMAP hbm, DWORD biStyle, WORD biBits, HPALETTE hpal, WORD wUsage);
HANDLE      DibFromDib(HANDLE hdib, DWORD biStyle, WORD biBits, HPALETTE hpal, WORD wUsage);
HBITMAP     BitmapFromDib(HANDLE hdib, HPALETTE hpal, WORD wUsage);
BOOL        SetDibUsage(HANDLE hdib, HPALETTE hpal,WORD wUsage);
BOOL        DibInfo(HANDLE hdib,LPBITMAPINFOHEADER lpbi);
HANDLE      ReadDibBitmapInfo(HFILE fh);
BOOL        SetPalFlags(HPALETTE hpal, int iIndex, int cntEntries, WORD wFlags);
BOOL        PalEq(HPALETTE hpal1, HPALETTE hpal2);

BOOL        DrawBitmap(HDC hdc, int x, int y, HBITMAP hbm, DWORD rop);
BOOL        StretchBitmap(HDC hdc, int x, int y, int dx, int dy, HBITMAP hbm, int x0, int y0, int dx0, int dy0, DWORD rop);

BOOL        DibBlt(HDC hdc, int x0, int y0, int dx, int dy, HANDLE hdib, int x1, int y1, LONG rop, WORD wUsage);
BOOL        StretchDibBlt(HDC hdc, int x, int y, int dx, int dy, HANDLE hdib, int x0, int y0, int dx0, int dy0, LONG rop, WORD wUsage);

LPVOID      DibLock(HANDLE hdib,int x, int y);
VOID        DibUnlock(HANDLE hdib);
LPVOID      DibXY(LPBITMAPINFOHEADER lpbi,int x, int y);
HANDLE      CreateDib(HANDLE hdib, int dx, int dy);

#define BFT_ICON   0x4349   /* 'IC' */
#define BFT_BITMAP 0x4d42   /* 'BM' */
#define BFT_CURSOR 0x5450   /* 'PT' */

#define ISDIB(bft) ((bft) == BFT_BITMAP)
#define ALIGNULONG(i)   ((i+3)/4*4)        /* ULONG aligned ! */
#define WIDTHBYTES(i)   ((i+31)/32*4)      /* ULONG aligned ! */

#define PALVERSION      0x300
#define MAXPALETTE      256
