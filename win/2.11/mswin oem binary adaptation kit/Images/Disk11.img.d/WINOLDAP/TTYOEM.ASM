page ,132
;-------------------------------------------------------------------------
;
; MODULE TTYOEM.ASM -	Very OEM specific WINOLDAP code.
;			The actual handlers and assoc code
;			for old app keyboard and display emulation
;
;			$Author:   tqn  $
;			$Revision:   1.34  $
;			$Date:   16 Nov 1987  0:42:12  $
;

	.xlist
	include cmacros.inc
	include tty.inc
	include pdb.inc
	include macro.inc
	.list
	include grabber.inc

;========  HP Adaptation  =====================================================
;
;  Change the Int16Handler routine to process calls to the HP EX-BIOS
;
;  Modify Int12Handler, Int09Handler, and Int16Handler to deal with the
;  context switching in the extended mode (i.e. Windows swapped)
;
;  Add HP notification procedures when context switching a cooperating
;  DOS application. Without V_WINDOWS installed, disallow datacomm
;  programs to context switch if we are in the extended mode.
;
;  Change the Translate table to support the extra horizontal softkeys on Vectra
;
;  Author:
;    Tuan Nguyen
;
;  History:
;			09/20/86	Integrate new Int10handler.
;					Integrate V1.03
;			03/05/87	Integrate with the documented version of
;					Winoldap.
;
;==============================================================================

page

GetInfo 	equ	0fffeH		; Extended grabber call ~~TN 082086~~
cAlt		equ	038h
cTab		equ	15
kEsc		equ	1
cSpace		equ	37
cBreak		equ	080h
cDup		equ	0E0H		; Duplicate key predecessor code

KeyState	equ	417h
RSHIFT		equ	000000001b
LSHIFT		equ	000000010b
SHIFT		equ	000000011b
CTRL		equ	000000100b
ALT		equ	000001000b

;-----------------------------------------------
;
; External Windows procedures
;
externFP	<GetCurrentPDB,AnsiToOem>

;-----------------------------------------------
;
; External WINOLDAP procedures
;
externNP	GoFunc
externNP	GoWindows
externNP	GoGrab
externNP	RestoreMark
externNP	BadBeginPaint
externNP	BadEndPaint
externNP	GoMarkGrab
externNP	GoGetGrab
externNP	CanWeDoMenus
externNP	ExecuteMacro
externNP	DDESwitchOut

externW 	BufPara
externW 	BufSeg
externW 	BufSize
externW 	GrabSeg

;V1.03
externFP	GetCurrentTask
externNP	NewCur
externW 	RowCol
externW 	OrgVidCursLook
externD 	VidAddr

ExternW 	fEGA			; (TTYTASK) ~~TN 081486 ~~
ExternB 	pifBehavior		; (TTYWIN)
ExternB 	pifMsFlags		; (TTYWIN)
ExternW 	WinoldapDS		; (TTYDOS)
ExternW 	fExtend 		; (TTYEXT)
ExternB 	fVectra 		; (TTYTASK)
ExternB 	fHPGrab 		; (TTYTASK) ~~TN 082086~~
ExternW 	HPVectorTable		; (TTYSYS)
ExternW 	fMenu
ExternW 	fMenuActive
ExternW 	fPasteActive
ExternD 	Caller_Stack		; (TTYDOS) ~~TN 082086 ~~
ExternB 	fControlMenu		; (MENUDISP) ~~RCP 050487~~
ExternW 	GrabVersion
ExternB 	GrabReq
ExternB 	GrabData
ExternD 	pErrmode		; ~~TN 092086~~ <> 0: inside DOS
					; critical error hanlder
ExternD 	pInDOS			; ~~TN 092086~~ <> 0: DOS cannot be
					; reentered
ExternB 	fATIIBIOS		; is the AT II BIOS installed

sBegin		data
		assumes ds,data

ExternW 	OldApState		; State of the App	(TTYWIN)

AppfSwitch	db	0		; Private copy of fSwitch
		db	0		; Word aligned

sEnd		data

sBegin		code
		assumes cs,code
		assumes ds,data

	public	EnableMemory,DisableMemory
	public	EnableKeyboard,DisableKeyboard
	public	EnableVideo,DisableVideo
	public	EnableSwitcher,DisableSwitcher
	public	EnableTopView, DisableTopView
	public	GetKey,SetMode, ROMKeyboard
	public	EnableIntMask, CheckInts
	public	Int09Handler, SwallowKey
	public	CheckCom,TranslateKey, TranslateNumpad, CheckMarkStatus
	public	CheckNotify
	public	SetMenuMode
	public	RestoreVector, SetVector
	public	i9Switch, FinishSwitchMacro

; Debugging
	public	CheckApp
	public	ScreenGrab, int16handler
;***

GlobalB 	fWOAGrab,0	;If not zero, WOA int 9 handles screen grabs
GlobalB 	LastAlt,0	;shows Alt make code was last key
GlobalB 	AltSet,0	;shows that an alt make/break happened seq.
GlobalW 	fHotKey,0	;Incremented for each Alt+Tab

ExternW 	fMark
ExternW 	fEndBlock
;
; Storage for the various INT handlers we change
;
globalD 	prevInt09Proc,?
globalD 	prevInt10Proc,?
globalD 	prevInt12Proc,?
globalD 	prevInt15Proc,?
globalD 	prevInt16Proc,?

;
; Storage for several ROM BIOS things we modify
;
globalW 	EquipmentFlag,?
globalW 	CrtState,?

;
; Contains the Windows PDB, this is used to check for calls from outside Windows
;
globalW 	TopPDB,?

;
; Switcher flag - Non zero indicates that a switch has been requested but not
;			performed.
;
globalB 	fSwitch,?
globalB 	InROM,0

;
; Beep flag - Set when we want to do syncronous beep
;
globalB 	fBeep, 0
staticB 	,?

;**
;
; EnableIntMask - Make sure the right interrupts are enabled
;
;	This routine writes a mask to the 8259 int controller
;	to make sure the right set of IRQs is enabled.
;	This routine is called after an app in a Windows Exits.
;	Some apps are rude about "dirtying" up the mask and not
;	cleaning up after themselves.
;
; ENTRY:
;	None
; EXIT:
;	Reasonable mask to 8259
; USES:
;	AL,FLAGS
; SEE ALSO:
;	IBM PC Tech Ref Manual
;
EnableIntMask:
	cli
	in	al,21H
	and	al,11100111b
	out	21h,al
	sti
	ret

;**
;
; SetVector RestoreVector - Helper routines for doing INT vectors
;
; SetVector
;	ENTRY:
;	    CS:DX points to new handler
;	    AL = INT vector number
;	EXIT:
;	    Vector set
;	    fSwitch cleared!!!!!!!!!
;	USES:
;	    AX,DX,DS
; RestoreVector
;	ENTRY:
;	    DS:DX points to new handler
;	    AL = INT vector number
;	EXIT:
;	    Vector set
;	    fSwitch cleared!!!!!!!!!
;	USES:
;	    AX,DX
;
SetVector:
	push	cs
	pop	ds
RestoreVector:
	mov	ah,25h
	int	21h
;!! HP
; Reset fswitch in Enable/DisableSwitcher since fswitch is only valid for
; switchable BAD applications
;
;      mov     cs:[fSwitch],SW_CLEAR	   ; clear switch flag if
;*****
	ret


;--------------------------------------------------------------
;
;    Memory - INT 12
;
;**
;
; EnableMemory - Turn on our INT 12 hook for mem size
;
; ENTRY:
;	CurrentPDB set to correct value for TopPDB
; EXIT:
;	Current INT 12 vector stored in prevInt12proc
;	Our INT 12 handler installed
;	TopPDB set to current task when routine called
; USES:
;	AX,BX,DX,ES
;
EnableMemory:
	push	ds
	cCall	GetCurrentPDB
	mov	cs:[TopPDB],dx		; get value of top pdb

	mov	ax,3512h
	int	21h
	mov	word ptr cs:[prevInt12proc],bx
	mov	word ptr cs:[prevInt12proc+2],es

	mov	dx,codeOffset Int12Handler
	mov	al,12h
	call	SetVector
	pop	ds
	ret

;**
;
; DisableMemory - Turn off our INT 12 hook for mem size
;
; ENTRY:
;	None
; EXIT:
;	INT 12 vector stored in prevInt12proc restored
;	Current INT 12 vector discarded
; USES:
;	AX,DX
;
DisableMemory:
	push	ds
	lds	dx,cs:[prevInt12Proc]
	mov	al,12h
	call	RestoreVector
	pop	ds
	ret


