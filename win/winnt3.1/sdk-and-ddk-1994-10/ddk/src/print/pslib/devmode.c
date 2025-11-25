//--------------------------------------------------------------------------
//
// Module Name:  DEVMODE.C
//
// Brief Description:  This module contains the PSCRIPT driver's User
// Default DEVMODE setting routine
//
// Author:  Kent Settle (kentse)
// Created: 12-Dec-1992
//
// Copyright (c) 1992 Microsoft Corporation
//--------------------------------------------------------------------------

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "pscript.h"
#include "enable.h"

static  COLORADJUSTMENT defcoloradj =
{
    sizeof(COLORADJUSTMENT),
    0,
    ILLUMINANT_DEVICE_DEFAULT,
    20000,
    20000,
    20000,
    REFERENCE_BLACK_MIN,
    REFERENCE_WHITE_MAX,
    0,
    0,
    0,
    0
};

VOID SetFormSize(HANDLE, PSDEVMODE *, PWSTR);
VOID SetFormName(HANDLE, PSDEVMODE *, int);

//--------------------------------------------------------------------------
// BOOL SetDefaultPSDEVMODE(pdevmode, pDeviceName, pntpd)
// PSDEVMODE  *pdevmode;
// PSTR        pDeviceName;
// PNTPD       pntpd;
//
// Given a pointer to a PSDEVMODE structure, a pointer to the current
// device name, and a pointer to the current NTPD structure, this routine
// fills in the default PSDEVMODE structure.
//
// History:
//   12-Dec-1992    -by-    Kent Settle     (kentse)
//  Broke out of PSCRPTUI and PSCRIPT.
//   15-Apr-1992    -by-    Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

BOOL SetDefaultPSDEVMODE(pdevmode, pDeviceName, pntpd, hPrinter)
PSDEVMODE  *pdevmode;
PWSTR       pDeviceName;
PNTPD       pntpd;
HANDLE      hPrinter;
{
    WCHAR   FormName[CCHFORMNAME];
    int     i;
    PSTR    pstr;

    memset(pdevmode, 0, sizeof(PSDEVMODE));

    wcsncpy((PWSTR)&pdevmode->dm.dmDeviceName, pDeviceName, CCHDEVICENAME);

    pdevmode->dm.dmDriverVersion = DRIVER_VERSION;
    pdevmode->dm.dmSpecVersion = DM_SPECVERSION;
    pdevmode->dm.dmSize = sizeof(DEVMODE);
    pdevmode->dm.dmDriverExtra = (sizeof(PSDEVMODE) - sizeof(DEVMODE));
    pdevmode->dm.dmOrientation = DMORIENT_PORTRAIT;
    pdevmode->dm.dmDuplex = DMDUP_SIMPLEX;
    pdevmode->dm.dmCollate = DMCOLLATE_FALSE;
    pdevmode->dm.dmTTOption = DMTT_SUBDEV;

    pstr = (CHAR *)pntpd + pntpd->loDefaultForm;
    i = strlen(pstr) + 1;

    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pstr, i, (LPWSTR)FormName, i);
    wcsncpy((PWSTR)&pdevmode->dm.dmFormName, FormName, CCHFORMNAME);

    SetFormSize(hPrinter, pdevmode, FormName);

    pdevmode->dm.dmScale = 100;
    pdevmode->dm.dmCopies = 1;

    pdevmode->dm.dmPrintQuality = (SHORT)pntpd->iDefResolution;

    if (pntpd->flFlags & COLOR_DEVICE)
        pdevmode->dm.dmColor = DMCOLOR_COLOR;
    else
        pdevmode->dm.dmColor = DMCOLOR_MONOCHROME;

    pdevmode->dm.dmFields = DM_ORIENTATION | DM_PAPERSIZE | DM_SCALE |
                           DM_COPIES | DM_PRINTQUALITY | DM_COLOR |
                           DM_FORMNAME | DM_TTOPTION | DM_COLLATE;

    // state that we support duplex only if the printer really does.

    if ((pntpd->loszDuplexNone) || (pntpd->loszDuplexNoTumble) ||
        (pntpd->loszDuplexTumble))
        pdevmode->dm.dmFields |= DM_DUPLEX;

    // fill in default driver data.

    pdevmode->dwPrivDATA = PRIVATE_DEVMODE_ID;
    pdevmode->dwFlags = PSDEVMODE_FONTSUBST;
    pdevmode->coloradj = defcoloradj;

    return(TRUE);
}


