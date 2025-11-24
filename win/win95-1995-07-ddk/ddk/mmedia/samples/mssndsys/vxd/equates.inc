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
;   Title:    MSSNDSYS.386 - MICROSOFT Windows Sound System 386 Driver
;
;   Module:   EQUATES.INC - equates/structure definitions
;
;   Version:  1.00
;
;   Date:     March 17, 1992
;
;******************************************************************************

;==============================================================================
;                              E Q U A T E S
;==============================================================================

SS_PAL_PRODUCTREV_RQD   equ 04h

BUG macro stuff
IF1
%out ----&stuff&
endif
endm

LOHIB struc
        lob         db  ?
        hib         db  ?
LOHIB ends

MSSNDSYS_CB_STRUCT struc

        mscb_hlhws              dd      ?               ; list handle to 

MSSNDSYS_CB_STRUCT ends

VM_HWSTATE_NODE struc

        hws_dwFlags             dd      ?

        ; These "levels" determined when the hardware was
        ; acquired by the VM.

        hws_pSSI                dd      ?               ; associated SSI
        hws_abCODECState        db      32 dup (?)
        hws_dwDMACount          dd      ?
        hws_dwDMAAddr           dd      ?
        hws_wDMAMode            dw      ?
        hws_wReserved           dw      ?

VM_HWSTATE_NODE ends

HWCB_FLAG_ALREADYWARNEDCODEC    equ     00000001b
HWCB_FLAG_ALREADYWARNEDOPL3     equ     00000010b

HARDWARE_INSTANCE_NODE  struc

        hwl_pSSI                dd      ?               ; pointer to SSI

HARDWARE_INSTANCE_NODE  ends


;
;   Register offsets from base port for SndSys Audio card. Note that this
;   does not include the OPL3 at 0388h - 038Bh
;
;   NOTE: we MUST trap SS_PAL_RESERVED01 and SS_PAL_RESERVED02 along
;   with SS_PAL_CONFIG and SS_PAL_ID because the PAL aliases to the
;   first 4 ports.
;

SS_PAL_CONFIG           equ 00h
SS_PAL_RESERVED01       equ 01h
SS_PAL_RESERVED02       equ 02h
SS_PAL_ID               equ 03h

SS_LAST_PORT_PAL        equ 03h
.errnz (SS_LAST_PORT_PAL - SS_PAL_ID)


;
; NOTE! In version 4.0, these definitions have changed to be
; zero based from the CODEC base address, not from the AutoSelect
; location as in previous versions!
;

SS_CODEC_ADDRESS        equ 00h
SS_CODEC_DATA           equ 01h    
SS_CODEC_STATUS         equ 02h
SS_CODEC_DIRECT         equ 03h

;
; Last port to trap (inclusive) for PAL/CODEC on SndSys Audio card.
;
SS_LAST_PORT_CODEC      equ 03h
.errnz (SS_LAST_PORT_CODEC - SS_CODEC_DIRECT)

SS_LAST_PORT_SB         equ 0Fh

;
; SS_PAL_ID port related constants
;
PAL_ID_REVBITS          equ 07h
PAL_ID_PRODUCTBITS      equ 38h
PAL_ID_IRQSENSE         equ 40h
PAL_ID_8BITSLOT         equ 80h

;
;   Registers in the CODEC--addressed by writing register number you want
;   to 'select' to the SS_CODEC_ADDRESS port. SS_CODEC_DATA then can be
;   read or written depending on what you want to do with the selected
;   register.
;
;   NOTE: you must read the SS_CODEC_ADDRESS port and wait for the high
;   bit to clear--this is necessary to wait for the CODEC to finish
;   initializing (if it is). see MSSNDSYS_CODEC_Wait_For_Ready.
;

CODEC_REG_LEFTINPUT     equ 00h
CODEC_REG_RIGHTINPUT    equ 01h
CODEC_REG_LEFTAUX1      equ 02h
CODEC_REG_RIGHTAUX1     equ 03h
CODEC_REG_LEFTAUX2      equ 04h
CODEC_REG_RIGHTAUX2     equ 05h
CODEC_REG_LEFTOUTPUT    equ 06h
CODEC_REG_RIGHTOUTPUT   equ 07h
CODEC_REG_DATAFORMAT    equ 08h
CODEC_REG_INTERFACE     equ 09h
CODEC_REG_DSP           equ 0Ah
CODEC_REG_TEST          equ 0Bh
CODEC_REG_MISC          equ 0Ch
CODEC_REG_LOOPBACK      equ 0Dh
CODEC_REG_UPPERBASE     equ 0Eh
CODEC_REG_LOWERBASE     equ 0Fh
ifdef AZTECH
CODEC_REG_CAPDATAFMT    equ 1Ch
endif

CODEC_NUM_IDXREGS       equ 10h

CODEC_MODE_MCE          equ 40h
CODEC_MODE_TRD          equ 20h
AUTO_CAL                equ 08h


;
; Port offsets on OPL3
;
MSSNDSYS_DEF_BASE_OPL3  equ 0388h
SS_OPL3_0               equ 00h
SS_OPL3_1               equ 01h
SS_OPL3_2               equ 02h
SS_OPL3_3               equ 03h

;
; Last port to trap (inclusive) for OPL3 on SndSys Audio card.
;
SS_LAST_PORT_OPL3       equ 03h
.errnz (SS_LAST_PORT_OPL3 - SS_OPL3_3)