;**
;
; Int12Handler - INT 12 handler for app memory size requests
;
;	What we do is return a size in Kilobytes consistent
;	with the size of the partition we allocated to the app
;
; ENTRY:
;	CurrentPDB set to app making INT 12 reguest
; EXIT:
;	AX = size of mem in Kilo bytes
; USES:
;	AX
; SEE ALSO:
;	IBM PC Tech Ref Manual
;
Int12Handler	proc	far
	push	ds
	push	BX			;!! HP
	push	cx
	push	dx
;!! HP

	cmp	CS:[fExtend], 1 	; Are we in the extended mode ?
	jne	Int12H1 		; No, do the Windows way
	mov	AH, 51H 		; Get current PDB using DOS Caveat call
	int	21H			; BX= Current PDB
	mov	AX, BX
	jmp	Int12HCont

Int12H1:
	cCall	GetCurrentPDB		; return segment of current program

Int12HCont:
;******
	mov	ds,ax
	mov	ax,word ptr ds:[2]	; get last segment of program ...
	mov	cl,6			; ...see page e-8 of dos 2.0 manual
	shr	ax,cl			; convert to kilobytes
	pop	dx
	pop	cx
	pop	BX			;!! HP
	pop	ds
	iret
Int12Handler	endp
;--------------------------------------------------------------
;
;    Keyboard - INT 16
;
;**
;
; EnableKeyboard - Turn on our INT 16 hook for keyboard emulation
;
; ENTRY:
;	None
; EXIT:
;	Current INT 16 vector stored in prevInt16proc
;	Our INT 16 handler installed
; USES:
;	AX,BX,DX,ES
;
EnableKeyboard:
	push	ds
	mov	ax,3516h
	int	21h
	mov	word ptr cs:[prevInt16proc],bx
	mov	word ptr cs:[prevInt16proc+2],es

	mov	dx,codeOffset Int16Handler
	mov	al,16h
	call	SetVector
	pop	ds
	ret

;**
;
; DisableKeyboard - Turn off our INT 16 hook
;
; ENTRY:
;	None
; EXIT:
;	INT 16 vector stored in prevInt16proc restored
;	Current INT 16 vector discarded
; USES:
;	AX,DX
;
DisableKeyboard:
	push	ds
	lds	dx,cs:[prevInt16Proc]
	mov	al,16h
	call	RestoreVector
	pop	ds
	ret


;**
;
; GetKey - Routine to call INT 16 handler in before WINOLDAP
;
;	This routine is used to get a key if one is available
;	without using the WINOLDAP Int16Handler. This routine
;	also performs a switch to Windows if one is pending
;	(fSwitch set).
;
;	One of the primary uses of this routine is to get keys
;	when an app IS NOT running in a Window (Windows "disabled")
;
; ENTRY:
;	fSwitch may be set
;	prevInt16proc must be something reasonable
; EXIT:
;	Switch performed and fSwitch cleared if fSwitch was set
;	Zero set
;		No key available
;	Zero re-set
;		AL is key
;		AH is scan code
;		Key is removed from keyboard buffer
; USES:
;	AX,BL,FLAGS
;
; NOTES:
;	If an AT II extended keyboard is installed on the system this
;	routine will get all keys using Int 16H functions 10H and 11H.
;	These keys are placed in the instance data area keyboard buffer.
;	PeekKey and RemoveKey are responsible for filtering out any
;	extended keycodes if the app does not use the extended key
;	functions for the AT II keyboard.
;
GetKey: call	SwitchToWindows 	; we may want to switch to windows
	mov	BL,1
	test	CS:fATIIBIOS,0FFH	; Do we have an AT II Compatable BIOS
	jz	GetKey10
	add	BL,10H
GetKey10:
	mov	ah,bl			; ask for status
	call	ROMKeyboard
	jz	gkx			; keyboard empty, return 0
	mov	ah,bl
	dec	ah
	call	ROMKeyboard
gkz:
	or	bl,1			; clear zero flag
gkx:	ret


;**
;
; ROMKeyboard - Routine to call INT 16 handler in before WINOLDAP
;		  directly
;
;	This routine works exactly like INT 16, it goes around the
;	WINOLDAP Int16Handler.
;
; ENTRY:
;	prevInt16proc must be something reasonable
;	Regs as for INT 16
; EXIT:
;	As for INT 16
; USES:
;	As for INT 16
;
ROMKeyboard:
	pushf
	call	cs:[prevInt16proc]
	ret


;**
;
; Int16Handler - Handler for int 16
;
; ENTRY:
;	AH = 0
;		Read next key
;	AH = 1
;		Char available to read?
;	AH = 2
;		Return shift status
; EXIT:
;	input AH = 0 or 10H
;		AL is key
;		AH is scan code
;	input AH = 1 or 11H
;		Z set
;		    No char available
;		Z re-set
;		    char available
;		    AL is key
;		    AH is scan code
;	input AH = 2 or 12H
;		Shift status in AL
; USES:
;	AX,FLAGS
; SEE ALSO:
;	IBM PC Tech Ref Manual
; NOTES:
;	Functions 10H through 12H return with the regs unchanged if
;	an AT II keyboard is not installed.  Otherwise Func31, Func32 or
;	the ROM Int 16H handler is called to process these requests.
;
Int16Handler	proc	far
	push	bp
	mov	bp,sp
	mov	bp,[bp+4]	; code segment of caller outside windows?
	cmp	cs:[TopPDB],bp
	pop	bp
	ja	int6$A		; Yes, go to old handler
	sti
	cmp	ah,2		; go straight to ROM for shift status
	je	int6$A
	cmp	ah,12H
	jne	int6$0
int6$A: jmp	cs:[prevInt16Proc]	; Let old handler handle the request

int6$0:
	push	bp
	push	es
	push	si
	push	di
	push	dx
	push	cx
	push	bx

	cmp	ah,1		; get status?
	je	int6$1A
	cmp	ah,11H
	jne	int6$1
	test	CS:fATIIBIOS,0FFH
	jz	int6$2
	mov	ah,32
	jmp	int6$1B
int6$1A:
	mov	ah,11		; ask for status
int6$1B:
	call	GoFunc		; go get it
	or	al,al		; set zero flag if buffer emtpy
	mov	ax,dx		; load possible key into ax
	pop	bx
	pop	cx
	pop	dx
	pop	di
	pop	si
	pop	es
	pop	bp
	ret	2

int6$1: or	ah,ah		; get key?
	jz	int6$2A
	cmp	ah,10H
	je	int6$1C
	add	SP,14		; Make stack right, didn't mod any regs
				; if we got here
	jmp	int6$A		; Let BIOS do the work.
int6$1C:
	test	CS:fATIIBIOS,0FFH
	jz	int6$2
	mov	ah,31
	jmp	int6$2B
int6$2A:
	mov	ah,6
int6$2B:
	call	GoFunc

int6$2: pop	bx
	pop	cx
	pop	dx
	pop	di
	pop	si
	pop	es
	pop	bp
	iret

Int16Handler	endp

;--------------------------------------------------------------
;
;    Keyboard - INT 09
;
;**
;
; EnableSwitcher - Turn on our INT 09 hook for app switching
;
; ENTRY:
;	None
; EXIT:
;	Current INT 09 vector stored in prevInt09proc
;	Our INT 09 handler installed
; USES:
;	AX,BX,DX,ES
;
EnableSwitcher:
	push	ds
	mov	ax,3509h
	int	21h
	mov	word ptr cs:[prevInt09proc],bx
	mov	word ptr cs:[prevInt09proc+2],es
;!! HP
;
; During a context switch, fSwitch will be set until Gowindows has been
; successfully executed. This will guard us from executing nested calls
; to "GoWindows" when the user keeps on banging on the Alt-Tab or Alt-Enter
; key.

	mov	AL, SS:[AppfSwitch]	; First time: AppfSwitch = 0
	mov	CS:[fSwitch], AL	; Allow BAD app switching if first time in
	mov	CS:[fMenu], 0		; Reset pending menu request

;*****
	mov	dx,codeOffset Int09Handler
	mov	al,09h
	call	SetVector
	pop	ds
	ret

;**
;
; DisableSwitcher - Turn off our INT 09 hook for app switching
;
; ENTRY:
;	None
; EXIT:
;	INT 09 vector stored in prevInt09proc restored
;	Current INT 09 vector discarded
; USES:
;	AX,DX
;
DisableSwitcher:
	push	ds
;!! HP
	mov	AL, CS:[fswitch]	; save switch flag to private data
	mov	[AppfSwitch], AL
;*****
	lds	dx,cs:[prevInt09Proc]
	mov	al,09h
	call	RestoreVector
	pop	ds
	ret

