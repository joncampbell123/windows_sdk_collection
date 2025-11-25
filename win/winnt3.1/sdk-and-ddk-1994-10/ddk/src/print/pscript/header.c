//--------------------------------------------------------------------------
//
// Module Name:  HEADER.C
//
// Brief Description:  This module contains the PSCRIPT driver's header
// output functions and related routines.
//
// Author:  Kent Settle (kentse)
// Created: 26-Nov-1990
//
// Copyright (c) 1990-1993 Microsoft Corporation
//
// This routine contains routines to output the PostScript driver's header.
//--------------------------------------------------------------------------

#include "pscript.h"
#include "header.h"
#include "resource.h"
#include "enable.h"


extern HMODULE     ghmodDrv;    // GLOBAL MODULE HANDLE.
extern int NameComp(CHAR *, CHAR *);

BOOL bSendDeviceSetup(PDEVDATA);
VOID DownloadNTProcSet(PDEVDATA, BOOL);
VOID SetFormAndTray(PDEVDATA);

//--------------------------------------------------------------------------
// BOOL bOutputHeader(pdev)
// PDEVDATA    pdev;
//
// This routine sends the driver's header to the output channel.
//
// Parameters:
//   pdev:
//     pointer to DEVDATA structure.
//
// Returns:
//   This function returns TRUE if the header was successfully sent,
//   FALSE otherwise.
//
// History:
//   26-Nov-1990     -by-     Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

