//--------------------------------------------------------------------------
//
// Module Name:  AFMTOPFM.C
//
// This module of the afm compiler parses the afm file and collects
// information in the PFM structure.  It then passes control to the
// pfm module which outputs the pfm file.
//
// USAGE: AFM <AFM filename> <MS database filename>
//      output is filename.pfm.
//
// Author:  Kent Settle (kentse)
// Created: 18-Mar-1991
//
// Copyright (c) 1988 - 1993 Microsoft Corporation
//--------------------------------------------------------------------------

#include <string.h>
#include "pscript.h"
#include "mapping.h"
#include "tables.h"

int     _fltused;   // HEY, it shut's up the linker.  That's why it's here.

#define U_SPACE         0x20
#define U_SYMBOL_BULLET 0x00B7
#define U_BULLET        0x2022

// Alias Family Tables.

static char *TimesAlias[] = {"Times", "Tms Rmn", "Times Roman", "TimesRoman",
                             "TmsRmn", "Varitimes", "Dutch",
                             "Times New Roman", "TimesNewRomanPS",
                             NULL };

static char *HelveticaAlias[] = {"Helvetica", "Helv", "Arial", "Swiss", NULL};

static char *CourierAlias[] = {"Courier", "Courier New", NULL};

static char *HelveticaNarrowAlias[] = {"Helvetica-Narrow", "Helvetica Narrow",
                                       "Arial-Narrow", "Arial Narrow", NULL};

static char *PalatinoAlias[] = {"Palatino", "Zapf Calligraphic",
                                "Bookman Antiqua", "Book Antiqua",
                                "ZapfCalligraphic", NULL};

static char *BookmanAlias[] = {"ITC Bookman", "Bookman Old Style", "Bookman",
                               NULL};

static char *NewCenturySBAlias[] = {"NewCenturySchlbk", "New Century Schoolbook",
                                    "Century Schoolbook", "NewCenturySchoolBook",
                                    "New Century SchoolBook", "CenturySchoolBook",
                                    NULL};

static char *AvantGardeAlias[] = {"AvantGarde", "ITC Avant Garde Gothic",
                                  "Century Gothic", "ITC Avant Garde", NULL};

static char *ZapfChanceryAlias[] = {"ZapfChancery", "ITC Zapf Chancery",
                                    "Monotype Corsiva", NULL};

static char *ZapfDingbatsAlias[] = {"ZapfDingbats", "ITC Zapf Dingbats",
                                    "Monotype Sorts", "Zapf Dingbats", NULL};

static USHORT ausIFIMetrics2WinWeight[12] =
{
    FW_DONTCARE,
    FW_DONTCARE,
    FW_LIGHT,
    FW_LIGHT,
    FW_NORMAL,
    FW_NORMAL,
    FW_NORMAL,
    FW_BOLD,
    FW_BOLD,
    FW_BOLD,
    FW_BOLD,
    FW_BOLD
};

//#define   ALL_METRICS

// declarations of routines defined within this module.

VOID PrintLine(char *);
VOID ParseKernPairs(PPARSEDATA);
VOID ParseKernData(PPARSEDATA);
VOID ParseFontName(PPARSEDATA);
VOID ParseFullName(PPARSEDATA);
VOID ParseFamilyName(PPARSEDATA);
VOID ParseWeight(PPARSEDATA);
BOOL ParseCharMetrics(PPARSEDATA);
int ParseCharName(PPARSEDATA);
int ParseCharWidth(PPARSEDATA);
VOID ParseBoundingBox(PPARSEDATA);
VOID InitPfm(PPARSEDATA);
VOID SetWidths(PPARSEDATA);
BOOL WritePFM(PWSTR, PPARSEDATA);
VOID ParseAfm(PPARSEDATA);
int ParseCharCode(PPARSEDATA);
int GetCharCode(char *, PPARSEDATA);
BOOL GetFirstLastChar(PPARSEDATA);
VOID BuildNTFM(PPARSEDATA, PSTR);
VOID InitIFIMETRICS(PPARSEDATA);
VOID CompleteIFIMETRICS(PPARSEDATA);
VOID ParseMSFamilyName(PPARSEDATA);
VOID ParsePitch(PPARSEDATA);
VOID GetFamilyAliases(IFIMETRICS *, PSTR);

// external declarations.

extern BOOL GetLine(PPARSEDATA);
extern int GetFloat(int, PPARSEDATA);
extern int GetNumber(PPARSEDATA);
extern VOID UnGetLine(PPARSEDATA);
extern VOID EatWhite(PPARSEDATA);
extern VOID GetWord(char *, int, PPARSEDATA);
extern int MapToken(char *, TABLE_ENTRY *);
extern int GetKeyword(TABLE_ENTRY *, PPARSEDATA);

//--------------------------------------------------------------------------
//
// VOID InitPfm(pdata)
// PPARSEDATA   pdata;
//
// Initialize the NTFM structure.
//
// Returns:
//   This routine returns no value.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from PM, and cleaned up.
//--------------------------------------------------------------------------

VOID InitPfm(pdata)
PPARSEDATA  pdata;
{
    PNTFM   pntfm;

    pntfm = pdata->pntfm;

    pntfm->cjThis = DWORDALIGN(sizeof(NTFM));
    pntfm->ulVersion = (ULONG)PFM_VERSION;
    pntfm->fwdLowerCaseAscent = 0;
    pntfm->loszFullName = 0;
    pntfm->loszFontName = 0;
    pntfm->loszFamilyName = 0;
    pntfm->cKernPairs = 0;
    pntfm->cCharacters = 0;
    pntfm->cjSoftFont = 0;
    pntfm->loSoftFont = 0;
    pntfm->loIFIMETRICS = 0;
}


//--------------------------------------------------------------------------
// VOID ParseAfm(pdata)
// PPARSEDATA  pdata;
//
// Parses most of the AFM file to fill in the NTFM structure.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns no value.
//
// History:
//   19-Mar-1991    -by-    Kent Settle    (kentse)
//  Ripped out of main.
//--------------------------------------------------------------------------

