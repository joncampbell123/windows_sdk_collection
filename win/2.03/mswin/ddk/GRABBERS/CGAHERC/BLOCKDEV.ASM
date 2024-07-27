
		page	,132
		%out	Get/Put/MarkBlockDev
		name	BLOCKDEV
		title	Copyright (C) by Microsoft Inc. 1985-1987


;
; HISTORY
;	1.01	080787	*** *******	Prevent adding extra crlf at end of buffer
;				for FMT_OTHER (clipboard) in GetBlockDev.
;
;	1.02	091787	*** *******	Added code/macros to prevent snow when reading
;				or writing to CGA.
;


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT,ds:nothing


		.xlist
		include ic.inc
		include dc.inc
		include grabber.inc
		include cgaherc.inc
		.list

		ifdef	MULTIMODE
		extrn	Rmn8ToPc8:byte
		endif

		extrn	IC:byte
		extrn	DC:byte

		public	GetBlockDev
		public	PutBlockDev
		public	MarkBlockDev



		ifdef	CGA
mWaitHRetrace	macro
		local	whrSyncHi, whrSyncLow
whrSyncHi:
		in	al,dx
		test	al,IS_H_RETRACE
		jnz	whrSyncHi
whrSyncLow:
		in	al,dx
		test	al,IS_H_RETRACE
		jz	whrSyncLow
		endm
		endif



ATTR_TOGGLE	=	01110111b

wWidthBytes	dw	?

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
		mov	bh,ch
		mov	bl,cs:[IC.icWidthBytes]
		mov	cs:[wWidthBytes],bx

		ifdef	MULTIMODE
		mov	bx,offset Rmn8ToPc8
		endif

		call	cs:[gbFuncTable][bp]
		ret

gbFmtNative:
		push	si
		mov	cl,dl

		ifdef	CGA
		push	dx
		mov	dx,INPUT_STATUS
gbCgaLoop:
		mWaitHRetrace
		movsw
		loop	gbCgaLoop
		pop	dx

		else

		rep	movsw
		endif

		pop	si
		add	si,cs:[wWidthBytes]
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

		ifdef	CGA
		push	dx
		mov	dx,INPUT_STATUS
		endif
gbFmtOtherInner:
		ifdef	CGA
		mWaitHRetrace
		endif

		lodsb
		call	XlatChar
		stosb
		inc	si
		loop	gbFmtOtherInner
gbAddCrLf:
		ifdef	CGA
		pop	dx
		endif

		mov	ax,00A0Dh
		stosw
		pop	si
		add	si,cs:[wWidthBytes]
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



XlatChar	proc	near
		ifdef	MULTIMODE
		test	cs:[DC.dcExModeCtl],00000010b
		jz	xcHpDone
		xlat	cs:[Rmn8ToPc8]
xcHpDone:
		endif

		or	al,al
		jz	xcFixup
		cmp	al,0FFh
		je	xcFixup
		ret
xcFixup:
		mov	al,020h
		ret
XlatChar	endp


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
		mov	bh,ch
		mov	bl,cs:[IC.icWidthBytes]
		add	bp,offset pbFuncTable
pbOuterLoop:
		push	di
		mov	cl,dl

		ifdef	CGA
		push	dx
		mov	dx,INPUT_STATUS
		endif

		call	cs:[bp]

		ifdef	CGA
		pop	dx
		endif

		pop	di
		add	di,bx
		dec	dh
		jnz	pbOuterLoop
		ret


;
; PutBlockDev inner loop routines follow
;


pbFillBoth:
		ifdef	CGA
		push	ax
		mWaitHRetrace
		pop	ax
		stosw
		loop	pbFillBoth

		else

		rep	stosw
		endif
		ret

pbFillChar:
		ifdef	CGA
		push	ax
		mWaitHRetrace
		pop	ax
		endif

		stosb
		inc	di
		loop	pbFillChar
		ret

pbFillAttr:
		mov	al,ah
pbFillAttr$:
		inc	di

		ifdef	CGA
		mWaitHRetrace
		mov	al,ah
		endif

		stosb
		loop	pbFillAttr$
		ret

pbCopyBoth:
		ifdef	CGA
		mWaitHRetrace
		movsw
		loop	pbCopyBoth

		else

		rep	movsw
		endif
		ret

pbCopyChar:
		ifdef	CGA
		mWaitHRetrace
		endif

		movsb
		inc	di
		loop	pbCopyChar
		ret

pbCopyAttr:
		inc	di

		ifdef	CGA
		mWaitHRetrace
		endif

		movsb
		loop	pbCopyAttr
		ret

pbCopyCharFillAttr:
		lodsb

		ifdef	CGA
		push	ax
		mWaitHRetrace
		pop	ax
		endif

		stosw
		loop	pbCopyCharFillAttr
		ret

pbCopyAttrFillChar:
		mov	ah,ds:[si]

		ifdef	CGA
		push	ax
		mWaitHRetrace
		pop	ax
		endif

		stosw
		inc	si
		loop	pbCopyAttrFillChar
		ret
PutBlockDev	endp


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
		mov	bh,ch
		mov	bl,cs:[IC.icWidthBytes]
mbOuterLoop:
		push	si
		mov	cl,dl

		ifdef	CGA
		push	dx
		mov	dx,INPUT_STATUS
		endif
mbInnerLoop:
		ifdef	CGA
		mWaitHRetrace
		endif

		xor	byte ptr ds:[si],ATTR_TOGGLE
		inc	si
		inc	si
		loop	mbInnerLoop

		ifdef	CGA
		pop	dx
		endif

		pop	si
		add	si,bx
		dec	dh
		jnz	mbOuterLoop
		ret
MarkBlockDev	endp


_TEXT		ends
		end

