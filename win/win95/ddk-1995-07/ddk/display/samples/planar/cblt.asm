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
;----------------------------Module-Header------------------------------;
; Module Name: CBLT.ASM
;
; Subroutine to compile a BLT subroutine onto the stack.
;
; This file contains two subroutines which build a small program on the
; stack to accomplish the requested BLT.
;
; This file is part of a set that makes up the Windows BitBLT function
; at driver-level.
;-----------------------------------------------------------------------;

FIREWALLS = 0
THIS_IS_DOS_3_STUFF = 1		;remove this line for WinThorn
	.xlist
ifdef	TEFTI
	include TEFTI.INC
endif
	include CMACROS.INC
	include MACROS.INC
incLogical	= 1		;Include GDI logical object definitions
incDrawMode	= 1		;Include GDI DrawMode definitions
	include GDIDEFS.INC
	include DISPLAY.INC
	include	MFLAGS.INC
ifdef	GEN_COLOR_BLT
	include EGA.INC
endif
	include EGAMEM.INC
	include NJMP.INC
	.list


ifdef	THIS_IS_DOS_3_STUFF
	externA ScreenSelector		; Segment of Regen RAM
endif
ifdef	GEN_COLOR_BLT
	externA COLOR_DONT_CARE		; for use with GRAF_CDC register
endif
	public	cblt_3100
sBegin	Code
	assumes cs,Code
	assumes ds,Code
	assumes es,nothing


	.xlist

;	Following are the BitBLT include-files.  Some are commented out
;	because they contain address definitions are are included in
;	BITBLT.ASM, but are listed here for completeness.  The remaining
;	files include those that make up the local variable frame, and 
;	those containing subroutines.  The frame-variable files are
;	included immediately after the cProc CBLT declaration.  The
;	subroutines files are not included in CBLT.ASM.

	include	GENCONST.INC		;EQUs
	include	CLRCONST.INC		;EQUs
	include	DEVCONST.INC		;EQUs
;	include	GENDATA.INC		;bitmask and phase tables
;	include	CLRDATA.INC		;color/mono templates,data
	include	DEVDATA.INC		;driver-specific templates,data
	include	ROPDEFS.INC		;ROP definitions
;	include	ROPTABLE.INC		;Raster operation code templates
ifdef	THIS_IS_DOS_3_STUFF
else
;	include	FIREWALL.INC
;	include	DDC.INC
endif

	externW	roptable
	.list

	page
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	jmp_cx_nz   - Code template for near jump if CX-1 <> 0
;
;	jmp_cx_nz will skip the following near jump if CX-1 is zero.
;	CX will be left updated by this code.
;
;	jmp_cx_nz is used by both the inner loop code and the outer
;	loop code if a loop instruction cannot be used.
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

jmp_cx_nz:
	dec	cx			;Decrement counter
	jz	$+5
;	jmp	near label
	db	I_JMP_NEAR		;JMP opcode

JMP_CX_NZ_LEN	equ	$-jmp_cx_nz	;Length of procedure


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	phase_align - Template for phase alignment code
;
;	The following code is the template that performs the phase
;	alignment masking.  The source has already been aligned to
;	the destination.
;
;	A copy of the aligned source is made.  The phase mask is then
;	applied to the source and the copy.  The previously unused
;	bits are ORed into the used bits of the current source, and
;	the unused bits of the current source then become the unused
;	bits for the next source.
;
;
;	It assumes:
;
;		BP  =  phase alignment mask
;		AL  =  current byte to mask
;		BH  =  old unused bits
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

phase_align:
	mov	ah,al			;Make a copy of aligned source
	and	ax,bp			;Masked used, unused bits
	or	al,bh			;Mask in old unused bits
	mov	bh,ah			;Save new unused bits

PHASE_ALIGN_LEN	equ	$-phase_align	;Length of procedure

	page
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	masked_store - Template for storing first and last bytes of BLT
;
;	The following code is a template for storing the first and last
;	bytes of a BLT.  The unaltered bits are saved and the altered
;	bits set in the byte, then the byte is stored.
;
;
;	It assumes:
;
;		AL  =  The byte to be BLTed to the destination bitmap.
;		       All necessary logic operations have been performed
;		       on this byte.
;
;		AH  =  The destination byte.
;
;	The AND immediate will be fixed up.
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

masked_store:
	and	ax,01234h		;Mask altered/unaltered bits
	or	al,ah			;Combine the bits
	stosb				;And store the result

MASKED_STORE_LEN	equ	$-masked_store	;Length of the template
MASKED_STORE_MASK	equ	-5		;Offset to where mask goes


	page
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	Pattern Fetch Code
;
;	The pattern fetch code will be created on the fly since
;	most of the instructions need fixups.
;
;	This template is really just a comment to indicate what
;	the generated code should look like.
;
;	Entry:	None
;
;	Exit:	DH = pattern
;
;	Uses:	AX,BX,CX,DH,flags
;
;
;	The following registers are available to the pattern fetch
;	logic (as implemented herein):
;
;		AX,BX,CX,DX,flags
;
;
;	For monochrome brushes:
;
;	    mov     ax,1234h		;Load segment of the brush
;	    mov     bx,1234h		;Load offset of the brush
;	    mov     cx,ds		;Save DS
;	    mov     ds,ax		;DS:BX --> brush
;	    mov     dh,7[bx]		;Get next brush byte
;	    mov     al,ss:[1234h]	;Get brush index
;	    add     al,gl_direction	;Add displacement to next byte (+1/-1)
;	    and     al,00000111b	;Keep it in range
;	    mov     ss:[1234h],al	;Store displacement to next byte
;	    mov     ds,cx		;Restore DS
;
;
ifdef	GEN_COLOR_BLT
;	For color brushes:
;
;	    mov     ax,1234h		;Load segment of the brush
;	    mov     bx,1234h		;Load offset of the brush
;	    mov     cx,ds		;Save DS
;	    mov     ds,ax		;DS:BX --> brush
;	    mov     dh,7[bx]		;Get next brush byte
;	    mov     al,ss:[1234h]	;Get brush index
;	    add     al,SIZE Pattern	;Add disp. to next plane's bits
;	    and     al,00011111b	;Keep it within the brush
;	    mov     ss:[1234h],al	;Store disp. to next plane's bits
;	    mov     ds,cx		;Restore DS
;
endif
;
;	For both templates, SS:[1234] is the address of the 7 in the
;	"mov dh,7[bx]" instruction.  This is the index to this scan's
;	bit pattern in the brush.  This value will range from 0 to
;	(SIZE pattern)-1 for monochrome devices, and from 0 to
;	((NumberPlanes)*(SIZE pattern))-1 for color devices.
;
ifdef	GEN_COLOR_BLT
;	For color brushes, SS:[1234] must also be fixed up when the next
;	scan line is selected, else it would index into the monochrome
;	portion of the brush (e.g. 1,9,17,25, where 25 is not part of the
;	color brush).
endif
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

	page
;-----------------------------Public-Routine----------------------------;
; CBLT
;
; Compile a BLT onto the stack.
;
; Entry:
;	ES:DI --> memory on stack to receive BLT program
; Returns:
;	Nothing
; Registers Preserved:
; Registers Destroyed:
; Calls:
;	y_update
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

;	Note:	The definition of CBLT below is FAR in order to maintain
;		the stack frame created for BITBLT, though it is reached
;		with a NEAR call.  The files GENLOCAL.INC, CLRLOCAL.INC,
;		and DEVLOCAL.INC define the parameters and local variable
;		names.

cProc	CBLT,<FAR,PUBLIC>,<>

	.xlist
	include	GENLOCAL.INC		;params and generic locals
	include	CLRLOCAL.INC		;color/monochrome-related locals
	include	DEVLOCAL.INC		;device-related locals
	.list

cBegin	nogen



;	Initialize plane indicator.

ifdef	GEN_COLOR_BLT
	mov	ax,(PLANE_1*256)+I_MOV_BL_BYTE_I
	stosw
endif

	subttl	Compile - Outer Loop
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	Create the outerloop code.  The first part of this code will save
;	the scan line count register, destination pointer, and the source
;	pointer (if there is a source).
;
;
;	The generated code should look like:
;
;		push	cx		;Save scan line count
;		push	di		;Save destination pointer
;	<	push	si	>	;Save source pointer
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

	mov	bl,gl_the_flags
	mov	ax,I_PUSH_CX_PUSH_DI	;Save scan line count, destination ptr
	stosw
	test	bl,F0_SRC_PRESENT	;Is a source needed?
	jz	cblt_2020		;  No
	mov	al,I_PUSH_SI		;  Yes, save source pointer
	stosb

cblt_2020:



