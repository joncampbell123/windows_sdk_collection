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

	page	,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	DSS.ASM "DrawSpecialString"
; This module contains the routines which special case the rendering
; of phase aligned fixed pitch fonts to the display.
;
;-----------------------------------------------------------------------;
	.xlist
	include cmacros.inc
	include gdidefs.inc
	include display.inc
	include mflags.inc
	include ega.inc
	include egamem.inc
	include macros.inc
	include strblt.inc
	include fontseg.inc
	.list
	externNP set_ega_opaque_mode_386

if MASMFLAGS and EGA
	externA	ScreenSelector
endif

;-----------------------------------------------------------------------;
; D A T A 
;-----------------------------------------------------------------------;
;-----------------------------------------------------------------------;
; C O D E
;-----------------------------------------------------------------------;
createSeg _PROTECT,pCode,para,public,CODE
sBegin	pCode
	.386p
	assumes cs,pCode
	assumes ds,nothing
	assumes es,nothing

define_frame dss_strblt_dummy_386	;Define strblt's frame
cBegin	<nogen>
cEnd	<nogen>

;-----------------------------------------------------------------------;
; DRAWSPECIALSTRING
;-----------------------------------------------------------------------;
public	DrawSpecialString_386
DrawSpecialString_386 proc near	

;-----------------------------------------------------------------------;
;Equates		                 Used By
;-----------------------------------------------------------------------;
BitModifier  	  equ	ss:[0]	;32	;Color Bitmap output routines
Mask1		  equ	ss:[0]	;32	;BuildSuperGlyph
Mask2		  equ	ss:[4]	;32	;BuildSuperGlyph
BitInverter	  equ	ss:[4]	;32	;Color Bitmap output routines
NotFg		  equ	ss:[8]	;8	;Color Bitmap output routines
BMask1		  equ	ss:[8]	;32	;BuildSuperGlyph
ClipMask	  equ	ss:[9]	;8	;Color Bitmap output routines
nPlanes		  equ	ss:[10] ;8      ;Color Bitmap output routines
Height		  equ 	ss:[12]	;16	;Output routines, BuildSuperGlyph
SaveBp		  equ 	ss:[14]	;16	;

	les	di,lp_surface		;--> destination surface
	assumes es,EGAMem		;
	test	accel,IS_OPAQUE
	jz	short DSS_pp_ega_trans
	call	set_ega_opaque_mode_386	;Set up reg. for opaque mode.
	jmp	short DSS_pp_done

DSS_pp_ega_trans:
	mov	dx,EGA_BASE + GRAF_ADDR	;Select the Graphics Controller

if MASMFLAGS and VGA
	mov	ax,(M_AND_WRITE + M_COLOR_READ) shl 8 + GRAF_MODE ;write mode 3, read mode 1
else
	mov	ax,(M_PROC_WRITE + M_COLOR_READ) shl 8 + GRAF_MODE ;write mode 0, read mode 1
	mov	shadowed_mode,ah		
endif
	out	dx,ax			
	mov	ax,GRAF_CDC
	out	dx,ax			;set Color Don't Care to zero.  

	mov	ah,bptr colors[FOREGROUND]
	mov	al,GRAF_SET_RESET
	out	dx,ax
	mov	ax,MM_ALL shl 8 + GRAF_ENAB_SR
	out	dx,ax
	mov	ax,0FF00h+GRAF_BIT_MASK	;Transparent mode expects GRAF_ADDR
	out	dx,ax			;  to be set to the bitmask register

DSS_pp_done:	
	xor	bx,bx
	mov	al,accel
	shr	al,1
	rcl	bl,2			;bl = 0000 00x0
	.errnz	IS_OPAQUE-1

