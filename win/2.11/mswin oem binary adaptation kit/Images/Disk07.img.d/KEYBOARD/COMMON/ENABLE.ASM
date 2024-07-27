;****** ENABLE.ASM *********************************************************
;                                                                          *
;   Copyright (C) 1983,1984,1985,1986,1987,1988 by Microsoft Inc.          *
;   Copyright (C) 1985,1986,1987 by Olivetti.				   *
;									   *
;   Contains Inquire() Enable() Disable() and SetOliLights().              *
;                                                                          *
;***************************************************************************
;
;	History 	(peterbe = plb)
;	Windows 2.1
;	23 jun 88	peterbe	Added %out to indicate ENVOY ifdef.
;	15 jun 88	peterbe	Moved includes to before ifdefs
;	14 jun 88	peterbe	Added hp enhancments (ENVOY ifdefs)
;	26 may 88	peterbe	Enable(), Disable() now save/restore
;				value of 40h:96h in old_enhanced.
;
;	Windows 2.03
;	15 jan 88	plb	Revised comments in CapsLock section of
;				InitKeyState().
;	14 jan 88	plb	comment out int 21h fcn 0c call in Disable.
;	11 jan 88	plb	Changed mask for clearing KB_type bit in Enable
;	13 nov 87	plb	Changed SetOliLights() check for kbd/sys type.
;	12 nov 87	plb	Enable/Disable clear/set RT flag in KB_type.
;	27 oct 87	plb	Put ScreenSwitchEnable() in 'ifndef W103'.
;	22 oct 87	AP	Added Ericsson SetEriLights for 9140 in PC
;	08 oct 87	plb	Added ScreenSwitchEnable() for OS/2
;	06 oct 87	plb	separated out from OLIKBD.ASM
;				
;
include	keyboard.inc
include	olikbd.inc
include	vkoem.inc
include	vkwin.inc

if1
%out
%out Olivetti ENABLE.ASM  Windows 2.00
endif


; Numbers of lights, etc. on M24's, 6300's, etc.
LedCmd		equ	13h	; command to program lights

CapsLight	equ	1	; caps lock light
NumLight	equ	2	; num lock light
ScrollLight	equ	4	; scroll lock on AT&T 302 keyboard

; bits in kb_flag (ROM BIOS keyboard flags):

fCaps		equ	01000000b
fNum		equ	00100000b
fScroll		equ	00010000b
fOliLites 	equ	fCaps+FNum


RAMBIOS	SEGMENT	AT 40H
	org  96h
KB_type	label BYTE  ; contains RT keyboard identification
RAMBIOS ENDS

;***************************************************************************
; DATA segment -- data declarations and local data
;***************************************************************************

sBegin	DATA

assumes DS,DATA

; DATA segment variables accessed in KbInit() below

; from 'inquire' module

;;;	extrn	iqdNumFunc:WORD		; number of function keys

; Data to specify keyboard type and system type, in the keyboard table
; module.  Some of these values are loaded with the table, others are
; computed in the INIT code in this module.

;;	extrn	TableType:byte		; Table type loaded (1,2,3,4)
					; These 2 bytes MUST BE CONSECUTIVE:
ifdef ERICSSON
	extrn	KeyType:byte		; Computed keyboard type (1,2,3,4)
endif
	extrn	IsOli:byte		; NZ if ROM identifies this as an
					; Olivetti computer.

	extrn	fKbRt:byte		; flags RT (101-102) keyboard
	extrn	OliType:byte		; NZ if Olivetti-protocol keyboard,
					; identifies keyboard type. Also
;;ifdef ERICSSON			; for AT&T.
	extrn	PCType:byte		; Copy of system type ID
;;endif
;;	extrn	PCTypeHigh:byte		;  from ROM address FFFF:000E
					; For PCType values, see OLIKBD.INC
	extrn	RTFlags:byte		; if RT, copy of KeyType

; labels in keyTranslationTable ..
;;	extrn	X1:byte			; swap X1 and X2 if IBM AT keyboard,
;;	extrn	X2:byte			; after the right table is set up.

; originally in Inquire
	EXTRN	inquireData:BYTE

; originally in Enable,Disable
	EXTRN	keybd_int:FAR
	EXTRN	event_proc:DWORD
	EXTRN	bios_proc:DWORD
	EXTRN	LightsAddr:DWORD	; old int16h address
	EXTRN	bios1b_proc:DWORD
	EXTRN	INT1BHandler:FAR	; In DATACOM.INC
	EXTRN	enabled:WORD


	extrn	AsciiTabBase:word
	extrn	KeyTabEnd:word

