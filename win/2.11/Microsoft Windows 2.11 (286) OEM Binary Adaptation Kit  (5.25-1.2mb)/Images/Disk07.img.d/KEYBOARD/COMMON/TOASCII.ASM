;****** TOASCII.ASM (International) (Olivetti) *****************************
;                                                                          *
;   Copyright (C) 1983,1984,1985,1986,1987,1988 by Microsoft Inc.          *
;                                                                          *
;***************************************************************************
;	History
;	** Windows 2.xx **
;	(from now on, put latest comments FIRST)
; 
; Microsoft 1.10 releases
;
;	15 jun 88	peterbe	Changed VkKeyScan values for control-alt etc.
;			Moved includes to top of file, before any ifdefs.
;	14 jun 88	peterbe	Changed return value of VkKeyScan to int.
;	08 jun 88	peterbe	Put ifdef VKFUN around VkKeyScan()
;	02 jun 88	peterbe	Put VkKeyScan() in a LOADONCALL segment.
;	01 jun 88	peterbe	Added VkKeyScan() function.
;
;
; Olivetti 1.09 release
;	05 jan 88	plb	'shift_state' variable deleted.
;				In ToAsc3 loop, return -1 if table entry
;				for control is -1.
; Olivetti 1.08 release
;	27 oct 87	plb	Added 'ifdef CEDILLA' in deadkey code.
;				Added 'ifdef NODEAD' around deadkey code.
;				Code after T1NoCtrl for calculating shift
;				bitmask in DH had to be moved.
;	22 oct 87	AP	Added call to SetEriLights to take care of
;				9140 in EPC. (Timer does bad job!)
;	15 oct 87	plb	Handle cedilla deadkey with special code.
;				ICO 00 key only handled if KeyType is 2.
;				Removed VK_OEM_ALT code.
;	23 sep 87	plb	ShiftLockTab moved here -- this and code
;				requires SG asm flag. (Swiss German only!)
;	17 sep 87	plb	Adding new processing for ICO 00 key.
;				Now FORCE '0' if scan code is 54h and virt.
;				code is VK_0.
;				'datestring' removed.
;	31 aug 87	plb	Removed VK_ICO_00 processing.  Now translate
;				00 as 2 VK_0's in DATACOM.ASM.
;	29 jun 87	plb	Removed Alt/Shift-Alt processing.
;
;	History after Olivetti release 1.03/1.03:
;	 3 mar 87	plb	adding handling for VK_OEM_SHIFTLOCK
;				1) Changed DoLights to set/rest fCaps
;				   according to SHIFTLOCK state
;	 			2) Changed code around ToAsc3c to translate
;				   SHIFTLOCK based on 80h bit in KeyState.
;	 4 mar 87	plb	Adding code to translate new ShiftLockTab 
;				table for Swiss German keyboard cap. umlaut.
;	 9 mar 87	plb	Changed code after ToAsc2b to handle CapsLock
;				instead of ShiftLock specially for SwissGerman.
;	11 mar 87	updated datestring
;
;	Microsoft TOASCII.ASM (1.01 Windows):
;	4-26-85		MS	Microsoft version
;				(various updates up to Oli 1.03 rel 1.03)

include	keyboard.inc
include	vkwin.inc
include	vkoem.inc

; Added 30 sep 85:
include olikbd.inc
;

if1
%out
%out   ToAscii.asm  (Microsoft)
endif

	TITLE	TOASCII	- default ASCII	International translation routine

; Added features upon UK routine:

; 1) Ctrl-Alt, ctrl-shift-alt allows to get some ASCII codes ([{]}\|@#^)
; 2) Dead key function (Accents) (Augmented by Olivetti)
; 3) Caps Lock for special cases (keys 0 to 9)
; 4) Special Olivetti handling of SHIFTLOCK for RT 101/102 keyboards.
;
; Every country Xlation routine sharing these kind of features (and no more)
; uses the same assembly code. These countries are the following:
;  Denmark, Finland, France, Germany, Norway, Spain, Sweden, Ireland.

; **** TOASCOM.INC (Common TOASCII code for most of Europe) ***********
;
;	This code was originally in the TOASCOM.INC include file.
;	Since the code is table-driven, by data in the DATA module,
;	different versions are no longer necessary.
;

