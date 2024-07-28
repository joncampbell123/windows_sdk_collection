	page	,132
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GetName.asm
;
; Copyright 1989-1990 by Microsoft Corporation.  All Rights Reserved.
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
;	Bit 20 of lParam is the Extended bit (distingushes some keys on
;	Enhanced keyboard).
;
;	Bit 21 of lParam is a don't care bit (don't distingush between
;	left and right control, shift, Enter keys, between edit keys
;	in edit area and on numeric pad, etc).  The app calling this
;	function sets this bit in lParam, if it so desires.
;
;   lpStr:	Pointer to output string.
;
;   iSize:	Maximum length of output string, not including null byte.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; History:
;
; 18 dec 89 1.54 peterbe	Added ring, nRING, dRING for Iceland ring-
;				-deadkey.
; 15 jul 89	peterbe		Added "don't care" test in lParam.
;				Added additional function key support.
; 13 jun 89	peterbe		removed extra xor after TrySingleMap:
; 19 may 89	peterbe		Added name entry for right CONTROL key.
; 18 may 89	peterbe		Changed some face names, mainly upper/lower
;				case changes.
; 12 mar 89	peterbe		Added code for calling GetKeyString() in
;				DLL. No "don't care" yet.
; 11 mar 89	peterbe		Adding code for string table lookup.
;				Works for USA strings in this module.
;				Need to implement call to DLL's function.
;				Need to update string list, implement
;				"don't care" (bit 21) stuff.
; 10 mar 89	peterbe		Created, up to tables for US IBM keyboard.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include cmacros.inc
include trans.inc

sBegin DATA

externW	<hTables>			; handle for keyboard DLL.

; int FAR PASCAL GetKeyString(int sIndex, LPSTR lpstring, int maxlen)
pGetKeyString	dd	0		; pointer to GetKeyString()

sEnd DATA

externFP <MapVirtualKey>		; in KEYBOARD (TABS.ASM)
externFP <GetProcAddress>		; in Kernel

createSeg _GETNAME, GETNAME, BYTE, PUBLIC, CODE
sBegin	GETNAME        ; Beginning of code segment
assumes	CS,GETNAME
assumes	DS,DATA

    ; for debug
    public StringTab
    public NormalScan, NormalIndex, ExtendScan, ExtendIndex

szGetKeyString db	"GetKeyString", 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Tables for GetKeyNameText()
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro for defining StringTab:

stab macro ?FLAG, l, n, s
    IF ?FLAG EQ 0
	n	= 	temp
	temp	=	temp + 1
	l	db	s	; string
		db	0	; 0-terminate
    ELSE
		dw	GETNAMEoffset	l
    ENDIF
endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; StringTab[]:
;
; Table of English-language key names.  StringTab[] is an array of
; offsets to the strings, indiced by the numeric values nEsc, nBS,
; ... nDELETE, which are consecutive integers, defined by the macros
; here.
;
; This table is to be copied into the keyboard DLL's, and translated
; for non-English keyboards.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	temp	=	0	; init. variable for defining indices.

IRP flag, <0,1>

    if	flag eq 1
	    StringTab	label	word
    endif