BOOL  bOutputHeader(pdev)
PDEVDATA     pdev;
{
    CHAR            buf[128];
    CHAR           *pstr;
    WCHAR          *pwstr;
    DWORD           cTmp, i;
    BOOL            bDuplex;
    SYSTEMTIME      systime;
    DWORD           pbgr;
    PSRESOLUTION   *pRes;
    PNTPD           pntpd;

    // don't do anything if the header has already been sent to the printer.

    if (pdev->dwFlags & PDEV_PROCSET)
	return TRUE;

    // set the header sent flag.  this needs to be done before a call
    // to Print is made in order to keep from getting stuck in a loop.

    pdev->dwFlags |= PDEV_PROCSET;

    // if RAWDATA has already been sent to the printer, then we only want
    // to send down our ProcSet, and not the rest of the header, or any
    // of the device setup commands.

    if (pdev->dwFlags & PDEV_RAWDATASENT)
    {
        // define our procedure set, FALSE means don't even think
        // about defining the error handler..

        DownloadNTProcSet(pdev, FALSE);

        // output a save command for the first page.  this is done in the
        // header as part of the effort to avoid outputting two headers in
        // the raw data case.  FALSE means to perform a save and not a gsave.

        ps_save(pdev, FALSE);
        pdev->dwFlags |= PDEV_WITHINPAGE;

        return(TRUE);
    }

    // get a local pointer.

    pntpd = pdev->pntpd;

    // output the header comments.  NOTE, these comments conform to
    // Version 2.0 of the Adobe Structuring Conventions.

    // if the printer supports job switching, put the printer into
    // postscript mode now.

    if (pntpd->flFlags & PJL_PROTOCOL)
        PrintString(pdev, "\033%-12345X@PJL ENTER LANGUAGE=POSTSCRIPT\n");
    if (pntpd->flFlags & SIC_PROTOCOL)
    {
        // call directly to bPSWrite to output the necessary escape commands.
        // PrintString will NOT output '\000'.

        bPSWrite(pdev, "\033\133\113\030\000\006\061\010\000\000\000\000\000", 13);
        bPSWrite(pdev, "\000\000\000\000\000\000\000\000\004\033\133\113\003", 13);
        bPSWrite(pdev, "\000\006\061\010\004", 5);
    }

    //!!! output something different if Encapsulated PS.
    //!!! PrintString(pdev, "%!PS-Adobe-2.0 EPSF-2.0\n");

    PrintString(pdev, "%!PS-Adobe-2.0\n");

    // output the title of the document.

    if (pdev->pwstrDocName)
    {
//!!! need to output UNICODE document name???  -kentse.
//!!! for now just lop off top word.
        pstr = buf;
        pwstr = pdev->pwstrDocName;

        cTmp = min((sizeof(buf) - 1), wcslen(pwstr));

        while (cTmp--)
            *pstr++ = (CHAR)*pwstr++;

        // NULL terminate the document name.

        *pstr = '\0';

	PrintString(pdev, "%%Title: ");
	PrintString(pdev, buf);
	PrintString(pdev, "\n");
    }
    else
	PrintString(pdev, "%%Title: Untitled Document\n");

    // let the world know who we are.

    PrintString(pdev, "%%Creator: Windows NT 3.1\n");

    // print the date and time of creation.

    GetLocalTime(&systime);

    PrintString(pdev, "%%CreationDate: ");
    PrintDecimal(pdev, 1, systime.wHour);
    PrintString(pdev, ":");
    PrintDecimal(pdev, 1, systime.wMinute);
    PrintString(pdev, " ");
    PrintDecimal(pdev, 1, systime.wMonth);
    PrintString(pdev, "/");
    PrintDecimal(pdev, 1, systime.wDay);
    PrintString(pdev, "/");
    PrintDecimal(pdev, 1, systime.wYear);

    // mark the bounding box of the document.

    PrintString(pdev, "\n%%BoundingBox: ");
    PrintDecimal(pdev, 4, pdev->CurForm.imagearea.left,
                 pdev->CurForm.imagearea.bottom,
                 pdev->CurForm.imagearea.right,
		 pdev->CurForm.imagearea.top);

    PrintString(pdev, "\n%%DocumentProcSets: Windows_NT_3.1\n");
    PrintString(pdev, "%%DocumentSuppliedProcSets: Windows_NT_3.1\n");

    if (pdev->cCopies > 1)
    {
        PrintString(pdev, "%%Requirements: numcopies(");
        PrintDecimal(pdev, 1, pdev->cCopies);
        PrintString(pdev, ") collate\n");
    }

    // we are done with the comments portion of the document.

    PrintString(pdev, "%%EndComments\n");

    // define our procedure set.

    DownloadNTProcSet(pdev, TRUE);

    PrintString(pdev, "%%EndProlog\n");

    // do the device setup.

    PrintString(pdev, "%%BeginSetup\n");

    // send the resolution selection command, if necessary.

    if (pntpd->cResolutions > 0)
    {
        pRes = (PSRESOLUTION *)((CHAR *)pntpd + pntpd->loResolution);

        // search each possible resolution until the specified one is
        // found.

        for (i = 0; i < (DWORD)pntpd->cResolutions; i++)
        {
            if (pRes[i].iValue == (DWORD)pdev->psdm.dm.dmPrintQuality)
            {
                PrintString(pdev, "%%BeginFeature: *Resolution ");
                PrintDecimal(pdev, 1, pdev->psdm.dm.dmPrintQuality);
                PrintString(pdev, "\n");
                PrintString(pdev, (CHAR *)pntpd + pRes[i].loInvocation);
                PrintString(pdev, "\n%%EndFeature\n");
            }
        }
    }

    // send form and tray selection commands.

    SetFormAndTray(pdev);

    // handle duplex if necessary.

    if ((pntpd->loszDuplexNone) ||
        (pntpd->loszDuplexNoTumble) ||
        (pntpd->loszDuplexTumble))
        bDuplex = TRUE;
    else
        bDuplex = FALSE;

    if (bDuplex)
    {
        if (pdev->psdm.dm.dmDuplex == DMDUP_HORIZONTAL)
        {
            if (pdev->psdm.dm.dmOrientation == DMORIENT_LANDSCAPE)
            {
                PrintString(pdev, "%%BeginFeature: *Duplex DuplexNoTumble\n");
                if (pntpd->loszDuplexNoTumble)
                    pstr = (char *)pntpd + pntpd->loszDuplexNoTumble;
            }
            else
            {
                PrintString(pdev, "%%BeginFeature: *Duplex DuplexTumble\n");
                if (pntpd->loszDuplexTumble)
                    pstr = (char *)pntpd + pntpd->loszDuplexTumble;
            }
        }
        else if (pdev->psdm.dm.dmDuplex == DMDUP_VERTICAL)
        {
            if (pdev->psdm.dm.dmOrientation == DMORIENT_LANDSCAPE)
            {
                PrintString(pdev, "%%BeginFeature: *Duplex DuplexTumble\n");
                if (pntpd->loszDuplexTumble)
                    pstr = (char *)pntpd + pntpd->loszDuplexTumble;
            }
            else
            {
                PrintString(pdev, "%%BeginFeature: *Duplex DuplexNoTumble\n");
                if (pntpd->loszDuplexNoTumble)
                    pstr = (char *)pntpd + pntpd->loszDuplexNoTumble;
            }
        }
        else // turn duplex off.
        {
            PrintString(pdev, "%%BeginFeature: *Duplex None\n");
            if (pntpd->loszDuplexNone)
                pstr = (char *)pntpd + pntpd->loszDuplexNone;
        }

        PrintString(pdev, pstr);
        PrintString(pdev, "\n%%EndFeature\n");
    }

    // handle collation if the device supports it.

    if (pntpd->loszCollateOn && pntpd->loszCollateOff)
    {
        if (pdev->psdm.dm.dmCollate = DMCOLLATE_TRUE)
        {
            PrintString(pdev, "%%BeginFeature: *Collate True\n");
            pstr = (char *)pntpd + pntpd->loszCollateOn;
        }
        else
        {
            PrintString(pdev, "%%BeginFeature: *Collate False\n");
            pstr = (char *)pntpd + pntpd->loszCollateOff;
        }

        PrintString(pdev, pstr);
        PrintString(pdev, "\n%%EndFeature\n");
    }

    if (pdev->cCopies > 1)
    {
        PrintString(pdev, "/#copies ");
        PrintDecimal(pdev, 1, pdev->cCopies);
        PrintString(pdev, " def\n");
    }

    PrintString(pdev, "%%EndSetup\n%%Page: 1 1\n%%BeginPageSetup\n");

    // the form / tray information has already been sent for the first page.

    pdev->dwFlags &= ~PDEV_CHANGEFORM;

    bSendDeviceSetup(pdev);

    PrintString(pdev, "%%EndPageSetup\n");

    // output a save command for the first page.  this is done in the
    // header as part of the effort to avoid outputting two headers in
    // the raw data case.  FALSE means to perform a save and not a gsave.

    ps_save(pdev, FALSE);
    pdev->dwFlags |= PDEV_WITHINPAGE;

    if (pdev->psdm.dwFlags & PSDEVMODE_NEG)
    {
        // fill the entire imageable area with white, which will get
        // transformed to black.  then restore the color to black.

        pbgr = RGB_WHITE;
        ps_setrgbcolor(pdev, (BGR_PAL_ENTRY *)&pbgr);

        PrintDecimal(pdev, 4, 0,
                     (pdev->CurForm.imagearea.left * pdev->psdm.dm.dmScale) / 100,
                     (pdev->CurForm.sizlPaper.cy -
                     ((pdev->CurForm.imagearea.top * pdev->psdm.dm.dmScale) / 100)),
                     (pdev->CurForm.imagearea.right * pdev->psdm.dm.dmScale) / 100,
                     (pdev->CurForm.sizlPaper.cy -
                     ((pdev->CurForm.imagearea.bottom * pdev->psdm.dm.dmScale) / 100)));

        PrintString(pdev, " box\n");
        pdev->cgs.dwFlags |= CGS_PATHEXISTS;

        ps_fill(pdev, (FLONG)FP_WINDINGMODE);
        pbgr = RGB_BLACK;
        ps_setrgbcolor(pdev, (BGR_PAL_ENTRY *)&pbgr);
    }

    pdev->dwFlags |= PDEV_COMPLETEHEADER;

    return(TRUE);
}


