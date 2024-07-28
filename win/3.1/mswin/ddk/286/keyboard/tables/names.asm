	page	,132
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NAMES.asm
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copyright (C) 1989-1990 Microsoft Corporation.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This contains the GetKeyString function for the keyboard DLL.
;
; int FAR PASCAL GetKeyString(int nString, LPSTR lpStringOut);
;
;	nString		index to list of strings
;
;	lpStringOut	selected string is copied to this address
;
;	The size of the string (exclusive of NULL termination) is
;	returned in AX.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include cmacros.inc
include trans.inc

sBegin DATA

sEnd DATA

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro for defining StringTab:

stab macro ?FLAG, l, n, s
    IF ?FLAG EQ 0
	n	= 	temp
	temp	=	temp + 1
	l	db	s	; string
		db	0	; 0-terminate
    ELSE
		dw	NAMESoffset	l
    ENDIF
endm

createSeg _NAMES, NAMES, BYTE, PUBLIC, CODE
sBegin	NAMES        ; Beginning of code segment
assumes	CS,NAMES
assumes	DS,DATA

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
include names.inc			; one of NAMES??.INC
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

cProc GetKeyString,<FAR,PASCAL,PUBLIC>,<si,di>

    parmW nString			; index to StringTab[]
    parmD lpStringOut			; copy string to this address
    parmW iSize				; max. size

cBegin

	mov	ax,nString		; get string index.
	xor	ah,ah			; change [AL] to word index..
	add	ax,ax
	mov	bx,ax
	mov	si, cs:[StringTab + bx]	; get offset of string from table.
	push	ds			; save DS,
	push	cs			; make DS = CS
	pop	ds			; DS:SI -> string
	les	di, lpStringOut		; ES:DI -> destination string
	xor	bx,bx			; clear byte count
	mov	cx,iSize		; get max string size
CopyLoop:
	lodsb				; get a byte
	stosb				; store it
	or	al,al			; was it null byte?
	jz	CopyDone		; if so, we're done .. almost ..
	inc	bx			; if not, increment byte count.
	loop	CopyLoop		; check max loop count & loop back.
	xor	al,al			; if we fall through,
	dec	di			;  back up, and
	stosb				;   store a 0 byte

CopyDone:
	mov	ax,bx			; return this byte count
	pop	ds			; restore DS

cEnd


sEnd NAMES

end
