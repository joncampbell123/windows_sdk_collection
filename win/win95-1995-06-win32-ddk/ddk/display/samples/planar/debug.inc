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

if MASMFLAGS and DEBUG

; [Windows] DebugOptions field values

; flags values

DBF_TRACE           equ 00000h
DBF_WARNING         equ 04000h
DBF_ERROR           equ 08000h
DBF_FATAL           equ 0c000h

; [Windows] DebugFilter and flags values

DBF_INTERNAL        equ 00000h  ; Obsolete: was 02000h
DBF_KERNEL          equ 01000h
DBF_USER            equ 00800h
DBF_GDI             equ 00400h
DBF_COMPAT          equ 00000h  ; Obsolete: was 00200h
DBF_LOGERROR        equ 00000h  ; Obsolete: was 00100h
DBF_PARAMERROR      equ 00000h  ; Obsolete: was 00080h
DBF_MMSYSTEM        equ 00040h
DBF_PENWIN          equ 00020h
DBF_KRN_MEMMAN      equ 00001h
DBF_KRN_LOADMODULE  equ 00002h
DBF_KRN_SEGMENTLOAD equ 00004h

;
; DebugErr macro - used for generating debugging messages
;
; Preserves all registers
;

externFP    DebugOutput,<C>

DebugErr    macro   flags,msg
        local   a,b
	push	es
	pusha
        push    cs
        push    offset a
        push    flags or DBF_GDI
        cCall   DebugOutput
        add     sp,2+4      ; DebugOutput is cdecl!
	popa
	pop	es
        jmp     short b
a:
        db      "DISPLAY: "
        db      msg
        db      0
b:
endm

else;   DEBUG

DebugErr    macro   flags,msg
endm
endif


