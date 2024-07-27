; equates needed for copy operation
;
; these equates are direct from the equate.inc file
; this file was created to minimize memory consumption on 
; assemble
;
;			Ralph Peterson  4/16/85


HP_SHEADER	STRUC
DH_ATR			dw 	0
DH_NAME_INDEX		dw	0
DH_V_DEFAULT		dw	0
DH_P_CLASS		dw	0
DH_C_CLASS		dw	0
DH_V_PARENT		dw	0
DH_V_CHILD		dw	0
DH_MAJOR		db	0
DH_MINOR		db	0
HP_SHEADER	ENDS

V_SPCCP		equ	004EH	;CCP translator driver
V_LCCP_NUMPAD	equ	0096H	;CCP to numeric pad filter
V_PGID_CCP	equ	00B4H	;Graphic to CCP filter


HPENTRY		equ	006FH	;interrupt entry into HP SYSTEM

F_ISR		equ	0000H	;F_ISR function code for driver routines