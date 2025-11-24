/*++

Copyright (c) 1993-95  Microsoft Corporation

Module Name:

    command.c

Abstract:

    This file contains the code for managing Command Blocks on the
    NE3200's Command Queue.

Environment:

    Kernel Mode - Or whatever is the equivalent on OS/2 and DOS.

--*/

//
// So we can trace things...
//
//#define STATIC

#include <ne3200sw.h>
#ifdef NDIS_WIN
    #pragma LCODE
#endif


VOID
NE3200SubmitCommandBlock(
    IN PNE3200_ADAPTER Adapter,
    IN PNE3200_SUPER_COMMAND_BLOCK CommandBlock
    )

/*++

Routine Description:

    Submit a complete Command Block for execution by the NE3200.

    NOTE:  This routine assumes that it is called with the lock held.

Arguments:

    Adapter - The adapter that points to the ring entry structures.

    CommandBlock - Holds the pointer to the Command Block to be
    submitted.

Return Value:

    None.

--*/

{

    //
    // Pointer to the most recently submitted Command Block.
    //
    PNE3200_SUPER_COMMAND_BLOCK PreviousCommandBlock;

    //
    // the state of the command block
    //
    USHORT CardState;

    // Ensure that our command block is on an even boundary.
    //
    ASSERT(!(NdisGetPhysicalAddressLow(CommandBlock->Self) & 1));

    //
    // Timestamp the command block.
    //

    CommandBlock->Timeout = FALSE;

    //
    // If the adapter is currently executing commands, then add this to
    // the end of the waiting list, else submit this command to the card.
    //

    if (Adapter->FirstCommandOnCard == NULL) {

        //
        // Submit this command to the card.
        //

        ASSERT(Adapter->FirstWaitingCommand == NULL);

        IF_LOG('1');

        Adapter->FirstCommandOnCard = CommandBlock;
        Adapter->LastCommandOnCard = CommandBlock;

        IF_NE3200DBG(SUBMIT) {

            DPrint2(
                "Starting command @ %08lX\n",
                (ULONG)NdisGetPhysicalAddressLow(CommandBlock->Self)
                );

        }

        NE3200_WRITE_COMMAND_POINTER(
            Adapter,
            NdisGetPhysicalAddressLow(CommandBlock->Self)
            );

        NE3200_WRITE_LOCAL_DOORBELL_INTERRUPT(
            Adapter,
            NE3200_LOCAL_DOORBELL_NEW_COMMAND
            );

    } else {

        //
        // Get the pointer to the most recently submitted
        // Command Block.
        //

        PreviousCommandBlock = Adapter->LastWaitingCommand;

        //
        // Link the most recently submitted Command Block to the
        // current Command Block.
        //

        if (PreviousCommandBlock != NULL ) {

            PreviousCommandBlock->Hardware.NextPending =
               NdisGetPhysicalAddressLow(CommandBlock->Self);
            PreviousCommandBlock->NextCommand = CommandBlock;

        } else {

            Adapter->FirstWaitingCommand = CommandBlock;

        }

        //
        // Update the pointer to the most recently submitted Command Block.
        //

        Adapter->LastWaitingCommand = CommandBlock;

        IF_NE3200DBG(SUBMIT) {

            DPrint2(
                "Queuing command block @ %08lX\n",
                (ULONG)CommandBlock
                );

        }

        IF_LOG('2');

    }

    IF_NE3200DBG(SUBMIT) {

        DPrint2(
            "Adapter->FirstCommandOnCard = %08lX\n",
            (ULONG)Adapter->FirstCommandOnCard
            );

        DPrint2(
            "Adapter->LastCommandOnCard  = %08lX\n",
            (ULONG)Adapter->LastCommandOnCard
            );

        DPrint2(
            "Adapter->FirstWaitingCard = %08lX\n",
            (ULONG)Adapter->FirstWaitingCommand
            );

        DPrint2(
            "Adapter->LastWaitingCommand  = %08lX\n",
            (ULONG)Adapter->LastWaitingCommand
            );

    }

}

BOOLEAN
NE3200AcquireCommandBlock(
    IN PNE3200_ADAPTER Adapter,
    OUT PNE3200_SUPER_COMMAND_BLOCK * CommandBlock
    )

/*++

Routine Description:

    Sees if a Command Block is available and if so returns its index.

    NOTE: This routine assumes that the lock is held.

Arguments:

    Adapter - The adapter that points to the ring entry structures.

    CommandBlock - Will receive a pointer to a Command Block if one is
    available.  This value is unpredicable if there is not a free
    Command Block.

Return Value:

    Returns FALSE if there are no free Command Blocks.

--*/