//--------------------------------------------------------------------------
// BOOL ValidateSetDEVMODE(pdevmodeT, pdevmodeS, hPrinter, pntpd)
// PSDEVMODE  *pdevmodeT;
// PSDEVMODE  *pdevmodeS;
// HANDLE      hPrinter;
// PNTPD       pntpd;
//
// This routine validates any sources DEVMODE fields designated, and
// copies them to the target DEVMODE.
//
// Parameters
//   pdevmodeT:
//     Pointer to target DEVMODE.
//
//   pdevmodeS:
//     Pointer to source DEVMODE.
//
//   hPrinter:
//     Handle to the printer.
//
//   pntpd:
//     Pointer to printer descriptor NTPD structure.
//
// Returns
//   This function returns TRUE if successful, FALSE otherwise.
//
// History:
//   14-Dec-1992    -by-    Kent Settle     (kentse)
// Moved from ..\pscript\enable.c, and generalized.
//   05-Aug-1991    -by-    Kent Settle     (kentse)
// Rewrote it.
//   24-Jan-1991    -by-    Kent Settle    (kentse)
// Wrote it.
//--------------------------------------------------------------------------

BOOL ValidateSetDEVMODE(pdevmodeT, pdevmodeS, hPrinter, pntpd)
PSDEVMODE  *pdevmodeT;
PSDEVMODE  *pdevmodeS;
HANDLE      hPrinter;
PNTPD       pntpd;
{
    int             i;
    PSRESOLUTION   *pRes;
    BOOL            bDuplex, bFound;
    DWORD           cbNeeded, cReturned;
    FORM_INFO_1    *pdbForm, *pdbForms;
    PWSTR           pwstrFormName;

    // verify a bunch of stuff in the DEVMODE structure.  if each item
    // selected by the user is valid, then set it in our DEVMODE
    // structure.

    // if we have a NULL source, then we have nothing to do.

    if (pdevmodeS == (LPPSDEVMODE)NULL)
        return(TRUE);

    // set the new orientation if its field is set, and the new
    // orientation is valid.

    if (pdevmodeS->dm.dmFields & DM_ORIENTATION)
    {
        // validate the new orientation.

        if ((pdevmodeS->dm.dmOrientation != DMORIENT_PORTRAIT) &&
            (pdevmodeS->dm.dmOrientation != DMORIENT_LANDSCAPE))
            pdevmodeT->dm.dmOrientation = DMORIENT_PORTRAIT;
        else
            pdevmodeT->dm.dmOrientation = pdevmodeS->dm.dmOrientation;
    }

    // if both the paper length and width fields are set and the
    // corresponding values are valid, use these values to choose the
    // form.  if not, and the paper size field is set, use that value.
    // if neither of these is used, check the form name.

    bFound = FALSE;

    if ((pdevmodeS->dm.dmFields & DM_PAPERLENGTH) &&
        (pdevmodeS->dm.dmFields & DM_PAPERWIDTH))
    {
        if (!pdevmodeS->dm.dmPaperLength || !pdevmodeS->dm.dmPaperWidth)
        {
            pdevmodeT->dm.dmPaperLength = 0;
            pdevmodeT->dm.dmPaperWidth = 0;
        }
        else
        {
            pdevmodeT->dm.dmPaperLength = pdevmodeS->dm.dmPaperLength;
            pdevmodeT->dm.dmPaperWidth = pdevmodeS->dm.dmPaperWidth;
            bFound = TRUE;
        }
    }

    // set the new paper size if its field is set.
    //!!! should we have some type of size checking???

    if ((pdevmodeS->dm.dmFields & DM_PAPERSIZE) && (!bFound))
    {
        pdevmodeT->dm.dmPaperSize = pdevmodeS->dm.dmPaperSize;

        // protect ourselves.

        if ((pdevmodeT->dm.dmPaperSize < DMPAPER_FIRST) ||
            (pdevmodeT->dm.dmPaperSize > DMPAPER_LAST))
            pdevmodeT->dm.dmPaperSize = DMPAPER_FIRST;

        // we need to keep the formname and paper size index in sync.

        SetFormName(hPrinter, pdevmodeT, pdevmodeT->dm.dmPaperSize);
        bFound = TRUE;
    }

    // validate the form name.

    if ((pdevmodeS->dm.dmFields & DM_FORMNAME) && (!bFound))
    {
        pdbForms = (FORM_INFO_1 *)NULL;

        pwstrFormName = (PWSTR)pdevmodeS->dm.dmFormName;

        // enumerate the forms database to make sure the user supplied
        // form is valid.  if it cannot be found, set to the default.

        if (!EnumForms(hPrinter, 1, NULL, 0, &cbNeeded, &cReturned))
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                if (pdbForms = (PFORM_INFO_1)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                                                         cbNeeded))
                {
                    if (EnumForms(hPrinter, 1, (LPBYTE)pdbForms,
                                  cbNeeded, &cbNeeded, &cReturned))
                    {
                        // search each form in the database for a matching name,
                        // when it has been found, then search the forms supported
                        // by the printer to make sure the printer can print it.

                        bFound = FALSE;
                        pdbForm = pdbForms;

                        for (i = 0; i < (int)cReturned; i++)
                        {
                            if (!(wcscmp((PWSTR)pdbForm->pName, pwstrFormName)))
                            {
                                bFound = TRUE;
                                wcscpy((PWSTR)pdevmodeT->dm.dmFormName, pwstrFormName);
                                break;
                            }

                            // search the next form in the database.

                            pdbForm++;
                        }

                        if (!bFound)
                        {
                            // could not find the user supplied name in
                            // the forms database, so set to default.

                            strcpy2WChar((PWSTR)pdevmodeT->dm.dmFormName,
                                         (CHAR *)pntpd + pntpd->loDefaultForm);
                        }
                    }
                }
            }
        }
        else
        {
            // could not find the user supplied name in
            // the forms database, so set to default.

            strcpy2WChar((PWSTR)pdevmodeT->dm.dmFormName,
                         (CHAR *)pntpd + pntpd->loDefaultForm);
        }

        // we need to keep the formname and paper size index in sync.

        SetFormSize(hPrinter, pdevmodeT, pdevmodeT->dm.dmFormName);

        if(pdbForms)
            GlobalFree((HGLOBAL)pdbForms);
    }

    if (pdevmodeS->dm.dmFields & DM_SCALE)
    {
        if ((pdevmodeS->dm.dmScale < MIN_SCALE) ||
            (pdevmodeS->dm.dmScale > MAX_SCALE))
            pdevmodeT->dm.dmScale = 100;
        else
            pdevmodeT->dm.dmScale = pdevmodeS->dm.dmScale;
    }

    if (pdevmodeS->dm.dmFields & DM_COPIES)
    {
        if ((pdevmodeS->dm.dmCopies < MIN_COPIES) ||
            (pdevmodeS->dm.dmCopies > MAX_COPIES))
            pdevmodeT->dm.dmCopies = 1;
        else
            pdevmodeT->dm.dmCopies = pdevmodeS->dm.dmCopies;
    }

    // update the print quality field, if it has been selected.
    // this basically translates to resolution.

    if (pdevmodeS->dm.dmFields & DM_PRINTQUALITY)
    {
        // if cResolutions == 0, then only the default resolutions is valid.

        pdevmodeT->dm.dmPrintQuality = (SHORT)pntpd->iDefResolution;

        if ((pntpd->cResolutions > 0) && (pdevmodeS->dm.dmPrintQuality > 0))
        {
            // the current device supports multiple resolutions, so make
            // sure that the user has selected one of them.

            pRes = (PSRESOLUTION *)((CHAR *)pntpd + pntpd->loResolution);

            for (i = 0; i < (int)pntpd->cResolutions; i++)
            {
                if ((pdevmodeS->dm.dmPrintQuality == (SHORT)pRes++->iValue))
                {
                    // we did find it, so overwrite the default value.

                    pdevmodeT->dm.dmPrintQuality = pdevmodeS->dm.dmPrintQuality;
                    break;
                }
            }
        }
    }

    // check the color flag.

    if (pdevmodeS->dm.dmFields & DM_COLOR)
    {
        // if the user has selected color on a color device print in color.
        // otherwise print in monochrome.

        if ((pntpd->flFlags & COLOR_DEVICE) &&
            (pdevmodeS->dm.dmColor == DMCOLOR_COLOR))
            pdevmodeT->dm.dmColor = DMCOLOR_COLOR;
        else
            pdevmodeT->dm.dmColor = DMCOLOR_MONOCHROME;
    }

    // check to see if the device handles duplex.

    if ((pntpd->loszDuplexNone) || (pntpd->loszDuplexNoTumble) ||
        (pntpd->loszDuplexTumble))
        bDuplex = TRUE;
    else
        bDuplex = FALSE;

    if (pdevmodeS->dm.dmFields & DM_DUPLEX)
    {
        if ((!(bDuplex)) ||
            ((pdevmodeS->dm.dmDuplex != DMDUP_SIMPLEX) &&
             (pdevmodeS->dm.dmDuplex != DMDUP_HORIZONTAL) &&
             (pdevmodeS->dm.dmDuplex != DMDUP_VERTICAL)))
            pdevmodeT->dm.dmDuplex = DMDUP_SIMPLEX;
        else
            pdevmodeT->dm.dmDuplex = pdevmodeS->dm.dmDuplex;
    }

    if (pdevmodeS->dm.dmFields & DM_COLLATE)
    {
        if ((!(pntpd->loszCollateOn)) ||
            ((pdevmodeS->dm.dmCollate != DMCOLLATE_TRUE) &&
             (pdevmodeS->dm.dmCollate != DMCOLLATE_FALSE)))
            pdevmodeT->dm.dmCollate = DMCOLLATE_FALSE;
        else
            pdevmodeT->dm.dmCollate = DMCOLLATE_TRUE;
    }

    // handle the driver specific data.  make sure it is ours.

    if ((pdevmodeS->dm.dmDriverExtra != 0) &&
        (pdevmodeS->dwPrivDATA == PRIVATE_DEVMODE_ID))
    {
        pdevmodeT->dwPrivDATA = PRIVATE_DEVMODE_ID;
        pdevmodeT->dwFlags = pdevmodeS->dwFlags;

        wcsncpy(pdevmodeT->wstrEPSFile, pdevmodeS->wstrEPSFile,
                (sizeof(pdevmodeT->wstrEPSFile) / sizeof(WCHAR)));

        pdevmodeT->coloradj = pdevmodeS->coloradj;
    }

    return(TRUE);
}



