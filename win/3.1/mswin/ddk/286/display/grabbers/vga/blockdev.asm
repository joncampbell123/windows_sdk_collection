
		page	,132
		%out	Get/Put/MarkBlockDev
		name	BLOCKDEV
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	BLOCKDEV.ASM
;
; DESCRIPTION
;	This file contains the device-specific inner and outer loops for
;	the block routines.
;
; SEE ALSO
;	BLOCK.ASM
;
; HISTORY
;	1.00	060187	jhc	Epoch
;	1.01	080787	jhc	Prevent adding extra crlf at end of buffer
;				for FMT_OTHER (clipboard) in GetBlockDev.
;	1.10	060688	vvr	changed widthbytes from byte to word
;
;	1.11	072789	jem	IFDEF unused functions for Windows 3.0
;
;	Copyright (c) 1989  Microsoft.	All Rights Reserved.


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT,ds:nothing

		.xlist
		include vgaic.inc
		include grabber.inc
		.list

		extrn	IC:byte


ATTR_TOGGLE	=	01110111b



gbFuncTable	label	word			;GetBlockDev dispatch table
		dw	gbFmtNative
		dw	gbFmtOther


		subttl	GetBlockDev
		page


;
; GetBlockDev - inner and outer loops of GetBlock
;
; ENTRY
;	ax	=  undefined
;	bx	=  undefined
;	cx	=  0
;	dx	=  Yext:Xext
;	bp	=  fFormat function request number (pre-adjusted)
;	ds:si	-> seg:offset of starting position on screen
;	es:di	-> seg:offset of user buffer to receive data
; EXIT
;	none
; USES
;	ax, bx, cx, dx, si, di, flags
;
		public	GetBlockDev

GetBlockDev	proc	near
		assume	ds:nothing
		cld	  			
;		mov	bh,ch			;ch = 0
		mov	bx,cs:[IC.icWidthBytes] ;bl = width of screen in bytes ~vvr
		call	cs:[gbFuncTable][bp]	;dispatch with bp
		ret

;
; gbFmtNative will copy character/attribute pairs for the whole rectangular
; block.
;

gbFmtNative:
		push	si			;save row start address
		mov	cl,dl			;cl = Xext
		rep	movsw			;jam first row
		pop	si			;recover row start address
		add	si,bx			;bump to next row
		dec	dh			;dec Yext
		jnz	gbFmtNative		;loop until Yext = 0
		ret

;
; gbFmtOther will copy characters only in the rectangular block to the data
; buffer in a variant of the clipboard format, which is defined by the AbSt
; structure in GRABBER.INC.  This format specifies that the end of each row of
; characters will contain a carriage-return/linefeed pair, except for the end
; of the last row, which must contain a word of zeros for null-termination.
;

gbFmtOther:
		mov	ax,cx			;ax = 0
		mov	al,GT_TEXT
		stosw				;stash gbType
		stosw				;and a bogus length for now
		push	di			;save start of destination data
gbFmtOtherOuter:
		push	si			;save row start address
		mov	cl,dl			;cl = Xext
		mov	ah,020h 		;ah = whitespace substitute
gbFmtOtherInner:
		lodsb				;get a screen character
		inc	si			;bump si past attribute
		or	al,al			;if char = 0,
		jz	gbFixChar		;  go substitute whitespace
		cmp	al,-1			;if char = 0FFh,
		je	gbFixChar		;  go substitute whitespace
		stosb				;else stash in buffer as is
		loop	gbFmtOtherInner 	;keep going until end of row
		jmp	short gbAddCrLf 	;then add a terminal CrLf
gbFixChar:
		mov	al,ah			;change to a space
		stosb				;stash in buffer
		loop	gbFmtOtherInner 	;keep going until end of row
gbAddCrLf:
		mov	ax,00A0Dh		;add a CrLf at end of row
		stosw
		pop	si			;recover row start address
		add	si,bx			;bump to next row
		dec	dh			;dec Yext
		jnz	gbFmtOtherOuter 	;loop until Yext = 0
gbNullTerminate:
		sub	di,2			;munge last CrLf
		xor	ax,ax			;null terminate whole buffer
		stosw
		mov	ax,di			;get current buffer address
		pop	di			;recover start of data
		sub	ax,di			;calculate data length
		mov	es:[di][-2],ax		;and stash it in gbSize
		ret
