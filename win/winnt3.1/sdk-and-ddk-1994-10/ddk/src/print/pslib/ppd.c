//--------------------------------------------------------------------------
//
// Module Name:  PPD.C
//
// Brief Description:  This module contains the PSCRIPT driver's PPD
// Compiler.
//
// Author:  Kent Settle (kentse)
// Created: 20-Mar-1991
//
// Copyright (c) 1991 Microsoft Corporation
//
// This module contains routines which will take an Adobe PPD (printer
//--------------------------------------------------------------------------

#include "string.h"
#include "pscript.h"

#define TESTING 0
#define SIZE_TEST 0

#define MAX_PS_NAME     256

// declarations of routines residing within this module.

VOID InitNTPD(PNTPD);
int GetString(char *, PPARSEDATA);
void GetDimension(PAPERDIM *, PPARSEDATA);
void GetImageableArea(RECTL *, PPARSEDATA);
int szLength(char *);
VOID BuildNTPD(PNTPD, PTMP_NTPD);
DWORD SizeNTPD(PNTPD, PTMP_NTPD);
int GetKeyword(TABLE_ENTRY *, PPARSEDATA);
VOID GetOptionString(PSTR, DWORD, PPARSEDATA);
int GetOptionIndex(TABLE_ENTRY *, PPARSEDATA);
int MapToken(char *, TABLE_ENTRY *);
VOID GetWord(char *, int, PPARSEDATA);
VOID ParsePPD(PNTPD, PTMP_NTPD, PPARSEDATA);
BOOL GetLine(PPARSEDATA);
VOID UnGetLine(PPARSEDATA);
VOID EatWhite(PPARSEDATA);
BOOL szIsEqual(char *, char *);
BOOL GetBuffer(PPARSEDATA);
int GetNumber(PPARSEDATA);
int GetFloat(int, PPARSEDATA);
int NameComp(CHAR *, CHAR *);
VOID ParseProtocols(PPARSEDATA, PNTPD);

// external declarations.

extern TABLE_ENTRY KeywordTable[];
extern TABLE_ENTRY SecondKeyTable[];
extern TABLE_ENTRY FontTable[];

//--------------------------------------------------------------------------
//
// VOID InitNTPD(pntpd);
// PNTPD    pntpd;
//
// Fills in the NTPD structure with initial values.
//
// Returns:
//   This routine returns no value.
//
// History:
//   22-Mar-1991    -by-    Kent Settle    (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

VOID InitNTPD(pntpd)
PNTPD   pntpd;
{
    pntpd->cjThis = sizeof(NTPD);
    pntpd->flFlags = 0;
    pntpd->ulVersion = (ULONG)NTPD_VERSION;
    pntpd->iDefResolution = DEF_RESOLUTION;
    pntpd->LangLevel = 1;
}


//--------------------------------------------------------------------------
//
// VOID ParsePPD(pntpd, ptmp, pdata);
// PNTPD        pntpd;
// PTMP_NTPD    ptmp;
// PPARSEDATA   pdata;
//
// Parses the PPD file, building the TMP_NTPD structure as it goes.
//
// Returns:
//   This routine returns no value.
//
// History:
//   03-Apr-1991    -by-    Kent Settle    (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

