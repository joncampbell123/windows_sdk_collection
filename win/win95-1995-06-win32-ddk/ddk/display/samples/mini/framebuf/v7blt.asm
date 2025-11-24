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
; V7BLT.ASM
;----------------------------------------------------------------------------
	.xlist
DOS5 = 1
	include cmacros.inc
	incLogical = 1
	include	gdidefs.inc
	include	macros.inc
	include	dibeng.inc
	.list
;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
ROP_P           equ     11110000b
ROP_Pn          equ     00001111b	
ROP_S           equ     11001100b	;SRCCOPY
ROP_Sn          equ     00110011b
ROP_DDx         equ     00000000b	;BLACKNESS
ROP_DDxn        equ     11111111b	;WHITENESS
ROP_Dn          equ     01010101b	;DSTINVERT
ROP_DPx         equ     01011010b	;PATINVERT

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
	externNP	PrepareForBlt
	externNP	SetSPointer
	externNP	SetDPointer
	externNP	SetSPointerDec
	externNP	SetDPointerDec
        externFP	DIB_BitBlt
        externFP	DIB_EndAccess
	externA		KernelsScreenSel	;equates to a000:0000
	externNP	ScreenToScreen1L
	externNP	MemoryToScreen1L
	externNP	MemoryToScreen3L
	externNP	PatCopyL
	externNP	PatCopySL
	externNP	DPx_SL
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
	jmp	DIB_BitBlt	

;----------------------------------------------------------------------------
; BltSpecial_V7
;----------------------------------------------------------------------------
cProc	BltSpecial_V7,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	include	special.inc		;include parms and locals

; extra for v7

	localB  b3C2
	localB  bF9
	localB  bF6
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

	lea	ax,SetV7Bank
	mov	pSetBank,ax
	lea	ax,SetV7SBank
	mov	pSetSBank,ax
	lea	ax,SetV7DBank
	mov	pSetDBank,ax
	lea	ax,SaveV7Bank
	mov	pSaveBank,ax
	lea	ax,RestoreV7Bank
	mov	pRestoreBank,ax
	mov	bNextBank,4
	mov	bBankShiftCount,2

        mov     al,bptr Rop+2	        ;Get the raster op code.
	cmp	al,ROP_S		;Yes. Check for special rops.
        jz      BS_S

        test    fs:[bx].deFlags,PALETTE_XLAT    ;palette xlate active?
        jnz     BS_LetDIBEngineDoIt             ;yes.

        cmp     al,ROP_P		
        jz      short BS_P
        cmp     al,ROP_DDx
        jz      short BS_0
        cmp     al,ROP_DDxn
        jz      short BS_1
        cmp     al,ROP_DPx
        jz      BS_DPx
        cmp     al,ROP_Dn
        jz      short BS_Dn
        cmp     al,ROP_Pn
        jz      BS_Pn
	jmp	BS_LetDIBEngineDoIt	;DIB Engine can do this blt.

BS_1:
	push	BS_Exit
        mov     eax,-1			;Blt solid white.
	mov	bx,offset PatCopySL
    	jmp     MemoryToScreen1L

BS_0:
	push	BS_Exit
	xor	eax,eax			;Blt solid black.
        mov	bx,offset PatCopySL
   	jmp     MemoryToScreen1L

BS_P:
	lds	si,lpPBrush
	cmp	[si].dp8BrushStyle,BS_HOLLOW
        je      BS_LetDIBEngineDoIt		;Let the DIB Engine handle it.
	test	[si].dp8BrushFlags,COLORSOLID
	jz	short BS_P_NotSolid		; get brush color and jmp to
	push	BS_Exit
	mov	eax,dword ptr [si].dp8BrushBits	; the SolidColor code.
        mov	bx,offset PatCopySL
   	jmp     MemoryToScreen1L

BS_P_NotSolid:		
	test	[si].dp8BrushFlags,PATTERNMONO
	jnz	BS_LetDIBEngineDoIt		;DIB Engine can do this blt.
	push	BS_Exit
        mov	bx,offset PatCopyL
   	jmp     MemoryToScreen3L

BS_Dn:
	push	BS_Exit
	mov	eax,-1			;we'll xor with FF to invert the bckgnd.
	mov	bx,offset DPx_SL
    	jmp     MemoryToScreen1L

BS_DPx:
	lds	si,lpPBrush
	cmp	[si].dp8BrushStyle,BS_HOLLOW
        je      BS_LetDIBEngineDoIt		;Let the DIB Engine handle it.
	test	[si].dp8BrushFlags,COLORSOLID
	jz	short BS_DPx_NotSolid	        ; get brush color and jmp to the 
	push	BS_Exit
	mov	eax,dword ptr [si].dp8BrushBits	; SolidColor code.
        mov	bx,offset DPx_SL
   	jmp     MemoryToScreen1L

BS_DPx_NotSolid:
	jmp	BS_LetDIBEngineDoIt	        ;DIB Engine can do this blt.