; flag for OS/2 to allow/prevent screen switches
; The ScreenSwitchEnable function, called from the display driver,
; sets/resets this flag.

ifndef W103
	extrn	fSwitchEnable:byte
endif


; For handling of Olivetti M24 keyboard lights

kflags		db	0ffh	; prev. value in [ShiftState]

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

else
    if1
    %out ... Does NOT have VECTRA support
    endif

endif	;ENVOY


sEnd

;***************************************************************************
;	Resident code for this module
;***************************************************************************
sBegin	CODE	    ; Beginning of code segment
assumes	CS,CODE
assumes	DS,DATA

;---------------------------------------------------------------------
;
;---- ScreenSwitchEnable( fEnable ) ----------------------------------
;
;   This function is called by the display driver to inform the keyboard
;   driver that the display driver is in a critical section and to ignore
;   all OS/2 screen switches until the display driver leaves its
;   critical section.  The fEnable parameter is set to FALSE to disable
;   screen switches, and TRUE to re-enable them.  By default, screen
;   switches are enabled.
;---------------------------------------------------------------------
;
ifndef W103
 if1
 %out .. has ScreenSwitchEnable()
 endif

cProc	ScreenSwitchEnable,<PUBLIC,FAR>
ParmW	fEnable

cBegin	ScreenSwitchEnable
	xor	ah,ah		    ; assume disable
	cmp	fEnable,0
	jz	disab
	inc	ah		    ; enable switches
disab:
	mov	fSwitchEnable,ah    ; set the flag
cEnd	ScreenSwitchEnable

else
 if1
 %out .. no ScreenSwitchEnable
 endif
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

endif	;ENVOY

;***************************************************************************
;
; Inquire( pKBINFO ) - copies information about the keyboard hardware into
; the area pointer to by the long pointer argument.  Returns a count of the
; number of bytes copied.
;
;***************************************************************************
cProc	Inquire,<PUBLIC,FAR>,<si,di>
	Parmd	pKBINFO

cBegin	Inquire
					; .. now pass data to Windows ..
	les	di,pKBINFO		; Get far pointer of destination area
	mov	si,dataOFFSET inquireData
	mov	ax,SIZE	KBINFO		; Get number of bytes to move
	mov	cx,ax
	rep	movsb			; Move the bytes
	sub	ax,cx			; Return actual byte count moved

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

; All done if just reenabling a different event proc
	cmp	[enabled],0
	je	ke_cont
	jmp	En_done
ke_cont:

; Wild stuff !! If RT keyboard has been detected, we'll let the
; BIOS think we have a standard keyboard.  Code is slightly different
; from Microsoft USA version because KB_type may NOT have been set
; originally.

	cmp	[fKbRt],0	; is it RT keyboard ?
	jz	ena10		; if not, skip
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

; Save away current keyboard interrupt vector value
ena10:
	mov	ah,35h
	mov	al,vector
	int	21h
	mov	WORD PTR [bios_proc],bx
	mov	WORD PTR [bios_proc+2],es

; Save away current keyboard call vector value
	mov	ax,3516H
	int	21h

	mov	WORD PTR [LightsAddr],bx
	mov	WORD PTR [LightsAddr+2],es

; Setup keyboard interrupt vector to point to our interrupt routine
	mov	ah,25h
	mov	al,vector
	mov	dx,dataOFFSET keybd_int
	int	21h

; Initialize keyboard state.
; ES:DI points to movable memory, so no allocs allowed.
	les	di,lpKeyState
	call	InitKeyState

	mov	[enabled],-1

; Get the INT 1B vector, save away.
	mov	ah, 35h		; Get Vector command
	mov	al, 1bh		; Interrupt vector
	int	21h
	mov	word ptr [bios1b_proc],bx
	mov	word ptr [bios1b_proc +	2],es

; Set a new INT 1B vector
	mov	ah, 25h		; Set Vector command
	mov	al, 1bh		; Interrupt vector
	mov	dx,dataOFFSET INT1BHandler	; In DATACOM.INC
	int	21h

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
        jne     ke_not_vectra
        mov     [fVectra], FV_NOA
        mov     ax, 0F000h              ; Check for vectra A,A+
        mov     es, ax
        mov     al, es:[0FAh]           ;  in F000:FA
        and     al, 00011111b           ;  0 means A+
        jnz     ke_not_vectra_A
        mov     [fVectra], FV_A ; Running on a Vectra A