VOID ParseAfm(pdata)
PPARSEDATA  pdata;
{
    int         iToken;
    BOOL        fEndOfInput;
    PNTFM       pntfm;
    PIFIMETRICS pTmpIFI;
    FLOAT       eCharSlope;

    // get local pointers.

    pntfm = pdata->pntfm;
    pTmpIFI = pdata->pTmpIFI;

    // fill in some default values.

    InitIFIMETRICS(pdata);

    fEndOfInput = FALSE;
    while (!fEndOfInput)
    {
        GetLine(pdata);
        iToken = GetKeyword(AFMKeywordTable, pdata);

        switch(iToken)
        {
            case TK_STARTFONTMETRICS:
                break;

            case TK_STARTKERNDATA:
                ParseKernData(pdata);
                break;

            case TK_FONTNAME:
                ParseFontName(pdata);
                break;

            case TK_FULLNAME:
                ParseFullName(pdata);
                break;

            case TK_FAMILYNAME:
                ParseFamilyName(pdata);
                break;

            case TK_WEIGHT:
                ParseWeight(pdata);

                pTmpIFI->usWinWeight = ausIFIMetrics2WinWeight[pTmpIFI->panose.bWeight];

                if (pTmpIFI->usWinWeight > FW_NORMAL)
                    pTmpIFI->fsSelection |= FM_SEL_BOLD;

                break;

            case TK_ITALICANGLE:
                eCharSlope = (FLOAT)(GetFloat(10, pdata) / 10);

                if ((LONG)(eCharSlope * 1000) == 0)
                {
                    pTmpIFI->ptlCaret.x = 0;
                    pTmpIFI->ptlCaret.y = 1;
                }
                else
                {
                    pTmpIFI->fsSelection |= FM_SEL_ITALIC;

                    // lean 17.5 degrees away from the y-axis.

                    pTmpIFI->ptlCaret.x = 3153;
                    pTmpIFI->ptlCaret.y = 10000;
                }

                break;

            case TK_MSFAMILY:
                ParseMSFamilyName(pdata);
                break;

            case TK_ISFIXEDPITCH:
                ParsePitch(pdata);

                break;

            case TK_ENCODINGSCHEME:
                break;

            case TK_UNDERLINEPOSITION:
                pTmpIFI->fwdUnderscorePosition = (FWORD) GetNumber(pdata);
#ifdef ALL_METRICS
                DbgPrint("fwdUnderscorePosition = %d\n",
                         pTmpIFI->fwdUnderscorePosition);
#endif
                break;

            case TK_UNDERLINETHICKNESS:
                pTmpIFI->fwdUnderscoreSize = (FWORD)GetNumber(pdata);
                pTmpIFI->fwdStrikeoutSize = pTmpIFI->fwdUnderscoreSize;
#ifdef ALL_METRICS
                DbgPrint("fwdUnderscoreSize = %d\n",
                         pTmpIFI->fwdUnderscoreSize);
#endif
                break;

            case TK_FONTBBOX:
                ParseBoundingBox(pdata);
                break;

            case TK_CAPHEIGHT:
                break;

            case TK_XHEIGHT:
                pTmpIFI->fwdXHeight = (FWORD)GetNumber(pdata);
#ifdef ALL_METRICS
                DbgPrint("fwdXHeight = %d\n", pTmpIFI->fwdXHeight);
#endif
                break;

            case TK_ASCENDER:
                pntfm->fwdLowerCaseAscent = (FWORD)GetNumber(pdata);
#ifdef ALL_METRICS
                DbgPrint("fwdLowerCaseAscent = %d\n",
                         pntfm->fwdLowerCaseAscent);
#endif
                break;

            case TK_STARTCHARMETRICS:
                ParseCharMetrics(pdata);
                break;

            case TK_ENDFONTMETRICS:
                fEndOfInput = TRUE;
                break;

        }

        pdata->szLine = pdata->rgbLine;
    }
}


//--------------------------------------------------------------------------
//
// VOID  SetWidths();
//
// This routine computes the maximum and average character widths from the
// character metrics in the NTFM structure.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns no value.
//
// History:
//   01-Apr-1991    -by-    Kent Settle    (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

VOID SetWidths(pdata)
PPARSEDATA  pdata;
{
    USHORT  i;
    int     iAvg;
    int     iMax;

    iAvg = 0;
    iMax = 0;

    // calculate maximum and average character widths.

    for (i = 0; i < pdata->pntfm->cCharacters; i++)
    {
        iAvg += (int)pdata->TmpCharWidths[i];
        if (iMax < (int)pdata->TmpCharWidths[i])
            iMax = (int)pdata->TmpCharWidths[i];
    }

    pdata->pTmpIFI->fwdAveCharWidth = (FWORD)(iAvg / pdata->pntfm->cCharacters);

    pdata->pTmpIFI->fwdMaxCharInc = (FWORD)iMax;

#ifdef ALL_METRICS
    DbgPrint("fwdAveCharWidth = %d\n", pdata->pTmpIFI->fwdAveCharWidth);
    DbgPrint("fwdMaxCharInc = %d\n", pdata->pTmpIFI->fwdMaxCharInc);
#endif
}


//--------------------------------------------------------------------------
//
// VOID ParseKernPairs();
//
// Parse the pairwise kerning data.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns no value.
//
// History:
//   13-Mar-1992    -by-    Kent Settle     (kentse)
//  Modified to use FD_KERNINGPAIR struct instead of KP.
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from PM, and cleaned up.
//--------------------------------------------------------------------------

