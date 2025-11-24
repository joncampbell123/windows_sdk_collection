        page    60, 132

;******************************************************************************
;---------------------------------------------------------------------------;
;
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1992- 1995     Microsoft Corporation.  All Rights Reserved.
;
;---------------------------------------------------------------------------;
;
;   Title:    MSSNDSYS.VXD - MICROSOFT Windows Sound System VXD Driver
;
;   Module:   MSSNDSYS.INC - public equates/structure definitions
;
;   Version:  1.00
;
;   Date:     March 17, 1992
;
;******************************************************************************
;
;   NOTES:
;       This VxD handles contention for the Sound System Audio card.  Note
;       this is NOT a full virtualizing driver.
;
;       If you are including this file in a VxD, you MUST include VMM.INC
;       first.
;
;******************************************************************************

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   The following equates can either be used by pmode apps, rmode apps,
;   or VxD's.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

Multimedia_OEM_ID               equ 0440h   ; MS Reserved OEM #34
MSSNDSYS_Device_ID              equ (Multimedia_OEM_ID + 31)

MSSNDSYS_OEM_MSFT               equ 0000h
MSSNDSYS_OEM_AST                equ 0001h
MSSNDSYS_OEM_ICL                equ 0002h

MSSNDSYS_Ver_Major              equ 4       ; version 4.00 of this VxD
MSSNDSYS_Ver_Minor              equ 0

;
; PMode and RMode API indexes
;

MSS_API_Get_Version             equ 0       ; get version info on MSSNDSYS

MSS_API_Get_Info                equ 1
MSS_API_GetInfoF_DevNode        equ 0001h   ; devnode specified in ECX
MSS_API_GetInfoF_Mask           equ 0001h   ; valid functions

MSS_API_Acquire                 equ 2

MSS_API_Release                 equ 3

MSS_API_Copy_To_Buffer          equ 4

MSS_API_Copy_From_Buffer        equ 5

;
; New APIs for Win95
;

MSS_API_Get_DMA_Count           equ 6

;
; Flags for acquiring CODEC and OPL3
;
fSS_ASS_Acquire_CODEC           equ 00000001b
fSS_ASS_Acquire_OPL3            equ 00000010b

;
; Error returns for MSS_API_Acquire_SndSys (returned in Client_AX)
;
MSS_API_ASS_Err_Bad_Base_Port   equ 0001h
MSS_API_ASS_Err_Already_Owned   equ 0002h
MSS_API_ASS_Err_State_Unknown   equ 8000h

;
; Error returns for MSS_API_Release_SndSys (returned in Client_AX)
;

MSS_API_RSS_Err_Bad_Base_Port   equ 0001h
MSS_API_RSS_Err_Not_Yours       equ 0002h

;
; subfunction  codes for MSSNDSYS_API_Get_DMA_Count
;

sfSS_GDC_Invalid        equ     0000h
sfSS_GDC_DAC_Count      equ     0001h
sfSS_GDC_ADC_Count      equ     0002h

;
;       Sound Blaster virtualization registers, stored per
;       hardware instance.
;

MAXLEN_DATAQUEUE        equ     10h

SBVIRTREGS      struc

        sbvr_dwDataReadPtr      dd      ?
        sbvr_dwDataQueuePtr     dd      ?
        sbvr_pWriteFunc         dd      ?

        sbvr_abDataRead         db      MAXLEN_DATAQUEUE dup (?)

        sbvr_bDataAvail         db      ?
        sbvr_bDataWrite         db      ?

        sbvr_bDataWritePtr      db      ?
        sbvr_bSpeakerStatus     db      ?

        sbvr_wSampleRate        dw      ?
        sbvr_wModeFlags         dw      ?

        sbvr_bCurrentRate       db      ?      
        sbvr_bAttenuation       db      ?
        sbvr_bApplyReg          db      ?
        sbvr_bTableMunge        db      ?

        sbvr_dwCODECRate        dd      ?

        sbvr_fwDMASim           dw      0   ; PCMCIA DMA sim flags

        sbvr_dwDMASimAddr       dd      0   ; PCMCIA DMA sim client buffer

        sbvr_wBlockSize         dw      ?
        sbvr_wReserved          dw      ?   ; filler

SBVIRTREGS      ends

;
;   Structure used for storing information about the state of the Sound
;   System Audio Card. This information can be retrieved by any VM by calling
;   the MSS_API_Get_Info function.
;

