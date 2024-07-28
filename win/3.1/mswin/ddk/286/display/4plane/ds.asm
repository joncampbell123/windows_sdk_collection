	page	,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	DS.ASM "DrawString"
; This module contains the routines which are required to render text
; onto the screen and bitmaps (both color and monochrome).
;
; Copyright (c) 1991 Microsoft Corporation
;
; History:
;   30-September-1991  [raypat]
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
	externNP DrawSpecialString_286
else
	externNP DrawSpecialString_386
endif


NLABEL	macro n
ifdef _286
public n&_286
n&_286:
else
public n&_386
n&_386:
endif
endm

PLABEL	macro n
n:
ifdef _286
public n&_286
n&_286:
else
public n&_386
n&_386:
endif
endm

if MASMFLAGS and EGA
SLOW_EGA = 0  	     ;Change this to a 1 for byte moves to video memory.
else
SLOW_EGA = 0				
endif

DEFAULT_CHAR_OPTIMIZATION = 1

;-----------------------------------------------------------------------;
; D A T A 
;-----------------------------------------------------------------------;
;-----------------------------------------------------------------------;
;The compose buffer is structured as follows (using as an example a 
;glyph height of 3):
; +-------+-------+-------+
; |   1   |   2   |   3   | The first glyph column.
; +-------+-------+-------+
; +-------+-------+-------+
; |   4   |   5   |   6   | The second glyph column.
; +-------+-------+-------+
;    ...     ...     ... etc.
;
;-----------------------------------------------------------------------;
createSeg _COMPOSE_AREA,Compose,word,public,DATA
sBegin	Compose
ifdef _286
public	ComposeArea
public	LastByteTouched
public	EndOfComposeArea
align	4
ComposeArea	 db	4096 dup(0)
EndOfComposeArea equ	$
LastByteTouched  dw	ComposeArea

else

externB	ComposeArea
externW LastByteTouched
externA EndOfComposeArea

endif
sEnd	Compose

;-----------------------------------------------------------------------;
; C O D E
;-----------------------------------------------------------------------;
ifdef _286
createSeg _REAL,rCode,para,public,CODE
sBegin	rCode
	.286p
MaskTable	label	word
	dw	 1111111111111111b	;phase 0
	dw	 0111111101111111b	;phase 1
	dw	 0011111100111111b	;phase 2
	dw	 0001111100011111b	;phase 3
	dw	 0000111100001111b	;phase 4
	dw	 0000011100000111b	;phase 5
	dw	 0000001100000011b	;phase 6
	dw	 0000000100000001b	;phase 7
	dw	 0000000000000000b	;phase 8 (for bold)
	assumes cs,rCode
else
createSeg _PROTECT,pCode,para,public,CODE
sBegin	pCode
	.386p
MaskTable	label	dword
	dd	 11111111111111111111111111111111b	;phase 0
	dd	 01111111011111110111111101111111b	;phase 1
	dd	 00111111001111110011111100111111b	;phase 2
	dd	 00011111000111110001111100011111b	;phase 3
	dd	 00001111000011110000111100001111b	;phase 4
	dd	 00000111000001110000011100000111b	;phase 5
	dd	 00000011000000110000001100000011b	;phase 6
	dd	 00000001000000010000000100000001b	;phase 7
	dd	 00000000000000000000000000000000b	;phase 8 (for bold)
	assumes cs,pCode
endif
	assumes ds,nothing
	assumes es,nothing

ifdef _286
define_frame strblt_dummy2_286		;Define strblt's frame
else
define_frame strblt_dummy2_386		;Define strblt's frame
endif

cBegin	<nogen>
cEnd	<nogen>

;-----------------------------------------------------------------------;
; DRAWSTRING
;-----------------------------------------------------------------------;
ifdef _286
public	DrawString_286
DrawString_286 proc near			
else
public	DrawString_386
DrawString_386 proc near			
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

ifdef _286
	push	word ptr ss:[0]
	push	word ptr ss:[2]
	push	word ptr ss:[4]
	push	word ptr ss:[6]
	push	word ptr ss:[8]
	push	word ptr ss:[0ah]
	push	word ptr ss:[0ch]
	push	word ptr ss:[0eh]
else
	push	dword ptr ss:[0]
	push	dword ptr ss:[4]
	push	dword ptr ss:[8]
	push	dword ptr ss:[0ch]
endif

	mov	SaveBp,bp
	mov	SpaceVerified,0

;-----------------------------------------------------------------------;
;If the font is fixed pitch, width=8, not clipped, phase 0, and 
;dest=device, then we will special case the string. This helps make 
;DOS boxes faster.
;-----------------------------------------------------------------------;
	mov	ah,accel		
	mov	al,excel
	and	ax,WIDTH_IS_8 shl 8 + IS_DEVICE
	xor	ax,WIDTH_IS_8 shl 8 + IS_DEVICE
	jz	short DS_MaybeSpecialCaseFont 

PLABEL DS_CannotSpecialCase
ifdef _286
	call	BuildSuperGlyph_286
else
	call	BuildSuperGlyph_386
endif
	sbb	al,al			;Will be 0ffffh if restart is necessary.
	mov	RestartFlag,al		;Zero otherwise.

	les	di,lp_surface		;--> destination surface
	assumes es,nothing		;  (need EGAMem if this is the device)

	test	excel,IS_DEVICE		;Only preprocess if this is
ifdef _286
	jz	short @f
else
	jz	DS_pp_done		;  the device
endif
	test	accel,IS_OPAQUE
	jz	short DS_pp_ega_trans
ifdef _286
	call	set_ega_opaque_mode_286	;Set up reg. for opaque mode.
else
	call	set_ega_opaque_mode_386	;Set up reg. for opaque mode.
endif
@@:
if WM3_DETECT
	mov	cx,1
endif
	jmp	DS_pp_done

PLABEL DS_MaybeSpecialCaseFont
	cmp	nBackups,0		;If the string backs up on itself we
	jne	DS_CannotSpecialCase	; cannot handle it with special case code.
	mov	ax,xOrigin		
	and	ax,7
	jnz	DS_CannotSpecialCase
	mov	ah,left_clip_mask
	mov	al,right_clip_mask
	or	al,ah
	jnz	DS_CannotSpecialCase	
if WM3_DETECT
	test	accel,IS_OPAQUE
	jnz	short @f
	mov	cx,CanDoWm3
	dec	cx
	jcxz	DS_CannotSpecialCase
@@:
endif
ifdef _286
	call	DrawSpecialString_286
else
	call	DrawSpecialString_386
endif
	jmp	DS_Exit

if WM3_DETECT
PLABEL	DS_MaybePatchUpRoutines
	lea	si,pNonClippedOutput
	lea	di,pClippedOutput
	mov	ax,cs:[si][bx]
	cmp	bx,4
	je	short DS_GetNonClippedWm3Routine
	cmp	bx,12
	je	short DS_GetNonClippedWm3Routine

PLABEL DS_SetNonClippedRoutine
	mov	NonClippedRoutine,ax
	mov	ax,cs:[di][bx]
	cmp	bx,4
	je	short DS_GetClippedWm3Routine
	cmp	bx,12
	je	short DS_GetClippedWm3Routine

PLABEL DS_SetClippedRoutine
	mov	ClippedRoutine,ax
	jmp	DS_RoutinesDefined

PLABEL DS_GetNonClippedWm3Routine
	lea	ax,DevTrNo3_NotClipped
	jmp	DS_SetNonClippedRoutine

PLABEL DS_GetClippedWm3Routine
	lea	ax,DevTrNo3_Clipped
	jmp	DS_SetClippedRoutine
endif


PLABEL DS_pp_ega_trans
	mov	dx,EGA_BASE + GRAF_ADDR	;Select the Graphics Controller

if WM3_DETECT
PLABEL DS_CheckDetectionStatus
	mov	cx,CanDoWm3
	dec	cx
	jcxz	short DS_Wm3_NotSupported
endif
if MASMFLAGS and VGA
	mov	ax,(M_AND_WRITE + M_DATA_READ) shl 8 + GRAF_MODE ;write mode 3
	out	dx,ax		

PLABEL DS_Wm3_NotSupported
endif
	mov	ah,bptr colors[FOREGROUND]
	mov	al,GRAF_SET_RESET
	out	dx,ax
	mov	ax,MM_ALL shl 8 + GRAF_ENAB_SR
	out	dx,ax
	mov	ax,0FF00h+GRAF_BIT_MASK	;Transparent mode expects GRAF_ADDR
	out	dx,ax			;  to be set to the bitmask register

PLABEL DS_pp_done
	xor	bx,bx
	mov	al,num_planes
	shr	al,1
	rcl	bl,1			;bl = 0000 000x

	mov	al,excel
	.errnz	IS_DEVICE-000001000b
	shr	al,4			
	rcl	bl,1			;bl = 0000 00xx

	mov	al,accel
	shr	al,1
	rcl	bl,2			;bl = 0000 xxx0
	.errnz	IS_OPAQUE-1

;-----------------------------------------------------------------------;
; index = 0000 0 mono/color dev/bmp opaque/trans
;-----------------------------------------------------------------------;
	mov	ax,seg ComposeArea
	mov	ds,ax
	assumes	ds,Compose

if WM3_DETECT
	jcxz	DS_MaybePatchUpRoutines
endif
	lea	si,pNonClippedOutput
	lea	di,pClippedOutput
	mov	ax,cs:[si][bx]
	mov	NonClippedRoutine,ax
	mov	ax,cs:[di][bx]
	mov	ClippedRoutine,ax

PLABEL DS_RoutinesDefined
	lea	si,ComposeArea		;ds:si --> ComposeArea

	les	di,lp_surface		;es:di --> destination surface
	assumes es,nothing		;  (need EGAMem if this is the device)

	mov	ax,text_bbox.top	;Compute Y component of the string
	mul	next_scan
	add	di,ax
	add	di,scan_start		;Add delta into scan

	mov	bx,clipped_font_height
	test	excel,FIRST_IN_PREV
	jz	short @f
	dec	nColumns
	add	si,bx
@@:
	cmp	RestartFlag,0
	jnz	short DS_OutputStringWithRestart

;-----------------------------------------------------------------------;
; Output the left, middle, and right part of the string.
;-----------------------------------------------------------------------;
PLABEL DS_OutputString
	mov	ah,left_clip_mask
	or	ah,ah
	jz	short @f
	mov	dx,bx
	call	word ptr ClippedRoutine
	xor	ah,ah
@@:	
	mov	cx,inner_byte_count
	or	cx,cx
	jz	short @f
	mov	dx,bx
	call	word ptr NonClippedRoutine
@@:
	mov	ah,right_clip_mask
	or	ah,ah
	jz	short @f
	mov	dx,bx
	call	word ptr ClippedRoutine
@@:

PLABEL DS_Exit
ifdef _286
	pop	word ptr ss:[0eh]
	pop	word ptr ss:[0ch]
	pop	word ptr ss:[0ah]
	pop	word ptr ss:[8]
	pop	word ptr ss:[6]
	pop	word ptr ss:[4]
	pop	word ptr ss:[2]
	pop	word ptr ss:[0]
else
	pop	dword ptr ss:[0ch]
	pop	dword ptr ss:[8]
	pop	dword ptr ss:[4]
	pop	dword ptr ss:[0]
endif

	test	excel,IS_DEVICE
	jz	short @f

	mov	dx,EGA_BASE + GRAF_ADDR	;Select the Graphics Controller
	mov	ax,(M_PROC_WRITE + M_DATA_READ) shl 8 + GRAF_MODE ;mode 0
	out	dx,ax
@@:	ret

;-----------------------------------------------------------------------;
;                   -- RESTART LOGIC--
; Output the left, middle, and right part of the string 
;-----------------------------------------------------------------------;
PLABEL DS_OutputStringWithRestart
	mov	ah,left_clip_mask
	or	ah,ah
	jz	short @f
	mov	dx,bx
	call	word ptr ClippedRoutine
	mov	left_clip_mask,0
	dec	nColumns
@@:	
PLABEL DS_RestartLoop
	mov	cx,inner_byte_count
	mov	ax,nColumns
	min_ax	cx
	sub	inner_byte_count,ax
	or	ax,ax
	jz	short @f
	mov	cx,ax
	mov	dx,bx
	call	word ptr NonClippedRoutine
