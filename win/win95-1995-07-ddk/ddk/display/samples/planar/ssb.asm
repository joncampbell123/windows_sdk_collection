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

	.286
;-----------------------------Module-Header-----------------------------;
; Module Name:	SSB.ASM
;
;   This module contains the SaveScreenBitmap routine.
;
; Exported Functions:	SaveScreenBitmap
;
; Public Functions:	none
;
; Public Data:		none
;
; General Description:
;
;	The subroutine SaveScreenBitmap saves a single bitmap from
;	the display to unused display memory, or restores or discards
;	a bitmap from otherwise unused memory to the display.  It
;	returns an error if the memory is either nonexistent, already
;	in use, or has been changed since the last save.
;
;	If the extra memory is being used by another program, the 
;	"SHADOW_EXISTS" bit will be turned off.  When it becomes free
;	again, the "SHADOW_TRASHED" and "SHADOW_EXISTS" bits will be set.
;	Thus, whenever the "SHADOW_EXISTS" bit is set, the memory is
;	available for use by this function.
;
; Restrictions:
;
;	Only one bitmap can be saved at a time.
;
;	On a VGA, the save area is 24575 bytes in size.  As a comparison,
;       the visible portion of screen memory is 38400 bytes.
;
;	On an EGA, the save area is 32767 bytes in size.  As a comparison,
;       the visible portion of screen memory is 28000 bytes.
;-----------------------------------------------------------------------;
	include	cmacros.inc
 	include mflags.inc
	include	ega.inc
	include	egamem.inc
	include	cursor.inc
ifdef TEFTI
	%out TEFTI timing code is present. Remove before RELEASE!!
	include	tefti.inc
endif
        externFP exclude_far            ; exclude cursor from save area
        externFP unexclude_far          ; redraw cursor
	externA	ScreenSelector
	externA	SCREEN_W_BYTES
	externA	SSB_SIZE_IN_BYTES
	externA	SSB_START_OFFSET
	externA	SSB_END_OFFSET

;-----------------------------------------------------------------------;
; Allowable values for the command parameter to SaveScreenBitmap.
;-----------------------------------------------------------------------;
SSB_SAVE	equ	0
SSB_RESTORE	equ	1
SSB_IGNORE	equ	2

;-----------------------------------------------------------------------;
; Allowable states of the shadow_mem_status variable.
;-----------------------------------------------------------------------;
SHADOW_EXISTS	equ	00000001b	;Shadow memory exists
SHADOW_IN_USE	equ	00000010b	;Shadow contains valid image
SHADOW_TRASHED	equ	00000100b	;Shadow contents have been destroyed

sBegin 	Data
;-----------------------------------------------------------------------;
; Coordinates and extents of last save region.
;-----------------------------------------------------------------------;
	x1	dw	?		;left 
	x2	dw	?		;right
	x1B	dw	?		;left (byte aligned)
	x2B	dw	?		;right (byte aligned)
	y1	dw	?		;top
	y2	dw	?		;bottom
	xExt	dw	?		;width
	yExt	dw	?		;height

;-----------------------------------------------------------------------;
; Performance Testing Variables.  Only needed for Tefti Timing.
;-----------------------------------------------------------------------;
ifdef TEFTI
public	SSB_time
public	TextOut_time
public	BitBlt_time
public	SSB_count
public	TextOut_count
public	BitBlt_count
SSB_time	dd	0
TextOut_time	dd	0
BitBlt_time	dd	0
SSB_count	dd	0
TextOut_count	dd	0
BitBlt_count	dd	0
endif
sEnd	Data

;--------------------------Exported-Routine-----------------------------;
; SaveScreenBitmap
; Move a bitmap between active and unused display memory.
; Entry:
;	EGA registers in default state
; Returns:
;	AX = positive if no error
; Error Returns:
;	AX = 0 if error occured
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	None.
;-----------------------------------------------------------------------;
sBegin	Code
assumes cs,Code
assumes ds,Data
assumes es,nothing
cProc	SaveScreenBitmap,<FAR,PUBLIC>,<si,di>
	parmD	lprect			;--> rectangle to operate on
	parmW	cmd			;0 = save, 1 = restore, 2 = ignore
	localB	status			;shadow memory status byte
	localB	bMask
	localW	wStartSrcAddr
	localW	wStartDestAddr
	localW	wNumScans
	localW	bSideFlags
	localW	wEndSkip

