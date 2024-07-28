	page	,132
;****** TOASCII.ASM ********************************************************
;                                                                          *
;  Copyright (C) 1983-1990 by Microsoft Corporation.  All Rights Reserved. *
;                                                                          *
;   Translate virtual keycodes to ASCII.                                   *
;                                                                          *
;***************************************************************************
;
;	History
;
;	** Windows 3.xx **
;
;	19 mar 90	peterbe		Bug 10346:
;					Fixed handling of ctrl+shift input.
;					Removed bogus code at T1NoCtrl put
;					in 19 apr 89; changed code after
;					ToAscShCtrl:, at ToAsc3isCtrl: to
;					handle ctrl+shift+letter combos.
;	04 feb 90	peterbe		See warning comment about ToAscii()
;					pState parameter.
;	02 feb 90	peterbe		Clear fDeadKey when translation isn't
;					found.
;	24 jan 90	peterbe		Flags which were in state block changed
;					back to static variables fDeadKey,
;					fAnsi, fLastScan. Necessary because
;					a released Excel version assumes the
;					state block is only 4 bytes!  This will
;					mean that sequences of calls to
;					ToAscii() from User and and app which
;					include deadkeys and alt-numpad input
;					will be fouled up.  DON't allow it!
;
;	12 dec 89	peterbe		Further check in SetLightHardware()
;					for Oli. M240, which has 101/102 kbd.
;
;	11 dec 89	peterbe		Put checks in SetLightHardware() to
;					only call Oli. LED code if it's an
;					Olivetti system with OliType = 1, 2,
;					4.  Then installing KBDOLI.DRV on
;					wrong system won't cause crash.
;
;	16 oct 89	peterbe		Add offLastScan in state block, use
;					this flag to ignore repeats of alt-
;					numpad keys.  Also, init. flags for
;					numpad input when ALT is depressed.
;
;	12 oct 89	peterbe		Remove stuff around ToAscSyncNumlock:
;					for handling SendMessage(), which
;					won't work anyway since the key state
;					isn't set.
;
;	19 sep 89	peterbe		Load BX at ToAscSyncNumLock:
;
;	23 aug 89	peterbe		More NCR-related changes:
;					Handle special case where an app sends
;					a message with VK_NUMLOCK.. extended
;					bit isn't set in this case, so sync.
;					BIOS numlock flag.  See note above
;					DoLights() regarding this.
;
;	22 aug 89	peterbe		Old SetLightsHardware code integrated
;					into DoLights() (save a CALL!)
;					Added code related to NumLockFlag.
;					Further DoLights() changes.
;					All related to NCR keyboard problem.
;
;	07 aug 89	peterbe		No longer require NumLock to be down
;					for alt+numpad input.
;
;	18 jun 89	peterbe		Checked in AP's changes for Nokia 9140
;					(isNumPad:)
;
;	07 jun 89	peterbe		(1) KeyFlags parameter bit 1 indicates
;					extended key, so no ALT-numpad trans.
;					(2) if numlock is off, no ALT-numpad
;					keys are translated (even on enh. kbd.).
;
;	19 apr 89	peterbe		After T1NoCtrl:, code code which makes
;					Control+Shift = Control.
;					(for PageMaker)
;
;	06 apr 89	peterbe		Implementing lights-only call.
;					Deadkey flag now in state block
;					also fAnsi.
;
;	09 mar 89	peterbe		moved CLD to start of ToAscii().
;	10 nov 88	peterbe		TableSeg is externW now!
;	08 sep 88	peterbe		Removed ICO_00 code
;	19 aug 88	peterbe		Removed useless ID string.
;	18 aug 88	peterbe		Removed PCtype test in SetLightHardware
;	17 aug 88	peterbe		Checking NOKIA ifdefs
;	15 aug 88	peterbe		Fixing ICO 00 key code.
;	12 aug 88	peterbe		Changing to use RAMBIOS in DoLights,
;					load ES with 40H instead of 0.
;					Move kb_flags bits defs to keyboard.inc
;	11 aug 88	peterbe		Add page directive at beginning.
;					No longer have separate ICO and
;					Ico2 ifdefs -- need to test for
;					Type 2 keyboard.
;					Force definition of VKFUN.
;	02 aug 88	peterbe		Call GetTableSeg() only if hTables
;					is < 32.
;	28 jul 88	peterbe		Call GetTableSeg() now.
;	26 jul 88	peterbe		Deadkey accents no longer in main
;					translation tables.
;	21 jul 88	peterbe		Resolved ALT (SHIFT-ALT) menu bug.
;					Recoding VkKeyScan().
;	20 jul 88	peterbe		Mostly working.
;					Need to Resolve alt-letter,
;					alt-space bug.
;	18 jul 88	peterbe		Radical changes for new tables.
;					Removed olikbd.inc
;
;***************************************************************************

