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

;----------------------------------------------------------------------------
; CONTROL.ASM 
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
        .xlist
DOS5 = 1
	include cmacros.inc
	incControl = 1
	include gdidefs.inc
	include macros.inc
	.list

;----------------------------------------------------------------------------
; E X T E R N
;----------------------------------------------------------------------------
	externFP	DIB_Control
;
sBegin	Data
	externB 	MouseTrailsFlag 	;in CURSOR.ASM
sEnd	Data

;----------------------------------------------------------------------------
; C O D E  
;----------------------------------------------------------------------------
sBegin	Code
assumes cs,Code
	.386

cProc	Control,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmD	lpDevice
	parmW	function
	parmD	lpInData
	parmD	lpOutData
cBegin
;
;Upon Entry:
;	AX --> Data.
;
	assumes	ds,nothing
	assumes	es,nothing
	assumes	fs,nothing
	assumes gs,nothing
	lds	di,lpInData		;Get pointer to the data
	assumes ds,nothing
	mov	ax,_DATA
	mov	es,ax			;make ES --> Data
	assumes es,Data
	mov	bx,function

	cmp	bx,GETCOLORTABLE
	je	C_GetColorTable

	cmp	bx,MOUSETRAILS
	je	C_MouseTrails

        cmp     bx,QUERYESCSUPPORT
        jne     C_CallDibControl

        mov     ax,wptr [di]            ;Get queried escape function number
        cmp     ax,QUERYESCSUPPORT
        je      C_Exit
        cmp     ax,GETCOLORTABLE
	je	C_Exit
	cmp	ax,MOUSETRAILS
	je	C_Exit

PLABEL C_CallDibControl
	push	lpDevice
	push	function
	push	lpInData
	push	lpOutData
	call	DIB_Control		;Let the DIB Engine handle the rest.

PLABEL C_Exit
cEnd

PLABEL C_GetColorTable
        mov     al,byte ptr [di]	;color table index
	les	di,lpOutData
	cld
	cli				;DAC reads cannot be interrupted
        mov     dx,03C7H
	out	dx,al			;write out the index to the DAC
	mov	dx,03C9H
	in	al,dx			;read the red value
	shl	al,2
	stosb
	in	al,dx			;read the green value
	shl	al,2
        stosb
	in	al,dx			;read the blue value
	shl	al,2
        stosb
	sti				;reenable the interrupts
	xor	ax,ax
        stosb
	mov	ax,1
	jmp	C_Exit

PLABEL	C_MouseTrails
;
;We just set a flag here.  They'll get the actual mouse trails when the
;cursor changes (not immediately).
;
	mov	es:MouseTrailsFlag,0	;assume he wants mouse trails off
	mov	ax,1			;set return code GOOD
	mov	bx,[di] 		;get the requested action
	or	bx,bx			;does he want them off?
	jz	C_Exit			;yes, flag is set
	cmp	bx,-2			;does he want them off?
	je	C_Exit			;yes, flag is set
	mov	es:MouseTrailsFlag,0ffh ;he wants them on, set flag to ON
	jmp	C_Exit			;

sEnd
end
