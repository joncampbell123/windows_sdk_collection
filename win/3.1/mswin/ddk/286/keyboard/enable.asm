	page	,132

;****** ENABLE.ASM *********************************************************
;                                                                          *
; Copyright (C) 1983-1990 by Microsoft Corporation. All rights reserved.   *
;									   *
;   Copyright (C) 1985,1986,1987 by Ing. C. Olivetti & Co, SPA.		   *
;									   *
;   Contains Inquire() Enable() Disable() SetOliLights() SetEriLights()    *
;	GetKeyboardType().                                                 *
;                                                                          *
;***************************************************************************
;
;	History
;
;	Windows 3.0
;
;	02 feb 90	davidw		Cleaning up ctrl-brk, and re-doing
;					Ctrl-Alt-SysReq.
;
;	29 jan 90	peterbe		Changed init of nmi_vector.
;					Change is in effect if NEWNMI defined
;					in makefile (see $(ENABLEOPTS))
;
;	24 jan 90	peterbe		# of bytes in state block is now
;					4 (four) MAXIMUM for Excel 2.1c
;					compatibility.
;
;	14 jan 90	davidw		Re-enabling ctrl-brk
;
;	18 dec 89	peterbe		Added a couple of lables for debug.
;	21 nov 89	peterbe		Changed PIN's and POUT's back to
;					in's and out's.
;
;	03 nov 89	peterbe		remove int 3
;
;	16 oct 89	peterbe		Integrate HP Vectra changes.
;					Mainly, 
;
;	09 oct 89	peterbe		new code at sks1:
;
;	01 oct 89	peterbe		InitKeyState(): use symbolic values
;					from keyboard.inc.
;					Now call InitKeyState() EVERY time
;					Enable() is called.
;					InitKeystate() access 40:17h directly
;					instead of int 16h func.2 (see if 1).
;
;	25 sep 89	peterbe		fSysReq init'd to 0 now.
;
;	19 sep 89	peterbe		EnableKBSysReq() now increments the
;					flag if its param is NZ, decrements it
;					if the parm is 0 and the flag wasn't.
;
;	18 sep 89	peterbe		Added EnableKBSysReq() -- called from
;					KERNEL to enable/disable NMI simulation
;					on CTRL-ALT-SysReq key.  The flag
;					fSysReq is init'd to TRUE now..
;
;	07 sep 89	peterbe		commented out INT1BHandler installation.
;
;	14 jul 89	peterbe		GetKeyType(2) returns no. of func. keys
;
;	07 jul 89	peterbe		Inquire returns <size KBINFO>
;
;	30 jun 89	peterbe		Added iqdNumFunc -- must move to before
;					inquireDataEnd and redefine KBINFO
;					before next build.
;
;	28 jun 89	peterbe		Fixed GetKeyboardType.
;
;	08 jun 89	peterbe		Use VK_NUMLOCK instead of VK_OEM_NUMBER
;
;	17 may 89	peterbe		Load CSAlias into AX before GotCSAlias:
;
;	06 apr 89	peterbe		Changed size of state block to 8
;					bytes (inquireData). inquireDataSize
;					now matches SIZE inquireData.
;
;	27 dec 88	peterbe		Removed unused entries in inquireData
;					and shortened Inquire().
;
;	20 dec 88	peterbe		Shortened ScreenSwitchEnable().
;
;	14 dec 88	peterbe		Removed EnablePrintScreen().
;
;	01 dec 88	davidw		Made bi-modal.	Replaced GetCSAlias
;					with AllocCStoDSAlias.
;
;	29 nov 88	peterbe		Added EnablePrintScreen().
;
;	22 nov 88	peterbe		Added GetKeyboardType(). future
;					expansion of this!
;
;	21 sep 88	peterbe		Made old 'enabled' flag a byte.
;					Added fTables as flag for loading
;					DLL, so NewTable() won't be called when
;					coming back from Winoldap.
;
;	24 aug 88	peterbe		Checking before calling GetCSAlias()
;
;	23 aug 88	peterbe		GetCSAlias() is in TABS now.
;
;	22 aug 88	peterbe		Set up nmi_vector here now.
;					Skeleton of GetCSAlias routine exists.
;					Moved LightsAddr, etc. here from TRAP
;
;	19 aug 88	peterbe		Moved inquireData stuff here.
;					keybd_int, int1bhandler are now in
;					CODE segment
;
;	17 aug 88	peterbe		Added %out's for system type ifdefs.
;
;	12 aug 88	peterbe		Added a comment.
;					Moved RAMBIOS to keyboard.inc
;					Changed some names.
;
;	11 aug 88	peterbe		Add page directive at beginning.
;
;	08 aug 88	peterbe		Moved RT code in Enable() so keyboard
;					type is determined first!
;					Removed X1/X2 externals
;					InitKeyState updated.
;
;	27 jul 88	peterbe		Renamed keyTranslationTable to keyTrTab
;
;	26 jul 88	peterbe		Remove fKbRt references.
;
;	25 jul 88	peterbe		Change RTFlags to fKeyType
;
;	18 jul 88	peterbe		Eliminate olikbd.inc
;
;	Windows 2.1
;
;	23 jun 88	peterbe	Added %out to indicate ENVOY ifdef.
;	15 jun 88	peterbe	Moved includes to before ifdefs
;	14 jun 88	peterbe	Added hp enhancments (ENVOY ifdefs)
;	26 may 88	peterbe	Enable(), Disable() now save/restore
;				value of 40h:96h in old_enhanced.
;
;
include	keyboard.inc
include	vkoem.inc
include	vkwin.inc

