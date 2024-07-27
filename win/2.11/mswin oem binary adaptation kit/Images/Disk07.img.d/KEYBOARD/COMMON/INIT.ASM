;****** INIT.ASM ***********************************************************
;                                                                          *
;   Copyright (C) 1983,1984,1985,1986 by Microsoft Inc.                    *
;   Copyright (C) 1985,1986,1987 by Olivetti.				   *
;									   *
;***************************************************************************
;
;	History
;	plb = peterbel = peterbe
;
;	===== Windows 2.10 ==============
;
;	22 jun 88	peterbe	PS/2 mod's 50, 60 skip Oli. kbd. ID code now,
;				when possible. otherwise recognize mod 35
;				short keyboard (sec. id string FA AB 54).
;	15 jun 88	peterbe	Put include files before any ifdefs.
;
;	===== Windows 2.0/386 ===========
;	11 jan 88	plb	Changed 7fh mask to 0efh in Enable(). This fixes
;				PRINTscreen bug on enhanced keyboard.
;	05 jan 88	plb	Set number of function keys to 12 (SaveRT101102)
;	16 nov 87	plb	Change so Oli code for kbd. ident. is used
;				for systems with PC/XT/AT PCType. Change
;				code after noRT: to just check PCType.
;	12 nov 87	plb	Don't test BIOS in RT keyboard test
;				(just before noRT:)
;				Put in 'jmp IDExit' before IsntRT: so X1/X2
;				swap doesn't happen for non-Oli. RT keyboards.
;	11 nov 87	plb	Changed kb identification so PC, XT, AT clones
;				if NOT Enhanced bit, try to get ID from
;				keyboard.  This excludes PS/2 systems.
;	31 oct 87	plb	Reversed test on SwapX1X2 in OEMKeyScan().
;	29 oct 87	plb	Added OemTable3 for Olivetti Spain II keyboard.
;				Also, use OemTable1 if Ericsson type 5.
;	28 oct 87	AP	Added reenable of keyclick on Ericsson 9140
;				keyboard after reset to get id in PC system.
;	27 oct 87	plb	Added 'ifdef W103' for 1.03 test version.
;	26 oct 87	plb	If not Olivetti or Ericsson, just use system
;				ID and BIOS RT flag to set keyboard type.
;	22 oct 87	AP	Let plb's code take care of IBM keyboard
;				in Ericsson system.
;	10 oct 87	plb	Fixed X1/X2 test to identify Olivetti systems.
;				Now set 'SwapX1X2' if swap is done, used in
;				Windows 386 OEMKeyScan() code.
;	08 oct 87	AP	Added code to identify Ericsson keyboards
;				Set (if blank) KeyBoardId in Ericsson systems
;	07 oct 87	plb	Moved SetOliLights, Inquire, Enable, Disable
;				to Enable.asm.  Renamed from OLIKBD.ASM to
;				INIT.ASM
;	25 sep 87	plb	Added X1/X2 swap in init. code.
;				Added OEMKeyScan routine and tables.
;				iqdNumFunc is always 10 now due to Winoldap
;				limitations.
;	24 sep 87	plb	Added check for Olivetti ROM, IsOli byte.
;
;	===== Windows 2.0 ===========
;	29 jun 87	plb	Commented out WIN.INI dependent stuff, reading
;				file and function key count.
;	30 jun 87	plb	Combining 2 parts of data segment.
;				Added complete keyboard identification.
;				No. of function keys obtained from keyboard
;				type.  File input commented out.
;	 1 jul 87	plb	Added include of national PATCH.INC
;				Added code for patching tables.
;				Changed setting of 10h bit in RTFlags because
;				patching code may overwrite this.
;	 2 jul 87	plb	Updating SetOliLights to use OliType.
;				Also, SetOliLights, DoLights will handle
;				AT&T 302 keyboard.  Dolights is streamlined
;				a little.
;	19 aug 87	plb	Removed commented-out code.  Variables which
;				were in INIT segment are now in DATA segment,
;				because some Windows code looks at INIT
;				segment's checksum, by golly.
;	31 aug 87	plb	'datestring' replaced with fixed 'Copyright'.
;				Version string is now in DATACOM.ASM
;	17 sep 87	plb	Removed version from Copyright.
;
;				
include	keyboard.inc
include	olikbd.inc

