
		page	,132
		%out	Get/Put/MarkBlock
		name	BLOCK
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	BLOCK.ASM
;
; DESCRIPTION
;	This file contains the bulk of the code for the GetBlock, PutBlock,
;	and MarkBlock routines.
;
; SEE ALSO
;	BLOCKDEV.ASM in both EGA and CGAHERC subdirectories.
;
; HISTORY
;	1.00	060187	jhc	Epoch
;	1.01	080787	jhc	Added code to support fullscreen MarkBlock
;				and GetBlock on graphics screens.
;
;	1.02	081887	jhc	Removed an overlooked int 3
;
;	1.03	091787	jhc	Buffer size calculation at gbCalcOther was 2
;				bytes too long after removal of extra crlf
;				in the BlockDev modules.
;
;	1.04	072789	jem	IFDEF unused functions for Windows 3.0
;
;	Copyright (c) 1989 Microsoft.  All Rights Reserved.


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include vgaic.inc
		include grabber.inc
		.list

		extrn	IC:byte
		extrn	GetBlockDev:near
		extrn	GrabBits:near

IFDEF	GRAB_VER_2

		extrn	PutBlockDev:near
		extrn	MarkBlockDev:near
		extrn	InvertGrph:near

ENDIF	;GRAB_VER_2

		subttl	GetBlock


;
; GetBlock - copy rectangular area of screen to buffer
;
;	GetBlock copies a rectangular block of screen data to a specified
;	buffer in a specified format.
;
; ENTRY
;	ds	=  cs
;	es:di	-> GrabRequest structure
;
;	In GrabRequest:
;
;	lpData	If lpData = NULL, GetBlock does not actually perform the
;		transfer; it simply returns the number of bytes the operation
;		would have required.  Otherwise, lpData is assumed to point to
;		the buffer to receive the screen data.
;
;	Xorg	Xorg specifies the unsigned x coordinate of the origin of
;		the transfer in alpha coordinate space.
;
;	Yorg	Yorg specifies the unsigned y coordinate of the origin of
;		the transfer in alpha coordinate space.
;
;	Xext	Xext specifies the unsigned x extent of the rectangular block
;		to transfer as measured from the origin specified by Xorg and
;		Yorg.  If Xext is zero, the entire width of the current screen
;		is assumed.
;
;	Yext	Yext specifies the unsigned y extent of the rectangular block
;		to transfer as measured from the origin specified by Xorg and
;		Yorg.  If Yext is zero, the entire height of the current
;		screen is assumed.
;
;	Style	The only field of Style used for GetBlock operations is the
;		fFormat field.
;
;		If fFormat = FMT_NATIVE, the data is copied in the native
;		screen format for the current screen mode.  Thus alpha screens
;		result in character/attribute pairs being copied, while
;		graphics screens result in a bitmap.  Note that native screen
;		mode is most useful for saving areas where a popup menu will
;		appear; using the data returned from GetBlock, a subsequent
;		PutBlock with fFormat = FMT_NATIVE, will restore that area.
;		Since the native format is screen dependent, it is recommended
;		that it be used for save/restore purposes only; no
;		interpretation of the data should be attempted.
;
;		If fFormat = FMT_OTHER, the data will be copied in a variant
;		of the Windows clipboard format defined by the grab buffer
;		structure GrabSt in GRABBER.INC.  This buffer will have gbType
;		GT_TEXT for alpha screens and gbType GT_NEWBITMAP format for
;		graphics screens.
;
;	Char	Not used for GetBlock operations.
;
;	Attr	Not used for GetBlock operations.
;
; EXIT
;	ax	=  number of bytes transfered
;
; ERROR EXIT
;	ax	=  error code
;	cf	=  1
;
; USES
;	ax, flags
;
; NOTES
;	Note that when using FMT_OTHER in graphics mode, GetBlock attempts to
;	reduce the amount of informationless data in the region by finding a
;	minimum bounding box for the graphic image.  Since the computation of
;	the bounding box is based on the screen having been inverted by a
;	MarkBlock request, the byte count returned when lpData = NULL will
;	always reflect the size of the buffer required to capture a region of
;	the requested size assuming no compression was possible.  When lpData
;	!= NULL, it is assumed a standard grab is in progress, that the screen
;	has been inverted, and that compression is desired.  Thus in this
;	case, the size returned may be LESS that that returned by a call with
;	lpData = NULL, but no case will it exceed that size.
;
;	In text mode, the display adapter may contain multiple character sets
;	or allow downloadble character sets.  The data returned in clipboard
;	format will be translated to the 'standard' OEM set if it can be
;	determined that another set is in use, which character set it is, and
;	if a translation table is available for the job.  The EGA is an
;	example of an adapter that makes it hard to determine that a set has
;	been downloaded and practically impossible to determine which one it
;	is.
;
;
		public	GetBlock