{

    if (Adapter->NumberOfAvailableCommandBlocks) {

        //
        // Return the Command Block pointer.
        //

        *CommandBlock = Adapter->NextCommandBlock;

        //
        // Update the head of the Command Queue.
        //

        Adapter->NextCommandBlock++;

        if (Adapter->NextCommandBlock >=
            Adapter->CommandQueue + Adapter->NumberOfCommandBlocks) {

            Adapter->NextCommandBlock = Adapter->CommandQueue;

        }

        //
        // Update number of available Command Blocks.
        //

        Adapter->NumberOfAvailableCommandBlocks--;

        IF_NE3200DBG(ACQUIRE) {

            DPrint2(
                "Acquired adapter command block @ %08lX\n",
                *(PULONG)CommandBlock
                );

        }

        return TRUE;

    } else {

        IF_NE3200DBG(ACQUIRE) {

            DPrint1(
                "Failed to acquire adapter command block\n"
                );

        }

        Adapter->Stage2Open = FALSE;
        return FALSE;

    }

}

VOID
NE3200AcquirePublicCommandBlock(
    IN PNE3200_ADAPTER Adapter,
    OUT PNE3200_SUPER_COMMAND_BLOCK * CommandBlock
    )

/*++

Routine Description:

    Gets the public command block.

    NOTE: This routine assumes that the lock is held.

Arguments:

    Adapter - The adapter that points to the ring entry structures.

    CommandBlock - Will receive a pointer to a Command Block.

Return Value:

    None.

--*/

{

    //
    // This is a pointer to the "private" Command Block.
    //
    PNE3200_SUPER_COMMAND_BLOCK PrivateCommandBlock = Adapter->PublicCommandBlock;

    //
    // Initialize the Command Block.
    //

    NdisZeroMemory(
        PrivateCommandBlock,
        sizeof(NE3200_SUPER_COMMAND_BLOCK)
        );

    PrivateCommandBlock->Hardware.NextPending = NE3200_NULL;
    PrivateCommandBlock->Self = Adapter->PublicCommandBlockPhysical;

    //
    // Return the Command Block pointer.
    //

    *CommandBlock = PrivateCommandBlock;

    IF_NE3200DBG(ACQUIRE) {

        DPrint2(
            "Acquired private command block @ %08lX\n",
            (ULONG)PrivateCommandBlock
            );

    }

}


VOID
NE3200RelinquishCommandBlock(
    IN PNE3200_ADAPTER Adapter,
    IN PNE3200_SUPER_COMMAND_BLOCK CommandBlock
    )

/*++

Routine Description:

    Relinquish the Command Block resource.  If this is a "public"
    Command Block, then update the CommandQueue.  If this is a
    "private" Command Block, then free its memory.

    NOTE: This routine assumes that the lock is held.

Arguments:

    Adapter - The adapter that owns the Command Block.

    CommandBlock - The Command Block to relinquish.

Return Value:

    None.

--*/

{

    //
    // Points to the owning open binding if this is a "private"
    // Command Block.  NULL if this is a "public" Command Block.
    //
    PNE3200_OPEN OwningOpenBinding;

    IF_NE3200DBG(SUBMIT) {

        DPrint2(
                "Relinquishing command @ %08lX\n",
                (ULONG)NdisGetPhysicalAddressLow(CommandBlock->Self)
                );

    }

    //
    // If this is the last pending command block, then we
    // can nuke the adapter's last pending command pointer.
    //

    if (CommandBlock == Adapter->LastCommandOnCard) {

        //
        // If there is a waiting chain of commands -- submit those
        //

        if (Adapter->FirstWaitingCommand != NULL) {

            //
            // Submit this command to the card.
            //

            Adapter->FirstCommandOnCard = Adapter->FirstWaitingCommand;
            Adapter->LastCommandOnCard = Adapter->LastWaitingCommand;
            Adapter->FirstWaitingCommand = NULL;
            Adapter->LastWaitingCommand = NULL;

            IF_NE3200DBG(SUBMIT) {

                DPrint2(
                    "Starting command @ %08lX\n",
                    (ULONG)NdisGetPhysicalAddressLow(Adapter->FirstCommandOnCard->Self)
                    );

            }

            NE3200_WRITE_COMMAND_POINTER(
                Adapter,
                NdisGetPhysicalAddressLow(Adapter->FirstCommandOnCard->Self)
                );

            NE3200_WRITE_LOCAL_DOORBELL_INTERRUPT(
                Adapter,
                NE3200_LOCAL_DOORBELL_NEW_COMMAND
                );

        } else {

            Adapter->LastCommandOnCard = NULL;
            Adapter->FirstCommandOnCard = NULL;

        }

    } else {

        //
        // Point the adapter's first pending command to the
        // next command on the command queue.
        //

        Adapter->FirstCommandOnCard = CommandBlock->NextCommand;

    }

    CommandBlock->Hardware.NextPending = NE3200_NULL;
    CommandBlock->Hardware.State = NE3200_STATE_FREE;

    //
    // Retrive the owning open binding from the Command Block.
    //

    OwningOpenBinding = CommandBlock->OwningOpenBinding;

    if (OwningOpenBinding == NULL) {

        //
        // This is a "public" Command Block.
        //

        Adapter->NumberOfAvailableCommandBlocks++;

    }

}