if1
%out
%out INIT.ASM  Windows 2.00
ifdef	ERICSSON
%out .	with Ericsson support
endif
endif

ROMBIOS	SEGMENT	AT 0F000h
ifdef	ERICSSON
	ORG	0FFF5H
Eri_ID		LABEL BYTE
	ORG	0FFFCH
Eri_type	LABEL BYTE
endif
	org  0FFFEh
PC_type	label BYTE  ; contains computer identification
ROMBIOS	ENDS

RAMBIOS	SEGMENT	AT 40H
	org  96h
KB_type	label BYTE  ; contains RT keyboard identification
ifdef	ERICSSON
	ORG 0EEH
KeyBoardId LABEL BYTE
ID_IBM	EQU	0AAH
ID1050	EQU	0A5H
ID1051	EQU	0A8H
ID9140	EQU	0A6H
endif
RAMBIOS	ENDS

;***************************************************************************
; DATA segment -- data declarations and local data
;***************************************************************************

sBegin	DATA

assumes DS,DATA

; DATA segment variables accessed in KbInit() below

; from 'inquire' module .. not accessed here unless no. of function keys
; varies.

	extrn	iqdNumFunc:WORD		; number of function keys

; Data to specify keyboard type and system type, in the keyboard table
; module.  Some of these values are loaded with the table, others are
; computed in the INIT code in this module.

	extrn	TableType:byte		; Table type loaded (1,2,3,4)
					; These 2 bytes MUST BE CONSECUTIVE:
	extrn	KeyType:byte		; Computed keyboard type (1,2,3,4)
	extrn	IsOli:byte		; NZ if ROM identifies this as an
					; Olivetti computer.
ifdef	ERICSSON
	extrn	IsEri:byte		; NZ if ROM identifies this as an
					; Ericsson computer.
endif
	extrn	fKbRt:byte		; flags RT (101-102) keyboard
	extrn	OliType:byte		; NZ if Olivetti-protocol keyboard,
					; identifies keyboard type. Also
					; for AT&T.
	extrn	PCType:byte		; Copy of system type ID
	extrn	PCTypeHigh:byte		;  from ROM address FFFF:000E
					; For PCType values, see OLIKBD.INC
	extrn	RTFlags:byte		; if RT, copy of KeyType

; labels in keyTranslationTable ..
	extrn	X1:byte			; swap X1 and X2 if IBM AT keyboard,
	extrn	X2:byte			; after the right table is set up.

; OS2 flag in DATACOM.ASM, set here

ifndef W103
	if1
	%out .       INIT has OS2 support
	endif
	extrn	fOS2Box:byte
endif


; Acknowledge byte for interrupt controller.  This varies depending on
; system type.

	public	AckByte
AckByte		db	eoi		; eoi or eoiAT

; Temporary variables 

KeyID0		db	0		; bytes returned from keyboard
KeyID1		db	0
KeyID2		db	0
KeyID3		db	0
KeyID4		db	0

nKeyID		db	0		; count of bytes above actually read.

ifdef X1X2
SwapX1X2	db	0		; set if X1/X2 are to be swapped.
endif

sEnd

;***************************************************************************
; Initialization code -- called only once, when the driver is loaded.
;***************************************************************************

createSeg _INIT, INIT, BYTE, PUBLIC, CODE
sBegin INIT
assumes CS,INIT
assumes DS,DATA

Copyright db	'<Copyright (c) Olivetti 1987>'


; Patch table


include PATCH.INC


; ****** KbInit ******* Initialization code *******************
;
; This code is called ONLY when the keyboard driver is loaded.
; It identifies the system type and the keyboard type, and
; performs modifications of the keyboard translation tables
; and set flag bytes specific to the type of keyboard attached.
;
; *************************************************************
cProc KbInit,<PUBLIC,FAR>
cBegin KbInit

;
; Determine PC type and save in DS:PCType and PCTypeHigh
;
	push	ds
	mov	ax,ROMBIOS
	mov	ds,ax
assumes DS,ROMBIOS
ifdef	ERICSSON
	pop	es
	push	es
assumes ES,DATA
	cmp	Eri_ID,'P'
	jne	notEri_ID
	mov	al,PC_type
	mov	ah,Eri_type
	inc	IsEri
	jmp	SHORT gotPCtype
