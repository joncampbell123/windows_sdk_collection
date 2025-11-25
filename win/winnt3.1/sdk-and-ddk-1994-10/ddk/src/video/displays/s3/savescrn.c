/*****************************************************************************
 * 12/9/92
 *
 *  Save Screen Bits
 *
 * Copyright (c) Microsoft Corp. 1992
 *****************************************************************************/

#include "driver.h"

BOOL bMoveSaveScreenBitsToHost(PPDEV ppdev, PSAVEDSCRNBITS *ppssbNewNode);

ULONG   nSaveScreenBitsCalls,
        nSaveScreenBits,
        nRestoreScreenBits,
        nFreeScreenBits,
        nRestoreScreenBitsHit,
        nRestoreScreenBitsMiss,
        nSsbMovedToHostFromSS_SAVE,
        nSsbMovedToHostFromSrcBmCache;



/*****************************************************************************
 * DrvSaveScreenBits - Save and Restore the screen bits.
 *
 *  Notes:
 *
 *           The same storage in off screen video memory is used
 *           for the saved screen bits and source bitmap caching.
 *
 *           The off screen memory is treated as a one level deep
 *           cache.  So, if a source bitmap is used while there are
 *           saved screen bits in the offscreen area then the saved
 *           screen bits are moved to host memory.
 *
 *               typedef struct _savedscrnbitshdr {
 *                   struct  __savedscrnbitshdr * pssbhLink;
 *                   ULONG   iUniq;
 *                   INT     x,
 *                           y,
 *                           cx,
 *                           cy;
 *               } SAVEDSCRNBITSHDR;
 *
 *           The Saved Screen Bits Header has two functions, closely
 *           related but different.  When the SavedScreenBitsHeader is
 *           used in the pdev the fields have the following meaning:
 *
 *           pssbhLink:  This is the link to a list of saved areas in
 *                       host memory.  If this link is NULL there is no
 *                       list of saved areas.
 *
 *           iUniq:      This is the unique ID for the screen bits in
 *                       off screen video memory.  If this is -1 then
 *                       there are no valid bits in off screen memory.
 *
 *                       This field may be -1 and pssbhLink may be non-null.
 *                       This case would indicate that the off screen memory
 *                       does not contain valid bits, but there are
 *                       valid bits stored in host memory.
 *
 *           When used as a header for saved bits in host memory:
 *
 *           pssbhLink:  This is the link to the next set of saved
 *                       bits.  If this is NULL then this is the last
 *                       set of bits in the chain.
 *
 *           iUniq:      This is the uinque ID for bits associated with
 *                       this header.
 *
 ****************************************************************************/