cBegin
ifdef TEFTI
	push	ax
	push	dx
	timer_begin
	pop	dx
	pop	ax
endif
ife	???				;if no locals
	xor	ax,ax			;  check anyway
	call	my_check_stack
endif
	jc	ssb_exit_error_vect	;no room, abort

ifdef _BANK
	jmp	ssb_exit_error_vect
endif
;-----------------------------------------------------------------------;
;	Read the flag byte into AL, then check to see if any operation
;	is possible.  We cannot continue if:
;		shadow mem does not exist
;		we are to save to shadow mem already in use
;		we are to restore from shadow mem not in use
;		we are to restore from shadow mem stolen (trashed)
;-----------------------------------------------------------------------;
public	ssb_chk_mem_exists
ssb_chk_mem_exists:
	mov	ax,ScreenSelector
	mov	es,ax
	assumes	es,EGAMem
	mov	al,shadow_mem_status	;get status byte
	test	al,SHADOW_EXISTS	;see if mem is there
	jz	ssb_exit_error_vect	;no --> get out now
	mov	status,al		;store status for future
	cmp	cmd,SSB_SAVE		;if not a save command then
	jne	ssb_restore_check	;go check if it is a restore command.
	test	al,SHADOW_IN_USE	;Can't save if something is already
	jnz	ssb_exit_error_vect	;saved in the memory.
	jmp	ssb_perform_save

ssb_exit_error_trashed:
	and	al,not (SHADOW_IN_USE or SHADOW_TRASHED)
	mov	shadow_mem_status,al

public	ssb_exit_error_vect
ssb_exit_error_vect:
	clc				;clear carry to show error.
	jmp	ssb_exit_error		;jmp to error exit.

;-----------------------------------------------------------------------;
;	The function code was SSB_IGNORE.  User is telling us to forget
;	about what is saved in the save area so clear the SHADOW_IN_USE 
;	bit, the SHADOW_TRASHED bit, and exit successfully.
;-----------------------------------------------------------------------;
public	ssb_ignore_it
ssb_ignore_it:
	and	al,not (SHADOW_IN_USE or SHADOW_TRASHED)
	mov	shadow_mem_status,al
	jmp	ssb_exit_ok

public	ssb_restore_check
ssb_restore_check:
	cmp	cmd,SSB_RESTORE
	jne	ssb_ignore_it
	test	al,SHADOW_IN_USE	;see if there is something to restore.
	jz	ssb_exit_error_vect	;no, jmp to error exit.
	test	al,SHADOW_TRASHED	;has the save area been munged?
	jnz	ssb_exit_error_trashed	;yes, jmp to error exit.

;-----------------------------------------------------------------------;
;	      		  R E S T O R E 
;-----------------------------------------------------------------------;
public	ssb_perform_restore
ssb_perform_restore:
	assumes	ds,Data
	assumes	es,EGAMem

;-----------------------------------------------------------------------;
;	Exclude cursor.
;-----------------------------------------------------------------------;
@@:	mov	cx,x1B			; X1
	mov	si,x2B			; X2
	mov	dx,y1			; Y1
	mov	di,y2			; Y2
	call	exclude_far		; exclude cursor from blt area

;-----------------------------------------------------------------------;
;	Set up the VGA/EGA for write mode 1.
;-----------------------------------------------------------------------;
	mov	dx,EGA_BASE+GRAF_ADDR	;On the graphics controller...
	mov	al,GRAF_MODE		;address is: graphics mode register...
	mov	ah,M_LATCH_WRITE	;data is: write mode 1 
if MASMFLAGS and EGA
	mov	shadowed_mode,ah	;Needed so mouse cursor interrupt can
					;restore the EGA state.
endif
	out	dx,ax			;Tell the the h/w.

