/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

*******************************************************************************

                                   driver

*******************************************************************************

*/
/*#define NICK_DEBUG			/* Comment out if no debug */
#define OBJ_FONT 3

/* This bit in the dfType field signals that the dfBitsOffset field is an
   absolute memory address and should not be altered. */
#define PF_BITS_IS_ADDRESS  4

/* This bit in the dfType field signals that the font is device realized. */
#define PF_DEVICE_REALIZED  0x80

/* These bits in the dfType give the fonttype -
       raster, vector, other1, other2. */
#define PF_RASTER_TYPE      0
#define PF_VECTOR_TYPE      1
#define PF_OTHER1_TYPE      2
#define PF_OTHER2_TYPE      3

/*      GDI font families.                                            */
#define FF_DONTCARE   (0<<4)  /* Don't care or don't know.            */
#define FF_ROMAN      (1<<4)  /* Variable stroke width, serifed.      */
                              /* Times Roman, Century Schoolbook, etc.*/
#define FF_SWISS      (2<<4)  /* Variable stroke width, sans-serifed. */
                              /* Helvetica, Swiss, etc.               */
#define FF_MODERN     (3<<4)  /* Constant stroke width, serifed or sans-serif */
                              /* Pica, Elite, Courier, etc.           */
#define FF_SCRIPT     (4<<4)  /* Cursive, etc.                        */
#define FF_DECORATIVE (5<<4)  /* Old English, etc.                    */

/*      Font weights lightest to darkest.                               */
#define FW_DONTCARE             0
#define FW_THIN                 100
#define FW_EXTRALIGHT           200
#define FW_LIGHT                300
#define FW_NORMAL               400
#define FW_MEDIUM               500
#define FW_SEMIBOLD             600
#define FW_BOLD                 700
#define FW_EXTRABOLD            800
#define FW_HEAVY                900

#define FW_ULTRALIGHT           FW_EXTRALIGHT
#define FW_REGULAR              FW_NORMAL
#define FW_DEMIBOLD             FW_SEMIBOLD
#define FW_ULTRABOLD            FW_EXTRABOLD
#define FW_BLACK                FW_HEAVY


#if 0
typedef struct
{
    short int xcoord;
    short int ycoord;
} PTTYPE;

#endif

/*      DC Management Flags                                        */
#define DC_SPDevice   0000001 /*Seperate PDevice required per device/filename */
#define DC_1PDevice   0000002 /*Only 1 PDevice allowed per device/filename    */
#define DC_IgnoreDFNP 0000004 /*Ignore device/filename pairs when matching    */

#if 0
#define RC_NONE 0

typedef struct
{
    short int dpVersion;
    short int dpTechnology;
    short int dpHorzSize;
    short int dpVertSize;
    short int dpHorzRes;
    short int dpVertRes;
    short int dpBitsPixel;
    short int dpPlanes;
    short int dpNumBrushes;
    short int dpNumPens;
    short int futureuse;
    short int dpNumFonts;
    short int dpNumColors;
    short int dpDEVICEsize;
    unsigned short int dpCurves;
    unsigned short int dpLines;
    unsigned short int dpPolygonals;
    unsigned short int dpText;
    unsigned short int dpClip;
    unsigned short int dpRaster;
    short int dpAspectX;
    short int dpAspectY;
    short int dpAspectXY;
    short int dpStyleLen;
    PTTYPE    dpMLoWin;
    PTTYPE    dpMLoVpt;
    PTTYPE    dpMHiWin;
    PTTYPE    dpMHiVpt;
    PTTYPE    dpELoWin;
    PTTYPE    dpELoVpt;
    PTTYPE    dpEHiWin;
    PTTYPE    dpEHiVpt;
    PTTYPE    dpTwpWin;
    PTTYPE    dpTwpVpt;
    short int dpLogPixelsX;
    short int dpLogPixelsY;
    short int dpDCManage;
    short int dpCaps1;
    long  int dpSpotSizeX;
    long  int dpSpotSizeY;
    short int dpPalColors;
    short int dpPalReserved;
    short int dpPalResolution;
} GDIINFO;
#endif

typedef GDIINFO *PGDIINFO;
typedef GDIINFO FAR *LPGDIINFO;

#if 0

#ifndef DF_MAPSIZE
#define DF_MAPSIZE 1
#endif


typedef struct
{
    short int   dfType;
    short int   dfPoints;
    short int   dfVertRes;
    short int   dfHorizRes;
    short int   dfAscent;
    short int   dfInternalLeading;
    short int   dfExternalLeading;
    BYTE        dfItalic;
    BYTE        dfUnderline;
    BYTE        dfStrikeOut;
    short int   dfWeight;
    BYTE        dfCharSet;
    short int   dfPixWidth;
    short int   dfPixHeight;
    BYTE        dfPitchAndFamily;
    short int   dfAvgWidth;
    short int   dfMaxWidth;
    BYTE        dfFirstChar;
    BYTE        dfLastChar;
    BYTE        dfDefaultChar;
    BYTE        dfBreakChar;
    short int   dfWidthBytes;
    unsigned long int   dfDevice;
    unsigned long int   dfFace;
    unsigned long int   dfBitsPointer;
    unsigned long int   dfBitsOffset;
    BYTE        dfMaps[DF_MAPSIZE];
} FONTINFO;
#endif

typedef FONTINFO *PFONTINFO;
typedef FONTINFO FAR *LPFONTINFO;