sBegin	DATA	    ; Beginning of data segment

; some flags in the keyboard table:

	extrn	KeyType:byte	; keyboard type
	extrn	IsOli:byte	; set if Olivetti system
	extrn	IsEri:byte	; set if Ericsson system
ifdef	ERICSSON
	extrn	PCType:byte
endif

; EXTRN declarations for offsets of tables  at beginning of translation
; table file.

; Virtual code to ASCII translation table
	EXTRN   AsciiTabBase:word
	EXTRN   VirtualDefault:word

; Table for ALT and SHIFT-ALT -- removed for Windows 2.0, 29 jun 87

; Table for CTRL-ALT and CTRL-SHIFT-ALT
	EXTRN   CtrlAltBase:word
	EXTRN   CtrlAltDefault:word

; Table to build accented characters with dead keys, and dead-key codes.

	EXTRN   DeadKeyBase:word
	EXTRN   DeadKeyDefault:word
	extrn	MortoBase:word			; offset of TavoloMorto

; Table for characters (other than A..Z) affected by Caps Lock:

	EXTRN   CapitalTabBase:word
	EXTRN   CapitalDefault:word


; Flag byte for RT keyboard (Oli. 3 mar 87)

	EXTRN	RTFlags:byte

; scan codes for special keys, etc.

S_ICO_00	equ	54h

ShiftState  EQU	417H	; rom bios data area of shift states

fCaps		equ	01000000b
fNum		equ	00100000b
fScroll		equ	00010000b

KeyPadDigits	db	7,8,9,-1	; 71 .. 74
		db	4,5,6,-1	; 75 .. 78
		db	1,2,3		; 79 .. 81
		db	0		; 82

fAnsi		db	0	; determines whether ALT-NUMPAD code is
				; OEM or ANSI; set if initial digit is 0.

ifndef NODEAD
dead_key	db	0
endif

ifdef SG
; Table for keys which have different translation for shiftlock than for shift
;  Swiss German driver ONLY.
;  (accessed only if kbSLTable bit of RTFlags is set).
;  (defined only if SG is defined in assembly)
;  This has only 2 bytes per entry.

ShiftLockTab label byte
db	VK_OEM_5,	0c4h	; A umlaut
db	VK_OEM_7,	0d6h	; O umlaut
db	VK_OEM_1,	0dch	; U umlaut 
ShiftLockDefLab label byte
	db	-1, -1
endif ; SG

sEnd	DATA	    ; End of data segment

sBegin	CODE	    ; Beginning of code segment

	externFP	OemToAnsi


; Routine to make Olivetti M24 lights correspond to keyboard flags state:
	extrn	SetOliLights:near	; in PRGKBD.ASM
ifdef	ERICSSON
	extrn	SetEriLights:near
endif


assumes	CS,CODE
assumes	DS,DATA
;
; Procedure to provide the default ASCII translation for an MS-WIN
; virtual key code.
;
cProc	ToAscii,<PUBLIC,FAR>,<si,di>
	ParmW	VirtKey			; MS virtual keycode
	ParmW	Scancode		; 'hardware' raw scancode
	ParmD	pKeyState		; vector of key state flags
	ParmD	pState			; (output) vector of data words
	ParmW	KeyFlags		; flags menu display
cBegin	ToAscii
	mov	ax,VirtKey		; get the virtual key
	mov	dx,ax			; save (in DL or DX) for later
	mov	bx,Scancode		; get the (raw) scancode
	or	bx,bx
	jge	toasc1			; Make, continue
	cmp	al,VK_MENU		; Break, is it the Alt key?
	jz	ToAscAltUp
	xor	ax,ax			; No, just return 0 function
	jmp	ToAscx			; value
ToAscAltUp:				; Yes, Handle ALT-Numpad keys
	les	di,pState		; Yes, get state block pointer
	xor	ax,ax
	xchg	al,[fAnsi]		; clear ANSI flag, get state
	cmp	byte ptr es:[di],ah	; Anything in buffer
	jnz	ToAscAltNumPad		; No, ignore this key
	jmp	ToAsc0