@@:
	push	es
	push	di

	mov	ax,seg ComposeArea
	mov	es,ax
	assumes	es,Compose
	lea	di,ComposeArea			;es:di --> ComposeArea
	mov	cx,bx
rep	movsb
	mov	cx,si
	sub	cx,di
	inc	cx
ifdef _286
	shr	cx,1
	inc	cx
	xor	ax,ax
rep	stosw
else
	shr	cx,2
	inc	cx
	xor	eax,eax
rep	stosd
endif
	lea	di,ComposeArea			;es:di --> ComposeArea
	mov	ds,wptr lfd.lp_font_bits[2]	;ds:esi --> font
	assumes	ds,FontSeg
	call	BuildSuperGlyph_10

	pop	di
	pop	es
	mov	ax,seg ComposeArea
	mov	ds,ax
	mov	bx,clipped_font_height
	assumes	ds,Compose
	lea	si,ComposeArea		;ds:si --> ComposeArea
	jc	DS_RestartLoop
	jmp	DS_OutputString

ifdef _286
DrawString_286	endp
else
DrawString_386	endp
endif



;-----------------------------------------------------------------------;
; BUILDSUPERGLYPH
;-----------------------------------------------------------------------;
ifdef _286
public	BuildSuperGlyph_286
BuildSuperGlyph_286	proc	near
else
public	BuildSuperGlyph_386
BuildSuperGlyph_386	proc	near
endif
	mov	ds,wptr lfd.lp_font_bits[2]	;ds:esi --> font
	assumes	ds,FontSeg
	mov	ax,seg ComposeArea
	mov	es,ax
	assumes	es,Compose
	lea	di,ComposeArea			;es:di --> ComposeArea

if DEFAULT_CHAR_OPTIMIZATION
;-----------------------------------------------------------------------;
;Get the pointer to the break character.  We can skip over stack
;entires that point to this character since it is blank. However, since
;there is a possibility that this character could be non-blank, we 
;verify that the first occurance of the character is blank.  If it is NOT
;blank, we set the value of SpaceCharOffset to zero which will cause us
;to not skip subsequent blank characters. 
;We use a stack frame variable (SpaceVerified) to indicate that we have 
;verified the blank character. This flag has the following meaning:
; 0: Blank character has not been verified.
; 1: Blank character is blank.
; 2: Blank character is non-blank.
;-----------------------------------------------------------------------;
	xor 	ax,ax
	mov	al,lfd.break_char
ifdef _286
	shl	ax,2
	mov	bx,ax
	mov	bx,word ptr fsCharOffset[bx][PROP_OFFSET] ;ptr to space char.
	mov	SpaceCharOffset,bx
else
	shl	ax,1
	mov	bx,ax
	shl	ax,1
	add	bx,ax
	mov	ebx,dword ptr fsCharOffset[bx][PROP_OFFSET] ;ptr to space char.
	mov	SpaceCharOffset,ebx
endif
endif
;-----------------------------------------------------------------------;
;Clear out old string data from compose buffer.
;-----------------------------------------------------------------------;
ifdef _286
	xor	ax,ax
	push	di
	mov	cx,LastByteTouched
	sub	cx,di
	shr	cx,1
	inc	cx
rep	stosw
	pop	di
else
	xor	eax,eax
	push	di
	mov	cx,LastByteTouched
	sub	cx,di
	shr	cx,2
	inc	cx
rep	stosd
	pop	di
endif

PLABEL BuildSuperGlyph_10
	mov	LastByteTouched,ax

	mov	bx,clipped_font_height
	mov	ax,bx
	shl	ax,1
	add	ax,bx
	neg	ax
	add	ax,offset EndOfComposeArea
	mov	HighWater,ax

ifndef _286
	test	fontweight,1
	jnz	BBSG_GetStackEntry
endif

;-----------------------------------------------------------------------;
;Process stack entry loop -- N O R M A L --
;-----------------------------------------------------------------------;
PLABEL BSG_GetStackEntry
	mov	dx,bx
	mov	si,buffer		;--> next character's data
	mov	cx,wptr ss:[si].fd_width
	errnz	fd_phase-fd_width-1

	test	cx,8000h
ifdef _286
	jnz	BSG_StopOrBackup_10
	cmp	di,HighWater		;Are we near the end of ComposeArea?
	ja	BSG_MustRestart_10	;Yes, exit, output what we have
					;so far, and start over.
	xchg 	cl,ch			;Put phase in cl. Width in ch.
	mov	si,ss:[si][-2]		;Get ptr to glyph.
else
	jnz	BSG_StopOrBackup
	cmp	di,HighWater		;Are we near the end of ComposeArea?
	ja	BSG_MustRestart		;Yes, exit, output what we have
					;so far, and start over.
	xchg 	cl,ch			;Put phase in cl. Width in ch.
	mov	esi,ss:[si][-4]		;Get ptr to glyph.
endif

if DEFAULT_CHAR_OPTIMIZATION
;-----------------------------------------------------------------------;
;Skip over any space character entries.
;-----------------------------------------------------------------------;
ifdef _286
	cmp	si,SpaceCharOffset
	je	BSG_SkipNullChar_10
else
	cmp	esi,SpaceCharOffset
	je	BSG_SkipNullChar
endif
endif

;-----------------------------------------------------------------------;
;And now the hard part....
;Currently:
;  dx = clipped height
;  bx = clipped height (constant)
;  cx = width | phase
;  es:di --> Compose buffer
;  ds:(e)si --> font data for this stack entry.
;-----------------------------------------------------------------------;
PLABEL BSG_CantSkipBlank
	test	cx,00ffh
ifdef _286
	jz	BSG_PhaseIsZero_10
else
	jz	BSG_PhaseIsZero
endif

;-----------------------------------------------------------------------;
;PHASE NOT 0 CODE  -- N O R M A L --
;-----------------------------------------------------------------------;
PLABEL BSG_PhaseIsNonZero
	mov	bp,cx
	and	bp,00ffh
ifdef _286
	shl	bp,1
	mov	bp,cs:[bp].MaskTable
	mov	Mask1,bp
	not	bp
	test	dx,1
	jnz	BSG_AlignHeight_10
@@:
else
	shl	bp,2
	mov	ebp,cs:[bp].MaskTable
	mov	Mask1,ebp
	not	ebp
	test	dx,3
	jnz	BSG_AlignHeight
endif

PLABEL BSG_HeightIsAligned
	mov	Height,dx
ifdef _286
	and	dx,0eh
else
	and	edx,0ch
endif
	jz	short BSG_Compose16Bytes
	neg	dx
	add	dx,16
ifdef _286
	sub	si,dx
	sub	di,dx
	mov	ax,bx			;save bx.
	mov	bx,dx			;
	mov	dx,cs:[bx].BSG_ComposeTable
	mov	bx,ax
	jmp	dx
else
	sub	esi,edx
	sub	di,dx
	jmp	cs:[edx].BSG_ComposeTable
endif
ifdef _286
;-----------------------------------
; Some jump vectors for 286 version.
;-----------------------------------
BSG_PhaseIsZero_10:
	jmp	BSG_PhaseIsZero
BSG_MustRestart_10:
	jmp	BSG_MustRestart
BSG_StopOrBackup_10:
	jmp	BSG_StopOrBackup
BSG_AlignHeight_10:
	jmp	BSG_AlignHeight

if DEFAULT_CHAR_OPTIMIZATION
BSG_SkipNullChar_10:
	jmp	BSG_SkipNullChar
endif

endif


PLABEL BSG_ShouldNeverGetHere
	int	3			;Should never get here!
	jmp	BSG_LoopBottom

ifdef _286
BSG_ComposeTable	label	word
	dw	BSG_ShouldNeverGetHere
	dw	BSG_Compose14Bytes
	dw	BSG_Compose12Bytes
	dw	BSG_Compose10Bytes
	dw	BSG_Compose8Bytes
	dw	BSG_Compose6Bytes
	dw	BSG_Compose4Bytes
	dw	BSG_Compose2Bytes
else
BSG_ComposeTable	label	word
	dw	BSG_ShouldNeverGetHere
	dw	BSG_ShouldNeverGetHere
	dw	BSG_Compose12Bytes
	dw	BSG_ShouldNeverGetHere
	dw	BSG_Compose8Bytes
	dw	BSG_ShouldNeverGetHere
	dw	BSG_Compose4Bytes
	dw	BSG_ShouldNeverGetHere
endif

;-----------------------------------------------------------------------;
;Compose the rest of the column.
;-----------------------------------------------------------------------;
PLABEL BSG_Compose16Bytes
ifdef _286
	mov	ax,ds:[si]
	ror	ax,cl
	mov	dx,ax
	and	ax,Mask1
	rol	dx,8
	or	es:[di],ax
	and	dx,bp
	or	es:[di][bx],dx

PLABEL BSG_Compose14Bytes
	mov	ax,ds:[si+2]
	ror	ax,cl
	mov	dx,ax
	and	ax,Mask1
	rol	dx,8
	or	es:[di+2],ax
	and	dx,bp
	or	es:[di+2][bx],dx
else
	mov	eax,ds:[esi]
	ror	eax,cl
	mov	edx,eax
	and	eax,Mask1
	rol	edx,8
	or	es:[di],eax
	and	edx,ebp
	or	es:[di][bx],edx
endif


PLABEL BSG_Compose12Bytes
ifdef _286
	mov	ax,ds:[si+4]
	ror	ax,cl
	mov	dx,ax
	and	ax,Mask1
	rol	dx,8
	or	es:[di+4],ax
	and	dx,bp
	or	es:[di+4][bx],dx

PLABEL BSG_Compose10Bytes
	mov	ax,ds:[si+6]
	ror	ax,cl
	mov	dx,ax
	and	ax,Mask1
	rol	dx,8
	or	es:[di+6],ax
	and	dx,bp
	or	es:[di+6][bx],dx
else
	mov	eax,ds:[esi+4]
	ror	eax,cl
	mov	edx,eax
	and	eax,Mask1
	rol	edx,8
	or	es:[di+4],eax
	and	edx,ebp
	or	es:[di+4][bx],edx
endif

PLABEL BSG_Compose8Bytes
ifdef _286
	mov	ax,ds:[si+8]
	ror	ax,cl
	mov	dx,ax
	and	ax,Mask1
	rol	dx,8
	or	es:[di+8],ax
	and	dx,bp
	or	es:[di+8][bx],dx

PLABEL BSG_Compose6Bytes
	mov	ax,ds:[si+10]
	ror	ax,cl
	mov	dx,ax
	and	ax,Mask1
	rol	dx,8
	or	es:[di+10],ax
	and	dx,bp
	or	es:[di+10][bx],dx
else
	mov	eax,ds:[esi+8]
	ror	eax,cl
	mov	edx,eax
	and	eax,Mask1
	rol	edx,8
	or	es:[di+8],eax
	and	edx,ebp
	or	es:[di+8][bx],edx
endif

PLABEL BSG_Compose4Bytes
ifdef _286
	mov	ax,ds:[si+12]
	ror	ax,cl
	mov	dx,ax
	and	ax,Mask1
	rol	dx,8
	or	es:[di+12],ax
	and	dx,bp
	or	es:[di+12][bx],dx

PLABEL BSG_Compose2Bytes
	mov	ax,ds:[si+14]
	ror	ax,cl
	mov	dx,ax
	and	ax,Mask1
	rol	dx,8
	or	es:[di+14],ax
	and	dx,bp
	or	es:[di+14][bx],dx
else
	mov	eax,ds:[esi+12]
	ror	eax,cl
	mov	edx,eax
	and	eax,Mask1
	rol	edx,8
	or	es:[di+12],eax
	and	edx,ebp
	or	es:[di+12][bx],edx
endif
	
PLABEL BSG_LoopBottom
ifdef _286
 	add	si,16
	add	di,16
	sub	word ptr Height,16
	jg	BSG_Compose16Bytes_10
@@:
else
 	add	esi,16
	add	di,16
	sub	word ptr Height,16
	jg	BSG_Compose16Bytes
endif

PLABEL BSG_EntryComplete
	mov	bp,SaveBp

