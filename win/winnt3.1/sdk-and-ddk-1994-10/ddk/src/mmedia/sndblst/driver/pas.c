/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    pas.c

Abstract:

    This module contains code configuration code MediaVision's Pro audio
    spectrum.  The card is run in Sound Blaster compatibiltiy mode.

    Support is provided for volume setting and line input and
    microphone mix level setting.

    The card is located by searching.  No user configuration is supported.

Author:

    Robin Speed (RobinSp) 17-Oct-1992

Environment:

    Kernel mode

Revision History:

--*/

#include <sound.h>



//--------------========================---------------------------
//---------====< GLOBAL DATA SECTION >====-------------------------
//--------------========================---------------------------

// The board signature is the first value in the PAS 16 wakeup sequence
// BC is the factory default.  A board jumpered to recognize the BD signature
// will not respond to a BC init command.

UCHAR SignatureTable[4]={0xBC,0xBD,0xBE,0xBF};



// SEARCH LOCATIONS LISTED IN ORDER OF SEARCH

ULONG search_locations[]=
    {
    0x0388,0x0384,0x038C,
    0x0288,0x0280,0x0284,0x028C
    };

//
// MPU stuff here until we work out what we want
//

#define MPU_ADDR       0x330

#define MPU_IRQ        2
#define	MPU_EMUL_IRQ   EMUL_IRQ_2


//
// Local routines
//


BOOLEAN
VerifyProHardware(
    PGLOBAL_DEVICE_INFO pGDI,
    ULONG port,
    FOUNDINFO *pFI);

BOOLEAN
VerifyNothingThere(
    PGLOBAL_DEVICE_INFO pGDI,
    ULONG port);

BOOLEAN
WakeUpAtAddress(
    PGLOBAL_DEVICE_INFO pGDI,
    ULONG wPort,
    FOUNDINFO *pFoundInfo );

void
InitProHardware(
    PFOUNDINFO pFI,
    PSB_CONFIG_DATA ConfigData);

void
InitMixerState(
    PFOUNDINFO pFI);

NTSTATUS
ReportUsage(
    PGLOBAL_DEVICE_INFO pGDI,
    ULONG BasePort
);

#ifdef ALLOC_PRAGMA
#pragma alloc_text(init,VerifyProHardware)
#pragma alloc_text(init,VerifyNothingThere)
#pragma alloc_text(init,WakeUpAtAddress)
#pragma alloc_text(init,InitProHardware)
#pragma alloc_text(init,InitMixerState)
#pragma alloc_text(init,ReportUsage)
#pragma alloc_text(init,FindPasHardware)
#pragma alloc_text(init,InitPasAndMixer)
#endif