ToAscAltNumPad:				; ALT release after digits were typed:
	or	al,al			; convert to ANSI?
	jnz	ToAscA			; 
	regptr	esdi,es,di
	cCall	OemToAnsi,<esdi,esdi>	; number was started with 0 digit.
ToAscA:
	jmp	ToAsc6			; go return (length 1).

;---------------------------------------------------------------------
;
;  Normal Virtual Key -> ASCII translation
;
ToAsc1:
	les	si,pKeyState	; get pointer to key state
	mov	ch,80H

;;;; Olivetti modification -- calculate shift state as bitmask in DH..
;;;; cl is used as counter here. (Updated 5 jan 88)

	mov	cl,0				; calculate shift state	
	test	es:[si+VK_SHIFT],ch		; is Shift key down?
	jz	T1NoShift
	inc	cl				; value is 1
T1NoShift:
	test	es:[si+VK_MENU],ch		; Is alt key down?
	jz	T1NoMenu
	or	cl,2				; value is 2

T1NoMenu:
	test	es:[si+VK_CONTROL],ch		; is Control down?
	jz	T1NoCtrl
	add	cl,4				; value is 4

T1NoCtrl:
	mov	dh,1				; calculate bitmask for state
	shl	dh,cl				; shift to bit for state


;;; dead key code follows, in conditional assembly section...

ifdef NODEAD

 if1
 %out .. No dead key handling
 endif

else

 if1
 %out .. Handles dead keys
 endif

;;;;;;;; determine if this key is a dead key ;;;;;;;;;;;;

	mov	bx,MortoBase			; get offset of TavoloMorto
	sub	bx,3

;;; Search TavoloMorto for dead key code
MortoLoop:					; [AL] is virt. keycode
	add	bx,3				; 3 bytes per entry
	cmp	byte ptr [bx],0			; check first byte..
	jz	non_e_morto			; 0 terminates
	cmp	al,[bx]				; does virtual code match?
	jne	MortoLoop
	test	[bx+1],dh			; is bit for this shift state
	jz	MortoLoop			; set? if so ..
						; it is a dead-key
	mov	al,[bx+2]			; get accent code
	mov	[dead_key],al			; save dead key
	les	di,pState
	stosb					; save in state block
	mov	ax,-1				; flag as dead key
JToAscX:
	jmp	ToAscX				; .. and exit

non_e_morto:

endif	; 'ifndef NODEAD' deadkey code

;;;;;;;;; end of Olivetti dead-key code ;;;;;;;;;;;;;;;;;;

;	Special handling for ICO 00 key -- need to force '0' if scancode
;	was 54h. (17 sep 87 update)

	cmp	KeyType,2			; Olivetti ICO keyboard?
	jne	Not00
	cmp	al,VK_0				; Some kind of 0 key?
	jne	Not00
	mov	bx, Scancode			; get scan code
	cmp	bl, S_ICO_00			; ICO 00 scan code?
	jne	Not00
	mov	ax,'0'				; yes,
	jmp	ToAsc5
Not00:

	; again, [DH] has bit [Olivetti shift no.] set.
	test	dh,(shift_5+shift_7)		; ctrl-alt,or shift-ctrl-alt? 
	jz	NotCtrlAlt
	jmp	ToAsc2				; .. check ctrl-alt table
NotCtrlAlt:
	test	dh,shift_3			; shift-alt? (16dec85)
	jnz	ToAscSearchAlt			; .. check alt table (16dec85)
	test	dh,shift_4			; alt?
	jnz	ToAscCkNumPad			; yes: check numpad input
JToAsc2b:
	jmp	ToAsc2b				; no: check non-alt tables.
ToAscCkNumPad:					; STATE is ALT!
	test	byte ptr KeyFlags,1		; is menu displayed
	jnz	JToAsc2b			; [ if too far, see above ]
	mov	bx,Scancode			; get raw scancode again
