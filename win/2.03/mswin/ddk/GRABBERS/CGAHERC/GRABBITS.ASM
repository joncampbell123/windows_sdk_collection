
		page	,132
		%out	GrabBits
		name	GRABBITS
		title	Copyright (C) by Microsoft Inc. 1985-1987


;
; HISTORY
;	1.01	080787	*** *******	Added code to properly interface with routines
;				in BLOCK.ASM in order to support fullscreen
;				MarkBlock and GetBlock on graphics screens.
;
;	1.02	081187	*** *******	Was multiplying a word by a byte in both GetBits
;				and GrabBits -- symptoms of which only showed
;				up when grabbing EGAMONO screens.
;
;	1.10	091987	*** *******	Re-designed GrabBits to use an incremental
;				algorithm for frame buffer access.  Transformed
;				GetBits into ComputeYBufPos.  These changes
;				address previous performance problems.
;


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include ic.inc
		include grabber.inc
		.list

		extrn	IC:byte
		extrn	lpGrabBuffer:dword
		extrn	GrabBufferSize:word

		public	GrabBits
		public	InvertGrph


left		dw	0
right		dw	0
top		dw	0
bottom		dw	0


		subttl	InvertGrph


;
; InvertGrph - inverts the pixels of the current graphics screen
;
; ENTRY
;	none
; EXIT
;	none
; USES
;	cx, si, flags
;
InvertGrph	proc	near
		assume	ds:_TEXT
		push	ds
		mov	cx,[IC.icScrLen]	;# of bytes to flip
		lds	si,[IC.iclpScr]
		assume	ds:nothing
		shr	cx,1
		shr	cx,1
ig0:
		not	word ptr [si]		;invert byte
		inc	si			;address the next
		inc	si
		not	word ptr [si]		;again...take advantage of BIU
		inc	si			;rather than flush pipeline
		inc	si			;with a loop instruction so soon
		loop	ig0

		pop	ds
		ret
InvertGrph	endp


		subttl	ComputeBufPos
		page


;
; ComputeYBufPos - adjust si to point within scanline in frame buffer
;
; ENTRY
;	bx	=  y screen coordinate in pixels (origin is at upper left)
; EXIT
;	si	=  adjusted to point to proper scan line
; USES
;	ax, si, flags
;
ComputeYBufPos	proc	near
		assume	ds:nothing
		push	cx
		push	dx
		mov	ax,bx			;process interlace page
		and	al,[IC.icInterlaceM]	;isolate interlace bits
		mov	cx,0000Dh		;multiply by 2000h to get the
		shl	ax,cl			;  correct interlace base page
		add	si,ax			;add in base

		mov	ax,bx			;process offset from page
		mov	cl,[IC.icInterlaceS]	;divide by interlace factor
		shr	ax,cl			;ax = bx / interlaceshift
		mov	cl,[IC.icWidthBytes]	;note ch is still 0
		mul	cx			;ax = ax * icWidthBytes
		add	si,ax			;add in offset for final addr
		pop	dx
		pop	cx
		ret
ComputeYBufPos	endp


		subttl	GrabBits
		page


;
; GrabBits - take snapshot of graphics screen
;
; ENTRY
;	es:di	-> GrabRequest structure
; EXIT
;	none
; USES
;	all
;
GrabBits	proc	near
		assume	ds:_TEXT

		mov	ax,es:[di]
		or	ax,es:[di][2]
		jnz	gbGo

		mov	al,[IC.icWidthBytes]
		mul	[IC.icPixelsY]
		add	ax,(SIZE GrabSt) - 2
		ret
gbGo:
		cld
		mov	dx,-1
		xor	bx,bx			;y = 0
		mov	[left],dx		;left = top = -1
		mov	[top],dx
		mov	[right],bx		;right = bottom = 0
		mov	[bottom],bx
		lds	si,[IC.iclpScr]
		assume	ds:nothing
gbOuterLoop0:
		xor	cx,cx			;x = 0
		xor	si,si			;start at far left
		call	ComputeYBufPos
gbInnerLoop0:
		lodsw
		cmp	ax,dx			;if completely blank,
		je	gbNextWord		;  keep scanning
gbChkLeft:
		cmp	cx,[left]
		jae	gbChkRight
		mov	[left],cx		;update left border
gbChkRight:
		cmp	cx,[right]
		jbe	gbChkTop
		mov	[right],cx		;update right border
gbChkTop:
		cmp	bx,[top]
		jae	gbChkBottom
		mov	[top],bx		;update top border
gbChkBottom:
		cmp	bx,[bottom]
		jbe	gbNextWord
		mov	[bottom],bx		;update bottom border
gbNextWord:
		inc	cx
		inc	cx			;x = x + 2
		cmp	cl,[IC.icWidthBytes]	;if x-axis not finished,
		jb	gbInnerLoop0		;  go do more

		inc	bx			;y = y + 1
		cmp	bx,[IC.icPixelsY]	;if y-axis not finished,
		jb	gbOuterLoop0		;  go do more

		cmp	[top],-1		;if we found some data,
		jne	gbDoGrab		;  go grab it
		ret				;else get out now

gbDoGrab:
		inc	[bottom]		;fix y for subtraction
		add	[right],2		;fix x for subtraction

		les	di,es:[di][grlpData]	;get lp to grab buffer
		push	di			;save buffer offset
		mov	ax,GT_NEWBITMAP 	;stash gbType
		stosw
		xor	ax,ax			;stash gbSize (currently zero)
		stosw

		mov	ax,[right]		;compute width in pixels
		sub	ax,[left]
		shl	ax,1			;width = # Bytes * 8
		shl	ax,1
		shl	ax,1
		stosw				;stash gbWidth in pixels

		mov	ax,[bottom]
		sub	ax,[top]
		stosw				;stash gbHeight in pixels

		xor	ah,ah
		mov	al,[IC.icPlanes]
		stosw				;stash gbPlanes

		mov	al,1
		stosw				;stash gbPixel (use 1 for now)

		add	di,gbBits - gbWidth2	;point at bitmap area
		mov	bx,[top]		;starting y
		mov	dx,[right]
		sub	dx,[left]		;dx = x-axis byte count
		shr	dx,1			;dx = x-axis word count
gbOuterLoop1:
		mov	cx,dx
		mov	si,[left]		;always start at left side
		call	ComputeYBufPos
		rep	movsw			;copy x-axis to buffer
		inc	bx			;y = y + 1
		cmp	bx,[bottom]		;if y-axis not finished,
		jb	gbOuterLoop1		;  go do more
gbCalcSizes:
		pop	bx			;recover buffer offset
		sub	di,gbWidth		;calculate buffer size in bytes
		mov	es:[bx][gbSize],di	;stash it

		mov	ax,[IC.icSizeX] 	;calc bitmap width in 0.1mm units
		mov	cx,[IC.icPixelsX]
		mul	word ptr es:[bx][gbWidth]
		div	cx			;ax = Xmm * gbWidth/jcMaxPixelsX
		mov	es:[bx][gbWidth2],ax	;stash it

		mov	ax,[IC.icSizeY] 	;calc bitmap height in 0.1mm units
		mov	cx,[IC.icPixelsY]
		mul	word ptr es:[bx][gbHeight]
		div	cx			;ax = Ymm * gbHeight/jcMaxPixelsY
		mov	es:[bx][gbHeigh2],ax	;stash it
		ret
GrabBits	endp


_TEXT		ends
		end

