/******************************************************************************
 *
 *  S3 Bank Manager
 *
 * Copyright (c) 1992 Microsoft Corporation
 *****************************************************************************/

#include "driver.h"

// Bank select forward prototype.

VOID vSetS3Bank(PPDEV ppdev, UINT iBank);

/******************************************************************************
 * bBankInit - Initialize the bank manager.
 *
 *  Setup the ppdev->arclBanks Array.  Since this driver is chip specific,
 *  we can take a number of short cuts.  We know there will always
 *  be 1 meg of memory on the board, and there will only be 1 64K R/W
 *  bank.
 *
 *****************************************************************************/
BOOL bBankInit(
    PPDEV ppdev,
    BOOL fFirstTime)
{
    INT     i;
    BYTE    jMemCfg;

    DISPDBG((3, "S3.DLL: bBankInit - Entry\n"));

    if (fFirstTime)
    {
        ppdev->BankSize     = MEMORY_APERTURE_SIZE;
        ppdev->ScansPerBank = ppdev->BankSize / ppdev->cxMaxRam;
        ppdev->MaxBanks     = ppdev->cyMaxRam / ppdev->ScansPerBank;

        ppdev->prclBanks = LocalAlloc(LPTR, ppdev->MaxBanks * sizeof (RECT));
        if (ppdev->prclBanks == NULL)
        {
            DISPDBG((0, "S3.DLL!bBankInit -  LocalAlloc (prclBanks) failed \n"));
            EngSetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return (FALSE);
        }

        ppdev->pBanks = LocalAlloc(LPTR, ppdev->MaxBanks * sizeof (BANK));
        if (ppdev->pBanks == NULL)
        {
            DISPDBG((0, "S3.DLL!bBankInit -  LocalAlloc (pBanks) failed \n"));
            EngSetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return (FALSE);
        }

        // Note: We only care about scans for the S3 chip.  So don't
        // bother initializing the left and right sides of the rectangle.

        for (i = 0; i < ppdev->MaxBanks; i++) {
            ppdev->prclBanks[i].top    = i * ppdev->ScansPerBank;
            ppdev->prclBanks[i].bottom = ppdev->prclBanks[i].top + ppdev->ScansPerBank;
        }
    }

    // Unlock some of the S3 registers.

    OUTPW (CRTC_INDEX, ((REG_UNLOCK_1 << 8) | S3R8));

    // Enable the Memory Aperture.

    OUTP(CRTC_INDEX, S3R1);
    jMemCfg = INP(CRTC_DATA);
    jMemCfg |= CPUA_BASE;
    OUTP(CRTC_DATA, jMemCfg);

    // Get the initial value of S3R5 and save it away.
    // This will prevent reading the Aperature control register
    // each time we want to set the bank.

    OUTP(CRTC_INDEX, S3R5);
    ppdev->jS3R5 = INP(CRTC_DATA);
    ppdev->jS3R5 &= 0xF0;                 // Just mask for the bits we want.

    return(TRUE);
}

/******************************************************************************
 * bBankEnumStart - Start the bank enumeration.
 *
 *  Save the original values for pvScan0 and rclBounds (The start of the
 *  engine managed bitmap and the clipping bounding rectangle.)
 *
 *  Setup all the banks to enumerate in the ppdev->pBanks array.
 *  Setup the first bank to enumerate.
 *
 *  Entry:  prclScan->bottom - Ptr to the top (first) scan to render (inclusive)
 *          prclScan->top    - Ptr to the bottom (last) scan to render (inclusive)
 *          pso              - The surface object.
 *          pco              - The clipping object.
 *
 *  Exit:   TRUE             - if any rendering needs to be done.
 *          FALSE            - if there is nothing to render, (i.e. the object
 *                             is completely clipped out.)
 *          prclScan->bottom - The Top Scan to render into.
 *          prclScan->top    - The Bottom Scan to render into.
 *
 *****************************************************************************/