notEri_ID:
endif

	mov	ax,word ptr [PC_type]

ifdef	ERICSSON
gotPCtype:
endif
	pop	ds
assumes DS,DATA
	mov	word ptr PCType,ax	; save both bytes
	cmp	al, M28ID		; is an M28 (AT)-like system?
	jne	UnlikeAT
	mov	AckByte, eoiAT		; set EOI byte for interrupt ack.
UnlikeAT:


	
	; Here, we determine if this is an Olivetti/AT&T computer.
	; If it is, we set the IsOli byte.

	push	es
	mov	ax,0fc00h
	mov	es,ax
	cmp	es:[0050h],'LO'		; look at ROM copyright message
	pop	es
	jne	NotOliRom
	inc	IsOli
	jmp	getKbIDIsOli		; jump over Ericsson code..
NotOliRom:

ifdef	ERICSSON
; Check for Ericsson BIOS

	cmp	IsEri,1
	je	getKbIdEri
	jmp	getKbIdNotEri

; Here we are sure to have an Ericsson BIOS

getKbIdEri:
	push	es
	mov	ax,RAMBIOS
	mov	es,ax
assumes ES,RAMBIOS

	mov	al,PCTypeHigh
	cmp	al,1			; EPPC
	jne	getKbIDEriCont

; The Ericsson Portable PC have just one keyboard

	mov	ah,005H
	mov	KeyBoardId,ID_IBM	; Set KeyBoardID (WinOldAp ?)
	jmp	getKbIDEriRet

; Not a Portabel

getKbIDEriCont:
	EnterCrit
	mov	al,KeyBoardId		; KeyBoardID at power on
	mov	KeyID0,al
	inc	nKeyID
	push	ax			; Should be in range A0 to AF
	and	al,0F0H
	cmp	al,0A0H
	pop	ax
	je	getKbIDEriFound
	dec	nKeyID			; Sorry

; Assume 1050, check for PC or 286

	mov	KeyBoardId,ID1050	; Set KeyBoardID (WinOldAp ?)
	cmp	PCType,0FCH
	je	getKbIdEriAT		; If no ID and an AT compatibel

; PC system

	mov	al,000H			; Reset PC keyboard (1050/1051/9140)
	out	kb_data,al
	push	cx			; Wait a while
	mov	cx,00000H
getKbIdEriLoop1:
	loop	getKbIdEriLoop1
	pop	cx
	in	al,kb_data
	cmp	al,ID9140
	jne	NoClickOn
	push	ax
	mov	al,0D2H			; The reset has disabled the keyclick
	out	kb_data,al
	pop	ax
NoClickOn:
	push	ax			; Should be in range A0 to AF
	and	al,0F0H
	cmp	al,0A0H
	pop	ax
	jne	getKbIdEriOhOh

; Got a valid ID, save it

	mov	KeyBoardId,al		; Set KeyBoardID (for WinOldAp ?)
	mov	KeyID0,al
	inc	nKeyID
	jmp	SHORT getKbIdEriAck

; AT system

getKbIdEriAT:
	mov	ah,0FFH			; Reset keyboard in AT system
	call	kbout
	call	kbin			; Get response from keyboard
	jc	getKbIdEriFound
	cmp	al,0FAH			; Should be Ack from slave processor
	jne	getKbIdEriFunny		;	but check for ID instead
	call	kbin
	jc	getKbIdEriFound

; Check if ID (value in al) is valid
getKbIdEriFunny:
	push	ax			; Should be in range A0 to AF
	and	al,0F0H
	cmp	al,0A0H
	pop	ax
	jne	getKbIdEriOhOh

; OK. Save it

	mov	KeyBoardId,al		; Set KeyBoardID (for WinOldAp ?)
	mov	KeyID0,al
	inc	nKeyID
;;	jmp	getKbIdEriAck
	errnz	($-getKbIdEriAck)
getKbIdEriOhOh:

; Acknowledge any system

getKbIdEriAck:
	in	al,kb_ctl
	push	ax
	or	al,080H
	out	kb_ctl,al
	pop	ax
	out	kb_ctl,al
