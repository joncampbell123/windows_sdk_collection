	page	,132

;*** TRAP.ASM for Microsoft Windows 3.00  keyboard drivers ******************
;
;	Copyright (C) 1984-1990 Microsoft Corporation.  All Rights Reserved.
;
;	Copyright 1985-1987 by Ing. C. Olivetti & Co, SPA.
;
;	This contains the hardware interrupt handler (keybd_int)
;	for the Windows keyboard driver.
;
;	NOTE: BIGSTK may or may not be defined here; see below.
;
;***********************************************************
;	Conditional-assembly flags:
;
;	Only ONE of the following should be set in the command line!
;
;	ENHANCE	handles 'RT' 101/102-key enhanced keyboard.
;
;	ICO	handles Olivetti M24 keyboard lights and Olivetti M24
;			'ICO' 102-key keyboard features, also 101/102
;			Enhanced keyboard, and AT&T 301 & 302 keyboards.
;
;	ENVOY	Handles Hewlett-Packard keyboards, including Envoy and
;		enhanced.
;
;	NOKIA	NOKIA/Ericsson 1050, 9140, etc., keyboards.
;	
;***********************************************************
; History Windows 3.00
; 
; 16 apr 90	peterbe		4 lines before CheckDel:, changed jump target
;				from kbToBios to kbi1 to REALLY fix HP bug
;				related to CtrlAlt+char.
;
; 13 apr 90	peterbe		In SetShiftState(), unscrambled and fixed
;				code for HP/Nokia in ifdef's, to fix KBDHP
;				driver's handling of Ctrl+Alt+char.
;
; 02 feb 90	davidw		Cleaning up ctrl-brk, and redoing
;				Ctrl-Alt-SysReq
;
; 14 jan 90	davidw		Re-enabling ctrl-brk
;
; 21 nov 89	peterbe		Changed PIN's and POUT's back to in's and out's
;
; 09 nov 89	peterbe		(for HP): call KbdRst just before CheckDel
;
; 03 oct 89	peterbe		Remove INT 3 put in by hp
;
; 31 oct 89	peterbe		(1.48!) only test for control at ccNum1
;				This fixed bug # 5775
;
; 15 oct 89	peterbe		Integrated HP changes.  DosXMacro defined
;				as null in 'ifndef'.
;
; 13 oct 89	peterbe		Change handling of NumLock+SHIFT+numpad
;				cursor key to optimize repeated key, stop
;				wierd beeping.  (second update: fix handling
;				of cursor key release).
;
; 18 sep 89	peterbe		SysReq code now tests fSysReq instead of
;				looking at seg of NMI vector to enable
;				trap to debugger.
;
; 07 sep 89	peterbe		Removed CtlBrkCase(), INT1BHandler().
;				Ctl-break -> VK_CANCEL, ASCII 3 (ctl-C).
;
; 31 aug 89	peterbe		Fixed (type 4) Pause problem: toggled
;				numlock: added pause_proc(), and test
;				for shifts at ccNum1:
;
; 24 aug 89	peterbe		SetShiftState handles WORD at 40:17, so
;				Ctrl & Alt work on some machines.
;
; 23 aug 89	peterbe		Use fExtend (former IcoExtend) to flag
;				VK_NUMLOCK msgs. which actually come from kbd.
;				Then ToAscii() can handle these messages
;				slightly differently (for NCR problem).
;
; 22 aug 89	peterbe		Added NumLockFlag handling: to solve NCR
;				keyboard problem.  See SetShiftState().
;
; 03 aug 89	peterbe		Add STI before IRET.
;
; 16 jul 89	peterbe		In FakeCapsLock(), load CapsLock scan code.
;				(For German, etc. keyboards.)
;				Changed jne kbToBios to jne kbi1 before
;				kbToBios so CTRL-ALT keys will work.
;
; 15 jul 89	peterbe		Ifdef out some function key code no longer
;				needed since Winoldap doesn't have to trans.
;				function keys any more.
;
; 14 jul 89	peterbe		Ignore E1 prefix from Enhanced Pause key.
;				No longer pass control-numlock to BIOS for
;				Pause!!  (will still go to BIOS if fHold is
;				set, so this can be cleared).
;
; 11 may 89	peterbe		'Extended' keys on ICO (2) keyboard pass
;				flag in BH to event_proc now, like type 4.
; 20 apr 89	peterbe		Removed old comments above kbic: about
;				Business Network and W.1.00 keyboard test.
; 07 apr 89	peterbe		No longer check screen mode for graphics.
;				Makes life easier for display driver coders.
; 14 mar 89	peterbe		Revised printscreen code again.  ALT-print is
;				now current window snap.  unshifted-print is
;				full screen snap for Enhanced and ICO kbd's.
;				SHIFT-PRINT is full screen for XT, AT.
; 12 mar 89	peterbe		Rewrote printscreen/snapshot code for detecting
;				key combinations.  ALT-PrintScn is now
;				full-screen snapshot, while shift-PrintScn is
;				current-window snap.  Errors corrected.
;
; 14 dec 88	peterbe		Remove prtsc_event.  Printscreen now sends
;				special VK_SNAPSHOT.
;
; 01 dec 88	davidw		Made it bi-modal.
;
; 29 nov 88	peterbe		Added prtsc_event, and kbCheckCapture code.
;				Printscreen causes prtsc_event() to be called.
;
; 02 nov 88	peterbe		SysReq key ignored if fOS2Box is set, due
;				to conflict in keyboard usage.
;
; 23 sep 88	peterbe		Removed VK_PREFIX stuff, set bit 0 of BH
;				when calling event_proc to indicate extended
;				key on enhanced keyboard.
;				(made sure BH = 0 otherwise!)
;
; 22 sep 88	peterbe		Changed scan code for VK_PREFIX to 60h.
;				Define VK_PREFIX value here until it's
;				finalized.
;
; 21 sep 88	peterbe		Adding VK_PREFIX output when extended key is
;				typed. (if VK_PREFIX is defined).
;
; 08 sep 88	peterbe		Fixing extra WM_KEYUP in VK_ICO_00 code.
;
; 26 aug 88	peterbe		Put in some comments regarding AT keyboard
;				enable.
;
; 23 aug 88	peterbe		Removed an ifdef at notkbi1:
;
; 22 aug 88	peterbe		Fixing NMI trap (SysReq) code. Enable() must
;				now set up nmi_vector, which MUST be in the
;				CODE segment.
;				Moved LightsAddr,enabled,bios1b_proc to Enable.
;				bios_proc MUST be in CODE segment also
;
; 19 aug 88	peterbe		MAJOR CHANGES: Renamed to TRAP.ASM (was datacom)
;				Moved code to CODE segment, radical change
;				in usage of segment registers.
;				ES -> RAMBIOS
;				Moved 'include date.inc' to INIT segment.
;				Removed inquireData.
;
; 18 aug 88	peterbe		Changed some exclusion ifdefs, removed old
;				debug stuff, fixed comments and added some FF's
;				Added alternate jz for ENVOY at notkbi1Env:
;				At kbi12:, set DS to 40H.
;				At sysreqdown:, set DS to 0.
;
; 17 aug 88	peterbe		Checking NOKIA ifdefs.
;
; 16 aug 88	peterbe		Set ENHANCE if ICO is set. Olivetti driver
;				always handles compatible 101-102 keyboard now.
;				Fixed printscreen code accordingly.
;
; 15 aug 88	peterbe		Changed SysReq code to handle ICO keyboard.
;
; 14 aug 88	peterbe		Moved keyTrTab to TABS.ASM file.
;
; 12 aug 88	peterbe		Changing to keep DS=40h, rename RAM BIOS
;				variables, use RAMBIOS def.
;				Moved fCaps etc. defs to keyboard.inc
;
; 11 aug 88	peterbe		Add page directive at beginning.
;				Updated code in ENHANCE ifdefs to check
;				KeyType, where necessary.
;				Code in ICO ifdefs checks whether this is
;				a 102-key ICO keyboard.
;
; 08 aug 88	peterbe		Init. KeyType to 0 now.  PCType, KeyType,
;				etc. moved to TABS.ASM.
;
; 02 aug 88	peterbe		SYSREQ code turns off Control and Alt
;				before doing NMI.
;
; 27 jul 88	peterbe		Renamed keyTranslationTable to keyTrTab
;
; 26 jul 88	peterbe		Moved version string to TABS.ASM.
;
; 15 jul 88	peterbe		Add lots of ifdefs for different keyboard
;				types.  Now 1 driver per keyboard type!
;				keyTrTab moved into this file.
;				All the OEM-specific extensions to this
;				table are here.
;
; ==== Windows 2.10 ====================================
; 27 jun 88	peterbe	Now use fCaps flag in BIOS 40:17h instead of fShiftLock
; 23 jun 88	peterbe	Changed CMP fVectra... to TEST fVectra (after kbi4:)
; 21 jun 88	peterbe	Some ShiftLock code factored out as 'FakeCapsLock'
; 20 jun 88	peterbe	Experimental implementation of ShiftLock.
; 15 jun 88	peterbe	moved includes to before any ifdefs
; 14 jun 88	peterbe	Added hp enhancements -- BIGSTK, ENVOY ifdefs
;
; ========================================================================