//--------------------------------------------------------------------------
// BOOL bSendDeviceSetup(pdev)
// PDEVDATA    pdev;
//
// This routine sends the driver's device setup section of the header
// to the output channel.
//
// Parameters:
//   pdev:
//     pointer to DEVDATA structure.
//
// Returns:
//   This function returns TRUE if the header was successfully sent,
//   FALSE otherwise.
//
// History:
//   26-Nov-1990     -by-     Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

BOOL bSendDeviceSetup(pdev)
PDEVDATA    pdev;
{
    PSTR    pstr;
    PNTPD   pntpd;

    // set up for new form if necessary.

    if (pdev->dwFlags & PDEV_CHANGEFORM)
    {
        SetFormAndTray(pdev);
        pdev->dwFlags &= ~PDEV_CHANGEFORM;
    }

    pntpd = pdev->pntpd;

    // rotate and translate if we are in landscape mode.

    if (pdev->psdm.dm.dmOrientation == DMORIENT_LANDSCAPE)
    {
#ifdef LANDSCAPE_270_ROTATE
        PrintString(pdev, "270 rotate ");
        PrintDecimal(pdev, 2, -pdev->CurForm.sizlPaper.cx, 0);
        PrintString(pdev, " translate\n");
#else   // 90 degree rotation case.
        PrintString(pdev, "90 rotate ");
        PrintDecimal(pdev, 2, 0, -pdev->CurForm.sizlPaper.cy);
        PrintString(pdev, " translate\n");
#endif
    }

    // this implementation of mirror imaging is to simply flip over the
    // x axis.  ie, the image is reversed in the horizontal direction.
    //!!! perhaps at some point we will want to allow flipping in the
    //!!! vertical direction as well.  -kentse.

    if (pdev->psdm.dwFlags & PSDEVMODE_MIRROR)
    {
        PrintDecimal(pdev, 1, pdev->CurForm.sizlPaper.cx);
        PrintString(pdev, " 0 translate -1 1 scale\n");
    }

    // send the proper normalized transfer function, if one exists.
    // send the inverted transfer function if the PSDEVMODE_NEG
    // flag is set.

    if (pdev->psdm.dwFlags & PSDEVMODE_NEG)
    {
        // if an inverse normalized transfer function is defined for
        // this device, send it to the printer, else send the default
        // inverse transfer function.

        if (pntpd->loszInvTransferNorm)
        {
            pstr = (char *)pntpd + pntpd->loszInvTransferNorm;
            PrintString(pdev, pstr);
        }
        else // default inverse transfer function.
            PrintString(pdev, "{1 exch sub}");

        PrintString(pdev, " settransfer\n");
    }
    else
    {
        // send the normalized transfer function to the printer if
        // one exists for this printer.

        if (pntpd->loszTransferNorm)
        {
            pstr = (char *)pntpd + pntpd->loszTransferNorm;
            PrintString(pdev, pstr);
            PrintString(pdev, " settransfer\n");
        }
    }

    // set the default line width (8 / 1000th inch).

    pdev->cgs.psfxLineWidth = 0;    // force the linewidth to be set.
    ps_setlinewidth(pdev, PSFX_DEFAULT_LINEWIDTH);

    return(TRUE);
}