;;;	cmp	bl,57				; space?
;;;	je	JToAsc2b			; yes, go on directly
	XOR	bh,bh				;;;;;; clear bh
	cmp	bx,82				; is it on keypad?
	ja	ToAscSearchAlt			; no..., maybe it's in table
	cmp	bx,71
	jb	ToAscSearchAlt			; no .. in table?
	sub	bl,71
	mov	bh,[bx+KeyPadDigits]		; translate keycode to digit
	cmp	bh,-1				; is it a digit?	
	jz	ToAsc2b
				; *** it IS ALT-NUMPAD digit! *****
	les	di,pState	; Get pointer to state block
	mov	al,10
	mov	bl,es:[di]	; get oldvalue
	mul	bl		; times 10
	add	al,bh		; Add in this digit
	xor	ah,ah		; clear ah
	mov	es:[di],ax	; save 10 * oldvalue + digit
	cmp	al,bl		; if value is 10,
	jne	ToAscDigitDone	;  this is initial 0 digit..
	mov	[fAnsi],1	;  so then do ANSI
ToAscDigitDone:
	mov	al,ah		; AX = 0
	jmp	ToAscX		; *** end of ALT-NUMPAD code ****

ToAscSearchAlt:		; ALT or SHIFT-ALT (new, 16 Dec 85)
	jmp	ToAsc2b		; 29 jun 87 ... just exit.

					; CTRL-ALT or CTRL-SHIFT-ALT
ToAsc2:					; we know here Ctrl Alt keys are down
	mov	bx,CtrlAltDefault	; prepare to scan table
	mov	byte ptr [bx],al
	mov	bx,CtrlAltBase
	sub	bx,3
ToAsc2a:
	add	bx,3
	cmp	[bx],al			; look for valid code (+Ctrl Alt)
	jnz	ToAsc2a
					; Some kind of ctrl-ALT..
	mov	ax,[bx+1]		; get Xlation for base and shifted
	test	dh,shift_7		; ctrl-alt or shift-ctrl-alt?
ToAsc2CheckShift:
	jz	ToAsc2FoundNoShift	; 
	xchg	al,ah
ToAsc2FoundNoShift:
	jmp	short ToAsc3a

ToAsc2b:				; Not ALT:

ifdef SG
	;  .. check for special SHIFTLOCK ..
	; New code (Oli.) 4 mar 87 for special CapsLock translations.
	; updated 9 mar 87 .. CapsLock instead of ShiftLock
	; updated 23 sep 87 .. table is local, use IFDEF
; Swiss German Only
	test	[RTFlags],kbShiftLock	; KB is RT with shiftlock?
	jnz	ToAsc2sg		;  no, normal transl.
	test	[RTFlags],kbSLTable	; KB has special caps-lock codes?
	jz	ToAsc2sg		;  no, normal transl.
	test	byte ptr es:[si+VK_CAPITAL],1	; test capslock state
	jz	ToAsc2sg		; no capslock, do normal transl.
	mov	byte ptr ShiftLockDefLab,al ; OK, store scan code in default
	lea	bx,ShiftLockTab		; point to base of table
	sub	bx,2
ToAsc2c:				; search ShiftLockTab
	add	bx,2
	cmp	[bx],al			; look for code in table
	jne	ToAsc2c			; loop back if not found
	cmp	byte ptr[bx+1],-1	; if 'default',
	je	ToAsc2sg		;  go use normal transl. Otherwise,
	mov	al,[bx+1]		; get translated CapsLock code
	jmp	ToAsc5			; and return with it.
ToAsc2sg:
	; end of 4 mar 87 patch for SHIFTLOCK
endif	; SG -- Swiss German special code

	; Translate from normal ASCII table
	mov	bx,VirtualDefault
	mov	byte ptr [bx],al
	mov	bx,AsciiTabBase
	sub	bx,4
ToAsc3:
	add	bx,4
	cmp	[bx],al
	jnz	ToAsc3
						; 05 jan 87: change control
						; key handling.
	test	es:[si+VK_CONTROL],ch		; is control key down?
	jz	ToAsc3b				; no, forget it.
	mov	al,[bx+3]			; it's a control key
	cmp	al,-1				; translation defined?
	je	ToAsc3a				; if not, return nothing..
	or	al,al				; is shift required?
	jns	ToAsc3a				; no, return the key
	and	al,01111111b			; clear hi bit
	test	es:[si+VK_SHIFT],ch
	jnz     ToAsc3a
	mov	al,-1				; no shift, return nothing.
ToAsc3a:
	jmp	ToAsc4z				; Ctrl code got.
