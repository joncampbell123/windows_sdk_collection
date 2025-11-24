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

;---------------------------------------------------------------------
; TRAIL.ASM
; This module contains the routines which are required to draw
; the cursor and cursor trail.
;---------------------------------------------------------------------
	.286
	include cmacros.inc
	include windefs.inc
 	include mflags.inc
	include	cursor.inc
	include ega.inc
	include egamemd.inc
	??_out	Cursors
	externA ScreenSelector		;Segment of screen
	externNP enable_switching	;Allow screen group switching
	externNP disable_switching	;Disallow screen group switching
	externNP save_hw_regs
	externNP res_hw_regs

ifdef _BANK
	externNP SetBanks
	externNP SetReadBank
	externNP SetWriteBank
	externNP NextReadBank
	externNP NextWriteBank
endif
;---------------------------------------------------------------------
; Equates and Macros.
;---------------------------------------------------------------------
SAVE_HEIGHT	equ	32
SAVE_WIDTH	equ	5
SAVE_PIXWIDTH	equ	5*8
SAVE_BYTECOUNT	equ	32*5

QtoQ	macro	From,To
	mov	ax,From.sx
	mov	To.sx,ax
	mov	ax,From.sy
	mov	To.sy,ax
	mov	ax,From.mx
	mov	To.mx,ax
	mov	ax,From.my
	mov	To.my,ax
	mov	ax,From.rawx
	mov	To.rawx,ax
	mov	ax,From.pScreenPtr
	mov	To.pScreenPtr,ax
	endm

WrapQ	macro	reg
	cmp	reg,Qsize
	jle	short $+4
	xor	reg,reg
	endm

;---------------------------------------------------------------------
; Inquire_data contains information about mouse acceleration
; which the window manager uses.
;---------------------------------------------------------------------
createSeg _BLUEMOON,BlueMoonSeg,word,public,CODE
sBegin	BlueMoonSeg
inquire_data	CURSORINFO   <X_RATE,Y_RATE>
;--------------------------Exported-Routine-----------------------------;
; WORD Inquire(lpCURSORINFO)
; CURSORINFO far *lpCURSORINFO; //pointer where cursor info goes
; 
; Information about the pointer is returned to the caller in the
; given buffer.  The number of bytes copied into lpCURSORINFO
; is returned.
;-----------------------------------------------------------------------;
	assumes ds,Data
cProc	Inquire,<PUBLIC,FAR,WIN,PASCAL>,<si,di>
	parmD	lp_cursor_info		;Where to put the data
cBegin
	push	cs
	pop	ds
	assumes ds,BlueMoonSeg
	les	di,lp_cursor_info	;--> destination area
	assumes es,nothing
	mov	si,BlueMoonSegOFFSET inquire_data
	mov	ax,size CURSORINFO	;Return code is # of bytes moved
	mov	cx,ax
	cld
	rep	movsb
cEnd
sEnd	BlueMoonSeg

;---------------------------------------------------------------------
; 	D A T A   S E G M E N T
;---------------------------------------------------------------------
sBegin	Data
	public	Qsize
	public	Qhead
	public	Qtail
	public	wXORCursor
	externW CURSOR_X_COORDINATE
	externW CURSOR_Y_COORDINATE
	externW X_HOT_SPOT
	externW Y_HOT_SPOT
	externB enabled_flag		;Non-zero if output allowed
	externB	in_background		;background or not
	externD	lpfnUpdateInking
	externB	fInkAvailable

;---------------------------------------------------------------------
; cur_cursor contains the cursor data structure (less the
; actual bits) for the current cursor shape.
;---------------------------------------------------------------------
cur_cursor	cursorShape <,,,,,>

;---------------------------------------------------------------------
; The following are the masks which make up the cursor image.
;---------------------------------------------------------------------
cur_and_mask	db	MASK_LENGTH dup (?)
cur_xor_mask	db	MASK_LENGTH dup (?)

;---------------------------------------------------------------------
; rotation maintains the number of bits the cursor masks have
; been rotated.  This value is always between 0 and 7
;---------------------------------------------------------------------
rotation	db	0
real_width	dw	CUR_ICON_WIDTH*8

;---------------------------------------------------------------------
; Save area queue.
;---------------------------------------------------------------------
QueueEntry	struc
  sx		dw	?
  sy		dw	?
  mx		dw	?
  my		dw	?
  rawx		dw	?
  pSaveArea	dw	?
  pScreenPtr	dw	?
  spare		dw	?
QueueEntry	ends
rawy	equ	my
Queue	QueueEntry <,,,,,save_area_0,,>
	QueueEntry <,,,,,save_area_1,,>
	QueueEntry <,,,,,save_area_2,,>
	QueueEntry <,,,,,save_area_3,,>
	QueueEntry <,,,,,save_area_4,,>
	QueueEntry <,,,,,save_area_5,,>
	QueueEntry <,,,,,save_area_6,,>
	QueueEntry <,,,,,save_area_7,,>