PLABEL BSG_MaybeAdjustColumn
	mov	si,di
	add	si,bx
	mov	ax,LastByteTouched
	max_ax	si
	mov	LastByteTouched,ax
  	add	cl,ch
	cmp	cl,8
	jl	BSG_AdjustColumn
	sub	buffer,(size frame_data)
	jmp	BSG_GetStackEntry
PLABEL BSG_Done
	clc	
	ret

ifdef _286
;-----------------------------------
; Some jump vectors for 286 version.
;-----------------------------------
PLABEL BSG_Compose16Bytes_10
	jmp	BSG_Compose16Bytes
endif

PLABEL BSG_AlignHeight
ifndef _286
	mov	ax,dx
	and	ax,3
	cmp	ax,3
	jz	short BSG_Compose3Bytes
	cmp	ax,2
	jz	short BSG_Compose2Bytes
endif

PLABEL BSG_Compose1Byte
ifdef _286
	lods	byte ptr ds:[si]
	and	ax,00ffh
else
	lods	byte ptr ds:[esi]	;Must follow this instr. with 386 instr!
	and	eax,000000ffh		;ah must be clear so we'll do it this way.
endif
	ror	ax,cl
	or	es:[di],al
	or	es:[di][bx],ah
	inc	di
	dec	dx
	jz	BSG_EntryComplete
	jmp	BSG_HeightIsAligned

ifndef _286
PLABEL BSG_Compose2Bytes
	push	dx
	lods	word ptr ds:[esi]
	and	eax,0000ffffh
	ror	eax,cl
	mov	edx,eax
	and	eax,Mask1
	rol	edx,8
	or	es:[di],eax
	and	edx,ebp
	or	es:[di][bx],edx
	add	di,2
	pop	dx
	sub	dx,2
	jz	BSG_EntryComplete
	jmp	BSG_HeightIsAligned

PLABEL BSG_Compose3Bytes
	mov	eax,ds:[esi]
	and	eax,00ffffffh
	jnz	short @f
	add	esi,3
	add	di,3
	sub	dx,3
	jz	BSG_EntryComplete
	jmp	BSG_HeightIsAligned
@@:	push	dx
	ror	eax,cl
	mov	edx,eax
	and	eax,Mask1
	rol	edx,8
	or	es:[di],eax
	and	edx,ebp
	or	es:[di][bx],edx
	add	esi,3
	add	di,3
	pop	dx
	sub	dx,3
	jz	BSG_EntryComplete
	jmp	BSG_HeightIsAligned
endif

PLABEL BSG_AdjustColumn
	sub	di,bx
	sub	buffer,(size frame_data)
	jmp	BSG_GetStackEntry


PLABEL BSG_StopOrBackup
	cmp	cx,0ffffh		;stop sentinel.
	je	BSG_Done		

PLABEL BSG_Backup			;Could be 80xx (backup) else Nop
	shl	ch,1			;Shift off msb.
	jnz	short BSG_IsNop		;if still non zero, it is a Nop entry.
	xor	ch,ch
	mov	ax,ss:[si][-2]		;Get num. of pixels to backup.
	add	cx,ax			;Add in phase.
	jns	short BSG_SameByte	;We're in the same byte.
	neg	cx
	dec	cx
	shr	cx,3
	mov	ax,clipped_font_height
	jcxz    short @f
	inc	cx
	mul	cx
@@:	sub	di,ax

PLABEL BSG_SameByte
	sub	buffer,4
	jmp	BSG_GetStackEntry

PLABEL BSG_IsNop
	rcr	ch,1			;ch should be 88h + phase now.
	sub	ch,88h			;Remove Nop marker. Now ch = phase.
	add	di,bx			;Advance di by one column height.
	jmp	BSG_MaybeAdjustColumn	;Space forward.

if DEFAULT_CHAR_OPTIMIZATION
PLABEL BSG_SkipNullChar
	mov	al,SpaceVerified	;Get verified byte flag.
	dec	al			;
	jl	short BSG_MustVerify	;if it was 0, then we must 1st verify.
ifdef _286
	jg	BSG_CantSkipBlank_10	;if it was 2, then we can't skip it.
else
	jg	BSG_CantSkipBlank	;if it was 2, then we can't skip it.
endif
;-----------------------------------------------------------------------;
;We can skip the blank character because it is really blank.
;-----------------------------------------------------------------------;
	add	di,bx
	jmp	BSG_MaybeAdjustColumn

ifdef _286
;-------------------------------
; A jump vector for 286 version.
;-------------------------------
BSG_CantSkipBlank_10:
	jmp	BSG_CantSkipBlank_10
endif

;-----------------------------------------------------------------------;
;Verify that the blank character is truly blank.
;-----------------------------------------------------------------------;
PLABEL BSG_MustVerify
	mov	SpaceVerified,1		;Assume character is really blank.
ifdef _286
	push	si			;Save ptr to blank character.
	xor	al,al			;
@@:	or	al,ds:[si]		;OR all of the bits of the character
	inc	si			;together.  If we end up with zero,
else
	push	esi			;Save ptr to blank character.
	xor	al,al			;
@@:	or	al,ds:[esi]		;OR all of the bits of the character
	inc	esi			;together.  If we end up with zero,
endif
	dec	dx			;then the character is blank.
	jnz	@b
@@:
ifdef _286
	pop	si
else
	pop	esi
endif
	mov	dx,bx			;restore dx to character height.
	or	al,al			;If al != 0 then
	jnz	short @f		; we can't skip this character.
	add	di,bx			;al == 0, so we can skip this character.
	jmp	BSG_MaybeAdjustColumn	
@@:
	mov	SpaceVerified,2		;Show that the blank char. is not empty.
	mov	SpaceCharOffset,0	;Make ptr 0 so we won't jmp to this code again.
	jmp	BSG_CantSkipBlank	;Go compose it.
endif

;-----------------------------------------------------------------------;
;The follow two composing routines are used by the BackSlacking restart
;code (see below).
;-----------------------------------------------------------------------;
PLABEL BSG_SlowCompose
ifdef _286
	lods	byte ptr ds:[si]
	and	ax,00ffh
else
	lods	byte ptr ds:[esi]	;Must follow this instr. with 386 instr!
	and	eax,000000ffh		;ah must be clear so we'll do it this way.
endif
	ror	ax,cl
	or	es:[di],al
	or	es:[di][bx],ah
	inc	di
	dec	dx
	jnz	BSG_SlowCompose
	ret

PLABEL BBSG_SlowCompose
ifdef _286
	lods	byte ptr ds:[si]
	and	ax,00ffh
else
	lods	byte ptr ds:[esi]	;Must follow this instr. with 386 instr!
	and	eax,000000ffh		;ah must be clear so we'll do it this way.
endif
	ror	ax,cl
	push	bx
	mov	bx,ax
	ror	bx,1
	or	ax,bx
	pop	bx
	or	es:[di],al
	or	es:[di][bx],ah
	inc	di
	dec	dx
	jnz	BBSG_SlowCompose
	ret

;-----------------------------------------------------------------------;
;Yuck!  Because we are restarting, it is possible that a future stack
;entry might backup into the current compose buffer.  So, before 
;transferring the compose buffer to the output device, we must first 
;search forward for any offending stack entries and "OR" the corresponding
;glyph bits into the right place in our current compose buffer.  Each
;offending stack entry will then be "NOPed" so that we don't do anything
;with it when we encounter it again while processing a future compose
;buffer.  
;-----------------------------------------------------------------------;	
PLABEL BBSG_MustRestart
	mov	ax,offset BBSG_SlowCompose
	mov	SlowComposeProc,ax
	jmp	short @f

PLABEL BSG_MustRestart
	mov	ax,offset BSG_SlowCompose
	mov	SlowComposeProc,ax
@@:	mov	ax,LastByteTouched
	xor	dx,dx
	div	bx
	dec	ax
	mov	nColumns,ax
	push	di
	call	DealWithBackSlackers	     ;if we find a BackSlacker
	pop	di
	stc
	ret

;-----------------------------------------------------------------------;
;Note: dx must be zero upon entry to this routine.
;-----------------------------------------------------------------------;	
NLABEL	DealWithBackSlackers
DealWithBackSlackers	proc	near
@@:  	add	cl,ch			;Add phase + width
	cmp	cl,8			;If <= 8, 
	jl	short DWBS_GetStackEntry;  then we did only one column.
	inc	dx
	add	di,bx			;Bits were in both columns. Advance di.
PLABEL DWBS_GetStackEntry
	sub	si,(size frame_data)	;Point to next stack entry.
DWBS_GetStackEntry1:
	mov	cx,wptr ss:[si].fd_width;Get phase and width field.
	test	cx,8000h		;Is it a SPECIAL entry?
	jnz	short DWBS_StopOrBackup	;Yes. Handle it.
	or	dx,dx			;Have we gone negative?
	jge	DealWithBackSlackers	;No. Keep processing stack entries.

PLABEL DWBS_FoundABackSlacker		;Yes!!!
	add	bptr ss:[si+1],88h	;NOP this entry.
	push	si			;Save stack entry pointer.
	xchg 	cl,ch			;Put phase in cl.
ifdef _286
	mov	si,ss:[si][-2]		;Get ptr to glyph.
else
	mov	esi,ss:[si][-4]		;Get ptr to glyph.
endif
	push	dx
	mov	dx,bx			;Make sure glyph height is in dx.
	push	di
	call	SlowComposeProc		;Compose the bits into the Compose buffer.
	pop	di
	pop	dx
	pop	si			;Restore stack entry pointer.
	jmp	DealWithBackSlackers	;And back we go for more work....
	
PLABEL DWBS_StopOrBackup
	cmp	cx,0ffffh		;Is it a STOP sentinel?
	jne	short @f		; If so, we are done.
	ret
@@:	shl	ch,1			;Is it a NOP entry?
	jz	short @f		;If so, then
	rcr	ch,1			; remove the NOP marker and 
	sub	ch,88h			; space forward appropriately.
	jmp	DealWithBackSlackers
@@:					;Must be BACKUP marker.
	mov	ax,ss:[si][-2]		;Get num. of pixels to backup.
	sub	si,4			;adjust buffer ptr.
	add	cx,ax			;Add in phase.
	jns	DWBS_GetStackEntry1     ;We're in the same byte.
	neg	cx
	dec	cx
	shr	cx,3
	mov	ax,clipped_font_height
	dec	dx
	jcxz    short @f
	sub	dx,cx
	inc	cx
	push	dx
	mul	cx
	pop	dx
@@:	sub	di,ax
 	jmp	DWBS_GetStackEntry1
DealWithBackSlackers	endp

;-----------------------------------------------------------------------;
;PHASE 0 CODE  -- NORMAL --
;-----------------------------------------------------------------------;
PLABEL BSG_PhaseIsZero
ifdef _286
	test	dx,1
else
	test	dx,3
endif
	jnz	short BSG_P0AlignHeight

PLABEL BSG_P0HeightIsAligned
	mov	ax,dx
ifdef _286
	and	ax,0eh
else
	and	eax,0ch
endif

	jz	short BSG_P0Compose16Bytes
	neg	ax
	add	ax,16
ifdef _286
	sub	si,ax
	sub	di,ax
	push	bx
	mov	bx,ax
	mov	ax,cs:[bx].BSG_P0ComposeTable
	pop	bx
	jmp	ax
else
	sub	esi,eax
	sub	di,ax
	jmp	cs:[eax].BSG_P0ComposeTable
endif

PLABEL BSG_P0ShouldNeverGetHere
	int	3			;Should never get here!
	jmp	short BSG_P0LoopBottom

ifdef _286
BSG_P0ComposeTable	label	word
	dw	BSG_P0ShouldNeverGetHere
	dw	BSG_P0Compose14Bytes
	dw	BSG_P0Compose12Bytes
	dw	BSG_P0Compose10Bytes
	dw	BSG_P0Compose8Bytes
	dw	BSG_P0Compose6Bytes
	dw	BSG_P0Compose4Bytes
	dw	BSG_P0Compose2Bytes
else
BSG_P0ComposeTable	label	word
	dw	BSG_P0ShouldNeverGetHere
	dw	BSG_P0ShouldNeverGetHere
	dw	BSG_P0Compose12Bytes
	dw	BSG_P0ShouldNeverGetHere
	dw	BSG_P0Compose8Bytes
	dw	BSG_P0ShouldNeverGetHere
	dw	BSG_P0Compose4Bytes
	dw	BSG_P0ShouldNeverGetHere
