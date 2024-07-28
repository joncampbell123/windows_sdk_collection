	page	,132
;*** XLAT.ASM -- ***********************************************************
;                                                                          *
; Copyright (C) 1985-1990 by Microsoft Corporation. All Rights Reserved.   *
;                                                                          *
;***************************************************************************

.xlist
include	cmacros.inc
.list

	TITLE	XLAT - translate OemToAnsi and AnsiToOem

;***************************************************************************
;
;   History Windows 3.00
;
;	21 feb 90	peterbe		Return values from AnsiToOem(),
;					OemToAnsi() are now -1 (0ffffH).
;	15 feb 90	peterbe		See IF MULTISEG: if this is != 0,
;					we do multisegment translations, else
;					we just fix up DI and stop when hitting
;					segment limit.
;	14 feb 90	peterbe		AnsiToOem() and OemToAnsi() now check
;					for segment wrap, in real and protect
;					mode.  The src and dst strings can now
;					overlap segment limits and be of any
;					length.
;	05 dec 89	peterbe		AnsiToOem() and OemToAnsi() always
;					return 1 (TRUE) in AX.
;					AnsiToOemBuff() etc. are still void.
;	05 jul 89	peterbe		Implemented AnsiToOemBuff() and
;					OemToAnsiBuff().
;					Eliminated lstrsetup(), lstrfinish()
;	30 jun 89	peterbe		Added skeletons for 
;					OemToAnsiBuff().
;	29 apr 89	peterbe		Added support for extended ANSI
;					-- actually simplifies code --
;					in an ifdef.
;	27 feb 89	peterbe		Added CodePage
;	11 aug 88	peterbe		Add page directive at beginning.
;	26 jul 88	peterbe		Change includes and comments.
;					Minor pointer changes
;
;***************************************************************************
;
; Note:  Because of the limited number of segment registers, the tables
; are in the code segment.  DS:SI and ES:DI point to the input and output
; strings, respectively.  The same parameters and local variables are
; used in AnsiToOem() and OemToAnsi() at the same stack-frame offsets,
; so common subroutines are called to access these variables.
;
;		Lasciate ogni speranza, voi ch' entrata!
;		(Leave behind every hope, you who enter!)
;
;			-- Dante
;
;***************************************************************************

; to facilitate debugging:
;; public aoIncrSrc, aoIncrDst, oaIncrSrc, oaIncrDst, IncrSrc, IncrDst

; this flag determines what happens when we reach the end of a segment
; in AsciToOem() or OemToAscii().

ifndef MULTISEG
  MULTISEG	equ	1		; default to multiseg translation.
endif

if1
    %out
    %out  XLAT.ASM
    if MULTISEG
	%out .. multisegment strings handled.
    else
	%out .. multisegment strings NOT handled.
    endif
endif


;***************************************************************************
;
; This module is used for all countries.  The tables are in the
; include files XLATUS.INC, XLATNO.INC, XLATPO.INC, etc.
;
; The goal of this module is to translate strings from Ansi to Oem
; character set or the opposite. If there is no equivalent character
; we use the followings rules:
;
;  1) we put a similar character (e.g. character without accent)
;  2) In OemToAnsi, graphics vertical, horizontal, and junction characters
;     are usually translated to '|', '-', and '+' characters, as appropriate,
;     unless the ANSI set is expanded to include such graphics.
;  3) Otherwise we put underscore "_".
;
;***************************************************************************

ExternA	<__WinFlags>			; this tells us whether we're in
					; protect mode.
ExternA	<__AHINCR>			; how much to increment a segreg to
					; get next segment or selector

sBegin	CODE

;***************************************************************************
;
; Include the ANSI/OEM translation tables.
; Normally, this is the Code Page 437 table.  For Norway, Denmark, Portugal,
; French Canada, and other code pages different from 437 these tables are
; overlaid during enabling of the drivers.

	public	CodePage		; (word) (Code page number)
	public	AnsiToOemTable		; tables in XLATxx.INC
	public	CtrlOemToAnsi
	public	OemToAnsiTable

