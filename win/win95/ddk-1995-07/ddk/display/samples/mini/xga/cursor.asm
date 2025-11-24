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
; CURSOR.ASM
;----------------------------------------------------------------------------
        .xlist
DOS5=1
	include cmacros.inc
	include dibeng.inc
	include xga.inc
	include windefs.inc
        .list

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
	externFP DIB_Inquire
	externFP DIB_SetCursorExt
	externFP DIB_CheckCursorExt
	externFP DIB_MoveCursorExt


;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin		Data
;
;
externB 	WindowsEnabledFlag		;in VGA.ASM
externW 	CRTCRegisterBase		;in VGA.ASM
externD 	lpDriverPDevice 		;in ENABLE.ASM
externW 	wChipId 			;in VGA.ASM
externW 	wBpp				;in VGA.ASM
externD 	CursorStartAddr 		;in VGA.ASM
externD 	XGARegs 			;in VGA.ASM
;
;
public	CursorXPos, CursorYPos
CursorXPos		dw	0
CursorYPos		dw	0
;
;We allocate a fixed save area for cursorShape data structures which are
;passed to us during SetCursor calls.  Currently, we allocate a large
;enough save area for a 64x64 cursor.
;
public	CursorShapeSave
CursorShapeSave 	db	size cursorShape dup(?)
CursorShapeBitmapSave	db	256 dup(?)
CursorShapeSaveLen	equ	$ - DataOFFSET CursorShapeSave
CursorShapeBitmapLen	equ	$ - DataOFFSET CursorShapeBitmapSave
;
public	CursorDefinedFlag
CursorDefinedFlag	db	0	;1 if hardware cursor is defined
					;2 if DIB engine cursor is defined
					;0 if no cursor is defined
;
public	MouseTrailsFlag
MouseTrailsFlag 	db	0	;FFH if mouse trails enabled
;
sEnd		Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin  Code
assumes cs,Code
        .386
;
;
subttl		Set XGA Hardware Cursor Shape
page +
cProc		SetCursor,<FAR,PUBLIC>
;
	parmD	lpCursorShape
;
	localW	CursorBitmapSize
	localW	SpriteBufferPosition
;
cBegin
.386
;
;Upon executing cBegin:
;	AX contains _DATA.
;
	assumes ds,Data
	push	esi		;save 32 bit versions of these
	push	edi		;
	mov	es,ax		;we want ES --> Data for now
	assumes es,Data
	xor	al,al		;assume cursor is to be turned off
	xchg	al,CursorDefinedFlag
				;now AL has previous cursor type
	cmp	WindowsEnabledFlag,0
				;does Windows own the CRTC?
	jne	@F		;nope, don't turn off the hardware cursor
;
;First, turn off the hardware cursor:
;
	cmp	al,1		;is a hardware cursor defined?
	jne	@F		;nope, don't turn off the hardware cursor
	lfs	di,XGARegs	;FS:DI --> XGA Coprocessor registers
	MakeHardwareNotBusy	fs,di
	mov	dx,CRTCRegisterBase
	mov	ax,36h		;a zero written to reg 36H will do it
	out	dx,ax		;
	jmp	SCTestForNullCursor
;
;Next, call the DIB engine to turn off the software cursor:
;
@@:	cmp	al,2		;is a DIB engine cursor defined?
	jne	SCTestForNullCursor
				;nope, don't turn off the DIB engine cursor
	push	es		;save ES over this call
	arg	0		;send a NULL cursor shape
	arg	0		;
	arg	<word ptr lpDriverPDevice+2>
	arg	<word ptr lpDriverPDevice>
	cCall	DIB_SetCursorExt;
	pop	es		;restore ES --> Data
;
public	SCTestForNullCursor
SCTestForNullCursor:
;
;If lpCursorShape is NULL, we are to leave the cursor off:
;
@@:	cmp	seg_lpCursorShape,0
				;are we to leave cursor off?
	je	SCExit		;yes, we're done!
