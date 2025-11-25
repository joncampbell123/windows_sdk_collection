//--------------------------------------------------------------------------
//
// Module Name:  PFM.H
//
// Brief Description:  This module contains the PSCRIPT driver's
// font metrics defines.
//
// Author:  Kent Settle (kentse)
// Created: 22-Jan-1991
//
// Copyright (C) 1991 - 1992 Microsoft Corporation.
//--------------------------------------------------------------------------

#define MAX_STRING      80
#define MAX_KERNPAIRS	1024
#define MAX_CHARS       300

#define ANSI_CHARSET    0
#define SYMBOL_CHARSET  2
#define OEM_CHARSET     255

#define PFM_VERSION     0x00010000

#define INIT_IFI    2048
#define INIT_PFM  131072 + INIT_IFI   // storage to allocate to build NTFM.

#define MIN_UNICODE_VALUE       0
#define MAX_UNICODE_VALUE       0xFFFE
#define INVALID_UNICODE_VALUE   0xFFFF
// The AFM tokens.

#define TK_UNDEFINED            0
#define TK_STARTKERNDATA        2
#define TK_STARTKERNPAIRS       3
#define TK_KPX                  4
#define TK_ENDKERNPAIRS         5
#define TK_ENDKERNDATA          6
#define TK_FONTNAME             7
#define TK_WEIGHT               8
#define TK_ITALICANGLE          9
#define TK_ISFIXEDPITCH         10
#define TK_UNDERLINEPOSITION    11
#define TK_UNDERLINETHICKNESS   12
#define TK_FONTBBOX             13
#define TK_CAPHEIGHT            14
#define TK_XHEIGHT              15
#define TK_DESCENDER            16
#define TK_ASCENDER             17
#define TK_STARTCHARMETRICS     18
#define TK_ENDCHARMETRICS       19
#define TK_ENDFONTMETRICS       20
#define TK_STARTFONTMETRICS     21
#define TK_ENCODINGSCHEME       22
#define TK_FULLNAME             23
#define TK_FAMILYNAME           24
#define TK_MSFAMILY             25

// font defines.

#define ARIAL                               1
#define ARIAL_BOLD                          2
#define ARIAL_BOLDOBLIQUE                   3
#define ARIAL_OBLIQUE                       4
#define ARIAL_NARROW                        5
#define ARIAL_NARROW_BOLD                   6
#define ARIAL_NARROW_BOLDOBLIQUE            7
#define ARIAL_NARROW_OBLIQUE                8
#define AVANTGARDE_BOOK                     9
#define AVANTGARDE_BOOKOBLIQUE              10
#define AVANTGARDE_DEMI                     11
#define AVANTGARDE_DEMIOBLIQUE              12
#define BOOKMAN_DEMI                        13
#define BOOKMAN_DEMIITALIC                  14
#define BOOKMAN_LIGHT                       15
#define BOOKMAN_LIGHTITALIC                 16
#define COURIER                             17
#define COURIER_BOLD                        18
#define COURIER_BOLDOBLIQUE                 19
#define COURIER_OBLIQUE                     20
#define GARAMOND_BOLD                       21
#define GARAMOND_BOLDITALIC                 22
#define GARAMOND_LIGHT                      23
#define GARAMOND_LIGHTITALIC                24
#define HELVETICA                           25
#define HELVETICA_BLACK                     26
#define HELVETICA_BLACKOBLIQUE              27
#define HELVETICA_BOLD                      28
#define HELVETICA_BOLDOBLIQUE               29
#define HELVETICA_CONDENSED                 30
#define HELVETICA_CONDENSED_BOLD            31
#define HELVETICA_CONDENSED_BOLDOBL         32
#define HELVETICA_CONDENSED_OBLIQUE         33
#define HELVETICA_LIGHT                     34
#define HELVETICA_LIGHTOBLIQUE              35
#define HELVETICA_NARROW                    36
#define HELVETICA_NARROW_BOLD               37
#define HELVETICA_NARROW_BOLDOBLIQUE        38
#define HELVETICA_NARROW_OBLIQUE            39
#define HELVETICA_OBLIQUE                   40
#define KORINNA_BOLD                        41
#define KORINNA_KURSIVBOLD                  42
#define KORINNA_KURSIVREGULAR               43
#define KORINNA_REGULAR                     44
#define LUBALINGRAPH_BOOK                   45
#define LUBALINGRAPH_BOOKOBLIQUE            46
#define LUBALINGRAPH_DEMI                   47
#define LUBALINGRAPH_DEMIOBLIQUE            48
#define NEWCENTURYSCHLBK_BOLD               49
#define NEWCENTURYSCHLBK_BOLDITALIC         50
#define NEWCENTURYSCHLBK_ITALIC             51
#define NEWCENTURYSCHLBK_ROMAN              52
#define PALATINO_BOLD                       53
#define PALATINO_BOLDITALIC                 54
#define PALATINO_ITALIC                     55
#define PALATINO_ROMAN                      56
#define SOUVENIR_DEMI                       57
#define SOUVENIR_DEMIITALIC                 58
#define SOUVENIR_LIGHT                      59
#define SOUVENIR_LIGHTITALIC                60
#define SYMBOL                              61
#define TIMES_BOLD                          62
#define TIMES_BOLDITALIC                    63
#define TIMES_ITALIC                        64
#define TIMES_ROMAN                         65
#define TIMES_NEW_ROMAN                     66
#define TIMES_NEW_ROMAN_BOLD                67
#define TIMES_NEW_ROMAN_BOLDITALIC          68
#define TIMES_NEW_ROMAN_ITALIC              69
#define VARITIMES_BOLD                      70
#define VARITIMES_BOLDITALIC                71
#define VARITIMES_ITALIC                    72
#define VARITIMES_ROMAN                     73
#define ZAPFCALLIGRAPHIC_BOLD               74
#define ZAPFCALLIGRAPHIC_BOLDITALIC         75
#define ZAPFCALLIGRAPHIC_ITALIC             76
#define ZAPFCALLIGRAPHIC_ROMAN              77
#define ZAPFCHANCERY_MEDIUMITALIC           78
#define ZAPFDINGBATS                        79