//--------------------------------------------------------------------
// BOOL bSendPSProcSet(pdev, ulPSid)
// PDEVDATA        pdev;
// ULONG           ulPSid;
//
// Routine Description:
//
// This routine will output the PS Procset Resource referenced by ulPSid
// to the PS Interpreter. See PSPROC.H for valid ids.
//
// Return Value:
//
//  FALSE if an error occurred.
//
// Author:
//
//  15-Feb-1993 created  -by-  Rob Kiesler
//
//
// Revision History:
//--------------------------------------------------------------------

BOOL bSendPSProcSet(pdev, ulPSid)
PDEVDATA    pdev;
ULONG       ulPSid;
{
    HANDLE  hRes;
    USHORT  usSize;
    HANDLE  hProcRes;
    PSZ     pntps;

    if (pdev->dwFlags & PDEV_CANCELDOC)
        return(TRUE);

    if (!(hRes = FindResource(ghmodDrv, MAKEINTRESOURCE(ulPSid),
				  MAKEINTRESOURCE(PSPROC))))
	{
	    RIP("PSCRIPT!bSendPSProcSet: Couldn't find proc set resource\n");
	    return(FALSE);
	}

	usSize = (USHORT)SizeofResource(ghmodDrv, hRes);

	//
    // Get the handle to the resource.
    //
	if (!(hProcRes = LoadResource(ghmodDrv, hRes)))
	{
	    RIP("PSCRIPT!bSendPSProcSet: LoadResource failed.\n");
	    return(FALSE);
	}

    //
	// Get a pointer to the resource data.
    //
	if (!(pntps = (PSZ) LockResource(hProcRes)))
	{
	    RIP("PSCRIPT!bSendPSProcSet: LockResource failed.\n");
	    FreeResource(hProcRes);
	    return(FALSE);
	}
    if (!bPSWrite(pdev, pntps, usSize))
	{
	    RIP("PSCRIPT!bSendPSProcSet: Output of Header failed.\n");
	    FreeResource(hProcRes);
	    return(FALSE);
	}

    FreeResource(hProcRes);
    bPSFlush(pdev);
    return(TRUE);
}


