;---------------------------------------------------------------------------;
;
;   DRIVER.INC
;
;---------------------------------------------------------------------------;
;---------------------------------------------------------------------------;
;
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1991 - 1995    Microsoft Corporation.  All Rights Reserved.
;
;---------------------------------------------------------------------------;


;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   equates and structure definitions
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

PIC_EOI_MASTER          equ 20h         ; where to send master EOI
PIC_EOI_SLAVE           equ 0A0h        ; where to send slave EOI

PIC_IMR_MASTER          equ 21h
PIC_IMR_SLAVE           equ 0A1h

;
; Port for CODEC
;

CODEC_ADDRESS           equ 00h
CODEC_DATA              equ 01h
CODEC_STATUS            equ 02h
CODEC_DIRECT            equ 03h

;
; Direction (ADC vs DAC) declarations
;

DIRECTION_DAC           equ 00h
DIRECTION_ADC           equ 01h

;
; who's using the interrupt
;
INT_FREE                equ 0           ; nobody
INT_WAVEOUT             equ 1           ; wave output
INT_WAVEIN              equ 2           ; wave input

;
;   Wave input and output state flags...
;

WIF_ALLOCATED           equ 00000001b
WIF_STARTED             equ 00000010b
WIF_SUSPENDED           equ 00000100b
WIF_RESTART             equ 00001000b

WOF_ALLOCATED           equ 00000001b
WOF_SUSPENDED           equ 00000100b
WOF_RESTART             equ 00001000b

LOWPRISAVE struc

   lps_dwUser                   dd      ?
   lps_fwidStarted              dw      ?
   lps_dwWaveFormat             dd      ?
   lps_lpwidQueue               dd      ?
   lps_wDMAHalfBufSize          dw      ?

LOWPRISAVE ends