#define FIRST_FONT                          1
#define DEFAULT_FONT                        COURIER
#define NUM_INTERNAL_FONTS                  79

extern PutByte(SHORT);
extern PutWord(SHORT);
extern PutLong(long);

typedef USHORT  SOFFSET;        // short offset.

#define DWORDALIGN(a) ((a + (sizeof(DWORD) - 1)) & ~(sizeof(DWORD) - 1))
#define WCHARALIGN(a) ((a + (sizeof(WCHAR) - 1)) & ~(sizeof(WCHAR) - 1))

typedef struct
{
    CHAR   *pstrAdobeFace;
    CHAR   *pstrWinFace;
} WINFONTPAIR;

typedef struct
{
    SHORT   left;
    SHORT   top;
    SHORT   right;
    SHORT   bottom;
} SRECT;

// entry for each soft font.

typedef struct
{
    ULONG   iFace;                      // identifier for this font.
    PWSTR   pwstrPFMFile;               // pointer to .PFM filename for this font.
    struct _SOFTFONTENTRY *psfePrev;	// pointer to previout entry.
    struct _SOFTFONTENTRY *psfeNext;	// pointer to next SOFTFONTENTRY.
} SOFTFONTENTRY, *PSOFTFONTENTRY;

// NT Font Metrics structure.

typedef struct
{
    ULONG   cjThis;             // size of this struct, with attached data.
    ULONG   ulVersion;          // version of this structure.
    ULONG   cjSoftFont; 	// size of softfont data, if any.
    RECT    rcBBox;             // bounding box of entire font.
    FWORD   fwdLowerCaseAscent;
    LOFFSET loszFullName;       // offset to FullName from top of struct.
    LOFFSET loszFontName;       // offset to FontName.
    LOFFSET loszFamilyName;     // offset to FamilyName.
    ULONG   cKernPairs;
    LOFFSET loKernPairs;        // offset to start of FD_KERNINGPAIR structs.
    ULONG   cCharacters;
    LOFFSET loCharMetrics;      // offset to array of USHORTS then BYTES.
    LOFFSET loSoftFont;         // offset to softfont code, if any, else NULL.
    LOFFSET loIFIMETRICS;       // offset to IFIMETRICS structure.
} NTFM, *PNTFM;

// AFM to PFM compiler data structure.

typedef struct _PARSEDATA
{
    char        rgbBuffer[2048];    // the input buffer.
    int         cbBuffer;           // number bytes in buffer.
    char       *pbBuffer;           // pointer to current location in buffer.
    char        rgbLine[160];       // The current line of text being processed
    char       *szLine;             // Ptr to the current location in the line
    BOOL        fEOF;
    BOOL        fUnGetLine;
    HANDLE      hFile;
    PNTFM       pntfm;
    IFIMETRICS *pTmpIFI;
    int         cbFullName;
    int         cbFontName;
    int         cbFamilyName;
    char        szTmpFullName[MAX_STRING];
    char        szTmpFontName[MAX_STRING];
    char        szTmpFamilyName[MAX_STRING];
    FD_KERNINGPAIR TmpKernPairs[MAX_KERNPAIRS];
    USHORT      TmpCharWidths[MAX_CHARS];
    BYTE        TmpCharCodes[MAX_CHARS];
} PARSEDATA, *PPARSEDATA;