VOID ParseKernPairs(pdata)
PPARSEDATA  pdata;
{
    int i;
    int iCh1;
    int iCh2;
    int iKernAmount;
    int iToken;
    int cPairs;
    char szWord[80];
    WCHAR   wc1, wc2;
    UCMap  *pmap;
    UCMap  *pmapReset;
    BOOL    bFound;
    cPairs = GetNumber(pdata);

    pdata->pntfm->cKernPairs = (USHORT)cPairs;

    // point to the appropriate mapping table in mapping.h.

    if (!strcmp(pdata->szTmpFontName, "Symbol"))
        pmap = SymbolMap;
    else if (!strcmp(pdata->szTmpFontName, "ZapfDingbats"))
        pmap = DingbatsMap;
    else
        pmap = LatinMap;

    pmapReset = pmap;

    for (i = 0; i < cPairs; ++i)
    {
        if (GetLine(pdata))
            break;
        if (GetKeyword(AFMKeywordTable, pdata) != TK_KPX)
        {
            UnGetLine(pdata);
            break;
        }

        GetWord(szWord, sizeof(szWord), pdata);
#ifdef ALL_METRICS
        DbgPrint("Char1 = %s ", szWord);
#endif
        iCh1 = GetCharCode(szWord, pdata);
        GetWord(szWord, sizeof(szWord), pdata);
#ifdef ALL_METRICS
        DbgPrint("Char2 = %s ", szWord);
#endif
        iCh2 = GetCharCode(szWord, pdata);

        iKernAmount = GetNumber(pdata);

#ifdef ALL_METRICS
        DbgPrint("Amount = %d\n", iKernAmount);
#endif

        iCh1 = iCh1 & 0x0FF;    // 0 <= CharacerCode <= 255.
        iCh2 = iCh2 & 0x0FF;

        // we now have the postscript character code for each character
        // of the kerning pair.  we now need to convert them to UNICODE.

        pmap = pmapReset;

        bFound = FALSE;

        while (pmap->szChar)
        {
            if ((USHORT)(pmap->usPSValue & 0x7FFF) == (USHORT)iCh1)
            {
                wc1 = pmap->usUCValue;
                bFound = TRUE;
                break;
            }
            pmap++;
        }

        if (!bFound)
            DbgPrint("GetPairs: Error char code %d not found.\n", iCh1);

        pmap = pmapReset;

        bFound = FALSE;

        while (pmap->szChar)
        {
            if ((USHORT)(pmap->usPSValue & 0x7FFF) == (USHORT)iCh2)
            {
                wc2 = pmap->usUCValue;
                bFound = TRUE;
                break;
            }
            pmap++;
        }

#ifdef ALL_METRICS
        if (!bFound)
            DbgPrint("GetPairs: Error char code %d not found.\n", iCh2);
#endif
        pdata->TmpKernPairs[i].wcFirst = wc1;
        pdata->TmpKernPairs[i].wcSecond = wc2;
        pdata->TmpKernPairs[i].fwdKern = (FWORD)iKernAmount;
    }

    GetLine(pdata);
    iToken = GetKeyword(AFMKeywordTable, pdata);

#if DBG
    if (pdata->fEOF)
        RIP("GetPairs: Premature end of file encountered\n");
    else if (iToken != TK_ENDKERNPAIRS)
    {
        DbgPrint("GetPairs: expected EndKernPairs\n");
        DbgPrint("%s\n", pdata->rgbLine);
    }
#endif

    return;
}


//--------------------------------------------------------------------------
// VOID ParseKernData(pdata)
// PPARSEDATA  pdata;
//
// Start processing the pairwise kerning data.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns no value.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from PM, and cleaned up.
//--------------------------------------------------------------------------

VOID ParseKernData(pdata)
PPARSEDATA  pdata;
{
    if (!GetLine(pdata))
    {
        if (GetKeyword(AFMKeywordTable, pdata) == TK_STARTKERNPAIRS)
            ParseKernPairs(pdata);
        else
	    RIP("PSCRPTUI!ParseKernData: expected StartKernPairs\n");
    }
    else
	RIP("PSCRPTUI!ParseKernData: unexpected end of file\n");
}


//--------------------------------------------------------------------------
// VOID ParseFontName(pdata)
// PPARSEDATA  pdata;
//
// Move the font name from the input buffer into the pfm structure.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns no value.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from PM, and cleaned up.
//--------------------------------------------------------------------------

VOID ParseFontName(pdata)
PPARSEDATA  pdata;
{
    EatWhite(pdata);

    // get the length of the font name.

    pdata->cbFontName = strlen((char *)(pdata->szLine)) + 1;

    // now copy the facename to temporary storage.

    memcpy(pdata->szTmpFontName, pdata->szLine, pdata->cbFontName);

#ifdef ALL_METRICS
    DbgPrint("FontName = %s.\n", pdata->szLine);
#endif
}


//--------------------------------------------------------------------------
// VOID ParseFullName(pdata)
// PPARSEDATA  pdata;
//
// Move the full name from the input buffer into the pfm structure.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns no value.
//
// History:
//   29-Mar-1991    -by-    Kent Settle    (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

VOID ParseFullName(pdata)
PPARSEDATA  pdata;
{
    EatWhite(pdata);

    // get the length of the full name.

    pdata->cbFullName = strlen((char *)(pdata->szLine)) + 1;

    // now copy the Uniquename to temporary storage.

    memcpy(pdata->szTmpFullName, pdata->szLine, pdata->cbFullName);

#ifdef ALL_METRICS
    DbgPrint("FullName = %s.\n", pdata->szLine);
#endif
}


//--------------------------------------------------------------------------
//
// VOID ParseFamilyName();
//
// Move the family name from the input buffer into the pfm structure.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns no value.
//
// History:
//   29-Mar-1991    -by-    Kent Settle    (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

VOID ParseFamilyName(pdata)
PPARSEDATA  pdata;
{
    EatWhite(pdata);

    // get the length of the family name.

    pdata->cbFamilyName = strlen((char *)(pdata->szLine)) + 1;

    // now copy the familyname to temporary storage.

    memcpy(pdata->szTmpFamilyName, pdata->szLine, pdata->cbFamilyName);

#ifdef ALL_METRICS
    DbgPrint("FamilyNameName = %s.\n", pdata->szLine);
#endif
}


//--------------------------------------------------------------------------
// VOID ParseMSFamilyName(pdata)
// PPARSEDATA  pdata;
//
// gets the MSFamily name, and converts it to a value.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns no value.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from PM, and cleaned up.
//--------------------------------------------------------------------------

VOID ParseMSFamilyName(pdata)
PPARSEDATA  pdata;
{
    char szWord[80];
    BOOL bFound = FALSE;
    TABLE_ENTRY  *pTable;

    // clear out the MSFamily flags.

    pdata->pTmpIFI->jWinPitchAndFamily &= ~(FF_DECORATIVE | FF_DONTCARE |
                                            FF_MODERN | FF_ROMAN | FF_SCRIPT |
                                            FF_SWISS);
    GetWord(szWord, sizeof(szWord), pdata);

#ifdef ALL_METRICS
    DbgPrint("MSFamily = %s\n", szWord);
#endif

    pTable = MSFamilyTable;
    while (pTable->szStr)
    {
        if (!strcmp(szWord, pTable->szStr))
        {
            pdata->pTmpIFI->jWinPitchAndFamily |= pTable->iValue;
            bFound = TRUE;
            break;
        }

        ++pTable;
    }

    // set flag, if not found.

    if (!bFound)
    {
#if DBG
#ifdef ALL_METRICS
        DbgPrint("ParseMSFamilyName: unknown MSFamily = \"%s\"\n", szWord);
#endif
#endif
        pdata->pTmpIFI->jWinPitchAndFamily |= FF_DONTCARE;
    }
}