VOID ParsePPD(pntpd, ptmp, pdata)
PNTPD       pntpd;
PTMP_NTPD   ptmp;
PPARSEDATA  pdata;
{
    int     iKeyword;
    int     i, j;
    char    szWord[256];

    while (TRUE)
    {
        // get the next line from the PPD file.

        if (GetLine(pdata))
        {
#if TESTING
            DbgPrint("Normal End of File.\n");
#endif
            break;
        }

        // get the next Keyword from the PPD file.

        iKeyword = GetKeyword(KeywordTable, pdata);

        // we are done if end of file.

        if (iKeyword == TK_EOF)
            break;

        // there will actually be a lot of Keywords we don't care
        // about.  for speed's sake, let's trap them here.

        if (iKeyword == TK_UNDEFINED)
            continue;

        switch (iKeyword)
        {
            case COLORDEVICE:
                GetWord(szWord, sizeof(szWord), pdata);
		if (!(strncmp(szWord, "True", 4)))
                {
                    pntpd->flFlags |= COLOR_DEVICE;
#if TESTING
		    DbgPrint("Device is Color.\n");
#endif
                }
#if TESTING
                else
		    DbgPrint("Device is Black & White.\n");
#endif
                break;

            case VARIABLEPAPER:
		GetWord(szWord, sizeof(szWord), pdata);
		if (!(strncmp(szWord, "True", 4)))
                {
                    pntpd->flFlags |= VARIABLE_PAPER;
#if TESTING
		    DbgPrint("Device supports Variable Paper.\n");
#endif
                }
#if TESTING
                else
		    DbgPrint("Device does not support Variable Paper.\n");
#endif
                break;

            case ENDOFFILE:
                GetWord(szWord, sizeof(szWord), pdata);
                if (!(strncmp(szWord, "False", 5)))
                {
                    pntpd->flFlags |= NO_ENDOFFILE;
#if TESTING
                    DbgPrint("Device does not want Ctrl-D.\n");
#endif
                }
                else
{
                    pntpd->flFlags &= ~NO_ENDOFFILE;
#if TESTING
                    DbgPrint("Device does want Ctrl-D.\n");
#endif
}

                break;

            case DEFAULTMANUALFEED:
                GetWord(szWord, sizeof(szWord), pdata);
		if (!(strncmp(szWord, "True", 4)))
                {
                    pntpd->flFlags |= MANUALFEED_ON;
#if TESTING
		    DbgPrint("Device defaults to Manual Feed.\n");
#endif
                }
#if TESTING
                else
		    DbgPrint("Device does not default to Manual Feed.\n");
#endif
                break;

            case PROTOCOLS:
                ParseProtocols(pdata, pntpd);
                break;

            case NICKNAME:
                ptmp->cbPrinterName = GetString(ptmp->szPrinterName, pdata);

                // make room for UNICODE printer name.

                ptmp->cbPrinterName *= 2;
#if TESTING
		DbgPrint("PrinterName = %s\n", ptmp->szPrinterName);
#endif
                break;

            case PRTVM:
                // fill in the free virtual memory in kilobytes.

                i = GetNumber(pdata);
                pntpd->cbFreeVM = (i >> 10);
#if TESTING
		DbgPrint("FreeVM = %d KB.\n", pntpd->cbFreeVM);
#endif
                break;

            case LANGUAGELEVEL:
                // fill in the language level.  default to level 1 if
                // level 2 is not specified.

                i = GetNumber(pdata);

                if (i != 2)
                    i = 1;

                pntpd->LangLevel = (DWORD)i;
#if TESTING
		DbgPrint("LanguageLevel = %d.\n", pntpd->LangLevel);
#endif
                break;

            case DEFAULTRESOLUTION:
                pntpd->iDefResolution = (USHORT)GetNumber(pdata);
#if TESTING
		DbgPrint("DefResolution = %d.\n", pntpd->iDefResolution);
#endif
                break;

            case SETRESOLUTION:
            case RESOLUTION:
                // increment the resolution count.  for most printers, which
                // do not support this, pntpd->cResolutions will be zero.
                // this obviously means to use the defaultresolution.

                i = pntpd->cResolutions;
                i++;
                if (i > MAX_RESOLUTIONS)
                {
                    RIP("Too Many Resolutions\n");
                    break;
                }
                pntpd->cResolutions = (USHORT)i;

                // get the resolution itself.

                i--;
                ptmp->siResolutions[i].usIndex = (USHORT)GetNumber(pdata);
#if TESTING
		DbgPrint("Resolution Value = %d\n",
                         ptmp->siResolutions[i].usIndex);
#endif
                // now get the string to send to the printer to set the
                // resolution.

                GetString(ptmp->siResolutions[i].szString, pdata);
                break;

            case SCREENFREQ:
                // the screen frequency is stored within quotes.
                // advance to first quotation mark, then one character past.

                while (*(pdata->szLine) != '"')
                    pdata->szLine++;
                pdata->szLine++;

                pntpd->iScreenFreq = (USHORT)GetFloat(10, pdata);
#if TESTING
		DbgPrint("ScreenFrequency * 10 = %d\n", pntpd->iScreenFreq);
#endif
                break;

            case SCREENANGLE:
                // the screen angle is stored within quotes.
                // advance to first quotation mark, then one character past.

                while (*(pdata->szLine) != '"')
                    pdata->szLine++;
                pdata->szLine++;

                pntpd->iScreenAngle = (USHORT)GetFloat(10, pdata);
#if TESTING
		DbgPrint("ScreenAngle * 10 = %d\n", pntpd->iScreenAngle);
#endif
                break;

            case TRANSFER:
                // GetOptionIndex will get the string defining the type
                // of transfer function.  Normalized is the one we
                // care about.

                i = GetOptionIndex(SecondKeyTable, pdata);
                if (i == NORMALIZED)
                {
                    ptmp->cbTransferNorm = GetString(ptmp->szTransferNorm, pdata);
#if TESTING
		    DbgPrint("TransferNormalized = %s\n",
                             ptmp->szTransferNorm);
#endif
                }
                else if (i == NORM_INVERSE)
                {
                    ptmp->cbInvTransferNorm = GetString(ptmp->szInvTransferNorm, pdata);
#if TESTING
		    DbgPrint("InvTransferNormalized = %s\n",
                             ptmp->szInvTransferNorm);
#endif
                }

                break;

            case DUPLEX:
                // GetOptionIndex will get the string defining the type
                // of duplex function.

                i = GetOptionIndex(SecondKeyTable, pdata);

                switch (i)
                {
                    case OPTION_FALSE:
                        ptmp->cbDuplexNone = GetString(ptmp->szDuplexNone, pdata);
#if TESTING
                        DbgPrint("szDuplexNone = %s.\n", ptmp->szDuplexNone);
#endif
                        break;

                    case OPTION_TRUE:
                        ptmp->cbDuplexNoTumble = GetString(ptmp->szDuplexNoTumble, pdata);
#if TESTING
                        DbgPrint("szDuplexNoTumble = %s.\n", ptmp->szDuplexNoTumble);
#endif
                        break;

                    case OPTION_NONE:
                        ptmp->cbDuplexNone = GetString(ptmp->szDuplexNone, pdata);
#if TESTING
                        DbgPrint("szDuplexNone = %s.\n", ptmp->szDuplexNone);
#endif
                        break;

                    case DUPLEX_TUMBLE:
                        ptmp->cbDuplexTumble = GetString(ptmp->szDuplexTumble, pdata);
#if TESTING
                        DbgPrint("szDuplexTumble = %s.\n", ptmp->szDuplexTumble);
#endif
                        break;

                    case DUPLEX_NO_TUMBLE:
                        ptmp->cbDuplexNoTumble = GetString(ptmp->szDuplexNoTumble, pdata);
#if TESTING
                        DbgPrint("szDuplexNoTumble = %s.\n", ptmp->szDuplexNoTumble);
#endif
                        break;
                }

                break;

            case COLLATE:
                // GetOptionIndex will get the string defining the type
                // of collate function.

                i = GetOptionIndex(SecondKeyTable, pdata);
                if (i == OPTION_TRUE)
                {
                    ptmp->cbCollateOn = GetString(ptmp->szCollateOn, pdata);
#if TESTING
		    DbgPrint("CollateOn = %s\n",
                             ptmp->szCollateOn);
#endif
                }
                else if (i == OPTION_FALSE)
                {
                    ptmp->cbCollateOff = GetString(ptmp->szCollateOff, pdata);
#if TESTING
		    DbgPrint("CollateOff = %s\n",
                             ptmp->szCollateOff);
#endif
                }

                break;

            case DEFAULTPAGESIZE:
                // GetOptionIndex, will get the string defining the defaultpagesize,
                // and return the corresponding value from PaperTable.

                GetOptionString(ptmp->szDefaultForm, sizeof(ptmp->szDefaultForm),
                            pdata);

#if TESTING
                DbgPrint("DefaultForm = %s.\n", ptmp->szDefaultForm);
#endif
                break;

            case PAGESIZE:
                // increment the paper size count.

                i = pntpd->cPSForms;

                if (i >= MAX_PAPERSIZES)
                {
                    RIP("Too Many PaperSizes.\n");
                    break;
                }

                pntpd->cPSForms++;

                // get the form name.

                GetOptionString(ptmp->FormEntry[i].szName,
                            sizeof(ptmp->FormEntry[i].szName), pdata);

                // now get the form invocation string to send to the printer.

                GetString(ptmp->FormEntry[i].szInvocation, pdata);

                break;

            case PAGEREGION:
                // increment the page region count.

                i = pntpd->cPageRegions;

                if (i >= MAX_PAPERSIZES)
                {
                    RIP("Too Many PageRegions.\n");
                    break;
                }
                pntpd->cPageRegions++;

                // get the form name, and the invocation string to
                // set the page region.

                GetOptionString(ptmp->PageRegion[i].szName,
                            sizeof(ptmp->PageRegion[i].szName), pdata);

#if TESTING
		DbgPrint("PageRegion %s.\n", ptmp->PageRegion[i].szName);
#endif

                // now get the pageregion invocation string.

                GetString(ptmp->PageRegion[i].szInvocation, pdata);

                break;

            case IMAGEABLEAREA:
                // increment the imageablearea count.

                i = ptmp->cImageableAreas;

                if (i >= MAX_PAPERSIZES)
                {
                    RIP("Too Many ImageableAreas.\n");
                    break;
                }

                ptmp->cImageableAreas++;

                // get the form name.

                GetOptionString(ptmp->ImageableArea[i].szForm,
                                sizeof(ptmp->ImageableArea[i].szForm),
                                pdata);

                // now get the rectangle of the imageablearea.

                GetImageableArea(&ptmp->ImageableArea[i].rect, pdata);
#if TESTING
		DbgPrint("ImageableArea %s = %d %d %d %d\n",
                         ptmp->ImageableArea[i].szForm,
                         ptmp->ImageableArea[i].rect.left,
                         ptmp->ImageableArea[i].rect.top,
                         ptmp->ImageableArea[i].rect.right,
                         ptmp->ImageableArea[i].rect.bottom);
#endif
                break;

            case PAPERDIMENSION:
                // increment the paperdimension count.

                i = ptmp->cPaperDimensions;

                if (i >= MAX_PAPERSIZES)
                {
                    RIP("Too Many PaperDimensions.\n");
                    break;
                }

                ptmp->cPaperDimensions++;

                GetOptionString(ptmp->PaperDimension[i].szForm,
                                sizeof(ptmp->PaperDimension[i].szForm),
                                pdata);

                // now get the rectangle of the paper itself.

                GetDimension(&ptmp->PaperDimension[i], pdata);
#if TESTING
		DbgPrint("PaperDimension %s = %d %d\n",
                         ptmp->PaperDimension[i].szForm,
                         ptmp->PaperDimension[i].sizl.cx,
                         ptmp->PaperDimension[i].sizl.cy);
#endif
                break;

            case DEFAULTOUTPUTBIN:
                GetOptionString(ptmp->szDefaultOutputBin,
                                sizeof(ptmp->szDefaultOutputBin), pdata);

#if TESTING
		DbgPrint("DefaultOutputBin = %s\n", ptmp->szDefaultOutputBin);
#endif
                break;


            case OUTPUTBIN:
                // increment the output bin count.

                i = pntpd->cOutputBins;
                i++;
                if (i > MAX_BINS)
                {
                    RIP("Too Many OutputBins.\n");
                    break;
                }
                pntpd->cOutputBins = (USHORT)i;

                i--;

                GetOptionString(ptmp->siOutputBin[i].szName,
                                sizeof(ptmp->siOutputBin[i].szName), pdata);
#if TESTING
		DbgPrint("OutputBin Name = %s\n", ptmp->siOutputBin[i].szName);
#endif

                // now get the string to send to the printer to set the
                // outputbin.

                GetString(ptmp->siOutputBin[i].szInvocation, pdata);
                break;

            case DEFAULTINPUTSLOT:
                GetOptionString(ptmp->szDefaultInputSlot,
                                sizeof(ptmp->szDefaultInputSlot), pdata);

#if TESTING
		DbgPrint("DefaultInputSlot = %s\n", ptmp->szDefaultInputSlot);
#endif
                break;


            case INPUTSLOT:
                // increment the output bin count.

                i = pntpd->cInputSlots;
                i++;
                if (i > MAX_BINS)
                {
                    RIP("Too Many InputSlots.\n");
                    break;
                }
                pntpd->cInputSlots = (USHORT)i;

                i--;

                GetOptionString(ptmp->siInputSlot[i].szName,
                                sizeof(ptmp->siInputSlot[i].szName), pdata);

#if TESTING
		DbgPrint("InputSlot Name = %s\n", ptmp->siInputSlot[i].szName);
#endif

                // now get the string to send to the printer to set the
                // inputslot.

                GetString(ptmp->siInputSlot[i].szInvocation, pdata);
                break;

            case MANUALFEED:
                // GetOptionIndex will get the string defining the type
                // of ManualFeed function.

                i = GetOptionIndex(SecondKeyTable, pdata);

                if (i == OPTION_TRUE)
                {
                    // get and save the string to set manual feed to TRUE
                    // for the given printer.

                    ptmp->cbManualTRUE = GetString(ptmp->szManualTRUE, pdata);
                }
                else if (i == OPTION_FALSE)
                {
                    // get and save the string to set manual feed to FALSE
                    // for the given printer.

                    ptmp->cbManualFALSE = GetString(ptmp->szManualFALSE, pdata);
                }

                break;

            case DEFAULTFONT:
                // GetOptionIndex, will get the string defining the defaultfont,
                // and return the corresponding value from FontTable.

                i = GetOptionIndex(FontTable, pdata);
                pntpd->usDefaultFont = (USHORT)i;
#if TESTING
		DbgPrint("DefaultFont = %d\n", i);
#endif
                break;

            case DEVICE_FONT:
                // get the index of the font.

                j = GetOptionIndex(FontTable, pdata);

                if (j == TK_UNDEFINED)
                {
#if TESTING
                    DbgPrint("ParsePPD: font not found.\n");
#endif
                    break;
                }

                // increment the font counter if the font was valid.

                i = pntpd->cFonts;
                i++;

                if (i > MAX_FONTS)
                {
                    RIP("Too Many Fonts.\n");
                    break;
                }

                pntpd->cFonts = (USHORT)i;

                // GetOptionIndex, will get the string defining the font,
                // and return the corresponding value from FontTable.

                i--;

                ptmp->bFonts[i] = (BYTE)j;
#if TESTING
		DbgPrint("Font Value = %d\n", j);
#endif
                break;

            default:
                break;
        }
    }
}