;-----------------------------------------------------------------------;
;	Compute parameters for copy loop.
;-----------------------------------------------------------------------;
	mov	bx,SCREEN_W_BYTES
	mov	ax,y1
	mul	bx
	mov	di,x1B
	shr	di,3	
	add	di,ax			;di = address of 1st dest byte.
	mov	si,SSB_START_OFFSET	;si = address of 1st src byte.
	mov	bx,xExt
	shr	bx,3			;Number of bytes in a scan (bx).
	mov	wEndSkip,0
	mov	bSideFlags,0		;assume we don't have to do left,right sides.
	mov	ax,x1			
	cmp	ax,x1B
	jz	@f
	inc	di
	inc	si
	dec	bx
	inc	wEndSkip
	or	bSideFlags,00000010b
@@:	mov	ax,x2			
	cmp	ax,x2B
	jz	@f
	dec	bx
	inc	wEndSkip
	or	bSideFlags,00000001b
@@:	cmp	bx,0
	jle	short SSB_DoSides	;if bx <= 0, then there is no middle.
	mov	ax,SCREEN_W_BYTES	;Compute increment to add to
	sub	ax,bx			;get to starting of next scan (ax).
	mov	dx,yExt			;Number of scans to copy (dx).
	push	ds			;Save ds.
	mov	cx,ScreenSelector
	mov	es,cx			;Src and Dest segment register
	mov	ds,cx			;must point to screen memory.
	assumes	ds,EGAMem
	assumes	es,EGAMem
	cld				;Direction flag is 'forward'.

;-----------------------------------------------------------------------;
;	Restore the byte aligned region to on-screen memory. 
;-----------------------------------------------------------------------;
@@:	mov	cx,bx
	rep	movsb
	add	di,ax
	add	si,wEndSkip
	dec	dx
	jnz	@b
	pop	ds
	assumes	ds,Data
SSB_DoSides:
	mov	ax,bSideFlags
	or	ax,ax
	jnz	@f
	jmp	ssb_update_status
	
;----------------------------------------------------------------------------;
;	Set up the VGA/EGA for read and write mode 0.
;----------------------------------------------------------------------------;
@@:	mov	dx,EGA_BASE+GRAF_ADDR	;On the graphics controller...
	mov	al,GRAF_MODE		;address is: graphics mode register...
	mov	ah,M_DATA_READ+M_PROC_WRITE;data is: read/write mode 0
if MASMFLAGS and EGA
	cli
endif
	out	dx,ax			;Tell the the h/w.
if MASMFLAGS and EGA
	mov	shadowed_mode,ah	;Needed so mouse cursor interrupt can
	sti				;restore the VGA/EGA state.
endif

;-----------------------------------------------------------------------;
;	Restore the left (partial byte) edge.
;-----------------------------------------------------------------------;
ssb_do_left:
	test	bSideFlags,00000010b
	jnz	@f
	jmp	ssb_do_right
@@:	mov	cx,x1
	sub	cx,x1B
	mov	al,-1
	shr	al,cl
	not	al
	mov	bMask,al
	mov	bx,SCREEN_W_BYTES
	mov	ax,y1
	mul	bx
	mov	di,x1B
	shr	di,3	
	add	di,ax			;di = address of 1st dest byte.
	mov	wStartDestAddr,di
	mov	si,SSB_START_OFFSET	;si = address of 1st src byte.
	mov	wStartSrcAddr,si
	mov	bx,xExt
	shr	bx,3			;Number of bytes in a scan (bx).
	mov	cx,yExt			;Number of scans to copy (cx).
	mov	wNumScans,cx
	push	ds			;Save ds.
	mov	dx,ScreenSelector
	mov	es,dx			;Src and Dest segment register
	mov	ds,dx			;must point to screen memory.
	assumes	ds,EGAMem
	assumes	es,EGAMem

;-----------------------------------------------------------------------;
; 	Do plane 0.
;-----------------------------------------------------------------------;
	mov	dx,EGA_BASE + SEQ_ADDR	;On the sequencer....
	mov	ax,MM_C0 shl 8 + SEQ_MAP_MASK ;address is: map mask register...
	out	dx,ax			;data is: enable plane 0 for writing.
	mov	dx,EGA_BASE + GRAF_ADDR	;On the graphics controller...
	mov	ax,RM_C0 shl 8 + GRAF_READ_MAP ;address is: map mask register...
	out	dx,ax			;data is: enable plane 0 for reading.
	mov	dh,bMask		;dh = logic mask.
