/*---------------------------------------------------------------------------*\
| DEVICE OBJECT MODULE                                                        |
|                                                                             |
|   OBJECT <DEVOBJECT>                                                        |
|                                                                             |
|   METHODS                                                                   |
|     GetDeviceObjects()                                                      |
|     FreeDeviceObjects()                                                     |
|     CreateDeviceObject()                                                    |
|     CopyDeviceObject()                                                      |
|     SetCurrentObject()                                                      |
|     GetCurrentObject()                                                      |
|     AddObject()                                                             |
|     RemoveObject()                                                          |
|     GetObjectCount()                                                        |
|                                                                             |
|   Copyright 1990-1992 by Microsoft Corporation                              |
|                                                                             |
\*---------------------------------------------------------------------------*/
#define DEV_PEN                 1
#define DEV_BRUSH               2
#define DEV_FONT                3
#define DEV_INDEX               4

typedef HANDLE HDEVOBJECT;

typedef struct tagDEVOBJECT
{
     HDC          hDC;
     WORD         wType;
     WORD         wFlags;
     GLOBALHANDLE hMem;
     short        nCount;
     short        nCurrent;
} DEVOBJECT;
typedef DEVOBJECT      *PDEVOBJECT;
typedef DEVOBJECT NEAR *NPDEVOBJECT;
typedef DEVOBJECT FAR  *LPDEVOBJECT;

HDEVOBJECT PASCAL GetDeviceObjects(HDC,WORD,WORD);
HDEVOBJECT PASCAL FreeDeviceObjects(HDEVOBJECT);
int        PASCAL SetCurrentObject(HDEVOBJECT,short);
int        PASCAL GetCurrentObject(HDEVOBJECT);
int        PASCAL GetObjectCount(HDEVOBJECT);
BOOL       PASCAL CopyDeviceObject(LPSTR,HDEVOBJECT);
HANDLE     PASCAL CreateDeviceObject(HDEVOBJECT);
int        PASCAL AddObject(HDEVOBJECT,LPSTR);
int        PASCAL RemoveObject(HDEVOBJECT);

int        FAR PASCAL EnumAllDevicePens(LPLOGPEN,LPDEVOBJECT);
int        FAR PASCAL EnumAllDeviceBrushes(LPLOGBRUSH,LPDEVOBJECT);
int        FAR PASCAL EnumAllDeviceFonts(LPLOGFONT,LPTEXTMETRIC,short,LPDEVOBJECT);
int        FAR PASCAL EnumAllFontFaces(LPLOGFONT,LPTEXTMETRIC,short,LPDEVOBJECT);
/*---------------------------------------------------------------------------*\
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
| LIBRARY MODULE - support routines                                           |
\*---------------------------------------------------------------------------*/
LPSTR    PASCAL litoa(int,LPSTR,int);
int      PASCAL latoi(LPSTR);
LPSTR    PASCAL ReverseString(LPSTR,int,int);

/*---------------------------------------------------------------------------*\
| Flags for GetDeviceObject (currently fonts only)                            |
\*---------------------------------------------------------------------------*/
#define TT_FONTS                0x0001
#define NON_TT_DEVICE_FONTS     0x0002
#define NON_TT_NON_DEVICE_FONTS 0x0004
#define ALL_DEVICE_FONTS        (NON_TT_DEVICE_FONTS | TT_FONTS)
#define ALL_NON_TT_FONTS        (NON_TT_DEVICE_FONTS | NON_TT_NON_DEVICE_FONTS)
#define ALL_FONTS               (ALL_NON_TT_FONTS | TT_FONTS)

/*---------------------------------------------------------------------------*\
| TEST MODULE                                                                 |
\*---------------------------------------------------------------------------*/
#define FUNCT_SIZE                8
#define DESCR_SIZE               60
#define AREA_SIZE                 3
#define LINE_SIZE                75

#define ERROR_NONE                0              // Future implementation
#define ERROR_ALLOC               1              //   for logfile output
#define ERROR_RETURN              2              //   capabilities.
#define ERROR_TEST                3              //

#define LOGFILE_LEVEL0            0              // HEADER INFORMATION
#define LOGFILE_LEVEL1            1              // Basic Test Info
#define LOGFILE_LEVEL2            2              // Timing addition


typedef struct tagKRNTEST
{
     WORD wKernelFlags;
} KRNTEST;
typedef KRNTEST      *PKRNTEST;
typedef KRNTEST NEAR *NPKRNTEST;
typedef KRNTEST FAR  *LPKRNTEST;

