
;
; NAME
;	OEM.INC
;
; DESCRIPTION
;	This file defines various OEM-specific constants for use mostly by
;	OEM.ASM, but also may find use in other grabber routines.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;


FALSE		=	0
TRUE		=	1


;
; Define IBM PC port locations for the Intel 8243 Programmable Interval Timer
; chip
;

PIT_PORTA	=	040h
PIT_PORTB	=	041h
PIT_PORTC	=	042h
PIT_PORTD	=	043h


;
; Define IBM PC port locations for the Intel 8255 Programmable Peripheral
; Interface chip
;

PPI_PORTA	=	060h
PPI_PORTB	=	061h
PPI_PORTC	=	062h
PPI_PORTD	=	063h


;
; Define timimgs and divisors to make sounds via the PIT, PPI chips and the
; PC's speaker circuitry.
;

BEEP_TIME1	=	02400h
BEEP_TIME2	=	03400h
BEEP_TONE1	=	00533h
BEEP_TONE2	=	00633h


;
; Define various OEM identification ids, version numbers, etc for the grabber.
;

GRABBER_ID	=	04A43h
OEM_VIDEO_ID	=	04850h
OEM_ALT_VDO_ID	=	04E50h			;ADD6F.COM TSR has been run
OEM_VERHI	=	002h
OEM_VERLO	=	001h

OEM_KBRT	=	00010000b		;RT keyboard presence bit
OEM_SHIFTMASK	=	00001111b		;isolate non-toggle keys
OEM_SHIFTSTATE	=	00001000b		;Alt key
OEM_TRIGGER	=	037h			;PrtScr key
OEM_TRIGGERRT	=	054h			;SysReq key (for RT keybd)
OEM_INTNUMBER	=	009h			;hook into keyboard ISR


;
; Structure definition for the HP EX-BIOS video data area.  See HP Vectra
; Technical Reference, Volume 2 for details.
;

HpBiosData	struc
  hbdHeader	db	6	dup (?)
  hbdPrimary	db	?
  hbdSecondary	db	?
  hbdFoundRom	db	?
  hbdIds	db	4	dup (?)
  hbdStatus	db	4	dup (?)
  hbdExtStatus	db	4	dup (?)
  hbdParmBlock	db	39	dup (?)
  hbdIbmMode	db	?
  hbdExtMode	db	?
  hbdScrData	db	?
HpBiosData	ends