endif


PLABEL BSG_P0Compose16Bytes
ifdef _286
	mov	ax,ds:[si]
	or	es:[di],ax

PLABEL BSG_P0Compose14Bytes
	mov	ax,ds:[si+2]
	or	es:[di+2],ax

else
	mov	eax,ds:[esi]
	or	es:[di],eax
endif

PLABEL BSG_P0Compose12Bytes
ifdef _286
	mov	ax,ds:[si+4]
	or	es:[di+4],ax

PLABEL BSG_P0Compose10Bytes
	mov	ax,ds:[si+6]
	or	es:[di+6],ax

else
	mov	eax,ds:[esi+4]
	or	es:[di+4],eax
endif

PLABEL BSG_P0Compose8Bytes
ifdef _286
	mov	ax,ds:[si+8]
	or	es:[di+8],ax

PLABEL BSG_P0Compose6Bytes
	mov	ax,ds:[si+10]
	or	es:[di+10],ax

else
	mov	eax,ds:[esi+8]
	or	es:[di+8],eax
endif

PLABEL BSG_P0Compose4Bytes
ifdef _286
	mov	ax,ds:[si+12]
	or	es:[di+12],ax

PLABEL BSG_P0Compose2Bytes
	mov	ax,ds:[si+14]
	or	es:[di+14],ax

else
	mov	eax,ds:[esi+12]
	or	es:[di+12],eax
endif
	
PLABEL BSG_P0LoopBottom
ifdef _286
 	add	si,16
else
 	add	esi,16
endif
	add	di,16
	sub	dx,16
	jg	BSG_P0Compose16Bytes
	jmp	BSG_EntryComplete

PLABEL BSG_P0AlignHeight
ifndef _286
	mov	ax,dx
	and	ax,3
	cmp	ax,3
	jz	short BSG_P0Compose3Bytes
	cmp	ax,2
	jz	short BSG_P0Compose2Bytes
endif

PLABEL BSG_P0Compose1Byte
ifdef _286
	lods	byte ptr ds:[si]
	xor	ah,ah
else
	lods	byte ptr ds:[esi]	;Must follow this instr. with 386 instr!
	and	eax,000000ffh		;ah must be clear so we'll do it this way.
endif
	or	es:[di],al
	inc	di
	dec	dx
ifdef _286
	jz	BSG_EntryComplete_10
else
	jz	BSG_EntryComplete
endif
	jmp	BSG_P0HeightIsAligned

ifdef _286
;-------------------------------
; A jump vector for 286 version.
;-------------------------------
BSG_EntryComplete_10:
	jmp	BSG_EntryComplete
endif

ifndef _286
PLABEL BSG_P0Compose2Bytes
	lods	word ptr ds:[esi]
	and	eax,0000ffffh
	or	es:[di],eax
	add	di,2
	sub	dx,2
	jz	BSG_EntryComplete
	jmp	BSG_P0HeightIsAligned

PLABEL BSG_P0Compose3Bytes
	mov	eax,ds:[esi]
	and	eax,00ffffffh
	or	es:[di],eax
	add	esi,3
	add	di,3
	sub	dx,3
	jz	BSG_EntryComplete
	jmp	BSG_P0HeightIsAligned
endif

ifndef _286
;-----------------------------------------------------------------------;
;Process stack entry loop -- B O L D --
;-----------------------------------------------------------------------;
PLABEL BBSG_GetStackEntry
	mov	dx,bx
	mov	si,buffer
	mov	cx,wptr ss:[si].fd_width
	errnz	fd_phase-fd_width-1

	test	cx,8000h
	jnz	BBSG_StopOrBackup
	cmp	di,HighWater		;Are we near the end of ComposeArea?
	ja	BBSG_MustRestart	;Yes, exit, output what we have
					;so far, and start over.
	xchg 	cl,ch			;Put phase in cl. Width in ch.
	mov	esi,ss:[si][-4]


if DEFAULT_CHAR_OPTIMIZATION
;-----------------------------------------------------------------------;
;Skip over any space or null character entries.
;-----------------------------------------------------------------------;
	cmp	esi,SpaceCharOffset
	je	BBSG_SkipNullChar
endif

;-----------------------------------------------------------------------;
;And now the hard part....
;Currently:
;  dx = clipped height
;  bx = clipped height (constant)
;  cx = width | phase
;  es:di --> Compose buffer
;  ds:esi --> font data for this stack entry.
;-----------------------------------------------------------------------;
PLABEL BBSG_CantSkipBlank
	test	cx,00ffh
	jz	BBSG_PhaseIsZero

;-----------------------------------------------------------------------;
;PHASE NOT 0 CODE  -- B O L D --
;-----------------------------------------------------------------------;
PLABEL BBSG_PhaseIsNonZero
	mov	bp,cx
	and	bp,00ffh

	shl	bp,2
	mov	eax,cs:[bp].MaskTable
	mov	Mask1,eax
	not	eax
	mov	Mask2,eax
	mov	ebp,cs:[bp][4].MaskTable
	mov	BMask1,ebp
	not	ebp			;BMask2
	test	dx,3
	jnz	BBSG_AlignHeight

PLABEL BBSG_HeightIsAligned
	mov	Height,dx
	and	edx,0ch
	jz	short BBSG_Compose16Bytes
	neg	dx
	add	dx,16
	sub	esi,edx
	sub	di,dx
	jmp	cs:[edx].BBSG_ComposeTable

PLABEL BBSG_ShouldNeverGetHere
	int	3			;Should never get here!
	jmp	BBSG_LoopBottom

BBSG_ComposeTable	label	word
	dw	BBSG_ShouldNeverGetHere
	dw	BBSG_ShouldNeverGetHere
	dw	BBSG_Compose12Bytes
	dw	BBSG_ShouldNeverGetHere
	dw	BBSG_Compose8Bytes
	dw	BBSG_ShouldNeverGetHere
	dw	BBSG_Compose4Bytes
	dw	BBSG_ShouldNeverGetHere

;-----------------------------------------------------------------------;
;Compose the rest of the column.
;-----------------------------------------------------------------------;
PLABEL BBSG_Compose16Bytes
	mov	eax,ds:[esi]
	ror	eax,cl
	mov	edx,eax
	and	eax,Mask1
	rol	edx,8
	or	es:[di],eax
	and	edx,Mask2
	or	es:[di][bx],edx

	mov	eax,ds:[esi]
	ror	eax,1
	ror	eax,cl
	mov	edx,eax
	and	eax,BMask1
	rol	edx,8
	or	es:[di],eax
	and	edx,ebp
	or	es:[di][bx],edx

PLABEL BBSG_Compose12Bytes
	mov	eax,ds:[esi+4]
	ror	eax,cl
	mov	edx,eax
	and	eax,Mask1
	rol	edx,8
	or	es:[di+4],eax
	and	edx,Mask2
	or	es:[di+4][bx],edx

	mov	eax,ds:[esi+4]
	ror	eax,1
	ror	eax,cl
	mov	edx,eax
	and	eax,BMask1
	rol	edx,8
	or	es:[di+4],eax
	and	edx,ebp
	or	es:[di+4][bx],edx

PLABEL BBSG_Compose8Bytes
	mov	eax,ds:[esi+8]
	ror	eax,cl
	mov	edx,eax
	and	eax,Mask1
	rol	edx,8
	or	es:[di+8],eax
	and	edx,Mask2
	or	es:[di+8][bx],edx

	mov	eax,ds:[esi+8]
	ror	eax,1
	ror	eax,cl
	mov	edx,eax
	and	eax,BMask1
	rol	edx,8
	or	es:[di+8],eax
	and	edx,ebp
	or	es:[di+8][bx],edx

PLABEL BBSG_Compose4Bytes
	mov	eax,ds:[esi+12]
	ror	eax,cl
	mov	edx,eax
	and	eax,Mask1
	rol	edx,8
	or	es:[di+12],eax
	and	edx,Mask2
	or	es:[di+12][bx],edx

	mov	eax,ds:[esi+12]
	ror	eax,1
	ror	eax,cl
	mov	edx,eax
	and	eax,BMask1
	rol	edx,8
	or	es:[di+12],eax
	and	edx,ebp
	or	es:[di+12][bx],edx
	
PLABEL BBSG_LoopBottom
 	add	esi,16
	add	di,16
	sub	word ptr Height,16
	jg	BBSG_Compose16Bytes

PLABEL BBSG_EntryComplete
	mov	bp,SaveBp

PLABEL BBSG_MaybeAdjustColumn
	mov	si,di
	add	si,bx
	mov	ax,LastByteTouched
	max_ax	si
	mov	LastByteTouched,ax
  	add	cl,ch
	cmp	cl,8
	jl	BBSG_AdjustColumn
	sub	buffer,(size frame_data)
	jmp	BBSG_GetStackEntry

PLABEL BBSG_AlignHeight
	mov	ax,dx
	and	ax,3
	cmp	ax,3
	jz	BBSG_Compose3Bytes
	cmp	ax,2
	jz	short BBSG_Compose2Bytes
	
PLABEL BBSG_Compose1Byte
	lods	byte ptr ds:[esi]	;Must follow this instr. with 386 instr!
	and	eax,000000ffh		;ah must be clear so we'll do it this way.
	ror	ax,cl
	push	bx
	mov	bx,ax
	ror	bx,1
	or	ax,bx
	pop	bx
	or	es:[di],al
	or	es:[di][bx],ah
	inc	di
	dec	dx
	jz	BBSG_EntryComplete
	jmp	BBSG_HeightIsAligned

PLABEL BBSG_Compose2Bytes
	push	dx
	mov	eax,ds:[esi]
	and	eax,0000ffffh
	ror	eax,cl
	mov	edx,eax
	and	eax,Mask1
	rol	edx,8
	or	es:[di],eax
	and	edx,Mask2
	or	es:[di][bx],edx

	lods	word ptr ds:[esi]
	and	eax,0000ffffh
	ror	eax,1
	ror	eax,cl
	mov	edx,eax
	and	eax,BMask1
	rol	edx,8
	or	es:[di],eax
	and	edx,ebp
	or	es:[di][bx],edx

	add	di,2
	pop	dx
	sub	dx,2
	jz	BBSG_EntryComplete
	jmp	BBSG_HeightIsAligned

PLABEL BBSG_Compose3Bytes
	mov	eax,ds:[esi]
	and	eax,00ffffffh
	jnz	short @f
	add	esi,3
	add	di,3
	sub	dx,3
	jz	BBSG_EntryComplete
	jmp	BBSG_HeightIsAligned
@@:	push	dx
	ror	eax,cl
	mov	edx,eax
	and	eax,Mask1
	rol	edx,8
	or	es:[di],eax
	and	edx,Mask2
	or	es:[di][bx],edx

	mov	eax,ds:[esi]
	and	eax,00ffffffh
	ror	eax,1
	ror	eax,cl
	mov	edx,eax
	and	eax,BMask1
	rol	edx,8
	or	es:[di],eax
	and	edx,ebp
	or	es:[di][bx],edx

	add	esi,3
	add	di,3
	pop	dx
	sub	dx,3
	jz	BBSG_EntryComplete
	jmp	BBSG_HeightIsAligned

PLABEL BBSG_AdjustColumn
	sub	di,bx
	sub	buffer,(size frame_data)
	jmp	BBSG_GetStackEntry

PLABEL BBSG_StopOrBackup
	cmp	cx,0ffffh		;stop sentinel.
	je	BSG_Done		;Must be 8000h (Backup sentinel).

PLABEL BBSG_Backup
	shl	ch,1			;Shift off msb.
	jnz	short BBSG_IsNop	;if still non zero, it is a Nop entry.
	xor	ch,ch
	mov	ax,ss:[si][-2]		;Get num. of pixels to backup.
	add	cx,ax			;Add in phase.
	jns	short BBSG_SameByte	;We're in the same byte.
	neg	cx
	dec	cx
	shr	cx,3
	mov	ax,clipped_font_height
	jcxz    short @f
	inc	cx
	mul	cx
@@:	sub	di,ax

PLABEL BBSG_SameByte
	sub	buffer,4
	jmp	BBSG_GetStackEntry