Qhead		dw	0
Qtail		dw	0
Qsize		dw	7
wXORCursor	dw	0
FromQ	equ	Queue[bx]
ToQ	equ	Queue[bp]

exclude_left	dw	0		;left side  of exclusion rectangle
exclude_top	dw	0		;top	    of exclusion rectangle
exclude_right	dw	0		;right side of exclusion rectangle
exclude_bottom	dw	0		;bottom     of exclusion rectangle

IS_EXCLUDED	equ 000000001b		;Cursor is excluded.
IS_BUSY      	equ 000000010b		;Critical section.
IS_NULL	    	equ 000000100b		;Cursor is null.
HAS_MOVED	equ 000001000b		;Cursor has moved.
IS_VISIBLE   	equ 000010000b		;Cursor is visible.

externB	CURSOR_STATUS
externW wInhibitCount

public	screen_busy
screen_busy	db	1
if MASMFLAGS and VGA
globalB shadow_mem_status, 0
endif

sEnd	Data

;---------------------------------------------------------------------
; 	C O D E   S E G M E N T
;---------------------------------------------------------------------
sBegin	Code
	assumes	cs,Code
	assumes	es,nothing
	assumes	ds,Data

;---------------------------------------------------------------------
; DrawCursor
; Entry:
;   ds:	Data segment.
;   CURSOR_X_COORDINATE, CURSOR_Y_COORDINATE: location to draw cursor.
;   X_HOT_SPOT, Y_HOT_SPOT:  hot spot in cursor.
;---------------------------------------------------------------------
DrawCursor	proc	near
	test	enabled_flag,0FFh	;Cannot output if display has
	jnz	short @f
	jmp	DrawCursorExit		;been disabled
@@:	call	disable_switching
	mov	ax,ScreenSelector	;save_hw_regs requires this
	mov	es,ax
	assumes es,EGAMem
	call	save_hw_regs		;Save EGA registers

;---------------------------------------------------------------------
; Set the EGA up for copying bytes on the EGA.  This will be
; done using M_PROC_WRITE, M_DATA_READ, DR_SET, and setting
; GRAF_BIT_MASK to 0 (write Mode 1).  All of
; the above except GRAF_BIT_MASK was set by save_hw_regs.
; All planes were also enabled for writing by save_hw_regs.
;---------------------------------------------------------------------
	mov	dx,EGA_BASE + GRAF_ADDR
	mov	ax,GRAF_BIT_MASK
	out	dx,ax			;Disable all bits
	cld				;This is interrupt code, do this!
	push	bp			;Must save this.
	mov	bx,Qhead		
	shl	bx,4			;16 bytes per queue entry.
	mov	si,CURSOR_X_COORDINATE	;cursor x position.
	sub	si,X_HOT_SPOT
	mov	Queue[bx].rawx,si
	sar	si,3			;divide by 8.
	mov	Queue[bx].mx,si		;Save in the queue.
	or	si,si			;If it is less than zero..
	jge	@f			;then
	xor	si,si			;clip it at zero.
@@:	cmp	si,SCAN_BYTES-SAVE_WIDTH;Otherwise,  if it is within one
	jle	@f			;save_width of the right edge, then
	mov	si,SCAN_BYTES-SAVE_WIDTH;clip it to that point.
@@:	mov	Queue[bx].sx,si		;Save in the queue.
	mov	ax,CURSOR_Y_COORDINATE	;cursor y position.
	sub	ax,Y_HOT_SPOT
	mov	Queue[bx].my,ax		;Save it in the queue.
	or	ax,ax			;If it is less than zero...
	jg	@f			;then
	xor	ax,ax			;clip it at zero.
@@:	cmp	ax,SCREEN_HEIGHT-SAVE_HEIGHT ;If it is within one save_height
	jle	@f			;of the bottom, then clip it 
	mov	ax,SCREEN_HEIGHT-SAVE_HEIGHT ; at that point.
@@:	mov	Queue[bx].sy,ax		;Save it in the queue.
	imul	ax,SCAN_BYTES		;Compute the screen address of the 
	add	si,ax			;the area we are about to save.
	mov	Queue[bx].pScreenPtr,si ;store it into the queue.
	push	ds			;
	call	ScreenToSave		;Save off the area under the cursor.
	pop	ds
	assumes ds,Data
	cmp	wXORCursor,0		;Do not allow MouseTrails if
	jne	short @f		;this is an XOR-only cursor.
	mov	ax,Qhead
	cmp	ax,Qtail
	je	short @f
	jmp	short DC_EraseTail
@@:	mov	ax,Qhead
	inc	ax
	WrapQ	ax
	cmp	ax,Qtail		;Queue is full if Qhead == Qtail.
	jne	short @f
DC_EraseTail:
	call	EraseTail_10		;free up a location in the queue.
