
		page	,132
		%out	Bin2Hex
		name	BIN2HEX
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	BIN2HEX.ASM
;
; DESCRIPTION
;	This file contains the binary-to-hex conversion routines AX2HEX and
;	AH2HEX, used for words and bytes respectively.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT

		public	AX2Hex
		public	AH2Hex


		subttl	Binary to Hex Converters


;
; AX2Hex - binary to hex converter
;
; ENTRY
;	ax	=  word to convert (AX2Hex)
;	ah	=  byte to convert (AH2Hex)
;
;	ds:si	-> 4-byte output buffer (AX2Hex)
;	ds:si	-> 2-byte output buffer (AH2Hex)
;
;	ds:bx	-> 16-byte lookup table to use for xlat
; EXIT
;	none
; USES
;	ax, flags
;
AX2Hex		proc	near
		assume	ds:nothing
		push	ax			;save byte in ah
		mov	ah,al			;make a copy of byte
		shr	ah,1			;isolate upper nybble
		shr	ah,1
		shr	ah,1
		shr	ah,1
		and	al,00001111b		;isolate lower nybble
		xlat				;xlate	 lower nybble
		xchg	al,ah
		xlat				;xlate	 upper nybble
		mov	ds:[si][2],ax		;stash in lsw of string
		pop	ax			;recover ah

AH2Hex		proc	near
		mov	al,ah			;make a copy of byte
		shr	ah,1			;isolate upper nybble
		shr	ah,1
		shr	ah,1
		shr	ah,1
		and	al,00001111b		;isolate lower nybble
		xlat				;xlate	 lower nybble
		xchg	al,ah
		xlat				;xlate	 upper nybble
		mov	ds:[si],ax		;stash in msw of string
		ret
AH2Hex		endp
AX2Hex		endp


_TEXT		ends
		end

