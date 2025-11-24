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
	assumes	ds,nothing
	assumes	es,nothing
	assumes	fs,nothing
	assumes	gs,nothing
	lds	di,lpInData		;Get pointer to the data
        mov     bx,function

        cmp     bx,QUERYESCSUPPORT
        je      C_DoQueryEscSupport

	cmp	bx,GETCOLORTABLE
        je      C_GetColorTable

PLABEL C_CallDibControl
	push	lpDevice
	push	function
	push	lpInData
	push	lpOutData
	call	DIB_Control		;Let the DIB Engine handle the rest.

PLABEL C_Exit
cEnd

PLABEL C_DoQueryEscSupport
        mov     bx,wptr [di]            ;Get queried escape function number

	cmp	bx,QUERYESCSUPPORT
	je	C_Supported
        cmp     bx,GETCOLORTABLE
	je	C_Supported

        jmp     C_CallDibControl        ;We don't support it, see if DIBEngine

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

PLABEL C_Supported
	mov	ax,1
        jmp     C_Exit

sEnd
end
