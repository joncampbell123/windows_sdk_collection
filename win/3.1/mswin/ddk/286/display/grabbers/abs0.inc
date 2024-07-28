
;
; NAME
;	ABS0.INC
;
; DESCRIPTION
;	This file provides a way to address numerous variables in the PC's
;	vector table and ROM BIOS data area.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;	1.01	060188	vvr	added constants for VGA
;

Abs0		segment at 0000h
		org	00400h + 010h
EquipFlag	dw	?

		org	00400h + 017h
KbFlag		db	?

		org	00400h + 049h
CrtMode 	db	?
CrtCols 	dw	?
CrtLen		dw	?
CrtStart	dw	?
CursorPosn	dw	8 dup (?)
CursorMode	dw	?
ActivePage	db	?
Addr6845	dw	?
CrtModeSet	db	?
CrtPalette	db	?

		org	00400h + 084h
Rows		db	?
Points		dw	?
Info		db	?
Info3		db	?
flags		db	?			; ^vvr 06/01/88
DCC		db	?			; ^vvr 06/01/88

		org	00400h + 096h
KbFlagRt	db	?

		org	00400h + 0A8h
lpSavePtr	label	dword
oSavePtr	dw	?
sSavePtr	dw	?
Abs0		ends


VideoBiosData	struc
  vbdCrtMode	db	?
  vbdCrtCols	dw	?
  vbdCrtLen	dw	?
  vbdCrtStart	dw	?
  vbdCursorPosn dw	8	dup (?)
  vbdCursorMode dw	?
  vbdActivePage db	?
  vbdAddr6845	dw	?
  vbdCrtModeSet db	?
  vbdCrtPallete db	?
VideoBiosData	ends

