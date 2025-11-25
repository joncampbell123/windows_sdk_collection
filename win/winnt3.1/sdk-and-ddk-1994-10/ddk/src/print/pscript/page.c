//--------------------------------------------------------------------------
//
// Module Name:  PAGE.C
//
// Brief Description:  DrvStartPage and DrvSendPage routines.  Also,
//                     DrvStartDoc, DrvEndDoc and DrvAbortDoc.
//
// Author:  Kent Settle (kentse)
// Created: 01-May-1991
//
// Copyright (C) 1991 - 1992 Microsoft Corporation.
//
// A little history on how DrvStartDoc and DrvEndDoc bracket a document
// in different cases.
//
// Case 1 - Not raw data.  In this case, the application will issue
//          an ESCAPE(STARTDOC) command.  It will get to the driver as
//          DrvStartDoc.  At this point the driver will set a flag, saying
//          DrvStartDoc has been called.  As soon as any drawing command
//          comes in, and we are about to output something to the printer
//          the driver checks to see if DrvStartDoc has been called.  If
//          it has, it then checks to see if the header has been sent.
//          if the header has been sent, the driver just continues normal
//          output.  If the header has not yet been output, the driver
//          outputs it, then continues with normal output.  If DrvStartDoc
//          has not been called when any drawing begins, no output is
//          sent to the printer.  This is done to force applications to
//          call start doc and end doc.
//
// Case 2 - Raw data.  In this case, the application will issue an
//          ESCAPE(STARTDOC) command.  It will get to the driver as
//          DrvStartDoc.  At this point the driver will set a flag, saying
//          DrvStartDoc has been called.  The difference here is that
//          the application will now issue ESCAPE(PASSTHROUGH) commands.
//          which, in the driver, will not call the normal printing
//          routine.  Therefore, it will not check to see if DrvStartDoc
//          has been called, and will not output the header.  This will
//          prevent the driver from outputting two headers in the raw
//          data case.  At EndDoc time, the driver checks to see if
//          we have sent the header to the printer.  If we have not, ie
//          we are sending raw data, then DrvEndDoc will simply send
//          the end of job command, and not close down dictionaries and
//          anything else dependent on the header.
//
// History:
//   01-May-1991    -by-    Kent Settle       (kentse)
// Created.
//--------------------------------------------------------------------------

#include "pscript.h"
#include "enable.h"
#include <string.h>

extern BOOL bSendDeviceSetup(PDEVDATA);

//--------------------------------------------------------------------------
// VOID DrvStartDoc(pso, pwszDocName, dwJobId)
// SURFOBJ    *pso;
// PWSTR      pwszDocName;
// DWORD      dwJobId;
//
// This function is called to begin a print job.  The title of the
// document is pointed to by pvIn.
//
// History:
//   13-Sep-1991    -by-    Kent Settle     [kentse]
//  Wrote it.
//--------------------------------------------------------------------------

BOOL DrvStartDoc(pso, pwszDocName, dwJobId)
SURFOBJ    *pso;
PWSTR      pwszDocName;
DWORD      dwJobId;
{
    PDEVDATA    pdev;

    // get the pointer to our DEVDATA structure and make sure it is ours.

    pdev = (PDEVDATA) pso->dhpdev;

    if (!bValidatePDEV(pdev))
    {
	RIP("PSCRIPT!DrvStartDoc: invalid pdev.\n");
	SetLastError(ERROR_INVALID_PARAMETER);
	return(FALSE);
    }

    // set a flag saying that startdoc has been called.

    pdev->dwFlags |= PDEV_STARTDOC;
    pdev->iPageNumber = 1;

    // copy document name into pdev, if we have been passed one.

    if (pdev->pwstrDocName)
	HeapFree(pdev->hheap, 0, (LPSTR)pdev->pwstrDocName);

    if (pwszDocName)
    {
	if (!(pdev->pwstrDocName = (PWSTR)HeapAlloc(pdev->hheap, 0,
                                      (wcslen(pwszDocName)+1)*sizeof(WCHAR))))
	{
	    RIP("PSCRIPT!DrvStartDoc: HeapAlloc failed.\n");
	    return(FALSE);
	}

        wcscpy(pdev->pwstrDocName, pwszDocName);
    }

    return(TRUE);
}