;
public	SCDispatch
SCDispatch:
	lds	si,lpCursorShape;DS:SI --> CursorShape data structure
	assumes ds,nothing
	mov	ax,[si].csColor ;get cursor format
	cmp	ax,0101h	;is this a color cursor?
	jne	SCCallDIBEngine ;yes, go call the DIB engine
	cmp	MouseTrailsFlag,0
				;are mouse trails turned on?
	jne	SCCallDIBEngine ;yes, go call the DIB engine
	cmp	wChipId,AGX_ID	;running on an IIT AGX?
	jne	SCDefineHardwareCursor
				;nope, continue drawing hardware cursor
	cmp	wBpp,8		;running on an IIT AGX in 8 BPP mode?
	je	SCDefineHardwareCursor
				;yes, we can use the hardware cursor
;
public	SCCallDIBEngine
SCCallDIBEngine:
;
;We call the DIB engine under certain circumstances to draw our hardware
;cursor:
;	1) If we're passed a color cursor.
;	2) If mouse trails are turned on.
;	3) If we're running the IIT AGX at 4 BPP.
;
	push	es		;save ES --> Data
	arg	lpCursorShape	;
	arg	<word ptr lpDriverPDevice+2>
	arg	<word ptr lpDriverPDevice>
	cCall	DIB_SetCursorExt;
	pop	ds		;restore DS --> Data
	assumes ds,Data
	mov	CursorDefinedFlag,2
				;indicate that we're using a DIB engine cursor
	jmp	SCTurnOnCursor	;go turn on the cursor in correct spot
;
public	SCDefineHardwareCursor
SCDefineHardwareCursor:
;
;If the focus changes to a non-Windows VM, the VDD may put the screen
;into a non-HiRes mode.  Thus, the cursor cache will be lost.  We
;therefore will copy the CursorShape data structure into our own
;Data segment buffer.	That way, if the focus does change, we'll
;be able to call SetCursor again to reset the cursor when the focus
;returns to the HiRes Windows screen.
;
;First, set ES:DI --> CursorShapeSave structure where we'll save the
;CursorShape data structure that was sent to us:
;
	assumes ds,nothing
	mov	di,DataOFFSET CursorShapeSave
;
;Get the size of the CursorShape data structure to copy:
;
	mov	ax,[si].csWidthBytes
	mov	dx,[si].csHeight
	shl	ax,1		;* 2 because there's an AND and XOR mask
	mul	dx		;now AX has total bytes in cursor bitmap
	add	ax,size cursorShape
	cmp	ax,CursorShapeSaveLen
				;make sure there's enough room to copy it!
	jg	SCExit		;not enough room, cursor was probably too big
	mov	cx,ax		;copy the cursorShape to our local data segment
	rep	movsb		;
	mov	ax,es		;make DS --> Data again
	mov	ds,ax		;
	assumes ds,Data
;
;See if the Windows VM is the owner of the display hardware (that is, we're
;not running a full-screen DOS box).  If we're running a DOS box, don't set
;the cursor into the hardware at this time.  Since we do have a valid
;cursor loaded into our save area, we do mark the cursor as defined so
;that when we get the display focus again, the cursor will be correctly
;loaded at that time (see DevToForeground in SSWITCH.ASM).
;
	cmp	WindowsEnabledFlag,0
				;are we in a disabled state?
	je	@F		;nope, continue
	mov	CursorDefinedFlag,1
				;yes! flag that hardwar cursor has been defined
	jmp	SCExit		;leave without loading the cursor
;
;We need to calculate the total size of the cursor bitmap in bytes.  This
;will be used as our AND to XOR bitmap increment and the loop counter in
;the set cursor loop:
;
@@:	mov	si,DataOFFSET CursorShapeSave
				;now DS:SI --> our local copy of cursorShape
	mov	ax,[si].csHeight
	mul	[si].csWidthBytes
	mov	CursorBitmapSize,ax
