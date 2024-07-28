        TITLE ISR.ASM
        page 60,132

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; ISR.ASM - Interrupt service routine entry point
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

?PLM=1	    ; PASCAL Calling convention is DEFAULT
?WIN=0      ; Windows calling convention

	.286
        .xlist
        include cmacros.inc
        include vcap.inc
        .list

        externNP        InStreamISR                             ; cap.c
	externFP 	CT_IRQClear

; -------------------------------------------------------
;               DATA SEGMENT DECLARATIONS
; -------------------------------------------------------

ifndef SEGNAME
        SEGNAME equ <_TEXT>
endif

createSeg %SEGNAME, CodeSeg, word, public, CODE

        externD         _dwVideoClock
        externB         gbInt,             -1
        externW         gwBaseReg,         -1

sBegin Data
	wIntCount	dw	0000h
sEnd Data

sBegin CodeSeg
        assumes cs,CodeSeg
        assumes ds,Data
        assumes es,nothing


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; CT_ISR	 Interrupt handler entry point
;
; Ideas from the SndBlst driver
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	assumes ds,nothing
	assumes es,nothing

	public CT_ISR
.386
CT_ISR proc far

	cld			; never assume this at interrupt time!

	.386
	pushad
	push	ds
	push	es
	push	fs
;	push	gs

        ; set up local DS

        mov     ax, DataBASE
        mov     ds, ax
	assumes ds,Data

	add	_dwVideoClock.lo,1
	adc	_dwVideoClock.hi,0

	; Set it up so other interrupts can come through.
        ; send EOI to PIC
        mov     al, 20H                ; non-specific EOI

        cmp     [gbInt], 8
        jb      @f
        out     0A0h, al               ; slave EOI
@@:     out     020h, al               ; master EOI

        cCall   CT_IRQClear            ; re-enable device interrupts
        
	cmp	[wIntCount],0
	jnz	isrExit

	; Check the timing and perform the frame capture if needed
	inc	[wIntCount]
	sti
	cCall	InStreamISR
	dec	[wIntCount]

isrExit:
;	pop	gs
	pop	fs
	pop	es
	pop	ds
	assumes ds,nothing
	popad
	.286
        iret

CT_ISR endp

sEnd
end