getKbIdEriFound:
	mov	al,KeyBoardId
	cmp	al,ID_IBM
	je	getKbIdNotEri		; If IBM keyboard ID, get more info
	mov	ah,5
	cmp	al,ID1050
	je	getKbIDEriDone
	cmp	al,ID1051
	je	getKbIDEriDone
	mov	ah,6
	cmp	al,ID9140
	je	getKbIDEriDone
	mov	ah,1			; Best guess ??? (should not happen)
getKbIDEriDone:
	LeaveCrit
getKbIDEriRet:			; Portabel goes here (not to Found above)
	pop	es
	jmp	SaveTypeInAH
; Continue below for non Ericsson systems

getKbIdNotEri:
endif

	; It's not an Olivetti or Ericsson system.
	; Determine how we're going to get the keyboard type:

	cmp	PCType, IBMPCID
	je	getKbIDIsOli
	cmp	PCType, IBMXTID
	je	getKbIDIsOli
	cmp	PCType, IBMATID
	jne	NotOliOrEri		; not PC/XT/AT primary ID
					; it has AT ID:
	cmp	PCTypeHigh, 04		; PS/2 model 50?
	je	NotOliOrEri
	cmp	PCTypeHigh, 05		; PS/2 model 60?
	je	NotOliOrEri
	cmp	PCTypeHigh, 0bah	; PS/2 model 60?
	je	NotOliOrEri

getKbIDIsOli:
	; The system is an Olivetti system, or PC/XT/AT compatible.
	; This code identifies 83-key Olivetti (AT&T 301), AT&T 302,
	; Olivetti (ICO) 102-key extended, Olivetti 101-102 (IBM RT)
	; Olivetti 85/86 key, and other common 83, 86, and 101-102
	; key keyboards with XT or AT protocols.

	push	ds			; make long destination pointer to
	pop	es			; array of keyboard ID bytes.
	mov	di,dataOFFSET KeyID0

	EnterCrit
	mov	ah, 05h
	call	kbout			; send primary ID request.
	call	kbin
	jnz	NoTimeout
	mov	al,0			; timeout, make ID zero.
NoTimeout:
	stosb				; save byte in KBID0
	inc	nKeyID
	LeaveCrit


	; In the next few instructions, we try to identify an Olivetti-
	; -protocol keyboard.  AL will contain the keyboard ID, AH will
	; contain the corresponding table number, if a match is found.
	; ID's:
	; 02=M24 86, 04=ATT6300/+ with 3 lights, 01=ICO 102, 40=101/102,
	; 42=XT keyboard on 1076.
	;
					; Is it.. ID==2
	cmp	al,KB83			; M24 83-key? (AT&T 301 keyboard)
	jne	TryATandT		; 
	mov	ah,1			; table type
	jmp	short FoundOliKB
TryATandT:				; Is it.. ID==4
	cmp	al,KB302		; AT&T 6300/plus 302 keyboard?
	jne	TryIco			;  (the one with 3 lights)
	mov	ah,1			; table type
	jmp	short FoundOliKB
TryIco:					; Is it.. ID==1
	cmp	al,KB102		; Olivetti extended (ICO) 102-key
	jne	TryOli86
	mov	ah,2			; table type
;;;	mov	iqdNumFunc,18		; 18 function keys
	jmp	short FoundOliKB
TryOli86:				; Is it.. ID==10H
	cmp	al,KB86			; Olivetti 85 or 86-key 'M28' keyboard.
	jne	TryOli101102
	mov	ah,3			; table: same as 1 or 2, depending
	jmp	short FoundOliKB	; on country.
TryOli101102:				; Is it.. ID==40H
	cmp	al,KB102RT		; Olivetti protocol 101-102 'A'?
	jne	TryXT1076
SaveRT101102:				; moved this label 26 oct 87
	mov	ah,4			; table type is RT
	mov	fKbRt,10h		; flag 101-102 'RT' keyboard
	mov	iqdNumFunc,12		; 12 function keys (05 jan 88)
	jmp	short FoundOliKB
TryXT1076:				; Is it.. ID==42H
	cmp	al,KBXT83		; XT 83-key keyboard on 1076?
	jne	TryXT
	mov	ah,1			; table type

FoundOliKB:
	mov	OliType, al		; save Olivetti/ATT keyboard ID [AL]
	jmp	SaveTypeInAH		; and table type in [AH]

TryXT:					; is this a system with XT protocol?
	cmp	al,0			; (Such as 1030)
	jne	TryAT			; if not zero, check for AT protocol.