ifdef	GEN_COLOR_BLT
	subttl	Compile - Plane Selection
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	If the destination device is color and the display is involved in
;	the blt, then the color plane selection logic must be added in.
;	If the destination is monochrome, then no plane logic is needed.
;	Two color memory bitmaps will not cause the plane selection logic
;	to be copied.
;
;
;	The generated code should look like:
;
;	<	push	bx	>	;Save plane index
;	<	plane selection >	;Select plane
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;


	test	bl,F0_DEST_IS_COLOR	;Is the destination color?
	jz	cblt_pattern_fetch	;  No
	mov	al,I_PUSH_BX		;Save plane index
	stosb
	test	bl,F0_DEST_IS_DEV+F0_SRC_IS_DEV	;Is the device involved?
	jz	cblt_pattern_fetch		;  No


;	The device is involved for a color blt.  Copy the logic for selecting
;	the read/write plane, and perform any fixups that are needed.

	mov	si,CodeOFFSET cps	;--> plane select logic
	mov	cx,LENGTH_CPS/2
	rep	movs wptr es:[di],wptr cs:[si]
if	LENGTH_CPS AND 1
	movs	bptr es:[di],bptr cs:[si]
endif
endif	;GEN_COLOR_BLT



	subttl	Compile - Pattern Fetch
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	Set up any pattern fetch code that might be needed.
;	The pattern code has many fixups, so it isn't taken from a
;	template.  It is just stuffed as it is created.
;
;
;	Entry:	None
;
;	Exit:	DH = pattern
;
;	Uses:	AX,BX,CX,DH,flags
;
;
;	For monochrome brushes:
;
;  	    mov     ax,1234h		;Load segment of the brush
;	    mov     bx,1234h		;Load offset of the brush
;  	    mov     cx,ds		;Save DS
;  	    mov     ds,ax		;DS:BX --> brush
;	    mov     dh,7[bx]		;Get next brush byte
;	    mov     al,ss:[1234h]	;Get brush index
;	    add     al,gl_direction	;Add displacement to next byte (+1/-1)
;	    and     al,00000111b	;Keep it in range
;	    mov     ss:[1234h],al	;Store displacement to next byte
;  	    mov     ds,cx		;Restore DS
;
;
ifdef	GEN_COLOR_BLT
;	For color brushes:
;
;  	    mov     ax,1234h		;Load segment of the brush
;	    mov     bx,1234h		;Load offset of the brush
;  	    mov     cx,ds		;Save DS
;  	    mov     ds,ax		;DS:BX --> brush
;	    mov     dh,7[bx]		;Get next brush byte
;	    mov     al,ss:[1234h]	;Get brush index
;	    add     al,SIZE Pattern	;Add displacement to next plane's bits
;	    and     al,00011111b	;Keep it within the brush
;	    mov     ss:[1234h],al	;Store displacement to next plane's bits
;  	    mov     ds,cx		;Restore DS
;
;	    The address of the increment for the brush is saved for
;	    the plane looping logic if the destination is a three plane
;	    color device.  For a four plane color device, the AND
;	    automatically handles the wrap and no fixup is needed at
;	    the end of the plane loop.
endif
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

cblt_pattern_fetch:
	test	bl,F0_PAT_PRESENT	;Is a pattern needed?
	jz	cblt_initial_byte_fetch ;  No, skip pattern code
	mov	al,I_MOV_AX_WORD_I	;mov ax,SEG_lpPBrush
	stosb
	mov	ax,seg_lpPBrush
	stosw
	mov	al,I_MOV_BX_WORD_I 	;mov bx,OFF_lpPBrush
	stosb
	mov	ax,off_lpPBrush
	stosw
	mov	ax,I_MOV_CX_DS		;mov cx,ds
	stosw
	mov	ax,I_MOV_DS_AX		;mov ds,ax
	stosw
	mov	ax,I_MOV_DH_BX_DISP8	;mov dh,gl_pat_row[bx]
	stosw
	mov	dx,di			;Save address of the brush index
	mov	al,gl_pat_row		;Set initial pattern row
	mov	bh,00000111b		;Set brush index mask
	and	al,bh			;Make sure it's legal at start
	stosb
	mov	ax,I_SS_OVERRIDE+(I_MOV_AL_MEM*256)
	stosw				;mov al,ss:[xxxx]
	mov	ax,dx
	stosw
	mov	al,I_ADD_AL_BYTE_I
	mov	ah,gl_direction		;Set brush index
	errnz	INCREASE-1		;Must be a 1
	errnz	DECREASE+1		;Must be a -1

ifdef	GEN_COLOR_BLT
	test	bl,F0_COLOR_PAT		;Color pattern required?
	jz	cblt_2060		;  No

ifndef	FOUR_PLANE
	mov	dl_addr_brush_index,dx	;Save address of brush index
endif

	mov	ah,SIZE_PATTERN		;Set increment to next plane
	mov	bh,00011111b		;Set brush index mask

cblt_2060:
endif
	stosw
	mov	ah,bh			;and al,BrushIndexMask
	mov	al,I_AND_AL_BYTE_I
	stosw
	mov	ax,I_SS_OVERRIDE+(I_MOV_MEM_AL*256)
	stosw				;mov ss:[xxxx],al
	mov	ax,dx
	stosw
	mov	ax,I_MOV_DS_CX		;mov ds,cx
	stosw

	subttl	Compile - Initial Byte Fetch
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	Create the initial byte code.  This may consist of one or two
;	initial fetches (if there is a source), followed by the required
;	logic action.  The code should look something like:
;
;	BLTouterloop:
;	<	mov	bp,gl_mask_p >  ;Load phase mask for entire loop
;	<	xor	bh,bh	    >	;Clear previous unused bits
;
;	;	Perform first byte fetch
;
;	<	lodsb		    >	;Get source byte
ifdef	GEN_COLOR_BLT
;	<	color<==>mono munge >	;Color <==> mono conversion
endif
;	<	phase alignment     >	;Align bits as needed
;
;	;	If an optional second fetch is needed, perform one
;
;	<	lodsb		    >	;Get source byte
ifdef	GEN_COLOR_BLT
;	<	color to mono munge >	;Color to mono munging
endif
;	<	phase alignment     >	;Align bits as needed
;
;		logical action		;Perform logical action required
;
;		mov	ah,es:[di]	;Get destination
;		and	ax,cx		;Saved unaltered bits
;		or	al,ah		;  and mask in altered bits
;		stosb			;Save the result
;
;
;	The starting address of the first fetch/logical combination will be
;	saved so that the code can be copied later instead of recreating it
;	(if there are two fecthes, the first fetch will not be copied)
;
;	The length of the code up to the masking for altered/unaltered bits
;	will be saved so the code can be copied into the inner loop.
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;


cblt_initial_byte_fetch:
	xor	dx,dx
	or	dh,gl_phase_h		;Is the phase 0? (also get the phase)
	jz	cblt_3020		;  Yes, so no phase alignment needed
	mov	al,I_MOV_BP_WORD_I 	;Set up the phase mask
	stosb
	mov	ax,gl_mask_p		;Place the mask into the instruction
	stosw
	mov	ax,I_XOR_BH_BH		;Clear previous unused bits
	stosw

cblt_3020:
	mov	gl_start_fl,di		;Save starting address of action
	test	gl_the_flags,F0_SRC_PRESENT ;Is there a source?
	jnz	cblt_3040		;  Yes, generate fetch code
	jmp	cblt_4000		;  No, don't generate fetch code



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	Generate the required sequence of instructions for a fetch
;	sequence.  Only the minimum code required is generated.
;
;	The code generated will look something like the following:
;
;	BLTfetch:
;	<	lodsb		      > ;Get the next byte
ifdef	GEN_COLOR_BLT
;	<	color munging	      > ;Mono <==> color munging
endif
;
;	;	If the phase alignment isn't zero, then generate the minimum
;	;	phase alignment needed.  RORs or ROLs will be generated,
;	;	depending on the fastest sequence.  If the phase alignment
;	;	is zero, than no phase alignment code will be generated.
;
;	<	ror	al,1	      > ;Rotate as needed
;	<	ror	al,1	      > ;Rotate as needed
;	<	ror	al,1	      > ;Rotate as needed
;	<	ror	al,1	      > ;Rotate as needed
;	<	mov	ah,al	      > ;Mask used, unused bits
;	<	and	ax,bp	      > ;(BP) = phase mask
;	<	or	al,bh	      > ;Mask in old unused bits
;	<	mov	bh,ah	      > ;Save new unused bits
;
;
;	The nice thing about the above is it is possible for the fetch to
;	degenerate into a simple LODSB instruction.
;
;	If this was a iAPX80286 implementation, if would be faster to
;	make three or four rotates into a "ror al,n" instruction.
;
;	Currently:	BL = gl_the_flags
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;


