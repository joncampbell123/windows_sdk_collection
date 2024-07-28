	page	,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	DSS.ASM "DrawSpecialString"
; This module contains the routines which special case the rendering
; of phase aligned fixed pitch fonts to the display.
;
; Copyright (c) 1991 Microsoft Corporation
;
; History:
;   28-October-1991  Ray Patrick [raypat]
;   wrote it.
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
ifdef _286
	externNP set_ega_opaque_mode_286
else
	externNP set_ega_opaque_mode_386
endif

;-----------------------------------------------------------------------;
; D A T A 
;-----------------------------------------------------------------------;
;-----------------------------------------------------------------------;
; C O D E
;-----------------------------------------------------------------------;
ifdef _286
createSeg _REAL,rCode,para,public,CODE
sBegin	rCode
	.286p
	assumes cs,rCode
else
createSeg _PROTECT,pCode,para,public,CODE
sBegin	pCode
	.386p
	assumes cs,pCode
endif
	assumes ds,nothing
	assumes es,nothing

ifdef _286
define_frame dss_strblt_dummy_286	;Define strblt's frame
cBegin	<nogen>
cEnd	<nogen>
else
define_frame dss_strblt_dummy_386	;Define strblt's frame
cBegin	<nogen>
cEnd	<nogen>
endif

;-----------------------------------------------------------------------;
; DRAWSPECIALSTRING
;-----------------------------------------------------------------------;
ifdef _286
public	DrawSpecialString_286
DrawSpecialString_286 proc near	
else
public	DrawSpecialString_386
DrawSpecialString_386 proc near	
endif

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

ifdef _286
ColumnsToDo	  equ   ss:[6]  ;16     ;Number of columns to word move.
endif

	les	di,lp_surface		;--> destination surface
	assumes es,nothing		;  (need EGAMem if this is the device)
	test	accel,IS_OPAQUE
	jz	short DSS_pp_ega_trans
ifdef _286
	call	set_ega_opaque_mode_286	;Set up reg. for opaque mode.
else
	call	set_ega_opaque_mode_386	;Set up reg. for opaque mode.
endif
	jmp	short DSS_pp_done

DSS_pp_ega_trans:
	mov	dx,EGA_BASE + GRAF_ADDR	;Select the Graphics Controller
if MASMFLAGS and VGA
	mov	ax,(M_AND_WRITE + M_DATA_READ) shl 8 + GRAF_MODE ;mode 
	out	dx,ax		
endif

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

	les	di,lp_surface		;es:di --> destination surface
	assumes es,nothing		;  (need EGAMem if this is the device)

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
	ret
ifdef _286
DrawSpecialString_286	endp
else
DrawSpecialString_386	endp
endif

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
ifdef _286
	mov	ColumnsToDo,cx		;cache columns to do in upper word of ecx.
else
	rol	ecx,16			;cache columns to do in upper word of ecx.
endif
DevOpSc_WordsLoop:
	push	di			;save ptr. destination screen word.
	mov	cx,Height		;Load glyph height.
ifdef _286
	test	cx,1
else
	test	cx,3
endif
	jnz	DevOpSc_AdjustHeight
DevOpSc_HeightAdjusted:
ifdef _286
	push	si
	mov	bx,ss:[si][-6]	        ;bx-->2nd column
	mov	si,ss:[si][-2]		;si-->1st column
	sub	bx,si
@@:	mov	dx,[bx][si]		;Get 2 words from 2nd column.
	lods	word ptr [si]		;Get 2 words from 1st column.
	xchg	ah,dl			;transpose (ax,dx ready to output).
	mov	es:[di],ax		;Write ax to the screen.
	add	di,bp			;point to next scan.
	mov	es:[di],dx		;write dx to the screen.
	add	di,bp			;point to next scan.
	sub	cx,2			;More to do?
	jg	@b			;yes.
	pop	si
else
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
endif

DevOpSc_InnerLoopBottom:
ifdef _286
	sub	si,8			;si-->next stack entry.
else
	sub	si,12			;si-->next stack entry.
endif
	pop	di			;get di back
	add	di,2			;point di to next screen word (along scan).