ssb_lp0:mov	al,es:[di]		;Get screen byte (s).
	mov	ah,ds:[si]		;Get off-screen byte (p)
	mov	dl,al			;dl = s
	xor	al,ah			;al = s xor p == c  ;change mask
	and	al,dh			;al = c & bMask == c' ;clipped chng mask.
	xor	ah,al			;ah = c' xor p == s and p munged together.
	cmp	dl,ah			;If src and dest are the same,
	jz	@f			;don't bother writing to the screen.
	mov	es:[di],ah		;Set screen byte (dest).
@@:	add	di,SCREEN_W_BYTES
	add	si,bx
	loop	ssb_lp0
	mov	si,wStartSrcAddr
	mov	di,wStartDestAddr
	mov	cx,wNumScans

;-----------------------------------------------------------------------;
; 	Do plane 1.
;-----------------------------------------------------------------------;
	mov	dx,EGA_BASE + SEQ_ADDR	;On the sequencer....
	mov	ax,MM_C1 shl 8 + SEQ_MAP_MASK ;address is: map mask register...
	out	dx,ax			;data is: enable plane 1 for writing.
	mov	dx,EGA_BASE + GRAF_ADDR	;On the graphics controller...
	mov	ax,RM_C1 shl 8 + GRAF_READ_MAP ;address is: map mask register...
	out	dx,ax			;data is: enable plane 1 for reading.
	mov	dh,bMask		;dh = logic mask.
ssb_lp1:mov	al,es:[di]		;Get screen byte (s).
	mov	ah,ds:[si]		;Get off-screen byte (p)
	mov	dl,al			;dl = s
	xor	al,ah			;al = s xor p == c  ;change mask
	and	al,dh			;al = c & bMask == c' ;clipped chng mask.
	xor	ah,al			;ah = c' xor p == s and p munged together.
	cmp	dl,ah			;If src and dest are the same,
	jz	@f			;don't bother writing to the screen.
	mov	es:[di],ah		;Set screen byte (dest).
@@:	add	di,SCREEN_W_BYTES
	add	si,bx
	loop	ssb_lp1
	mov	si,wStartSrcAddr
	mov	di,wStartDestAddr
	mov	cx,wNumScans

;-----------------------------------------------------------------------;
; 	Do plane 2.
;-----------------------------------------------------------------------;
	mov	dx,EGA_BASE + SEQ_ADDR	;On the sequencer....
	mov	ax,MM_C2 shl 8 + SEQ_MAP_MASK ;address is: map mask register...
	out	dx,ax			;data is: enable plane 2 for writing.
	mov	dx,EGA_BASE + GRAF_ADDR	;On the graphics controller...
	mov	ax,RM_C2 shl 8 + GRAF_READ_MAP ;address is: map mask register...
	out	dx,ax			;data is: enable plane 2 for reading.
	mov	dh,bMask		;dh = logic mask.
ssb_lp2:mov	al,es:[di]		;Get screen byte (s).
	mov	ah,ds:[si]		;Get off-screen byte (p)
	mov	dl,al			;dl = s
	xor	al,ah			;al = s xor p == c  ;change mask
	and	al,dh			;al = c & bMask == c' ;clipped chng mask.
	xor	ah,al			;ah = c' xor p == s and p munged together.
	cmp	dl,ah			;If src and dest are the same,
	jz	@f			;don't bother writing to the screen.
	mov	es:[di],ah		;Set screen byte (dest).
@@:	add	di,SCREEN_W_BYTES
	add	si,bx
	loop	ssb_lp2
	mov	si,wStartSrcAddr
	mov	di,wStartDestAddr
	mov	cx,wNumScans

;-----------------------------------------------------------------------;
; 	Do plane 3.
;-----------------------------------------------------------------------;
	mov	dx,EGA_BASE + SEQ_ADDR	;On the sequencer....
	mov	ax,MM_C3 shl 8 + SEQ_MAP_MASK ;address is: map mask register...
	out	dx,ax			;data is: enable plane 3 for writing.
	mov	dx,EGA_BASE + GRAF_ADDR	;On the graphics controller...
	mov	ax,RM_C3 shl 8 + GRAF_READ_MAP ;address is: map mask register...
	out	dx,ax			;data is: enable plane 3 for reading.
	mov	dh,bMask		;dh = logic mask.