//--------------------------------------------------------------------------
// VOID SetFormSize(pdevmode, FormName);
// PSDEVMODE  *pdevmode;
// PWSTR       FormName;
//
// This routine sets the dmPaperSize, dmPaperLength, and dmPaperWidth fields
// of the DEVMODE structure, based on FormName.
//
// Parameters
//   pdevmode:
//     Pointer to DEVMODE to be modified.
//
//   FormName:
//     UNICODE formname.
//
//
// Returns
//   This function returns NO VALUE.
//
// Rewrote it.
//   22-Mar-1993    -by-    Kent Settle     (kentse)
// Wrote it.
//--------------------------------------------------------------------------

VOID SetFormSize(hPrinter, pdevmode, FormName)
HANDLE      hPrinter;
PSDEVMODE  *pdevmode;
PWSTR       FormName;
{
    int             iForm;
    DWORD           cbNeeded, cReturned;
    FORM_INFO_1    *pdbForm, *pdbForms;
    BOOL            bFound;
    SHORT           Length, Width;

    // enumerate the forms database.  then locate the index of the form
    // within the database.

    pdbForms = (FORM_INFO_1 *)NULL;

    if (!EnumForms(hPrinter, 1, NULL, 0, &cbNeeded, &cReturned))
    {
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            if (pdbForms = (PFORM_INFO_1)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                                                     cbNeeded))
            {
                if (EnumForms(hPrinter, 1, (LPBYTE)pdbForms,
                              cbNeeded, &cbNeeded, &cReturned))
                {
                    // search the forms in the database.  if the form is found
                    // within the first DMPAPER_LAST forms, then we have a
                    // predefined form, otherwise it is user defined.

                    bFound = FALSE;
                    pdbForm = pdbForms;
                    cReturned = min(cReturned, DMPAPER_LAST);

                    for (iForm = 1; iForm <= (int)cReturned; iForm++)
                    {
                        if (!(wcscmp((PWSTR)pdbForm->pName, FormName)))
                        {
                            // ah-ha!  iForm is the index of the form.

                            bFound = TRUE;
                            break;
                        }

                        // search the next form in the database.

                        pdbForm++;
                    }

                    if (!bFound)
                    {
                        // must be a user defined form.

                        iForm = DMPAPER_USER;
                    }
                }
            }
        }
    }


    if (iForm == DMPAPER_USER)
    {
        // we have a user defined form.  set the length and width of the
        // form from the forms database.  these are defined in .001mm in
        // the database, and the DEVMODE wants .1mm.

        Length = pdbForm->Size.cy / 100;
        Width = pdbForm->Size.cx / 100;
    }
    else
    {
        // iForm is already set properly, simply set the user width and
        // length to zero.

        Length = 0;
        Width = 0;
    }

    pdevmode->dm.dmPaperSize = iForm;
    pdevmode->dm.dmPaperLength = Length;
    pdevmode->dm.dmPaperWidth = Width;

    if (pdbForms)
        GlobalFree((HGLOBAL)pdbForms);
}