;
;Now, see if we're on an IIT AGX. It defines the cursor differently than
;the XGA:
;
	cmp	wChipId,AGX_ID	;on an IIT AGX?
	je	SCDefineAGXCursor
;
;Next, clear the 64x64 sprite buffer to transparent:
;
	mov	dx,CRTCRegisterBase
	mov	ax,60h		;set Sprite offset registers to zero so we
	out	dx,ax		;start clearing the buffer at the beginning
	inc	al		;
	out	dx,ax		;
	mov	al,6ah		;set the XGA CRTC index to the sprite buffer
	out	dx,al		;
	add	dl,2		;DX --> CRTC data register (a DWORD register)
	mov	eax,0aaaaaaaah	;this is value to write into sprite buffer
	mov	ch,64		;we'll use this as a line loop counter
;
public	SCClearSpriteBufferLoop
SCClearSpriteBufferLoop:
;
;Each pass of this loop clears one line of the sprite buffer:
;
	out	dx,eax		;
	out	dx,eax		;
	out	dx,eax		;
	out	dx,eax		;
	dec	ch		;
	jnz	SCClearSpriteBufferLoop
;
;Now, reset the hardware to setup for loading the actual sprite data:
;
	sub	dl,2		;DX --> XGA CRTC index register again
	mov	ax,60h		;set Sprite offset registers to zero so we
	out	dx,ax		;start filling the buffer at the beginning
	inc	al		;
	out	dx,ax		;
	mov	SpriteBufferPosition,0
	mov	al,6ah		;set the XGA CRTC index to the sprite buffer
	out	dx,al		;
	add	dl,2		;DX --> CRTC data register (a DWORD register)
;
;We now copy the cursor bitmap to the XGA "sprite" hardware.  It turns
;out that we need to pack the two bitmaps of the passed-in cursor bitmap
;into a 2 BPP format as we load it into the XGA hardware, XOR bit in
;low bit of 2 BPP format, AND bit in the high bit of 2 BPP format.
;
	mov	cl,byte ptr [si].csHeight
				;CL contains the line loop counter
	mov	ch,byte ptr [si].csWidthBytes
				;CH contains the source byte loop counter
	mov	esi,DataOFFSET CursorShapeBitmapSave
				;DS:ESI --> the actual cursor bitmap to write
	movzx	edi,CursorBitmapSize
				;we use this as our AND to XOR increment
;
public	SCLoadSpriteLineLoop
SCLoadSpriteLineLoop:
	push	cx		;save line loop counter and byte loop counter
;
public	SCLoadSpriteByteLoop
SCLoadSpriteByteLoop:
	mov	bh,[si] 	;BH contains next byte of AND bitmap
	mov	bl,[esi+edi]	;BL contains next byte of XOR bitmap
;
	rol	bl,1		;get bit 0 of XOR bitmap into CY
	rcr	ax,1		;and store it into pixel 0 of packed format
	rol	bh,1		;get bit 0 of AND bitmap into CY
	rcr	ax,1		;and store it into pixel 0 of packed format
;
	rol	bl,1		;get bit 1 of XOR bitmap into CY
	rcr	ax,1		;and store it into pixel 1 of packed format
	rol	bh,1		;get bit 1 of AND bitmap into CY
	rcr	ax,1		;and store it into pixel 1 of packed format
;
	rol	bl,1		;get bit 2 of XOR bitmap into CY
	rcr	ax,1		;and store it into pixel 2 of packed format
	rol	bh,1		;get bit 2 of AND bitmap into CY
	rcr	ax,1		;and store it into pixel 2 of packed format
;
	rol	bl,1		;get bit 3 of XOR bitmap into CY
	rcr	ax,1		;and store it into pixel 3 of packed format
	rol	bh,1		;get bit 3 of AND bitmap into CY
	rcr	ax,1		;and store it into pixel 3 of packed format