//--------------------------------------------------------------------------
// VOID ParseMSFamilyName(pdata)
// PPARSEDATA  pdata;
//
// gets the MSFamily name, and converts it to a value.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns no value.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from PM, and cleaned up.
//--------------------------------------------------------------------------

VOID ParsePitch(pdata)
PPARSEDATA  pdata;
{
    CHAR    szWord[6];

    GetWord(szWord, sizeof(szWord), pdata);

    pdata->pTmpIFI->jWinPitchAndFamily &= ~(FIXED_PITCH | VARIABLE_PITCH);

    if ((!(strncmp(szWord, "True", 4))) ||
        (!(strncmp(szWord, "true", 4))))
    {
        pdata->pTmpIFI->jWinPitchAndFamily |= FIXED_PITCH;
#ifdef ALL_METRICS
        DbgPrint("Font is fixed pitch.\n");
#endif
    }
    else
    {
        pdata->pTmpIFI->jWinPitchAndFamily |= VARIABLE_PITCH;
#ifdef ALL_METRICS
        DbgPrint("Font is variable pitch.\n");
#endif
    }
}

//--------------------------------------------------------------------------
// VOID ParseWeight(pdata)
// PPARSEDATA  pdata;
//
// Parse the fonts weight and set the corresponding entry in the PFM
// structure.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns no value.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from PM, and cleaned up.
//--------------------------------------------------------------------------

VOID ParseWeight(pdata)
PPARSEDATA  pdata;
{
    char szWord[80];
    BOOL bFound = FALSE;
    TABLE_ENTRY  *pTable;

    GetWord(szWord, sizeof(szWord), pdata);

#ifdef ALL_METRICS
    DbgPrint("Weight = %s\n", szWord);
#endif

    pTable = WeightTable;
    while (pTable->szStr)
    {
#ifdef ALL_METRICS
        DbgPrint("\tweight = %s\n", pTable->szStr);
#endif
	if (!strcmp(szWord, pTable->szStr))
        {
            pdata->pTmpIFI->panose.bWeight = pTable->iValue;
            bFound = TRUE;
            break;
        }

        ++pTable;
    }

    // check for and flag error conditions.

    if (!bFound)
    {
#if DBG
#ifdef ALL_METRICS
        DbgPrint("ParseWeight: unknown font weight = \"%s\"\n", szWord);
#endif
#endif
        pdata->pTmpIFI->panose.bWeight = PAN_WEIGHT_MEDIUM;
    }
}


//--------------------------------------------------------------------------
// BOOL ParseCharMetrics(pdata)
// PPARSEDATA  pdata;
//
// Parse the character metrics entry in the input file and set the
// width and bounding box in the PFM structure.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns no value.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from PM, and cleaned up.
//--------------------------------------------------------------------------

BOOL ParseCharMetrics(pdata)
PPARSEDATA  pdata;
{
    int     cChars;
    USHORT  usCount = 0;
    int     i;
    int     iWidth;
    int     iChar;

    cChars = GetNumber(pdata);
    for (i = 0; i < cChars; ++i)
    {
        if (GetLine(pdata))
        {
	    RIP("PSCRPTUI!ParseCharMetrics: unexpected end of file encountered\n");
            return(FALSE);
        }

        iChar = ParseCharCode(pdata);

        iWidth = ParseCharWidth(pdata);

        iChar = ParseCharName(pdata);

        if (iChar >= 0)
        {
            pdata->TmpCharWidths[usCount] = (USHORT)iWidth;
            pdata->TmpCharCodes[usCount] = (BYTE)iChar;

            usCount++;   // increment character counter.

#ifdef ALL_METRICS
            DbgPrint("iChar = %x, CharWidth = %d\n",
                     pdata->TmpCharCodes[usCount - 1],
                     pdata->TmpCharWidths[usCount - 1]);
#endif
        }
    }

    pdata->pntfm->cCharacters = usCount;
    GetLine(pdata);
    if (GetKeyword(AFMKeywordTable, pdata) != TK_ENDCHARMETRICS)
    {
	RIP("PSCRPTUI!ParseCharMetrics: expected EndCharMetrics\n");
	SetLastError(ERROR_INVALID_DATA);
        return(FALSE);
    }

    return(TRUE);
}


//--------------------------------------------------------------------------
// int ParseCharName(pdata)
// PPARSEDATA  pdata;
//
// Parse a character's name and return its numeric value.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns numeric value of given character.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from PM, and cleaned up.
//--------------------------------------------------------------------------

int ParseCharName(pdata)
PPARSEDATA  pdata;
{
    int iChar;
    char szWord[18];

    EatWhite(pdata);
    GetWord(szWord, sizeof(szWord), pdata);

    if (!strcmp("N", szWord))
    {
        GetWord(szWord, sizeof(szWord), pdata);
#ifdef ALL_METRICS
        DbgPrint("Char = %s ", szWord);
#endif
        iChar = GetCharCode(szWord, pdata);
    }
    else
    {
	RIP("PSCRPTUI!ParseCharName: expected name field\n");
	SetLastError(ERROR_INVALID_DATA);
        return(-1);
    }

    return(iChar);
}


//--------------------------------------------------------------------------
// int ParseCharWidth(pdata)
// PPARSEDATA  pdata;
//
// Parse a character's width and return its numeric value.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns numeric value of given character's width.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from PM, and cleaned up.
//--------------------------------------------------------------------------