ifdef _286
	dec	word ptr ColumnsToDo	;All screen word columns processed?
else
	rol	ecx,16			;Get columns to do...
	dec	cx			;All screen word columns processed?
	rol	ecx,16			;put updated columns to do in upper word.
endif
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
ifdef _286
	push	si
	mov	si,ss:[si][-2]		;ds:si-->column
@@:	lods	byte ptr ds:[si]
	mov	es:[di],al
	add	di,bp
	dec	dx
	jnz	@b
	pop	si
	sub	si,4
else
	push	si
	mov	esi,ss:[si][-4]		;ds:esi-->column
@@:	lods	byte ptr ds:[esi]
	mov	es:[di],al
	add	di,bp
	dec	dx
	jnz	@b
	pop	si
	sub	si,6
endif
	mov	bp,SaveBp
	pop	di
	inc	di
	dec	cx
DevOpSc_Exit:
	ret				;We're done.

DevOpSc_AdjustHeight:
ifdef _286
	push	si
	mov	bx,ss:[si][-6]
	mov	si,ss:[si][-2]
@@:	mov	al,[si]
	mov	ah,[bx]
	mov	es:[di],ax
	add	di,bp
	inc	si
	inc	bx
	dec	cx
	mov	ax,si
	pop	si
	mov	ss:[si][-6],bx
	mov	ss:[si][-2],ax
else
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
endif
	jcxz	DevOpSc_InnerLoopBottom
	jmp	DevOpSc_HeightAdjusted

DevOp_SpecialCase	endp



;------------------------------
;Jump vector for 286, EGA code.
;------------------------------
ifdef _286
if MASMFLAGS and EGA
DevTrSc_AdjustHeight_10:
	jmp	DevTrSc_AdjustHeight
endif
endif

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
ifdef _286
	mov	ColumnsToDo,cx
else
	rol	ecx,16			;cache columns to do in upper word of ecx.
endif
DevTrSc_WordsLoop:
	push	di			;save ptr. destination screen word.
	mov	cx,Height		;Load glyph height.
ifdef _286
	test	cx,1
else
	test	cx,3
endif

ifdef _286
if MASMFLAGS and EGA
	jnz	DevTrSc_AdjustHeight_10	;I hate jump vectors.....
else
	jnz	DevTrSc_AdjustHeight
endif
else
	jnz	DevTrSc_AdjustHeight
endif

DevTrSc_HeightAdjusted:
	push	si
ifdef _286
	mov	bx,ss:[si][-6]		;bx-->2nd column
	mov	si,ss:[si][-2]		;si-->1st column
	sub	bx,si
DevTrSc_WordsInnerLoop:
	mov	dx,[bx][si]		;Get 2 words from 2nd column.
	lods	word ptr [si]		;Get 2 words from 1st column.
else
	mov	ebx,ss:[si][-10]	;ebx-->2nd column
	mov	esi,ss:[si][-4]		;esi-->1st column
	sub	ebx,esi
DevTrSc_WordsInnerLoop:
	mov	edx,[ebx][esi]		;Get 4 words from 2nd column.
	lods	dword ptr [esi]		;Get 4 words from 1st column.
endif
	xchg	ah,dl			;transpose (ax,dx ready to output).
	or	ax,ax			;If empty, don't write to the screen.
	jz	short @f
if MASMFLAGS and EGA
	push	dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	xchg	es:[di],al		;write ax to screen.
	mov	al,ah
	out	dx,al
	xchg	es:[di+1],ah		;write ax to screen.
	pop	dx
else
	xchg	es:[di],al		;write ax to screen.
	xchg	es:[di+1],ah		;write ax to screen.
endif
@@:	add	di,bp			;point to next scan.
	or	dx,dx			;If empty, don't write to the screen.
	jz	short @f
if MASMFLAGS and EGA
	mov	ax,dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	xchg	es:[di],al		;write ax to screen.
	mov	al,ah
	out	dx,al
	xchg	es:[di+1],ah		;write ax to screen.
else
	xchg	es:[di],dl		;write dx to the screen.
	xchg	es:[di+1],dh		;write dx to the screen.