;
	rol	bl,1		;get bit 4 of XOR bitmap into CY
	rcr	ax,1		;and store it into pixel 4 of packed format
	rol	bh,1		;get bit 4 of AND bitmap into CY
	rcr	ax,1		;and store it into pixel 4 of packed format
;
	rol	bl,1		;get bit 5 of XOR bitmap into CY
	rcr	ax,1		;and store it into pixel 5 of packed format
	rol	bh,1		;get bit 5 of AND bitmap into CY
	rcr	ax,1		;and store it into pixel 5 of packed format
;
	rol	bl,1		;get bit 6 of XOR bitmap into CY
	rcr	ax,1		;and store it into pixel 6 of packed format
	rol	bh,1		;get bit 6 of AND bitmap into CY
	rcr	ax,1		;and store it into pixel 6 of packed format
;
	rol	bl,1		;get bit 7 of XOR bitmap into CY
	rcr	ax,1		;and store it into pixel 7 of packed format
	rol	bh,1		;get bit 7 of AND bitmap into CY
	rcr	ax,1		;and store it into pixel 7 of packed format
;
;See if we're at the end of a line:
;
	out	dx,ax		;write the finished packed pixel word
	inc	si		;bump to next byte in source bitmap
	dec	ch		;at the end of our line?
	jnz	SCLoadSpriteByteLoop
				;nope, go do next byte of line
;
;We're at the end of our line, bump the hardware so we're looking at the
;next line of the sprite buffer.
;
;NOTE: The way that the XGA reference manual documents the Sprite Index
;      Registers (60H & 61H) is WRONG.	They are incremented by 1 for
;      every 4 pixels written.	Therefore, since the sprite buffer has
;      64 pixels in each horizontal scanline, we increment the Sprite
;      Index Registers by 16 to increment to the next line.
;
	pop	cx		;restore line loop counter to CL
				;restore byte loop counter to CH
	dec	cl		;are we done loading sprite?
	jz	SCDoneLoadingCursor
				;yes! we're done
	sub	dl,2		;DX --> XGA CRTC index register again
	add	SpriteBufferPosition,16
				;bump to next line of sprite buffer
	mov	al,60h		;set Sprite offset register to next line
	mov	ah,byte ptr SpriteBufferPosition
	out	dx,ax		;set hardware to next line
	inc	al		;
	mov	ah,byte ptr SpriteBufferPosition+1
	out	dx,ax		;
	mov	al,6ah		;set the XGA CRTC index to the sprite buffer
	out	dx,al		;
	add	dl,2		;DX --> CRTC data register (a DWORD register)
	jmp	SCLoadSpriteLineLoop
;
public	SCDefineAGXCursor
SCDefineAGXCursor:
	mov	cl,byte ptr [si].csHeight
				;CL contains the line loop counter
	mov	ch,byte ptr [si].csWidthBytes
				;CH contains the source byte loop counter
	mov	esi,DataOFFSET CursorShapeBitmapSave
				;DS:ESI --> the actual cursor bitmap to write
	movzx	ebx,CursorBitmapSize
				;we use this as our AND to XOR increment
;
;Next, clear the 64x64 sprite buffer to transparent:
;
	les	edi,fword ptr CursorStartAddr
	assumes es,nothing
	push	cx
	push	edi
	mov	cx, 200h
;
SCFillTransparentLoop_AGX:
	mov	dword ptr es:[edi],0aaaaaaaah
	add	edi,4
	loop	SCFillTransparentLoop_AGX
	pop	edi
	pop	cx
	add	di, 32*32+16	;put the cursor image in lower_right corner
	add	di, 2		;of 2K reserved area
;
public	SCLoadSpriteLineLoop_AGX
SCLoadSpriteLineLoop_AGX:
	push	cx		;save line loop counter and byte loop counter
