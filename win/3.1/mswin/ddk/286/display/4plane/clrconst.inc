	.xlist
;----------------------------Module-Header------------------------------;
; Module Name: CLRCONST.BLT
;
; Color/monochrome-specific constants.
;
; Created: In Windows' distant past (c. 1983)
;
; Copyright (c) 1983 - 1987  Microsoft Corporation
;
; This file is part of a set that makes up the Windows BitBLT function
; at driver-level.
;-----------------------------------------------------------------------;
	.list

ifdef	GEN_COLOR_BLT
if NUMBER_PLANES eq 3
PLANE_1			equ	00100001b
else
PLANE_1			equ	00010001b
endif
endif
