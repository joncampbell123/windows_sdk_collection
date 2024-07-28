/**[f******************************************************************
* pfm.h -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1989-1990 Microsoft Corporation.
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
  
// history
//  17 july 89  peterbe     Checked in this version with
//                  Deskjet defs ECMA94_CHARSET and
//                  epsymDESKJET8.
  
/*  HP Default Character -- this is used EVERYWHERE, all
*  driver code, all translation tables, all pfm makers.
*/
#define HP_DF_CH    ((BYTE) 0x7F)
  
  
/*  HP Character sets
*/
#define MATH8_CHARSET       180
#define PIFONT_CHARSET      181
#define LINEDRAW_CHARSET    182
#define PCLINE_CHARSET      183
#define TAXLINE_CHARSET     184
#define USLEGAL_CHARSET     185
#define ECMA94_CHARSET      186
  
/*  DRIVERINFO version number (i.e., version of the structure).
*/
#define DRIVERINFO_VERSION  1
  
#define PCM_MAGIC   0xCAC
#define PCM_VERSION 0x310
  
typedef struct _pcmheader {
    WORD pcmMagic;
    WORD pcmVersion;
    DWORD pcmSize;
    DWORD pcmTitle;
    DWORD pcmPFMList;
} PCMHEADER, FAR * LPPCMHEADER;
  
  
typedef struct
{
    WORD    dfVersion;
    DWORD   dfSize;
    char    dfCopyright[60];
    WORD    dfType;
    WORD    dfPoints;
    WORD    dfVertRes;
    WORD    dfHorizRes;
    WORD    dfAscent;
    WORD    dfInternalLeading, dfExternalLeading;
    BYTE    dfItalic, dfUnderline, dfStrikeOut;
    WORD    dfWeight;
    BYTE    dfCharSet;
    WORD    dfPixWidth;
    WORD    dfPixHeight;
    BYTE    dfPitchAndFamily;
    WORD    dfAvgWidth;
    WORD    dfMaxWidth;
    BYTE    dfFirstChar, dfLastChar, dfDefaultChar, dfBreakChar;
    WORD    dfWidthBytes;
    DWORD   dfDevice;
    DWORD   dfFace;
    DWORD   dfBitsPointer;
    DWORD   dfBitsOffset;
    WORD    dfCharOffset[1];    /* size is dfLastChar-dfFirstChar+2 */
} PFMHEADER;
  
typedef struct
{
    WORD    dfSizeFields;
    DWORD   dfExtMetricsOffset;
    DWORD   dfExtentTable;
    DWORD   dfOriginTable;
    DWORD   dfPairKernTable;
    DWORD   dfTrackKernTable;
    DWORD   dfDriverInfo;
    DWORD   dfReserved;
} PFMEXTENSION;
  
typedef struct
{
    short   emSize;
    short   emPointSize;
    short   emOrientation;
    short   emMasterHeight;
    short   emMinScale;
    short   emMaxScale;
    short   emMasterUnits;
    short   emCapHeight;
    short   emXHeight;
    short   emLowerCaseAscent;
    short   emLowerCaseDescent;
    short   emSlant;
    short   emSuperScript;
    short   emSubScript;
    short   emSuperScriptSize;
    short   emSubScriptSize;
    short   emUnderlineOffset;
    short   emUnderlineWidth;
    short   emDoubleUpperUnderlineOffset;
    short   emDoubleLowerUnderlineOffset;
    short   emDoubleUpperUnderlineWidth;
    short   emDoubleLowerUnderlineWidth;
    short   emStrikeOutOffset;
    short   emStrikeOutWidth;
    WORD    emKernPairs;
    WORD    emKernTracks;
} EXTTEXTMETRIC;
  
typedef struct
{
    union {
        BYTE each[2];
        WORD both;
    } kpPair;
    short kpKernAmount;
} KERNPAIR;
  
typedef struct
{
    short ktDegree;
    short ktMinSize;
    short ktMinAmount;
    short ktMaxSize;
    short ktMaxAmount;
} KERNTRACK;
  
typedef enum
{
    epsymUserDefined,
    epsymRoman8,
    epsymKana8,
    epsymMath8,
    epsymUSASCII,
    epsymLineDraw,
    epsymMathSymbols,
    epsymUSLegal,
    epsymRomanExt,
    epsymISO_DenNor,
    epsymISO_UK,
    epsymISO_France,
    epsymISO_German,
    epsymISO_Italy,
    epsymISO_SwedFin,
    epsymISO_Spain,
    epsymGENERIC7,
    epsymGENERIC8,
    epsymECMA94,
    epsymDESKJET8
} SYMBOLSET;
  
typedef struct
{
    SYMBOLSET symbolSet;        /* kind of translation table */
    DWORD offset;               /* location of user-defined table */
    WORD len;                   /* length (in bytes) of table */
    BYTE firstchar, lastchar;   /* table ranges from firstchar to lastchar */
} TRANSTABLE;
  
typedef struct
{
    WORD epSize;                /* size of this data structure */
    WORD epVersion;             /* number indicating version of struct */
    DWORD epMemUsage;           /* amt of memory font takes up in printer */
    DWORD epEscape;             /* pointer to escape that selects the font */
    TRANSTABLE xtbl;            /* character set translation info */
} DRIVERINFO;
  
typedef PFMHEADER far * LPPFMHEADER;
typedef PFMEXTENSION far * LPPFMEXTENSION;
typedef EXTTEXTMETRIC far * LPEXTTEXTMETRIC;
typedef KERNPAIR far * LPKERNPAIR;
typedef DRIVERINFO far * LPDRIVERINFO;