;-----------------------------------------------------------------------;
; index = opaque/trans
;-----------------------------------------------------------------------;
	lea	si,ScNonClippedOutput
	mov	ax,cs:[si][bx]
	mov	NonClippedRoutine,ax

	mov	ax,text_bbox.top	;Compute Y component of the string
	mul	next_scan
	add	di,ax
	add	di,scan_start		;Add delta into scan

	mov	ds,wptr lfd.lp_font_bits[2] ;ds:(e)si --> font
	assumes	ds,FontSeg

;-----------------------------------------------------------------------;
; Output the string to the display.
;-----------------------------------------------------------------------;
DSS_OutputString:
	mov	dx,clipped_font_height
	mov	cx,inner_byte_count
	call	word ptr NonClippedRoutine

	mov	dx,EGA_BASE + GRAF_ADDR	;Select the Graphics Controller
	mov	ax,(M_PROC_WRITE + M_DATA_READ) shl 8 + GRAF_MODE ;mode 0
if MASMFLAGS and EGA
	mov	shadowed_mode,ah		
endif
	out	dx,ax

	ret
DrawSpecialString_386	endp

;----------------------------------------------------------------------------;
;>>>>>>>>>>>>>>>>>>>  O U T P U T    R O U T I N E S  <<<<<<<<<<<<<<<<<<<<<<<;
;----------------------------------------------------------------------------;
;----------------------------------------------------------------------------;
; Entry:
;       cx     scan length in bytes (number of columns)
;       dx     clipped height font in scans.
;   ds:(e)si-->Font bits.
;    es:di --> Destination (screen memory).
;----------------------------------------------------------------------------;

ScNonClippedOutput	label	word
	dw	offset DevTr_SpecialCase
	dw	offset DevOp_SpecialCase

;----------------------------------------------------------------------------;
; D E V I C E
; OPAQUE: Special Cased (FIXED PITCH, PHASE 0).
;----------------------------------------------------------------------------;
DevOp_SpecialCase	proc	near
	mov	Height,dx		;save off glyph height.
	mov	si,buffer		;ss:si-->stack entries.
	test	di,1			;Even destination boundary?
	jz	short DevOpSc_DestEven	;Yes.

DevOpSc_DestOdd:
	call	DevOpSc_Byte1		;Do 1 byte column....

DevOpSc_DestEven:
DevOpSc_Words:
	push	cx
	shr	cx,1			;If cx is 1 or 0, then..
	jz	short DevOpSc_WordsExit ; nothing to do.
	mov	bp,next_scan		;Cache next_scan in bp for faster access.
	rol	ecx,16			;cache columns to do in upper word of ecx.
DevOpSc_WordsLoop:
	push	di			;save ptr. destination screen word.
	mov	cx,Height		;Load glyph height.
	test	cx,3
	jnz	DevOpSc_AdjustHeight
DevOpSc_HeightAdjusted:
	push	si
	mov	ebx,ss:[si][-10]	;ebx-->2nd column
	mov	esi,ss:[si][-4]		;esi-->1st column
	sub	ebx,esi
@@:	mov	edx,[ebx][esi]		;Get 4 words from 2nd column.
	lods	dword ptr [esi]		;Get 4 words from 1st column.
	xchg	ah,dl			;transpose (ax,dx ready to output).
	mov	es:[di],ax		;Write ax to the screen.
	rol	eax,16			;Prepare to transpose upper words.
	add	di,bp			;point to next scan.
	mov	es:[di],dx		;write dx to the screen.
	rol	edx,16			;Prepare to transpose upper words.
	xchg	ah,dl			;transpose (ax,dx ready to output).
	add	di,bp			;point to next scan.
	mov	es:[di],ax		;write ax to screen.
	add	di,bp			;point to next scan.
	mov	es:[di],dx		;write dx to screen.
	add	di,bp			;point to next scan.
	sub	cx,4			;More to do?
	jg	@b			;yes.
	pop	si

