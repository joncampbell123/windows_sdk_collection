/******************************Module*Header*******************************\
* Module Name: savescr.c                                                   *
*                                                                          *
* DrvSaveScreenBits                                                        *
*                                                                          *
* Copyright (c) 1992 Microsoft Corporation                                 *
\**************************************************************************/


#include <driver.h>


//**************************************************************************//
// Note: if this is turned on, you *MUST* make sure to invalidate and reset //
// this when going to/returning from fullscreen mode (in DrvAssertMode).    //
//**************************************************************************//
#define OFFSCREEN_SUPPORTED 0   // 1 to support saves to offscreen memory
                                //  (code currently not written)

VOID
vRestoreScreenBitsFromOffscreen(
   PDEVSURF pdsurf,
   PRECTL prcl,
   PBYTE pjSrcBuffer
   );

VOID
vRestoreScreenBitsFromMemory(
   PDEVSURF pdsurf,
   PRECTL prcl,
   PBYTE pjSrcBuffer,
   ULONG ulRestoreWidthInBytes,
   ULONG ulSrcDelta
   );

VOID
vSaveScreenBitsToMemory(
   PDEVSURF pdsurf,
   PRECTL prcl,
   PVOID pjDestBuffer,
   ULONG ulSaveWidthInBytes,
   ULONG ulSaveHeight,
   ULONG ulDestScanWidth
   );

/******************************Public*Routine******************************\
* DrvSaveScreenBits(pso,iMode,iIdent,prcl)                                 *
*                                                                          *
* Saves and restores the specified area of the screen                      *
*                                                                          *
\**************************************************************************/