SNDSYSINFO struc
        ssi_dwSize              dd  ?   ; size of structure
        ssi_wFlags              dw  0   ; see flags defined below
        ssi_wIOAddress          dw  0   ; IO Address card is configured on
        ssi_bIRQ                db  -1  ; IRQ that _must_ be used by client
        ssi_bDMADAC             db  -1  ; DMA channel for DAC
        ssi_bDMAADC             db  -1  ; DMA channel for ADC
        ssi_bVersionCODEC       db  0   ; revision of the CODEC
        ssi_wVersionVxD         dw  0   ; version of this VxD
        ssi_wVersionPAL         dw  0   ; high byte=product, low byte=revision
        ssi_dwDMABufferHandle   dd  0   ; DMA buffer handle from VMM
        ssi_lpDMABufferPhys     dd  0   ; _physical address_ of DMA buffer
        ssi_lpDMABufferLinear   dd  0   ; _linear address_ of DMA buffer
        ssi_dwDMABufferLen      dd  0   ; length in bytes of DMA buffer
        ssi_wDMABufferSelector  dw  0   ;
        ssi_wIOAddressOPL3      dw  0   ;
        ssi_dwCODECOwnerCur     dd  ?   ; VM handle of current CODEC owner
        ssi_dwCODECOwnerLast    dd  ?   ; VM handle of previous CODEC owner
        ssi_dwIRQHandle         dd  ?   ; VPICD IRQ handle
        ssi_dwOPL3OwnerCur      dd  ?   ; VM handle of current OPL3 owner
        ssi_dwOPL3OwnerLast     dd  ?   ; VM handle of previous OPL3 owner

        ssi_dwDMADACHandle      dd  ?   ; Handle for DAC DMA channel
        ssi_dwDMAADCHandle      dd  ?   ; Handle for ADC DMA channel

        ssi_wIOAddressSB        dw  ?   ; Sound Blaster base (virt or HW)

        ; Actual CODEC base returned by MSSNDSYS.VXD v4.00

        ssi_wCODECBase          dw  0   ; actual CODEC base

        ; MSSNDSYS.VXD v4.00 removed the hot key handles from the SSI

        ssi_hlPipe              dd  0   ; pipe list
        ssi_wSCSIStatus         dw  0   ; SCSI status register for
                                        ;    Sound/SCSI II combo
        ssi_wReserved           dw  ?   ; filler

        ssi_wCODECClass         dw  ?   ; CODEC classification
        ssi_wAGABase            dw  ?   ; AGA base for Compaq BA

        ; OEM ID for extended capabilities, Microsoft's ID is 0.

        ssi_wOEM_ID             dw  MSSNDSYS_OEM_MSFT

        ; Hardware options returned by MSSNDSYS.VXD v4.00

        ssi_wHardwareOptions    dw  0   ; flags defined in DAK.INC

        ; PnP DevNode in MSSNDSYS.VXD v4.00

        ssi_dn                  dd  0   ; PnP devnode (hardware instance)

        ; MSSNDSYS.VXD v4.00 expanded the SSI structure to include
        ; handles to I/O trapping stubs and also included the
        ; Sound Blaster virtual registers in the per hardware
        ; instance structure.

        ssi_hAutoSelectStubs    dd  0   ; trap stubs for AutoSelect
        ssi_hCODECStubs         dd  0   ; trap stubs for CODEC
        ssi_hOPL3Stubs          dd  0   ; trap stubs for OPL3
        ssi_hSBStubs            dd  0   ; trap stubs for SB virtualization or
                                        ;     SB hardware register plane

        ssi_SBVirtRegs          db  (SIZE SBVIRTREGS) dup (?)

        ;
        ;
        ; PCMCIA DMA virtualization...
        ;

        ssi_dwSBDMAHandle       dd  0   ; SB DMA channel for PCMCIA virt.
                                        ;    or real handle for HWSB

        ssi_bOPL3Status         db  ?   ; OPL3 status (when eating ints)
        ssi_bPCMCIA_Flags       db  ?   ; PCMCIA specific flags
        ssi_wPCMCIA_SRAMBase    dw  ?   ; SRAM data port base
        ssi_wPCMCIA_Id          dw  ?   ; board ID
        ssi_wReserved2          dw  ?   ; filler

ifdef AZTECH
        ;
        ; Aztech variable
        ;
        ssi_bAZT_CardType       db  0   ; Aztech Sound Card Type
                                        ; = Washington16 / Nova16
        ssi_bWSTMode            db  0   ; Washington 16 mode
                                        ; = WSS_MODE / SBPRO_MODE
        ssi_wReserved3          dw  ?   ; filler
endif


SNDSYSINFO ends

SSI_FLAG_DISABLEWARNING         equ 0001h   ; warnings disabled on contention
SSI_FLAG_SHAREDIRQEVENT         equ 0002h   ; scheduled shared IRQ event
SSI_FLAG_SINGLEMODEDMA          equ 0004h   ; for Symphony chip sets
SSI_FLAG_IRQSHARING             equ 0008h   ; shared IRQ

SSI_FLAG_SUSPENDED              equ 0010h   ; saved state when suspended

SSI_FLAG_BUSTYPE_ISAPNP         equ 0100h   ; ISAPNP enumerated
SSI_FLAG_BUSTYPE_PCMCIA         equ 0200h   ; PCMCIA enumerated
SSI_FLAG_BUSTYPE_PNPBIOS        equ 0400h   ; PNPBIOS enumerated

SSI_FLAG_HWSB                   equ 1000h   ; Sound Blaster in HW
SSI_FLAG_OPL3STATUS             equ 2000h   ; OPL3 status register saved
SSI_FLAG_SBACQUIRED             equ 4000h   ; set when VM acquires virt SB
SSI_FLAG_IRQWASUNMASKED         equ 8000h   ; irq being used was unmasked

PCMCIA_ID_WAVJAMMER_REV_E       equ 0001h   ; .WAVJammer Rev. E
PCMCIA_ID_WAVJAMMER_REV_F       equ 0002h   ; .WAVJammer Rev. F
PCMCIA_ID_WAVJAMMER_REV_OTHER   equ 0003h   ; .WAVJammer unknown rev

PCMCIA_ID_SOUNDSCSI             equ 0100h   ; Sound/SCSI II Combo

SBVIRT_DMASIMF_AUTOINIT         equ 0001h   ; auto-init DMA simulation
SBVIRT_DMASIMF_MASKED           equ 0002h   ; channel masked

SBVIRT_FLAG_BUSTYPE_PCMCIA      equ 0001h   ; SB virtualization on PCMCIA
SBVIRT_FLAG_VIRT_DMA            equ 0010h   ; SB virtualization wants DMA
SBVIRT_FLAG_VIRT_IRQ            equ 0020h   ; SB virtualization wants IRQ