/****************************************************************************
 *
 * Report all the ports used by the PAS assuming it's at a given port
 * location.
 *
 * We report this stuff on the driver object, the sound blaster stuff
 * gets reported on the wave in device object.
 *
 * We don't report the ad lib port here because we don't want to rule
 * out an ad lib driver loading.
 *
 ****************************************************************************/

 NTSTATUS
 ReportUsage(
     PGLOBAL_DEVICE_INFO pGDI,
     ULONG BasePort
 )
 {
     PCM_RESOURCE_LIST ResList;
     PCM_PARTIAL_RESOURCE_DESCRIPTOR Descriptor;
     ULONG NumberOfEntries;
     NTSTATUS Status;
     BOOLEAN ResourceConflict = FALSE;
     int i;

     static ULONG PortList[] = {  0xb88, 0x7f88, 0x8388, 0xf388,
                                  0xf788, 0xff88, 0x4388, 0xbf88 };

     ULONG Size;

     Size =
         FIELD_OFFSET(
             CM_RESOURCE_LIST,
             List[0].PartialResourceList.PartialDescriptors[
                 sizeof(PortList) / sizeof(PortList[0]) + 1].Type);


     //
     // Create a structure for our reporting
     //

     ResList = ExAllocatePool(PagedPool, Size);

     if (ResList == NULL) {
         return STATUS_INSUFFICIENT_RESOURCES;
     }

     RtlZeroMemory(ResList, Size);

     //
     // Copy our reporting data into the resources list
     //

     ResList->Count = 1;

     ResList->List[0].InterfaceType = pGDI->BusType;
     ResList->List[0].BusNumber = pGDI->BusNumber;

     ResList->List[0].PartialResourceList.Count =
         sizeof(PortList) / sizeof(PortList[0]) + 1;

     Descriptor = ResList->List[0].PartialResourceList.PartialDescriptors;


     for (i = 0; i < sizeof(PortList) / sizeof(PortList[0]) + 1; i++, Descriptor++) {
         Descriptor->Type = CmResourceTypePort;

         Descriptor->ShareDisposition = CmResourceShareDriverExclusive;

         //
         // NOTE - because all ports base have the lower 2 bits 0
         // and all ranges start on an aligned address it is
         // true that doing the XORing does not affect the start
         // of the range.  This would not in general be true
         //


         if (i == 0) {
             Descriptor->u.Port.Start.LowPart = PAS_2_WAKE_UP_REG;

             Descriptor->u.Port.Length = 1;
         } else {
             Descriptor->u.Port.Start.LowPart =
                 PortList[i - 1] ^ DEFAULT_BASE ^ BasePort;

             Descriptor->u.Port.Length = 4;
         }
     }

#if 0
     if (ReportDMAAndInterrupt) {

        Descriptor->Type = CmResourceTypeInterrupt;

        Descriptor->ShareDisposition = CmResourceShareDriverExclusive;

        Descriptor->Flags =
           InterruptMode == CM_RESOURCE_INTERRUPT_LATCHED;

        Descriptor->u.Interrupt.Level = InterruptNumber;

        Descriptor->u.Interrupt.Vector = InterruptNumber;

        Descriptor++;


        Descriptor->Type = CmResourceTypeDma;

        Descriptor->ShareDisposition = CmResourceShareDeviceExclusive;

        Descriptor->u.Dma.Channel = DmaChannel;

        Descriptor->u.Dma.Port = 0;  // ???
     }

#endif

     Status = IoReportResourceUsage(NULL,
                                    pGDI->DriverObject,
                                    NULL,
                                    0,
                                    pGDI->DeviceObject[WaveOutDevice],
                                    ResList,
                                    Size,
                                    FALSE,
                                    &ResourceConflict);

     if (ResourceConflict) {
         Status = STATUS_DEVICE_CONFIGURATION_ERROR;
     }

     ExFreePool(ResList);

     return Status;
 }



    /*\
;---|*|------====< DWORD GetProTableRead() >====------
;---|*|
;---|*| Detects which version of the Pro AudioSpectrum is installed
;---|*|
;---|*| Entry Conditions:
;---|*| Pointer to Profile Structure.  If the caller wants to specify
;---|*| the preferred base address for cards not yet init'd, they
;---|*| are passed in this structure.  The NumFound field indicates
;---|*| the number of location requests and the board address elements
;---|*| indicate the locations.
;---|*|
;---|*| Also passed in pointer to port (the one we found in the registry
;---|*| if any
;---|*|
;---|*| Exit Conditions:
;---|*| Returns number of cards found
;---|*| ProFile structure has been updated.
;---|*|
;   \*/


