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

;----------------------------------------------------------------------------;
; MODE.ASM
; Copyright (c) 1993 Microsoft Corporation
;----------------------------------------------------------------------------;
        .xlist
        include cmacros.inc
        .list

createSeg _MODE,ModeSeg,word,public,CODE
sBegin  ModeSeg
assumes cs,ModeSeg
        .386


;----------------------------------------------------------------------------;
; SetMode
; Entry:
;   ax = Desired mode number.
; Exit:
;   al = resulting mode number
;   ah = requested mode number
; preserves bx,cx
;----------------------------------------------------------------------------;
public SetMode
SetMode	proc	far
	push	bx
	push	cx
;----------------------------------------------------------------------------;
;Check if we are in the right mode.
;----------------------------------------------------------------------------;
	push	ax
	or	ah,ah			;Is this a V7 mode?
	jnz	short SM_V7set		;yes.  Jmp to the V7 handling code.
	cmp	ax,0070h		;Are we trying an Everex board?
	jne	short @f		;If so, then load bl with a 2
	mov	bl,2			; (required for 800x600 mode)
@@:	int	10h			;Set the mode.
	mov	ax,0F00h		;Call BIOS to get mode back.
	jmp	short SM_CheckMode
SM_V7set:
	mov	bl,al			;Get mode number.
	mov	al,5			;Set the mode.
	int	10h			
	mov	ax,6f04h		;Call BIOS to get mode back.
SM_CheckMode:
	int	10h	
	pop	bx			;Get desired mode back.
	mov	ah,bl
	pop	cx			;Get desired height back.		
	pop	bx			;Get desired width back.
	ret
SetMode	endp


;----------------------------------------------------------------------------;
; SetColors
;   Initializes the color palette with the system colors.  
;   Called at physical enable time and at screen switch time (if we are 
;   running with a foreign vdd).
;----------------------------------------------------------------------------;
public SetColors
SetColors	proc	far
;
; Program the DAC, so that index 7 (palette value 07) has (19,1a,1b) &
; index 8 (palette value 0fh) has (28,29,2a)			          
;
        mov     cx,2223h
        mov     dx,2100h
	mov	bx,7
	mov	ax,1010h
        int     10h                     ;index 7 = RGB(21,22,23)

        mov     cx,3132h
        mov     dx,3000h
	mov	bx,0fh
	mov	ax,1010h
        int     10h                     ;index 8 = RGB(30,31,32)
;
; Now load the attribute controller with our palette.  instead of calling
; int 10h to do this, we program the h/w directly because some vdd's may
; ignore the bios int 10h call to set up the palette.
;
	mov	dx,3cch
	in	al,dx
	test	al,1
	mov	dx,3bah
	jz	short @f
	add	dl,20h
@@:	in	al,dx			;changes attr to index mode.	

	mov	dl,0c0h			;dx = (3c0h) attribute controller
	mov	bx,16
	mov	al,11h			;set overscan register
	out	dx,al
	jmp	$+2			;Delay
	mov	al,ModePalette[bx]	;
	out	dx,al			;set overscan color
	jmp	$+2			;Delay
	dec	bx
@@:	mov	al,bl
	out	dx,al			;store attr index
	jmp	$+2			;Delay
	mov	al,ModePalette[bx]
	out	dx,al			;store attr data
	dec	bx
	jns	@b
	mov	al,20h			;turn on the logical palette.
	out	dx,al		
	ret
SetColors	endp

;----------------------------------------------------------------------------;
; Initial values for the attribute controller.
;----------------------------------------------------------------------------;
ModePalette label	byte
	db	00h			;Black
	db	0ch			;dark Red
	db	0ah			;dark Green
	db	0eh			;mustard
	db	01h			;dark Blue
	db	15h			;purple 
	db	23h			;turquoise
	db	07h			;gray
	db	0fh			;blend of blue
	db	24h			;Red
	db	12h			;Green
	db	36h			;Yellow
	db	09h			;Blue
	db	2Dh			;Magenta
	db	1Bh			;Cyan
	db	3Fh			;White
	db	0			;Overscan will be black

sEnd    ModeSeg
end