; ========================================================================
; Check for mutually incompatible ifdefs

ifdef ENHANCE
    TYPEERRORFLAG equ 1
endif; ENHANCE

ifdef ICO
    TYPEERRORFLAG equ 1
    ; also handle enhanced keyboard
    ENHANCE = 1
endif ; ICO

ifdef NOKIA
    TYPEERRORFLAG equ 2
endif ; NOKIA

ifdef ENVOY
    TYPEERRORFLAG equ 1
    ifdef ICO
	TYPEERRORFLAG equ 3
    endif
    ; also handle enhanced keyboard
    ENHANCE = 1
endif ; ENVOY

; ========================================================================
; display which option(s) we've chosen.

if1

%out
%out	.... TRAP.ASM -- Windows keyboard INT 09H ....
%out	.... Keyboards handled besides XT ....

    ifdef NOKIA
	%out .  NOKIA 1050 and 9140 version
    endif

    ifdef ICO
	%out .  Olivetti M24 83 & 102-key (ICO) version
	%out .  AT&T 301, 302 keyboards on 6300, 6300+.
    endif

    ifdef ENVOY
	%out .  Hewlett-Packard Vectra & Envoy version
    endif

    ifdef ENHANCE
	%out .  Handles Enhanced, XT, AT keyboards
    endif


endif	; if1

; ========================================================================

include keyboard.inc
include vkwin.inc
include vkoem.inc

ifndef DosXMacro
    DosXMacro macro x, y

    endm
endif

; Allocate big stack when calling
; BIGSTK equ 0


SYSREQ	=	054h	; PC-AT SYS REQ key .. not on M24 or ICO keyboard
			; (on Enhanced keyboard, is PrintScn/SysReq)

if1
    ifdef SYSREQ
	%out .. Has SysReq handling
    else
	%out .. Does NOT have SysReq handling
    endif
endif


; scan codes for special keys

cEsc		EQU	 1	;
cReturn		EQU	28

cCtrl		EQU	29
cLShift		EQU	42
cSlash		EQU	53	; 35h
cRShift		EQU	54	; 36h
cPrint		EQU	55	; 37h IBM keyboard printscreen
cAlt		EQU	56	; 38h
cCapsLock	EQU	58	; 3ah
cF1		equ	59	; 3bh F1 key on anything
cF7		equ	65	; 41h F7 key on anything
cNumLock	EQU	69	; 45h
cBreak		EQU	70	; 46h
cUp		equ	72	; 48h up key
cLeft		equ	75	; 4bh left key
cRight		equ	77	; 4dh right key
cDown		equ	80	; 50h down key
cDelete		EQU	83

cExtended	EQU	96	; E0h-80h - for RT extended codes - 13feb87
cExtended1	equ	97	; E1h-80h - prefix for Pause key

ifdef	NOKIA

cCommand	EQU	0FAH	; NOKIA

else	; NOKIA

cCommand	EQU	0F0H	; not NOKIA

endif	; NOKIA

ifdef ICO
				; Olivetti only:
cIcoPrint	equ	85	; 55h ICO keyboard printscreen
cIcoDivide	equ	95	; Ico divide key
				; ICO extended funct.keys F11..F18 are 96..103
endif ; ICO

; **************** data segment begins here ***********************

sBegin DATA

; ********** System Type information ******************

    externB fKeyType		; flags mainly for RT keyboard.

; These things are updated at INIT or ENABLE time from the keyboard
; tables or from WIN.INI.

    externB	PCType		; identifies system type
    externB	PCTypeHigh
    externB	KeyType		; keyboard type (usually == TableType)
    externB	IsEri
    externB	IsOli
    externB	OliType

; Flag for change in BIOS NumLock flag.

    externB	NumLockFlag

; Table for translating scan codes to virtual keycodes.
; (in TABS.ASM)

externW	KeyTransBase	;  dataOffset keyTrTab
externW KeyTransTblSize	;  KeyTransTblEnd - keyTrTab 

; This is the translation for the numeric pad when NUMLOCK is set.
; This is the same for all keyboards, and is fixed in length.

	public	KeyNumTrans
KeyNumTrans	LABEL	BYTE
	DB	VK_NUMPAD7	; numpad-7
	DB	VK_NUMPAD8	; numpad-8
	DB	VK_NUMPAD9	; numpad-9
	DB	VK_SUBTRACT	; numpad-minus
	DB	VK_NUMPAD4	; numpad-4
	DB	VK_NUMPAD5	; numpad-5
	DB	VK_NUMPAD6	; numpad-6
	DB	VK_ADD		; numpad-plus
	DB	VK_NUMPAD1	; numpad-1
	DB	VK_NUMPAD2	; numpad-2
	DB	VK_NUMPAD3	; numpad-3
	DB	VK_NUMPAD0	; numpad-0
	DB	VK_DECIMAL	; numpad-period

	public	keyNumBase

keyNumBase	dw	dataOffset KeyNumTrans

; External from INIT.ASM
; Value of acknowledge byte
	extrn	AckByte:byte

; Kernel routine for extended memory reset
;
EXTRN	KbdRst:FAR

; Kernel routine for ctrl-brk handling
;
EXTRN	DoSignal:FAR

; Kernel routine for CVW handling
;
EXTRN	CVWBreak:FAR

; Flag for enabling SysReq key

    externB fSysReq

; *********************** Local data *********************************
;
;
; Address of keyboard event-proc
;
; the keyboard event_proc is called with the following parameters:
;
;	ah == 80h for uptrans, 00h for downtrans
;	al == virtual key
;	bh == 0 if no prefix byte, 1 if E0 prefix byte preceded this scancode.
;	bl == scan code

	even

public  event_proc

event_proc  DD	0   ; Address of enabled keyboard event procedure

ifdef BIGSTK
    if1
	%out HAS BIG LOCAL STACK
    endif
;
; Private keyboard stack (HP c-ralphp 6/9/88) see comments in keybd_int
;
staticB	, ?, 384
globalW	Stack, ?
staticW	SaveSS,	0
staticW	SaveSP,	0
staticW	NextSS,	0

; end (HP c-ralphp modifications)
else
    if1
	%out .. DOES NOT HAVE BIG LOCAL STACK
    endif
endif ; BIGSTK

; (MS code has ShiftLockException table here)

fBreak	    DB	0
fReEnter    DB	0	; reentrancy flag

fAltGrDn    db	0	; set if AltGr is down -- for preventing repeats.

LastCursor  db  0	; last NUMERIC PAD cursor scancode read with NumLock
			; and SHIFT on.

ifdef	ENVOY

;-------------------------------------------------------------
;
; Envoy data area
;
include	equate.inc	; HP-System equates

; Envoy keyboard equates
;
cCCP_UP	    EQU	60h
cf8	    EQU	77h
cPlus	    EQU	4Eh
cMinus	    EQU	4Ah
cCCPDel	    EQU	69h
cBackSlash  EQU	2Bh

; Flags for Hewlett-Packard
;
staticB	 fModifier,0	; -1 if modifier. Set by SetShiftState
FV_A		= 001b
FV_ENVOY	= 100b
externB	 fVectra, 0	; 1 if Vectra. Set by Enable