#if 0
typedef struct
{
    short int ftHeight;
    short int ftWidth;
    short int ftEscapement;
    short int ftOrientation;
    short int ftWeight;
    BYTE ftItalic;
    BYTE ftUnderline;
    BYTE ftStrikeOut;
    BYTE ftOutPrecision;
    BYTE ftClipPrecision;
    unsigned short int ftAccelerator;
    short int ftOverhang;
} TEXTXFORM;
#endif

typedef TEXTXFORM *PTEXTXFORM;
typedef TEXTXFORM FAR *LPTEXTXFORM;

typedef struct
{
   int Hue,
       Lightness,
       Saturation;
} HLSBLOCK;
typedef HLSBLOCK *PHLSBLOCK;
typedef HLSBLOCK FAR *LPHLSBLOCK;

#if 0
typedef struct
{
    short int         Rop2;
    short int         bkMode;
    unsigned long int bkColor;
    unsigned long int TextColor;
    short int         TBreakExtra;
    short int         BreakExtra;
    short int         BreakErr;
    short int         BreakRem;
    short int         BreakCount;
    short int         CharExtra;
    unsigned long int LbkColor;
    unsigned long int LTextColor;
 } DRAWMODE;
#endif

typedef DRAWMODE *PDRAWMODE;
typedef DRAWMODE FAR *LPDRAWMODE;

typedef DWORD PCOLOR;
typedef PCOLOR *PPCOLOR;
typedef PCOLOR FAR *LPPCOLOR;

typedef LPSTR LPDM;
typedef short FAR *LPSHORT;

/* Output Style definitions used by GDI support routines written in C */

#define OS_ARC                         3
#define OS_SCANLINES                   4
#define OS_RECTANGLE                   6
#define OS_ELLIPSE                     7
#define OS_MARKER                      8
#define OS_POLYLINE                    18
#define OS_WINDING_NUMBER_FILL_POLYGON 20
#define OS_ALTERNATE_FILL_POLYGON      22
#define OS_PIE                         23
#define OS_POLYMARKER                  24
#define OS_CHORD                       39
#define OS_CIRCLE                      55

typedef struct
{
    short Index;
    DWORD Color;
} COLORTABLEENTRY;
typedef COLORTABLEENTRY *PCOLORTABLEENTRY;
typedef COLORTABLEENTRY FAR *LPCOLORTABLEENTRY;

/* Driver exported function declarations */
extern void FAR PASCAL get_bounds (LPPDEVICE,LPPOINT,LPPOINT,short);
/*extern void FAR PASCAL BitBlt (LPPDEVICE,int,int,LPPDEVICE,int,int,int,int,
  DWORD,LPSTR,LPDRAWMODE);*/
extern DWORD FAR PASCAL ColorInfo (LPPDEVICE,DWORD,LPPCOLOR);
extern int FAR PASCAL Control (LPPDEVICE,int,LPSTR,LPSTR);
extern int FAR PASCAL DeviceMode (HANDLE,HANDLE,LPSTR,LPSTR);
extern void FAR PASCAL Disable (LPPDEVICE);
extern int FAR PASCAL Enable (LPSTR,int,LPSTR,LPSTR,LPSTR);
extern int FAR PASCAL EnumDFonts (LPPDEVICE,LPSTR,FARPROC,LPSTR);
extern FAR PASCAL EnumObj (LPPDEVICE,int,FARPROC,LPSTR);
extern short FAR PASCAL Output (LPPDEVICE,int,int,LPPOINT,LPSTR,LPSTR,LPDRAWMODE,
  LPRECT);
extern DWORD FAR PASCAL Pixel (LPPDEVICE,int,int,DWORD,LPDRAWMODE);
extern int FAR PASCAL RealizeObject (LPPDEVICE,int,LPSTR,LPSTR,LPTEXTXFORM);
extern int FAR PASCAL ScanLR (LPPDEVICE,int,int,DWORD,int);
extern DWORD FAR PASCAL StrBlt (LPPDEVICE,int,int,LPRECT,LPSTR,int,LPFONTINFO,
  LPDRAWMODE,LPTEXTXFORM);

//extern void FAR PASCAL OutputDebugString(LPSTR);

DWORD NEAR PASCAL GetPaperInfo(LPENVIRONMENT, LPSTR, WORD);
DWORD NEAR PASCAL GetBinInfo(LPENVIRONMENT, LPSTR, WORD);


#if 0
/* Spooler function declarations */
short FAR PASCAL OpenJob (LPSTR,LPSTR,short);
void FAR PASCAL CloseJob (short);
short FAR PASCAL WriteSpool (short,LPSTR,short);
void FAR PASCAL WriteDialog (short,LPSTR,short);
BOOL FAR PASCAL StartSpoolPage (short);
BOOL FAR PASCAL EndSpoolPage (short);
void FAR PASCAL DeleteJob (short);
#endif

/* Driver supported function declarations */

BOOL FAR PASCAL StartPlotPage (LPPDEVICE,HANDLE);
BOOL FAR PASCAL EndPlotPage (LPPDEVICE,HANDLE);
short FAR PASCAL StartJob (LPPDEVICE,LPSTR,LPSTR,HANDLE);
short FAR PASCAL EndJob (LPPDEVICE,HANDLE);
short FAR PASCAL PutJob (LPPDEVICE,HANDLE,LPSTR,short);
void FAR PASCAL PutDialog (LPPDEVICE,HANDLE,LPSTR,short);
BOOL FAR PASCAL GetSpoolState ();
int FAR PASCAL ColorDistance (DWORD,DWORD);
short FAR PASCAL GetDistance (short,short,short,short);
PSTR FAR PASCAL GetString (short);