GetBlock	proc	near
		assume	ds:_TEXT
		push	bx
		push	cx
		push	dx
		push	si
		push	di
		push	bp
		push	ds
		push	es

		mov	dl,es:[di][grXext]
		mov	dh,es:[di][grYext]
		mov	bl,es:[di][grXorg]
		mov	bh,es:[di][grYorg]

		test	[IC.icScrType],ST_GRPH	;if graphics screen,
		jnz	gbGrph			;  handle it separately

		xor	ax,ax			;ax, cx = 0
		mov	cx,ax
		cmp	dl,al			;if Xext != 0,
		jne	gbXok			;  go check Yext
		mov	dl,[IC.icCharsX]	;else use full X screen extent
		mov	bl,al			;  and set Xorg = 0;
gbXok:
		cmp	dh,al			;if Yext != 0,
		jne	gbYok			;  go compute buffer size
		mov	dh,[IC.icCharsY]	;else use full Y screen extent
		mov	bh,al			;  and set Yorg = 0;
gbYok:
		mov	al,dl			;compute buffer size
		mul	dh			;ax = xy
		mov	bp,word ptr es:[di][grStyle]
		and	bp,FMT_OTHER		;if style is not FMT_OTHER,
		jz	gbCalcNative		;  calculate for FMT_NATIVE
gbCalcOther:
		mov	bp,2
		mov	cl,dh
		shl	cl,1			;cl = 2y
		add	ax,cx			;ax = xy + 2y
		add	ax,2 + 2		;ax = xy + 2y + slop
		jmp	short gbCalc
gbCalcNative:
		shl	ax,1			;ax = 2xy
gbCalc:
		les	di,es:[di][grlpData]	;get -> buffer
		mov	cx,es
		or	cx,di			;if NULL,
		jz	gbExitGood		;  just return with size in ax

		push	ax			;save size
		mov	al,[IC.icCharsX]	;al = current screen width
		mul	bh			;ax = (Yorg*width + Xorg)
		add	al,bl
		adc	ah,0
		shl	ax,1			;ax = ax*2 for attibutes

		lds	si,[IC.iclpScr]
		assume	ds:nothing
		add	si,ax			;regen addr = ax + crtstart
		xor	cx,cx

		call	GetBlockDev		;let low level routines do it

		pop	ax			;recover size
gbExitGood:
		clc				;show success
gbExit:
		pop	es
		pop	ds
		pop	bp
		pop	di
		pop	si
		pop	dx
		pop	cx
		pop	bx
		ret

gbGrph:
		or	dx,dx				;if not full screen,
		jnz	gbErrUs 			;  we don't support it

		test	es:[di][grStyle],FMT_OTHER	;if not GRAB (FMT_OTHER),
		jz	gbErrUS 			;  we don't support it

		test	[IC.icScrType],ST_LARGE 	;if not a "tame" mode,
		jnz	gbErrUS 			;  we don't support it

		call	GrabBits
		jmp	short gbExitGood
gbErrUs:
		mov	ax,ERR_UNSUPPORTED
		stc					;show error
		jmp	short gbExit
GetBlock	endp


IFDEF	GRAB_VER_2

		subttl	PutBlock
		page


