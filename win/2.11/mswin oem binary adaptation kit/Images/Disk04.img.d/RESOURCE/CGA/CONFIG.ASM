;***************************************************************************
;									   *
;   Copyright (C) 1985-1986 by Microsoft Inc.				   *
;									   *
;***************************************************************************

	title	Hardware Dependent Parameters
	%out	config
	page	,132


GetRGB	macro	Name
	extrn	R_&Name:abs
	extrn	G_&Name:abs
	extrn	B_&Name:abs
	db	R_&Name,G_&Name,B_&Name,0
	endm



OEM	segment public

;	Machine dependent parameters

	dw	12			;Height of vertical thumb
	dw	24			;Width of horizontal thumb
	dw	2			;Icon horiz compression factor
	dw	4			;Icon vert compression factor
	dw	1			;Cursor horz compression factor
	dw	2			;Cursor vert compression factor
	dw	0			;Kanji window height
	dw	2			;cxBorder (thickness of vertical lines)
	dw	1			;cyBorder (thickness of horizontal lines)

;	Default system color values

	GetRGB	SCROLLBAR
	GetRGB	BACKGROUND
	GetRGB	ACTIVETITLE
	GetRGB	INACTIVETITLE
	GetRGB	MENU
	GetRGB	WINDOW
	GetRGB	WINDOWFRAME
	GetRGB	MENUTEXT
	GetRGB	WINDOWTEXT
	GetRGB	TITLETEXT
	GetRGB	ACTIVEBORDER
	GetRGB	INACTIVEBORDER
	GetRGB	APPWORKSPACE

;	dw	0			;Unused words
;	dw	0
;	dw	0
;	dw	0
;	dw	0
;	dw	0
	dw	0
	dw	0

OEM	ends
end

