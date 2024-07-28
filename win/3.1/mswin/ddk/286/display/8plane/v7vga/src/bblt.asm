
;
;	FILE:	bblt.asm
;	DATE:	9/1/90
;	AUTHOR: Jim Keller
;
;	This module contains the entry point for the bitblt driver function
;	as well as several support routines.
;

SRCFILE_BBLT	equ	1
incLogical	=	1
incDrawMode	=	1

?CHKSTK 	 = 1
?CHKSTKPROC	macro
		endm

include cmacros.inc
include gdidefs.inc
include macros.mac
include display.inc
include cursor.inc
include njumps.mac
include bblt.inc
include bltutil.inc
include vgareg.inc
include vgautil.inc
include blt11.inc
include blt18.inc
include blt81.inc
include blt88.inc
include blt216.inc
include bltstos.inc
include bltpat.inc

externFP PrestoChangeoSelector	; CS <--> DS conversion (hopefully CS <-> SS)
EXTRN	unexclude_far:FAR
EXTRN	exclude_far:FAR
EXTRN   PaletteModified:byte
EXTRN	set_bank_select:near

sBegin	Data
EXTRN   ScratchSel:word
EXTRN	enabled_flag:byte
EXTRN	PaletteTranslationTable:byte
EXTRN	PaletteIndexTable:byte
EXTRN	adPalette:byte
EXTRN	bank_select:byte
EXTRN	ScreenSelector:word
sEnd    Data

.286
sBegin	Code
assumes cs,Code
assumes ds,Data
assumes es,Nothing
assumes ss,Nothing
include bitblt.var
include ropdefs.blt
include roptable.blt

externW _cstods

cProc   BITBLT,<FAR,PUBLIC,PASCAL>,<>
cBegin  nogen
        push    bp
        mov     bp,sp

        ?CHKSTKPROC BLT_STACK_SIZE+40h  ; leave room for si,di,ds + rsvrptrs + slop
        jnc     blt_stack_ok
        jmp     blt_exit
blt_stack_ok:

        push    ds
        mov     ds,[_cstods]
        push    si
        push    di

        WriteAux    <'Bitblt'>
	call	bitblt_prep_rop_etal
 	jc	bitblt_error

	mov	ax,device_flags
	and	al,DEVFLAG_BOTH_SCREEN
	je	@F
	test	byte ptr local_enable_flag,0FFH
	jne	@F
	sub	ax,ax
	jmp	blt_noscreen

@@:	test	rop_flags,SOURCE_PRESENT
	je	@F
	call	bitblt_prep_src 		;if carry set on return, then
	jc	bitblt_done			; entire blt was clipped out

@@:	cmp	word ptr xExt,0
	je	bitblt_done
	cmp	word ptr yExt,0
	je	bitblt_done
	mov	ax,SrcxOrg			;keep these around in case of
	mov	src_left_edge,ax		; exclusion rect. Couldn't do
	mov	ax,SrcyOrg			; it earlier because prep_src
	mov	src_top_edge,ax 		; clips input to the screen.
	mov	ax,DestxOrg			; Must do before set_directions
	mov	dst_left_edge,ax		; which may modify SrcxOrg,
	mov	ax,DestyOrg			; SrcyOrg, etc.
	mov	dst_top_edge,ax

	call	bitblt_prep_dst
	call	blt_set_directions		;set x,y blt directions

	lea	ax,pattern_space		;just in case pattern offset
	mov	pattern_offset,ax		; is accessed later
        test    rop_flags,PATTERN_PRESENT
	je	@F
	call	bitblt_prep_pattern		;if carry set on return, there

        jc      bitblt_error                    ; was problem with the pattern

@@:	call	bitblt_prep_final
	call	bitblt_dispatch
	jnc	bitblt_done
	call	bitblt_execute

bitblt_done:
	mov	ax,1				;indicate success
	push	ax
	jmp	short @F

bitblt_error:
        sub     ax,ax                           ;indicate error
	push	ax