ssb_lp3:mov	al,es:[di]		;Get screen byte (s).
	mov	ah,ds:[si]		;Get off-screen byte (p)
	mov	dl,al			;dl = s
	xor	al,ah			;al = s xor p == c  ;change mask
	and	al,dh			;al = c & bMask == c' ;clipped chng mask.
	xor	ah,al			;ah = c' xor p == s and p munged together.
	cmp	dl,ah			;If src and dest are the same,
	jz	@f			;don't bother writing to the screen.
	mov	es:[di],ah		;Set screen byte (dest).
@@:	add	di,SCREEN_W_BYTES
	add	si,bx
	loop	ssb_lp3
	pop	ds
	assumes	ds,Data

;-----------------------------------------------------------------------;
;	Restore the right (partial byte) edge.
;-----------------------------------------------------------------------;
public	ssb_do_right
ssb_do_right:
	test	bSideFlags,00000001b
	jnz	@f
	jmp	ssb_update_status
@@:	mov	cx,x2B
	sub	cx,x2
	mov	al,-1
	shl	al,cl
	not	al
	mov	bMask,al
	mov	bx,SCREEN_W_BYTES
	mov	ax,y1
	mul	bx
	mov	di,x2B
	shr	di,3	
	add	di,ax			;di = address of 1st dest byte.
	mov	wStartDestAddr,di
	mov	si,SSB_START_OFFSET	
	mov	bx,xExt
	shr	bx,3			;Number of bytes in a scan (bx).
	add	si,bx			;si = address of 1st src byte.
	dec	si
	mov	wStartSrcAddr,si
	mov	cx,yExt			;Number of scans to copy (cx).
	mov	wNumScans,cx
	push	ds			;Save ds.
	mov	dx,ScreenSelector
	mov	es,dx			;Src and Dest segment register
	mov	ds,dx			;must point to screen memory.
	assumes	ds,EGAMem
	assumes	es,EGAMem

;-----------------------------------------------------------------------;
; 	Do plane 0.
;-----------------------------------------------------------------------;
	mov	dx,EGA_BASE + SEQ_ADDR	;On the sequencer....
	mov	ax,MM_C0 shl 8 + SEQ_MAP_MASK ;address is: map mask register...
	out	dx,ax			;data is: enable plane 0 for writing.
	mov	dx,EGA_BASE + GRAF_ADDR	;On the graphics controller...
	mov	ax,RM_C0 shl 8 + GRAF_READ_MAP ;address is: map mask register...
	out	dx,ax			;data is: enable plane 0 for reading.
	mov	dh,bMask		;dh = logic mask.
ssb_rp0:mov	al,es:[di]		;Get screen byte (s).
	mov	ah,ds:[si]		;Get off-screen byte (p)
	mov	dl,al			;dl = s
	xor	al,ah			;al = s xor p == c  ;change mask
	and	al,dh			;al = c & bMask == c' ;clipped chng mask.
	xor	ah,al			;ah = c' xor p == s and p munged together.
	cmp	dl,ah			;If src and dest are the same,
	jz	@f			;don't bother writing to the screen.
	mov	es:[di],ah		;Set screen byte (dest).
@@:	add	di,SCREEN_W_BYTES
	add	si,bx
	loop	ssb_rp0
	mov	si,wStartSrcAddr
	mov	di,wStartDestAddr
	mov	cx,wNumScans

;-----------------------------------------------------------------------;
; 	Do plane 1.
;-----------------------------------------------------------------------;
	mov	dx,EGA_BASE + SEQ_ADDR	;On the sequencer....
	mov	ax,MM_C1 shl 8 + SEQ_MAP_MASK ;address is: map mask register...
	out	dx,ax			;data is: enable plane 1 for writing.
	mov	dx,EGA_BASE + GRAF_ADDR	;On the graphics controller...
	mov	ax,RM_C1 shl 8 + GRAF_READ_MAP ;address is: map mask register...
	out	dx,ax			;data is: enable plane 1 for reading.
	mov	dh,bMask		;dh = logic mask.
