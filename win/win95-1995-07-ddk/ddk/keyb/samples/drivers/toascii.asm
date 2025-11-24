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

      page    ,132
;***************************************************************************
;
;****** TOASCII.ASM ********************************************************
;                                                                          *
;   Translate virtual keycodes to ASCII.                                   *
;                                                                          *
;***************************************************************************

.xlist
include keyboard.inc

include vkwin.inc
include vkoem.inc
.list
include lcid.inc

if1
%out
%out   ToAscX.asm  (Microsoft)
endif
        TITLE   TOASCIIX - NEW ASCII translation routine

;*****************************************************************************

;
; For now the tables are hard coded here.
;
; Data above the TABLES line is hard coded into the driver. data below will
; be moved when Damian finishes Enable()
;

externFP    	OemToAnsi
externFP	RareInquireEx

ifdef JAPAN
externFP SetLEDsAX
endif

regptr      esdi,    es,di

by          equ      byte ptr
SCAN_ALT    equ      038H

;*****************************************************************************

sBegin   DATA

externB         VirtKeys
externA         BaseAnsi_Len
externB         AsciiVirtKeys
externB         BaseAnsi

externW         pLCIDlist
externW         nKeyboards
externD         LightsAddr

externB 	fKeyType            ; flags mainly for RT keyboard.
externB         NumPad

globalB  fLastScan      0               ; last numeric-pad scancode
globalB  fAnsi          0               ; Flags Ansi conversion in alt-numpad
                                        ; input.
globalB  fDeadKey       0               ; last dead key hit

globalW pSystemLocale 0
globalD SystemLocale  <0FFFF0409h>

globalD CurrentLocale,  <0FFFF0409H>
globalW pCurrentLocale, 0

globalB NumLockFlag     0

TableSeg	dw	0		; for backhack for CCW

;---------------------------------------------------------------------------

CtrlBaseVKeys   label byte
	db	0DH		; Ctrl-Return = LF
	db	08H		; Ctrl-Backspace = 07FH
	db	20H		; Ctrl-Space = Space

CtrlShiftBaseVKeys label byte
	db	'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
CTRLVKEY_LEN		equ $ - CtrlBaseVKeys
CTRLSHIFTVKEY_LEN	equ $ - CtrlShiftBaseVKeys

NormalBaseVKeys label byte
	db	060h		; numpad 0
	db	061h		; numpad 1
	db	062h		; numpad 2
	db	063h		; numpad 3
	db	064h		; numpad 4
	db	065h		; numpad 5
	db	066h		; numpad 6
	db	067h		; numpad 7
        db      068h		; numpad 8
	db	069h		; numpad 9
	db	003H

ShiftBaseVKeys	label byte
        db	01BH		; VK_ESCAPE
        db	009H		; VK_TAB
        db	00DH		; VK_RETURN

AltBaseVKeys	label byte
        db	008H		; VK_BACK
        db	020H		; VK_SPACE
        db      06fh		; numpad /
	db	06ah		; numpad *
	db	06dh		; numpad -
	db	06bh		; numpad +
	db	06eh		; numpad .

NORMALVKEY_LEN	equ $ - NormalBaseVKeys
SHIFTVKEY_LEN	equ $ - ShiftBaseVKeys
ALTVKEY_LEN	equ $ - AltBaseVKeys

;-----------------------------------------------------------------------------

CtrlBaseAnsiValues	label byte
	db	0AH		; Ctrl-Return = LF
	db	07FH		; Ctrl-Backspace = 07FH
	db	020H		; Ctrl-Space = Space

CtrlShiftBaseAnsiValues label byte
	db	1,2,3,4,5,6,7,8,9,10
	db	11,12,13,14,15,16,17,18,19,20
	db	21,22,23,24,25,26

NormalBaseAnsiValues	label byte
        db      '0'		; numpad 0
	db	'1'		; numpad 1
	db	'2'		; numpad 2
	db	'3'		; numpad 3
	db	'4'		; numpad 4
	db	'5'		; numpad 5
	db	'6'		; numpad 6
	db	'7'		; numpad 7
	db	'8'		; numpad 8
	db	'9'		; numpad 9
	db	3
ShiftBaseAnsiValues	label byte
	db	01bH
	db	9		; tab
	db	00dH		; return