cblt_3040:
ifdef	GEN_COLOR_BLT
	shl	bl,1			;Color conversion?
	jc	cblt_3060		;  Yes, gag and choke on it
	jmp	cblt_3180		;  No, we were lucky this time
	errnz	F0_GAG_CHOKE-10000000b

cblt_3060:
	mov	dl_moore_flags,0	;Assume REP cannot be used

	assumes ds,nothing

ifdef THIS_IS_DOS_3_STUFF
	lds	si,lpDrawMode		;--> background color
	lea	si,bkColor[si]		;  (lea preserves the flags)
else

; Here we use the knowledge that our brush is ddc_oem_brush (in DDC!).

	lds	si,lpPBrush		;--> background color
	ddc_check	<si - ddc_oem_brush>
	lea	si,[si][ddc_image_back_color_ours - ddc_oem_brush] ; preserves flags
endif
	njs	cblt_3100		;Mono ==> color
	errnz	F0_COLOR_PAT-01000000b



	subttl	Compile - Initial Byte Fetch, Color ==> Mono
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	Generate the code to go from color to mono.  Color to mono
;	should map all colors that are background to 1's (white), and
;	all colors which aren't background to 0's (black).  If the source
;	is the display, then the color compare register will be used.
;	If the source is a memory bitmap, each byte of the plane will be
;	XORed with the color from that plane, with the results all ORed
;	together.  The final result will then be complemented, giving
;	the desired result.
;
;	The generated code for bitmaps should look something like:
;
;	    mov     al,plane_w[si]	;Get C1 byte of source
;	    mov     ah,2*plane_w[si]	;Get C2 byte of source
;	    xor     ax,C1BkColor+(C2BkColor*256) ;XOR with plane's color
;	    or	    ah,al		;OR the result
;	if NUMBER_PLANES eq 4
;	    mov	    al,3*plane_w[si]	;Get C3 byte of source
;	    xor	    al,C3BkColor
;	    or	    ah,al
;	endif
;	    lodsb			;Get C0 source
;	    xor     al,C0BkColor 	;XOR with C0BkColor
;	    or	    al,ah		;OR with previous result
;	    not     al			;NOT to give 1's where background
;
;
;	    where plane_w is defined to be:
;
;    		a)  bmWidthPlanes for bitmaps <64K
;		b)  bmWidthBytes  for bitmaps >64K
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;


cblt_3070:
	test	bl,F0_SRC_IS_DEV SHL 1	;If device, use color compare register
	jz	cblt_3080		;It's a memory bitmap



;	We're in luck, the color compare register can be used.  Set up
;	for a color read, and use the normal mono fetch code.  Show the
;	innerloop code that the REP instruction can be used if this is
;	a source copy.


	mov	dl_moore_flags,F1_REP_OK;Show rep instructions ok in loop
	mov	cx,dx			;Save dx
	mov	ah,[si].SPECIAL		;Get SPECIAL byte of color
	and	ah,MM_ALL

	mov	al,GRAF_COL_COMP	;Stuff color into compare register
	mov	dx,EGA_BASE+GRAF_ADDR
	out	dx,ax
	mov	ax,COLOR_DONT_CARE	;Set Color Don't Care register
	out16	dx,ax

	push	ds
	mov	ax,ScreenSelector	;Show color read mode to the
	mov	ds,ax			;  EGA restoration code
	assumes ds,EGAMem

	mov	ax,M_COLOR_READ SHL 8 + GRAF_MODE
if MASMFLAGS and EGA
	mov	shadowed_mode,ah	    ;Must shadow this for state code
endif
	out	dx,ax
	pop	ds
	assumes ds,nothing

	mov	dx,cx
	jmp	cblt_3170		;Go generate mono fetch code


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	The source is a memory bitmap.	Generate the code to compute
;	the result of the three (or four!!!) planes:
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

cblt_3080:
	mov	ax,I_MOV_AL_SI_DISP16
	stosw
	mov	ax,gl_src.plane_w
	stosw
	xchg	bx,ax			; xchg is one byte less than mov
	add	bx,bx
	mov	ax,I_MOV_AH_SI_DISP16
	stosw
	mov	ax,bx
	stosw
	mov	al,I_XOR_AX_WORD_I
	stosb
	mov	al,[si].SPECIAL		;get the color index byte
	mov	ah,al			;have the same in AH
	and	ax,(C2_BIT shl 8) or C1_BIT
	neg	al
	sbb	al,al			;al will be 0ffh if plane bit is 1
	neg	ah
	sbb	ah,ah			;ah wil be 0ffh if plane bit is 1
	stosw
	mov	ax,I_OR_AH_AL
	stosw
if NUMBER_PLANES eq 4
;	    mov	    al,3*plane_w[si]	;Get C3 byte of source
;	    xor	    al,C3BkColor
;	    or	    ah,al
	mov	ax,I_MOV_AL_SI_DISP16
	stosw
	mov	ax,gl_src.plane_w
	add	ax,bx
	stosw
	mov	al,I_XOR_AL_BYTE_I
	mov	ah,[si].SPECIAL
	and	ah,C3_BIT
	neg	ah
	sbb	ah,ah
	stosw
	mov	ax,I_OR_AH_AL
	stosw
endif
	mov	ax,I_LODSB+(I_XOR_AL_BYTE_I*256)
	stosw
	mov	al,[si].SPECIAL		;get index byte
	shr	al,1			;get C0_BIT into carry
	sbb	al,al			;make it 0ffh if bit was set
	.errnz C0_BIT - 00000001b
	stosb				;save the modified value
	errnz	pcol_C0
	mov	ax,I_OR_AL_AH
	stosw
	mov	ax,I_NOT_AL
	stosw
	jmp	short cblt_3160		;Go create logic code

	subttl	Compile - Initial Byte Fetch, Mono ==> Color
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	The conversion is mono to color.  Generate the code to
;	do the conversion, and generate the table which will
;	have the conversion values in it.
;
;	When going from mono to color, 1 bits are considered to be
;	the background color, and 0 bits are considered to be the
;	foreground color.
;
;	For each plane:
;
;	  If the foreground=background=1, then 1 can be used in
;	  place of the source.
;
;	  If the foreground=background=0, then 0 can be used in
;	  place of the source.
;
;	  If the foreground=0 and background=1, then the source
;	  can be used as is.
;
;	  If the foreground=1 and background=0, then the source
;	  must be complemented before using.
;
;	  Looks like a boolean function to me.
;
;	An AND mask and an XOR mask will be computed for each plane,
;	based on the above.  The source will then be processed against
;	the table.  The generated code should look like
;
;		lodsb
;		and	al,ss:[xxxx]
;		xor	al,ss:[xxxx+1]
;
;
;	The table for munging the colors as stated above should look like:
;
;	     BackGnd   ForeGnd	  Result    AND  XOR
;		1	  1	    1	     00   FF
;		0	  0	    0	     00   00
;		1	  0	    S	     FF   00
;		0	  1	not S	     FF   FF
;
;	From this, it can be seen that the XOR mask is the same as the
;	foreground color.  The AND mask is the XOR of the foreground
;	and the background color.  Not too hard to compute
;
;
;	It can also be seen that if the background color is white and the
;	foreground (text) color is black, then the conversion needn't be
;	generated (it just gives the source).  This is advantageous since
;	it will allow phased aligned source copies to use REP MOVSW.
;
;
;	Currently:	ds:si --> bkColor in lpDrawMode
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

	errnz	TextColor-bkColor-4	;Must be contiguous

ifdef THIS_IS_DOS_3_STUFF
DIFF4	equ	(TextColor - bkColor)
else
DIFF4	equ	(ddc_image_color_ours - ddc_image_back_color_ours)
	.errnz	ddc_image_back_color_ours - ddc_image_color_ours - 8 ; needed?
endif

;++++++++++++++++++++++++++++++++++
;	Check to see if the background color is black, and the
;	foreground color is white.  This can be determined by
;	looking at the accelerator flags in the physical color.

cblt_3100:
	mov	ah,bptr [si][DIFF4].SPECIAL
	xor	ah,MONO_BIT		;Map black to white
	and	ah,[si].SPECIAL 	;AND in forground color
	cmp	ah,MONO_BIT+ONES_OR_ZEROS
	jne	cblt_3110		;Not black
	mov	dl_moore_flags,F1_REP_OK+F1_NO_MUNGE;Show reps as ok, no color munge table
	jmp	short cblt_3170		;Normal fetch required

;	No way around it.  The color conversion table and code
;	must be generated.

cblt_3110:
	.errnz	TextColor - bkColor - 4