ssb_rp1:mov	al,es:[di]		;Get screen byte (s).
	mov	ah,ds:[si]		;Get off-screen byte (p)
	mov	dl,al			;dl = s
	xor	al,ah			;al = s xor p == c  ;change mask
	and	al,dh			;al = c & bMask == c' ;clipped chng mask.
	xor	ah,al			;ah = c' xor p == s and p munged together.
	cmp	dl,ah			;If src and dest are the same,
	jz	@f			;don't bother writing to the screen.
	mov	es:[di],ah		;Set screen byte (dest).
@@:	add	di,SCREEN_W_BYTES
	add	si,bx
	loop	ssb_rp1
	mov	si,wStartSrcAddr
	mov	di,wStartDestAddr
	mov	cx,wNumScans

;-----------------------------------------------------------------------;
; 	Do plane 2.
;-----------------------------------------------------------------------;
	mov	dx,EGA_BASE + SEQ_ADDR	;On the sequencer....
	mov	ax,MM_C2 shl 8 + SEQ_MAP_MASK ;address is: map mask register...
	out	dx,ax			;data is: enable plane 2 for writing.
	mov	dx,EGA_BASE + GRAF_ADDR	;On the graphics controller...
	mov	ax,RM_C2 shl 8 + GRAF_READ_MAP ;address is: map mask register...
	out	dx,ax			;data is: enable plane 2 for reading.
	mov	dh,bMask		;dh = logic mask.
ssb_rp2:mov	al,es:[di]		;Get screen byte (s).
	mov	ah,ds:[si]		;Get off-screen byte (p)
	mov	dl,al			;dl = s
	xor	al,ah			;al = s xor p == c  ;change mask
	and	al,dh			;al = c & bMask == c' ;clipped chng mask.
	xor	ah,al			;ah = c' xor p == s and p munged together.
	cmp	dl,ah			;If src and dest are the same,
	jz	@f			;don't bother writing to the screen.
	mov	es:[di],ah		;Set screen byte (dest).
@@:	add	di,SCREEN_W_BYTES
	add	si,bx
	loop	ssb_rp2
	mov	si,wStartSrcAddr
	mov	di,wStartDestAddr
	mov	cx,wNumScans

;-----------------------------------------------------------------------;
; 	Do plane 3.
;-----------------------------------------------------------------------;
	mov	dx,EGA_BASE + SEQ_ADDR	;On the sequencer....
	mov	ax,MM_C3 shl 8 + SEQ_MAP_MASK ;address is: map mask register...
	out	dx,ax			;data is: enable plane 3 for writing.
	mov	dx,EGA_BASE + GRAF_ADDR	;On the graphics controller...
	mov	ax,RM_C3 shl 8 + GRAF_READ_MAP ;address is: map mask register...
	out	dx,ax			;data is: enable plane 3 for reading.
	mov	dh,bMask		;dh = logic mask.
ssb_rp3:mov	al,es:[di]		;Get screen byte (s).
	mov	ah,ds:[si]		;Get off-screen byte (p)
	mov	dl,al			;dl = s
	xor	al,ah			;al = s xor p == c  ;change mask
	and	al,dh			;al = c & bMask == c' ;clipped chng mask.
	xor	ah,al			;ah = c' xor p == s and p munged together.
	cmp	dl,ah			;If src and dest are the same,
	jz	@f			;don't bother writing to the screen.
	mov	es:[di],ah		;Set screen byte (dest).
@@:	add	di,SCREEN_W_BYTES
	add	si,bx
	loop	ssb_rp3
	pop	ds
	assumes	ds,Data

;-----------------------------------------------------------------------;
;	Update the local copy of the shadow memory status byte.
;-----------------------------------------------------------------------;
ssb_update_status:
	mov	bl,status		;get status byte
	and	bl, not SHADOW_IN_USE	;indicate mem is available.
	jmp	ssb_restore_video_state

public	ssb_exit_error_vect2
ssb_exit_error_vect2:
	clc
	jmp	ssb_exit_error		;jump to error exit.