@@:	mov	ax,device_flags
	and	al,DEVFLAG_BOTH_SCREEN
	je	short bitblt_noreg_restore

	call	unexclude_far

	mov	al,SET_RESET_FOREGROUND or WRITE_MODE_0
	mov	ah,FUNC_COPY or SEQ_CHAIN_4_MODE
	call	vga_set_features
	mov	dx,VGAREG_SQ_ADDR
	mov	ax,VGAREG_SQ_MAP_MASK or (0FH SHL 8)
	out	dx,ax

	mov	al,VGAREG_SQ_SPLITBANK		;enable splitbank addressing
	out	dx,al
	inc	dx
	in	al,dx
	or	al,80H
	out	dx,al

	dec	dx
	mov	ax,VGAREG_SQ_SRC_SPLITBANK	;reset the split banks to 0
	out	dx,ax				;splitbank offsets are added
	mov	al,VGAREG_SQ_DST_SPLITBANK	; in even if splitbanking is
	out	dx,ax				; disabled (chip bug! - VRAMII)

	mov	al,VGAREG_SQ_SPLITBANK		;disable splitbank addressing
	out	dx,al
	inc	dx
	in	al,dx
	and	al,7FH
	out	dx,al

	mov	dl,global_bank
	call	set_both_pages

bitblt_noreg_restore:
	pop	ax

blt_noscreen:
	WriteAux <'Bitblt-out'>
        cld

        pop     di
        pop     si
        pop     ds
        mov     sp,bp
blt_exit:
        pop     bp
	retf	0020H

cEnd    nogen



;
;	bitblt_prep_rop_etal
;
;	This routine sets some flags dealing with the rop and stores the
;	offset of the rop template and the rop length on the stack. It
;	also sets a few other variables that I can't really find a place
;	for elsewhere but need to get set right away.
;

PUBLIC	bitblt_prep_rop_etal
bitblt_prep_rop_etal	PROC	NEAR

	sub	ax,ax			;assume one of first 128 rops
	mov	bx,[bp + 10H]
	or	bl,bl
	jns	@F
	or	ah, HIGH NEGATE_NEEDED
	not	bl
@@:	add	bx,bx
	xor	ax,roptable[bx] 	;get the rop flags
	mov	rop_flags,ax		;store them on the stack

	mov	bl,ah			;compute length of rop code template
	and	bx,HIGH ROPLength
	shr	bx,2
	mov	cl,byte ptr roptable[bx + 256]	;table of lengths
	sub	ch,ch
	mov	bx,ax			;compute offset of rop code template
	and	bx,ROPOffset
	mov	rop_offset,bx
	mov	rop_length,cx

	mov	ax,ScratchSel
        mov     WorkSelector,ax
        mov     my_data_seg,ds
        mov     al,enabled_flag
	mov	local_enable_flag,al
	mov	al,bank_select
	mov	global_bank,al
	mov	al,Video_Board_Flags
	mov	local_board_flags,al

	les	di,lpDrawMode			;these may not be needed but
	mov	ah,es:[di].bkColor.pcol_fb	; putting them on the stack
	mov	al,es:[di].TextColor.pcol_fb	; now can save a lot of hassle
	mov	mono_bits,ax			; and segment reloads later
	mov	ah,es:[di].bkColor.pcol_Clr	; if they are needed
        mov     al,es:[di].TextColor.pcol_Clr
	mov	colors,ax
	mov	word ptr background_mode,OPAQUE
	mov	ax,es:[di].bkMode
	cmp	ax,3
	jne	@F
	mov	word ptr background_mode,TRANSPARENT
@@:

	mov	ax,ScreenSelector
	mov	src_blt_segment,ax	;We may not have Data Segment available
	mov	dst_blt_segment,ax	; later. If the blt is not the Screen
	sub	ax,ax			; then these will be changed later
	mov	src_blt_offset,ax
        mov     dst_blt_offset,ax
	mov	pattern_row_counter,ax	; used in blts with color patterns
	cmp	PaletteModified,0FFH
	jne	@F
	or	al,DEVFLAG_PALETTE_MODIFIED

@@:
	lds	si,lpSrcDev
        les     di,lpDestDev
	test	rop_flags,SOURCE_PRESENT
	je	bitblt_prep_rop_src_done

	mov	bx,ds		
	or	bx,si
	jz	BPRE_ErrorExit

	cmp	[si].bmType,1
	jc	@F
	or	al,DEVFLAG_SRC_SCREEN or DEVFLAG_SRC_COLOR
@@:	cmp	[si].bmBitsPixel,08H
	jne	bitblt_prep_rop_src_done
	or	al,DEVFLAG_SRC_COLOR

bitblt_prep_rop_src_done:
	cmp	es:[di].bmType,1
	jc	@F
	or	al,DEVFLAG_DST_SCREEN or DEVFLAG_DST_COLOR
@@:	cmp	es:[di].bmBitsPixel,08H
	jne	@F
	or	al,DEVFLAG_DST_COLOR