NotOliOrEri:

;	This system has an XT keyboard interface, or the system is neither
;	an Olivetti nor an Ericsson system, and it is PC/XT/AT type.
;	(This part is based on Microsoft code...)
;	Test for RT (Olivetti 101/102 key) keyboard
;
	push	ds
	mov	ax,RAMBIOS
	mov	ds,ax
assumes DS,RAMBIOS
	test	byte ptr [KB_type],10h	; is RT keyboard flag set?
	pop	ds
assumes DS,DATA
	jz	noRT			; skip if not
;;; remove this 12 nov 87
;;	mov	ah,91h			; test if BIOS supports this keyboard
;;	int	16h			; if so, ah will be decremented by at
;;	test	ah,80h			; least 12h, else ah will not change.
;;	jnz	noRT			; skip if not.
					; It's an 'RT' 101-101 key keyboard.
	mov	al,0			; flag as non-Olivetti protocol
	jmp	SaveRT101102		; go back and save type.
noRT:

	; Change 16 nov 87 .. 
	mov	ah,1			; most likely 83, but..
	cmp	PCType, IBMATID		; PC/XT or AT?
	jne	SaveTypeInAH
	mov	ah,3			; it/s AT type
	jmp	SaveTypeInAH

TryAT:					; Try for AT (M28) protocol...
					; this wasn't Olivetti or XT protocol.
	cmp	al,0feh			; first KB type byte must be FE, 41, 81
	je	GetAtID
	cmp	al,041h
	je	GetAtID
	cmp	al,081h
	jne	SaveTypeInAH

GetAtID:				; is it 86 or 101-101?
	EnterCrit
	mov	ah, 0f2h		; send second command
	call	kbout			; request secondary id.
GetAtLoop:
	call	kbin			; read response from keyboard,
	jc	GotNoMore		;  'till there're no more or too many
	inc	nKeyID			; keep count,
	cmp	nKeyID,5		; only store first few.
	jg	GotNoMore
	stosb				; Save KeyID1, ..2, ..3
	jmp	GetAtLoop

GotNoMore:
	LeaveCrit
	mov	ah,1			; default to 83-key
	cmp	nKeyID,2
	jl	SaveTypeInAH		; no response
	cmp	KeyID1,0fah		; response begins with FA?
	jne	SaveTypeInAH		; if not, use 83-key
	mov	ah,3
	cmp	nKeyID, 4		; Check for 101-102 response
	jl	SaveTypeInAH
	cmp	word ptr KeyID2, 41abh	; want FA AB 41.. (full enhanced kbd)
	je	GotEnhId
	cmp	word ptr KeyID2, 54abh	; or FA AB 54 (PS/2 mod 35 keyboard)
	jne	SaveTypeInAH
GotEnhId:
	inc	ah			; it's type 4: RT 101-101.
	mov	fKbRt,10h		; flag 101-102 'RT' keyboard
;;;	mov	iqdNumFunc,12		; 12 function keys

SaveTypeInAH:
	mov	KeyType, ah		; save table number
	mov	al,AckByte
	out	ack_port,al

	; ==== patch keyboard table ====
	; PatchTable[] contains the offsets in INIT of the patch
	; lists for keyboards type 2, 3, and 4.
	; If the entry in PatchTable is -1, the table is the same
	; as type 1 keyboard, and is not patched.  Otherwise, the
	; data for each piece of the patch is prefixed with a 2-word
	; header (count and starting destination offset in DATA).
	; The header for each piece immediately follows the last data
	; byte of the preceding piece.
	; if the count is 0, the patching is done.
	cmp	ah, 1			; patch table if table no is
	jle	NoPatch			; 2, 3, 4
ifndef	ERICSSON
	cmp	ah, 4
else
	cmp	ah,7
endif
	jg	NoPatch
	dec	ah
	dec	ah
	add	ah,ah			; make word index to PatchTable
	mov	bl,ah
	xor	bh,bh
	mov	si,cs:PatchTable[bx]
	cmp	si,-1
	je	NoPatch			; -1 means same as table 1.
	push	ds
	pop	es			; ES = DS (DATA)
	push	cs
	pop	ds			; DS = CS (INIT)
	cld				; copy upward in memory.
