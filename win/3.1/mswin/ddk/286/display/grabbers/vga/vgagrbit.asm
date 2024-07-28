
		page	,132
		%out	GrabBits
		name	GRABBITS
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	GRABBITS.ASM
;
; DESCRIPTION
;	This file contains the bulk of the code used to handle block
;	operations on graphics screens.
;
; HISTORY
;	1.00	060187	jhc	Epoch
;	1.01	080787	jhc	Added code to properly interface with routines
;				in BLOCK.ASM in order to support fullscreen
;				MarkBlock and GetBlock on graphics screens.
;
;	1.02	081187	jhc	Was multiplying a word by a byte in both GetBits
;				and GrabBits -- symptoms of which only showed
;				up when grabbing EGAMONO screens.
;
;	1.10	091987	jhc	Re-designed GrabBits to use an incremental
;				algorithm for frame buffer access.  Transformed
;				GetBits into ComputeYBufPos.  These changes
;				address previous performance problems.
;
;	1.11	060688	vvr	changed code to reflect widthbytes > 256
; 				mode 13 has 320 pixels with 8 per pixel
;
;	1.12	072789	jem	IFDEF unused functions for Windows 3.0
;
;	Copyright (c) 1989 Microsoft.  All Rights Reserved.


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include vgaic.inc
		include grabber.inc
		.list

		extrn	IC:byte
		extrn	lpGrabBuffer:dword
		extrn	GrabBufferSize:word

		public	GrabBits


left		dw	0			;holds coords of bounding box
right		dw	0
top		dw	0
bottom		dw	0


IFDEF	GRAB_VER_2

		subttl	InvertGrph


;
; InvertGrph - inverts the pixels of the current graphics screen
;
;	The name InvertGrph could be aliased with the phrase "A full-screen
;	MarkBlock on a graphics screen." That is in fact what triggers a call
;	to this routine.
;
; ENTRY
;	none
; EXIT
;	none
; USES
;	cx, si, flags
;
		public	InvertGrph

InvertGrph	proc	near


		assume	ds:_TEXT
		push	ds
		mov	cx,[IC.icScrLen]	;# of bytes to flip
		lds	si,[IC.iclpScr] 	;get ptr to screen
		assume	ds:nothing
		shr	cx,1			;divide cx by 2 for word ops
		shr	cx,1			;divide again for double loop
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

ENDIF	;GRAB_VER_2

		subttl	ComputeBufPos
		page


;
; ComputeYBufPos - compute pointer into screen buffer from y coordinate
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
		mov	ax,bx			;compute interlace bank
		and	al,[IC.icInterlaceM]	;isolate interlace bits
		mov	cx,0000Dh		;multiply by 2000h to get the
		shl	ax,cl			;  correct interlace bank
		add	si,ax			;and add it in

		mov	ax,bx			;process offset from bank
		mov	cl,[IC.icInterlaceS]	;divide by interlace factor
		shr	ax,cl			;ax = bx / interlaceshift
		mov	cx,[IC.icWidthBytes]	;note ch is still 0  ~vvr
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
;	The name GrabBits could be aliased with the phrase "A full-screen
;	GetBlock with format OTHER on a graphics screen."  That is in fact
;	what triggers a call to this routine.
;
;	One feature of this function is that the snapshot of the graphics
;	image will be "trimmed down" to the least bounding box that will
;	contain the image.  In other words, a snapshot of a completely black
;	screen would return nothing.
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

		mov	ax,[IC.icWidthBytes]		;~vvr
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
		cmp	cx,[IC.icWidthBytes]	;if x-axis not finished, ~vvr
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