int ParseCharWidth(pdata)
PPARSEDATA  pdata;
{
    int iWidth;
    char szWord[16];

    GetWord(szWord, sizeof(szWord), pdata);
    if (!strcmp("WX", szWord))
    {
        iWidth = GetNumber(pdata);
        if (iWidth==0)
        {
	    RIP("PSCRPTUI!ParseCharWidth: zero character width\n");
	    SetLastError(ERROR_INVALID_DATA);
            return(-1);
        }

        EatWhite(pdata);
        if (*(pdata->szLine++) != ';')
        {
	    RIP("PSCRPTUI!ParseCharWidth: missing semicolon\n");
	    SetLastError(ERROR_INVALID_DATA);
            return(-1);
        }
    }
    else
    {
	RIP("PSCRPTUI!ParseCharWidth: expected \"WX\"\n");
	SetLastError(ERROR_INVALID_DATA);
        return(-1);
    }

    return(iWidth);
}


//--------------------------------------------------------------------------
// int ParseCharCode(pdata)
// PPARSEDATA  pdata;
//
// Parse the ASCII form of a character's code point and return its
// numeric value.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns numeric value of given character's codepoint.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from PM, and cleaned up.
//--------------------------------------------------------------------------

int ParseCharCode(pdata)
PPARSEDATA  pdata;
{
    int iChar;
    char szWord[16];

    iChar = 0;
    GetWord(szWord, sizeof(szWord), pdata);
    if (!strcmp("C", szWord))
    {
        iChar = GetNumber(pdata);
        if (iChar==0)
        {
	    RIP("PSCRPTUI!ParseCharCode: invalid character code\n");
	    SetLastError(ERROR_INVALID_DATA);
            return(-1);
        }

        EatWhite(pdata);
        if (*(pdata->szLine++) != ';')
        {
	    RIP("PSCRPTUI!ParseCharCode: missing semicolon\n");
	    SetLastError(ERROR_INVALID_DATA);
            return(-1);
        }
    }
    return(iChar);
}


//--------------------------------------------------------------------------
// VOID ParseBoundingBox(pdata)
// PPARSEDATA  pdata;
//
// Parses a characters's bounding box and stores its size in
// the NTFM structure.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns no value.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from PM, and cleaned up.
//--------------------------------------------------------------------------

VOID ParseBoundingBox(pdata)
PPARSEDATA  pdata;
{
    pdata->pntfm->rcBBox.left = GetNumber(pdata);
    pdata->pntfm->rcBBox.bottom = GetNumber(pdata);
    pdata->pntfm->rcBBox.right = GetNumber(pdata);
    pdata->pntfm->rcBBox.top = GetNumber(pdata);
}



//--------------------------------------------------------------------------
// BOOL WritePFM(pwstrPFMFile, pdata)
// PWSTR       pwstrPFMFile;
// PPARSEDATA  pdata;
//
// Flush the ouput buffer to the file.    Note that this function is only
// called after the entire pfm structure has been built in the output buffer.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns TRUE if success, FALSE otherwise.
//
// History:
//   20-Mar-1991    -by-    Kent Settle    (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

BOOL WritePFM(pwstrPFMFile, pdata)
PWSTR       pwstrPFMFile;
PPARSEDATA  pdata;
{
    HANDLE  hPFMFile;
    ULONG   ulCount;

    // create the .PFM file.

    hPFMFile = CreateFile(pwstrPFMFile, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                          CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hPFMFile == INVALID_HANDLE_VALUE)
    {
	RIP("PSCRPTUI!WritePFM: Can't open .PFM file.\n");
	return(FALSE);
    }

    // write to the .PFM file, then close it.

    if (!WriteFile(hPFMFile, (LPVOID)pdata->pntfm, (DWORD)pdata->pntfm->cjThis,
	      (LPDWORD)&ulCount, (LPOVERLAPPED)NULL))
    {
	RIP("PSCRPTUI!WritePFM: WriteFile to .PFM file failed.\n");
	return(FALSE);
    }

    if (ulCount != pdata->pntfm->cjThis)
    {
	RIP("PSCRPTUI!WritePFM: WriteFile count to .PFM file failed.\n");
	return(FALSE);
    }

    if (!CloseHandle(hPFMFile))
	RIP("PSCRPTUI!WritePFM: CloseHandle of .PFM file failed.\n");

    return(TRUE);
}


//--------------------------------------------------------------------------
//
// int GetCharCode(szWord, pdata)
// char       *szWord;
// PPARSEDATA  pdata;
//
// This routine is given a pointer to the character string.  It returns
// the character code, as defined in mapping.h, corresponding to this
// character.
//
// Parameters:
//   szWord
//     Pointer to character name string.
//
// Returns:
//   This routine returns -1 if character not found, otherwise it
//   returns the character code.
//
// History:
//   27-Mar-1991    -by-    Kent Settle    (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

int GetCharCode(szWord, pdata)
char       *szWord;
PPARSEDATA  pdata;
{
    UCMap  *pmap;

    //!!! maybe there is a better way to do this???

    // point to the appropriate mapping table in mapping.h.

    if (!strcmp(pdata->szTmpFontName, "Symbol"))
        pmap = SymbolMap;
    else if (!strcmp(pdata->szTmpFontName, "ZapfDingbats"))
        pmap = DingbatsMap;
    else
        pmap = LatinMap;

    while (pmap->szChar)
    {
	if (!strcmp(szWord, pmap->szChar))
            return((int)(pmap->usPSValue & 0x7FFF));
        pmap++;
    }

#ifdef ALL_METRICS
    DbgPrint("GetCharCode: Undefined Character = %s.\n", szWord);
#endif
    return(-1);
}


//--------------------------------------------------------------------------
//
// BOOL GetFirstLastChar()
//
// This routine searches through the encoding table in mapping.h to
// determine the first and last characters in the font.  The character
// codes are stored into the NTFM structure in UNICODE value.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns TRUE for success, FALSE otherwise.
//
// History:
//   18-Apr-1991    -by-    Kent Settle    (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