;
SCLoadSpriteByteLoop_AGX:
	mov	dh,[si] 	;BH contains next byte of AND bitmap
	mov	dl,[esi+ebx]	;BL contains next byte of XOR bitmap
	inc	si		;bump to next byte in source bitmap
REPT	8
	rol	dl,1		;get bit 0 of XOR bitmap into CY
	rcr	ax,1		;and store it into pixel 0 of packed format
	rol	dh,1		;get bit 0 of AND bitmap into CY
	rcr	ax,1		;and store it into pixel 0 of packed format
ENDM
	mov	es:[edi], al	;write the finished packed pixel word
	mov	es:[edi+1], ah	;write the finished packed pixel word
	add	edi, 4
	dec	ch		;at the end of our line?
	jnz	SCLoadSpriteByteLoop_AGX
	add	edi, 16		;next line
	pop	cx		;restore line loop counter to CL
	dec	cl		;are we done loading sprite?
	jnz	SCLoadSpriteLineLoop_AGX
;
public	SCDoneLoadingCursor
SCDoneLoadingCursor:
	mov	CursorDefinedFlag,1
				;flag that a hardware cursor has been defined
;
public	SCTurnOnCursor
SCTurnOnCursor:
	arg	CursorXPos	;send this as absX
	arg	CursorYPos	;send this as absY
	cCall	MoveCursor	;this will move us to the proper position
	cmp	CursorDefinedFlag,2
				;using a DIB engine cursor?
	je	SCExit		;yes, skip the following
	mov	dx,CRTCRegisterBase
	mov	ax,0136h	;this will turn on the cursor
	out	dx,ax		;
;
SCExit:
	pop	edi		;restore EDI & ESI
	pop	esi		;
.286c
cEnd
;
;
subttl		CheckCursor Entry Point
page +
cProc		CheckCursor,<FAR,PUBLIC>
;
cBegin		<nogen>
	mov	ax,DGROUP	;
	mov	ds,ax		;make DS --> Data
	assumes ds,Data
	cmp	CursorDefinedFlag,2
				;using the DIB engine cursor?
	jne	CCExit		;nope, we don't need to do anything
	cmp	WindowsEnabledFlag,0
				;does Windows own the screen?
	jne	CCExit		;nope, we don't need to do anything
	arg	<word ptr lpDriverPDevice+2>
	arg	<word ptr lpDriverPDevice>
	cCall	DIB_CheckCursorExt
;
CCExit:
	ret			;we have a hardware cursor, never excluded!
cEnd		<nogen>
;
;
subttl		MoveCursor Routine
page +
cProc		MoveCursor,<FAR,PUBLIC>,<di>
;
        parmW   absX
	parmW	absY
;
cBegin
.386
;
;First, save off the passed in coordinates in our Data segment.  We'll
;need them if SetCursor is called in order to put the new cursor up at
;the correct coordinates:
;
	assumes ds,Data
	mov	bx,absX 	;
	mov	CursorXPos,bx	;
	mov	cx,absY 	;
	mov	CursorYPos,cx	;
;
;Now, see if the cursor should be turned on:
;
	cmp	CursorDefinedFlag,0
	je	MCExit		;no cursor defined, don't do anything
;
;See if we should call the DIB engine to move the cursor:
;
	cmp	CursorDefinedFlag,2
				;using a DIB engine cursor?
	jne	MCMoveHardwareCursor
	arg	bx		;send X position
	arg	cx		;send Y position
	arg	<word ptr lpDriverPDevice+2>
	arg	<word ptr lpDriverPDevice>
	cCall	DIB_MoveCursorExt
	jmp	MCExit		;