;ifdef THIS_IS_DOS_3_STUFF
if 0
	mov	ah,DIFF4[si]		;Get foreground Red color (00 or FF)
	lodsb				;Get background Red color (00 or FF)
	xor	al,ah
	mov	wptr cl_a_brush.(pcol_C0 * 2),ax
	mov	ah,DIFF4[si]		;Get foreground Green color
	lodsb				;Get background Green color
	xor	al,ah
	mov	wptr cl_a_brush.(pcol_C1 * 2),ax
	mov	ah,DIFF4[si]		;Get foreground Blue color
	lodsb				;Get background Blue color
	xor	al,ah
	mov	wptr cl_a_brush.(pcol_C2 * 2),ax
else
	mov	cl,[si].SPECIAL		; Get BackGround Colors
	mov	ch,DIFF4[si].SPECIAL	; Get ForeGround Colors
	xor	cl,ch			      
	shr	cl,1
	sbb	al,al
	shr	ch,1
	sbb	ah,ah
	mov	wptr cl_a_brush.(pcol_C0 * 2),ax
	shr	cl,1
	sbb	al,al
	shr	ch,1
	sbb	ah,ah
	mov	wptr cl_a_brush.(pcol_C1 * 2),ax
	shr	cl,1
	sbb	al,al
	shr	ch,1
	sbb	ah,ah
	mov	wptr cl_a_brush.(pcol_C2 * 2),ax
if NUMBER_PLANES eq 4
	shr	cl,1
	sbb	al,al
	shr	ch,1
	sbb	ah,ah
	mov	wptr cl_a_brush.(pcol_C3 * 2),ax
endif
endif
	errnz	<TextColor - bkColor - 4>


;	Generate the code for munging the color as stated above.

	mov	ax,I_LODSB+(I_SS_OVERRIDE*256)
	stosw				;lodsb
	mov	ax,I_AND_AL_MEM		;and al,ss:[xxxx]
	stosw
	lea	ax,cl_a_brush		;  Set address of color munge
	stosw
	mov	bx,ax			;  Save address
	mov	al,I_SS_OVERRIDE 	;ss:
	stosb
	mov	ax,I_XOR_AL_MEM		;xor al,[xxxx]
	stosw
	lea	ax,1[bx]		;  Set address of XOR mask
	stosw

cblt_3160:
	jmp	short cblt_3240


cblt_3170:


endif	;GEN_COLOR_BLT

;	Just need to generate the normal fetch sequence (lodsb)

cblt_3180:
	mov	al,I_LODSB		;Generate source fetch
	stosb


	subttl	Compile - Phase Alignment
	page

	assumes ds,Code

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	Generate the phase alignment if any.
;
;	It is assumed that AL contains the source byte
;
;	Currently:
;
;	    DS = CS
;	    DH = phase alignment
;
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

cblt_3240:
	mov	cx,di			;end of fetch code
	sub	cx,gl_start_fl		;start of fetch code
	mov	cFetchCode,cx		;save size of fetch code
	xor	cx,cx			;Might have garbage in it
	or	dh,dh			;Any phase alignment?
	jz	cblt_3280		;  No, so skip alignment
	mov	cl,dh			;Get horizontal phase for rotating
	mov	ax,I_ROL_AL_N		;Assume rotate left n times
	cmp	cl,5			;4 or less rotates?
	jc	cblt_3260		;  Yes
	neg	cl			;  No, compute ROR count
	add	cl,8
	mov	ah,HIGH I_ROR_AL_N
	errnz	<(LOW I_ROL_AL_N)-(LOW I_ROR_AL_N)>

cblt_3260:
	stosw				;Stuff the phase alignment rotates
	mov	al,cl			;  then the phase alignment code
	stosb

; Do not generate phase masking if there is only 1 src And only 1 dest byte.
; This is not just an optimization, see comments where these flags are set.

	xor	ch,ch
	mov	al,gl_first_fetch
	and	al,FF_ONLY_1_SRC_BYTE or FF_ONLY_1_DEST_BYTE
	xor	al,FF_ONLY_1_SRC_BYTE or FF_ONLY_1_DEST_BYTE
	jz	cblt_3280

	mov	si,CodeOFFSET phase_align
	mov	cl,(PHASE_ALIGN_LEN SHR 1)
	rep	movs wptr es:[di],wptr cs:[si]
if	PHASE_ALIGN_LEN AND 1
	movs	bptr es:[di],bptr cs:[si]
endif

cblt_3280:
	test	gl_first_fetch,FF_TWO_INIT_FETCHES;Generate another fetch?
	jz	cblt_4000		;  No

;	A second fetch needs to be stuffed.  Copy the one just created.

	mov	si,di			;Get start of fetch logic
	xchg	si,gl_start_fl		;Set new start, get old
	mov	cx,di			;Compute how long fetch is
	sub	cx,si			;  and move the bytes
	rep	movs byte ptr es:[di],byte ptr es:[si]

	subttl	Compile - ROP Generation
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	Create the logic action code
;
;	The given ROP will be converted into the actual code that
;	performs the ROP.
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;


SRC_IN_AL 	equ	00000001b	;Source field is in AL		(0)
DEST_IN_AH	equ	00000010b	;Destination field is in AH	(1)
PUSH_POP_FLAG	equ	00000100b	;Next push/pop is a pop 	(1)


;	Copy the ROP template into the BLT

cblt_4000:
	mov	ax,gl_operands		;Get back rop data
	mov	bl,ah			;Get count of number of bits to move
	and	bx,HIGH ROPLength
	shr	bx,2
	mov	cl,cs:bptr roptable+256[bx];Get length into cx
	xor	ch,ch			;
	errnz	ROPLength-0001110000000000b

	mov	bx,ax			;Get offset of the template
	and	bx,ROPOffset
	jz	cblt_4020		;Source copy
	lea	si,cs:roptable[bx] 	;--> the template
	rep	movs bptr es:[di],bptr cs:[si]			;Move the template

cblt_4020:
	mov	bx,ax			;Keep rop around
	or	ah,ah			;Generate a negate?
	jns	cblt_4040		; No
	mov	ax,I_NOT_AL
	stosw

cblt_4040:
	mov	gl_end_fl,di		;Save end of fetch/logic operation



	subttl	Compile - Mask And Save
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	Generate code to mask and save the result.  If the destination
;	isn't in a register, it will be loaded from ES:[DI] first.  The
;	mask operation will then be performed, and the result stored.
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

	mov	al,I_ES_OVERRIDE 	;Load destination in AH
	stosb
	mov	ax,I_MOV_AH_DEST
	stosw

cblt_4280:
	mov	si,CodeOFFSET masked_store ;Move rest of masked store template
	movs   wptr es:[di],wptr cs:[si]
	movs   wptr es:[di],wptr cs:[si]
	movs   wptr es:[di],wptr cs:[si]
	errnz	MASKED_STORE_LEN-6	;Must be six bytes long
	mov	ax,gl_start_mask	;Stuff start mask into
	xchg	ah,al			;  the template
	mov	es:MASKED_STORE_MASK[di],ax
	mov	gl_end_fls,di		;Save end of fetch/logic/store operation

	assumes ds,nothing



	subttl	Compile - Inner Loop Generation
	page