;**
;
; Int09Handler - INT 09 handler to intercept WinOldAp keys
;
;	[Alt] and [Alt-Space] enter the old application into menu mode.
;	[Alt-Tab] is the hotkey which allows a quick context switch to
;	another application. [Alt-Esc] performs a context switch.
;	All other keystrokes are passed on to the previous INT 9 handler.
;
; ENTRY:
;	None (INT handler)
; EXIT:
;	MAY switch out of old app if INT CS is in old app partition
;	  Otherwise switch is posted for later by setting fSwitch
; USES:
;	None (INT Handler)
;
Int09Handler:
	push	ds
	push	ax
	xor	ax,ax
	mov	ds,ax
   ;
   ; Read in the keycode. Switch to the approriate
   ; subsection based and keycode and Alt state
   ;
	in	al,60h				;get the key
   ;
   ; Pass on the E0 code
   ;
	cmp	al, cDup
	je	i9CallOld
   ;
   ; Look for Alt breaks
   ;
	cmp	al, cAlt or cBreak
	je	i9altbreak
   ;
   ; Set the LastAlt flag
   ;
	mov	cs:LastAlt, al
   ;
   ; Look for Alt makes
   ;
	cmp	al, cAlt
	je	i9altmake
   ;
   ; Look for Alt combinations
   ;
	test	byte ptr ds:[KeyState], ALT	; is ALT the only control
	jz	i9CheckMark			;      key down?
	test	byte ptr ds:[KeyState],CTRL	; No ctrl+Alt combos
	jnz	i9CheckMark

	cmp	al, 15
	je	i9tab
	cmp	al, kEsc
	je	i9Escshrt
	test	byte ptr ds:[KeyState],SHIFT
	jnz	i9CheckMark
	cmp	al, 37H
	je	i9Grab
	cmp	al, 57
	je	i9space
	test	byte ptr ds:[KbFlagRt],OEM_KBRT
	jz	i9CheckMark
	cmp	al,54H
	je	i9Grab
	errn$	i9CheckMark


i9CheckMark:
	call	CheckMarkStatus
	errn$	i9CallOld
    ;
    ; All done call the old Int9 handler
    ;
i9CallOld:
	pop	ax
	pop	ds
	jmp	cs:[prevInt09Proc]

i9Escshrt:				; needed a long jump
	jmp	i9Esc

    ;--
    ;
    ; Bellow are the subsections that handle the keycodes and
    ;  Alt combinations which we are interested in.
    ;

    ; ALT Make: If we are menu mode, we need to leave it.
    ;
i9altmake:
	cmp	cs:fMenuActive, 0
	je	i9CallOld
	mov	cs:AltSet, 1		; AltSet will bring the menu down.
	mov	cs:LastAlt, 0
	jmp	i9CallOld

    ; Alt Break: If the last keycode was an alt make we need to
    ;		 enter menu mode.
    ;		 If we are in hotkey mode, we need to take down the menu
    ;		 and start a context switch.
    ;
i9altbreak:
	cmp	cs:[fHotKey], HK_CLEAR
	je	i9chkalt
	mov	cs:[fHotKey], HK_CLEAR	; Clear fHotKey to bring down menu
	jmp	i9CallOld		; Context switch
i9chkalt:
	cmp	cs:[LastAlt], cAlt	; Check lastkey for Atl
	jne	i9CallOld
	call	SetMenuMode		; Enter menu mode
	jmp	i9CallOld

    ; Alt-PrtScr: Screen Grab
    ;
i9Grab:
	call	Screengrab
	jc	i9CallOld
	jmp	i9Swallow		;kill the keys

    ; Alt-Space: Control Menu mode
    ;
i9space:
	inc	cs:[fControlMenu]
	call	SetMenuMode
	jc	i9CallOld
i9Swallow:
	call	SwallowKey		;don't want it at Int16
	pop	ax
	pop	ds
	iret

    ; Alt-Tab:	Alt-tab enter us into the hotkey mode.
    ;		The hotkey mode is handle through the menu code.
    ;
i9tab:
	call	SetMenuMode
	jc	i9CallOld		; Do nothing if can't go into
					;  MenuMode
	mov	cs:[fHotKey], HK_PREV
	test	byte ptr ds:[KeyState], SHIFT
	jz	i9tset
	mov	cs:[fHotkey], HK_NEXT
i9tset:
	jmp	i9CallOld

    ; Alt-Esc:	Alt-esc immediately tries to context switch
    ;		us. Check the shift state to determine if
    ;		the next or the previous window should be switched to.
    ;
i9esc:
	cmp	cs:[fHotKey], HK_CLEAR
	jne	i9CallOld
	mov	al, SW_ESCTYPE
	test	byte ptr ds:[KeyState], SHIFT
	jz	i9Switch
	mov	al, SW_SHIFT or SW_ESCTYPE
	errn$	i9Switch

;***
;
; i9Switch - the context switch part of Int09Handler
;
;	We have decided to context switch. Execute the switch
;	macro to switch synconously
;
; ENTRY
;	AX - Switch mode (SW_ESC or SW_SHIFTESC)
;	AX, BX and DS on the stack
;
; EXIT
;
i9Switch:
    ;
    ; First process the the interrupt
    ;
	pushf
	call	cs:[prevInt09Proc]	; process the interrupt
    ;
    ; If the App is dead, don't switch, just close
    ;
	mov	ds, cs:[WinoldapDS]
	cmp	OldApState, SF_TERMINATED; Is the app dead?
	jl	i9markSwitch		;   No, switch
	mov	OldApState, SF_CLOSED	;   Yes, close
	jmp	i9endSwitch
    ;
    ; Mark fSwitch to indicate we want to switch.
    ;
i9markSwitch:
	mov	cs:[fSwitch], al	; Mark the type of switch is pending
	mov	ax, SwitchOutID 	; Execute the switch macro
	save	<bx,cx,dx,es>		; FinishSwitchMacro will set
	cCall	ExecuteMacro, <ax>	;   SW_SWITCH
	or	AX,AX			; was there a macro at all?
	jz	i9endSwitch
	mov	AX,SwitchOutID
	call	FinishSwitchMacro
	errn$	i9endSwitch
    ;
    ; Restore all of the registers
    ;
i9endSwitch:
	pop	ax
	pop	ds
	iret

;***
;
; FinishSwitchMacro - Looks for the completion of the switch macro
;
;	The switch macro is compleated. If this is a bad application,
;	then the SW_SWITCH bit of fSwitch is set which will cause
;	a context switch to occur. In the process of the context swtich
;	DDESwitchOut will occur. Good Apps must call DDESwitchOut
;	directly.
;
; ENTRY
;	AX - MacroID
;
; ON EXIT
;	if Bad
;	   fSwitch is set
;	else
;	   DDESwitchOut is called
; USES
;	BX, CX, DX
;
FinishSwitchMacro:
	cmp	ax, SwitchOutID
	jne	sfExit
	test	[pifBehavior], BADBITS
	jnz	fsBad
	save	<ax>
	cCall	DDESwitchOut
	jmp	sfExit
fsBad:	or	cs:[fSwitch], SW_SWITCH
sfExit:
	ret


;***
;
; SwallowKey - swallows the last key typed at the keyboard
;
;	This routine swallows a key so that it does not
;	show up at Int 16.
;
; ENTRY
;
; EXIT
;	Key is swallowed
;
; USES
;	none
;
SwallowKey:
	push	AX
	in	AL,61H
	mov	AH,AL
	or	AL,80H
	out	61H,AL
	xchg	AH,AL
	out	61H,AL
	cli
	mov	AL,20H
	out	20H,AL
	sti
	pop	AX
	ret


;**
;
; ScreenGrab - This routine performs a screen grab if the grabber
;	       variable (fWOAGrab) is enabled.
;
;	ENTRY
;
;	EXIT
;		Clipboard buffer updated if need be
;
;	USES
;
ScreenGrab:
	push	AX
	push	BX
	push	CX
	push	DX
	push	DS
	push	ES
	push	DI
	push	SI
	xor	AL,AL
	xchg	AL,CS:fWOAGrab
	push	AX
	test	AL,0FFH
	jz	SGFail
	mov	DS,CS:[WinoldapDS]	; get current winoldap DS
	mov	AX,DS
	mov	ES,AX
	mov	AX,GRAB_GETINFO
	lea	DI,GrabData
	mov	SI,InquireGrab
	call	GoGrab			; initialize Grabber data structures
	mov	AX,CS
	mov	ES,AX
	lea	DI,GrabReq
	xor	AX,AX
	mov	word ptr CS:[DI].grlpData,AX
	mov	word ptr CS:[DI].grlpData+2,AX
	mov	CS:[DI].grXext,AL
	mov	CS:[DI].grYext,AL
	mov	CS:[DI].grStyle,FMT_OTHER
	call	GoGetGrab		; get the size needed

	cmp	AX,BufSize
	ja	SGFail

	mov	AX,BufSeg
	mov	word ptr ES:[DI].grlpData+2,AX
	xor	AX,AX
	mov	word ptr ES:[DI].grlpData,AX
	call	GoMarkGrab
	call	GoGetGrab
	xor	CX,CX
	loop	$
	call	GoMarkGrab
	clc
	jmp	SGRet