PatchLoop:
	lodsw				; get byte count
	cmp	ax,0
	jz	PatchDone		; Done if byte count == 0.
	mov	cx,ax			; save count
	lodsw				; get destination offset in table
	mov	di,ax			;  and put in DI
	rep	movsb			; copy bytes
	jmp	PatchLoop		; go get next section to patch.

PatchDone:
	push	es			; restore segment registers.
	pop	ds

NoPatch:
	cmp	fKbRt,0			; check flag for 101-102 'RT' keyboard
	jz	IsntRT			; if set, set bit also in RTFlags,
	or	RTFlags,10h		; which may have been overwritten.
	jmp	IDExit			; also, skip X1/X2 swap code.

IsntRT:
ifdef X1X2				; (Not for USA systems)

; === Handle keyboards which must swap scan codes at X1: and X2: ====

	; Is it an AT keyboard on non-Oli syst.?
	; handle IBM clones and Olivetti:
	cmp	KeyType,1		; 83 etc key keyboard?
	je	IDExit			; Yes: no swap.
	cmp	IsOli, 0		; No..
	jnz	IDExit			; No Olivetti keyboards do this.
ifdef	ERICSSON
	; handle Ericsson
	cmp	KeyType,5		; Ericsson 1050/1051 keyboard
	jne	NotType5
	push	es
	mov	ax,RAMBIOS
	mov	es,ax
	cmp	es:KeyBoardId,ID1050
	pop	es
	je	IDExit
NotType5:
	cmp	KeyType,6		; Ericsson 9140 keyboard
	je	IDExit
endif					; ( ERICSSON )

	mov	al,X1			; If AT, but not Olivetti,
	xchg	X2,al			; swap virtual keycodes at X1 and X2.
	mov	X1,al 
	inc	SwapX1X2		; set flag for 386 Oem code.
endif					; (X1/X2 swap code)

IDExit:

; This is a convenient place to test to see if we are running in the
; OS/2 compatibility box.  If the DOS version # is 10 or greater then
; it is OS/2.  Set a flag so we test for Ctl+Esc to do screen switches.

ifndef W103
	mov	ah,30h
	int	21h
	cmp	al,0Ah
	jl	notOS2
	inc	fOS2Box
notOS2:
endif
	
	xor	ax,ax
	not	ax		; return success

cEnd KbInit


; *****************************************************************
; KBIN:
; Get character from keyboard.
; It is assumed that a command (05H or 0F2H) has been set to request
; that the keyboard send an identification byte.
; It is also assumed that interrupts are disabled.
;
; If a character is returned, the carry bit is reset.
; If no character is returned,  the carry bit is set.
; *****************************************************************

kbin proc near

	xor	cx, cx		; set timeout
kbiwait:
	in	al,kb_status	; wait for port ready
	test	al,1		; ready?
	jnz	kbiAvailable
	loop	kbiwait

	stc			; timeout, set carry and exit.
	ret

kbiAvailable:			; we received a byte.
	in	al,kb_data	; get data byte.
	clc			; clear carry
	ret			; al contains input byte.
	
kbin endp

; KBOUT: **********************************************************
; This duplicates 'kout' in the CODE segment, but so what, this
; is in a disposable segment, we throw it away.
; Call with output byte in AH, AL and CX are trashed.
; *****************************************************************

kbout proc near

kowait:
	call	kbwait
	in	al,kb_status	; get 8041 status
	test	al,10b		; can we output a byte?
	jnz	kowait

	call	kbwait
	mov	al,ah		; ok, send the byte
	out	kb_data,al
	ret

kbwait:
	mov	cx,100
	loop	$
	ret

kbout endp


sEnd INIT	; end of disposable initialization code.

;***************************************************************************
;	Nonresident LOADONCALL code for this module
;***************************************************************************

ifdef W103
 if1
 %out .. Windows 1.0X version -- No OemKeyScan()
 endif
else
 if1
 %out .. Windows 2.0 version with OemKeyScan() in NORES segment
 endif

createSeg _NORES, NORES, BYTE, PUBLIC, CODE
sBegin	NORES
assumes	CS,NORES
assumes	DS,DATA

