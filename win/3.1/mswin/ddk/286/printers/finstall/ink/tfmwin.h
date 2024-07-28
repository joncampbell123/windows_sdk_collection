/**[f******************************************************************
* tfmwin.h -
*
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */
  
/*
***************************************************************************
Windows Screen Font Formatter -- HEADER
***************************************************************************
  
E.D. Trautman                       Mar88-
  
Filename: windows.h
MSC 5.0 (large model)
  
========================================================
|| (C) Copyright 1988 IntelliMetrics Instrument Corp. ||
|| (C) Copyright 1988 Compugraphic Corporation        ||
========================================================
  
Notes:
------
1.
  
  
***************************************************************************/
  
/*************************************************************************/
  
#define KERNMAX 512     /* max  number of kern pairs    */
  
/*  HP Default Character -- this is used EVERYWHERE, all
*  driver code, all translation tables, all pfm makers.
*/
#define HP_DF_CH    ((BYTE) 0x7F)
  
  
/* Other constants which are used in the TFM converter
*/
#define DEV_RES   300
#define MIN_SIZE  4
#define MAX_SIZE  999
  
  
  
/* SS Class numbers
*/
#define MATH      18
#define WIN       9
  
  
  
/*  Face Numbers
*/
#define FF_DECORATIVE (5<<4)
#define FF_DONTCARE (0<<4)
#define FF_MODERN (3<<4)
#define FF_ROMAN (1<<4)
#define FF_SCRIPT (4<<4)
#define FF_SWISS (2<<4)
  
  
/*  HP Character sets
*/
#define MATH8_CHARSET       180
#define PIFONT_CHARSET      181
#define LINEDRAW_CHARSET    182
#define PCLINE_CHARSET      183
#define TAXLINE_CHARSET     184
  
  
/* symbol set classification */
  
