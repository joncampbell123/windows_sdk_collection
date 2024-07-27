;*** DATACOM.ASM for Olivetti Windows 2.00  keyboard drivers ******************
;
;	Copyright 1984,1985 by Microsoft Corp.
;	Copyright 1985,1986,1987 by Olivetti.
;
;	This is for the Microsoft Windows 2.0 keyboard driver 
;
;	This is a synthesis of enhancements to the old DATACOM.INC
;	for Olivetti keyboards, and enhancements in the Microsoft
;	version for the RT=Ronco=Olivetti 101/102-key keyboard with
;	its strange extended keycode sequences.
;
;	It is now a separate .ASM source file.
;
;	In the comments below, I have referred to the 101/102 key keyboard
;	as the 'RT' keyboard, as Microsoft does, for brevity.
;
;	Note version/revision message near beginning of DATA segment.
;
;	NOTE: BIGSTK is defined here; see below.
;
;***********************************************************
; History
;		plb = peterbel = peterbe
; ==== Windows 2.10 ====================================
; 23 jun 88	peterbe	Commented import of AckByte
;			First instr after kbi4: is now TEST CS:fVectra,..
; 21 jun 88	peterbe	Commented ENVOY ifdefs better.
; 15 jun 88	peterbe	moved includes to before any ifdefs
; 14 jun 88	peterbe	Added hp enhancements -- BIGSTK, ENVOY ifdefs
; ==== Windows 2.0, Windows 386, under OS/2 as well ====
;
; 12 jan 88	plb	Restored VK_PAUSE code around kbi1_0:, kbi1a:.
; 11 jan 88	plb	Changed a few comments to correspond to MS code.
;			Changed flag HEAPWALK to OS2Debug.
; 07 jan 88	plb	Put test for 2 shift keys down at noSpecial:
;			Added CtrlNumlock, -Capslock, code around kbic: label
;			Ctrl-Numlock code around kbi1_0: removed. Is this OK for
;			Ericsson?
; 06 jan 88	plb	Put test for fIgnoreKey near beginning of
;			standard_proc, looking at VK_PAUSE code..
; 05 jan 88	plb	F11 F12 no longer shifted to Shift F1, Shift F2 on
;			RT keyboard.  This means that oldapps won't see
;			F11, F12, but Excel will!
; 17 dec 87	plb	More ifdef HEAPWALK..
; 10 dec 87	plb	Debugging
; 09 dec 87	plb	Add code from Microsoft version in OS/2 code near
;			kbi1: and kbi111:
; 04 dec 87	plb	Add LeavePrevProc label, for Olivetti-only make.
; 1.07 release
; 10 nov 87	plb	Put check for cDelete after kb1a:, moved kbi0:,
;			as for Microsoft US driver.
; 30 oct 87	plb	Put 'ifndef W103' around extrn KbdRst...
; 28 oct 87	plb	Put call to KbdRst in interrupt code to reset ext. mem.
; 27 oct 87	AP	..
; 27 oct 87	plb	code relating to fSwitchEnable is in 'ifndef W103'
; 26 oct 87	plb	Moved 'Version' string to DATE.ASM
; 15 oct 87	plb	Add code and fAltGrDn to prevent AltGr repeats.
;			Added fOS2Box, etc., and code for OS/2 hot key (kbi1:).
; 08 oct 87	AP	Added code to handle Ericsson keyboards
; 08 oct 87	plb	Added fSwitchEnable flag for OS/2 screen switch.
;  1 sep 87	plb	Further ICO adaptations for brain-damaged Winoldap.
;			Handle 2nd Return, Divide, cursor keys, etc.
; ==== Windows 2.0 =======
; Rev 1.01:
; 31 aug 87	plb	Convert VK_ICO_00 into 2 VK_0 messages. 00 key
;			should work now with brain-damaged WINOLDAP.
; Rev 1.00:
; 31 aug 87	plb	Fixed non-functioning code for ICO F17, F18.
; Rev 0.00:
; 28 aug 87	plb	Mapping function keys > 10 into shift plus
;			F1..F10 .. change both VK and Scan code, and
;			send appropriate VK_SHIFT up/down messages.
;			(Code between Translated: and CheckNumPad:)
; 30 jun 87	plb	Use AckByte from OLIKBD.ASM for interrupt
; 			acknowledge.
; 29 jun 87	plb	Began 2.00 version.
; 			Externals now relate to new table structure,
; 			and new bytes indicating table types, etc.
; 			VK_OEM_ALT processing commented out.
; 20 aug 87	plb	Updated code for handling fReEnter and
; 			interrupt enable/disable at "kbiR:".
;
; ========================================================================

include keyboard.inc
include olikbd.inc
include vkwin.inc
include vkoem.inc

if1
%out
%out	DATACOM.ASM Olivetti keyboard interrupt code
ifdef	ERICSSON
%out	.	with Ericsson support
endif
endif

; Allocate big stack when calling
BIGSTK equ 0

; Double byte range values for the Far East.
; The values defined here are for the Rest Of the World.
;
BeginRange1	equ	255
EndRange1	equ	254
BeginRange2	equ	255
EndRange2	equ	254

SYSREQ	=	054h	; PC-AT SYS REQ key .. not on M24 or RT keyboard

; ROM BIOS addresses

ShiftState	EQU	417H	; rom bios data area of shift states
CrtMode		EQU	449H	; rom bios data area for screen mode
KbFlag1		EQU	418H	; rom bios data area for hold state (13feb87)
fReset		equ	472H	; rom bios area for reset flags

ifdef	ERICSSON
ProtocolFlag	EQU	416H
$PrefixFlag	EQU	00000100B
KeyBoardId	EQU	4EEH
ID_IBM	EQU	0AAH
ID1050	EQU	0A5H
ID1051	EQU	0A8H
ID9140	EQU	0A6H
endif	; ERICSSON