ke_not_vectra_A:

; If fKbRt is not set, I assume that we have an Envoy keyboard attached
; which needs special treatment
;
        cmp     [fKbRt], 0
        jne     ke_not_envoy
        mov     ax, F16_KBD             ; Get keyboard information
        int     16h
        or      ah, ah                  ; Succesful?
        jnz     ke_not_envoy            ;  No, not envoy
        cmp     bl, 20h                 ; Is the HP-HIL ID below 20h
        jae     ke_not_envoy            ;  No, not envoy
        or      [fVectra], FV_ENVOY     ;  Yes, envoy

; Now save and set the translators by modifying their parents
; in their device headers
;
        mov     si, V_PSOFTKEY          ; Save and set PSOFTKEY         ;9/23/86
        mov     di, V_RAW
        cCall   ExchParent, <si, di>
        mov     OldSoftkeyTrans, ax

        mov     si, V_SPCCP             ; Save and set PCCP
        cCall   ExchParent, <si, di>
        mov     OldCCPTrans, ax                                         ;9/23/86


; Save the state of Scandor. If V_SCANDOOR is not present
; these calls will have no effect.
;
        mov     bl, 0                   ; State 0
        RSysCal V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_GET_STATE>
        mov     ScanState0, bh
        mov     bl, 2                   ; State 2
        RSysCal V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_GET_STATE>
        mov     ScanState2, bh


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
ke_not_envoy:
ke_not_vectra:
endif   ;ENVOY

; Set lights properly
ifdef	ERICSSON
	cmp	KeyType,6		; 9140 keyboard
	jne	Not9140_2
	cmp	WORD PTR PCType,00FEH	;     - " -    connected to plain old PC
	jne	Not9140_2
	EnterCrit
	call	SetEriLights
	LeaveCrit
	jmp	SHORT En_done
Not9140_2:
endif
	call	SetOliLights

En_done:
cEnd	Enable


; InitKeyState
; The point of this routine is to make the shift, control, alt,
; numlock, capslock or shiftlock, and scrollock bits in KeyState
; correspond to the ROM BIOS's key state bits when Windows starts.
; NOTE:  the sign bit of each byte in KeyState indicates up/down,
;  and bit 0 is toggled on each depression.
; The entries in KeyState have already been initialized to 0's by
; Windows.

InitKeyState proc near
	mov	ah,2		; get shift state into AL
	int	16H

	; the binary masks
	; The first 3 flags need their STATE (80H) bit initialized:

	; Uncommented (Oli.plb 4 mar 87) to be same as M'soft 1.03 release
	mov     dl,10000000b	; load STATE bit
	mov     ah,00000011b	; init. VK_SHIFT byte in KeyState
	mov     bx,VK_SHIFT
	call    SetKeyState
	mov     ah,00000100b	; init. VK_CONTROL byte in KeyState
	mov     bl,VK_CONTROL
	call    SetKeyState
	mov     ah,00001000b	; init. VK_MENU byte in KeyState
	mov     bl,VK_MENU
	call    SetKeyState

	; The SCROLL and NUMLOCK flags need their TOGGLE bits initialized
	mov	dl,1		; load TOGGLE bit
	mov	ah,00010000b
	mov	bl,VK_OEM_SCROLL
	call	SetKeyState
	mov	ah,00100000b
	mov	bl,VK_OEM_NUMBER
	call	SetKeyState

	; This is a little different from the Microsoft version.
	; In this version, the VK_CAPITAL state is used for
	; capslock as well as for shiftlock.

	; The VK_CAPITAL byte needs its TOGGLE bit (01h) initialized, unless
	;  this keyboard has SHIFTLOCK instead, in which case the STATE
	;  bit (80h) must be initialized.
	test	[RTFlags],kbShiftLock	; i.e. KB is RT, and has shiftlock
	jz	InitCapsLock
	mov	dl,80h		; it's SHIFTLOCK, so STATE is important.
InitCapsLock:
	mov	ah,01000000b	; BIOS CAPS LOCK/SHIFT LOCK bit?
	mov	bl,VK_CAPITAL	; = VK_OEM_SHIFTLOCK as well

SetKeyState:
	; BX determines which byte in the key state vector to modify.
	; DL is the new value for this byte
	; (AL & AH) determines whether to copy the byte: AL contains
	;    the BIOS shift state.
	test	al,ah
	jz	sks1
	mov	es:[di+bx],dl