SGFail:
	stc				; show that we didn't do it
SGRet:
	pop	AX			;get the grabber count back
	xchg	AL,CS:fWOAGrab
	pop	SI
	pop	DI
	pop	ES
	pop	DS
	pop	DX
	pop	CX
	pop	BX
	pop	AX
	ret



;**
;
; CheckMarkStatus - Determines if a mark should be aborted
;
;	If a Mark has just been terminated and we are not processing
;	one of our Alt shifted keys then restore the mark area and
;	reset fMark.
;
;	ENTRY
;		al = keyboard input
;
;	EXIT
;		Screen restored if need be
;		fMark updated
;
;	USES
;		none
;
CheckMarkStatus:
	test	CS:[fSwitch], SW_SWITCH
	jnz	CMSRet
	test	AL,cBreak		;is this a break code?
	jnz	CMSRet			;if so, do nothing
	cmp	CS:[fMenuActive],0	;no mark action if in a menu
	jne	CMSRet
	cmp	CS:[fMark],0
	je	CMSRet
	cmp	CS:[fEndBlock],0
	je	CMSRet
	call	RestoreMark		;we are done marking and there is a
					;marked area on the screen, so restore
	mov	CS:[fMark],0
CMSRet:
	ret



;***
;
; SetMenuMode - Sets the menu flag to request a menu mode
;
;	If we can go into menu mode set the menu flag
;	In order to go into menu mode the proper grabber
;	and video mode must be present.
;
; ENTRY
;
; EXIT
;	If possible menu, fMenu is set
;	Carry is set if failure.
;
; USES
;	None
;
SetMenuMode:
	push	AX
	push	BX
	push	CX
	push	DX
	push	SI
	push	DI
	push	DS
	mov	ds, cs:[WinoldapDS]
	call	CanWeDoMenus
	jc	SMBeep
	cmp	CS:[fMenuActive],0
	jnz	SMOk			; If menus up, no error but do nothing
	lds	bx,cs:[pErrMode]
	cmp	byte ptr ds:[bx],0
	jnz	SMBeep
    ;
    ; All Ok: set the menu flag
    ;
	mov	word ptr CS:[fMenu],1
SMOk:
	clc
	jmp	SMexit
    ;
    ; Not ok. Beep the user
    ;
SMBeep:
;	mov	cs:[fBeep],1		;no more beeps comment out
    ;
    ; Return a failure
    ;
SMError:
	stc
SMexit:
	pop	DS
	pop	DI
	pop	SI
	pop	DX
	pop	CX
	pop	BX
	pop	AX
	ret



;***
;
; SwitchToWindows SwitchToWindows2 - Leave old app and go back to Windows
;		if possible.
;
; ENTRY:
;	None (fSwitch set for call to SwitchToWindows)
; EXIT:
;	fSwitch = 0 if switch successful
; USES:
;	ALL but DI,SI
;
SwitchToWindows:
	test	cs:[fSwitch], SW_SWITCH ; do we want to switch
	jz	stw1
SwitchToWindows2:
   ;
   ; Do not switch if we are pasting, marking or in menu mode
   ;
	cmp	cs:[fMenuActive], 0
	jne	stwBeep
	cmp	cs:[fPasteActive], 0
	jne	stwBeep
	cmp	cs:[fMark], 0
	jne	stwBeep

   ;
   ; Initailize the stack only once
   ;
	cmp	Word Ptr CS:[Caller_Stack +2], 0 ; Is it initialized ?
	jne	SW2a			; Yes, so don't init it again
	mov	AX, SS
	mov	Word Ptr CS:[Caller_Stack +2], AX
	mov	AX, SP
	sub	AX, 128 		; Add buffer zone
	mov	Word Ptr CS:[Caller_Stack], AX
SW2a:
    ;
    ; We now ask the 8259A INT controller if there are any outstanding
    ;	(unacknowledged) IRQs. This is command 00001011B (read IS reg).
    ;
	call	CheckInts
	jnz	stw1
	call	CheckApp		; Ask app for permission if installed
	jnz	stwBeep 		; Not granted, so cancel request ~~TN 081486 ~~
	call	GoWindows		; try to switch to windows
	pushf
	call	NotifyApp		; Notify app about the completion of
	popf				; a context switch
	jc	stw1			; did it work?
	jmp	stw1A
    ;
    ; Unable to switch: Beep for the user
    ;
stwBeep:
	mov	cs:[fBeep], 1		; Tell the user of the failure
stw1A:
	mov	cs:[fSwitch], SW_CLEAR	; yes, mark as completed
stw1:
	ret

;=========== CheckInts ====================================================
;
;	DESCRIPTION
;		This routine tells if there are any unacknowledged
;		hardware interrupts pending
;
;	ENTRY
;
;	EXIT
;		ZF set if no Ints
;		ZF cleared if ints pending
;
;	USES
;		AL, flags
;

CheckInts:
	mov	al,00001011b		; don't switch if unack ints
	out	20H,al
	jmp	$+2
	in	al,20H
	or	al,al
	ret

page
;========  CheckApp  =========================================================
;
;  If the Windows notification hooks are installed, check to see if
;  the current DOS application will grant the permission to context switch
;
;  Outputs:
;    ZF:	0: Permission not granted: Wait again
;		1: Ok to switch or notification hooks not installed
;
;=============================================================================


HPENTRY 	equ	6FH		; HPSYSTEM interrupt vector
V_WINDOWS	equ	168H		; Windows private vector in the
;					; ..HPSYSTEM vector table
F_SYSTEM	equ	02		; SYSTEM function
SF_START	equ	02		; Start Application subfunction
SF_REPORT_STATE equ	04		; Status inquiry subfunction
;
ROM_SEGMENT	equ	0F000H		; ROM BIOS segment on Vectra
ROM_AREA	equ	0E000H		; ROM BIOS area (0E000h-0FFFFh)

CheckApp	Proc	Near

	push	AX			; Save context
	push	SI
	push	DI
	push	BP
	push	DS

;
;
; Do not switch if inside DOS or inside DOS critical error handler
;
	lds	si,cs:[pInDOS]
	mov	al,ds:[si]
	lds	si,cs:[pErrMode]
	or	al,ds:[si]
	jnz	CheckARet		; ZF= 0 :Cannot switch

	call	CheckNotify		; Is Windows Notification installed?
	jnz	CheckACont		; Yes, ask for permission

; if (Extended mode) AND (COM ports in use) then return (FALSE)

	cmp	CS:[fExtend], 0
	jz	CheckARet		; Clear to switch (ZF=1)
	mov	DS, CS:[WinoldapDS]
	test	[pifMsFlags], fCOM1 +fCOM2 ; COM ports in use ?
	jz	CheckARet		; Nop, Clear to switch
	mov	CS:[fSwitch], SW_CLEAR	; Abort switch (ZF = 0)
	jmp	CheckARet

CheckACont:
;
	push	CX
	push	DX
	mov	CX, Word Ptr CS:[Caller_Stack +2] ; CX:DX= caller's SS:SP
	mov	DX, Word Ptr CS:[Caller_Stack]

	mov	BP, V_WINDOWS
	mov	AX, F_SYSTEM shl 8 + SF_REPORT_STATE
	int	HPENTRY

	pop	DX
	pop	CX

	or	AX, AX			; Success if 0

CheckARet:
	pop	DS
	pop	BP
	pop	DI
	pop	SI
	pop	AX
	ret

CheckApp	Endp


;	CheckNotify
;
;  Check to see if the Windows notification hooks are installed by the
;  cooperating DOS application.
;
;  Outputs:
;    ZF:	1: Hooks not installed
;		0: Hooks in
;
;  Registers
;    AX
;

CheckNotify	Proc	Near

	push	ES

	cmp	CS:[fVectra], 1 	; Do we have a Vectra ?
	jnz	CheckNIBMAT		; Check the IBM way
	mov	ES, CS:[HPVectorTable]
	cmp	ES:[V_WINDOWS +2], ROM_SEGMENT
	jmp	CheckNRet		; ZF set if V_WINDOWS CS = ROM_SEGMENT

CheckNIBMAT:
	xor	AX, AX
	mov	ES, AX
	cmp	ES:[4 *HPENTRY +2], ROM_AREA
	jb	CheckNPC		; Go check for IBM-PC
	xor	AX, AX			; ZF set if HPENTRY CS >= ROM_AREA
	jmp short CheckNRet