; scan codes for special keys

cEsc		EQU	 1	;
cReturn		EQU	28

cCtrl		EQU	29
cLShift		EQU	42
cSlash		EQU	53	; 13feb87
cRShift		EQU	54
cPrint		EQU	55	; IBM keyboard printscreen
cAlt		EQU	56
cCapsLock	EQU	58
cF1		equ	59	; 3bh F1 key on anything
cF7		equ	65	; 41h F7 key on anything
cNumLock	EQU	69
; cScrollLock	EQU	70	; this name no longer used.
cBreak		EQU	70
cUp		equ	72	; up key
cLeft		equ	75	; left key
cRight		equ	77	; right key
cDown		equ	80	; down key
cDelete		EQU	83

cExtended	EQU	96	; E0h-80h - for RT extended codes - 13feb87
ifndef	ERICSSON
cCommand	EQU	0F0H
else
cCommand	EQU	0FAH
endif
				; Olivetti only:
cIcoPrint	equ	85	; 55h ICO keyboard printscreen
cIcoDivide	equ	95	; Ico divide key
				; ICO extended funct.keys F11..F18 are 96..103



; Flag bits in ShiftState (or kb_flag)

fCaps		equ	01000000b
fNum		equ	00100000b
fScroll		equ	00010000b
fAlt		equ	00001000b
fCtrl		equ	00000100b
fLshift		equ	00000010b
fRshift		equ	00000001b
fShift		equ	fLshift+fRshift	; either or both shift keys

; Flag bits in KbFlag1

fHold		equ	00001000b	; 13feb87


; **************** data and code begins here ***********************

sBegin DATA

; ********** externals for data in national keyboard table ******************

	extrn	TableType:byte	; Table type loaded (1, 2, 3, 4)
	extrn	KeyType:byte	; Keyboard type (1, 2, 3, 4)
	extrn	fKbRt:byte	; flags RT 101/102 keyboards
	extrn	OliType:byte	; if Olivetti protocol, lists keyboard type.
	extrn	PCType:byte	; identifies system type
	extrn	PCTypeHigh:byte
	extrn	RTFlags:byte
ifdef	ERICSSON
	extrn	IsEri:byte
endif	; ERICSSON

; Some of these are commented out, since not used in this module.

; Table for translating scan codes to virtual keycodes.
	extrn	KeyTransBase:word
	extrn	KeyTransTblSize:word

; Table for translating numeric pad keys in NUMLOCK mode.
	extrn	KeyNumBase:word

; External from INIT.ASM:
; Value of acknowledge byte
	extrn	AckByte:byte

; Kernel routine for extended memory reset
;
ifndef W103
EXTRN	KbdRst:FAR
endif

; *********************** Local data *********************************
;


; Keyboard information block
;
		PUBLIC	inquireData
		public	iqdNumFunc
inquireData	LABEL	BYTE
		DB	BeginRange1
		DB	EndRange1
		DB	BeginRange2
		DB	EndRange2

		DW	statesize	 ; #bytes of state info
iqdNumFunc	label	byte		; updated by KbdInit
		DW	numFunc		 ; How many Func. keys are on keyboard
		DW	hasBreak	 ; true => keyb supplies make and break
		DW	intsPerSec	 ; max rate of keyboard input events

; Where to save far procedure addresses
;
	PUBLIC  event_proc, bios_proc, bios1b_proc, enabled, LightsAddr
bios_proc   DD	0   ; Saved address of BIOS hardware keyboard interrupt handler
bios1b_proc DD	0   ; Saved address of BIOS INT 1B.
LightsAddr  DD	0   ; Saved address of INT 16H at boot time

event_proc  DD	0   ; Address of enabled keyboard event procedure
nmi_vector  DD	0   ; address of nmi vector

ifdef BIGSTK
;
; Private keyboard stack (HP c-ralphp 6/9/88) see comments in keybd_int
;
staticB	, ?, 384
globalW	Stack, ?
staticW	SaveSS,	0
staticW	SaveSP,	0
staticW	NextSS,	0

; end (HP c-ralphp modifications)
endif ; BIGSTK

; (MS code has ShiftLockException table here)

enabled	    DW	0
fBreak	    DB	0
fReEnter    DB	0	; reentrancy flag

fAltGrDn    db	0	; set if AltGr is down -- for preventing repeats.

ifdef PrintDebug
if1
%out .. PrintDebug code
endif
	public	PrintFlags
PrintFlags	db	0
endif


ifdef	ENVOY
    if1
    %out	.. Has VECTRA support 
    endif
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

; Flags for
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

else	; not ENVOY
    if1
    %out .. does NOT have VECTRA support
    endif

; for debug with ENVOY undefined in ENABLE
staticB	 fVectra, 0	; 1 if Vectra. Set by Enable

endif	;ENVOY



; flags for running in OS/2 Compatibility box

; don't do this for special 1.03 Windows driver..
ifdef W103
 if1
 %out .. No OS/2 support
 endif

else	; W103
 if1
 %out .. Has fSwitchEnable OS/2 support
 endif

ifdef OS2Debug
	if1
	%out .. Has OS2 debugging stuff
	endif
else
	if1
	%out .. Does not have OS2 flags debugging
	endif
endif	; OS2Debug

	public fOS2Box		; set in INIT.ASM
ifdef OS2Debug
fOS2Box		db	1 ; set if running in OS/2 Compatibility box
else
fOS2Box		db	0 ; set if running in OS/2 Compatibility box
endif

fIgnoreKey	db	0 ; Set to ignore keys used for screen switch
fIgnoreEsc	db	0 ; Count of up keys to ignore
	public fSwitchEnable	; accessed in ENABLE.ASM
fSwitchEnable	db	1 ; Flag to prevent/allow screen switches