BOOL bBankEnumStart(
    PPDEV ppdev,
    PRECTL prclScans,
    SURFOBJ *pso,
    CLIPOBJ *pco)
{
    LONG    TopScan, BottomScan, BankBottom;
    PBYTE   pbScan0;
    INT     i, iFirstBank, iLastBank;

    DISPDBG((3, "S3.DLL: bBankEnumStart - Entry\n"));

    // Save the original pvScan0 and the ClipObj's rclBounds.

    ppdev->pvOrgScan0      = pso->pvScan0;
    ppdev->rclOrgBounds    = pco->rclBounds;
    ppdev->iOrgDComplexity = pco->iDComplexity;
    ppdev->fjOptions       = pco->fjOptions;

    // Set the flag in the clip object to tell the engine to forcible
    // clip to the rclBounds of the clip object.

    pco->fjOptions |= OC_BANK_CLIP;

    // If the clipping is not trivial, do an intersection of the
    // top & bottom scans with the clipping bounds.

    if (pco->iDComplexity != DC_TRIVIAL)
    {
        TopScan    = max (pco->rclBounds.top, prclScans->top);
        BottomScan = min (pco->rclBounds.bottom, prclScans->bottom);
    }
    else
    {
        TopScan    = prclScans->top;
        BottomScan = prclScans->bottom;
    }

    // Init the variables used to set up the banks.

    iLastBank = -1;
    ppdev->cBanks    = 0;
    ppdev->iBank     = 0;
    pbScan0   = (PBYTE) pso->pvScan0;

    // Find the bank that contains the top scan (the first bank).

    for (i = 0; i < ppdev->MaxBanks; i++)
    {
        BankBottom = ppdev->prclBanks[i].bottom;
        if (BankBottom > TopScan)
        {
            // The Top scan is within this bank,

            iFirstBank = i;

            // Set up the bank entry.

            ppdev->pBanks[ppdev->iBank].pvScan0     = pbScan0 - (i * ppdev->BankSize);
            ppdev->pBanks[ppdev->iBank].Bank        = i;
            ppdev->pBanks[ppdev->iBank].rclClip.top = TopScan;

            if (BankBottom < BottomScan)
            {
                ppdev->pBanks[ppdev->iBank].rclClip.bottom = BankBottom;
            }
            else
            {
                ppdev->pBanks[ppdev->iBank].rclClip.bottom = BottomScan;
                iLastBank = iFirstBank;
            }

            // Set the left and right bounds of the bank clip rect.

            ppdev->pBanks[ppdev->iBank].rclClip.left  = pco->rclBounds.left;
            ppdev->pBanks[ppdev->iBank].rclClip.right = pco->rclBounds.right;

            // Bump bank array index to next entry and the bank
            // count.

            ppdev->iBank++;
            ppdev->cBanks++;

            // Since we found the first bank, bail out of this loop.

            break;

        }
    }

    if (i == ppdev->MaxBanks) {

        RIP("S3.DLL: bBankEnumStart - error \n");
        return FALSE;
    }

    if (iLastBank != iFirstBank)
    {
        // Set the last bank to the first bank in case there are no
        // middle banks.

        iLastBank = iFirstBank;

        // Take care of all the middle banks.

        for (i = iFirstBank+1; i < ppdev->MaxBanks; i++)
        {
            if (ppdev->prclBanks[i].bottom < BottomScan)
            {
                // This bank is completely within the rendering rectangle.

                ppdev->pBanks[ppdev->iBank].pvScan0        = pbScan0 - (i * ppdev->BankSize);
                ppdev->pBanks[ppdev->iBank].Bank           = i;
                ppdev->pBanks[ppdev->iBank].rclClip.top    = ppdev->prclBanks[i].top;
                ppdev->pBanks[ppdev->iBank].rclClip.bottom = ppdev->prclBanks[i].bottom;

                // Set the left and right bounds of the bank clip rect.

                ppdev->pBanks[ppdev->iBank].rclClip.left  = pco->rclBounds.left;
                ppdev->pBanks[ppdev->iBank].rclClip.right = pco->rclBounds.right;

                // Bump bank array index to next entry and the bank
                // count.

                ppdev->iBank++;
                ppdev->cBanks++;

                iLastBank = i;
            }
            else
            {
                // The bottom of the bank we're currently "looking" at is
                // below (greater in value) than the bottom of the rectangle
                // were rendering, so there we're done looking at the middle
                // banks.

                break;
            }
        }

        // Now take care of the last bank.

        iLastBank++;

        if (ppdev->prclBanks[iLastBank].top < BottomScan)
        {
            ppdev->pBanks[ppdev->iBank].pvScan0        = pbScan0 - (iLastBank * ppdev->BankSize);
            ppdev->pBanks[ppdev->iBank].Bank           = iLastBank;

            ppdev->pBanks[ppdev->iBank].rclClip.top    = ppdev->prclBanks[iLastBank].top;
            ppdev->pBanks[ppdev->iBank].rclClip.bottom = BottomScan;

            ppdev->pBanks[ppdev->iBank].rclClip.left   = pco->rclBounds.left;
            ppdev->pBanks[ppdev->iBank].rclClip.right  = pco->rclBounds.right;

            ppdev->cBanks++;

        }
    }

    // We need to make sure the clipping level is at least DC_RECT.
    // Some code in the engine (like EngCopybits) will ignore the
    // clip rectangle entirely if the level is DC_TRIVIAL.

    if (pco->iDComplexity == DC_TRIVIAL)
        pco->iDComplexity = DC_RECT;

    // Now take care of setting the first bank

    ppdev->iBank = 1;                     // set the next bank index
    ppdev->cBanks--;                      // set # of remaining banks

    // Set the top and bottom scans for the caller

    prclScans->top    = ppdev->pBanks[0].rclClip.top;     // set top scan.
    prclScans->bottom = ppdev->pBanks[0].rclClip.bottom;  // set bottom scan.

    // Set the S3 chip.

    vSetS3Bank(ppdev, ppdev->pBanks[0].Bank);

    // Set the clip rectangle for the engine.

    pco->rclBounds = ppdev->pBanks[0].rclClip;

    // Set pvScan0 for the engine.

    pso->pvScan0 = ppdev->pBanks[0].pvScan0;

    ENABLE_DIRECT_ACCESS;

    return (TRUE);
}