CheckNPC:
	cmp word ptr ES:[4 *HPENTRY +2], 0 ; Initial HPENTRY value in an IBM-PC

CheckNRet:
	pop	ES
	ret

CheckNotify	endp


page
;=======  NotifyApp  ==========================================================
;
;  If the Windows notification hooks are installed by the Oldapp, Notify
;  that app that we have successfully complete a context switch
;
;==============================================================================

NotifyApp	Proc	Near

	push	AX

	call	CheckNotify
	jz	NotifyRet		; NOP if hook not installed
	push	BP
	push	DS
;
	push	CX
	push	DX
	mov	CX, Word Ptr CS:[Caller_Stack +2] ; CX:DX= caller's SS:SP
	mov	DX, Word Ptr CS:[Caller_Stack]

	mov	BP, V_WINDOWS
	mov	AX, F_SYSTEM shl 8 + SF_START
	int	HPENTRY

	pop	DX
	pop	CX
	pop	DS
	pop	BP

NotifyRet:
	pop	AX
	ret

NotifyApp	Endp

page
;-------------------------------------------------------
;
;  Video - INT 10
;
;**
;
; EnableVideo - Turn on our INT 10 and INT 15 hooks for Video emulation
;		  and set various system defaults.
;
; ENTRY:
;	None
; EXIT:
;	Current INT 10 vector stored in prevInt10proc
;	Our INT 10 handler installed
;	Current INT 15 vector stored in prevInt15proc
;	Our INT 15 handler installed
;	Current crtmode and column width saved in CrtState
;	CrtState set to 7 with 80 columns
;	Current equip word saved in EquipmentFlag
;	Equip word set to 80X25 BW on BW card
; USES:
;	AX,BX,DX,ES,FLAGS
;
EnableVideo:
	push	ds
	mov	ax,3510h
	int	21h
	mov	word ptr cs:[prevInt10proc],bx
	mov	word ptr cs:[prevInt10proc+2],es

	mov	dx,codeOffset Int10Handler
	mov	al,10h
	call	SetVector

	call	EnableTopView

	xor	ax,ax			; Point to ROM comm area
	mov	ds,ax

	mov	ax,MaxCol*256+7 	; Screen columns to MaxCol, mode 7
	xchg	ax,ds:[449h]		; get and set crt mode
	mov	cs:[CrtState],ax	 ; Save mode

	mov	ax,ds:[410h]		; get equipment flag
	mov	cs:[EquipmentFlag],ax	; Save it
	or	al,00110000b		; 80X25 BW using BW card
	mov	ds:[410h],ax		; Set it
	pop	ds
	ret

;**
;
; DisableVideo - Turn off our video emulation and restore state
;
; ENTRY:
;	None
; EXIT:
;	INT 10 vector stored in prevInt10proc restored
;	Current INT 10 vector discarded
;	INT 15 vector stored in prevInt15proc restored
;	Current INT 15 vector discarded
;	Equip word restored from EquipmentFlag
;	Crtmode and col width restored from CrtState
; USES:
;	AX,DX,FLAGS
;
DisableVideo:
	push	ds
	lds	dx,cs:[prevInt10Proc]
	mov	al,10h
	call	RestoreVector
	call	DisableTopView
	xor	ax,ax
	mov	ds,ax
	mov	ax,cs:[EquipmentFlag]
	mov	ds:[410h],ax			; restore equipment flag
	mov	ax,cs:[CrtState]
	mov	ds:[449h],ax			; restore crt mode
	pop	ds
	ret


;**	EnableTopView
;
; Save the previous int 15h and install our TopView fiter
;
; Exit:
;  PrevInt15Proc	intiliazed
;
; Uses:
;  AX,BS,ES,FLAGS
;

EnableTopView	proc	NEAR

	push	DS

	mov	ax,3515h
	int	21h
	mov	word ptr cs:[prevInt15proc],bx
	mov	word ptr cs:[prevInt15proc+2],es

	mov	dx,codeOffset Int15Handler
	mov	al,15h
	call	SetVector

	pop	DS
	ret

EnableTopView	endp


;**	DisableTopView
;
; Restore the original Topview int 15h handler
;
; Entry:
;  PrevInt15Proc
;
; Uses:
;  AX,DX
;

DisableTopView	proc	NEAR

	push	DS
	lds	dx,cs:[prevInt15Proc]
	mov	al,15h
	call	RestoreVector
	pop	DS
	ret

DisableTopView	endp


;
; This is the dispatch table for the INT 10 functions we handle
;
		dw	Int10$FE	; AH = fe	Get vid buffer addr
		dw	Int10$FF	; AH = ff	Update vid buffer
VideoJump	dw	Int10$0 	; AH = 0	Set mode
		dw	Int10$1 	; AH = 1	Set Cursor Type
		dw	Int10$2 	; AH = 2	Set Cursor pos
		dw	Int10$3 	; AH = 3	Get cursor pos
		dw	Int10$4 	; AH = 4	Get light pen pos
		dw	Int10$5 	; AH = 5	Set active page
		dw	Int10$6 	; AH = 6	Scroll active page up
		dw	Int10$7 	; AH = 7	Scroll active page down
		dw	Int10$8 	; AH = 8	Get char/attrib at cursor
		dw	Int10$9 	; AH = 9	Write char/attrib at cursor
		dw	Int10$10	; AH = 10	Write char strng at cursor
		dw	Int10$11	; AH = 11	Set Color pallete
		dw	Int10$12	; AH = 12	Write dot
		dw	Int10$13	; AH = 13	Read dot
		dw	Int10$14	; AH = 14	Write tty
		dw	Int10$15	; AH = 15	Get vid state
		dw	Int10$00	; AH = 16	Set pallete reg EGA
		dw	Int10$00	; AH = 17	Char gen EGA
		dw	Int10$00	; AH = 18	Alternate select EGA
		dw	Int10$19	; AH = 19	Write string EGA

;**
;
; Int10Handler - INT 10 handler to emulate functions in a Window
;
; ENTRY:
;	AH is function
;	Others as per particular AH
; EXIT:
;	As per particular INT 10 (see IBM tech ref)
; USES:
;	As per particular INT 10 (see IBM tech ref)
; SEE ALSO:
;	IBM PC tech ref manual
;
Int10Handler:
	push	bp
	mov	bp,sp
	mov	bp,[bp+4]	; code segment of caller outside windows?
	cmp	cs:[TopPDB],bp
	jbe	vid0		; no, continue
    ;
    ; INT 10 called from outside Windows, restore old CrtState and equip
    ; word, and dispatch to old INT 10 code
    ;
	push	ds
	xor	bp,bp
	mov	ds,bp
	mov	bp,cs:[EquipmentFlag]
	mov	ds:[410h],bp			; restore equipment flag
	mov	bp,cs:[CrtState]
	mov	ds:[449h],bp			; restore crt mode
	pop	ds
	pop	bp
	jmp	cs:[prevInt10proc]		; yes, do old processing

vid0:	sti
    ;
    ; We understand calls < 19, feh and ffh. If call is not understood
    ;	it is ignored by iret to caller with no regs modified.
    ;
	cmp	ah,19
	jb	vidA
	cmp	ah,0feh 		; get video address?
	jb	vid5
vidA:	push	si
	push	es
	push	di
	push	dx
	push	cx
	push	bx
	push	ax

    ;
    ; Compute address of handler and dispatch
    ;
	push	ax
	mov	al,ah
	cbw
	shl	ax,1
	mov	si,codeOffset VideoJump
	add	si,ax
	pop	ax
	add	ah,12			; Most GOFUNC dispatch codes are biased
					;  by 12 from the INT 10 function #s

	push	ds			; Set Z flag if doing print screen
	xor	bp,bp
	mov	ds,bp
	cmp	byte ptr ds:[500h],1
	pop	ds

	call	cs:[si]

	pop	ax
vid1:	pop	bx
vid2:	pop	cx
	pop	dx
vid3:	pop	di
	pop	es
vid4:	pop	si
vid5:	pop	bp
	iret


;**
;
; int10$FE - TopView Get Video Buffer routine
;
; ENTRY:
;	ES:DI -> hardware video buffer addr
; EXIT:
;	ES:DI -> logical video buffer address
; USES:
;	ES,DI
; SEE ALSO
;	TopView Programmer Toolkit
;
int10$FE:
	mov	ah,24
	call	GoFunc
	mov	di,ax
	pop	ax		; Discard return address
	pop	ax		; AX unchanged
	pop	bx		; BX unchanged
	pop	cx		; CX unchanged
	pop	dx		; DX unchanged
	pop	si		; Discard input ES
	pop	si		; Discard input DI
	jmp	vid4