DevOpSc_InnerLoopBottom:
	sub	si,12			;si-->next stack entry.
	pop	di			;get di back
	add	di,2			;point di to next screen word (along scan).
	rol	ecx,16			;Get columns to do...
	dec	cx			;All screen word columns processed?
	rol	ecx,16			;put updated columns to do in upper word.
	jnz	DevOpSc_WordsLoop	;no.
	mov	bp,SaveBp		;yes. Restore bp.
DevOpSc_WordsExit:
	pop	cx
	and	cx,1			;If odd then do a final byte wide column.
	jz	short DevOpSc_Exit
	mov	dx,Height		;restore dx with height.

DevOpSc_Byte1:		
	push	di		
	mov	bp,next_scan
	push	si
	mov	esi,ss:[si][-4]		;ds:esi-->column
@@:	lods	byte ptr ds:[esi]
	mov	es:[di],al
	add	di,bp
	dec	dx
	jnz	@b
	pop	si
	sub	si,6
	mov	bp,SaveBp
	pop	di
	inc	di
	dec	cx
DevOpSc_Exit:
	ret				;We're done.

DevOpSc_AdjustHeight:
	push	si
	mov	ebx,ss:[si][-10]
	mov	esi,ss:[si][-4]
@@:	mov	al,[esi]
	mov	ah,[ebx]
	mov	es:[di],ax
	add	di,bp
	inc	esi
	inc	ebx
	dec	cx
	test	cx,3
	jnz	@b
	mov	eax,esi
	pop	si
	mov	ss:[si][-10],ebx
	mov	ss:[si][-4],eax
	jcxz	DevOpSc_InnerLoopBottom
	jmp	DevOpSc_HeightAdjusted

DevOp_SpecialCase	endp



;----------------------------------------------------------------------------;
; D E V I C E
; TRANSPARENT: Special Cased (FIXED PITCH, PHASE 0).
;----------------------------------------------------------------------------;
DevTr_SpecialCase	proc	near
	mov	Height,dx		;save off glyph height.
	mov	si,buffer		;ss:si-->stack entries.
	test	di,1			;Even destination boundary?
	jz	short DevTrSc_DestEven	;Yes.

DevTrSc_DestOdd:
	call	DevTrSc_Byte1		;Do 1 byte column....

DevTrSc_DestEven:
DevTrSc_Words:
	push	cx
	shr	cx,1			;If cx is 1 or 0, then..
	jz	DevTrSc_WordsExit 	; nothing to do.
	mov	bp,next_scan		;Cache next_scan in bp for faster access.
	rol	ecx,16			;cache columns to do in upper word of ecx.
DevTrSc_WordsLoop:
	push	di			;save ptr. destination screen word.
	mov	cx,Height		;Load glyph height.
	test	cx,3
	jnz	DevTrSc_AdjustHeight

DevTrSc_HeightAdjusted:
	push	si
	mov	ebx,ss:[si][-10]	;ebx-->2nd column
	mov	esi,ss:[si][-4]		;esi-->1st column
	sub	ebx,esi
DevTrSc_WordsInnerLoop:
	mov	edx,[ebx][esi]		;Get 4 words from 2nd column.
	lods	dword ptr [esi]		;Get 4 words from 1st column.
	xchg	ah,dl			;transpose (ax,dx ready to output).
	or	ax,ax			;If empty, don't write to the screen.
	jz	short @f
if MASMFLAGS and EGA
	push	dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	and	es:[di],al		;write ax to screen.
	mov	al,ah
	out	dx,al
	and	es:[di+1],ah		;write ax to screen.
	pop	dx
else
	and	es:[di],al		;write ax to screen.
	and	es:[di+1],ah		;write ax to screen.
endif
@@:	add	di,bp			;point to next scan.
	or	dx,dx			;If empty, don't write to the screen.
	jz	short @f
if MASMFLAGS and EGA
	mov	ax,dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	and	es:[di],al		;write ax to screen.
	mov	al,ah
	out	dx,al
	and	es:[di+1],ah		;write ax to screen.
