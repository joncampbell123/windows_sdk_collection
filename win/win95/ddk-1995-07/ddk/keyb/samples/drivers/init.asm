;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

        page    ,132
;****** INIT.ASM ***********************************************************
;                                                                          
;   New API for Chicago Multilingual NewTableEx()
;
include keyboard.inc
include lcid.inc

if1
%out
%out INIT.ASM  Windows 3.00
    %out .      without Ericsson/NOKIA support
endif

        extrn   __ROMBIOS:abs
        extrn   GetModuleHandle:far

        ExternFP  <LocalInit>
        ExternFP  <LocalAlloc, LocalReAlloc, LocalSize, LocalFree>
        ExternFP  <FindResource, LoadResource, LockResource, FreeResource>
        ExternFP  <GlobalSize, GlobalFree, GlobalFix, GlobalWire>
	ExternFP  FixupKbdTable		

ROMBIOS SEGMENT AT 0F000h
        org  0FFFEh
PC_type label BYTE  ; contains computer identification
ROMBIOS ENDS


;***************************************************************************
; DATA segment -- data declarations and local data
;***************************************************************************

sBegin  DATA

assumes DS,DATA

; DATA segment variables accessed in KbInit() below

; Data to specify system type
; Some of these values are loaded with the table, others are
; computed in the INIT code in this module.

        extrn   PCType:byte             ; Copy of system type ID
        extrn   PCTypeHigh:byte         ;  from ROM address FFFF:000E
                                        ; For PCType values, see OLIKBD.INC

; Acknowledge byte for interrupt controller.  This varies depending on
; system type.

        public  AckByte
AckByte         db      eoi             ; eoi or eoiAT


        public  InitDataEnd

InitDataEnd label byte


GlobalW pLCIDList,0     	;Internal Pointer to LCID listing
GlobalW nKeyboards, 0           ;num of keyboards loaded
GlobalW sizeLCIDTab,0           ;size of Table
GlobalW pDataEntry,0            ;Point Data is been put into table

globalW NumLCIDBuffers,0        ;Number of LCID buffers available

globalW HlibKbd,0               ;Store kbd Module Handle here
sEnd


;***************************************************************************
; Initialization code -- called only once, when the driver is loaded.
;***************************************************************************

createSeg _INIT, INIT, BYTE, PUBLIC, CODE
sBegin INIT
assumes CS,INIT
assumes DS,DATA
.386

; Begin with date and copyright strings.

extrn GlobalSmartPageLock:FAR

; ****** KbInit ******* Initialization code *******************
;
; This code is called ONLY when the keyboard driver is loaded.
; It identifies the system type and the keyboard type, and
; performs modifications of the keyboard translation tables
; and set flag bytes specific to the type of keyboard attached.
;
; *************************************************************
cProc KbInit,<PUBLIC,FAR,LOADDS>
cBegin KbInit

;
; Determine PC type and save in DS:PCType and PCTypeHigh
;
;*;
;*; we now need to initialise the heap first, otherwise all the LocalAlloc's
;*; are going to fail.
;*;
        jcxz    error                   ; Jump if no heap specified: this will
                                        ; fail us badly!

        ;***
        ;*** Call LocalInit() to set up the heap
        ;***
        xor     ax, ax
        cCall   LOCALINIT <ds, ax, cx>
        or      ax, ax                  ; Did it do it ok ?
        jnz     InitOK
error:
        xor     ax,     ax
        jmp     KbInitDone

InitOK:
        mov     hlibKbd, di

        push    ds
        mov     ax,__ROMBIOS
        mov     ds,ax
        assumes DS,ROMBIOS

        mov     ax,word ptr [PC_type]

        pop     ds
        assumes DS,DATA
        mov     word ptr PCType,ax      ; save both bytes
        cmp     al, M28ID               ; is an M28 (AT)-like system?
        jne     UnlikeAT
        mov     AckByte, eoiAT          ; set EOI byte for interrupt ack.
UnlikeAT:

;-------------- alloc the buffer area for the first time ---------------------

        Arg     <LMEM_MOVEABLE, sizeofLCIDIB>   ; size of 1 slot ...
        cCall   LocalAlloc
	or	ax, ax                          ; any problems?
	jz	KbInitDone

        mov     NumLCIDBuffers, 1      		;number of slots in table
        mov     pLCIDList, ax			; Offset to LCID list

;---------------------- lock the data and code pages -------------------------

	cCall	GlobalFix, <ds>
	cCall	GlobalWire, <ds>

	cCall	GlobalSmartPageLock, <_TEXT>
        cCall   GlobalSmartPageLock, <ds>

;------------------------------- success/exit --------------------------------
        xor     ax,ax
        not     ax              ; return success
KbInitDone:
cEnd KbInit

.286p

sEnd INIT       ; end of disposable initialization code.

if2
%out end INIT.ASM
%out
endif

END KbInit
