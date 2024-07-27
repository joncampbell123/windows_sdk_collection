
		page	,132
		%out	RealizeColor
		name	RCOLOR
		title	Copyright (C) by Microsoft Inc. 1985-1987


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include grabber.inc
		.list


		extrn	PhysColorTable:byte
		public	RealizeColor
		subttl	RealizeColor


;
; RealizeColor - maps logical color to physical color
;
; ENTRY
;	bh	=  logical color id to be mapped
;	ds	=  cs
; EXIT
;	bh	=  mapped physical color
; USES
;	ax, bx, flags
;
RealizeColor	proc	near
		assume	ds:_TEXT
		cmp	bh,MAX_LC_COLOR
		ja	rcErr

		mov	al,bh
		mov	bx,offset PhysColorTable
		xlat
		mov	bh,al
		clc
		ret
rcErr:
		xor	bx,bx
		stc
		ret
RealizeColor	endp


_TEXT		ends
		end