;
; PutBlock - copy buffer to rectangular area of screen
;
;	PutBlock copies a specified buffer in a specified format to a
;	rectangular block on the screen.
;
; ENTRY
;	ds	=  cs
;	es:di	-> GrabRequest structure
;
;	In GrabRequest:
;
;	lpData	lpData must point to the buffer from which data will be
;		copied.
;
;	Xorg	Xorg specifies the unsigned x coordinate of the origin of
;		the transfer in alpha coordinate space.
;
;	Yorg	Yorg specifies the unsigned y coordinate of the origin of
;		the transfer in alpha coordinate space.
;
;	Xext	Xext specifies the unsigned x extent of the rectangular block
;		to transfer as measured from the origin specified by Xorg and
;		Yorg.  If Xext is zero, the entire width of the current screen
;		is assumed.
;
;	Yext	Yext specifies the unsigned y extent of the rectangular block
;		to transfer as measured from the origin specified by Xorg and
;		Yorg.  If Yext is zero, the entire height of the current
;		screen is assumed.
;
;	Style	Both the fFormat and fScreenOP fields of Style must be
;		valid for PutBlock operations.
;
;		fFormat:
;
;		If fFormat = FMT_NATIVE, the data is copied in the native
;		screen format for the current screen mode.  This is useful for
;		restoring sections of the screen previously saved using a
;		GetBlock with fFormat = FMT_NATIVE.  fScreenOp will be ignored
;		for this format.
;
;		If fFormat = FMT_OTHER, the fScreenOp field of Style specifies
;		the screen operation requested and thus, explicitly specifies
;		the format of the input data.
;
;		fScreenOp:
;
;		fScreenOp must be one of the 8 values defined in
;		GRABBER.INC.  The value of fScreenOp determines the expected
;		format of the input buffer at lpData.  Note that 5 of these 8
;		opcodes specify a fill operation with either a character
;		operand, an attribute operand, or both.  In these cases, the
;		GrabRequest elements Char and Attr must contain the character
;		or attribute to be used for the fill.
;
;	Char	If fScreenOp is either F_BOTH, F_CHAR, or C_ATTR_F_CHAR, this
;		must contain the character code to be used for the fill
;		operation.
;
;	Attr	If fScreenOp is either F_BOTH, F_ATTR, or C_CHAR_F_ATTR, this
;		must contain the attribute code to be used for the fill
;		operation.
;
; EXIT
;	none
;
; ERROR EXIT
;	ax	=  error code
;	cf	=  1
;
; USES
;	ax, flags
;
;

		public	PutBlock

PutBlock	proc	near


		assume	ds:_TEXT
		push	bx
		push	cx
		push	dx
		push	si
		push	di
		push	bp
		push	ds
		push	es

		mov	dl,es:[di][grXext]
		mov	dh,es:[di][grYext]
		mov	bl,es:[di][grXorg]
		mov	bh,es:[di][grYorg]

		test	[IC.icScrType],ST_GRPH	;if graphics screen,
		jnz	pbGrph			;  handle it separately

		xor	ax,ax			;ax, cx = 0
		mov	cx,ax
		cmp	dl,al			;if Xext != 0,
		jne	pbXok			;  go check Yext
		mov	dl,[IC.icCharsX]	;else use full X screen extent
		mov	bl,al			;  and set Xorg = 0;
pbXok:
		cmp	dh,al			;if Yext != 0,
		jne	pbYok			;  go compute buffer size
		mov	dh,[IC.icCharsY]	;else use full Y screen extent
		mov	bh,al			;  and set Yorg = 0;
pbYok:
		mov	al,[IC.icCharsX]	;al = current screen width
		mul	bh			;ax = (Yorg*width + Xorg)
		add	al,bl
		adc	ah,0
		shl	ax,1			;ax = ax*2 for attibutes
		mov	cx,ax			;save offset for now

		mov	bp,word ptr es:[di][grStyle]
		test	bp,FMT_OTHER
		jnz	pbFixBp
		mov	bp,C_BOTH		;FMT_NATIVE implies copy both
pbFixBp:
		and	bp,SCR_OP_MASK
		shl	bp,1			;bp has function index
		mov	ah,es:[di][grAttr]	;ah has attr
		mov	al,es:[di][grChar]	;al has char

		lds	si,es:[di][grlpData]	;ds:si -> src data
		les	di,cs:[IC.iclpScr]	;es:di -> dst data (screen)
		assume	ds:nothing
		add	di,cx			;bump to correct scr offset
		xor	cx,cx

		call	PutBlockDev		;let low level routines do it