@@:
	mov	bx,Qhead		;bx = h
	mov	bp,bx			;bp = h+1
	shl	bx,4
	inc	bp
	WrapQ	bp
	shl	bp,4
	QtoQ	FromQ,ToQ
	push	ds
	call	SaveToSave		;save(h+1) <== save(h)
	pop	ds
	call	CursorToSave		;save(h+1) <-- cursor(h)
	mov	bx,bp
	push	ds
	call	SaveToScreen		;screen <== save(h+1)
	pop	ds

	mov	ax,Qhead		;Update Qhead now.
	inc	ax
	WrapQ	ax
	mov	Qhead,ax

	pop	bp
	call	res_hw_regs		;Restore user's state
	call	enable_switching
DrawCursorExit:
	ret
DrawCursor	endp


;---------------------------------------------------------------------
; SaveToSave
;   Copy save region 'a' to a save region 'b' with possible 
;   clipping.
; Entry
;   bx : From Queue index.
;   bp : To Queue index.
;   es : ScreenSelector
;   ds : Data
;---------------------------------------------------------------------
SaveToSave	proc	near
	assumes	es,EGAMem
	assumes	ds,Data
;---------------------------------------------------------------------
; First check if there is any intersection.  If not, then no
; work is necessary.
;---------------------------------------------------------------------
STS_HorzCheck:
	mov	ax,FromQ.sx
	mov	cx,ToQ.sx
	add	ax,SAVE_WIDTH
	cmp	ax,cx
	jle	STS_Exit
	sub	ax,SAVE_WIDTH
	add	cx,SAVE_WIDTH
	cmp	ax,cx
	jge	STS_Exit
STS_VertCheck:
	mov	ax,FromQ.sy
	mov	cx,ToQ.sy
	add	ax,SAVE_HEIGHT
	cmp	ax,cx
	jle	STS_Exit
	sub	ax,SAVE_HEIGHT
	add	cx,SAVE_HEIGHT
	cmp	ax,cx
	jl	STS_IntersectCopy
STS_Exit:
	ret

;---------------------------------------------------------------------
; Find the intersecting rectangle's row in 'a' and 'b'.  Alter the
; src and destination pointers up to that point.
;--------------------------------------------------------------------- 
STS_IntersectCopy:
	assumes	ds,Data
ifdef _BANK
	mov	si,ax
	mov	ax,1
	call	SetBanks		;Set read/write bank to 2nd bank.
	mov	ax,si
endif
	mov	si,FromQ.pSaveArea
	mov	di,ToQ.pSaveArea
	sub	cx,SAVE_HEIGHT
	sub	ax,cx
	mov	dl,al
	imul	ax,SAVE_WIDTH
	jl	@f
	add	di,ax		;Alter dest (b) start row.
	neg	dl
	jmp	short STS_FindColumns
@@:	sub	si,ax		;Alter src (a) start row.
;---------------------------------------------------------------------
; Find the intersecting rectangle's column in 'a' and 'b'.  Alter the
; src and destination pointers up to that point.
;--------------------------------------------------------------------- 
STS_FindColumns:
	add	dl,SAVE_HEIGHT
	mov	ax,FromQ.sx
	mov	cx,ToQ.sx
	sub	ax,cx
	mov	dh,al
	jl	@f
	add	di,ax		;Alter dest (b) start row.
	neg	dh
	jmp	short STS_FindSkip
@@:	sub	si,ax		;Alter src (a) start row.
;---------------------------------------------------------------------
; Compute the skip value to add to the src/dest pointers after
; a given row copy.
;--------------------------------------------------------------------- 
STS_FindSkip:
	xor	ax,ax
	mov	al,dh
	neg	al
	add	dh,SAVE_WIDTH

;---------------------------------------------------------------------
; Do the copy.
;--------------------------------------------------------------------- 
	xor	ch,ch
	push	es
	pop	ds
	assumes	ds,EGAMem
STS_CopyRow:
	mov	cl,dh		;Set bytes to copy.
	rep	movsb
	add	si,ax		;Add skip values to src.
	add	di,ax		;and dest. pointers.
	dec	dl
	jnz	STS_CopyRow
	ret
SaveToSave	endp

;---------------------------------------------------------------------
; ScreenToSave
;   Copy screen region to a save region.
; Entry:
;   bx : Queue entry of destination save area.
;   es : EGAMem
;   ds : Data
;---------------------------------------------------------------------
ScreenToSave	proc	near
	assumes	ds,Data
	assumes	es,EGAMem
ifdef _BANK
	mov	ax,1
	call	SetWriteBank		;Write bank will be 1.
	xor	ax,ax			;Assumes read bank of 0.
	cmp	Queue[bx].sy,511	;Bank switch at this scan line.
	jle	short @f
	mov	ax,1			;Read bank is 1.
@@:	call	SetReadBank
endif
	mov	si,Queue[bx].pScreenPtr
	mov	di,Queue[bx].pSaveArea
	push	es
	pop	ds
	assumes	ds,EGAMem	
	mov	dx,SAVE_HEIGHT
	mov	ax,SCAN_BYTES - SAVE_WIDTH
ifdef _BANK
	inc	ax
endif
@@:	mov	cx,SAVE_WIDTH
	rep	movsb
