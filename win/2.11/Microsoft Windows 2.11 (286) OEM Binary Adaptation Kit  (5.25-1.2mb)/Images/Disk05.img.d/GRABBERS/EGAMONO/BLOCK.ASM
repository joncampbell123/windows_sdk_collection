
		page	,132
		%out	Get/Put/MarkBlock
		name	BLOCK
		title	Copyright (C) by Microsoft Inc. 1985-1987


;
; HISTORY
;	1.01	080787	*** *******	Added code to support fullscreen MarkBlock
;				and GetBlock on graphics screens.
;
;	1.02	081887	*** *******	Removed an overlooked int 3
;
;	1.03	091787	*** *******	Buffer size calculation at gbCalcOther was 2
;				bytes too long after removal of extra crlf
;				in the BlockDev modules.
;


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include ic.inc
		include grabber.inc
		.list

		extrn	IC:byte
		extrn	GetBlockDev:near
		extrn	PutBlockDev:near
		extrn	MarkBlockDev:near
		extrn	InvertGrph:near
		extrn	GrabBits:near

		public	GetBlock
		public	PutBlock
		public	MarkBlock


		subttl	GetBlock


;
; GetBlock - mark a block of screen data
;
; ENTRY
;	es:di	-> valid GrabRequest structure
;	ds	=  cs
; EXIT
;	none
; USES
;	ax, flags
;
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

		test	[IC.icScrType],ST_GRPH
		jnz	gbGrph

		xor	ax,ax
		mov	cx,ax
		cmp	dl,al
		jne	gbXok
		mov	dl,[IC.icCharsX]
		mov	bl,al
gbXok:
		cmp	dh,al
		jne	gbYok
		mov	dh,[IC.icCharsY]
		mov	bh,al
gbYok:
		mov	al,dl
		mul	dh			;ax = xy
		mov	bp,word ptr es:[di][grStyle]
		and	bp,FMT_OTHER
		jz	gbCalcNative
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
		mov	al,[IC.icCharsX]	;get X (big X)
		mul	bh			;ax = y*X + x
		add	al,bl
		adc	ah,0
		shl	ax,1			;ax = ptr to scr data

		lds	si,[IC.iclpScr]
		assume	ds:nothing
		add	si,ax			;regen addr = ax + crtstart
		xor	cx,cx

		call	GetBlockDev

		pop	ax			;recover size
gbExitGood:
		clc
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
		stc
		jmp	short gbExit
GetBlock	endp


		subttl	PutBlock
		page


;
; PutBlock - put a block of data on screen
;
; ENTRY
;	es:di	-> valid GrabRequest structure
;	ds	=  cs
; EXIT
;	none
; USES
;	ax, flags
;
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

		test	[IC.icScrType],ST_GRPH
		jnz	pbGrph

		xor	ax,ax
		mov	cx,ax
		cmp	dl,al
		jne	pbXok
		mov	dl,[IC.icCharsX]
		mov	bl,al
pbXok:
		cmp	dh,al
		jne	pbYok
		mov	dh,[IC.icCharsY]
		mov	bh,al
pbYok:
		mov	al,[IC.icCharsX]	;current # of columns
		mul	bh			;ax = (row*width + col)
		add	al,bl
		adc	ah,0
		shl	ax,1
		mov	cx,ax			;save offset for now

		mov	bp,word ptr es:[di][grStyle]
		test	bp,FMT_OTHER
		jnz	pbFixBp
		mov	bp,C_BOTH
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

		call	PutBlockDev
pbExitGood:
		clc
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
		stc
		jmp	short pbExit
PutBlock	endp


		subttl	MarkBlock
		page


;
; MarkBlock - mark a block of screen data
;
; ENTRY
;	es:di	-> valid GrabRequest structure
;	ds	=  cs
; EXIT
;	none
; USES
;	ax, flags
;
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

		test	[IC.icScrType],ST_GRPH
		jnz	mbGrph

		xor	ax,ax
		mov	cx,ax
		cmp	dl,al
		jne	mbXok
		mov	dl,[IC.icCharsX]
		mov	bl,al
mbXok:
		cmp	dh,al
		jne	mbYok
		mov	dh,[IC.icCharsY]
		mov	bh,al
mbYok:
		mov	al,[IC.icCharsX]	;current # of columns
		mul	bh			;ax = (row*width + col)
		add	al,bl
		adc	ah,0
		shl	ax,1
		inc	ax
		lds	si,[IC.iclpScr]
		assume	ds:nothing
		add	si,ax			;regen addr = ax + crtstart

		call	MarkBlockDev
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
		or	dx,dx
		jnz	mbErrUs

		test	[IC.icScrType],ST_LARGE
		jnz	mbErrUS

		call	InvertGrph
		jmp	short mbExitGood
mbErrUs:
		mov	ax,ERR_UNSUPPORTED
		stc
		jmp	short mbExit
MarkBlock	endp


_TEXT		ends
		end