//--------------------------------------------------------------------------
//
// VOID BuildNTPD(pntpd, ptmp);
// PNTPD        pntpd;
// PTMP_NTPD    ptmp;
//
// Fills in the NTPD structure with values derived from the TMP_NTPD
// structure.
//
// Returns:
//   This routine returns no value.
//
// History:
//   25-Mar-1991    -by-    Kent Settle    (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

VOID BuildNTPD(pntpd, ptmp)
PNTPD       pntpd;
PTMP_NTPD   ptmp;
{
    DWORD           i, j;
    BYTE           *pfont;
    PSRESOLUTION   *pRes;
    PSFORM         *pForm;
    PSOUTPUTBIN    *pBin;
    PSINPUTSLOT    *pSlot;

    // start by adding the printer name to the end of the NTPD structure.
    // the printer name is stored as a UNICODE string, so it needs to be
    // WCHAR aligned.

#if SIZE_TEST
    DbgPrint("Entering BuildNTPD: cjThis = %d.\n", pntpd->cjThis);
#endif

    pntpd->cjThis = WCHARALIGN(pntpd->cjThis);

#if DBG
    // make sure alignment is proper.

    ASSERTPS(((pntpd->cjThis % sizeof(WCHAR)) == 0),
             "pntpd->lowszPrinterName not properly aligned.\n");
#endif

    pntpd->lowszPrinterName = pntpd->cjThis;

    strcpy2WChar((PWSTR)((PSTR)pntpd + pntpd->lowszPrinterName),
                 ptmp->szPrinterName);

#if TESTING
    DbgPrint("PrinterName = %ws\n",
             (PWSTR)((PSTR)pntpd + pntpd->lowszPrinterName));
#endif

    // now add the set resolution strings to the end of the structure,
    // if there are any to add.  it is worth noting how these are stored
    // in the NTPD structure.  an array of cResolutions PSRESOLUTION
    // structures are stored at the end of the NTPD structure.  within
    // each PSRESOLUTION structure is an offset to the string corresponding
    // to the resolution in question.

    // the start of the PSRESOLUTION array must be DWORD aligned.

    pntpd->cjThis += ptmp->cbPrinterName;
    pntpd->cjThis = DWORDALIGN(pntpd->cjThis);

#if DBG
    // make sure alignment is proper.

    ASSERTPS(((pntpd->cjThis % sizeof(DWORD)) == 0),
             "pntpd->loResolution not properly aligned.\n");
#endif

    pntpd->loResolution = pntpd->cjThis;

#if SIZE_TEST
    DbgPrint("post cbPrinterName: cjThis = %d.\n", pntpd->cjThis);
#endif

    if (pntpd->cResolutions != 0)
    {
        // add the PSRESOLUTION array to the end of the NTPD structure.

        pntpd->cjThis += pntpd->cResolutions * sizeof(PSRESOLUTION);

        pRes = (PSRESOLUTION *)((CHAR *)pntpd + pntpd->loResolution);

        // for each resolution, fill in the PSRESOLUTION structure.
        // then add the string itself to the end of the NTPD structure.

        for (i = 0; i < (DWORD)pntpd->cResolutions; i++)
        {
            pRes[i].iValue = (DWORD)ptmp->siResolutions[i].usIndex;
            pRes[i].loInvocation = pntpd->cjThis;

            j = szLength(ptmp->siResolutions[i].szString);
            memcpy((CHAR *)pntpd + pntpd->cjThis,
                   ptmp->siResolutions[i].szString, j);
            pntpd->cjThis += j;
        }
    }

#if SIZE_TEST
    DbgPrint("post resolutions: cjThis = %d.\n", pntpd->cjThis);
#endif

    // add the transfernormalized string to the end of the structure.
    // check to make sure we have a string to copy.

    i = (USHORT)ptmp->cbTransferNorm;

    if (i != 0)
    {
        pntpd->loszTransferNorm = pntpd->cjThis;
        pntpd->cjThis += i;

        memcpy((CHAR *)pntpd + pntpd->loszTransferNorm,
               ptmp->szTransferNorm, i);
    }

#if SIZE_TEST
    DbgPrint("post cbTransferNorm: cjThis = %d.\n", pntpd->cjThis);
#endif

#if TESTING
    if (i != 0)
	DbgPrint("Transfer Normalized Not Found.\n");
    else
	DbgPrint("Transfer Normalized = %s\n", ptmp->szTransferNorm);
#endif

    // add the inverse transfernormalized string to the end of the structure.
    // check to make sure we have a string to copy.

    if (i = ptmp->cbInvTransferNorm)
    {
        pntpd->loszInvTransferNorm = pntpd->cjThis;
        pntpd->cjThis += i;

        memcpy((CHAR *)pntpd + pntpd->loszInvTransferNorm,
               ptmp->szInvTransferNorm, i);
    }

#if SIZE_TEST
    DbgPrint("post cbInvTransferNorm: cjThis = %d.\n", pntpd->cjThis);
#endif

#if TESTING
    if (i != 0)
	DbgPrint("Inverse Transfer Normalized Not Found.\n");
    else
	DbgPrint("Inverse Transfer Normalized = %s\n", ptmp->szTransferNorm);
#endif

    // fill in the default form name.

    pntpd->loDefaultForm = pntpd->cjThis;
    strcpy((CHAR *)pntpd + pntpd->loDefaultForm, ptmp->szDefaultForm);
    pntpd->cjThis += szLength(ptmp->szDefaultForm);

    // add the form names list to the end of the structure.

    pntpd->cjThis = DWORDALIGN(pntpd->cjThis);

#if SIZE_TEST
    DbgPrint("post szDefaultForm: cjThis = %d.\n", pntpd->cjThis);
#endif

#if DBG
    // make sure alignment is proper.

    ASSERTPS(((pntpd->cjThis % sizeof(DWORD)) == 0),
             "pntpd->loPSFORMArray not properly aligned.\n");
#endif

    pntpd->loPSFORMArray = pntpd->cjThis;
    pntpd->cjThis += pntpd->cPSForms * sizeof(PSFORM);

#if SIZE_TEST
    DbgPrint("post cPSForms: cjThis = %d.\n", pntpd->cjThis);
#endif

    pForm = (PSFORM *)((CHAR *)pntpd + pntpd->loPSFORMArray);

    // for each form, fill in the offset to the form name string, the
    // offset to the invocation string, then the strings themselves.

    for (i = 0; i < pntpd->cPSForms; i++)
    {
        pForm[i].loFormName = pntpd->cjThis;
        strcpy((CHAR *)pntpd + pntpd->cjThis, ptmp->FormEntry[i].szName);
        pntpd->cjThis += szLength(ptmp->FormEntry[i].szName);

        pForm[i].loSizeInvo = pntpd->cjThis;
        strcpy((CHAR *)pntpd + pntpd->cjThis, ptmp->FormEntry[i].szInvocation);
        pntpd->cjThis += szLength(ptmp->FormEntry[i].szInvocation);

#if TESTING
        DbgPrint("FormEntry %s = %s.\n", ptmp->FormEntry[i].szName,
                 ptmp->FormEntry[i].szInvocation);
#endif
    }

#if SIZE_TEST
    DbgPrint("post all forms: cjThis = %d.\n", pntpd->cjThis);
#endif

    // fill in the page region information. an offset to each string is set
    // in the corresponding PSFORM struct. then add the string itself to the
    // end of the NTPD structure.

    for (i = 0; i < pntpd->cPageRegions; i++)
    {
        for (j = 0; j < pntpd->cPSForms; j++)
        {
            if (!(NameComp((CHAR *)pntpd + pForm[j].loFormName,
                         ptmp->PageRegion[i].szName)))
            {
                pForm[j].loRegionInvo = pntpd->cjThis;
                strcpy((CHAR *)pntpd + pntpd->cjThis,
                       ptmp->PageRegion[i].szInvocation);
                pntpd->cjThis += szLength(ptmp->PageRegion[i].szInvocation);

#if TESTING
    DbgPrint("PageRegion[%d] = %s\n", i, ptmp->PageRegion[i].szName);
#endif
            }
        }
    }

#if SIZE_TEST
    DbgPrint("post page regions: cjThis = %d.\n", pntpd->cjThis);
#endif

    // fill in the imageablearea information.  an RECTL for each form is
    // included within the PSFORM struct.

    for (i = 0; i < ptmp->cImageableAreas; i++)
    {
        for (j = 0; j < pntpd->cPSForms; j++)
        {
            if (!(NameComp((CHAR *)pntpd + pForm[j].loFormName,
                         ptmp->ImageableArea[i].szForm)))
            {
                pForm[j].imagearea = ptmp->ImageableArea[i].rect;
#if TESTING
    DbgPrint("ImageableArea %s = %d %d %d %d.\n",
             ptmp->PageRegion[i].szName,
             pForm[j].imagearea.left, pForm[j].imagearea.top,
             pForm[j].imagearea.right, pForm[j].imagearea.bottom);
#endif
            }
        }
    }

#if SIZE_TEST
    DbgPrint("post imageableareas: cjThis = %d.\n", pntpd->cjThis);
#endif

    // fill in the paper dimension information.  a SIZEL for each form is
    // included within the PSFORM struct.

    for (i = 0; i < ptmp->cPaperDimensions; i++)
    {
        for (j = 0; j < pntpd->cPSForms; j++)
        {
            if (!(NameComp((CHAR *)pntpd + pForm[j].loFormName,
                         ptmp->PaperDimension[i].szForm)))
            {
                pForm[j].sizlPaper = ptmp->PaperDimension[i].sizl;
#if TESTING
    DbgPrint("PaperDimension %s = %d %d.\n",
             ptmp->PaperDimension[i].szForm,
             pForm[j].sizlPaper.cx, pForm[j].sizlPaper.cy);
#endif
            }
        }
    }

#if SIZE_TEST
    DbgPrint("post paper dimensions: cjThis = %d.\n", pntpd->cjThis);
#endif

    // now add the outputbin strings to the end of the structure,
    // if there are any to add.  it is worth noting how these are stored
    // in the NTPD structure.  an array of cOutputBins PSOUTPUTBIN
    // structures are stored at the end of the NTPD structure.  within
    // each PSOUTPUTBIN structure are offsets to the strings corresponding
    // to the output bin and invocation.  if there is only the default
    // outputbin defined, cOutputBins will be zero.  otherwise it is assumed
    // there will be at least two output bins defined.  if there is only one
    // defined, it will be the same as the default.

    pntpd->loDefaultBin = pntpd->cjThis;
    strcpy((CHAR *)pntpd + pntpd->loDefaultBin, ptmp->szDefaultOutputBin);
    pntpd->cjThis += szLength(ptmp->szDefaultOutputBin);
    pntpd->cjThis = DWORDALIGN(pntpd->cjThis);

#if SIZE_TEST
    DbgPrint("post szDefaultOutputBin: cjThis = %d.\n", pntpd->cjThis);
#endif

    if (pntpd->cOutputBins > 0)
    {
#if DBG
    // make sure alignment is proper.

    ASSERTPS(((pntpd->cjThis % sizeof(DWORD)) == 0),
             "pntpd->loPSOutputBins not properly aligned.\n");
#endif

        // add the PSOUTPUTBIN array to the end of the NTPD structure.

        pntpd->loPSOutputBins = pntpd->cjThis;

        pntpd->cjThis += pntpd->cOutputBins * sizeof(PSOUTPUTBIN);

#if SIZE_TEST
    DbgPrint("post cOutputBins: cjThis = %d.\n", pntpd->cjThis);
#endif

        pBin = (PSOUTPUTBIN *)((CHAR *)pntpd + pntpd->loPSOutputBins);

        // for each outputbin, fill in the PSOUTPUTBIN structure.
        // then add the string itself to the end of the NTPD structure.

        for (i = 0; i < pntpd->cOutputBins; i++)
        {
            // copy the output bin name and the invocation string.

            pBin[i].loBinName = pntpd->cjThis;
            strcpy((CHAR *)pntpd + pntpd->cjThis, ptmp->siOutputBin[i].szName);
            pntpd->cjThis += szLength(ptmp->siOutputBin[i].szName);

            pBin[i].loBinInvo = pntpd->cjThis;
            strcpy((CHAR *)pntpd + pntpd->cjThis,
                   ptmp->siOutputBin[i].szInvocation);
            pntpd->cjThis += szLength(ptmp->siOutputBin[i].szInvocation);

#if TESTING
	    DbgPrint("OutputBin[%d] = %s\n", i,
                     (CHAR *)pntpd + pBin[i].loBinName);
#endif
        }
    }

#if SIZE_TEST
    DbgPrint("post all output bins: cjThis = %d.\n", pntpd->cjThis);
#endif

    // now add the inputslot strings to the end of the structure,
    // if there are any to add.  it is worth noting how these are stored
    // in the NTPD structure.  an array of cInputSlots PSINPUTSLOT
    // structures are stored at the end of the NTPD structure.  within
    // each PSINPUTSLOT structure are offsets to the strings corresponding
    // to the slot name and invocation.  if there is only the default inputslot
    // defined, cInputSlots will be zero.  otherwise it is assumed there
    // will be at least two input slots defined.  if there is only one
    // defined, it will be the same as the default.

    pntpd->loDefaultSlot = pntpd->cjThis;
    strcpy((CHAR *)pntpd + pntpd->loDefaultSlot, ptmp->szDefaultInputSlot);
    pntpd->cjThis += szLength(ptmp->szDefaultInputSlot);
    pntpd->cjThis = DWORDALIGN(pntpd->cjThis);

#if SIZE_TEST
    DbgPrint("post szDefaultInputSlot: cjThis = %d.\n", pntpd->cjThis);
#endif

    if (pntpd->cInputSlots > 0)
    {
#if DBG
    // make sure alignment is proper.

    ASSERTPS(((pntpd->cjThis % sizeof(DWORD)) == 0),
             "pntpd->loPSInputSlots properly aligned.\n");
#endif

        // add the PSINPUTSLOT array to the end of the NTPD structure.

        pntpd->loPSInputSlots = pntpd->cjThis;

        pntpd->cjThis += pntpd->cInputSlots * sizeof(PSINPUTSLOT);

#if SIZE_TEST
    DbgPrint("post cInputSlots: cjThis = %d.\n", pntpd->cjThis);
#endif

        pSlot = (PSINPUTSLOT *)((CHAR *)pntpd + pntpd->loPSInputSlots);

        // for each inputslot, fill in the PSINPUTSLOT structure.
        // then add the string itself to the end of the NTPD structure.

        for (i = 0; i < pntpd->cInputSlots; i++)
        {
            // copy the input slot name and the invocation string.

            pSlot[i].loSlotName = pntpd->cjThis;
            strcpy((CHAR *)pntpd + pntpd->cjThis, ptmp->siInputSlot[i].szName);
            pntpd->cjThis += szLength(ptmp->siInputSlot[i].szName);

            pSlot[i].loSlotInvo = pntpd->cjThis;
            strcpy((CHAR *)pntpd + pntpd->cjThis,
                   ptmp->siInputSlot[i].szInvocation);
            pntpd->cjThis += szLength(ptmp->siInputSlot[i].szInvocation);

#if TESTING
	    DbgPrint("InputSlot[%d] = %s\n", i,
                     (CHAR *)pntpd + pSlot[i].loSlotName);
#endif
        }
    }

#if SIZE_TEST
    DbgPrint("post all input slots: cjThis = %d.\n", pntpd->cjThis);
#endif

    // add the manualfeedtrue string to the end of the structure.
    // check to make sure we found a string first.

    if (i = ptmp->cbManualTRUE)
    {
        pntpd->loszManualFeedTRUE = pntpd->cjThis;
        pntpd->cjThis += i;

        memcpy((CHAR *)pntpd + pntpd->loszManualFeedTRUE,
               ptmp->szManualTRUE, i);
    }

#if SIZE_TEST
    DbgPrint("post cbManualTRUE: cjThis = %d.\n", pntpd->cjThis);
#endif

#if TESTING
    if (i != 0)
	DbgPrint("ManualTRUE not found.\n");
    else
	DbgPrint("ManualTRUE = %s\n", ptmp->szManualTRUE);
#endif

    // add the manualfeedfalse string to the end of the structure.
    // check to make sure we found a string first.

    if (i = ptmp->cbManualFALSE)
    {
        pntpd->loszManualFeedFALSE = pntpd->cjThis;
        pntpd->cjThis += i;

        memcpy((CHAR *)pntpd + pntpd->loszManualFeedFALSE,
               ptmp->szManualFALSE, i);
    }

#if SIZE_TEST
    DbgPrint("post cbManualFALSE: cjThis = %d.\n", pntpd->cjThis);
#endif

#if TESTING
    if (i != 0)
	DbgPrint("ManualFALSE not found.\n");
    else
	DbgPrint("ManualFALSE = %s\n", ptmp->szManualFALSE);
#endif

    // add the duplex none string to the end of the structure.
    // check to make sure we found a string first.

    if (i = ptmp->cbDuplexNone)
    {
        pntpd->loszDuplexNone = pntpd->cjThis;
        pntpd->cjThis += i;

        memcpy((PSTR)pntpd + pntpd->loszDuplexNone, ptmp->szDuplexNone, i);
    }

#if SIZE_TEST
    DbgPrint("post cbDuplexNone: cjThis = %d.\n", pntpd->cjThis);
#endif

#if TESTING
    if (i != 0)
	DbgPrint("DuplexNone not found.\n");
    else
	DbgPrint("DuplexNone = %s\n", ptmp->szDuplexNone);
#endif

    // add the duplex tumble string to the end of the structure.
    // check to make sure we found a string first.

    if (i = ptmp->cbDuplexTumble)
    {
        pntpd->loszDuplexTumble = pntpd->cjThis;
        pntpd->cjThis += i;

        memcpy((PSTR)pntpd + pntpd->loszDuplexTumble, ptmp->szDuplexTumble, i);
    }

#if SIZE_TEST
    DbgPrint("post cbDuplexTumble: cjThis = %d.\n", pntpd->cjThis);
#endif

#if TESTING
    if (i != 0)
	DbgPrint("DuplexTumble not found.\n");
    else
	DbgPrint("DuplexTumble = %s\n", ptmp->szDuplexTumble);
#endif

    // add the duplex no tumble string to the end of the structure.
    // check to make sure we found a string first.

    if (i = ptmp->cbDuplexNoTumble)
    {
        pntpd->loszDuplexNoTumble = pntpd->cjThis;
        pntpd->cjThis += i;

        memcpy((PSTR)pntpd + pntpd->loszDuplexNoTumble, ptmp->szDuplexNoTumble, i);
    }

#if SIZE_TEST
    DbgPrint("post cbDuplexNoTumble: cjThis = %d.\n", pntpd->cjThis);
#endif

#if TESTING
    if (i != 0)
	DbgPrint("DuplexNoTumble not found.\n");
    else
	DbgPrint("DuplexNoTumble = %s\n", ptmp->szDuplexNoTumble);
#endif

    // add the collate on string to the end of the structure.
    // check to make sure we found a string first.

    if (i = ptmp->cbCollateOn)
    {
        pntpd->loszCollateOn = pntpd->cjThis;
        pntpd->cjThis += i;

        memcpy((PSTR)pntpd + pntpd->loszCollateOn, ptmp->szCollateOn, i);
    }

#if SIZE_TEST
    DbgPrint("post cbCollateOn: cjThis = %d.\n", pntpd->cjThis);
#endif

#if TESTING
    if (i != 0)
	DbgPrint("CollateOn not found.\n");
    else
	DbgPrint("CollateOn = %s\n", ptmp->szCollateOn);
#endif

    // add the collate off string to the end of the structure.
    // check to make sure we found a string first.

    if (i = ptmp->cbCollateOff)
    {
        pntpd->loszCollateOff = pntpd->cjThis;
        pntpd->cjThis += i;

        memcpy((PSTR)pntpd + pntpd->loszCollateOff, ptmp->szCollateOff, i);
    }

#if SIZE_TEST
    DbgPrint("post cbCollateOff: cjThis = %d.\n", pntpd->cjThis);
#endif

#if TESTING
    if (i != 0)
	DbgPrint("CollateOff not found.\n");
    else
	DbgPrint("CollateOff = %s\n", ptmp->szCollateOff);
#endif

    // now add the fonts to the end of the structure, an array of
    // pntpd->cFonts BYTES are stored at the end of the NTPD structure.

    // add the BYTE array to the end of the NTPD structure.

    pntpd->loFonts = pntpd->cjThis;
    pntpd->cjThis += pntpd->cFonts;

#if SIZE_TEST
    DbgPrint("post cFonts: cjThis = %d.\n", pntpd->cjThis);
#endif

    pfont = (BYTE *)pntpd + pntpd->loFonts;

    for (i = 0; i < (DWORD)pntpd->cFonts; i++)
    {
        pfont[i] = ptmp->bFonts[i];
#if TESTING
	DbgPrint("Font[%d] = %d\n", i, (int)pfont[i]);
#endif
    }
}