; This table is used to translate the function keys and cursor pad
; keys. This is now neccesary because they are now placed in a raw
; mode.
;
HPTransTable	label	byte
	DB	VK_UP			; CCP up arrow
	DB	VK_LEFT			; CCP left arrow
	DB	VK_DOWN			; CCP down arrow
	DB	VK_RIGHT		; CCP right arrow
	DB	VK_HOME			; CCP home
	DB	VK_PRIOR		; CCP PgUp
	DB	VK_END			; CCP end
	DB	VK_NEXT			; CCP PgDn
	DB	VK_INSERT		; CCP Ins
	DB	VK_DELETE		; CCP Del
	DB	VK_CLEAR		; CCP Cntr
	DB	5 dup (-1)
	DB	VK_F1			; f1
	DB	VK_F2			; f2
	DB	VK_F3			; f3
	DB	VK_F4			; f4
	DB	VK_F5			; f5
	DB	VK_F6			; f6
	DB	VK_F7			; f7
	DB	VK_F8			; f8


; Reset Vector used for soft resets
;
ResetVector	dd	0ffff0000h					;9/3/86


;~~ VVR 092789
; debugging information

SOFT	db	"Softkey pressed",0,13,10
Mod1	db	"Modifier is -1",0, 13, 10

Mod0	db	"Modifier is 0", 0, 13, 10
endif	;ENVOY

; flags for running in OS/2 Compatibility box

fIgnoreKey	db	0 ; Set to ignore keys used for screen switch
fIgnoreEsc	db	0 ; Count of up keys to ignore

	public fSwitchEnable	; accessed in ENABLE.ASM

fSwitchEnable	db	1 ; Flag to prevent/allow screen switches


; Address of routine called to handle a scan code.
; For non-RT keyboards, this never changes -- it's always 'standard_proc'.
; For RT keyboards, this changes when a prefix code is seen

staticW	kbd_proc,standard_proc

ifdef ICO	; Olivetti M24 102-key keyboard

; Table to translate ICO scan codes in range cIcoPrint..cIcoDivide
; .. so that WINOLDAP will recognize these..

IcoTransT label byte
	db	cPrint			; 55h PrintScreen
	db	56h			; 56h Help
	db	cReturn			; 57h Return
	db	cLeft			; 58h Left cursor
	db	cDown			; 59h Down cursor
	db	cRight			; 5ah Right cursor
	db	cUp			; 5bh Up cursor
	db	5ch			; 5ch Clear
	db	cBreak			; 5dh Break
	db	5eh			; -- nothing --
	db	cSlash			; 5fh Divide key

endif ; ICO

fExtend	db	0		; flag for above keys.

globalD bios_proc, 0
globalD nmi_vector, 0

sEnd DATA

;***********************************************************
;	Some pointer variables in the CODE segment.
;	These are initialized in ENABLE, using
;	a DATA alias to the CODE segment, so that
;	they can be written in protect mode.
;

sBegin CODE				; CODE segment now.

    assumes	CS,CODE
    assumes	DS,DATA

    hack_for_old_cvw	    dw	offset CVWHackCall
			    dw	seg CVWHackCall

cProc	CVWHackCall, <FAR,PUBLIC>
cBegin <nogen>

    push    ds				; save DS
    push    ax				; ... AX
    mov     ax, _DATA			; reload DS
    mov     ds, ax
    assumes ds, data
    pop     ax				; restore AX
    push    word ptr [bios_proc][2]
    push    word ptr [bios_proc][0]	; put address of bios proc on stack
    push    bp
    mov     bp, sp
    mov     ds, [bp+6]			; restore ds
    pop     bp
    retf    2				; "return" popping ds
					; affecting a jump to bios
cEnd   <nogen>

cProc	GetBIOSKeyProc, <FAR, PUBLIC>
cBegin
    mov     ax, word ptr [bios_proc][0]
    mov     dx, word ptr [bios_proc][2]
cEnd

    assumes	DS,nothing