GetBlockDev	endp


IFDEF	GRAB_VER_2

		subttl	PutBlockDev
		page


pbFuncTable	label	word			;PutBlockDev dispatch table
		dw	pbFillBoth
		dw	pbFillChar
		dw	pbFillAttr
		dw	pbCopyBoth
		dw	pbCopyChar
		dw	pbCopyAttr
		dw	pbCopyCharFillAttr
		dw	pbCopyAttrFillChar

ENDIF	;GRAB_VER_2

IFDEF	GRAB_VER_2

;
; PutBlockDev - inner and outer loops of PutBlock
;
; ENTRY
;	ax	=  Attr:Char for fill operations
;	bx	=  undefined
;	cx	=  0
;	dx	=  Yext:Xext
;	bp	=  fScreenOp function request number (pre-adjusted)
;	ds:si	-> seg:offset of user data for copy operations
;	es:di	-> seg:offset of starting position on screen
; EXIT
;	none
; USES
;	ax, bx, cx, dx, si, di, bp, flags
;
		public	PutBlockDev

PutBlockDev	proc	near

		assume	ds:nothing
		cld
;		mov	bh, ch			;ch = 0
		mov	bx,cs:[IC.icWidthBytes] ;bl = width of screen in bytes ~vvr
		add	bp,offset pbFuncTable	;dispatch with bp
pbOuterLoop:
		push	di			;save row start address
		mov	cl,dl			;cl = Xext
		call	cs:[bp] 		;dispatch inner loop
		pop	di			;recover row start address
		add	di,bx			;bump to next row
		dec	dh			;dec Yext
		jnz	pbOuterLoop		;loop until Yext = 0
		ret


;
; PutBlockDev inner loop routines follow.
;


pbFillBoth:
		rep	stosw			;fill with Attr & Char
		ret

pbFillChar:
		stosb				;fill with Char
		inc	di			;bump past attribute
		loop	pbFillChar
		ret

pbFillAttr:
		mov	al,ah			;al = Attr
pbFillAttr$:
		inc	di			;bump past character
		stosb				;fill with Attr
		loop	pbFillAttr$
		ret

pbCopyBoth:
		rep	movsw			;copy both chars & attributes
		ret

pbCopyChar:
		movsb				;copy char data only
		inc	di			;bump past attribute
		loop	pbCopyChar
		ret

pbCopyAttr:
		inc	di			;bump past character
		movsb				;fill with Attr data only
		loop	pbCopyAttr
		ret

pbCopyCharFillAttr:
		lodsb				;get char data
		stosw				;stash char, fill with Attr
		loop	pbCopyCharFillAttr
		ret

pbCopyAttrFillChar:
		mov	ah,ds:[si]		;get attr data
		stosw				;stash attr, fill with Char
		inc	si			;bump to next attr data
		loop	pbCopyAttrFillChar
		ret

PutBlockDev	endp

ENDIF	;GRAB_VER_2

IFDEF	GRAB_VER_2

		subttl	MarkBlockDev
		page


;
; MarkBlockDev - inner and outer loops of MarkBlock
;
; ENTRY
;	ax	=  undefined
;	bx	=  undefined
;	cx	=  0
;	dx	=  Yext:Xext
;	ds:si	-> seg:offset of starting position on screen
; EXIT
;	none
; USES
;	bx, cx, dx, si, flags
;
		public	MarkBlockDev

MarkBlockDev	proc	near

		assume	ds:nothing
;		mov	bh,ch			;ch = 0
		mov	bx,cs:[IC.icWidthBytes] ;bl = width of screen in bytes ~vvr
mbOuterLoop:
		push	si			;save row start address
		mov	cl,dl			;cl = Xext
mbInnerLoop:
		xor	byte ptr ds:[si],ATTR_TOGGLE
		inc	si			;bump to next attribute
		inc	si
		loop	mbInnerLoop		;keep going until end of row

		pop	si			;recover row start address
		add	si,bx			;bump to next row
		dec	dh			;dec Yext
		jnz	mbOuterLoop		;loop until Yext = 0
		ret

MarkBlockDev	endp

ENDIF	;GRAB_VER_2

_TEXT		ends
		end