AltBaseAnsiValues label byte
        db      8		; backspace
	db	020H		; space
	db      '/'		; numpad /
	db	'*'		; numpad *
	db	'-'		; numpad -
	db	'+'		; numpad +
	db	'.'		; numpad .

;-----------------------------------------------------------------------------

BaseVKeyTables	label word
	dw	offset NormalBaseVKeys		; state = 0 (normal)
	dw	offset AltBaseVKeys		; state = 1 (alt)
	dw	offset ShiftBaseVKeys		; state = 2 (shift)
	dw	offset AltBaseVKeys		; state = 3 (alt+shift)
	dw	offset CtrlBaseVKeys 		; state = 4 (control)
	dw	0				; state = 5 (control+alt)
	dw	offset CtrlShiftBaseVKeys	; state = 6 (control+shift)
	dw	0				; state = 7 (ctrl+alt+shift)

BaseAnsiTables	label word
	dw	offset NormalBaseAnsiValues	; 0 (normal)
	dw	offset AltBaseAnsiValues	; 1 (alt)
	dw	offset ShiftBaseAnsiValues	; 2 (shift)
	dw	offset AltBaseAnsiValues	; 3 (alt+shift)
	dw	offset CtrlBaseAnsiValues 	; 4 (control)
	dw	0				; 5 (control+alt)
	dw	offset CtrlShiftBaseAnsiValues	; 6 (control+shift)
	dw	0				; 7 (ctrl+alt+shift)

BaseLengths	label word
	dw	offset NORMALVKEY_LEN
	dw	offset ALTVKEY_LEN
	dw	offset SHIFTVKEY_LEN
	dw	offset ALTVKEY_LEN
	dw	offset CTRLVKEY_LEN
	dw	0
	dw	offset CTRLSHIFTVKEY_LEN
	dw	0

sEnd     DATA

;*****************************************************************************

sBegin   CODE        ; Beginning of code segment
.386p
assumes CS,CODE
assumes DS,DATA

;*****************************************************************************

;
; edx enters with the lcid
;
; exit:
; eax = lcid if succeeds
;  es = segment of the table
;  dx = segment of the table
;  bx = table offset or zero
;  si = address in table of offset
;
; BIG DIRTY ASSUMPTION:
; 	No caller here will be calling out from the driver during the call.
; 	This means LMHtoP is sufficient.
;
GetLCIDtable label far
        public  GetLCIDtable
        ;
        ; get the LCID table we want to use
        ;
	cld					; set this for everyone!
        mov	cx,	nKeyboards
	jcxz	NoLCID				; could be too early!
        xor	esi,	esi
	mov	si,	pLCIDlist
	LMHtoP	si

GetNextLCID:
        lodsd
        cmp   eax,     edx
        je    GotLCID
        inc   si             ; skip the pointer
        inc   si
        loop  GetNextLCID
NoLCID:
        xor   bx,      bx
        jmp   GetLCIDDone

GotLCID:
        ;
        ; si points to the table offset, make bx point to lcid table
        ;
	.errnz (pKeyData-4)			; si not at slot start
	mov	bx, [si]
	mov	dx, ds
	mov	es, dx

GetLCIDDone:
        retf

;*****************************************************************************

	;
	; comes in with bx == pCurrentLocale, only uses ax
	;

SetCurrentLocale	label far
	public SetCurrentLocale

fChanging = (kbAltGr or kbShiftLock)

	mov	ax, [bx+LCID_FLAGS]	; get flags for this lcid
	mov	ah, ds:[fKeyType]	; get global flags
	and	ah, not fChanging	; keep all but AltGr
	and	al, fChanging		; get only AltGr flag
	or	ah, al			; merge flags
	mov	ds:[fKeyType], ah	; put them back
	retf

;*****************************************************************************

cProc   InquireEx,<PASCAL, LOADDS, PUBLIC,FAR>
	parmW	wCode
	parmD	lpBuffer
	parmW	nCount
cBegin
	push	esi
	mov	edx, lpBuffer

	mov	ax, wCode
	cmp	ax, INQEX_SETBASELOCALE
	je	SetBaseLocale
	cmp	ax, INQEX_SETSYSLOCALE
	je	SetSysLocale
	;
	; table thing.
	;
	pop	esi
	jmp	RareInquireEx
SetSysLocale:

	call	GetLCIDtable
	or	bx, bx
	jz	InqExDone
	xchg	pSystemLocale, bx
	mov	edx, lpBuffer
	mov	SystemLocale, edx
	cmp	bx, pCurrentLocale
	jne	InqExDone
	jmp	CurrFromSys