//--------------------------------------------------------------------------
// VOID SetFormSize(pdevmode, FormName);
// PSDEVMODE  *pdevmode;
// PWSTR       FormName;
//
// This routine sets the dmPaperSize, dmPaperLength, and dmPaperWidth fields
// of the DEVMODE structure, based on FormName.
//
// Parameters
//   pdevmode:
//     Pointer to DEVMODE to be modified.
//
//   FormName:
//     UNICODE formname.
//
//
// Returns
//   This function returns NO VALUE.
//
// Rewrote it.
//   22-Mar-1993    -by-    Kent Settle     (kentse)
// Wrote it.
//--------------------------------------------------------------------------

VOID SetFormName(hPrinter, pdevmode, iForm)
HANDLE      hPrinter;
PSDEVMODE  *pdevmode;
int         iForm;
{
    DWORD           cbNeeded, cReturned;
    FORM_INFO_1    *pdbForm, *pdbForms;
    BOOL            bSuccess;

    // if a user form is set then do nothing.

    if (iForm == DMPAPER_USER)
        return;

    pdbForms = (FORM_INFO_1 *)NULL;
    bSuccess = FALSE;

    // enumerate the forms database.  then locate the index of the form
    // within the database.

    if (!EnumForms(hPrinter, 1, NULL, 0, &cbNeeded, &cReturned))
    {
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            if (pdbForms = (PFORM_INFO_1)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                                                     cbNeeded))
            {
                if (EnumForms(hPrinter, 1, (LPBYTE)pdbForms,
                              cbNeeded, &cbNeeded, &cReturned))
                {
                    pdbForm = pdbForms;
                    pdbForm+= (iForm - 1);
                    bSuccess = TRUE;
                }
            }
        }
    }