VKFUN = 0

include	keyboard.inc

include	vkwin.inc
include	vkoem.inc

if1
%out
%out   ToAscii.asm  (Microsoft)
    ifdef ICO
	%out Has Olivetti lights and ICO 00 key support.
    endif
    ifdef NOKIA
	%out Has Nokia LED support
    endif

endif

	TITLE	TOASCII	- ASCII	translation routine

	externFP	OemToAnsi
	externFP	GetTableSeg

sBegin	DATA	    ; Beginning of data segment

    externW hTables		; handle of table DLL

; some flags in the keyboard table:

    extrn	KeyType:byte	; keyboard type
    extrn	OliType:byte	; Olivetti keyboard hardware type ID
    extrn	PCType:byte	; keyboard type
    extrn	IsOli:byte	; 


; EXTRN declarations for translation tables.

; lists of trans tables for various shift states
;   (unsh/shift, ctrl, alt, ctrlalt)

    extrn	VirtAdr:word	; 4 pointers to virtual key lookup tables
    extrn	AsciiTab:word	; 4 vectors of Ascii translations
    extrn	VKSizes:word	; 4 sizes in bytes

; table for Caps Lock on keys other than VK_A..VK_Z

    extrn	pCapital:word	; list of virtual keys with capslock
    extrn	szCapital:word	; size

; table which identifies key+shift combinations for dead keys
; and translation to accent code.

    extrn	pMorto:word	; each word entry is VK (HI) + shift (HI)
    extrn	szMorto:word	; size in bytes of above table
    extrn	pMortoCode:word	; translation.

; table which translates deadkey accent + letter to accented letter

    extrn	szDeadKey:word	; size in bytes of table below
    extrn	pDeadKeyCode:word  ; each entry is accent (HI) + letter (LO)
    extrn	pDeadChar:word	; each entry is translated accented letter.


; special table for characters which are different with SHIFT and SHIFTLOCK
; This is mainly used for a Swiss-German keyboard for capital umlaut letters.

    extrn	szSGCaps:word	; size of search table
    extrn	pSGCapsVK:word	; list of virtual keycodes
    extrn	pSGTrans:word	; list of translated characters.

; Segment of tables (from TABS.ASM).

    externW	TableSeg


; Pointer to BIOS INT 16H handler, used for setting keyboard LED's on
; AT-compatible systems.

    extrn	LightsAddr:dword


; Rom bios data byte containing shift states
; This is accessed to synchronize BIOS handling of these keys
; with the windows driver, to handle LED's on the keyboard,
; and synchronize with the interrupt routine for keyboards which
; have the typewriter-type shiftlock.

; this maps shift state number into indices to tables like VKSizes.
ShiftMap label byte	; shift state numbers calculated in ToAscii()

    db	0		; 0: no shift
    db	0		; 1: shift
    db	2		; 2: control
    db	2		; 3: (shift-control -- handle almost like plain ctrl.)
    db	-1		; 4: alt (no translations for this)
    db	-1		; 5: (shift-alt) (no translations for this)
    db	4		; 6: control-alt
    db	6		; 7: shift-control-alt

; table for translating keypad codes for Alt-Numpad character input.

KeyPadDigits	db	7,8,9,-1	; 71 .. 74
		db	4,5,6,-1	; 75 .. 78
		db	1,2,3		; 79 .. 81
		db	0		; 82


; Flag for signaling a change in the BIOS NumLock in the interrupt routine.
; This is set when the interr. code sends VK_NUMLOCK.

public		NumLockFlag
NumLockFlag	db	0

; (24 jan 90: make these static instead of on state block)