; vv VVR 091989 

include int31.inc
; include debug.inc

; ^^ VVR 091989

if1
%out
%out ...........
%out ENABLE.ASM
%out ...........
    ifdef ICO
	%out .. With Olivetti M24/AT&T 6300 support
    endif
    ifdef NOKIA
	%out .. With Nokia support
    endif
    ifdef ENVOY
	%out .. With HP Vectra support
    endif
endif

if1
ifdef NEWNMI
%out .. NMI save in EnableKBSysReq()
else
%out .. NMI save only in Enable()
endif
endif

;***************************************************************************
; Some Olivetti M24x /AT&T 6300x keyboard definitions.
;***************************************************************************

ifdef ICO

; Numbers of lights, etc. on Olivetti M24's, 6300's, etc.

LedCmd		equ	13h	; command to program lights

CapsLight	equ	1	; caps lock light
NumLight	equ	2	; num lock light
ScrollLight	equ	4	; scroll lock on AT&T 302 keyboard

; bits in kb_flag (ROM BIOS keyboard flags):

fOliLites 	equ	fCaps+FNum
endif ; ICO


; Double byte range values for the Far East.
; The values defined here are for the Rest Of the World.
; These values are for the inquireData (KBINFO) structure defined below.
; ('KeyInfo' in the Kernel, pState in USER)
;
BeginRange1	equ	255
EndRange1	equ	254
BeginRange2	equ	255
EndRange2	equ	254

;***************************************************************************
; DATA segment -- data declarations and local data
;***************************************************************************

sBegin	DATA

assumes DS,DATA

; DATA segment variables accessed in KbInit() below


