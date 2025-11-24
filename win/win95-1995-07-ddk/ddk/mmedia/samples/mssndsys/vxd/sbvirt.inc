        page    60, 132

;******************************************************************************
;---------------------------------------------------------------------------;
;
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1992- 1995	Microsoft Corporation.	All Rights Reserved.
;
;---------------------------------------------------------------------------;
;
;   Title:    MSSNDSYS.VXD - MICROSOFT Windows Sound System VxD
;
;   Module:   SBVIRT.INC - Include file for SBVIRT.ASM
;
;   Version:  1.00
;
;   Date:     November 24, 1992
;
;******************************************************************************
;
;   Functional Description:
;
;      Traps given DOS IO addresses and maps to WSS IO.
;
;******************************************************************************

;
; Port offsets from base port for Sound Blaster DSP
;

DSP_PORT_CMSD0      equ 0000h  ; C/MS music voice 1-6 data port, write only
DSP_PORT_CMSR0      equ 0001h  ; C/MS music voice 1-6 register port, write only
DSP_PORT_CMSD1      equ 0002h  ; C/MS music voice 7-12 data port, write only
DSP_PORT_CMSR1      equ 0003h  ; C/MS music voice 7-12 register port, write only

DSP_PORT_MIXREG     equ 0004h  ; Pro card mixer address register, write only
DSP_PORT_MIXDATA    equ 0005h  ; Pro card mixer data register, read/write

DSP_PORT_RESET      equ 0006h  ; DSP Reset, write only
DSP_PORT_07h        equ 0007h  ; reserved port

DSP_PORT_FMD0       equ 0008h  ; FM music data/status port, read/write 
DSP_PORT_FMR0       equ 0009h  ; FM music data/status port, write only

DSP_PORT_READ       equ 000Ah  ; DSP Read data, read only
DSP_PORT_0Bh        equ 000Bh  ; reserved port
DSP_PORT_WRITE      equ 000Ch  ; DSP Write data or command, write
DSP_PORT_0Dh        equ 000Dh  ; reserved port
DSP_PORT_DATAAVAIL  equ 000Eh  ; DSP Data available status (bit 7), read only
DSP_PORT_0Fh        equ 000Fh  ; reserved port (used on Pro cards)

;
; DSP commands
;

MIDIRD          equ 31h         ; interrupt-driven midi input
MIDIRD2         equ 35h         ; interrupt-driven midi input
MIDIRDTS        equ 37h         ; interrupt-driven midi input (time-stamped)
MIDIWR          equ 38h         ; midi output
WAVEWRPIO       equ 10h         ; wave output (programmed I/O)
WAVEWR          equ 14h         ; interrupt-driven 8 bit linear wave output
WAVEWRA         equ 1Ch         ; auto mode 8 bit out
WAVERD          equ 24h         ; interrupt-driven 8 bit linear wave input
WAVERDA         equ 2Ch         ; auto mode 8 bit in
HIGHSPEED       equ 90h         ; high speed mode
SETSAMPRATE     equ 40h         ; set sample rate
SETBLCKSIZE     equ 48h         ; set block size
SPKRON          equ 0D1h        ; speaker on
SPKROFF         equ 0D3h        ; speaker off
SPKRSTATUS      equ 0D8h        ; speaker status (0=off, FF=on)
HALTDMA         equ 0D0h        ; halt DMA
CONTDMA         equ 0D4h        ; continue DMA
STOPAUTO        equ 0DAh        ; stop DMA autoinit mode
INVERTER        equ 0E0h        ; byte inverter
GETDSPVER       equ 0E1h        ; get dsp version
RESVD_0         equ 0E2h        ; jump table munging
RESVD_1         equ 0E4h        ; load register
RESVD_2         equ 0E8h        ; apply register
GENERATEINT     equ 0F2h        ; cause sndblst to generate an interrupt.

;
; offsets of mixer ports
;

MIXADDRPORT     equ 4h
MIXDATAPORT     equ 5h
WARMSTARTPORT   equ 6h

;
; mixer registers
;

DATARESETREG    equ 00h
MSTRVOLREG      equ 02h
VOICEVOLREG     equ 04h
FMVOLREG        equ 06h
CDVOLREG        equ 08h
MICVOLREG       equ 0Ah
ADCINPREG       equ 0Ch
VTSCREG         equ 0Eh

;
; initial register values
;

SB_REG_READ_INIT        equ     0AAh
SB_REG_WRITE_INIT       equ     07Fh
SB_REG_DATAAVAIL_INIT   equ     07Fh

;
; specific data returned through DSP commands 
;

;
; simulated DSP version information...
;

DSP_RETURN_DSP20       equ     0202h           ; version 2.02
DSP_RETURN_DSP15       equ     0501h           ; version 1.05
DSP_RETURN_DSPVERPLUS  equ     5742h           ; version marker

;
; data available flags
;

FLAG_DATAREADY  equ     10000000b

;
; write ready flags
;

FLAG_WRITEBUSY  equ     10000000b

;
; speaker status
;

STATUS_SPEAKER_ON       equ     0FFh
STATUS_SPEAKER_OFF      equ     00h

;
; mode flags
;

MODE_PPIO               equ     0001h
MODE_DMAREQ             equ     0002h
MODE_IRQACK             equ     0004h
MODE_FLOPPED            equ     0008h
MODE_AUTODMA            equ     0010h