//--------------------------------------------------------------------------
// DWORD SizeNTPD(pntpd, ptmp)
// PNTPD       pntpd;
// PTMP_NTPD   ptmp;
//
// This routine determines the size of the NTPD structure for the
// given printer.
//
// Returns:
//   This routine returns the size of the NTPD structure in BYTES, or
// zero for error.
//
// History:
//   29-Sep-1992    -by-    Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

DWORD SizeNTPD(pntpd, ptmp)
PNTPD       pntpd;
PTMP_NTPD   ptmp;
{
    DWORD           i, dwSize;

    // start by adding the printer name to the end of the NTPD structure.

#if SIZE_TEST
    DbgPrint("Entering SizeNTPD.\n");
#endif

    dwSize = sizeof(NTPD);
    dwSize = WCHARALIGN(dwSize);
    dwSize += ptmp->cbPrinterName;
    dwSize = DWORDALIGN(dwSize);

#if SIZE_TEST
    DbgPrint("post cbPrinterName: dwSize = %d.\n", dwSize);
#endif

    // now add the set resolution strings to the end of the structure,
    // if there are any to add.  it is worth noting how these are stored
    // in the NTPD structure.  an array of cResolutions PSRESOLUTION
    // structures are stored at the end of the NTPD structure.  within
    // each PSRESOLUTION structure is an offset to the string corresponding
    // to the resolution in question.

    if (pntpd->cResolutions != 0)
    {
        // add the PSRESOLUTION array to the end of the NTPD structure.

        dwSize += pntpd->cResolutions * sizeof(PSRESOLUTION);

        // for each resolution, fill in the PSRESOLUTION structure.
        // then add the string itself to the end of the NTPD structure.

        for (i = 0; i < (DWORD)pntpd->cResolutions; i++)
            dwSize += (DWORD)szLength(ptmp->siResolutions[i].szString);
    }

#if SIZE_TEST
    DbgPrint("post resolutions: dwSize = %d.\n", dwSize);
#endif

    // add the transfernormalized string to the end of the structure.

    dwSize += ptmp->cbTransferNorm;

#if SIZE_TEST
    DbgPrint("post cbTransferNorm: dwSize = %d.\n", dwSize);
#endif

    // add the inverse transfernormalized string to the end of the structure.

    dwSize += ptmp->cbInvTransferNorm;

#if SIZE_TEST
    DbgPrint("post cbInvTransferNorm: dwSize = %d.\n", dwSize);
#endif

    // add the form names list to the end of the structure.

    dwSize += szLength(ptmp->szDefaultForm);
    dwSize = DWORDALIGN(dwSize);

#if SIZE_TEST
    DbgPrint("post szDefaultForm: dwSize = %d.\n", dwSize);
#endif

    dwSize += pntpd->cPSForms * sizeof(PSFORM);

#if SIZE_TEST
    DbgPrint("post cPSForms: dwSize = %d.\n", dwSize);
#endif

    // for each form, allow room for the FormName, SizeInvo, and
    // RegionInvo strings.

    for (i = 0; i < pntpd->cPSForms; i++)
    {
        // account for the form name and invocation strings.

        dwSize += szLength(ptmp->FormEntry[i].szName) +
                  szLength(ptmp->FormEntry[i].szInvocation);

    }

#if SIZE_TEST
    DbgPrint("post all forms: dwSize = %d.\n", dwSize);
#endif

    // make room for the page region invocation strings.

    for (i = 0; i < pntpd->cPageRegions; i++)
        dwSize += szLength(ptmp->PageRegion[i].szInvocation);

#if SIZE_TEST
    DbgPrint("post page regions: dwSize = %d.\n", dwSize);
#endif

    // now add the outputbin strings to the end of the structure,
    // if there are any to add.  it is worth noting how these are stored
    // in the NTPD structure.  an array of cOutputBins PSOUTPUTBIN
    // structures are stored at the end of the NTPD structure.  within
    // each PSOUTPUTBIN structure are offsets to the strings corresponding
    // to the output bin and invocation.  if there is only the default
    // outputbin defined, cOutputBins will be zero.  otherwise it is assumed
    // there will be at least two output bins defined.  if there is only one
    // defined, it will be the same as the default.

    dwSize += szLength(ptmp->szDefaultOutputBin);
    dwSize = DWORDALIGN(dwSize);

#if SIZE_TEST
    DbgPrint("post szDefaultOutputBin: dwSize = %d.\n", dwSize);
#endif

    if (pntpd->cOutputBins > 0)
    {
        dwSize += pntpd->cOutputBins * sizeof(PSOUTPUTBIN);

        // for each outputbin, fill in the PSOUTPUTBIN structure.

        for (i = 0; i < pntpd->cOutputBins; i++)
        {
            dwSize += szLength(ptmp->siOutputBin[i].szName);
            dwSize += szLength(ptmp->siOutputBin[i].szInvocation);
        }
    }

#if SIZE_TEST
    DbgPrint("post all output bins: dwSize = %d.\n", dwSize);
#endif

    // now add the inputslot strings to the end of the structure,
    // if there are any to add.  it is worth noting how these are stored
    // in the NTPD structure.  an array of cInputSlots PSINPUTSLOT
    // structures are stored at the end of the NTPD structure.  within
    // each PSINPUTSLOT structure are offsets to the strings corresponding
    // to the slot name and the invocation string.  if there is only the
    // default inputslot defined, cInputSlots will be zero.  otherwise it
    // is assumed there will be at least two input slots defined.  if there
    // is only one defined, it will be the same as the default.

    dwSize += szLength(ptmp->szDefaultInputSlot);
    dwSize = DWORDALIGN(dwSize);

#if SIZE_TEST
    DbgPrint("post szDefaultInputSlot: dwSize = %d.\n", dwSize);
#endif

    if (pntpd->cInputSlots > 0)
    {
        dwSize += pntpd->cInputSlots * sizeof(PSINPUTSLOT);

        // for each inputslot, fill in the PSINPUTSLOT structure.

        for (i = 0; i < pntpd->cInputSlots; i++)
        {
            dwSize += szLength(ptmp->siInputSlot[i].szName);
            dwSize += szLength(ptmp->siInputSlot[i].szInvocation);
        }
    }

#if SIZE_TEST
    DbgPrint("post all input slots: dwSize = %d.\n", dwSize);
#endif

    // add the manualfeed strings to the end of the structure.

    dwSize += ptmp->cbManualTRUE + ptmp->cbManualFALSE;

#if SIZE_TEST
    DbgPrint("post manual strings: dwSize = %d.\n", dwSize);
#endif

    // add the duplex strings.

    dwSize += ptmp->cbDuplexNone + ptmp->cbDuplexTumble +
              ptmp->cbDuplexNoTumble;

#if SIZE_TEST
    DbgPrint("post duplex strings: dwSize = %d.\n", dwSize);
#endif

    // add the collate strings.

    dwSize += ptmp->cbCollateOn + ptmp->cbCollateOff;

#if SIZE_TEST
    DbgPrint("post collate strings: dwSize = %d.\n", dwSize);
#endif

    // now add the fonts to the end of the structure,
    // it is worth noting how these are stored in the NTPD structure.
    // an array of pntpd->cFonts BYTES are stored at the end of the NTPD
    // structure.

    // add the BYTE array to the end of the NTPD structure.

    dwSize += pntpd->cFonts;

#if SIZE_TEST
DbgPrint("SizeNTPD - dwSize = %d.\n", dwSize);
#endif

    return(dwSize);
}