//--------------------------------------------------------------------------
// VOID DrvStartPage(pso)
// SURFOBJ    *pso;
//
// Asks the driver to send any control information needed at the start of
// a page.  The control codes should be sent via WritePrinter.
//
// History:
//   02-May-1991    -by-    Kent Settle     [kentse]
//  Wrote it.
//--------------------------------------------------------------------------

BOOL DrvStartPage(pso)
SURFOBJ    *pso;
{
    PDEVDATA    pdev;
    DWORD       pbgr;

    // get the pointer to our DEVDATA structure and make sure it is ours.

    pdev = (PDEVDATA) pso->dhpdev;

    if (!bValidatePDEV(pdev))
    {
	RIP("PSCRIPT!DrvStartPage: invalid pdev.\n");
	SetLastError(ERROR_INVALID_PARAMETER);
	return(FALSE);
    }

    // bracket each page with a save/restore.  however, to get around
    // sending out two headers when dealing with raw data, do not
    // send out the save if the header has not been sent.  the header
    // will do the save for the first page.

//!!! there must be a cleaner way of doing all this!!! - kentse.

    if (pdev->dwFlags & PDEV_COMPLETEHEADER)
    {
        // output the page number to the printer and update the page count.

        pdev->iPageNumber++;

        PrintString(pdev, "%%Page: ");
        PrintDecimal(pdev, 2, pdev->iPageNumber, pdev->iPageNumber);
        PrintString(pdev, "\n%%BeginPageSetup\n");

        // showpage wipes out graphics state and transforms, etc.
	// so we need to reset some information each page.

        bSendDeviceSetup(pdev);
        PrintString(pdev, "%%EndPageSetup\n");

	// FALSE means to perform a save command, not a gsave.

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
    }

    return(TRUE);
}


//--------------------------------------------------------------------------
// VOID DrvEndDoc(pso)
// SURFOBJ    *pso;
//
// Informs the driver that the document is ending.
//
// History:
//   13-Sep-1991    -by-    Kent Settle     [kentse]
//  Wrote it.
//--------------------------------------------------------------------------

BOOL DrvEndDoc(pso, fl)
SURFOBJ    *pso;
FLONG       fl;
{
    PDEVDATA    pdev;

    UNREFERENCED_PARAMETER(fl);

    // get the pointer to our DEVDATA structure and make sure it is ours.

    pdev = (PDEVDATA) pso->dhpdev;

    if (!bValidatePDEV(pdev))
    {
	RIP("PSCRIPT!DrvEndDoc: invalid pdev.\n");
	SetLastError(ERROR_INVALID_PARAMETER);
	return(FALSE);
    }

    // if RAWDATA has been sent, then we want to do nothing here.

    if (pdev->dwFlags & PDEV_RAWDATASENT)
    {
        // reset some flags.

        pdev->dwFlags &= ~(PDEV_STARTDOC | PDEV_COMPLETEHEADER |
                           PDEV_PROCSET | PDEV_RAWDATASENT);
        return(TRUE);
    }

    // output the PostScript trailer code if our header was sent out.

    if ((pdev->dwFlags & PDEV_STARTDOC) &&
        (pdev->dwFlags & PDEV_COMPLETEHEADER))
    {
        // turn off manual feed if it was on.

        if (pdev->dwFlags & PDEV_MANUALFEED)
        {
            PrintString(pdev, "%%BeginFeature: *ManualFeed False\n");
            PrintString(pdev, (CHAR *)pdev->pntpd +
                        pdev->pntpd->loszManualFeedFALSE);
            PrintString(pdev, "\n%%EndFeature\n");
        }

        // output the Adobe Trailer seperator, and end the dictionary
        // started at the beginning of the print job.

        PrintString(pdev, "%%Trailer\nend\n");
    }

    // terminate the print job if this is not eps output.

    if (!(pdev->psdm.dwFlags & PSDEVMODE_EPS))
    {
        // set the header sent flag.  this will prevent the following
        // from happening:  if we have been sending out raw data, we
        // will not have sent the header at this point.  when we make
	// the next Print call, it will check to see if we have
        // sent out the header, and send it if we have not.  so lie to
        // it and tell it we have sent the header.

        pdev->dwFlags |= PDEV_PROCSET;

        if (pdev->pntpd->flFlags & PJL_PROTOCOL)
        {
            // if the printer supports PJL job switching, send out the universal
            // end of language code.

            PrintString(pdev, "\033%-12345X");
        }
        else if (pdev->pntpd->flFlags & SIC_PROTOCOL)
        {
            // if the printer supports the Lexmark SIC protocol, send out the
            // end PostScript code.

            bPSWrite(pdev, "\033\133\113\001\000\006", 7);
        }
        else
        {
            // end the print job.  The character '\4' is
            // the end of job character for PostScript.

            if (!(pdev->pntpd->flFlags & NO_ENDOFFILE))
                PrintString(pdev, "\004");
        }

    }

    // flush the buffer.

    bPSFlush(pdev);

    // reset some flags.

    pdev->dwFlags &= ~(PDEV_STARTDOC | PDEV_COMPLETEHEADER | PDEV_PROCSET);

    return(TRUE);
}