include xlatus.inc			; code page 437

sEnd

sBegin CODE

assumes	CS,CODE
assumes	DS,NOTHING

;***************************************************************************
;
; GetKBCodePage() -- return code page of oem/ansi translation tables.
;
cProc	GetKBCodePage,<PUBLIC,FAR>
cBegin	nogen

	mov	ax, CodePage
	ret
cEnd nogen


;***************************************************************************
;
; The translation tables are now in the CODE segment = CS.  This is
; because DS and ES point to the input and output strings, so CS has
; to point to the conversion table's segment (for the XLAT instruction).
; The offsets in BX for AnsiToOemTable and OemToAnsiTable point to 160
; or 128 bytes below the beginnings of the actual tables -- this is
; because these tables only contain translations starting at 160 and
; 128, respectively.
;
; If an extended ANSI set is used, the code for AnsiToOem() is
; identical to the code for OemToAnsi(), except for the table address.
;
; (the input value is adjusted for control characters in OemToAnsi).
;
; On entry, DS:SI and ES:DI point to the source and destination strings.
; The XLAT instruction is forced by the ASSUMES, and by the dummy reference
; to 'Dummy', to have a CS override.
;

assumes	CS,CODE
assumes	DS,NOTHING

;***************************************************************************
;
; AnsiToOem( pSrc, pDst ) - Translates the string at pSrc into
; the string at pDst.  pSrc == pDst is legal.
;
; Always returns 1 (TRUE) in AX.

Dummy label byte

cProc	AnsiToOem,<PUBLIC,FAR>,<si,di>
	ParmD   pSrc
	ParmD   pDst

	localW	srcLimit		; 1 + top of current source segment
	localW	dstLimit		; 1 + top of current destination segment

cBegin	AnsiToOem

	cld
	lds	si,pSrc			; get pointer to source string
	les	di,pDst			; get pointer to dest. string
	call	GetLimits		; establish seg limits

ifdef EXTENDEDANSI

	mov	bx,offset AnsiToOemTable-128	; get table offset
	jmp	short oa1		; in this case, code is same as
					; OemToAnsi() except for translation
					; tables used.

else ; (not) EXTENDEDANSI

	; This code is used for the standard ANSI character set (not-ASCII
	; characters >= 160 only).
	;
	mov	bx,offset AnsiToOemTable-160	; get table offset
	jmp	short ao1

	; in the limit checks, we jump only in the rare case that we passed the
	; end of a segment on the last increment of SI or DI, so this is
	; ususally pretty quick.

ao0:					; check limits
	cmp	si, srcLimit
	je	aoIncrSrc		; past limit, update DS:SI
ao0a:
	cmp	di, dstLimit
	je	aoIncrDst		; past limit, update ES:DI
ao0b:
	
ao1:	
	lodsb				; read byte from string
	cmp	al,160			; 160 = first difference of ...
	jae	aoXLAT			; ...a printable character

ao2:	
	stosb				; write byte to string
	or	al,al
	jnz	ao0
	jmp	short AnsiOemExit	; exit

aoXLAT:					; have to translate
	xlat	Dummy			; actually CS:AnsiToOemTable
	jmp	ao2

if MULTISEG

; Increment segment register to next selector, and clear index register.

aoIncrSrc:
	call	short IncrSrc
	jmp	ao0a

aoIncrDst:
	call	short IncrDst
	jmp	ao0b

else	; not MULTISEG

; if this code is enabled, we just stuff a null and die if we reach the
; end of either the src or dst segment.  We always check to see if DI
; has wrapped.

aoIncrSrc:
aoIncrDst:
	cmp	di, dstlimit		; is DI past limit?
	jne	aoStuffZero		; if so,
	dec	di			; back up to stuff 0

    aoStuffZero:
	xor	al,al
	stosb				; null-terminate where we died.
	;jmp	AnsiOemExit		; now we exit from AnsiToOem()..

endif	; not MULTISEG

AnsiOemExit:				; normal return
	mov	ax, -1			; return FFFF