ifdef OS2Debug
	public myShifts			; so we know where to look.
		db	'S'		; 
myShifts	db	0		; copy of BIOS shift flags
Count11		db	0		; count of calls to ROM
endif	; OS2Debug

endif	; (else) W103

; Address of routine called to handle a scan code.
; For non-RT keyboards, this never changes -- it's always 'standard_proc'.
; For RT keyboards, this changes when a prefix code is seen

staticW	kbd_proc,standard_proc

; Table to translate ICO scan codes in range cIcoPrint..cIcoDivide
; .. so that WINOLDAP will recognize these..

IcoTransT label byte
	db	cPrint		; PrintScreen
	db	56h		; Help
	db	cReturn		; Return
	db	cLeft		; Left cursor
	db	cDown		; Down cursor
	db	cRight		; Right cursor
	db	cUp		; Up cursor
	db	5ch		; Clear
	db	cBreak		; Break
	db	5eh		; -- nothing --
	db	cSlash		; Divide key

;***********************************************************
;
;--- keyboard hardware service -----------------------
;
;***********************************************************

public keybd_int
keybd_int   PROC    FAR
assumes	CS,DATA
	push	ax			; save registers we use
	push	ds

	xor	ax,ax
	mov	ds,ax

ifdef OS2Debug
					; keep shift state locally
	mov	al,ds:[ShiftState]	; for observation with heapwalk
	mov	cs:myShifts,al		; for debug
endif

	in	al,kb_data			; get the scan code

	; next few lines changed .. 13feb87 .. to be like Microsoft

	cmp	al,cCommand			; is it a keyboard command?
	jb	kbiTestUp			; (note: cCommand is different
						;  for Ericsson!)
	jmp	kbi0

kbiTestUp:

ifdef	ERICSSON
	cmp	al,80H
	jne	NotPrefix
	or	BYTE PTR ds:[ProtocolFlag],$PrefixFlag
	jmp	kbi0			; Let BIOS also get prefix
NotPrefix:
	jbe	notkbi1
	jmp	kbi1
notkbi1:
else	; ERICSSON
	test	al,80h				; is it an up transition?
	jnz	kbi1				; .. if not, continue
endif	; ERICSSON
	test	byte ptr ds:[kbFlag1],fHold	; in hold state?
	jnz	jkbi0				; if so, jump to ROM

; When running Windows, it can happen that we will be interrupted by a popup
; in text mode that will read keyboard (Business Network). This very
; wierd case is detected by finding the screen in text mode (this is
; quite uncommon when this keyboard driver is activated).
;
ifndef TEST
	; IF you want to test this driver with the OEM keyboard test,
	; TKEYBD.EXE, it must be assembled with the TEST flag set so
	; that this code is NOT executed.
	cmp	byte ptr ds:[CrtMode],4		; text mode?
	jb	jkbi0				; yes, jump to ROM.
endif

	mov	ah,ds:[ShiftState]		; get current shift state..

; Added (from Microsoft) 07jan88:
        cmp     al,cNumLock             ; is it NumLock?
        jz      kbic
        cmp     al,cCapsLock            ; is it CapsLock ?
        jnz     kbib                    ; no...
kbic:
        test    ah,fCtrl                ; is it Ctrl-NumLock or -CapsLock?
        jnz     jkbi0                    ; yes, jump to ROM
	jmp     kbi1			; no, 
; End 07jan88

kbib:	cmp	al,cDelete
	jz	kbia
	cmp	al,cBreak
	jz	kbia


ifdef	ENVOY ; Envoy code fragment 1

; Look for CTRL-Alt + or - which on Vectra A, and A+ is handled in the
; int9 BIOS.
;
	test	cs:fVectra, FV_A ; If Vectra A,A+, Pass thru Cntl-Alt + and -
	jz	kbihp
	cmp	al,cPlus
	je	kbia
	cmp	al,cMinus
	je	kbia

kbihp:
	cmp	al,cBackSlash	; If Carrera, pass thru Cntl-Alt-\
	je	kbia		; for speed change

; The following code was added to test for the possiblity of a CCP CTRL-ALT-DEL
;
	test	cs:[fVectra], FV_ENVOY
	jz	no_reset
	cmp	al, cCCPDel
	je	kbia
no_reset:

endif ; ENVOY


; Screen print test.  This is really wierd now because of handling
; M24, IBM, ICO, RT keyboards!
	cmp	al,cIcoPrint		; ICO printscreen key?
	je	kbIsPrint		; if ICO, go test shift
kbTestPrint:
	cmp	al,cPrint		; IBM printscreen (Multiply) key
	jnz	kbi1
	cmp	cs:[fKbRt],0		; Yes, is it RT keyboard?
	jnz	kbibb			;  if so, go check previous proc.
kbIsPrint:
	test	ah,fShift		; shift down?
	jnz	kbi0			; let ROM handle print-screen
	jmp	short kbia
; RT keyboard: kbd_proc now contains prev_was_ext_proc, and shift doesn't
; need to be down..
kbibb:
	cmp	cs:[kbd_proc], dataOFFSET prev_was_ext_proc
	jnz	kbia
	or	ds:byte ptr[ShiftState],fLshift	; force Left Shift
	mov	cs:[kbd_proc], dataOFFSET standard_proc
jkbi0:
	jmp	short kbi0

; 27jan86 .. handle printscreen here	; ALT-PRINTSCN (e.g. screen grabber).

kbia:	mov	ah,ds:[ShiftState]	; neither shift down,
	not	ah
	test	ah,fAlt+fCtrl	; test for CTRL-ALT something...
	jnz	kbi1

; 10 nov 87: add this
	cmp	al,cDelete
	jnz	kbi0


ifdef	ENVOY ; Envoy code fragment 2

