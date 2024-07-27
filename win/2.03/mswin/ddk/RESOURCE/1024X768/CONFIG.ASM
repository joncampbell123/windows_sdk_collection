page    ,132
;***************************************************************************
;									   *
;   Copyright (C) 1985-1986 by Microsoft Inc.				   *
;									   *
;***************************************************************************

	title	Hardware Dependent Parameters
	%out	config


OEM	segment public

;	Machine dependent parameters

	dw	18			;Height of vertical thumb
	dw	20			;Width of horizontal thumb
        dw      1                       ;Icon horiz compression factor
        dw      1                       ;Icon vert compression factor
	dw	1			;Cursor horz compression factor
	dw	1			;Cursor vert compression factor
	dw	0			;Kanji window height
	dw	1			;cxBorder (thickness of vertical lines)
	dw	1			;cyBorder (thickness of horizontal lines)

;	Default system color values

        db      03Fh,03fh,03fh,0        ;colour of scroll bar
        db      000h,0FFh,0ffh,0        ;colour of desktop
        db      000h,000h,0FFh,0        ;colour of active title bar
        db      0ffh,0ffh,0ffh,0        ;colour of inactive title bar
        db      000h,0FFh,0ffh,0        ;colour of menu background
        db      0FFh,0FFh,0FFh,0        ;colour of window background
        db      000h,000h,000h,0        ;colour of window frame (caption)
        db      000h,000h,000h,0        ;colour of menu text
        db      000h,000h,000h,0        ;colour of text in window
        db      0FFh,0FFh,0FFh,0        ;colour of text in a caption
        db      07Fh,07Fh,07Fh,0        ;colour of text in active border
        db      07Fh,07Fh,07Fh,0        ;colour of text in inactive border
        db      000h,080h,0FFh,0        ;colour of text in application workspace


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