fDeadKey	db	0		; saved accent value or 0

fAnsi		db	0		; Flags Ansi conversion in alt-numpad
					; input.

fLastScan	db	0		; last numeric-pad scancode

sEnd	DATA	    ; End of data segment

sBegin	CODE	    ; Beginning of code segment


; Routine to make Olivetti M24 lights correspond to keyboard flags state:

ifdef ICO
    extrn	SetOliLights:near
endif

ifdef	NOKIA
    extrn	SetEriLights:near
endif	; NOKIA


assumes	CS,CODE
assumes	DS,DATA
;
; Procedure to provide the default ASCII translation for an MS-WIN
; virtual key code.
;
; WARNING: no more than the first 2 words of the vector pointed to by
; pState must be altered by this function, due to assumptions made by
; some apps!
;
cProc	ToAscii,<PUBLIC,FAR>,<si,di>
	ParmW	VirtKey			; MS virtual keycode
	ParmW	Scancode		; 'hardware' raw scancode
	ParmD	pKeyState		; vector of key state flags
	ParmD	pState			; (output) vector of (1 or 2) data words
	ParmW	KeyFlags		; flags menu display, enhanced keys.
cBegin	ToAscii

	cld				; Make sure DF is cleared in this proc.
	cmp	hTables,32		; is DLL loaded?
	jae	ToAscSkipGet		; .. handle must be >= 32
	cCall	GetTableSeg		; load default table, if no DLL.
ToAscSkipGet:

	; If the state of NumLock has toggled, make sure its LED is set/reset.
	; It's necessary to do this quickly after the change in NumLock.
	; this way it happens on the very next call to ToAscii() after the
	; interrupt.

	cmp	NumLockFlag,0		; Has BIOS NumLock bit been changed?
	jz	ToAscSameNumLock	; yes,
	call	SetLEDs			;  set LEDs
	mov	NumLockFlag,0		; and clear the flag
ToAscSameNumLock:

	; This handles special case of calling Toascii(0,0,pKeyState,.....),
	; which just sets the keyboard lights according to pKeyState.

	mov	ax,VirtKey		; get the virtual key
	or	ax,ax			; is it nonzero?
	jnz	ToAscVirtNZ		; nonzero, normal processing.
					; Zero, so synch the BIOS flag and
	mov	al,fNum			; synchronize BIOS numlock flag
	mov	bx,VK_NUMLOCK		; with VK_NUMLOCK toggle in the
	call	ss1			; KeyState vector.
	jmp	ToAscBadChar		; set the LED's

ToAscVirtNZ:
	mov	dx,ax			; save (in DL or DX) for later
	mov	bx,Scancode		; get the (raw) scancode
	or	bx,bx
	jge	ToAsc1			; Make, continue

	; On a break which isn't ALT, we check to see if it was the last key
	; that was depressed (which might be repeating).  if it is,
	; we clear the last-scancode byte in the state vector, so that if
	; the same numeric key is depressed again (with ALT down), it WILL be
	; translated as a digit.
	; if it's an ALT key being released, we output the accumulated
	; ansi or oem character, if it's nonzero

	cmp	al,VK_MENU		; Break, is it the Alt key?
	les	di,pState		; (first, setup pointer to pState, clear
	je	ToAscAltUp		; again, was it the ALT key?
	xor	ax,ax			; no, it wasn't ALT. 
	cmp	bl,fLastScan		; is this the last key that went down?
	jne	ToAscNotSameBreak	; if it was, clear this flag, so the
	mov	fLastScan,al		; next depression won't 

ToAscNotSameBreak:
	jmp	ToAscx			; Not ALT, just return 0 function value

ToAscAltUp:				; It's ALT, Handle ALT-Numpad keys
	xor	ax,ax			; we want to clear some things:
	mov	fLastScan,al		;   clear last-scancode byte.)
	xchg	al,fAnsi		; clear ANSI flag, get state
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
;	This is a MAKE (Key depression)
;
;	AL, DL		contain virtual keycode
;	BL		contains 'raw' scancode.
;
ToAsc1:
	cmp	al, VK_MENU			; depression of ALT key?
	jne	ToAsc1NotMenu
	les	si, pState			; clear things in state block
	xor	cx, cx
	mov	es:[si], cx			; 'value' word = 0
	mov	fAnsi, cl			; data for alt+numpad = 0