endif ; EXTENDEDANSI

cEnd	AnsiToOem

;***************************************************************************
;
; OemToAnsi( pSrc, pDst ) - Translates the string at pSrc into
; the string at pDst.  pSrc == pDst is legal.
;
; Always returns 1 (TRUE) in AX.
;
cProc	OemToAnsi,<PUBLIC,FAR>,<si,di>
	ParmD   pSrc
	ParmD   pDst

	localW	srcLimit		; 1 + top of current source segment
	localW	dstLimit		; 1 + top of current destination segment

cBegin	OemToAnsi

	cld
	lds	si,pSrc			; get pointer to source string
	les	di,pDst			; get pointer to dest. string
	call	short GetLimits		; establish seg limits

	mov	bx,offset OemToAnsiTable-128	; get table offset

	jmp	short oa1		; we assume we're in bounds 1st time

	; in the limit checks, we jump only in the rare case that we passed the
	; end of a segment on the last increment of SI or DI.

oa0:					; check limits
	cmp	si, srcLimit
	je	oaIncrSrc		; past limit, update DS:SI
oa0a:
	cmp	di, dstLimit
	je	oaIncrDst		; past limit, update ES:DI
oa0b:

oa1:
	lodsb				; get next char from OEM string.
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
	jnz	oa0			; .. if not, get another
	jmp	short OemAnsiExit

oaControl:
	add	al,96			; control, adjust index to table.
oaXLAT:
	xlat	Dummy			; actually CS:OemToAnsiTable
	jmp	oa2			; go store character

; code for handling segment wrap (real) or reaching seg. limit (protect)
; get next selector and clear index register.

if MULTISEG

oaIncrSrc:
	call	short IncrSrc
	jmp	oa0a

oaIncrDst:
	call	short IncrDst
	jmp	oa0b

else	; not MULTISEG

; if this code is enabled, we just stuff a null and die.

oaIncrSrc:
oaIncrDst:
	cmp	di, dstlimit		; is DI past limit?
	jne	oaStuffZero		; if so,
	dec	di			; back up to stuff 0

    oaStuffZero:
	xor	al,al
	stosb				; null-terminate where we died.
	jmp	OemAnsiExit

endif	; not MULTISEG

;***************************************************************************
; Special code for handling segment limits.

; GetLimits(): initialize segment limits for OemToAnsi() and AnsiToOem().
; Call this with DS and ES pointing to the current source and destination
; segments already.  SS:BP points to C stack frame for AnsiToOem() or
; OemToAnsi().  After executing this, srcLimit and dstLimit will indicate
; the upper bounds of each segment (actually, 1 + upper bound).

GetLimits proc near

if 0
; Don't do this for 3.0.  Breaks under 386 pmode where the limit of the
; first selector is the entire global alloced block. > WORD.
	mov	ax, __WinFlags		; get the kernel flags
	test	al, 1
	jz	GLReal			;  are we in protect mode now?

.286p					; Protect-mode instructions here!

	mov	bx, ds			; get upper bound of source segment
	xor	ax, ax			; clear in case lsl fails
	lsl	ax, bx			; get segment limit
	inc	ax			; increment to next byte
	mov	srcLimit, ax

	mov	bx, es			; get upper bound of destination segment
	xor	ax, ax
	lsl	ax, bx
	inc	ax
	mov	dstLimit, ax

.8086					; 8086 instructions only
	ret

GLReal:
endif
	xor	ax, ax			; real-mode limit
	mov	srcLimit, ax
	mov	dstLimit, ax
	ret

GetLimits endp

; IncrSrc moves DS:SI to the start of the next segment.

if MULTISEG

IncrSrc proc near

	push	ax			; real ..
	push	bx
	mov	ax, ds
	add	ax, __AHINCR		; move DS up 64K or to next selector
	mov	ds, ax
	xor	si, si			; clear SI
