
;
; Define equates and structures for EGA Register Interface
;
ERI_READREG	=	0F0h
ERI_WRITEREG	=	0F1h
ERI_READRANGE	=	0F2h
ERI_WRITERANGE	=	0F3h
ERI_READSET	=	0F4h
ERI_WRITESET	=	0F5h
ERI_REVERTDEF	=	0F6h
ERI_DEFINEDEF	=	0F7h
ERI_GETDEF	=	0F8h
ERI_CONTEXTINFO =	0F9h
ERI_INQUIRE	=	0FAh

ERI_CI_GETSIZE	=	000h
ERI_CI_SAVE	=	001h
ERI_CI_RESTORE	=	002h
ERI_CI_FONTINFO =	003h
ERI_CI_GETBFLAG =	004h

ERI_ID_CRTC	=	000h
ERI_ID_SEQ	=	008h
ERI_ID_GRAPH	=	010h
ERI_ID_ATTR	=	018h
ERI_ID_MISCOUT	=	020h
ERI_ID_FEAT	=	028h
ERI_ID_GRPOS1	=	030h
ERI_ID_GRPOS2	=	038h


FontInfo	struc
  fibank0	db	?
  fibank1	db	?
  fibank2	db	?
  fibank3	db	?
FontInfo	ends

FI_CUSTOM	=	000h
FI_EMPTY	=	0FFh
FI_MAXINDEX	=	4