SetBaseLocale:
	call	GetLCIDtable
	or	bx, bx
	jnz	@F
CurrFromSys:
	mov	bx, pSystemLocale
	mov	eax, SystemLocale
@@:
	mov	pCurrentLocale, bx
	mov	CurrentLocale, eax
	call	SetCurrentLocale
InqExDone:
	pop	esi
cEnd

;*****************************************************************************
; ** GetTableSeg() ***************************************
;
; This finds the paragraph of the TABS segment and stores
; it in TableSeg.
;
; Calling this will force the segment to be loaded.
;
; This segment isn't written to.
;
; REMEMBER that AX is TRASHED !!
;

cProc	GetTableSeg,<PASCAL,PUBLIC,FAR,LOADDS>
cBegin
	mov	ax,ds
	mov	TableSeg,ax
cEnd

;*****************************************************************************

        ;***
        ;*** old style entry point
        ;***

ToAscii label far
        public ToAscii
        mov     ax, ds      ; prolgue
        nop
        mov     es, ax
        pop     ebx             ; get back the return address
        mov     ecx, es:CurrentLocale
        push    ecx
        push    ebx
        jmp     ToAsciiEx+3

;*****************************************************************************

;
; Procedure to provide the default ASCII translation for an MS-WIN
; virtual key code.
;
; WARNING: no more than the first 2 words of the vector pointed to by
; pState must be altered by this function, due to assumptions made by
; some apps!
;

cProc   ToAsciiEx,<LOADDS, PASCAL, PUBLIC,FAR>

        ParmW   VirtKey      ; MS virtual keycode
        ParmW   Scancode     ; 'hardware' raw scancode
        ParmD   pKeyState    ; vector of key state flags
        ParmD   pState       ; (output) vector of (1 or 2) data words
        ParmW   KeyFlags     ; flags menu display, enhanced keys.
        ParmD   lcid         ; locale value

	localV  KS, %(SIZE DeadKeyState)
cBegin
        push    ds
        push    fs
        push    esi
        push    edi

        ; If the state of NumLock has toggled, make sure its LED is set/reset.
        ; It's necessary to do this quickly after the change in NumLock.
        ; this way it happens on the very next call to ToAscii() after the
        ; interrupt.

        cmp     NumLockFlag,    0   ; Has BIOS NumLock bit been changed?
        jz      ToAscSameNumLock    ; no, bypass
        call    SetLEDs		        ; yes, set LEDs
        mov     NumLockFlag,    0   ; and clear the flag for next change

ToAscSameNumLock:

        mov   ax, VirtKey
        or    ax, ax
        jnz   RealConv				;this is a real VKey request

        ; This handles special case of calling Toascii(0,0,pKeyState,.....),
        ; which just sets the keyboard lights according to pKeyState.
;
; Zero Vkey, so synch the BIOS flag
;
	mov	al,fNum			; synchronize BIOS numlock flag with
	mov	bx,VK_NUMLOCK	; VK_NUMLOCK toggle in the KeyState
	call	ss1			; vector. Set one LED based on VKey
	jmp	ToAsc00		    ; Go do all lights and exit with zero
;
; now get the scancode converted into an offset for the state tables
;
RealConv:
	mov	KS.dksVKey, al		; vkey as byte value.
	mov	KS.dksPudding, 0
	mov     edx, lcid		;local ID

	call    GetLCIDtable
	or      bx, bx
	jz      ToAsc00			;invalid locale ID table, exit

	mov     ax, VirtKey
	mov     dx, ScanCode
	or      dx, dx
	jge     ToAsciiMakeKey	;go do keydown logic

;----------------------------------------------------------------------------
BreakKey:
        ;
        ; key is going up, we dont do a conversion, however, we need to check
        ; the numpad keys
        ;
        ; On a break which isn't ALT, we check to see if it was the last key
        ; that was depressed (which might be repeating).  if it is,
        ; we clear the last-scancode byte in the state vector, so that if
        ; the same numeric key is depressed again (with ALT down), it WILL be
        ; translated as a digit.
        ; if it's an ALT key being released, we output the accumulated
        ; ansi or oem character, if it's nonzero

        cmp     al, VK_MENU            ; Break, is it the Alt key?
        je      AltUp                  ; again, was it the ALT key?

        xor     ax, ax                 ; no, it wasn't ALT.
        cmp     dl, fLastScan          ; is this the last key that
                                       ; went down?
        jne     NotSameBreak           ; if it was, clear this flag, so the
        mov     fLastScan, al          ; next depression won't

