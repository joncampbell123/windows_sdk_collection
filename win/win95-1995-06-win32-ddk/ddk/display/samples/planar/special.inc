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

	.xlist
;----------------------------Module-Header------------------------------;
; Module Name: SPECIAL.BLT
;
; Dispatcher for special cases of BLT's and supporting subroutines.
;
; This file is part of a set that makes up the Windows BitBLT function
; at driver-level.
;-----------------------------------------------------------------------;
	.list

	.xlist
;	Having include files embedded within include files is, strictly
;	speaking, against house style.  However, it makes separation of
;	device independent vs dependent code easier.

	include	EGA.INC
	include EGAMEM.INC
	.list

	.xlist
	public	check_device_special_cases
if	MASMFLAGS and PUBDEFS
ifdef	GEN_COLOR_BLT
	public	cdsc_dest_is_device
	public	cdsc_its_not_src_copy
	public	cdsc_not_s_or_p
	public	cdsc_its_1
	public	cdsc_its_0
	public	cdsc_its_patblt
	public	cdsc_not_solid_nor_grey
	public	cdsc_its_solid_color
	public	cdsc_its_a_solid_color
	public	cdsc_its_inverse_patblt
	public	cdsc_its_dn
	public	cdsc_its_dpx
	public	cdsc_its_solid_dpx
	public	cdsc_its_grey_dpx
	public	cdsc_its_src_copy
	public	cdsc_source_is_memory
	public	cdsc_blt_not_special_cased
endif	;GEN_COLOR_BLT
ifdef	GEN_COLOR_BLT
	public	blt_done_as_special_case
	public	cdsc_exit
endif	;GEN_COLOR_BLT
ifdef	GEN_COLOR_BLT

	public	ega_src_copy
	public	ega_src_copy_40
	public	ega_src_copy_50
	public	ega_src_copy_60
	public	ega_src_copy_80
	public	ega_src_copy_90
	public	ega_src_copy_100

	public	ega_src_copy_partial_byte
	public	ega_src_copy_part_10
	public	ega_src_copy_part_20

	public	ega_src_copy_calc_params
	public	ega_src_copy_calc_10
	public	ega_src_copy_calc_20
	public	ega_src_copy_calc_30

	public	do_solid_patcopy
      ;;  public  ega_solid_pat
      ;;  public  not_solid_color
      ;;  public  ega_solid_pat_20
      ;;  public  ega_solid_pat_30
      ;;  public  ega_solid_pat_40
      ;;  public  ega_solid_pat_50

	public	do_wes_invert	
	public	do_wes_dpx_solidpat
	public	no_left_invert_edge
	public	no_inner_invert_loop
	public	no_last_invert_edge
	public	do_wes_dpx_solid_pat_end

	public	do_wes_patblt
	public	no_left_pat_edge
	public	no_inner_pat_loop
	public	no_last_pat_edge

	public	do_wes_mono_trick
	public	no_left_edge
	public	no_inner_loop
	public	no_last_edge

	public	calc_parms
	public	crosses_byte_boundary
	public	no_source
	public	no_pattern
if	MASMFLAGS and DEBUG
	public	yext_is_zero
	public	yext_not_zero
endif

	public	mono_to_color_blt
	public	pmono_to_color_loop
	public	pnext_byte
	public	phase_zero
	public	zmono_to_color_loop
	public	phase_neg
	public	nmono_to_color_loop
	public	nnext_byte
	public	leave_in_set_mode

	public	edge_mono_to_color_blt
	public	pfirst_pass
	public	phase_is_negative1
	public	nfirst_pass
	public	end_pass_one
	public	skip_first_pass
	public	psecond_pass
	public	phase_is_negative2
	public	nsecond_pass
	public	no_planes_left

	public	pat_blt
	public	set_next_plane
	public	hit_next_byte
	public	pat_blt_next_scan
	public	pat_blt_loop

	public	edge_pat_blt
	public	enable_next_plane
	public	over_scans

	public	invert
	public	invert_next_scan

	public	edge_invert
	public	edge_invert_next_scan
endif	;GEN_COLOR_BLT
endif	;MASMFLAGS and PUBDEFS

ifdef	THIS_IS_DOS_3_STUFF
else
;	.286p
endif

ifdef	THIS_IS_DOS_3_STUFF
	externA	ScreenSelector
endif

ifdef	GEN_COLOR_BLT
	externA	SCREEN_W_BYTES
endif
	.list

	page

;----------------------------Private-Routine----------------------------;
; check_device_special_cases
;
; Check for fast special cases of BLT.
;
;
; Determine if the BLT is a special case which can be performed with
; static code as opposed to code compiled on the stack, and, if so,
; dispatch to the proper static code.
;
; The parameters needed for the BLT (phase alignment, directions of
; movement, ...) have been computed and saved.  These parameters will
; now be interpreted and a BLT created on the stack.
;
; If the raster op is source copy, both devices are the screen, and the
; phase alignment is 0, then the copy can be performed by the static
; code using the EGA's write mode 1.
;
; If the rasterop is P, Pn, DDx (0), DDxn (1), and the brush is	solid
; or grey (for P and Pn), and the destination device is the screen,
; then the operation can be performed by the static code using the EGA's
; write mode 2 (write mode 0 for greys).
;
; Entry:
;	SS:BP --> frame of BitBLT local variables
;	EGA registers in default state
; Returns:
;	Carry set if BLT was performed with static code.
; Error Returns:
;	Carry clear if BLT was not a special case.
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,DS,ES,flags
; Registers Preserved:
;	BP
; Calls:
;	ega_solid_pat
;	do_wes_patblt
;	do_wes_invert
;	do_wes_dpx_solid_pat
;       do_grey_dpx
;	ega_src_copy
;	do_wes_mono_trick
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

ifdef	GEN_COLOR_BLT
ROP_P		equ	0F0h
ROP_Pn		equ	 0Fh
ROP_S		equ	0CCh
ROP_DDx		equ	  0
ROP_DDxn	equ	0FFh
ROP_Dn		equ	055h
ROP_DPx		equ	05Ah
endif

	assume	ds:nothing
	assume	es:nothing

check_device_special_cases	proc	near

ifdef	GEN_COLOR_BLT
	xor	cx,cx
	mov	dh,gl_the_flags		;Keep the flags in DH for a while
	test	dh,F0_DEST_IS_DEV	;Is the destination a device?
	jnz	cdsc_dest_is_device
	jmp	cdsc_blt_not_special_cased ;Not the device, cannot special case it

ifdef _BANK
cdsc_blt_not_special_cased_10:
	jmp	cdsc_blt_not_special_cased
endif

cdsc_dest_is_device:
	mov	di,gl_dest.next_scan	;Special case code expects this
	mov	al,bptr (Rop[2])	;Get the raster op
	cmp	al,ROP_S		;Is it src copy?

ifdef _BANK
	jnz	cdsc_blt_not_special_cased_10
else
	jnz	cdsc_its_not_src_copy
endif

	jmp	cdsc_its_src_copy	;  Yes, go check it out

cdsc_its_not_src_copy:
	cmp	al,ROP_P
	jz	cdsc_its_patblt

cdsc_not_s_or_p:
	cmp	al,ROP_DDx
	jz	cdsc_its_0
	cmp	al,ROP_Pn
	jz	cdsc_its_inverse_patblt
	cmp	al,ROP_Dn
	jz	cdsc_its_dn
	cmp	al,ROP_DPx
	jz	cdsc_its_dpx
	cmp	al,ROP_DDxn
	jz	cdsc_its_1
	jmp	cdsc_blt_not_special_cased

;	Its "1" (DDxn).
cdsc_its_1:
	mov	bl,0FFh
	mov	cl_brush_accel,SOLID_BRUSH ;(no brush given for DDx or DDxn)
	;;;call    ega_solid_pat
	call	do_solid_patcopy
	jmp	cdsc_exit

cdsc_its_0:
	xor	bl,bl
	mov	cl_brush_accel,SOLID_BRUSH ;(no brush given for DDx or DDxn)
	;;;call    ega_solid_pat
	call	do_solid_patcopy
	jmp	SHORT cdsc_exit

cdsc_its_patblt:
	mov	bl,cl_brush_accel	; color in lower bits, flags in upper
	test	bl,SOLID_BRUSH
	jnz	cdsc_its_a_solid_color
     ;;;   jmp	   cdsc_blt_not_special_cased ;; ~~~

	test	bl,GREY_SCALE
	jz	cdsc_not_solid_nor_grey

; It's grey
	xor	bl,bl
	mov	ds,seg_lpPBrush 	; set brush segment
	call	ega_solid_pat
	jmp	short cdsc_exit

cdsc_not_solid_nor_grey:
	call	do_wes_patblt
	jmp	short cdsc_exit

cdsc_its_solid_color:
	not	bl
cdsc_its_a_solid_color:
	call	do_solid_patcopy	;color and accl. flags already in BL
	jmp	short cdsc_exit

cdsc_its_inverse_patblt:
	mov	bl,cl_brush_accel	; color in lower bits, flags in upper
	test	bl,SOLID_BRUSH
	jnz	cdsc_its_solid_color

	test	bl,GREY_SCALE
	jz	cdsc_blt_not_special_cased ; can't special case

;	It's grey.
	mov	bl,-1
	mov	ds,seg_lpPBrush 	; set brush segment
	call	ega_solid_pat
	jmp	short cdsc_exit

cdsc_its_dn:
	call	do_wes_invert
	jmp	short cdsc_exit

cdsc_its_dpx:
	mov	ah,cl_brush_accel	; color in lower bits, flags in upper
	test	ah,SOLID_BRUSH
	jnz	cdsc_its_solid_dpx	; dpx with solid brush
	test	ah,GREY_SCALE		; do we have a grey brush
	jz	cdsc_blt_not_special_cased ; can't special case
cdsc_its_grey_dpx:
	call	do_grey_dpx
	jmp	short cdsc_exit
cdsc_its_solid_dpx:
	call	do_wes_dpx_solidpat	; solid color => can special case
	jmp	short cdsc_exit


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	This is a source copy.	The phase must be zero to special case the
;	source copy, and both devices must be the screen.
;
;
;	errnz	   F0_SRC_IS_DEV - 00001000b
;	errnz	 F0_SRC_IS_COLOR - 00000100b
;
;its_src_copy:
;	and	dh,F0_SRC_IS_DEV + F0_SRC_IS_COLOR
;	shiftr	dh,2
;	cmp	gl_phase_h,1		; Gives CF if horizontal phase = zero
;	rcl	dh,1
; Now we have the needed flags in the lower 3 bits of DH
;					; Src=EGA  Src=Color  Phase0
;
;	dw	do_wes_mono_trick	;    0	      0		0
;	dw	blt_not_a_special_case	;    0	      0		1
;	dw	blt_not_a_special_case	;    0	      1		0
;	dw	blt_not_a_special_case	;    0	      1		1
;	dw	do_wes_mono_trick	;    1	      0		0
;	dw	blt_not_a_special_case	;    1	      0		1
;	dw	blt_not_a_special_case	;    1	      1		0
;	dw	ega_src_copy		;    1	      1		1
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;


cdsc_its_src_copy:
	test	dh,F0_SRC_IS_DEV