;
public	MCMoveHardwareCursor
MCMoveHardwareCursor:
;
;Just setup the hardware with the new position corrected for the Hot-Spots!
;We correct for the Hot-Spots by using the Horizontal and Vertical Preset
;registers.  If our coordinate when corrected for the Hot-Spots, becomes
;negative, we have to set the Preset registers accordingly so that the
;cursor actually can start at zero.
;
	cmp	WindowsEnabledFlag,0
				;is Windows active on the screen?
	jne	MCExit		;no! leave without moving the cursor
	cmp	wChipId,AGX_ID	;running on an IIT AGX?
	jne	MCMoveXGACursor ;nope, go do it for the XGA
;
public	MCMoveAGXCursor
MCMoveAGXCursor:
	push	fs		;better save this
	lfs	di,XGARegs	;FS:DI --> XGA Coprocessor registers
	MakeHardwareNotBusy	fs,di
	pop	fs		;restore saved FS
	mov	dx,CRTCRegisterBase
				;DX --> XGA Extended CRTC index register
	mov	ax,2032h	;Horizontal preset = 32 because cursor
				;is in lower_right corner of 2K area
	sub	bx,CursorShapeSave.csHotX
	jge	@F		;our starting X-coordinate is positive!
	sub	ah, bl		;
	xor	bx, bx		;if cursor is wide enough, we'll go to zero
@@:	out	dx,ax		;set the Sprite Horizontal Preset register
	mov	al,30h		;this is Sprite Horizontal Start Reg Low
	mov	ah,bl		;
	out	dx,ax		;
	inc	al		;this is Sprite Horizontal Start Reg High
	mov	ah,bh		;
	out	dx,ax		;
;
	mov	ax, 2035h  	;Vertical preset = 32
	sub	cx,CursorShapeSave.csHotY
	jge	@F		;our starting X-coordinate is positive!
	sub	ah, cl
	xor	cx, cx		;if cursor is high enough, we'll go to zero
@@:	out	dx,ax		;set the Sprite Vertical Preset register
	mov	al,33h		;this is Sprite Vertical Start Reg Low
	mov	ah,cl		;
	out	dx,ax		;
	inc	al		;this is Sprite Vertical Start Reg High
	mov	ah,ch		;
	out	dx,ax		;
	jmp	MCExit		;we're done on the AGX
;
public	MCMoveXGACursor
MCMoveXGACursor:
	mov	dx,CRTCRegisterBase
				;DX --> XGA Extended CRTC index register
	mov	ax,32h		;assume no Horizontal preset needed
	sub	bx,CursorShapeSave.csHotX
	jge	@F		;our starting X-coordinate is positive!
	add	bx,CursorShapeSave.csHotX
				;if cursor is wide enough, we'll go to zero
	mov	ah,byte ptr CursorShapeSave.csHotX
@@:	out	dx,ax		;set the Sprite Horizontal Preset register
	mov	al,30h		;this is Sprite Horizontal Start Reg Low
	mov	ah,bl		;
	out	dx,ax		;
	inc	al		;this is Sprite Horizontal Start Reg High
	mov	ah,bh		;
	out	dx,ax		;
;
	mov	ax,35h		;assume no Vertical preset needed
	sub	cx,CursorShapeSave.csHotY
	jge	@F		;our starting X-coordinate is positive!
	add	cx,CursorShapeSave.csHotY
				;if cursor is wide enough, we'll go to zero
	mov	ah,byte ptr CursorShapeSave.csHotY
@@:	out	dx,ax		;set the Sprite Vertical Preset register
	mov	al,33h		;this is Sprite Vertical Start Reg Low
	mov	ah,cl		;
	out	dx,ax		;
	inc	al		;this is Sprite Vertical Start Reg High
	mov	ah,ch		;
	out	dx,ax		;
;
MCExit:
.286c
cEnd
;
;
subttl		Inquire Entry Point (Handled by DIB Engine)
page +
cProc		Inquire,<PUBLIC,FAR>
;
	parmD	lpCursorInfo
;
cBegin		<nogen>
.386
	jmp	DIB_Inquire
.286c
cEnd		<nogen>
;
;
sEnd            Code
end