;-----------------------------------------------------------------------;
;	      		     S A V E
;-----------------------------------------------------------------------;
public	ssb_perform_save
ssb_perform_save:
	les	bx,lprect		;ES:BX --> save area rectangle.
	assumes es,nothing
	assumes	ds,Data

;-----------------------------------------------------------------------;
;	Clip X coordinates to edge of screen
;-----------------------------------------------------------------------;
public	ssb_clip_x
ssb_clip_x:
	mov	cx,es:[bx]		;get x1
	mov	dx,es:[bx+4]		;get x2
	cmp	cx,dx
	je	ssb_exit_error_vect2	;Cannot save zero pixels.
	dec	dx			;make it inclusive.
	or	cx,cx
	jg	short @f
	xor	cx,cx
@@:	cmp	cx,SCREEN_WIDTH-1
	jl	short @f
	mov	cx,SCREEN_WIDTH-1
@@:	or	dx,dx
	jg	short @f
	xor	dx,dx
@@:	cmp	dx,SCREEN_WIDTH-1
	jl	short @f
	mov	dx,SCREEN_WIDTH-1

;-----------------------------------------------------------------------;
;	Byte align the left and right sides and compute x extent.
;-----------------------------------------------------------------------;
@@:	mov	x1,cx			;Save non-byte aligned x1.
	and	cx,0FFF8h
	mov	x1B,cx
	mov	x2,dx			;Save non-byte aligned x2.
	and	dx,0FFF8h
	add	dx,7
	mov	x2B,dx
	sub	dx,cx
	inc	dx
	mov	xExt,dx			;Store results locally.

;-----------------------------------------------------------------------;
;	Clip Y coordinates to edge of screen
;-----------------------------------------------------------------------;
	mov	cx,es:[bx+2]		;get y1
	mov	dx,es:[bx+6]		;get y2
	cmp	cx,dx
	je	ssb_exit_error_vect2	;Cannot save zero pixels.
	dec	dx			;make inclusive.
	or	cx,cx
	jg	@f
	xor	cx,cx
@@:	cmp	cx,SCREEN_HEIGHT-1
	jl	@f
	mov	cx,SCREEN_HEIGHT-1
@@:	or	dx,dx
	jg	@f
	xor	dx,dx
@@:	cmp	dx,SCREEN_HEIGHT-1
	jl	@f
	mov	dx,SCREEN_HEIGHT-1
@@:	mov	y1,cx
	mov	y2,dx
	sub	dx,cx			;compute yExt
	inc	dx
	mov	yExt,dx			;store results locally.

;-----------------------------------------------------------------------;
;	Compute size (in bytes) of area to save.
;-----------------------------------------------------------------------;
	mov	ax,xExt
	shr	ax,3			;Make it bytes.
	mov	bx,yExt
	mul	bx	
	cmp	ax,SSB_SIZE_IN_BYTES
	jbe	@f
	jmp	ssb_exit_error_vect2	;Too big, jmp to error exit.

;-----------------------------------------------------------------------;
;	Exclude cursor.
;-----------------------------------------------------------------------;
@@:	mov	cx,x1B			; X1
	mov	si,x2B			; X2
	mov	dx,y1			; Y1
	mov	di,y2			; Y2
	call	exclude_far		; exclude cursor from blt area

;-----------------------------------------------------------------------;
;	Set up the VGA/EGA for write mode 1.
;-----------------------------------------------------------------------;
	mov	dx,EGA_BASE+GRAF_ADDR	;On the graphics controller...
	mov	al,GRAF_MODE		;address is: graphics mode register...
	mov	ah,M_LATCH_WRITE	;data is: write mode 1 
	mov	cx,ScreenSelector
	mov	es,cx			;Src and Dest segment register
	assumes	es,EGAMem
if MASMFLAGS and EGA
	mov	shadowed_mode,ah	;Needed so mouse cursor interrupt can
					;restore the the EGA state.
endif
	out	dx,ax			;Tell the the h/w.
	mov	dx,EGA_BASE + SEQ_ADDR	;On the sequencer....
	mov	al,SEQ_MAP_MASK		;address is: map mask register...
	mov	ah,MM_ALL		;data is: enable all 4 bit planes.
	out	dx,ax			;Tell the h/w.