; If this is a cCCPDel on the envoy keyboard, we need to reset ourselves.
; Note: Since Envoy is operating in RAW mode, the bios won't do it for
; us.
;
	cmp	al, cCCPDel	; cCCPDel can only be generated on the Envoy
	jne	kbi0		; keyboard in RAW mode.
	mov	word ptr ds:[fReset], 1234h	;  Soft reset the system
	jmp	cs:[ResetVector]

endif	; ENVOY

ifndef W103
; (Next 3 lines copied from MS version 28 oct 87 : peterbel)
; Inform the kernel that Ctl+Alt+Del is happening so that he can inform
; any expanded memory card to reset. (Wed 21-Oct-1987 : bobgu)
	call	KbdRst
endif
kbi0:

	pop	ds
	pop	ax
	jmp	cs:[bios_proc]	; let ROM handle CTRL-ALT del, break, print
				; (or anything else)

; Read keyboard status and reset the keyboard

kbi1:
; (begin insertion from Microsoft 2.0 driver, 15 oct 87)
; For OS/2 hot key support, we need to pass the Ctl and Esc key transitions
; on to the OS/2 interrupt handler.  This will allow Ctl+Esc to take us
; to the switcher.  Since we also need to use these keys, we must simulate
; an interrupt to the previous interrupt vector so that we get control
; again when they finish.

; There are two possibilities for Alt+Esc key sequence:
;	1.     Alt(down), Esc(down), Alt(up), Esc(up)
;	2.     Alt(down), Esc(down), Esc(up), Alt(up)
;
; The only key that causes Windows grief is the Esc key.  We must ignore
; the Esc key up and down if is being used to get us to the switcher.  In
; the second case, it is easy to detect the Alt key being down and set
; the fIgnoreKey flag for both the down and up transitions.  In the first
; case, however, we don't know if the Esc(up) was used with the Alt key
; or not.  For this case, we set a flag (fIgnoreEsc) when we see the down
; of the Esc.  We then ignore any Esc(up) keys.  But to play it safe, we
; also need to clear fIgnoreEsc if we see any down transitions, which
; means that the switcher probably ate the Esc(up).

ifndef W103

	cmp	cs:[fOS2Box],0	; In the Box?
	jnz	IsOS2		; Yes
	jmp	kbi13		; No
IsOS2:		

; next 5 lines added 09dec87
; Alt key up or down?  If so, pass it through the OS/2 driver
	cmp	al,cAlt 	    ; Alt key down ?
	jz	kbi11		    ; yes, pass it through
	cmp	al,cAlt+80h	    ; Alt key up ?
	jz	kbi11		    ; yes, pass it through

	mov	cs:[fIgnoreKey],0   ; initial state
	cmp	cs:[fIgnoreEsc],0   ; ignoring Esc up?
	jz	kbi14		    ; nope..
	mov	cs:[fIgnoreEsc],0   ; clear the flag
	cmp	al,cEsc+80h	    ; up of the Esc key?
	jnz	kbi14		    ; no - Esc was lost - don't ignore key
	inc	cs:[fIgnoreKey]     ; set flag to ignore key
	jmp	kbi11		    ; and pass it to OS/2

kbi14:

; Only interested in the Escape key down if the Control key is also down.

	mov	ah,ds:[ShiftState]
	test	ah,fCtrl	    ; Control key down?
	jz	kbi10		    ; no - check for control key
	cmp	al,cEsc 	    ; Escape key down?
	jnz	kbi10		    ; nope

; Clt+Esc - we really are doing a screen switch.  Set the flag so that
; we ignore this key (but correctly set our key state...)

	inc	cs:[fIgnoreKey]     ; set the flag to ignore
	inc	cs:[fIgnoreEsc]     ; set the "ignore Esc key" flag
	jmp	kbi11		    ; and pass off to OS/2

kbi10:
	cmp	al,cCtrl	    ; Control Key down?
	jz	kbi11		    ; yes

; If fSwitchEnable is zero, the display driver is in a critical section
; and we cannot switch now.  Don't pass any up-clicks to the OS/2 keyboard
; handler.
; next 2 lines added 09dec87
	cmp	cs:[fIgnoreEsc],0   ; About to switch ?
	jz	kbi111		    ; no - pass them anyway
	cmp	fSwitchEnable,0     ; prevent screen switch?
	jz	kbi13		    ; yes - do normal stuff
kbi111:					; label added 09dec87
	cmp	al,cCtrl+80h	    ; Control Key Up?
	jz	kbi11		    ; yes
	cmp	al,cEsc+80h	    ; Esc key up
	jnz	kbi13		    ; no - do normal stuff

kbi11:
ifdef OS2Debug
	inc	Count11
endif	; OS2DEBUG
	pop	ds		    ; restore DS
	push	ds		    ; DS & AX still on stack...
	push	ax		    ; save current AX
	pushf			    ; Simulate an INT to the previous
	push	cs		    ; interrupt vector.
	lea	ax,kbi12
	push	ax
	jmp	cs:[bios_proc]
kbi12:
        xor     ax,ax
        mov     ds,ax
	pop	ax		    ; get back our AX

endif	; (Not Windows 1.0x 'W103')

kbi13:
	; (end insertion from Microsoft 2.0 driver, 15 oct 87)

	; slightly rearranged 15 oct 87 to be more like Microsoft:
	push	ax
	in	al,kb_ctl	; reset interface chip
	mov	ah,al
	or	al,80h
	out	kb_ctl,al
	xchg	ah,al
	out	kb_ctl,al
	pop	ax		; just for delay..

	push	ax
	mov	al,cs:AckByte	; acknowledge interrupt
	out	ack_port,al
	pop	ax

	mov	ah,80h			; move the high order bit to the high
	and	ah,al			; order byte.
	xor	al,ah			; turn off bit if set

	push	bx		; stack = [AX,DS,BX]