ULONG DrvSaveScreenBits(
    SURFOBJ *pso,
    ULONG iMode,
    ULONG ident,
    RECTL *prcl)
{
    PPDEV               ppdev;
    PSAVEDSCRNBITSHDR   pssbhInPdev;
    PSAVEDSCRNBITS      pssb, pssbLast, pssbTemp;
    PSAVEDSCRNBITS      pssbNewNode;
    WORD                cmd;
    BOOL                bRet;
    INT                 cx, cy, cxInWords, x;

    ppdev = (PPDEV) pso->dhpdev;

#if 1
    DISPDBG((1, "S3.DLL!DrvSaveScreenBits - Entry\n"));
    DISPDBG((1, "\tiMode: %d\n", iMode));
    DISPDBG((1, "\tident: %x\n", ident));
#endif

    nSaveScreenBitsCalls++;

    switch (iMode)
    {
        case SS_SAVE:

            // First test if this rectangle is small enough for us to
            // handle in the off screen memory.

            cx = prcl->right  - prcl->left;
            cy = prcl->bottom - prcl->top;

            if ((cx > OFF_SCREEN_BITMAP_CX) ||
                (cy > OFF_SCREEN_BITMAP_CY))
            {
                return(0);
            }

            nSaveScreenBits++;

            // Invalidate any cached source bitmap.

            ppdev->hsurfCachedBitmap = (HSURF) -1;

            // if valid bits are already in the cache
            // move them to host memory.

            pssbhInPdev = &(ppdev->SavedScreenBitsHeader);

            if (pssbhInPdev->iUniq != -1)
            {
                // We're treating the saved bits as a stack, since this
                // makes the most sense from how User uses it.

                nSsbMovedToHostFromSS_SAVE++;

                DISPDBG((1, "S3.DLL - Saved Screen Bits Moved to Host Memory from SS_SAVE\n"));

                bRet = bMoveSaveScreenBitsToHost(ppdev, &pssbNewNode);
                if (bRet == FALSE)
                    return(0);

                // Connect this newNode to the beginning of the list of
                // save screen bits nodes.

                pssbTemp                   = pssbhInPdev->pssbLink;
                pssbhInPdev->pssbLink      = pssbNewNode;
                pssbNewNode->ssbh.pssbLink = pssbTemp;

            }

            // Tag the off screen cache with the save bits information.

            pssbhInPdev->iUniq = ++(ppdev->iUniqeSaveScreenBits);
            pssbhInPdev->x     = prcl->left;
            pssbhInPdev->y     = prcl->top;
            pssbhInPdev->cx    = cx;
            pssbhInPdev->cy    = cy;

            // Copy the screen bits to the off screen cache.

            cmd  = BITBLT | DRAW | DIR_TYPE_XY | WRITE | DRAWING_DIR_TBLRXM;

            FIFOWAIT(FIFO_2_EMPTY);

            TEST_AND_SET_FRGD_MIX(SRC_DISPLAY_MEMORY | OVERPAINT);

            OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

            FIFOWAIT(FIFO_7_EMPTY);

            OUTPW(CUR_X, pssbhInPdev->x);
            OUTPW(CUR_Y, pssbhInPdev->y);
            OUTPW(DEST_X, OFF_SCREEN_BITMAP_X);
            OUTPW(DEST_Y, OFF_SCREEN_BITMAP_Y);

            OUTPW(RECT_WIDTH, pssbhInPdev->cx - 1);
            OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | pssbhInPdev->cy - 1));

            OUTPW(CMD, cmd);

            // Return the ID of this set of screen bits.

            return(pssbhInPdev->iUniq);

        case SS_RESTORE:

            nRestoreScreenBits++;

            // If the bits to restore are in the off screen cache
            // copy them to the screen.  Invalidate the cache.

            pssbhInPdev = &(ppdev->SavedScreenBitsHeader);

            if (ident == pssbhInPdev->iUniq)
            {
                nRestoreScreenBitsHit++;

                // Copy the cached off screen bits to the screen.

                cmd  = BITBLT | DRAW | DIR_TYPE_XY | WRITE | DRAWING_DIR_TBLRXM;

                FIFOWAIT(FIFO_2_EMPTY);

                TEST_AND_SET_FRGD_MIX(SRC_DISPLAY_MEMORY | OVERPAINT);

                OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

                FIFOWAIT(FIFO_7_EMPTY);

                OUTPW(CUR_X,  OFF_SCREEN_BITMAP_X);
                OUTPW(CUR_Y,  OFF_SCREEN_BITMAP_Y);
                OUTPW(DEST_X, pssbhInPdev->x);
                OUTPW(DEST_Y, pssbhInPdev->y);

                OUTPW(RECT_WIDTH, pssbhInPdev->cx - 1);
                OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | pssbhInPdev->cy - 1));

                OUTPW(CMD, cmd);

                // Invalidate the cache.

                pssbhInPdev->iUniq = (ULONG) -1;

                return (TRUE);

            }
            else
            {
                // First find the SavedBits Node.

                nRestoreScreenBitsMiss++;

                pssbLast = (PSAVEDSCRNBITS) pssbhInPdev;
                for (pssb = pssbhInPdev->pssbLink;
                     pssb != NULL;
                     pssb = pssb->ssbh.pssbLink)
                {
                    // If this is the node we're looking for then
                    // restore the bits and free the node.

                    if (pssb->ssbh.iUniq == ident)
                    {
                         x = pssb->ssbh.x;
                        cx = pssb->ssbh.cx;
                        cy = pssb->ssbh.cy;

                        // Restore the bits to the screen.

                        if (!(x & 0x1) && !(cx & 0x01))
                        {
                            cmd = RECTANGLE_FILL     | BYTE_SWAP      | BUS_SIZE_16 |
                                  DRAWING_DIR_TBLRXM | DIR_TYPE_XY    | WAIT |
                                  DRAW               | LAST_PIXEL_ON  | WRITE;
                        }
                        else
                        {
                            cmd = RECTANGLE_FILL     | BUS_SIZE_8     |
                                  DRAWING_DIR_TBLRXM | DIR_TYPE_XY    | WAIT |
                                  DRAW               | LAST_PIXEL_ON  | WRITE;
                        }

                        FIFOWAIT(FIFO_8_EMPTY);

                        TEST_AND_SET_WRT_MASK(0xff);
                        OUTPW (MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));
                        TEST_AND_SET_FRGD_MIX(SRC_CPU_DATA | OVERPAINT);

                        OUTPW (CUR_X, x);
                        OUTPW (CUR_Y, pssb->ssbh.y);
                        OUTPW (RECT_WIDTH, (cx - 1));
                        OUTPW (MULTIFUNC_CNTL, (RECT_HEIGHT | (cy - 1)));

                        GPWAIT();

                        OUTPW (CMD, cmd);

                        // Now transfer the data from the screen to the host memory bitmap.

                        CHECK_DATA_READY;

                        if (!(x & 0x1) && !(cx & 0x01))
                        {
                            cxInWords = (cx + 1) / 2;
                            vDataPortOut(ppdev, (PWORD) pssb->aBits, (cxInWords * cy));
                        }
                        else
                        {
                            vDataPortOutB(ppdev, (PBYTE) pssb->aBits, (cx * cy));
                        }

                        CHECK_DATA_COMPLETE;

                        // Snip this node out of the list.

                        pssbLast->ssbh.pssbLink = pssb->ssbh.pssbLink;

                        // free the memory used to hold the bits.

                        LocalFree(pssb);

                        return (TRUE);
                    }

                    pssbLast = pssb;
                }

                DISPDBG((0, "S3.DLL!DrvSaveScreenBits (restore) - Could not find iUniq in the Saved Bits list\n"));
                return (0);
            }

            break;

        case SS_FREE:

            nFreeScreenBits++;

            pssbhInPdev = &(ppdev->SavedScreenBitsHeader);

            // If the bits to free are in the offscreen cache
            // Just invalidate the cache.

            if (ident == pssbhInPdev->iUniq)
            {
                pssbhInPdev->iUniq = (ULONG) -1;
            }
            else
            {
                pssbLast = (PSAVEDSCRNBITS) pssbhInPdev;
                for (pssb = pssbhInPdev->pssbLink;
                     pssb != NULL;
                     pssb = pssb->ssbh.pssbLink)
                {
                    if (pssb->ssbh.iUniq == ident)
                    {
                        // Snip this node out of the list.

                        pssbLast->ssbh.pssbLink = pssb->ssbh.pssbLink;

                        // free the memory used to hold the bits.

                        LocalFree(pssb);

                        return (TRUE);
                    }

                    pssbLast = pssb;
                }

                DISPDBG((0, "S3.DLL!DrvSaveScreenBits (free) - Could not find iUniq in the Saved Bits list\n"));
            }

            break;

        default:
            break;

    }

    return (0);


}