BS_Pn:
	lds	si,lpPBrush
	cmp	[si].dp8BrushStyle,BS_HOLLOW
        je      BS_LetDIBEngineDoIt		;Let the DIB Engine handle it.
	test	[si].dp8BrushFlags,COLORSOLID
	jz	BS_LetDIBEngineDoIt	        ;DIB Engine can do this blt.
	push	BS_Exit
	mov	eax,dword ptr [si].dp8BrushBits	; SolidColor code.
	not	eax
        mov	bx,offset PatCopySL
   	jmp     MemoryToScreen1L

BS_S:
	lfs	bx,lpSrcDev
	mov	ax,fs:[bx].deFlags	   ;ax = src pdevice flags.
	mov	SrcFlags,ax		   ;save it.
	test	ax,VRAM			   ;is this my PDevice?
        jz      BS_LetDIBEngineDoIt        ;No.
	mov	al,byte ptr SrcxOrg
	mov	ah,byte ptr DestxOrg
	and	ax,0303H
	cmp	al,ah
	jne	BS_LetDIBEngineDoIt
	push	BS_Exit			   ;will return to here.
	mov	ax,BUSY
	xchg	fs:[bx].deFlags,ax
	mov	DeviceFlags,ax
	jmp	ScreenToScreen1L

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
; SaveV7Bank
;   Saves the current bank setting 
;----------------------------------------------------------------------------
SaveV7Bank	proc	near
	push	dx
	push	ax
	mov	dx,3cch
	in	al,dx
	mov	b3C2,al
	mov	dx,3C4h
	mov	al,0f9h
	out	dx,al
	inc	dx
	in	al,dx
	mov	bF9,al
	dec	dx
	mov	al,0F6h
	out	dx,al
	inc	dx
	in	al,dx
	mov	bF6,al
	pop	ax
	pop	dx
	ret
SaveV7Bank	endp

;----------------------------------------------------------------------------
; RestoreV7Bank
;   Restores the current bank setting 
;----------------------------------------------------------------------------
RestoreV7Bank	proc	near
	push	dx
	push	ax
	mov	dx,3c2h
	mov	al,b3C2
	out	dx,al
	mov	dx,3C4h
	mov	al,0F9h
	mov	ah,bF9
	out	dx,ax
	mov	al,0F6h
	mov	ah,bF6
	out	dx,ax
	pop	ax
	pop	dx
	ret
RestoreV7Bank	endp


;----------------------------------------------------------------------------
; SetV7Bank
; Entry:
;  dl = bank number (0-15)
;----------------------------------------------------------------------------
PPROC	SetV7Bank	near
	push	ax
	push	dx
	push	bx
        mov     bl,dl
        and     bl,1                    ; BL = extended page select
        mov     ah,dl
        and     ah,2
        shl     ah,4                    ; AH = page select bit
        and     dl,00ch
        mov     bh,dl
        shr     dl,2
        or      bh,dl                   ; BH = 256K bank select
        mov     dx,03cch
        in      al,dx                   ; Get Miscellaneous Output Register
        and     al,not 20h              ; Clear page select bit
        or      al,ah                   ; Set page select bit (maybe)
        mov     dl,0c2h                 ; Write Miscellaneous Output Register
        out     dx,al
        mov     dl,0c4h                 ; Sequencer
        mov     al,0f9h                 ; Extended page select register
        mov     ah,bl                   ; Extended page select value
        out     dx,ax
        mov     al,0f6h                 ; 256K bank select
        out     dx,al
        inc     dx                      ; Point to data
        in      al,dx
        and     al,0f0h                 ; Clear out bank select banks
        or      al,bh                   ; Set bank select banks (maybe)
        out     dx,al
	pop	bx
        pop     dx
        pop     ax
	ret
SetV7Bank	endp

;----------------------------------------------------------------------------
; SetV7SBank
;----------------------------------------------------------------------------
PPROC	SetV7SBank	near
	push	ax
	push	dx
	mov	ah,dl			;save off bank number.
	mov	dx,3c4h	
	mov	al,0f6h
	out	dx,al			;select RAMBANK register
	inc	dx
	in	al,dx			;get bank states
	and	al,0F3h			;mask off read bank bits
	shl	ah,2			;adjust desired bank value
	or	al,ah			;merge with exiting bank state.
	out	dx,al			;send it to the hardware
	mov	dx,3c4h			;restore 3c4 to 2 (plane mask)
	mov	al,2
	out	dx,al
	pop	dx
	pop	ax
	ret
SetV7SBank	endp

;----------------------------------------------------------------------------
; SetV7DBank
;----------------------------------------------------------------------------
PPROC	SetV7DBank	near
	push	ax
	push	dx
	mov	ah,dl			;Save desired write bank
	mov	dx,3c4h
	mov	al,0F6h
	out	dx,al			;select RAMBANK register
	inc	dx
	in	al,dx			;get bank states
	and	al,0FCh			;mask off write bank bits
	or	al,ah			;merge with exiting bank state.
	out	dx,al			;send it to the hardware
	mov	dx,3c4h			;restore 3c4 to 2 (plane mask)
	mov	al,2
	out	dx,al
	pop	dx
	pop	ax
	ret
SetV7DBank	endp	


sEnd	Code
end