;**
;
; int10$FF - TopView Update video display
;
; ENTRY:
;	ES:DI -> First modified char of logical video buffer from call FE
;	CX = Number of bytes modified
; EXIT:
;	Display updated
; USES:
;	None
; SEE ALSO
;	TopView Programmer Toolkit
;
int10$FF:
	mov	ah,25
	mov	bx,di
	jmp	GoFunc

;**
;
; Calls
;	14	Write TTY
;	1	Set Cursor Type
;	2	Set Cursor Position
;	6	Scroll active page up
;	7	Scroll active page down
;	9	Write char/attrib at cursor
;	10	Write char at cursor
; Dispatch directly to GoFunc
;
Int10$14:
	mov	ah,8
Int10$1:
Int10$6:
Int10$7:
Int10$9:
Int10$10:
	jmp	GoFunc
Int10$2:
	je	Int05$2
	jmp	GoFunc

;**
;
; Calls
;	5	Select Active page
;	11	Set color pallete
;	12	Write Dot
;	13	Read Dot
;	16	Set Pallete Regs
;	17	Char Generator
;	18	Alternate Select
; Are NOPs (ignored)
;
Int10$5:
Int10$11:
Int10$12:
Int10$13:
Int10$00:
	ret

;**
;
; Int10$4 - Read Light Pen position
;
; ENTRY:
;	None
; EXIT:
;	AH = 0 Light pen not triggered
; USES:
;	AX
; SEE ALSO:
;	IBM PC Tech Ref Manual
;
Int10$4:
	xor	ax,ax		; return no light pen
	pop	bx		; remove return address
	pop	bx		; remove pushed AX
	jmp	vid1		; pop the rest and return

;**
;
; Int10$0 - Set Mode
;
; ENTRY:
;	AL = Mode
; EXIT:
;	Screen Cleared, Cursor homed. The actual set mode is effectively
;		ignored.
; USES:
;	None
; SEE ALSO:
;	IBM PC Tech Ref Manual
;
Int10$0:
	mov	ax,18*256+0
	xor	cx,cx
	mov	dx,Line25*256+Col80
	mov	bh,7
	call	GoFunc		; clear the screen
	xor	dx,dx
	mov	ah,14
	jmp	GoFunc		; home the cursor


;**
;
; Int10$3 - Get Cursor position
;
; ENTRY:
;	BH = Page # (ignored)
; EXIT:
;	DH,DL = Row,Col
;	CH,CL = Cursor mode
; USES:
;	DX,CX
; SEE ALSO:
;	IBM PC Tech Ref Manual
;
Int10$3:
	je	Int05$3
	call	GoFunc
Int10$3a:
	mov	cx,ax
	pop	ax		; discard return address
	pop	ax		; Input AX
	pop	bx		; Input BX
	pop	di		; Input CX discarded
	pop	di		; Input DX discarded
	jmp	vid3

;**
;
; Int10$8 - Read Char/Attrib at cursor
;
; ENTRY:
;	BH = Page # (ignored)
; EXIT:
;	AH,AL = attr,char
; USES:
;	AX
; SEE ALSO:
;	IBM PC Tech Ref Manual
;
Int10$8:
	je	Int05$8
	call	GoFunc
Int10$8a:
	pop	bx
	pop	bx
	jmp	vid1

;**
;
; Int10$15 - Get Current Video state
;
; ENTRY:
;	None
; EXIT:
;	AL = Mode (always 7)
;	AH = MaxCol
;	BH = Current active display page (always 0)
; USES:
;	AX,BX
; SEE ALSO:
;	IBM PC Tech Ref Manual
;
Int10$15:
	mov	ax,MaxCol*256+7 ; 80 column, monochrome display
	mov	bh,0		; active display page 0
	pop	cx		; get return address
	pop	cx		; pop off ax
	pop	cx		; pop off bx
	jmp	vid2

;**
;
; Int10$19 - Write String EGA SPECIFIC
;
; ENTRY:
;	ES:BP -> string
;	CX = char count of string
;	DX = Position for first char of string
;	BH = Page # (ignored)
;	AL = 0
;		BL attrib for all chars
;		String is CX chars
;		Cursor not moved
;	AL = 1
;		BL attrib for all chars
;		String is CX chars
;		Cursor moved
;	AL = 2
;		String is CX char,attrib pairs
;		Cursor not moved
;	AL = 3
;		String is CX char,attrib pairs
;		Cursor moved
; EXIT:
;	String printed
; USES:
;	None
; SEE ALSO:
;	IBM PC Tech Ref Manual
;
Int10$19:
	shl	dh,1
	shl	dh,1
	shl	dh,1
	shl	dh,1
	or	dl,dh		; dl = full cursor position
	mov	dh,bl		; dh = attribute
	mov	bx,bp		; es:bx points to string
	mov	ah,26
	jmp	GoFunc



;------------------------------------------------------
;
;  Setup DS to point to WINOLDAP DS
;
Int05$DS:
	push	dx
	push	ax
	cCall	GetCurrentTask
	mov	ds,ax
	pop	ax
	pop	dx

	mov	ds,ds:[TDB_taskSS]
	ret


;------------------------------------------------------
;
;  Set Cursor position
;
Int05$2:
	push	ds
	call	Int05$DS
	cmp	dh,MaxLine
	jb	Int05$2a
	sub	dh,MaxLine
Int05$2a:
	mov	bx,dx
	xchg	dx,[RowCol]
	call	NewCur
	pop	ds
	ret


;------------------------------------------------------
;
;  Get Cursor Information
;
Int05$3:
	push	ds
	call	Int05$DS
	mov	ax,[OrgVidCursLook]
	mov	dx,[RowCol]
	pop	ds
	jmp	Int10$3a


;------------------------------------------------------
;
;  Read Character and attribute
;
Int05$8:
	push	ds
	call	Int05$DS
	les	di,[VidAddr]
	mov	ax,es:[di]
	pop	ds
	jmp	Int10$8a

;-------------------------------------------------------
;
; Mini TopView handler
;
;**
;
; Int15Handler - INT 15 handler to emulate functions in a Window
;			that are TopView calls
;
;     THIS CODE HANDLES CALLS 1022H and 1034H ONLY. ISOBJ and GETVER.
;
; ENTRY:
;	AH is function
;	Others as per particular AH
; EXIT:
;	As per particular INT 15 (see IBM TopView tech ref)
; USES:
;	As per particular INT 10 (see IBM TopView tech ref)
; SEE ALSO:
;	IBM TopView Programmers Toolkit
;
Int15Handler:
	cmp	ax,1022H
	jnz	i15a
	mov	bx,-1		; show that topview is here
	iret
i15a:	cmp	ax,1034H
	jnz	i15b
	mov	bx,1		; indicate version 1 of topview
	iret
i15b:	jmp	cs:[prevInt15proc]


;==================================================================
;
;**
;
; SetMode - Place the display adaptor in the correct initial mode
;
;   This routine is called before starting an old app not in a Window
;   to place the adaptor in a correct initial mode.
;
;   Optional entry point InitScreen in the grabber used if it is present.
;   Presense is checked for by looking for the opcode for a three byte jump
;   at the InitScreen location, if its there the optional entry point is taken.
;
; ENTRY:
;	GrabSeg = seg addr of correct grabber
; EXIT:
;	Default screen mode set
; USES:
;	ALL
;
SetMode:
    ;
    ; Look to see if grabber has an InitScreen entry point
    ;
	mov	ds,cs:[GrabSeg] 	; get segment of screen grabber
	mov	si,InitScreen		; point to new entry point
	cmp	byte ptr ds:[si],0E9H	; is there a 3 byte jump?
	jnz	sm1			; no, just do the old stuff
	jmp	GoGrab			; call the screen grabber

    ;
    ;  Default SetMode if ScreenGrabber doesnt do it
    ;
sm1:	int	11h			; Get equip
	and	al,00110000b		; Mask to display bits
	cmp	al,00010000b		; 40X25 BW on color?
	mov	ax,3			; Mode 3 if not 40X25 BW on color
	jnz	sm2
	mov	ax,1			; Mode 1 if 40X25 BW on color
sm2:	int	10h			; Set Mode
	ret

;==================================================================
;
;  Communications support
;
;
; These are the string IDs for strings in STRINGTABLE (see Winoldap.rc)
;
msgCOM1 equ	8001H	;COM1 is not available
msgCOM2 equ	8002H	;COM2 is not available
msgCOM3 equ	8003H	;COM1 and COM2 are not available