ifdef _BANK
	dec	si
endif
	add	si,ax
ifdef _BANK
	jc	short ScToSv_GoToNextBank
endif
	dec	dx
	jnz	@b
	ret

ifdef _BANK
ScToSv_GoToNextBank:
	push	dx
	push	ax
	call	NextReadBank
	pop	ax
	pop	dx
	dec	dx
	jnz	@b
	ret
endif
ScreenToSave	endp

;---------------------------------------------------------------------
; SaveToScreen
;   Copy save region to screen location.
; Entry:
;   bx : Queue index of src save area.
;   ds : Data
;   es : EGAMem
;---------------------------------------------------------------------
SaveToScreen	proc	near
	assumes	ds,Data
	assumes	es,EGAMem	
ifdef _BANK
	mov	ax,1
	call	SetReadBank		;Read bank will be 1.
	xor	ax,ax			;Assumes write bank of 0.
	cmp	Queue[bx].sy,511	;Bank switch at this scan line.
	jle	short @f
	mov	ax,1			;Write bank is 1.
@@:	call	SetWriteBank
endif
	mov	si,Queue[bx].pSaveArea
	mov	di,Queue[bx].pScreenPtr
	push	es
	pop	ds
	assumes	ds,EGAMem	
	mov	dx,SAVE_HEIGHT
	mov	ax,SCAN_BYTES - SAVE_WIDTH
ifdef _BANK
	inc	ax
endif
@@:	mov	cx,SAVE_WIDTH
	rep	movsb
ifdef _BANK
	dec	di
endif
	add	di,ax
ifdef _BANK
	jc	short SvToSc_GoToNextBank
endif
	dec	dx
	jnz	@b
	ret

ifdef _BANK
SvToSc_GoToNextBank:
	push	dx
	push	ax
	call	NextWriteBank
	pop	ax
	pop	dx
	dec	dx
	jnz	@b
	ret
endif
SaveToScreen	endp

;---------------------------------------------------------------------
; CursorToSave
;   Copy cursor to save region with possible clip.
; Entry:
;   bx:   From queue.
;   bp:   To queue.
;---------------------------------------------------------------------
CursorToSave	proc	near
	assumes	es,EGAMem
	assumes	ds,Data
CTS_HorzCheck:
	mov	ax,FromQ.mx
	mov	cx,ToQ.sx
	add	ax,SAVE_WIDTH
	cmp	ax,cx
	jle	CTS_Exit
	sub	ax,SAVE_WIDTH
	add	cx,SAVE_WIDTH
	cmp	ax,cx
	jge	CTS_Exit
CTS_VertCheck:
	mov	ax,FromQ.my
	mov	cx,ToQ.sy
	add	ax,SAVE_HEIGHT
	cmp	ax,cx
	jle	CTS_Exit
	sub	ax,SAVE_HEIGHT
	add	cx,SAVE_HEIGHT
	cmp	ax,cx
	jl	CTS_IntersectCopy
CTS_Exit:
	ret

;---------------------------------------------------------------------
; Find the intersecting rectangle's row in 'a' and 'b'.  Alter the
; src and destination pointers up to that point.
;--------------------------------------------------------------------- 
CTS_IntersectCopy:
ifdef _BANK
	mov	ax,1
	call	SetBanks
endif
	assumes	ds,Data
	push	bx
	call	rotate_masks
	pop	bx

;---------------------------------------------------------------------
; Set the EGA for AND mode and draw the cursor.
;---------------------------------------------------------------------
	mov	dx,EGA_BASE + GRAF_ADDR
	mov	ax,DR_AND shl 8 + GRAF_DATA_ROT
	out	dx,ax
	mov	ax,0FF00h + GRAF_BIT_MASK
	out	dx,ax
	mov	si,DataOFFSET cur_and_mask
	mov	di,ToQ.pSaveArea
	call	CTS_DrawCursor

;---------------------------------------------------------------------
; Set the EGA for XOR mode and draw the cursor.
;---------------------------------------------------------------------
	mov	dx,EGA_BASE + GRAF_ADDR
	mov	ax,DR_XOR shl 8 + GRAF_DATA_ROT
	out	dx,ax
	mov	ax,0FF00h + GRAF_BIT_MASK
	out	dx,ax
	mov	si,DataOFFSET cur_xor_mask
	mov	di,ToQ.pSaveArea
	call	CTS_DrawCursor
;---------------------------------------------------------------------
; Set the EGA back to normal.
;---------------------------------------------------------------------
	mov	dx,EGA_BASE + GRAF_ADDR
	mov	ax,DR_SET shl 8 + GRAF_DATA_ROT
	out	dx,ax
	mov	ax,GRAF_BIT_MASK
	out	dx,ax
	ret

CTS_DrawCursor:
	mov	ax,FromQ.my
	mov	cx,ToQ.sy
	sub	ax,cx
	mov	dl,al
	imul	ax,SAVE_WIDTH
	jl	@f
	add	di,ax		;Alter dest (b) start row.
	neg	dl
	jmp	short CTS_FindColumns