//--------------------------------------------------------------------------
//
// BOOL GetBuffer();
//
// This routines reads a new buffer full of text from the input file.
//
// Note: If the end of file is encountered in this function then
//     the program is aborted with an error message.    Normally
//     the program will stop processing the input when it sees
//     the end of information keyword.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns TRUE if end of file, FALSE otherwise.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from Windows 3.0, and cleaned up.
//--------------------------------------------------------------------------

BOOL GetBuffer(pdata)
PPARSEDATA  pdata;
{
    // initialize the buffer count to zero.

    pdata->cbBuffer = 0;

    // read in the next buffer full of data if we have not already hit the
    // end of file.

    if (!pdata->fEOF)
    {
        ReadFile(pdata->hFile, pdata->rgbBuffer, sizeof(pdata->rgbBuffer),
                 (LPDWORD)&pdata->cbBuffer, (LPOVERLAPPED)NULL);

        if (pdata->cbBuffer == 0)
            pdata->fEOF = TRUE;
    }

    pdata->pbBuffer = pdata->rgbBuffer;
    return(pdata->fEOF);
}


//--------------------------------------------------------------------------
//
// BOOL GetLine(pdata);
// PPARSEDATA   pdata;
//
// This routine gets the next line of text out of the input buffer.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns TRUE if end of file, FALSE otherwise.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from Windows 3.0, and cleaned up.
//--------------------------------------------------------------------------