ToAsc1NotMenu:

; Calculate internal shift-state byte, save in DH.
;
;	1 = shift
;	2 = control
;	4 = alt
;	.. and combinations of the above!

	les	si,pKeyState			; get pointer to key state
	mov	ch,80H				; this is a mask for state
						; bit in key state vector entry.
	xor	dh,dh				; calculate shift state	
	test	es:[si+VK_SHIFT],ch		; is Shift key down?
	jz	T1NoShift
	inc	dh				; value is 1
T1NoShift:
	test	es:[si+VK_MENU],ch		; Is alt key down?
	jz	T1NoMenu
	or	dh,4				; value is 4

T1NoMenu:
	test	es:[si+VK_CONTROL],ch		; is Control down?
	jz	T1NoCtrl
	add	dh,2				; value is 2

T1NoCtrl:
;	remove this 19mar90:
;	cmp	dh,3				; control+shift
;	jne	T1NotCtrlShift
;	mov	dh,2				; yes, make just shift.

public T1NotCtrlShift
T1NotCtrlShift:


; Dead key code follows.

; Determine if this key is a dead key.
; If it is, place the accent code in byte 0 of the state block (windows
; uses this as value for WM_DEADCHAR), and byte 4 as a flag used here
; in ToAscii the next time it's called.
; 

	; AL = virtual keycode
	; DH = shift state

	mov	ah,dh				; get shift state
	mov	cx,szMorto			; get table size
	shr	cx,1				; make word count
	jcxz	ToAscNodead			; do nothing if 0
	mov	es,TableSeg			; point ES:DI to table
	mov	di,pMorto			; get table address
	push	di				; save it
	repne scasw				; look for VK + shift
	pop	cx				; get base adr. back
	jne	ToAscNoDead
	sub	di,cx				; compute index
	dec	di
	shr	di,1				; change to byte offset
	mov	bx,pMortoCode
	mov	al,es:[bx+di]			; look up accent character
	les	di,pState			; also
	mov	fDeadKey,al			; save as static flag
	stosb					; and as WM_DEADKEY value
	mov	ax,-1				; count < 0 indicates dead key
	jmp	ToAscX				; .. and exit

ToAscNoDead:


; DH contains shift state (1:shift, 2:ctrl, 4:alt):

	cmp	dh,4				; alt?
	je	ToAscAlt
	cmp	dh,5				; shift alt?
	jne	ToAsc2				; some other state.

; STATE is ALT!  Either we're processing ALT-numpad input, or we're
; handling menu input.

ToAscAlt:
	test	byte ptr KeyFlags,1		; is menu displayed (bit 0)?
	jnz	ToAscNotNumPad			; if so, skip

	mov	bx,Scancode			; get raw scancode again
	xor	bh,bh				; clear bh
ifdef	NOKIA				; @@@ 89-03-15 / AP
	cmp	KeyType,6			; 9140 is special case
	jne	Not9140NumPad
	cmp	bl,82 + 080H			; NumPad has prefix
	ja	ToAscNotNumpad
	cmp	bl,71 + 080H
	jb	ToAscNotNumpad
	sub	bl,71 + 080H
	jmp	SHORT isNumPad
Not9140NumPad:
endif	; NOKIA				; @@@ 89-03-15 / AP
	cmp	bl,82				; is it on keypad?
	ja	ToAscNotNumpad			; no... no translation
	cmp	bl,71
	jb	ToAscNotNumpad			; no .. 
	sub	bl,71				; it's on the numeric pad.
ifdef	NOKIA				; @@@ 89-03-15 / AP
isNumPad:
endif	; NOKIA				; @@@ 89-03-15 / AP
	mov	bh,[bx+KeyPadDigits]		; translate keycode to digit
	cmp	bh,-1				; is it a digit?	
	jz	ToAscNotNumpad

	; *** it IS ALT-NUMPAD digit! -- do we want it? *****

	test	byte ptr KeyFlags,2		; is enhanced key (bit 1)?
	jnz	ToAscReturnZero			; yes .. so we don't want it.
						; This ISN'T an enhanced key.