else
	and	es:[di],dl		;write dx to the screen.
	and	es:[di+1],dh		;write dx to the screen.
endif

@@:	add	di,bp			;point to next scan.
	rol	eax,16			;Prepare to transpose upper words.
	rol	edx,16			;Prepare to transpose upper words.
	xchg	ah,dl			;transpose (ax,dx ready to output).
	or	ax,ax			;If empty, don't write to the screen.
	jz	short @f
if MASMFLAGS and EGA
	push	dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	and	es:[di],al		;write ax to screen.
	mov	al,ah
	out	dx,al
	and	es:[di+1],ah		;write ax to screen.
	pop	dx
else
	and	es:[di],al		;write ax to screen.
	and	es:[di+1],ah		;write ax to screen.
endif
@@:	add	di,bp			;point to next scan.
	or	dx,dx			;If empty, don't write to the screen.
	jz	short @f
if MASMFLAGS and EGA
	mov	ax,dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	and	es:[di],al		;write ax to screen.
	mov	al,ah
	out	dx,al
	and	es:[di+1],ah		;write ax to screen.
else
	and	es:[di],dl		;write dx to screen.
	and	es:[di+1],dh		;write dx to screen.
endif
@@:	add	di,bp			;point to next scan.
	sub	cx,4			;More to do?
	jg	DevTrSc_WordsInnerLoop	;yes.
	pop	si

DevTrSc_InnerLoopBottom:
	sub	si,12			;si-->next stack entry.
	pop	di			;get di back
	add	di,2			;point di to next screen word (along scan).
	rol	ecx,16			;Get columns to do...
	dec	cx			;All screen word columns processed?
	rol	ecx,16			;put updated columns to do in upper word.
	jnz	DevTrSc_WordsLoop	;no.
	mov	bp,SaveBp		;yes. Restore bp.
DevTrSc_WordsExit:
	pop	cx
	and	cx,1			;If odd then do a final byte wide column.
	jz	short DevTrSc_Exit
	mov	dx,Height		;restore dx with height.

DevTrSc_Byte1:		
	push	di		
	mov	bp,next_scan
	push	si
	mov	esi,ss:[si][-4]		;ds:esi-->column
DevTrSc_Byte1Loop:
	lods	byte ptr ds:[esi]
	or	al,al
	jz	short @f
if MASMFLAGS and EGA	
	push	dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	and	es:[di],al		;write ax to screen.
	pop	dx
else
	and	es:[di],al
endif
@@:	add	di,bp
	dec	dx
	jnz	DevTrSc_Byte1Loop
	pop	si
	sub	si,6
	mov	bp,SaveBp
	pop	di
	inc	di
	dec	cx
DevTrSc_Exit:
	ret				;We're done.

DevTrSc_AdjustHeight:
	push	si
	mov	ebx,ss:[si][-10]
	mov	esi,ss:[si][-4]
DevTrSc_AdjustHeightLoop:
	mov	al,[esi]
	mov	ah,[ebx]
	or	ax,ax
	jz	short @f
if MASMFLAGS and EGA	
	push	dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	and	es:[di],al
	mov	al,ah
	out	dx,al
	and	es:[di+1],ah
	pop	dx
else
	and	es:[di],al
	and	es:[di+1],ah
endif
@@:	add	di,bp
	inc	esi
	inc	ebx
	dec	cx
	test	cx,3
	jnz	DevTrSc_AdjustHeightLoop
	mov	eax,esi
	pop	si
	mov	ss:[si][-10],ebx
	mov	ss:[si][-4],eax
if MASMFLAGS and VGA
	jcxz	DevTrSc_InnerLoopBottom
else
	jcxz	short @f
endif
	jmp	DevTrSc_HeightAdjusted
if MASMFLAGS and EGA
@@:	jmp	DevTrSc_InnerLoopBottom
endif
DevTr_SpecialCase	endp

sEnd	pCode
	end