//--------------------------------------------------------------------------
// VOID DownloadNTProcSet(pdev, bEhandler)
// PDEVDATA    pdev;
// BOOL        bEhandler;
//
// This routine sends the driver's ProcSet to the output channel.
//
// Parameters:
//   pdev:
//     pointer to DEVDATA structure.
//
//   bEhandler:
//     TRUE if we should even consider sending the error handler,
//     otherwise FALSE.
//
// Returns:
//   This function returns no value.
//
// History:
//   11-May-1993     -by-     Kent Settle     (kentse)
//  Broke into a separate routine.
//--------------------------------------------------------------------------

VOID DownloadNTProcSet(pdev, bEhandler)
PDEVDATA    pdev;
BOOL        bEhandler;
{
    PSZ            *ppsz;

    // define our procedure set.

    PrintString(pdev, "%%BeginProcSet: Windows_NT_3.1\n");
    PrintString(pdev, "% Copyright (c) 1991 - 1993 Microsoft Corporation\n");

    // we need to define the true resolution of the printer.

    PrintString(pdev, "100 dict begin ");
    PrintString(pdev, "/DPI ");
    PrintDecimal(pdev, 1, pdev->psdm.dm.dmPrintQuality);
    PrintString(pdev, " def\n");

    PrintString(pdev, "/_snap {transform 36 DPI div sub round 36 DPI div add\n");
    PrintString(pdev, "exch 36 DPI div sub round 36 DPI div add exch itransform}bind def\n");

    // download our error handler if we are told to.

    if (bEhandler)
    {
        if (pdev->psdm.dwFlags & PSDEVMODE_EHANDLER)
        {
            ppsz = apszEHandler;
            while (*ppsz)
            {
                PrintString(pdev, (PSZ)*ppsz++);
                PrintString(pdev, "\n");
            }
        }
    }
    // download our procedure definitions code.

    ppsz = apszHeader;
    while (*ppsz)
    {
	PrintString(pdev, (PSZ)*ppsz++);
	PrintString(pdev, "\n");
    }

    PrintString(pdev, "%%EndProcSet\n");

    pdev->dwFlags |= PDEV_PROCSET;
}