PLABEL BBSG_IsNop
	rcr	ch,1			;ch should be 88h + phase now.
	sub	ch,88h			;Remove Nop marker. Now ch = phase.
	add	di,bx			;Advance di by one column height.
	jmp	BBSG_MaybeAdjustColumn	;Space forward.


if DEFAULT_CHAR_OPTIMIZATION
PLABEL BBSG_SkipNullChar
	mov	al,SpaceVerified	;Get flag.
	dec	al			;
	jl	short BBSG_MustVerify	;if it was 0, then we must 1st verify.
	jg	BBSG_CantSkipBlank	;if it was 2, then we can't skip it.
;-----------------------------------------------------------------------;
;We can skip the blank character because it is really blank.
;-----------------------------------------------------------------------;
	add	di,bx
	jmp	BBSG_MaybeAdjustColumn

;-----------------------------------------------------------------------;
;Verify that the blank character is truly blank.
;-----------------------------------------------------------------------;
PLABEL BBSG_MustVerify
	mov	SpaceVerified,1		;Assume character is really blank.
	push	esi			;Save ptr to blank character.
	xor	al,al			;
@@:	or	al,ds:[esi]		;OR all of the bits of the character
	inc	esi			;together.  If we end up with zero,
	dec	dx			;then the character is blank.
	jnz	@b
	pop	esi
	mov	dx,bx			;restore dx to character height.
	or	al,al			;If al != 0 then
	jnz	short @f		; we can't skip this character.
	add	di,bx			;al == 0, so we can skip this character.
	jmp	BBSG_MaybeAdjustColumn	
@@:
	mov	SpaceVerified,2		;Show that the blank char. is not empty.
	mov	SpaceCharOffset,0	;Make ptr 0 so we won't jmp to this code again.
	jmp	BBSG_CantSkipBlank	;Go compose it.
endif

;-----------------------------------------------------------------------;
;PHASE 0 CODE -- BOLD --
;-----------------------------------------------------------------------;
PLABEL BBSG_PhaseIsZero
	test	dx,3
	jnz	BBSG_P0AlignHeight

PLABEL BBSG_P0HeightIsAligned
	mov	Height,dx
	and	edx,0ch
	jz	short BBSG_P0Compose16Bytes
	neg	dx
	add	dx,16
	sub	esi,edx
	sub	di,dx
	jmp	cs:[edx].BBSG_P0ComposeTable

PLABEL BBSG_P0ShouldNeverGetHere
	int	3			;Should never get here!
	jmp	BBSG_P0LoopBottom

BBSG_P0ComposeTable	label	word
	dw	BBSG_P0ShouldNeverGetHere
	dw	BBSG_P0ShouldNeverGetHere
	dw	BBSG_P0Compose12Bytes
	dw	BBSG_P0ShouldNeverGetHere
	dw	BBSG_P0Compose8Bytes
	dw	BBSG_P0ShouldNeverGetHere
	dw	BBSG_P0Compose4Bytes
	dw	BBSG_P0ShouldNeverGetHere

;-----------------------------------------------------------------------;
;Compose the rest of the column.
;-----------------------------------------------------------------------;
PLABEL BBSG_P0Compose16Bytes
	mov	eax,ds:[esi]
	or	es:[di],eax
	ror	eax,1
	mov	edx,eax
	and	eax,01111111011111110111111101111111b
	rol	edx,8
	or	es:[di],eax
	and	edx,10000000100000001000000010000000b
	or	es:[di][bx],edx


PLABEL BBSG_P0Compose12Bytes
	mov	eax,ds:[esi+4]
	or	es:[di+4],eax
	ror	eax,1
	mov	edx,eax
	and	eax,01111111011111110111111101111111b
	rol	edx,8
	or	es:[di+4],eax
	and	edx,10000000100000001000000010000000b
	or	es:[di+4][bx],edx

PLABEL BBSG_P0Compose8Bytes
	mov	eax,ds:[esi+8]
	or	es:[di+8],eax
	ror	eax,1
	mov	edx,eax
	and	eax,01111111011111110111111101111111b
	rol	edx,8
	or	es:[di+8],eax
	and	edx,10000000100000001000000010000000b
	or	es:[di+8][bx],edx

PLABEL BBSG_P0Compose4Bytes
	mov	eax,ds:[esi+12]
	or	es:[di+12],eax
	ror	eax,1
	mov	edx,eax
	and	eax,01111111011111110111111101111111b
	rol	edx,8
	or	es:[di+12],eax
	and	edx,10000000100000001000000010000000b
	or	es:[di+12][bx],edx
	
PLABEL BBSG_P0LoopBottom
 	add	esi,16
	add	di,16
	sub	word ptr Height,16
	jg	BBSG_P0Compose16Bytes
	jmp	BBSG_EntryComplete

PLABEL BBSG_P0AlignHeight
	mov	ax,dx
	and	ax,3
	cmp	ax,3
	jz	short BBSG_P0Compose3Bytes
	cmp	ax,2
	jz	short BBSG_P0Compose2Bytes

PLABEL BBSG_P0Compose1Byte
	lods	byte ptr ds:[esi]	;Must follow this instr. with 386 instr!
	and	eax,000000ffh		;ah must be clear so we'll do it this way.
	push	bx
	mov	bx,ax
 	ror	bx,1
	or	ax,bx
	pop	bx
	or	es:[di],al
	or	es:[di][bx],ah
	inc	di
	dec	dx
	jz	BBSG_EntryComplete
	jmp	BBSG_P0HeightIsAligned

PLABEL BBSG_P0Compose2Bytes
	push	dx
	lods	word ptr ds:[esi]
	and	eax,0000ffffh
	or	es:[di],eax
	ror	eax,1
	mov	edx,eax
	and	eax,01111111011111110111111101111111b
	rol	edx,8
	or	es:[di],eax
	and	edx,10000000100000001000000010000000b
	or	es:[di][bx],edx
	add	di,2
	pop	dx
	sub	dx,2
	jz	BBSG_EntryComplete
	jmp	BBSG_P0HeightIsAligned

PLABEL BBSG_P0Compose3Bytes
	push	dx
	mov	eax,ds:[esi]
	and	eax,00ffffffh
	or	es:[di],eax
	ror	eax,1
	mov	edx,eax
	and	eax,01111111011111110111111101111111b
	rol	edx,8
	or	es:[di],eax
	and	edx,10000000100000001000000010000000b
	or	es:[di][bx],edx
	add	esi,3
	add	di,3
	pop	dx
	sub	dx,3
	jz	BBSG_EntryComplete
	jmp	BBSG_P0HeightIsAligned

endif
ifdef _286
BuildSuperGlyph_286	endp
else
BuildSuperGlyph_386	endp
endif


;----------------------------------------------------------------------------;
;>>>>>>>>>>>>>>>>>>>  O U T P U T    R O U T I N E S  <<<<<<<<<<<<<<<<<<<<<<<;
;----------------------------------------------------------------------------;
;----------------------------------------------------------------------------;
; Entry:
;       cx     scan length in bytes (number of columns)
;       dx     height of mono image in scans.
;       bx     height of mono image in scans (constant).
;    ds:si --> ComposeArea containing mono. image of string.
;    es:di --> Destination (screen memory).
; Exit:
;    es:di --> last screen byte + 1 of top scan.
;    ds:si --> top of last column processed + 1
;	bx     preserved.
;----------------------------------------------------------------------------;
pNonClippedOutput	label	word
	dw	offset CBmpTr_NotClipped
	dw	offset CBmpOp_NotClipped
	dw	offset DevTr_NotClipped
	dw	offset DevOp_NotClipped

	dw	offset MBmpTr_NotClipped
	dw	offset MBmpOp_NotClipped
	dw	offset DevTr_NotClipped
	dw	offset DevOp_NotClipped

pClippedOutput	label	word
	dw	offset CBmpTr_Clipped
	dw	offset CBmpOp_Clipped
	dw	offset DevTr_Clipped
	dw	offset DevOp_Clipped

	dw	offset MBmpTr_Clipped
	dw	offset MBmpOp_Clipped
	dw	offset DevTr_Clipped
	dw	offset DevOp_Clipped


;----------------------------------------------------------------------------;
; D E V I C E
; OPAQUE: Not clipped
;----------------------------------------------------------------------------;
DevOp_NotClipped	proc	near
NLABEL DevOp_NotClipped

	test	di,1			;Even destination boundary?
	jz	short DevOp_DestEven	;Yes.

PLABEL DevOp_DestOdd
	call	DevOp_Byte1		;Do 1 byte column....
	mov	dx,bx			;Restore dx with height of image.

PLABEL DevOp_DestEven
PLABEL DevOp_Words
	push	cx
	shr	cx,1			;If cx is 1 or 0, then..
	jz	short DevOp_WordsExit   ; nothing to do.
	mov	bp,next_scan		;Cache next_scan in bp for faster access.
ifdef _286
	mov	ColumnsToDo,cx		;save off the number of columns to do.
else
	rol	ecx,16			;cache columns to do in upper word of ecx.
endif
	mov	Height,dx		;save off glyph height.
PLABEL DevOp_WordsLoop
	push	di			;save ptr. destination screen word.
	mov	cx,Height		;Load glyph height.
ifdef _286
	test	cx,1
else
	test	cx,3
endif
	jnz	short DevOp_AdjustHeight
PLABEL DevOp_HeightAdjusted
@@:
ifdef _286
	mov	ax,[si]			;Get 2 words from 1st column.
	mov	dx,[si][bx]		;Get 2 words from 2nd column.
	xchg	ah,dl			;transpose (ax,dx ready to output).
if SLOW_EGA
	mov	es:[di],al		;Write al to the screen.
	mov	es:[di+1],ah		;Write ah to the screen.
else
	mov	es:[di],ax		;Write ax to the screen.
endif
	add	di,bp			;point to next scan.
	mov	es:[di],dx		;write dx to the screen.
	add	di,bp			;point to next scan.
	add	si,2			;point to 2 bytes down the glyph column.
	sub	cx,2			;More to do?
else
	mov	eax,[si]		;Get 4 words from 1st column.
	mov	edx,[si][bx]		;Get 4 words from 2nd column.
	xchg	ah,dl			;transpose (ax,dx ready to output).
if SLOW_EGA
	mov	es:[di],al		;Write al to the screen.
	mov	es:[di+1],ah		;Write ah to the screen.
else
	mov	es:[di],ax		;Write ax to the screen.
endif
	rol	eax,16			;Prepare to transpose upper words.
	add	di,bp			;point to next scan.
if SLOW_EGA
	mov	es:[di],dl		;Write dl to the screen.
	mov	es:[di+1],dh		;Write dh to the screen.
else
	mov	es:[di],dx		;write dx to the screen.
endif
	rol	edx,16			;Prepare to transpose upper words.
	xchg	ah,dl			;transpose (ax,dx ready to output).
	add	di,bp			;point to next scan.
if SLOW_EGA
	mov	es:[di],al		;write al to screen.
	mov	es:[di+1],ah		;write ah to screen.
else
	mov	es:[di],ax		;write ax to screen.
endif
	add	di,bp			;point to next scan.
if SLOW_EGA
	mov	es:[di],dl		;write dx to screen.
	mov	es:[di+1],dh		;write dx to screen.
else
	mov	es:[di],dx		;write dx to screen.
endif
	add	di,bp			;point to next scan.
	add	si,4			;point to 4 bytes down the glyph column.
	sub	cx,4			;More to do?
endif
	jg	@b			;yes.

PLABEL DevOp_InnerLoopBottom
	add	si,bx			;no. Point si to next glyph column.
	pop	di			;get di back
	add	di,2			;point di to next screen word (along scan).
ifdef _286
	dec	word ptr ColumnsToDo	;All screen word columns processed?
else
	rol	ecx,16			;Get columns to do...
	dec	cx			;All screen word columns processed?
	rol	ecx,16			;put updated columns to do in upper word.
endif
	jnz	DevOp_WordsLoop		;no.
	mov	bp,SaveBp		;yes. Restore bp.
PLABEL DevOp_WordsExit
	pop	cx
	and	cx,1			;If odd then do a final byte wide column.
	jz	short DevOp_Exit
PLABEL DevOp_Byte1
	mov	dx,bx			;restore dx with height.
	push	di		
	mov	bp,next_scan
@@:
	lodsb
	mov	es:[di],al
	add	di,bp
	dec	dx
	jnz	@b
	mov	bp,SaveBp
	pop	di
	inc	di
	dec	cx