BOOL GetLine(pdata)
PPARSEDATA  pdata;
{
    int cbLine;
    char bCh;

    if (pdata->fUnGetLine)
    {
        pdata->szLine = pdata->rgbLine;
        pdata->fUnGetLine = FALSE;
        return(FALSE);
    }

    cbLine = 0;
    pdata->szLine = pdata->rgbLine;
    *(pdata->szLine) = 0;

    if (!pdata->fEOF)
    {
        while(TRUE)
        {
            if (pdata->cbBuffer <= 0)
            {
                if (GetBuffer(pdata))    // done if end of file hit.
                break;
            }

            while(--pdata->cbBuffer >= 0)
            {
                bCh = *(pdata->pbBuffer++);
                if (bCh == '\n' || bCh == '\r' || ++cbLine > sizeof(pdata->rgbLine))
                {
                    *(pdata->szLine) = 0;
                    pdata->szLine = pdata->rgbLine;
                    EatWhite(pdata);
                    if (*(pdata->szLine) != 0)
                    {
                        pdata->szLine = pdata->rgbLine;
                        return(pdata->fEOF);
                    }

                    pdata->szLine = pdata->rgbLine;
                    cbLine = 0;
                    continue;
                }

                *(pdata->szLine++) = bCh;
            }
        }
    }

    *(pdata->szLine) = 0;

    pdata->szLine = pdata->rgbLine;
    return(pdata->fEOF);
}


//--------------------------------------------------------------------------
//

// VOID UnGetLine(pdata)
// PPARSEDATA  pdata;
//
// This routine pushes the most recent line back into the input buffer.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns no value.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from Windows 3.0, and cleaned up.
//--------------------------------------------------------------------------

VOID UnGetLine(pdata)
PPARSEDATA  pdata;
{
    pdata->fUnGetLine = TRUE;
    pdata->szLine = pdata->rgbLine;
}


//--------------------------------------------------------------------------
//
// int GetKeyword(pTable, pdata)
// TABLE_ENTRY    *pTable;
// PPARSEDATA      pdata;
//
// Get the next token from the input stream.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns integer value of next token.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from Windows 3.0, and cleaned up.
//--------------------------------------------------------------------------

int GetKeyword(pTable, pdata)
TABLE_ENTRY    *pTable;
PPARSEDATA      pdata;
{
    char szWord[256];

    if (*(pdata->szLine) == 0)
        if (GetLine(pdata))
            return(TK_EOF);

    GetWord(szWord, sizeof(szWord), pdata);
    return(MapToken(szWord, pTable));
}