ifdef _BANK
	jz	cdsc_blt_not_special_cased
else
	jz	cdsc_source_is_memory
endif

	cmp	gl_phase_h,0		; is horizontal phase zero?
	jnz	cdsc_blt_not_special_cased ;  No, can't condense source copy

ifdef _BANK
	call	bankable_ega_src_copy	;  yes, go do it
else
	call	ega_src_copy		;  yes, go do it
endif
	jmp	short cdsc_exit


cdsc_source_is_memory:
	test	dh,F0_SRC_IS_COLOR	;mono-mem to color-EGA conversion?
	jz	cdsc_fast_mono_blt	;mono is wes special case

	call	EGA_Fast_Color_Src_Copy
	jmp	short cdsc_exit

cdsc_fast_mono_blt:
	test	gl_src.dev_flags,SPANS_SEG ;Does the BLT span a segment?
	jnz	cdsc_blt_not_special_cased ;Yes, cannot special case it

	call	do_wes_mono_trick
	jmp	short cdsc_exit


cdsc_blt_not_special_cased:
endif	;GEN_COLOR_BLT
	clc
	ret

ifdef	GEN_COLOR_BLT
blt_done_as_special_case:
cdsc_exit:
	stc
	ret
endif	;GEN_COLOR_BLT

check_device_special_cases	endp

	assume	ds:nothing
	assume	es:nothing

;******************************************************************************
;
;   EGA_Fast_Color_Src_Copy
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

EGA_Fast_Color_Src_Copy proc near

	call	calc_parms
	cld


; 	DS:SI set to upper left	of bitmap or pattern
; 	ES:DI set to upper left
;	DX = src bitmap width  FOR FULL SCAN (4x one scan if color)
; 	CX = yExt
; 	BX = offset into pattern (if pat present)
;	sets dest_right_edge
;	sets gl_start_mask[0]
;	sets gl_last_mask[0]
;	sets gl_inner_loop_count


	mov	ah, bptr gl_start_mask[0]
	mov	al, bptr gl_last_mask[0]
	mov	bx, dx
	mov	dx, gl_inner_loop_count

	mov	cl, gl_phase_h
	test	cl, cl
	jns	SHORT Src_Phase_Is_Right
	add	cl, 8
Src_Phase_Is_Right:
	push	bp

	test	gl_src.dev_flags,SPANS_SEG ;Does the BLT span a segment?
	jnz	SHORT EGA_Do_Big_Ole_Blt

	mov	bp, yExt

	call	EGA_Single_Seg_Src_Copy

	pop	bp
	ret

;------------------------------------------------------------------------------
;------------------------------------------------------------------------------
;------------------------------------------------------------------------------
;
;   DS:SI -> Bitmap structure
;
EGA_Do_Big_Ole_Blt:
	pusha
	mov	cx, SrcxOrg
	shr	cx, 3
	mov	ax, SrcyOrg
	mov	di, yExt
	mov	bp, sp
	mov	[bp.Pusha_BP], di

	add	cx, WORD PTR [si.bmBits]
	mov	[bp.Pusha_Temp], cx

	mov	di, WORD PTR [si.bmBits+2]	; di = Initial segment
EGA_Fast_Seg_Loop:
	mov	cx, [si.bmScanSegment]
	sub	cx, ax
	jle	SHORT EGA_Fast_Next_Seg

	mov	dx, [bp.Pusha_BP]
	cmp	cx, dx
	jl	SHORT yExt_Is_OK
	mov	cx, dx				; CX = yExt
yExt_Is_OK:
	sub	dx, cx				; Subtract from total
	mov	[bp.Pusha_BP], dx		; and remember this


; cX = # scan lines
; ax = Starting scan line
	imul	[bp.Pusha_BX]
	mov	si, ax
	add	si, [bp.Pusha_Temp]

	push	ds
	push	di
	mov	ds, di

	mov	ax, cx
	mov	di, SCREEN_W_BYTES
	imul	di
	mov	di, [bp.Pusha_DI]
	add	ax, di
	mov	[bp.Pusha_DI], ax

	push	cx
	mov	ax, [bp.Pusha_AX]
	mov	bx, [bp.Pusha_BX]
	mov	cx, [bp.Pusha_CX]
	mov	dx, [bp.Pusha_DX]
	pop	bp

	call	EGA_Single_Seg_Src_Copy

	pop	di
	pop	ax
	mov	bp, sp

	xor	cx, cx
	cmp	[bp.Pusha_BP], cx
	je	EGA_Fast_Big_Done

	mov	ds, ax
	mov	si, [bp.Pusha_SI]

EGA_Fast_Next_Seg:
	mov	ax, cx
	neg	ax
	add	di, [si.bmSegmentIndex]
	jmp	EGA_Fast_Seg_Loop

EGA_Fast_Big_Done:
	popa
	pop	bp				; Restore original BP (NEEDED!)
	ret

EGA_Fast_Color_Src_Copy endp

;------------------------------------------------------------------------------



