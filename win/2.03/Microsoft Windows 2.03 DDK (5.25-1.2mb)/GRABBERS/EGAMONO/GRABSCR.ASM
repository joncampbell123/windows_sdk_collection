
		page	,132
		%out	GrabScreen
		name	GRABSCR
		title	Copyright (C) by Microsoft Inc. 1985-1987


;
; HISTORY
;	1.01	091587	*** *******	Added delay loop between screen inversions.
;				Screen was flashing too fast to let user see
;				that a grab occured.
;


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include grabber.inc
		.list

		extrn	IC:byte
		extrn	GetMode:near
		extrn	GetBlock:near
		extrn	MarkBlock:near

		public	lpGrabBuffer
		public	GrabBufferSize
		public	GrabScreen


lpGrabBuffer	dd	?
GrabBufferSize	dw	?
GR		GrabRequest	<0,0,0,0,0,FMT_OTHER,0,0>


		subttl	GrabScreen


;
; GrabScreen - take full screen snapshot
;
;	GrabScreen is called by the OEM event handler when the grab screen
;	trigger occurs.  Since this event is usually a keysequence, it is
;	highly asynchronous, and the state of DOS is unknown.  Thus, we may
;	NOT call DOS, for instance, to save the screen to a metafile on
;	disk.
;
;	The "snapshot" appearance is simulated via a full-screen
;	MarkBlock/GetBlock/MarkBlock request sequence to our block routines.
;	Note that since the display state is unknown on entry, we must first
;	do a GetMode in order to initialize the IC structure used by these
;	block routines.  Also note that all regs must be saved!  We are called
;	by an OEM ISR that may only save what it uses.
;
; ENTRY
;	none
; EXIT
;	none
; USES
;	none
;
GrabScreen	proc	near
		push	ax
		push	bx
		push	cx
		push	dx
		push	si
		push	di
		push	bp
		push	ds
		push	es

		call	GetMode 			;get display state

		mov	ax,cs
		mov	ds,ax
		mov	es,ax
		assume	ds:_TEXT			;ds = es = cs

		xor	ax,ax				;set grlpData = NULL
		mov	word ptr [GR.grlpData],ax
		mov	word ptr [GR.grlpData][2],ax
		mov	di,offset GR			;es:di -> GR
		call	GetBlock			;get size needed for grab
		jc	gsX

		cmp	ax,[GrabBufferSize]		;if size needed > size allocated,
		ja	gsX				;  leave now

		mov	ax,word ptr [lpGrabBuffer]	;else grlpData -> our buffer
		mov	word ptr [GR.grlpData],ax
		mov	ax,word ptr [lpGrabBuffer][2]
		mov	word ptr [GR.grlpData][2],ax

		call	MarkBlock			;invert full screen
		call	GetBlock			;grab	full screen

		xor	cx,cx				;give user time to see
		loop	$				;  the screen inverted

		call	MarkBlock			;invert full screen
gsX:
		pop	es
		pop	ds
		pop	bp
		pop	di
		pop	si
		pop	dx
		pop	cx
		pop	bx
		pop	ax
		ret
GrabScreen	endp


_TEXT		ends
		end