@@:	mov	device_flags,ax
	clc	
	ret

BPRE_ErrorExit:
	mov	word ptr device_flags,0
	stc	
	ret


bitblt_prep_rop_etal	ENDP			;

;
;	bitblt_prep_pattern
;
;	This routine checks the brush for validity and then converts
;	the brush to color or mono as specified by the dst type. The
;	resulting brush is then rotated and placed on the stack on a
;	64 byte boundary -- i.e. an address equal to 0 modulo 64.
;

PUBLIC	bitblt_prep_pattern
bitblt_prep_pattern	PROC	NEAR

	lds	si,lpPBrush
	mov	ax,ds
	or	ax,si				;if the brush is NULL then
	je	bitblt_prep_pattern_error	; there is an error
	mov	al,[si].oem_brush_accel
	mov	brush_accel_flags,ax
	mov	ax,[si].oem_brush_style
	cmp	ax,BS_HOLLOW			;if the brush is hollow then
	je	bitblt_prep_pattern_error	; there is an error

        lea     di,pattern_space
        add     di,SIZE_PATTERN * SIZE_PATTERN - 1
        and     di,NOT (SIZE_PATTERN * SIZE_PATTERN - 1)
        mov     pattern_offset,di
	lea	di,pattern_work

	mov	bx,ss
	mov	es,bx
	cmp	ax,BS_SOLID			;solid and hatched brushes
	je	bitblt_prep_pattern_copy	; go exactly as they are since
	cmp	ax,BS_HATCHED			; they are created in both hues
	je	bitblt_prep_pattern_copy	; but they may need rotation

	test	byte ptr [si].oem_brush_accel,GREY_SCALE
	je	bitblt_prep_color_pattern
	test	word ptr device_flags,DEVFLAG_DST_COLOR
	jne	@F
	mov	bx,mono_bits
        call    brush_convert_mono_to_mono
	jmp	short	bitblt_prep_pattern_rotate
@@:	mov	bx,colors
	call	brush_convert_mono_to_color
        jmp     short   bitblt_prep_pattern_rotate

bitblt_prep_color_pattern:
	test	word ptr device_flags,DEVFLAG_DST_COLOR
	jne	@F
	mov	bx,my_data_seg
	call	brush_convert_color_to_mono
	jmp	short	bitblt_prep_pattern_rotate
@@:	call	brush_convert_color_to_color
	jmp	short	bitblt_prep_pattern_rotate

bitblt_prep_pattern_error:
        stc
        ret

bitblt_prep_pattern_copy:
	mov	cx,SIZE_PATTERN * SIZE_PATTERN
	test	word ptr device_flags,DEVFLAG_DST_COLOR
	jnz	@F
	mov	cx,SIZE_PATTERN
	add	si,oem_brush_mono
@@:	rep	movsb

bitblt_prep_pattern_rotate:
	mov	bx,0707H			;assume the worst that we must
	test	word ptr device_flags,DEVFLAG_DST_SCREEN     ; byte align the
	je	@F				; pattern in the x direction
	test	rop_flags,SOURCE_PRESENT	;if the latches can be used
	jne	@F				; we only need to order the
	mov	bx,0704H			; nibbles in the x direction
	test	word ptr local_board_flags,BOARD_FLAG_ENABLE_216
	je	@F			;if a 216 is available, no need to
	mov	bx,0700H		; rotate, since 8 bytes can be done
					; in a single cycle
@@:	mov	ax,es
	mov	ds,ax
	lea	si,pattern_work
	mov	di,pattern_offset
	mov	ch,byte ptr DestyOrg
	mov	cl,byte ptr DestxOrg
	and	cx,bx
	test	word ptr device_flags,DEVFLAG_DST_COLOR  ;set Z flag for color
	call	pattern_rotate		; pattern rotate, clear Z flag if mono

bitblt_prep_pattern_done:
	lds	si,lpSrcDev			;restore these pointers
	les	di,lpDestDev
	clc
	ret

bitblt_prep_pattern	ENDP		;

;
;	bitblt_prep_src
;
;	This routine does almost everything necessary to prepare the src
;	for the blt. It does not compute the start address, however. Since
;	it is not yet known whether the src and dst overlap, the X and Y
;	directions of the blt are not yet determined and hence the starting
;	coords are unknown.
;
;	RETURNS:
;	CARRY	CLEAR then bltting should continue
;		SET   bitblt should terminate because it was totally clipped
;			(this is not an error, but there is nothing to blt!)

