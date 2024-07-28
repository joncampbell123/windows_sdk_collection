/**[f******************************************************************
* $sfpfm.h
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
  
#define ZAPF100     (('1' << 8) | 'D')
#define ZAPF200     (('2' << 8) | 'D')
#define ZAPF300     (('3' << 8) | 'D')
#define ZAPFPS      (('S' << 8) | 'D')
#define DESKTOP     (('T' << 8) | 'D')
#define ASCII       (('S' << 8) | 'U')
#define PIFONT      (('I' << 8) | 'P')
#define LEGAL       (('G' << 8) | 'L')
#define PSMATH      (('S' << 8) | 'M')
#define MSPUB       (('B' << 8) | 'P')
#define PC8         (('C' << 8) | 'P')
#define PC8DN       (('D' << 8) | 'P')
#define PC850       (('M' << 8) | 'P')
#define MATH8       (('8' << 8) | 'M')
#define ROMAN8      (('8' << 8) | 'R')
#define PSTEXT      (('S' << 8) | 'T')
#define VENTMATH    (('M' << 8) | 'V')
#define VENTUS      (('U' << 8) | 'V')
#define ECMA94      (('1' << 8) | 'E')
#define VENTINT     (('I' << 8) | 'V')
#define WINSET      (('N' << 8) | 'W')
  
  
/* FIXME ...some of these should be merged in with driver '.h' files */
#define HDPI    300
#define VDPI    300
  
// Needed Information from TD_MACROS.H
#ifndef PTPIN
#define PTPIN 72.31         /* pts per inch         */
#endif
  
#ifndef INCHPPT
#define INCHPPT 0.01383         /* inches per pt        */
#endif
  
#ifndef HPPTPIN
#define HPPTPIN 72          /* pts per inch, HP style   */
#endif
  
#define PTRES 100           /* resolution of ptsize     */
#define STRES 100
  
/*  Pick the proper resolution for character x and y        */
#define XR  HDPI
#define YR  VDPI
  
#define HPPT2DOT(val,sc,res) (int) ldiv((lmul(lmul((long)(val), (long)(res)), (long)(sc)) + 36L), 72L)
#define XPT2DOT(val,sc) HPPT2DOT(val,sc,XR)
#define YPT2DOT(val,sc) HPPT2DOT(val,sc,YR)
#define FIXEM(x) (short) ldiv(lmul((long)(x),(long)ah->scaleFactor),8782L)
  
#define DPI_TO_DPM(x) (ldiv(lmul((long)(x),3937L) + 50L, 100L))
  
#define SPACE 32
#define CGTIMESUID   92500
#define CGTIMESIID   92501
#define CGTIMESBID   92504
#define CGTIMESBIID  92505
#define CGDINGBATSID  3848
  
#define PAIRS_FILE  "IF.KRN"
#define SYM_FILE    "IF.SS"
  
#define DEVSIZE 32          /* max length of device name    */
#define ESCSIZE 32          /* max length of escape string  */
  
#define POSTURE_POS 17
#define POSTURE_LEN 1
  
#define STROKEWT_POS 19
#define STROKEWT_LEN 2
  
#define SERIF_POS 28
#define SERIF_LEN 2
  
#define FACENAME_POS 35
#define FACENAME_LEN 16    /* length of typeface abbrev. in font alias seg. */
#define FACESIZE     70    /* terminate this name w/ NULL in PFM and add symset*/
  
#define TSTITALIC()     (set->slantStyle==20)
#define TSTFIXEDPITCH() (ah->isFixedPitch!=0)
  
/*      Design Escapement                   */
#define DESESCAPE(i)    (extent_tbl[i])
  
/*  SCALE - Scales an object in device units to the appropriate point size
*
*      a  The point size multiplied by 8
*      b  The object to scale (in design units)
*      c  The resolution in dots per meter (DPM), use DPI_TO_DPM to get DPM
*/
/* #define SCALE(a,b,c) (int)ldiv(ldiv((lmul(lmul(ldiv(lmul((long)a,(long)b),2000L),c),16L) + 8),16L),100000L) */
#define SCALE(a,b,c) (int)ldiv((lmul(ldiv(lmul((long)a,(long)b),2000L),c) + 50000),100000L)
  
  
//Structure Definitions that should have been in CG include files
//They deal with information returned through the CGIFsegments call
  
typedef struct {
    unsigned short      NFACES;
    char                typeFaceName[50];
    char                familyName[20];
    char                weight[20];
    long                typeFaceSet[12];
} typefaceHeaderType;
  
typedef struct {
    UBYTE  stemStyle;
    UBYTE  stemMod;
    UBYTE  stemWeight;
    UBYTE  slantStyle;
    UBYTE  horizStyle;
    UBYTE  vertXHeight;
    UBYTE  videoStyle;
    UBYTE  copyUsage;
} descriptorSetType;
  
/*** Font Alias Table data structure ***/
typedef struct fontAliasTableType {
    char                        aliasTableName[20];
    unsigned short              NXREF;
    char                        xRefArray[100];
    unsigned long               CGtfnum;
} FONTALIASTABLETYPE;
  
typedef struct fontAliasDataType {
    unsigned short              NFATS;
    struct fontAliasTableType   fontAliasTable[1];
} FONTALIASDATATYPE;
  