;	Now for the hard stuff; The inner loop (said with a "gasp!").
;
;	If there is no innerloop, then no code will be generated
;	(now that's fast!).

cblt_5000:
	mov	dx,gl_inner_loop_count 	;Get the loop count
	or	dx,dx			;If the count is null
;	jz	cblt_6000
	jz	cblt_5140		;  don't generate any code.



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	We have something for a loop count.  If this just happens to be
;	a source copy (S) with a phase of zero, then the innerloop degenerates
;	to a repeated MOVSB instruction.  This little special case is
;	worth checking for and handling!
;
;	Also, if this is one of the special cases {P, Pn, DDx, DDxn}, then it
;	will also be special cased since these are all pattern fills (pattern,
;	not pattern, 0, 1).
;
;	The same code can be shared for these routines, with the exception
;	that patterns use a STOSx instruction instead of a MOVSx instruction
;	and need a value loaded in AX
;
;
ifdef	GEN_COLOR_BLT
;	So we lied a little.  If a color conversion is going on, then the
;	REP MOVSB might not be usable.	If the F1_REP_OK flag has been set, then
;	we can use it.	The F1_REP_OK flag will be set for a mono ==> color
;	conversion where the background color is white and the foreground
;	color is black, or for a color ==> mono conversion with the screen
;	as the source (the color compare register will be used).
;
;	For the special cases {P, Pn, DDx, DDxn}, color conversion is
;	not possible, so ignore it for them.
endif
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

	mov	bl,bptr (Rop)		;Get the raster op
	test	bl,EPS_INDEX		;Can this be special cased?
	jnz	cblt_5500		;  No
	errnz	<HIGH EPS_INDEX>
	errnz	SPEC_PARSE_STR_INDEX	;The special case index must be 0

	test	bl,EPS_OFF		;Is this a source copy
	jz	cblt_5040		;  Yes
	errnz	<SOURCE_COPY AND 11b>	;Offset for source copy must be 0



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	We should have one of the following fill operations:
;
;		P	- Pattern
;		Pn	- NOT pattern
;		DDx	- 0 fill
;		DDxn	- 1 fill
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

	mov	ax,I_MOV_AL_0FFH 	;Assume this is a 0 or 1 fill
	test	bl,01h			;Is it 0 or 1 fill?
	jz	cblt_5020		;  Yes, initialize AX with 0FFh
	mov	ax,I_MOV_AL_DH		;  No,	initialize AX with pattern

	errnz	   PAT_COPY-0000000000100001b
	errnz	NOTPAT_COPY-0000000000000001b
	errnz	 FILL_BLACK-0000000001000010b
	errnz	 FILL_WHITE-0000000001100010b

cblt_5020:
	stosw
	mov	ax,I_MOV_AH_AL
	stosw
	mov	si,I_STOSB		;Set up for repeated code processor
	test	bl,LogPar		;If Pn or 0, then complement pattern
	jnz	cblt_5060		;  Is just P or 1
	errnz	<HIGH LogPar>
	mov	ax,I_NOT_AX		;  Is Pn or 0, complement AX
	stosw
	jmp	short cblt_5060

	errnz	   PAT_COPY-00100001b
	errnz	NOTPAT_COPY-00000001b
	errnz	 FILL_BLACK-01000010b
	errnz	 FILL_WHITE-01100010b




;	This is a source copy.	The phase must be zero for a source copy
;	to be condensed into a REP MOVSx.

cblt_5040:
	test	gl_phase_h,0FFh		;Is horizontal phase zero?
	jnz	cblt_5500		;  No, can't condense source copy
	mov	si,I_MOVSB		;Set register for moving bytes

ifdef	GEN_COLOR_BLT
;	For a color conversion, F1_REP_OK must be set.

	test	gl_the_flags,F0_GAG_CHOKE ;Color conversion?
	jz	cblt_5060		;  No, rep is OK to use
	test	dl_moore_flags,F1_REP_OK;  Yes, can we rep it?
	jz	cblt_5500		;    No, do it the hard way
endif



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	This is a source copy or pattern fill.	Process an odd byte with
;	a MOVSB or STOSB, then process the rest of the bytes with a REP
;	MOVSW or a REP STOSW.  If the REP isn't needed, leave it out.
;
;	Don't get caught on this like I did!  If the direction of the
;	BLT is from right to left (decrementing addresses), then both
;	the source and destination pointers must be decremented by one
;	so that the next two bytes are processed, not the next byte and
;	the byte just processed.  Also, after all words have been processed,
;	the source and destination pointers must be incremented by one to
;	point to the last byte (since the last MOVSW or STOSW would have
;	decremented both pointers by 2).
;
;	If the target machine is an 8086, then it would be well worth the
;	extra logic to align the fields on word boundaries before the MOVSxs
;	if at all possible.
;
;	The generated code should look something like:
;
;	WARP8:				     ;This code for moving left to right
;		movsb			     ;Process an odd byte
;		ld	cx,gl_inner_loop_count/2 ;Set word count
;		rep			     ;If a count, then repeat is needed
;		movsw			     ;Move words until done
;
;
;	WARP8:				     ;This code for moving left to right
;		movsb			     ;Process an odd byte
;		dec	si		     ;adjust pointer for moving words
;		dec	di
;		ld	cx,gl_inner_loop_count/2 ;Set word count
;		rep			     ;If a count, then repeat is needed
;		movsw			     ;Move words until done
;		inc	si		     ;adjust since words were moved
;		inc	di
;
;
;	Of course, if any part of the above routine isn't needed, it isn't
;	generated (i.e. the generated code might just be a single MOVSB)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

cblt_5060:
	shr	dx,1			;Byte count / 2 for words
	jnc	cblt_5080		;  No odd byte to move
	mov	ax,si			;  Odd byte, move it
	stosb

cblt_5080:
	jz	cblt_5140		;No more bytes to move
	xor	bx,bx			;Flag as stepping from left to right
	cmp	bl,gl_step_direction	;Moving from the right to the left?
	errnz	STEPLEFT		;  (left direction must be zero)
	jnz	cblt_5100		;  No
	mov	ax,I_DEC_SI_DEC_DI	;  Yes, decrement both pointers
	stosw
	mov	bx,I_INC_SI_INC_DI	;Set up to increment the pointers later

cblt_5100:
	cmp	dx,1			;Move one word or many words?
	jz	cblt_5120		;  Only one word
	mov	al,I_MOV_CX_WORD_I 	;  Many words, load count
	mov	ah,dl
	stosw
	mov	al,dh			;Set MSB of count
	mov	ah,I_REP		;  and a repeat instruction
	stosw

cblt_5120:
	mov	ax,si			;Set the word instruction
	inc	ax
	stosb
	errnz	I_MOVSW-I_MOVSB-1	;The word form of the instruction
	errnz	I_STOSW-I_STOSB-1	;  must be the byte form + 1

	or	bx,bx			;Need to increment the pointers?
	jz	cblt_5140		;  No
	mov	ax,bx			;  Yes, increment both pointers
	stosw

cblt_5140:
	jmp	short cblt_6000		;Done setting up the innerloop
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	There is some count for the innerloop of the BLT.  Generate the
;	required BLT. Two or four copies of the BLT will be placed on the
;	stack.	 This allows the LOOP instruction at the end to be distributed
;	over two or four bytes instead of 1, saving 11 or 12 clocks for each
;	byte (for 4).  Multiply 12 clocks by ~ 16K and you save a lot of
;	clocks!
;
;	If there are less than four (two) bytes to be BLTed, then no looping
;	instructions will be generated.  If there are more than four (two)
;	bytes, then there is the possibility of an initial jump instruction
;	to enter the loop to handle the modulo n result of the loop count.
;
;	The innerloop code will look something like:
;
;
;	<	mov	cx,loopcount/n> ;load count if >n innerloop bytes
;	<	jmp	short ???     > ;If a first jump is needed, do one
;
;	BLTloop:
;		replicate initial byte BLT code up to n times
;
;	<	loop	BLTloop >	;Loop until all bytes processed
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;


cblt_5500:
	mov	bx,gl_end_fl		;Compute size of the fetch code
	sub	bx,gl_start_fl
	inc	bx			;A stosb will be appended
	mov	si,4			;Assume replication 4 times
	mov	cl,2			;  (shift count two bits left)
	cmp	bx,32			;Small enough for 4 times?
	jc	cblt_5520		;  Yes, replicate 4 times
	shr	si,1			;  No,	replicate 2 times
	dec	cx

cblt_5520:
	cmp	dx,si			;Generate a loop?
	jle	cblt_5540		;  No, just copy code
	mov	al,I_MOV_CX_WORD_I
	stosb				;mov cx,loopcount/n
	mov	ax,dx			;Compute loop count
	shr	ax,cl
	stosw
	shl	ax,cl			;See if loopcount MOD n is 0
	sub	ax,dx
	jz	cblt_5540		;Zero, no odd count to handle

	page
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	There is an odd portion of bytes to be processed.  Increment
;	the loop counter for the odd pass through the loop and then
;	compute the displacement for entering the loop.
;
;	To compute the displacement, subtract the number of odd bytes
;	from the modulus being used  (i.e. 4-3=1).  This gives the
;	number of bytes to skip over the first time through the loop.
;
;	Multiply this by the number of bytes for a logic sequence,
;	and the result will be the displacement for the jump.
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;


	inc	wptr es:-2[di]		;Not zero, adjust for partial loop
	add	ax,si			;Compute where to enter the loop at
	mul	bl
	mov	cx,ax
	mov	al,I_JMP_NEAR		;Stuff jump instruction
	stosb
	mov	ax,cx			;Stuff displacement for jump
	stosw



;	Currently:	DX = loop count
;			SI = loop modulus
;			BX = size of one logic operation
;			DI --> next location in the loop

cblt_5540:
	mov	cx,bx			;Set move count
	mov	bx,dx			;Set maximum for move
	cmp	bx,si			;Is the max > what's left?
	jle	cblt_5560		;  No, just use what's left
	mov	bx,si			;  Yes, copy the max

cblt_5560:
	sub	dx,si			;If dx > 0, then loop logic needed
	mov	si,gl_start_fl		;--> fetch code to copy
	mov	ax,cx			;Save a copy of fetch length
	rep	movs bptr es:[di],bptr es:[si];Move fetch code and stuff stosb
	mov	si,di			;--> new source (and top of loop)
	sub	si,ax
	mov	bptr es:-1[di],I_STOSB
	dec	bl			;One copy has been made
	mul	bl			;Compute # bytes left to move
	mov	cx,ax			;Set move count
	rep	movs bptr es:[di],bptr es:[si];Move the fetches
	sub	si,ax			;Restore pointer to start of loop

	page
;	The innermost BLT code has been created and needs the looping
;	logic added to it.  If there is any looping to be done, then
;	generate the loop code.  The code within the innerloop may be
;	greater than 126 bytes, so a LOOP instruction may not be used
;	in this case.

cblt_5580:
	or	dx,dx			;Need a loop?
	jle	cblt_6000		;  No, don't generate one

	mov	ax,si			;Compute offset of loop
	sub	ax,di
	cmp	ax,-125 		;Can this be a short label?
	jc	cblt_5600		;  No, must make it a near jmp

	sub	al,2			;Bias offset by length of LOOP inst.
	mov	ah,al
	mov	al,I_LOOP
	stosw				;Set the loop instruction
	jmp	short cblt_6000		;Go process the last byte code


cblt_5600:
	mov	si,CodeOFFSET jmp_cx_nz ;Move in the dec CX jnz code
	movs	wptr es:[di],wptr cs:[si]
	movs	wptr es:[di],wptr cs:[si]
	errnz	JMP_CX_NZ_LEN-4		;Must be four bytes long
	sub	ax,6			;Adjust jump bias
	stosw				;  and store it into jump



	subttl	Compile - Last Byte Processing
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	All the innerloop stuff has been processed.  Now generate the code for
;	the final byte if there is one.  This code is almost identical to the
;	code for the first byte except there will only be one fetch (if a
;	fetch is needed at all).
;
;	The code generated will look something like:
;
;	<	fetch		>	;Get source byte
;	<	align		>	;Align source if needed
;		action			;Perform desired action
;		mask and store
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

cblt_6000:
	mov	dx,gl_last_mask		;Get last byte mask
	or	dh,dh			;Is there a last byte to be processed?
	jz	cblt_6100		;  No.

	mov	cx,gl_end_fls		;Get end of fetch/logic/store operation
	mov	si,gl_start_fl		;Get start of fetch/logic sequence
	sub	cx,si			;Compute length of the code
	test	gl_first_fetch,FF_NO_LAST_FETCH
	jz	cblt_include_fetch
	test	gl_the_flags,F0_SRC_PRESENT  ; was there a fetch?
	jz	cblt_was_no_fetch
	cmp	gl_phase_h,0	      ; Phase zero case is not combined
				      ; into innerloop as it should be.
				      ; If the final byte is full then we
				      ; better not remove the lodsb ( i.e.
	je	cblt_include_fetch    ; 0 - 0 = 0 would make us think we could)

	mov	ax,cFetchCode ; don't copy the fetch (lodsb)
	add	si,ax
	sub	cx,ax

cblt_was_no_fetch:
cblt_include_fetch:

	rep	movs bptr es:[di],bptr es:[si];Copy the fetch/action/store code
	xchg	dh,dl
	mov	es:MASKED_STORE_MASK[di],dx ;Stuff last byte mask into the code


	subttl	Compile - Looping Logic
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	Looping logic.
;
;	The looping logic must handle monochrome bitmaps, color bitmaps,
;	huge bitmaps, the device, the presence or absence of a source
;	or pattern, and mono <==> color interactions.
;
;	The type of looping logic is always based on the destination.
;
;
ifdef	GEN_COLOR_BLT
;	Plane Update Facts:
;
;	1)  If the destination device is color, then there will be
;	    logic for plane selection.	Plane selection is performed
;	    at the start of the loop for the display.  Plane selection
;	    for bitmaps is performed at the end of the loop in anticipation
;	    of the next plane.
;
;
;	    The following applies when the destination is color:
;
;
;	    a)	The destination update consists of:
;
;		1)  If the destination is the display, the next plane will
;		    be selected by the plane selection code at the start
;		    of the scan line loop.
;
;		2)  If not the display, then the PDevice must a bitmap.
;		    The next plane will be selected by updating the
;		    destination offset by the plane_w value.
;
;
;	    b)	If F0_GAG_CHOKE isn't specified, then there may be a source.
;		If there is a source, it must be color, and the update
;		consists of:
;
;		1)  If the source is the display, the next plane will be
;		    selected by the plane selection code at the start of
;		    the loop.
;
;		2)  If not the display, then the PDevice must a bitmap.
;		    The next plane will be selected by updating the
;		    destination offset by the plane_w value.
;
;
;	    c)	If F0_GAG_CHOKE is specified, then the source must be a
;		monochrome bitmap which is undergoing mono to color
;		conversion.  The AND & XOR mask table which is used
;		for the conversion will have to be updated, unless
;		the F1_NO_MUNGE flag is set indicating that the color
;		conversion really wasn't needed.
;
;		The source's pointer will not be updated.  It will
;		remain pointing to the same scan of the source until
;		all planes of the destination have been processed.
;
;
;	    d)	In all cases, the plane mask rotation code will be
;		generated.  If the plane indicator doesn't overflow,
;		then start at the top of the scan line loop for the
;		next plane.
;
;		If the plane indicator overflows, then:
;
;		    1)	If there is a pattern present, it's a color
;			pattern fetch.	The index of which scan of
;			the brush to use will have to be updated.
;
;		    2)	Enter the scan line update routine
;
;
;	2)	If the destination is monochrome, then there will be no
;		plane selection logic.
;
;		If F0_GAG_CHOKE is specified, then color ==> mono conversion
;		is taking place.  Any plane selection logic is internal
;		to the ROP byte fetch code.  Any color brush was pre-
;		processed into a monochrome brush, so no brush updating
;		need be done
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;



	subttl	Looping Logic - Plane Selection
	page
