/****************************************************************************
 DIB.h

 The DIB module handles loading and management of DIBs from files.

****************************************************************************/


/****************************************************************************
   Constants
****************************************************************************/

/* Bit values for the DIB attributes flag (fFileOptions),
   also used as control IDs for the radiobuttons for DIB bitcount 
   in the File/Open dialog */
#define F_1BPP        DLGOPEN_1BPP
#define F_4BPP        DLGOPEN_4BPP
#define F_8BPP        DLGOPEN_8BPP
#define F_24BPP       DLGOPEN_24BPP

/* Bit values for the DIB attributes flag (fFileOptions), 
   also used as control IDs for the radiobuttons for DIB compression type 
   in the File/Open dialog */
#define F_RLE4        DLGOPEN_RLE4
#define F_RLE8        DLGOPEN_RLE8
#define F_RGB         DLGOPEN_RGB

/* flags for _lseek */
#define  SEEK_CUR  1
#define  SEEK_END  2
#define  SEEK_SET  0

/* Number of bytes to be read during each read operation. */
#define MAXREAD  32768   

/* Header signatutes for various resources */
#define BFT_ICON   0x4349   /* 'IC' */
#define BFT_BITMAP 0x4d42   /* 'BM' */
#define BFT_CURSOR 0x5450   /* 'PT' */

/* Macro to determine if resource is a DIB */
#define ISDIB(bft) ((bft) == BFT_BITMAP)

/* Macro to align given value to the closest DWORD (unsigned long ) */
#define ALIGNULONG(i)    (((i) + 3) / 4 * 4)

/* Macro to determine to round off the given value to the closest byte */
#define WIDTHBYTES(i)    (((i) + 31) / 32 * 4)

#define PALVERSION       0x300
#define MAXPALETTE       256      /* max # supported palette entries */


/****************************************************************************
   Globals
****************************************************************************/

extern HANDLE   curDIB;             /* current DIB */
extern char     curDIBName[128];    /* name of current DIB file */
extern POINT    curDIBSize;         /* size of current DIB */


/****************************************************************************
   Functions
****************************************************************************/

int OpenNewCurDIB( HWND parent );

HANDLE OpenDIB( LPSTR szFile );
BOOL WriteDIB( LPSTR szFile,HANDLE hdib );
WORD PaletteSize( VOID FAR * pv );
WORD DibNumColors( VOID FAR * pv );
HPALETTE CreateDibPalette( HANDLE hdib );
HPALETTE CreateBIPalette( LPBITMAPINFOHEADER lpbi );
HANDLE DibFromBitmap( HBITMAP hbm, DWORD biStyle, WORD biBits, HPALETTE hpal );
HANDLE BitmapFromDib( HANDLE hdib, HPALETTE hpal );
BOOL DibBlt( HDC hdc, int x0, int y0, HANDLE hdib, int x1, int y1, 
             int dx, int dy, int startScan, int numScans );
BOOL StretchDibBlt( HDC hdc, int x0, int y0, int dx, int dy, 
                    HANDLE hdib, int x1, int y1, int dx1, int dy1, LONG rop );
BOOL DibInfo( HANDLE hdib,LPBITMAPINFOHEADER lpbi );
HANDLE ReadDibBitmapInfo( int fh );
BOOL DrawBitmap( HDC hdc, int x, int y, HBITMAP hbm, DWORD rop );

DWORD PASCAL lread( int fh, VOID FAR *pv, DWORD ul );
DWORD PASCAL lwrite( int fh, VOID FAR *pv, DWORD ul );