; Strings -- normal.

	;	select	label	index		string

	STAB	flag,	lESC,	nEsc,		"Esc"
	STAB	flag,	lBS,	nBS,		"Backspace"
	STAB	flag,	lTAB,	nTab,		"Tab"
	STAB	flag,	lENT,	nEnter,		"Enter"
	STAB	flag,	lCTRL,	nCtrl,		"Ctrl"
	STAB	flag,	lSH,	nShift,		"Shift"
	; left-hand '/' key has other translations on non-USA keyboards.
	; the Num Div key is actually an extended key..
	STAB	flag,	lND,	nNumDiv,	"Num /"
	STAB	flag,	lRSh,	nRShift,	"Right Shift"
	STAB	flag,	lNM,	nNumMult,	"Num *"
	STAB	flag,	lAlt,	nAlt,		"Alt"
	STAB	flag,	lSP,	nSpace,		"Space"
	STAB	flag,	lCaps,	nCapsLock,	"Caps Lock"
	STAB	flag,	lF1,	nF1,		"F1"
	STAB	flag,	lF2,	nF2,		"F2"
	STAB	flag,	lF3,	nF3,		"F3"
	STAB	flag,	lF4,	nF4,		"F4"
	STAB	flag,	lF5,	nF5,		"F5"
	STAB	flag,	lF6	nF6,		"F6"
	STAB	flag,	lF7,	nF7,		"F7"
	STAB	flag,	lF8,	nF8,		"F8"
	STAB	flag,	lF9,	nF9,		"F9"
	STAB	flag,	lF10,	nF10,		"F10"

	STAB	flag,	lPause,	nPause,	        "Pause"
	STAB	flag,	lScr,	nScroll,	"Scroll Lock"
	STAB	flag,	lNum7,	nNum7,		"Num 7"
	STAB	flag,	lNum8,	nNum8,		"Num 8"
	STAB	flag,	lNum9,	nNum9,		"Num 9"
	STAB	flag,	lNumSb,	nNumSub,	"Num -"
	STAB	flag,	lNum4,	nNum4,		"Num 4"
	STAB	flag,	lNum5,	nNum5,		"Num 5"
	STAB	flag,	lNum6,	nNum6,		"Num 6"
	STAB	flag,	lNumPl,	nNumPlus,	"Num +"
	STAB	flag,	lNum1,	nNum1,		"Num 1"
	STAB	flag,	lNum2,	nNum2,		"Num 2"
	STAB	flag,	lNum3,	nNum3,		"Num 3"
	STAB	flag,	lNum0,	nNum0,		"Num 0"
	STAB	flag,	lNumDc,	nNumDec,	"Num Del"
	STAB	flag,	lF11,	nF11,		"F11"
	STAB	flag,	lF12,	nF12,		"F12"

	STAB	flag,	lF13,	nF13,		"F13"	; ICO, NOKIA, others
	STAB	flag,	lF14,	nF14,		"F14"
	STAB	flag,	lF15,	nF15,		"F15"
	STAB	flag,	lF16,	nF16,		"F16"
	STAB	flag,	lF17,	nF17,		"F17"
	STAB	flag,	lF18,	nF18,		"F18"
	STAB	flag,	lF19,	nF19,		"F19"
	STAB	flag,	lF20,	nF20,		"F20"
	STAB	flag,	lF21,	nF21,		"F21"
	STAB	flag,	lF22,	nF22,		"F22"
	STAB	flag,	lF23,	nF23,		"F23"
	STAB	flag,	lF24,	nF24,		"F24"
	STAB	flag,	lhelp,	nhelp,		"Help"	; ICO (extend bit set)
	STAB	flag,	lClear,	nClear,		"Clear"	; ICO (extend bit set)
	STAB	flag,	lBreak,	nBreak,		"Break"	; ICO (extend bit set)
	STAB	flag,	l00,	n00		"<00>"	; ICO (extend bit is
							; set on second 0)

; strings for extended keys.
	STAB	flag,	lPrtS,	nPrtScreen,	"Prnt Scrn"
	STAB	flag,	lNumL,	nNumLock,       "Num Lock"
	STAB	flag,	lNumEn,	nNumEnter,	"Num Enter"
	STAB	flag,	lAltGr,	nAltGr,		"Right Alt"
	STAB	flag,	lHome,	nHome,		"Home"
	STAB	flag,	lUP,	nUP,		"Up"
	STAB	flag,	lPGUP,	nPGUP,		"Page Up"
	STAB	flag,	lLEFT,	nLEFT,		"Left"
	STAB	flag,	lRIGHT,	nRIGHT,		"Right"
	STAB	flag,	lEND,	nEND,		"End"
	STAB	flag,	lDOWN,	nDOWN,		"Down"
	STAB	flag,	lPGDN,	nPGDOWN,	"Page Down"
	STAB	flag,	lINS,	nINSERT,	"Insert"
	STAB	flag,	lDEL,	nDELETE,	"Delete"
	STAB	flag,	lRCtrl,	nRCtrl,		"Right Ctrl"