endif	;GEN_COLOR_BLT

;	Get saved parameters off of the stack.
;
;	<	pop	bx	      > ;Get plane indicator
;	<	pop	si	      > ;Get source pointer
;		pop	di		;Get destination pointer
;		pop	cx		;Get loop count


cblt_6100:
	mov	bh,gl_the_flags		;These flags will be used a lot
ifdef	GEN_COLOR_BLT
	test	bh,F0_DEST_IS_COLOR	;Is the destination color?
	jz	cblt_6120		;  No
	mov	al,I_POP_BX		;Restore plane index
	stosb

cblt_6120:
endif
	test	bh,F0_SRC_PRESENT	;Is a source needed?
	jz	cblt_6140		;  No
	mov	al,I_POP_SI		;  Yes, get source pointer
	stosb

cblt_6140:
	mov	ax,I_POP_DI_POP_CX	;Get destination pointer
	stosw				;Get loop count
ifdef	GEN_COLOR_BLT
	test	bh,F0_DEST_IS_COLOR	;Color scanline update?
	jnz	cblt_6160		;  Yes
	jmp	cblt_6300		;  No, just do the mono scanline update




;	The scanline update is for color.  Generate the logic to update
;	a brush, perform plane selection, process mono ==> color conversion,
;	and test for plane overflow.


cblt_6160:
	or	bh,bh			;Color conversion?
	jns	cblt_6180		;  No
	errnz	F0_GAG_CHOKE-10000000b


	page
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	The source is monochrome.  Handle mono ==> color conversion.
;	The AND & XOR mask table will need to be rotated for the next
;	pass over the source.
;
;	The source scanline pointer will not be updated until all planes
;	have been processed for the current scan.
;
;	If F1_NO_MUNGE has been specified, then the color conversion table
;	and the color conversion code was not generated, and no update
;	code will be needed.
;
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;


	test	dl_moore_flags,F1_NO_MUNGE ;Is there really a conversion table?
	jnz	short cblt_6200		;  No, so skip the code

	mov	al,I_MOV_BP_WORD_I 	;lea bp,cl_a_brush
	stosb
	lea	ax,cl_a_brush		;Get address of table
	stosw
	lea	si,rot_and_xor		;--> rotate code
	mov	cx,LEN_ROT_AND_XOR/2
	rep	movs wptr es:[di],wptr cs:[si]
if	LEN_ROT_AND_XOR AND 1
	movs	bptr es:[di],bptr cs:[si]
endif
	jmp	short cblt_6200



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	If there is a source, it must be color.  If it is a memory
;	bitmap, then the next plane must be selected, else it is
;	the display and the next plane will be selected through
;	the hardware registers.
;
;	<	add	si,plane_w>
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;