;
; AGA offsets (Compaq's Business Audio)
;

AGA_PCR                 equ     0000h           ; peripheral config reg
AGA_OSCR                equ     0001h           ; option slot config reg
AGA_PRMR                equ     0003h           ; PBIC resource mapping reg

;
; OPL3 registers
;

AD_LSI                  equ     0001h
AD_LSI2                 equ     0101h
AD_TIMER1               equ     0002h
AD_TIMER2               equ     0003h
AD_MASK                 equ     0004h
AD_CONNECTION           equ     0104h
AD_NEW                  equ     0105h
AD_NTS                  equ     0008h
AD_MULT                 equ     0020h
AD_MULT2                equ     0120h
AD_LEVEL                equ     0040h
AD_LEVEL2               equ     0140h
AD_AD                   equ     0060h
AD_AD2                  equ     0160h
AD_SR                   equ     0080h
AD_SR2                  equ     0180h
AD_FNUMBER              equ     00a0h
AD_FNUMBER2             equ     01a0h
AD_BLOCK                equ     00b0h
AD_BLOCK2               equ     01b0h
AD_DRUM                 equ     00bdh
AD_FEEDBACK             equ     00c0h
AD_FEEDBACK2            equ     01c0h
AD_WAVE                 equ     00e0h
AD_WAVE2                equ     01e0h

;
; PIC register defintions
;

PIC_MASTER_OCW_REG      equ     020h
PIC_MASTER_MASK_REG     equ     021h
PIC_SLAVE_OCW_REG       equ     0A0h
PIC_SLAVE_MASK_REG      equ     0A1h

PIC_IRR_NEXT            equ     0Ah
PIC_ISR_NEXT            equ     0Bh
PIC_POLL_MODE           equ     0Ch

PIC_SPECIFIC_EOI        equ     60h
PIC_SPECIFIC_EOI_SLAVE  equ     62h

PIC_POLL_IRQ_FIRED      equ     80h
PIC_POLL_IRQ_MASK       equ     07h

;
; DMA register definitions
; 

DMA8ADR                 equ     00h
DMA8CNT                 equ     01h
DMA8STAT                equ     08h
DMA8SMR                 equ     0ah
DMA8MOD                 equ     0bh
DMA8CLR                 equ     0ch

;
; CODEC IDs
;

VER_AD1848J             equ     09h
VER_AD1848K             equ     0Ah
VER_CSPROTO             equ     81h
VER_CS4248              equ     8Ah

CODEC_J_CLASS           equ     00h
CODEC_K_CLASS           equ     01h
CODEC_KPLUS_CLASS       equ     02h

;
; 1848 specific configuration bits
;

AD1848_CONFIG_PEN       equ     01h     ; Playback Enable
AD1848_CONFIG_CEN       equ     02h     ; Capture Enable
AD1848_CONFIG_SDC       equ     04h     ; Single DMA Channel
AD1848_CONFIG_ACAL      equ     08h     ; Auto-Calibrate
AD1848_CONFIG_RESERVED  equ     30h     ; Reserved
AD1848_CONFIG_PPIO      equ     40h     ; Playback PIO Enable
AD1848_CONFIG_CPIO      equ     80h     ; Capture PIO Enable

CS4231_MISC_MODE2       equ     40h     ; Mode 2 enable

;
; New Media Sound/SCSI II combo equates
;

NMC_REG_CTRL_WAVJAMMER          equ     0Ah

NMCSS16_CTRL_INTDISABLE         equ     08h
NMCSS16_CTRL_CODECINTSTATUS     equ     40h 

AIC6x60_INTSTATUS               equ     20h

ifdef AZTECH
SBPRO_MODE              equ     00h
WSS_MODE                equ     01h
endif

;===========================================================================
;                   Bit fields used for hwi_HardwareOptions
;===========================================================================

;          
;                      Hardware
;                      Options
;
;          +---+---+---+---+---+---+---+---+
;  byte 0  | 0 | 0 | 0 | 0 | X | X | X | X |
;          +---+---+---+---+---+---+---+---+
;
;                                        ^
;                                        |
;                                    ^   +--------- MSFT Hardware?
;                                    |
;                                ^   +------------- MSFT Support?
;                                |
;                             ^  +----------------- Auto Select/Detect?
;                             |
;                             +-------------------- FM synth on board?
;
;          +---+---+---+---+---+---+---+---+
;  byte 1  | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |        RESERVED!
;          +---+---+---+---+---+---+---+---+

DAK_MSFTHARDWARE        equ     0001h      ; obsolete
DAK_AUTOSELECT          equ     0004h      ; PAL? (Auto-Options?)
DAK_FMSYNTH             equ     0008h      ; synth on board?
DAK_COMPAQBA            equ     0010h      ; Compaq Business Audio detected
DAK_COMPAQI             equ     0020h      ;     Compaq BA without AGA
DAK_DUALDMA             equ     0040h      ; Set when using sim. rec/play


;
; Get_pSSI_From_XXX flags:
;

pSSI_FromDevNode        equ     0000h
pSSI_FromIRQ            equ     0001h
pSSI_FromDMADAC         equ     0002h
pSSI_FromDMAADC         equ     0003h
pSSI_FromCODEC          equ     0004h
pSSI_FromOPL3           equ     0005h
pSSI_FromSBDMA          equ     0006h

fpSSI_FromDWord         equ     0000h
fpSSI_FromWord          equ     0001h

fIAV_ForReal            equ     0001h

ifdef AZTECH
AZTECH_WASHINGTON       equ     0011h
AZTECH_NOVA             equ     000Ch
endif