NTSTATUS
FindPasHardware(
    PGLOBAL_DEVICE_INFO pGDI,
    PSB_CONFIG_DATA ConfigData )
{
    int i;
    FOUNDINFO *pFoundInfo;
    ULONG CurrentPort;
    NTSTATUS Status;

    // RtlZeroMemory(pPf, sizeof(*pPf)); already 0 on NT

//
//  Get access to all IO ports
//

    pGDI->PASInfo.PROBase =
        SoundMapPortAddress(pGDI->BusType,
                            pGDI->BusNumber,
                            0,
                            0x10000,
                            &pGDI->MemType);

//
//  Check each possible location for a card, starting with the one
//  we were given (unless it was 0)
//
    for (i=0;
         i < sizeof(search_locations) / sizeof(search_locations[0]);
         i++)
    {

        CurrentPort = search_locations[i];

        if (CurrentPort != 0) {

            //
            // Check we have access
            //

            Status = ReportUsage(pGDI, CurrentPort);

            if (NT_SUCCESS(Status)) {

                pFoundInfo=&pGDI->PASInfo;

                if (VerifyProHardware(pGDI, CurrentPort, pFoundInfo)) {

                    return STATUS_SUCCESS;
                }
            } else {
                if (Status != STATUS_DEVICE_CONFIGURATION_ERROR) {
                    return Status;
                }
            }
        }
    }
//
//  Finally, we attempt to wake up hardware at default locations unless
//  we're already found something
//

    if (!pGDI->Hw.ThunderBoard) {
        for (i=0;
             i < sizeof(search_locations) / sizeof(search_locations[0]);
             i++)
        {

            CurrentPort = search_locations[i];
            if (CurrentPort != 0) {


                pFoundInfo=&pGDI->PASInfo;

                if ( VerifyNothingThere ( pGDI, CurrentPort ))
                {
                    //
                    // Check we have access
                    //

                    Status = ReportUsage(pGDI, CurrentPort);

                    if (NT_SUCCESS(Status)) {

                        if (WakeUpAtAddress ( pGDI, CurrentPort, pFoundInfo )) {
                            return STATUS_SUCCESS;
                        } else {
                            //
                            // Must be at the first possible location
                            //

                            return STATUS_DEVICE_CONFIGURATION_ERROR;
                        }
                    } else {
                        if (Status != STATUS_DEVICE_CONFIGURATION_ERROR) {
                            return Status;
                        }
                    }
                }
            }

        }
    }

    return STATUS_DEVICE_CONFIGURATION_ERROR;
}

    /*\
;---|*|------====< int VerifyProHardware() >====------
;---|*|
;---|*| Detects which version of the Pro AudioSpectrum is installed
;---|*|
;---|*| Entry Conditions:
;---|*|     wParam1= Base I/O address to check
;---|*|
;---|*| Exit Conditions:
;---|*|     Returns AX=PORT ADDRESS      if Pro AudioSpectrum found
;---|*|     Returns AX=NO_PAS_INSTALLED  if Pro AudioSpectrum not found
;---|*|
;---|*| WARNING:  THIS IS PASCAL TYPE, IT POPS ITS PARAMETERS
;   \*/