endif

@@:	add	di,bp			;point to next scan.
ifndef _286
	rol	eax,16			;Prepare to transpose upper words.
	rol	edx,16			;Prepare to transpose upper words.
	xchg	ah,dl			;transpose (ax,dx ready to output).
	or	ax,ax			;If empty, don't write to the screen.
	jz	short @f
if MASMFLAGS and EGA
	push	dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	xchg	es:[di],al		;write ax to screen.
	mov	al,ah
	out	dx,al
	xchg	es:[di+1],ah		;write ax to screen.
	pop	dx
else
	xchg	es:[di],al		;write ax to screen.
	xchg	es:[di+1],ah		;write ax to screen.
endif
@@:	add	di,bp			;point to next scan.
	or	dx,dx			;If empty, don't write to the screen.
	jz	short @f
if MASMFLAGS and EGA
	mov	ax,dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	xchg	es:[di],al		;write ax to screen.
	mov	al,ah
	out	dx,al
	xchg	es:[di+1],ah		;write ax to screen.
else
	xchg	es:[di],dl		;write dx to screen.
	xchg	es:[di+1],dh		;write dx to screen.
endif
@@:	add	di,bp			;point to next scan.
endif
ifdef _286
	sub	cx,2			;More to do?
else
	sub	cx,4			;More to do?
endif
	jg	DevTrSc_WordsInnerLoop	;yes.
	pop	si

DevTrSc_InnerLoopBottom:
ifdef _286
	sub	si,8			;si-->next stack entry.
else
	sub	si,12			;si-->next stack entry.
endif
	pop	di			;get di back
	add	di,2			;point di to next screen word (along scan).
ifdef _286
	dec	word ptr ColumnsToDo	;All screen word columns processed?
else
	rol	ecx,16			;Get columns to do...
	dec	cx			;All screen word columns processed?
	rol	ecx,16			;put updated columns to do in upper word.
endif
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
ifdef _286
	mov	si,ss:[si][-2]		;ds:si-->column
DevTrSc_Byte1Loop:
	lods	byte ptr ds:[si]
else
	mov	esi,ss:[si][-4]		;ds:esi-->column
DevTrSc_Byte1Loop:
	lods	byte ptr ds:[esi]
endif
	or	al,al
	jz	short @f
if MASMFLAGS and EGA	
	push	dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	xchg	es:[di],al		;write ax to screen.
	pop	dx
else
	xchg	es:[di],al
endif
@@:	add	di,bp
	dec	dx
	jnz	DevTrSc_Byte1Loop
	pop	si
ifdef _286
	sub	si,4
else
	sub	si,6
endif
	mov	bp,SaveBp
	pop	di
	inc	di
	dec	cx
DevTrSc_Exit:
	ret				;We're done.

DevTrSc_AdjustHeight:
	push	si
ifdef _286
	mov	bx,ss:[si][-6]
	mov	si,ss:[si][-2]
DevTrSc_AdjustHeightLoop:
	mov	al,[si]
	mov	ah,[bx]
else
	mov	ebx,ss:[si][-10]
	mov	esi,ss:[si][-4]
DevTrSc_AdjustHeightLoop:
	mov	al,[esi]
	mov	ah,[ebx]
endif
	or	ax,ax
	jz	short @f
if MASMFLAGS and EGA	
	push	dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	xchg	es:[di],al
	mov	al,ah
	out	dx,al
	xchg	es:[di+1],ah
	pop	dx
else
	xchg	es:[di],al
	xchg	es:[di+1],ah
endif
@@:	add	di,bp
ifdef _286
	inc	si
	inc	bx
	dec	cx
	mov	ax,si
	pop	si
	mov	ss:[si][-6],bx
	mov	ss:[si][-2],ax
else
	inc	esi
	inc	ebx
	dec	cx
	test	cx,3
	jnz	DevTrSc_AdjustHeightLoop
	mov	eax,esi
	pop	si
	mov	ss:[si][-10],ebx
	mov	ss:[si][-4],eax
endif
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

ifdef _286
sEnd	rCode
else
sEnd	pCode
endif
	end