/******************************************************************************
 * bSrcBankEnumStart - Start the bank enumeration when the screen is the
 *                     source
 *
 *  Save the original values for pvScan0 and rclBounds (The start of the
 *  engine managed bitmap and the clipping bounding rectangle.)
 *
 *  Setup all the banks to enumerate in the ppdev->pBanks array.
 *  Setup the first bank to enumerate.
 *
 *  Entry:  prclScan->bottom - Ptr to the top (first) scan to render (inclusive)
 *          prclScan->top    - Ptr to the bottom (last) scan to render (inclusive)
 *          pso              - The surface object.
 *          pco              - The clipping object.
 *
 *  Exit:   TRUE             - if any rendering needs to be done.
 *          FALSE            - if there is nothing to render, (i.e. the object
 *                             is completely clipped out.)
 *          prclScan->bottom - The Top Scan to render into.
 *          prclScan->top    - The Bottom Scan to render into.
 *
 * NOTE: This routine is a little trickier than bBankEnumStart.  We make GDI
 * read only the current bank by altering the CLIPOBJ before passing the
 * call back to GDI.  The problem here is that the CLIPOBJ applies only to the
 * destination, but for this case it's the screen that is the source.  So we
 * have to adjust for this accordingly.
 *
 *****************************************************************************/