NotSameBreak:
        jmp     Done                   ; Not ALT, just return 0
                                       ; function value

;-----------------------------------------------------------------------------

        ;
        ; It's ALT, Handle ALT-Numpad keys
        ;
AltUp:
      les   di,         pState         ; (1st, setup pointer to pState, clear
      xor   ax,         ax             ; we want to clear some things:
      mov   fLastScan,  al             ;   clear last-scancode byte.)
      xchg  al,         fAnsi          ; clear ANSI flag, get state
      cmp   by es:[di], ah             ; Anything in buffer
      jnz   AltNumPad                  ; No, ignore this key
      ;
      ;*** Exit with 0 function value and 0 in state block: **
      ;*** ------------------------------------------------ **
      ;
ToAsc0:
ToAsc00:
        call    DoLights		   ; turn on/off lights
        xor     ax,         ax	   ; return 0 value
        mov     dx,         ax
        jmp     StoreChars		   ; save in user buffer and exit

AltNumPad:                         ; ALT release after digits were typed:
        or      al,         al     ; convert to ANSI?
        jnz     NoOemConv		   ; no, must start with zero
        cCall   OemToAnsi,  <esdi,esdi> ; number was started with 0 digit.
NoOemConv:
        mov     ax,         1
        jmp     Done               ; go return (length 1).

;---------------------------------------------------------------------------
;------------------------------- MAINLINE CODE -------------------------------
;---------------------------------------------------------------------------
ToAsciiMakeKey:
	public ToAsciiMakeKey
;
; workhorse of the call - make (keydown) state - bomb out early if
; the scan code is ALT
;
;On entry:  bx  = base pointer          uses:  cx
;           dx  = ScanCode
;
;Sets:   fs:di = ptr to key state table
;           si = ptr to the state keys used
;          edx = saves scancode in high word, states in low word
;          ebx = LCID ptr in high word
;
	cmp	al, VK_MENU
	je	AltDown				;wait for more key strokes

	lfs	di,     pKeyState           ; get the key state table
	shl	edx,    16                  ; save ScanCode

	mov	dx,     0100H               ; dh=1st state value,
                                    ; dl=total state
	mov	cx,     [bx+NSTATE_KEYS]    ; number of state keys
	mov	si,     [bx+STATE_LIST]     ; point to the state keys to use

	rol	ebx,    16                  ; save away the lcid pointer
	xor	bh,     bh                  ; ensure pointer clean

;------------------------------ Get State Value ------------------------------
;
; bl    = ????
; bh    = 0
; edx   = [scan code] + 1 for bit location + 0 for current state bits
; fs:di = pKeyState
;
NextState:
        lodsb                       ; get the next state key the lcid uses
        mov     bl,     al
        lodsb                       ; get the toggle state
        mov     ah,     al
        mov     al,     fs:[bx+di]  ; get the key state
        test    al,     ah          ; see if toggled
        jz      @F
        or      dl,     dh          ; add the state into the list
@@:
        shl     dh,     1           ; move the state up one
        loop    NextState

        ror     ebx,    16          ; get back the lcid pointer

;--------------------------- Check Special States ----------------------------
;
; CAPSLOCK  uses ax, cx, si
;
;  bx    = lcid pointer
;  dl    = state flags
;  fs:di = KeyState pointer
;
KbdFlaggedStates:
	public KbdFlaggedStates

	mov	si, [bx+CAPSTABLE]	; si points to bit table
	mov	ax, VirtKey			; ax is vkey
	mov	cx, ax		; save it!
	xor	ah, ah		; Vkey is only a byte (protect the shift)
	shr	al, 3		; gives byte index into table
	add	si, ax		; [si] now points to byte with bit
;
	and	cx, 7		; make vkey 0..7 index
	mov	al, 1		; initial bit of mask
	shl	al, cl		; al is bit mask
;
	mov	cl, [si]	; get byte with bit for this Vkey
	and	al, cl		; al now has bit for this vkey only
	setnz	al		; al now 1 if bit is set