@@:	sub	si,ax		;Alter src (a) start row.

;---------------------------------------------------------------------
; Find the intersecting rectangle's column in 'a' and 'b'.  Alter the
; src and destination pointers up to that point.
;--------------------------------------------------------------------- 
CTS_FindColumns:
	add	dl,SAVE_HEIGHT
	mov	ax,FromQ.mx
	mov	cx,ToQ.sx
	sub	ax,cx
	mov	dh,al
	jl	@f
	add	di,ax		;Alter dest (b) start row.
	neg	dh
	jmp	short CTS_FindSkip
@@:	sub	si,ax		;Alter src (a) start row.

;---------------------------------------------------------------------
; Compute the skip value to add to the src/dest pointers after
; a given row copy.
;--------------------------------------------------------------------- 
CTS_FindSkip:
	xor	ax,ax
	mov	al,dh
	neg	al
	add	dh,SAVE_WIDTH

;---------------------------------------------------------------------
; Do the copy.
;--------------------------------------------------------------------- 
	xor	ch,ch
CTS_CopyRow:
	mov	cl,dh		;Set bytes to copy.
@@:	mov	ch,es:[di]	;Load latches.
	movsb			;Do logic operation.
	dec	cl
	jnz	@b
	add	si,ax		;Add skip values to src.
	add	di,ax		;and dest. pointers.
	dec	dl
	jnz	CTS_CopyRow
	ret
CursorToSave	endp

;---------------------------------------------------------------------
; EraseTail -- Erase the tail up to one image.
;---------------------------------------------------------------------
EraseTail	proc	near
	assumes	ds,Data
	mov	ax,Qtail
	inc	ax
	WrapQ	ax
	cmp	ax,Qhead
	jne	short @f
	ret
@@:	test	enabled_flag,0FFh
	jnz	@f
	ret
@@:	call	disable_switching
	mov	ax,ScreenSelector	;save_hw_regs requires this
	mov	es,ax
	assumes es,EGAMem
	call	save_hw_regs		;Save EGA registers
	mov	dx,EGA_BASE + GRAF_ADDR
	mov	ax,GRAF_BIT_MASK
	out	dx,ax			;Disable all bits
	cld				;This is interrupt code, do this!
	push	bp			;Must save this.
	call	EraseTail_10
	pop	bp
	call	res_hw_regs		;Restore user's state
	call	enable_switching
	ret

;---------------------------------------------------------------------
; save(t+1:h) <-- save(t)
;---------------------------------------------------------------------
EraseTail_10:
	mov	bx,Qtail		;bx = t  (FromQ)
	mov	ax,bx
	shl	bx,4			;16 bytes per queue entry.
@@:	inc	ax	
	WrapQ	ax
	mov	bp,ax			;bp = t+1 (ToQ)
	shl	bp,4			;16 bytes per queue entry.
	push	ax 
	push	ds 
	call	SaveToSave
	pop	ds
	assumes	ds,Data
	pop	ax 
	cmp	ax,Qhead
	jne	@b
;---------------------------------------------------------------------
; save(t) <-- cursor(t+1:h)
;---------------------------------------------------------------------
	mov	bp,Qtail		;bp = t (ToQ)
	mov	ax,bp
	shl	bp,4			;16 bytes per queue entry.
@@:	inc	ax
	WrapQ	ax
	mov	bx,ax			;bx = t+1 (FromQ)
	shl	bx,4
	push	ax 
	call	CursorToSave
	pop	ax 
	cmp	ax,Qhead
	jne	@b
;---------------------------------------------------------------------
; save(h) <-- cursor(t+1:h-1)
;---------------------------------------------------------------------
	mov	bp,Qhead		;bp = h (ToQ)
	shl	bp,4			;16 bytes per queue entry.
	mov	ax,Qtail
@@:	inc	ax
	WrapQ	ax			;from the queue tail.
	cmp	ax,Qhead
	je	@f
	mov	bx,ax			;bx = t+1 (FromQ)
	shl	bx,4
	push	ax 
	call	CursorToSave
	pop	ax 
	jmp	@b
@@:
;---------------------------------------------------------------------
; Erase the tail cursor image.
; screen <== save(t)
;---------------------------------------------------------------------
	mov	ax,Qtail
	mov	bx,ax
	inc	ax
	WrapQ	ax
	mov	Qtail,ax
	shl	bx,4
	push	ds
	call	SaveToScreen
	pop	ds
	assumes	ds,Data
	ret
EraseTail	endp	

;---------------------------------------------------------------------
; LoadCursor
;
;   Load AND and XOR cursor masks into the cursor work areas.
; Entry:
;	DS:SI  =  User's AND mask of cursor.
;	ES     =  Data segment
;---------------------------------------------------------------------
	assumes ds,nothing		;DS is pointing to user data
	assumes es,Data 
LoadCursor	proc	near
	cld