BOOL bSrcBankEnumStart(
    PPDEV ppdev,
    PRECTL prclScans,
    SURFOBJ *pso,
    CLIPOBJ *pco,
    PRECTL  prclDest)
{
    LONG    TopScan, BottomScan, BankBottom;
    PBYTE   pbScan0;
    INT     i, iFirstBank, iLastBank, cyFirstBank, cyMiddleBanks;
    RECTL   rclBounds;

    DISPDBG((3, "S3.DLL: bSrcBankEnumStart - Entry\n"));

    // Save the original pvScan0 and the ClipObj's rclBounds.

    ppdev->pvOrgScan0      = pso->pvScan0;
    ppdev->rclOrgBounds    = pco->rclBounds;
    ppdev->iOrgDComplexity = pco->iDComplexity;
    ppdev->fjOptions       = pco->fjOptions;

    // If the clipping is not trivial, do an intersection of the
    // Set the flag in the clip object to tell the engine to forcible
    // clip to the rclBounds of the clip object.

    pco->fjOptions |= OC_BANK_CLIP;

    // top & bottom scans with the clipping bounds.

    // NOTE: Since the Scans rectangle and the clipping rectangle
    //       refer to two different surfaces, we will always use the
    //       the Scans rectangle to determine the bank(s) we need to
    //       enumerate.

    // rclBounds keeps track of the y-extents of the destination, and is
    // calculated from the intersection of the destination rectangle
    // and the CLIPOBJ's bounds:

    if (pco->iDComplexity != DC_TRIVIAL)
    {
        rclBounds.top    = max(prclDest->top,    pco->rclBounds.top);
        rclBounds.bottom = min(prclDest->bottom, pco->rclBounds.bottom);
    }
    else
    {
        rclBounds.top    = prclDest->top;
        rclBounds.bottom = prclDest->bottom;
    }

    // TopScan and BottomScan keep track of the corresponding y-extents of
    // the source:

    TopScan    = prclScans->top    + (rclBounds.top    - prclDest->top);
    BottomScan = prclScans->bottom + (rclBounds.bottom - prclDest->bottom);

    // Init the variables used to set up the banks.

    iLastBank     = -1;
    ppdev->cBanks = 0;
    ppdev->iBank  = 0;
    pbScan0       = (PBYTE) pso->pvScan0;

    // Find the bank that contains the top scan (the first bank).

    for (i = 0; i < ppdev->MaxBanks; i++)
    {
        BankBottom = ppdev->prclBanks[i].bottom;
        if (BankBottom > TopScan)
        {
            // The Top scan is within this bank,

            iFirstBank = i;

            // Set up the bank entry.

            ppdev->pBanks[ppdev->iBank].pvScan0     = pbScan0 - (i * ppdev->BankSize);
            ppdev->pBanks[ppdev->iBank].Bank        = i;
            ppdev->pBanks[ppdev->iBank].rclClip.top = rclBounds.top;

            // Take care of the case where the entire operation
            // fits within one bank.

            if (BankBottom < BottomScan)
            {
                // The operation spans more than this one bank.

                cyFirstBank = BankBottom - TopScan;
                ppdev->pBanks[ppdev->iBank].rclClip.bottom = rclBounds.top + cyFirstBank;
            }
            else
            {
                // The operation is completel contained with in this
                // single bank.

                ppdev->pBanks[ppdev->iBank].rclClip.bottom = rclBounds.bottom;
                iLastBank = iFirstBank;
            }

            // Set the left and right bounds of the bank clip rect.

            ppdev->pBanks[ppdev->iBank].rclClip.left  = pco->rclBounds.left;
            ppdev->pBanks[ppdev->iBank].rclClip.right = pco->rclBounds.right;

            // Bump bank array index to next entry and the bank
            // count.

            ppdev->iBank++;
            ppdev->cBanks++;

            // Since we found the first bank, bail out of this loop.

            break;

        }
    }

    if (i == ppdev->MaxBanks) {

        RIP("S3.DLL: bSrcBankEnumStart - error \n");
        return FALSE;
    }

    if (iLastBank != iFirstBank)
    {
        // Set the last bank to the first bank in case there are no
        // middle banks.

        iLastBank = iFirstBank;

        // Take care of all the middle banks.
        // Update rclBounds for this bank.

        rclBounds.top += cyFirstBank;

        cyMiddleBanks = ppdev->ScansPerBank;
        rclBounds.bottom = rclBounds.top + cyMiddleBanks;

        for (i = iFirstBank+1; i < ppdev->MaxBanks; i++)
        {
            if (ppdev->prclBanks[i].bottom < BottomScan)
            {
                // This bank is completely within the rendering rectangle.

                ppdev->pBanks[ppdev->iBank].pvScan0        = pbScan0 - (i * ppdev->BankSize);
                ppdev->pBanks[ppdev->iBank].Bank           = i;

                ppdev->pBanks[ppdev->iBank].rclClip.top    = rclBounds.top;
                ppdev->pBanks[ppdev->iBank].rclClip.bottom = rclBounds.bottom;

                // Set the left and right bounds of the bank clip rect.

                ppdev->pBanks[ppdev->iBank].rclClip.left  = pco->rclBounds.left;
                ppdev->pBanks[ppdev->iBank].rclClip.right = pco->rclBounds.right;

                // Update for the next bank bounds.

                rclBounds.top += cyMiddleBanks;

                rclBounds.bottom = rclBounds.top + cyMiddleBanks;

                // Bump bank array index to next entry and the bank
                // count.

                ppdev->iBank++;
                ppdev->cBanks++;

                iLastBank = i;

            }
        }

        // Now take care of the last bank.

        iLastBank++;

        if (ppdev->prclBanks[iLastBank].top < BottomScan)
        {
            ppdev->pBanks[ppdev->iBank].pvScan0        = pbScan0 - (iLastBank * ppdev->BankSize);
            ppdev->pBanks[ppdev->iBank].Bank           = iLastBank;

            ppdev->pBanks[ppdev->iBank].rclClip.top    = rclBounds.top;
            ppdev->pBanks[ppdev->iBank].rclClip.bottom = rclBounds.bottom;

            ppdev->pBanks[ppdev->iBank].rclClip.left   = pco->rclBounds.left;
            ppdev->pBanks[ppdev->iBank].rclClip.right  = pco->rclBounds.right;

            ppdev->cBanks++;

        }
    }

    // We need to make sure the clipping level is at least DC_RECT.
    // Some code in the engine (like EngCopybits) will ignore the
    // clip rectangle entirely if the level is DC_TRIVIAL.

    if (pco->iDComplexity == DC_TRIVIAL)
        pco->iDComplexity = DC_RECT;

    // Now take care of setting the first bank

    ppdev->iBank = 1;                     // set the next bank index
    ppdev->cBanks--;                      // set # of remaining banks

    // Set the top and bottom scans for the caller

    prclScans->top    = ppdev->pBanks[0].rclClip.top;     // set top scan.
    prclScans->bottom = ppdev->pBanks[0].rclClip.bottom;  // set bottom scan.

    // Set the S3 chip.

    vSetS3Bank(ppdev, ppdev->pBanks[0].Bank);

    // Set the clip rectangle for the engine.

    pco->rclBounds = ppdev->pBanks[0].rclClip;

    // Set pvScan0 for the engine.

    pso->pvScan0 = ppdev->pBanks[0].pvScan0;

    ENABLE_DIRECT_ACCESS;

    return (TRUE);
}