typedef FONTALIASTABLETYPE FAR * LPFONTALIASTABLETYPE;
typedef FONTALIASDATATYPE FAR * LPFONTALIASDATATYPE;
  
  
// The following structures used in the symbol sets file are also in
// ix.h but this file also requires other include files which contain
// definitions which conflict with ours (redefinitions, etc.).
// We really need to see if we can clean up this entire header file mess!
/*--------------------------------*/
/* Symbol set file directoy entry */
/*--------------------------------*/
/* Bullet 1.20 -  */
typedef struct
{
    UWORD ss_code;    /* symbol set code */
    UWORD symSetCode; /* 32 * PCLnum + (short)PCLchar - 64 */
    UWORD type;
    UWORD class;
    UWORD first_code; /* first code */
    UWORD num_codes;  /* number of codes */
    ULONG offset;     /* offset to symbol set in symbol set file */
  
}  SS_DIRECTORY;
  
/*--------------------------------*/
/*        Symbol set entry        */
/*--------------------------------*/
  
typedef struct
{
    UWORD cgnum;
    UWORD bucket_type;  /* should be a BYTE when I learn packed structures */
} SS_ENTRY;
  
  
  
/*  HP Default Character -- this is used EVERYWHERE, all
*  driver code, all translation tables, all pfm makers.
*/
#define HP_DF_CH    46
  
  
/*  DRIVERINFO version number (i.e., version of the structure).
*/
#define DRIVERINFO_VERSION  1
  
  
#define PTM_MAGIC   0xDAD
#define PTM_VERSION 0x100
  
typedef struct _ptmheader {
    WORD ptmMagic;    /* Used by font installer to recognize PTM file */
    WORD ptmVersion;  /* hi byte = version, lo byte = revision, BCD fmt */
    DWORD ptmSize;    /* size in bytes of entire PTM file */
    WORD ptmNumPFMs;  /* # of PFM's in PTM file */
    DWORD ptmPFMList; /* absolute file offset to first PFM in file */
    DWORD ptmReserved;  /* must be zero (for now) */
} PTMHEADER, FAR * LPPTMHEADER;
  
  
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
  
typedef struct      /* Windows' kerning pair struct */
{
    union {
        BYTE each[2];
        WORD both;
    } kpPair;
    WORD kpKernAmount;
} KERNPAIR, FAR * LPKERNPAIR;
  
typedef struct {
    int npairs;
    HANDLE kph;
    LPKERNPAIR kpptr;
} KERNPAIRS;
  
typedef struct {
    UWORD lnum;   /* Left CG character code value */
    UWORD rnum;   /* Right CG character code value */
} CGPAIR, FAR * LPCGPAIR;
  
typedef struct {
    BYTE ss_char;   /* local symbol set char code value */
    UWORD cgnum;    /* corresponding CG char code value */
} MAPENTRY, FAR * LPMAPENTRY;
  
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
    epsymECMA94
} SYMBOLSET;
  
typedef struct
{
    SYMBOLSET symbolSet;        /* kind of translation table */
    DWORD offset;           /* location of user-defined table */
    WORD len;           /* length (in bytes) of table */
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
  
//typedef struct
//  {
//  WORD epSize;                /* size of this data structure */
//  WORD epVersion;             /* number indicating version of struct */
//  WORD epSsid;                /* Symbol set id for this font */
//     WORD epFontClass;           /* Class for this type of font */
//     char epFamilyName[21];      /* Family name (CG Times, etc.) */
//  } DRIVERINFO;
  
typedef PFMHEADER * PPFMHEADER;
typedef PFMEXTENSION * PPFMEXTENSION;
typedef EXTTEXTMETRIC * PEXTTEXTMETRIC;
typedef KERNPAIR * PKERNPAIR;
typedef DRIVERINFO * PDRIVERINFO;
  
typedef PFMHEADER far * LPPFMHEADER;
typedef PFMEXTENSION far * LPPFMEXTENSION;
typedef EXTTEXTMETRIC far * LPEXTTEXTMETRIC;
typedef KERNPAIR far * LPKERNPAIR;
typedef DRIVERINFO far * LPDRIVERINFO;
  
  
typedef struct {    /* general 'More Font Metrics' structure */
    UWORD spacesize;  /* size of space char  (design)     */
    UWORD baseline;   /* Baseline Distance (design)       */
    UWORD cellwidth;  /* Cell Width (design)          */
    UWORD cellheight; /* Cell height (design)         */
    UWORD extleading; /* optimal leading (design)     */
    UWORD intleading; /* optimal leading (design)     */
    UWORD charspacing;    /* optimal character spacing (design)   */
  
    UWORD emsize;     /* number of units per EM       */
  
    UWORD ptsize;     /* point size (pts * 100) */
    UWORD setsize;    /* set size (pts * 100) */
  
} MFM;
  
typedef MFM * PMFM;
typedef MFM far * LPMFM;
  
  
  
#define BASELINE     6614     /* Baseline Dist (design) */
#define CELLWIDTH    14813    /* Cell Width (design)        */
#define CELLHEIGHT   8782     /* Cell height (design)      */
#define LEADING      1755     /* optimal leading (design)*/
  
  
/*** Added for move family name routine (dtk) 11-12-90 ***/
  
#define MAXNAME      80
  
#define FAMSPECS {"Italic","It","Ob","Dia","Krsv","Rmn","Antiqua","Bd","Hlb",\
"ITALIC", "Bold", "BOLD"}
#define CFAMSPECS {"Bk","Md","Rg"}
  
#define CGFOUND    "CG "
#define ITCFOUND   "ITC "
#define VGCFOUND   "VGC "
#define ITCSUB1    "LSC "
#define ITCSUB2    "L&C "
  
#define FAMILYNAMLEN 20
  
