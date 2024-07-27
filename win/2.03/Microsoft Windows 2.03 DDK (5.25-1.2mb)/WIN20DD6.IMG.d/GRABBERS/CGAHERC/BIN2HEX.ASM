
		page	,132
		%out	Bin2Hex
		name	BIN2HEX
		title	Copyright (C) by Microsoft Inc. 1985-1987


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
		push	ax
		mov	ah,al
		shr	ah,1
		shr	ah,1
		shr	ah,1
		shr	ah,1
		and	al,00001111b
		xlat
		xchg	al,ah
		xlat
		mov	ds:[si][2],ax
		pop	ax

AH2Hex		proc	near
		mov	al,ah
		shr	ah,1
		shr	ah,1
		shr	ah,1
		shr	ah,1
		and	al,00001111b
		xlat
		xchg	al,ah
		xlat
		mov	ds:[si],ax
		ret
AH2Hex		endp
AX2Hex		endp


_TEXT		ends
		end