BOOL GetFirstLastChar(pdata)
PPARSEDATA  pdata;
{
    PUCMap  pmap;
    PUCMap  pmapReset;
    USHORT  usFirst = MAX_UNICODE_VALUE;
    USHORT  usLast = MIN_UNICODE_VALUE;
    USHORT  i;
    USHORT  usTmp;

    //!!! maybe there is a better way to do this???

    // point to the appropriate mapping table in mapping.h.

    if (!strcmp(pdata->szTmpFontName, "Symbol"))
        pmap = SymbolMap;
    else if (!strcmp(pdata->szTmpFontName, "ZapfDingbats"))
        pmap = DingbatsMap;
    else
        pmap = LatinMap;

    pmapReset = pmap;

    // for every character in the font, do a lookup in the mapping
    // table in mapping.h to find the corresponding UNICODE value.

    for (i = 0; i < pdata->pntfm->cCharacters; i++)
    {
        while (pmap->szChar)
        {
            if ((BYTE)pdata->TmpCharCodes[i] == (BYTE)(pmap->usPSValue & 0xFF))
            {
                usTmp = pmap->usUCValue;
                if (usFirst > usTmp)
                    usFirst = usTmp;
                if (usLast < usTmp)
                    usLast = usTmp;
                break;
            }

            pmap++;
        }

        // reset pointer.

        pmap = pmapReset;
    }

    // make sure we have valid numbers.

    if (usFirst > usLast)
    {
	RIP("PSCRPTUI!etFirstLastChar: first char > last.\n");
	SetLastError(ERROR_INVALID_DATA);
	return(FALSE);
    }

    // everything must be OK.

    pdata->pTmpIFI->wcFirstChar = (WCHAR)usFirst;
    pdata->pTmpIFI->wcLastChar = (WCHAR)usLast;

#ifdef ALL_METRICS
    DbgPrint("FirstChar = %x, LastChar = %x.\n", usFirst, usLast);
#endif

    return(TRUE);
}


//--------------------------------------------------------------------------
//
// VOID BuildNTFM(pdata, pPFA)
// PPARSEDATA  pdata;
// CHAR       *pPFA;
//
// This routine builds an NTFM structure from all the temporary values
// which have been determined by now.
//
// Returns:
//   This routine returns no value.
//
// History:
//   18-Apr-1991    -by-    Kent Settle    (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

VOID BuildNTFM(pdata, pPFA)
PPARSEDATA  pdata;
CHAR       *pPFA;
{
    PNTFM       pntfm;
    int         cbKernPairs, cbWidths;
    DWORD       cbPFA;
    PSTR        pstr;

    // get local pointer.

    pntfm = pdata->pntfm;

    // deal with the softfont if there is one.

    if (pPFA)
    {
        // the first DWORD of the .PFA file contains the length in BYTES of the
        // remainder of the .PFA file.

        cbPFA = *(DWORD *)pPFA;
        pntfm->cjSoftFont = DWORDALIGN(cbPFA);
    }

    // there are several bits of data which have now been put into temporary
    // storage, but need to be attached to the NTFM structure.

    cbKernPairs = (pntfm->cKernPairs * sizeof(FD_KERNINGPAIR));
    cbWidths = DWORDALIGN(pntfm->cCharacters * sizeof(USHORT));
    pntfm->cjThis += DWORDALIGN(pdata->cbFullName) + DWORDALIGN(pdata->cbFontName) +
                     DWORDALIGN(pdata->cbFamilyName) + DWORDALIGN(cbKernPairs) +
                     cbWidths + DWORDALIGN(pntfm->cCharacters) +
                     DWORDALIGN(pntfm->cjSoftFont);

    if (pntfm->cjThis > INIT_PFM)
    {
#if DBG
        DbgPrint("afm: PFM size greater than allocated.\n");
#endif
        return;
    }

    pntfm->loszFullName = DWORDALIGN(sizeof(NTFM));
    pntfm->loszFontName = pntfm->loszFullName + DWORDALIGN(pdata->cbFullName);
    pntfm->loszFamilyName = pntfm->loszFontName + DWORDALIGN(pdata->cbFontName);
    pntfm->loKernPairs = pntfm->loszFamilyName +
                         DWORDALIGN(pdata->cbFamilyName);
    pntfm->loCharMetrics = pntfm->loKernPairs + DWORDALIGN(cbKernPairs);

    if (pPFA)
        pntfm->loSoftFont = pntfm->loCharMetrics +
                            DWORDALIGN(pntfm->cCharacters) + cbWidths;

    pntfm->loIFIMETRICS = pntfm->loCharMetrics +
                          DWORDALIGN(pntfm->cCharacters) + cbWidths +
                          DWORDALIGN(pntfm->cjSoftFont);

    pstr = (char *)pntfm + pntfm->loszFullName;
    memcpy(pstr, pdata->szTmpFullName, pdata->cbFullName);
    pstr += DWORDALIGN(pdata->cbFullName);
    memcpy(pstr, pdata->szTmpFontName, pdata->cbFontName);
    pstr += DWORDALIGN(pdata->cbFontName);
    memcpy(pstr, (char *)pdata->szTmpFamilyName, pdata->cbFamilyName);
    pstr += DWORDALIGN(pdata->cbFamilyName);
    memcpy(pstr, (char *)pdata->TmpKernPairs, cbKernPairs);
    pstr += DWORDALIGN(cbKernPairs);
    memcpy(pstr, (char *)pdata->TmpCharWidths, cbWidths);
    pstr += cbWidths;
    memcpy(pstr, (char *)pdata->TmpCharCodes, pntfm->cCharacters);
    pstr += DWORDALIGN(pntfm->cCharacters);

#ifdef ALL_METRICS
    DbgPrint("FullName = %s\n", (char *)pntfm + pntfm->loszFullName);
    DbgPrint("FontName = %s\n", (char *)pntfm + pntfm->loszFontName);
    DbgPrint("FamilyName = %s\n", (char *)pntfm + pntfm->loszFamilyName);
#endif

    // deal with the soft font, if there is one.

    if (pPFA)
    {
        // skip over the DWORD count.

        pPFA += sizeof(DWORD);
        memcpy(pstr, pPFA, cbPFA);
        pstr += DWORDALIGN(cbPFA);
    }

    // fill in the rest of the IFIMETRICS.

    CompleteIFIMETRICS(pdata);

    pntfm->cjThis += pdata->pTmpIFI->cjThis;

    // copy the IFIMETRICS stucture to the NTFM.

    memcpy(pstr, (CHAR *)pdata->pTmpIFI, pdata->pTmpIFI->cjThis);
}


//--------------------------------------------------------------------------
//
// VOID InitIFIMETRICS(pdata)
// PPARSEDATA  pdata;
//
// This routine initializes the IFIMETRICS structure with default values.
//
// Returns:
//   This routine returns no value.
//
// History:
//   25-Mar-1993    -by-    Kent Settle    (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

