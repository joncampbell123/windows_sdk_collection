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
; ET4000.ASM
;----------------------------------------------------------------------------
	.xlist
	include macros.inc
	DOS5=1
	include cmacros.inc
	include device.inc
	.list

;----------------------------------------------------------------------------
; M A C R O S
;----------------------------------------------------------------------------
PushReg	macro
	push	dx
	in	al,dx
	mov	bl,al	
	inc	dx
	in	al,dx
	dec	dx
	mov	bh,al
	push	bx
	inc	cx
	endm

PopReg	macro
	pop	bx
	pop	dx
	mov	al,bl
	out	dx,al
	inc	dx
	mov	al,bh
	out	dx,al
	dec	dx
	dec	cx
	endm

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
pMiscRead 	EQU	3CCh		; VGA read Misc. output
pMiscWrite	EQU	3C2h		; VGA write Misc. output
pStatColr	EQU	3DAh		; Status register
pStatMono	EQU	3BAh
pAttr		EQU	3C0h		; Attribute(palette) address/data

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _INIT,InitSeg,word,public,CODE
sBegin  InitSeg
assumes cs,InitSeg
	.386

;----------------------------------------------------------------------------
; Is_ET4000
; Attribute Controller Reg 16h is known to exist only on Tseng Labs VGA
; Exit:
;  ax:dx == ET4000 if detected.
;----------------------------------------------------------------------------
cProc	Is_ET4000,<NEAR,PUBLIC,PASCAL,NODATA>,<si>
	localW	ModeReg
	localW	StatReg
	localB	SaveModePort
cBegin
	xor	cx,cx
	xor	si,si
	cli
	mov	dx,pMiscRead
	in	al,dx
	test	al,1			;If lsb is set, then this is color.
	mov	dx,pStatColr
	jnz	short @f
	mov	dx,pStatMono
@@:	mov	StatReg,dx		;StatReg = 3BA or 3DA
	sub	dl, pStatColr-03D8h	; 
	mov	ModeReg,dx		;ModeReg = 3B8 or 3D8.
	in	al,dx
	mov	SaveModePort,al

	mov	dx,3cdh 	        ;Set to bank switch register	 				
	in	al,dx			;read it			 							
	push	ax			;save it			 							
	cmp	al,0FFh			;Does it look like it might exist?
	je	short @f		;no, or the bank is at 15. Skip unlock.

; write ET3000/4000 unlock "KEY"

	mov	dx,3BFh
	mov	al,3
	out	dx,al
	mov	dx,ModeReg
	mov	al,0A0h
	out	dx,al
	mov	dx,3cdh

@@:	mov	al,11h			;Test value			 							
	out	dx,al			;Write it			 							
	in	al,dx			;Read it			 							
	cmp	al,11h			;Did we get back what we wrote? 							
	jne	@f			;Nope, we're not an ET-4000	 							
	mov	al,22h			;Test value			 							
	out	dx,al			;Write it			 							
	in	al,dx			;Read it			 							
	cmp	al,22h			;Did we get back what we wrote? 							
	jne	@f		        ;Nope, we're not an ET-4000 	 						     
	pop	ax			;restore old value of 3cd   	 					     
	out	dx,al
	jmp	IET4_NextTest
@@:	pop	ax		        ;restore old value of 3cd 			  
	out	dx,al
	jmp	IET4_Not

PLABEL IET4_NextTest
	call	Check_Writing_ATC16	;sets carry if Tseng chip.
	jnc	short IET4_Not

; Check for ET3000 h/w

	mov	dx,3d4h
	in	al,dx
	mov	bl,al
	mov	al,33h
	out	dx,al
	inc	dx
	in	al,dx
	mov	bh,al		; save 3d4, 3d5 in bl,bh

	mov	al,05h		; high nibble always reads back as zero.
	out	dx,al
	dec	dx		; re-select the index register to avoid
	mov	al,33h		;  bus noise
	out	dx,al
	inc	dx
	in	al,dx		; get back value.
	mov	cl,al		; save for later comparison

	mov	al,0Ah
	out	dx,al
	dec	dx		; re-select the index register to avoid
	mov	al,33h		;  bus noise
	out	dx,al
	inc	dx
	in	al,dx		; get back value.
	mov	ch,al		; save for later comparison

	mov	al,bh
	out	dx,al		; restore original value of 3d5.
	dec	dx
	mov	al,bl
	out	dx,al		; restore original value of 3d4.

	cmp	cx,0A05h	; ET4000 if we can read back what we wrote.
	jne	IET4_Not


PLABEL IET4_Success
	mov	si,ET4000

PLABEL	IET4_Not
	mov	dx,ModeReg
	mov	al,SaveModePort
	out	dx,al

	mov	ax,si
	xor	dx,dx
	sti
cEnd


;----------------------------------------------------------------------------
;   Check_Writing_ATC16
;   exit: 'Carry Clear' if NOT TLVGA, else 'Carry Set' if TLVGA.
;----------------------------------------------------------------------------
PPROC Check_Writing_ATC16	near
	mov	dx,StatReg
	in	al,dx			;changes attr to index mode

	mov	dx,pAttr
	in	al,dx
	mov	bh,al			;Save current index of attribute controller.
	IO_Delay
	mov	al,16h+20h		;Select 16h (leave video on)
	out	dx,al			;changes attr to data mode
	IO_Delay
	inc	dx
	in	al,dx			;get data of attr16
	IO_Delay
	dec	dx
	mov	bl,al			;Save current reg 16h in BL
	xor	al,10h			;Complement bit 4
	out	dx,al			;Write it out (changes to index mode)
	IO_Delay

	mov	dx,ModeReg
	in	al,dx
	mov	dx,pAttr
	mov	al,16h+20h		;Select 16h (leave video on)
	out	dx,al
	IO_Delay
	inc	dx
	in	al,dx			;get data of attr16
	IO_Delay
	xor	ah,ah
	dec	dx
	xor	al,10h
	cmp	al,bl			;Q: Is value same as written?
	jnz	SHORT Restore16 	;   N: Is not TLVGA
	inc	ah                      ;   Y: Is TLVGA

Restore16:
	mov	al,bl
	out	dx,al
	IO_Delay
	mov	dx,ModeReg
	in	al,dx
	IO_Delay
	mov	dx,pAttr
	mov	al,bh
	out	dx,al
	xor	al,al
	sub	al,ah			;Set carry if ah = 1.
	ret
Check_Writing_ATC16	endp

	
sEnd	InitSeg
end