#if DBG
    if (!bSuccess)
        DbgPrint("PSCRIPT!_SetFormName: EnumForms failed.\n");
#endif

    // copy the form name into the DEVMODE structure.

    if (bSuccess)
        wcsncpy(pdevmode->dm.dmFormName, pdbForm->pName, CCHFORMNAME);

    if (pdbForms)
        GlobalFree((HGLOBAL)pdbForms);
}


DWORD
PickDefaultHTPatSize(
    DWORD   xDPI,
    DWORD   yDPI,
    BOOL    HTFormat8BPP
    )

/*++

Routine Description:

    This function return default halftone pattern size used for a particular
    device resolution

Arguments:

    xDPI            - Device LOGPIXELS X

    yDPI            - Device LOGPIXELS Y

    8BitHalftone    - If a 8-bit halftone will be used


Return Value:

    DWORD   HT_PATSIZE_xxxx


Author:

    29-Jun-1993 Tue 14:46:49 created  -by-  Daniel Chou (danielc)


Revision History:


--*/

{
    DWORD   HTPatSize;

    //
    // use the smaller resolution as the pattern guide
    //

    if (xDPI > yDPI) {

        xDPI = yDPI;
    }

    if (xDPI >= 2400) {

        HTPatSize = HT_PATSIZE_16x16_M;

    } else if (xDPI >= 1800) {

        HTPatSize = HT_PATSIZE_14x14_M;

    } else if (xDPI >= 1200) {

        HTPatSize = HT_PATSIZE_12x12_M;

    } else if (xDPI >= 900) {

        HTPatSize = HT_PATSIZE_10x10_M;

    } else if (xDPI >= 400) {

        HTPatSize = HT_PATSIZE_8x8_M;

    } else if (xDPI >= 180) {

        HTPatSize = HT_PATSIZE_6x6_M;

    } else {

        HTPatSize = HT_PATSIZE_4x4_M;
    }

    if (HTFormat8BPP) {

        HTPatSize -= 2;
    }

    return(HTPatSize);
}