VOID InitIFIMETRICS(pdata)
PPARSEDATA  pdata;
{
    PIFIMETRICS pTmpIFI;

    pTmpIFI = pdata->pTmpIFI;

    // initialize the size of the structure
    // we will have to add the size of added strings later

    pTmpIFI->cjThis    = DWORDALIGN(sizeof(IFIMETRICS));

    pTmpIFI->ulVersion = FM_VERSION_NUMBER;

    pTmpIFI->jWinCharSet            =  ANSI_CHARSET;                // !!! i guess
    pTmpIFI->jWinPitchAndFamily     =  FF_DONTCARE | VARIABLE_PITCH;// !!! i Guess
    pTmpIFI->flInfo                 =  FM_INFO_ARB_XFORMS                  |
                                       FM_INFO_NOT_CONTIGUOUS              |
                                       FM_INFO_TECH_OUTLINE_NOT_TRUETYPE   |
                                       FM_INFO_1BBP                        |
                                       FM_INFO_RIGHT_HANDED;
    pTmpIFI->fsType                 =  FM_NO_EMBEDDING;
    pTmpIFI->fwdUnitsPerEm          =  1000;             // for PostScript fonts.
    pTmpIFI->fwdLowestPPEm          =  1;                // !!!I guess [kirko]

    pTmpIFI->fwdCapHeight           =  0;                       //!!![kirko] Kent-can you do better?
    pTmpIFI->fwdSubscriptXSize      =  300;                     // !!! a guess
    pTmpIFI->fwdSubscriptYSize      =  300;                     // !!! a guess
    pTmpIFI->fwdSubscriptXOffset    =  600;                     // !!! a guess
    pTmpIFI->fwdSubscriptYOffset    = -100;                     // !!! a guess
    pTmpIFI->fwdSuperscriptXSize    =  300;                     // !!! a guess
    pTmpIFI->fwdSuperscriptYSize    =  300;                     // !!! a guess
    pTmpIFI->fwdSuperscriptXOffset  =  600;                     // !!! a guess
    pTmpIFI->fwdSuperscriptYOffset  =  300;                     // !!! a guess

    pTmpIFI->wcBreakChar            = U_SPACE;
    WideCharToMultiByte(CP_ACP, 0, (LPWSTR)&pTmpIFI->wcBreakChar, 1,
                        (LPSTR)&pTmpIFI->chBreakChar, 1, NULL, NULL);

    pTmpIFI->ptlBaseline.x          = 1;
    pTmpIFI->ptlBaseline.y          = 0;
    pTmpIFI->ptlAspect.x            = 1;
    pTmpIFI->ptlAspect.y            = 1;
    pTmpIFI->achVendId[0]           = 'U';
    pTmpIFI->achVendId[1]           = 'n';
    pTmpIFI->achVendId[2]           = 'k';
    pTmpIFI->achVendId[3]           = 'n';
    pTmpIFI->ulPanoseCulture        = FM_PANOSE_CULTURE_LATIN;
    pTmpIFI->panose.bFamilyType     = PAN_ANY;
    pTmpIFI->panose.bSerifStyle     = PAN_ANY;
    pTmpIFI->panose.bProportion     = PAN_ANY;
    pTmpIFI->panose.bContrast       = PAN_ANY;
    pTmpIFI->panose.bStrokeVariation= PAN_ANY;
    pTmpIFI->panose.bArmStyle       = PAN_ANY;
    pTmpIFI->panose.bLetterform     = PAN_ANY;
    pTmpIFI->panose.bMidline        = PAN_ANY;
    pTmpIFI->panose.bXHeight        = PAN_ANY;
}



//--------------------------------------------------------------------------
//
// VOID CompleteIFIMETRICS(pdata)
// PPARSEDATA  pdata;
//
// This routine fill in IFIMETRICS values using NTFM values.
//
// Returns:
//   This routine returns no value.
//
// History:
//   25-Mar-1993    -by-    Kent Settle    (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