/*****************************************************************************
 * bMoveSaveScreenBitsToHost
 ****************************************************************************/
BOOL bMoveSaveScreenBitsToHost(PPDEV ppdev, PSAVEDSCRNBITS *ppssbNewNode)
{
    PSAVEDSCRNBITSHDR   pssbhInPdev;
    PSAVEDSCRNBITS      pssbNewNode;
    INT                 nSize, cx, cy, cxInWords, x;
    WORD                cmd;
    WORD*               pwBuffer;
    BYTE*               pjBuffer;
    ULONG               i;

    pssbhInPdev = &(ppdev->SavedScreenBitsHeader);

    // Allocate a new node for the bit in the cache.
    // Note: this calculation assumes an 8bpp display.

     x = pssbhInPdev->x;
    cx = pssbhInPdev->cx;
    cy = pssbhInPdev->cy;


    nSize = sizeof (SAVEDSCRNBITSHDR) + ((cx + 4) & ~0x3) * cy;

    pssbNewNode = (PSAVEDSCRNBITS) LocalAlloc(LMEM_FIXED, nSize);

    if (pssbNewNode == NULL)
    {
        DISPDBG((0, "S3.DLL!bMoveSaveScreenBitsToHost - LocalAlloc for pssbNewNode failed\n"));
        EngSetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return (FALSE);
    }

    // Pass back the address of the node we allocated here.

    *ppssbNewNode = pssbNewNode;

    // Assign all the save screen bits header information
    // about the bits in the cache to the newly allocated
    // node.

    pssbNewNode->ssbh = *pssbhInPdev;

    // Copy the bits into the new node.

    if (!(x & 0x1) && !(cx & 0x01))
    {
        cmd =   RECTANGLE_FILL     | BYTE_SWAP      | BUS_SIZE_16 |
                DRAWING_DIR_TBLRXM | DIR_TYPE_XY    | WAIT |
                DRAW               | LAST_PIXEL_ON  | READ;
    }
    else
    {
        cmd =   RECTANGLE_FILL     | BUS_SIZE_8 |
                DRAWING_DIR_TBLRXM | DIR_TYPE_XY    | WAIT |
                DRAW               | LAST_PIXEL_ON  | READ;
    }


    FIFOWAIT(FIFO_7_EMPTY);

    TEST_AND_SET_RD_MASK(0xff);
    OUTPW (MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

    OUTPW (CUR_X, OFF_SCREEN_BITMAP_X);
    OUTPW (CUR_Y, OFF_SCREEN_BITMAP_Y);
    OUTPW (RECT_WIDTH, (cx - 1));
    OUTPW (MULTIFUNC_CNTL, (RECT_HEIGHT | (cy - 1)));

    OUTPW (CMD, cmd);

    // Wait for the Data Available.

    while (!(inpw(GP_STAT) & READ_DATA_AVAILABLE));

    // Now transfer the data from the screen to the host memory bitmap.

    // NOTE: We call vDataPortIn once for each scan instead of doing just
    // one large call to vDataPortIn because for some reason some machines
    // choke on a REP INSW of such a large size -- the S3 gets into a mode
    // such that even when the transfer is completely done, it still
    // claims there's more data to be sent.  This happens on the machine
    // regardless of S3 chip type, but this seems to be an effective work-
    // around for all cases:

    if (!(x & 0x1) && !(cx & 0x01))
    {
        pwBuffer = (WORD*) pssbNewNode->aBits;
        cxInWords = (cx + 1) / 2;
        for (i = cy; i > 0; i--)
        {
            vDataPortIn(ppdev, pwBuffer, cxInWords);
            pwBuffer += cxInWords;
        }
    }
    else
    {
        pjBuffer = (BYTE*) pssbNewNode->aBits;
        for (i = cy; i > 0; i--)
        {
            vDataPortInB(ppdev, pjBuffer, cx);
            pjBuffer += cx;
        }
    }

    return(TRUE);
}