PUBLIC	bitblt_prep_src
bitblt_prep_src PROC	NEAR

	call	blt_clip
	jc	bitblt_prep_src_done

	mov	ax,[si].bmFillBytes		;copy a few things to the stack
	mov	src_fill_bytes,ax
	mov	ax,[si].bmWidthBytes
	mov	src_width_bytes,ax
	mov	ax,xExt 			;assume a color src: thus the x
	mov	src_xExt_bytes,ax		; extent of the blt IN BYTES is

	test	word ptr device_flags,DEVFLAG_SRC_COLOR  ; the same as the xExt
	jne	@F
	mov	bx,SrcxOrg			;for mono blts, the x extent
	lea	ax,[bx + 07H]			; IN BYTES!! is equal to
	add	ax,xExt 			; [(x + xExt + 7) / 8 - x / 8]
	shr	ax,3				;Any partial byte on the left
	shr	bx,3				; or right edge is considered
	sub	ax,bx				; a whole byte for this purpose
	mov	src_xExt_bytes,ax
	mov	src_more_than_1byte,ax

	mov	ax,SrcxOrg			;the align_rotate_count serves
	and	ax,07H				; different purposes depending
	test	word ptr device_flags,DEVFLAG_DST_COLOR  ; upon both src hue and dst hue
	jne	@F
	mov	bx,DestxOrg			;see the actual compiling code
	and	bx,07H				; to determine the purpose of
	sub	bx,ax				; align_rotate_count for a
	mov	ax,bx				; given src,dst hue combination
@@:	mov	align_rotate_count,ax		; cases
	clc

bitblt_prep_src_done:
	ret

bitblt_prep_src ENDP				;

;
;	bitblt_prep_dst
;
;	This routine does almost everything necessary to prepare the dst
;	for the blt. It does not compute the start address, however. Since
;	it is not yet known whether the src and dst overlap, the X and Y
;	directions of the blt are not yet determined and hence the starting
;	coords are unknown.
;
;	RETURNS:
;	CARRY	SET	the dst is the screen which was not enabled - ERROR
;		CLEAR	evreything's cool -- proceed with the blt
;

PUBLIC	bitblt_prep_dst
bitblt_prep_dst PROC	NEAR

	mov	ax,es:[di].bmFillBytes		;copy a few things to the stack
	mov	dst_fill_bytes,ax
	mov	ax,es:[di].bmWidthBytes
	mov	dst_width_bytes,ax
	mov	ax,xExt 			;assume a color dst: thus the x
	mov	dst_xExt_bytes,ax		; extent of the blt IN BYTES is
	mov	inner_loop_count,ax		; the same as the xExt

	test	word ptr device_flags,DEVFLAG_DST_COLOR
	jne	@F
	mov	bx,DestxOrg			;for mono dst, we need to get
	mov	cx,ax				; the left and right edge masks
	call	get_edge_masks_bytes		;the dst x extent IN BYTES =
	mov	first_edge_mask,ax		; the inner_loop_count +
	mov	last_edge_mask,bx		; 1 (if firstedge mask not 0) +
	mov	inner_loop_count,cx		; 1 (if secondedgemask not 0)
	cmp	ah,0FFH 			;the second edge mask is NEVER
	adc	cx,1				; zero -- see get_edge_mask
	mov	dst_xExt_bytes,cx		; for details

	test	rop_flags,SOURCE_PRESENT
	je	bitblt_prep_dst_done
	test	word ptr device_flags,DEVFLAG_SRC_COLOR
	je	bitblt_prep_dst_done		;align_rotate_count serves
	mov	ax,DestxOrg			; different purposes depending
	add	ax,xExt 			; on the hue of the src and dst
	and	ax,07H				;this computation is for color
	je	zero_alignment
	neg	ax				; to mono blts. blt_src_prep
	add	ax,8				; has computations for other

public  zero_alignment
zero_alignment:
	mov	align_rotate_count,ax		; cases

@@:	test	word ptr device_flags,DEVFLAG_DST_SCREEN
	je	bitblt_prep_dst_done
	mov	bx,DestxOrg			; planar mode. When the dst
	mov	cx,xExt 			; is the screen, this is almost
	call	get_edge_masks_nibbles		; always the case (think about
	mov	byte ptr first_edge_mask,bl	; src copy and pattern fills.)
	mov	byte ptr last_edge_mask,bh
	mov	nibble_count,cx

bitblt_prep_dst_done:
	ret

bitblt_prep_dst ENDP				;