;
; If CAPS bit or CAPSNORMAL = 0 then CAPS ignored, and state depends on SHIFT
;
; else CAPSLOCK gives shifted state and CAPSLOCK+SHIFT gives normal state for
;      those VKEYS which have associated CAPS bit set.
;
	mov	ch, fs:[di+VK_CAPITAL]	; get CAPSLOCK state

	and	al, ch			; only 1 iff bit set AND capslock on
	shl	al, 1			; now bit one ==> shift in table
	and	al, byte ptr [bx+LCID_FLAGS]    ; gives us caps_normal
	xor	dl, al			; if shift OR capslock, shift set (never both)
;
;SHIFTLOCK:  -  overrides CAPSNORMAL if SHIFTLOCKUSED. The main difference
;   with shiftLock is that more keys are considered to be in the shifted
;   state (e.g. top row of keys and punctuation keys). CAPSNORMAL is set to
;   indicate that the caps bitmap table exists and is not needed here.
;   CapsLock (really ShiftLock) sets the shift bit to gain access to this
;   larger set of keys.
;
; Note, however, that later-on the NumPad keys look specifically for the
; state of the Shift key itself, if the NumLock state = ON, and if the shift
; is not been pressed then numbers are the result. This is the one exception
; to the general rule that ShiftLock causes all keys to be shifted. This
; problem is not treated here
;
;  uses ax, cx
;
;  bx    = lcid pointer
;  dl    = state flags
;  fs:di = KeyState pointer
;
	shl	ch, 1		; now bit 1 (2nd bit) shift position
	mov	al, dl		; al is flags
	and	al, 5		; alt or ctrl masks
	neg	al			; reverse
	sbb	al, al		; set flag
	not	al			; zero if no alt or ctrl

	mov	ah, byte ptr [bx+LCID_FLAGS]	; ah has flags
	and	al, ah			; mask in existing flags.
	and	al, SHIFTLOCKUSED	; flag only set if no alt or ctrl

	shr	al, 5			; al has shiftlock in shift state
	and	al, ch			; if CAPS or SHIFT state active
	or	dl, al			; mask in VK_SHIFT
;
;----------------------------- NUMLOCK exception -----------------------------
;
;This exception applies only to the ShiftLocked keyboard.
;
	test	ah, SHIFTLOCKUSED	;were we dealing with a shiftlocked kbd?
    jnz	doNumLock	;yes, do the NumPad exception

public ShtLockRtn
ShtLockRtn:		;return to here after exception processing.
;
;----------------------------- Find Table Offset -----------------------------
;
; dl = state
;
; uses   di = si = ptr to list of available states
;       eax = saves virtual key
;        cx = virtual key
;
	xor	dh, dh                  ; make dx = shift state
	mov	KS.dksState, dl			; save dx
	mov	di, [bx+TOASC_STATES]	;
	mov	ax, VirtKey				;
	mov	cx, ax                  ; get virtkey
	ror	eax, 16             ; save vkey for later
	mov	si, di				;
	or	dl, dl              ; is there a state at all?
	jz	GotState            ; no, so no need to check numpad, no alt key
                            ; also no need to scasb at notnumpad
;
;------------------------------- Special Checks ------------------------------
;
; firstly check for Ctrl/Ctrl-Shift and Alt/Alt-shift, and if they
; exist special case for either control characters, or menu
; processing or numpad processing. Note that NotNumPad will also process
; numpad keys where the ALT key is not down. See Numpad number keys when
; CapsLock=ON above.
;
; HACKEROONEE - optimise this crap!
;
	mov	al, dl				;
	and	al, 0fdH			; mask out shift/caps state
	cmp	al, 4				; control state alone?
	je	NotNumPad			; yes, let layout take care of
							; control first.
TryAltStuff:

	test	dl, 0fcH		; if anything else is down, not numpad
	jnz		NotNumPad		; ... i.e. AltGr (Alt+Ctrl)

	test	dl, 1			; test for alt present
	jz		NotNumPad		; no alt key
	;
	; we have alt or alt+shift, see if we are in menu mode.
	;
	test  by KeyFlags, 1	;
	jnz   NotNumPad			; menu bit set, not numpad