ToAsc3b:
	mov	ax,[bx+1]	; get translation for base and shifted
	test	es:[si+VK_SHIFT],ch
	jz	ToAsc3c
	xchg	al,ah
ToAsc3c:
	; handle caps lock (most keyboards) or shift lock (many RT keyboards)
	test	[RTFlags],kbShiftLock	; this kbd. has CapsLock or ShiftLock?
	jz	ToAsc3cTCap
	test	byte ptr es:[si+VK_OEM_SHIFTLOCK],80h	; test shiftlock state
	jnz	ToAsc3cCapLock
	jmp	short ToAsc4a
ToAsc3cTCap:
	test	byte ptr es:[si+VK_CAPITAL],1	; test for Caps lock toggle
	jz	ToAsc4a
ToAsc3cCapLock:
	cmp	dl,VK_A			; [dl] = VirtKey
	jb	ToAsc3d
	cmp	dl,VK_Z
	ja	ToAsc3d
	xchg	al,ah
	jmp	short ToAsc4a
ToAsc3d:
	mov	bx,CapitalDefault	; now, we need to look for extra
	mov	byte ptr[bx],dl		; characters affected by Caps Lock
	mov	bx,CapitalTabBase
	sub	bx,2
ToAsc4:
	add	bx,2
	cmp	[bx],dl		   ; test if virtual key is affected
	jnz	ToAsc4
	cmp	byte ptr [bx+1],-1 ; valid code ?
	jz	ToAsc4a		   ; no, jump
	xchg	al,ah		   ; yes, reverse the codes
ToAsc4a:

ifndef NODEAD

	; Dead key translation

	mov	ah,[dead_key]
	or	ah,ah			; is it a dead key already stored ?
	jz	ToAsc4z			; no, jump

; Code for Cedilla deadkey (Canadian, Netherlands) 15/10/87
ifdef CEDILLA				; handle Cedilla , Netherlands, Canada.
	cmp	ah,0b8h			; cedilla?
	jne	ToAscNotCedilla
	cmp	al,-1			; valid key?
	je	ToAsc4za		; no, just make DoLights
	mov	byte ptr [dead_key],0	; reset dead key flag
	cmp	al,'c'			; 'c' ?
	jne	ToAscChkC
	mov	al,0e7h			; c cedilla lc
	jmp	ToAsc5

ToAscChkC:
	cmp	al,'C'			; 'C' ?
	jne	ToAsc4zc		; if not, leave cedilla
	mov	al,0c7h			; C cedilla uc
	jmp	ToAsc5

ToAscNotCedilla:
endif

	mov	bx,DeadKeyDefault	; yes, prepare to scan table
	mov	byte ptr[bx],al
	mov	bx,DeadKeyBase
	sub	bx,6			; 6 including tilde column
ToAsc4c:
	add	bx,6
	cmp	[bx],al		; look for valid dead_key_table entry
	jnz	ToAsc4c
				; search done..
	cmp	byte ptr [bx+2],-1	; Is it valid? it's valid if it
	jne	ToAsc4DeadValid		; has either an acute accent or
	cmp	byte ptr [bx+5],-1	; a tilde.
	jz	ToAsc4zb		; no, jump
Toasc4DeadValid:
	mov	byte ptr [dead_key],0	; yes, reset dead key flag
	cmp	ah,grave	; is it grave accent ?
	jnz	ToAsc4d		; no
	mov	al,[bx+1]	; yes -- GRAVE is column 1
	jmp	short ToAsc4found
ToAsc4d:
	cmp	ah,acute	; is it acute accent ?
	jnz	ToAsc4e		; no
	mov	al,[bx+2]	; yes -- ACUTE is column 2
	jmp	short ToAsc4found
ToAsc4e:
	cmp	ah,circumflex	; is it circumflex ?
	jnz	ToAsc4f		; no
	mov	al,[bx+3]	; yes -- CIRCUMFLEX is column 3
	jmp	short ToAsc4found
ToAsc4f:
	cmp	ah,tilde	; is it tilde?
	mov	al,[bx+5]	; prepare for 'yes' -- TILDE is col. 5
	jz	Toasc4found
	mov	al,[bx+4]	; no, so it is umlaut -- column 4.