BOOLEAN
VerifyProHardware(
    PGLOBAL_DEVICE_INFO pGDI,
    ULONG port,
    FOUNDINFO *pFI)
{
    UCHAR bData, bTemp;

    pFI->TranslateCode = port ^ DEFAULT_BASE;

    bData=PASX_IN (pFI, INTERRUPT_CTRL_REG);

    if (bData==0xFF)                        // 0xFF usually means nothing there
        goto VerifyFailed;
    pFI->wBoardRev= (bData >>5);            // board rev is 3 topmost bits

    switch (pFI->wBoardRev)
    {
        case PAS_VERSION_1:
        //case PAS_PLUS:                    // same boardrev as PAS_SIXTEEN
        case PAS_SIXTEEN:
        case PAS_CDPC:
            break;

        default:
            goto VerifyFailed;              // unknown hardware type
    }

    PASX_OUT(pFI, INTERRUPT_CTRL_REG, bData ^ 0xE0);  // try changing version bits
    bTemp=PASX_IN (pFI, INTERRUPT_CTRL_REG);           // they should be read only

    if ((bTemp & (D7+D6+D5)) != (bData & (D7+D6+D5))) {
        PASX_OUT(pFI,  INTERRUPT_CTRL_REG, bData);     // Excuse me, stranger.
        goto VerifyFailed;
    }

    if (pFI->wBoardRev==PAS_VERSION_1) {

        pFI->Caps.CapsBits.CDInterfaceType=SCSI_TYPE;

        //
        // test for Enhanced SCSI mod (U48)
        //

        PASX_OUT(pFI,  ENHANCED_SCSI_DETECT_REG, 0 );    // write to try changing version bits
        KeStallExecutionProcessor(10); // wait 10 us
        bTemp=PASX_IN ( pFI, ENHANCED_SCSI_DETECT_REG );     // they should be read only

        switch (bTemp & 1)      // bit0==1 means old SCSI PAL
            {
            case 0:
                pFI->Caps.CapsBits.EnhancedSCSI=TRUE;
                // allow to fall thru

            case 1:
                goto ProVerified;
        }
    } else {
        // if PAS hardware installed, the reset bit can never be on

        bTemp=PASX_IN (pFI, SYSTEM_CONFIG_1);     // get PAS config register
        if (bTemp & D7)                     // D7 is reset bit
            goto VerifyFailed;


        bTemp=PASX_IN (pFI, SLAVE_MODE_READ);

        if (bTemp & SLAVE_MODE_OPL3)
            pFI->Caps.CapsBits.OPL_3=TRUE;

        if (bTemp & SLAVE_MODE_16)
            {
            pFI->Caps.CapsBits.DAC16=TRUE;
            pFI->Caps.CapsBits.DualDAC=TRUE;

            // if 16-bit DAC, and not a CDPC, it has a 508 chip.
            // Note: PAS 16 w/ VGA will have Mixer 508 also.

            if (pFI->wBoardRev != PAS_CDPC)
                pFI->Caps.CapsBits.Mixer_508=TRUE;

            }

        pFI->Caps.CapsBits.CDInterfaceType=(bTemp & (D1+D0));

        if (pFI->Caps.CapsBits.CDInterfaceType==SCSI_TYPE)
            pFI->Caps.CapsBits.SCSI_IO_16=TRUE;

        pFI->Caps.CapsBits.Slot16=TRUE;
        pFI->Caps.CapsBits.SoundBlaster=TRUE;

        bTemp=PASX_IN (pFI, MASTER_MODE_READ);        // get slave bits
        if ((bTemp & D0)==0)
            pFI->Caps.CapsBits.MCA=TRUE;

        if (bTemp & D2)
            pFI->Caps.CapsBits.CDPC=TRUE;

        pFI->wChipRev=PASX_IN (pFI, CHIP_REV);

#if 0  // We're not interested in this stuff
        if (pFI->wChipRev >= CHIP_REV_D)
        {

            bData=PASX_IN(pFI,  EMULATION_ADDRESS_POINTER );
            bTemp=PASX_IN(pFI,  COMPATIBLE_REGISTER_ENABLE );

            if (bTemp & MPU_ENABLE_BIT)     // MPU emulation Enabled?
                pFI->MPUPort=0x300 + (bData & 0xf0);

            if (bTemp & SB_ENABLE_BIT)      // SB emulation Enabled?
                {
                pFI->SBPort=0x200 + ((bData & 0x0f)<<4);
                }

            //
            // Report back IRQ usage of PAS DAC and CD
            //

            bData=PASX_IN(pFI,  IO_PORT_CONFIG_3 );
            pFI->ProIRQ=IRQTable[bData & 0x0f]; // convert to IRQ value

            pFI->CDIRQ=IRQTable[bData >> 4];     // convert to IRQ value

            //
            // Report back DMA usage of PAS
            //

            bData=PASX_IN(pFI,  IO_PORT_CONFIG_2 );
            pFI->ProDMA=DMATable[bData & (D2+D1+D0)];


            // Note: Rev D doesn't allow readback of SB IRQ/DMA pointers
            //       nor the MPU IRQ.  The "Set and forget" feature, we
            //       call it.

        }
#endif // We're not interested in this stuff

     }

ProVerified:

    dprintf2(("Found PRO hardware at %X", port));
    pFI->ProPort=port;                  // found at this port
    return TRUE;

////////////////////////////////

VerifyFailed:
    pFI->wBoardRev=0;               // found at this port
    pFI->Caps.dwCaps=0;             // No Board, No Caps
    return FALSE;

}

    /*\
;---|*|------====< BOOL VerifyNothingThere(port) >====------
;---|*|
;---|*| Tries to determine whether a quadport is vacant
;---|*|
;---|*| Entry Conditions:
;---|*|     port= Base I/O address to check
;---|*|
;---|*| Exit Conditions:
;---|*|     Returns TRUE  if nothing found
;---|*|     Returns FALSE if it looks like hardware is there
;---|*|
;   \*/