;=============================================================
;
; NUMPAD processing - with the ALT key down (not menu case)
; ---------------------------------------------------------
;
        rol     edi, 16              	; save it in case not valid key
        ror     edx, 16              	; dx is scancode again, edx is state
        mov     ax, dx              	; ax = dx = scancode
        lea     di, NumPad
        mov     cx, 10
        repnz   scasb
        jnz     NotValidNumpad

        xchg    dl, fLastScan       	; dl = last scancode
        cmp     dl, al              	; if last == ScanCode ...
        je      NumpadDone              ; key repeating, dont process

        test    by KeyFlags, 2       	; is enhanced set ?
        jnz     ToAsc0                  ; yes, dont want it.

        sub     di, dataOFFSET NumPad
        dec     di                      ; di is now the value for the key

        mov     dx, di      	; dx = new value
        les     di, pState  	; state[0] is value
        mov     ax, es:[di]
        mov     cx, ax
        shl     ax, 3
        shl     cx, 1
        add     ax, cx      	; ax = 10 * old value
        add     ax, dx      	; ax is new value
        xor     ah, ah
        stosw       		; save value
        or      ax, ax
        jne     SetAnsiNum              ; if 1st digit == 0, ax = 0
        mov     fAnsi,          1

ToBadAsc:
SetAnsiNum:
NumpadDone:
        xor   ax,      ax
        jmp   Done				;exit

AltDown:
        mov   fAnsi,   0		; wait for more key strokes
        jmp   ToAsc00			; return zero and exit

        ;
        ; key was NOT valid, clear alt bit, and try processing as normal
        ;
NotValidNumpad:
        ror     edx,            16      ; edx is scancode again, dx is state
        ror     edi,            16      ; get back the state table
;;;;    and     dl,             0feH	; kill the ALT state
        jz      GotState                ; state == 0, OK

;=============================================================
;
; normal - nonnumpad - processing:
; --------------------------------
; Notice however, that numpad keys are processed through here
; when the ALT key has not been pressed. (e.g. for shift states)
; See Shiftlocked/CapsLock processing above on numpad number keys, where
; the shiftlockused kbd exception is trapped before it gets to here.
;
NotNumPad:
public NotNumPad

        mov     al,     dl              ; al = dl = the state value
        mov     cx,     [bx+NSTATES]	;
        repnz   scasb
        jnz     BaseToAscii		; no state in layout, try
								; driver's char tables

;=============================================================
;Except for deadkeys and ligatures the special processing is
;mostly done.
;
GotState:
        ;
        ; di is now the place where we left off looking for a state. I.e. it
        ; is the state table pointer. (di - si = state table to use).
        ;
        sub     di, si                      	; DONT sub 2, offset correct

        mov     si, [bx+VKEY_LIST_LENS]     	; still have byte, get count
        add     si, di                      	; --------------------------
        mov     cl, [si]
        xor     ch, ch                      	; cx is now the scanlist len

        shl     di, 1                       	; was byte offs, now word
                                            	; -----------------------
        mov     dx, [bx+STATETABLES]        	; dx is offset of state lists
        add     dx, di                      	; dx points to correct list
        mov     ax, [bx+VKEY_LISTS]         	; ax points to scan lists
        add     di, ax                      	; di points to correct list
        mov     di, [di]                    	; di holds the correct list
        mov     si, di                      	; si also holds list start
        ror     eax, 16                     	; ax is virtkey again

        ;
        ; got the state values
        ;
        ; ax = VKey
        ; bx = basic pointer
        ; cx = len of correct ScanToIdx list
        ; dx = pointer to correct state list
        ; si = offset of correct ScanToIdx list
        ; di = offset of correct ScanToIdx list
        ;
        repnz scasb		;find a VKEY match
        jnz   BaseToAscii        ; vkey does not convert to a char
        dec   di
        sub   di, si             ; di is now offset from table start
        mov   si, dx
        mov   si, [si]           ; si is now the table address
        add   si, di             ; si is now the character

        xor   dx, dx             ; assume failure
        xor   ah, ah
        lodsb                    ; get the character
        or    ax, ax             ; it was a blank, i.e. this one doesn't
        jz    BaseToAscii        ; exist! Try the base list