ToAscDoDigit:
	les	di,pState			; Get pointer to state block
						; check for repeats:
	mov	al,byte ptr Scancode		; get scancode
	xchg	al,fLastScan			; swap with LAST scancode
	cmp	al,fLastScan			; is there a change?
	jne	ToAscIsNewDigit			; if not, key was repeating!
	xor	ax,ax				; clear count
	jmp	ToAscX

ToAscIsNewDigit:
	mov	al,10
	mov	bl,es:[di]			; get old numeric value
	mul	bl				; times 10
	add	al,bh				; Add in this digit
	xor	ah,ah				; clear ah
	mov	es:[di],ax			; save 10 * oldvalue + digit
	cmp	al,bl				; if value is 10,
	jne	ToAscDigitDone			;  this is initial 0 digit..
	mov	fAnsi,1				;  so then do ANSI
ToAscDigitDone:
	mov	al,ah				; AX = 0
	jmp	ToAscX

ToAscReturnZero:				; we decided not to translate
	jmp	ToAsc0

ToAscNotNumpad:					; not numpad:
	and	dh,3				; clear ALT bit, do normal
	jmp	short ToAsc2			; unshift/shift translation.

; *** end of ALT-NUMPAD code ***



; Handle [DH] == Unshift, Shift, Control, CTRL-ALT or CTRL-SHIFT-ALT

ToAsc2:

SG	equ	0	; try it!
ifdef SG	; -- Swiss German special code

; This handles keyboards which have some characters which are different
; with SHIFT LOCK and SHIFT, such as the Swiss German Enhanced keyboard.

	les	si,pKeyState			; get pointer to key state
	test	byte ptr es:[si+VK_CAPITAL],1	; test for Caps lock toggle
	jz	ToAscNoSpecSL			; if caps lock off, skip this
	mov	cx,szSGCaps			; anything in table?
	jcxz	ToAscNoSpecSL			; skip if empty...
	mov	es,TableSeg
	mov	di,pSGCapsVK			; ES:DI -> virtual key list
	push	di
	repne scasb
	pop	cx
	jne	ToAscNoSpecSL
	sub	di,cx				; found, compute index
	dec	di
	mov	bx,pSGTrans
	mov	al,es:[bx+di]			; look up umlaut character
	jmp	ToAscGoodChar			; 

ToAscNoSpecSL:

endif	; SG -- Swiss German special code

; handle alpha characters specially.  Just use arithmetic
; to translate VK to letter or control code!

						; various changes 19mar90:
	cmp	dh, 3				; shifts 0, 1, 2, 3
	ja	ToAsc3NotAlpha			; .. maybe ctrl-alt
	cmp	al,VK_A
	jb	ToAsc3NotAlpha
	cmp	al,VK_Z
	ja	ToAsc3NotAlpha
	cmp	dh,2				; CTRL (2) or CTRL+SHIFT (3)?
	jl	ToAsc3NotCtrl
ToAsc3isCtrl:					; CTRL or CTRL+SHIFT + letter:
	and	al,1fh				; make control char
	jmp	ToAscGoodChar

ToAsc3NotCtrl:					; handle shifted or unshifted
	mov	ah,al				; UC letter in AH,
	add	al,32				; LC letter in AL.
	jmp	short ToAscTestShift		; go consider caps lock..

ToAsc3NotAlpha:

; Likewise for numeric-pad virtual keys VK_NUMPADx ...

	cmp	al,VK_NUMPAD0			; handle VK_NUMPADx
	jb	ToAscNotVK_NUMPAD
	cmp	al,VK_NUMPAD9
	ja	ToAscNotVK_NUMPAD
	cmp	dh,1				; must be unshifted or shift
	ja	JToAscBadChar
	add	al,'0'-VK_NUMPAD0		; just make digit!
	jmp	ToAscGoodChar

JToAscBadChar:
	jmp	ToAscBadChar

ToAscNotVK_NUMPAD:

; Handle unshifted, shifted, control, control-alt, shift-control-alt..

; Search table for match to virtual keycode...