;**
;
; CheckCOM - Check pif bits for COM port consistency
;
;   This routine checks the given pif bits for COM usage to see
;	if they are OK. This is done for IBM by checking
;	the RS232_BASE port locations in the ROM BIOS data area
;	to see if the device is valid.
;
;  NOTE: If BOTH COM ports are requested the validity of EITHER port
;	is sufficient to be successful.
;
; ENTRY:
;	AL = pif behavior bits including COM bits
; EXIT:
;	Carry Set
;		The pif bits request invalid hardware
;		AX,DI - STRINGTABLE value of correct error message
;	Carry Clear
;		The pif bits are OK
; USES:
;	AX,SI,DI,ES,FLAGS
;
CheckCOM:
	xor	si,si
	mov	es,si

	mov	di,msgCOM1
	test	al,fCOM1		; Does the guy want COM1?
	jz	cc1			; no...
	cmp	es:[RS232_base],si	; is it around?
	jz	cc2			; no, possible error

cc1:	mov	di,msgCOM2
	test	al,fCOM2		; Does he want COM2
	jz	cc4			; no...
	cmp	es:[RS232_base+2],si	; is it around?
	jnz	cc4			; yes...return

    ;
    ; Here if error on COM1 OR COM2, check if both are requested.
    ; If both are requested, presense of EITHER makes things OK.
    ;
cc2:	and	al,fCOM1+fCOM2
	cmp	al,fCOM1+fCOM2		; does he want both?
	jnz	cc3			; no, error, message in DI from above
	mov	ax,es:[RS232_base]	; COM1
	or	ax,es:[RS232_base+2]	; COM2
	jnz	cc4			; If EITHER exists on both request, OK
	mov	di,msgCom3		; Message for bad both
cc3:	mov	ax,di			; Message to AX too
	stc
	ret

cc4:	clc
	ret

;-----------------------------------------------------------
;
; This is a "buffer" to use to call AnsiToOem
;
TransBuf	dw	0

;
; THE FOLLOWING IS THE DATA STRUCTURE USED TO PERFORM THE TRANSLATION
;
;  Given a scan code and an ASCII value, translate the key
;  for the IBM PC.
;
;  This table is indexed by the scan code and the shift,ctrl, or alt state.
;  Each entry is four bytes long, each byte representing the translation for
;  base, shifted, control, and alt state respectively.
;
;  The correct translation action is encoded as follows:
;
;    0-127 = Add this value to scan code, make ascii value zero
;  128-252 = Use 128 less than this value for ascii, use scan code unmodified
;  -1 (255)= ignore key
;  -2 (254)= ignore key if no ANSI translation, otherwise use it
;  -3 (253)= make ascii value zero if no ANSI translation, otherwise use it
;
; For an Extended keyboard the Numpad Enter and Numpad / return identical
; codes as those for the regular Enter and / keys.  This is due to the
; Windows keyboard handler giving the same codes to Winoldap for these keys.
; They are indistinguishable.  The Alt-Numpad keys will also return the
; same codes as that of the cursor pad keys since the cursor pad keys are
; mapped to the numpad keys.

	;	base,shf,ctl,alt