sks1:	ret

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
cProc   ExchParent, <NEAR, PUBLIC>, <BP,SI,DI,DS,ES>
        ParmW   Device
        ParmW   NewParent
cBegin  ExchParent

        mov     si, Device
        mov     di, NewParent

; get HP vector table
        mov     bp, V_SYSTEM
        mov     ah, F_INS_BASEHPVT
        syscall

; get the Device header address
        mov     es, es:[si+4]
        mov     ax, di
        xchg    ax, es:[DH_V_PARENT]    ; load and save a new parent

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
	je	Dis_done

; Flush BIOS keyboard buffer when restoring BIOS interrupt
;;	mov	ax,0C00H
;;	int	21h

; Wild stuff !! If RT keyboard has been detected, we put back
; the Enhanced keyboard flag at 40:96h, whatever it was.

	cmp	[fKbRt],0	; is it RT keyboard ?
	jz	dis10		; if not, skip
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
	mov	ah,25h
	mov	al,vector
	push	ds
	lds	dx,[bios_proc]
	int	21h
	pop	ds

	mov	ah,25h
	mov	al,1bh
	push	ds
	lds	dx,[bios1b_proc]
	int	21h
	pop	ds

ifdef   ENVOY
;!!!-------------------------------------------------------------
;
; if Envoy keyboard, restore the old keyboard translators which are
;  saved in OldKeyTrans. This is first done using the EX-BIOS and
;  then done by adjusting the translators device headers directly.
;
        test    [fVectra], FV_ENVOY
        jz      kd_not_envoy

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


; Restore the state of Scandor. If V_SCANDOOR is not present
; these calls will have no effect.
;
        mov     bl, 0                   ; State 0
        mov     bh, ScanState0
        RSysCal V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_SET_STATE>
        mov     bl, 2                   ; State 2
        mov     bh, ScanState2
        RSysCal V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_SET_STATE>

kd_not_envoy:
endif   ;ENVOY

	mov	[enabled],0

Dis_done:
cEnd	Disable

;***************************************************************************
;	Handle lights for Olivetti system.
;	or some systems, calling INT 16H will set the lights according
;	to the setting of the keyboard flags.
;	For M24's, 6300's 6300+'s, etc., it is necessary to send commands
;	to the keyboard.
;	This is called in ToAscii.asm as well as in this module.
;***************************************************************************

	public	SetOliLights
SetOliLights proc near

;; Change 13 nov 87
	cmp	IsOli,0			; Olivetti ROM?
	jz	SetCloneWay		; For Olivetti systems:
	cmp	OliType,0		; Keyboard identified itself as
					; 01, 02, 04, 10, 40, 42, etc.
	jz	SetCloneWay
	cmp	PCType, M28ID		; M28 Bios int 16h handles lights?
	jne	SetOliWay

;; Replaces:
;;	cmp	OliType,0		; Keyboard identified itself as
;;					; 01, 02, 04, 10, 40, 42, etc.
;;	jnz	SetOliWay

SetCloneWay:				; all other systems...
	mov	ah,1			; Calling this INT 16H function will
	pushf				; set lights according to bits in
	call	[LightsAddr]		;  [kb_flag].
	ret

SetOliWay:
	; It's an Olivetti machine with the 83, 86, 101-102A,
	; or ICO (102 extended) keyboard, etc., or a AT&T 6300 with the 301
	; keyboard or a 6300 Plus with the 302 keyboard.
	; Int 16H won't set lights, so we do it directly, with some effort.

	push	es
	mov	si,BIOSDATASEG
	mov	es,si
	mov	ah,es:[kb_flag]		; look at ROM's shift state
	pop	es

	mov	al,fOliLites		; get mask for lights we handle here.
	cmp	OliType, KB86		; is it 86-key keyboard, or
	je	SetHasScroll
	cmp	OliType, KB302		; is it AT&T 302 keyboard?
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

ifdef	ERICSSON

;***************************************************************************
;	Handle lights for Ericsson 9140 in Ericsson PC.
;	This is called in ToAscii.asm if required.
;***************************************************************************

	public	SetEriLights
SetEriLights proc near
	push	es
	mov	si,BIOSDATASEG
	mov	es,si
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
	ret

SetEriLights endp
endif

sEnd	CODE		; End of code segment

if2
%out end OLIKBD.ASM
%out
endif

END
