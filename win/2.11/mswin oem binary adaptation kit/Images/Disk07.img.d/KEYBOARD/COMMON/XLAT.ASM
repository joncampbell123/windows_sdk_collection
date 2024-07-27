;*** XLAT.ASM -- Olivetti M24 international version ************************
;                                                                          *
;   Copyright (C) 1985 by Microsoft Corporation                            *
;                                                                          *
;***************************************************************************

;   Olivetti History Windows 2.0 vesion
if1
%out
%out  XLAT.ASM  (international)   28 oct 87
endif
;	28 oct 87	plb	Changed 158 to 160 in AnsiToOem() after ao0:
;	26 oct 87	plb	Code moved back to code segment, like
;				original Microsoft code.
;	15 oct 87	plb	xlatXX.inc files are included in here
;				directly now. IFDEFS are now added to
;				select which file is included.
;
; Windows 1.0x versions: .............................................
;	15 jul 85	plb	Modified OemToAnsi so ALL characters
;				are copied, whether translated or not.
;
;	26 jul 85	plb	OemToAnsi translates a few 'control' codes.
;	29 jul 85	plb	OemToAnsi fix finalized.
;	 9 sep 85	plb	Copied to international\USA directory.
;	10 oct 85	plb	Tables moved to DATA module.
;				Put XLAT instruction and load of table
;				address in DATA segment.
;	11 oct 85	plb	Reorganization.  Put -160, -128 back in
;				DATA.ASM def. of OOemToAnsi, etc. Put
;				main body of code in DATA segment.
;	 5 dec 85	plb	AnsiToOemTable allowed to have 2 bytes
;				before label .. for characters 158,159.
;				Changed made in DoAnsiToOem...
;	16 dec 85	plb	Changed Firstinit ref. to AsciiTabBase
;	12 jun 86	plb	Order of lstrsetup() parameters changed.
;
; Note to those who wish to work on this code: you really have to
; understand how the segment registers are being used.
;
;	Lasciate ogni speranza, voi ch' entrata!
;
;			-- Dante

.xlist
include	cmacros.inc
.list

	TITLE	XLAT - translate OemToAnsi and AnsiToOem

; This module is used for all countries.  The tables are in the
; include files XLATUS.INC, XLATNO.INC, XLATPO.INC, etc., which
; are included into DATA.ASM, etc.

; The goal of this module is to translate strings from Ansi to Oem
; character set or the opposite. If there is no equivalent character
; we use the followings rules:
;  1) we put a similar character (e.g. character without accent)
;  2) if rule 1 is not applicable we put underscore"_".

sBegin	CODE

	extrn	AsciiTabBase:byte	; dummy

; include the translation tables for this particular part of the world.

ifdef NOxlat
include xlatno.inc
else
ifdef POxlat
include xlatpo.inc
else
ifdef CAxlat
include xlatca.inc
else
ifdef SPxlat
include xlatsp.inc
else
include xlatus.inc
endif
endif
endif
endif

; Pointers to XLAT tables
OAnsiToOem	dw	codeOffset AnsiToOemTable-160
OOemToAnsi	dw	codeOffset OemToAnsiTable-128

	page
; The translation tables are now in the CODE segment.  This is
; because DS and ES point to the input and output strings, so CS has
; to point to the conversion table's segment (for the XLAT instruction)
; OAnsiToOem and OOemToAnsi point to 160 and 128 bytes below the starts
; of the actual tables -- this is because only the latter part of each
; of these tables is used. (the input value is adjusted for control
; characters in OemToAnsi).

; On entry, DS:SI and ES:DI point to the source and destination strings.
; The XLAT instruction is forced by the ASSUMES, and by the dummy reference
; to AsciiTabBase, to have a CS override.

assumes	CS,CODE
assumes	DS,NOTHING

;
; AnsiToOem( pSrc, pDst ) - Translates the string at pSrc into
; the string at pDst.  pSrc == pDst is legal.
;
;  On Return AX = 0 if some of the translations were not exact
;

cProc	AnsiToOem,<PUBLIC,FAR>
;       ParmD   pSrc
;       ParmD   pDst
cBegin	nogen

	call	lstrsetup		; Setup pointers,save registers
	mov	bx,OAnsiToOem		; get table offset in DATA segment

ao1:	lodsb				; read byte from string
	cmp	al,160			; 160 = first difference of ...
	jae	aoXLAT			; ...a printable character
ao2:	stosb				; write byte to string
	or	al,al
	jnz	ao1
	jmp	lstrfinish		; exit

aoXLAT:					; have to translate
	xlat	AsciiTabBase		; actually CS:AnsiToOemTable
	jmp	ao2

cEnd	nogen

;
; OemToAnsi( pSrc, pDst ) - Translates the string at pSrc into
; the string at pDst.  pSrc == pDst is legal.
;
;  On Return AX = 0 if some of the translations were not exact
;
cProc	OemToAnsi,<PUBLIC,FAR>
;       ParmD   pSrc
;       ParmD   pDst
cBegin	nogen

	call	lstrsetup
	mov	bx,cs:OOemToAnsi	; get conversion table offset

oa1:	lodsb				; get next char from OEM string.
					; (handle control chars)
	cmp	al,128			; above ASCII range?
	jae	oaXLAT
	or	al,al			; NUL->NUL
	jz	oa2
	cmp	al,32			; it's ASCII, is it a control char?
	jb	oaControl
oa2:
	stosb				; store character.
	or	al,al			; NULL character?
	jnz	oa1			; .. if not, get another
	jmp	lstrfinish

oaControl:
	add	al,96			; control, adjust index to table.
oaXLAT:
	xlat	AsciiTabBase		; actually CS:OemToAnsiTable
	jmp	oa2			; go store character

cEnd	nogen
	page
; Entry and exit routines.

lstrsetup proc near

	pop	dx		; SAVE return address offset in DX
	mov	bx,sp

	push	ds
	push	si
	push	di
				; order changed, 12 June 1986
	lds	si,ss:[bx+8]	; get first C pointer parameter
	les	di,ss:[bx+4]	; get second C pointer parameter

	cld			; reset direction for LODSB,STOSB
	mov	ah,-1		; set flag
	jmp	dx		; return to proc above.

lstrsetup endp

lstrfinish  proc    far

	cwd			; set ax = to hi bit of ah=Flag
	mov	ax,dx

	pop	di		; restore registers
	pop	si
	pop	ds
	ret	8		; far return, flushing parameters

lstrfinish  endp

if2
%out end XLAT.ASM
%out
endif

sEnd	CODE		; End of code segment
END