;-----------------------------------------------------------------------;
;	Compute parameters for copy loop.
;-----------------------------------------------------------------------;
	mov	bx,SCREEN_W_BYTES
	mov	ax,y1
	mul	bx
	mov	si,x1B
	shr	si,3	
	add	si,ax			;si = address of 1st src byte.
	mov	di,SSB_START_OFFSET	;di = address of 1st dest byte.
	mov	bx,xExt
	shr	bx,3			;Number of bytes in a scan (bx).
	mov	ax,SCREEN_W_BYTES	;Compute increment to add to
	sub	ax,bx			;get to starting of next scan (ax).
	mov	dx,yExt			;Number of scans to copy (dx).
	push	ds
	mov	cx,ScreenSelector
	mov	cx,es			;Src and Dest segment register
	mov	ds,cx			;must point to screen memory.
	assumes	ds,EGAMem
	cld				;Direction flag is 'forward'.

;-----------------------------------------------------------------------;
;	Copy the region to off-screen memory. 
;-----------------------------------------------------------------------;
@@:	mov	cx,bx
	rep	movsb
	add	si,ax
	dec	dx
	jnz	@b

;-----------------------------------------------------------------------;
;	Update the local copy of the shadow memory status byte.
;-----------------------------------------------------------------------;
	pop	ds
	assumes	ds,Data
	mov	bl,status		;get status byte
	and	bl,not SHADOW_TRASHED	;indicate mem not trashed
	or	bl,SHADOW_IN_USE	;indicate mem in use

;-----------------------------------------------------------------------;
;	Reset the EGA/VGA registers to the their original state.
;	Note: es still has the screen selector loaded.
;-----------------------------------------------------------------------;
public	ssb_restore_video_state
ssb_restore_video_state:
;----------------------------------------------------------------------------;
;	Set up the VGA/EGA for read and write mode 0.
;----------------------------------------------------------------------------;
	mov	dx,EGA_BASE+GRAF_ADDR	;On the graphics controller...
	mov	al,GRAF_MODE		;address is: graphics mode register...
	mov	ah,M_DATA_READ+M_PROC_WRITE;data is: write mode 0
if MASMFLAGS and EGA
	cli
endif
	out	dx,ax			;Tell the the h/w.
if MASMFLAGS and EGA
	mov	shadowed_mode,ah	;Needed so mouse cursor interrupt can
	sti				;restore the VGA/EGA state.
endif

;----------------------------------------------------------------------------;
; 	Enable all bits in the Bit Mask register.  Leave this register
;	mapped in the Graphics Controller port.
;----------------------------------------------------------------------------;
	mov	al,GRAF_BIT_MASK	;address is: Bit Mask Register.
	mov	ah,0ffh			;data is: select all bits.
	out	dx,ax			;Tell the h/w.

;----------------------------------------------------------------------------;
;	Enable all bit planes for writing.
;----------------------------------------------------------------------------;
	mov	dx,EGA_BASE + SEQ_ADDR	;On the sequencer....
	mov	al,SEQ_MAP_MASK		;address is: map mask register...
	mov	ah,MM_ALL		;data is: enable all 4 bit planes.
	out	dx,ax			;Tell the h/w.

;----------------------------------------------------------------------------;
;	Update the shadow_mem_status variable and redraw the cursor.
;----------------------------------------------------------------------------;
	mov	shadow_mem_status,bl	
	call	unexclude_far

;----------------------------------------------------------------------------;
;	Exit.
;----------------------------------------------------------------------------;
public	ssb_exit_ok
ssb_exit_ok:
	stc				;prepare for "successful" exit	    
public	ssb_exit_error
ssb_exit_error:
	sbb	ax,ax
ifdef TEFTI
	assumes	ds,Data
	push	ax
	push	dx
	timer_end
	add	word ptr SSB_time,ax
	adc	word ptr SSB_time+2,dx
	add	word ptr SSB_count,1
	adc	word ptr SSB_count+2,0
	pop	dx
	pop	ax
endif
cEnd
sEnd	Code
	end