/******************************************************************************
 * bBankEnum - Enumerate the next bank.
 *
 *  Just enumerate the next bank.  pvScan0, the clipping Rect, and the
 *  chip are all set up for this bank.
 *
 *  Entry:  none
 *
 *  Exit:   TRUE        - if this bank needs any rendering.
 *          FALSE       - if there is nothing to render.
 *
 *          prclScans   - The Scans to render into.
 *
 ******************************************************************************/
BOOL bBankEnum(
    PPDEV ppdev,
    PRECTL prclScans,
    SURFOBJ *pso,
    CLIPOBJ *pco)
{
    DISPDBG((3, "S3.DLL: bBankEnum - Entry\n"));

    if (ppdev->cBanks == 0)
        return (FALSE);

    // Set the top and bottom scans for the caller

    prclScans->top    = ppdev->pBanks[ppdev->iBank].rclClip.top;     // set top scan.
    prclScans->bottom = ppdev->pBanks[ppdev->iBank].rclClip.bottom;  // set bottom scan.

    // Set the S3 chip.

    vSetS3Bank(ppdev, ppdev->pBanks[ppdev->iBank].Bank);

    // Set the clip rectangle for the engine.

    pco->rclBounds = ppdev->pBanks[ppdev->iBank].rclClip;

    // Set pvScan0 for the engine.

    pso->pvScan0 = ppdev->pBanks[ppdev->iBank].pvScan0;

    // Update the bank index and bank count.

    ppdev->cBanks--;
    ppdev->iBank++;

    return (TRUE);
}

/******************************************************************************
 * bBankEnumEnd - Finish the bank enumeration.
 *
 *  Cleanup from the enumeration.  This restores the original pvScan0 and
 *  ClipRectangle.
 *
 ******************************************************************************/
BOOL bBankEnumEnd(
    PPDEV ppdev,
    SURFOBJ *pso,
    CLIPOBJ *pco)
{
    DISPDBG((3, "S3.DLL: bBankEnumEnd - Entry\n"));

    pso->pvScan0      = ppdev->pvOrgScan0;
    pco->rclBounds    = ppdev->rclOrgBounds;
    pco->iDComplexity = ppdev->iOrgDComplexity;
    pco->fjOptions    = ppdev->fjOptions;

    ENABLE_S3_ENGINE;

    return (TRUE);
}




/******************************************************************************
 * vSetS3Bank - Set the bank on the S3 chip.
 ******************************************************************************/
VOID vSetS3Bank(
    PPDEV ppdev,
    UINT iBank)
{

    TEST_928("S3.DLL!vSetS3Bank - pre bank switch 928 failure\n");

    // Set the bank

    OUTPW1 (CRTC_INDEX, (((ppdev->jS3R5 | (0x0F & iBank)) << 8) | S3R5));

    if (ppdev->bNewBankControl == TRUE)
    {
        OUTP1 (CRTC_INDEX, EX_SCTL_2);
        OUTP1 (CRTC_DATA, ppdev->ExtSysCtl2 | ((0x30 & iBank) >> 2));
    }

    // CHIPBUG.
    // Anil said I should read this back.

    INP1(CRTC_DATA);

    TEST_928("S3.DLL!vSetS3Bank - post bank switch 928 failure\n");

}