PLABEL DevOp_Exit
	ret				;We're done.

PLABEL DevOp_AdjustHeight
@@:	mov	al,[si]
	mov	ah,[si][bx]
if SLOW_EGA
	mov	es:[di],al
	mov	es:[di+1],ah
else
	mov	es:[di],ax
endif
	add	di,bp
	inc	si
	dec	cx
ifndef _286
	test	cx,3
	jnz	@b
endif
	jcxz	DevOp_InnerLoopBottom
	jmp	DevOp_HeightAdjusted

DevOp_NotClipped	endp

;----------------------------------------------------------------------------;
; D E V I C E
; OPAQUE: clipped
;----------------------------------------------------------------------------;
DevOp_Clipped	proc	near
NLABEL DevOp_Clipped
	mov	cx,dx

	call	setup_ega_opaque_clip_magic
	jc	short @f		;planes are disable, skip fg pass.

;----------------------------------------------------------------------------;
;Do the foreground planes of the bit pattern.
;----------------------------------------------------------------------------;
	push	cx
	push	si
	push	di
	call	DevOp_FgPass
	pop	di
	pop	si
	pop	cx

@@:
	mov	dx,EGA_BASE + SEQ_DATA
	mov	al,bptr colors[BACKGROUND]
	out	dx,al

;----------------------------------------------------------------------------;
;If the bg planes are a subset of the fg planes then we are done (since the
;bg planes were set correctly with the fg planes.  If there are other bg
;planes that need to be done, then we do them by writing the complement of
;the pattern to video memory.
;----------------------------------------------------------------------------;
	mov	ax,colors
	xor	al,ah			;determine mismatching bits
	and	al,ah			;isolate those in the bk color
	jz	short DevOp_NoPlanesLeft

;----------------------------------------------------------------------------;
;Do the remaining background planes of the bit pattern.
;----------------------------------------------------------------------------;
	push	cx
	push	si
	push	di
	call	DevOp_BgPass
	pop	di
	pop	si
	pop	cx

PLABEL	DevOp_NoPlanesLeft
ifdef _286
	call	set_ega_opaque_mode_286	;reset video registrs for opaque mode.
else
	call	set_ega_opaque_mode_386	;reset video registrs for opaque mode.
endif
	add	si,cx
	inc	di
	ret

PLABEL	DevOp_FgPass
	mov 	dx,next_scan
@@:	lodsb
	xchg	es:[di],al
	add	di,dx
	dec	cx
	jnz	@b
	ret

PLABEL	DevOp_BgPass
	mov 	dx,next_scan
@@:	lodsb
	not	al
	xchg	es:[di],al
	add	di,dx
	dec	cx
	jnz	@b
	ret

DevOp_Clipped	endp

;----------------------------------------------------------------------------;
; D E V I C E
; TRANSPARENT: Not clipped
;----------------------------------------------------------------------------;
DevTr_NotClipped	proc	near
NLABEL DevTr_NotClipped
	test	di,1			;Even destination boundary?
	jz	short DevTr_DestEven	;Yes.

PLABEL	DevTr_DestOdd
	call	DevTr_Byte1		;Do 1 byte column....
	mov	dx,bx			;Restore dx with height of image.

PLABEL	DevTr_DestEven
PLABEL	DevTr_Words
	push	cx
	shr	cx,1			;If cx is 1 or 0, then..
if MASMFLAGS and EGA
	jz	DevTr_WordsExit   	; nothing to do.
else
	jz	short DevTr_WordsExit   ; nothing to do.
endif
	mov	bp,next_scan		;Cache next_scan in bp for faster access.
ifdef _286
	mov	ColumnsToDo,cx
else
	rol	ecx,16			;cache columns to do in upper word of ecx.
endif
	mov	Height,dx		;save off glyph height.
PLABEL	DevTr_WordsLoop
	push	di			;save ptr. destination screen word.
	mov	cx,Height		;Load glyph height.
ifdef _286
	test	cx,1
else
	test	cx,3
endif
	jnz	DevTr_AdjustHeight
PLABEL	DevTr_HeightAdjusted
ifdef _286
	mov	ax,[si]			;Get 2 words from 1st column.
	mov	dx,[si][bx]		;Get 2 words from 2nd column.
else
	mov	eax,[si]		;Get 4 words from 1st column.
	mov	edx,[si][bx]		;Get 4 words from 2nd column.
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
@@:
	add	di,bp			;point to next scan.
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

@@:
	add	di,bp			;point to next scan.
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
@@:
	add	di,bp			;point to next scan.
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
@@:
	add	di,bp			;point to next scan.
endif

ifdef _286
	add	si,2			;point to 2 bytes down the glyph column.
	sub	cx,2			;More to do?
else
	add	si,4			;point to 4 bytes down the glyph column.
	sub	cx,4			;More to do?
endif
	jg	DevTr_HeightAdjusted	;yes.

PLABEL	DevTr_InnerLoopBottom
	add	si,bx			;no. Point si to next glyph column.
	pop	di			;get di back
	add	di,2			;point di to next screen word (along scan).
ifdef _286
	dec	word ptr ColumnsToDo	;All screen word columns processed?
else
	rol	ecx,16			;Get columns to do...
	dec	cx			;All screen word columns processed?
	rol	ecx,16			;put updated columns to do in upper word.
endif
	jnz	DevTr_WordsLoop		;no.
	mov	bp,SaveBp		;yes. Restore bp.
PLABEL	DevTr_WordsExit
	pop	cx
	and	cx,1			;If odd then do a final byte wide column.
	jz	short DevTr_Exit
PLABEL	DevTr_Byte1
	mov	dx,bx			;restore dx with height.
	push	di		
	mov	bp,next_scan
PLABEL	DevTr_Byte1Loop
	lodsb
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
	jnz	DevTr_Byte1Loop
	mov	bp,SaveBp
	pop	di
	inc	di
	dec	cx
PLABEL	DevTr_Exit
	ret				;We're done.

PLABEL	DevTr_AdjustHeight
	mov	al,[si]
	mov	ah,[si][bx]
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
	inc	si
	dec	cx
ifndef _286
	test	cx,3
	jnz	DevTr_AdjustHeight
endif
	jcxz	DevTr_InnerLoopBottom
	jmp	DevTr_HeightAdjusted
DevTr_NotClipped	endp

;----------------------------------------------------------------------------;
; D E V I C E
; TRANSPARENT: clipped
;----------------------------------------------------------------------------;
DevTr_Clipped	proc	near
NLABEL DevTr_Clipped
	push	di
	mov	cx,next_scan
PLABEL	DevTr_ClippedLoop
	mov	al,[si]	
	and	al,ah			;ah = clip mask.
	jz	short @f
if MASMFLAGS and EGA
	push	dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	xchg	es:[di],al
	pop	dx
else
	xchg	es:[di],al
endif
@@:	add	di,cx
	inc	si
	dec	dx
	jnz	DevTr_ClippedLoop
	pop	di
	inc	di
	ret
DevTr_Clipped	endp

;----------------------------------------------------------------------------;
; B I T M A P (color)
; OPAQUE: Not clipped
;
;	  bg fg   Bitpattern (1110 1110)
;         -----
;         0  0   0000 0000  (0's)
;         0  1   1110 1110  (pattern)
;         1  0   0001 0001  (not pattern)
;         1  1   1111 1111  (1's)
;
;  So...if the fg and bg colors share a plane, then stuff 1's in for the 
;  bit pattern and fall through.
;
;
;	 mov	bl,bit pattern
;        mov	al,not fg
;	 mov	bh,bg
;	 xor	bh,al
;	 shr	bh
;	 jnc	short @f  ;we can do this w/o jumps!  See the actual code.
;	 mov	bl,0ffh
;@@:	 
;	 ror	al
;	 sbb	ah,ah	  ;ah = 11111111 if fg bit is zero, 00000000 if 1.
;	 xor	bl,ah	  ;gives Not Pattern if fg bit is zero, Pattern otherwise.
;
;----------------------------------------------------------------------------;
CBmpOp_NotClipped	proc	near
NLABEL	CBmpOp_NotClipped
	mov	al,num_planes
	dec	al
	mov	byte ptr nPlanes,al
	mov	ah,byte ptr colors[FOREGROUND]
	mov	al,byte ptr colors[BACKGROUND]
	not	ah			;ah = not fg.
	mov	NotFg,ah
	xor	al,ah			;al = 1's where fg/bg share a plane.

; 1st three times.
@@:		
	ror	al,1			;'carry' if plane is shared.
	push	ax
ifdef _286
	sbb	ax,ax		        ;0ffffh if plane is shared, 0 otherwise.
	mov	BitModifier,ax
	ror	byte ptr NotFg,1
	sbb	ax,ax
	mov	BitInverter,ax
else
	sbb	eax,eax		        ;0ffffffffh if plane is shared, 0 otherwise.
	mov	BitModifier,eax
	ror	byte ptr NotFg,1
	sbb	eax,eax
	mov	BitInverter,eax
endif
	push	di
	push	si
	push	cx
	call	MBmpOp_NC_ColorEntryPoint
	pop	cx
	pop	si
	pop	di
	add	di,next_plane
	mov	dx,bx
	pop	ax
	dec	byte ptr nPlanes
	jnz	@b

; Fourth time -- don't push and pop stuff.
	ror	al,1			;'carry' if plane is shared.
ifdef _286
	sbb	ax,ax		        ;0ffffh if plane is shared, 0 otherwise.
	mov	BitModifier,ax
	ror	byte ptr NotFg,1
	sbb	ax,ax
	mov	BitInverter,ax
else
	sbb	eax,eax		        ;0ffffffffh if plane is shared, 0 otherwise.
	mov	BitModifier,eax
	ror	byte ptr NotFg,1
	sbb	eax,eax
	mov	BitInverter,eax
endif
	call	MBmpOp_NC_ColorEntryPoint
	mov	ax,next_plane
	sub	di,ax
	sub	di,ax
	sub	di,ax
	ret
CBmpOp_NotClipped	endp

;----------------------------------------------------------------------------;
; B I T M A P (color)
; OPAQUE: clipped
;----------------------------------------------------------------------------;
CBmpOp_Clipped	proc	near
NLABEL	CBmpOp_Clipped
 	mov	ClipMask,ah
	mov	al,num_planes
	dec	al
	mov	byte ptr nPlanes,al
	mov	ah,byte ptr colors[FOREGROUND]
	mov	al,byte ptr colors[BACKGROUND]
	not	ah			;ah = not fg.
	mov	NotFg,ah
	xor	al,ah			;al = 1's where fg/bg share a plane.
; 1st three times.
@@:		
	ror	al,1			;'carry' if plane is shared.
	push	ax
ifdef _286
	sbb	ax,ax		        ;0ffffh if plane is shared, 0 otherwise.
	mov	BitModifier,ax
	ror	byte ptr NotFg,1
	sbb	ax,ax
	mov	BitInverter,ax
else
	sbb	eax,eax		        ;0ffffffffh if plane is shared, 0 otherwise.
	mov	BitModifier,eax
	ror	byte ptr NotFg,1
	sbb	eax,eax
	mov	BitInverter,eax
endif
	push	di
	push	si
	push	cx
	mov	ah,ClipMask
	call	MBmpOp_C_ColorEntryPoint
	pop	cx
	pop	si
	pop	di
	add	di,next_plane
	mov	dx,bx
	pop	ax
	dec	byte ptr nPlanes
	jnz	@b

; Fourth time -- don't push and pop stuff.
	ror	al,1			;'carry' if plane is shared.
ifdef _286
	sbb	ax,ax		        ;0ffffh if plane is shared, 0 otherwise.
	mov	BitModifier,ax
	ror	byte ptr NotFg,1
	sbb	ax,ax
	mov	BitInverter,ax
else
	sbb	eax,eax		        ;0ffffffffh if plane is shared, 0 otherwise.
	mov	BitModifier,eax
	ror	byte ptr NotFg,1
	sbb	eax,eax
	mov	BitInverter,eax
endif
	mov	ah,ClipMask
	call	MBmpOp_C_ColorEntryPoint
	mov	ax,next_plane
	sub	di,ax
	sub	di,ax
	sub	di,ax
	ret
CBmpOp_Clipped	endp


;----------------------------------------------------------------------------;
; B I T M A P (color)
; TRANSPARENT: Not clipped
;----------------------------------------------------------------------------;
CBmpTr_NotClipped	proc	near
NLABEL  CBmpTr_NotClipped
	mov	al,num_planes
	dec	al
	mov	byte ptr nPlanes,al
	mov	ah,byte ptr colors[FOREGROUND]
	not	ah			;ah = not fg.
; 1st three times.
@@:		
	ror	ah,1
	push	ax
ifdef _286
	sbb	ax,ax
	mov	BitInverter,ax
else
	sbb	eax,eax
	mov	BitInverter,eax
endif
	push	di
	push	si
	push	cx
	call	MBmpTr_NC_ColorEntryPoint
	pop	cx
	pop	si
	pop	di
	add	di,next_plane
	mov	dx,bx
	pop	ax
	dec	byte ptr nPlanes
	jnz	@b

; Fourth time -- don't push and pop stuff.
	ror	ah,1
ifdef _286
	sbb	ax,ax
	mov	BitInverter,ax
else
	sbb	eax,eax
	mov	BitInverter,eax
endif
	call	MBmpTr_NC_ColorEntryPoint
	mov	ax,next_plane
	sub	di,ax
	sub	di,ax
	sub	di,ax
	ret
CBmpTr_NotClipped	endp

;----------------------------------------------------------------------------;
; B I T M A P (color)
; TRANSPARENT: clipped
;----------------------------------------------------------------------------;
CBmpTr_Clipped	proc	near
NLABEL	CBmpTr_Clipped
	mov	ClipMask,ah
	mov	al,num_planes
	dec	al
	mov	byte ptr nPlanes,al
	mov	ah,byte ptr colors[FOREGROUND]
	mov	al,byte ptr colors[BACKGROUND]
	not	ah			;ah = not fg.
; 1st three times.
@@:		
	ror	ah,1
	push	ax
ifdef _286
	sbb	ax,ax
	mov	BitInverter,ax
else
	sbb	eax,eax
	mov	BitInverter,eax
endif
	push	di
	push	si
	push	cx
	mov	ah,ClipMask
	call	MBmpTr_C_ColorEntryPoint
	pop	cx
	pop	si
	pop	di
	add	di,next_plane
	mov	dx,bx
	pop	ax
	dec	byte ptr nPlanes
	jnz	@b

; Fourth time -- don't push and pop stuff.
	ror	ah,1
ifdef _286
	sbb	ax,ax
	mov	BitInverter,ax
else
	sbb	eax,eax
	mov	BitInverter,eax
endif
	mov	ah,ClipMask
	call	MBmpTr_C_ColorEntryPoint
	mov	ax,next_plane
	sub	di,ax
	sub	di,ax
	sub	di,ax
	ret
CBmpTr_Clipped	endp

;----------------------------------------------------------------------------;
; B I T M A P (mono)
; OPAQUE: Not clipped
;----------------------------------------------------------------------------;
MBmpOp_NotClipped	proc	near
NLABEL	MBmpOp_NotClipped
	mov	ah,byte ptr colors[FOREGROUND]
	mov	al,byte ptr colors[BACKGROUND]
	not	ah			;ah = not fg.
	mov	NotFg,ah
	xor	al,ah			;al = 1's where fg/bg share a plane.
	ror	al,1			;'carry' if plane is shared.
ifdef _286
	sbb	ax,ax		        ;0ffffh if plane is shared, 0 otherwise.
	mov	BitModifier,ax
	ror	byte ptr NotFg,1
	sbb	ax,ax
	mov	BitInverter,ax
else
	sbb	eax,eax		        ;0ffffffffh if plane is shared, 0 otherwise.
	mov	BitModifier,eax
	ror	byte ptr NotFg,1
	sbb	eax,eax
	mov	BitInverter,eax
endif

PLABEL	MBmpOp_NC_ColorEntryPoint
	test	di,1			;Even destination boundary?
	jz	short MBmpOp_DestEven	;Yes.

PLABEL	MBmpOp_DestOdd
	call	MBmpOp_Byte1		;Do 1 byte column....
	mov	dx,bx			;Restore dx with height of image.

PLABEL	MBmpOp_DestEven
PLABEL	MBmpOp_Words
	push	cx
	shr	cx,1			;If cx is 1 or 0, then..
	jz	short MBmpOp_WordsExit   ; nothing to do.
	mov	bp,next_scan		;Cache next_scan in bp for faster access.
ifdef _286
	mov	ColumnsToDo,cx
else
	rol	ecx,16			;cache columns to do in upper word of ecx.
endif
	mov	Height,dx		;save off glyph height.
PLABEL	MBmpOp_WordsLoop
	push	di			;save ptr. destination screen word.
	mov	cx,Height		;Load glyph height.
ifdef _286
	test	cx,1
else
	test	cx,3
endif
	jnz	MBmpOp_AdjustHeight
PLABEL	MBmpOp_HeightAdjusted
@@:
ifdef _286
	mov	ax,[si]			;Get 2 words from 1st column.
	mov	dx,[si][bx]		;Get 2 words from 2nd column.
	or	ax,BitModifier
	or	dx,BitModifier
	xor	ax,BitInverter
	xor	dx,BitInverter
	xchg	ah,dl			;transpose (ax,dx ready to output).
	mov	es:[di],ax		;Write ax to the screen.
	add	di,bp			;point to next scan.
	mov	es:[di],dx		;write dx to the screen.
	add	di,bp			;point to next scan.
	add	si,2			;point to 2 bytes down the glyph column.
	sub	cx,2			;More to do?
else
	mov	eax,[si]		;Get 4 words from 1st column.
	mov	edx,[si][bx]		;Get 4 words from 2nd column.
	or	eax,BitModifier
	or	edx,BitModifier
	xor	eax,BitInverter
	xor	edx,BitInverter
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
	add	si,4			;point to 4 bytes down the glyph column.
	sub	cx,4			;More to do?
endif
	jg	@b			;yes.

PLABEL	MBmpOp_InnerLoopBottom
	add	si,bx			;no. Point si to next glyph column.
	pop	di			;get di back
	add	di,2			;point di to next screen word (along scan).
ifdef _286
	dec	word ptr ColumnsToDo	;All screen word columns processed?
else
	rol	ecx,16			;Get columns to do...
	dec	cx			;All screen word columns processed?
	rol	ecx,16			;put updated columns to do in upper word.
endif
	jnz	MBmpOp_WordsLoop	;no.
	mov	bp,SaveBp		;yes. Restore bp.
PLABEL	MBmpOp_WordsExit
	pop	cx
	and	cx,1			;If odd then do a final byte wide column.
	jz	short MBmpOp_Exit
PLABEL	MBmpOp_Byte1
	mov	dx,bx			;restore dx with height.
	push	di		
	mov	bp,next_scan
	push	cx
	mov	cx,BitInverter
@@:
	lodsb
	or	al,BitModifier
	xor	al,cl
	mov	es:[di],al
	add	di,bp
	dec	dx
	jnz	@b
	pop	cx
	mov	bp,SaveBp
	pop	di
	inc	di
	dec	cx
PLABEL	MBmpOp_Exit
	ret				;We're done.

PLABEL	MBmpOp_AdjustHeight
@@:	mov	al,[si]
	mov	ah,[si][bx]
	or	ax,BitModifier
	xor	ax,BitInverter
	mov	es:[di],ax
	add	di,bp
	inc	si
	dec	cx
ifndef _286
	test	cx,3
	jnz	@b
endif
	jcxz	MBmpOp_InnerLoopBottom
	jmp	MBmpOp_HeightAdjusted

MBmpOp_NotClipped	endp

;----------------------------------------------------------------------------;
; B I T M A P (mono)
; OPAQUE: clipped
;----------------------------------------------------------------------------;
MBmpOp_Clipped	proc	near
NLABEL	MBmpOp_Clipped
	push	ax
	mov	ah,byte ptr colors[FOREGROUND]
	mov	al,byte ptr colors[BACKGROUND]
	not	ah			;ah = not fg.
	mov	NotFg,ah
	xor	al,ah			;al = 1's where fg/bg share a plane.
	ror	al,1			;'carry' if plane is shared.
ifdef _286
	sbb	ax,ax		        ;0ffffh if plane is shared, 0 otherwise.
	mov	BitModifier,ax
	ror	byte ptr NotFg,1
	sbb	ax,ax
	mov	BitInverter,ax
else
	sbb	eax,eax		        ;0ffffffffh if plane is shared, 0 otherwise.
	mov	BitModifier,eax
	ror	byte ptr NotFg,1
	sbb	eax,eax
	mov	BitInverter,eax
endif
	pop	ax

PLABEL	MBmpOp_C_ColorEntryPoint
	push	di
	mov	cx,next_scan
PLABEL	MBmpOp_ClippedLoop
	mov	al,[si]	
	or	al,BitModifier
	xor	al,BitInverter
	
	xor	al,es:[di]		;al = src XOR dest
	and	al,ah			;ah = clip mask. 1=src, 0=dest
	xor	es:[di],al		;Merge src with dest based on clip mask.

	add	di,cx
	inc	si
	dec	dx
	jnz	MBmpOp_ClippedLoop
	pop	di
	inc	di
	ret
MBmpOp_Clipped	endp


;----------------------------------------------------------------------------;
; B I T M A P (mono)
; TRANSPARENT: Not clipped
;----------------------------------------------------------------------------;
MBmpTr_NotClipped	proc	near
NLABEL	MBmpTr_NotClipped
	mov	ah,byte ptr colors[FOREGROUND]
	not	ah			;ah = not fg.
	ror	ah,1
ifdef _286
	sbb	ax,ax
	mov	BitInverter,ax
else
	sbb	eax,eax
	mov	BitInverter,eax
endif

PLABEL	MBmpTr_NC_ColorEntryPoint
	test	di,1			;Even destination boundary?
	jz	short MBmpTr_DestEven	;Yes.

PLABEL	MBmpTr_DestOdd
	call	MBmpTr_Byte1		;Do 1 byte column....
	mov	dx,bx			;Restore dx with height of image.

PLABEL	MBmpTr_DestEven
PLABEL	MBmpTr_Words
	push	cx
	shr	cx,1			;If cx is 1 or 0, then..
	jz	short MBmpTr_WordsExit  ; nothing to do.
	mov	bp,next_scan		;Cache next_scan in bp for faster access.
ifdef _286
	mov	ColumnsToDo,cx
else
	rol	ecx,16			;cache columns to do in upper word of ecx.
endif
	mov	Height,dx		;save off glyph height.
PLABEL	MBmpTr_WordsLoop
	push	di			;save ptr. destination screen word.
	mov	cx,Height		;Load glyph height.
ifdef _286
	test	cx,1
else
	test	cx,3
endif
	jnz	MBmpTr_AdjustHeight
PLABEL	MBmpTr_HeightAdjusted
@@:
	push	cx
	mov	cx,word ptr BitInverter
ifdef _286
	mov	ax,[si]			;Get 2 words from 1st column.
	mov	dx,[si][bx]		;Get 2 words from 2nd column.
	xchg	ah,dl			;transpose (ax,dx ready to output).
	xor	es:[di],cx
	or	es:[di],ax		;Write ax to the screen.
	xor	es:[di],cx
	add	di,bp			;point to next scan.
	xor	es:[di],cx
	or	es:[di],dx		;Write dx to the screen.
	xor	es:[di],cx
	add	di,bp			;point to next scan.
	add	si,2			;point to 2 bytes down the glyph column.
	pop	cx
	sub	cx,2			;More to do?
else
	mov	eax,[si]		;Get 4 words from 1st column.
	mov	edx,[si][bx]		;Get 4 words from 2nd column.
	xchg	ah,dl			;transpose (ax,dx ready to output).
	xor	es:[di],cx
	or	es:[di],ax		;Write ax to the screen.
	xor	es:[di],cx
	add	di,bp			;point to next scan.
	xor	es:[di],cx
	or	es:[di],dx		;Write dx to the screen.
	xor	es:[di],cx

	rol	eax,16			;Prepare to transpose upper words.
	rol	edx,16			;Prepare to transpose upper words.
	xchg	ah,dl			;transpose (ax,dx ready to output).
	add	di,bp			;point to next scan.
	xor	es:[di],cx
	or	es:[di],ax		;Write ax to the screen.
	xor	es:[di],cx
	add	di,bp			;point to next scan.
	xor	es:[di],cx
	or	es:[di],dx		;Write dx to the screen.
	xor	es:[di],cx
	add	di,bp			;point to next scan.
	add	si,4			;point to 4 bytes down the glyph column.
	pop	cx
	sub	cx,4			;More to do?
endif
	jg	@b			;yes.

PLABEL	MBmpTr_InnerLoopBottom
	add	si,bx			;no. Point si to next glyph column.
	pop	di			;get di back
	add	di,2			;point di to next screen word (along scan).
ifdef _286
	dec	word ptr ColumnsToDo	;All screen word columns processed?
else
	rol	ecx,16			;Get columns to do...
	dec	cx		
	rol	ecx,16			;put updated columns to do in upper word.
endif
	jnz	MBmpTr_WordsLoop	;no.
	mov	bp,SaveBp		;yes. Restore bp.
PLABEL	MBmpTr_WordsExit
	pop	cx
	and	cx,1			;If odd then do a final byte wide column.
	jz	short MBmpTr_Exit
PLABEL	MBmpTr_Byte1
	mov	dx,bx			;restore dx with height.
	push	di		
	mov	bp,next_scan
	push	cx
	mov	cx,BitInverter
@@:
	lodsb
	xor	es:[di],cl
	or	es:[di],al		;Write al to the screen.
	xor	es:[di],cl
	add	di,bp
	dec	dx
	jnz	@b
	pop	cx
	mov	bp,SaveBp
	pop	di
	inc	di
	dec	cx
PLABEL	MBmpTr_Exit
	ret				;We're done.

PLABEL	MBmpTr_AdjustHeight
	push	dx
	mov	dx,BitInverter
@@:	mov	al,[si]
	mov	ah,[si][bx]
	xor	es:[di],dx
	or	es:[di],ax
	xor	es:[di],dx
	add	di,bp
	inc	si
	dec	cx
ifndef _286
	test	cx,3
	jnz	@b
endif
	pop	dx
	jcxz	MBmpTr_InnerLoopBottom
	jmp	MBmpTr_HeightAdjusted
MBmpTr_NotClipped	endp

;----------------------------------------------------------------------------;
; B I T M A P (mono)
; TRANSPARENT: clipped
;----------------------------------------------------------------------------;
MBmpTr_Clipped	proc	near
NLABEL	MBmpTr_Clipped
	push	ax
	mov	ah,byte ptr colors[FOREGROUND]
	not	ah			;ah = not fg.
	ror	ah,1
ifdef _286
	sbb	ax,ax
	mov	BitInverter,ax
else
	sbb	eax,eax
	mov	BitInverter,eax
endif
	pop	ax

PLABEL	MBmpTr_C_ColorEntryPoint
	push	di
	mov	bp,next_scan
	mov	cx,BitInverter
PLABEL	MBmpTr_ClippedLoop
	mov	al,[si]	
	and	al,ah			;ah = clip mask.
	xor	es:[di],cl
	or	es:[di],al
	xor	es:[di],cl
	add	di,bp
	inc	si
	dec	dx
	jnz	MBmpTr_ClippedLoop
	mov	bp,SaveBp
	pop	di
	inc	di
	ret
MBmpTr_Clipped	endp

;--------------------------------------------------------------------------;
; setup_ega_opaque_clip_magic
;
; Entry:
;	ah = clip mask.
; Returns:
;	none
; Error Returns:
;	none
; Registers Destroyed:
;	AX, DX
; Registers Preserved:
;	BX, CX, SI, DI, BP, DS, ES
; Calls:
;	none
; History:
;  Sat Apr 18, 1987 10:47:53p	-by-  Tony Pisculli	[tonyp]
; wrote it
;--------------------------------------------------------------------------;

setup_ega_opaque_clip_magic	proc near
NLABEL	setup_ega_opaque_clip_magic

	mov	dx,EGA_BASE + GRAF_ADDR
	mov	al,GRAF_BIT_MASK
	out	dx,ax

	mov	ax,DR_SET shl 8 + GRAF_DATA_ROT
	out	dx,ax

	mov	ah,bptr colors[FOREGROUND]
	mov	al,GRAF_SET_RESET
	out	dx,ax

	xor	ah,bptr colors[BACKGROUND]
	not	ah
	mov	al,GRAF_ENAB_SR
	out	dx,ax

	mov	dx,EGA_BASE + SEQ_DATA
	or	ah,bptr colors[FOREGROUND]
	and	ah,0fh
	jz	short @f
	mov	al,ah
	out	dx,al
	clc
	ret
@@:
	stc		
	ret
setup_ega_opaque_clip_magic	endp

;---------------------------Public-Routine------------------------------;
;
; set_ega_opaque_mode
;
;   The ega is programmed for entire byte opaquing
;
; Entry:
;	None
; Returns:
;	none
; Error Returns:
;	none
; Registers Destroyed:
;	AX, DX
; Registers Preserved:
;	BX,CX,SI,DI,DS,ES
; Calls:
;
; History:
;  Wed Mar 04, 1987 04:32:46a	-by-  Tony Pisculli	[tonyp]
; wrote it
;-----------------------------------------------------------------------;
	assumes ds,nothing
	assumes es,EGAMem

ifdef _286
	public	set_ega_opaque_mode_286
	set_ega_opaque_mode_286	proc near
else
	public	set_ega_opaque_mode_386
	set_ega_opaque_mode_386	proc near
endif
	mov	dx,EGA_BASE + SEQ_DATA
	mov	al,MM_ALL
	out	dx,al
	mov	dl,GRAF_ADDR
	mov	ax,0FFh shl 8 + GRAF_BIT_MASK
	out	dx,ax
	mov	ax,colors
	xor	ah,al
	mov	al,GRAF_SET_RESET
	out	dx,ax
	not	ah
	mov	al,GRAF_ENAB_SR
	out	dx,ax
	mov	ax,DR_XOR shl 8 + GRAF_DATA_ROT
	out	dx,ax
	mov	al,tonys_bar_n_grill
	ret

ifdef _286
set_ega_opaque_mode_286	endp
else
set_ega_opaque_mode_386	endp
endif

if WM3_DETECT
;----------------------------------------------------------------------------;
; D E V I C E
; TRANSPARENT: Not clipped (NO WRITE MODE 3 support)
;----------------------------------------------------------------------------;
DevTrNo3_NotClipped	proc	near
NLABEL DevTrNo3_NotClipped
	test	di,1			;Even destination boundary?
	jz	short DevTrNo3_DestEven	;Yes.

PLABEL	DevTrNo3_DestOdd
	call	DevTrNo3_Byte1		;Do 1 byte column....
	mov	dx,bx			;Restore dx with height of image.

PLABEL	DevTrNo3_DestEven
PLABEL	DevTrNo3_Words
	push	cx
	shr	cx,1			;If cx is 1 or 0, then..
	jz	DevTrNo3_WordsExit   	; nothing to do.
	mov	bp,next_scan		;Cache next_scan in bp for faster access.
ifdef _286
	mov	ColumnsToDo,cx
else
	rol	ecx,16			;cache columns to do in upper word of ecx.
endif
	mov	Height,dx		;save off glyph height.
PLABEL	DevTrNo3_WordsLoop
	push	di			;save ptr. destination screen word.
	mov	cx,Height		;Load glyph height.
ifdef _286
	test	cx,1
else
	test	cx,3
endif
	jnz	DevTrNo3_AdjustHeight
PLABEL	DevTrNo3_HeightAdjusted
ifdef _286
	mov	ax,[si]			;Get 2 words from 1st column.
	mov	dx,[si][bx]		;Get 2 words from 2nd column.
else
	mov	eax,[si]		;Get 4 words from 1st column.
	mov	edx,[si][bx]		;Get 4 words from 2nd column.
endif
	xchg	ah,dl			;transpose (ax,dx ready to output).
	or	ax,ax			;If empty, don't write to the screen.
	jz	short @f
	push	dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	xchg	es:[di],al		;write ax to screen.
	mov	al,ah
	out	dx,al
	xchg	es:[di+1],ah		;write ax to screen.
	pop	dx
@@:
	add	di,bp			;point to next scan.
	or	dx,dx			;If empty, don't write to the screen.
	jz	short @f
	mov	ax,dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	xchg	es:[di],al		;write ax to screen.
	mov	al,ah
	out	dx,al
	xchg	es:[di+1],ah		;write ax to screen.

@@:
	add	di,bp			;point to next scan.
ifndef _286
	rol	eax,16			;Prepare to transpose upper words.
	rol	edx,16			;Prepare to transpose upper words.
	xchg	ah,dl			;transpose (ax,dx ready to output).
	or	ax,ax			;If empty, don't write to the screen.
	jz	short @f
	push	dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	xchg	es:[di],al		;write ax to screen.
	mov	al,ah
	out	dx,al
	xchg	es:[di+1],ah		;write ax to screen.
	pop	dx
@@:
	add	di,bp			;point to next scan.
	or	dx,dx			;If empty, don't write to the screen.
	jz	short @f
	mov	ax,dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	xchg	es:[di],al		;write ax to screen.
	mov	al,ah
	out	dx,al
	xchg	es:[di+1],ah		;write ax to screen.
@@:
	add	di,bp			;point to next scan.
endif

ifdef _286
	add	si,2			;point to 2 bytes down the glyph column.
	sub	cx,2			;More to do?
else
	add	si,4			;point to 4 bytes down the glyph column.
	sub	cx,4			;More to do?
endif
	jg	DevTrNo3_HeightAdjusted	;yes.

PLABEL	DevTrNo3_InnerLoopBottom
	add	si,bx			;no. Point si to next glyph column.
	pop	di			;get di back
	add	di,2			;point di to next screen word (along scan).
ifdef _286
	dec	word ptr ColumnsToDo	;All screen word columns processed?
else
	rol	ecx,16			;Get columns to do...
	dec	cx			;All screen word columns processed?
	rol	ecx,16			;put updated columns to do in upper word.
endif
	jnz	DevTrNo3_WordsLoop		;no.
	mov	bp,SaveBp		;yes. Restore bp.
PLABEL	DevTrNo3_WordsExit
	pop	cx
	and	cx,1			;If odd then do a final byte wide column.
	jz	short DevTrNo3_Exit
PLABEL	DevTrNo3_Byte1
	mov	dx,bx			;restore dx with height.
	push	di		
	mov	bp,next_scan
PLABEL	DevTrNo3_Byte1Loop
	lodsb
	or	al,al
	jz	short @f
	push	dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	xchg	es:[di],al		;write ax to screen.
	pop	dx
@@:	add	di,bp
	dec	dx
	jnz	DevTrNo3_Byte1Loop
	mov	bp,SaveBp
	pop	di
	inc	di
	dec	cx
PLABEL	DevTrNo3_Exit
	ret				;We're done.

PLABEL	DevTrNo3_AdjustHeight
	mov	al,[si]
	mov	ah,[si][bx]
	or	ax,ax
	jz	short @f
	push	dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	xchg	es:[di],al
	mov	al,ah
	out	dx,al
	xchg	es:[di+1],ah
	pop	dx
@@:	add	di,bp
	inc	si
	dec	cx
ifndef _286
	test	cx,3
	jnz	DevTrNo3_AdjustHeight
endif
	jcxz	DevTrNo3_InnerLoopBottom
	jmp	DevTrNo3_HeightAdjusted
DevTrNo3_NotClipped	endp

;----------------------------------------------------------------------------;
; D E V I C E
; TRANSPARENT: clipped (NO WRITE MODE 3 support)
;----------------------------------------------------------------------------;
DevTrNo3_Clipped	proc	near
NLABEL DevTrNo3_Clipped
	push	di
	mov	cx,next_scan
PLABEL	DevTrNo3_ClippedLoop
	mov	al,[si]	
	and	al,ah			;ah = clip mask.
	jz	short @f
	push	dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	xchg	es:[di],al
	pop	dx
@@:	add	di,cx
	inc	si
	dec	dx
	jnz	DevTrNo3_ClippedLoop
	pop	di
	inc	di
	ret
DevTrNo3_Clipped	endp
endif

ifdef _286
sEnd	rCode
else
sEnd	pCode
endif
	end