VOID SetFormAndTray(pdev)
PDEVDATA    pdev;
{
    WCHAR           FormName[CCHFORMNAME];
    WCHAR          *pFormName;
    WCHAR          *pSlotName;
    WCHAR          *pPrinterForm;
    WCHAR           ManualName[MAX_SLOT_NAME];
    WCHAR           SearchName[MAX_SLOT_NAME];
    BOOL            bManual, bForm, bFound, bRegion;
    WCHAR          *pwstr;
    PSINPUTSLOT    *pSlot;
    DWORD           i;
    PSFORM         *pPSForm;
    PNTPD           pntpd;

    pntpd = pdev->pntpd;

    // select the paper tray.  do this by selecting the first tray
    // which contains the form in question.

    // get a unicode version of the form name.

    strcpy2WChar(FormName, pdev->CurForm.PrinterForm);

    // get the manual tray name.

    LoadString(ghmodDrv, (SLOT_MANUAL + SLOTS_BASE),
               ManualName, (sizeof(ManualName) / sizeof(ManualName[0])));

    // assume the form is not in manual feed.

    if (pdev->dwFlags & PDEV_MANUALFEED)
    {
        PrintString(pdev, "%%BeginFeature: *ManualFeed False\n");
        PrintString(pdev, (CHAR *)pntpd + pntpd->loszManualFeedFALSE);
        PrintString(pdev, "\n%%EndFeature\n");
        pdev->dwFlags &= ~PDEV_MANUALFEED;
    }

    bManual = FALSE;

    // we have the form name, now check the registry to see if this form
    // is in any of the paper trays.

    if (pdev->pTrayFormTable)
    {
        pwstr = pdev->pTrayFormTable;
        bForm = FALSE;

        while (*pwstr)
        {
            pSlotName = pwstr;
            pFormName = pSlotName + (wcslen(pSlotName) + 1);
            pPrinterForm = pFormName + (wcslen(pFormName) + 1);

            if (!(wcscmp(pPrinterForm, FormName)))
            {
                // we found the form question.  get the tray name.

                if (!(wcscmp(pSlotName, ManualName)))
                {
                    // the form is in the manual tray, but see if it is
                    // one of the other trays first.

                    bManual = TRUE;

                    pwstr = pPrinterForm + (wcslen(pPrinterForm) + 1);
                }
                else
                {
                    bForm = TRUE;
                    break;
                }
            }
            else
            {
                // this was not the form in question.  skip over the
                // tray-form triplet.

                pwstr = pPrinterForm + (wcslen(pPrinterForm) + 1);
            }
        }

        // if the tray-form pair was found, output the proper commands
        // to select the tray in question.

        bFound = FALSE;

        if (bForm)
        {
            // select the tray if there are multiple trays to select from.

            if (pntpd->cInputSlots > 0)
            {
                pSlot = (PSINPUTSLOT *)((CHAR *)pntpd + pntpd->loPSInputSlots);

                // get each slot name from the NTPD, and do a look up in the
                // registry to find the associated form name.

                for (i = 0; i < pntpd->cInputSlots; i++)
                {
                    strcpy2WChar(SearchName, (CHAR *)pntpd + pSlot->loSlotName);

                    if (!(wcscmp(pSlotName, SearchName)))
                    {
                        bFound = TRUE;
                        break;
                    }

                    pSlot++;
                }

                if (bFound)
                {
                    PrintString(pdev, "%%BeginFeature: *InputSlot ");
                    PrintString(pdev, (CHAR *)pntpd + pSlot->loSlotName);
                    PrintString(pdev, "\n");

                    PrintString(pdev, (CHAR *)pntpd + pSlot->loSlotInvo);
                    PrintString(pdev, "\n%%EndFeature\n");
                }
            }

        }

        // if the form was not found in one of the paper trays, and the printer
        // supports manual feed, see if the form is is the manual feed slot.

        if ((!bFound) && (pntpd->loszManualFeedTRUE != 0) && (bManual))
        {
            // the requested form is in the manual feed slot,
            // so select manual feed.

            PrintString(pdev, "%%BeginFeature: *ManualFeed True\n");
            PrintString(pdev, (CHAR *)pntpd + pntpd->loszManualFeedTRUE);
            PrintString(pdev, "\n%%EndFeature\n");
            pdev->dwFlags |= PDEV_MANUALFEED;
        }
    }

    // select the page region if we are also selecting from multiple
    // paper trays or manual feed, otherwise select page size.

    bRegion = FALSE;

    if ((pdev->dwFlags & PDEV_MANUALFEED) || (bFound))
        bRegion = TRUE;

    // check for the odd occurence where there are no PageRegions (WANG15FP.PPD).

    // find the PSFORM structure in the NTPD for the current form.

    pPSForm = (PSFORM *)((CHAR *)pntpd + pntpd->loPSFORMArray);

    if (!pntpd->cPageRegions)
        bRegion = FALSE;

    if (bRegion)
        PrintString(pdev, "%%BeginFeature: *PageRegion ");
    else
        PrintString(pdev, "%%BeginFeature: *PageSize ");

    PrintString(pdev, pdev->CurForm.PrinterForm);
    PrintString(pdev, "\n");

    pPSForm = (PSFORM *)((CHAR *)pntpd + pntpd->loPSFORMArray);

    for (i = 0; i < pntpd->cPSForms; i++)
    {
        if (!(NameComp((CHAR *)pdev->CurForm.PrinterForm,
                     (CHAR *)pntpd + pPSForm->loFormName)))
        {
            if (bRegion)
                PrintString(pdev, (CHAR *)pntpd + pPSForm->loRegionInvo);
            else
                PrintString(pdev, (CHAR *)pntpd + pPSForm->loSizeInvo);

            PrintString(pdev, "\n");
            break;
        }

        // point to the next PSFORM.

        pPSForm++;
    }

    PrintString(pdev, "%%EndFeature\n");
}