; Here, we need to handle a special case: "y umlaut" has
; the code 0FFh that means also -1 (used to indicate no Xlation).
; So we'll isolate this situation to allow this code to be sent.

;  Here we know we have umlaut, so just test if character= 'y'

	cmp	byte ptr[bx],'y'
	jnz	ToAsc4found	; no, it is not 'y umlaut'
	mov	al,0FFh		; yes , store y-umlaut code
	jmp	short ToAsc5
ToAsc4found:
	cmp	al,-1		; is the new code valid ?
	jnz	ToAsc5		; yes, jump
	mov	al,[bx]		; no, get back original code
	jmp	short ToAsc4zc	; and store them

; end of dead key translation
endif	; ifndef NODEAD

;; Key has been translated, call DoLights if -1 is translation, e.g.
;; in the case of shift keys.

ToAsc4z:
	cmp	al,-1
	jnz	ToAsc5
ToAsc4za:
	call	DoLights	; turn on/off lights

ToAsc0:		;*** Exit with 0 function value and 0 in state block: **
	xor	ax,ax			; return 0 value
	les	di,pState		; put 0 in state block word
	stosw
	jmp	ToAscX

ifndef NODEAD
				; MORE dead key stuff..
ToAsc4zb:			; invalid deadkey table entry.
	cmp	al,-1		; is it a valid code ?
	jz     ToAsc4za		; no, just make DoLights
ToAsc4zc:			; AH contains accent character
	; (This code changed to return 2 words in state block)
	mov	byte ptr[dead_key],0 ; reset dead key flag
	push	ax		; save character
	mov	al,ah		; get accent
	xor	ah,ah
	les	di,pState
	stosw			; store accent
	pop	ax		; get character back
	xor	ah,ah
	stosw			; store character
	mov	ax,2		; length= 2
	jmp	short ToAscX
endif				; ifndef NODEAD

ToAsc5:
	les	di,pState	; Get pointer to state block
	stosb
ToAsc6:
	mov	ax,1		; Return length
ToAscX:

; Here with result in AX and possibly the state block

cEnd	ToAscii

; This routine sets ROM BIOS's RAM flags for shift state, then does
; what is needed to make the lights, if any, correspond to the flags.

DoLights proc near
ifdef	ERICSSON
	cmp	KeyType,6		; 9140 keyboard
	jne	Not9140_1
	cmp	WORD PTR PCType,00FEH	;     - " -    connected to plain old PC
	jne	Not9140_1
	EnterCrit			; Prevent timer from updating LED's
Not9140_1:
endif
	xor	bx,bx

	test	RTFlags,kbShiftLock	; RT keyboard with SHIFTLOCK?
	jz	DoLi1

	mov	al,fCaps		; RT with SHIFTLOCK:
	mov	bl,VK_OEM_SHIFTLOCK
	call	ss1a			; test for state of virtual key
	jmp	short DoLi2
DoLi1:
	mov	al,fCaps		; handle CAPS LOCK
	mov	bl,VK_CAPITAL
	call	ss1
DoLi2:
	mov	al,fScroll		; handle SCROLL LOCK
	mov	bl,VK_OEM_SCROLL
	call	ss1

	mov	al,fNum			; handle NUM LOCK
	mov	bl,VK_OEM_NUMBER
	call	ss1

ifdef	ERICSSON
	cmp	KeyType,6		; 9140 keyboard
	jne	Not9140_2
	cmp	WORD PTR PCType,00FEH	;     - " -    connected to plain old PC
	jne	Not9140_2
	call	SetEriLights
	LeaveCrit			; Enable timer again
	ret
Not9140_2:
endif
;	To set keyboard lights for M24:
	call	SetOliLights		; set numlock, capslock lights
	ret				; main return form DoLights

; Subroutine used in above code: this has 2 entry points.
; 	ss1: set light depending on TOGGLE state of key
;	ss1a: set light depending on up/down state of key

ss1a:					; test for key up/down
	les	si,pKeyState
	test	byte ptr es:[si+bx],80h	; bit 7 set on downstroke
	jmp	short ss1test		;   cleared on upstroke

ss1:					; test for key toggled
	les	si,pKeyState
	test	byte ptr es:[si+bx],1	; bit 0 complemented on downstroke
