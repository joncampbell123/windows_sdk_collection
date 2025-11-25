/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    volume.c

Abstract:

    This module contains code for set and get volume IOCTLs

Author:

    Robin Speed (RobinSp) 21-Oct-1992

Environment:

    Kernel mode

Revision History:

--*/

#include <string.h>
#include <soundlib.h>          // Definition of what's in here

//
// Internal routines
//

VOID
SoundVolumeNotify(
    IN OUT PLOCAL_DEVICE_INFO pLDI
);

//
// Return the current volume setting.  If the card doesn't support
// volume setting return maximum volume (FFFFFFFF)
//

NTSTATUS
SoundIoctlGetVolume(
    IN     PLOCAL_DEVICE_INFO pLDI,
    IN     PIRP pIrp,
    IN     PIO_STACK_LOCATION IrpStack
)
{
    PWAVE_DD_VOLUME pVol;

    if (pLDI->CreationFlags & SOUND_CREATION_NO_VOLUME) {
        return STATUS_NOT_SUPPORTED;
    }

    if (IrpStack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(WAVE_DD_VOLUME)) {
        dprintf1(("Supplied buffer to small for requested data"));
        return STATUS_BUFFER_TOO_SMALL;
    }


    //
    // say how much we're sending back
    //

    pIrp->IoStatus.Information = sizeof(WAVE_DD_VOLUME);

    //
    // cast the buffer address to the pointer type we want
    //

    pVol = (PWAVE_DD_VOLUME)pIrp->AssociatedIrp.SystemBuffer;

    //
    // fill in the info
    //

    *pVol = pLDI->Volume;

    return STATUS_SUCCESS;
}
//
// Set the volume
//