ULONG DrvSaveScreenBits(SURFOBJ *pso, ULONG iMode, ULONG iIdent, RECTL *prcl)
{
    PDEVSURF pdsurf;
    PPDEV ppdev;
    PSAVED_SCREEN_BITS pSSB, *pLastSSBPtr, pSSBTemp;
    BOOL bIdentFound;
    ULONG ulSaveSize, ulSaveHeight;
    ULONG ulSaveWidthInBytes, ulSaveWidthInAlignedBytes;

//    ASSERT(pso != (SURFOBJ *) NULL,"DrvSaveScreenBits invalid pso");

    pdsurf = (PDEVSURF) pso->dhsurf;

//    ASSERT(pdsurf != (PDEVSURF) NULL,"DrvSaveScreenBits invalid dhsurf");
//    ASSERT(pdsurf->iFormat == BMF_PHYSDEVICE,
//            "DrvSaveScreenBits DFBs not supported");

    ppdev = pdsurf->ppdev;  // find the PDEV that goes with this surface

    //
    // Save, restore, or free a block of screen bits.
    //

    switch(iMode)
    {
        //
        // Save a block of screen bits.
        //

        case SS_SAVE:

//            ASSERT(prcl != (RECTL *) NULL,"DrvSaveScreenBits invalid prcl");
            // Figure out how big the save area will be
            ulSaveHeight = prcl->bottom - prcl->top;
            ulSaveWidthInBytes =
                    ((ULONG)((prcl->right + 7) - (prcl->left & ~0x07))) >> 3;

#if OFFSCREEN_SUPPORTED
            // This is the size of the the save area for one plane, or the
            // number of addresses required to save this in offscreen memory.
            // A system memory save area would require four times as many bytes
            ulSaveSize = ulSaveHeight * pSSB->ulSaveWidthInBytes;

            // See if there's enough memory left in the display memory heap to
            // save this rectangle
            if (ulSaveSize >
                    (pdsurf->pjAdapterHeapTop - pdsurf->pjAdapterHeapStart))
            {
#endif
                // Not enough offscreen memory; store in system memory.
                // Calculate new buffer width, allowing for padding so we can
                // dword align
                ulSaveWidthInAlignedBytes =
                    (((ULONG)((prcl->right + 31) - (prcl->left & ~0x1F)))
                                  >> 5) << 2;

                // # of bytes to hold all 4 planes of save rect in memory
                ulSaveSize = ((ulSaveHeight * ulSaveWidthInAlignedBytes) << 2)
                        + sizeof(SAVED_SCREEN_BITS);

                // If the preallocated saved screen bits buffer is free and big
                // enough to handle this save, we'll use that
                if ((ppdev->flPreallocSSBBufferInUse == FALSE) &&
                    (ulSaveSize <= ppdev->ulPreallocSSBSize))
                {
                    // Save in preallocated buffer

                    pSSB = (PSAVED_SCREEN_BITS) ppdev->pjPreallocSSBBuffer;

                    // Mark that we're saving in the preallocated buffer
                    pSSB->bFlags = SSB_IN_PREALLOC_BUFFER;

                    // Make sure no other screen bits save tries to use the
                    //  buffer
                    ppdev->flPreallocSSBBufferInUse = TRUE;
                }
                else
                {
                    // Save in system memory buffer

                    // Allocate a structure to contain the save info and the
                    // save buffer (four planes' worth)
                    pSSB = (PSAVED_SCREEN_BITS)
                           (LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT,
                           ulSaveSize));
                    if (pSSB == NULL)
                    {
                        // Couldn't get memory, so fail this call
                        return((ULONG)0);
                    }

                    // Mark that we're not saving in display memory
                    pSSB->bFlags = 0;
                }

                // Start address at which to save, accounting for
                // the number of bytes by which to pad on the left to dword
                // align (assumes each scan line starts dword aligned, which is
                // true in 640, 800, and 1024 wide cases)
                pSSB->pjBuffer = ((PBYTE) pSSB) + sizeof(SAVED_SCREEN_BITS) +
                                 ((prcl->left >> 3) & 0x03);

                pSSB->ulSaveWidthInBytes = ulSaveWidthInBytes;

                // Distance from end of one scan to start of next (number of
                // padding bytes for dword alignment purposes)
                pSSB->ulDelta =
                        ulSaveWidthInAlignedBytes - pSSB->ulSaveWidthInBytes;

                // Save the rectangle to system memory
                vSaveScreenBitsToMemory(pdsurf,
                                        prcl,
                                        pSSB->pjBuffer,
                                        pSSB->ulSaveWidthInBytes,
                                        ulSaveHeight,
                                        ulSaveWidthInAlignedBytes);
#if OFFSCREEN_SUPPORTED
            }
            else
            {
                // Store in offscreen memory
                // Allocate a structure to contain the save info
                pSSB = (PSAVED_SCREEN_BITS)
                       (LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT,
                       sizeof(SAVED_SCREEN_BITS));
                if (pSSB == NULL)
                {
                    // Couldn't get memory, so fail this call
                    return(0);
                }
                pSSB->bFlags = SSB_IN_ADAPTER_MEMORY;

                pSSB->ulSaveWidthInBytes = ulSaveWidthInBytes;

                /***/

            }
#endif

            // Link the new saved screen bits block into the list
            pSSB->pvNextSSB = pdsurf->ssbList;
            pdsurf->ssbList = pSSB;

            return((ULONG) pSSB);

        //
        // Restore a saved screen bits block to the screen, then free it.
        //

        case SS_RESTORE:
//            ASSERT(prcl != (RECTL *) NULL,"DrvSaveScreenBits invalid prcl");

            // Point to the first block in the saved screen bits list
            pSSB = pdsurf->ssbList;

            // Try to find the specified saved screen bits block
            bIdentFound = FALSE;
            while ((pSSB != (PSAVED_SCREEN_BITS) NULL) && !bIdentFound)
            {
                if (pSSB == (PSAVED_SCREEN_BITS) iIdent)
                {
                    // It's a match; restore this block

                    // Handle copies from offscreen memory and system memory
                    // separately
#if OFFSCREEN_SUPPORTED
                    if (pSSB->bFlags & SSB_IN_ADAPTER_MEMORY)
                    {
                        vRestoreScreenBitsFromOffscreen(pdsurf,
                                                        prcl,
                                                        pSSB->pjBuffer);
                    }
                    else
                    {
#endif
                        vRestoreScreenBitsFromMemory(pdsurf,
                                                     prcl,
                                                     pSSB->pjBuffer,
                                                     pSSB->ulSaveWidthInBytes,
                                                     pSSB->ulDelta);
#if OFFSCREEN_SUPPORTED
                    }
#endif

                    bIdentFound = TRUE;
                }
                else
                {
                    // Not a match, so check another block, if there is one.
                    // Point to the next saved screen bits block
                    pSSB = (PSAVED_SCREEN_BITS) pSSB->pvNextSSB;
                }
            }

            // See if we succeeded in finding a block to restore
            if (!bIdentFound)
            {
                // It was a bad identifier, so we'll return failure

                DISPDBG((0, "DrvSaveScreenBits SS_RESTORE invalid iIdent"));
                return(FALSE);
            }

            // Always free the saved screen bits block after restoring it

        //
        // Free up the saved screen bits block.
        //

        case SS_FREE:

            // Point to the first block in the saved screen bits list
            pSSB = pdsurf->ssbList;

            // Point to the pointer to the first block, so we can unlink the
            // first block if it's the one we're freeing
            pLastSSBPtr = &pdsurf->ssbList;

            // Try to find the specified saved screen bits block
            while (pSSB != (PSAVED_SCREEN_BITS) NULL)
            {
                if (pSSB == (PSAVED_SCREEN_BITS) iIdent)
                {
                    // It's a match; free up this block

                    // Unlink the block from the list
                    *pLastSSBPtr = (PSAVED_SCREEN_BITS) pSSB->pvNextSSB;

                    // If the block is in offscreen memory, adjust the display
                    // memory heap to free up the offscreen memory allocated to
                    // the block
                    if (pSSB->bFlags & SSB_IN_ADAPTER_MEMORY)
                    {
                        if (pSSB->pjBuffer != pdsurf->pjAdapterHeapTop)
                        {
                            // The block is not on top of the heap, so compact
                            // the heap to move the newly freed space to the
                            // top of the display memory heap

                            // First, adjust the pointers to all the blocks
                            // that are above the freed one in the display
                            // memory heap
                            pSSBTemp = pdsurf->ssbList;
                            while (pSSBTemp != NULL)
                            {
                                if (pSSBTemp->pjBuffer < pSSB->pjBuffer)
                                {
                                    // This block is above the one we're
                                    // freeing in the display memory heap, so
                                    // shift its pointer down (the actual
                                    // movement of the bytes happens below)
                                    pSSBTemp->pjBuffer += pSSB->ulSize;
                                }
                                pSSBTemp = (PSAVED_SCREEN_BITS)
                                           pSSBTemp->pvNextSSB;
                            }
                            // Now actually shift all the higher blocks to
                            // squeeze out the block we just freed

                            memcpy(pdsurf->pjAdapterHeapTop + pSSB->ulSize,
                                   pdsurf->pjAdapterHeapTop,
                                   pSSB->pjBuffer - pdsurf->pjAdapterHeapTop);
                        }

                        // Adjust the top of the used display memory heap to
                        // account for the block we just freed
                        pdsurf->pjAdapterHeapTop += pSSB->ulSize;
                    }
                    else if (pSSB->bFlags & SSB_IN_PREALLOC_BUFFER)
                    {
                        // If the block's save area is in the preallocated
                        // buffer, mark that the buffer is no longer in use
                        // and is free for reuse
                        ppdev->flPreallocSSBBufferInUse = FALSE;

                        // We're done; there's nothing to free up
                        return(TRUE);
                    }

                    // Deallocate the block's memory
                    LocalFree(pSSB);

                    // We've successfully freed the block
                    return(TRUE);
                }

                // Not a match, so check another block, if there is one
                // Remember the block that points to the block we're advancing
                // to, for unlinking later
                pLastSSBPtr = (PSAVED_SCREEN_BITS *) &pSSB->pvNextSSB;

                // Point to the next saved screen bits block
                pSSB = (PSAVED_SCREEN_BITS) pSSB->pvNextSSB;
            }

            // It was a bad identifier, so we'll do nothing. We won't return
            // FALSE because SS_FREE always returns TRUE

            DISPDBG((0, "DrvSaveScreenBits SS_FREE invalid iIdent"));

            return(TRUE);

        //
        // An unknown mode was passed in.
        //

        default:

            DISPDBG((0, "DrvSaveScreenBits invalid iMode"));

            return(FALSE);
            break;
    }
}