; Data to specify keyboard type and system type, in the keyboard table
; module.  Some of these values are loaded with the table, others are
; computed in the INIT code in this module.

	extrn	TableType:byte		; Table type loaded (1,2,3,4)

	extrn	KeyType:byte		; Computed keyboard type (1,2,3,4,5,6)

	extrn	OliType:byte		; NZ if Olivetti-protocol keyboard,
					; identifies keyboard type. Also
					; for AT&T.
					; For PCType values, see OLIKBD.INC
	extrn	fKeyType:byte		; if RT, copy of KeyType

	extrn	PCType:byte		; needed for Nokia

	EXTRN	event_proc:DWORD	; in TRAP.ASM (user's keyboard handler)


public LightsAddr			; used in ToAscii
LightsAddr	DD	0		; old int16h address

public enabled				; public for debug
enabled		db	0		; enabled flag

public fTables				; public for debug
fTables		db	0		; Call has been made to NewTable

public fSysReq
fSysReq		db	0		; Enables CTRL-ALT-SysReq if NZ

; Keyboard information block (copied to 'KeyInfo' in the kernel)
; this is a KBINFO data structure.. defined in KERNEL.INC, USER.INC, USER.H
; and WINDEFS.INC.
;
; As of 3.0, build 1.30, KBINFO includes the number of function keys
; As of 3.0, build 1.59, the number of bytes in the state block is
; fixed at 4 MAX, for compatibility with Excel 2.1c!!!
;
		PUBLIC	inquireData
		PUBLIC	iqdNumFunc
inquireData	LABEL	BYTE
		DB	BeginRange1
		DB	EndRange1
		DB	BeginRange2
		DB	EndRange2
		DW	4		; #bytes of state info for ToAscii()
iqdNumFunc	label	word
		dw	10		; number of function keys

; flag for OS/2 to allow/prevent screen switches
; The ScreenSwitchEnable function, called from the display driver,
; sets/resets this flag. A nonzero value indicates that switching
; is enabled.

	extrn	fSwitchEnable:byte


; For handling of Olivetti M24 keyboard lights

kflags		db	0ffh	; prev. value in [kb_flag]

; Saved value of enhanced keyboard flag.

old_enhanced	db	0	; save value at 40h:96h here..


ifdef	ENVOY
    if1
    %out ... Has VECTRA support
    endif
;-------------------------------------------------------------
;
; Envoy	data area
;
include	equate.inc	; HP-System equates

FV_NOEXBIOS	= 0
FV_A		= 001b
FV_NOA		= 010b
FV_ENVOY	= 100b

WF_PMODE	equ	01


	PUBLIC fVectra

staticB	 fVectra, 0	; 1 if Vectra. Set by Enable



; This is used to store	the old	HP-HIL keyboard	translators
;
OldSoftkeyTrans	dw	?						;9/23/86
OldCCPTrans	dw	?						;9/23/86

;SCANDOOR State	0 and State 2 saved here		;11/02/87 VVR
;
ScanState0	db	?				;11/02/87
ScanState2	db	?				;11/02/87 VVR

RealMode_Word_Struc	Real_Mode_Call_Struc	<>

externA	WinFlags

else
    if1
    %out ... Does NOT have VECTRA support
    endif

endif	;ENVOY


externD bios_proc	    ; these are in DS for ROM
externD nmi_vector

sEnd

;***************************************************************************
;	Resident code for this module
;***************************************************************************

externFP	NewTable	; in TABS

sBegin	CODE	    ; Beginning of code segment
assumes	CS,CODE
assumes	DS,DATA

; Light handler in ToAscii.asm

    extrn SetLightHardware: near

; keyboard interrupt trap in TRAP.ASM

    EXTRN keybd_int:FAR

;---------------------------------------------------------------------
;
;---- ScreenSwitchEnable( fEnable ) ----------------------------------
;
;   This function is called by the display driver to inform the keyboard
;   driver that the display driver is in a critical section and to ignore
;   all OS/2 screen switches until the display driver leaves its
;   critical section.  The fEnable parameter is set to 0 to disable
;   screen switches, and a NONZERO value to re-enable them.  At startup,
;   screen switches are enabled.
;---------------------------------------------------------------------
;
cProc	ScreenSwitchEnable,<PUBLIC,FAR>
ParmW	fEnable

cBegin	ScreenSwitchEnable

	mov	ax,fEnable		; get WORD parameter
	or	al,ah			; stuff any NZ bits into AL
	mov	fSwitchEnable,al	; save BYTE.

cEnd	ScreenSwitchEnable

;---------------------------------------------------------------------
;
;---- EnableKBSysReq( fSys ) ----------------------------------
;
;   This function enables and shuttles off NMI interrupt simulation
;   (trap to debugger) when CTRL-ALT-SysReq is pressed.
;   CVWBreak overides int 2.
;   fSysParm	= 01	enable	int 2
;		= 02	disable int 2
;		= 04	enable	CVWBreak
;		= 08	disable CVWBreak
;
;---------------------------------------------------------------------
;
cProc	EnableKBSysReq,<PUBLIC,FAR>
ParmW	fSysParm

cBegin	EnableKBSysReq

	mov	ax, fSysParm		; get WORD parameter

	test	al,01			; turn on int 2?
	jz	@F
	or	fSysReq,01		; yes, turn it on!
@@:	test	al,02			; turn off int 2?
	jz	@F
	and	fSysReq,NOT 01		; yes, turn it off!

@@:	test	al,04			; turn on CVWBreak?
	jz	@F
	or	fSysReq,02		; yes, turn it on!
@@:	test	al,08			; turn off CVWBreak?
	jz	@F
	and	fSysReq,NOT 02		; yes, turn it off!
@@:
	xor	ah,ah
	mov	al,fSysReq
ifdef NEWNMI
	push	ax			; save return value
	call	short GetNmi		; save NMI
	pop	ax			; restore ..
endif

cEnd	EnableKBSysReq

; save NMI vector.  Called above in EnableKBSysReq() and in Enable().

ifdef NEWNMI
GetNmi proc near
	mov	ax,3502h
	int	21h
	mov	word ptr ds:[nmi_vector][0],bx
	mov	word ptr ds:[nmi_vector][2],es
	ret
GetNmi endp
endif

ifdef	ENVOY

;------------------------------------------------------------------
;
; RSysCal	Driver,	AXReg
;
; Purpose	General	purpose	HP system calling routine
;
; Parameters	Driver	which will be stored in	BP
;		AX value
;
; Results	returns	AH which is 0 for success
;				    2 for unsupported
;
;-------------------------------------------------------------------
RSysCal	macro	device,	AXReg
	mov	ax, device
	push	ax
	mov	ax, AXReg
	call	HPSysCall
	endm

cProc	HPSysCall,<NEAR>, <ds,bp>
	parmW	Device

cBegin	HPSysCall
	mov	bp, Device
	int	HPENTRY			; Status in AH

cEnd	HPSysCall

;------------------------------------------------------------------
;
; RSysCalPM	Driver,	AXReg
;
; Purpose	General	purpose	HP system calling routine 
;		protected mode
;
; Parameters	Driver	which will be stored in	BP
;		AX value
;
; Results	returns	AH which is 0 for success
;				    2 for unsupported
;
;-------------------------------------------------------------------
RSysCalPM	macro	device,	AXReg

	push	es
	push	di
	mov	di, offset RealMode_Word_Struc
	mov	RealMode_BP[di], Device
	mov	RealMode_AX[di], AXReg
	mov	RealMode_BX[di], bx
	mov	ax, ds
	mov	es, ax				; make es = ds
	mov	bl, 6fh
	xor	bh, bh
	xor	cx, cx
	mov	ax, 0300h
	int	31h
	mov	ax, es:RealMode_AX[di]
	mov	bx, es:RealMode_BX[di]
	pop	di
	pop	es
	endm

endif	;ENVOY

;***************************************************************************
;
; Inquire( pKBINFO ) - copies information about the keyboard hardware into
; the area pointer to by the long pointer argument.  Returns a count of the
; number of bytes copied.
;
; The Windows kernel calls this to copy information to its 'KeyInfo' data
; structure.
;
;***************************************************************************
cProc	Inquire,<PUBLIC,FAR>,<si,di>
	Parmd	pKBINFO

cBegin	Inquire
					; .. now pass data to Windows ..
	les	di,pKBINFO		; Get far pointer of destination area
	mov	si,dataOFFSET inquireData	; Get source
	mov	ax,size KBINFO		; Get number of bytes to move
	mov	cx,ax			;  (Return byte count in AX)
	rep	movsb			; Move the bytes

cEnd	Inquire

;***************************************************************************
;
; Enable( eventProc ) - enable hardware keyboard interrupts, with the passed
; procedure address being the target of all keyboard events.
;
; lpKeyState is a long pointer to the Windows 256 byte keystate table
;
;***************************************************************************
cProc	Enable,<PUBLIC,FAR>,<si,di>
	ParmD	eventProc
	ParmD	lpKeyState
cBegin	Enable
; Save away passed address of event procedure
	les	bx,eventProc
	mov	ax,es
	mov	WORD PTR [event_proc],bx
	mov	WORD PTR [event_proc+2],es

; Initialize shift-key bytes in keyboard state vector to correspond to
; BIOS's flags in 40:17h.
; ES:DI points to movable memory, so no allocs allowed up to the point
; InitKeyState() is called.

; We do this EVERY time Enable is called, since we may be coming back from
; a full-screen old app.

	les	di,lpKeyState
	call	InitKeyState

; All done if just reenabling a different event proc
	cmp	[enabled],0
	jz	ke_cont
	jmp	EnableExit
ke_cont:

; Get Alias for the CS (so we can write to CODE in protect mode)
; This is only done the FIRST time Enable() is called, not when
; reenabling after a fullscreen (good) app.

	mov	ax,3500h or vector
	int	21h				; vector is in ES:BX
	mov	word ptr [bios_proc][0], bx
	mov	word ptr [bios_proc][2], es
; Save away current keyboard call vector value
	mov	ax,3516H
	int	21h

	mov	WORD PTR [LightsAddr],bx
	mov	WORD PTR [LightsAddr+2],es

; Setup keyboard interrupt vector to point to our interrupt routine
	mov	ax,2500h or vector
	mov	dx,codeOFFSET keybd_int
	push	ds				; save DS
	push	cs
	pop	ds				; set DS = CS
	int	21h				; set the vector
	pop	ds				; restore DS

; Get keyboard information from WIN.INI, and change keyboard trans. tables
; if a new keyboard DLL can be loaded.

	cmp	fTables,0			; don't do this when
	jnz	GotNewTables			; re-enabling after coming
	inc	fTables				; back from fullscreen oldapp.
	cCall	NewTable			; Go try to load DLL.
GotNewTables:
	
; If Enhanced (type 4) keyboard has been detected, we'll let the
; BIOS think we have a standard keyboard. This is to facilitate
; handling of PrintScreen and Pause keys by the BIOS.
;
; This stuff must be done AFTER the NewTable() call, because this
; clears the RT keyboard flag!

	cmp	[KeyType],4		; is it RT keyboard ?
	jnz	ena10			; if not, skip
	push	ds
	mov	ax,RAMBIOS
	mov	ds,ax
   assumes ds,RAMBIOS

	mov	al, byte ptr[KB_type]	; get old 40h:96h (enhanced flag)
	and	byte ptr[KB_type],0efH	; clear bit 4

	pop	ds
   assumes ds,DATA
	mov	old_enhanced, al	; save old enhanced flag byte
					; (which may or may not have
					; bit 4 set!)
ena10:


	mov	[enabled],-1


; Setup nmi_vector (for SysReq key handling) and CSAlias.
; Need to alias CODE because of protect mode.

ifdef NEWNMI
	call	GetNmi
else
	Public GetNmi
GetNmi:
	mov	ax,3502h
	int	21h
	mov	word ptr [nmi_vector][0],bx
	mov	word ptr [nmi_vector][2],es
	Public GotNmi
GotNmi:
endif

ifdef   ENVOY
;!!!------------------------------------------------------------
;
; This stuff is inserted into the Microsoft Enable routines.
; If we are running on a Envoy, it saves the old keyboard
; translators in OldKeyTrans and sets the translators to raw mode.
;
        mov     [fVectra], FV_NOEXBIOS
        mov     ax, F16_INQUIRE
        xor     bx, bx
        int     16h
        cmp     bx, 'HP'
	je	ke_vectra
        jmp     ke_not_vectra
ke_vectra:
        mov     [fVectra], FV_NOA
	mov	ax, __ROMBIOS		; Check for vectra A,A+
        mov     es, ax
        mov     al, es:[0FAh]           ;  in F000:FA
        and     al, 00011111b           ;  0 means A+
        jnz     ke_not_vectra_A
        mov     [fVectra], FV_A ; Running on a Vectra A
ke_not_vectra_A:

; If KeyType != 4, I assume that we have an Envoy keyboard attached
; which needs special treatment
;

	cmp     [KeyType], 4
	jne	ke_envoy
        jmp	ke_not_envoy
ke_envoy:
					; not RT keyboard
        mov     ax, F16_KBD             ; Get keyboard information
        int     16h
        or      ah, ah                  ; Succesful?
	jz	ke_envoy_2
        jmp     ke_not_envoy            ;  No, not envoy
ke_envoy_2:
        cmp     bl, 20h                 ; Is the HP-HIL ID below 20h
	jb	ke_envoy_1
        jmp     ke_not_envoy            ;  No, not envoy
ke_envoy_1:
        or      [fVectra], FV_ENVOY     ;  Yes, envoy


; Now save and set the translators by modifying their parents
; in their device headers
;

        mov     si, V_SPCCP             ; Save and set PCCP
        mov	di, V_RAW
	cCall   ExchParent, <si, di>
        mov     OldCCPTrans, ax                                         ;9/23/86


	mov     si, V_PSOFTKEY          ; Save and set PSOFTKEY         ;9/23/86
        mov     di, V_RAW
        cCall   ExchParent, <si, di>
        mov     OldSoftkeyTrans, ax

; I now set the translators in to raw mode again using the EX-BIOS.
; This was done in the hope that this code will work with future
; DIN Envoy keyboards.
;
        mov     ax, F16_SET_TRANSLATORS
        mov     bl, 04h                 ; set CCP to raw mode
        int     16h

	mov     ax, F16_SET_TRANSLATORS
        mov     bl, 06h                 ; set softkeys to raw mode
        int     16h

	jmp	ke_not_vectra

ke_not_envoy:

; Save the state of Scandor. If V_SCANDOOR is not present
; these calls will have no effect.
;
	mov	cx, WinFlags
	and	cx, WF_PMODE 
	cmp	cx, WF_PMODE 		;check only prot-mode
	je	kbe_prot_mode
	jmp	kbe_real_mode		; jmp > 256 bytes
kbe_prot_mode:
        mov     bl, 0                   ; State 0
        RSysCalPM V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_GET_STATE>
        mov     ScanState0, bh
        mov     bl, 2                   ; State 2
        RSysCalPM V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_GET_STATE>
        mov     ScanState2, bh
	jmp	ke_not_vectra

kbe_real_mode:
        mov     bl, 0                   ; State 0
        RSysCal V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_GET_STATE>
        mov     ScanState0, bh
        mov     bl, 2                   ; State 2
        RSysCal V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_GET_STATE>
        mov     ScanState2, bh
ke_not_vectra:
endif   ;ENVOY

; Set lights properly
ifdef	NOKIA
	cmp	KeyType,6		; 9140 keyboard
	jne	Not9140_2
	cmp	WORD PTR PCType,00FEH	;     - " -    connected to plain old PC
	jne	Not9140_2
	EnterCrit
	call	SetEriLights
	LeaveCrit
	jmp	SHORT EnableExit
Not9140_2:
endif	; NOKIA

	call	SetLightHardware

    public EnableExit	; for debug
EnableExit:

cEnd	Enable

;***************************************************************************
; InitKeyState
;
; The point of this routine is to make the shift, control, alt,
; numlock, capslock or shiftlock, and scrollock bits in KeyState
; correspond to the ROM BIOS's key state bits when Windows starts.
;
; We assume that Windows has first cleared its key state vector to
; all 0's.
;
; NOTE:  the sign bit of each byte in KeyState indicates up/down,
;  and bit 0 is toggled on each depression.
; The entries in KeyState have already been initialized to 0's by
; Windows.
;
; NOTE: When this routine is called, the keyboard type may NOT have been
; determined yet.
;
; This is called with ES:DI => key state vector

public InitKeyState 
InitKeyState proc near

	; Get BIOS shift state byte directly from RAM (40H:17H) instead
	; of via int 16H call.

	push	es			; remember, we need ES:DI
    if 1
	; access RAM directly
	mov	si,RAMBIOS		; segment 40H or selector for it.
	mov	es,si
	mov	al,byte ptr es:[kb_flag] ; [AL] = BIOS keystate (40H:17H)
    else
	; let the BIOS do it
	mov	ah,2
	int	16h
    endif
	pop	es

	; the binary masks
	; The shift (first 3) flags need their STATE (80H) bit initialized:
	; so we put 80h into DL to load into the state vector.

	mov     dl,80h			; load STATE bit
	mov     ah,fShift		; init. VK_SHIFT byte in KeyState
	mov     bx,VK_SHIFT		; note -- THIS CLEARS BH!
	call    SetKeyState
	mov     ah,fCtrl		; init. VK_CONTROL byte in KeyState
	mov     bl,VK_CONTROL
	call    SetKeyState
	mov     ah,fALt			; init. VK_MENU byte in KeyState
	mov     bl,VK_MENU		; 
	call    SetKeyState		;

	; The CAPSLOCK, SCROLL and NUMLOCK flags need their TOGGLE
	; bits initialized, so put 1 in DL to load into the state vector.
	mov	dl,1			; load TOGGLE bit
	mov	ah,fScroll
	mov	bl,VK_OEM_SCROLL
	call	SetKeyState
	mov	ah,fNum
	mov	bl,VK_NUMLOCK
	call	SetKeyState
	mov	ah,fCaps		; BIOS CAPS LOCK/SHIFT LOCK bit?
	mov	bl,VK_CAPITAL

public SetKeyState, sks1

SetKeyState:
	; BX determines which byte in the key state vector to modify.
	; DL is the new value for this byte
	; (AL & AH) determines whether to copy the byte: AL contains
	;    the BIOS shift state.
	test	al,ah
	jz	sks1
	mov	es:[di+bx],dl		; SET rgbKeyState[bx]
	ret
sks1:
	mov	byte ptr es:[di+bx],0	; CLEAR rgbKeyState[bx]
	ret

InitKeyState endp

ifdef   ENVOY

;!!!-----------------------------------------------------------------------
;
; ExchParent( Device, NewParent )
;
; Purpose       Replace the parent of the specified device.
;
; Parameter     Device - the specifed device vector
;               NewParent - the new parent vector
;
; Result        The previous parent
;
;-------------------------------------------------------------------------
cProc   ExchParent, <NEAR, PUBLIC>, <BP,SI,DI,DS,ES,CX>
        ParmW   Device
        ParmW   NewParent
cBegin  ExchParent

	mov	cx, WinFlags
	and	cx, WF_PMODE			; get prot_mode flag
	cmp	cx, WF_PMODE			; see if it is set
	je	prot_mode
	jmp	real_mode

prot_mode:
	mov	ax, ds
	mov	es, ax				; make es = ds
	mov	bx, offset RealMode_Word_Struc
	mov	RealMode_BP[bx], V_SYSTEM
	mov	RealMode_AH[bx], F_INS_BASEHPVT
	mov	di, bx
	mov	bl, 6fh			; calling int 6f through int 31h
	xor	bh, bh
	xor	cx, cx
	mov	ax, 0300h
	int	31h			;es:di points to RealMode_Word_struc
	mov	ax, es
	mov	ds, ax			; make it point to RealMode_Word_Str
	mov	bx, di
	mov	bx, RealMode_ES[bx]	; now get real mode ES
	mov	ax, 0002h		; convert Segment to Selector
	int	31h			; do the conversion

	mov     si, Device
	mov	es, ax			; ax (es) has the selector
	mov	bx, es:[si+4]		; get segment into bx
	mov	ax, 0002h		; convert it again to access segment
	int	31h			; do the conversion
	mov 	es, ax			; use the di that is passed 
	mov 	ax, NewParent		; now prepare to exchange parents
	xchg	ax, es:[DH_V_PARENT]
	jmp	exch_end	

; get HP vector table
real_mode:
        mov     si, Device
        mov     di, NewParent
        mov     bp, V_SYSTEM
        mov     ah, F_INS_BASEHPVT
        syscall

; get the Device header address
        mov     es, es:[si+4]
        mov     ax, di
        xchg    ax, es:[DH_V_PARENT]    ; load and save a new parent

exch_end:

cEnd    ExchParent

endif   ;ENVOY

;***************************************************************************
; Disable( eventProc ) - disable hardware keyboard interrupts, restoring
; the previous IBM BIOS keyboard interrupt handler.
;
;***************************************************************************
cProc	Disable,<PUBLIC,FAR>,<si,di>

cBegin	Disable
; Do nothing if not enabled
	cmp	[enabled],0
	jnz	DisableNotDone
	jmp	Dis_done

DisableNotDone:
; Flush BIOS keyboard buffer when restoring BIOS interrupt
;;	mov	ax,0C00H
;;	int	21h

; Wild stuff !! If RT keyboard has been detected, we put back
; the Enhanced keyboard flag at 40:96h, whatever it was.

	cmp	[KeyType],4	; is it RT keyboard ?
	jne	dis10		; if not, skip
				; is RT
	mov	bl,old_enhanced	; get saved value for 40:96h
	push	ds
	mov	ax,RAMBIOS
	mov	ds,ax
   assumes ds,RAMBIOS
	mov	byte ptr[KB_type],bl	; restore Enhanced flag
	pop	ds
   assumes ds,DATA
dis10:

; Restore the keyboard interrupt vector to point to previous value
	mov	ax,2500h or vector
	push	ds
	lds	dx,[bios_proc]
	int	21h
	mov	ax,RAMBIOS
	mov	ds,ax
   assumes ds,RAMBIOS
   	and	WORD PTR [kb_flag],0FCFFh	; clear left ctrl+left alt
	pop	ds
   assumes ds,DATA

ifdef   ENVOY
;!!!-------------------------------------------------------------
;
; if Envoy keyboard, restore the old keyboard translators which are
;  saved in OldKeyTrans. This is first done using the EX-BIOS and
;  then done by adjusting the translators device headers directly.
;

        test    [fVectra], FV_ENVOY
	jnz	kd_envoy
        jmp     kd_not_envoy
kd_envoy:

; Using EX-BIOS set the keyboard the translators to their default
;
        mov     ax, F16_SET_TRANSLATORS         ; Map V_CCP to V_CCPCUR
        mov     bl, 0
        int     16h

        mov     ax, F16_SET_TRANSLATORS         ; Map V_SOFTKEY to V_SKEY2FKEY
        mov     bl, 5
        int     16h

; Now restore the saved translators
;

        mov     si, V_SPCCP                     ; Restore cursor pad    ;9/23/86
        mov     ax, OldCCPTrans
        cCall   ExchParent, <si, ax>

        mov     si, V_PSOFTKEY                  ; Restore softkeys
        mov     ax, OldSoftkeyTrans
        cCall   ExchParent, <si, ax>                                    ;9/23/86
	
	jmp	kbd_en_end_hp

kd_not_envoy:
; Restore the state of Scandor. If V_SCANDOOR is not present
; these calls will have no effect.
;

	mov	cx, WinFlags
	and	cx, WF_PMODE 
	cmp	cx, WF_PMODE 		;check only prot-mode
	je	kbd_prot_mode
	jmp	kbd_real_mode		; jmp > 256 bytes

kbd_prot_mode:
        mov     bl, 0                   ; State 0
        mov     bh, ScanState0
        RSysCalPM V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_SET_STATE>
        mov     bl, 2                   ; State 2
        mov     bh, ScanState2
        RSysCalPM V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_SET_STATE>
	jmp	kbd_en_end_hp
kbd_real_mode:
        mov     bl, 0                   ; State 0
        mov     bh, ScanState0
        RSysCal V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_SET_STATE>
        mov     bl, 2                   ; State 2
        mov     bh, ScanState2
        RSysCal V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_SET_STATE>

kbd_en_end_hp:

endif   ;ENVOY

	mov	[enabled],0

Dis_done:
cEnd	Disable

ifdef ICO

;***************************************************************************
;	Handle lights for Olivetti system.
;	For M24's, 6300's 6300+'s, etc., it is necessary to send commands
;	to the keyboard to set the LED's on the keyboard.
;	This is called from SetLightHardware in ToAscii.asm.
;***************************************************************************

	public	SetOliLights
SetOliLights proc near

SetOliWay:
	; It's an Olivetti machine with the 83, 86, 101-102A,
	; or ICO (102 extended) keyboard, etc., or a AT&T 6300 with the 301
	; keyboard or a 6300 Plus with the 302 keyboard.
	; Int 16H won't set lights, so we do it directly, with some effort.

	push	es
	mov	si,RAMBIOS
	mov	es,si
    assumes es,RAMBIOS
	mov	ah,es:[kb_flag]		; look at ROM's shift state
	pop	es
    assumes es,nothing

	mov	al,fOliLites		; get mask for lights we handle here.

	cmp	KeyType, 3		; is it 86-key keybd. (not too likely),
	je	SetHasScroll
	cmp	KeyType, 4		; or Enhanced keyboard (on M240)
	je	SetHasScroll
	cmp	OliType, KB302		; or is it AT&T 302 keyboard?
	jne	SetNotScroll
SetHasScroll:
	or	al,fScroll
SetNotScroll:
					; now look at [kb_flag] ..
	and	ah,al			; only look at lights we have.
	cmp	ah,kflags		; look at previous state
	je	LightsExit		; if change..
					; set all lights:
	mov	kflags,ah		; save current state

	test	al, fScroll		; if AT&T 302 keyboard,
	jz	SetNotScroll2
	mov	bl,ScrollLight		; set bit for scroll lock light
	mov	cl,ah			; get desired keyboard state.
	and	cl,fScroll		; get scroll lock on/off bit.
	call	DoLights
SetNotScroll2:

	mov	bl,CapsLight		; set bit for capslock light
	mov	cl,kflags		; get kbd. flags
	and	cl,fCaps		; get capslock on/off bit
	call	DoLights

	mov	bl,NumLight		; set bit for numlock light
	mov	cl,kflags		; get kbd. flags
	and	cl,fNum			; get numlock on/off bit
	call	DoLights

LightsExit:
	ret

SetOliLights endp

; (Still in 'ifdef ICO')
;***************************************************************************
;
; DoLights, - this routine provides access to lights that
; are on the keyboard.  The first parameter specifies which light to turn
; on or off (as defined below) and the second parameter specifies which to do
; (0 means off and not zero means on). 
;
; The sense of this function is reversed for the numlock/function lock light
; if an ICO keyboard is installed.
;
; Internal NEAR proc.:
; Call this with
;	BL = light setting/resetting bit (1-based)
;		CapsLight	= 1	caps lock light
;		NumLight	= 2	num lock light
;		ScrollLight	= 4	scroll lock on AT&T 302 keyboard
;	cl = on/off flag.
;
; This sends a 2-byte command to the keyboard:  the first byte is 13H, the
; second indicates in the sign bit whether to turn a light on or off, and
; in the low 3 bits which light is to be changed.
;***************************************************************************

DoLights proc near


	xor	ah,ah		; clear data value to send
	cmp	cl,0		; Turn on or off.
	jz	DoWhich
	mov	ah,80h		; 0: turn off, 80h: turn on
DoWhich:
	or	ah,bl		; Byte to send to keyboard.
	cmp	bl,NumLight	; If it's numlock light, is it
	jne	DoLightsProg
	cmp	OliType,KB102	; the ICO keyboard?
	jnz	DoLightsProg
	xor	ah,80h		; complement if ICO.

DoLightsProg:			; (AH) = data byte for LED command.
	EnterCrit
	push	ax
	mov	ah,LedCmd	; command to program LED's on keyboard
	call	kout		; send byte in AH to keyboard
	pop	ax		; command byte
	call	kout		; send byte in AH to keyboard
	LeaveCrit
	ret

DoLights endp

; (Still in 'ifdef ICO')
;***************************************************************************
; send byte in AH to the keyboard. AL is lost
;
;***************************************************************************

kout proc near

	push	cx
koutwait:
	call	koutdelay	; delay to make sure status is stable
	in	al,kb_status	; get 8041 status
	test	al,10b		; can we output a byte?
	jnz	koutwait

	call	koutdelay
	mov	al,ah		; ok, send the byte
	out	kb_data,al

	pop	cx
	ret

koutdelay:
	mov	cx,100		; delay a little
	loop	$
	ret

kout endp

endif ; ICO

ifdef	NOKIA

;***************************************************************************
;	Handle lights for Ericsson 9140 in Ericsson PC.
;	This is called in ToAscii.asm if required.
;***************************************************************************

	public	SetEriLights
SetEriLights proc near
	push	es
	mov	si,RAMBIOS		; was 'BIOSDATASEG'
	mov	es,si
    assumes es,RAMBIOS
	mov	al,es:[kb_flag]		; look at ROM's shift state

	and	al,060H
	errnz	(60H-(fCaps+fNum))
	mov	ah,al			; Save later in kflags
	xor	al,kflags
	and	al,060H
	jz	EriLightsExit

	mov	al,ah
	and	al,060H
	mov	cl,4
	shr	al,cl
	or	al,90H
	mov	cx,0

EriLightLoop:
	push	ax
	in	al,71H
	test	al,1
	pop	ax
	loopz	EriLightLoop

	out	60H,al
	mov	kflags,ah

EriLightsExit:
	pop	es
    assumes es,nothing
	ret

SetEriLights endp
endif	; NOKIA

sEnd CODE

;***************************************************************************
;	int GetKbdType(Which)
;
;	if Which == 0, returns keyboard type (1..6)
;
;	if Which == 1, returns keyboard subtype ("OliType")
;
;	if Which == 2, returns number of function keys
;
;	otherwise returns 0.
;
;***************************************************************************

createSeg _GETTYPE, GETTYPE, BYTE, PUBLIC, CODE
sBegin	GETTYPE
assumes	CS,GETTYPE
assumes	DS,DATA

cProc GetKeyboardType,<PUBLIC,FAR>,<si,di>

    ParmB Which

cBegin GetKeyboardType

	xor	ax,ax				; clear return value
	cmp	Which,0
	jne	GetKbd1
	mov	al,KeyType			; type= from SYSTEM.INI
	jmp	short GetKbdExit
GetKbd1:
	cmp	Which,1
	jne	GetKbd2
	mov	al,OliType			; subtype= from SYSTEM.INI
	jmp	short GetKbdExit
GetKbd2:
	cmp	Which,2
	jne	GetKbdExit
	mov	ax,iqdNumFunc			; number of function keys

GetKbdExit:

cEnd GetKeyboardType

sEnd GETTYPE

if2
%out .. end ENABLE.ASM
%out
endif

END