;-----------------------------------------------------------------------------
; DEADKEY Processing:
; -------------------
;
; if the previous character was a deadkey, see if this translates
;
        or    ah, fDeadKey    ; ah was zero, if still zero, no dead key
        je    PrevNotDead     ; no previous deadkey detected
        ;
        ; previous character was a dead key, need to find it's location. As
        ; we are checking against an internal API, the offset is
        ;
        mov   	fDeadKey, 0  	   ; make sure next is dead
        xchg  	al, ah             ; al is the deadkey, works both ways
        mov   	cx, [bx+NUM_DEAD]
        jcxz	BadPrevDead

        mov	di, [bx+DEAD_KEYTRANS]
        mov	cx, [di]           ; cx holds the number of conversions
        inc	di
        inc	di
        mov	dx, di             ; dx,di is start of list
        mov	si, di             ; si is start of table
        add	si, cx             ; si points to start ...
        add	si, cx             ; ... of conversions (word offs)
        repnz	scasw          ; look for a accent/char match
        jnz	BadPrevDead1       ; didn't find it, return two chars

        sub	di, dx             ; di is offset
        shr	di, 1              ; make byte offs
        dec	di                 ; zero based
        add	si, di
        lodsb                  ; get the character
        jmp	NotSpecial
;
; Make the double deadkey case visible - output both characters
; This takes care of double bouncing keys.
;
BadPrevDead1:
;;	cmp	al, ah			; hit deadkey twice?
;;	je	ToBadAsc		; yes, return nothing

BadPrevDead:
	public BadPrevDead

	les	di, pState		; save them here
	stosb
	xor	al, al
	stosb
	shr	ax, 8	  		; al=ah, ah = 0
	mov	dx, 2			; returning two characters
	jmp   StoreChars1
;
;-----------------------------------------------------------------------------
; Check the DEADCOMBOS flag and search one of two styles of deadkey table
;
PrevNotDead:
	mov	cx, [bx+NUM_DEAD]
	jcxz	TryLigatures
	mov	di, [bx+DEAD_KEYS]
	test	[bx+LCID_FLAGS], DEADCOMBOS  ; new style deadkey list?
	jnz	NotNormalDeadKeys                ; yes, go do new style

	repnz	scasb                        ; try find deadkey in list
	jmp	@F                               ; go check if we found it

NotNormalDeadKeys:
	;
	; we need to load up the char (we have in al), the vkey, and the
	; state so that a DWord search can be performed.
	;
	mov	edx, KS
	mov	dl, al			; edx = [char | Vkey | state | 0]
	mov	eax, edx

	repnz	scasd		; did we find it?
@@:
	jnz	TryLigatures	; no, go try ligatures
;
; Yes, we found a deadkey
;
        mov   	dx, -1			; yes, mark it.
        mov   	fDeadKey,   al  ; store the new deadkey
        xor   	ah, ah
        jmp   	StoreChars		;store, then exit

;-----------------------------------------------------------------------------
;
TryLigatures:
	public TryLigatures

	xor	cx, cx			; use ch as zero
	mov	si, [bx+LIG_KEYS]
	or	si, si
	jz	NotSpecial	; al was the char we found
	mov	edx, KS
	mov	dl, al		; edx=[0|state|vkey|char]

TryNextLig:
	lodsd			; eax is next candidate
	or	ax, ax
	jz	NoLigFound
	cmp	eax, edx
	je	LigFound
	mov	cl, [si]	; fetch extra count
	add	si, cx		; skip over (byte)
	add	si, cx		; skip over (word)
	inc	si			; skip counter
	jmp	TryNextLig	; and get next lig.

LigFound:
	les	di, pState
	xor	ah, ah
	stosw			; store the original char
	lodsb			; get the counter
	mov	dl, al		; for return value
	inc	dl			; include the first char
	xor	dh, dh		; counter is a word
	mov	cl, al		; cx is count of remaining
	rep	movsw		; move the bytes over
	jmp	Done1		; outa here!

NoLigFound:
	mov	al, dl			; get char back
	jmp	NotSpecial

;-----------------------------------------------------------------------------
;
; A space was typed immediately following a deadkey - make the deadkey visible.
;
SpaceDead:
	public SpaceDead

	xor	al, al				; clear fDeadKey and return
	xchg	al, fDeadKey	; previous one
	jmp	NotSpecial

;-----------------------------------------------------------------------------
;  Note that numpad number keys come through here for CAPSNORMAL kbds. For
;  SHIFTLOCKED kbds they are specially processed much earlier above. The rule is
;  that the shift state is removed for numpad number keys (shift key not down).
;
BaseToAscii:
	public baseToAscii
        ;***
        ;*** didn't find the character in the lcid tables, try it in the
        ;*** drivers global list
        ;***
        ;*** BX IS DESTROYED HERE
        ;***

	mov	al, byte ptr VirtKey	; special case, deadchar & ' '

	cmp	al, ' '
	sete	cl			; 1 iff char==' '
	mov	ch, fDeadKey
	neg	ch
	sbb	ch, ch			; deadkey presence gives -1
	and	cl, ch			; non-zero iff both true
	jnz	SpaceDead		; go make eadkey visible

	mov	di, word ptr KS.dksState	; zero in high byte!

	and	di, 7			; we assume specials, such as