//--------------------------------------------------------------------------
// VOID GetOptionString(pstrOptionName, cbBuffer, pdata)
// PSTR        pstrOptionName;
// DWORD       cbBuffer;
// PPARSEDATA  pdata;
//
// This routine fills in the option name of the next option.
//
// Parameters:
//   pstrOptionName - place to put option name.
//
//   cbBuffer - size of buffer.
//
// Returns:
//   This routine returns no value.
//
// History:
//   08-Apr-1992    -by-    Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

VOID GetOptionString(pstrOptionName, cbBuffer, pdata)
PSTR        pstrOptionName;
DWORD       cbBuffer;
PPARSEDATA  pdata;
{
    if (*(pdata->szLine) == 0)
        if (GetLine(pdata))
            return;

    EatWhite(pdata);

    // copy the form name until the ':' deliminator is encountered.

    while (cbBuffer--)
    {
        *pstrOptionName = *(pdata->szLine++);

        if ((*pstrOptionName == ':') || (*pstrOptionName == '\0'))
        {
            *pstrOptionName = '\0';  // add the zero terminator.
            break;
        }

        pstrOptionName++;
    }

    // strip off any trailing spaces, 'cause some people just can't
    // follow the spec.

    pstrOptionName--;

    if ((*pstrOptionName == ' ') || (*pstrOptionName == '\t'))
    {
        while ((*pstrOptionName == ' ') || (*pstrOptionName == '\t'))
            *pstrOptionName-- = '\0';
    }

    return;
}


//--------------------------------------------------------------------------
// int GetOptionIndex(pTable, pdata)
// TABLE_ENTRY    *pTable;
// PPARSEDATA      pdata;
//
// Get the next token from the input stream.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns integer value of next token.
//
// History:
//   08-Apr-1991    -by-    Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

int GetOptionIndex(pTable, pdata)
TABLE_ENTRY    *pTable;
PPARSEDATA      pdata;
{
    char    szWord[256];
    int     cbWord;
    char   *pszWord;

    if (*(pdata->szLine) == 0)
        if (GetLine(pdata))
            return(TK_EOF);

    EatWhite(pdata);

    cbWord = sizeof(szWord);
    pszWord = szWord;

    while (--cbWord > 0)
    {
        *pszWord = *(pdata->szLine++);

        // search to the end of the option.  this could be either the
        // colon, which ends the option, or the slash, which begins the
        // translation string (which we will ignore).

        if ((*pszWord == ':') || (*pszWord == '/'))
        {
            *pszWord = 0;
            break;
        }

        pszWord++;
    }

    return(MapToken(szWord, pTable));
}


//--------------------------------------------------------------------------
//
// int MapToken(szWord, pTable)
// char           *szWord;        // Ptr to the ascii keyword string
// TABLE_ENTRY    *pTable;
//
// This routine maps an ascii key word into an integer token.
//
// Parameters:
//   szWord
//     Pointer to the ascii keyword string.
//
// Returns:
//   This routine returns int identifying token.
//
// History:
//   03-Apr-1991    -by-    Kent Settle    (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

int MapToken(szWord, pTable)
char           *szWord;        // Ptr to the ascii keyword string
TABLE_ENTRY    *pTable;
{
    while (pTable->szStr)
    {
        if (szIsEqual(szWord, pTable->szStr))
            return(pTable->iValue);

        ++pTable;
    }

#if TESTING
    DbgPrint("MapToken could not map %s.\n", szWord);
#endif
    return(TK_UNDEFINED);
}


//--------------------------------------------------------------------------
// VOID GetWord(szWord, cbWord, pdata)
// char       *szWord;        // Ptr to the destination area
// int         cbWord;        // The size of the destination area
// PPARSEDATA  pdata;
//
// This routine gets the next word delimited by white space
// from the input buffer.
//
// Parameters:
//   szWord
//     Pointer to the destination area.
//
//   cbWord
//     Size of destination area.
//
// Returns:
//   This routine returns no value.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from Windows 3.0, and cleaned up.
//--------------------------------------------------------------------------

VOID GetWord(szWord, cbWord, pdata)
char       *szWord;        // Ptr to the destination area
int         cbWord;        // The size of the destination area
PPARSEDATA  pdata;
{
    char bCh;

    EatWhite(pdata);
    while (cbWord--)
    {
        switch(bCh = *(pdata->szLine++))
        {
            case 0:
            case ' ':
            case '\t':
            case '\n':     // take care of newline and carriage returns.
            case '\r':
                --pdata->szLine;
                *szWord = 0;
                return;
            case ':':       // the colon is a delimeter in PPD files,
                break;      // and should simply be skipped over.
            default:
                *szWord++ = bCh;
                break;
        }
    }

    *szWord = 0;
}


//--------------------------------------------------------------------------
// int GetString(szDst, pdata)
// char       *szDst;
// PPARSEDATA  pdata;
//
// This routine gets a " bracketed string from the ppd_file, attaching
// a zero terminator to it.
//
// Returns:
//   This routine returns the length of the string, including the zero
//   terminator.
//
// History:
//   03-Apr-1991    -by-    Kent Settle    (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

int GetString(szDst, pdata)
char       *szDst;
PPARSEDATA  pdata;
{
    int     i;

    // advance to the first quotation mark, then one character past it.

    while (*(pdata->szLine) != '"')
        pdata->szLine++;
    pdata->szLine++;

    // initialize string length counter to include zero terminator.

    i = 1;

    // copy the string itself.  be sure to ignore ppd file comments (#).

    while (*(pdata->szLine) && *(pdata->szLine) != '"' &&
           *(pdata->szLine) != '%')
    {
        *szDst++ = *(pdata->szLine++);
        i++;
    }

    // get the next line if the string is longer than one line.

    if (*(pdata->szLine) != '"')
    {
        while (!(GetLine(pdata)))
        {
            while (*(pdata->szLine) && *(pdata->szLine) != '"' &&
                   *(pdata->szLine) != '%')
            {
                *szDst++ = *(pdata->szLine++);
                i++;
            }

            if (*(pdata->szLine) == '"')
                break;

            // how 'bout a new line.

            *szDst++ = '\n';
            i++;
        }
    }

    // add the zero terminator.

    *szDst = 0;

    // return the length of the string.

    return (i);
}


//--------------------------------------------------------------------------
// VOID EatWhite(pdata)
// PPARSEDATA  pdata;
//
// This routine moves the input buffer pointer forward to the
// next non-white character.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns TRUE if end of file, FALSE otherwise.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from Windows 3.0, and cleaned up.
//--------------------------------------------------------------------------

VOID EatWhite(pdata)
PPARSEDATA  pdata;
{
    while (TRUE)
    {
        // skip to the next line if necessary.

        if ((*(pdata->szLine) == '\0') || (*(pdata->szLine) == '\n') ||
            (*(pdata->szLine) == '\r'))
            GetLine(pdata);

        // we are done if we hit a non-white character.

        if ((*(pdata->szLine) != ' ') && (*(pdata->szLine) != '\t'))
            break;

        pdata->szLine++;
    }
}


//--------------------------------------------------------------------------
// int GetNumber(pdata)
// PPARSEDATA  pdata;
//
// This routine parses an ASCII decimal number from the
// input file stream and returns its value.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns integer value of ASCII decimal number.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from Windows 3.0, and cleaned up.
//--------------------------------------------------------------------------

int GetNumber(pdata)
PPARSEDATA  pdata;
{
    int iVal;
    BOOL fNegative;

    fNegative = FALSE;

    iVal = 0;

    EatWhite(pdata);

    // skip quotation mark if number is in quotes.

    if (*(pdata->szLine) == '"')
    {
        pdata->szLine++;
        EatWhite(pdata);    // necessary if " 0 1".
    }

    if (*(pdata->szLine) == '-')
    {
        fNegative = TRUE;
        ++(pdata->szLine);
    }

    // handle the case where the value is '.2'.  make it zero.

    if (*(pdata->szLine) == '.')
    {
        // skip all the fractional digits.

        while ((*(pdata->szLine)) && (*(pdata->szLine) != ' ') &&
               (*(pdata->szLine) != '\t') && (*(pdata->szLine) != '"'))
            pdata->szLine++;

        return(0);
    }

    if (*(pdata->szLine) < '0' || *(pdata->szLine) > '9')
    {
        RIP("PSCRIPT!GetNumber:  invalid number found.  things will go bad from here.\n");
        return(0);
    }

    while (*(pdata->szLine) >= '0' && *(pdata->szLine) <= '9')
        iVal = iVal * 10 + (*(pdata->szLine++) - '0');

    // some .PPD files, which will not be mentioned do NOT follow
    // the Adobe spec, and put non-integer values where they
    // do not belong.  therefore, if we hit a non-integer value,
    // simply lop off the fraction.

    if (*(pdata->szLine) == '.')
    {
	// just skip along until we hit some white space.

        while ((*(pdata->szLine)) && (*(pdata->szLine) != ' ') &&
               (*(pdata->szLine) != '\t') && (*(pdata->szLine) != '"'))
	    pdata->szLine++;
    }

    if (fNegative)
        iVal = - iVal;

    return(iVal);
}


