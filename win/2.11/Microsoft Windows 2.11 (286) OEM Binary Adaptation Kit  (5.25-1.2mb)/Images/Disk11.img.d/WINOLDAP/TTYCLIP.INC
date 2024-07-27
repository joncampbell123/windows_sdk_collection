WOABITMAP STRUC
	woabmType	  DW ?
	woabmWidth	  DW ?
	woabmHeight	  DW ?
	woabmWidthBytes   DW ?
	woabmPlanes	  DB ?
	woabmBitsPixel	  DB ?
	woabmBits	  DQ ?
	woabmWidth2	  DW ?
	woabmHeight2	  DW ?
WOABITMAP ENDS

WOAMETAFILEPICT STRUC
	woamm		DW ?
	woaXext 	DW ?
	woaYext 	DW ?
	woahMeta	DW ?
WOAMETAFILEPICT ENDS

;from windows.inc, the range of values for GetDeviceCaps
DRIVERVERSION		equ	0
ASPECTXY		equ	44


WOACF_OEMTEXT		equ	1
WOACF_SHORTBITMAP	equ	2
WOACF_BITMAP		equ	3
WOACF_HP8TEXT		equ	4
WOACF_WRITEVAL		equ	5
WOACF_METAFILEPICT	equ	6
WOACF_TEXT		equ	7
WOACF_DSPTEXT		equ	CF_DSPTEXT
WOACF_DSPBITMAP 	equ	CF_DSPBITMAP
WOACF_DSPMETAFILEPICT	equ	CF_DSPMETAFILEPICT