ifdef	SYSREQ
;
;  The (PC-AT) SYS REQ key is used to simulate an NMI.
;
;  This is done by clearing up the stack and doing a far jump to the NMI
;  interrupt vector location.  If the NMI interrupt points into the ROM,
;  we don't jump, since the ROM treats NMIs as parity errors.
;
; This key is recognized ONLY on the M28 (86-key) keyboard.
; On the M24, this keycode is for the 00 key.
;
	cmp	al,SYSREQ	    ; SYSREQ key?
	jne	notsys
        cmp     cs:[fOS2Box],0      ; in OS/2 compat. box ?
        jnz     notsys              ; .. yep, we're not debugging Windows!
                                    ; (CtlAltSysReq conflicts with OS/2
                                    ; usage).

; addition for RT keyboard
	; Must use CTRL ALT SCR-PRT on this keyboard, since Windows uses
	; ALT SCR-PRT for screen grab.
	cmp	cs:[fKbRt],0		; RT keyboard?
	jz	sys10			; if not, skip
	test	ds:byte ptr [ShiftState], fCtrl	; else, is Ctrl down?
	mov	al, cPrint		; change scan code
	jz	notsys
	jmp	short sysreqwait
sys10:
; end RT addition

	cmp	cs:[KeyType],2		; is this ICO keyboard?
	je	notsys			; yes, it's 00 key!!
ifdef	ERICSSON
	cmp	IsEri,1
	jne	NoEricsson
	cmp	PCType,0FCH		; Ignore if not AT-compatible
	jne	notsys
	cmp	KeyType,6
	je	notsys			; No SYSREQ on 9140 (but SyReq)
NoEricsson:
endif	; ERICSSON

sysreqwait:
	or	ah,ah		    ; wait for key-up
	js	sysreqdown
igkey:	jmp	ignorekey
sysreqdown:
	mov	ax,word	ptr ds:[2*4+2]
	mov	word ptr cs:[nmi_vector+2],ax
	cmp	ax,0F000H	    ; see if it points to ROM BIOS
	je	igkey		    ;  or, possibly ..
	; addition 21 oct 1986 plb
	cmp	ax,0070H	; does it point to DOS BIOS?
	je	igkey		;  if so, ignore this.
	; end of 21 oct 86 change

	; we now assume NMI points to SymDeb , so we simulate the NMI
	mov	ax,word	ptr ds:[2*4]
	mov	word ptr cs:[nmi_vector],ax
	pop	bx
	pop	ds
	pop	ax
	jmp	cs:[nmi_vector]

notsys:
ENDIF				    ; End of SYSREQ stuff

; Microsoft's comment in 1.03 sources and later:
; We are about to enable interrupt, before doing that we have to protect
; against ourself coming before being done.

	cmp	cs:[fReEnter],0		; are we alone?
	jz	kbiR
	jmp	ignorekey		; is he typing REALLY fast?

kbiR:	inc	cs:[fReEnter]
    ;
    ; (20 Aug 87: -- the following code is adapted from the Microsoft
    ; 2.00 keyboard driver KEYBOARD.ASM)
    ;
    ; Microsoft's NOTE:
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

ifdef BIGSTK

; Modification to allow apps with small stack sizes not to blow up
; (i.e. diskcopy) (HP c-ralphp 6/9/88)
;
;
; Switch to a private stack
;
	mov	cs:SaveSS, ss
	mov	cs:SaveSP, sp
	mov	cs:NextSS, cs
	mov	ss, cs:NextSS
	mov	sp, dataOFFSET Stack

; end (HP c-ralphp modifications)
endif ; BIGSTK

	sti				; interrupts on for others...

;;	push	bx			; we push/pop BX elsewhere
	xor	bh,bh
	call	cs:[kbd_proc]	; standard_proc or prev_was_ext_proc
;;	pop	bx


ifdef BIGSTK
; Modification to restore the original stack (HP c-ralphp 6/9/88)
;
; Restore the old stack
;
	cli
	mov	ss, cs:SaveSS
	mov	sp, cs:SaveSP

; end (HP c-ralphp modifications)
endif ; BIGSTK

	mov	cs:[fReEnter],0		; unlock keyboard
;
; Re-enable keyboard INTs at the interrupt controller
;
	cli				; Ints off again. NOTE: don't need
					; to turn them back on because we are
					; going to to an IRET when we finish.
	in	al,21h			; get IRQ mask register
	and	al,NOT 02h		; turn on kybd ints again (IRQ 1)
	jmp	$+2			; I/O delay for AT type machines
	jmp	$+2
	out	21h,al			; restore correct mask
	jmp	$+2			; I/O delay for AT type machines

ignorekey:

	pop	bx
	pop	ds
	pop	ax
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

standard_proc proc near

	call	SetShiftState		; Microsoft does this here ...

; [next 3 instructions added 06jan88 ]
; If used key as OS/2 screen switch, don't do anything except set the
; shift state.
	cmp	cs:[fIgnoreKey],0
	jz	noIgnore
	jmp	stdproc_end
noIgnore:

	cmp	cs:[fKbRt],0		; is this RT keyboard?
	jz	noSpecial

;; The following block of code is for RT keyboard only!!

	cmp	al,cExtended		; is this Extended scan code?
	jne	noPrevExt		; if so, next keyboard int handled
	mov	cs:[kbd_proc],dataOFFSET prev_was_ext_proc	; by this..
	jmp	stdproc_end

noPrevExt:

; code to handle ShiftLock
; This is considerably different from the implementation in the Microsoft
; driver, though the first few instructions are somewhat similar.  Here,
; a virtual VK_OEM_SHIFTLOCK key is used, which ToAscii will later
; interpret.

	test	cs:[RTFlags],kbShiftLock	; ShiftLock for this KB?
	jz	noSpecial		; no, normal Caps Lock instead.
	cmp	al,cCapsLock		; yes -- is this Capslock key?
	jnz	noCaps
	cmp	ah,0			; up or down?
	jnz	shiftUp
	mov	al,VK_OEM_SHIFTLOCK	; send code for mythical caps lock
	jmp	kbi4