LC_GetANDMask:	
	mov	di,DataOFFSET cur_and_mask
	mov	al,0ffh 		;Set implicit part of AND mask
	call	LoadCursor_10		;Move AND mask
;---------------------------------------------------------------------
; Detect XOR-Only cursors and inhibit MouseTrails if so.
;---------------------------------------------------------------------
	mov	di,DataOFFSET cur_and_mask
	mov	ax,0ffffh
	mov	cx,SAVE_HEIGHT	
@@:	and	ax,es:[di]
	and	ax,es:[di+2]
	and	al,es:[di+4]
	add	di,5
	loop	@b
	inc	ax			;ax = 0 if XOR cursor.
	mov	wXORCursor,ax
LC_GetXORMask:	
	mov	di,DataOFFSET cur_xor_mask
	xor	al,al			;Set implicit part of XOR mask (0)
	mov	rotation,al		;Show mask isn't rotated

LoadCursor_10:
	mov	cx,SAVE_HEIGHT/2 	;Set height for move
	errnz	<SAVE_HEIGHT and 1>	;  (Must be even)
@@:	movsw				;Move explicit part
	movsw				;Move explicit part
	stosb				;Move implicit part
	movsw
	movsw
	stosb
	errnz	SAVE_WIDTH-5
	loop	@b
	ret
LoadCursor	endp

;---------------------------------------------------------------------
; CursorOff
;   The old cursor is removed from the screen if it currently
;   is on the screen.
; Entry:
;	DS = Data
;---------------------------------------------------------------------
	assumes ds,Data
	assumes es,nothing
CursorOff	proc	near
	assumes	ds,Data
	assumes	es,nothing
	mov	ax,Qtail
	cmp	ax,Qhead
	jne	short @f
	ret
@@:	test	enabled_flag,0FFh
	jnz	short @f
	mov	ax,Qtail
	mov	Qhead,ax
	ret
@@:	call	disable_switching
	mov	ax,ScreenSelector	;save_hw_regs requires this
	mov	es,ax
	assumes es,EGAMem
	call	save_hw_regs		;Save EGA registers
	mov	dx,EGA_BASE + GRAF_ADDR
	mov	ax,GRAF_BIT_MASK
	out	dx,ax			;Disable all bits
	cld				;This is interrupt code, do this!
	push	bp			;Must save this.
;---------------------------------------------------------------------
; Intersect-copy the save area of the queue tail into each entry
; of the queue.
;---------------------------------------------------------------------
	mov	ax,Qtail
CO_OuterLoop:
	cmp	ax,Qhead
	je	short CO_Exit
	mov	bx,ax
	shl	bx,4			;16 bytes per queue entry.
@@:	inc	ax			;Select queue image one up 
	WrapQ	ax
	cmp	ax,Qhead		;If it is the head, we are done.
	je	short @f
	mov	bp,ax			
	shl	bp,4
	push	ax 
	push	ds 
	call	SaveToSave		;bx = from Queue, bp = to Queue
	pop	ds
	pop	ax 
	assumes	ds,Data
	jmp	@b
;---------------------------------------------------------------------
; Erase the tail cursor image.
;---------------------------------------------------------------------
@@:	push	ds
	call	SaveToScreen
	pop	ds
	assumes	ds,Data
	mov	ax,Qtail
	inc	ax
	WrapQ	ax
	mov	Qtail,ax
	jmp	CO_OuterLoop
CO_Exit:
	pop	bp
	call	res_hw_regs		;Restore user's state
	call	enable_switching
a_return:
	ret
CursorOff	endp

;--------------------------Private-Routine----------------------------
; rotate_masks
;
;   The cursor/icon masks are rotated to be aligned for the
;   new (x,y).	The rotate is performed as a single-bit shift
;   of the entire mask.
;
; Entry:
;	DS = Data
;	direction flag cleared
;	bx: queue containing cursor location.
; Returns:
;	direction flag cleared
; Error Returns:
;	No error return.
; Registers Preserved:
;	DS
; Registers Destroyed:
;	AX,DX,SI,DI,ES,FLAGS
; Calls:
;	None
;---------------------------------------------------------------------
	assumes ds,Data
	assumes es,nothing

rotate_masks	proc near
	mov	al,byte ptr Queue[bx].rawx ;Get d0..d2 of new X coordinate
	mov	bl,00000111B
	and	al,bl
	and	bl,rotation		;Get d0..d2 of current rotation
	mov	rotation,al		;Save new rotation index
	sub	al,bl			;Compute delta rotate
	jz	a_return		;  Mask is already aligned
	push	es
	push	ds
	pop	es
	assumes es,Data
	jl	rot_cur_left		;  New < old, rotate left
	mov	bh,al			;Save rotate count
	mov	si,DataOFFSET cur_and_mask
	call	rot_right
	mov	al,bh
	mov	si,DataOFFSET cur_xor_mask
	call	rot_right
	pop	es
	ret