BOOLEAN
VerifyNothingThere(
    PGLOBAL_DEVICE_INFO pGDI,
    ULONG port)
{
    UCHAR   b0,b1,b2,b3;

    //
    // Check we have access
    //

    if (!NT_SUCCESS(SoundReportResourceUsage(pGDI->DeviceObject[WaveOutDevice],
                                             pGDI->BusType,
                                             pGDI->BusNumber,
                                             NULL,
                                             Latched,
                                             FALSE,
                                             NULL,
                                             &port,
                                             4))) {
        return FALSE;
    }

    b0=READ_PORT_UCHAR(pGDI->PASInfo.PROBase + port+0);
    b1=READ_PORT_UCHAR(pGDI->PASInfo.PROBase + port+1);
    b2=READ_PORT_UCHAR(pGDI->PASInfo.PROBase + port+2);
    b3=READ_PORT_UCHAR(pGDI->PASInfo.PROBase + port+3);

    if (b0==b1 && b1==b2 && b2==b3)
        return TRUE;
    else
        return FALSE;

}
    /*\
;---|*|------====< int WakeUpAtAddress(WORD wPort) >====------
;---|*|
;---|*| Tries to wake up sleeping relocatable hardware at a specified
;---|*| address.  Does not check for hardware already in that location
;---|*| If it does wake up a card, it does the minimum amount of
;---|*| initialization to enable the hardware.
;---|*|
;---|*| Entry Conditions:
;---|*|     wPort= Base I/O address to wake card up at.
;---|*|
;---|*| Exit Conditions:
;---|*|     Returns AX=PORT ADDRESS      if Pro AudioSpectrum found
;---|*|     Returns AX=0         if Pro AudioSpectrum not available
;---|*|
;   \*/
BOOLEAN
WakeUpAtAddress(
    PGLOBAL_DEVICE_INFO pGDI,
    ULONG wPort,
    FOUNDINFO *pFoundInfo )
{
    int     i,j;

    for (i = 0; i < sizeof(SignatureTable) / sizeof(SignatureTable[0]); i++)
        {
        for (j = 0; j < 20; j++)
        {
            WRITE_PORT_UCHAR(pGDI->PASInfo.PROBase + PAS_2_WAKE_UP_REG, SignatureTable[i]);
            KeStallExecutionProcessor(1);
            WRITE_PORT_UCHAR(pGDI->PASInfo.PROBase + PAS_2_WAKE_UP_REG, (UCHAR)((wPort >> 2) & 0xFF));
            KeStallExecutionProcessor(1);
        }

        if (VerifyProHardware(pGDI, wPort, pFoundInfo))

            //
            // Found one - wTranslateCode translates to the board's
            // correct port.
            //

            pFoundInfo->Caps.CapsBits.Did_HW_Init=TRUE;

            if (pFoundInfo->wBoardRev > PAS_VERSION_1 )
            {
                /* Only enable FM feature if we're going to sit at
                   the right address */

                UCHAR Features = PCM_FEATURE_ENABLE | MIXER_FEATURE_ENABLE |
                                 SB_FEATURE_ENABLE | FM_FEATURE_ENABLE;

                PASX_OUT(pFoundInfo, FEATURE_ENABLE, Features);
            }

            return (TRUE);
        }
    return (FALSE);     // not found

}

    /*\
;---|*|------====< BOOL VerifyLegalAddress(WORD wPort) >====------
;---|*|
;---|*| Tests a caller-nominated base port address for being a legal
;---|*| place for a relocatable PAS to reside.
;---|*|
;---|*| Entry Conditions:
;---|*|     wPort= Base I/O address to check
;---|*|
;---|*| Exit Conditions:
;---|*|     Returns AX= TRUE if the address is legal
;---|*|     Returns AX= FALSE otherwise
;---|*|
;   \*/
BOOLEAN
VerifyLegalAddress(ULONG wPort)
{
    if ((wPort <0x240) || (wPort > 0x3c0) || (wPort & 0xf))
        return FALSE;
    else
        return TRUE;
}