#define LATIN_TEXT_SET    0x0001
#define INTL_TEXT_SET     0x0002
#define PROPER_QUOTE_SET  0x0004
#define DOUBLE_QUOTE_SET  0x0008
#define MATH_SET          0x0010
#define LEGAL_SET         0x0020
#define LINE_SET          0x0040
#define PC_LINE_SET       0x0080
#define PI_SET            0x0100
#define DINGBATS_SET      0x0200
#define OCR_SET           0x0400
#define BARCODE_SET       0x0800
#define SPECIAL_SET       0x1000
#define TAXLINE_SET       0x2000
  
  
/*  DRIVERINFO version number (i.e., version of the structure).
*/
#define DRIVERINFO_VERSION  1
  
  
//typedef struct
//  {
//  WORD    dfVersion;
//  DWORD   dfSize;
//  char    dfCopyright[60];
//  WORD    dfType;
//  WORD    dfPoints;
//  WORD    dfVertRes;
//  WORD    dfHorizRes;
//  WORD    dfAscent;
//  WORD    dfInternalLeading, dfExternalLeading;
//  BYTE    dfItalic, dfUnderline, dfStrikeOut;
//  WORD    dfWeight;
//  BYTE    dfCharSet;
//  WORD    dfPixWidth;
//  WORD    dfPixHeight;
//  BYTE    dfPitchAndFamily;
//  WORD    dfAvgWidth;
//  WORD    dfMaxWidth;
//  BYTE  dfFirstChar, dfLastChar, dfDefaultChar, dfBreakChar;
//  WORD    dfWidthBytes;
//  DWORD   dfDevice;
//  DWORD   dfFace;
//  DWORD   dfBitsPointer;
//  DWORD   dfBitsOffset;
//  WORD    dfCharOffset[1];    /* size is dfLastChar-dfFirstChar+2 */
//  } PFMHEADER;
//
//typedef struct
//  {
//  WORD    dfSizeFields;
//  DWORD   dfExtMetricsOffset;
//  DWORD   dfExtentTable;
//  DWORD   dfOriginTable;
//  DWORD   dfPairKernTable;
//  DWORD   dfTrackKernTable;
//  DWORD   dfDriverInfo;
//  DWORD   dfReserved;
//  } PFMEXTENSION;
//
//typedef struct
//  {
//  short   etmSize;
//  short   etmPointSize;
//  short   etmOrientation;
//  short   etmMasterHeight;
//  short   etmMinScale;
//  short   etmMaxScale;
//  short   etmMasterUnits;
//  short   etmCapHeight;
//  short   etmXHeight;
//  short   etmLowerCaseAscent;
//  short   etmLowerCaseDescent;
//  short   etmSlant;
//  short   etmSuperScript;
//  short   etmSubScript;
//  short   etmSuperScriptSize;
//  short   etmSubScriptSize;
//  short   etmUnderlineOffset;
//  short   etmUnderlineWidth;
//  short   etmDoubleUpperUnderlineOffset;
//  short   etmDoubleLowerUnderlineOffset;
//  short   etmDoubleUpperUnderlineWidth;
//  short   etmDoubleLowerUnderlineWidth;
//  short   etmStrikeOutOffset;
//  short   etmStrikeOutWidth;
//  WORD    etmKernPairs;
//  WORD    etmKernTracks;
//  } EXTTEXTMETRIC;
//
//typedef struct
//  {
//  union {
//      BYTE each[2];
//      WORD both;
//  } kpPair;
//  short kpKernAmount;
//  } KERNPAIR;
  
  
typedef struct
{
    WORD width;
    /*      WORD offset; */
  
} CHARWIDTH;
  
  
//typedef enum
//  {
//  epsymUserDefined,
//  epsymRoman8,
//  epsymKana8,
//  epsymMath8,
//  epsymUSASCII,
//  epsymLineDraw,
//  epsymMathSymbols,
//  epsymUSLegal,
//  epsymRomanExt,
//  epsymISO_DenNor,
//  epsymISO_UK,
//  epsymISO_France,
//  epsymISO_German,
//  epsymISO_Italy,
//  epsymISO_SwedFin,
//  epsymISO_Spain,
//  epsymGENERIC7,
//  epsymGENERIC8,
//  epsymECMA94
//  } SYMBOLSET;
//
//typedef struct
//  {
//  SYMBOLSET symbolSet;        /* kind of translation table */
//  DWORD offset;           /* location of user-defined table */
//  WORD len;           /* length (in bytes) of table */
//  BYTE firstchar, lastchar;   /* table ranges from firstchar to lastchar */
//  } TRANSTABLE;
//
//typedef struct
//  {
//  WORD  epSize;       /* size of this data structure */
//  WORD  epVersion;    /* number indicating version of struct */
//  DWORD epMemUsage;   /* amt of memory font takes up in printer */
//  DWORD epEscape;     /* pointer to escape that selects the font */
//  TRANSTABLE xtbl;    /* character set translation info */
///*    UWORD epFontClass; */   /* 0:bitmaps;1:PCLEO;2:compressed */
//  } DRIVERINFO;
  
typedef PFMHEADER far * LPPFMHEADER;
typedef PFMEXTENSION far * LPPFMEXTENSION;
typedef EXTTEXTMETRIC far * LPEXTTEXTMETRIC;
typedef KERNPAIR far * LPKERNPAIR;
typedef DRIVERINFO far * LPDRIVERINFO;
typedef CHARWIDTH far * LPCHARWIDTH;
  
  
typedef PFMHEADER windows_header_type;
  
typedef struct {
    int nchars;
    int horiz_table[257];
    int vert_table[257];
    int offset_table[257];
    int escape_table[257];
    int width_table[257];
    int extent_table[257];
} windows_metrics_type;
  
//typedef struct {
//  int npairs;
//  LPKERNPAIR kpptr;
//  } kern_pairs_type;
  
/*      Font weights lightest to darkest.      */
  
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
  
  
//#define BASELINE     6587     /* Baseline Dist (design)   */
#define BASELINE     6614     /* Baseline Dist (design) */
#define CELLWIDTH    14813    /* Cell Width (design)        */
#define CELLHEIGHT   8782     /* Cell height (design)      */
//#define LEADING      1756     /* optimal leading (design)*/
#define LEADING      1755     /* optimal leading (design)*/
#define BREAKCHAR    127      /* Break character         */
//#define M8FRSTCHAR   32       /* first char for M8 set   */
//#define FRSTCHAR     0        /* first char for WN set   */
//#define LASTCHAR     255      /* last char for all sets  */
#define PTPIN        72.31    /* pts per inch           */
#define HPPTPIN      72       /* pts per inch, HP style */
#define PFMTYPE      128      /* PFM file type designator*/
  
/* EOF */