cblt_6180:
	test	bh,F0_SRC_PRESENT	;Is there really a source?
	jz	cblt_6200		;No source.
	test	bh,F0_SRC_IS_DEV	;Is the source the display?
	jnz	cblt_6200		;  Yes, use hardware plane selection
	mov	ax,I_ADD_SI_WORD_I 	;  No, generate plane update
	stosw				;Add si,plane_w
	mov	ax,gl_src.plane_w
	stosw




; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	If the destination isn't the device, then it must be a color
;	memory bitamp, and it's pointer will have to be updated by
;	bmWidthPlanes.	If it is the display, then the next plane
;	will be selected through the hardware registers.
;
;	<	add	di,plane_w>
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

cblt_6200:
	test	bh,F0_DEST_IS_DEV	;Is the destination the display
	jnz	cblt_6220		;  Yes, don't generate update code
	mov	ax,I_ADD_DI_WORD_I 	;  No, update bitmap to the next plane
	stosw
	mov	ax,gl_dest.plane_w
	stosw





; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	The source and destination pointers have been updated.
;	Now generate the plane looping logic.
;
;	<	shl	bl,1	      > ;Select next plane
;	<	jc	$+5	      > ;  No, reset to first
;	<	jmp	StartOfLoop   > ;  Yes, go process next
;	<	mov	bl,PLANE_1     > ;Reset plane indicator
;
;	or
;
;	<	shl	bl,1	      > ;Select next plane
;	<	jnc	StartOfLoop   > ;  Yes, go process next
;	<	mov	bl,PLANE_1     > ;Reset plane indicator
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

cblt_6220:
	mov	ax,I_SHL_BL_1		;Stuff plane looping logic
	stosw

	mov	dx,off_gl_blt_addr	;Compute relative offset of
	sub	dx,di			;  start of loop
ifdef	GEN_COLOR_BLT
	add	dx,2			;account for init of plane indicator
endif
	cmp	dx,-125 		;Can this be a short label?
	jc	cblt_6240		;  No, must make it a near jmp
	sub	dl,2			;Bias offset by length of jz inst.
	mov	ah,dl
	mov	al,I_JNC
	stosw				;jnc StartOfLoop
	jmp	short cblt_6260

cblt_6240:
	mov	ax,I_JC_P5H		;jc $+5
	stosw
	mov	al,I_JMP_NEAR		;jmp near
	stosb
	sub	dx,5			;Adjust jump bias
	mov	ax,dx
	stosw				;Store jmp displacement

cblt_6260:
	mov	ax,(PLANE_1*256)+I_MOV_BL_BYTE_I
	stosw



	subttl	Looping Logic - Color Brush Update
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	The plane update logic has been copied.  If a pattern was
;	involved for a color BLT, then the pattern index will need
;	to be updated to the next scanline for three plane mode.
;
;	This will involve subtracting off 3*SIZE_PATTERN (MonoPlane),
;	and adding in the increment.  The result must be masked with
;	00000111b to select the correct source.  Note that the update
;	can be done with an add instruction and a mask operation.
;
;	inc   index+MonoPlane	inc-MonoPlane	result	 AND 07h
;
;	 1	 0+32 = 32	  1-32 = -31	   1	     1
;	 1	 7+32 = 39	  1-32 = -31	   8	     0
;	-1	 0+32 = 32	 -1-32 = -33	  FF	     7
;	-1	 7+32 = 39	 -1-32 = -33	   6	     6
;
;	<	mov	al,ss:[1234]  > ;Get brush index
;	<	add	al,n	      > ;Add displacement to next byte
;	<	and	al,00000111b  > ;Keep it in range
;	<	mov	ss:[1234],al  > ;Store displacement to next byte
;
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;


	test	bh,F0_PAT_PRESENT	;Is a pattern involved?
	jz	cblt_6300		;  No
	mov	ax,I_SS_OVERRIDE+(I_MOV_AL_MEM*256)
	stosw				;mov al,ss:[xxxx]
	mov	dx,dl_addr_brush_index
	mov	ax,dx
	stosw
	mov	al,I_ADD_AL_BYTE_I
	mov	ah,gl_direction		;add al,bais
	sub	ah,oem_brush_mono	;Anybody ever fly one of these things?
	errnz	INCREASE-1		;Must be a 1
	errnz	DECREASE+1		;Must be a -1
	stosw
	mov	ax,0700h+I_AND_AL_BYTE_I	;and al,00000111b
	stosw
	mov	ax,I_SS_OVERRIDE+(I_MOV_MEM_AL*256)
	stosw				;mov ss:[xxxx],al
	mov	ax,dx
	stosw

endif	;GEN_COLOR_BLT

	subttl	Looping Logic - Scan Line Update
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	Generate the next scanline code.  The next scan line code must
;	handle monochrome bitmaps, the device, huge bitmaps, the presence
;	or absence of a source.
;
ifdef	GEN_COLOR_BLT
;	Also color bitmaps, and mono <==> color interactions.
endif
;
;	<	add si,gl_src.next_scan> ;Normal source scan line update
;	<	Huge Bitmap Update    > ;>64K source update code
;		add di,gl_dest.next_scan ;Normal destination scan line update
;	<	Huge Bitmap Update    > ;>64K destination update code
;
;
;	All updates will at least consist of the add IndexReg,plane_w.
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;


cblt_6300:
	mov	ch,gl_direction		;Load this for YUpdate code
	test	bh,F0_SRC_PRESENT	;Is there a source?
	jz	cblt_6340		;  No, skip source processing
	mov	dx,I_ADD_SI_WORD_I 	;add si,increment
	mov	bx,((HIGH I_MOV_SI_AX)*256)+(HIGH I_LEA_AX_SI_DISP16)
	mov	cl,HIGH I_MOV_AX_DS
	push	bp
	lea	bp,gl_src
	call	y_update 		;Generate the Y scan line update code
	pop	bp			;Restore frame pointer

cblt_6340:
	mov	dx,I_ADD_DI_WORD_I 	;add reg,increment
	mov	bx,((HIGH I_MOV_DI_AX)*256)+(HIGH I_LEA_AX_DI_DISP16)
	mov	cl,HIGH I_MOV_AX_ES
	push	bp
	lea	bp,gl_dest 		;--> destination data
	call	y_update 		;Generate the Y scan line update code
	pop	bp			;Restore frame pointer



;	Compile the scan line loop.  The code simply jumps to the start
;	of the outer loop if more scans exist to be processed.


cblt_6380:
	mov	ax,off_gl_blt_addr	;Compute relative offset of
	sub	ax,di			;  start of loop
ifdef	GEN_COLOR_BLT
	add	dx,2			;account for init of plane indicator
endif
	cmp	ax,-125 		;Can this be a short label?
	jc	cblt_6400		;  No, must make it a near jmp
	sub	al,2			;Bias offset by length of LOOP inst.
	mov	ah,al
	mov	al,I_LOOP
	stosw				;Set the loop instruction
	jmp	short cblt_6420

cblt_6400:
	mov	si,CodeOFFSET jmp_cx_nz ;Move in the dec CX jnz code
	movs	wptr es:[di],wptr cs:[si]
	movs	wptr es:[di],wptr cs:[si]
	errnz	JMP_CX_NZ_LEN-4		;Must be four bytes long
	sub	ax,6			;Adjust jump bias
	stosw				;  and store it into jump

cblt_6420:
	mov	al,I_RET_FAR		;Stuff the far return instruction
	stosb

;------ ret --------
	db	I_RET_NEAR	; near return
cEnd	nogen


	subttl	Scan Line Update Generation
	page