void InitPasAndMixer(PFOUNDINFO pFI, PSB_CONFIG_DATA ConfigData)
{
    InitProHardware(pFI, ConfigData);
    InitMixerState(pFI);
}

void InitProHardware(PFOUNDINFO pFI, PSB_CONFIG_DATA ConfigData)
{
    UCHAR   bInput;
    int     i;

//if (!pf.ProCard.Caps.CapsBits.Did_HW_Init)
//  continue;                           // not sure about this

    switch (pFI->wBoardRev) {

    case PAS_VERSION_1:
        break;


    //case PAS_PLUS:                    // same boardrev as PAS_SIXTEEN
    case PAS_CDPC:
    case PAS_SIXTEEN:

        // no interrupts, please!
        PASX_OUT(pFI,  INTERRUPT_ENABLE, 0 );
        PASX_OUT(pFI,  INTERRUPT_CTRL_REG, 0 );

        if (pFI->wBoardRev == PAS_CDPC || pFI->Caps.CapsBits.DAC16) {
            //
            // PAS 16 or CDPC
            //

            PASX_OUT(pFI, FEATURE_ENABLE,PCM_FEATURE_ENABLE |
                                    FM_FEATURE_ENABLE  |
                                    MIXER_FEATURE_ENABLE |
                                    SB_FEATURE_ENABLE);
        } else {
            //
            // PAS plus
            //

            PASX_OUT(pFI, FEATURE_ENABLE,PCM_FEATURE_ENABLE |
                                    FM_FEATURE_ENABLE  |
                                    SB_FEATURE_ENABLE);

        }

        PASX_OUT(pFI, PCM_CONTROL,PCM_STEREO|PCM_ENGINE);

        //
        // disable original PAS emulation
        //

        bInput = PASX_IN(pFI,  SYSTEM_CONFIG_1 );
        bInput = bInput | 2;

        PASX_OUT(pFI,  SYSTEM_CONFIG_1, bInput ); // set sys config 1

        KeStallExecutionProcessor(10);  // wait 10 units?

        PASX_OUT(pFI,  SYSTEM_CONFIG_2, 0 ); // set sys config 2

        if (pFI->wBoardRev != PAS_CDPC) {

            PASX_OUT(pFI, SYSTEM_CONFIG_3,C3_ENHANCED_TIMER |
                                            C3_INVERT_BCLK  |
                                            C3_SYNC_PULSE);
        } else {
            PASX_OUT(pFI, SYSTEM_CONFIG_3, 0);
        }

      //    if (pf.ProCard[0].wChipRev >= CHIP_REV_D)
      //        {
            PASX_OUT(pFI,  EMULATION_ADDRESS_POINTER,
                (MPU_ADDR&0x0f0)+((ConfigData->Port & 0x0f0)>>4) );

            pFI->MPUPort=MPU_ADDR;
            pFI->SBPort=ConfigData->Port;

            PASX_OUT(pFI,  EMULATION_INTERRUPT_POINTER,
                (SOUND_DEF_DMACHANNEL << 6)+
                ((ConfigData->InterruptNumber == 9 ? 2 :
                  ConfigData->InterruptNumber)<<3)+
                MPU_EMUL_IRQ);

            pFI->MPUIRQ=MPU_IRQ;
            pFI->SBIRQ=ConfigData->InterruptNumber;
            pFI->SBDMA=SOUND_DEF_DMACHANNEL;


            PASX_OUT(pFI,
                     COMPATIBLE_REGISTER_ENABLE,
                     COMPAT_MPU+COMPAT_SB);
        //  }

        PASX_OUT(pFI,  INTERRUPT_ENABLE, INT_SB );
        if (pFI->wBoardRev == PAS_CDPC || !pFI->Caps.CapsBits.DAC16) {
            PASX_OUT(pFI,  FILTER_REGISTER, FILTER_NOMUTE );
        }

        break;


    }   // switch (wBoardRev);
}