; dl doesn't have the shift state. test later for shift state (dh) is
; correct. Bug fix : 3906
;	cmp	dl,1				; unshifted or shifted?
;	jbe	ToAscLoShifts			; .. ctrl or above..

	mov	bl,dh				; get shift state
	xor	bh,bh
	mov	bl,[bx+ShiftMap]		; map to table index
	cmp	bl,-1				; legal shift state?
	je	JToAscBadChar			; not found if bad state
	mov	cx,[bx+VKSizes]			; get size of table for shift
	jcxz	JToAscBadChar			; if empty, bad char.
	mov	es,TableSeg			; ES for string searches..
	mov	di,[bx+VirtAdr]			; ES:DI -> table to search
	push	di				; save start of table
	repne scasb				; search for VK code
	pop	cx				; get base ptr. back,
	jne	JToAscBadChar			; if not found, jump.
	sub	di,cx				; found, so
	dec	di				; compute index to item found.

; Now, we handle control and higher shifts differently from unshift and shift.

	cmp	dh,1
	jbe	ToAscLoShifts
						; it's ctrl, ctrl-alt, etc.
	add	di,[bx+AsciiTab]		; get pointer to trans. chr.
	mov	al,es:[di]

; Control and shift-control require checking the sign bit..

	cmp	dh,2				; is it ctrl?
	jne	ToAscShCtrl
	test	al,80h				; control -- don't allow entry
	jne	ToAscBadChar			; with sign bit set.
	jmp	ToAscGoodChar			; return with this char.
ToAscShCtrl:
	cmp	dh,3				; is it shift-ctrl?
	jne	JToAscGoodChar
	test	al,80h				; character must have sign set
	jz	ToAscBadChar			; (changed 19mar90)
	and	al,1fH				; mask to 5 bits
JToAscGoodChar:
	jmp	ToAscGoodChar			; return with this char.

; look up unshifted/shifted chars in table..

ToAscLoShifts:
	add	di,di				; double index
	add	di,[bx+AsciiTab]		; compute ptr. to translations
	mov	ax,es:[di]

ToAscTestShift:
	or	dh,dh				; unshifted or shift?
	jz	ToAscTestCaps			; (Z if no shift)
	xchg	al,ah

ToAscTestCaps:

; handle caps lock

	les	si,pKeyState			; get pointer to key state
	test	byte ptr es:[si+VK_CAPITAL],1	; test for Caps lock toggle
	jz	ToAscNoCaps
						; Caps lock IS set...
	cmp	dl,VK_A				; [dl] = VirtKey
	jb	ToAscCapNotAlpha		; letters A..Z are affected
	cmp	dl,VK_Z				; by caps lock.
	ja	ToAscCapNotAlpha
	xchg	al,ah				; It's a letter, swap
	jmp	short ToAscNoCaps

ToAscCapNotAlpha:			; other key affected by CapsLock?
	mov	cx,szCapital			; prepare to search
	jcxz	ToAscNoCaps			; jump if no entries
	mov	es,TableSeg
	mov	di,pCapital			; ES:DI -> caps lock list.
	mov	bx,ax				; save trans. characters
	mov	al,dl				; get VK back
	repne scasb				; search Capital table
	mov	ax,bx				; get chars back
	jne	ToAscNoCaps			; in table?
	xchg	al,ah		   		; yes, reverse the codes
ToAscNoCaps:

	cmp	al,-1				; end of shift/unshift trans.
	jne	ToAscGoodChar			; char was good
						; char was -1


; Key has been translated, call DoLights if -1 is translation, e.g.
; in the case of shift keys.

ToAscBadChar:
	call	DoLights		; turn on/off lights

ToAsc0:		;*** Exit with 0 function value and 0 in state block: **
	xor	ax,ax			; return 0 value
	les	di,pState		; put 0 in state block word
	stosw
	jmp	ToAscX

ToAscGoodChar:					; A valid character has


; Dead key translation.  Check to see if the PREVIOUS keystroke was a dead
; key.

	mov	ah,fDeadKey			; get dead key value (0=none)
	or	ah,ah				; is a dead key stored ?
	jz	ToAscGoodExit			; no, jump

