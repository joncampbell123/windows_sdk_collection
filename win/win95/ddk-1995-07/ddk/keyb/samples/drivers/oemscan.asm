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
;*** OEMSCAN.ASM .. contains OemKeyScan() **********************************
;
;	Translate ASCII to scan code and shift state for Windows 386.
;
;	Nonresident LOADONCALL code for this module
;
;	This version uses OemToAnsi() and VkKeyScan() to assist in the
;	translation to a shift state and a Windows virtual keycode. 
;	For letters and most control codes these functions are not called.
;	The table keyTrTab[] is searched to get the scan code.
;
;***************************************************************************

if1
    %out
    %out .. OEMSCAN.ASM, Windows 3.xx version.
endif

include keyboard.inc
include vkwin.inc

externFP OemToAnsi
externFP AnsiToOem
externFP VkKeyScan
externFP MapVirtualKey					;[damianf] for keyTrTab

sBegin DATA

OldChar	db	0

TransBuf label byte
	dw	0

sEnd DATA

createSeg _OEMSC, OEMSC, BYTE, PUBLIC, CODE
sBegin	OEMSC
assumes	CS,OEMSC
assumes	DS,DATA
.386

; This function maps OEM ASCII codes (0..FF) into OEM scan codes and
; shift states.  It is passed the ASCII code as OEMChar, and returns:
;
;	(AX): scan code
;	(DX): shift state
;
;	Table entries:
;
;	First byte, Modifier flags (Shift state) (returned in DX)
;		bit  7 - Insert state active		(Not used here)
;		bit  6 - Caps Lock state active		(Not used here)
;		bit  5 - Num Lock state active		(Not used here)
;		bit  4 - Scroll Lock state active	(Not used here)
;		bit  3 - Alt shift depressed	(Rejected here -- see code)
;		bit  2 - Ctrl shift depressed
;		bit  1 - left shift depressed	USE THIS ONE FOR SHIFTED CHARS
;		bit  0 - right shift depressed	DO NOT USE THIS ONE
;
;	    NOTE: Setting the Shift state to -1 (0FFH) is an indication that
;		there is no scan code translation for this key. This can be
;		used to indicate OEM keys which do not map to a Single scan code
;		(chars built from multi-key sequences such as dead-keys).
;		For entries of this type, the Scan code (second byte) is
;		irrelevant (setting it to -1 too is fine).
;
;	Second byte, Scan Code (returned in AX)
;
;	If the character is not found in the tables, or is flagged
;	as undefined in the indexed (lower) part of the table, -1
;	is returned in AX and DX.
;
cProc OEMKeyScan,<PASCAL, LOADDS, PUBLIC,FAR>,<si,di>

    ParmB OEMChar

cBegin OemKeyScan

	; If character >= 80H, call OemToAnsi() to convert..

	mov	al,OEMChar
;;	xor	ah,ah				; make sure AH = 0
	cmp	al,80H				; Do we need to convert
	jb	OemAnsiSame			;  this to ANSI?

OemConvert:
	mov	TransBuf,al			; if so, store in memory.
	mov	OldChar,al			; and save for comparison
	mov	di,DATAoffset TransBuf
	regptr	dsdi,ds,di
	cCall	OemToAnsi,<dsdi,dsdi>		; Convert to ANSI
	mov	al,TransBuf			; get character from string
	xor	ah,ah				; make sure AH = 0
	; Now we check the translation: first, is it > 7fh, and second,
	; is the reverse translation the original character.
	cmp	al,80h				; is it >= 80h?
	jb	jOemNoKey			; if not, it's probably bad.
	push	ax				; save the ANSI character
	mov	di,DATAoffset TransBuf		; prepare to translate back
	regptr	dsdi,ds,di
	cCall	AnsiToOem,<dsdi,dsdi>
	mov	al,TransBuf			; get new OEM char
	cmp	al,OldChar			; compare with old OEM char
	pop	ax				; get the ANSI char. back
	je	CallVkKeyScan			; same as orig., so we're OK...
jOemNoKey:
	jmp	OemNoKey			; can't translate reliably.

OemAnsiSame:

    ; If this is a letter or certain control codes, we can calculate
    ; the Windows virtual keycode and the shift state directly.

	xor	dx,dx				; Upper-case letter?
	cmp	al,VK_A
	jb	NotCap
	cmp	al,VK_Z
	ja	NotCap
	mov	dl,2				; Yes, set SHIFT
	jmp	short VkLookup

NotCap:
	cmp	al,VK_SPACE			; Space?
	je	VkLookup
	cmp	al,VK_RETURN			; Return?
	je	VkLookup
	cmp	al,VK_TAB			; TAB?
	je	VkLookup
	cmp	al,VK_BACK			; backspace?
	je	VkLookup
	cmp	al,'a'				; lower-case letter?
	jb	NotLower
	cmp	al,'z'
	ja	NotLower
	sub	al,32				; convert lc letter to VK.
	jmp	short VkLookup			; look up scan code (DX==0).

NotLower:
	cmp	al,0ah				; linefeed?
	je	CallVkKeyScan
	cmp	al,1				; control-A..control-Z?
	jb	CallVkKeyScan
	cmp	al,26
	ja	CallVkKeyScan
	add	al,VK_A-1			; yes, compute VK code and
	mov	dl,4				; set CONTROL in shift byte
	jmp	short VkLookup

CallVkKeyScan:

    ; Use VkKeyScan() to get the key combination needed.
    ; If it returns a control-alt shift combination, we reject the
    ; translation, since this is probably different in the MSDOS
    ; driver.

	xor	ah,ah
	cCall	VkKeyScan,<ax>
	cmp	ax,-1
	je	OemNoKey
	test	ah,4				; requires ALT key?  If so,
	jnz	OemNoKey			; this is probably not good!
	mov	dl,ah				; move and convert shift
	xor	ah,ah				; bits.
	mov	dh,ah
	shl	dl,1

VkLookup:	

    ; finally, find scan code for this Windows virtual keycode.
    ; Use MapVirtual Key keyTrTab is now gone. [damianf]

	push	dx
	xor	bx,bx				; VK to SCAN
    	cCall	MapVirtualKey, <ax,bx>
    ; ax contains scan code.
	pop	dx
	jmp	short OemKeyExit

OemNoKey:
	xor	ax,ax				; not found, set -1 return
	dec	ax
	cwd

OemKeyExit:

cEnd OemKeyScan
.286p
sEnd OEMSC

if2
%out .. end OEMSCAN.ASM
%out
endif

END


