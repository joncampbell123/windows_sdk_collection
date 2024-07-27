
		page	,132
		%out	In/OutHiLow
		name	INOUT
		title	Copyright (C) by Microsoft Inc. 1985-1987


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		public	InHiLow
		public	OutHiLow


		subttl	InHiLow


;
; InHiLow - read a word from two consecutive indexed data registers
;
; ENTRY
;	al	=  lower register index
;	dx	=  port address
; EXIT
;	al	=  data from register indexed by al
;	ah	=  data from register indexed by al + 1
; USES
;	ax, flags
;
InHiLow 	proc	near
		mov	ah,al
		out	dx,al
		inc	dx
		in	al,dx
		xchg	al,ah
		dec	dx

		inc	al
		out	dx,al
		inc	dx
		in	al,dx
		xchg	al,ah
		dec	dx
		ret
InHiLow 	endp


		subttl	OutHiLow


;
; OutHiLow - sends a word to two consecutive indexed data registers
;
; ENTRY
;	al	=  lower register index
;	bl	=  data for register indexed by al
;	bh	=  data for register indexed by al + 1
;	dx	=  port address
; EXIT
;	none
; USES
;	ax, flags
;
OutHiLow	proc	near
		mov	ah,bl
		out	dx,ax
		inc	ax
		mov	ah,bh
		out	dx,ax
		ret
OutHilow	endp


_TEXT		ends
		end