ss1test:
	mov	si,0
	mov	es,si
	jz	ss2
	or	es:[ShiftState],al
	ret
ss2:	not	al
	and	es:[ShiftState],al
	ret

DoLights endp

sEnd	CODE		; End of code segment

ifdef VKFUN

; LOADONCALL DISCARDABLE segment for VkKeyScan()

createSeg _NORES1, NORES1, BYTE, PUBLIC, CODE
sBegin	NORES1
assumes	CS,NORES1
assumes	DS,DATA

; long VkKeyScan(char)
;
; This function is passed an ANSI code, and searches the translation
; tables for a virtual keycode which translates to this character.
; The VK_ code is returned in AL, and the shift state is returned
; in AH as follows:
;
;	0	no shift
;	1	character is shifted
;	2	character is control character
;
;	6	character is control-alt
;	7	character is shift-control-alt.
;
;	3,4,5	represent shift key combinations which are not used for
;		characters.

; if no key with this translation is found, -1 is returned.
; Also, translations with virtual keycodes for the numeric pad
; (VK_NUMPAD0..VK_DIVIDE) are ignored.  This is to force a translation
; for the main keyboard.

cProc	VkKeyScan,<PUBLIC,FAR>,<si,di>
	ParmW	AnsiCode		; Ansi code.
cBegin	VkKeyScan
	mov	ax, AnsiCode
	xor	ah,ah			; assume unshifted first.

	cmp	al,'A'			; UC letter's VK_ code is just
	jb	vkLower			; the ASCII code.
	cmp	al,'Z'
	ja	vkLower
	inc	ah			; set shifted state
	jmp	short vkleave

vkLower:
	cmp	al,'a'			; LC letter's VK_ code is just
	jb	vkAscii			; the ASCII code - 32.
	cmp	al,'z'
	ja	vkAscii
	sub	al,32			; VK is UC letter. 
	jmp	short vkleave

	; Translate from normal ASCII table
	; Scan downwards so OEM codes are seen before keypad codes for
	; MINUS, PLUS, DECIMAL, DIVIDE -- also, most punctuation is
	; seen first that way.

vkAscii:
	mov	bx,VirtualDefault	; Now, get adr. of end of table.
vk1:
	sub	bx,4
	cmp	bx,AsciiTabBase
	jb	vkCtrlAlt
	mov	dl,byte ptr [bx]	; Get virtual keycode from table,
	cmp	dl,VK_NUMPAD0		; and make sure it's not a numeric
	jb	vk1notpad		; pad key: 0..9, VK_MULTIPLY, ADD,
	cmp	dl,VK_DIVIDE		; SEPARATER, SUBTRACT, DECIMAL, or
	jbe	vk1			; DIVIDE
vk1notpad:
	mov	cx,word ptr[bx+1]	; get unshifted & shift
	cmp	al,cl
	je	vkunshifted
	cmp	al,ch
	je	vkshifted
	cmp	al,byte ptr[bx+3]	; get control & next virtual code.
	jne	vk1

vkcontrol:
	inc	ah			; ah = 2
vkshifted:
	inc	ah			; ah = 1
vkunshifted:
	mov	al,byte ptr [bx]	; Get virtual keycode from table.
	jmp	short vkexit

vkCtrlAlt:
	mov	ah,6			; try control-alt
	mov	bx,CtrlAltBase
vk2:
	cmp	bx,CtrlAltDefault
	jae	vkNotFound
	mov	cx,[bx+1]
	cmp	al,cl
	je	vkunshifted		; control-alt
	cmp	al,ch
	je	vkshifted		; shift-control-alt
	add	bx,3
	jmp	vk2
	
vkNotFound:
	mov	al,0ffh
	
vkexit:
	cmp	al,0ffh			; check for not-found
	jne	vkleave
	cbw				; not found, return -1
vkleave:

cEnd	VkKeyScan

sEnd NORES1	; end of nonresident segment

if2
%out .. VkKeyScan() implemented
%out
endif

else

if2
%out .. VkKeyScan() IS NOT implemented
%out
endif

endif	; VKFUN

if2
%out .. end TOASCII.asm
%out ..
endif

END		; of ToAscii.asm
