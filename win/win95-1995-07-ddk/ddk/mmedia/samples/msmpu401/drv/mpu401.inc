
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
;
;   mpu401.inc
;
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   equates
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 

TRUE          equ 1
FALSE         equ 0

DSR           equ 80H       ; data set ready 0 = ready
DRR           equ 40H       ; data receive ready 0 = ready


PIC_EOI_MASTER      equ 20h             ; where to send master EOI
PIC_EOI_SLAVE       equ 0A0h            ; where to send slave EOI

PIC_IMR_MASTER      equ 21h
PIC_IMR_SLAVE       equ 0A1h


; index MPU-401 registers

MPU401_REG_DATA         equ     00h
MPU401_REG_STATUS       equ     01h
MPU401_REG_COMMAND      equ     01h

MPU401_DSR              equ     80h     ; data set ready, 0 = ready
MPU401_DRR              equ     40h     ; data receive ready, 0 = ready

MPU401_CMD_UART_MODE    equ     3fh     ; put card in 'UART' mode
MPU401_CMD_RESET        equ     0ffh    ; reset command

;
;   MIDI input and output state flags...
;

MIF_ALLOCATED       equ 00000001b
MIF_STARTED         equ 00000010b
MIF_INISR           equ 10000000b

MOF_ALLOCATED       equ 00000001b

;
;   For InitResetAdapter.  _POWERON is 'power on' state; _UART is what
;   this driver requires the adapter to be set to.
;

IRA_MODE_POWERON    equ 0
IRA_MODE_UART       equ 1

HARDWAREINSTANCE struc

   hwi_pVxDEntry                dd      ?
   hwi_cAcquire                 dd      ?

   hwi_wHardwareOptions         dw      ?
   hwi_wIOAddressMPU401         dw      ?
   hwi_bIRQ                     db      ?
   hwi_bReserved                db      ?
   hwi_dn                       dd      ?

   hwi_uVxDId                   dw      ?
   hwi_cReference               dw      ?
   hwi_cEnable                  dw      ?
   hwi_fEnabled                 dw      ?

   ; IRQ stuff

   hwi_uISRDataSel              dw      ?
   hwi_uISRCodeSel              dw      ?

   hwi_bIntVector               db      ?
   hwi_bOrigIntMask             db      ?
   hwi_dwOldISR                 dd      ?
   hwi_wEOICommands             dw      ?
   hwi_fISRCanReadData          dw      ?

   ; isr pipe storage

   hwi_hpisr                    dd      ?
   hwi_fnisrPipe                dd      ?

   ; midi flags

   hwi_bMidiInFlags             db      ?
   hwi_bMidiOutFlags            db      ?
   hwi_wMidiInPersistence       dw      ?

   ;
   ; Allocated MIDI output/input clients, for this hardware
   ; instance... if any.
   ;
   ; NOTE! These are used by the interrupt handler and 
   ; output data functions to send/receive full-duplex data
   ; when processing the MIDI stream.
   ;

   hwi_pmic                     dw      ?
   hwi_pmoc                     dw      ?

   ; link to next instance

   hwi_pNext                    dw      ?

   ; midi streams info (ifdef MIDI_STREAMS)
   ;

  ;hwi_msd                      db (SIZE MIDISTREAMDATA) dup (?)
   ;

HARDWAREINSTANCE ends

PORTALLOC struc

   pa_phwi              dw      ?
   pa_hMidi             dw      ?
   pa_dwCallback        dd      ?
   pa_dwInstance        dd      ?
   pa_dwFlags           dd      ?

PORTALLOC ends

MIDIIN_QUEUE_SIZE  equ  64 

MIDIINCLIENT struc

   mic_pa               db      (SIZE PORTALLOC) dup (?)
   mic_wDataQHead       dw      ?
   mic_wDataQTail       dw      ?
   mic_abDataQueue      db      (MIDIIN_QUEUE_SIZE) dup (?)

   mic_fSysEx           db      ?
   mic_bStatus          db      ?
   mic_bBytesLeft       db      ?
   mic_bBytePos         db      ?

   mic_dwShortMsg       dd      ?
   mic_dwMsgTime        dd      ?
   mic_dwRefTime        dd      ?

   mic_dwCurData        dd      ?
   mic_lpmhQueue        dd      ?

MIDIINCLIENT ends

;---------------------------------Macro---------------------------------;
;
; EnterCrit
;
;   saves the current state of the interrupt flag on the stack then
;   disables interrupts.
;
; Registers Destroyed:
;       BX, FLAGS
;
;------------------------------------------------------------------------;

EnterCrit macro
    local   no_cli
    pushf
    pushf
    pop     bx
    test    bh,2            ; if interupts are already off, dont blow
    jz      no_cli          ; ... ~300 clocks doing the cli
    cli
no_cli:
endm

;---------------------------------Macro---------------------------------;
;
; LeaveCrit
;
;   restore the interrupt state saved by EnterCritAndTrashBX
;
; Registers Destroyed:
;       BX, FLAGS
;
;------------------------------------------------------------------------;

LeaveCrit macro reg
    local   no_sti
    pop     bx
    test    bh, 2
    jz      no_sti
    sti
no_sti:
endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   debug support
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ifdef DEBUG
    extrn OutputDebugStr:far          ; mmsystem
    extrn _wDebugLevel:word           ; initc.c
endif

D1 macro text
    DOUT 1, < ",13,10,"MSMPU401: &text&>
    endm
D2 macro text
    DOUT 2, < &text&>
    endm
D3 macro text
    DOUT 3, < &text&>
    endm
D4 macro text
    DOUT 4, < &text&>
    endm

DOUT macro level, text
    local   string_buffer
    local   wrong_level

ifdef DEBUG

_DATA segment
string_buffer label byte
    db      "&text&",0
_DATA ends

    cmp     [_wDebugLevel], level
    jl      wrong_level
    push    DataBASE
    push    DataOFFSET string_buffer
    call    OutputDebugStr
wrong_level:
endif
    endm


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   assert macros
;
;   AssertF byte        -- fail iff byte==0
;   AssertT byte        -- fail iff byte!=0
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

AssertF     macro exp
    local   assert_ok
ifdef DEBUG
    push    ax
    
    mov     al,exp
    or      al,al
    jnz     assert_ok

    D1      <AssertF fail (&exp&)>
    int     3

assert_ok:
    pop     ax
endif
    endm

AssertT     macro exp
    local   assert_ok
ifdef DEBUG
    push    ax
    
    mov     al,exp
    or      al,al
    jz      assert_ok

    D1      <AssertT fail (&exp&)>
    int     3

assert_ok:
    pop     ax
endif
    endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; reminder macro
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

BUG macro stuff
if1
%out ----&stuff&
endif
endm
