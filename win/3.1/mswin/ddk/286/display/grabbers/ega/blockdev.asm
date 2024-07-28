
		page	,132
		%out	Get/Put/MarkBlockDev
		name	BLOCKDEV
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; HISTORY
;	1.01	080787	jhc	Prevent adding extra crlf at end of buffer
;				for FMT_OTHER (clipboard) in GetBlockDev.
;       1.02    080989   ac     IFDEFed portions that have changed or are not
;                               needed under windows 3.0 winoldaps.

_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT,ds:nothing

		.xlist
		include ic.inc
		include grabber.inc
		.list

		extrn	IC:byte

		public	GetBlockDev

IFDEF	GRAB_VER_2
		public	PutBlockDev
		public	MarkBlockDev
ENDIF	;GRAB_VER_2



ATTR_TOGGLE	=	01110111b



gbFuncTable	label	word
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
;
;
GetBlockDev	proc	near
		assume	ds:nothing
		cld
;***		    mov     bh,ch
;***		    mov     bl,cs:[IC.icWidthBytes]
		mov	bx,cs:[IC.icWidthBytes]
		call	cs:[gbFuncTable][bp]
		ret

gbFmtNative:
		push	si
		mov	cl,dl
		rep	movsw
		pop	si
		add	si,bx
		dec	dh
		jnz	gbFmtNative
		ret


gbFmtOther:
		mov	ax,cx
		mov	al,GT_TEXT
		stosw
		stosw
		push	di
gbFmtOtherOuter:
		push	si
		mov	cl,dl
		mov	ah,020h
gbFmtOtherInner:
		lodsb
		inc	si
		or	al,al
		jz	gbFixChar
		cmp	al,-1
		je	gbFixChar
		stosb
		loop	gbFmtOtherInner
		jmp	short gbAddCrLf
gbFixChar:
		mov	al,ah
		stosb
		loop	gbFmtOtherInner
gbAddCrLf:
		mov	ax,00A0Dh
		stosw
		pop	si
		add	si,bx
		dec	dh
		jnz	gbFmtOtherOuter
gbNullTerminate:
		sub	di,2			;080787 munge last crlf
		xor	ax,ax
		stosw
		mov	ax,di
		pop	di
		sub	ax,di
		mov	es:[di][-2],ax
		ret
GetBlockDev	endp


IFDEF	GRAB_VER_2

		subttl	PutBlockDev
		page


pbFuncTable	label	word
		dw	pbFillBoth
		dw	pbFillChar
		dw	pbFillAttr
		dw	pbCopyBoth
		dw	pbCopyChar
		dw	pbCopyAttr
		dw	pbCopyCharFillAttr
		dw	pbCopyAttrFillChar


;
; PutBlockDev - inner and outer loops of PutBlock
;
; ENTRY
;	ax	=  grAttr:grChar for fill operations
;	bx	=  undefined
;	cx	=  0
;	dx	=  Yext:Xext
;	bp	=  fScreenOp function request number (pre-adjusted)
;	ds:si	-> seg:offset of user data for copy operations
;	es:di	-> seg:offset of starting position on screen
; EXIT
;	none
; USES
;
;
PutBlockDev	proc	near
		assume	ds:nothing
		cld
;***		    mov     bh,ch
;***		    mov     bl,cs:[IC.icWidthBytes]
		mov	bx,cs:[IC.icWidthBytes]
		add	bp,offset pbFuncTable
pbOuterLoop:
		push	di
		mov	cl,dl
		call	cs:[bp]
		pop	di
		add	di,bx
		dec	dh
		jnz	pbOuterLoop
		ret


;
; PutBlockDev inner loop routines follow
;


pbFillBoth:
		rep	stosw
		ret

pbFillChar:
		stosb
		inc	di
		loop	pbFillChar
		ret

pbFillAttr:
		mov	al,ah
pbFillAttr$:
		inc	di
		stosb
		loop	pbFillAttr$
		ret

pbCopyBoth:
		rep	movsw
		ret

pbCopyChar:
		movsb
		inc	di
		loop	pbCopyChar
		ret

pbCopyAttr:
		inc	di
		movsb
		loop	pbCopyAttr
		ret

pbCopyCharFillAttr:
		lodsb
		stosw
		loop	pbCopyCharFillAttr
		ret

pbCopyAttrFillChar:
		mov	ah,ds:[si]
		stosw
		inc	si
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
;
;
MarkBlockDev	proc	near
		assume	ds:nothing
;***		    mov     bh,ch
;***		    mov     bl,cs:[IC.icWidthBytes]
		mov	bx,cs:[IC.icWidthBytes]
mbOuterLoop:
		push	si
		mov	cl,dl
mbInnerLoop:
		xor	byte ptr ds:[si],ATTR_TOGGLE
		inc	si
		inc	si
		loop	mbInnerLoop

		pop	si
		add	si,bx
		dec	dh
		jnz	mbOuterLoop
		ret
MarkBlockDev	endp

ENDIF	;GRAB_VER_2


_TEXT		ends
		end


