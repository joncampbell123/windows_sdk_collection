;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

	page	,132
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GetName.asm
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   Contains GetKeyNameText() function.
;
;   int far PASCAL GetKeyNameText(DWORD lParam, LPSTR lpStr, WORD size);
;
;   lParam: value from WM_KEYDOWN message, etc.
;
;	Byte 3 (bits 16..23) of lParam contains a scan code.
;
;	Bit 24 of lParam is the Extended bit (distingushes some keys on
;	Enhanced keyboard).
;
;	Bit 25 of lParam is a don't care bit (don't distingush between
;	left and right control, shift, Enter keys, between edit keys
;	in edit area and on numeric pad, etc).  The app calling this
;	function sets this bit in lParam, if it so desires.
;
;   lpStr:	Pointer to output string.
;
;   iSize:	Maximum length of output string, not including null byte.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DOS5=1			; No real mode prologs
.NOLIST			;cmacros.inc
include cmacros.inc
.list

NAMELENGTH	equ	30			;30 chars for keyname !
						;since localised [damianf]

sBegin DATA

externW		HlibKbd				;Handle to keyboard [damianf]
externD		lpfnLoadString			;address of LoadString
externW		ScanCodes			;mapvirtualkey
externA		KBD_SCANS			;mapvirtualkey


ExtKeysBegin	label	byte			;Extended keys ah,1
  dw	0101h	;PrtScreen
  dw	0145h	;NumLock  						
  dw	011ch	;NumEnter 
  dw	011dh	;RCtrl 	
  dw	0135h	;NumDiv  	
  dw	0138h	;AltGr 	
  dw	0147h	;Home 	
  dw	0148h	;Up 		
  dw	0149h	;PgUp 	
  dw	014bh	;Left 	
  dw	014dh	;Right 	
  dw	014fh	;End 	
  dw	0150h	;Down 	
  dw	0151h	;PgDown  	
  dw	0152h	;Insert  	
  dw	0153h	;Delete  	
  dw    015bh   ;nexus
  dw    015ch   ;nexus
  dw    015dh   ;nexus
SizeExtKeys equ ($ - ExtKeysBegin) shr 1

;Dead Keys are ANSI Values

DeadKeyNames	label	byte
	db	0b4h		;acute
	db	092h		;acute using apostrophe
	db	060h		;grave
	db	05eh		;circumflex
	db	022h		;umlaut using double quote
	db	0a8h		;umlaut
	db	0b7h		;dot
	db	098h		;tilde
	db	0b8h		;cedilla
	db	0b0h		;ring
	db	07eh		;"Tilde"
	db	088h		;"Circumflex"
	db	0a1h		;"Hachek"
	db	0a2h		;"Breve"
	db	0bdh		;"DoubleAcute"
	db	0b2h		;"Ogonek"

SizeDeadkeyNames equ $ - DeadKeyNames
sEnd DATA

externFP <MapVirtualKey>		; in KEYBOARD (TABS.ASM)

createSeg _GETNAME, GETNAME, BYTE, PUBLIC, CODE
sBegin	GETNAME        ; Beginning of code segment
assumes	CS,GETNAME
assumes	DS,DATA

.386
;*****************************************************************************
;****************************** GetKeyNameText *******************************
;*****************************************************************************

cProc   GetKeyNameText,<LOADDS, FAR,PASCAL,PUBLIC>,<si,di>

	parmW  	lParamHi	; lo byte contains scan code, 
				; bit 8 means extended key, 
				; bit 9 means ignore distinction between
				; left and right keys (e.g. SHIFT).
	parmW  	lParamLo	; we don't look at low word of lParam.
	parmD  	lpStr		; output string
	parmW	iSize		; max no of bytes in output excl. of 0-term.

cBegin
	cmp	iSize,1			; don't bother if string's tiny!
	jae	nzstring
zexit:
	xor	ax, ax
	jmp	exit
	