shiftUp:				; CapsLock up .. do nothing
	jmp	stdproc_end
noCaps:					; not Caps Lock key
	cmp	al,cLShift		; is (non-extended) Left Shift?
	je	isShift
	cmp	al,cRShift
	jnz	noShift
isShift:				; it's shift key, clear Caps Lock
	or	ah,ah			; is it make?
	js	noShift			; skip if not

	push	ax			; it's shift, so fake virtual
	push	bx			; CAPSLOCK going UP...
	mov	ax,VK_OEM_SHIFTLOCK+8000H
	call	cs:[event_proc]
	pop	bx
	pop	ax			; continue processing SHIFT key..

noShift:

;; End of special code for RT keyboard ...

noSpecial:
; (code up to nosp1: added 07 jan 88 from Microsoft KEYBOARD.ASM)
; If the 2 shift keys are down, when the 1rst is released we need
; to keep Shift State down, so we'll ignore the first Shift break.
; Remember: we already called SetShiftState that may have reset
;           the Shift state we are interested in, Yeerk ! 
;
        test    byte ptr ds:[ShiftState],fShift
        jz      cbd0                ; no Shift down
        or      ah,ah               ; is it break ?
        jns     cbd0                ; no
        cmp     al,cLShift          ; LeftShift ?
        jz      nosp1               ; yes, ignore
        cmp     al,cRShift          ; RightShift ?
        jnz     cbd0                ; no, skip
nosp1:  jmp     stdproc_end         ; yes, ignore
cbd0:

ifdef	ERICSSON
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
;;	mov	al,1CH			; Make it normal Enter
NotExtraEnter:
endif	; ERICSSON

;
; Translate scan code to virtual key
; First, is it control-numlock or control-break?
;
	test	byte ptr ds:[ShiftState],fCtrl	; is it Ctrl?
	jz	kbi2

;; test for control-numlock:
 	cmp	al,cNumLock
	jnz	kbi1a
ifdef	ERICSSON
	cmp	KeyType,6			; 9140 is special case
	jne	kbi1_0
	test	BYTE PTR ds:[ProtocolFlag],$PrefixFlag
	jz	kbi2
	and	BYTE PTR ds:[ProtocolFlag],NOT $PrefixFlag
	add	al,080H
kbi1_0:
endif	; ERICSSON

	; Control + Numlock translated to VK_PAUSE ..
	mov	bx,ax
	mov	al,VK_PAUSE
	jmp	kbi4			; (no longer short)

kbi1a:					; it's NOT NumLock

	cmp	al,cBreak	    ; is it Break?
	jnz	kbi2
ifdef	ERICSSON
	cmp	KeyType,6			; 9140 is special case
	jne	kbi1_1
	test	BYTE PTR ds:[ProtocolFlag],$PrefixFlag
	jz	kbi2
	and	BYTE PTR ds:[ProtocolFlag],NOT $PrefixFlag
	add	al,080H
kbi1_1:
endif	; ERICSSON
	mov	cs:[fBreak],0
	mov	bx,ax
	call	CtlBrkCase	    ; Test for CtlBreak
	mov	al,VK_CANCEL
	cmp	cs:[fBreak],0
	jnz	tokbi4
	jmp	stdproc_end	;IgnoreKey -- ignore if somebody is trapping us
tokbi4:
	jmp	kbi4

kbi2:	xor	bx,bx			; it's NOT Break
	mov	bl,al			; save scan code

ifdef	ENVOY ; Envoy code fragment 3

;
; If we are using an envoy keyboard and
; if it is an HP Cursor pad key or an HP softkey scancode,
; use the HP table.
;
	test	cs:fVectra, FV_ENVOY
	jz	env_cont
	cmp	bl, cCCP_UP
	jb	env_cont
	cmp	bl, cf8
	ja	env_cont
	mov	al, cs:[bx+HPTransTable-cCCP_UP]
	jmp	kbi4
env_cont:				; it's not, continue

endif ; ENVOY

ifndef	ERICSSON
	mov	al,-1
	cmp	bl,byte ptr KeyTransTblSize	; dw in Olivetti version
	jb	kbi2a
	jmp	kbi4
kbi2a:

else
	cmp	IsEri,1
	jne	NotEnter
	test	BYTE PTR ds:[ProtocolFlag],$PrefixFlag
	jz	NotPrefixed
	and	BYTE PTR ds:[ProtocolFlag],NOT $PrefixFlag
	add	bl,080H
NotPrefixed:
	cmp	BYTE PTR ds:[KeyBoardId],ID1051
	jne	NotEnter
	cmp	bl,4Eh
	jne	NotEnter
	mov	bl,78H
NotEnter:
endif

	push	si
	mov	si,KeyTransBase
	mov	al,cs:[bx+si]			; get the translation
	pop	si

Translated:

ifdef	ERICSSON
	cmp	KeyType,6
	je	CheckNumPad
endif	; ERICSSON

	; Check for ICO keyboard.

	cmp	cs:[KeyType], 2			; check to see
	jne	CheckNumPad	; [ CheckF1116 ]; if it's an ICO keyboard..
	cmp	al, VK_ICO_00			; is this ICO 00 key?
	jne	IcoTranslate			; it is 00 key...
	mov	al, VK_0			; change to VK_0
	call	cs:[event_proc]			; send first 0
	or	ah,ah				; make or break?
	js	TrToKbi4
	xor	ah,80h				; it's make, so
	call	cs:[event_proc]			; fake key going up
	xor	ah,80h				; and down again
						; then send second 0...
TrToKbi4:
	jmp	kbi4				; go send char codes in AX,BX