;;;;;	cmp	di, 8		; capslock have no effect on
;;;;;					; base keys
;;;;;	jae	ToAsc00		; dont recognise it

	add	di, di			; word offset
	mov	dx, [di].BaseAnsiTables
	mov	cx, [di].BaseLengths
	mov	di, [di].BaseVKeyTables
	mov	bx, di				; dont need bx anymore

	repnz	scasb		; do we find it?
	jnz	ToAsc00			; nope! Just do lights.

	sub	di, bx			;
	add	di, dx			; di now points to ansi
	mov	al, [di].-1

 	cmp	fDeadkey,0	; deadkey active?
	jne	BkSpDead        ; yes, make sure no BK SP

    .errnz $-NotSpecial
;-----------------------------------------------------------------------------
;
;  Various Exit cases:
;  -------------------
;
NotSpecial:
	xor	ah, ah		; ensure all clear
        mov	dx, 1           ; return value

StoreChars:
        les	di, pState      ; save them here

StoreChars1:
        stosw
Done1:
        mov	ax, dx           ; we're done, 1/0 character(s)
Done:
        pop     edi
        pop     esi
        pop     fs
        pop     ds
	cEnd
;
; Backspace while deadkey flag is set
;
BkSpDead:
public BkSpDead
	cmp	al,8		; is it backspace?
	jne	NotSpecial  	; no, do nothing
	xor	al,al		; yes, return no char
	mov	fDeadkey,0 	;      and kill deadkey
	jmp     NotSpecial      ;

;------------------------  CapsLock/ShiftLock Anomolies --------------------
;
; The CapsLock state is a pseudo shift state in general - however, it should
; not affect the NUMPAD number keys on any type of keyboard, so when NumLock=On
; the number keys remain unaffected. For non-ShiftLocked kbds this is not a
; problem because the CAPS bitmap holds the shift state OFF. For shiftlocked
; kbds the shift bit must be knocked down if NUMLOCK=ON and SHIFT KEY = UP
;
;NUMLOCK:
;
;  bx    = lcid pointer
;  dl    = state flags
;  fs:di = KeyState pointer
;
; uses  di = si = ptr to list of available states
;      eax = saves virtual key
;       cx = virtual key
;
Public DoNumLock
DoNumLock:

	mov	 al, fs:[di+VK_NUMLOCK]	;get NUMLOCK state - is it off?
	or	 al, al			;dumb mov instruction needs this
	jz	 ShtLockRtn		;yes, dont care about NUMPAD numbers
;
; Treat the shift state exception here: if the shift key is not down then
; do explicit check for NUMPAD number keys.
;
;; mov   al, fs:[di+VK_SHIFT]  ;get SHIFT key state, is it down?
;; or    al, al			;dumb mov instruction needs this
;; jnz   ShtLockRtn		;yes, dont care about numbers

	mov	 ax, VirtKey	;get VKey
	cmp	 al, VK_NUMPAD0	;
	setae	cl			;--> 0 iff too small
	cmp	 al, VK_NUMPAD9	;if a NUMPAD number key set flag
	seta	ch			;--> 1 iff too large
	xor	 cl, ch		;limit the range
	shl	 cl, 1		;make like shift state bit
	not	 cl			;convert to mask
	and	 dl, cl		;remove shift state if true
					;emulates the caps bitmap table
	jmp	ShtLockrtn

;*****************************************************************************
;********************************** dolights *********************************
;*****************************************************************************

; This routine sets ROM BIOS's RAM flags for shift state, then does
; what is needed to make the lights, if any, correspond to the flags.
;
; The BIOS CapsLock and ScrollLock flags are synchronized with the KeyState
; vector here.   The BIOS NumLock flag is sync'd in the interrupt routine
; when the key is pressed, and also in ToAscii() if either (a) the specal
; call from SetKeyboardState() is made, or (b) if an app sends a WM_KEYDOWN
; message with VK_NUMLOCK, with the extended bit in KeyFlags off.


DoLights proc near

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

public SetLightHardware			; called from Enable()
SetLightHardware:

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

END		; of ToAscii.asm

