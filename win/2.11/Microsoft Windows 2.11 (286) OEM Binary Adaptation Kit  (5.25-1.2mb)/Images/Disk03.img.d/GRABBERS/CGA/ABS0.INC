

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