VOID CompleteIFIMETRICS(pdata)
PPARSEDATA  pdata;
{
    PIFIMETRICS     pTmpIFI;
    WINFONTPAIR    *pTable;
    BOOL            bFound;
    PSTR            pstr;
    PNTFM           pntfm;
    LONG            InternalLeading;

    // get local pointers.

    pntfm = pdata->pntfm;
    pTmpIFI = pdata->pTmpIFI;

    // for Win31 compatability - if there is a match in WinFontTable for
    // the Adobe facename, use the Windows Face Name for the IFIMETRICS
    // family name, otherwise, use the Adobe family name.

//!!! use nls conversion for this.

    pTable = (WINFONTPAIR *)WinFontTable;
    bFound = FALSE;

    while(pTable->pstrAdobeFace)
    {
        if (!(strncmp(pTable->pstrAdobeFace,
             (CHAR *)pntfm + pntfm->loszFontName, MAX_FONT_NAME)))
        {
            pstr = pTable->pstrWinFace;
            bFound = TRUE;
            break;
        }

        pTable++;
    }

    if (!bFound)
        pstr = (CHAR *)pntfm + pntfm->loszFamilyName;

    // WIN31 COMPATABILITY!  Check to see if this face name has aliases.
    // if it does, then we need to set the FM_INFO_FAMILY_EQUIV bit of
    // pTmpIFI->flInfo, and fill in an array of family aliases.

    GetFamilyAliases(pTmpIFI, pstr);

    // make the style name the same as the family name.

    pTmpIFI->dpwszStyleName = pTmpIFI->dpwszFamilyName;

    // get the face name for the font.

    pTmpIFI->dpwszFaceName = pTmpIFI->cjThis;

    strcpy2WChar((PWSZ)((char *)pTmpIFI + pTmpIFI->dpwszFaceName),
                 (PSZ)((char *)pntfm + pntfm->loszFontName));

    // while we have the face name, lets fill in the windows compatability
    // stuff.

    if (!strcmp((char *)pntfm + pntfm->loszFontName, "Symbol"))
        pTmpIFI->jWinCharSet = SYMBOL_CHARSET;
    else if (!strcmp((char *)pntfm + pntfm->loszFontName, "ZapfDingbats"))
        pTmpIFI->jWinCharSet = OEM_CHARSET;
    else
        pTmpIFI->jWinCharSet = ANSI_CHARSET;

    pTmpIFI->chFirstChar = (BYTE)0x20;
    pTmpIFI->chLastChar = (BYTE)0xFE;

    pTmpIFI->cjThis += ((wcslen((PWSZ)((char *)pTmpIFI +
                               pTmpIFI->dpwszFaceName)) + 1) * sizeof(WCHAR));

    // get the unique name for the font.

    pTmpIFI->dpwszUniqueName = pTmpIFI->cjThis;
    strcpy2WChar((PWSZ)((char *)pTmpIFI + pTmpIFI->dpwszUniqueName),
                 (PSZ)((char *)pntfm + pntfm->loszFullName));
    pTmpIFI->cjThis += ((wcslen((PWSZ)((char *)pTmpIFI +
                              pTmpIFI->dpwszUniqueName)) + 1) * sizeof(WCHAR));

    pTmpIFI->dpFontSim              =  0;

    pTmpIFI->fwdWinAscender         =  (FWORD) pntfm->rcBBox.top;
    pTmpIFI->fwdWinDescender        = -(FWORD) pntfm->rcBBox.bottom;
    pTmpIFI->fwdMacAscender         =  pTmpIFI->fwdWinAscender;
    pTmpIFI->fwdMacDescender        =  -pTmpIFI->fwdWinDescender;

    // as windows does, set total leading to 19.6% of EmHeight.
    // therefore, ExternalLeading = 196 - InternalLeading.

    InternalLeading = (pntfm->rcBBox.top - pntfm->rcBBox.bottom) -
                      ADOBE_FONT_UNITS;

    if (InternalLeading < 0)
        InternalLeading = 0;

    pTmpIFI->fwdMacLineGap          =  196 - InternalLeading;
    if (pTmpIFI->fwdMacLineGap < 0)
        pTmpIFI->fwdMacLineGap = 0;

    pTmpIFI->fwdTypoLineGap         =  0;
    pTmpIFI->fwdTypoAscender        =  pTmpIFI->fwdWinAscender;
    pTmpIFI->fwdTypoDescender       =  pTmpIFI->fwdWinDescender;
    pTmpIFI->fwdStrikeoutPosition   =  pTmpIFI->fwdWinAscender / 2;

    // hardcoded from mapping.h values.

    if (!(strcmp((char *) pntfm + pntfm->loszFontName, "ZapfDingbats")))
    {
        pTmpIFI->wcDefaultChar = U_SPACE;
        pTmpIFI->chDefaultChar = (BYTE)0x20;
    }
    else if (!(strcmp((char *) pntfm + pntfm->loszFontName, "Symbol")))
    {
        pTmpIFI->wcDefaultChar = U_SYMBOL_BULLET;
        pTmpIFI->chDefaultChar = (BYTE)0xB7;
    }

    else
    {
        pTmpIFI->wcDefaultChar = U_BULLET;
        pTmpIFI->chDefaultChar = (BYTE)0x8C;
    }

    WideCharToMultiByte(CP_ACP, 0, (LPWSTR)&pTmpIFI->wcDefaultChar, 1,
                        (LPSTR)&pTmpIFI->chDefaultChar, 1, NULL, NULL);

    pTmpIFI->cKerningPairs          = pntfm->cKernPairs;

    // set the average and maximum character widths for the font.

    SetWidths(pdata);

    // find the first and last characters for the given font.

    GetFirstLastChar(pdata);
}


//--------------------------------------------------------------------------
//
// VOID GetFamilyAliases(pifi, pstr)
// IFIMETRICS *pifi;
// PSTR        pstr;
//
// This routine fill in the family name of the IFIMETRICS structure.
//
// Returns:
//   This routine returns no value.
//
// History:
//   25-Mar-1993    -by-    Kent Settle    (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

VOID GetFamilyAliases(pifi, pstr)
IFIMETRICS *pifi;
PSTR        pstr;
{
    PSTR       *pTable;
    PWSTR       pwstr;
    DWORD       cb;

    // assume no alias table found.

    pTable = (PSTR *)(NULL);

    // this is an ugly hardcoded Win31 Hack that we need to be compatible
    // with since some stupid apps have hardcoded font names.

    if (!(strcmp(pstr, "Times")))
        pTable = TimesAlias;

    else if (!(strcmp(pstr, "Helvetica")))
        pTable = HelveticaAlias;

    else if (!(strcmp(pstr, "Courier")))
        pTable = CourierAlias;

    else if (!(strcmp(pstr, "Helvetica-Narrow")))
        pTable = HelveticaNarrowAlias;

    else if (!(strcmp(pstr, "Palatino")))
        pTable = PalatinoAlias;

    else if (!(strcmp(pstr, "Bookman")))
        pTable = BookmanAlias;

    else if (!(strcmp(pstr, "NewCenturySchlbk")))
        pTable = NewCenturySBAlias;

    else if (!(strcmp(pstr, "AvantGarde")))
        pTable = AvantGardeAlias;

    else if (!(strcmp(pstr, "ZapfChancery")))
        pTable = ZapfChanceryAlias;

    else if (!(strcmp(pstr, "ZapfDingbats")))
        pTable = ZapfDingbatsAlias;

    // get offset to family name from start of IFIMETRICS structure.

    pifi->dpwszFamilyName = pifi->cjThis;
    pwstr = (PWSZ)((char *)pifi + pifi->dpwszFamilyName);

    if (pTable)
    {
        // set the pifi->flInfo flag.

        pifi->flInfo |= FM_INFO_FAMILY_EQUIV;

        // now fill in the array of alias family names.

        while (*pTable)
        {
            strcpy2WChar(pwstr, (PSTR)*pTable++);
            cb = ((wcslen(pwstr) + 1) * sizeof(WCHAR));
            pifi->cjThis += cb;
            pwstr += (cb >> 1);
        }

        // add the extra NULL terminator to the end of the array.

        *pwstr = (WCHAR)'\0';
        pifi->cjThis += sizeof(WCHAR);
    }
    else
    {
        // fill in the single family name.

        strcpy2WChar(pwstr, pstr);
        pifi->cjThis += ((wcslen(pwstr) + 1) * sizeof(WCHAR));
    }
}