IcoTranslate:
	cmp	bl,cIcoPrint			; is it a special ICO key?
	jb	CheckNumPad			; (in range 55h..5fh)
	cmp	bl,cIcoDivide
	ja	CheckF1116			; might be F11..F18..

	; Translate scan code to normal IBM range so braindamaged WINOLDAP
	; will recognize these keys!
	push	si
	mov	si,dataOFFSET IcoTransT
	sub	bx,cIcoPrint
	mov	bl,cs:[bx+si]
	pop	si
	jmp	kbi4

; ====== Handle function keys F11, F12, .. F18 on ICO keyboards =====
; Scan codes as well as VK_ codes are mapped into those for F1..F10,
; with shift state set to give the right translation.

CheckF1116:
	cmp	al, VK_F11			; 2 ranges: VK_F11..VK_F16 and
	jb	CheckNumPad			; VK_F17..VK_F18
	cmp	al, VK_F16
	ja	CheckF1718
						; it's F11..F16
	sub	al, (VK_F11-VK_F1)		; Map into VK_F1..VK_F6
	mov	bl, (cF1-VK_F1)			; compute corresponding
	add	bl, al				; scan code
	jmp	short DoF11F18

CheckF1718:
	cmp	al, VK_F17
	jb	CheckNumPad
	cmp	al, VK_F18
	ja	CheckNumPad
						; it's F17 or F18.
	sub	al, (VK_F17-VK_F7)		; map into VK_F7..VK_F8
	mov	bl, (cF7-VK_F7)			; compute scan code
	add	bl, al

DoF11F18:
	test	byte ptr ds:[ShiftState],fShift	; any shift key down?
ifndef	ERICSSON
	jnz	kbi4				; yes, already shifted.
						; no, we have to fake it..
else
	jnz	TrToKbi4
endif
	push	ax
	push	bx
	mov	ax,VK_SHIFT			; push imaginary shift key
	mov	bl,54
	call	cs:[event_proc]
	pop	bx
	pop	ax
	call	cs:[event_proc]			; send the function key
	mov	ax,VK_SHIFT+8000H
	mov	bl,54
	jmp	short kbi4			; go release imaginary shift

; ====== Handle keys on numeric pad ============
CheckNumPad:

	xor	bh,bh				; is it on numeric keypad?
ifdef	ERICSSON
	cmp	KeyType,6			; 9140 is special case
	jne	Not9140NumPad
	cmp	bl,71 + 080H			; NumPad has prefix
	jb	kbi4
	cmp	bl,83 + 080H
	ja	kbi4
	jmp	SHORT isNumPad
Not9140NumPad:
endif	; ERICSSON
	cmp	bl,71
	jb	kbi4
	cmp	bl,83
	ja	kbi4

isNumPad:					; yes...
	test	byte ptr ds:[ShiftState],fNum	; is num lock down?
	jz	kbi4				; no, do normal processing
	test	byte ptr ds:[ShiftState],fShift ; either shift key down?
	jnz	kbi3				; yes, weird stuff

	mov	al,bh
	or	al,al
	jnz	kbi4

	push	si
	mov	si,KeyNumBase
ifdef	ERICSSON
	cmp	KeyType,6			; 9140 still has prefix
	jne	Not9140				;	in scancode
	mov	al,cs:[bx+si-(71 + 080H)]	; new: movable table
	pop	si
	jmp	SHORT kbi4
Not9140:
endif	; ERICSSON
	mov	al,cs:[bx+si-71]		; new: movable table
	pop	si

	jmp	short kbi4

kbi3:	push	ax
	push	bx
	mov	ax,VK_SHIFT+8000H
	mov	bl,54
	call	cs:[event_proc]
	pop	bx
	pop	ax
	call	cs:[event_proc]	; send actual key...
	mov	ax,VK_SHIFT	; pretend shift key went down
	mov	bl,54
;
; Call windows with ah == 80h for uptrans, 00h for downtrans
; al = virtual key, bl = scan code
;
; Windows preserves all registers
;
kbi4:

ifdef	ENVOY ; Envoy code fragment 4


;!!! if Vectra, down transistion and not a modifier or a lock, key click

	test	cs:fVectra, FV_A	; changed to TEST 23jun88 peterbe
	jz	no_click
	cmp	cs:[fModifier],	0
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
	call	cs:[event_proc]
	pop	ax
	push	bp
	mov	ah, F_SND_CLICK
	mov	bp, V_SYSTEM
	int	6fh
	pop	bp
	jmp	stdproc_end
no_click:

endif	;ENVOY

ifndef	ERICSSON
	call	cs:[event_proc]
else	; not ERICSSON

; The following code handles some extra keys on the Ericsson 9140
;	keyboard. The keys are EnlargeWindow, Jump, and BackTAB.
;	EnlargeWindow will not work if/when the Alt-F10 accelerator
;	key is removed.


	cmp	KeyType,6			; All extra keys are on
	jne	kbi99				;	9140 keyboard
        test    byte ptr ds:[ShiftState],fAlt	; is Alt already down?
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
	push    ax
        push    bx
        mov     ax,VK_MENU
        mov     bl,38H
        call    cs:[event_proc]
        pop     bx
        pop     ax
        call    cs:[event_proc] ; send actual key...
        mov     ax,VK_MENU+8000H     ; pretend alt key went up
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
        test    byte ptr ds:[ShiftState],fShift  ; is Shift already down?
	jz	kbi98a
	jmp	kbi3				; Shift BACKTAB is TAB
kbi98a:
	push    ax
        push    bx
        mov     ax,VK_SHIFT
        mov     bl,54
        call    cs:[event_proc]
        pop     bx
        pop     ax
        call    cs:[event_proc] ; send actual key...
        mov     ax,VK_SHIFT + 8000H     ; pretend shift key went up
        mov     bl,54