; Handle translation of deadkey accent (previously stored) + current char.
; to some accented character.  The value in AH is actually the accent.

	mov	cx,szDeadKey			; if deadkey table empty,
	jcxz	ToAscGoodExit			; handle as normal key.
	shr	cx,1
	mov	es,TableSeg			; AL=char, AH=accent
	mov	di,pDeadKeyCode			; DI -> search table
	push	di				; save base address
	repne scasw				; now search the table
	pop	cx				; get base back
	jne	ToAscDeadBad			; found?

	sub	di,cx				; yes, compute index
	shr	di,1				; / 2
	dec	di
	mov	bx,pDeadChar
	mov	al,byte ptr es:[di+bx]		; get translated char.
	mov	fDeadKey,0			; reset dead key flag
	jmp	ToAscGoodExit

ToAscDeadBad:			; AH = accent character, AL = new ASCII

; We couldn't find the combination of letter + accent in the
; deadkey translation table, so return the letter and the
; accent as 2 words in the state block.

	push	ax				; save character
	mov	al,ah				; get accent
	xor	ah,ah
	les	di,pState
	stosw					; store accent
	pop	ax				; get character back
	xor	ah,ah
	stosw					; store character
	mov	fDeadKey,0			; reset dead key flag
	mov	ax,2				; length= 2
	jmp	short ToAscX


ToAscGoodExit:

	les	di,pState	; Get pointer to state block
	stosb
ToAsc6:
	mov	ax,1		; Return length
ToAscX:

; Here with result in AX and possibly the state block

cEnd	ToAscii

; This routine sets ROM BIOS's RAM flags for shift state, then does
; what is needed to make the lights, if any, correspond to the flags.
;
; The BIOS CapsLock and ScrollLock flags are synchronized with the KeyState
; vector here.   The BIOS NumLock flag is sync'd in the interrupt routine
; when the key is pressed, and also in ToAscii() if either (a) the specal
; call from SetKeyboardState() is made, or (b) if an app sends a WM_KEYDOWN
; message with VK_NUMLOCK, with the extended bit in KeyFlags off.

DoLights proc near

ifdef	NOKIA
	cmp	KeyType,6		; 9140 keyboard
	jne	Not9140_1
	cmp	WORD PTR PCType,00FEH	;     - " -    connected to plain old PC
	jne	Not9140_1
	EnterCrit			; Prevent timer from updating LED's
Not9140_1:
endif	; NOKIA

	xor	bx,bx

	mov	al,fCaps		; handle CAPS LOCK
	mov	bl,VK_CAPITAL
	call	ss1
DoLi2:
	mov	al,fScroll		; handle SCROLL LOCK
	mov	bl,VK_OEM_SCROLL
	call	ss1

; Now that we've set the BIOS flags, we set the LED's, either in
; hardware-specific ways, or by calling an INT 16H function, depending
; on the system:

SetLEDs:				; call here from start of ToAscii().

ifdef	NOKIA
	; It's a Nokia system, so determine the system/keyboard type...
	cmp	KeyType,6		; 9140 keyboard only
	jne	Not9140_2
	cmp	WORD PTR PCType,00FEH	;     - " -    connected to plain old PC
	jne	Not9140_2
	call	SetEriLights
	LeaveCrit			; Enable timer again
	ret
Not9140_2:
endif	; NOKIA

public SetLightHardware			; called from Enable()
SetLightHardware:
ifdef ICO
	; it's an older Olivetti or AT&T system without INT 16 LED support:
	cmp	IsOli, 0		; is this REALLY Olivetti system?
	jz	SetCloneWay		; 
	cmp	OliType, 0		; Keyboard identified itself as
	jz	SetCloneWay		; 01, 02, 04, 10, 40, 42, etc.
	cmp	PCType, IDM240		; if it's M240 (which has 101/102 kbd),
	je	SetM24Way		;  we use the M24 protocol; otherwise,
	cmp	OliType, 4		; we only handle LED's M24-way
	ja	SetCloneWay		; if it's type 1, 2, 4
SetM24Way:
	call	SetOliLights		; We assume M24, M240, 6300, 6300+
	ret				; return from SetLightHardware() or
					; DoLights()...
SetCloneWay:
endif ; ICO
	mov	ah,1			; Calling this INT 16H function will
	pushf				; set lights according to bits in
	call	[LightsAddr]		;  [kb_flag].
	ret				; main return from DoLights

; Subroutine used in above code.
; NOTE: this function assumes the parameters of ToAscii() are on the
; stack!!
; 	ss1: set light depending on TOGGLE state of key