rot_right:
;---------------------------------------------------------------------
; Rotate the given mask right (al) bits. If the rotate is more than
; two bits, use the fast code.
;---------------------------------------------------------------------
	cmp	al,3			;Use the big rotate code?
	jnc	rot_right_big		;  Yes, it will be faster
	mov	bl,al			;Save rotate count

rot_right_10:
	mov	di,si			;Set pointers
	mov	cx,MASK_LENGTH/2	;Set # words to rotate
	errnz	<MASK_LENGTH and 1>	;Must be a multiple of 2 for words

rot_right_20:
	mov	ax,word ptr [di]	;Rotate this word
	xchg	ah,al
	rcr	ax,1
	xchg	ah,al
	stosw				;Store new word and update pointer
	loop	rot_right_20

;---------------------------------------------------------------------
; Now finish the rotate by setting the first bit in the first byte
; to the bit shifted out of the last byte.  I believe (I know!)
; that a couple of rotates will handle this quite nicely.
;---------------------------------------------------------------------
	mov	al,[si]
	rcl	al,1			;Rmv unwanted bit, add in wanted bit
	ror	al,1			;Put wanted bit in D7 where it belongs
	mov	[si],al 		;Store new byte
	dec	bl			;More to rotate?
	jnz	rot_right_10		;  Yes, rotate them
	ret

;---------------------------------------------------------------------
; The big rotate rotates 'n' bits at a time.  Time will be saved
; using this code for rotates of over two bits.
;---------------------------------------------------------------------
rot_right_big:
	mov	cl,al			;Set rotate count
	mov	di,si			;Set pointers
	mov	dx,MASK_LENGTH		;Set # bytes to rotate
	xor	bl,bl			;Zero initial previous bits

rot_right_big_10:
	xor	ax,ax			;Zero LSB for the shift
	mov	ah,byte ptr [di]	;Rotate this byte
	shr	ax,cl			;Rotate as needed
	or	ah,bl			;Get previous unused bits
	mov	bl,al			;Save new unused bits
	mov	al,ah			;Store new byte
	stosb				;  and update pointers
	dec	dx			;Loop until entire mask rotated
	jnz	rot_right_big_10

;---------------------------------------------------------------------
; Now finish the rotate by setting the first byte's high
; order bits to the bits shifted out of the last byte.
;---------------------------------------------------------------------
	or	byte ptr [si],bl
	ret

;---------------------------------------------------------------------
; Rotate the given mask left (al) bits.  If the rotate is
; more than two bits, use the fast code.
;---------------------------------------------------------------------
rot_cur_left:
	neg	al			;Mask shift count positive
	mov	bh,al			;Save rotate count
	mov	si,DataOFFSET cur_and_mask
	call	rot_left
	mov	al,bh
	mov	si,DataOFFSET cur_xor_mask
	call	rot_left
	pop	es
	ret

rot_left:
	std				;Decrement for rotating left
	cmp	al,3			;Use the big rotate code?
	jnc	rot_left_big		;  Yes, it will be faster
	mov	bl,al			;Save rotate count

rot_left_10:
	mov	di,si			;Set pointers
	mov	cx,MASK_LENGTH		;Get # bytes in mask
	add	di,cx			;--> last word
	sub	di,2
	mov	dx,di			;Will need this later
	shr	cx,1			;Compute # words to move
	errnz	<MASK_LENGTH and 1>	;Must be a multiple of 2 for words

rot_left_20:
	mov	ax,word ptr [di]	;Rotate this word
	xchg	ah,al
	rcl	ax,1
	xchg	ah,al
	stosw				;Store new word and update pointer
	loop	rot_left_20

;---------------------------------------------------------------------
; Now finish the rotate by setting the last bit in the last byte
; to the bit shifted out of the first byte.
;---------------------------------------------------------------------
	mov	di,dx			;Get pointer to last word
	mov	al,[di][1]		;  (want the last BYTE)
	rcr	al,1			;Remove unwanted bit, add in wanted bit
	rol	al,1			;Put wanted bit in D0 where it belongs
	mov	[di][1],al		;Store new byte
	dec	bl			;More to rotate?
	jnz	rot_left_10		;  Yes, rotate them
	cld				;Clear for other folks
	ret

;---------------------------------------------------------------------
; The big rotate rotates 'n' bits at a time.  Time will be saved
; using this code for rotates of over two bits.
;---------------------------------------------------------------------
rot_left_big:
	mov	cl,al			;Set rotate count
	mov	dx,MASK_LENGTH		;Get # bytes in mask
	add	si,dx			;--> last word
	dec	si
	mov	di,si			;Will need this later
	xor	bl,bl			;Zero initial previous bits

rot_left_big_10:
	xor	ax,ax			;Zero MSB for the shift
	mov	al,byte ptr [di]	;Rotate this byte
	shl	ax,cl			;Rotate as needed
	or	al,bl			;Get previous unused bits
	mov	bl,ah			;Save new unused bits
	stosb				;Store byte and update pointer
	dec	dx			;Loop until entire mask rotated
	jnz	rot_left_big_10