//--------------------------------------------------------------------------
// BOOL DrvSendPage(pso)
// SURFOBJ    *pso;
//
// Requests that the printer send the raw bits from the indicated surface
// to the printer via WritePrinter.  (WritePrinter does not have to be used when
// the hardcopy device is accessed via I/O ports.
//
// If the surface is a bitmap on which the drawing has been accumulated,
// the driver should access the bits via SURFOBJ service functions.  If
// the surface is a journal, the driver should request that the journal
// be played back to a bitmap or device surface, and get the bits
// accordingly.  Some drivers may have used a device managed surface and
// sent the bits to the printer as the drawing orders came in.  In that
// case, this call does not send out the drawing.
//
// The control code which causes a page to be ejected from the printer
// should be sent as a result of this call.
//
// If this function is slow, we have to worry about the user wanting to
// abort the print job while in this call.  Therefore, the driver should
// call EngCheckAbort at least once every ten seconds to see if printing
// should be terminated.  If EngCheckAbort returns TRUE, then processing
// of the page should be stopped and this function should return.  Note
// that EngPlayJournal will take care of querying for the abort itself.
// The driver need only be concerned if its own code will run continuously
// for more than ten seconds.
//
// Parameters:
//   pso:
//     The surface object on which the drawing has been accumulated.  The
//     object can be queried to find its type and what PDEV it is
//     associated with.
//
// Returns:
//   This function returns no value.
//
// History:
//   01-May-1991    -by-    Kent Settle     [kentse]
//  Wrote it.
//--------------------------------------------------------------------------

BOOL DrvSendPage(pso)
SURFOBJ    *pso;
{
    PDEVDATA    pdev;

    // get the pointer to our DEVDATA structure and make sure it is ours.

    pdev = (PDEVDATA) pso->dhpdev;

    if (!bValidatePDEV(pdev))
    {
	RIP("PSCRIPT!DrvSendPage: invalid pdev.\n");
	SetLastError(ERROR_INVALID_PARAMETER);
	return(FALSE);
    }

    if (pdev->psdm.dwFlags & PSDEVMODE_EPS)
    {
        // EPS files consist of one page only, so terminate the
        // document.

        if (pdev->dwFlags & PDEV_COMPLETEHEADER)
        {
	    // output the Adobe Trailer seperator, then end the dictionary
	    // started at the beginning of the print job.

	    PrintString(pdev, "%%Trailer\nend\n");

            pdev->dwFlags &= ~PDEV_COMPLETEHEADER;
        }

	// close the page with a restore.  FALSE means restore, not grestore.

	ps_restore(pdev, FALSE);
        pdev->dwFlags &= ~PDEV_WITHINPAGE;
    }
    else
    {
        // if the header has not been sent, nothing will have been sent, so
        // we have no page to end.

        if (pdev->dwFlags & PDEV_PROCSET)
        {
            // reset PDEV flags concerned with per page information.

            pdev->dwFlags &= ~(PDEV_FONTREDEFINED | PDEV_LATINENCODED |
                              PDEV_SYMENCODED | PDEV_DINGENCODED);

            // close the page with a restore.  FALSE means restore, not grestore.

            ps_restore(pdev, FALSE);
            pdev->dwFlags &= ~PDEV_WITHINPAGE;

            // generate a PostScript page eject command.

            ps_showpage(pdev);

            // flush the output buffer.

            bPSFlush(pdev);
        }
    }

    return(TRUE);
}