ss1:					; test for key toggled
	les	si,pKeyState
	test	byte ptr es:[si+bx],1	; bit 0 complemented on downstroke
ss1test:
	mov	si,RAMBIOS
	mov	es,si
	jz	ss2
	or	es:[kb_flag],al
	ret
ss2:	not	al
	and	es:[kb_flag],al
	ret

DoLights endp

sEnd	CODE		; End of code segment

ifdef VKFUN

; LOADONCALL DISCARDABLE segment for VkKeyScan()

createSeg _VKKSC, VKKSC, BYTE, PUBLIC, CODE
sBegin	VKKSC
assumes	CS,VKKSC
assumes	DS,DATA

; int VkKeyScan(char)
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

vkshifts label byte	; This maps index to actual shift state
			; note the DW's to pad since the index is double!
    dw	0	; unshifted (or shifted)
    dw	2	; control
    dw	6	; control-alt
    db	7	; shift-control-alt

cProc	VkKeyScan,<PUBLIC,FAR>,<si,di>
	ParmW	AnsiCode			; Ansi code.
cBegin	VkKeyScan

	cmp	hTables,32			; is DLL loaded?
	jae	VkSkipGet			; .. handle must be >= 32
	cCall	GetTableSeg			; load table, if no DLL.
VkSkipGet:

	mov	ax, AnsiCode
	xor	ah,ah				; assume unshifted first.

	cmp	al,0FFh				; filler key in asctran
	je	vkNotFound			; get out
	cmp	al,'A'				; UC letter's VK_ code is just
	jb	vkLower				; the ASCII code.
	cmp	al,'Z'
	ja	vkLower
	inc	ah				; set shifted state
	jmp	short vkleave

vkLower:
	cmp	al,'a'				; LC letter's VK_ code is just
	jb	vkAscii				; the ASCII code - 32.
	cmp	al,'z'
	ja	vkAscii
	sub	al,32				; VK is UC letter. 
	jmp	short vkleave

	; Translate from normal ASCII tables
	; Loop through all 4 possible tables, to find a matching
	; ASCII code.

vkAscii:
	mov	es,TableSeg			; point ES to tables' segment
	xor	bx,bx				; index to VirtAdr, etc.
	mov	cx,4				; loop counter
vkLoop:
	push	cx
	mov	cx, [bx+VKSizes]		; get size of VirtAdr[bx] table
	jcxz	vkLoopNext
	or	bx,bx				; is it shift or unshift?
	jnz	vkLoopHigherShift
	shl	cx,1				; yes, double count
    vkLoopHigherShift:
	mov	di,[bx+AsciiTab]		; get adr of Ascii char. array
	push	di				; save base adr.
	repne scasb
	pop	dx				; get base adr. back.
	je	vkFound
    vkLoopNext:					; not found, try next shift?
	add	bx,2
	pop	cx
	 loop	vkLoop

	; we completed loop, so not found so far.. is it
	; a control-letter character?  We do this check LAST to avoid
	; translating CR, LF, TAB, etc. this way.

	cmp	al,1				; control-letter's VK_ code
	jb	vkNotFound			; is just the ASCII code + 64.
	cmp	al,26
	ja	vkNotFound
	add	al,64				; VK control code + 64
	mov	ah,2				; set control
	jmp	short vkleave

vkNotFound:
	mov	ax,0ffffh			; Character was not
	jmp	short vkleave			; found, return -1

vkFound:
	pop	cx				; restore stack.
	mov	ah,cs:[vkshifts+bx]		; convert BX to shift state
	sub	di,dx				; compute index to Ascii array
	dec	di
	or	bx,bx				; shift or unshift, or higher?
	jnz	vkLookup
						; it's SHIFT or UNshift.
	test	di,1				; if odd, shift, else unshift
	jz	vkUnshift
	inc	ah				; it's Shift.
vkUnshift:
	shr	di,1				; cut index in half

vkLookup:
	mov	bx,[bx+VirtAdr]			; adr of VK_ table
	mov	al,es:[bx+di]			; look up virtual keycode.
	
vkleave:

cEnd	VkKeyScan


sEnd VKKSC	; end of nonresident segment

if2
%out .. VkKeyScan()  implemented !!
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

