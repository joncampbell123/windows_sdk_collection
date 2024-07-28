
		page	,132
		%out	In/OutHiLow
		name	INOUT
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	INOUT.ASM
;
; DESCRIPTION
;	This file contains some i/o helper routines that aid in reading
;	and writing to indexed chips such as the CRT controllers in most
;	display adapters.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;


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
		mov	ah,al			;save index
		out	dx,al			;and send it out
		inc	dx			;bump to data port
		in	al,dx			;get data
		xchg	al,ah			;save it in ah and recover index
		dec	dx			;drop back to address port

		inc	al			;bump index up
		out	dx,al			;and send it out
		inc	dx			;bump to data port
		in	al,dx			;get data
		xchg	al,ah			;and arrange properly in ax
		dec	dx			;recover original dx
		ret
InHiLow 	endp


		subttl	OutHiLow


;
; OutHiLow - sends a word to two consecutive indexed data registers
;
; CAUTION
;	This routine uses the "out dx,ax" instruction which fails on some
;	machines that are not totally IBM compatible.  It may also fail on
;	very slow 8-bit I/O devices that do not assert wait states.  HP
;	Vectras and supported I/O devices have neither of these problems.
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
		mov	ah,bl			;ah = lower data byte
		out	dx,ax			;al out to [dx], ah to [dx+1]
		inc	ax			;bump index
		mov	ah,bh			;ah = upper data byte
		out	dx,ax			;do it again
		ret
OutHilow	endp


_TEXT		ends
		end