HARDWAREINSTANCE struc

   hwi_wHardwareOptions         dw      ?
   hwi_wFlags                   dw      ?
   hwi_wOEM_ID                  dw      ?
   hwi_wIOAddressCODEC          dw      ?
   hwi_wIOAddressAGA            dw      ?
   hwi_wIOAddressOPL3           dw      ?
   hwi_wSRAMBase                dw      ?
   hwi_bIRQ                     db      ?
   hwi_bPlaybackDMA             db      ?
   hwi_bCaptureDMA              db      ?
   hwi_wCODECClass              dw      ?
   hwi_lpDMABufferPhys          dd      ?
   hwi_dwDMABufferLen           dd      ?
   hwi_wDMABufferSelector       dw      ?
   hwi_dn                       dd      ?

   hwi_cReference               dw      ?
   hwi_cEnable                  dw      ?

   hwi_fEnabled                 dw      ?
   hwi_fJustStart               dw      ?
   hwi_fBadBoard                dw      ?
   hwi_fDo11kHzMCE              dw      ?
   hwi_fSingleModeDMA           dw      ?

   hwi_wLastDMAPos              dw      ?
   hwi_wSamplesPerSec           dw      ?
   hwi_bMute                    db      ?
   hwi_bModeChange              db      ?
   hwi_fwodDMABusy              dw      ?
   hwi_fwidDMABusy              dw      ?
   hwi_pmi                      dd      ?
   hwi_dwWaitLoop               dd      ?

   hwi_bAcquireCountCODEC       db      ?
   hwi_bAcquireCountOPL3        db      ?

   ; DMA stuff

   hwi_portCaptureDMAAdr        db      ?
   hwi_portCaptureDMACnt        db      ?
   hwi_portCaptureDMASMR        db      ?
   hwi_portCaptureDMAMod        db      ?
   hwi_portCaptureDMAClr        db      ?
   hwi_portCaptureDMAPReg       db      ?
   hwi_portCaptureEnable        db      ?
   hwi_portCaptureDisable       db      ?
   hwi_portCaptureDMARead       db      ?

   hwi_portPlaybackDMAAdr       db      ?
   hwi_portPlaybackDMACnt       db      ?
   hwi_portPlaybackDMASMR       db      ?
   hwi_portPlaybackDMAMod       db      ?
   hwi_portPlaybackDMAClr       db      ?
   hwi_portPlaybackDMAPReg      db      ?
   hwi_portPlaybackEnable       db      ?
   hwi_portPlaybackDisable      db      ?
   hwi_portPlaybackDMAWrite     db      ?

   hwi_fDMADone                 dw      ?
   hwi_wDMABufferLenMinusOne    dw      ?
   hwi_wDMAHalfBufSize          dw      ?
   hwi_wCODECSamples            dw      ?
   hwi_wBytesPerSample          dw      ?

   hwi_lpDMABuffer              dd      ?

   hwi_wDMAPhysAddr             dw      ?
   hwi_bDMAPhysPage             db      ?

   ; IRQ stuff

   hwi_uISRDataSel              dw      ?
   hwi_uISRCodeSel              dw      ?

   hwi_bIntVector               db      ?
   hwi_bOrigIntMask             db      ?
   hwi_dwOldISR                 dd      ?
   hwi_wEOICommands             dw      ?
   hwi_bIntUsed                 db      ?

   ; wid/wod common

   hwi_fAcceptCloseRates        dw      ?
   hwi_dwCurCODECOwner          dd      ?
   hwi_dwWaveFormat             dd      ?
   hwi_wOldFormat               dw      ?
   hwi_bwidFlags                db      ?
   hwi_bwodFlags                db      ?

   ; wid storage

   hwi_fwidStarted              dw      ?
   hwi_fLowPriStateSaved        dw      ?
   hwi_dwLowUser                dd      ?
   hwi_lps                      db      SIZE LOWPRISAVE dup (?)
   hwi_lpwidQueue               dd      ?
   hwi_hpwidCurData             dd      ?
   hwi_dwwidCurCount            dd      ?
   hwi_dwwidLeftPeak            dd      ?
   hwi_dwwidRightPeak           dd      ?

   ; wod storage

   hwi_fwodStarted              dw      ?
   hwi_lpSilenceStart           dd      ?
   hwi_wSilenceSize             dw      ?
   hwi_fwodPaused               dw      ?
   hwi_lpwodQueue               dd      ?
   hwi_lpwodDeadHeads           dd      ?
   hwi_hpwodCurData             dd      ?
   hwi_dwwodCurCount            dd      ?
   hwi_lpwodLoopStart           dd      ?
   hwi_dwwodLoopCount           dd      ?
   hwi_fwodSkipThisLoop         dw      ?
   hwi_fwodBreakLoop            dw      ?
   hwi_dwwodLeftPeak            dd      ?
   hwi_dwwodRightPeak           dd      ?
   hwi_dwInterruptTimeStamp     dd      ?
   hwi_dwPauseTimeDelta         dd      ?
   hwi_dwLastPosition           dd      ?

   ; mixer storage

   hwi_hpmxd                    dd      ?
   hwi_fnmxdPipe                dd      ?

   hwi_bDACToCODECLeft          db      ?
   hwi_bDACToCODECRight         db      ?

   ; link to next instance

   hwi_pNext                    dw      ?

HARDWAREINSTANCE ends

;---------------------------------Macro---------------------------------;
;
; EnterCrit
;
;   saves the current state of the interupt flag on the stack then
;   disables interupts.
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
;   restore the interupt state saved by EnterCrit
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
; pause
;   macro to insure that an instruction fetch occurs between IN and/or
;   OUT instructions on the PC-AT machine ALSO defeat the prefetch cache!
;
; Registers Destroyed:
;   None
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pause macro
        jmp     $+2
endm


;---------------------------------------------------------------------------;
;
;   debug support
;
;---------------------------------------------------------------------------;

ifdef DEBUG
    extrn OutputDebugStr:far          ; mmsystem
    extrn _wDebugLevel:word           ; initc.c
endif

D1 macro text
    DOUT 1, < ",13,10,"SNDSYS: &text&>
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


;---------------------------------------------------------------------------;
;
;   assert macros
;
;   AssertF byte        -- fail iff byte==0
;   AssertT byte        -- fail iff byte!=0
;
;---------------------------------------------------------------------------;

AssertF     macro exp
    local   assert_ok
ifdef DEBUG
    push    ax

    mov     al, exp
    or      al, al
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

    mov     al, exp
    or      al, al
    jz      assert_ok

    D1      <AssertT fail (&exp&)>
    int     3

assert_ok:
    pop     ax
endif
    endm

parmU macro n
?pp <n>,<word>, 2, 2
endm

;---------------------------------------------------------------------------;
;
; reminder macro
;
;---------------------------------------------------------------------------;

BUG macro stuff
IF1
%out ----&stuff&
ENDIF
endm