nzstring:
;
; First, try using MapVirtualKey() to translate scan -> VK, then VK -> ANSI, 
; for scan codes up to 35H.  35H is the '/' key (USA),  which on enhanced 
; keyboard is divide key if enhanced bit is set. 56H is the 102nd key on 
; non-US enhanced keyboards ('\' on UK keyboard) -- so we check for this too.
; OEMs like Nokia and Olivetti should add special checks here..
;
	and	lParamHi, 003ffH	; mask out bits we dont want
	mov	ax, lParamHi
	cmp	al, 56h			; check for key 102  .. if it is,
	je	TrySingleMap		;  .. use single-char string.
	cmp	al, 35h			; compare to scan code for USA '/' key.
	ja	NoSingleMap		;   if >, can't be single character
	jb	TrySingleMap		;   if <, is single char or control
	test	ah,1			; '/' is special case, extended kbd.
	jnz	NoSingleMap		;  .. single char if not extended.
;
; MapVirtualKey() accepts type 0 = VkKey to ScanCode
;           		       1 = ScanCode to VkKey
;   returns: AL = Code byte    2 = VkKey to ANSI char
;   AH = 80H if deadkey
;
TrySingleMap:
	xor	ah,ah			; need scan code only.
	mov	bx, 1			; translate scan to VK, if possible.
	cCall	MapVirtualKey,<ax,bx>
	xor	ah,ah			; ignore state info
;;**;;	mov	si, ax			; save VKEY value.
	or	al,al			; Got a good VK?
	jz	NoSingleMap1		; Try for longer name

	mov	bx, 2			; translate VK to ANSI
	cCall	MapVirtualKey,<ax,bx>
	test	ah,80h			; dead key?
	jnz	MapDeadKey		; yes, go fixup state 
	cmp	al, 32			; if it's 0 or a control char, we want
	jbe	NoSingleMap1		; to try for a long name for the key.
;
; Vkey and upper case ANSI have the same values for A-Z and 0-9, but
; we use the ANSI code to make sure all keys return ANSI code.
;
SaveSingleCharString:
;;**;;	mov	ax, si			; get back vkey
	les	bx, lpStr		; get string address
	mov	es:[bx], al		; put character in lpStr[0], and
	mov	byte ptr es:[bx+1],0	; null terminate it.
	mov	ax, 1			; 1 byte length.
	jmp	exit

;----------------------------- Fix for Deadkeys ----------------------------

MapDeadKey:
	;
        ; Map the ANSI dead key translation in AL to a string index
	; Note that the value we search for is an ANSI code, not a scan code.
	;
	; do this to ensure we recognise the deadkey.  If not then return
	; it as a single character.
	;
	mov	ah, 3			; extended key, ignore side
	jmp	short GKNTFound		; go and try to load it.

;------------------------------ Left/Right distinction --------------------
;
; If we care about left/right distinction then knock down bit-25 and go get
; the name. If we don't care then clear bit 25 and conditionally clear bit-24
; if Right-Ctrl or Righr-Alt. Only right-shift requires a scancode change.
;
NoSingleMap1:
	mov	ax, lParamHi		; get back the scan code

NoSingleMap:
	test	ah, 2			; ignore distinction?
	jz	DifferenceOK		; no, it matters
	cmp	al, 036H		; right shift ....
	jne	@f			; no, try the others
	mov	al, 02aH		; ... becomes left shift
;
;  If niether Ctrl nor Alt then do not touch bit 24, so that 
;  non-extended keys are not changed to extended ones. 
;
@@:
	test	ah, 1			; left anyway?
	jz	DifferenceOK		; yes, go get name
	cmp	al, 01dH		; right control ....
	sete	dl			; 0/1 = no/yes
	cmp	al, 038H		; right alt ....
	sete	dh			; 0/1 = no/yes
	xor	dl, dh			; if either present
	xor	ah, dl			; knock down bit 24.
DifferenceOK:
	and	ah, 1			; mask out side differences

;-------------------------- Name from Table -----------------------------

GKNTFound:
;
; Try and get the name from the resource.  If we fail, there was no match
; If we found it then we have a name for the string. Notice that only when
; a deadkey exists is the ignore distinction bit left = 1. 
;
	push	HlibKbd
	push	ax
	push	lpStr
	push	iSize
	Call	dword ptr lpfnLoadString	;Call User LoadString
						;Address is filled
.286p						;ax = no. copied !
exit:

cEnd

sEnd  GETNAME
end