typedef struct tagUSRTEST
{
     WORD  wDlgFlags;
     DWORD dwDlgStyles;
     WORD  wWinFlags;
     DWORD dwWinStyles;
} USRTEST;
typedef USRTEST      *PUSRTEST;
typedef USRTEST NEAR *NPUSRTEST;
typedef USRTEST FAR  *LPUSRTEST;

typedef struct tagGDITEST
{
     HDC        hDC;
     HDEVOBJECT hPens;                           // index specifiers
     HDEVOBJECT hBrushes;                        // index specifiers
     HDEVOBJECT hFonts;                          // index specifiers
     HANDLE     hRegion;
     int        nBkMode;
     DWORD      crBkColor;
     int        nPolyFillMode;
     int        nROP2;
     int        nStretchMode;
     DWORD      crTextColor;
     DWORD      dwROP;
} GDITEST;
typedef GDITEST      *PGDITEST;
typedef GDITEST NEAR *NPGDITEST;
typedef GDITEST FAR  *LPGDITEST;

typedef struct tagTEST
{
     char    lpszModule[10];
     HANDLE  hInstance;
     LPSTR   lpszFunction;
     LPSTR   lpszDescription;
     WORD    wTestArea;
     WORD    wStatus;
     WORD    wGranularity;
     WORD    wIterations;
     RECT    rTestRect;
     KRNTEST ktTest;
     USRTEST utTest;
     GDITEST gtTest;
} TEST;
typedef TEST      *PTEST;
typedef TEST NEAR *NPTEST;
typedef TEST FAR  *LPTEST;

BOOL FAR PASCAL InitTest(LPSTR,HANDLE,LPTEST);
BOOL FAR PASCAL KillTest(LPTEST);

/*---------------------------------------------------------------------------*\
| DRAWING OBJECTS.                                                            |
\*---------------------------------------------------------------------------*/
#define OBJ_ARC                 1
#define OBJ_CHORD               2
#define OBJ_ELLIPSE             3
#define OBJ_LINE                4
#define OBJ_PIE                 5
#define OBJ_POLYGON             6
#define OBJ_POLYLINE            7
#define OBJ_RECTANGLE           8
#define OBJ_ROUNDRECT           9

/*---------------------------------------------------------------------------*\
|                                                                             |
\*---------------------------------------------------------------------------*/
typedef struct tagFONT
{
     short      nFontType;
     LOGFONT    lf;
     TEXTMETRIC tm;
} FONT;
typedef FONT      *PFONT;
typedef FONT NEAR *NPFONT;
typedef FONT FAR  *LPFONT;

/*---------------------------------------------------------------------------*\
| DEVICE INFORMATION MODULE                                                   |
\*---------------------------------------------------------------------------*/
typedef struct tagDEVINFO
{
     short nDriverVersion;
     short nTechnology;
     short nHorzSizeMM;
     short nVertSizeMM;
     short nHorzRes;
     short nVertRes;
     short nLogPixelsX;
     short nLogPixelsY;
     short nBitsPixel;
     short nPlanes;
     short nBrushes;
     short nPens;
     short nFonts;
     short nColors;
     short nAspectX;
     short nAspectY;
     short nAspectXY;
     short nPDeviceSize;
     WORD  wClipCaps;
     WORD  wRasterCaps;
     WORD  wCurveCaps;
     WORD  wLineCaps;
     WORD  wPolygonCaps;
     WORD  wTextCaps;
} DEVINFO;
typedef DEVINFO      *PDEVINFO;
typedef DEVINFO NEAR *NPDEVINFO;
typedef DEVINFO FAR  *LPDEVINFO;

BOOL PASCAL GetDeviceInfo(HWND,LPDEVINFO);

/*---------------------------------------------------------------------------*\
| PRINTER MODULE                                                              |
\*---------------------------------------------------------------------------*/
typedef struct tagPRINTER
{
     char szProfile[15];
     char szName[80];
     char szDriver[15];
     char szPort[40];
     char szSystemVer[10];
     char szDriverVer[10];
} PRINTER;
typedef PRINTER      *PPRINTER;
typedef PRINTER NEAR *NPPRINTER;
typedef PRINTER FAR  *LPPRINTER;

HDC PASCAL GetPrinterDC(HWND hWnd, LPSTR lpProfile, LPHANDLE lphDevMode);