; OEM scan code tables for Windows 386 pasting of clipboard characters
; into MSDOS applications:
;
; The following tables map OEM ASCII character codes into PC scan codes
; and shift-state values.  For each type of keyboard, there are two
; tables:
;
; The first handles ASCII codes 0..7FH, and is indexed by the
; ASCII code.
;
; The second, starting 256 bytes after the first one, is an associative
; table for those characters in the range 80h..FFh which are on the
; keyboard (typically, 5 or 10 characters).
;
; There is a set of tables for two types of keyboards:
;
; -- XT/AT -- Usually "type 1" elsewhere in this driver
; -- RT (extended) -- Usually "type 4" elsewhere in this driver

OemTable1 label byte	; table for XT/AT keyboards
include oem1x.inc

;; addition 29 oct 87
ifdef AT_TAB_386
  if1
    %out .  ES AT table for 386
  endif
OemTable3 label byte	; table for AT keyboards, if different from XT
include oem3x.inc
endif
;; end addition 29 oct 87

OemTable4 label byte	; table for RT keyboards
include oem4x.inc

; This function maps OEM ASCII codes (0..FF) into OEM scan codes and
; shift states.  It is passed the ASCII code as OEMChar, and returns:
;
;	(AX): scan code
;	(DX): shift state
;
;	Table entries:
;
;	First byte, Modifier flags (Shift state) (returned in DX)
;		bit  7 - Insert state active
;		bit  6 - Caps Lock state active
;		bit  5 - Num Lock state active
;		bit  4 - Scroll Lock state active
;		bit  3 - Alt shift depressed
;		bit  2 - Ctrl shift depressed
;		bit  1 - left shift depressed	USE THIS ONE FOR SHIFTED CHARS
;		bit  0 - right shift depressed	DO NOT USE THIS ONE
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
cProc OEMKeyScan,<PUBLIC,FAR>,<si,di>
    ParmB OEMChar
cBegin OemKeyScan

;; addition 29 oct 87
	cmp	TableType, 5	; Ericsson type 5? .. use type 1.
	je	OemIs1

ifdef AT_TAB_386
	cmp	TableType, 3	; Type 3 (Olivetti Spain II 86-key)
	jne	OemTry1
	lea	si,OemTable3
	jmp	short OemLookup
OemTry1:
endif
;; end addition 29 oct 87

	cmp	TableType, 1	; XT/AT kbd. table?
	jne	OemTry4
OemIs1:				; (29 oct 87)
	lea	si,OemTable1
	jmp	short OemLookup
OemTry4:
	cmp	TableType, 4	; RT (Oli. 101-102) extended kbd. table?
	jne	OemNoKey
	lea	si,OemTable4
OemLookup:
	mov	bl,OEMChar
	or	bl,bl		; in indexed part of table?
	js	OemScan		; if not, go scan associative table.
	xor	ax,ax		; if so, compute index and look up..
	xor	bh,bh
	shl	bx,1
	jmp	short OemLoadKey

OemScan:			; associative search of last part of table.
	mov	ah,bl		; AH contains character now.
	mov	bx,256 		; set index to assoc. table
OemScanLoop:
	mov	al, byte ptr cs:[si+bx]
	cmp	al, -1
	je	OemNoKey	; table terminates with -1
	cmp	al, ah		; check against character
	je	OemLoadAssoc
	add	bx, 3
	jmp	OemScanLoop

OemLoadAssoc:			; found..
	inc	bx		; move index to scancode and shifts.
	xor	ax,ax		; Need both bytes cleared.

OemLoadKey:
	mov	dx,word ptr cs:[si+bx]	; get scancode and shifts.
	xchg	dh,al		; AL is scan code, DL is shifts.
	cmp	dl,-1		; Valid?

ifndef X1X2
	jne	OemKeyExit
else
	je	OemNoKey
				; check for X1/X2 scancode swap.
	cmp	SwapX1X2, 0	; if set, swap
	jz	OemKeyExit	; if so...
	cmp	al, 41		; if scan code is 41 or 43, 
	je	OemX1X2
	cmp	al, 43
	jne	OemKeyExit
OemX1X2:
	xor	al, 2		; change 41 to 43, or vice versa.
	jmp	short OemKeyExit
endif	; X1X2

OemNoKey:
	xor	ax,ax		; set -1 return
	dec	ax
	mov	dx,ax
OemKeyExit:

cEnd OemKeyScan

sEnd NORES

endif	; ifndef W103

if2
%out end INIT.ASM
%out
endif

END KbInit