TransKeyTable:
	DB	 -2, -2, -2, -2        ; Scan code 0 undefined
	DB	 -2, -2, -2, -26       ; Esc
	DB	 -2, -2, -2,118        ; 1
	DB	 -2, -2, -2,118        ; 2
	DB	 -2, -2, -2,118        ; 3
	DB	 -2, -2, -2,118        ; 4
	DB	 -2, -2, -2,118        ; 5
	DB	 -2, -2, -2,118        ; 6
	DB	 -2, -2, -2,118        ; 7
	DB	 -2, -2, -2,118        ; 8
	DB	 -2, -2, -2,118        ; 9
	DB	 -2, -2, -2,118        ; 0
	DB	 -2, -2, -2,118        ; -
	DB	 -2, -2, -2,118        ; =
	DB	 -2, -2, -2, 0	       ; backspace
	DB	 -2,  0, 134,150       ; tab
	DB	 -2, -2, -2,  0        ; q
	DB	 -2, -2, -2,  0        ; w
	DB	 -2, -2, -2,  0        ; e
	DB	 -2, -2, -2,  0        ; r
	DB	 -2, -2, -2,  0        ; t
	DB	 -2, -2, -2,  0        ; y
	DB	 -2, -2, -2,  0        ; u
	DB	 -2, -2, -2,  0        ; i
	DB	 -2, -2, -2,  0        ; o
	DB	 -2, -2, -2,  0        ; p
	DB	 -2, -2, -2,  0        ; [
	DB	 -2, -2, -2,  0        ; ]
	DB	 -2, -2, -2,  0        ; return
	DB	 -1, -1, -1, -1        ; Ctrl
	DB	 -2, -2, -2,  0        ; a
	DB	 -2, -2, -2,  0        ; s
	DB	 -2, -2, -2,  0        ; d
	DB	 -2, -2, -2,  0        ; f
	DB	 -2, -2, -2,  0        ; g
	DB	 -2, -2, -2,  0       ; h
	DB	 -2, -2, -2,  0        ; j
	DB	 -2, -2, -2,  0        ; k
	DB	 -2, -2, -2,  0        ; l
	DB	 -2, -2, -2,  0        ; ;
	DB	 -2, -2, -2,  0        ; '
	DB	 -2, -2, -2,  0        ; `
	DB	 -1, -1, -1, -1        ; left-shift
	DB	 -2, -2, -2,  0        ; \
	DB	 -2, -2, -2,  0        ; z
	DB	 -2, -2, -2,  0        ; x
	DB	 -2, -2, -2,  0        ; c
	DB	 -2, -2, -2,  0        ; v
	DB	 -2, -2, -2,  0        ; b
	DB	 -2, -2, -2,  0        ; n
	DB	 -2, -2, -2,  0        ; m
	DB	 -2, -2, -2,  0        ; ,
	DB	 -2, -2, -2,  0        ; .
	DB	 -2, -2, -2,  0        ; /
	DB	 -1, -1, -1, -1        ; right-shift
	DB	 -2, -2, 59,  0        ; PrtSc
	DB	 -1, -1, -1, -1        ; Alt
	DB	 -2, -2, -2, -2        ; space
	DB	 -1, -1, -1, -1        ; CapsLock
	DB	 -3, 25, 35, 45        ; F1
	DB	 -3, 25, 35, 45        ; F2
	DB	 -3, 25, 35, 45        ; F3
	DB	 -3, 25, 35, 45        ; F4
	DB	 -3, 25, 35, 45        ; F5
	DB	 -3, 25, 35, 45        ; F6
	DB	 -3, 25, 35, 45        ; F7
	DB	 -3, 25, 35, 45        ; F8
	DB	 -3, 25, 35, 45        ; F9
	DB	 -3, 25, 35, 45        ; F10
	DB	 -1, -1, -2, -1        ; NumLock
	DB	 -1, -1, -2, -1        ; ScrollLock
	DB	 -3,'7'+128, 48, 80        ; numpad-7
	DB	 -3,'8'+128, 64, 80        ; numpad-8
	DB	 -3,'9'+128, 59, 80        ; numpad-9
	DB	 -3, -3    , 68,  0	   ; numpad-minus
	DB	 -3,'4'+128, 40, 80        ; numpad-4
	DB	 -2,'5'+128, 67, 80        ; numpad-5
	DB	 -3,'6'+128, 39, 80        ; numpad-6
	DB	 -3, -3    , 66,  0	   ; numpad-plus
	DB	 -3,'1'+128, 38, 80        ; numpad-1
	DB	 -3,'2'+128, 65, 80        ; numpad-2
	DB	 -3,'3'+128, 37, 80        ; numpad-3
	DB	 -3,'0'+128, 64, 80        ; numpad-0
	DB	 -3,'.'+128, 64, 80        ; numpad-period
	DB	 -2, -2, -2, -2 	   ;
	DB	 -2, -2, -2, -2 	   ;
	DB	 -2, -2, -2, -2 	   ; 102nd key (UK, FrCan. ...)
	DB	 46, 48, 50, 52 	   ; F11
	DB	 46, 48, 50, 52 	   ; F12
TransKeyTableLen = ($ - TransKeyTable)/4	; count of keys

;
; Additional tables to support Cursor Keypad of the HP Envoy keyboard
;
	;	base,shf,ctl,alt
EnvoyCursorTable:
	DB	-3, -3, -1, -1		   ; cursorpad-up
	DB	-3, -3, 18, -1		   ; cursorpad-lft
	DB	-3, -3, -1, -1		   ; cursorpad-dn
	DB	-3, -3, 17, -1		   ; cursorpad-rht
	DB	-3, -3, 19, -1		   ; cursorpad-home
	DB	-3, -3, 31, -1		   ; cursorpad-pgup
	DB	-3, -3, 15, -1		   ; cursorpad-end
	DB	-3, -3, 15, -1		   ; cursorpad-pgdn
	DB	-3, -3, -1, -1		   ; cursorpad-ins
	DB	-3, -3, -1, -1		   ; cursorpad-del

;This table maps the Vectra cursor keypad scancodes into
; numpad keys for compatibility
;
EnvoyScanCodeTable	db	72	;ccp_up
			db	75	;ccp_lft
			db	80	;ccp_dn
			db	77	;ccp_rht
			db	71	;ccp_home
			db	73	;ccp_pgup
			db	79	;ccp_end
			db	81	;ccp_pgdn
			db	82	;ccp_ins
			db	83	;ccp_del

;**
;
; TranslateKey - Translate scan code/key combo to INT 16 AX return
;
;  This routine is used to convert the KEY messages recieved by WINOLDAP
;	from Windows into the correct INT 16 keys to give to an app
;	running with the WINOLDAP keyboard hooks in.
;
; ENTRY:
;	CH = Scan Code
;	CL = Windows virtual key message parm (VK_???)
;	AX = Ansi translation of key (-1 if none)
;	     Value + 256 if Alt-0-Numpad
;	DH = Modifier states
;	    bit 8 = State of ALT key	    Highest presedence
;	    bit 7 = State of CTL key
;	    bit 6 = State of Shift key	    Lowest presedence
; EXIT:
;	On Exit, AX contains the key for INT 16H
;		AL is key
;		AH is scan code
; USES:
;	AX,BX,CX,DX,SI,ES,FLAGS
; SEE ALSO:
;	IBM PC Tech Ref Manual
;
TranslateKey:
    ;
    ; Range check the scan code in HP's Envoy function table.
    ;
    ; Normally, HP's Envoy keyboard reports back scan codes which
    ; are compatible with the AT keyboard. However, under Windows
    ; the keyboard is set into "Raw" mode where unique scan codes
    ; are report. Here, we must map these unique scan codes into
    ; their IBM equivalents.
    ;
	xor	bx,bx
	cmp	ch, 70h 		; f1
	jb	tkCursorRange		; Still hope, try cursor pad
	cmp	ch, 77h 		; f8
	ja	tk0			; No hope, return -1, -1
	sub	ch, 70h - 3Bh		; Map to F1-F8
	jmp	tkNormalRange
    ;
    ; Range check the scan code in HP's cursor pad range.
    ;
tkCursorRange:
	cmp	ch, 60h 		; UP
	jb	tkNormalRange		; Still hope, try normal range
	cmp	ch, 6Ah 		; Center
	ja	tk0			; No hope, return -1, -1
	mov	si, codeOffset EnvoyCursorTable+3
	mov	bl, ch
	sub	bl, 60h
	mov	ch, cs:EnvoyScanCodeTable[bx]
	jmp	tkTableFound
    ;
    ; Range check the scan code in the normal table
    ;
tkNormalRange:
	mov	bl,ch
	cmp	ch,TransKeyTableLen	; Scan code in range?
	ja	tk0			; No, return -1,-1
	mov	si, codeOffset TransKeyTable+3
    ;
    ; Index to correct table entry
    ;
tkTableFound:
	shl	bx,1			; Four bytes ber entry
	shl	bx,1
	add	bx,si			; Index to ALT translation
    ;
    ; Check for CTRL-ALT combonation, if it is, turn off CTRL and ALT indicators
    ; This is done so that the European keyboards work. It effects only
    ; a very limited set of USA apps, which can be made to work by
    ; setting the pif keyboard bit.
    ;
	mov	dl,dh
	not	dl
	test	dl,11000000b		; is it CTL-ALT?
	jnz	tkn			; no.
	and	dh,00111111b		; yes, pretend its off for europe
    ;
    ; Index to correct trans byte based on modifiers
    ;
tkn:	shl	dx,1			; is the ALT key down
	jc	tka			; Yes, am pointing at right byte
	dec	bx			; Go to ctrl byte
	shl	dx,1			; is the CTL key down?
	jc	tka			; Yes, am pointing at right byte
	dec	bx			; Go to shf byte
	shl	dx,1			; is the SHIFT key down?
	jc	tka			; Yes, am pointing at right byte
	dec	bx			; go to base byte
    ;
    ; Perform translation
    ;
tka:	mov	bh,cs:[bx]		; get "op-code"

    ;
    ; Test for -1
    ;  If found, ignore the key
    ;
	cmp	bh,-1			; ignore key?
	jnz	tk1			; no
tk0:	mov	ax,-1			; ignore this key return -1,-1.
	ret
    ;
    ; Test for -2
    ;  If match, ignore the key if no ANSI translation
    ;
tk1:	cmp	bh,-2			; ignore key if no ansi translation?
	jnz	tk2			; no, try next
	cmp	ax,-1			; Is there ansi translation?
	jz	tk0			; no, ignore key
	xchg	ah,ch			; scan code to AH
	jmp	short tk6		; go translate key
    ;
    ; Test for -3 (Numeric pad, cursor pad, or softkeys)
    ;  If match, uses any present ANSI translation
    ;
tk2:	cmp	bh,-3			; ASCII zero if no translation?
	jnz	tk3			; no, try next
	xor	cl,cl			; Assume ascii zero
	cmp	ax,-1			; ANSI translation?
	jz	tk2a			; No, ASCII is 0
	mov	cl,al			; ASCII is ANSI translation
tk2a:	xchg	ax,cx			; scan/char to AX
	jmp	short tk6		; go translate

tk3:	mov	al,bh			; use opcode for ascii value
	and	al,01111111b		; ignoring hi bit
	xchg	ah, ch
	cmp	bh,128			; were we right?
	jae	tk7			; yes, return code
    ;
    ; Translation code < 128, add to scan code, ASCII 0
    ;
	xor	al,al			; default case = ascii value 0
	add	ah,bh			; and scan code = bh + ch
    ;
    ; Alt-Numpad magic is performed here.
    ;
    ; If the user presses Alt-Numpad or Alt-0-Numpad,
    ; the WM_Keyfirst-1 message sent by TTYkeyup will have a 0 scancode.
    ; Given that Alt-Numpad was pressed, an AnsiToOem tranlation
    ; needs to be done if 0 was pressed first. TranslateNumpad will
    ; set AsciiValue > 256 to indicate this condition.
    ;
tk6:
	cmp	ah, 0			; 0 scancode?
	jne	tkConv			;  No, translate
	cmp	ch, 0			; AsciiValue > 256 ?
	je	tk7			;  No, don't translate
	errn$	tkConv
    ;
    ; Perform AnsiToOem
    ;
tkConv:
	cmp	al,128			; ascii/ansi value < 128?
	jb	tk7			; yes, just return, AX correct
	push	ax			; save scan code in AH
	mov	ah,0			; AL = ANSI char, AH NUL terminator
	mov	bx,codeOffset TransBuf	; Buffer offset
	mov	cs:[bx],ax		; Nul termed string in buffer
	regptr	csbx,cs,bx
	cCall	AnsiToOem,<csbx,csbx>	; translate key from Ansi To OEM
	pop	ax			; Recover scan code in AH
	mov	al,byte ptr cs:[TransBuf] ; Translated key to AL
tk7:	ret

KeyPadDigits	db	7,8,9,-1
		db	4,5,6,-1
		db	1,2,3
		db	0
;**
;
; TranslateNumpad - tanslates Alt-Numpad keys into character codes
;
; This routine is called by TTYsyskeyboard to perform Alt-Numpad
; tranlsations.
;
; ENTRY:
;	CX = Windows virtual key message parm (VK_???)
;	ES:DI = Windows lParam
;	AX = Current AsciiValue
;	DX = Current AsciiCount
;
; EXIT:
;	AX = Updated AsciiValue (value+256 if Alt-0-Numpad)
;	DX = Updated AsciiCount
;
; USES:
;	BX,FLAGS
;
TranslateNumpad:
    ;
    ; First test to see if this key is on the numpad
    ;
	mov	bx,es
	cmp	bl,83				; is it on keypad?
	jae	tnNo				; no...
	sub	bl,71
	jb	tnNo
	xor	bh,bh
	mov	bh,cs:[bx+KeyPadDigits]
	cmp	bh,-1				; no...
	je	tnNo
    ;
    ; On the numpad, BH contains the value.
    ; Update AsciiValue
    ;
	mov	bl,10
	mov	dl, ah
	mul	bl
	add	al,bh		; Add in this digit
	mov	ah, dl
	mov	dx, 1
	jnz	tnExit
	mov	ah,01h
	jmp	tnExit
    ;
    ; Not on the numpad.
    ;
tnNo:
	xor	ax, ax
	xor	dx, dx

tnExit:
	ret

sEnd	code
	end
