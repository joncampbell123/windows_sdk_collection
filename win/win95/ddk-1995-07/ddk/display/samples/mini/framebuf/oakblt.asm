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
; OAKBLT.ASM
;----------------------------------------------------------------------------
	.xlist
DOS5 = 1
	include cmacros.inc
	include	macros.inc
	include	dibeng.inc
	.list

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
        externFP        DIB_BitBlt
        externFP        DIB_EndAccess
	externNP	PrepareForBlt
	externNP	SetSPointer
	externNP	SetDPointer
	externNP	SetSPointerDec
	externNP	SetDPointerDec
	externNP	ScreenToScreen1

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
ROP_S           equ     11001100b	;SRCCOPY

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin	Data
sEnd	Data

;----------------------------------------------------------------------------
; C O D E  
;----------------------------------------------------------------------------
sBegin	Code
assumes cs,Code
	.386

BS_LetDIBEngineDoIt:
	assumes	es,Data
	pop	edi
	pop	esi
	lea	sp,[bp-2]
	pop	ds
	pop	bp
	jmp     DIB_BitBlt	

;----------------------------------------------------------------------------
; BltSpecial_Oak
;----------------------------------------------------------------------------
cProc	BltSpecial_Oak,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	include	special.inc		;include parms and locals
cBegin
	assumes	ds,nothing
	assumes	es,Data
	assumes	fs,nothing
	assumes	gs,nothing
        lfs     bx,lpDestDev            ;fs:bx-->dest pdevice
        test    fs:[bx].deFlags,VRAM    ;is destination the screen?
        jz      BS_LetDIBEngineDoIt     ;no.
	test	fs:[bx].deFlags,BUSY	;Is the screen busy?
        jnz     BS_LetDIBEngineDoIt     ;yes.
	xor	ax,ax
	mov	SrcFlags,ax		;initialize Src pdevice Flags.

        mov     al,bptr Rop+2	        ;Get the raster op code.
	cmp	al,ROP_S		;Yes. Check for special rops.
	jnz	BS_LetDIBEngineDoIt	;DIB Engine can do this blt.

BS_S:
	lfs	bx,lpSrcDev
	mov	ax,fs:[bx].deFlags	   ;ax = src pdevice flags.
	mov	SrcFlags,ax		   ;save it.
	test	ax,VRAM			   ;is this my PDevice?
	jz	BS_LetDIBEngineDoIt	   ;No.
	lea	ax,SetOakSBank
	mov	pSetSBank,ax
	lea	ax,SetOakDBank
	mov	pSetDBank,ax
	lea	ax,SaveOakBank
	mov	pSaveBank,ax
	lea	ax,RestoreOakBank
	mov	pRestoreBank,ax
	mov	ax,BUSY
	xchg	fs:[bx].deFlags,ax
	mov	DeviceFlags,ax
	call	ScreenToScreen1		   ;Call screen to screen blt.

BS_Exit:
	lfs	bx,lpDestDev
	mov	ax,DeviceFlags
	xchg	fs:[bx].deFlags,ax
	push	lpDestDev
	push	CURSOREXCLUDE
	call	DIB_EndAccess
	cld
cEnd

;----------------------------------------------------------------------------
; SaveOakBank
;   Saves the current bank setting 
;----------------------------------------------------------------------------
PPROC SaveOakBank	near
	push	dx
	push	ax
	mov	dx,3deh
	mov	al,11h
	out	dx,al
	inc	dx
	in	al,dx
	mov	bCurrentBankSetting,al
	pop	ax
	pop	dx
	ret
SaveOakBank	endp

;----------------------------------------------------------------------------
; RestoreBank
;   Restores the current bank setting 
;----------------------------------------------------------------------------
RestoreOakBank	proc	near
	push	dx
	push	ax
	mov	dx,3deh
	mov	al,11h
	mov	ah,bCurrentBankSetting
	out	dx,ax
	pop	ax
	pop	dx
	ret
RestoreOakBank	endp

;----------------------------------------------------------------------------
; SetOakSBank
;----------------------------------------------------------------------------
PPROC	SetOakSBank	near
	push	ax
	push	dx
	mov	ah,dl			;Save read bank #
	mov	dx,3deh			
	mov	al,11h
	out	dx,al
	inc	dx
	in	al,dx			;get bank states
	and	al,0F0h			;mask off read bank bits
	or	al,ah			;merge with exiting bank state.
	out	dx,al			;send it to the hardware
	pop	dx
	pop	ax
	ret
SetOakSBank	endp

;----------------------------------------------------------------------------
; SetOakDBank
;----------------------------------------------------------------------------
PPROC	SetOakDBank	near
	push	ax
	push	dx
	mov	ah,dl			;Save read bank #
	mov	dx,3deh			
	mov	al,11h
	out	dx,al
	inc	dx
	in	al,dx			;get bank states
	and	al,00Fh			;mask off read bank bits
	shl	ah,4			
	or	al,ah			;merge with exiting bank state.
	out	dx,al			;send it to the hardware
	pop	dx
	pop	ax
	ret
SetOakDBank	endp

sEnd	OakSeg
end