;
;	bitblt_prep_final
;
;	This routine does the last little bit of preparation that can be done
;	before calling blt specific routines. Src and dst start address are
;	computed IF the bitmaps are memory devices. (Since screen blts may use
;	wierd VGA addressing modes, screen address calculations are not done
;	here, although the screen selector is stored in the src and dst blt
;	segment variable.) Also, if the src is memory and the dst is the screen
;	or vice versa, the appropriate translate table is copied to the stack
;	for easier access in the bltting routines.
;

PUBLIC	bitblt_prep_final
bitblt_prep_final	PROC	NEAR

	test	rop_flags,SOURCE_PRESENT	;first see if the src is used
	je	@F
	test	word ptr device_flags,DEVFLAG_SRC_SCREEN
	jne	@F
        mov     ax,SrcyOrg                      ;if src is used and is memory
        mov     bx,SrcxOrg                      ; compute the starting address
	call	set_memory_address
        mov     src_blt_offset,ax
        mov     src_blt_segment,dx

@@:	test	word ptr device_flags,DEVFLAG_DST_SCREEN
	jne	@F
	mov	ax,DestyOrg			;if the dst is memory then
        mov     bx,DestxOrg                     ; compute the starting memory
	lds	si,lpDestDev
	call	set_memory_address		; address
	lds	si,lpSrcDev
        mov     dst_blt_offset,ax
        mov     dst_blt_segment,dx

@@:	mov	ax,device_flags
	and	al,DEVFLAG_BOTH_SCREEN
	jne	@F
	jmp	bitblt_prep_final_done

@@:	mov	bx,src_left_edge		;if we are using a software
	mov	ax,src_top_edge 		; cursor it has to be excluded
	mov	dx,dst_left_edge		;the device flags argument lets
	mov	cx,dst_top_edge 		; get_exclusion_region know
        mov     di,xExt                         ; to use the src rect, the dst
        mov     si,yExt                         ; rect or their union when
	push	word ptr device_flags		; computing the exclusion area
        call    get_exclusion_region
	mov	cx,bx
	mov	dx,ax
	xchg	si,di
	call	exclude_far
        pop     ax                              ;remove device flags parm

final_no_exclude:
	mov	ax,device_flags
	test	ax,DEVFLAG_PALETTE_MODIFIED
        jz      bitblt_prep_final_done

        and     ax,DEVFLAG_DST_SCREEN+DEVFLAG_SRC_SCREEN
        jz      bitblt_prep_final_done

        cmp     ax,DEVFLAG_DST_SCREEN+DEVFLAG_SRC_SCREEN
        je      bitblt_prep_final_done

        mov     si,DataOFFSET PaletteIndexTable
        test    ax,DEVFLAG_DST_SCREEN
        jz      bitblt_prep_copy_translate
        mov     si,DataOFFSET PaletteTranslationTable

bitblt_prep_copy_translate:
        mov     ds,my_data_seg
        assumes ds,Data
        mov     ax,ss
	mov	es,ax
	lea	di,color_xlat_table
        mov     cx,256/2
        rep     movsw
        lds     si,lpSrcDev
        les     di,lpDestDev
        or      word ptr device_flags,DEVFLAG_XLAT_REQUIRED ; compile code flag
        assumes ds,nothing

bitblt_prep_final_done:
	clc
	ret

bitblt_prep_final       ENDP

;
;	bitblt_dispatch
;
;	This routine dispatches to the correct blt code for the particular
;	case at hand. If the blt is a compiled blt then this function will
;	also do the control transfer to the code generated on the stack.
;

PUBLIC	bitblt_dispatch
bitblt_dispatch PROC	NEAR

        xor     ax,ax
        mov     cblt_code_end, ax
	lea	di,cblt_code
        mov     ax,ss
        mov     es,ax

	test	word ptr device_flags,DEVFLAG_DST_SCREEN ;blts to screen can
	je	dispatch_dst_memory		; be handled quicker by hard-
	test	rop_flags,SOURCE_PRESENT	; code, if only the dst and a
	jne	dispatch_src_to_screen		; pattern (but no src) are
	cmp	word ptr background_mode,OPAQUE ; are involved and if bgmode
	je	@F				; is OPAQUE because the vga
	jmp	dispatch_blt88			; hardware can be used
@@:	call	blt_pat_dst			
	jmp	bitblt_hardcode_done