;---------------------------------------------------------------------
; Now finish the rotate by setting the last byte's low
; order bits to the bits shifted out of the first byte.
;---------------------------------------------------------------------
	or	byte ptr [si],bl
	cld
	ret
rotate_masks	endp


;---------------------------------------------------------------------
; Exclude - Set Cursor Exclusion Rectangle
; Entry:
;	  (cx) = left
;	  (dx) = top
;	  (si) = right	(inclusive)
;	  (di) = bottom (inclusive)
;---------------------------------------------------------------------
cProc 	exclude_far,<FAR,PUBLIC>
cBegin	<nogen>
	call 	exclude		; make near call to the "exclude" routine
	ret
cEnd	<nogen>

externW	_cstods				;in cursor.asm

cProc	exclude,<NEAR,PUBLIC>
cBegin
	mov	ds,cs:_cstods		;Need access to our data segment
	assumes ds,Data
	assumes es,nothing		;Have no idea what's in it
	and	cx,CUR_ROUND_LEFT	;Round left coordinate down
	or	si,CUR_ROUND_RIGHT	;Round right coordinate up
	mov	ax,cx
	mov	cl,IS_BUSY		;set draw busy and get current
	xchg	cl,CURSOR_STATUS	;status into CL
	mov	exclude_left,ax 	;Set up exclusion rectangle
	mov	exclude_top,dx
	mov	exclude_right,si
	mov	exclude_bottom,di
	test	cl,IS_VISIBLE		;Is the cursor visible?
	jz	short E_Exit		;no, we are done.
	call	ExcludeTest		;Test against the rectangle.
	jnc	short E_Exit		;Don't need to exclude	

E_RemoveCursor:
	push	cx
	call	CursorOff		;Remove cursor
	pop	cx
	and	cl,not IS_VISIBLE
E_Exit:                                                       
	or	cl,IS_EXCLUDED
	xchg	cl,CURSOR_STATUS	;update the cursor status flag
cEnd

;---------------------------------------------------------------------
; ExcludeTest
; Returns:
;  Carry if excluded.
;  No Carry if not excluded.
;  preserves:
;   cx
;---------------------------------------------------------------------
public	ExcludeTest
ExcludeTest	proc	near
	mov	di,Qtail
ET_Loop:
	mov	si,di
	shl	si,4
	mov	ax,Queue[si].sx
	shl	ax,3
	mov	bx,Queue[si].sy

	cmp	ax,exclude_right	;Is left of cursor > right of exclude?
	jg	short @f	 	;  Yes, not excluded
	add	ax,SAVE_WIDTH*8-1	;Add in width of cursor save area.
	cmp	ax,exclude_left 	;Is right of cursor < left of exclude?
	jl	short @f	 	;  Yes, not excluded
	cmp	bx,exclude_bottom	;Is top of cursor > bottom of exclude
	jg	short @f	 	;  Yes, not excluded
	add	bx,SAVE_HEIGHT-1 	;Add in height of cursor save area.
	cmp	bx,exclude_top		;Is bottom of cursor < top of exclude
	jl	short @f	 	;  Yes, not excluded
	stc
	ret
@@:	inc	di
	WrapQ	di
	cmp	di,Qhead
	jne	ET_Loop
	clc
	ret
ExcludeTest	endp

;---------------------------------------------------------------------
; UnExclude - Remove Exclusion Area
;---------------------------------------------------------------------
cProc 	unexclude_far,<FAR,PUBLIC>
cBegin 	<nogen>
	call 	unexclude	; make near call to the "unexclude" routine
	ret
cEnd	<nogen>

cProc	unexclude,<NEAR,PUBLIC>
cBegin
	mov	ds,cs:_cstods
	assumes ds,Data
	assumes es,nothing		;Have no idea what's in it
	mov	cl,IS_BUSY		;set draw busy and get current
	xchg	cl,CURSOR_STATUS	;status into CL
	cmp	wInhibitCount,0		;Only unexclude if User.exe says
	jnz	short @f		;it okay.
	and	cl,not IS_EXCLUDED	;Show not excluded.

U_check_for_ink_available:

	xor	ch,ch
	xchg	ch,fInkAvailable	;atomic
	or	ch,ch
	jz	U_no_ink_available

	; there is ink available, let's see if we can draw it

	test	cl,IS_BUSY
	jnz	U_screen_busy

	; we can draw the ink, so call lpfnUpdateInking and
	; then clear the fInkAvailable flag

	push	cx
	call	dword ptr lpfnUpdateInking
	pop	cx

	; maybe some new ink arrived while we were doing inking

	jmp	short U_check_for_ink_available

U_screen_busy:

	mov	fInkAvailable,1

U_no_ink_available:

@@:	xchg	cl,CURSOR_STATUS	;Put it back into status variable.
cEnd

public	DrawCursor
public	SaveToSave
public	CursorToSave
public	CursorOff
public	ScreenToSave
public	SaveToScreen
public	EraseTail
public	LoadCursor
public	rotate_masks
sEnd	Code
	end	
