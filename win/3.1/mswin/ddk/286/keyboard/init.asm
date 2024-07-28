	page	,132
;****** INIT.ASM ***********************************************************
;                                                                          *
;   Copyright (C) 1983-1990 by Microsoft Corporation. All Rights Reserved. *
;									   *
;   Copyright (C) 1985,1986,1987 by Ing C. Olivetti & Co, SPA.		   *
;									   *
;***************************************************************************
;
;	History
;	plb = peterbel = peterbe
;
;	===== Windows 3.00 ==============
;
;	02 feb 90	davidw		Cleaning up ctrl-brk.
;
;	14 jan 90	davidw		Re-enabling ctrl-brk
;
;	16 dec 88	peterbe		Removed commented-out init code
;					Updated some comments.
;
;	30 nov 88	davidw		Making it more bi-modal.
;
;	19 aug 88	peterbe		Moved date string here
;					Removed reference to iqdNumFunc
;
;	12 aug 88	peterbe		Moved RAMBIOS to keyboard.inc
;
;	11 aug 88	peterbe		Add page directive at beginning.
;
;	02 aug 88	peterbe		Removed call to GetTableSeg()
;
;	28 jul 88	peterbe		make cCall to TableSeg..
;
;	27 jul 88	peterbe		Renamed keyTranslationTable to keyTrTab
;
;	26 jul 88	peterbe		Removed extrn fKbRt.
;
;	25 jul 88	peterbe		Changed call to GetTableSeg().
;
;	21 jul 88	peterbe		Move OemKeyScan to separate file.
;
;	18 jul 88	peterbe		Eliminate olikbd.inc
;
;	15 jul 88	peterbe		Experimental version -- remove lots of
;					KBD ID code.
;	===== Windows 2.10 ==============
;
;	22 jun 88	peterbe	PS/2 mod's 50, 60 skip Oli. kbd. ID code now,
;				when possible. otherwise recognize mod 35
;				short keyboard (sec. id string FA AB 54).
;	15 jun 88	peterbe	Put include files before any ifdefs.
;				
include	keyboard.inc

if1
%out
%out INIT.ASM  Windows 3.00
    ifdef	NOKIA
    %out .	with Ericsson/NOKIA support
    else
    %out .	without Ericsson/NOKIA support
    endif
endif

	extrn	__ROMBIOS:abs
	extrn	GetModuleHandle:far

ROMBIOS	SEGMENT	AT 0F000h
ifdef	NOKIA
	ORG	0FFF5H
Eri_ID		LABEL BYTE
	ORG	0FFFCH
Eri_type	LABEL BYTE
endif
	org  0FFFEh
PC_type	label BYTE  ; contains computer identification
ROMBIOS	ENDS

; RAMBIOS	SEGMENT	AT 40H
; 	org  96h
; KB_type	label BYTE  ; contains RT keyboard identification
; ifdef	NOKIA
; 	ORG 0EEH
; KeyBoardId LABEL BYTE
; ID_IBM	EQU	0AAH
; ID1050	EQU	0A5H
; ID1051	EQU	0A8H
; ID9140	EQU	0A6H
; endif
; RAMBIOS	ENDS

;***************************************************************************
; DATA segment -- data declarations and local data
;***************************************************************************


sBegin	DATA

assumes DS,DATA

; DATA segment variables accessed in KbInit() below

; Data to specify system type
; Some of these values are loaded with the table, others are
; computed in the INIT code in this module.

	extrn	IsOli:byte		; NZ if ROM identifies this as an
					; Olivetti computer.
ifdef	NOKIA
	extrn	IsEri:byte		; NZ if ROM identifies this as an
					; Ericsson computer.
endif	; NOKIA

	extrn	OliType:byte		; NZ if Olivetti-protocol keyboard,
					; identifies keyboard type. Also
					; for AT&T.
	extrn	PCType:byte		; Copy of system type ID
	extrn	PCTypeHigh:byte		;  from ROM address FFFF:000E
					; For PCType values, see OLIKBD.INC

; Acknowledge byte for interrupt controller.  This varies depending on
; system type.

	public	AckByte
AckByte		db	eoi		; eoi or eoiAT


	public	InitDataEnd

InitDataEnd label byte

sEnd

;***************************************************************************
; Initialization code -- called only once, when the driver is loaded.
;***************************************************************************

createSeg _INIT, INIT, BYTE, PUBLIC, CODE
sBegin INIT
assumes CS,INIT
assumes DS,DATA

; Begin with date and copyright strings.

include date.inc

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
	mov	ax,__ROMBIOS
	mov	ds,ax
assumes DS,ROMBIOS


ifdef	NOKIA
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
endif	; NOKIA

	mov	ax,word ptr [PC_type]

ifdef	NOKIA
gotPCtype:
endif	; NOKIA

	pop	ds
assumes DS,DATA
	mov	word ptr PCType,ax	; save both bytes
	cmp	al, M28ID		; is an M28 (AT)-like system?
	jne	UnlikeAT
	mov	AckByte, eoiAT		; set EOI byte for interrupt ack.
UnlikeAT:

	; Here, we determine if this is an Olivetti/AT&T computer.
	; If it is, we set the IsOli byte.

ifdef ICO
	push	es
	mov	ax,__ROMBIOS
	mov	es,ax
	cmp	es:[0C050h],'LO'	; look at ROM copyright message
	pop	es
	jne	NotOliRom
	inc	IsOli
NotOliRom:
endif ; ICO

	xor	ax,ax
	not	ax		; return success

cEnd KbInit

sEnd INIT	; end of disposable initialization code.

if2
%out end INIT.ASM
%out
endif

END KbInit