;----------------------------Private-Routine----------------------------;
; y_update
;
; Generate Y update code.
;
;
; The Y update code is generated as follows:
;
; For the display, small bitmaps, and huge bitmaps where the BLT
; doesn't span a segment bounday, all that need be done is add
; next_scan to the offset portion of the bits pointer.  next_scan
; is a 2's complement if the BLT is Y-, so an addition can always
; be done.
;
;     < add   si,next_scan >
;       add   di,next_scan
;
;
; For huge bitmaps where the BLT spans a segment boundary, the
; above update must be performed, and the overflow/undeflow
; detected.  This isn't too hard to detect.
;
; For any huge bitmap, there can be a maximum of Planes*bmWidthBytes-1
; unused bytes in a 64K segment.  The minimum is 0.  The scan line
; update always updates to the first plane of the next (previous) scan.
;
;
; When the BLT is Y+, if the new offset is anywhere within the
; unused bytes of a segment, or in the first scan of a segment,
; then overflow must have occured:
;
;       -bmFillBytes <= offset < Planes*bmWidthBytes
;
; Since the update is always made to the first plane of a scan,
; Planes in the above equation can be thrown out.  Also, if
; bmFillBytes is added to both sides of the equation:
;
; 	0 <= offset < bmWidthBytes+bmFillBytes	(unsigned compare)
;
; will be true if overflow occurs.  The Y+ overflow check will
; look like:
;
;
;     lea ax,bmFillBytes[si]		;Adjust for fill bytes now
;     cmp ax,bmWidthBytes+bmFillBytes	;Overflow occur?
;     jnc NoOverflow			;  No
;     cmp cx,2				;Any more scans?
;     jnc NoOverflow			;  No, don't update selector
;     add si,bmFillBytes		;Step over fill bytes
;     mov ax,ds				;Compute new selector
;     add ax,bmSegmentIndex
;     mov ds,ax
;
;   NoOverflow:
;
;
;
; For Y- BLTs, the test is almost the same.  The equation becomes
;
;    -(Planes*bmWidthBytes) > offset	(unsigned compare)
;
; then underflow occurs.  Planes in the above equation cannot be
; thrown out.  The Y- underflow check will look like:
;
;     mov ax,si
;     cmp ax,-(Planes*bmWidthBytes)	;Overflow occur?
;     jc  NoOverflow			;  No
;     cmp cx,2				;Any more scans?
;     jnc NoOverflow			;  No, don't update selector
;     add si,bmFillBytes		;Step over fill bytes
;     mov ax,ds				;Compute new selector
;     add ax,bmSegmentIndex
;     mov ds,ax
;
; bmFillBytes and bmSegment index will be the 2's complement by
; now if the BLT is Y-.
;
;
; Entry:
;	SS:BP --> source or destination data
;	SS:DI --> where to generate the code
;	DX     =  update register (add si,wordI & mov ax,si)
;	BL     =  lea register (SI or DI)
;	BH     =  mov si,ax   or   mov di,ax register
;	CL     =  segment register (DS or ES)
;	CH     =  Direction
; Returns:
;	SS:BP --> source or destination data
;	SS:DI --> where to generate the code
;	BL     =  lea register (SI or DI)
;	BH     =  mov si,ax   or   mov di,ax register
;	CL     =  segment register (DS or ES)
;	CH     =  Direction
; Registers Preserved:
;	DX,SI
; Registers Destroyed:
;	AX,DI,flags
; Calls:
;	None
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

y_update	proc	near

;	Stuff the scan line increment for the source or destination
;
;	<   add     si,1234h	>	;Update source
;	<   add     di,9ABCh	>	;Update destination


	mov	ax,next_scan[bp] 	;Get the increment
ifndef _BANK
	or	ax,ax			;If zero, don't generate the code
	jz	y_update_10
endif
	xchg	ax,dx			;Set opcode
	stosw
	xchg	ax,dx			;Set increment
	stosw
ifdef _BANK
	test	dev_flags[bp],IS_DEVICE
	jnz	y_update_handle_bank_switch
endif

y_update_10:
	test	dev_flags[bp],SPANS_SEG	;Does the BLT span a segment?
	jnz	spans_a_segment		;  Yes, lots of work
	ret				;  No, all done



;	The BLT spans a segment.  The code to detect when the segment is
;	crossed must be generated, as given above.


spans_a_segment:
	mov	ah,dh			;Set register for MOV
	errnz	<(HIGH I_ADD_SI_WORD_I) - (HIGH I_MOV_AX_SI)>
	errnz	<(HIGH I_ADD_DI_WORD_I) - (HIGH I_MOV_AX_DI)>

	mov	al,LOW I_MOV_AX_SI	;Assume Y- BLT
	errnz	<(LOW I_ADD_SI_WORD_I) - (LOW I_ADD_DI_WORD_I)>

	cmp	ch,DECREASE		;Y- BLT?
	je	y_update_30		;  Yes

	mov	ah,bl			;lea reg, bmFillBytes
	mov	al,LOW I_LEA_AX_SI_DISP16
	errnz	<(LOW I_LEA_AX_SI_DISP16) - (LOW I_LEA_AX_DI_DISP16)>

	stosw
	mov	ax,fill_bytes[bp]

y_update_30:
	stosw

	mov	al,I_CMP_AX_WORD_I
	stosb
	mov	ax,comp_value[bp]
	stosw

	mov	al,comp_test[bp]
	mov	ah,HIGH I_JC_P12H
	stosw
	errnz	<(HIGH I_JC_P12H) - (HIGH I_JNC_P12H)>

	mov	ax,I_CMP_CX_2
	stosw

	mov	ax,2+((LOW I_JC_P0DH)*256)
	stosw

	mov	al,(HIGH I_JC_P0DH)
	stosb
	errnz	<(LOW I_MOV_SI_AX)-(LOW I_MOV_DI_AX)>

	xchg	ax,dx			;Get add si, or add di,
	stosw
	mov	ax,fill_bytes[bp]
	stosw

	mov	al,LOW I_MOV_AX_DS
	mov	ah,cl
	stosw
	errnz	<(LOW I_MOV_AX_DS)-(LOW I_MOV_AX_ES)>

	mov	al,I_ADD_AX_WORD_I
	stosb

	mov	ax,seg_index[bp]
	stosw

	mov	al,LOW I_MOV_DS_AX	;mov SegmentReg,ax
	mov	ah,cl
	stosw
	errnz	<(LOW I_MOV_DS_AX)-(LOW I_MOV_ES_AX)>
	errnz	<(HIGH I_MOV_DS_AX)-(HIGH I_MOV_AX_DS)>
	errnz	<(HIGH I_MOV_ES_AX)-(HIGH I_MOV_AX_ES)>

y_update_40:
	ret

ifdef _BANK
y_update_handle_bank_switch:
	mov	bx,offset PreviousBankTemplate ;Assumes ax is negative.
	mov	cx,PreviousBank_len
	or	ax,ax			;ax = amount to add to si|di
	jl	short @f
	mov	bx,offset NextBankTemplate
	mov	cx,NextBank_len
@@:
	mov	si,bx
	rep	movs bptr es:[di], bptr cs:[si]
	ret

align 2
PreviousBankTemplate:
	jb	short @f
	push	ax
	push	dx
	mov	dx,03cdh		;dx = GDS Segment select.
	in	al,dx			;Get current setting.
	sub	al,11h			;Decrement write bank.
	out	dx,al			;tell the h/w.
	pop	dx
	pop	ax
@@:
PreviousBank_len	equ	$-PreviousBankTemplate ;Length of the template

align 2
NextBankTemplate:
	ja	short @f
	push	ax
	push	dx
	mov	dx,03cdh		;dx = GDS Segment select.
	in	al,dx			;Get current setting.
	add	al,11h			;Increment write bank.
	out	dx,al			;tell the h/w.
	pop	dx
	pop	ax
@@:
NextBank_len	equ	$-NextBankTemplate ;Length of the template
endif

y_update	endp
sEnd	Code

if 	MASMFLAGS and PUBDEFS
	public	jmp_cx_nz
	public	phase_align
	public	masked_store
	public	cblt_2020
ifdef	GEN_COLOR_BLT
endif	;GEN_COLOR_BLT
	public	cblt_pattern_fetch
ifdef	GEN_COLOR_BLT
	public	cblt_2060
endif
	public	cblt_initial_byte_fetch
	public	cblt_3020
	public	cblt_3040
ifdef	GEN_COLOR_BLT
	public	cblt_3060
	public	cblt_3070
	public	cblt_3080
	public	cblt_3100
	public	cblt_3110
	public	cblt_3160
	public	cblt_3170
endif	;GEN_COLOR_BLT
	public	cblt_3180
	public	cblt_3240
	public	cblt_3260
	public	cblt_3280
	public	cblt_4000
	public	cblt_4020
	public	cblt_4040
	public	cblt_4280
	public	cblt_5000
	public	cblt_5020
	public	cblt_5040
	public	cblt_5060
	public	cblt_5080
	public	cblt_5100
	public	cblt_5120
	public	cblt_5140
	public	cblt_5500
	public	cblt_5520
	public	cblt_5540
	public	cblt_5560
	public	cblt_5580
	public	cblt_5600
	public	cblt_6000
	public	cblt_6100
ifdef	GEN_COLOR_BLT
	public	cblt_6120
endif
	public	cblt_6140
ifdef	GEN_COLOR_BLT
	public	cblt_6160
	public	cblt_6180
	public	cblt_6200
	public	cblt_6220
	public	cblt_6240
	public	cblt_6260
endif	;GEN_COLOR_BLT
	public	cblt_6300
	public	cblt_6340
	public	cblt_6380
	public	cblt_6400
	public	cblt_6420

	public	y_update
	public	y_update_10
	public	spans_a_segment
	public	y_update_30
	public	y_update_40
endif
 	end