//--------------------------------------------------------------------------
// int GetFloat(iScale, pdata)
// int         iScale;        // The amount to scale the value by
// PPARSEDATA  pdata;
//
// This routine parses an ASCII floating point decimal number from the
// input file stream and returns its value scaled by a specified amount.
//
// Parameters:
//   None.
//
// Returns:
//   This routine returns integer value of ASCII decimal number.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from Windows 3.0, and cleaned up.
//--------------------------------------------------------------------------

int GetFloat(iScale, pdata)
int         iScale;        // The amount to scale the value by
PPARSEDATA  pdata;
{
    long lVal;
    long lDivisor;
    BOOL fNegative;

    EatWhite(pdata);

    fNegative = FALSE;
    lVal = 0L;

    if (*(pdata->szLine) == '-')
    {
        fNegative = TRUE;
        pdata->szLine++;
    }

    if (*(pdata->szLine) < '0' || *(pdata->szLine) > '9')
    {
        RIP("PSCRIPT!GetFloat: invalid number.\n");
        return(0);
    }

    while (*(pdata->szLine) >= '0' && *(pdata->szLine) <= '9')
        lVal = lVal * 10 + (*(pdata->szLine++) - '0');

    lDivisor = 1L;
    if (*(pdata->szLine) == '.')
    {
        pdata->szLine++;
        while (*(pdata->szLine) >= '0' && *(pdata->szLine) <= '9')
        {
            lVal = lVal * 10 + (*(pdata->szLine++) - '0');
            lDivisor = lDivisor * 10;
        }
    }
    lVal = (lVal * iScale) / lDivisor;

    if (fNegative)
        lVal = - lVal;

    return((short)lVal);
}


//--------------------------------------------------------------------------
// void GetDimension(pdim, pdata)
// PAPERDIM   *pdim;
// PPARSEDATA  pdata;
//
// This routine extracts the paper dimension from the ppd file.
//
// Returns:
//   This routine returns no value.
//
// History:
//   03-Apr-1991    -by-    Kent Settle     (kentse)
//  Rewrote it.
//   25-Mar-1991    -by-    Kent Settle    (kentse)
//  Stole from Windows 3.0, and cleaned up.
//--------------------------------------------------------------------------

void GetDimension(pdim, pdata)
PAPERDIM   *pdim;
PPARSEDATA  pdata;
{
    pdim->sizl.cx = GetNumber(pdata);
    EatWhite(pdata);
    pdim->sizl.cy = GetNumber(pdata);
}


//--------------------------------------------------------------------------
// void GetImageableArea(prect, pdata)
// RECTL      *prect;
// PPARSEDATA  pdata;
//
// This routine extracts the imageable area from the ppd file.
//
// Returns:
//   This routine returns no value.
//
// History:
//   03-Apr-1991    -by-    Kent Settle     (kentse)
//  Rewrote it.
//   25-Mar-1991    -by-    Kent Settle    (kentse)
//  Stole from Windows 3.0, and cleaned up.
//--------------------------------------------------------------------------

void GetImageableArea(prect, pdata)
RECTL      *prect;
PPARSEDATA  pdata;
{
    prect->left = GetNumber(pdata);
    prect->bottom = GetNumber(pdata);
    prect->right = GetNumber(pdata);
    prect->top = GetNumber(pdata);
}


//--------------------------------------------------------------------
// szLength(pszScan)
//
// This routine calculates the length of a given string, including
// the terminating NULL.  This routine checks to make sure the
// string is not longer than MAX_STRING.
//
// History:
//   19-Mar-1991        -by-    Kent Settle     (kentse)
// Created.
//--------------------------------------------------------------------

int szLength(pszScan)
char    *pszScan;
{
    int i;
    char *pszTmp;

    pszTmp = pszScan;

    i = 1;
    while (*pszScan++ != '\0')
        i++;

    // do a little internal checking.

    if (i > MAX_PPD_STRING)
    {
	DbgPrint("String Length too long!\n");
	DbgPrint("Offending String: \"%s\"", pszTmp);
        RIP("PSCRIPT!szLength:  about to overrun buffer.\n");
    }

    return(i);
}


//--------------------------------------------------------------------------
//
// BOOL szIsEqual(sz1, sz2)
// char *sz1;
// char *sz2;
//
// This routine compares two NULL terminated strings.
//
// Parameters:
//   sz1
//     Pointer to string 1.
//
//   sz2
//     Pointer to string2.
//
// Returns:
//   This routine returns TRUE if strings are same, FALSE otherwise.
//
// History:
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from Windows 3.0, and cleaned up.
//--------------------------------------------------------------------------

BOOL szIsEqual(sz1, sz2)
char *sz1;
char *sz2;
{
    while (*sz1 && *sz2)
    {
        if (*sz1++ != *sz2++)
            return(FALSE);
    }

    return(*sz1 == *sz2);
}


//--------------------------------------------------------------------------
// int NameComp(pname1, pname2)
// CHAR   *pname1;
// CHAR   *pname2;
//
// This routine is a glorified version of strcmp, in that it first gets
// rid of any PostScript translation strings before comparing the strings.
//
// Returns same as strcmp.
//
// History:
//   20-Mar-1993    -by-    Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

int NameComp(pname1, pname2)
CHAR   *pname1;
CHAR   *pname2;
{
    CHAR    buf1[MAX_PS_NAME];
    CHAR    buf2[MAX_PS_NAME];
    PSTR    pstr1, pstr2;

    // loop through the first name.  copy it into a buffer until we hit
    // either the NULL terminator, or the '/' translation string deliminator.

    pstr1 = pname1;
    pstr2 = buf1;

    while (*pstr1 && (*pstr1 != '/'))
        *pstr2++ = *pstr1++;

    *pstr2 = '\0';

    // now do the same for the second name.

    pstr1 = pname2;
    pstr2 = buf2;

    while (*pstr1 && (*pstr1 != '/'))
        *pstr2++ = *pstr1++;

    *pstr2 = '\0';

    // now both buffers contain the names with any translation strings removed.

    return(strcmp(buf1, buf2));
}


//--------------------------------------------------------------------------
// int NameComp(pname1, pname2)
// CHAR   *pname1;
// CHAR   *pname2;
//
// This routine is a glorified version of strcmp, in that it first gets
// rid of any PostScript translation strings before comparing the strings.
//
// Returns same as strcmp.
//
// History:
//   20-Mar-1993    -by-    Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

VOID ParseProtocols(pdata, pntpd)
PPARSEDATA      pdata;
PNTPD           pntpd;
{
    CHAR    buf[256];
    CHAR   *pbuf;
    BOOL    bMore, bWordDone;
    CHAR    jCh;
    DWORD   cjBuf;

    // there may be several protocols defined, so loop until we get
    // them all.

    bMore = TRUE;

    while (bMore)
    {
        // gobble up any white space.

        EatWhite(pdata);

        pbuf = buf;
        cjBuf = sizeof(buf) - 1;

        bWordDone = FALSE;

        while(cjBuf--)
        {
            switch(jCh = *(pdata->szLine++))
            {
                case 0:
                case '\n':     // take care of newline and carriage returns.
                case '\r':
                    --pdata->szLine;
                    *pbuf = 0;
                    bMore = FALSE;
                    bWordDone = TRUE;
                    break;
                case ' ':
                case '\t':
                    --pdata->szLine;
                    *pbuf = 0;
                    bWordDone = TRUE;
                    break;
                default:
                    *pbuf++ = jCh;
                    break;
            }

            if (bWordDone)
            {
                // reset pointer to start of buffer.

                pbuf = buf;
                cjBuf = sizeof(buf) - 1;

                if (!(strncmp(pbuf, "PJL", 3)))
                {
                    pntpd->flFlags |= PJL_PROTOCOL;
#if TESTING
                    DbgPrint("Device supports PJL protocol.\n");
#endif
                }
                else if (!(strncmp(pbuf, "SIC", 3)))
                {
                    pntpd->flFlags |= SIC_PROTOCOL;
#if TESTING
                    DbgPrint("Device supports SIC protocol.\n");
#endif
                }

                // now go see if there is another protocol to parse.

                break;
            }
        }

    }
}