/*---------------------------------------------------------------------------*\
| DATE TIME MODULE                                                            |
\*---------------------------------------------------------------------------*/
typedef struct tagDATETIME
{
     BYTE bDay;
     BYTE bMonth;
     WORD wYear;
     BYTE bDayWeek;
     BYTE bHours;
     BYTE bMinutes;
     BYTE bSeconds;
     BYTE bHundreths;
} DATETIME;
typedef DATETIME      *PDATETIME;
typedef DATETIME NEAR *NPDATETIME;
typedef DATETIME FAR  *LPDATETIME;

void FAR PASCAL GetSystemDateTime(LPDATETIME);

/*---------------------------------------------------------------------------*\
|                                                                             |
\*---------------------------------------------------------------------------*/
typedef struct tagPRINTCAPS
     {
          short nIndex;
          PSTR  szType;
     } PRINTCAPS;
typedef PRINTCAPS      *PPRINTCAPS;
typedef PRINTCAPS NEAR *NPPRINTCAPS;
typedef PRINTCAPS FAR  *LPPRINTCAPS;

typedef struct tagLONGTEXT
     {
          DWORD dwFlag;
          PSTR lpText;
     } LONGTEXT;
typedef LONGTEXT      *PLONGTEXT;
typedef LONGTEXT NEAR *NPLONGTEXT;
typedef LONGTEXT FAR  *LPLONGTEXT;

/*---------------------------------------------------------------------------*\
| FILE I/O MODULE                                                             |
\*---------------------------------------------------------------------------*/
DWORD FAR PASCAL WriteFile(int,LPSTR,DWORD);
DWORD FAR PASCAL ReadFile(int,LPSTR,DWORD);

/*---------------------------------------------------------------------------*\
| BITMAP MODULE - support routines.                                           |
\*---------------------------------------------------------------------------*/
#define DIB_OS2                 0
#define DIB_WIN                 1

HBITMAP      FAR PASCAL CreateBrushBitmap(HDC,short,short,HBRUSH);
HBITMAP      FAR PASCAL CreatePixelBitmap(HDC,short,short);
HBITMAP      FAR PASCAL CreateRGBBitmap(HDC,short,short,COLORREF);
BOOL         FAR PASCAL OutputDDBToDevice(HDC,short,short,HBITMAP,DWORD);
BOOL         FAR PASCAL OutputDIBToDevice(HDC,short,short,HANDLE,DWORD);
HANDLE       FAR PASCAL ConvertDDBToDIB(HDC,HBITMAP,WORD,DWORD);
int          FAR PASCAL GetColorTableSize(LPBITMAPINFOHEADER);
GLOBALHANDLE FAR PASCAL GetMemoryBitmap(HBITMAP);
BOOL         FAR PASCAL CompareBitmaps(HBITMAP,HBITMAP);

/*---------------------------------------------------------------------------*\
| MISC - support routines.                                                    |
\*---------------------------------------------------------------------------*/
BOOL FAR PASCAL TstExtTextOutRect(HDC,WORD,LPRECT,LPSTR,short,LPINT);
BOOL FAR PASCAL TstDrawObject(HDC,short,short,short,short,WORD);
BOOL FAR PASCAL TstGrayScale(HDC,short,short,short,short);
BOOL FAR PASCAL TstBitBltRop(HDC,short,short,short,short,HBRUSH,HBRUSH,HBRUSH,DWORD,BOOL);
BOOL FAR PASCAL TstColorMapping(HDC,COLORREF);
BOOL FAR PASCAL VerifyPixelColor(HDC,short,short,COLORREF);

BOOL FAR        FindBitmapBit(LPSTR,int);
BOOL FAR        CheckROPBit(BOOL,BOOL,BOOL,BOOL,WORD);

/*---------------------------------------------------------------------------*\
| LOGFILE - Logfile output routines.                                          |
\*---------------------------------------------------------------------------*/
int FAR PASCAL CreateLogFile(LPSTR);
int FAR PASCAL WriteLogFile(LPSTR,LPSTR);

/*---------------------------------------------------------------------------*\
| COMMON MACROES.                                                             |
\*---------------------------------------------------------------------------*/
#define SWAP_INT(x,y)  {int _t; _t=x; x=y; y=_t;}
#define SWAP_BYTE(x,y) {char _t; _t=*x; *x=*y; *y=_t;}
