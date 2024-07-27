page	,132
;***************************************************************************
;									   *
;		Copyright (C) 1984-1986 by Microsoft Inc.		   *
;									   *
;***************************************************************************

Title	Binary Drawing Modes
%out	Drawmode



;	History:
;
;	    3-30-85	Since the raster op is performed on all pixels
;			of a line, including the holes in a styled line
;			when opaque mode is set, the drawing modes cannot
;			assume that BH contains the pen (color).   The
;			draw modes PDna and PDno were the only two changed.



	.xlist
	include cmacros.inc
	.list


sBegin	code
assumes cs,code


	public	DrawModeTbl		;Drawmode address table
	public	DrawModeLen		;Drawmode lengths




;	Output And Pixel Drawing Mode Logic Sequences
;
;	The logical templates for the requested function follow.  The
;	code will be copied into the created DDA and executed there.
;
;	The code will also be called from the PIXEL routine to perform
;	the nessacary drawing mode for that function.
;
;	The logical sequences assume the following on entry:
;
;	Entry:	al = current pen
;		ah = destination



DDx:					;D = 0
	xor	al,al
DDxEnd:
	ret


DPna:					;D = D AND (NOT P)
	not	al
	and	al,ah
DPnaEnd:
	ret



Pn:					;D = NOT P
	not	al
PnEnd:
	ret



PDna:					;D = (NOT D) AND P
	not	ah
	and	al,ah
	not	ah
PDnaEnd:
	ret



D:					;D = D	 (I hope nobody ever uses this)
	mov	al,ah
DEnd:
	ret



Dn:					;D = NOT D
	mov	al,ah
	not	al
DnEnd:
	ret



DPx:					;D = D XOR P
	xor	al,ah
DPxEnd:
	ret



DPxn:					;D = NOT (D XOR P)
	xor	al,ah
	not	al
DPxnEnd:
	ret



DPa:					;D = D AND P
	and	al,ah
DPaEnd:
	ret



DPan:					;D = NOT (D AND P)
	and	al,ah
	not	al
DPanEnd:
	ret



DPno:					;D = (NOT P) OR D
	not	al
	or	al,ah
DPnoEnd:
	ret



PDno:					;D = (NOT D) OR P
	not	ah
	or	al,ah
	not	ah
PDnoEnd:
	ret



DPo:					;D = D OR P
	or	al,ah
DPoEnd:
	ret



DPon:					;D = NOT (D OR P)
	or	al,ah
	not	al
DPonEnd:
	ret



DDxn:					;D = 1
	mov	al,0FFH
DDxnEnd:
	ret



P:					;D = P action routine
PEnd:
	ret



;	The drawing mode table contains the starting address of the code
;	template for each drawing mode.

DrawModeTbl	label	word

	dw	DDx			;D = 0
	dw	DPon			;D = NOT (D OR P)
	dw	DPna			;D = D AND (NOT P)
	dw	Pn			;D = NOT P
	dw	PDna			;D = (NOT D) AND P
	dw	Dn			;D = NOT D
	dw	DPx			;D = D XOR P
	dw	DPan			;D = NOT (D AND P)
	dw	DPa			;D = D AND P
	dw	DPxn			;D = NOT (D XOR P)
	dw	D			;D = D
	dw	DPno			;D = (NOT P) OR D
	dw	P			;D = P
	dw	PDno			;D = (NOT D) OR P
	dw	DPo			;D = D OR P
	dw	DDxn			;D = 1




;	The drawing mode length table contains the length of the code
;	for each drawing mode.	The length is needed for computing how
;	many bytes of code is to be moved into the created line drawing
;	code.

DrawModeLen	label	byte

	db	DDxEnd	- DDx		;D = 0
	db	DPonEnd - DPon		;D = NOT (D OR P)
	db	DPnaEnd - DPna		;D = D AND (NOT P)
	db	PnEnd	- Pn		;D = NOT P
	db	PDnaEnd - PDna		;D = (NOT D) AND P
	db	DnEnd	- Dn		;D = NOT D
	db	DPxEnd	- DPx		;D = D XOR P
	db	DPanEnd - DPan		;D = NOT (D AND P)
	db	DPaEnd	- DPa		;D = D AND P
	db	DPxnEnd - DPxn		;D = NOT (D XOR P)
	db	DEnd	- D		;D = D
	db	DPnoEnd - DPno		;D = (NOT P) OR D
	db	PEnd	- P		;D = P
	db	PDnoEnd - PDno		;D = (NOT D) OR P
	db	DPoEnd	- DPo		;D = D OR P
	db	DDxnEnd - DDxn		;D = 1


sEnd	code
end
