;***************************************************************************
;									   *
;   Copyright (C) 1985-1989 by Microsoft Inc.                              *
;									   *
;***************************************************************************

        title   Control Panel Color Table
	%out	config
	page	,132


RGB     macro   R, G, B
        db      R,G,B,0
	endm



COLORTABLE  segment public

        dw  16                  ; # colors in table

        RGB 000h,000h,000h
        RGB 020h,020h,020h
        RGB 028h,028h,028h
        RGB 03Fh,03Fh,03Fh
        RGB 05Ah,05Ah,05Ah
        RGB 060h,060h,060h
        RGB 080h,080h,080h
        RGB 099h,099h,099h
        RGB 0A1h,0A1h,0A1h
        RGB 0AAh,0AAh,0AAh
        RGB 0C1h,0C1h,0C1h
        RGB 0CFh,0CFh,0CFh
        RGB 0E2h,0E2h,0E2h
        RGB 0EFh,0EFh,0EFh
        RGB 0F7h,0F7h,0F7h
        RGB 0FFh,0FFh,0FFh

COLORTABLE     ends
end