; Strings for dead keys.
; NOTE: Translate these even if your keyboard doesn't have these dead keys!

	STAB	flag,	dACUTE,	nACUTE,		"Acute"
	STAB	flag,	dGRAVE,	nGRAVE,		"Grave"
	STAB	flag,	dCIRCU,	nCIRCUMFLEX,	"Circumflex"
	STAB	flag,	dUMLAU,	nUMLAUT,	"Umlaut"
	STAB	flag,	dTILDE,	nTILDE,		"Tilde"
	STAB	flag,	dCED,	nCEDILLA,	"Cedilla"
	STAB	flag,	dRING,	nRING		176	; prob. Iceland only

endm	; End of IRP

; The tables up to here are duplicated, with strings translated, in the
; keyboard DLL's.  The string indices 0..nCEDILLA should be identical
; here and in all DLL's.

; The following tables translate a virtual keycode to a string table
; index.  This is then used to get the string offset from StringTab[],
; or from the keyboard DLL in the case of non-USA keyboards.
; The macros used are defined in TRANS.INC.

; This table translates scan codes to string indices for NORMAL keys.
; The macros generate separate byte arrays for the scan codes and the
; translated indices.

; It is OEM-dependent.

irp F, <0,1>

    klabels NormalScan, NormalIndex, F

	ktrans	01h,	nEsc,	F

	ktrans	0eh,	nBS,	F
	ktrans	0fh,	nTAB,	F

	ktrans	1ch,	nEnter,	F
	ktrans	1dh,	nCtrl,	F

	ktrans	2ah,	nShift,	F	; left shift
	ktrans	36h,	nRShift, F	; right shift
	ktrans	37h,	nNumMult, F

	ktrans	38h,	nAlt,	F
	ktrans	39h,	nSpace,	F
	ktrans	3ah,	nCapsLock, F
	ktrans	3bh,	nF1,	F
	ktrans	3ch,	nF2,	F
	ktrans	3dh,	nF3,	F
	ktrans	3eh,	nF4,	F
	ktrans	3fh,	nF5,	F

	ktrans	40h,	nF6,	F
	ktrans	41h,	nF7,	F
	ktrans	42h,	nF8,	F
	ktrans	43h,	nF9,	F
	ktrans	44h,	nF10,	F
	ktrans	45h,	nPause, F
	ktrans	46h,	nScroll, F
	ktrans	47h,	nNum7,	F

	ktrans	48h,	nNum8,	F
	ktrans	49h,	nNum9,	F
	ktrans	4ah,	nNumSub, F
	ktrans	4bh,	nNum4,	F
	ktrans	4ch,	nNum5,	F
	ktrans	4dh,	nNum6,	F
	ktrans	4eh,	nNumPlus, F
	ktrans	4fh,	nNum1,	F

	ktrans	50h,	nNum2,	F
	ktrans	51h,	nNum3,	F
	ktrans	52h,	nNum0,	F
	ktrans	53h,	nNumDec, F

	; extra function keys for IBM enhanced keyboard
	ktrans	57h,	nF11,	F
	ktrans	58h,	nF12,	F

	; ifdef ICO
	ktrans	54h,	n00, F		; double zero key, 1st char.

	ktrans	60h,	nF11,	F
	ktrans	61h,	nF12,	F
	ktrans	62h,	nF13,	F
	ktrans	63h,	nF14,	F
	ktrans	64h,	nF15,	F
	ktrans	65h,	nF16,	F
	ktrans	66h,	nF17,	F
	ktrans	67h,	nF18,	F
	; endif

    klabdef NormalScanEnd, F

endm

; This table translates scan codes to string indices for EXTENDED keys.
; It is OEM-dependent.

irp F, <0,1>

    klabels ExtendScan, ExtendIndex, F
	ktrans	01h,	nPrtScreen,	F
	ktrans	45h,	nNumLock, F
	ktrans	1ch,	nNumEnter, F
	ktrans	1dh,	nRCtrl,	F

	ktrans	35h,	nNumDiv, F	; numpad '/' key

	ktrans	38h,	nAltGr,	F	; right alt key

	ktrans	45h,	nNumLock, F	; numlock key

	ktrans	47h,	nHome,	F	; 7 key

	ktrans	48h,	nUp,	F	; 8 key
	ktrans	49h,	nPgUp,	F	; 9 key
	ktrans	4bh,	nLeft,	F	; 4 key
	ktrans	4dh,	nRight,	F	; 6 key
	ktrans	4fh,	nEnd,	F	; 1 key

	ktrans	50h,	nDown,	F	; 2 key
	ktrans	51h,	nPgDown, F	; 3 key
	ktrans	52h,	nInsert, F	; 0 key
	ktrans	53h,	nDelete, F	; decimal/DEL key

	; ICO keyboard scan codes, keys with extend bit set
	; ifdef ICO
	ktrans	46h,	nBreak, F	; ICO break key
	ktrans	54h,	n00, F		; double zero key, 2nd char.
	ktrans	56h,	nHelp, F	; Help key
	ktrans	5ch,	nClear, F	; Clear key
	; endif

    klabdef ExtendScanEnd, F