kbi99:
        call    cs:[event_proc]

endif	; (ifndef) ERICSSON

stdproc_end:

	ret


standard_proc endp

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
;	by this (@#$) keyboard anduses the extended prefixes to
;	distingush between normal keys and new (RT) keys.
;
;***********************************************************

prev_was_ext_proc proc near

	mov	cs:[kbd_proc], dataOFFSET standard_proc
	cmp     al,cLShift          ; is it extended Left Shift ?
	jz	LeavePrevProc        ; if yes, eat it
	cmp	al,cRShift	    ; else is it extended Right Shift ?
	jz	LeavePrevProc        ; if yes, eat it

; Test if we receive PrintScreen. Remember the make code has been eaten
; before we get a chance to be called. The break code will release the
; temporarily LeftShift that was then set.

	cmp	al,cPrint
	jnz	prev10
	xor	ds:byte ptr[ShiftState],fLshift
LeavePrevProc:				; Add label 04dec87
	jmp	short prevproc_end

prev10:
        call    SetShiftState
	mov	bl,al		    ; bl= scancode (all along proc)

; Divide key special case: if Slash is found a VK_DIVIDE is sent
;
	cmp	al,cSlash	    ; is it extended Slash ?
	jnz	prev20		    ; if not, skip
	mov	al,VK_DIVIDE	    ; else send VK_DIVIDE
	jmp     short prev40

; When ShiftLock is on, we don't want to be get our VK_code shifted (none
; of our code correspond to alphanumeric), so we have nothing to do.
; We would have one exception (VK_DIVIDE), but we already took care of it.

; Break case
;
prev20:
	cmp	al,cBreak	    ; is it extended Break ?
	jnz	prev30		    ; if not, skip
        mov     cs:[fBreak],0
        call    CtlBrkCase          ; Test for CtlBreak
        mov     al,VK_CANCEL
        cmp     cs:[fBreak],0
        jnz     prev40
        jmp     short prevproc_end  ; ignore if somebody is trapping us

; Standard case, convert to our virtual key
;
prev30:
ifndef	ERICSSON
        mov     al,-1
;;;	(Olivetti change:)
        cmp     bl,byte ptr KeyTransTblSize	; test if scancode in table
        jae     prev40				; if not, skip (VK= -1)
else
;;	This code not used at the moment
;;	Just put here as a reminder
  if  0
	cmp	IsEri,1
	jne	prev30NotPrefixed
	test	BYTE PTR ds:[ProtocolFlag],$PrefixFlag
	jz	prev30NotPrefixed
	and	BYTE PTR ds:[ProtocolFlag],NOT $PrefixFlag
	add	bl,080H
prev30NotPrefixed:
  endif
endif
;;;	(Olivetti code next 4 lines:)
	push	si
	mov	si,KeyTransBase			; else, get the translation
	mov     al,cs:[bx+si]
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
	test	cs:[RTFlags],kbAltGr	; does this KB do this?
	jz	prev60			; skip on if not..
	test	byte ptr ds:[ShiftState],fCtrl	; is the real control key down?
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
	push	bx			; (but not set ShiftState bit!!)
	mov	al,VK_CONTROL		; [AH] tells whether fake ctrl key
	mov	bl,cCtrl		;  is going up or down..
	call	cs:[event_proc]		; we have control (key)
	pop	bx			; get ALT key parameters back,
	pop	ax			; and now tell Windows about that...
;
; Call windows with ah == 80h for uptrans, 00h for downtrans
; al = virtual key, bl = scan code
;
; Windows preserves all registers
prev60:
	call    cs:[event_proc]
prevproc_end: 


	ret

prev_was_ext_proc endp

CtlBrkCase PROC	NEAR

; we are now control-breaking

	push	bx		; save registers we use
	push	cx
	push	dx
	push	si
	push	di
	push	es
	push	bp

	INT	1bh

	pop	bp
	pop	es
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	ret
CtlBrkCase ENDP


	public	INT1BHandler
INT1BHandler PROC FAR
	inc	cs:[fBreak]
	iret
INT1BHandler ENDP


;
;  Keep accurate track of shift state byte at 40:17H
;
SetShiftState proc near

ifdef	ENVOY ; Envoy code fragment 5
	mov	cs:[fModifier],0
endif	; ENVOY

	mov	bl,fLshift
	cmp	al,cLshift
	jz	ccv4
	mov	bl,fRshift
	cmp	al,cRshift
	jz	ccv4
	mov	bl,fCtrl
	cmp	al,cCtrl
	jz	ccv4
	mov	bl,fAlt
	cmp	al,cAlt
ifdef	ERICSSON
	jz	ccv4
	cmp	KeyType,6			; 9140 is special case

ifdef ENVOY	; (and ERICSSON)  Envoy code fragment 6
	jne	ccv3
else
	jne	ccv6
endif	; (not) ENVOY

	cmp	al,055H				; The right Alt key
endif	; ERICSSON

	jnz	ccv6


ifdef	ENVOY ; Envoy code fragment 7
	jz	ccv4
ccv3:
	mov	cs:[fModifier],-1
	jmp	ccv6

else	; not ENVOY

	jnz	ccv6
endif	; (not) ENVOY

ccv4:	or	ah,ah
	jns	ccv5
	not	bl
	and	byte ptr ds:[ShiftState],bl
	ifdef OS2Debug
	 mov	bl,ds:[ShiftState]	; xxx for debug
	 mov	cs:myShifts,bl		; xxx for debug
	endif
	ret
ccv5:	or	byte ptr ds:[ShiftState],bl
ccv6:
	ifdef OS2Debug
	 mov	bl,ds:[ShiftState]	; xxx for debug
	 mov	cs:myShifts,bl		; xxx for debug
	endif
	ret

SetShiftState endp

sEnd DATA

if2
%out	END Datacom.inc
%out
endif

END