NTSTATUS
SoundIoctlSetVolume(
    IN     PLOCAL_DEVICE_INFO pLDI,
    IN     PIRP pIrp,
    IN     PIO_STACK_LOCATION IrpStack
)
{
    PWAVE_DD_VOLUME pVol;

    //
    // See if volume setting is supported
    //

    if (pLDI->CreationFlags & SOUND_CREATION_NO_VOLUME) {
        return STATUS_NOT_SUPPORTED;
    }

    if (IrpStack->Parameters.DeviceIoControl.InputBufferLength < sizeof(WAVE_DD_VOLUME)) {
        dprintf1(("Supplied buffer to small for requested data"));
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // cast the buffer address to the pointer type we want
    //

    pVol = (PWAVE_DD_VOLUME)pIrp->AssociatedIrp.SystemBuffer;

    //
    // If the device is 'open' or aux (which is not explicitly opened to
    // play) and the volume has changed then actually set the volume.
    //

    if (// (pLDI->DeviceType == AUX_DEVICE || *pLDI->DeviceBusy) &&
        (pLDI->Volume.Left != pVol->Left ||
         pLDI->Volume.Right != pVol->Right)) {

        //
        // Set the volume in the device
        //

        pLDI->Volume = *pVol;

        //
        // say how much we're sending back
        //

        pIrp->IoStatus.Information = sizeof(WAVE_DD_VOLUME);

        //
        // Not all devices have volume setting routines.  The 'real'
        // device may in fact be sitting in user mode waiting for a
        // volume change to complete
        //

        (*pLDI->DeviceInit->HwSetVolume)(pLDI);

#ifdef VOLUME_NOTIFY
        //
        // Tell anyone who's waiting for it to change
        //

        SoundVolumeNotify(pLDI);
#endif // VOLUME_NOTIFY

        pLDI->VolumeChanged = TRUE;
    }

    return STATUS_SUCCESS;
}

#ifdef VOLUME_NOTIFY


NTSTATUS
SoundIoctlGetChangedVolume(
    IN OUT PLOCAL_DEVICE_INFO pLDI,
    IN     PIRP pIrp,
    IN     PIO_STACK_LOCATION IrpStack
)
/*++

Routine Description:

    Checks the parameters and limits waiters (arbitrarily) to 8.
    Tests if the current volume is the same as that passed in.  If
    not then return the new volume immediately.  If the volume has
    not changed then put the Irp in the waiting list.

Arguments:

    pLDI - Local device info
    pIrp - Pointer to IO request packet
    IrpStack - stack location info

Return Value:

    STATUS_BUFFER_TOO_SMALL - sizes passed in too small
    STATUS_INSUFFICIENT_RESOURCES - if too many people are trying to wait
    STATUS_SUCCESS - if volume has changed
    STATUS_PENDING - if volume was same as before.

--*/
{
    PWAVE_DD_VOLUME pVol;
    pVol = (PWAVE_DD_VOLUME)pIrp->AssociatedIrp.SystemBuffer;

    //
    // Check input parameters
    //

    if (pLDI->CreationFlags & SOUND_CREATION_NO_VOLUME) {
        return STATUS_NOT_SUPPORTED;
    }

    if (IrpStack->Parameters.DeviceIoControl.InputBufferLength <
            sizeof(AUX_DD_VOLUME) ||
        IrpStack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(AUX_DD_VOLUME)) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // See if we can complete it now - ie if the device is 'playing'
    // and the volume set is not what was passed in.
    //

    if (// (pLDI->DeviceType == AUX_DEVICE || *pLDI->DeviceBusy) &&
        (pVol->Left != pLDI->Volume.Left || pVol->Right != pLDI->Volume.Right))
    {
        *pVol = pLDI->Volume;

        pIrp->IoStatus.Information = sizeof(*pVol);

        return STATUS_SUCCESS;
    } else {

        IoMarkIrpPending(pIrp);
        pIrp->IoStatus.Status = STATUS_PENDING;

        SoundAddIrpToCancellableQ(&pLDI->VolumeQueue, pIrp, FALSE);

        return STATUS_PENDING;
    }

}


VOID
SoundVolumeNotify(
    IN OUT PLOCAL_DEVICE_INFO pLDI
)
/*++

Routine Description:

    Notify all waiters on this device that the volume has changed.
    This involves just copying the data into their Irps and
    completing them.

Arguments:

    pLDI - Local device info

Return Value:

    None

--*/
{
    PLIST_ENTRY ListHead;
    ListHead = &pLDI->VolumeQueue;

    //
    // Remove all the queue entries, completing all
    // the Irps represented by the entries
    //

    for (;;) {
        PIRP pIrp;

        pIrp = SoundRemoveFromCancellableQ(ListHead);

        if (pIrp == NULL) {
            break;
        }
        *((PWAVE_DD_VOLUME)pIrp->AssociatedIrp.SystemBuffer) =
            pLDI->Volume;

        pIrp->IoStatus.Status = STATUS_SUCCESS;
        pIrp->IoStatus.Information = sizeof(WAVE_DD_VOLUME);

        //
        // Bump priority here because the application may still be trying
        // to be real-time
        //
        IoCompleteRequest(pIrp, IO_SOUND_INCREMENT);
    }
}


#endif // VOLUME_NOTIFY

//
// Default hardware volume setting routine - does nothing
//

VOID
SoundNoVolume(
    PLOCAL_DEVICE_INFO pLDI
)
{
   return;
}

VOID
SoundSaveDeviceVolume(
    PLOCAL_DEVICE_INFO pLDI,
    PWSTR KeyName
)
{
    if (pLDI->VolumeChanged) {

        dprintf3(("Saving volume setting for device type %4.4s : %8X, %8X",
                 &pLDI->Key, pLDI->Volume.Left, pLDI->Volume.Right));

        SoundWriteRegistryDWORD(KeyName,
                                pLDI->DeviceInit->LeftVolumeName,
                                pLDI->Volume.Left);
        SoundWriteRegistryDWORD(KeyName,
                                pLDI->DeviceInit->RightVolumeName,
                                pLDI->Volume.Right);
        pLDI->VolumeChanged = FALSE;
    }
}


#if 0
//
// Pitch is always 1.0 as the card does not support pitch shift
//

NTSTATUS SoundIoctlGetPitch(PLOCAL_DEVICE_INFO pLDI, PIRP pIrp, PIO_STACK_LOCATION IrpStack)
{
    PWAVE_DD_PITCH pPitch;

    if (pLDI->DeviceType != WAVE_OUT) {
        return STATUS_INVALID_PARAMETER;
    }

    if (IrpStack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(WAVE_DD_PITCH)) {
        dprintf1(("Supplied buffer to small for requested data"));
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // say how much we're sending back
    //

    pIrp->IoStatus.Information = sizeof(WAVE_DD_PITCH);

    //
    // cast the buffer address to the pointer type we want
    //

    pPitch = (PWAVE_DD_PITCH)pIrp->AssociatedIrp.SystemBuffer;

    //
    // fill in the info
    //

    pPitch->Pitch = 0x10000;

    return STATUS_SUCCESS;
}

//
// Playback rate is always 1.0 as the card does not support rate shift
//

NTSTATUS SoundIoctlGetPlaybackRate(PLOCAL_DEVICE_INFO pLDI, PIRP pIrp, PIO_STACK_LOCATION IrpStack)
{
    PWAVE_DD_PLAYBACK_RATE pPlaybackRate;

    if (pLDI->DeviceType != WAVE_OUT) {
        return STATUS_INVALID_PARAMETER;
    }

    if (IrpStack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(WAVE_DD_PLAYBACK_RATE)) {
        dprintf1(("Supplied buffer to small for requested data"));
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // say how much we're sending back
    //

    pIrp->IoStatus.Information = sizeof(WAVE_DD_PLAYBACK_RATE);

    //
    // cast the buffer address to the pointer type we want
    //

    pPlaybackRate = (PWAVE_DD_PLAYBACK_RATE)pIrp->AssociatedIrp.SystemBuffer;

    //
    // fill in the info
    //

    pPlaybackRate->Rate = 0x10000;

    return STATUS_SUCCESS;
}

#endif