endm

; This table translates some right-hand keys into left-hand keys

irp F, <0,1>

    klabels ExtraKey, NormalKey, F

	ktrans	nRshift,	nShift,	F
	ktrans	nRCtrl,		nCtrl, F
	ktrans	nAltGr,		nAlt, F
	ktrans	nNumEnter,	nEnter, F

    klabdef ExtraKeyEnd, F

endm


; This table translates possible ANSI codes for dead keys to string indices.

umlau0		equ	022h
umlau1		equ	0a8h
umlau2		equ	0b7h
acute		equ	0b4h
grave		equ	060h
cedill		equ	0b8h
ring		equ	176

irp F, <0,1>

    klabels DeadScan, DeadIndex, F

	ktrans	acute,	nAcute,		F	; 2 possible acute codes
	ktrans	"'",	nAcute,		F
	ktrans	grave,	nGrave,		F
	ktrans	"^",	nCircumflex,	F
	ktrans	umlau0,	nUmlaut,	F	; 3 possible umlaut/dierisis
	ktrans	umlau1,	nUmlaut,	F	;  codes
	ktrans	umlau2,	nUmlaut,	F
	ktrans	"~",	nTilde,		F
	ktrans	cedill, nCedilla,	F
	ktrans	ring,	nRING,	F

    klabdef DeadScanEnd, F

endm

; Actual code for GetKeyNameText()...

cProc   GetKeyNameText,<FAR,PASCAL,PUBLIC>,<si,di>

    parmW  lParamHi	; lo byte contains scan code, bit 8 means extended
			; key, bit 9 means ignore distinction between
			; left and right keys (e.g. SHIFT).
    parmW  lParamLo	; we don't look at low word of lParam.
    parmD  lpStr	; output string
    parmW  iSize	; max no of bytes in output string excl. of 0-term.

cBegin
	cmp	iSize,1			; don't bother if string's tiny!
	jae	nzstring
zexit:
	mov	ax, 0
	jmp	exit
	
nzstring:
	; First, try using MapVirtualKey() to translate scan -> VK, then
	; VK -> ANSI, for scan codes up to 35H.  35H is the '/' key (USA),
	; which on enhanced keyboard is divide key if enhanced bit is set.  
	; 56H is the 102nd key on non-US enhanced keyboards ('\' on UK
	; keyboard) -- so we check for this too.
	; OEMs like Nokia and Olivetti should add special checks here..

	mov	ax, lParamHi
	cmp	al, 56h			; check for key 102  .. if it is,
	je	TrySingleMap		;  .. use single-char string.
	cmp	al, 35h			; compare to scan code for USA '/' key.
	ja	NoSingleMap		;   if >, can't be single character
	jb	TrySingleMap		;   if <, is single char or control
	test	ah,1			; '/' is special case, extended kbd.
	jnz	NoSingleMap		;  .. single char if not extended.

TrySingleMap:
	xor	ah,ah			; need scan code only.
	mov	bx, 1			; translate scan to VK, if possible.
	cCall	MapVirtualKey,<ax,bx>
	xor	ah,ah
	or	al,al			; Got a good VK?
	jz	NoSingleMap
	mov	bx, 2			; translate VK to ANSI
	cCall	MapVirtualKey,<ax,bx>
	test	ah,80h			; dead key?
	jnz	MapDeadKey
	cmp	al, 32			; if it's 0 or a control char, we want
	jbe	NoSingleMap		; to try for a long name for the key.

