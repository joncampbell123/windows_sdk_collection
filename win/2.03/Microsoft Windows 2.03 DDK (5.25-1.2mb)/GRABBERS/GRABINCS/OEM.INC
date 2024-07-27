
FALSE		=	0
TRUE		=	1

PIT_PORTA	=	040h
PIT_PORTB	=	041h
PIT_PORTC	=	042h
PIT_PORTD	=	043h

PPI_PORTA	=	060h
PPI_PORTB	=	061h
PPI_PORTC	=	062h
PPI_PORTD	=	063h

BEEP_TIME1	=	02400h
BEEP_TIME2	=	03400h
BEEP_TONE1	=	00533h
BEEP_TONE2	=	00633h

GRABBER_ID	=	04A43h
OEM_VIDEO_ID	=	04850h
OEM_VERHI	=	002h
OEM_VERLO	=	001h

OEM_KBRT	=	00010000b		;RT keyboard presence bit
OEM_SHIFTMASK	=	00001111b		;isolate non-toggle keys
OEM_SHIFTSTATE	=	00001000b		;Alt key
OEM_TRIGGER	=	037h			;PrtScr key
OEM_TRIGGERRT	=	054h			;SysReq key (for RT keybd)
OEM_INTNUMBER	=	009h			;hook into keyboard ISR


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