#define     INPUT_VOLUME    0xd8d8
#define     OUTPUT_VOLUME   0xc0c0
#define     MAX_OUTPUT_VOLUME  0xFFFF
void
InitMixerState(PFOUNDINFO pFI)
{
    dprintf4(("Calling SetFilter"));
    SetFilter(pFI, FILTER_MUTE);

    SetInput(pFI,
            IN_SYNTHESIZER,             // input number (see patch.h)
            INPUT_VOLUME,               // range 0-31
            _LEFT,                      // _LEFT or _RIGHT (can't do both at once)
            MIXCROSSCAPS_NORMAL_STEREO, // see pasdef.h
            OUT_AMPLIFIER);             // output number (see patch.h)


    SetInput(pFI,
            IN_SYNTHESIZER,             // input number (see patch.h)
            INPUT_VOLUME,               // range 0-31
            _RIGHT,                     // _LEFT or _RIGHT (can't do both at once)
            MIXCROSSCAPS_NORMAL_STEREO, // see pasdef.h
            OUT_AMPLIFIER);             // output number (see patch.h)

    SetInput(pFI,
            IN_MIXER,                   // input number (see patch.h)
            0, //INPUT_VOLUME,               // range 0-31
            _LEFT,                      // _LEFT or _RIGHT (can't do both at once)
            MIXCROSSCAPS_NORMAL_STEREO, // see pasdef.h
            OUT_AMPLIFIER );            // output number (see patch.h)

    SetInput(pFI,
            IN_MIXER,                   // input number (see patch.h)
            0, //INPUT_VOLUME,               // range 0-31
            _RIGHT,                     // _LEFT or _RIGHT (can't do both at once)
            MIXCROSSCAPS_NORMAL_STEREO, // see pasdef.h
            OUT_AMPLIFIER);             // output number (see patch.h)

    SetInput(pFI,
            IN_EXTERNAL,                // input number (see patch.h)
            INPUT_VOLUME,               // range 0-31
            _LEFT,                      // _LEFT or _RIGHT (can't do both at once)
            MIXCROSSCAPS_NORMAL_STEREO, // see pasdef.h
            OUT_AMPLIFIER); //OUT_PCM );                  // output number (see patch.h)

    SetInput(pFI,
            IN_EXTERNAL,                // input number (see patch.h)
            INPUT_VOLUME,               // range 0-31
            _RIGHT,                     // _LEFT or _RIGHT (can't do both at once)
            MIXCROSSCAPS_NORMAL_STEREO, // see pasdef.h
            OUT_AMPLIFIER); //OUT_PCM );                  // output number (see patch.h)

    SetInput(pFI,
            IN_INTERNAL,                // input number (see patch.h)
            INPUT_VOLUME,               // range 0-31
            _LEFT,                      // _LEFT or _RIGHT (can't do both at once)
            MIXCROSSCAPS_NORMAL_STEREO, // see pasdef.h
            OUT_AMPLIFIER); //OUT_PCM );                  // output number (see patch.h)

    SetInput(pFI,
            IN_INTERNAL,                // input number (see patch.h)
            INPUT_VOLUME,               // range 0-31
            _RIGHT,                     // _LEFT or _RIGHT (can't do both at once)
            MIXCROSSCAPS_NORMAL_STEREO, // see pasdef.h
            OUT_AMPLIFIER); //OUT_PCM );                  // output number (see patch.h)

    SetInput(pFI,
            IN_MICROPHONE,              // input number (see patch.h)
            INPUT_VOLUME,               // range 0-31
            _LEFT,                      // _LEFT or _RIGHT (can't do both at once)
            MIXCROSSCAPS_NORMAL_STEREO, // see pasdef.h
            OUT_PCM );                  // output number (see patch.h)

    SetInput(pFI,
            IN_MICROPHONE,              // input number (see patch.h)
            INPUT_VOLUME,               // range 0-31
            _RIGHT,                     // _LEFT or _RIGHT (can't do both at once)
            MIXCROSSCAPS_NORMAL_STEREO, // see pasdef.h
            OUT_PCM );                  // output number (see patch.h)

    SetInput(pFI,
            IN_PCM,                     // input number (see patch.h)
            0, //INPUT_VOLUME,               // range 0-31
            _LEFT,                      // _LEFT or _RIGHT (can't do both at once)
            MIXCROSSCAPS_NORMAL_STEREO, // see pasdef.h
            OUT_AMPLIFIER );            // output number (see patch.h)

    SetInput(pFI,
            IN_PCM,                     // input number (see patch.h)
            0, //INPUT_VOLUME,               // range 0-31
            _RIGHT,                     // _LEFT or _RIGHT (can't do both at once)
            MIXCROSSCAPS_NORMAL_STEREO, // see pasdef.h
            OUT_AMPLIFIER );            // output number (see patch.h)

    SetInput(pFI,
            IN_PC_SPEAKER,              // input number (see patch.h)
            INPUT_VOLUME,               // range 0-31
            _LEFT,                      // _LEFT or _RIGHT (can't do both at once)
            MIXCROSSCAPS_NORMAL_STEREO, // see pasdef.h
            OUT_AMPLIFIER );            // output number (see patch.h)

    SetInput(pFI,
            IN_PC_SPEAKER,              // input number (see patch.h)
            INPUT_VOLUME,               // range 0-31
            _RIGHT,                     // _LEFT or _RIGHT (can't do both at once)
            MIXCROSSCAPS_NORMAL_STEREO, // see pasdef.h
            OUT_AMPLIFIER );            // output number (see patch.h)

#if 0
    SetInput(pFI,
            IN_SNDBLASTER,              // input number (see patch.h)
            SBWaveOutVolume.Left,       // range 0-31
            _LEFT,                      // _LEFT or _RIGHT (can't do both at once)
            MIXCROSSCAPS_NORMAL_STEREO, // see pasdef.h
            OUT_AMPLIFIER );            // output number (see patch.h)

    SetInput(pFI,
            IN_SNDBLASTER,              // input number (see patch.h)
            SBWaveOutVolume.Right,      // gain range 0-31
            _RIGHT,                     // _LEFT or _RIGHT (can't do both at once)
            MIXCROSSCAPS_NORMAL_STEREO, // see pasdef.h
            OUT_AMPLIFIER );            // output number (see patch.h)
#endif

    SetOutput(pFI,
               OUT_AMPLIFIER,           // output number
               OUTPUT_VOLUME,           // volume range 0-63
               _LEFT );                 // _LEFT or _RIGHT (can't do both at once)

    SetOutput(pFI,
               OUT_AMPLIFIER,           // output number
               OUTPUT_VOLUME,           // volume range 0-63
               _RIGHT );                // _LEFT or _RIGHT (can't do both at once)

    SetOutput(pFI,
               OUT_PCM,                 // output number
               MAX_OUTPUT_VOLUME,       // volume range 0-63
               _LEFT );                 // _LEFT or _RIGHT (can't do both at once)

    SetOutput(pFI,
               OUT_PCM,                 // output number
               MAX_OUTPUT_VOLUME,       // volume range 0-63
               _RIGHT );                // _LEFT or _RIGHT (can't do both at once)

    SetFilter(pFI, FILTER_LEVEL_6);

}