SaveSingleCharString:
	les	bx, lpStr		; get string address
	mov	es:[bx], al		; put character in lpStr[0], and
	mov	byte ptr es:[bx+1],0	; null terminate it.
	mov	ax, 1			; 1 byte length.
	jmp	exit

MapDeadKey:
	; Map the ANSI dead key translation in AL to a string index
	; Note that the value we search for is an ANSI code, not a scan code.
	mov	di, GETNAMEoffset DeadScan	; setup for 'Dead' table
	mov	bx, GETNAMEoffset DeadIndex
	mov	cx, (GETNAMEoffset DeadScanEnd)-(GETNAMEoffset DeadScan)
	jmp	short LookupInScanTable

NoSingleMap:

	; The translation for this key is going to be a string from our
	; tables.  First we convert it to a string table index, then
	; we copy the string from the tables for the appropriate
	; language.  This 2-part translation facilitates localization.
	;
	; Get lParam again, and look up in appropriate table
	mov	ax, lParamHi		; AL is scan code, AH(0) is extended
	test	ah, 1			; flag .. is this an extended key?
	jnz	LookupEnhanced

	mov	di, GETNAMEoffset NormalScan	; setup for 'normal' table
	mov	bx, GETNAMEoffset NormalIndex
	mov	cx, (GETNAMEoffset NormalScanEnd)-(GETNAMEoffset NormalScan)
	jmp	short LookupInScanTable

LookupEnhanced:
	mov	di, GETNAMEoffset ExtendScan	; setup for extended table
	mov	bx, GETNAMEoffset ExtendIndex
	mov	cx, (GETNAMEoffset ExtendScanEnd)-(GETNAMEoffset ExtendScan)

LookupInScanTable:
	cld				; search up!
	push	cs
	pop	es			; ES:DI points to scancode table
	push	di			; save start index
	repne scasb			; scan for match
	pop	cx			; get start index back
	jne zexit			; exit if not found.
	sub	di,cx			; OK, compute index
	dec	di
	mov	al, es:[bx+di]		; look up string table index.

	; The string index has been found (AL).  Check the "don't care" bit
	; and perform any appropriate translation. We map, for example,
	; a right-hand SHIFT key into a right-hand SHIFT key.

	test	ah, 2			; test don't care bit
	jz	DoCare
					
	mov	di, GETNAMEoffset ExtraKey	; look up in ExtraKey table
	mov	bx, GETNAMEoffset NormalKey	; and trans. to this.
	mov	cx, (GETNAMEoffset ExtraKeyEnd)-(GETNAMEoffset ExtraKey)
	push	di			; save start index
	repne scasb			; scan for match
	pop	cx			; get start index back
	jne	DoCare			; if 
	sub	di,cx			; OK, compute index
	dec	di
	mov	al, es:[bx+di]		; look up index for left-hand key

DoCare:
	; Now, we determine if the DLL is loaded and if the GetKeyString()
	; function exists in it.

	cmp	hTables, 32			; IS there a DLL loaded?
	jb	GetLocalString
	push	ax				; save string index
	mov	ax, GETNAMEoffset szGetKeyString
	cCall	GetProcAddress, <hTables, cs, ax>
	mov	word ptr pGetKeyString, ax	; save ptr. offset
	or	ax, dx				; is pointer NZ?
	pop	ax				; (restore string index)
	jz	GetLocalString		 	; no, do locally
	mov	word ptr [pGetKeyString + 2], dx ; OK, save
	les	di, lpStr			; ES:DI -> output string
	regptr	esdi, es, di
	mov	bx, iSize
	xor	ah,ah				; clear AH
	cCall	[pGetKeyString], <ax, esdi, bx> ; call the DLL's function
	jmp	short exit			; we're done.

	; A DLL is not loaded, or one is and it doesn't have a GetKeyString()
	; function.  So look the string up in our local table and copy it
	; to the caller's space.  First, look up the address of the string
	; in StringTab[].

GetLocalString:

	xor	ah,ah			; change [AL] to word index..
	add	ax,ax
	mov	bx,ax
	mov	si, cs:[StringTab + bx]	; get offset of string from table.
	push	ds			; save DS,
	push	cs			; make DS = CS
	pop	ds			; DS:SI -> string
	les	di, lpStr		; ES:DI -> destination string
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

exit:

cEnd

sEnd  GETNAME

        end