dispatch_src_to_screen:

	test	word ptr device_flags,DEVFLAG_SRC_SCREEN  ;if both src and dst
	je	dispatch_memory_to_screen	; are the screen and we are

	test	word ptr local_board_flags,BOARD_FLAG_ENABLE_216  ;on a 216
	je	@F				; and the rop
	mov	bl,byte ptr Rop[2]		; only involves the src and
	mov	bh,bl				; dst, then the 216 hardware
	and	bx,0FF0H			; can handle it
	shr	bl,4
	cmp	bl,bh
	jne	dispatch_true_rop3
        cmp     word ptr background_mode,OPAQUE
	jne	dispatch_mode_xpar

	call	blt216_src_dst
	jmp	short bitblt_hardcode_done

@@:	cmp	byte ptr Rop[2],11001100B	; scr to scr but no 216
	jne	dispatch_true_rop3		; but we will still use
	cmp	word ptr background_mode,OPAQUE ; latches to do a srccopy
	jne	dispatch_mode_xpar		; if opaque mode and
	mov	al,byte ptr SrcxOrg		; nibble aligned.
	mov	ah,byte ptr DestxOrg
	and	ax,0303H
        cmp     al,ah
	jne	dispatch_true_rop3
	call	bltstos_srccopy
        jmp     short   bitblt_hardcode_done

dispatch_true_rop3:
dispatch_mode_xpar:
	call	bltstos 			;compiling screen_to_screen blt
	jmp	short	bitblt_dispatch_done	;code which is still quite fast

dispatch_dst_memory:

	test	word ptr device_flags,DEVFLAG_DST_COLOR
	jnz	dispatch_dst_color_memory
	test	rop_flags,SOURCE_PRESENT	;if src is not used in the blt
	jz	short	dispatch_blt81		; or the blt has a color src
	test	word ptr device_flags,DEVFLAG_SRC_COLOR  ;use blt_color_to_xxxx
	jnz	short	dispatch_blt81		; which is faster code than
	jmp	short	dispatch_blt11

dispatch_dst_color_memory:

        test    rop_flags,SOURCE_PRESENT        ;if src is not used in the blt
        jz      short   dispatch_blt88          ; or the blt has a color src

dispatch_memory_to_screen:

        test    word ptr device_flags,DEVFLAG_SRC_COLOR  ;use blt_color_to_xxxx
        jnz     short   dispatch_blt88          ; which is faster code than
        jmp     short   dispatch_blt18          ; than blt_mono_to_xxxx

dispatch_blt11:
	call	blt11
	jmp	short	bitblt_dispatch_done

dispatch_blt18:
	call	blt18
	jmp	short	bitblt_dispatch_done

dispatch_blt81:
	call	blt81
	jmp	short	bitblt_dispatch_done

dispatch_blt88:
	call	blt88

bitblt_dispatch_done:
        ;
        ; check for overflow of the cblt_code buffer, if we did
        ; then all our locals are corrupt, just get the hell out fast!
        ;
        xor     ax,ax
        cmp     cblt_code_end, ax
        je      bitblt_dispatch_exit
ifdef DEBUG
        int 3
endif
        pop     bx                              ; remove return addr.
        jmp     blt_noscreen                    ; jmp directly to BitBlt exit

bitblt_dispatch_exit:
	stc					;flag that blt was compiled
	ret					; but not executed

bitblt_hardcode_done:
	clc					;flag that blt was executed
	ret

bitblt_dispatch ENDP				;




;
;	bitblt_execute
;
;	This routine dispatches to the correct blt code for the particular
;	case at hand. If the blt is a compiled blt then this function will
;	also do the control transfer to the code generated on the stack.
;

PUBLIC	bitblt_execute
bitblt_execute	PROC	NEAR

	push	bp				;save bp
	push	cs				;put return addr on stack
        lea     ax,bitblt_execute_retaddr
        push    ax

	mov	ax,WorkSelector
	cCall	PrestoChangeoSelector,<ss,ax>
	push	ax				;push compiled code segment
        lea     ax,cblt_code
        push    ax                              ;push the compiled code offset

	cld
	or	word ptr dst_xExt_bytes,0
	jns	@F
	std

@@:	lds	si,dword ptr src_blt_offset	;src address -- if src is used
	les	di,dword ptr dst_blt_offset	;dst address
        lea     bx,color_xlat_table             ;may be needed
        sub     bp,bp                           ;pattern pointer
	retf					;Call the compiled code!!!

bitblt_execute_retaddr: 			;here's where we return from
	pop	bp				; the compiled code -- amazing!
	ret

bitblt_execute	ENDP

sEnd    CODE

END