pbExitGood:
		clc				;show success
pbExit:
		pop	es
		pop	ds
		pop	bp
		pop	di
		pop	si
		pop	dx
		pop	cx
		pop	bx
		ret

pbGrph:
		mov	ax,ERR_UNSUPPORTED
		stc				;show error
		jmp	short pbExit


PutBlock	endp

ENDIF	;GRAB_VER_2

		subttl	MarkBlock
		page


IFDEF	GRAB_VER_2

;
; MarkBlock - toggle color of screen block
;
;	MarkBlock toggles the color of a rectangular block of the screen such
;	that the block can be differentiated from non-marked regions of the
;	screen.  The marking process is performed with a reversible operation
;	(xor, not) such that a subsequent call to mark the same block
;	effectively unmarks the the region.  This allows Winoldap to perform a
;	marking operation in a non-destructive manner so that the block need
;	not be saved beforehand.
;
; ENTRY
;	ds	=  cs
;	es:di	-> GrabRequest structure
;
;	In GrabRequest:
;
;	lpData	Not used for MarkBlock operations.
;
;	Xorg	Xorg specifies the unsigned x coordinate of the origin of
;		the mark in alpha coordinate space.
;
;	Yorg	Yorg specifies the unsigned y coordinate of the origin of
;		the mark in alpha coordinate space.
;
;	Xext	Xext specifies the unsigned x extent of the rectangular block
;		to mark as measured from the origin specified by Xorg and
;		Yorg.  If Xext is zero, the entire width of the current screen
;		is assumed.
;
;	Yext	Yext specifies the unsigned y extent of the rectangular block
;		to mark as measured from the origin specified by Xorg and
;		Yorg.  If Yext is zero, the entire height of the current
;		screen is assumed.
;
;	Style	Not used for MarkBlock operations.
;
;	Char	Not used for MarkBlock operations.
;
;	Attr	Not used for MarkBlock operations.
;
; EXIT
;	none
;
; ERROR EXIT
;	ax	=  error code
;	cf	=  1
; USES
;	ax, flags
;
;

		public	MarkBlock

MarkBlock	proc	near


		assume	ds:_TEXT
		push	bx
		push	cx
		push	dx
		push	si
		push	ds

		mov	dl,es:[di][grXext]
		mov	dh,es:[di][grYext]
		mov	bl,es:[di][grXorg]
		mov	bh,es:[di][grYorg]

		test	[IC.icScrType],ST_GRPH	;if graphics screen,
		jnz	mbGrph			;  handle it separately

		xor	ax,ax			;ax, cx = 0
		mov	cx,ax
		cmp	dl,al			;if Xext != 0,
		jne	mbXok			;  go check Yext
		mov	dl,[IC.icCharsX]	;else use full X screen extent
		mov	bl,al			;  and set Xorg = 0;
mbXok:
		cmp	dh,al			;if Yext != 0,
		jne	mbYok			;  go compute buffer size
		mov	dh,[IC.icCharsY]	;else use full Y screen extent
		mov	bh,al			;  and set Yorg = 0;
mbYok:
		mov	al,[IC.icCharsX]	;al = current screen width
		mul	bh			;ax = (Yorg*width + Xorg)
		add	al,bl
		adc	ah,0
		shl	ax,1			;ax = ax*2 for attibutes

		inc	ax			;bump to first attribute addr
		lds	si,[IC.iclpScr]
		assume	ds:nothing
		add	si,ax			;regen addr = ax + crtstart

		call	MarkBlockDev		;let low level routines do it
mbExitGood:
		clc
mbExit:
		pop	ds
		pop	si
		pop	dx
		pop	cx
		pop	bx
		ret

mbGrph:
		or	dx,dx			;if not full screen,
		jnz	mbErrUs 		;  we don't support it

		test	[IC.icScrType],ST_LARGE ;if not a "tame" mode,
		jnz	mbErrUS 		;  we don't support it

		call	InvertGrph		;let low level routines do it
		jmp	short mbExitGood
mbErrUs:
		mov	ax,ERR_UNSUPPORTED
		stc				;show error
		jmp	short mbExit


MarkBlock	endp

ENDIF	;GRAB_VER_2

_TEXT		ends
		end