if 0
	mov	ax, __WINFLAGS
	test	al, 1
	jz	IncrSrcReal		; keep limit 0 in real mode
					; protect mode:
	mov	bx, ds			; get upper bound of source segment
	xor	ax, ax			; clear in case lsl fails
.286p
	lsl	ax, bx			; get segment limit
.8086
	inc	ax			; increment to next byte
	mov	srcLimit, ax

IncrSrcReal:
endif

	pop	bx
	pop	ax
	ret

IncrSrc endp

; IncrDst moves ES:DI to the start of the next segment.

IncrDst proc near

	push	ax			; real ..
	push	bx
	mov	ax, es
	add	ax, __AHINCR		; move ES up 64K or to next selector
	mov	es, ax
	xor	di, di			; clear DI
if 0
	mov	ax, __WINFLAGS
	test	al, 1
	jz	IncrDstReal
					; protect mode:
	mov	bx, es			; get upper bound of destination segment
	xor	ax, ax
.286p
	lsl	ax, bx
.8086
	inc	ax
	mov	dstLimit, ax

IncrDstReal:
endif

	pop	bx
	pop	ax
	ret

IncrDst endp

endif	; MULTISEG

;***************************************************************************

; finally, the exit from OemToAnsi():

OemAnsiExit:
	mov	ax, -1			; return FFFF

cEnd	OemToAnsi

;***************************************************************************
;
; Translate from Oem to Ansi or Ansi to Oem, with count.

cProc	AnsiToOemBuff,<PUBLIC,FAR>,<si,di>
	ParmD   pSrc
	ParmD   pDst
	ParmW   nLength
cBegin	AnsiToOemBuff

	cld
	lds	si,pSrc			; get pointer to source string
	les	di,pDst			; get pointer to dest. string
	mov	cx,nLength		; get byte count,
	jcxz	AnsiOemBuffExit		; do nothing if it's 0!

ifdef EXTENDEDANSI

	mov	bx,offset AnsiToOemTable-128	; get table offset
	jmp	short oab1		; in this case, code is same as
					; OemToAnsi() except for translation
					; tables used.

else ; (not) EXTENDEDANSI

	; This code is used for the standard ANSI character set (not-ASCII
	; characters >= 160 only).
	;
	mov	bx,offset AnsiToOemTable-160	; get table offset

aob1:	lodsb				; read byte from string
	cmp	al,160			; 160 = first difference of ...
	jae	aobXLAT			; ...a printable character
aob2:	stosb				; write byte to string
	loop	aob1			; check count, loop or
	jmp	short AnsiOemBuffExit	;   exit

aobXLAT:					; have to translate
	xlat	Dummy			; actually CS:AnsiToOemTable
	jmp	aob2

endif ; EXTENDEDANSI

AnsiOemBuffExit:

cEnd	AnsiToOemBuff

;***************************************************************************
;
cProc	OemToAnsiBuff,<PUBLIC,FAR>,<si,di>
	ParmD   pSrc
	ParmD   pDst
	ParmW   nLength
cBegin	OemToAnsiBuff

	cld
	lds	si,pSrc			; get pointer to source string
	les	di,pDst			; get pointer to dest. string
	mov	cx,nLength		; get byte count,
	jcxz	OemAnsiBuffExit		; do nothing if it's 0!

	mov	bx,offset OemToAnsiTable-128	; get table offset

oab1:	lodsb				; get next char from OEM string.
					; (handle control chars)
	cmp	al,128			; above ASCII range?
	jae	oabXLAT
	or	al,al			; NUL->NUL
	jz	oab2
	cmp	al,32			; it's ASCII, is it a control char?
	jb	oabControl
oab2:
	stosb				; store character.
	loop	oab1			; check count, loop or
	jmp	short OemAnsiBuffExit	;  exit..

oabControl:
	add	al,96			; control, adjust index to table.
oabXLAT:
	xlat	Dummy			; actually CS:OemToAnsiTable
	jmp	oab2			; go store character

OemAnsiBuffExit:

cEnd	OemToAnsiBuff

if2
%out end XLAT.ASM
%out
endif

sEnd	CODE

END