;******************************************************************************
;
;   EGA_Single_Seg_Src_Copy
;
;   DESCRIPTION:
;
;   ENTRY:
;	Direction flag is clear
;	AH = Mask for left side of bitmap
;	AL = Mask for right side of bitmap
;	BX = 4x size of scan line in bytes (amt to add to get to next scan)
;	CL = Phase
;	DX = Inner loop count
;	BP = yExt (# lines to draw)
;	DS:SI = First source byte
;	ES:DI = First EGA byte
;
;   EXIT:
;	No return values
;
;   USES:
;	AX, BX, CX, DX, SI, DI, BP, Flags
;
;==============================================================================

;==============================================================================
;
;   Src_Copy_Do_Single_Byte (Macro)
;
;   DESCRIPTION:
;	Macro used to do a single edge byte for the middle loop of non-phase
;	alighed src copies.
;
;   ENTRY:
;	Direction flag is clear
;	DL = Previous data byte
;	DH = 0
;	DS:SI -> Bitmap data for next byte
;	ES:DI -> Display byte to update
;
;   EXIT:
;	BX trashed
;	DL updated to next byte of bitmap
;	DS:SI -> Next byte of source
;
;------------------------------------------------------------------------------

Src_Copy_Do_Single_Byte MACRO

	lodsb
	mov	bx, dx
	ror	bx, cl
	mov	dl, al
	shr	al, cl
	or	al, bh
	stosb

	ENDM


;
;   Mask table used for various phase masking operations.
;
Mask_Look_Up LABEL BYTE
	db	01111111b	; 1
	db	00111111b	; 2
	db	00011111b	; 3
	db	00001111b	; 4
	db	00000111b	; 5
	db	00000011b	; 6
	db	00000001b	; 7

;
;   Pusha structure used to access
;
Pusha_Struc	STRUC
Pusha_DI	dw	?
Pusha_SI	dw	?
Pusha_BP	dw	?
Pusha_Temp	dw	?
Pusha_BX	dw	?
Pusha_DX	dw	?
Pusha_CX	dw	?
Pusha_AX	dw	?
Pusha_Struc	ENDS






;------------------------------------------------------------------------------

EGA_Src_Check_Flash:
	pusha
	mov	ax, 400h/8
	mov	bx, dx
	xor	dx, dx
	idiv	bx

	cmp	bp, ax
	ja	SHORT EGA_Break_Up_Flash
	popa
	jmp	SHORT EGA_Src_No_Flash

EGA_Break_Up_Flash:
	mov	bp, sp
	mov	[bp.Pusha_Temp], ax

	mov	bx, ax
	imul	[bp.Pusha_BX]
	push	ax
	mov	ax, bx
	mov	bx, SCREEN_W_BYTES
	imul	bx
	push	ax

EGA_Break_Up_Loop:
	mov	ax, [bp.Pusha_AX]
	mov	bx, [bp.Pusha_BX]
	mov	dx, [bp.Pusha_DX]
	mov	bp, [bp.Pusha_Temp]
	call	EGA_Src_No_Flash
	mov	bp, sp
	add	bp, 4
	mov	ax, [bp-2]
	add	[bp.Pusha_SI], ax
	mov	ax, [bp-4]
	add	[bp.Pusha_DI], ax
	mov	ax, [bp.Pusha_Temp]
	mov	bx, [bp.Pusha_BP]
	sub	bx, ax
	mov	[bp.Pusha_BP], bx
	cmp	bx, ax
	ja	SHORT EGA_Break_Up_Again
	add	sp, 4
	popa
	jmp	SHORT EGA_Src_No_Flash

EGA_Break_Up_Again:
	mov	cx, [bp.Pusha_CX]
	mov	di, [bp.Pusha_DI]
	mov	si, [bp.Pusha_SI]
	jmp	EGA_Break_Up_Loop





PUBLIC	EGA_Single_Seg_Src_Copy



;------------------------------------------------------------------------------

EGA_Single_Seg_Src_Copy PROC NEAR

	cmp	dx, 4
	jbe	SHORT EGA_Src_No_Flash
	cmp	bp, 32
	ja	SHORT EGA_Src_Check_Flash

EGA_Src_No_Flash:

;------------------------------------------------------------------------------
;
;   Draw left edge if there is one
;
;------------------------------------------------------------------------------

	test	ah, ah				; Q: Is there any left edge
	jz	Color_Src_Copy_No_1st_Byte	;    N: Draw middle
						;    Y: Draw it
	pusha					; Save 'em all
	mov	ch, NOT 0			; CH = all 1 bits for left edge
	call	Edge_Color_Src_Copy		; Draw it
	popa					; Restore
	jz	SHORT Adjust_Destination	; ZF set means don't inc src
	inc	si				; else bump source to next byte
Adjust_Destination:
	inc	di				; Destination goes one forward

;------------------------------------------------------------------------------
;
;   Do the middle hunk if there is one.  This code special cases phase aligned
;   bitblts for super-duper fast copies.
;
;------------------------------------------------------------------------------
Color_Src_Copy_No_1st_Byte:
	test	dx, dx				; Q: Any middle piece?
	jz	SHORT Slime_No_Middle_Jump	;    N: Check for right edge
						;    Y: Do it.


if NUMBER_PLANES eq 4
	mov	ah,11h				; Set up plane mask in AH
else
	mov	ah,21h
endif

	sub	bx, dx				; Amount to add to source

	test	cl, cl				; Q: Is the phase 0?
	jnz	SHORT Src_Copy_Middle_With_Phase;    N: Do slower, harder code
						;    Y: YAHOO!	Do fast stuff

	push	ax				; Save mask
Src_No_Phase_Next_Plane:
	mov	cx, dx				; Save inner loop count

	mov	al, MM_ALL
	and	al, ah
	mov	dx, EGA_BASE + SEQ_DATA
	out	dx, al				; Select next plane to write

	mov	dx, cx				; Restore DX to inner loop count

	push	ax				; Save plane select value
	push	si				; Save current source
	push	di				; Save destination
	push	bp				; Save yExt

	mov	ax, SCREEN_W_BYTES
	sub	ax, dx				; AX = Amount to add to dest

Src_No_Phase_Scan_Loop:
	REPMOVSB				; Copy it REALLY FAST!
	add	si, bx				; SI -> Next source scan
	add	di, ax				; DI -> Next scan start point
	mov	cx, dx				; CX = Inner loop count
	dec	bp				; Q: Any more scan lines?
	jnz	Src_No_Phase_Scan_Loop		;    Y: Copy them too.
;
;   NOTE:  At this point, CX should contain the inner loop count (look a few
;	   lines down to see why)

	pop	bp				; Restore scan count
	pop	di				; Restore destination ptr
	pop	si				; Restore source ptr
	pop	ax				; Restore plane select mask

	add	cx, bx				; CX = 4x size of scan plane
	shr	cx, 2				; CX = Inc to next scan plane
	add	si, cx				; Inc to next plane of scan

	shl	ah, 1				; Q: Any more planes to do?
	jnc	Src_No_Phase_Next_Plane 	;    Y: Do them
						;    N: Done!  Get out
	sub	cx, cx				; Restore phase (0)

	pop	ax
	jmp	Src_Copy_Middle_Done		; Jmp here to adjust SI pointer
						; and BX to size of scan

;
;   This label is jumped to from above to avoid the short jump problem with
;   a conditional jump instruction.
;
Slime_No_Middle_Jump:
	jmp	Color_Src_Copy_No_Middle

;
;   There is phase alignment for this one.
;
;   At this point BX has been adjusted for the inner loop count and AH contains
;   the plane mask value.
;
Src_Copy_Middle_With_Phase:
	pusha

	mov	bl, cl				; BL = Phase
	xor	bh, bh				; BX = Phase
	mov	ch, BYTE PTR cs:Mask_Look_Up[bx-1]; Get mask from table

	mov	bx, bp
	mov	bp, sp

;
;   At this point, BP -> Top of stack
;		   BX = yExt
;		   CH = Mask, CL = Phase
;		   DX = Inner loop count
;
Src_Phase_Plane_Loop:
	mov	[bp.Pusha_Temp], bx		; Save yExt for loop

	mov	bp, dx				; Move inner loop cnt to BP

	mov	al, MM_ALL
	and	al, ah
	mov	dx, EGA_BASE + SEQ_DATA
	out	dx, al				; Select the plane

	xor	dh, dh				; Zero high word of DX
;
;   At this point, BP = Inner loop count
;		   DH = 0
;		   CH = Mask, CL = Phase
;		   DS:SI -> Source byte
;		   ES:DI -> Dest byte
;
;
Src_Phase_Scan_Loop:
	lodsb					; Get 1st byte of data
	mov	dl, al

	sub	bp, 2				; If middle count is 2 then
	jl	Src_Copy_Do_Last_Byte		; -1 means do last byte only
	je	Src_Copy_Word_Phase_Align_Loop	; 2 byte copy always use word

	test	di, 1				; Q: Dest word aligned?
	jz	SHORT Src_Copy_Word_Phase_Align_Loop; Y: Copy middle
	Src_Copy_Do_Single_Byte 		;     N: Do 1 byte to align it
	dec	bp				;	 dec loop count

Src_Copy_Word_Phase_Align_Loop:
	mov	bx, dx				; Get prev byte into BX (BH 0)
	ror	bx, cl				; Rotate data into BH
	lodsw					; Get next word
	mov	dl, ah				; Update previous byte in DL
	ror	ax, cl				; Rotate data around in AX
	and	al, ch				; Mask off appropriate bits
	or	al, bh				; Stick in appropriate bits
	stosw					; Slam it on the display
	sub	bp, 2				; 2 more down
	jge	Src_Copy_Word_Phase_Align_Loop	; If >= 0 then more to go

	test	bp, 1				; Count of -1 means one more
	jz	SHORT Src_Copy_Middle_Next_Scan ; If -2 then done

Src_Copy_Do_Last_Byte:
	Src_Copy_Do_Single_Byte 		; Copy the last byte to screen

Src_Copy_Middle_Next_Scan:
	mov	bp, sp
	mov	bx, [bp.Pusha_BX]		; BX = Scan size - in lp cnt
	dec	[bp.Pusha_Temp] 		; Q: Any more scans?
	jz	Src_Copy_Plane_Done		;    N: Do next plane
						;    Y: Move to next scan
	mov	bp, [bp.Pusha_DX]		; BP = Inner loop count
	add	di, SCREEN_W_BYTES		; Dest to next screen scan
	sub	di, bp
	lea	si, [si+bx-1]			; Source to next bitmap scan
	jmp	Src_Phase_Scan_Loop

Src_Copy_Plane_Done:
	mov	dx, [bp.Pusha_DX]		; DX = Inner loop count
	add	bx, dx				; BX = Size of one scan
	shr	bx, 2				; BX = Size of scan plane
	mov	si, [bp.Pusha_SI]		; Get source pointer
	add	si, bx				; Source to next plane
	mov	[bp.Pusha_SI], si		; Save new source

	mov	ax, [bp.Pusha_AX]
	shl	ah, 1				; Q: Any more planes to do?
	jc	SHORT Src_Phase_Middle_Done	;    N: Get out
	mov	[bp.Pusha_AX], ax		;    Y: Save new mask value
	mov	di, [bp.Pusha_DI]		; Restore Dest pointer
	mov	bx, [bp.Pusha_BP]		; Required at top of loop (yExt)
	jmp	Src_Phase_Plane_Loop		; Do the next plane

Src_Phase_Middle_Done:
	popa

;
;   At this point, SI will have been incremented by one scan line and BX
;   will be equal to the size of one scan - inner loop count.
;
Src_Copy_Middle_Done:
	add	bx, dx				; Restore BX to original val
	sub	si, bx				; Restore original SI value

;------------------------------------------------------------------------------
;
;   Now do the right edge if there is one.
;
;------------------------------------------------------------------------------

Color_Src_Copy_No_Middle:
	test	al, al				; Q: Any right edge
	jz	Short_Ret_Label 		;    N: Done!  Return

	add	di, dx				; DI -> Right edge of display
	add	si, dx				; SI -> Right edge of source

	xor	ch, ch				; CH = 0 for right edge
	mov	ah, al				; Move mask into AH register

;
;   Fall-through to edge copy code
;
if @Version LT 600
IF2
	.ERRNZ	Edge_Color_Src_Copy-$		; Paranoia...
ENDIF
endif

EGA_Single_Seg_Src_Copy endp


;******************************************************************************
;
;   Edge_Color_Src_Copy
;
;   DESCRIPTION:
;
;   ENTRY:
;	AH = bitmask
;	BP = Number of scan lines (yExt)
;	BX = 4x size of one scan line
;	DS:SI = Data bytes
;	ES:DI = First EGA Byte
;	CL = Rotation value (phase) -- ALWAYS POSITIVE
;	CH = -1 if left edge.  0 if right edge.
;
;   EXIT:
;	ZF set 0 if 1st data byte not completely used
;	   clear if all bits in 1st data byte used
;
;   USES:
;	AX, BX, CX, DX, SI, DI, BP, and Flags
;
;   CALLS:
;	None
;
;
;==============================================================================

Edge_Color_Src_Copy   proc    near
;
;	Set bit mask.
;
	mov	dx, EGA_BASE + GRAF_ADDR
	mov	al, GRAF_BIT_MASK
	out16	dx, ax
;
;   Determine if we need to fetch a byte or word of data to draw this edge.
;   When this code is done, CH will be 0 if byte fetch required, !=0 if word
;   fetch required
;
	test	cl, cl				; Q: Phase of 0?
	jnz	SHORT Check_Word_Or_Byte	;    N: May require word fetch
	mov	ch, cl				;    Y: Never need next byte
	jmp	SHORT Edge_Copy_Set_Up_Mask

Check_Word_Or_Byte:
	mov	dh, ch				; DX = -1 or 0 depending on edge
	xchg	bx, cx				; BL = Phase
	xor	bh, bh				; BX = Phase (zero high word)
	mov	bh, BYTE PTR cs:Mask_Look_Up[bx-1]; Get mask from table
	xchg	cx, bx				; Restore BX, mask in CH
	xor	ch, dh				; Invert if left edge
	and	ch, ah				; 1 bits mean word requried

Edge_Copy_Set_Up_Mask:
if NUMBER_PLANES eq 4
	mov	ah,11h			; left nibble gives carry to end loop
else
	mov	ah,21h
endif

;
;   Loop copies one plane of data for the edge.
;
Edge_Copy_Next_Plane:
	push	si				; Save source and dest ptrs
	push	di

	mov	al, MM_ALL
	and	al, ah
	mov	dx, EGA_BASE + SEQ_DATA
	out	dx, al				; Select the next plane

	mov	dx, bp				; DX = Number of scan lines

	test	ch, ch				; Q: Do word fetch?
	jnz	SHORT Edge_Copy_Use_Word	;    Y: Jump to word loop
						;    N: Copy bytes
Edge_Copy_Scan_Byte:
	mov	al,[si] 			; Get the data
	ror	al, cl				; Phase align it
	xchg	es:[di], al			; Xchg to load and store
	add	di, SCREEN_W_BYTES		; DI -> Next screen byte
	add	si, bx				; SI -> Next data byte
	dec	dx				; Q: Any more data?
	jnz	Edge_Copy_Scan_Byte		;    Y: Loop for full yExt

Edge_Copy_Scan_Done:
	pop	di				; Restore source and dest
	pop	si

	mov	dx, bx				; DX = Offset to next plan of
	shr	dx, 2				; bitmap scan line
	add	si, dx				; SI -> Next plane of source

	shl	ah, 1				; Q: Any more planes?
	jnc	Edge_Copy_Next_Plane		;    Y: Loop for all planes
;
;	Restore bitmask to default.
;
	mov	dx, EGA_BASE + GRAF_ADDR
	mov	ax, 0FF00h + GRAF_BIT_MASK
	out16	dx, ax

;
;   Don't use DEC for next instruction since it dosen't change the carry flag.
;   Return with Zero Flag set if some data in current byte was not used.
;   The caller won't increment the source pointer if ZF is set.
;
	sub	cl, 1				; Carry if rot value = 0
	adc	ch, 0				; CH != 0 if rot value was 0

;
;   Exit point for Edge_Color_Src_Copy *AND* EGA_Single_Seg_Src_Copy
;
Short_Ret_Label:
	ret

;
;   Word fetch required for this edge.
;
Edge_Copy_Use_Word:
	mov	ch, ah			; Save plane mask value in CH

Edge_Copy_Scan_Word:
	mov	ax, [si]		; Get data word
	ror	ax, cl			; Phase alignment
	xchg	es:[di], ah		; Slap in the right data
	add	di, SCREEN_W_BYTES	; DI -> Next scan on display
	add	si, bx			; SI -> Next scan of data
	dec	dx			; Q: End of loop
	jnz	Edge_Copy_Scan_Word	;    N:
	mov	ah, ch			; Restore plane mask value to AH
					; NOTE:  Don't need to restore
					; CH since !=0 means do word copy
					; and AH is always != 0
	jmp	Edge_Copy_Scan_Done

Edge_Color_Src_Copy    endp




ifdef	GEN_COLOR_BLT
;----------------------------Private-Routine----------------------------;
; ega_src_copy
;
; EGA special case for source copy.
;
; The following routine is invoked instead of generating code for a
; source copy with no phase alignement.  The actual time involved in
; executing the screen to screen source copy as static code as compared
; to compiled code is a win for small blts and is about the same for
; the entire screen.
;
; Entry:
;	DI = gl_src.Incr = gl_dest.Incr (same device)
; Returns:
;	Nothing
; Registers Destroyed:
;	AX,CX,DX,SI,DI,DS,ES,flags
; Registers Preserved:
;	BX,BP
; Calls:
;	ega_src_copy_calc_params
;	ega_src_copy_partial_byte
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assume	ds:nothing
	assume	es:nothing

ega_src_copy	proc near

	call	ega_src_copy_calc_params

ega_src_copy_40:
	xor	cx,cx			;Process first byte
	mov	ch,bptr gl_start_mask[1]
	jcxz	ega_src_copy_50		;No first byte
	call	ega_src_copy_partial_byte ;Process first byte


ega_src_copy_50:
	mov	cx,gl_inner_loop_count 	;Set count for innerloop
	jcxz	ega_src_copy_80		;No innerloop or last byte


;	Set up the EGA and the shadow registers.  The DataRotate register
;	has already been set up.  The Mode register, MapMask, and BitMask
;	registers need to be setup.


	mov	dl,SEQ_DATA		;Enable write to all planes
	mov	al,MM_ALL
	out	dx,al

	mov	dl,GRAF_ADDR
	mov	ax,0000h+GRAF_BIT_MASK
	out16	dx,ax

ega_src_copy_60:
	rep	movsb			;All that to move some bytes!
	cmp	gl_mask_p,cx		;Only an innerloop?
	jne	ega_src_copy_80		;  No
	add	si,bx			;--> next source
	add	di,bx			;--> next destination
	mov	cx,gl_inner_loop_count 	;Set count for innerloop
	dec	yExt			;Any more scans to process?
	jnz	ega_src_copy_60		;  Yes
	jmp	short ega_src_copy_100	;  No

ega_src_copy_80:
	mov	ch,bptr gl_last_mask[1]	;Handle last byte (with no innerloop)
	jcxz	ega_src_copy_90
	call	ega_src_copy_partial_byte

ega_src_copy_90:
	add	si,bx			;--> next source
	add	di,bx			;--> next destination
	dec	yExt			;Any more scans to process?
	jnz	ega_src_copy_40		;  No

ega_src_copy_100:

if	MASMFLAGS and DEBUG
	xor	bx,bx
endif

	ret

ega_src_copy	endp


;----------------------------Private-Routine----------------------------;
; ega_src_copy_partial_byte
;
; Handle a partial byte of a source copy.
;
; Entry:
;	DX = rGraphics	(EGA_BASE + GRAF_ADDR)
;	CH = bit mask
;	DS:SI --> source bits byte to partially copy
;	ES:DI --> destination of bits
; Returns:
;	DX = rGraphics
;	DS:SI --> byte after one copied
;	ES:DI --> byte after one copied
; Registers Destroyed:
;	AX,CX,flags
; Registers Preserved:
;	BX,BP
; Calls:
;	Nothing
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assume	ds:nothing
	assume	es:nothing

ega_src_copy_partial_byte	proc	near

	mov	ah,ch			;Set the BitMask
	mov	al,GRAF_BIT_MASK
	out16	dx,ax
	mov	cx,0800h+SEQ_MAP_MASK	;Set loop counter
	jmp	short ega_src_copy_part_20

ega_src_copy_part_10:
	mov	al,[si] 		;Copy this byte
	xchg	al,es:[di]		;xchg to load EGA's latches first

ega_src_copy_part_20:
	mov	ax,cx			;Enable write to next plane
	mov	dl,SEQ_ADDR
	out16	dx,ax

	mov	dl,GRAF_ADDR		;Enable read from next plane
	shr	ah,1
	cmp	ah,00000100b		;carry will be 0 for C3 only
	adc	ah,-1			; convert 0100b t0 011b
	mov	al,GRAF_READ_MAP
	out16	dx,ax
	shr	ch,1
	jnz	ega_src_copy_part_10
	lodsb				;Move final plane and update pointers
	mov	ah,es:[di]
	stosb
	ret

ega_src_copy_partial_byte	endp


;----------------------------Private-Routine----------------------------;
; ega_src_copy_calc_params
;
; This was the beginning of ega_src_copy (EGA to EGA only).  I've
; subroutinized it so that it can be used for mono-mem to color-EGA
; source copy also.
;
; Entry:
;	DI = scan increment (gl_dest.nextscan)
; Returns:
;	BX = scan bias
;	DX = GRAF_ADDR
;	DS:SI = source
;	ES:DI = dest
;	SS:BP --> BitBLT local variable frame
;	direction flag set/cleared as appropriate for this blt.
; Registers Destroyed:
;	AX,CX,flags
; Registers Preserved:
;	BP
; Alters:
; Calls:
;	Nothing
;-----------------------------------------------------------------------;

	assume	ds:nothing
	assume	es:nothing

ega_src_copy_calc_params	proc	near

;	Anytime the source and the destination are the same device,
;	the destination and the source increments will be the same.
;	Instead of pushing and popping the source and destination
;	pointers and adding in the gl_dest.Incr and gl_src.Incr, the bias
;	needed for adjusting the pointer at the end of a scan line
;	will be computed and used:
;
;		+X +Y	  subtract gl_inner_loop_count+2      50 - 30 =  20
;		+X -Y	  subtract gl_inner_loop_count+2     -50 - 30 = -80
;		-X +Y	  add	   gl_inner_loop_count+2      50 + 30 =  80
;		-X -Y	  add	   gl_inner_loop_count+2     -50 + 30 = -20
;

	mov	bx,gl_inner_loop_count 	; Compute number of bytes to copy
	mov	cx,bx
	xor	dx,dx
	inc	bx			; first byte always there
	mov	al,bptr gl_last_mask[1]
	cmp	al,1			;If bits effected in last byte
	cmc				;  adjust scanline increment
	adc	bx,dx			; add 1 to BX if gl_last_mask[1] = 0
	cmp	gl_step_direction,STEPRIGHT ;Stepping right? (+X)
	jz	ega_src_copy_calc_10	;  Yes
	std				;  No, will be decrementing (-X)
	neg	bx			;  Negate byte count for -X

ega_src_copy_calc_10:
	mov	di,gl_src.next_scan
	sub	di,bx			;Adjust scan increment
	mov	bx,di			;Save scan bias

	or	al,al			;Last byte already 0?
	jz	ega_src_copy_calc_20	;  Yes
	inc	al			;If last byte mask is 0FFh, combine
	jnz	ega_src_copy_calc_20	;  it with innerloop
	mov	bptr gl_last_mask[1],al	;  and flag it as such
	inc	cx

ega_src_copy_calc_20:
	mov	ah,bptr gl_start_mask[1];If start byte mask is 0FFh, combine
	inc	ah			;  it with innerloop
	jnz	ega_src_copy_calc_30	;  and flag it as such
	mov	bptr gl_start_mask[1],ah
	inc	cx

ega_src_copy_calc_30:
	mov	gl_inner_loop_count,cx 	;Set real innerloop count
	mov	gl_mask_p,ax		;Save "Only Inner Loop" flag
	lds	si,gl_src.lp_bits	;--> source
	les	di,gl_dest.lp_bits	;--> destination
	mov	dx,EGA_BASE + GRAF_ADDR
	ret

ega_src_copy_calc_params	endp

ifdef _BANK
;----------------------------Private-Routine----------------------------;
; bankable_ega_src_copy
;
; EGA special case for source copy.
;
; The following routine is invoked instead of generating code for a
; source copy with no phase alignement.  The actual time involved in
; executing the screen to screen source copy as static code as compared
; to compiled code is a win for small blts and is about the same for
; the entire screen.
;
; Entry:
;	DI = gl_src.Incr = gl_dest.Incr (same device)
; Returns:
;	Nothing
; Registers Destroyed:
;	AX,CX,DX,SI,DI,DS,ES,flags
; Registers Preserved:
;	BX,BP
; Calls:
;	ega_src_copy_calc_params
;	ega_src_copy_partial_byte
;-----------------------------------------------------------------------;
	assume	ds:nothing
	assume	es:nothing

bankable_ega_src_copy	proc near
	call	ega_src_copy_calc_params
	mov	bx,gl_src.next_scan
	or	bx,bx
	jl	short BESC_Yneg

BESC_Ypos:
	push	si
	push	di
	xor	cx,cx			;Process first byte
	mov	ch,bptr gl_start_mask[1]
	jcxz	short @f		;No first byte
	call	ega_src_copy_partial_byte ;Process first byte

@@:	mov	cx,gl_inner_loop_count 	;Set count for innerloop
	jcxz	short @f		;No innerloop or last byte

;	Set up the EGA and the shadow registers.  The DataRotate register
;	has already been set up.  The Mode register, MapMask, and BitMask
;	registers need to be setup.

	mov	dl,SEQ_DATA		;Enable write to all planes
	mov	al,MM_ALL
	out	dx,al
	mov	dl,GRAF_ADDR
	mov	ax,0000h+GRAF_BIT_MASK
	out16	dx,ax

	rep	movsb			;All that to move some bytes!

@@:	mov	ch,bptr gl_last_mask[1]	;Handle last byte (with no innerloop)
	jcxz	short @f
	call	ega_src_copy_partial_byte

@@:	pop	di
	pop	si
	add	si,bx			;--> next source
	ja	short @f
	push	dx
	call	NextReadBank
	pop	dx
@@:	add	di,bx			;--> next destination
	ja	short @f
	push	dx
	call	NextWriteBank
	pop	dx
@@:	dec	yExt			;Any more scans to process?
	jnz	BESC_Ypos		;  No
	ret

BESC_Yneg:
	push	si
	push	di
	xor	cx,cx			;Process first byte
	mov	ch,bptr gl_start_mask[1]
	jcxz	short @f		;No first byte
	call	ega_src_copy_partial_byte ;Process first byte

@@:	mov	cx,gl_inner_loop_count 	;Set count for innerloop
	jcxz	short @f		;No innerloop or last byte

;	Set up the EGA and the shadow registers.  The DataRotate register
;	has already been set up.  The Mode register, MapMask, and BitMask
;	registers need to be setup.

	mov	dl,SEQ_DATA		;Enable write to all planes
	mov	al,MM_ALL
	out	dx,al
	mov	dl,GRAF_ADDR
	mov	ax,0000h+GRAF_BIT_MASK
	out16	dx,ax

	rep	movsb			;All that to move some bytes!

@@:	mov	ch,bptr gl_last_mask[1]	;Handle last byte (with no innerloop)
	jcxz	short @f
	call	ega_src_copy_partial_byte

@@:	pop	di
	pop	si
	add	si,bx			;--> next source
	jb	short @f
	push	dx
	call	PreviousReadBank
	pop	dx
@@:	add	di,bx			;--> next destination
	jb	short @f
	push	dx
	call	PreviousWriteBank
	pop	dx
@@:	dec	yExt			;Any more scans to process?
	jnz	BESC_Yneg		;  No


if	MASMFLAGS and DEBUG
	xor	bx,bx
endif

	ret

bankable_ega_src_copy	endp
endif

;----------------------------Private-Routine----------------------------;
; ega_solid_pat
;
; EGA special case for solid color pattern copy.
;
; The following routine is invoked instead of generating code for a
; pattern copy.  The actual time involved in executing the pattern
; copy as static code as compared to compiled code is a win.
;
; This code can only be used if the pattern is a solid color or a grey,
; and the operation is to the screen.  In this case, the three bits of
; color stored in the accelerator byte of the brush will be used, or the
; bits of the grey brush.
;
; The logic operations which will invoke this routine are:
;
;	P
;	Pn
;	DDx
;	DDxn
;
; Entry:
;	BL = color to write or xor value for a grey pattern
;	CX = Mode register value (sort of)
;	DS = brush segment if grey scale
;	SS:BP = BitBLT local variable frame
; Returns:
;	Nothing
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,DS,ES,flags
; Registers Preserved:
;	BP
; Calls:
;	None
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assume	ds:nothing
	assume	es:nothing

ega_solid_pat	proc near

;	Instead of pushing and popping the destination pointer and adding in
;	the gl_dest.Incr, the bias needed for adjusting the pointer at the
;	end of a scan line will be computed and used.
;
;	Since this is a pattern copy, the gl_dest.Incr will be positive.

;	mov	si,di			;Get destination increment
;	sub	si,1			;Adjust for first byte
	lea	si,-1[di]
	sub	si,gl_inner_loop_count 	;Compute number of bytes to copy

;	Put color in Set/Reset if it is a solid color.

	mov	dx,EGA_BASE + GRAF_ADDR
	jcxz	not_solid_color
	mov	ax,MM_ALL * 256 + GRAF_ENAB_SR
	out	dx,ax
	mov	ah,bl
	mov	al,GRAF_SET_RESET
	out	dx,ax
not_solid_color:
	mov	al,GRAF_BIT_MASK	;Leave graphics controller pointing
	out	dx,al			;  to the bitmask register, which
	inc	dx			;  is where cursor leaves it too

; Set up for the loop.

	les	di,gl_dest.lp_bits	;--> destination
	mov	gl_phase_h,bl		;Save color to write or grey XOR mask

ega_solid_pat_20:
	mov	al,gl_phase_h		;Get the color to write
	test	cl_brush_accel,SOLID_BRUSH ;Grey scale brush?
	jnz	ega_solid_pat_30	;  No, a solid color
	mov	bl,gl_pat_row		;Get scan of brush
	inc	bl			;  and update brush pointer
	mov	gl_pat_row,bl
	dec	bl
	and	bx,00000111b
	add	bx,off_lpPBrush
	xor	al,bptr ds:[bx] 	;Invert if needed

ega_solid_pat_30:
	mov	bl,al
	mov	al,bptr gl_start_mask[1];Set bitmask for first byte
	out	dx,al
	mov	al,bl
	xchg	al,es:[di]		;xchg to load EGA's latches first
	inc	di			;PAT_COPY step +X always!

	mov	cx,gl_inner_loop_count 	;Set count for innerloop
	jcxz	ega_solid_pat_40	;No innerloop or last byte
	mov	al,0FFh 		;Inner loop alters all bits
	out	dx,al
	mov	al,bl
	REPSTOSB 

ega_solid_pat_40:
	mov	al,bptr gl_last_mask[1]	;Last byte?
	or	al,al
	jz	ega_solid_pat_50	;No last byte
	out	dx,al
	xchg	bl,es:[di]

ega_solid_pat_50:
	add	di,si			;--> next destination
	dec	yExt			;Any more scans to process?
	jnz	ega_solid_pat_20	;  Yes


if	MASMFLAGS and DEBUG
	xor	bx,bx
endif

	ret

ega_solid_pat	endp

;----------------------------Private-Routine----------------------------;
; do_wes_invert
; do_wes_dpx_solidpat
;
; 
; Entry:
;	SS:BP --> BitBLT local variable frame
;	AH = color of solid-pat.
; Returns:
;	Nothing
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,DS,ES,flags
; Registers Preserved:
;	BP
; Calls:
;	calc_parms
;	edge_invert
;	invert
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assume	ds:nothing
	assume	es:nothing

do_wes_invert		proc	near

	mov	ah,0Fh			; black


do_wes_dpx_solidpat:

;	Setup SET_RESET.

	mov	dx,EGA_BASE + GRAF_ADDR
	mov	al,GRAF_SET_RESET
	out16	dx,ax
	jmp	$+2
	mov	ax,0F00h + GRAF_ENAB_SR	; enable all planes
	out16	dx,ax
	jmp	$+2

;	Go to XOR mode.

	mov	ax,GRAF_DATA_ROT + 256 * DR_XOR
	out16	dx,ax

	call	calc_parms
	mov	ah,byte ptr gl_start_mask[0]
	or	ah,ah
	jz	no_left_invert_edge
	pushem	di
	call	edge_invert
	popem	di
	inc	di

no_left_invert_edge:
	mov	bx,gl_inner_loop_count
	or	bx,bx
	jz	no_inner_invert_loop
	mov	cx,yExt
	pushem	di
	call	invert
	popem	di
	add	di,gl_inner_loop_count

no_inner_invert_loop:
	mov	ah,byte ptr gl_last_mask[0]
	or	ah,ah
	jz	no_last_invert_edge
	mov	cx,yExt
	call	edge_invert

no_last_invert_edge:
	ret

do_wes_dpx_solid_pat_end:
do_wes_invert		endp


;----------------------------Private-Routine----------------------------;
;do_solid_patcopy() is called to copy (PatCopy) a solid brush directly to
;the screen.
;
;Entry:
;   BL	    color to write with
;   DI	    gl_dest.next_scan which is equal to width_b
;   SS:BP   local varible frame
;
;-----------------------------------------------------------------------;

assumes ds, nothing
assumes es, nothing

do_solid_patcopy    proc near

	sub	ax, ax			;accumulate flags in AL
	mov	si, di			;SI: gl_dest.next_scan (must be > 0)
	les	di, gl_dest.lp_bits	;ES:DI-->first byte to write to
	mov	bh, bptr gl_start_mask[1]
	cmp	ah, bh			;is there a left edge?
	rcl	al, 1			;CY if there is an edge to draw
	sub	bh, 0ffh		;is the left edge an entire byte wide?
	neg	bh			;CY if less than a byte
	rcl	al, 1			;accumulate flag into AL
	mov	bh, bptr gl_last_mask[1]
	cmp	ah, bh			;is there a right edge?
	rcl	al, 1			;CY if there is an edge to draw
	sub	bh, 0ffh		;is right edge an entire byte wide?
	neg	bh			;CY if less than a byte
	rcl	al, 1			;accumulate flag into AL
	test	al, 04h 		;set all pixels in left byte?
	jnz	dsp_keep_left_edge	;no. Do normal stuff
	inc	gl_inner_loop_count	;left edge is an entire byte. INC inner
	and	al, 0f7h		; loop cnt and draw it the fast way
dsp_keep_left_edge:
	test	al, 01h 		;set all pixels in right byte?
	jnz	dsp_keep_right_edge	;no. Do normal stuff
	inc	gl_inner_loop_count	;need to set all pixels in right byte.
	and	al, 0fdh		; inc inner loop cnt do it the fast way
dsp_keep_right_edge:
	test	al, 0ah 		;any edges to draw?
	jnz	dsp_draw_edges		;yes, go draw them
	jmp	SHORT dsp_draw_core_piece;no, just do the main chunk

dsp_draw_edges:
	mov	bh, al			;save flags in BH
	mov	dx,EGA_BASE + GRAF_ADDR
	mov	ax,MM_ALL * 256 + GRAF_ENAB_SR
	out	dx,ax			;enable writing to all planes at once
	mov	ah,bl
	mov	al,GRAF_SET_RESET
	out	dx,ax			;program to color value to write
	mov	al,GRAF_BIT_MASK	;Leave graphics controller pointing
	out	dx,al			;  to the bitmask register, which
	inc	dx			;  is where cursor leaves it too

	test	bh, 08h 		;need to draw the left edge?
	jz	dsp_draw_right_edge

	push	di			;save destination offset
	mov	al, bptr gl_start_mask[1]
	out	dx, al			;get it to the board
	mov	cx, yExt

dsp_left_edge_draw_loop:
	mov	al, bl			;copy color index into AL
	xchg	al, es:[di]		;load latches, copy color index
	add	di, si
	loop	dsp_left_edge_draw_loop

	pop	di			;restore dest offset
	inc	di			;update to new draw position

dsp_draw_right_edge:
	test	bh, 02h 		;is there a right edge to draw?
	jz	dsp_reset_registers	;no.  Restore default settings
	push	di			;save updated dest offset
	add	di, gl_inner_loop_count ;go to the right hand edge
	mov	al, bptr gl_last_mask[1]
	out	dx, al			;get it to the board
	mov	cx, yExt

dsp_right_edge_draw_loop:
	mov	al, bl			;copy color index into AL
	xchg	al, es:[di]		;load latches, copy color index
	add	di, si
	loop	dsp_right_edge_draw_loop

	pop	di			;restore dest offset

dsp_reset_registers:
	mov	al, 0ffh		;allow writing to all bits in the byte
	out	dx, al			;this is the default value

dsp_draw_core_piece:
	mov	cx, gl_inner_loop_count
	jcxz	dsp_all_done
	sub	si, cx			;account for DI being incr. by stosb
	mov	dx, EGA_BASE+SEQ_DATA	;time to copy pattern to board	DX=3C5h
	mov	al, 01h
	mov	cx, 4

dsp_load_latches_loop:
	out	dx, al			;select the next plane to write to
	shr	bl, 1			;move plane bit into carry
	sbb	ah, ah			;expand into AH
	mov	es:[di], ah		;copy it to the bit plane
	shl	al, 1			;update plane selector
	loop	dsp_load_latches_loop	;do all 4 planes

	mov	al, MM_ALL		;to enable all four planes
	out	dx, al			;enable all planes

	mov	dx, EGA_BASE+GRAF_ADDR	;DX=3CEh
	mov	ax, GRAF_BIT_MASK	;AH=0 ie., copy data from latches, AL=8
	out	dx, ax			;ignore CPU data on write to board

	mov	al, es:[di]		;load the latches
	mov	dx, yExt		;initialize loop counter
	mov	bx, gl_inner_loop_count ;initialize rep counter value

dsp_pat_blt_loop:
	mov	cx, bx			;CX: repeat count
	REPSTOSB NOAHLOAD		;do all bytes in this scanline
	add	di, si			;point to next scanline
	dec	dx
	jnz	dsp_pat_blt_loop

dsp_all_done:
	ret				;we're done, go home, have a beer.
do_solid_patcopy    endp


;----------------------------Private-Routine----------------------------;
; do_wes_patblt
;
;
; Entry:
;	SS:BP --> BitBLT local variable frame
; Returns:
;	Nothing
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,DS,ES,flags
; Registers Preserved:
;	BP
; Calls:
;	calc_parms
;	edge_pat_blt
;	pat_blt
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assume	ds:nothing
	assume	es:nothing

do_wes_patblt	proc	near

	call	calc_parms

	mov	ah,byte ptr gl_start_mask[0]
	or	ah,ah
	jz	no_left_pat_edge
	pushem	si,bx
	call	edge_pat_blt		; preserves DI
	popem	si,bx
	inc	di

no_left_pat_edge:
	mov	dx,gl_inner_loop_count
	or	dx,dx
	jz	no_inner_pat_loop
	mov	cx,yExt
	pushem	di,si,bx
	call	pat_blt
	popem	di,si,bx
	add	di,gl_inner_loop_count

no_inner_pat_loop:
	mov	ah,byte ptr gl_last_mask[0]
	or	ah,ah
	jz	no_last_pat_edge
	mov	cx,yExt
	call	edge_pat_blt

no_last_pat_edge:
	ret

do_wes_patblt	endp


;----------------------------Private-Routine----------------------------;
; do_wes_mono_trick
;
;
; Entry:
;	SS:BP --> BitBLT local variable frame
; Returns:
;	Nothing
; Registers Preserved:
;	BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,DS,ES,flags
; Calls:
;	calc_parms
;	edge_mono_to_color_blt
;	mono_to_color_blt
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assume	ds:nothing
	assume	es:nothing

do_wes_mono_trick	proc near

ifdef THIS_IS_DOS_3_STUFF
else
; We need to use the image color, but cl_both_colors in the pat color.
; At this point no one else needs the pat color, so we can change
; cl_both_colors.

	lds	si,lpPBrush		; ddc_oem_brush
if FIREWALLS
	cmp	wptr ds:[si][-ddc_oem_brush],DC_IDENT
	jz	its_a_dc
	public	not_my_ddc
not_my_ddc:
	int	3
its_a_dc:
endif
	mov	ah,[si][ddc_image_color_ours      - ddc_oem_brush].SPECIAL
	mov	al,[si][ddc_image_back_color_ours - ddc_oem_brush].SPECIAL
	and	ax,MM_ALL shl 8 + MM_ALL
	mov	cl_both_colors,ax

endif
	call	calc_parms

ifdef	TEFTI
	timer_begin
endif

	push	dx
	mov	ah,byte ptr gl_start_mask[0]
	or	ah,ah
	jz	no_left_edge
	pushem	di,si,dx
	mov	al,gl_phase_h
	mov	bx,cl_both_colors
;-	mov	cx,yExt
	call	edge_mono_to_color_blt
	popem	di,si,dx
	inc	di
	inc	si

no_left_edge:
	mov	bx,dx
	mov	dx,gl_inner_loop_count
	mov	cx,di			; compute and save the right-hand edge
	add	cx,dx
	push	cx
	or	dx,dx
	jz	no_inner_loop
	sub	bx,dx
	mov	cx,yExt
	mov	al,gl_phase_h
	cbw
	push	bp
	mov	bp,cl_both_colors
	xchg	bp,ax
	call	mono_to_color_blt
	pop	bp

no_inner_loop:
	pop	di
	pop	dx
	mov	ah,byte ptr gl_last_mask[0]
	or	ah,ah
	jz	no_last_edge

	mov	cx,gl_inner_loop_count
	mov	si,gl_end_fl		; src_right_edge (reuse stk variable)
	mov	cx,yExt
	mov	bx,cl_both_colors
	mov	al,gl_phase_h
	call	edge_mono_to_color_blt

no_last_edge:
ifdef	TEFTI
	timer_end
endif
	ret

do_wes_mono_trick	endp


;----------------------------Private-Routine----------------------------;
; calc_parms
;
; To avoid conditional jumps we will use some sick optimizations.
; Remember this:
;	adc	ax,-1		; DEC AX if carry clear
;	sbb	ax,0		; DEC AX if carry set
;	sbb	ax,-1		; INC AX if carry clear
;	adc	ax,0		; INC AX if carry set
;
; Entry:
;	SS:BP --> BitBLT local frame
; Returns:
;	DS:SI set to upper left of bitmap or pattern
;	      UNLESS BITMAP IS > 64K in which case DS:SI points to the
;	      source bitmap structure.
; 	ES:DI set to upper left
;	DX = src bitmap width  FOR FULL SCAN (4x one scan if color)
; 	CX = yExt
; 	BX = offset into pattern (if pat present)
;	sets dest_right_edge
;	sets gl_start_mask[0]
;	sets gl_last_mask[0]
;	sets gl_inner_loop_count
; Registers Destroyed:
;	AX,flags
; Registers Preserved:
;	BP
; Alters:
;
; Calls:
;	None
;-----------------------------------------------------------------------;

	.errnz	SIZE_PATTERN - 8		; any power of 2 will work

	assume	ds:nothing
	assume	es:nothing

calc_parms	proc	near

;	The destination.

	mov	di,ScreenSelector
	mov	es,di

;	Left edge.

	mov	di,DestxOrg		; in pixels
	mov	bx,di
	mov	cl,7
	and	cx,di			; save lower 3 bits
	mov	gl_phase_h,cl
	shiftr	di,3			; convert to bytes
					; DI set for left edge
	mov	al,0FFh
	shr	al,cl
	mov	byte ptr gl_start_mask[0],al

;	Right edge.

	add	bx,xExt			; right edge in pixels
	mov	cl,7
	and	cl,bl			; save lower 3 bits
	shiftr	bx,3			; convert to bytes
	mov	gl_start_fl,bx		; dest_right_edge (reuse stk variable)
	mov	al,0FFh
	shr	al,cl
	not	al

;	Check if the BLT does not cross any byte boundaries.

	sub	bx,di			; make BX # bytes including left edge
	jnz	crosses_byte_boundary
	and	byte ptr gl_start_mask[0],al
	xor	al,al

;	There are 2 cases where we get zero for gl_inner_loop_count:
;	When the start and end bytes are adjacent and when they are
;	the same byte.  In the latter case we get -1 for
;	gl_inner_loop_count so INC BX now so it will be zero.

	inc	bx
crosses_byte_boundary:

	cmp	al,0FFh
	sbb	al,-1			; AL=FF -> AL=0 (put in innerloop)
	mov	byte ptr gl_last_mask[0],al

;	Inner loop  --  combine edge bytes into inner loop if they are
;	full bytes.

	mov	gl_end_fl,bx		; src_right_edge (reuse stk variable)
	mov	al,byte ptr gl_start_mask[0]
	cmp	al,0FFh

;	If gl_start_mask = FF the carry is clear, otherwise carry is set.
;	We want to DEC BX if carry set because we have already included
;	the left edge byte in BX, but we shouldn't have included it if
;	it's only a partial byte.

	sbb	bx,0
	cmp	al,0FFh

;	If gl_start_mask = FF the carry is clear, otherwise carry is set.
;	We want to INC AL (zero it) if it is FF (carry clear) because we
;	will do this edge as part of the innerloop.

	sbb	al,-1
	mov	byte ptr gl_start_mask[0],al

	mov	gl_inner_loop_count,bx
	mov	ax,SCREEN_W_BYTES
	mul	DestyOrg
	add	di,ax

;	The source.

	test	gl_the_flags,F0_SRC_PRESENT
	jz	no_source

	lds	si,lpSrcDev
	mov	cx, word ptr [si].bmWidthBytes
	test	gl_the_flags,F0_SRC_IS_COLOR
	jz	SHORT Source_Width_Correct
	shl	cx, 2
Source_Width_Correct:

;	Left edge.

	mov	bx,SrcxOrg
	mov	dl,7
	and	dl,bl			; get lower 3 bits ( Src Mod 8 )
	sub	gl_phase_h,dl		; phase def'd as Mod8[gl_dest]-
					;		 Mod8[gl_src]

	test	gl_src.dev_flags,SPANS_SEG ;Does the BLT span a segment?
	jnz	SHORT Source_Ptr_Set

	lds	si,[si].bmBits
	shr	bx, 3
	add	si,bx			; SI set for left edge
	mov	ax,cx
	mul	SrcyOrg
	add	si,ax
	add	gl_end_fl,si		; src_right_edge (reuse stack variable)

Source_Ptr_Set:
	mov	dx,cx
	jmp	short	no_pattern
no_source:
	test	gl_the_flags,F0_PAT_PRESENT ; assuming P or S but not both
	jz	no_pattern
	mov	ds,seg_lpPBrush
	mov	si,off_lpPBrush
	mov	bx,DestyOrg
	and	bx,SIZE_PATTERN - 1
no_pattern:
	mov	cx,yExt

ifdef	THIS_IS_DOS_3_STUFF
if	MASMFLAGS and DEBUG
	or	cx,cx
	jnz	yext_not_zero
yext_is_zero:
	int	3
yext_not_zero:
   endif
else
if	MASMFLAGS and DEBUG
    if FIREWALLS
	or	cx,cx
	jnz	yExt_not_zero
yExt_is_zero:
	ReportError	msg_YExtIsZero
yExt_not_zero:
    endif
   endif
endif
	ret
calc_parms	endp


;----------------------------Private-Routine----------------------------;
; mono_to_color_blt
;
; This does phase-0, byte-aligned, mem-mono to ega-color blt.
;
; The Problem: copy to the ega a bitmap where "0"s in the bitmap mean
; color1 and "1"s in the bitmap mean color2, where color1 and color2
; are arbitrary colors.
;
; The solution:
;
;			plane0	plane1	plane2	plane3
;
; color1		1	1	0	0
; color2		1	0	1	0
; SetResetEnable	1	0	0	1
; SetReset		0	x	x	0
; latches		1	1	0	0	(=color1)
;
; Now with datarot = XOR we get
;
;  when databit=0	1	1	0	0	(=color1)
;  when databit=1	1	0	1	0	(=color2)
;
;
; Entry:
;	BP = phase ( -7 to 7)	(high byte ignored)
;	AL = background color ( "1" bits in mono-bitmap )
;	AH = foreground color
;	BX = SI wrap
;	DS:SI = Mono Bitmap first byte
;	ES:DI = First EGA Byte
;	CX = Number of scan lines
;	DX = bytes per scan line
;	GRAF_DATA_ROT = DR_SET
;	All Planes Enabled
; Returns:
;	Nothing
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,BP,flags
; Registers Preserved:
;	DS,ES
; Alters:
;	GRAF_SET_RESET
;	GRAF_ENAB_SR
;	GRAF_BIT_MASK
; Calls:
;	None
;-----------------------------------------------------------------------;

	assume	ds:nothing
	assume	es:nothing

mono_to_color_blt	proc	near

	push	bp			; phase
	push	dx			; bytes per scan line
	push	bx			; wrap for SI
	mov	bx,ax			; colors

;	First we put the foreground color into the latches.  We do this
;	by putting this color into SET_RESET, writing it, then reading it.
;	The memory location we will use is the first byte where we will blt.

	mov	dx,EGA_BASE + GRAF_ADDR
	mov	al,GRAF_SET_RESET
	out16	dx,ax
	mov	ax,0F00h + GRAF_ENAB_SR
	out16	dx,ax

;	Set bit mask = FF.
	mov	ax,0FF00h + GRAF_BIT_MASK
	out16	dx,ax

;	Fill the latches.
	mov	es:[di],al		; color in SetReset is written, not AL
	mov	al,es:[di]		; read to fill latches

;	Go to XOR mode.
	mov	ax,GRAF_DATA_ROT + 256 * DR_XOR
	out16	dx,ax

;	Now setup SET_RESET.

	mov	ax,bx			; restore colors
	xor	ah,al			; gives 0 where colors match
	mov	al,GRAF_SET_RESET
	out16	dx,ax
	not	ah
	mov	al,GRAF_ENAB_SR
	out16	dx,ax			; enable Set/Reset where colors match

	pop	bp			; wrap for SI
	pop	dx			; bytes per scan
	mov	bx,SCREEN_W_BYTES
	sub	bx,dx			; BX = wrap

	mov	ax,cx			; loop count
	pop	cx			; phase
	or	cl,cl
	js	phase_neg
	jz	phase_zero
	dec	si
;*	dec	bp
pmono_to_color_loop:
	pushem	ax,dx
pnext_byte:
	lodsw
	dec	si
	xchg	al,ah
	shr	ax,cl
;+	shl	ax,cl
	stosb
	dec	dx
	jnz	pnext_byte
	popem	ax,dx
	add	di,bx
	add	si,bp
	dec	ax
	jnz	pmono_to_color_loop
	jmp	short	leave_in_set_mode

phase_zero:
zmono_to_color_loop:
	mov	cx,dx
	shr	cx,1
	rep	movsw
	rcl	cx,1
	rep	movsb
	add	di,bx
	add	si,bp
	dec	ax
	jnz	zmono_to_color_loop
	jmp	short	leave_in_set_mode

phase_neg:
	neg	cl			; make CX = abs phase
nmono_to_color_loop:
	pushem	ax,dx
nnext_byte:
	lodsw
	dec	si
	rol	ax,cl
;+	shr	ax,cl
	stosb
	dec	dx
	jnz	nnext_byte
	popem	ax,dx
	add	di,bx
	add	si,bp
	dec	ax
	jnz	nmono_to_color_loop

leave_in_set_mode:
	mov	dx,EGA_BASE + GRAF_ADDR
	mov	ax,GRAF_DATA_ROT + 256 * DR_SET
	out16	dx,ax

	ret

mono_to_color_blt	endp


;----------------------------Private-Routine----------------------------;
; edge_mono_to_color_blt
;
; This problem here is the same as in mono_to_color_blt, except it
; is complicated by the need to preserve what is already in EGA memory
; for part of the byte which we are writing.
;
; We will set the BIT MASK to preserve these bytes.  We will then read
; the data from memory, and write it to the EGA using an XCHG so the
; latches are filled before the write -- so the appropriate EGA bits
; are preserved.
;
; The method for writing the data involves two passes.	The first pass
; writes the data to some of the planes, the second pass writes NOT the
; data to the other planes.  Depending on the two colors involved we
; may be able to skip one of the two passes.
;
; Define BkColor = the color corresponding to "1" bits in the data.
; Define TextColor = the color corresponding to "0" bits in the data.
;
; We will use the Set/Reset register to take care of the planes where
; the colors match.  These planes will be ignored in the rest of this
; comment block.
;
; The first pass writes "1"s where the data is "1".  Therefore, the
; condition for doing the first pass is that the BkColor has a "1"
; somewhere (ignoring those planes taken care of by Set/Reset).
; The second pass does whatever planes remain.	We can skip this pass
; if no planes remain.	To maximize to likelihood of this we make sure
; that all "Set/Reset" planes are enabled on the first pass (if the
; first pass occurs).
;
; Entry:
;	AH = bitmask
;	AL = phase (-7 to +7)
;	BH = foreground color
;	BL = background color
;	DX = src bitmap width in bytes
;	DS:SI = Mono Bitmap first byte
;	ES:DI = First EGA Byte
;	CX = Number of scan lines
;	DATA_ROT = DR_SET
; Returns:
;	Nothing
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,flags
; Registers Preserved:
;	BP,DS,ES
; Alters:
;	GRAF_SET_RESET
;	GRAF_BIT_MASK
;	GRAF_ENAB_SR
; Calls:
;	None
;-----------------------------------------------------------------------;

	assume	ds:nothing
	assume	es:nothing

edge_mono_to_color_blt	proc	near

	push	bp
	push	ax		; AL = phase
	mov	bp,dx

;	Set bit mask.

	mov	dx,EGA_BASE + GRAF_ADDR
	mov	al,GRAF_BIT_MASK
	out16	dx,ax

;	Put foreground color in Set/Reset and enable planes where colors
;	match.

	mov	ah,bh
	mov	al,GRAF_SET_RESET
	out16	dx,ax
	xor	ah,bl
	not	ah			; gives 1 where colors match
	mov	al,GRAF_ENAB_SR
	out16	dx,ax
	mov	dx,EGA_BASE + SEQ_DATA	; The rest of the OUTs are here.
	mov	al,ah
	not	ah			; Gives 1 where colors mismatch.

;	The following AND leaves 1 bits in AH for the planes which
;	CANNOT be done on the second pass.  So if this is zero we can
;	skip the first pass.

	and	ah,bl			; BL = BkColor = color where data is 1
	or	ah,bl			; planes to enable
	mov	bx,cx			; we're done with the colors in BX
	pop	cx			; phase
	jz	skip_first_pass

	or	al,ah			; Include "Set/Reset" planes.
	out	dx,al			; Enable planes for first pass.

	pushem	cx,si,di,ax,bx
	or	cl,cl
	js	phase_is_negative1
	dec	si
pfirst_pass:
;*	mov	ax,[si]
	mov	al,[si]
	mov	ah,[si+1]

	ror	ax,cl
	xchg	ah,es:[di]
	add	si,bp
	add	di,SCREEN_W_BYTES
	dec	bx
	jnz	pfirst_pass
	jmp	short	end_pass_one
phase_is_negative1:
	neg	cl			; make CL = abs phase
nfirst_pass:
	mov	ax,[si]
	rol	ax,cl
	xchg	al,es:[di]
	add	si,bp
	add	di,SCREEN_W_BYTES
	dec	bx
	jnz	nfirst_pass
end_pass_one:
	popem	cx,si,di,ax,bx

skip_first_pass:

;	Enable the other planes.

	not	ah
	and	ah,MM_ALL
	jz	no_planes_left
	mov	al,ah
	out	dx,al

	or	cl,cl
	js	phase_is_negative2
	dec	si
psecond_pass:
;*	mov	ax,[si]
	mov	al,[si]
	mov	ah,[si+1]

	not	ax
	ror	ax,cl
	xchg	ah,es:[di]
	add	si,bp
	add	di,SCREEN_W_BYTES
	dec	bx
	jnz	psecond_pass
	jmp	short	no_planes_left

phase_is_negative2:
	neg	cl			; make CL = abs phase
nsecond_pass:
	mov	ax,[si]
	not	ax
	rol	ax,cl
	xchg	al,es:[di]
	add	si,bp
	add	di,SCREEN_W_BYTES
	dec	bx
	jnz	nsecond_pass

no_planes_left:
	mov	al,MM_ALL
	out	dx,al
	pop	bp
	ret

edge_mono_to_color_blt	endp


;----------------------------Private-Routine----------------------------;
; pat_blt
;			XOR mode with data = FF for Pn?
;
; This BLTs an arbitrary 8x8 bit pattern (3 or 4 planes deep) to EGA.
;
; The method is simple.  Load the latches with the pattern for a
; particular scan line, then REP STOS this with the BIT MASK = 0
; so that only the latches get written.	 Before putting the pattern
; for the next scan line into the latches we will do all other scan
; lines with the same pattern.
;
; Entry:
;	DS:SI = pattern bytes
;	ES:DI = First EGA Byte
;	CX = Number of scan lines (yExt)
;	BX = offset into pattern
;	DX = bytes per scan line (scan_len)
;	GRAF_DATA_ROT = DR_SET
;	BIT_MASK = FF
; Returns:
;	Nothing
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,flags
; Registers Preserved:
;	BP,DS,ES
; Alters:
;	GRAF_BIT_MASK	(leaves it 00)
; Calls:
;	None
;-----------------------------------------------------------------------;

	.errnz	SIZE_PATTERN - 8	; actually any power of 2 is okay.

	assume	ds:nothing
	assume	es:nothing

pat_blt	proc	near

	push	bp
	push	dx			; scan_len
	push	cx			; yExt

if NUMBER_PLANES eq 4
	mov	ah,11h			; left nibble gives carry to end loop
else
	mov	ah,21h
endif

;	Set BP = min(yExt, scans/pattern).

	sub	cx,SIZE_PATTERN		; SIZE_PATTERN = 8 = yExt of pattern
	sbb	bp,bp
	and	bp,cx
	add	bp,SIZE_PATTERN

	mov	dx,EGA_BASE + SEQ_DATA
set_next_plane:
	push	bx
	push	di

;	Enable next plane.

	mov	al,MM_ALL
	and	al,ah
	out	dx,al

	mov	cx,bp
hit_next_byte:
	mov	al,[si+bx]		; Next pattern byte
	inc	bx
	and	bx,SIZE_PATTERN - 1
	mov	es:[di],al
	add	di,SCREEN_W_BYTES
	loop	hit_next_byte
	add	si,SIZE_PATTERN
	pop	di
	pop	bx
	shl	ah,1
	jnc	set_next_plane

;	Set bit mask = 00.

	mov	dx,EGA_BASE + GRAF_ADDR
	mov	ax,0000h + GRAF_BIT_MASK
	out16	dx,ax

; 	Enable all planes.

	mov	al,MM_ALL
	mov	dx,EGA_BASE + SEQ_DATA
	out	dx,al

	mov	cx,bp			; MIN(yExt,SIZE_PATTERN)
	pop	bp			; yExt
	pop	ax			; scan_len
	mov	si,SCREEN_W_BYTES
	sub	si,ax			; SI = next_scan
	mov	bx,(SIZE_PATTERN - 1) * SCAN_BYTES
	add	bx,si			; BX = 7 * SCREEN_W_BYTES + next_scan

pat_blt_next_scan:
	push	cx
	mov	si,di			; save SI
	mov	dx,bp			; save yExt
	mov	cl,es:[di]		; load latches

pat_blt_loop:
	mov	cx,ax			; AX = scan_len
	REPSTOSB NOAHLOAD
	add	di,bx			; BX = 7 * SCREEN_W_BYTES + next_scan
	sub	bp,SIZE_PATTERN
	jg	pat_blt_loop

	mov	bp,dx
	dec	bp
	mov	di,si
	add	di,SCREEN_W_BYTES
	pop	cx
	loop	pat_blt_next_scan

	pop	bp
	ret

pat_blt	endp


;----------------------------Private-Routine----------------------------;
; edge_pat_blt
;
; Entry:
;	AH = bitmask
;	DS:SI = pattern bytes
;	ES:DI = First EGA Byte
;	CX = Number of scan lines (yExt)
;	BX = offset into pattern
;	DATA_ROT = DR_SET
; Returns:
;	Nothing
; Registers Destroyed:
;	AX,CX,DX,SI,flags
; Registers Preserved:
;	BX,DI,BP,DS,ES
; Alters:
;	GRAF_BIT_MASK	(leaves it FF)
; Calls:
;	None
;-----------------------------------------------------------------------;

	assume	ds:nothing
	assume	es:nothing

edge_pat_blt	proc	near

	push	bp

;	Set bit mask.

	mov	dx,EGA_BASE + GRAF_ADDR
	mov	al,GRAF_BIT_MASK
	out16	dx,ax

if NUMBER_PLANES eq 4
	mov	ah,11h			; left nibble gives carry to end loop
else
	mov	ah,21h
endif

	mov	dx,EGA_BASE + SEQ_DATA
	sub	si,SIZE_PATTERN
	mov	bp,cx

enable_next_plane:
	push	bx
	push	di
	mov	cx,bp			; yExt
	mov	al,MM_ALL
	and	al,ah
	out	dx,al
	add	si,SIZE_PATTERN

over_scans:
	mov	al,[bx+si]		; pattern fetch
	inc	bx
	and	bx,SIZE_PATTERN - 1	; 7
	.errnz	SIZE_PATTERN - 8	; any power of 2 works
	xchg	es:[di],al
	add	di,SCREEN_W_BYTES
	loop	over_scans

	pop	di
	pop	bx
	shl	ah,1
	jnc	enable_next_plane

;	Restore bitmask to default.

	mov	dx,EGA_BASE + GRAF_ADDR
	mov	ax,0FF00h + GRAF_BIT_MASK
	out16	dx,ax

	pop	bp
	ret

edge_pat_blt	endp


;----------------------------Private-Routine----------------------------;
; invert
;
; Inverts pixels in a rectangle on the display, by simply writing the
; memory to itself, letting the EGA hardware perform the XORing.
;
; Entry:
;	ES:DI = First EGA Byte
;	CX = Number of scan lines (yExt)
;	BX = scan line length in bytes
;	DATA_ROT = DR_XOR
;	GRAF_SET_RESET = color to xor DEST with
;	GRAF_SR_ENAB = MM_ALL
; Returns:
;	Nothing
; Registers Destroyed:
;	AX,CX,DX,SI,DI,DS,flags
; Registers Preserved:
;	BX,BP,ES
; Alters:
;	GRAF_BIT_MASK	(leaves it FF)
; Calls:
;	None
;-----------------------------------------------------------------------;

	assume	ds:nothing
	assume	es:nothing

invert	proc	near

;	Set bit mask.

	mov	dx,EGA_BASE + GRAF_ADDR
	mov	ax,0FF00h + GRAF_BIT_MASK
	out16	dx,ax

	mov	dx,SCREEN_W_BYTES
	sub	dx,bx

	mov	si,es			; prepare to use movsb
	mov	ds,si

invert_next_scan:
	mov	si,di
	mov	ax,cx			; save CX
	mov	cx,bx			; scan len in bytes
	rep	movsb
	add	di,dx
	mov	cx,ax			; restore CX
	loop	invert_next_scan

	ret

invert	endp


;----------------------------Private-Routine----------------------------;
; edge_invert
;
; Inverts one byte on each scan line vertically according to the mask
; in AH.
;
; Entry:
;	AH = bitmask
;	ES:DI = First EGA Byte
;	CX = Number of scan lines (yExt)
;	DATA_ROT = DR_XOR
;	GRAF_SET_RESET = color to xor DEST with
;	GRAF_ENAB_SR = MM_ALL
; Returns:
;	Nothing
; Registers Destroyed:
;	AL,CX,DX,DI,flags
; Registers Preserved:
;	AH,BX,SI,BP,DS,ES
; Alters:
;	GRAF_BIT_MASK
; Calls:
;	None
;-----------------------------------------------------------------------;

	assume	ds:nothing
	assume	es:nothing

edge_invert	proc	near

;	Set bit mask.

	mov	dx,EGA_BASE + GRAF_ADDR
	mov	al,GRAF_BIT_MASK
	out16	dx,ax
	jmp	$+2

edge_invert_next_scan:
	xchg	es:[di],al
	add	di,SCREEN_W_BYTES
	loop	edge_invert_next_scan

	ret

edge_invert	endp



;----------------------------Private-Routine----------------------------;
; edge_grey_dpx
;
; Inverts one or two bytes on each scan line vertically according
; to the grey pattern given, under the passed clipping mask.
;
; Entry:
;       BX     =  brush index (0-7)
;       CX     =  number of scan lines (cyExt)
;       DL     =  lhs clipping mask
;       DH     =  rhs clipping mask
;       DS:SI --> base address of brush
;       ES:DI --> rhs EGA
;       ES:BP --> lhs EGA
; Returns:
;       Nothing
; Registers Destroyed:
;       AX,BX,CX,DX,SI,DI,FLAGS
; Registers Preserved:
;       DS,ES
; Alters:
;       None
; Calls:
;       None
;-----------------------------------------------------------------------;

        assumes ds,nothing
        assumes es,nothing

        public  edge_grey_dpx
edge_grey_dpx proc      near

        sub     bp,di                   ;Compute delta to lhs
        and     bx,00000111b            ;Make sure brush is valid
        .errnz  SIZE_PATTERN - 8
        or      dh,dh                   ;Dispatch based on one or two edges
        jz      edge_grey_dpx_one_loop

edge_grey_dpx_both_loop:
        mov     al,[si][bx]             ;Get next byte of brush
        mov     ah,al
        and     al,dl                   ;Mask with lhs clipping mask
        xchg    al,es:[di]              ;Invert necessary bits
        inc     bx                      ;--> next brush byte
        and     bl,00000111b            ;Handle any wrap
        .errnz  SIZE_PATTERN - 8
        and     ah,dh                   ;Mask with rhs clipping mask
        xchg    ah,es:[di][bp]          ;Invert necessary bits
        add     di,SCREEN_W_BYTES       ;--> next destination byte
        loop    edge_grey_dpx_both_loop
        ret

edge_grey_dpx_one_loop:
        mov     al,[si][bx]             ;Get next byte of brush
        and     al,dl                   ;Mask with lhs clipping mask
        xchg    al,es:[di]              ;Invert necessary bits
        inc     bx                      ;--> next brush byte
        and     bl,00000111b            ;Handle any wrap
        .errnz  SIZE_PATTERN - 8
        add     di,SCREEN_W_BYTES       ;--> next destination byte
        loop    edge_grey_dpx_one_loop
        ret

edge_grey_dpx endp



;----------------------------Private-Routine----------------------------;
; middle_grey_dpx
;
; Inverts a rectangle on the display using the passed grey pattern.
;
; Entry:
;       BX     =  brush index (0-7)
;       CX     =  # byte to invert on the scan
;       DX     = EGA_BASE + GRAF_ADDR
;       DS:SI --> base address of brush
;       ES:DI --> starting byte
;       BP     =  number of scan lines (cyExt)
;       DATA_ROT       = DR_XOR
;       GRAF_SET_RESET = All 1
;       GRAF_ENAB_SR   = MM_ALL
; Returns:
;       Nothing
; Registers Destroyed:
;       AX,CX,DX,SI,DI,DS,flags
; Registers Preserved:
;       BX,BP,ES
; Alters:
;       GRAF_BIT_MASK   (leaves it FF)
; Calls:
;       None
;-----------------------------------------------------------------------;

        assumes ds,nothing
        assumes es,nothing

        public  middle_grey_dpx
middle_grey_dpx proc near
   ;    mov     dx,EGA_BASE + GRAF_ADDR ; Leave the Graphics controller
        mov     al,GRAF_BIT_MASK        ;   address register pointing to
        out     dx,al                   ;   the bitmask register
        inc     dx                      ; --> Graphics controller data register
        .errnz  GRAF_DATA - GRAF_ADDR - 1

        mov     ah,bl                   ; Keep brush index here
        mov     bx,cx                   ; Save a copy of inner loop count here

middle_grey_dpx_loop:
        xchg    ax,bx
        xchg    bl,bh
        and     bx,00000111b
        errnz   SIZE_PATTERN-8
        mov     bh,[si][bx]             ; Get next byte of brush
        inc     bl                      ; --> byte of the brush
        xchg    bh,bl
        xchg    ax,bx
        out     dx,al                   ;
        mov     cx,bx
        push    si
        mov     si,di
        rep     movs byte ptr es:[di],es:[si]
        pop     si
        sub     di,bx
        add     di,SCREEN_W_BYTES	;next scan on screen
        dec     bp
        jnz     middle_grey_dpx_loop
        ret

middle_grey_dpx endp



;----------------------------Private-Routine----------------------------;
; do_grey_dpx
;
; This is EGA special cased code for the dpx raster op in the case where
; the pattern (p) is grey (the same on all planes).  We also come here
; for the special graying pattern rop.  This is not a normal P,S, and D rop,
; but a hack for pmwin.  It allows graying of things on the screen, meaning
; the background color is stuffed everywhere the pattern has a "1" bit.
; 
; Entry:
;       SS:BP --> BitBLT local variable frame
; Returns:
;       Nothing
; Registers Destroyed:
;       AX,BX,CX,DX,SI,DI,DS,ES,flags
; Registers Preserved:
;       BP
; Calls:
;       calc_parms
;       edge_grey_dpx
;       middle_grey_dpx
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

        assumes ds,nothing
        assumes es,nothing

        public  do_grey_dpx
do_grey_dpx     proc    near

        call    calc_parms

; DS:SI set to upper left of bitmap or pattern
; ES:DI set to upper left
; DX = src bitmap width  (if present)
; CX = yExt
; BX = offset into pattern (if pat present)
; sets dest_right_edge
; sets gl_start_mask[0]
; sets gl_last_mask[0]
; sets gl_inner_loop_count

        mov     dx,EGA_BASE + GRAF_ADDR
        mov     ax,DR_XOR shl 8 + GRAF_DATA_ROT ; XOR mode for grey dpx
        out16   dx,ax
ega_setup_done:

        mov     dl,byte ptr gl_start_mask[0]
        mov     dh,byte ptr gl_last_mask[0]
        or      dx,dx
        jz      do_grey_dpx_middle      ;Only middle bytes exist

        mov     ax,di                   ;Assume we have a left edge
        inc     ax                      ;+1 to get to start of middle bytes
        or      dl,dl                   ;Is there really a left edge?
        jnz     do_grey_dpx_have_lhs    ;  Yes, AX = correct middle byte start
        dec     ax                      ;  No, restore middle byte start
        xchg    dl,dh                   ;  Pretend only a lhs edge
        add     di,gl_inner_loop_count  ;    but make it be the rhs

do_grey_dpx_have_lhs:
        push    ax                      ;Save middle bytes start
        pushem  bx,cx,si,bp
        add     ax,gl_inner_loop_count  ;--> possible rhs
        xchg    ax,bp
        call    edge_grey_dpx
did_gray_pat_edge:
        popem   bx,cx,si,bp
        pop     di                      ;Restore middle bytes start

do_grey_dpx_middle:
        mov     ax,gl_inner_loop_count
        or      ax,ax
        jz      do_grey_dpx_exit
        push    bp
        mov     bp,cx
        xchg    ax,cx
        mov     dx,EGA_BASE + GRAF_ADDR         ;
        mov     ax,MM_ALL shl 8 + GRAF_ENAB_SR  ; Set Set/Reset to all "1" bits
        out16   dx,ax                           ; and Enable all planes (for
        .errnz  MM_ALL - 0Fh                    ; grey_dpx)
        mov     al,GRAF_SET_RESET               ;
        out16   dx,ax                           ;
gray_pat:

        call    middle_grey_dpx
        pop     bp

do_grey_dpx_exit:

        ret
do_grey_dpx     endp



endif	;GEN_COLOR_BLT