;***********************************************************
;
;--- keyboard hardware service -----------------------
;
;	Usage of segment registers:
;
;	Currently:		In 2.10 and earlier drivers:
;
;	CS = CODE		(was DATA)
;	DS = DATA		(was BIOS data area at 40h)
;	ES = RAMBIOS (40h)	(wasn't used)
;
;***********************************************************

public keybd_int
keybd_int   PROC    FAR


	push	ds
	push	ax			; save registers we use
	push	es

	mov	ax, _DATA		; What Cmacros does to 'DATA'
	mov	ds,ax			;  .. load DS
	mov	ax, RAMBIOS		; BIOS RAM segment at 40h
	mov	es,ax			;  .. point ES to that!
    assumes DS, DATA
    assumes ES, RAMBIOS

	in	al,kb_data		; get the scan code
	mov	ah,es:[kb_flag]		; get current shift state..

	cmp	al,cCommand		; is it a keyboard command?
	jb	kbiTestCursor		; (note: cCommand is different
					;  for Ericsson!)
	jmp	kbToBios		; if so, let BIOS do it

kbiTestCursor:
	cmp	LastCursor, 0		; is flag (scancode) set for numpad
	jz	kbiTestUp		; cursor?
	cmp	al, LastCursor		; yes, is this the same scancode?
	jne	kbiTestCursorUp
	jmp	kbi1			; yes, so it's a MAKE of the same one
kbiTestCursorUp:			; no..
	push	ax			; it might be BREAK for same code
	and	al,7fh
	cmp	LastCursor,al
	pop	ax
	je	kbiTestUp		; if ==, it is
	push	ax			; but it's not, so we
	push	bx
	mov	ax, VK_SHIFT		; fake SHIFT DOWN
	mov	bx,54
	call	ds:[event_proc]
	pop	bx
	pop	ax
	mov	LastCursor, 0		; clear flag

kbiTestUp:

ifdef	NOKIA

	cmp	al,80H			; handle Nokia prefix byte
	jne	NotPrefix
	or	BYTE PTR es:[ProtocolFlag],$PrefixFlag
	jmp	kbToBios		; Let BIOS also get prefix
NotPrefix:
	jb	notkbi1			; was jbe, but never ==
	jmp	kbi1			; it's an UP transition..			
notkbi1:

else	; not NOKIA

	test	al,80h			; is it an up transition?
	jz	notkbi1			; .. if not, continue
	jmp	kbi1			; it's an UP transition, jump..
notkbi1:				; it's a DOWN transition..

endif	; not NOKIA

	; this will make BIOS clear fHold if it's set:
	test	byte ptr es:[kb_flag_1],fHold	; in hold state?
	jnz	jkbToBios		; if so, jump to ROM

	cmp     al,cCapsLock            ; is it CapsLock ?
	jnz     kbib                    ; no...

kbic:					; it's CapsLock --
        test    ah,fCtrl                ; is it Ctrl-NumLock or -CapsLock?
        jnz     jkbToBios               ; yes, jump to ROM
	jmp     kbi1			; no, go check hotkey
jkbToBios:
	jmp	kbToBios


; It's not capslock 
; [AH] = BIOS shift state, [AL] = scan code, 
; If it's Delete or Break, go check for Ctl-Alt


kbib:	cmp	al,cDelete
	je	kbia
	cmp	al,cBreak
	je	kbia


ifdef	ENVOY
; Look for CTRL-Alt + or - which on Vectra A, and A+ is handled in the
; int9 BIOS.
;
	test	ds:fVectra, FV_A ; If Vectra A,A+, Pass thru Cntl-Alt + and -
	jz	kbihp
	cmp	al,cPlus
	je	kbia
	cmp	al,cMinus
	je	kbia

kbihp:
	cmp	al,cBackSlash		; If Carrera, pass thru Cntl-Alt-\
	je	kbia			; for speed change


; The following code was added to test for the possiblity of a CCP CTRL-ALT-DEL
;
	test	ds:[fVectra], FV_ENVOY
	jz	no_reset
	cmp	al, cCCPDel
	je	kbia
no_reset:
endif ; ENVOY

; OLD Screen Print code here deleted 14 dec 88 .. see screen-capture code
; at KBI1:

; kbia: check for Control ALT something here
; Scan code is one of
;
;	cDelete		(Reset)
;	cBreak		(interrupt)
;	cPrint		(Screen Grab)
;	
;	cCCPDel 	- Vectra
;	cBackSlash	- Vectra
;	cMinus		- Vectra
;	cPlus		- Vectra
;
; 
kbia:
	mov	ah,es:[kb_flag]		; Get BIOS shift state (again)
	not	ah
	test	ah,fAlt+fCtrl		; test for CTRL-ALT something...
	jnz	kbi1			; nope, go to hotkey check


; Control and Alt are BOTH down now.  We're going to BIOS
; if its cDelete..

	cmp	al,cDelete
	jne	kbi1

; It's control-alt-Delete ..
; Inform the kernel that Ctl+Alt+Del is happening so that he can inform
; any expanded memory card to reset. (Wed 21-Oct-1987 : bobgu)

	call	KbdRst

kbToBios:

	pop	es
	pop	ax

	push	word ptr [bios_proc][2]
	push	word ptr [bios_proc][0]     ; put address of bios proc on stack
	push	bp
	mov	bp, sp
	mov	ds, [bp+6]		    ; restore ds
	assumes	DS,NOTHING
	pop	bp
	retf	2			    ; "return" popping ds

kbi1:

;
; Reset the keyboard controller and acknowledge the interrupt.
;

	assumes	DS,DATA

kbi13:

	push	ax			; this little bit is for XT-like systems
	in	al,kb_ctl		; reset interface chip (8255)
	mov	ah,al
	or	al,80h
	out	kb_ctl,al
	xchg	ah,al
	out	kb_ctl,al
	pop	ax			; just for delay..

	;; ??? ;;			; for ATs and PS2s, may need 
					;  to enable keyboard here!

	push	ax			; this is for XT or AT-like systems
	mov	al,ds:AckByte		; acknowledge interrupt
	out	ack_port,al
	pop	ax

	mov	ah,80h			; move the high order bit to the high
	and	ah,al			; order byte.
	xor	al,ah			; turn off bit if set

	push	bx			; stack = [AX,DS,ES,BX]

; Check for print-screen.
;
; For 3.0 Windows:
;	ALT-printscreen is grab of current window.
;	unshifted printscreen is grab of whole screen.
;	(shift-printscreen on XT, AT keyboards, however).
;
; If this is detected, event_proc is called with VK = VK_SNAPSHOT
; and BX = 0 for current window, and 1 for full screen grab.
;
; AL contains scan code (hi bit is 0), AH contains up/down flag.
;
	mov	bl,es:[kb_flag]		; get current shift state..
	test	bl,fCtrl		; CTRL down? if so,
	jnz	kbiNotPrint		; This is no printscreen...!

ifdef ICO
	; In the ICO keyboard, there's a special PrintScreen key with a
	; unique scan code.  If this is it, we do our thing on unshifted
	; print screen or ALT print screen, like an enhanced keyboard
	; We ignore the XT/AT '*' key.
	cmp	ds:[KeyType],2		; Olivetti ICO 102-key?
	jne	kbiNotIcoPrint
	cmp	al,cIcoPrint		; yes, test for special ICO Prn Scn
	jne	kbiNotPrint		; no, it's not a printscreen at all
	test	bl,fShift		; shift must NOT be down
	jz	kbiCheckIsAlt
	jmp	ignorekey
    kbiNotIcoPrint:
endif ; ICO

ifdef ENHANCE ; 101/102 enhanced?
	; this little bit is a check for ALT-PRINT (= ALT-SysReq) on
	; enhanced keyboard.  What a pain!  the PRINT key gives unprefixed
	; SYSRQ scancode if ALT is down, and we want ALT-PRINT for current
	; window grab!
	cmp	ds:[KeyType],4		; Enhanced?
	jne	kbiNoAltPrint
	cmp	al,SYSREQ		; SYSREQ scan code?
	jne	kbiNoAltPrint		; .. nope.
	test	bl,fAlt			; ALT down? (must be, but be patient)
	jz	kbiNoAltPrint
	mov	al,VK_SNAPSHOT		; this is really a screen grab
	mov	bx,0			;  0: current window
	jmp	short kbiPrintSend	; 
    kbiNoAltPrint:
endif
	cmp	al, cPrint		; PrtScn scancode (multiply key)?
	jne	kbiNotPrint		; (XT, AT, or Enhanced)

ifdef ENHANCE ; 'RT' 101/102 keyboard supported?
	cmp	ds:[KeyType],4		; Enhanced keyboard?
	jne	kbiIsPrintKey		; if not, skip the following;
					; if so, don't need shift..
					; and check/restore kbd_proc..
	cmp	ds:[kbd_proc], codeOFFSET prev_was_ext_proc
	jne	kbiNotPrint		; MUST have E0 prefix for CTRL PRINT
	mov	ds:[kbd_proc], codeOFFSET standard_proc
	
	; it is an enhanced-keyboard printscreen key, so..
	test	bl,fShift		; is SHIFT down?
	jnz	kbiGoIgnore		; if so, don't do it.
	mov	al,VK_SNAPSHOT
	mov	bx,1			; full-screen grab: bx == 1
	jmp	short kbiPrintSend

    kbiIsPrintKey:

endif ; ENHANCE
	; We have Printscreen scancode on non-enhanced keyboard.
	; We look for SHIFT or ALT, and snap all or part of the screen.
	; (unshifted PrintScreen is '*' character in this case)
kbiCheckPrint:
	test	bl,fShift+fAlt		; is SHIFT or ALT down?
	jz	kbiNotPrint
kbiCheckIsAlt:
	mov	al,VK_SNAPSHOT		; send special snapshot VK code.
	test	bl,fAlt			; is ALT down?
	mov	bx,1			; set BX, but don't change Z flag!!
	jz	kbiPrintSend		; ALT-PRINT has SysRq scancode, so skip
	dec	bx			; if ALT, clear BX

kbiPrintSend:
	call	ds:[event_proc]		; send VK_SNAPSHOT !!
kbiGoIgnore:
	jmp	ignorekey		; .. no more processing

kbiNotPrint:

ifdef	SYSREQ
;
;  The (PC-AT) SYS REQ key is used to simulate an NMI.
;
;  This is done by clearing up the stack and doing a far jump to the NMI
;  interrupt vector location.  If the NMI interrupt points into the ROM,
;  we don't jump, since the ROM treats NMIs as parity errors.
;
;  When SymDeb returns from the NMI, it returns to wherever CS:IP was
;  when the keyboard interrupt happened!
;
; Depending on the system and keyboard, SysReq requires that ALT or
; both Control and ALT be down, when the scan code 54h is input.
;
; On the Olivetti M24, this is the scan code for the 00 key.
; 
; On Enhanced (RT) and ICO (102-key Olivetti M24) keyboards, interpret
; Control-Alt-PrintScreen or Control-Alt-00 as Sys Req.
; We do a little arithmetic on the ifdef's here...
;
; Additional test, required if this is an Enhanced or ICO (2) keyboard:
; Must use CTRL ALT SCR-PRT on these keyboards, since Windows uses
; ALT SCR-PRT for screen grab.


	cmp	al,SYSREQ	    	; SYSREQ key?
	jne	notsys

ifdef ENHANCE
ifndef ICO

	; This handles Enhanced but NOT ICO keyboards:
	; For Enhanced keyboards, this is the printscreen key, so
	; we change the scan code if Ctrl is not down.

	cmp	ds:[KeyType], 4		; skip the following test,
	jne	sysNotEnh		;  if it's XT or AT keyboard.
					; it IS an Enhanced keyboard
	test	byte ptr es:[kb_flag], fCtrl	; is Ctrl down?
	mov	al, cPrint		; change scan code
	jz	notsys
sysNotEnh:

endif ; ifndef ICO
endif ;  CtrlAltSysRq -- ENHANCE or ICO

ifdef ICO
	; this handles either Enhanced or ICO keyboards.
	; For Enhanced keyboards, this is the printscreen key, so
	; we change the scan code if Ctrl is not down.

	cmp	ds:[KeyType], 4		; skip the following test,
	jne	IcoNotEnh		;  if it's not 101-102
	test	es:byte ptr [kb_flag], fCtrl	; is Ctrl down?
	mov	al, cPrint		; change scan code
	jz	notsys
IcoNotEnh:
	cmp	ds:[KeyType], 2
	test	es:byte ptr [kb_flag], fCtrl	; is Ctrl down?
	jz	notsys

endif	; ICO


ifdef	NOKIA
	cmp	IsEri,1
	jne	NoEricsson
	cmp	PCType,0FCH		; Ignore if not AT-compatible
	jne	notsys
	cmp	KeyType,6
	je	notsys			; No SYSREQ on 9140 (but SyReq)
NoEricsson:
endif	; NOKIA


sysreqwait:
	or	ah,ah		    	; Only on key-down
	js	sysreqdown		; sign set if key-up
igkey:	
	jmp	ignorekey

sysreqdown:

	cmp	fSysReq, 0		; EnableKBSysReq() sets/resets this:
	jz	igkey			; can we break to debugger?

;;	%out uncommented stuff -- fix for ROM!
;;	mov	ax, word ptr cs:[nmi_vector+2]	; get seg. of NMI vector
;;	cmp	ax, 0F000H		; does it point to ROM
;;	je	igkey		    	; BIOS or, possibly ..
;;
;;	cmp	ax,0070H		; does it point to DOS BIOS?
;;	je	igkey			;  if so, ignore this.

	; we now assume NMI points to SymDeb , so we simulate the NMI.
	; But first, we turn control [and Alt?] off, in both the BIOS
	; and Windows!

	and	byte ptr es:[kb_flag], not (fCtrl+fAlt)	; do BIOS

	mov	ax,VK_MENU+8000h	; ALT off in Windows
	mov	bx,38h			; was bl .. make sure bh 0
	call	ds:[event_proc]
	mov	ax,VK_CONTROL+8000h	; CONTROL off in Windows
	mov	bx,1dh
	call	ds:[event_proc]

	; now determine if we are to call off to int 2 or kernel!

	test	fSysReq,02

	; now we simulate the NMI
	; The code pointer nmi_vector must be in the CODE segment, since
	; we want to restore ALL the other registers to what they were
	; when the INT 09 interrupt happened, when we emulate the NMI.
	; The Enable() function sets up nmi_vector.

	pop	bx
	pop	es
	pop	ax
	jnz	@F

	push   word ptr nmi_vector[2]
	push   word ptr nmi_vector[0]  ; address of nmi_vector on stack
	push   bp
	mov    ds, [bp+6]	       ; restore ds
	pop    bp
	retf   2		       ; "return" to nmi_vector & pop ds

@@:	pop	ds
	jmp	CVWBreak		; go to KERNEL!!
notsys:

ENDIF				    	; End of SYSREQ stuff

; We are about to enable interrupt, before doing that we have to protect
; against another interrupt happening before we are done with this one.

	cmp	ds:[fReEnter],0		; are we alone?
	jz	kbiR
	jmp	ignorekey		; is he typing REALLY fast?

kbiR:	inc	ds:[fReEnter]
    ;
    ;   In order for this to work in various 386 Virtual environments
    ;	just setting this "fReEnter" exclusion flag is not the right thing
    ;	to do. A 386 virtual machine monitor may be Simulating keyboard
    ;	activity to us and just doing this exclude will cause us to miss
    ;	lots of keys that the monitor is trying to send us because it will
    ;	send us keys as soon as we EOI the keyboard and enable interrupts.
    ;
    ;	We fix this problem by masking off the keyboard interrupt at the
    ;	interrupt controller. This prevents the 386 monitor from sending us
    ;	more keys till we're ready to get them. This works fine in the non-386
    ;	environments as well. This method is prefered over disabling the
    ;	keyboard at the Keyboard Controller because it is more portable.
    ;	There seems to be a fair variation amoung clones in the keyboard
    ;	controller used, but the keyboard is always IRQ 1, and the interrupt
    ;	controller is always at the IBM port addresses, and is an 8259.
    ;
    ;
	push	ax
	in	al,21h			; Get IRQ mask register
	or	al,02h			; mask off kybd ints (IRQ 1)
	jmp	$+2			; I/O delay for AT type machines
	jmp	$+2
	out	21h,al			; set new mask
	pop	ax

ifdef BIGSTK	; hp addition..

; Modification to allow apps with small stack sizes not to blow up
; (i.e. diskcopy) (HP c-ralphp 6/9/88)
;
;
; Switch to a private stack
;
	mov	ds:SaveSS, ss
	mov	ds:SaveSP, sp
	mov	ds:NextSS, cs
	mov	ss, ds:NextSS
	mov	sp, dataOFFSET Stack

; end (HP c-ralphp modifications)
endif ; BIGSTK

	sti				; interrupts on for others...

	xor	bh,bh
	call	ds:[kbd_proc]	; standard_proc or prev_was_ext_proc


ifdef BIGSTK
; Modification to restore the original stack (HP c-ralphp 6/9/88)
;
; Restore the old stack
;
	cli
	mov	ss, ds:SaveSS
	mov	sp, ds:SaveSP

; end (HP c-ralphp modifications)
endif ; BIGSTK

	mov	ds:[fReEnter],0		; unlock keyboard
;
; Re-enable keyboard INTs at the interrupt controller
;
	cli				; Ints off again.
	in	al,21h			; get IRQ mask register
	and	al,NOT 02h		; turn on kybd ints again (IRQ 1)
	jmp	$+2			; I/O delay for AT type machines
	jmp	$+2
	out	21h,al			; restore correct mask
	jmp	$+2			; I/O delay for AT type machines

	Public ignorekey
ignorekey:

	pop	bx
	pop	es
	pop	ax
	pop	ds
	sti				; add for '286
	iret

keybd_int   ENDP

;***********************************************************
; Standard routine for translating scan code to virtual keycode
; This is the default and the only one if the keyboard is NOT RT-like.
;
;	Input	AL= Scancode (without make/break bit)
;		AH= 00 if make, 80h if break
;		BH=0
;
;***********************************************************

	Public standard_proc

standard_proc proc near

	xor	bh,bh			; Clear Extended key flag
	cmp	al,cNumLock		; (See ICO code below)
	jne	notNumLock		; but if it's NumLock, we
	inc	bh			; set it.
notNumLock:
	mov	fExtend, bh

	call	SetShiftState

; [next 3 instructions added 06jan88 ]
; If used key as OS/2 screen switch, don't do anything except set the
; shift state.
	cmp	ds:[fIgnoreKey],0
	jz	noIgnore
	jmp	stdproc_end
noIgnore:


ifdef ENHANCE				; driver for enhanced keyboard

; The following block of code is for drivers handling Enhanced keyboard
; only!

	cmp	ds:[KeyType],4		; Enhanced?
	jne	noPrevExt
					; this IS an Enhanced keyboard.
	cmp	al,cExtended1		; E1 prefix for Pause key?
	jne	CheckForE0
	mov	ds:[kbd_proc],codeOFFSET pause_proc	; yes, handle specially
	jmp	short j_stdproc_end

CheckForE0:
	cmp	al,cExtended		; is this Extended scan code (E0)?
	jne	noPrevExt		; if so, next keyboard int handled
	mov	ds:[kbd_proc],codeOFFSET prev_was_ext_proc	; by this..
j_stdproc_end:
	jmp	stdproc_end

	Public noPrevExt

noPrevExt:

; code to handle ShiftLock.
;
; This code simulates pressing CapsLock when either the ShiftLock
; or either Shift key is pressed, depending on the state of the fCaps
; flag in kb_flag.
;
; To the Windows high-level code, and to the ToAscii() routine, this
; looks just like CapsLock.

	test	ds:[fKeyType],kbShiftLock	; ShiftLock for this KB?
	jz	noSpecial		; no, normal Caps Lock instead.

	cmp	al,cCapsLock		; yes -- is this Capslock key?
	jnz	noCaps
	or	ah,ah			; up or down?
	js	shiftLockUp

	test	byte ptr es:[kb_flag], fCaps	; shiftlock set?
	jnz	shiftLockUp		; if not, 
	call	FakeCapsLock		; fake CAPSLOCK key going down/up

shiftLockUp:				; CapsLock up .. do nothing
	jmp	stdproc_end

noCaps:					; not Caps Lock key
	cmp	al,cLShift		; is (non-extended) Left Shift?
	je	isShift
	cmp	al,cRShift
	jnz	noSpecial
isShift:				; it's shift key, clear Caps Lock
	or	ah,ah			; is it make?
	js	shiftUp			; skip if not

	test	byte ptr es:[kb_flag], fCaps	; is shiftlock flag set?
	jz	shiftUp			; if so, 
	call	FakeCapsLock		; fake CAPSLOCK key going down/up

shiftUp:

;; End of special code for RT keyboard ...

noSpecial:

endif	; ENHANCE

; If the 2 shift keys are down, when the first is released we need
; to keep Shift State down, so we'll ignore the first Shift break.
; Remember: we already called SetShiftState that may have reset
;           the Shift state we are interested in, Yeerk ! 
;
        test    byte ptr es:[kb_flag],fShift
        jz      cbd0                	; no Shift down
        or      ah,ah               	; is it break ?
        jns     cbd0                	; no
        cmp     al,cLShift          	; LeftShift ?
        jz      nosp1               	; yes, ignore
        cmp     al,cRShift          	; RightShift ?
        jnz     cbd0                	; no, skip
nosp1:  jmp     stdproc_end         	; yes, ignore
cbd0:

ifdef	NOKIA
	cmp	IsEri,1
	jne	NotExtraEnter
	cmp	al,120			; 1050 or 9140
	je	ExtraEnter
	cmp	KeyType,5
	jne	NotExtraEnter
	cmp	al,84			; EPPC, 1050
	je	ExtraEnter
	jmp	SHORT NotExtraEnter
ExtraEnter:
	mov	al,78H			; Make it Ericsson special Enter
					; (requires Ericsson Winoldap)
;;	mov	al,1CH			; Make it normal Enter (for standard
					; Winoldap)
NotExtraEnter:
endif ; NOKIA

;
; Translate scan code to virtual key
; First, is it control-numlock or control-break?
;
	test	byte ptr es:[kb_flag],fCtrl	; is it Ctrl?
	jz	kbi2

;; test for control-numlock:
 	cmp	al,cNumLock
	jnz	kbi1a

ifdef	NOKIA
	cmp	KeyType,6		; 9140 is special case
	jne	kbi1_0
	test	BYTE PTR es:[ProtocolFlag],$PrefixFlag
	jz	kbi2
	and	BYTE PTR es:[ProtocolFlag],NOT $PrefixFlag
	add	al,080H
kbi1_0:
endif	; NOKIA

	; Control + Numlock translated to VK_PAUSE ..
	mov	bx,ax
	mov	al,VK_PAUSE
	jmp	kbi4			; (no longer short)

kbi1a:					; it's NOT NumLock

	cmp	al,cBreak	    	; is it Break?
	jnz	kbi2

ifdef	NOKIA
	cmp	KeyType,6		; 9140 is special case
	jne	kbi1_1
	test	BYTE PTR es:[ProtocolFlag],$PrefixFlag
	jz	kbi2
	and	BYTE PTR es:[ProtocolFlag],NOT $PrefixFlag
	add	al,080H
kbi1_1:
endif ; NOKIA

	mov	ds:[fBreak],0
	mov	bx,ax
	test	ah,80h			; only signal for up transitions!
	jz	@F
	call	CtlBrkCase		; Test for CtlBreak
@@:	mov	al,VK_CANCEL
;;	cmp	ds:[fBreak],0		; 07 sep 89: just send VK_CANCEL
;;	jnz	tokbi4
;;	jmp	stdproc_end		; ignore if somebody is trapping us
tokbi4:
	jmp	kbi4

kbi2:	xor	bx,bx			; it's NOT Break
	mov	bl,al			; save scan code

ifdef	ENVOY
;
; If we are using an envoy keyboard and
; if it is an HP Cursor pad key or an HP softkey scancode,
; use the HP table.
;
public	hp_debug
hp_debug:
;;	int	3
	test	ds:fVectra, FV_ENVOY
	jz	env_cont
	cmp	bl, cCCP_UP
	jb	env_cont
	cmp	bl, cf8
	ja	env_cont
	DosXMacro	ds, SOFT
	mov	al, ds:[bx+HPTransTable-cCCP_UP]
	jmp	kbi4
env_cont:				; it's not, continue

endif ; ENVOY

ifndef	NOKIA	; Nokia NOT defined
	mov	al,-1
	cmp	bl,byte ptr KeyTransTblSize
	jb	kbi2a
	jmp	kbi4
kbi2a:

else	; NOKIA defined
	cmp	IsEri,1
	jne	NotEnter
	test	BYTE PTR es:[ProtocolFlag],$PrefixFlag
	jz	NotPrefixed
	and	BYTE PTR es:[ProtocolFlag],NOT $PrefixFlag
	add	bl,080H
NotPrefixed:
	cmp	BYTE PTR es:[KeyBoardId],ID1051
	jne	NotEnter
	cmp	bl,4Eh
	jne	NotEnter
	mov	bl,78H
NotEnter:
endif	; NOKIA defined

	push	si
	mov	si,KeyTransBase
	mov	al,ds:[bx+si]		; get the translation
	pop	si

Translated:

ifdef	NOKIA
    ifdef ICO
	; this case will never happen!!
	cmp	KeyType,6
	je	CheckNumPad
    endif
endif
	; Check for ICO keyboard.

ifdef ICO
	cmp	ds:[KeyType], 2		; check to see
	je	CheckIco00		; if it's an ICO keyboard..
	jmp	CheckNumPad

CheckIco00:
	cmp	al, VK_ICO_00		; is this ICO 00 key?
	jne	IcoTranslate		; it is 00 key...
	mov	al, VK_0		; change to VK_0
	or	ah,ah			; make or break?
	js	TrToKbi4
	xor	bh,bh			; make sure bh = 0
	call	ds:[event_proc]		; Make: send first 0
	xor	ah,80h			; it's make, so
	call	ds:[event_proc]		; fake key going up
	xor	ah,80h			; and down again
	jmp	kbi4			; then send second 0... 

TrToKbi4:
	jmp	kbi4			; go send char codes in AX,BX

IcoTranslate:
	cmp	bl,cIcoPrint		; is it a special ICO key?
	jb	CheckNumPad		; (in range 55h..5fh)
	cmp	bl,cIcoDivide
	ja	CheckF1116		; might be F11..F18..

	; Translate scan code to normal IBM range so WINOLDAP
	; will recognize these keys!
	; All of these keys will be flagged with 1 in BH, which
	; will be passed on to User in the event_proc call, as for
	; extended keys on IBM Extended keyboards.
	push	si
	mov	si,dataOFFSET IcoTransT
	sub	bx,cIcoPrint
	mov	bl,ds:[bx+si]
	pop	si
	mov	fExtend, 1		; flag as 'extended' key
	jmp	kbi4

CheckF1116:

ifdef ICO_CHANGE_F11_F18		; Map F11..F18 to shift F1..F8?

; if this code isn't assembled, we fall thru to CheckNumPad

; ====== Handle function keys F11, F12, .. F18 on ICO keyboards =====
; Scan codes as well as VK_ codes are mapped into those for F1..F10,
; with shift state set to give the right translation.

	cmp	al, VK_F11		; 2 ranges: VK_F11..VK_F16 and
	jb	CheckNumPad		; VK_F17..VK_F18
	cmp	al, VK_F16
	ja	CheckF1718
					; it's F11..F16
	sub	al, (VK_F11-VK_F1)	; Map into VK_F1..VK_F6
	mov	bl, (cF1-VK_F1)		; compute corresponding
	add	bl, al			; scan code
	jmp	short DoF11F18

CheckF1718:
	cmp	al, VK_F17
	jb	CheckNumPad
	cmp	al, VK_F18
	ja	CheckNumPad
					; it's F17 or F18.
	sub	al, (VK_F17-VK_F7)	; map into VK_F7..VK_F8
	mov	bl, (cF7-VK_F7)		; compute scan code
	add	bl, al

DoF11F18:
	test	byte ptr es:[kb_flag],fShift	; any shift key down?
ifndef	NOKIA
	jnz	kbi4			; yes, already shifted.
					; no, we have to fake it..
else	; This case will never happen!!!
	jnz	TrToKbi4
endif
	xor	bh,bh			; make sure bh = 0
	push	ax
	push	bx
	mov	ax,VK_SHIFT		; push imaginary shift key
	mov	bl,54
	call	ds:[event_proc]
	pop	bx
	pop	ax
	call	ds:[event_proc]		; send the function key
	mov	ax,VK_SHIFT+8000H
	mov	bl,54
	jmp	short kbi4		; go release imaginary shift

endif	; ICO_CHANGE_F11_F18

endif	; ICO

; ====== Handle keys on numeric pad ============
CheckNumPad:

	xor	bh,bh			; is it on numeric keypad?
ifdef	NOKIA
	cmp	KeyType,6		; 9140 is special case
	jne	Not9140NumPad
	cmp	bl,71 + 080H		; NumPad has prefix
	jb	kbi4
	cmp	bl,83 + 080H
	ja	kbi4
	jmp	SHORT isNumPad
Not9140NumPad:
endif

	cmp	bl,71
	jb	kbi4
	cmp	bl,83
	ja	kbi4

isNumPad:
	; yes...  This is the numeric pad.
	; Here, if NumLock is set, we change the virtual keycodes to
	; numeric VK_NUMPAD codes, so the keys will be translated
	; as numbers etc.  But if a shift key is down, we handle
	; these as cursor keys, but we need to make sure that these
	; are seen as UNSHIFTED
	test	byte ptr es:[kb_flag],fNum	; is num lock down?
	jz	kbi4			; no, do normal processing
	test	byte ptr es:[kb_flag],fShift ; either shift key down?
	jnz	kbi3			; yes, keep as cursor keys
					; no, treat as numeric keys.

	; Now we prepare to translate as a NUMERIC key
	mov	al,bh
	or	al,al
	jnz	kbi4

	push	si
	mov	si,KeyNumBase

ifdef	NOKIA
	cmp	KeyType,6		; 9140 still has prefix
	jne	Not9140			;	in scancode
	mov	al,ds:[bx+si-(71 + 080H)]; new: movable table
	pop	si
	jmp	SHORT kbi4
Not9140:
endif ; NOKIA

	mov	al,ds:[bx+si-71]	; new: movable table
	pop	si

	jmp	short kbi4

kbi3:	; The key is on the numeric pad, NumLock is set, but a shift
	; key is down (bit(s) set in BIOS key state), so we are going
	; to keep this as a cursor key.  To do this, we need to
	; make sure that Windows' state vector entry for VK_SHIFT is OFF
	; even though a shift key is actually down.

	cmp	bl, LastCursor		; are we repeating?
	je	kbi3a			; if so, we just send the key
	mov	LastCursor, bl		; otherwise, we save the scan code
					; as a flag and turn off VK_SHIFT.
	push	ax
	push	bx
	mov	ax,VK_SHIFT+8000H
	mov	bl,54
	call	ds:[event_proc]
	pop	bx
	pop	ax
	jmp	short kbi4

	; This is the same cursor key --  turn VK_SHIFT back on and
	; clear the flag if the cursor key is being released (break)

kbi3a:
	test	ah,80h			; break?
	jz	kbi4			; if not, keep shift off, and
	call	ds:[event_proc]		; send actual key now...
	mov	ax,VK_SHIFT		; pretend shift key went down
	mov	bl,54
	mov	LastCursor,ah		; clear flag

kbi4:

ifdef	ENVOY

;!!! if Vectra, down transition and not a modifier or a lock, key click

	test	ds:fVectra, FV_A	; 23 jun 88 peterbe, was CMP
	jz	no_click
	cmp	ds:[fModifier],	0
	je	no_click
	cmp	bl, cCapsLock
	je	no_click
	cmp	bl, cNumLock
	je	no_click
	cmp	bl, cBreak
	je	no_click
	cmp	ah, 0
	jne	no_click
	push	ax
	call	ds:[event_proc]
	pop	ax
;	push	bp
;	mov	ah, F_SND_CLICK
;	mov	bp, V_SYSTEM
;	int	6fh
;	pop	bp
	jmp	stdproc_end
no_click:

endif	;ENVOY

ifndef	NOKIA
	;
	; Call windows with ah == 80h for uptrans, 00h for downtrans
	; al = virtual key, bl = scan code
	; bh = 0 (except for ICO extended keys, and NumLock)
	;
	; Windows preserves all registers
	;
	xor	bh,bh			; clear and
	xchg	bh,fExtend		; get extended key flag
	call	ds:[event_proc]

else	; NOKIA

; The following code handles some extra keys on the Ericsson 9140
;	keyboard. The keys are EnlargeWindow, Jump, and BackTAB.
;	EnlargeWindow will not work if/when the Alt-F10 accelerator
;	key is removed.

	cmp	KeyType,6		; All extra keys are on
	jne	kbi99			;	9140 keyboard

        test    byte ptr es:[kb_flag],fAlt ; is Alt already down?
	jnz	kbi9
; Alt is up
	cmp	al,VK_OEM_JUMP
	jne	kbi51
	mov	al,VK_ESCAPE
	mov	bl,01H
	jmp	SHORT kbi6
kbi51:
	cmp	al,VK_OEM_ENLW
	jne	kbi52
	mov	al,VK_F10
	mov	bl,44H
	jmp	SHORT kbi6
kbi52:
	jmp	SHORT kbi98

kbi6:
	xor	bh,bh			; make sure bh=0
	push    ax
        push    bx
        mov     ax,VK_MENU
        mov     bl,38H
        call    ds:[event_proc]
        pop     bx
        pop     ax
        call    ds:[event_proc] ; send actual key...
        mov     ax,VK_MENU+8000H	; pretend alt key went up
        mov     bl,38H
	jmp	SHORT kbi99
; Alt is down
kbi9:
	cmp	al,VK_OEM_JUMP		; Alt Jump is ChgSc
	jne	kbi91
	mov	al,VK_TAB
	mov	bl,01H
	jmp	SHORT kbi99
kbi91:
	cmp	al,VK_OEM_ENLW		; Alt EnlW id DelW
	jne	kbi92
	mov	al,VK_F4
	mov	bl,44H
	jmp	SHORT kbi99
kbi92:
	jmp	SHORT kbi98

kbi98:
 	cmp	al,VK_OEM_BACKTAB
	jne	kbi99
	mov	al,VK_TAB
	mov	bl,0FH
        test    byte ptr es:[kb_flag],fShift ; is Shift already down?
	jz	kbi98a
	jmp	kbi3			; Shift BACKTAB is TAB
kbi98a:
	xor	bh,bh			; make sure bh = 0
	push    ax
        push    bx
        mov     ax,VK_SHIFT
        mov     bl,54
        call    ds:[event_proc]
        pop     bx
        pop     ax
        call    ds:[event_proc] ; send actual key...
        mov     ax,VK_SHIFT + 8000H     ; pretend shift key went up
        mov     bl,54
kbi99:

	xor	bh,bh			; make sure bh = 0
        call    ds:[event_proc]

endif	; NOKIA

stdproc_end:

	ret

standard_proc endp

ifdef ENHANCE

; For Shift Lock, fake Caps Lock key being depressed.
; This is called when the caps lock or shift key is pressed,
; and the value of fCaps flag at 40:17h is to be changed.

	Public FakeCapsLock

FakeCapsLock proc near

	push	ax			; fake CAPSLOCK key going ..
	push	bx
	mov	bx,cCapsLock		; load capslock scan code.
	mov	ax,VK_CAPITAL		; .. down and ..
	call	ds:[event_proc]
	mov	ax,VK_CAPITAL+8000H	; .. up.
	call	ds:[event_proc]
	pop	bx
	pop	ax			; continue processing SHIFT key..

	ret

FakeCapsLock endp


endif ; ENHANCE
ifdef ENHANCE
;***********************************************************
; prev_was_ext_proc -- used when previous was extended prefix (E0)
;
; used only by RT keyboard
;
;	Input	AL= Scancode (without make/break bit)
;		AH= 00 if make, 80h if break
;		BH=0
;
;	Mainly undoes Shifting and Unshifting generated internally
;	by this (@#$) keyboard and uses the extended prefixes to
;	distingush between normal keys and new (RT) keys.
;
;***********************************************************

	Public prev_was_ext_proc

prev_was_ext_proc proc near

	mov	ds:[kbd_proc], codeOFFSET standard_proc
	cmp     al,cLShift          ; is it extended Left Shift ?
	jz	LeavePrevProc        ; if yes, eat it
	cmp	al,cRShift	    ; else is it extended Right Shift ?
	jz	LeavePrevProc        ; if yes, eat it

; Test if we receive PrintScreen. Remember the make code has been eaten
; before we get a chance to be called. The break code will release the
; temporarily LeftShift that was then set.
	cmp	al,cPrint
	jnz	prev10
if 0
	; necessary?
	xor	byte ptr es:[kb_flag],fLshift
endif
LeavePrevProc:				; Add label 04dec87
	jmp	short prevproc_end

prev10:
	push	bx
        call    SetShiftStateNoNumlock
	pop	bx
	mov	bl,al			; bl= scancode (all along proc)

; Divide key special case: if Slash is found a VK_DIVIDE is sent
;
	cmp	al,cSlash		; is it extended Slash ?
	jnz	prev20			; if not, skip
	mov	al,VK_DIVIDE		; else send VK_DIVIDE
	jmp     short prev40

; When ShiftLock is on, we don't want to be get our VK_code shifted (none
; of our code correspond to alphanumeric), so we have nothing to do.
; We would have one exception (VK_DIVIDE), but we already took care of it.

; Break case
;
prev20:
	cmp	al,cBreak		; is it extended Break ?
	jnz	prev30			; if not, skip
        mov     ds:[fBreak],0
	test	ah,80h			; only signal for up transitions!
	jz	@F
	call	CtlBrkCase		; Test for CtlBreak
@@:	mov	al,VK_CANCEL		; always do VK_CANCEL
	JMP     SHORT PREV40		; 07 sep 89.. don't stuff below..
;;      cmp     ds:[fBreak],0
;;      jnz     prev40
;;      jmp     short prevproc_end	; ignore if somebody is trapping us

; Standard case, convert to our virtual key
;
prev30:
ifndef	NOKIA	; NOT Nokia
        mov     al,-1
        cmp     bl,byte ptr KeyTransTblSize ; test if scancode in table
        jae     prev40			; if not, skip (VK= -1)

else ; IS Nokia keyboard driver

;	This code not used at the moment
;	Just put here as a reminder
;;  if 0
;;	cmp	IsEri,1
;;	jne	prev30NotPrefixed
;;	test	BYTE PTR es:[ProtocolFlag],$PrefixFlag
;;	jz	prev30NotPrefixed
;;	and	BYTE PTR es:[ProtocolFlag],NOT $PrefixFlag
;;	add	bl,080H
;;prev30NotPrefixed:
;;  endif
endif	; NOKIA

	push	si
	mov	si,KeyTransBase		; get the translation
	mov     al,ds:[bx+si]
	pop	si

	errn$	prev40	; causes phase error if MASM size error
;
prev40:
;
; (addition for Oli. 4 mar 87 plb)
; If flag is set for AltGr (right hand ALT) handled as control-ALT,
; then insert a CONTROL key call to event_proc before the ALT key call,
; but only if the BIOS CONTROL key flag is false (got that?).
; (15 oct 87: add code to prevent repeats on AltGr key, which were putting
; strange things in Windows' input buffer.)
;
	cmp	bl,cAlt			; was this an ALT key?
	jne	prev60			; skip on if not..
	test	ds:[fKeyType],kbAltGr	; does this KB do this?
	jz	prev60			; skip on if not..
	test	byte ptr es:[kb_flag],fCtrl	; is the real control key down?
	jnz	prev60			; if so, don't bother..
					; the following code prevents repeats
	or	ah,ah			; on AltGr key depressions.
	jns	FakeAltGrDown		; up or down?
	mov	fAltGrDn,0		; up, clear flag
	jmp	short DoFakeAltGr	; go fake control-Alt release

FakeAltGrDown:
	cmp	fAltGrDn,0		; down, is this a repeat?
	jnz	prevproc_end 		; if so, ignore it.
	mov	fAltGrDn,1		; otherwise fake control-Alt depression

DoFakeAltGr:
	push	ax			; OK, let's fake a Control key
	push	bx			; (but not set kb_flag bit!!)
	mov	al,VK_CONTROL		; [AH] tells whether fake ctrl key
	mov	bx,cCtrl		;  is going up or down.. (bh = 0)
	call	ds:[event_proc]		; we have control (key)
	pop	bx			; get ALT key parameters back,
	pop	ax			; and now tell Windows about that...
;
; Windows preserves all registers
prev60:

; Call windows with ah == 80h for uptrans, 00h for downtrans
; al = virtual key, bh = extended key flag, bl = scan code
;
	or	bh,1			; this flag indicates extended key.
	call    ds:[event_proc]

prevproc_end: 

	ret

prev_was_ext_proc endp

;***********************************************************
; Handle Extended keyboard Pause key.
; This is called for the 2 bytes following an E1 prefix.
; the whole scan code sequence is
;
;	E1 1D 45  E1 9D C5
;
; The shift state bits are not changed.
;
; Input		AL = Scancode
;		AH = 00 if make, 80h if break
;		bh = 0
;***********************************************************

pause_proc proc near

	cmp	al, cCtrl		; first byte = 1D (control)?
	je	pause_proc_end		; yes, just pass this on

	mov	ds:[kbd_proc], codeOFFSET standard_proc
	cmp	al, 45h			; second byte?
	jne	pause_proc_end

	xor	bh, bh			; clear enhanced flag
	mov	bl, al			; scan code in BL
	mov	al, VK_PAUSE		; it's VK_PAUSE
	call	ds:[event_proc]

pause_proc_end:
	ret

pause_proc endp


endif ; ENHANCE

;ifdef NeverDoInt1BonCtrlBreak
	Public CtlBrkCase

CtlBrkCase PROC	NEAR

; we are now control-breaking

	push	bx			; save registers we use
	push	cx
	push	dx
	push	si
	push	di
	push	es
	push	bp

	call	DoSignal
;	 INT	 1bh

	pop	bp
	pop	es
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	ret
CtlBrkCase ENDP

ifdef NeverDoInt1BonCtrlBreak
	public	INT1BHandler
INT1BHandler PROC FAR
	inc	ds:[fBreak]
	iret
INT1BHandler ENDP
endif	; NeverDoInt1BonCtrlBreak

;
;  Keep accurate track of shift state byte at 40:17H
;  For Alt and Control, we set or reset bits in 40:18 also.
;  For Numlock, the state bit is in 40:18, and the toggle bit is
;  in 40:17h.
;
;	AL:	scan code
;	AH:	sign bit indicates up/down
;
;	Uses BL or BX as mask.
;
	Public SetShiftState		; public only for debug

SetShiftState proc near

ifdef	ENVOY
	mov	ds:[fModifier],0
endif

	cmp	al,cNumLock		; check for numlock
	jz	ccNum1

SetShiftStateNoNumlock:
	mov	bx,fLshift		; check shift, ctrl, alt bits
	cmp	al,cLshift
	jz	ccv4
	mov	bx,fRshift
	cmp	al,cRshift
	jz	ccv4
	mov	bx,fCtrlW
	cmp	al,cCtrl
	jz	ccv4
	mov	bx,fAltW
	cmp	al,cAlt

;;;;; Changed code below 13 April 1990 to fix KBDHP bug ;;;;;;;;;;;;;;;;

ifdef	ENVOY
if1
%out .. ENVOY code in SetShiftState
endif

	; Envoy version
	jz	ccv4
	mov	ds:[fModifier], -1
	ret

else
ifdef NOKIA
if1
%out .. NOKIA code in SetShiftState
endif

	; Nokia version
	jz	ccv4
	cmp	KeyType, 6		; 9140 is special case
	jne	ccv6			; .. it has special rt. alt. scan code
	cmp	al, 055H		; The right Alt key
	jnz	ccv6

else
if1
%out .. STANDARD code in SetShiftState
endif

	; standard version
	jnz	ccv6
    if 1
	jnz	ccv6			; TAKE THIS OUT WHEN DONE
					; old code duplicated this instruction
					; in this case!  This is just here so
					; KEYBOARD.DRV and KBDOLI.DRV will
					; binary compare!
    endif

endif
endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ccv4:
	DosXMacro	ds, Mod0
	or	ah,ah
	jns	ccv5
	not	bx
	and	word ptr es:[kb_flag],bx
	ret
ccv5:	or	word ptr es:[kb_flag],bx
ccv6:
	ret

; It's NumLock up/down.  This is similar to above, but keeps state in
; 40:18H, and toggles bit in 40:18H, so it's a little more complex.
; [BL] =  fNum is the appropriate bit in BOTH bytes.
; If CTRL is down, we do nothing.

ccNum1:
	test	byte ptr es:[kb_flag], fCtrl 	; (was fShift+fAlt+fCtrl)
	 jnz	ccv6				; must have Ctrl off!
	mov	bl,fNum				; bit for numlock
	or	ah,ah
	jns	ccNum2
	not	bl				; upstroke, so just 
	and	byte ptr es:[kb_flag_1],bl	; clear state
	ret
ccNum2:						; downstroke: already down?
	test	byte ptr es:[kb_flag_1],bl
	jnz	ccNum3
	xor	byte ptr es:[kb_flag],bl	; no, so toggle the toggle bit,
	inc	ds:[NumLockFlag]		; and set flag for ToAscii()

ccNum3:
	or	byte ptr es:[kb_flag_1],bl	; set state bit.
	ret

SetShiftState endp

	public endTRAP
endTRAP:

if2
%out	.... END TRAP.ASM ....
%out
endif

sEnd CODE

END
